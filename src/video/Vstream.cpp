//
// Created by SERT on 11.01.2024.
//

#include "video/Vstream.hpp"

Vstream::Vstream(const String& filename)
{
    int apiID = cv::CAP_ANY;
    cap.open(filename, apiID);
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return;
    }
    cout << "initialized VideoCapture.";
}

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
