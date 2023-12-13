#ifndef MONITOR_LINUX_PARSER_H
#define MONITOR_LINUX_PARSER_H

#include <unistd.h>

#include <algorithm>
#include <experimental/filesystem>
#include <experimental/optional>
#include <fstream>
#include <initializer_list>
#include <set>
#include <string>
#include <vector>

#include "parser.h"
class LinuxParser : public Parser {
 private:
  const std::string kProcDirectory{"/proc/"};
  const std::string kCmdlineFilename{"/cmdline"};
  const std::string kCpuinfoFilename{"/cpuinfo"};
  const std::string kStatusFilename{"/status"};
  const std::string kStatFilename{"/stat"};
  const std::string kUptimeFilename{"/uptime"};
  const std::string kMeminfoFilename{"/meminfo"};
  const std::string kVersionFilename{"/version"};
  const std::string kOSPath{"/etc/os-release"};
  const std::string kPasswordPath{"/etc/passwd"};

  enum CPUStates {
    kUser_ = 0,
    kNice_,
    kSystem_,
    kIdle_,
    kIOwait_,
    kIRQ_,
    kSoftIRQ_,
    kSteal_,
    kGuest_,
    kGuestNice_
  };

  std::vector<std::string> getProcessStat(int pid);

  int ReadStatInt(const std::string& item);

  long CalculateJiffies(std::initializer_list<int> keys);

  std::string readProcessStatus(int pid, const std::string& item);

 public:
  float MemoryUtilization() override;

  long UpTime() override;

  std::set<int> Pids() override;

  int TotalProcesses() override;

  int RunningProcesses() override;

  std::string OperatingSystem() override;

  std::string Kernel() override;

  std::vector<std::string> CpuUtilization() override;

  long Jiffies() override;

  // Read and return the number of active jiffies for a PID
  long ActiveJiffies(int pid) override;

  // Read and return the number of active jiffies for the system
  long ActiveJiffies() override;

  // Read and return the number of idle jiffies for the system
  long IdleJiffies() override;

  // Read and return the command associated with a process
  std::string Command(int pid) override;

  // Read and return the memory used by a process
  std::string Ram(int pid) override;

  // Read and return the user ID associated with a process
  std::string Uid(int pid) override;

  // Read and return the user associated with a process
  std::string User(int pid) override;

  long UpTime(int pid) override;
};

#endif  // MONITOR_LINUX_PARSER_H
