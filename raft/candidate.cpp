#include "candidate.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include "core.h"
#include "message_decoder.h"
#include "message_encoder.h"
#include "nlohmann/json.hpp"
#include "util/random.h"

Candidate::Candidate(RaftContext& raft_context,
                     StateMediator& state_mediator,
                     LogEntry& log_entry,
                     std::shared_ptr<VoteManager> vote_manager)
    : raft_context_{raft_context},
      state_mediator_{state_mediator},
      log_entry_{log_entry},
      vote_manager_{vote_manager},
      voting_term_{vote_manager->Term()},
      minimum_votes_{static_cast<std::size_t>(std::ceil(
          static_cast<double>(raft_context_.PeerView().size()) / 2))} {}

void Candidate::Start() {
  StartElection();
}

std::string Candidate::OnRequest(const Message& msg) {
  return std::visit(*this, msg);
}

std::string Candidate::operator()(const RequestVoteMsg& msg) {
  nlohmann::json response;
  if (raft_context_.IsLeaderAvailable()) {
    return Encode(InvalidMsg{.msg = "leader already exists, unable to vote"});
  }

  const bool approved =
      vote_manager_->ValidateVote(msg.addr, msg.term, msg.index);

  return Encode(
      VoteResultMsg{.addr = Addr{.host = raft_context_.LocalAddr().host,
                                 .port = raft_context_.LocalAddr().port},
                    .approved = approved});
}

std::string Candidate::operator()(const VoteResultMsg& msg) {
  nlohmann::json response;

  if (raft_context_.IsLeaderAvailable()) {
    return Encode(
        InvalidMsg{.msg = "leader already exist, unable to update votes"});
  }

  spdlog::debug("candidate vote result receive from {}:{} ", msg.addr.host,
                msg.addr.port);
  UpdateVotes(msg.addr, msg.approved);
  response["result"] = "ok";
  return response.dump();
}

std::string Candidate::operator()(const LeaderAliveMsg&) {
  spdlog::debug("candidate receive leader alive");
  stop_election_ = true;
  state_mediator_.SwitchToFollower();
  nlohmann::json response;
  response["result"] = "ok";
  return response.dump();
}

std::string Candidate::operator()(const AnnounceNewLeader&) {
  spdlog::debug("candidate receive new leader announcement");
  stop_election_ = true;
  state_mediator_.SwitchToFollower();
  nlohmann::json response;
  response["result"] = "ok";
  return response.dump();
}

std::string Candidate::operator()(const AppendLogsMsg& msg) {
  if (msg.leader_addr == raft_context_.CurrentLeader() &&
      msg.log_entry_item.index == log_entry_.LatestIndex() + 1) {
    log_entry_.Append(msg.log_entry_item);
  }

  stop_election_ = true;
  state_mediator_.SwitchToFollower();
  nlohmann::json response;
  response["result"] = "ok";
  return response.dump();
}

std::string Candidate::operator()(const ClientRequest&) {
  nlohmann::json response;
  response["result"] = "currently not a leader";
  return response.dump();
}

std::string Candidate::operator()(const InvalidMsg& msg) {
  nlohmann::json response;
  response["result"] = fmt::format("invalid msg from Candidate: {}", msg.msg);
  return response.dump();
}

void Candidate::UpdateVotes(Addr addr, bool approved) {
  if (!raft_context_.IsLeaderAvailable()) {
    const auto peers = raft_context_.PeerView();

    if (std::end(peers) !=
        std::find(std::begin(peers), std::end(peers), addr)) {
      spdlog::debug("updating votes from {}:{} -> {}", addr.host, addr.port,
                    approved ? "approved" : "denied");
      votes_.votes[addr] = approved;

      spdlog::debug("minimum votes needed {}", minimum_votes_);

      const std::size_t approved_count = votes_.ApprovedCount();
      spdlog::debug("number of approved votes: {}", approved_count);

      if (approved_count >= minimum_votes_) {
        state_mediator_.SwitchToLeader(raft_context_.LocalAddr());
        stop_election_ = true;
      }
    }
  }
}

void Candidate::StartElection() {
  CallAfter(GetRandom(5000, 20000), [self = shared_from_this(), this] {
    spdlog::debug("candidate election timeout");
    ++voting_term_;

    if (RunElection()) {
      StartElection();
    } else {
      state_mediator_.SwitchToFollower();
    }
  });
}

bool Candidate::RunElection() {
  if (stop_election_) {
    spdlog::debug("leader is available, stopping election");
    return false;
  }

  if (!raft_context_.IsLeaderAvailable()) {
    votes_ = Votes{};

    const std::string vote_request =
        Encode(RequestVoteMsg{.addr = {.host = raft_context_.LocalAddr().host,
                                       .port = raft_context_.LocalAddr().port},
                              .term = voting_term_,
                              .index = log_entry_.LatestIndex()});

    for (const auto& peer : raft_context_.PeerView()) {
      BroadcastToPeer(vote_request, peer);
    }

    return true;
  }

  return false;
}

void Candidate::BroadcastToPeer(std::string_view request, Addr addr) try {
  SendToClient(addr, request,
               [self = shared_from_this(), this](util::TcpStream&& stream) {
                 try {
                   const Message msg = MessageDecoder::Decode(stream.Text());
                   std::visit(*this, msg);
                   stream.Close();

                 } catch (const std::exception& ex) {
                   spdlog::error("candidate decode error for vote response: {}",
                                 std::strerror(errno));
                 }
               });
} catch (const std::exception& ex) {
  spdlog::error("candidate broadcast to peer [{}:{}] error: {}", addr.host,
                addr.port, ex.what());
}
