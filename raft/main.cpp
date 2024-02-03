#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include "core.h"
#include "raft.h"
#include "raft_context.h"
#include "util/tcp_server.h"

int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::debug);
  spdlog::debug("starting raft application");

  if (argc < 2) {
    throw std::runtime_error("<usage> ./raft <config file path>");
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    throw std::runtime_error(fmt::format("invalid file: {}", argv[1]));
  }

  nlohmann::json json;
  file >> json;

  const Addr local_addr{.host = "localhost",
                        .port = json["port"].get<unsigned short>()};

  unsigned short local_port = json["port"].get<unsigned short>();
  std::vector<Addr> peers;

  assert(json["peer_ports"].is_array());
  for (const auto& peer_port : json["peer_ports"]) {
    peers.emplace_back(
        Addr{.host = "localhost", .port = peer_port.get<unsigned short>()});
  }

  spdlog::debug("port: {}", local_port);
  for (const auto& peer : peers) {
    spdlog::debug("remote peer: {}:{}", peer.host, peer.port);
  }

  util::EventLoop& loop = GetLoop();

  Raft raft(local_addr, peers);

  util::TcpServer server(GetLoop(), local_addr.host, local_addr.port,
                         [&raft](util::TcpStream stream) {
                           return raft.OnRequest(std::move(stream));
                         });

  while (true) {
    loop.PollOnce();
  }

  return 0;
}
