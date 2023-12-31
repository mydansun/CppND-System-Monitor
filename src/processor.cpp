#include "processor.h"

#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  if (prevActive == 0 && prevIdle == 0 && prevTotal == 0) {
    prevActive = _parser->ActiveJiffies();
    prevIdle = _parser->IdleJiffies();
    prevTotal = prevActive + prevIdle;
    return 0.0f;
  }

  const long active = _parser->ActiveJiffies();
  const long idle = _parser->IdleJiffies();
  const long total = idle + active;

  const long delta_total = total - prevTotal;
  const long delta_idle = idle - prevIdle;

  prevActive = active;
  prevIdle = idle;
  prevTotal = total;

  if (delta_total == 0) {
    return 0.0f;
  }

  return static_cast<float>(
      static_cast<long double>((delta_total - delta_idle)) / delta_total);
}