#include <string>
//#include <cmath>
#include <chrono>
#include <sstream>
#include <iomanip>



#include "format.h"

using std::string;
using std::to_string;


string Format::ElapsedTime(long s) {
  std::chrono::seconds seconds{s};

  // return std::chrono::format("%T", seconds); // in C++20 :-)

  std::chrono::hours hours =
      std::chrono::duration_cast<std::chrono::hours>(seconds);

  seconds -= std::chrono::duration_cast<std::chrono::seconds>(hours);

  std::chrono::minutes minutes =
      std::chrono::duration_cast<std::chrono::minutes>(seconds);

  seconds -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

  std::stringstream ss{};

  ss << std::setw(2) << std::setfill('0') << hours.count()     // HH
     << std::setw(1) << ":"                                    // :
     << std::setw(2) << std::setfill('0') << minutes.count()   // MM
     << std::setw(1) << ":"                                    // :
     << std::setw(2) << std::setfill('0') << seconds.count();  // SS

  return ss.str();
}

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
//https://www.inchcalculator.com/seconds-to-time-calculator/
/*string Format::ElapsedTime(long seconds) {
   int HH, MM, SS;
   float calc, whole;
   calc = static_cast<float>(seconds) / 3600;
   calc = std::modf(calc,&whole);
   HH = static_cast<int>(whole);
   calc = calc * 60;
   calc = std::modf(calc,&whole);
   MM = static_cast<int>(whole);
   calc = calc * 60;
   calc = std::modf(calc,&whole);
   SS = static_cast<int>(whole);   

   string Result;
   Result = to_string(HH) + ':' + to_string(MM) + ':' + to_string(SS);
   return Result; 
}*/
