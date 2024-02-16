//
// Created by SERT on 29.01.2024.
//

#include "serial/Aserial.hpp"

Aserial::Aserial(const char * port, const speed_t baudRate)
{
    this->port_ = port;
    this->baudRate_ = baudRate;
    this->device_ = open(this->port_, O_RDWR | O_NOCTTY | O_NDELAY);

    if (this->device_ == -1) {
        std::cerr << "Aserial-error: Couldn't open serial port." << std::endl;
        exit(1);
    }

    termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(this->device_, &tty) != 0) {
        std::cerr << "Aserial-error: Couldn't get serial port attributes." << std::endl;
        exit(1);
    }

    cfsetospeed(&tty, baudRate);
    cfsetispeed(&tty, baudRate);

    tty.c_cflag |= (CLOCAL | CREAD); // Enable receiver and set local mode
    tty.c_cflag &= ~PARENB;          // No parity
    tty.c_cflag &= ~CSTOPB;          // 1 stop bit
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;              // 8 data bits

    tcsetattr(this->device_, TCSANOW, &tty);

    this->connected_ = true;
    this->connectionThread_ = thread([&](){this->listenFor_();});
}

Aserial::~Aserial()
{
    this->connected_ = false;
    this->connectionThread_.join();
    for (thread & t : this->listenFuncThreads_)
        t.join();
}

void Aserial::listenFor_()
{
    char buffer[256];
    int bytesRead = 0;
    char c;

    while (this->connected_)
    {
        while (read(this->device_, &c, 1) > 0 && c != '\n')
            buffer[bytesRead++] = c;

        bind funcBind = this->listenForMap[buffer];

        if (funcBind != nullptr)
            this->listenFuncThreads_.emplace_back(thread(funcBind));
    }
}

void Aserial::listenFor(map<const char *, bind> keyCallbackMap)
{
    this->listenForMap_.merge(keyCallbackMap);
}

void Aserial::listenFor(const char * key, bind callback)
{
    this->listenForMap_.insert({key, callback});
}

void Aserial::write(const char * data)
{
    writeSerialPort(data, sizeof(data));
}