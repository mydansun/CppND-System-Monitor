#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric
criteria "The student will be able to extract and display basic data about the
system."

You need to properly format the uptime. Refer to the comments mentioned in
format. cpp for formatting the uptime.*/

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  const set<int> newPids = LinuxParser::Pids();

  // Remove non-existing processes
  for (auto it = pids.begin(); it != pids.end();) {
    if (newPids.find(*it) == newPids.end()) {
      processes_.erase(std::remove(processes_.begin(), processes_.end(), *it),
                       processes_.end());
      it = pids.erase(it);
    } else {
      ++it;
    }
  }

  // Add new process
  for (const auto newPid : newPids) {
    if (pids.find(newPid) == pids.end()) {
      pids.insert(newPid);
      processes_.emplace_back(newPid);
    }
  }

  // Update CpuUtilization
  for (auto& process : processes_) {
    process.UpdateCpuUtilization(LinuxParser::ActiveJiffies(process.Pid()),
                                 LinuxParser::Jiffies());
  }
  std::stable_sort(processes_.begin(), processes_.end());
  std::reverse(processes_.begin(), processes_.end());
  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }