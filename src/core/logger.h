#ifndef LOGGER_H
#define LOGGER_H

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <chrono>
#include <ctime>

namespace Logger {

static std::string getTimeStr(){
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string s(30, '\0');
    std::strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return s;
}

static void log(const std::string &message)
{
    std::cout << getTimeStr() << ": " << message << std::endl;
}

}

#endif // LOGGER_H
