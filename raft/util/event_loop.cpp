#include "event_loop.h"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace util {

void EventLoop::Register(int fd, const std::function<void()>& handle) {
  spdlog::debug("Event loop registering fd: {}", fd);
  selector_.AddReader(fd);
  fd_to_handle_mappings_[fd] = handle;
}

void EventLoop::Remove(int fd) {
  selector_.Remove(fd);
  fd_to_handle_mappings_.erase(fd);
}

void EventLoop::CallAfter(int delay_ms, const std::function<void()>& handle) {
  const TimeHandle time_handle = {
      .time_point = std::chrono::steady_clock::now() +
                    std::chrono::milliseconds(delay_ms),
      .handle = handle};

  time_handles_.emplace_back(time_handle);
  std::push_heap(std::begin(time_handles_), std::end(time_handles_),
                 std::greater<TimeHandle>());
}

void EventLoop::PollOnce() {
  if (!time_handles_.empty()) {
    next_timeout_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - time_handles_.back().time_point)
            .count();

    next_timeout_ms = std::max(Selector::kReturnImmediately, next_timeout_ms);
  } else {
    next_timeout_ms = Selector::kBlockIndefinitely;
  }

  const int poll_count = selector_.Poll(poll_events_, next_timeout_ms);
  for (int i = 0; i < poll_count; ++i) {
    const int active_fd = poll_events_[i];
    ready_.emplace(fd_to_handle_mappings_[active_fd]);
  }

  const auto current_time = std::chrono::steady_clock::now();

  while (!time_handles_.empty()) {
    const TimeHandle& time_handle = time_handles_.front();

    if (current_time < time_handle.time_point) {
      break;
    }

    ready_.emplace(time_handle.handle);

    std::pop_heap(std::begin(time_handles_), std::end(time_handles_),
                  std::greater<TimeHandle>());
    time_handles_.pop_back();
  }

  while (!ready_.empty()) {
    const std::function<void()>& handle = ready_.front();
    handle();
    ready_.pop();
  }
}

}  // namespace util
