#include <string>
#include <sstream>
#include <iomanip>
#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long time) {
    long hour, min, sec;
    hour = time/3600;
    min = (time%3600) / 60;
    sec = (time%3600) % 60;
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << std::to_string(hour) << ":" << std::setw(2) << std::setfill('0') << std::to_string(min)
        << ":" << std::setw(2) << std::setfill('0') << std::to_string(sec);
    return ss.str();
}