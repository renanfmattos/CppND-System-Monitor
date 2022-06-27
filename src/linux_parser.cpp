#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

//Generic function provided by Udacity revisor. Thank you!
template <typename T>
T LinuxParser::findValueByKey(std::string const &keyFilter, std::string const &filename) {
  std::string line, key;
  T value;

  std::ifstream stream(kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  return value;
};

//Generic function provided by Udacity revisor. Thank you!
template <typename T>
T LinuxParser::getValueOfFile(std::string const &filename) {
  std::string line;
  T value;

  std::ifstream stream(kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
};


// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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

// DONE: Read and return the system memory utilization By Renan
float LinuxParser::MemoryUtilization() {
  unsigned int MemTotal, MemFree, MemUsed;
  string key;
  string value;
  string line;
  float Result = 0.0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "MemTotal:")
        MemTotal = std::stoi(value);
      else if (key == "MemFree:"){
        MemFree = std::stoi(value);
        break;
      }        
    }
      MemUsed = MemTotal - MemFree;
      Result = static_cast<float>(MemUsed) / MemTotal; 
  } 
  return Result; 
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string uptime = "0";
  string timeidle = "0";
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
    if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> uptime >> timeidle;
    }  
  return std::stol(uptime);   
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return IdleJiffies() + ActiveJiffies(); 
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  const string path {kProcDirectory + to_string(pid) + kStatFilename};  
  std::ifstream stream(path);
  long total = 0;
    if (stream.is_open()) 
    {
      string line;
      std::getline(stream, line);
      std::istringstream linestream(line);
      string utime;
      string stime;
      string cutime;
      string cstime;      
      for (int i = 1; i<=17; i++)
      {
        if (i<=14)
         linestream >> utime;
        else if (i==15) 
         linestream >> stime;
        else if (i==16) 
         linestream >> cutime; 
        else if (i==17) 
         linestream >> cstime;         
      }
      total += stol(utime) + stol(stime) + stol(cutime) + stol(cstime);       
    }    
  return total;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> act = CpuUtilization();
  return stol(act[0]) + stol(act[1]) + stol(act[2])  //user nice system
       + stol(act[5]) + stol(act[6]) + stol(act[7]); //irq softirq steal
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> idle = CpuUtilization();
  return stol(idle[3]) + stol(idle[4]); //idle iowait
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  string value;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    if (value == "cpu")
     for (int i = 0; i<10; i++)
      {
        linestream >> value;
        jiffies.push_back(value);
      }
 
  }
  return jiffies;    
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key;
  string value = "-1";
  string line;  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes")
        break;              
    }
  } 
   return std::stoi(value);   
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key;
  string value = "-1";
  string line;  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running")
        break;              
    }
  } 
   return std::stoi(value);   
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  const string path {kProcDirectory + to_string(pid) + kCmdlineFilename};  
  std::ifstream stream(path);
  string cmdline;
    if (stream.is_open()) 
    {      
      std::getline(stream, cmdline);     
    }  
  return cmdline;    
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  const string path {kProcDirectory + to_string(pid) + kStatusFilename};  
  std::ifstream stream(path);
  string vmsize {};  
  if (stream.is_open()) {
    string line;
    string key;
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> vmsize;
      if (key == "VmSize:")
        break;              
    }
  }
 
  return  to_string( static_cast<int>( std::stol(vmsize) / 1000 )); 
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  const string path {kProcDirectory + to_string(pid) + kStatusFilename};  
  std::ifstream stream(path);
  string uid {};  
  if (stream.is_open()) {
    string line;
    string key;
    while (std::getline(stream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> uid;
      if (key == "Uid:")
        break;              
    }
  }
  return uid; 
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string name {};
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    string line;
    string uid = Uid(pid);
    string uid_;
    while (std::getline(stream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> uid_ >> uid_;
      if (uid_ == uid)
        break;              
    }
  }  
  return name; 
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  const string path {kProcDirectory + to_string(pid) + kStatFilename};  
  std::ifstream stream(path);
  
  if (stream.is_open()) 
   {
      string line;
      string starttime;
      std::getline(stream, line);
      std::istringstream linestream(line);      
      for (int i = 1; i<=22; i++)
        linestream >> starttime;
      
      return std::stol(starttime) / sysconf(_SC_CLK_TCK);   
   }
  else
    return 0;   
}
