#include <string>
#include <cmath>

#include "format.h"

using std::string;
using std::to_string;


// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
//https://www.inchcalculator.com/seconds-to-time-calculator/
string Format::ElapsedTime(long seconds) {
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
}
