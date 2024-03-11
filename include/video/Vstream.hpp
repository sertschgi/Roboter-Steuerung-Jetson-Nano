//
// Created by SERT on 11.01.2024.
//

#ifndef ROBOTER_STEUERUNG_JETSON_NANO_VSTREAM_HPP
#define ROBOTER_STEUERUNG_JETSON_NANO_VSTREAM_HPP

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"

#include <thread>
#include <iostream>

#include <Argus/Argus.h>
#include <EGLStream/EGLStream.h>

using namespace std;
using namespace cv;

using Vframe = Mat;

  
#define VSTREAM_DEFAULT_DEVICE_ID 0
#define VSTREAM_DEFAULT_FLIP_METHOD 0
#define VSTREAM_DEFAULT_INPUT_WIDTH 1920
#define VSTREAM_DEFAULT_INPUT_HEIGHT 1080
#define VSTREAM_DEFAULT_INPUT_FORMAT "NV12"
#define VSTREAM_DEFAULT_FRAMERATE "1/30"

class Vstream {
private:
    const uint8_t deviceId_;
    const uint8_t flipMethod_;
    const uint16_t inputWidth_;
    const uint16_t inputHeight_;
    const uint16_t outputWidth_;
    const uint16_t outputHeight_;
    const string inputFormat_;
    const string outputFormat_;
    const string streamFramerate_; 
    bool running_ {};
    Vframe frame_ {};
    VideoCapture stream_ {};
    thread streamThread_ {};
    thread guiThread_ {};
    void read_();
    void gui_();
    string gstPipeline_() const;

public:
    Vstream(
	const uint8_t & deviceId, 
	const uint8_t & flipMethod, 
	const uint16_t & inputWidth, 
	const uint16_t & inputHeight,
	const uint16_t & outputWidth,
	const uint16_t & ouputHeight,
	const string & inputFormat,
	const string & framerate);
    Vstream(
	const uint8_t & deviceId = 0,
	const uint8_t & flipMethod = 0, 
	const uint16_t & inputWidth = 1920,
	const uint16_t & inputHeight = 1080,	
	const string & inputFormat = "NV12",
	const string & framerate = "1/30");
    ~Vstream();
    void start();
    void stop();
    [[nodiscard]] bool running() const;
    [[nodiscard]] Vframe frame() const;
    void gui();
};


#endif //ROBOTER_STEUERUNG_JETSON_NANO_VSTREAM_HPP
