#pragma once

#include <boost/noncopyable.hpp>
#include <memory>
#include "addr.h"
#include "log_entry.h"
#include "message.h"
#include "raft_context.h"
#include "state.h"
#include "state_mediator.h"
#include "vote_manager.h"

class Follower : public std::enable_shared_from_this<Follower>,
                 private boost::noncopyable,
                 public State {
 public:
  Follower(RaftContext& raft_context,
           StateMediator& state_mediator,
           LogEntry& log_entry,
           std::shared_ptr<VoteManager> vote_manager);

  std::string operator()(const RequestVoteMsg& msg);
  std::string operator()(const VoteResultMsg& msg);
  std::string operator()(const LeaderAliveMsg& msg);
  std::string operator()(const AnnounceNewLeader& msg);
  std::string operator()(const AppendLogsMsg& msg);
  std::string operator()(const ClientRequest& msg);
  std::string operator()(const InvalidMsg& msg);

  void Start() override;
  std::string OnRequest(const Message& msg) override;

 private:
  RaftContext& raft_context_;
  StateMediator& state_mediator_;
  LogEntry& log_entry_;
  std::shared_ptr<VoteManager> vote_manager_;
  bool timer_in_progress_{true};
};
