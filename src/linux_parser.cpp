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
//I added a default value if function fail!
template <typename T>
T LinuxParser::findValueByKey(std::string const &keyFilter, std::string const &filename, T const &Default) {
  std::string line, key;
  T value = Default;

  std::ifstream stream(kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) 
          return value;
        else
          value = Default;          
      }
    }
  }
  return value;
};

//Generic function provided by Udacity revisor. Thank you!
//I added a default value if function fail!
template <typename T>
T LinuxParser::getValueOfFile(std::string const &filename, T const &Default) {
  std::string line;
  T value = Default;

  std::ifstream stream(kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
};

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  string name = defaultOSName;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == filterPrettyName) {
          std::replace(value.begin(), value.end(), '_', ' ');
          name = value;
          return name;
        }
      }
    }
  }
  return name;
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
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {  
  float total = findValueByKey<float>(filterMemTotalString, kMeminfoFilename,0);
  float free = findValueByKey<float>(filterMemFreeString, kMeminfoFilename,0);
  if (total > 0)
   return (total - free) / total;
  else
   return 0; 
}

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

long LinuxParser::Jiffies() {
  return IdleJiffies() + ActiveJiffies(); 
}

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

long LinuxParser::ActiveJiffies() {
  vector<string> act = CpuUtilization();
  return stol(act[0]) + stol(act[1]) + stol(act[2])  //user nice system
       + stol(act[5]) + stol(act[6]) + stol(act[7]); //irq softirq steal
}

long LinuxParser::IdleJiffies() {
  vector<string> idle = CpuUtilization();
  return stol(idle[3]) + stol(idle[4]); //idle iowait
}

vector<string> LinuxParser::CpuUtilization() {
  string line;
  string value;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    if (value == filterCpu)
     for (int i = 0; i<10; i++)
      {
        linestream >> value;
        jiffies.emplace_back(value);
      }
 
  }
  return jiffies;    
}

int LinuxParser::TotalProcesses() {
  return findValueByKey<int>(filterProcesses, kStatFilename, 0);
}

int LinuxParser::RunningProcesses() {
   return findValueByKey<int>(filterRunningProcesses, kStatFilename, 0);
}

string LinuxParser::Command(int pid) {
  return getValueOfFile<string>(to_string(pid) + kCmdlineFilename, defaultStringValue);    
}

string LinuxParser::Ram(int pid) {
  long value = findValueByKey<long>(filterProcMem,to_string(pid) + kStatusFilename, 0);
  return  to_string( static_cast<int>( value / 1000 )); 
}

string LinuxParser::Uid(int pid) {
  return findValueByKey<string>(filterUID,to_string(pid) + kStatusFilename, defaultStringValue);   
}

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
