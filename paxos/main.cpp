#include <spdlog/spdlog.h>
#include <memory>
#include "acceptor.h"
#include "config.h"
#include "handlers/accept_prepare_handler.h"
#include "handlers/accept_propose_handler.h"
#include "handlers/learn_accept_handler.h"
#include "handlers/learn_retrieve_handler.h"
#include "handlers/propose_request_handler.h"
#include "http_server.h"
#include "learner.h"
#include "net.h"
#include "proposer.h"

int main(int argc, char** argv) {
  spdlog::set_level(spdlog::level::info);
  spdlog::info("paxos");

  if (argc < 2) {
    spdlog::error(
        "not enough argument, file path for config needed: <usage> ./paxos "
        "<config file path>");

    exit(-1);
  }

  const char* config_path = argv[1];

  Config config(config_path);

  spdlog::debug("local address: {}:{}", config.LocalAddress().host,
                config.LocalAddress().port);

  for (const auto& peer : config.PeerAddresses()) {
    spdlog::debug("peer address: {}:{}", peer.host, peer.port);
  }

  io_context ctx;
  HttpServer server;

  auto acceptor = std::make_shared<Acceptor>();
  auto learner = std::make_shared<Learner>();
  auto proposer = std::make_shared<Proposer>();

  tcp::resolver resolver(ctx);
  tcp::endpoint local_endpoint =
      *resolver.resolve(config.LocalAddress().host, config.LocalAddress().port);

  std::vector<tcp::endpoint> peers;
  for (const auto& peer : config.PeerAddresses()) {
    tcp::resolver resolver(ctx);
    auto endpoints = resolver.resolve(peer.host, peer.port);
    peers.emplace_back(*endpoints);
  }

  server.RegisterCallback("/proposer/request",
                          ProposeRequestHandler(proposer, learner, peers));
  server.RegisterCallback("/acceptor/prepare", AcceptPrepareHandler(acceptor));
  server.RegisterCallback("/acceptor/propose", AcceptProposeHandler(acceptor));
  server.RegisterCallback("/learner/accept", LearnAcceptHandler(learner));
  server.RegisterCallback("/learner/retrieve", LearnRetrieveHandler(learner));

  co_spawn(ctx, server.Run(local_endpoint), detached);

  ctx.run();
}
