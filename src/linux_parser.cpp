#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Get OS Info from the Linux File System
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Get Linux Kernel info from the Linux File System
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// Retrieves a list of PIDs on the system from /proc/ dir
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

//  Reads and returns the system memory utilization
float LinuxParser::MemoryUtilization() { 

  // placeholder to capture string from stringstream. String stream reads as ...
  // stream[0] = fieldname:
  // stream[1] = value
  // stream[2] = unit (kB)
  // for each line in file
  string line, placeholder; 
  double memAvail, memTotal;
  float memPerc;
  int count = 0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (count < 3) { // Not going to read the whole file since I know the fields I need
      std::getline(stream, line);
      std::istringstream linestream(line);
      
      if (count == 0) // get total memory on system
        linestream >> placeholder >> memTotal;
      
      if (count == 2) // get memory available
      {
        linestream >> placeholder >> memAvail;
      }
      count++; 
    }
    memPerc = (memTotal - memAvail) / memTotal;
  }
  return memPerc;
}

// returns # of seconds that the system has been up for
long LinuxParser::UpTime() {
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line;
  long upTime = 0;
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime;
  }
  return upTime; 
}

long LinuxParser::Jiffies() { 
    // unused because system jiffies are found in the cpuUtilization function and not necessary as standalone function
    return 0;
}

// Get PID stats and directly calculate CPU (average) usage 
float LinuxParser::PID_CPU_Utilization(int pid) {
  int count = 0;
  string line;
  long ticks, utime, stime, cutime, cstime, totalTime, seconds, uptime;
  ticks = utime = stime = cutime = cstime = totalTime = seconds = uptime = 0;
  std::ifstream stream(kProcDirectory  + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (count < 22) {
      std::getline(stream,line, ' ');
      if (count == 13 && line != "")
      {
        utime = std::stol(line);
      }
      if (count == 14 && line != "")
      {
        stime = std::stol(line);
      }
      if (count == 15 && line != "")
      {
        cutime = std::stol(line);
      }
      if (count == 16 && line != "")
      {
        cstime = std::stol(line);
      }          
      count++;
    }
  }
  if (line != "")
  {
    ticks = std::stol(line); // starttime 
  }
  
  uptime = UpTime(); // Get system uptime
  totalTime = utime + stime + cutime + cstime; // total time spent for the process 
  seconds = uptime - (ticks / sysconf(_SC_CLK_TCK)); // calculate time process has ran relative to system uptime
  return (float(totalTime) / sysconf(_SC_CLK_TCK)) / float(seconds); // return CPU utilization as float
}

//  Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies(long user, long nice, long system, long irq, long softirq, long steal) {
  return user + nice + system + irq + softirq + steal;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies(long idle, long iowait) { 
  return idle + iowait;
}

// Return active and idle jiffies for later use in % utilization
vector<long> LinuxParser::CpuUtilization() { 
  string line, placeholder;
  vector<long> cpuUtilization {};
  long userJiffies, niceJiffies, systemJiffies, idleJiffies, ioWaitJiffies, irqJiffies, softirqJiffies, stealJiffies;
  long activeTotal, idleTotal;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream >> placeholder >> userJiffies >> niceJiffies >> systemJiffies >> idleJiffies >> ioWaitJiffies >>
      irqJiffies >> softirqJiffies >> stealJiffies;
  }
  activeTotal = LinuxParser::ActiveJiffies(userJiffies, niceJiffies, systemJiffies, irqJiffies, softirqJiffies, stealJiffies);
  idleTotal = LinuxParser::IdleJiffies(idleJiffies, ioWaitJiffies);
  cpuUtilization.push_back(activeTotal);
  cpuUtilization.push_back(idleTotal);

  return cpuUtilization;
}

// Return total number of processes
int LinuxParser::TotalProcesses() { 
  string line, placeholder;
  int count = 0;
  int totalProcesses = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (count < 17) { 
      std::getline(stream, line);    
      count++;  
    }
    std::istringstream linestream(line);
    linestream >> placeholder >> totalProcesses;
  }
  return totalProcesses;
}

// Return Number of Running processes
int LinuxParser::RunningProcesses() {
  string line, placeholder;
  int count = 0;
  int totalProcesses = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (count < 18) { 
      std::getline(stream, line);    
      count++;  
    }
    std::istringstream linestream(line);
    linestream >> placeholder >> totalProcesses;
  }
  return totalProcesses;
}

// Get command name of PID
string LinuxParser::Command(int pid) { 
  std::ifstream stream(kProcDirectory  + std::to_string(pid) + kCmdlineFilename);
  string line;
  if (stream.is_open())
  {
    std::getline(stream, line);
  }
  size_t maxSize = 50;
  if(line.size() > maxSize)
    line.resize(maxSize);
  line = line + "...";
  return line;
}

// Get Ram of Process in kB
string LinuxParser::Ram(int pid) { 
  string line, placeholder, memory;
  int count = 0;
  std::ifstream stream(kProcDirectory  + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (count < 18) { // Not going to read the whole file since I know the fields I need
      std::getline(stream, line);  
      count++;
    }
    std::istringstream linestream(line);
    linestream >> placeholder >> memory;
  }
  long memMB = std::stol(memory) / 1024;

  return std::to_string(memMB); 
} 

// Gets user ID from PID Status
string LinuxParser::Uid(int pid) { 
  string line, placeholder, uid;
  int count = 0;
  std::ifstream stream(kProcDirectory  + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (count < 9) { // Not going to read the whole file since I know the fields I need
      std::getline(stream, line);
      count++;
    }
    std::istringstream linestream(line);
    linestream >> placeholder >> uid;
  }
  return uid;
}

// Compares user ID of PID to a series of entries. Returns the User of the PID
string LinuxParser::User(int pid) { 
  string line, placeholder, key, value, candidate;
  key = Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open())
  {
    while (candidate != key) {
      // string takes form of value:x:UID where x is the placeholder
      // use ":" as delimter to seperate entries
      std::getline(stream, value, ':');
      std::getline(stream, placeholder, ':');
      std::getline(stream, candidate, ':');
      std::getline(stream,line); // get rest of line until "/n" char
    }
  } 
  return value;
}
// returns uptime of process in seconds
long LinuxParser::UpTime(int pid) {
  int count = 0;
  string line;
  long uptime = 0, starttime = 0;
  std::ifstream stream(kProcDirectory  + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    while (count < 22) {
      std::getline(stream,line, ' ');
      count++;
    }
  }
  if (line != "")
  {
    starttime = std::stol(line);
  }
  uptime = UpTime(); // get system uptime

   
  return uptime - (starttime / sysconf(_SC_CLK_TCK));
}
