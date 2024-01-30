#include "tcp_server.h"
#include <spdlog/spdlog.h>
#include <cstring>

namespace util {

TcpServer::TcpServer(EventLoop& loop,
                     std::string_view host,
                     unsigned short port,
                     std::function<void(TcpStream&&)> handler)
    : loop_{loop}, handler_{handler} {
  server_socket_ = TcpSocket::Create();
  server_socket_.Bind(host, port);
  server_socket_.Listen(100);

  loop_.Register(server_socket_, [this] { this->OnAccept(); });
}

void TcpServer::OnAccept() {
  TcpSocket new_conn = server_socket_.Accept();

  if (!new_conn.Valid()) {
    return;
  }

  loop_.Register(new_conn, [this, conn = new_conn] { this->OnRequest(conn); });
}

void TcpServer::OnRequest(TcpSocket conn) {
  const int byte_recv = conn.Read(buffer_);

  // EOF or Error
  if (byte_recv <= 0) {
    spdlog::debug("TcpServer error: {}",
                  byte_recv == 0 ? "EOF" : std::strerror(errno));

    Disconnect(conn);
    return;
  }

  std::span<std::byte> recv_msg(buffer_.data(), byte_recv);
  handler_(TcpStream(recv_msg, &loop_, conn));
}

void TcpServer::Disconnect(TcpSocket conn) {
  loop_.Remove(conn);
  conn.Close();
}

}  // namespace util
