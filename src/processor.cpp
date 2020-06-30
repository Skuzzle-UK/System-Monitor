#include "processor.h"
#include "linux_parser.h"
#include <string>
#include <vector>

using std::vector;
using std::string;


#define idle 4
#define iowait 5

float Processor::Utilization() { 
  float timeIdle = LinuxParser::IdleJiffies();
  long timeNotIdle = LinuxParser::ActiveJiffies();
  float total = timeIdle + timeNotIdle;
  return (total - timeIdle) / total;
}