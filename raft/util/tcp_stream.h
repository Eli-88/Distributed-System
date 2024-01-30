#pragma once

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include "bytes.h"
#include "event_loop.h"
#include "selector.h"
#include "tcp_socket.h"

namespace util {

class TcpStream {
 public:
  static constexpr int kMaxLength = 4096;

  TcpStream(std::span<std::byte> buffer, EventLoop* loop, TcpSocket conn);

  std::string_view Text() const;
  std::tuple<std::array<char, 16>, unsigned short> GetRemoteAddr() const;
  void Send(std::string_view msg) const;
  void Close();

 private:
  int length_{0};
  EventLoop* loop_;
  TcpSocket conn_;
  std::array<char, kMaxLength> buffer_;
};

}  // namespace util
