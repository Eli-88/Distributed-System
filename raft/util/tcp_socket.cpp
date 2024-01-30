#include "tcp_socket.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

namespace util {

TcpSocket TcpSocket::Create() {
  const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == fd) {
    throw TcpCreateError();
  }
  return TcpSocket(fd);
}

TcpSocket::TcpSocket(int fd) : fd_{fd} {
  constexpr int optval = 1;
  ::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

TcpSocket TcpSocket::Accept() const {
  sockaddr_in addr{};
  std::memset(&addr, 0, sizeof(addr));
  socklen_t addr_len = sizeof(addr);

  const int new_fd =
      ::accept(fd_, reinterpret_cast<sockaddr*>(&addr), &addr_len);

  if (new_fd == -1) {
    throw TcpAcceptError();
  }

  return TcpSocket(new_fd);
}

void TcpSocket::Bind(const std::string_view host, unsigned short port) const {
  sockaddr_in addr{};
  std::memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (-1 == ::inet_pton(AF_INET, host.data(), &addr.sin_addr.s_addr)) {
    throw TcpInvalidHost();
  }

  if (-1 == ::bind(fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))) {
    throw TcpBindError();
  }
}

void TcpSocket::Listen(int connection_count) const {
  if (-1 == ::listen(fd_, connection_count)) {
    throw TcpListenError();
  }
}

void TcpSocket::Connect(const std::string_view host,
                        unsigned short port) const {
  sockaddr_in addr{};
  std::memset(&addr, 0, sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  ::inet_pton(AF_INET, host.data(), &addr.sin_addr.s_addr);

  if (-1 ==
      ::connect(fd_, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr))) {
    throw TcpConnectError();
  }
}

std::tuple<std::array<char, 16>, unsigned short> TcpSocket::GetRemoteAddr()
    const {
  sockaddr_in addr{};
  std::memset(&addr, 0, sizeof(addr));
  socklen_t addr_len = sizeof(addr);
  ::getpeername(fd_, reinterpret_cast<sockaddr*>(&addr), &addr_len);

  std::array<char, 16> ip{};
  ::inet_ntop(AF_INET, &(addr.sin_addr), ip.data(), INET_ADDRSTRLEN);
  const unsigned short port = ::ntohs(addr.sin_port);

  return {ip, port};
}

void TcpSocket::Close() const {
  ::shutdown(fd_, SHUT_RDWR);
  ::close(fd_);
}

int TcpSocket::Read(std::span<std::byte> output_buffer) const {
  return ::recv(fd_, output_buffer.data(), output_buffer.size(), 0);
}

int TcpSocket::Write(std::span<const std::byte> input_buffer) const {
  return ::send(fd_, input_buffer.data(), input_buffer.size(), 0);
}

bool TcpSocket::Valid() const {
  return fd_ != -1;
}

// helps to implicitly convert to int for file descriptor
TcpSocket::operator int() const {
  return fd_;
}

bool TcpSocket::operator<(const TcpSocket& rhs) const {
  return fd_ < rhs.fd_;
}

bool TcpSocket::operator==(const TcpSocket& rhs) const {
  return fd_ == rhs.fd_;
}

}  // namespace util
