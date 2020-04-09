#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "linux_parser.h"
#include <vector>

class Processor {
 public:
  float Utilization();

 private:
    long int prevTotal = 0, prevIdle = 0, prevNonIdle = 0;
};

#endif