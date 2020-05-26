#include "timer.hpp"

namespace txn {

void Timer::Record(const std::string& name) {
    timings[name] = Clock::now();
}

long int Timer::GetMs(const std::string& before,
                      const std::string& after) const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               timings.at(after) - timings.at(before))
        .count();
}
}  // namespace txn
