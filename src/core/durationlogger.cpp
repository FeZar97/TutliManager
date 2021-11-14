#include "durationlogger.h"

DurationLogger::DurationLogger(const std::string name)
{
    measuredEntityName = name;
    begin = std::chrono::steady_clock::now();
}

DurationLogger::~DurationLogger()
{
    std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    Logger::log("Execution of '" + measuredEntityName + "': " + std::to_string(elapsed_ms.count()) + " ms");
}
