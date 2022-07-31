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


int Process::Pid() {
    return pid_; 
}

float Process::CpuUtilization() {
    float total_time = (float)LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
    float seconds = (float)LinuxParser::UpTime() - (float)LinuxParser::UpTime(pid_);    
    if (seconds > 0)
     return ( total_time / seconds );  
    else
     return 0;
}

string Process::Command() {
    string cmd = LinuxParser::Command(pid_);
    if (cmd.size()<=50)
      return cmd;
    else
      return cmd.substr(0,46) + "...";
}

string Process::Ram() {    
    return LinuxParser::Ram(pid_); 
}

string Process::User() {
    string cmd = LinuxParser::User(pid_);
    if (cmd.size()<=6)
      return cmd;
    else
      return cmd.substr(0,3) + ".. ";    
}

long int Process::UpTime() {
    return LinuxParser::UpTime() - LinuxParser::UpTime(pid_);    
}

bool Process::operator<(Process const& a) const {    
    return (a.cpu_util < this->cpu_util);    
}
