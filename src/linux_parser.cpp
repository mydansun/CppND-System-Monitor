#include "linux_parser.h"

#include <unistd.h>

#include <filesystem>
#include <initializer_list>
#include <optional>
#include <string>
#include <vector>
#include <set>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

/**
 * Get all existing pids by using std::filesystem
 * @return
 */
std::set<int> LinuxParser::Pids() {
  std::set<int> pids;
  for (const auto& entry :
       std::filesystem::directory_iterator(kProcDirectory)) {
    string filename = entry.path().filename().string();
    if (entry.is_directory() &&
        std::all_of(filename.begin(), filename.end(), isdigit)) {
      int pid = stoi(filename);
      pids.insert(pid);
    }
  }
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key, value;
  std::optional<float> total, free;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while ((!total.has_value() || !free.has_value()) &&
           std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          total = stof(value);
          break;
        } else if (key == "MemFree:") {
          free = stof(value);
          break;
        } else {
          break;
        }
      }
    }
  }
  // avoid 0 error
  if (!total.has_value() || !free.has_value() || total.value() == 0.0f) {
    return 0.0f;
  }
  return (total.value() - free.value()) / total.value();
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line, uptime = "0", idle;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle;
  }
  return stol(uptime);
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::CalculateJiffies(
      {CPUStates::kUser_, CPUStates::kNice_, CPUStates::kSystem_,
       CPUStates::kIdle_, CPUStates::kIOwait_, CPUStates::kIRQ_,
       CPUStates::kSoftIRQ_, CPUStates::kSteal_});
}
long LinuxParser::CalculateJiffies(std::initializer_list<int> keys) {
  long result = 0;
  const std::vector<string> cpuValues = CpuUtilization();
  if (cpuValues.empty()) {
    result = 0;
  } else {
    for (const auto key : keys) {
      result += stol(cpuValues.at(key));
    }
  }
  return result;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  vector<string> values = getProcessStat(pid);
  if(values.empty()){
    return 0;
  }
  return stol(values.at(13)) + stol(values.at(14))+ stol(values.at(15))+ stol(values.at(16));
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::CalculateJiffies(
      {CPUStates::kUser_, CPUStates::kNice_, CPUStates::kSystem_,
       CPUStates::kIRQ_, CPUStates::kSoftIRQ_, CPUStates::kSteal_});
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  return LinuxParser::CalculateJiffies(
      {CPUStates::kIdle_, CPUStates::kIOwait_});
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> values;
  string line, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    // Skip the first word (cpu)
    linestream >> value;
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  return values;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { return ReadStatInt("processes"); }

/**
 * Read the system stat of the given item.
 * @param item
 * @return
 */
int LinuxParser::ReadStatInt(const string& item) {
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == item) {
          linestream >> value;
          return stoi(value);
        } else {
          // next line
          break;
        }
      }
    }
  }
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { return ReadStatInt("procs_running"); }

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  const string vmSize = readProcessStatus(pid, "VmSize:");

  // Some sleeping process has no vmSize column.
  if (vmSize.empty()) {
    return "0";
  } else {
    // Return in MB;
    return to_string(stol(vmSize) / 1024);
  }
}

/**
 * Read the process status of the given item
 * @param pid
 * @param item
 * @return
 */
string LinuxParser::readProcessStatus(int pid, const string& item) {
  string line, key, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == item) {
          linestream >> value;
          return value;
        } else {
          // next line
          break;
        }
      }
    }
  }
  return value;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { return readProcessStatus(pid, "Uid:"); }

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, username, x, uid;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> username >> x >> uid) {
        if (uid == LinuxParser::Uid(pid)) {
          return username;
        } else {
          // next line
          break;
        }
      }
    }
  }
  return username;
}

/**
 * Read and return the CPU time of a process
 * The online tutorial lists an example related [starttime] (see [Project: System Monitor/Process Data/Up Time])
 * However, this program uses this method to display the [TIME+] similar to Htop, not [starttime]. It is confusing...
 * Thus, We follow the intent of this program and return CPU time here instead of [starttime], supporting the correct display of [TIME+]
 * @param pid
 * @return
 */
long LinuxParser::UpTime(int pid) {
  vector<string> values = getProcessStat(pid);
  if (values.empty()) {
    return 0;
  }
  return (stol(values.at(13)) + stol(values.at(14))) / sysconf(_SC_CLK_TCK);
}

/**
 * Read and return the [starttime].
 * To calcualte the CpuUtilization, we need another method to return the real [starttime]
 * @param pid
 * @return
 */
long LinuxParser::StartTime(int pid) {
  vector<string> values = getProcessStat(pid);
  if (values.empty()) {
    return 0;
  }
  return stol(values.at(21)) / sysconf(_SC_CLK_TCK);
}

/**
 * Read the stat of the process
 * @param pid
 * @return
 */
vector<string> LinuxParser::getProcessStat(int pid) {
  string line, value;
  std::vector<string> values;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    };
  }
  return values;
}
