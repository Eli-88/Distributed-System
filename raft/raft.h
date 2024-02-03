#pragma once

#include <boost/noncopyable.hpp>
#include <memory>
#include "addr.h"
#include "in_memory_log_entry.h"
#include "raft_context.h"
#include "state.h"
#include "state_mediator.h"
#include "util/tcp_response.h"
#include "util/tcp_stream.h"
#include "vote_manager.h"

class Raft : private boost::noncopyable, public StateMediator {
 public:
  Raft(Addr local_addr, std::vector<Addr> peers);

  void OnRequest(util::TcpStream stream);

  void SwitchToFollower() override;
  void SwitchToLeader(const Addr& addr) override;
  void SwitchToCandidate() override;

 private:
  RaftContext raft_context_;
  std::shared_ptr<State> state_;
  InMemoryLogEntry log_entry_;
  std::shared_ptr<VoteManager> vote_manager_;
};
