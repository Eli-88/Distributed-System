#include "selector.h"
#include <fmt/format.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>

namespace util {

Selector::Selector() : poll_fd_(::epoll_create1(0)) {
  if (-1 == poll_fd_) {
    throw SelectorInitError();
  }
}

Selector::~Selector() {
  ::close(poll_fd_);
}

void Selector::AddReader(int fd) const {
  assert(fd != -1);

  struct epoll_event evt;
  evt.events = EPOLLIN;
  evt.data.fd = fd;

  if (-1 == epoll_ctl(poll_fd_, EPOLL_CTL_ADD, fd, &evt)) {
    throw SelectorAddReaderError();
  }
}

void Selector::Remove(int fd) const {
  assert(fd != -1);
  if (-1 == epoll_ctl(poll_fd_, EPOLL_CTL_DEL, fd, nullptr)) {
    throw SelectorRemoveReaderError();
  }
}

int Selector::Poll(std::span<int> output_events, int timeout) const {
  struct epoll_event events[kMaxPollEvents]{};

  const int num_events = epoll_wait(
      poll_fd_, events,
      std::min(kMaxPollEvents, static_cast<int>(output_events.size())),
      timeout);

  if (num_events < 0) {
    throw SelectorPollError();
  }

  std::transform(events, events + num_events, std::begin(output_events),
                 [](const auto& evt) { return evt.data.fd; });

  return num_events;
}

}  // namespace util
