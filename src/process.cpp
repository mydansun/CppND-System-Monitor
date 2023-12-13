#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() const { return pid; }

// Return this process's CPU utilization
void Process::UpdateCpuUtilization(const long activeJiffies,
                                   const long systemJiffies) {
  // Backup Method: To calculate avg cpu usage since the process start.
  //  const long cpuTime = _parser->UpTime(pid);
  //  const long totalTime = _parser->UpTime() -
  //  _parser->StartTime(pid); return static_cast<float>(static_cast<long
  //  double>(cpuTime) / totalTime);

  if (prevActiveJiffies == 0 && prevSystemJiffies == 0) {
    prevActiveJiffies = activeJiffies;
    prevSystemJiffies = systemJiffies;
    return;
  }

  const long deltaActive = activeJiffies - prevActiveJiffies;
  const long deltaSystem = systemJiffies - prevSystemJiffies;
  cpuUtilization =
      static_cast<float>(static_cast<long double>(deltaActive) / deltaSystem);

  prevActiveJiffies = activeJiffies;
  prevSystemJiffies = systemJiffies;
}

float Process::CpuUtilization() const { return cpuUtilization; }

// Return the command that generated this process
string Process::Command() const { return _parser->Command(pid); }

// Return this process's memory utilization
string Process::Ram() const { return _parser->Ram(pid); }

// Return the user (name) that generated this process
string Process::User() const { return _parser->User(pid); }

// Return the CPU time of this process (in seconds)
long int Process::UpTime() const { return _parser->UpTime(pid); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}

// Overload the "greater than" comparison operator for Process objects
bool Process::operator>(Process const& a) const {
  return CpuUtilization() > a.CpuUtilization();
}