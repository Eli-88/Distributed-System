#include "tcp_client.h"
#include <spdlog/spdlog.h>
#include <span>

namespace util {

TcpClient::TcpClient(EventLoop* loop,
                     std::string_view host,
                     unsigned short port,
                     std::function<void(TcpStream&&)> handler)
    : loop_{loop}, handler_{handler} {
  conn_ = TcpSocket::Create();
  conn_.Connect(host, port);
}

void TcpClient::Init() {
  loop_->Register(
      conn_, [self = shared_from_this()] { self->OnRequest(self->conn_); });
}

bool TcpClient::Send(std::string_view msg) {
  if (!conn_.Valid()) {
    return false;
  }

  conn_.Write(msg);
  return true;
}

void TcpClient::Close() {
  loop_->Remove(conn_);
  conn_.Close();
}

void TcpClient::OnRequest(TcpSocket conn) {
  spdlog::debug("TcpClient OnRequest");
  const int byte_recv = conn.Read(buffer_);

  // EOF or Error
  if (byte_recv <= 0) {
    loop_->Remove(conn);
    conn.Close();
    return;
  }

  std::span<std::byte> recv_msg(buffer_.data(), byte_recv);
  handler_(TcpStream(recv_msg, loop_, conn));
}

}  // namespace util
