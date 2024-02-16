//
// Created by SERT on 11.01.2024.
//

#include "../../include/video/Vstream.hpp"

Vstream::Vstream(const String& filename)
{
    this->stream_ = VideoCapture(filename);
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
}

void Vstream::stop()
{
    this->running_ = false;
    this->guiThread_.join();
    this->streamThread_.join();
    this->stream_.release();
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