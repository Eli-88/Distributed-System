#include "raft_context.h"

RaftContext::RaftContext(Addr local_addr, std::vector<Addr> peers)
    : local_addr_{local_addr}, peers_(std::move(peers)) {}

std::optional<Addr> RaftContext::CurrentLeader() const {
  return current_leader_;
}

Addr RaftContext::LocalAddr() const {
  return local_addr_;
}

bool RaftContext::IsLeader() const {
  if (!current_leader_) {
    return false;
  }

  return local_addr_ == current_leader_.value();
}

bool RaftContext::IsLeaderAvailable() const {
  return current_leader_.has_value();
}

std::span<const Addr> RaftContext::PeerView() const {
  return peers_;
}

void RaftContext::CurrentLeader(Addr leader) {
  current_leader_ = leader;
}

void RaftContext::ResetLeader() {
  current_leader_ = std::nullopt;
}
