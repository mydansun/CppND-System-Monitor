#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return _cpu; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  const set<int> newPids = _parser->Pids();

  // Remove non-existing processes
  for (auto it = _pids.begin(); it != _pids.end();) {
    if (newPids.find(*it) == newPids.end()) {
      _processes.erase(std::remove(_processes.begin(), _processes.end(), *it),
                       _processes.end());
      it = _pids.erase(it);
    } else {
      ++it;
    }
  }

  // Add new processes
  for (const auto newPid : newPids) {
    if (_pids.find(newPid) == _pids.end()) {
      _pids.insert(newPid);
      _processes.emplace_back(newPid, _parser);
    }
  }

  // Update CpuUtilization
  for (auto& process : _processes) {
    process.UpdateCpuUtilization(_parser->ActiveJiffies(process.Pid()),
                                 _parser->Jiffies());
  }

  // Sort by CPU usage (DESC)
  std::stable_sort(_processes.begin(), _processes.end(),
                   [](const auto& a, const auto& b) { return a > b; });
  return _processes;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return _parser->Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return _parser->MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return _parser->OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return _parser->RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return _parser->TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return _parser->UpTime(); }