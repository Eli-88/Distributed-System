#include "config.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <ranges>

Config::Config(std::string_view file_path) {
  spdlog::debug("opening file: {}", file_path);

  std::ifstream file(file_path.data());
  if (!file.is_open()) {
    throw std::runtime_error(fmt::format("fail to read file: {}", file_path));
  }

  std::string lines{std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>()};

  spdlog::debug("parsing file content: \n{}", lines);

  auto json = nlohmann::json::parse(lines);
  const std::string local_addr_str = json["local_address"].get<std::string>();
  local_address_ = StringToAddr(local_addr_str);

  if (!json["peer_addresses"].is_array()) {
    throw std::runtime_error(
        "peer_addresses is in wrong format, it shld be [ip1, ip2....]");
  }

  for (const auto& peer : json["peer_addresses"]) {
    peer_addresses_.emplace_back(StringToAddr(peer.get<std::string>()));
  }
}

Config::Addr Config::LocalAddress() const {
  return local_address_;
}

const std::vector<Config::Addr>& Config::PeerAddresses() const {
  return peer_addresses_;
}

Config::Addr Config::StringToAddr(std::string_view address) const {
  std::vector<std::string> split_result;
  boost::split(split_result, address, boost::is_any_of(":"));
  if (split_result.size() != 2) {
    throw std::runtime_error(
        fmt::format("wrong format for address: {}", address));
  }

  return Addr{.host = split_result.at(0), .port = split_result.at(1)};
}
