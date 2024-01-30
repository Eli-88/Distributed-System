#pragma once

#include <boost/noncopyable.hpp>
#include <cstdint>
#include <optional>
#include "addr.h"
#include "log_entry.h"
#include "message.h"
#include "raft_context.h"

class VoteManager : private boost::noncopyable {
 public:
  VoteManager(RaftContext& node, LogEntry& log_entry);
  void Start();
  bool ValidateVote(Addr addr, uint64_t term, uint64_t index);
  uint64_t Term() const;
  void ResetVote();

 private:
  RaftContext& raft_context_;
  LogEntry& log_entry_;
  std::optional<Addr> approved_vote_addr_;
  uint64_t current_term_{0};
};
