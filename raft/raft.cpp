#include "raft.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include "candidate.h"
#include "core.h"
#include "follower.h"
#include "leader.h"
#include "message_decoder.h"

Raft::Raft(Addr local_addr, std::vector<Addr> peers)
    : raft_context_(local_addr, peers),
      vote_manager_{std::make_shared<VoteManager>(raft_context_, log_entry_)} {
  SwitchToFollower();
  vote_manager_->Start();
}

void Raft::OnRequest(util::TcpStream&& stream) try {
  const Message decoded_msg = MessageDecoder::Decode(stream.Text());
  const std::string response = state_->OnRequest(decoded_msg);
  stream.Send(response);

} catch (const std::exception& ex) {
  spdlog::error("Raft OnRequest error: {}", ex.what());
  nlohmann::json response;
  response["result"] = "invalid";
  stream.Send(response.dump());
  stream.Close();
}

void Raft::SwitchToFollower() {
  vote_manager_->ResetVote();
  spdlog::debug("switch to follower");
  raft_context_.ResetLeader();
  state_ = std::make_shared<Follower>(raft_context_, *this, log_entry_,
                                      vote_manager_);
  state_->Start();
}

void Raft::SwitchToLeader(const Addr& addr) {
  if (!raft_context_.IsLeaderAvailable()) {
    raft_context_.CurrentLeader(addr);
    spdlog::debug("switch to leader");
    state_ = std::make_shared<Leader>(raft_context_, log_entry_, vote_manager_);
    state_->Start();
  }
}

void Raft::SwitchToCandidate() {
  vote_manager_->ResetVote();
  spdlog::debug("switch to candidate");
  state_ = std::make_shared<Candidate>(raft_context_, *this, log_entry_,
                                       vote_manager_);
  state_->Start();
}
