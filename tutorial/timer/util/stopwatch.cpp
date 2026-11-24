#include "stopwatch.h"

void StopWatch::Start() {
  m_time = std::time(nullptr);
}

time_t StopWatch::Stop() {
  auto result = std::time(nullptr) - m_time;
  m_time = 0;

  return result;
}