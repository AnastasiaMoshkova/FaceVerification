#ifndef LOGGER_H
#define LOGGER_H

#include <sstream>
#include <iostream>
#include <fstream>
#include "basictypes.h"

const bool LOGGER = true;

class Logger{
private:

    static void func(std::string str) {
        std::cout << str;
    }

public:
    template<typename ... Types>
    static void LOGE(Types... args){
        if (LOGGER) {
            std::cout << "ERROR: " << getTimeStamp() << ": " << getLogString(args...) << "\n";
        }
    }
    template<typename ... Types>
    static void LOGI(Types... args){
        if (LOGGER){
            std::cout << getTimeStamp() << ": " << getLogString(args...) << "\n";
        }
    }
};

#endif // LOGGER_H
