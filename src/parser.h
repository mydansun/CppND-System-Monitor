#ifndef MONITOR_PARSER_H
#define MONITOR_PARSER_H

#include <string>

class Parser {
  virtual std::string OperatingSystem() = 0;
};

#endif  // MONITOR_PARSER_H
