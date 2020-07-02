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

float Process::CpuUtilization() const { 
    long total_time = LinuxParser::ActiveJiffies(pid_);
    long seconds = LinuxParser::UpTime(pid_);
    float cpu_usage = (1.0*(total_time/sysconf(_SC_CLK_TCK)) / seconds);   
    return cpu_usage;
}


string Process::Command() { return LinuxParser::Command(Process::Pid()); }

string Process::Ram() { 
  string ram_kb = LinuxParser::Ram(pid_);
  int ram_stream;
  string ram_mb;
  std::stringstream stream;
  stream << ram_kb;
  stream >> ram_stream;
  ram_mb = std::to_string(ram_stream / 1024);
  return ram_mb;
}

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

//Overload operator for sort function
bool Process::operator<(Process const& a) const {
return (this->CpuUtilization() > a.CpuUtilization());
}
