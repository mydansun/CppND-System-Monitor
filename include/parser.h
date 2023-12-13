#ifndef MONITOR_PARSER_H
#define MONITOR_PARSER_H

#include <set>
#include <string>
#include <vector>

/**
 * This is the abstract class of Parser.
 * Each operating system should have its own implementation of Parser.
 * @see LinuxParser
 */
class Parser {
 public:
  virtual float MemoryUtilization() = 0;
  virtual long UpTime() = 0;
  virtual std::set<int> Pids() = 0;
  virtual int TotalProcesses() = 0;
  virtual int RunningProcesses() = 0;
  virtual std::string OperatingSystem() = 0;
  virtual std::string Kernel() = 0;
  virtual std::vector<std::string> CpuUtilization() = 0;
  virtual long Jiffies() = 0;
  virtual long ActiveJiffies(int pid) = 0;
  virtual long ActiveJiffies() = 0;
  virtual long IdleJiffies() = 0;

  virtual std::string Command(int pid) = 0;
  virtual std::string Ram(int pid) = 0;
  virtual std::string Uid(int pid) = 0;
  virtual std::string User(int pid) = 0;
  virtual long UpTime(int pid) = 0;
};

#endif  // MONITOR_PARSER_H
