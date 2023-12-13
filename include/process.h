#ifndef PROCESS_H
#define PROCESS_H

#include <string>

#include "parser.h"
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid() const;               // See src/process.cpp
  std::string User() const;      // See src/process.cpp
  std::string Command() const;   // See src/process.cpp
  float CpuUtilization() const;  // See src/process.cpp
  void UpdateCpuUtilization(long activeJiffies, long systemJiffies);
  std::string Ram() const;                 // See src/process.cpp
  long int UpTime() const;                 // See src/process.cpp
  bool operator<(Process const &a) const;  // See src/process.cpp
  bool operator>(Process const &a) const;
  void SetParser(Parser *parser) { _parser = parser; };
  explicit Process(int pid, Parser *parser) : pid(pid), _parser(parser){};
  Process() = default;

  bool operator==(int value) const { return pid == value; }

  // Declare any necessary private members
 private:
  int pid = 0;
  Parser *_parser = nullptr;
  long prevSystemJiffies{0};
  long prevActiveJiffies{0};
  float cpuUtilization{0.0f};
};

#endif