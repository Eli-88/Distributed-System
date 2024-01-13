#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include "awaitable.h"
#include "http.h"
#include "learner.h"
#include "net.h"
#include "proposer.h"
#include "timer.h"

class ProposeRequestHandler {
 public:
  ProposeRequestHandler(std::shared_ptr<Proposer> proposer,
                        std::shared_ptr<Learner> learner,
                        const std::vector<tcp::endpoint>& peers);
  awaitable<std::string> operator()(Request request);

 private:
  std::shared_ptr<Proposer> proposer_;
  std::shared_ptr<Learner> learner_;
  std::vector<tcp::endpoint> peers_;

 private:
  awaitable<bool> InitiateRequest(uint64_t proposal_number,
                                  std::string_view value,
                                  uint64_t minium_vote_count);

  awaitable<bool> PrepareRequest(std::vector<tcp_stream>& conns,
                                 uint64_t proposal_number,
                                 std::size_t minium_vote_count);

  awaitable<bool> ProposeRequest(std::vector<tcp_stream>& conns,
                                 uint64_t proposal_number,
                                 std::size_t minium_vote_count);

  awaitable<uint64_t> AccumulateVotes(std::vector<tcp_stream>& conns,
                                      Request request);

  awaitable<void> LearnValue(std::vector<tcp_stream>& conns,
                             uint64_t proposal_number,
                             std::string_view value);

  awaitable<void> Timeout(duration seconds);
};
