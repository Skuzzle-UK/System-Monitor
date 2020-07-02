#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "system.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

// BONUS: Update this to use std::filesystem
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

float LinuxParser::MemoryUtilization() {
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  string line;
  float memTotal, memFree;
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string key, unit;
      double value;
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal:") {
          memTotal = value;
        }
        if (key == "MemFree:") {
          memFree = value;
        }
      }
    }
    return (memTotal - memFree) / memTotal;
  }
  return 0.0;
}

long LinuxParser::UpTime() { 
  long uptime, something;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> something;
    return uptime;
  }
  return 0;
}


long LinuxParser::Jiffies() {
  vector<string> cpuStrings = LinuxParser::CpuUtilization();
  long jiffies = 0;
  for(std::vector<int>::size_type i = 1; i < cpuStrings.size(); i++){
    long longString;
    std::istringstream longStream(cpuStrings[i]);
    longStream >> longString;
    jiffies += longString;
  }
  return jiffies; }

long LinuxParser::ActiveJiffies(int pid) { 
  string line, ignore;
  long utime, stime, cutime, cstime, starttime, total;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> ignore >> ignore >> ignore >> ignore >> ignore
    >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
    >> ignore >> ignore >> utime  >> stime  >> cutime  >> cstime
    >> ignore >> ignore >> ignore >> ignore >> starttime;
  total = utime + stime + cutime + cstime;
  return total;
  }
  return 0; }


long LinuxParser::ActiveJiffies() {
  vector<string> cpuStrings = LinuxParser::CpuUtilization();
  vector<int> cpuInts;
  for(std::vector<int>::size_type i = 0; i < cpuStrings.size(); i++){
    int intString;
    std::istringstream intStream(cpuStrings[i]);
    intStream >> intString;
    cpuInts.push_back(intString);
  }
  
  CPUStates user = kUser_, nice = kNice_, system = kSystem_, irq = kIRQ_, softirq = kSoftIRQ_, steal = kSteal_;
  long notIdleJiffies = cpuInts[user] + cpuInts[nice] + cpuInts[system] + cpuInts[irq] + cpuInts[softirq] + cpuInts[steal];
  return notIdleJiffies;
}

long LinuxParser::IdleJiffies() { 
  vector<string> cpuStrings = LinuxParser::CpuUtilization();
  vector<int> cpuInts;
  for(std::vector<int>::size_type i = 0; i < cpuStrings.size(); i++){
    int intString;
    std::istringstream intStream(cpuStrings[i]);
    intStream >> intString;
    cpuInts.push_back(intString);
  }
  
  CPUStates idle = kIdle_, iowait = kIOwait_;
  long idleJiffies = cpuInts[idle] + cpuInts[iowait];
  return idleJiffies;
 }


vector<string> LinuxParser::CpuUtilization() { 
  string cpu, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  string line;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
  	vector<string> cpuString{cpu, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice};
    return { cpuString };
  }
  return {};
}

int LinuxParser::TotalProcesses() {
  vector<int> processes = LinuxParser::Pids();
  return processes.size();
}

int LinuxParser::RunningProcesses() {
string line;
int runningprocs = 0;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      string key;
      int value;
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          runningprocs = value;
        }
      }
    }
    return runningprocs;  
  }
return 0;
}


string LinuxParser::Command(int pid) { 
  string line, command;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> command;
    return command;
  }
  return string();
  }

string LinuxParser::Ram(int pid) {
  string line, key, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if(key == "VmSize:"){
          return value;
        }
      }
    }
  }
  return string();
}


string LinuxParser::Uid(int pid) { 
  string line, key, value, returnVal;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          returnVal = value;
        }
      }
    }
    return returnVal;
  }

    return string();
  }


string LinuxParser::User(int pid) { 
    string uid = LinuxParser::Uid(pid);
    string line, usr, x, u_i_d;
    std::ifstream stream(kPasswordPath);
    if (stream.is_open()) {
      while (std::getline(stream, line)){
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        linestream >> usr >> x >> u_i_d;
        if (u_i_d == uid){
          return usr;
        }
      }
    }
  return string();
  }

long LinuxParser::UpTime(int pid) { 
  string line, ignore;
  long starttime, seconds;

  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> ignore >> ignore >> ignore >> ignore >> ignore
    >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
    >> ignore >> ignore >> ignore  >> ignore  >> ignore  >> ignore
    >> ignore >> ignore >> ignore >> ignore >> starttime;
  
  long uptime = LinuxParser::UpTime();
  long hertz = sysconf(_SC_CLK_TCK); 
  seconds = uptime - (starttime / hertz);

  return seconds;
  }
  return 0;
}