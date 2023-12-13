#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "parser.h"
class Processor {
 public:
  float Utilization();
  explicit Processor(Parser *parser) : _parser(parser){};
  void SetParser(Parser *parser) { _parser = parser; }
  Processor() = default;

 private:
  Parser *_parser = nullptr;
  long prevActive{0};
  long prevIdle{0};
  long prevTotal{0};
};

#endif