#pragma once

#include <fmt/format.h>
#include <boost/noncopyable.hpp>
#include <cstring>
#include <exception>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>

namespace util {

class Selector : private boost::noncopyable {
 public:
  static constexpr int kBlockIndefinitely = -1;
  static constexpr int kReturnImmediately = 0;
  static constexpr int kMaxPollEvents = 32;

 public:
  Selector();
  ~Selector();

  void AddReader(int fd) const;
  void Remove(int fd) const;
  int Poll(std::span<int> output_events,
           int timeout = kBlockIndefinitely) const;

 private:
  int poll_fd_;
};

class SelectorAddReaderError : public std::exception {
 public:
  const char* what() const noexcept override { return msg_.c_str(); }

 private:
  const std::string msg_{
      fmt::format("SelectorAddReaderError: {}", std::strerror(errno)).c_str()};
};

class SelectorRemoveReaderError : std::exception {
 public:
  const char* what() const noexcept override { return msg_.c_str(); }

 private:
  const std::string msg_{
      fmt::format("SelectorRemoveReaderError: {}", std::strerror(errno))};
};

class SelectorPollError : public std::exception {
 public:
  const char* what() const noexcept override { return msg_.c_str(); }

 private:
  const std::string msg_{
      fmt::format("SelectorPollError: {}", std::strerror(errno))};
};

class SelectorInitError : public std::exception {
 public:
  const char* what() const noexcept override { return msg_.c_str(); }

 private:
  const std::string msg_{
      fmt::format("SelectorInitError: {}", std::strerror(errno))};
};

}  // namespace util
