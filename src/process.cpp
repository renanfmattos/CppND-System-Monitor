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

// DONE: Return this process's ID
int Process::Pid() {
    return pid_; 
}

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() {
    float total_time = (float)LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK);
    float seconds = (float)LinuxParser::UpTime() - (float)LinuxParser::UpTime(pid_);    
    return ( total_time / seconds );  
}

// DONE: Return the command that generated this process
string Process::Command() {
    string cmd = LinuxParser::Command(pid_);
    if (cmd.size()<=50)
      return cmd;
    else
      return cmd.substr(0,46) + "...";

}

// DONE: Return this process's memory utilization
string Process::Ram() {    
    return LinuxParser::Ram(pid_); 
}

// DONE: Return the user (name) that generated this process
string Process::User() {
    string cmd = LinuxParser::User(pid_);
    if (cmd.size()<=6)
      return cmd;
    else
      return cmd.substr(0,3) + ".. ";    
}

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() {
    return LinuxParser::UpTime() - LinuxParser::UpTime(pid_);    
}

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {    
    return (a.cpu_util < this->cpu_util);    
}
