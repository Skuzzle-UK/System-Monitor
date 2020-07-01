#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid):pid_(pid){}
 

int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() { 
    long total_time = LinuxParser::ActiveJiffies(pid_);
    long seconds = LinuxParser::UpTime(pid_);
    long hertz = CLOCKS_PER_SEC;
    float cpu_usage = 100 * ((total_time / hertz) / seconds);
    return cpu_usage;
}


string Process::Command() { return LinuxParser::Command(Process::Pid()); }

// TODO: Return this process's memory utilization
string Process::Ram() { return string(); }


string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds) - Maybe done
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }
