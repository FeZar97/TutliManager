#ifndef DURATIONLOGGER_H
#define DURATIONLOGGER_H

#include "logger.h"

class DurationLogger
{
    std::string measuredEntityName{""};
    std::chrono::time_point<std::chrono::steady_clock> begin;
public:
    DurationLogger(const std::string name = "");
    ~DurationLogger();
};

#endif // DURATIONLOGGER_H
