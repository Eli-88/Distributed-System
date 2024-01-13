#include "propose_request_handler.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <boost/range/combine.hpp>
#include <cassert>
#include <nlohmann/json.hpp>
#include <utility>

ProposeRequestHandler::ProposeRequestHandler(
    std::shared_ptr<Proposer> proposer,
    std::shared_ptr<Learner> learner,
    const std::vector<tcp::endpoint>& peers)
    : proposer_{proposer}, learner_{learner}, peers_{peers} {}

awaitable<std::string> ProposeRequestHandler::operator()(Request request) try {
  auto json = nlohmann::json::parse(request.body());
  const std::string value = json["value"].get<std::string>();

  std::size_t minium_vote_count = peers_.size() / 2 + 1;

  bool approve{false};
  int max_tries = 0;
  while (!approve && max_tries < 10) {
    const uint64_t proposal_number = proposer_->Propose();
    approve =
        co_await InitiateRequest(proposal_number, value, minium_vote_count);
    ++max_tries;
  }

  co_return approve ? "ok" : "not ok";

} catch (const std::exception& ec) {
  const std::string error_msg =
      fmt::format("/propose/request error: {}", ec.what());
  spdlog::error(error_msg);
  co_return error_msg;
}

awaitable<bool> ProposeRequestHandler::InitiateRequest(
    uint64_t proposal_number,
    std::string_view value,
    uint64_t minium_vote_count) {
  std::vector<tcp_stream> conns;
  auto executor = co_await this_coro::executor;

  for (const auto& peer : peers_) {
    tcp::socket conn(executor);
    const auto [conn_err] =
        co_await conn.async_connect(peer, use_nothrow_awaitable);
    if (conn_err) {
      spdlog::error("connection error for {}:{} -> {}",
                    peer.address().to_string(), peer.port(),
                    conn_err.message());
      continue;
    }

    conns.emplace_back(tcp_stream(std::move(conn)));
  }

  bool approve =
      co_await PrepareRequest(conns, proposal_number, minium_vote_count);
  if (!approve) {
    co_return false;
  }

  approve = co_await ProposeRequest(conns, proposal_number, minium_vote_count);

  if (approve) {
    spdlog::debug("approved value: {}", value);
    co_await LearnValue(conns, proposal_number, value);
  }

  co_return approve;
}

awaitable<bool> ProposeRequestHandler::PrepareRequest(
    std::vector<tcp_stream>& conns,
    uint64_t proposal_number,
    std::size_t minium_vote_count) try {
  nlohmann::json prepare_request_json;
  prepare_request_json["number"] = proposal_number;

  Request prepare_request;
  prepare_request.target("/acceptor/prepare");
  prepare_request.method(http::verb::post);
  prepare_request.keep_alive(true);
  prepare_request.body() = prepare_request_json.dump();
  prepare_request.prepare_payload();

  std::ostringstream os;
  os << prepare_request;
  spdlog::debug("PrepareRequest request: {}", os.str());

  uint64_t vote_count = co_await AccumulateVotes(conns, prepare_request);

  if (vote_count < minium_vote_count) {
    spdlog::debug("not enough votes for prepare request {}/{}", vote_count,
                  minium_vote_count);
    co_return false;
  }

  co_return true;
} catch (const std::exception& ec) {
  spdlog::error("prepare request error: {}", ec.what());
  co_return false;
}

awaitable<bool> ProposeRequestHandler::ProposeRequest(
    std::vector<tcp_stream>& conns,
    uint64_t proposal_number,
    std::size_t minium_vote_count) try {
  nlohmann::json propose_request_json;
  propose_request_json["number"] = proposal_number;

  Request propose_request;
  propose_request.target("/acceptor/propose");
  propose_request.method(http::verb::post);
  propose_request.keep_alive(true);
  propose_request.body() = propose_request_json.dump();
  propose_request.prepare_payload();

  std::ostringstream os;
  os << propose_request;
  spdlog::debug("ProposeRequest request: {}", os.str());

  uint64_t vote_count = co_await AccumulateVotes(conns, propose_request);

  if (vote_count < minium_vote_count) {
    spdlog::debug("not enough votes for propose request {}/{}", vote_count,
                  minium_vote_count);
    co_return false;
  }

  co_return true;
} catch (const std::exception& ec) {
  spdlog::error("propose request error: {}", ec.what());
  co_return false;
}

awaitable<uint64_t> ProposeRequestHandler::AccumulateVotes(
    std::vector<tcp_stream>& conns,
    Request request) {
  for (auto& conn : conns) {
    http::async_write(conn, request, detached);
  }

  static_buffer<1024> buffer{};

  uint64_t vote_count{};

  for (auto& conn : conns) {
    Response response;

    const auto read_result = co_await (
        http::async_read(conn, buffer, response, use_nothrow_awaitable) ||
        Timeout(150ms));
    if (read_result.index() == 1) {
      spdlog::debug("read timeout when accumulating votes");
      continue;
    }

    const auto [read_err, _] = std::get<0>(read_result);
    if (read_err) {
      spdlog::error("vote request read error: {}", read_err.message());
      continue;
    }

    spdlog::debug("Vote recv: {}", response.body());
    const auto json = nlohmann::json::parse(response.body());

    const bool approved = ("true" == json["approve"].get<std::string>());

    vote_count += approved ? 1 : 0;
  }
  co_return vote_count;
}

awaitable<void> ProposeRequestHandler::LearnValue(
    std::vector<tcp_stream>& conns,
    uint64_t proposal_number,
    std::string_view value) {
  nlohmann::json request_json;
  request_json["number"] = proposal_number;
  request_json["value"] = value;

  Request request;
  request.target("/learner/accept");
  request.method(http::verb::post);
  request.body() = request_json.dump();

  request.keep_alive(
      false);  // no need to keep alive as it is the last request needed

  request.prepare_payload();

  for (auto& conn : conns) {
    co_await http::async_write(conn, request, use_nothrow_awaitable);
  }

  learner_->Learn(proposal_number, value);
  co_return;
}

awaitable<void> ProposeRequestHandler::Timeout(duration delay) {
  steady_timer timer(co_await this_coro::executor);
  timer.expires_after(delay);
  co_await timer.async_wait(use_nothrow_awaitable);
}
