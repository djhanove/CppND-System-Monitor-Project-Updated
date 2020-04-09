#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return PID; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
    return LinuxParser::PID_CPU_Utilization(PID);
}

// Return the command that generated this process
string Process::Command() {
    return LinuxParser::Command(PID);
}

// Return this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(PID);
}

// Return the user (name) that generated this process
string Process::User() {
    return LinuxParser::User(PID);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
    return LinuxParser::UpTime(PID);
}