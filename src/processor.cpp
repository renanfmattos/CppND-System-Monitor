#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
    if (prevIdle == 0)  
      prevIdle = LinuxParser::IdleJiffies();
    if (prevActive == 0)  
      prevActive = LinuxParser::ActiveJiffies();


        
    long active = LinuxParser::ActiveJiffies();
    long idle = LinuxParser::IdleJiffies();
    long total = LinuxParser::Jiffies();
    
    long totald = total - (prevIdle + prevActive);
    long idled = idle - prevIdle;

    prevIdle = idle;
    prevActive = active;

    float result = 0;
    if (totald != 0)
     result =  (totald - idled) / static_cast<float>(totald);

    return result; 
}