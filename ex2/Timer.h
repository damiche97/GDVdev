#ifndef TIMER_H
#define TIMER_H

#include <chrono>

/**
 * Helper class to make measuring time easier
 */
class Timer {
  public:
    using clock = std::chrono::system_clock;

    void start();
    void stop();
    void reset();
    void resetAverage();

    clock::duration getMeasuredTime() const;
    clock::duration getAverageMeasuredTime() const;

  private:
    clock::time_point start_point;

    clock::duration measuredTime {0};
    clock::duration accumulatedTime {0};
    uint64_t resetCounter {0};
};

#endif
