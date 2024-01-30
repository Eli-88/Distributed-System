#pragma once

#include <fmt/format.h>
#include <string>

struct Addr {
  std::string host;
  unsigned short port;

  bool operator==(const Addr& other) const {
    return (host == other.host) && (port == other.port);
  }
};

namespace std {
template <>
struct hash<Addr> {
  std::size_t operator()(const Addr& addr) const {
    return std::hash<std::string>{}(fmt::format("{}:{}", addr.host, addr.port));
  }
};

}  // namespace std
