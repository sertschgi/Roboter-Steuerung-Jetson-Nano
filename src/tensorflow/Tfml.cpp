//
// Created by SERT on 11.01.2024.
//

#include <filesystem>
#include "tensorflow/Tfml.hpp"


void tfml::Pixel::normalize()
{
    this->x = (x/255);
    this->y = (y/255);
    this->z = (z/255);
}

tfml::Detector::Detector(const std::string & checkpointPath, const string & labelmapPath)
{
    filesystem::path fsLabelmapPath {  labelmapPath }; // labelmapPath.empty() ? "labelmap.txt" :
    filesystem::path fsCheckpointPath { checkpointPath }; // checkpointPath.empty() ? "checkpoint.tflte" :

    if (!exists(fsLabelmapPath))
        throw tfml::errors::LabelmapPathError();

    if (!exists(fsCheckpointPath))
        throw tfml::errors::CheckpointPathError();

    ifstream labelmap { absolute(fsLabelmapPath).string() };
    string label {};

    while (getline(labelmap, label))
        this->labels_.emplace_back(label);

    labelmap.close();

    this->model_ = FlatBufferModel::BuildFromFile(checkpointPath.c_str());
    if (!this->model_) throw tfml::errors::ModelInitializeError();

    unique_ptr<TfLiteGpuDelegateOptionsV2> options {
        make_unique<TfLiteGpuDelegateOptionsV2>(TfLiteGpuDelegateOptionsV2Default())
    };

    unique_ptr<TfLiteDelegate> delegate {
	    TfLiteGpuDelegateV2Create(options.get())
    };

    InterpreterBuilder interpBuild { *this->model_, BuiltinOpResolver() };
    interpBuild.AddDelegate(delegate.get());

    interpBuild(&this->interpreter_);

    this->interpreter_->AllocateTensors();

    this->inputTensor_ = unique_ptr<TfLiteTensor>(
            this->interpreter_->input_tensor(0)
            );
    this->scoresTensor_ = unique_ptr<TfLiteTensor>(
            this->interpreter_->output_tensor(2)
            );
    this->labelsTensor_ = unique_ptr<TfLiteTensor>(
            this->interpreter_->output_tensor(1)
            );
    this->boxesTensor_ = unique_ptr<TfLiteTensor>(
            this->interpreter_->output_tensor(0)
            );

    this->dims_.y = this->inputTensor_->dims->data[1];
    this->dims_.x = this->inputTensor_->dims->data[2];
    this->dims_.x = this->inputTensor_->dims->data[3];

}

tfml::Detector::~Detector()
{
    detectThread.join();
}

Vframe tfml::Detector::preprocessFrame_(const Vframe & frame) const
{
    Vframe inputFrame {};
    frame.convertTo(inputFrame, CV_32FC3);
    cvtColor(inputFrame, inputFrame, COLOR_BGR2RGB);

    auto * pixelsPtr { inputFrame.ptr<Pixel>(0,0) };

    const Pixel * pixelsPtrEnd { pixelsPtr + inputFrame.cols * inputFrame.rows };

    for (; pixelsPtr != pixelsPtrEnd; ++pixelsPtr)
        pixelsPtr->normalize();

    resize(inputFrame, inputFrame, Size(this->dims_.x, this->dims_.y));

    return inputFrame;
}

void tfml::Detector::detect(const Vframe& frame)
{
    Vframe inputFrame { this->preprocessFrame_(frame) };
    memcpy(this->inputTensor_->data.f,
           inputFrame.ptr<float>(0),
           this->dims_.x * this->dims_.y * this->dims_.z * sizeof(float));
    interpreter_->Invoke();
    this->frameDims.x = frame.cols;
    this->frameDims.y = frame.rows;
}

void tfml::Detector::detect(const Vstream & stream)
{
    detectThread.join();
    detectThread = thread([&]() {while (stream.running()) this->detect(stream.frame());});
}

size_t getNumberOfElements(const unique_ptr<TfLiteTensor> & tensor)
{
    size_t numberOfElements { 1 };
    for(size_t i {}; i < tensor->dims->size; ++i)
        numberOfElements *= tensor->dims->data[i];
    return numberOfElements;
}

template <typename T>
vector<T> tfml::Detector::tensorToVec_(const unique_ptr<TfLiteTensor> & tensor)
{
    if (is_same_v<T, float>)
    {
        return {tensor->data.f, tensor->data.f + getNumberOfElements(tensor)};
    }
    else if (is_same_v<T, uint>)
    {
        return {tensor->data.u32, tensor->data.u32 + getNumberOfElements(tensor)};
    }
    else if (is_same_v<T, uint8_t>)
    {
        return {tensor->data.uint8, tensor->data.uint8 + getNumberOfElements(tensor)};
    }
    else
    {
        return {tensor->data.i32, tensor->data.i32 + getNumberOfElements(tensor)};
    }
}

vector<string> tfml::Detector::toLabels_(const vector<uint8_t> & labelNumbers) const {
    vector<string> labelsVec {};
    for_each(labelNumbers.begin(),
             labelNumbers.end(),
             [&](size_t index){
                    labelsVec.emplace_back(this->labels_.at(index));
             }
    );
    return labelsVec;
}

[[nodiscard]] Rect tfml::Detector::intVecObjToRec_(const vector<int> & intVec, const size_t & i) const
{
    const double cx = intVec[4*i];
    const double cy = intVec[4*i+1];
    const double w = intVec[4*i+2];
    const double h = intVec[4*i+3];
    const double xmin = ((cx-(w/2.f))/this->dims_.x) * this->frameDims.x;
    const double ymin = ((cy-(h/2.f))/this->dims_.y) * this->frameDims.y;
    const double xmax = ((cx+(w/2.f))/this->dims_.x) * this->frameDims.x;
    const double ymax = ((cy+(h/2.f))/this->dims_.y) * this->frameDims.y;

    return Rect {(int) xmin, (int) ymin, (int) (xmax-xmin), (int) (ymax-ymin) };
}

vector<Rect> tfml::Detector::intVecToRectVec_(const vector<int> & intVec) const
{
    vector<Rect> rectVec {};
    for(size_t i {}; i < intVec.size(); i=i+4){
        rectVec.emplace_back(this->intVecObjToRec_(intVec, i));
    }
    return rectVec;
}

vector<tfml::DetObj> tfml::Detector::detection(float threshold) const
{
    auto boxesVec { tensorToVec_<int>(this->boxesTensor_) };
    auto scoresVec { tensorToVec_<float>(this->boxesTensor_) };
    auto labelsUIntVec { tensorToVec_<uint8_t>(this->labelsTensor_) };
    vector<string> labelsVec { this->toLabels_(labelsUIntVec) };


    vector<DetObj> detObjs {};
    for (size_t i {}; i < scoresVec.size(); ++i)
        if (scoresVec.at(i) > threshold)
            detObjs.emplace_back(DetObj {scoresVec.at(i),
                                         labelsVec.at(i),
                                         intVecObjToRec_(boxesVec, i)});

    return detObjs;
}
