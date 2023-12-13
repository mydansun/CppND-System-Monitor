#include "linux_parser.h"
#include "ncurses_display.h"
#include "system.h"

int main() {
  // Pointer to a Parser object
  Parser* parser;

#ifdef __linux__
  // Linux-specific Parser implementation
  LinuxParser linuxParser;
  parser = &linuxParser;
#endif

  if (parser == nullptr) {
    // Throw an exception if parser is not initialized
    throw std::runtime_error("Unsupported system.");
  }
  System system(parser);
  NCursesDisplay::Display(system);
}