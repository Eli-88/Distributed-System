#pragma once

#include <fmt/format.h>
#include <array>
#include <cstring>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include "concepts.h"
#include "util/bytes.h"

namespace util {

class TcpSocket {
 public:
  static TcpSocket Create();

  TcpSocket() = default;
  TcpSocket(const TcpSocket&) = default;

  // implicit conversion is allow to convert int fd into object
  TcpSocket(int fd);

  TcpSocket Accept() const;
  void Bind(const std::string_view host, unsigned short port) const;
  void Listen(int connection_count = 100) const;
  void Connect(const std::string_view host, unsigned short port) const;
  std::tuple<std::array<char, 16>, unsigned short> GetRemoteAddr() const;
  void Close() const;

  int Read(std::span<std::byte> output_buffer) const;

  template <ContainerConcept Container>
  int Read(Container& output_buffer) const {
    return Read(util::ConvertToBytes(output_buffer));
  }

  template <typename T>
  int Read(T* output_buffer, std::size_t length) const {
    return Read(util::ConvertToBytes(output_buffer, length));
  }

  int Write(std::span<const std::byte> input_buffer) const;

  template <ContainerConcept Container>
  int Write(const Container& input_buffer) const {
    return Write(util::ConvertToBytesConst(input_buffer));
  }

  template <typename T>
  int Write(const T* input_buffer, std::size_t length) const {
    return Write(util::ConvertToBytesConst(input_buffer, length));
  }

  bool Valid() const;

  // helps to implicitly convert to int for file descriptor
  operator int() const;

  bool operator<(const TcpSocket&) const;
  bool operator==(const TcpSocket&) const;

 private:
  int fd_{-1};
};

// Exceptions

class TcpAcceptError : public std::exception {
 public:
  const char* what() const noexcept { return msg_.c_str(); }

 private:
  std::string msg_{fmt::format("TcpAcceptError: {}", strerror(errno))};
};
class TcpBindError : public std::exception {
 public:
  const char* what() const noexcept { return msg_.c_str(); }

 private:
  std::string msg_{fmt::format("TcpBindError: {}", strerror(errno))};
};

class TcpConnectError : public std::exception {
 public:
  const char* what() const noexcept { return msg_.c_str(); }

 private:
  std::string msg_{fmt::format("TcpConnectError: {}", strerror(errno))};
};

class TcpListenError : public std::exception {
 public:
  const char* what() const noexcept { return msg_.c_str(); }

 private:
  std::string msg_{};
};

class TcpCreateError : public std::exception {
 public:
  const char* what() const noexcept { return msg_.c_str(); }

 private:
  std::string msg_{fmt::format("TcpCreateError: {}", strerror(errno))};
};

class TcpInvalidHost : public std::exception {
 public:
  const char* what() const noexcept { return msg_.c_str(); }

 private:
  std::string msg_{fmt::format("TcpInvalidHost: {}", strerror(errno))};
};

}  // namespace util
