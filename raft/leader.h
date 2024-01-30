#pragma once

#include <boost/noncopyable.hpp>
#include <memory>
#include <string_view>
#include "addr.h"
#include "log_entry.h"
#include "raft_context.h"
#include "state.h"
#include "vote_manager.h"

class Leader : public State,
               private boost::noncopyable,
               public std::enable_shared_from_this<Leader> {
 public:
  Leader(RaftContext& raft_context,
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
  LogEntry& log_entry_;
  std::shared_ptr<VoteManager> vote_manager_;

  void BroadcastToPeer(const Addr& addr, std::string_view peer);
  void PeriodicHealthCheck();
};
