#include "core.h"
#include <spdlog/spdlog.h>
#include <array>
#include "util/tcp_stream.h"

// TODO: may consider shifting this to util for common use
// applicable for single threaded callback invoke
class CoreTcpClient {
 public:
  CoreTcpClient(util::EventLoop& loop) : loop_{loop} {}

  void Register(util::TcpSocket conn,
                const std::function<void(util::TcpStream&&)> cb) {
    loop_.Register(conn, [conn = std::move(conn), &loop = loop_,
                          cb = std::move(cb)] {
      std::array<std::byte, util::TcpStream::kMaxLength> recv_buffer{};
      const int byte_recv = conn.Read(recv_buffer);

      // EOF or Error
      if (byte_recv <= 0) {
        loop.Remove(conn);
        conn.Close();
        return;
      }

      cb(std::move(util::TcpStream(
          std::span<std::byte>(recv_buffer.data(), byte_recv), &loop, conn)));
    });
  }

 private:
  util::EventLoop& loop_;
};

static CoreTcpClient tcp_client_service(GetLoop());

util::EventLoop& GetLoop() {
  static util::EventLoop loop;
  return loop;
}

void SendToClient(Addr addr,
                  std::string_view msg,
                  std::function<void(util::TcpStream&&)> callback) {
  util::TcpSocket conn = util::TcpSocket::Create();
  conn.Connect(addr.host, addr.port);

  tcp_client_service.Register(conn, callback);
  conn.Write(msg);
}

void CallAfter(int delay_ms, const std::function<void()>& handle) {
  GetLoop().CallAfter(delay_ms, handle);
}