#include "ncurses_display.h"

#include <curses.h>

#include <chrono>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "format.h"
#include "system.h"

using std::string;
using std::to_string;

// 50 bars uniformly displayed from 0 - 100 %
// 2% is one bar(|)
std::string NCursesDisplay::ProgressBar(float percent) {
  std::string result{"0%"};
  int size{50};
  float bars{percent * size};

  for (int i{0}; i < size; ++i) {
    result += i <= bars ? '|' : ' ';
  }

  string display{to_string(percent * 100).substr(0, 4)};
  if (percent < 0.1 || percent == 1.0)
    display = " " + to_string(percent * 100).substr(0, 3);
  return result + " " + display + "/100%";
}

void NCursesDisplay::DisplaySystem(System& system, WINDOW* window) {
  int row{0};
  mvwprintw(window, ++row, 2, "%s",
            ("OS: " + system.OperatingSystem()).c_str());
  mvwprintw(window, ++row, 2, "%s", ("Kernel: " + system.Kernel()).c_str());
  mvwprintw(window, ++row, 2, "CPU: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "%s", "");
  wprintw(window, "%s", ProgressBar(system.Cpu().Utilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  mvwprintw(window, ++row, 2, "Memory: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "%s", "");
  wprintw(window, "%s", ProgressBar(system.MemoryUtilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  mvwprintw(window, ++row, 2, "%s",
            ("Total Processes: " + to_string(system.TotalProcesses())).c_str());
  mvwprintw(
      window, ++row, 2, "%s",
      ("Running Processes: " + to_string(system.RunningProcesses())).c_str());
  mvwprintw(window, ++row, 2, "%s",
            ("Up Time: " + Format::ElapsedTime(system.UpTime())).c_str());
  wrefresh(window);
}

void NCursesDisplay::DisplayProcesses(std::vector<Process>& processes,
                                      WINDOW* window, int n) {
  int row{0};
  int const pid_column{2};
  int const user_column{9};
  int const cpu_column{16};
  int const ram_column{26};
  int const time_column{35};
  int const command_column{46};
  wattron(window, COLOR_PAIR(2));
  mvwprintw(window, ++row, pid_column, "PID");
  mvwprintw(window, row, user_column, "USER");
  mvwprintw(window, row, cpu_column, "CPU[%%]");
  mvwprintw(window, row, ram_column, "RAM[MB]");
  mvwprintw(window, row, time_column, "TIME+");
  mvwprintw(window, row, command_column, "COMMAND");
  wattroff(window, COLOR_PAIR(2));
  for (int i = 0; i < n; ++i) {
    // You need to take care of the fact that the cpu utilization has already
    // been multiplied by 100.
    //  Clear the line
    mvwprintw(window, ++row, pid_column, "%s",
              (string(window->_maxx - 2, ' ').c_str()));

    Process process;
    try {
      process = processes.at(i);
    } catch (std::out_of_range& e) {
      continue;
    }
    mvwprintw(window, row, pid_column, "%s", to_string(process.Pid()).c_str());
    mvwprintw(window, row, user_column, "%s", process.User().c_str());
    float cpu = process.CpuUtilization() * 100;
    mvwprintw(window, row, cpu_column, "%s",
              to_string(cpu).substr(0, 4).c_str());
    mvwprintw(window, row, ram_column, "%s", process.Ram().c_str());
    mvwprintw(window, row, time_column, "%s",
              Format::ElapsedTime(process.UpTime()).c_str());
    mvwprintw(window, row, command_column, "%s",
              process.Command().substr(0, window->_maxx - 46).c_str());
  }
}

[[noreturn]] void NCursesDisplay::Display(System& system) {
  initscr();      // start ncurses
  noecho();       // do not print input values
  cbreak();       // terminate ncurses on ctrl + c
  start_color();  // enable color

  int x_max, y_max, process_height;
  getmaxyx(stdscr, y_max, x_max);

  WINDOW* system_window = newwin(9, x_max - 1, 0, 0);
  process_height = std::max(y_max - 9, 4);
  WINDOW* process_window =
      newwin(process_height, x_max - 1, system_window->_maxy + 1, 0);

  while (true) {
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    box(system_window, 0, 0);
    box(process_window, 0, 0);
    DisplaySystem(system, system_window);
    DisplayProcesses(system.Processes(), process_window, process_height - 3);
    wrefresh(system_window);
    wrefresh(process_window);
    refresh();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  endwin();
}
