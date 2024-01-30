#pragma once

#include <boost/noncopyable.hpp>
#include <memory>
#include <unordered_map>
#include "addr.h"
#include "log_entry.h"
#include "message.h"
#include "raft_context.h"
#include "state.h"
#include "state_mediator.h"
#include "vote.h"
#include "vote_manager.h"

class Candidate : public std::enable_shared_from_this<Candidate>,
                  public State,
                  private boost::noncopyable {
 public:
  Candidate(RaftContext& raft_context,
            StateMediator& state_mediator,
            LogEntry& log_entry,
            std::shared_ptr<VoteManager> vote_manager);
  void Start() override;
  std::string OnRequest(const Message&) override;

  std::string operator()(const RequestVoteMsg& msg);
  std::string operator()(const VoteResultMsg& msg);
  std::string operator()(const LeaderAliveMsg& msg);
  std::string operator()(const AnnounceNewLeader& msg);
  std::string operator()(const AppendLogsMsg& msg);
  std::string operator()(const ClientRequest& msg);
  std::string operator()(const InvalidMsg& msg);

 private:
  RaftContext& raft_context_;
  StateMediator& state_mediator_;
  LogEntry& log_entry_;
  std::shared_ptr<VoteManager> vote_manager_;

  Votes votes_;
  uint64_t voting_term_{};
  std::size_t minimum_votes_;
  bool stop_election_{false};

 private:
  void UpdateVotes(Addr addr, bool approved);
  void StartElection();
  bool RunElection();
  void BroadcastToPeer(std::string_view request, Addr addr);
};
