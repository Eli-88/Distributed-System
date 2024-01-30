#include "tcp_stream.h"
#include <algorithm>

namespace util {

TcpStream::TcpStream(std::span<std::byte> buffer,
                     EventLoop* loop,
                     TcpSocket conn)
    : length_(buffer.size()), loop_{loop}, conn_{conn} {
  auto char_buffer = util::ConvertFromBytes<char>(buffer);
  std::copy_n(std::begin(char_buffer), char_buffer.size(), std::begin(buffer_));
}

std::string_view TcpStream::Text() const {
  return std::string_view(buffer_.data(), length_);
}
std::tuple<std::array<char, 16>, unsigned short> TcpStream::GetRemoteAddr()
    const {
  return conn_.GetRemoteAddr();
}

void TcpStream::Send(std::string_view msg) const {
  conn_.Write(msg);
}

void TcpStream::Close() {
  loop_->Remove(conn_);
  conn_.Close();
}

}  // namespace util
