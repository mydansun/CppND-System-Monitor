#include "linux_parser.h"

#include <unistd.h>

#include <algorithm>
#include <experimental/filesystem>
#include <fstream>
#include <initializer_list>
#include <set>
#include <string>
#include <vector>

std::vector<std::string> LinuxParser::getProcessStat(int pid) {
  std::string line, value;
  std::vector<std::string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  return values;
}
int LinuxParser::ReadStatInt(const std::string& item) {
  std::string line, key, value;
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
long LinuxParser::CalculateJiffies(std::initializer_list<int> keys) {
  long result = 0;
  const std::vector<std::string> cpuValues = CpuUtilization();
  if (cpuValues.empty()) {
    result = 0;
  } else {
    for (const auto key : keys) {
      result += stol(cpuValues.at(key));
    }
  }
  return result;
}
std::string LinuxParser::readProcessStatus(int pid, const std::string& item) {
  std::string line, key, value;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
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
float LinuxParser::MemoryUtilization() {
  std::string line, key, value;
  float total, free;
  bool has_total = false, has_free = false;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while ((!has_total || !has_free) && std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          total = stof(value);
          has_total = true;
          break;
        } else if (key == "MemFree:") {
          free = stof(value);
          has_free = true;
          break;
        } else {
          break;
        }
      }
    }
  }
  // avoid the divide-by-zero error
  if (!has_total || !has_free || total == 0.0f) {
    return 0.0f;
  }
  return (total - free) / total;
}
long LinuxParser::UpTime() {
  std::string line, uptime = "0", idle;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle;
  }
  return stol(uptime);
}
std::set<int> LinuxParser::Pids() {
  std::set<int> pids;
  for (const auto& entry :
       std::experimental::filesystem::directory_iterator(kProcDirectory)) {
    std::string filename = entry.path().filename().string();
    if (std::experimental::filesystem::is_directory(entry.status()) &&
        std::all_of(filename.begin(), filename.end(), isdigit)) {
      int pid = stoi(filename);
      pids.insert(pid);
    }
  }
  return pids;
}
int LinuxParser::TotalProcesses() { return ReadStatInt("processes"); }
int LinuxParser::RunningProcesses() { return ReadStatInt("procs_running"); }
std::string LinuxParser::OperatingSystem() {
  std::string line, key, value;
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
std::string LinuxParser::Kernel() {
  std::string os, kernel, version, string, line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}
std::vector<std::string> LinuxParser::CpuUtilization() {
  std::vector<std::string> values;
  std::string line, value;
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
long LinuxParser::Jiffies() {
  return CalculateJiffies({CPUStates::kUser_, CPUStates::kNice_,
                           CPUStates::kSystem_, CPUStates::kIdle_,
                           CPUStates::kIOwait_, CPUStates::kIRQ_,
                           CPUStates::kSoftIRQ_, CPUStates::kSteal_});
}
long LinuxParser::ActiveJiffies(int pid) {
  std::vector<std::string> values = getProcessStat(pid);
  if (values.empty()) {
    return 0;
  }
  return stol(values.at(13)) + stol(values.at(14)) + stol(values.at(15)) +
         stol(values.at(16));
}
long LinuxParser::ActiveJiffies() {
  return CalculateJiffies({CPUStates::kUser_, CPUStates::kNice_,
                           CPUStates::kSystem_, CPUStates::kIRQ_,
                           CPUStates::kSoftIRQ_, CPUStates::kSteal_});
}
long LinuxParser::IdleJiffies() {
  return CalculateJiffies({CPUStates::kIdle_, CPUStates::kIOwait_});
}
std::string LinuxParser::Command(int pid) {
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}
std::string LinuxParser::Ram(int pid) {
  const std::string vmSize = readProcessStatus(pid, "VmSize:");

  // Some sleeping process has no vmSize column.
  if (vmSize.empty()) {
    return "0";
  } else {
    // Return in MB;
    return std::to_string(stol(vmSize) / 1024);
  }
}
std::string LinuxParser::Uid(int pid) { return readProcessStatus(pid, "Uid:"); }
std::string LinuxParser::User(int pid) {
  std::string line, username, x, uid;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> username >> x >> uid) {
        if (uid == Uid(pid)) {
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

long LinuxParser::UpTime(int pid) {
  const auto stat = LinuxParser::getProcessStat(pid);
  if (stat.empty()) {
    return 0;
  }
  return LinuxParser::UpTime() -
         (std::stol(stat.at(21)) / sysconf(_SC_CLK_TCK));
}
