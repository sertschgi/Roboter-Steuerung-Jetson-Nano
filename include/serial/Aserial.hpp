//
// Created by SERT on 29.01.2024.
//

#ifndef ROBOTER_STEUERUNG_JETSON_NANO_ASERIAL_HPP
#define ROBOTER_STEUERUNG_JETSON_NANO_ASERIAL_HPP

#include <iostream>
#include <thread>
#include <cstring>
#include <map>
#include <vector>
#include <initializer_list>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>


using namespace std;

class Aserial {
private:
    const char * port_;
    const speed_t baudRate_;
    bool connected_ {};
    int device_ {};
    map<const char *, function<void()>> listenForMap_ {};
    vector<thread> listenFuncThreads_ {};
    thread connectionThread_ {};
    void listenFor_();

public:
    Aserial(const char * port, const speed_t baudRate);
    ~Aserial();
    void write(const char * data);
    void listenFor(map<const char *, function<void()>>  keyCallbackMap);
    void listenFor(const char * key, function<void()> callback);
};


#endif //ROBOTER_STEUERUNG_JETSON_NANO_ASERIAL_HPP
