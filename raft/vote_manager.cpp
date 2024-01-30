#include "vote_manager.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include "core.h"
#include "util/random.h"

VoteManager::VoteManager(RaftContext& raft_context, LogEntry& log_entry)
    : raft_context_{raft_context}, log_entry_{log_entry} {}

void VoteManager::Start() {}

bool VoteManager::ValidateVote(Addr addr, uint64_t term, uint64_t index) {
  if (raft_context_.IsLeaderAvailable() || approved_vote_addr_.has_value()) {
    spdlog::debug(
        "vote manager: rejecting votes from {}:{} , leader is {} and "
        "approved_vote_addr_ is {}",
        addr.host, addr.port,
        raft_context_.IsLeaderAvailable() ? "available" : "not available",
        approved_vote_addr_.has_value() ? "not reset" : "reset");
    current_term_ =
        std::max(term, current_term_);  // TODO: probably need to implement a
                                        // defer, or use any library that has it
    return false;
  }

  spdlog::debug("validating vote [{}:{}] at index {} term {}", addr.host,
                addr.port, index, term);

  if (current_term_ < term && log_entry_.LatestIndex() <= index) {
    spdlog::debug("vote approved [{}:{}] at index {} term {}", addr.host,
                  addr.port, index, term);
    approved_vote_addr_ = addr;
    current_term_ = std::max(term, current_term_);
    return true;
  }

  current_term_ = std::max(term, current_term_);
  return false;
}

uint64_t VoteManager::Term() const {
  return current_term_;
}

void VoteManager::ResetVote() {
  approved_vote_addr_ = std::nullopt;
}
