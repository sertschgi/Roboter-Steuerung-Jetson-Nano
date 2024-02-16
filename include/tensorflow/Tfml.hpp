//
// Created by SERT on 11.01.2024.
//

#ifndef ROBOTER_STEUERUNG_JETSON_NANO_TFML_HPP
#define ROBOTER_STEUERUNG_JETSON_NANO_TFML_HPP

#include <memory>
#include <cstdint>
#include <vector>
#include <string>
#include <exception>
#include <thread>
#include <filesystem>
#include <fstream>
#include <algorithm>

#include "config/config.hpp"
#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/model.h"
#include "tensorflow/lite/delegates/gpu/delegate.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/op_resolver.h"

#include "video/Vstream.hpp"

using namespace std;
using namespace tflite::impl;
using namespace tflite::ops::builtin;

using dimArray = Point3_<uint16_t>;

namespace tfml {
    namespace errors
    {
        class ModelInitializeError : exception
        {
            const char * what() { return "Failed to initialize model!"; }
        };
        class CheckpointPathError : exception
        {
            const char * what() { return "Checkpoint path does not exist!"; }
        };
        class LabelmapPathError : exception
        {
            const char * what() { return "Labelmap path does not exist!"; }
        };
    }

    struct DetObj
    {
        float score {};
        string label {};
        Rect points {};
    };

    class Pixel : Point3_<float>
    {
    public:
        void normalize();
    };

    class Detector
    {
    private:
        unique_ptr<FlatBufferModel> model_ {};
        unique_ptr<Interpreter> interpreter_ {};
        unique_ptr<TfLiteTensor> inputTensor_ {};
        unique_ptr<TfLiteTensor> scoresTensor_ {};
        unique_ptr<TfLiteTensor> labelsTensor_ {};
        unique_ptr<TfLiteTensor> boxesTensor_ {};
        dimArray frameDims {};
        vector<string> labels_ {};
        dimArray dims_ {};
        thread detectThread {};
        [[nodiscard]] Vframe preprocessFrame_(const Vframe & frame) const;
        template<typename T> [[nodiscard]] static vector<T> tensorToVec_(const unique_ptr<TfLiteTensor> & tensor);
        [[nodiscard]] vector<Rect> intVecToRectVec_(const vector<int> & intVec) const;
        [[nodiscard]] Rect intVecObjToRec_(const vector<int> & intVec, const size_t & i) const;
        [[nodiscard]] vector<string> toLabels_(const vector<uint8_t> & labelNumbers) const;
    public:
        Detector(const string & checkpointPath, const string & labelmapPath);
        ~Detector();
        void detect(const Vframe & frame);
        void detect(const Vstream & stream);
        [[nodiscard]] vector<DetObj> detection(float threshold = 0.5) const;
        
    };
}


#endif //ROBOTER_STEUERUNG_JETSON_NANO_TFML_HPP
