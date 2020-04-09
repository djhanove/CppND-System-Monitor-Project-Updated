#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    std::vector<long> jiffies =  LinuxParser::CpuUtilization(); // jiffies[0] = active, jiffies[1] = idle
    long int total = jiffies[0] + jiffies[1];
    long int dTotal = total - prevTotal;
    long int dIdle = jiffies[1] - prevIdle;
    //std::cout << "Total: " << total << std::endl;
    float CPU_Percentage = (float(dTotal) - float(dIdle)) / float(dTotal);
    
    // update class member values for more accurate CPU utilization
    prevNonIdle = jiffies[0];
    prevIdle = jiffies[1];
    prevTotal = total;

    return CPU_Percentage;
    
    }