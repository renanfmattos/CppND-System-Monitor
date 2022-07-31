#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using namespace std;


Processor& System::Cpu() {
    return cpu_; 
}

vector<Process>& System::Processes() {    
    vector<int> pids = LinuxParser::Pids();
    processes_.clear();
    for (auto i : pids){      
      processes_.emplace_back(i);
    }
    std::sort(processes_.begin(),processes_.end(),std::less<Process>());    
    return processes_; 
}

std::string System::Kernel() {
    return LinuxParser::Kernel(); 
}

float System::MemoryUtilization() { 
    return LinuxParser::MemoryUtilization();
}

std::string System::OperatingSystem() { 
    return LinuxParser::OperatingSystem(); 
}

int System::RunningProcesses() {
     return LinuxParser::RunningProcesses();
}

int System::TotalProcesses() {
     return LinuxParser::TotalProcesses();
}

long int System::UpTime() {
     return LinuxParser::UpTime();     
}