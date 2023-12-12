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
// It is avg cpu usage since the process start
void Process::UpdateCpuUtilization(const long activeTicks,
                                   const long systemTicks) {
  // Backup: To calculate avg cpu usage since the process start.
  //  const long cpuTime = LinuxParser::UpTime(pid);
  //  const long totalTime = LinuxParser::UpTime() -
  //  LinuxParser::StartTime(pid); return static_cast<float>(static_cast<long
  //  double>(cpuTime) / totalTime);

  if (prevActiveTicks == 0 && prevSystemTicks == 0) {
    prevActiveTicks = activeTicks;
    prevSystemTicks = systemTicks;
    return;
  }

  const long deltaActive = activeTicks - prevActiveTicks;
  const long deltaSystem = systemTicks - prevSystemTicks;
  cpuUtilization =
      static_cast<float>(static_cast<long double>(deltaActive) / deltaSystem);

  prevActiveTicks = activeTicks;
  prevSystemTicks = systemTicks;
}

float Process::CpuUtilization() const { return cpuUtilization; }

// Return the command that generated this process
string Process::Command() const { return LinuxParser::Command(pid); }

// Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(pid); }

// Return the user (name) that generated this process
string Process::User() const { return LinuxParser::User(pid); }

// Return the CPU time of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(pid); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}