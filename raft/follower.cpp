#include "follower.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "core.h"
#include "message_encoder.h"

Follower::Follower(RaftContext& raft_context,
                   StateMediator& state_mediator,
                   LogEntry& log_entry,
                   std::shared_ptr<VoteManager> vote_manager)
    : raft_context_{raft_context},
      state_mediator_{state_mediator},
      log_entry_{log_entry},
      vote_manager_{vote_manager} {}

void Follower::Start() {
  GetLoop().CallAfter(5000, [self = shared_from_this(), this] {
    if (!raft_context_.IsLeaderAvailable()) {
      timer_in_progress_ = false;
      state_mediator_.SwitchToCandidate();

    } else {
      spdlog::debug("reset leader for the next timeout check in follower");
      raft_context_.ResetLeader();  // reset for next timeout check
      Start();
    }
  });
}

std::string Follower::OnRequest(const Message& msg) {
  return std::visit(*this, msg);
}

std::string Follower::operator()(const RequestVoteMsg& msg) {
  if (raft_context_.IsLeaderAvailable()) {
    return Encode(
        InvalidMsg{.msg = "leader is available, unable to vote for: {}:{}"});
  }

  if (!timer_in_progress_) {
    return Encode(InvalidMsg{.msg = "follower session has ended"});
  }

  const bool approved =
      vote_manager_->ValidateVote(msg.addr, msg.term, msg.index);

  return Encode(VoteResultMsg{.addr{.host = raft_context_.LocalAddr().host,
                                    .port = raft_context_.LocalAddr().port},
                              .approved = approved});
}

std::string Follower::operator()(const VoteResultMsg&) {
  return Encode(
      InvalidMsg{.msg = "invalid msg: follower has no use for vote result"});
}

std::string Follower::operator()(const LeaderAliveMsg& msg) {
  raft_context_.CurrentLeader(msg.addr);

  nlohmann::json response;
  response["result"] = "ok";
  return response.dump();
}

std::string Follower::operator()(const AnnounceNewLeader& msg) {
  raft_context_.CurrentLeader(msg.addr);
  nlohmann::json response;
  response["result"] = "ok";
  return response.dump();
}

std::string Follower::operator()(const AppendLogsMsg& msg) {
  nlohmann::json response;

  if (raft_context_.IsLeader() &&
      raft_context_.CurrentLeader().value() == msg.leader_addr) {
    if (log_entry_.LatestIndex() == msg.log_entry_item.index + 1) {
      log_entry_.Append(msg.log_entry_item);
      response["result"] = "ok";
      return response.dump();
    }
  }

  return Encode(InvalidMsg{.msg = "invalid log entry"});
}

std::string Follower::operator()(const ClientRequest&) {
  return Encode(InvalidMsg{.msg = "currently not a leader"});
}

std::string Follower::operator()(const InvalidMsg& msg) {
  nlohmann::json response;
  response["result"] = fmt::format("invalid msg for follower {}", msg.msg);
  return response.dump();
}
