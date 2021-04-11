#include "Timer.h"

void Timer::start() {
  start_point = clock::now();
}

void Timer::stop() {
  const auto end_point = clock::now();
  const auto time_diff = end_point - start_point;

  measuredTime += time_diff;
  accumulatedTime += time_diff;
}

void Timer::reset() {
  measuredTime = clock::duration(0);
  resetCounter += 1;

  // Handle overflow of reset counter
  if (resetCounter == std::numeric_limits<
      decltype(resetCounter)
    >::max()) { // Has the maximum value for the counter been reached?
    resetCounter = 0;
    accumulatedTime = clock::duration(0);
  }
}

void Timer::resetAverage() {
  accumulatedTime = clock::duration(0);
  resetCounter = 0;
}

Timer::clock::duration Timer::getMeasuredTime() const {
  return measuredTime;
}

Timer::clock::duration Timer::getAverageMeasuredTime() const {
  return accumulatedTime / resetCounter;
}
