#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid() const;                         // See src/process.cpp
  std::string User() const;                // See src/process.cpp
  std::string Command() const;             // See src/process.cpp
  float CpuUtilization() const;            // See src/process.cpp
  void UpdateCpuUtilization(long activeTicks,long systemTicks);
  std::string Ram() const;                 // See src/process.cpp
  long int UpTime() const;                 // See src/process.cpp
  bool operator<(Process const& a) const;  // See src/process.cpp
  explicit Process(int pid) : pid(pid){};
  Process() = default;

  bool operator==(int value) const { return pid == value; }

  // Declare any necessary private members
 private:
  int pid;
  long prevSystemTicks{0};
  long prevActiveTicks{0};
  float cpuUtilization{0.0f};
};

#endif