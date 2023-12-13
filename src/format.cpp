#include "format.h"

#include <iomanip>
#include <sstream>

using std::string;

// Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int h, m, s;
  std::stringstream output;
  h = static_cast<int>(seconds / 3600);
  m = static_cast<int>(seconds / 60) % 60;
  s = static_cast<int>(seconds % 60);
  output << std::setfill('0') << std::setw(2) << h << ":";
  output << std::setfill('0') << std::setw(2) << m << ":";
  output << std::setfill('0') << std::setw(2) << s;
  return output.str();
}