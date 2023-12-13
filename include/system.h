#ifndef SYSTEM_H
#define SYSTEM_H

#include <set>
#include <string>
#include <vector>

#include "parser.h"
#include "process.h"
#include "processor.h"

class System {
 public:
  Processor &Cpu();
  std::vector<Process> &Processes();
  float MemoryUtilization();
  long UpTime();
  int TotalProcesses();
  int RunningProcesses();
  std::string Kernel();
  std::string OperatingSystem();
  void SetParser(Parser *parser) { _parser = parser; }
  explicit System(Parser *parser) : _parser(parser) { _cpu.SetParser(parser); };
  System() = default;

 private:
  Parser *_parser = nullptr;
  Processor _cpu{};
  std::set<int> _pids = {};
  std::vector<Process> _processes = {};
};

#endif