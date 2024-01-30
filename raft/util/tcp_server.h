#pragma once

#include <array>
#include <boost/noncopyable.hpp>
#include <functional>
#include <string>
#include <string_view>
#include "bytes.h"
#include "event_loop.h"
#include "selector.h"
#include "tcp_response.h"
#include "tcp_socket.h"
#include "tcp_stream.h"

namespace util {

class TcpServer : private boost::noncopyable {
 public:
  TcpServer(EventLoop& loop,
            std::string_view host,
            unsigned short port,
            std::function<void(TcpStream&&)> handler);

 private:
  EventLoop& loop_;
  std::array<std::byte, TcpStream::kMaxLength> buffer_{};

  TcpSocket server_socket_;
  std::function<void(TcpStream&&)> handler_;

  void OnAccept();
  void OnRequest(TcpSocket conn);

  void Disconnect(TcpSocket conn);
};

}  // namespace util
