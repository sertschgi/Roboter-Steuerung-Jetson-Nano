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

using namespace std;
using namespace cv;

using Vframe = Mat;

class Vstream {
private:
    bool running_ {};
    Vframe frame_ {};
    VideoCapture stream_ {};
    thread streamThread_ {};
    thread guiThread_ {};
    void read_();
    void gui_();
public:
    explicit Vstream(const String& filename);
    ~Vstream();
    void start();
    void stop();
    [[nodiscard]] bool running() const;
    [[nodiscard]] Vframe frame() const;
    void gui();
};


#endif //ROBOTER_STEUERUNG_JETSON_NANO_VSTREAM_HPP
