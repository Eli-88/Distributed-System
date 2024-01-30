#pragma once

#include <boost/noncopyable.hpp>
#include <chrono>
#include <functional>
#include <queue>
#include <span>
#include <unordered_map>
#include <vector>
#include "selector.h"

namespace util {

class EventLoop : private boost::noncopyable {
 private:
  struct TimeHandle {
    std::chrono::steady_clock::time_point time_point;
    std::function<void()> handle;

    bool operator>(const TimeHandle& rhs) const {
      return time_point > rhs.time_point;
    }
  };

 public:
  void Register(int fd, const std::function<void()>& handle);
  void Remove(int fd);
  void CallAfter(int delay_ms, const std::function<void()>& handle);

  void PollOnce();

 private:
  Selector selector_;
  std::unordered_map<int, std::function<void()>> fd_to_handle_mappings_;
  std::vector<TimeHandle> time_handles_;
  std::queue<std::function<void()>> ready_;

  std::array<int, Selector::kMaxPollEvents> poll_events_{};
  int next_timeout_ms = Selector::kBlockIndefinitely;
};

}  // namespace util
