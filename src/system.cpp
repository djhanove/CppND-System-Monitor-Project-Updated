#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { 
    return cpu_;
}

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    vector<int> pids = LinuxParser::Pids();
    processes_.clear(); // reset vector so that duplicate items don't stack up
    for (size_t i = 0; i < pids.size(); i++)
    {
        Process proc = Process(pids[i]);
        processes_.push_back(proc);
    }
    std::sort(processes_.begin(), processes_.end(),
        [](Process& procA, Process& procB) {
            // by default sort by highest to lowest CPU Usage
            return procA.CpuUtilization() > procB.CpuUtilization();
        });
    
    return processes_; 
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { 
    return LinuxParser::Kernel();
}

// Return the system's memory utilization
float System::MemoryUtilization() { 
    return LinuxParser::MemoryUtilization();
}

// Return the operating system name
std::string System::OperatingSystem() {
    return LinuxParser::OperatingSystem();
    }

// Return the number of processes actively running on the system
int System::RunningProcesses() {
    return LinuxParser::RunningProcesses();
}

// Return the total number of processes on the system
int System::TotalProcesses() {
    return LinuxParser::TotalProcesses();
}

// Return the number of seconds since the system started running
long int System::UpTime() {
    return LinuxParser::UpTime();
}