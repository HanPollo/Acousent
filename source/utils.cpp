#define _CRT_SECURE_NO_WARNINGS

#include "utils.h"

std::string currentTime(std::chrono::time_point<std::chrono::system_clock> now)
{
    // you need to get milliseconds explicitly
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
        ) % 1000;
    // and that's a "normal" point of time with seconds
    auto timeNow = std::chrono::system_clock::to_time_t(now);

    std::ostringstream currentTimeStream;
    currentTimeStream << std::put_time(localtime(&timeNow), "%d.%m.%Y %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << milliseconds.count()
        << " " << std::put_time(localtime(&timeNow), "%z");

    return currentTimeStream.str();
}