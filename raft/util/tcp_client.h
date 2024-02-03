#pragma once

#include <boost/noncopyable.hpp>
#include <memory>
#include <string_view>
#include "event_loop.h"
#include "tcp_socket.h"
#include "tcp_stream.h"

namespace util {

class TcpClient : public std::enable_shared_from_this<TcpClient>,
                  private boost::noncopyable {
 public:
  static std::shared_ptr<TcpClient> Create(
      EventLoop* loop,
      std::string_view host,
      unsigned short port,
      std::function<void(TcpStream)> handler) {
    auto client = std::make_shared<TcpClient>(loop, host, port, handler);
    client->Init();
    return client;
  }

  template <typename Alloc>
  static std::shared_ptr<TcpClient> Create(
      EventLoop* loop,
      std::string_view host,
      unsigned short port,
      std::function<void(TcpStream)> handler,
      const Alloc& allocator) {
    auto client =
        std::allocate_shared<TcpClient>(allocator, loop, host, port, handler);
    client->Init();
    return client;
  }

  TcpClient(EventLoop* loop,
            std::string_view host,
            unsigned short port,
            std::function<void(TcpStream)> handler);

  void Init();
  bool Send(std::string_view msg);
  void Close();

  void OnRequest(TcpSocket conn);

 private:
  EventLoop* loop_;
  std::function<void(TcpStream)> handler_;
  TcpSocket conn_;
  std::array<std::byte, TcpStream::kMaxLength> buffer_{};
};

}  // namespace util
