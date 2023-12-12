#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // See src/processor.cpp

  // Declare any necessary private members
 private:
  long prevActive{0};
  long prevIdle{0};
  long prevTotal{0};
};

#endif