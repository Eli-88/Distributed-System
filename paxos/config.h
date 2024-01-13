#pragma once

#include <string>
#include <string_view>
#include <vector>

class Config {
 public:
  struct Addr {
    std::string host;
    std::string port;
  };

 public:
  explicit Config(std::string_view file_path);
  Addr LocalAddress() const;
  const std::vector<Addr>& PeerAddresses() const;

 private:
  Addr local_address_;
  std::vector<Addr> peer_addresses_;

  Addr StringToAddr(std::string_view address) const;
};
