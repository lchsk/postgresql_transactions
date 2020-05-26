#include <chrono>
#include <string>
#include <unordered_map>

#pragma once

namespace txn {
using Clock = std::chrono::system_clock;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

struct Timer {
    void Record(const std::string& name);
    long int GetMs(const std::string&, const std::string&) const;

   private:
    std::unordered_map<std::string, TimePoint> timings;
};
}  // namespace txn
