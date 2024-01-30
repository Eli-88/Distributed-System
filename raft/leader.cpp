#include "leader.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "core.h"
#include "message_encoder.h"

Leader::Leader(RaftContext& raft_context,
               LogEntry& log_entry,
               std::shared_ptr<VoteManager> vote_manager)
    : raft_context_{raft_context},
      log_entry_{log_entry},
      vote_manager_{vote_manager} {}

void Leader::Start() {
  const std::string response = Encode(
      AnnounceNewLeader{.addr = Addr{.host = raft_context_.LocalAddr().host,
                                     .port = raft_context_.LocalAddr().port},
                        .term = vote_manager_->Term(),
                        .index = log_entry_.LatestIndex()});

  for (const auto& peer : raft_context_.PeerView()) {
    spdlog::debug("announce new leader to {}:{}", peer.host, peer.port);
    BroadcastToPeer(peer, response);
  }

  PeriodicHealthCheck();
}

std::string Leader::OnRequest(const Message& msg) {
  return std::visit(*this, msg);
}

std::string Leader::operator()(const RequestVoteMsg&) {
  nlohmann::json response;
  response["result"] = "Leader error: leader do no have to vote";
  return response.dump();
}

std::string Leader::operator()(const VoteResultMsg&) {
  nlohmann::json response;
  response["result"] = "Leader error: leader do no have to vote";
  return response.dump();
}

std::string Leader::operator()(const LeaderAliveMsg&) {
  nlohmann::json response;
  response["result"] = "Leader error: currently a leader and alive";
  return response.dump();
}

std::string Leader::operator()(const AnnounceNewLeader&) {
  nlohmann::json response;
  response["result"] = "Leader error: currently a leader and alive";
  return response.dump();
}

std::string Leader::operator()(const AppendLogsMsg&) {
  nlohmann::json response;
  response["result"] =
      "Leader error: currently a leader, handling the logs internally";
  return response.dump();
}

std::string Leader::operator()(const ClientRequest& msg) {
  const uint64_t next_index = log_entry_.LatestIndex() + 1;
  const LogEntryItem entry{
      .index = next_index, .term = vote_manager_->Term(), .data = msg.command};

  log_entry_.Append(entry);

  const std::string response = Encode(AppendLogsMsg{
      .leader_addr = Addr{.host = raft_context_.LocalAddr().host,
                          .port = raft_context_.LocalAddr().port},
      .log_entry_item = LogEntryItem{.index = log_entry_.LatestIndex(),
                                     .term = vote_manager_->Term(),
                                     .data = msg.command}});

  for (const auto& peer : raft_context_.PeerView()) {
    BroadcastToPeer(peer, response);
  }

  return response;
}

std::string Leader::operator()(const InvalidMsg&) {
  nlohmann::json response;
  response["result"] = "Leader error: invalid msg";
  return response.dump();
}

void Leader::BroadcastToPeer(const Addr& addr, std::string_view msg) try {
  SendToClient(addr, msg, [](util::TcpStream&& stream) { stream.Close(); });
} catch (const std::exception& ex) {
  spdlog::error("leader unable to broadcast to {}:{}", addr.host, addr.port);
}

void Leader::PeriodicHealthCheck() {
  CallAfter(2000, [self = shared_from_this(), this] {
    spdlog::debug("leader pings alive");

    const std::string ping = Encode(
        LeaderAliveMsg{.addr = Addr{.host = raft_context_.LocalAddr().host,
                                    .port = raft_context_.LocalAddr().port}});

    for (const auto& peer : raft_context_.PeerView()) {
      BroadcastToPeer(peer, ping);
    }

    PeriodicHealthCheck();
  });
}
