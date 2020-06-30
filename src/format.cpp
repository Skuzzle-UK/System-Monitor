#include <string>

#include "format.h"

using std::string;

string Format::ElapsedTime(long seconds) {
  int hours, minutes;
  hours = seconds / 60 / 60;
  seconds = seconds - (hours * 60 * 60);
  minutes = seconds / 60;
  seconds = seconds - (minutes * 60);
  
  string hh, mm, ss, output;
  
  if(hours < 10){
    hh = "0" + std::to_string(hours);
  }
  else{
    hh = std::to_string(hours);
  }
  
  if(minutes < 10){
    mm = "0" + std::to_string(minutes);
  }
  else{
    mm = std::to_string(minutes);
  }
  
  if(seconds < 10){
    ss = "0" + std::to_string(seconds);
  }
  else{
    ss = std::to_string(seconds);
  }
  
  output = hh + ":" + mm + ":" + ss;
  
  return output;
}