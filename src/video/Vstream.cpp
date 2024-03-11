//
// Created by SERT on 11.01.2024.
//

#include "video/Vstream.hpp"

string Vstream::gstPipeline_() const
{
    return "nvarguscamerasrc sensor_id=" + to_string(this->deviceId_) +
	   " ! video/x-raw(memory:NVMM), width=" + to_string(this->inputWidth_) +
	   ", height=" + to_string(this->inputHeight_) +
	   ", framerate=" + this->streamFramerate_ +
	   ", format=" + this->inputFormat_ +
	   " ! nvvidconv flip-method=" + to_string(this->flipMethod_) +
	   " ! video/x-raw, format=BGRx, width=" + to_string(this->outputWidth_) +
	   ", height=" + to_string(this->outputHeight_) +
	   ", pixel-aspect-ratio=1/1 ! videoconvert ! video/x-raw, format=BGR, appsink drop=1";
}

Vstream::Vstream(
	const uint8_t & deviceId, 
	const uint8_t & flipMethod, 
	const uint16_t & inputWidth, 
	const uint16_t & inputHeight,
        const uint16_t & outputWidth,
	const uint16_t & outputHeight,	
	const string & inputFormat, 
	const string & framerate) : deviceId_(deviceId), 
		  		    flipMethod_(flipMethod), 
		      	 	    inputWidth_(inputWidth), 
				    inputHeight_(inputHeight), 
				    outputWidth_(outputWidth), 
				    outputHeight_(outputHeight),
				    inputFormat_(inputFormat),
				    streamFramerate_(framerate)
{
    int apiID = cv::CAP_GSTREAMER;
    const string pipeline = this->gstPipeline_();
    this->stream_.open(pipeline, apiID);
    if (!this->stream_.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return;
    }
    cout << "[Info]: initialized VideoCapture." << endl;
}

Vstream::Vstream(
	const uint8_t & deviceId, 
	const uint8_t & flipMethod, 
	const uint16_t & inputWidth,
	const uint16_t & inputHeight,	
	const string & inputFormat,
	const string & framerate) : Vstream(deviceId, flipMethod, inputWidth, inputHeight, inputWidth, inputHeight, inputFormat, framerate) {} 

Vstream::~Vstream()
{
    this->stop();
}

void Vstream::read_()
{
    while (this->running_)
    {
        this->stream_.read(this->frame_);
    }
}

void Vstream::start()
{
    this->streamThread_.join();
    this->running_ = true;
    this->streamThread_ = thread([&]{this->read_();});
    cout << "started VideoCapture.";
}

void Vstream::stop()
{
    this->running_ = false;
    this->guiThread_.join();
    this->streamThread_.join();
    this->stream_.release();
    cout << "stopped VideoCapture.";
}

void Vstream::gui_()
{
    for (;;)
    {
        imshow("gui", this->frame_);
        if (waitKey(5) >= 0)
            break;
    }
}

void Vstream::gui()
{
    this->guiThread_ = thread([&]{this->gui_();});
}

bool Vstream::running() const
{
    return this->running_;
}

Vframe Vstream::frame() const
{
    return this->frame_;
}
