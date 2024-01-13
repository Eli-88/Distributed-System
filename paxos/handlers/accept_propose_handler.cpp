#include "accept_propose_handler.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

AcceptProposeHandler::AcceptProposeHandler(std::shared_ptr<Acceptor> acceptor)
    : acceptor_{acceptor} {}

awaitable<std::string> AcceptProposeHandler::operator()(Request request) try {
  spdlog::debug("AcceptProposeHandler recv: {}", request.body());

  auto json = nlohmann::json::parse(request.body());
  const uint64_t number = json["number"].get<uint64_t>();
  const bool success = acceptor_->Accept(number);

  nlohmann::json response_json;
  response_json["number"] = number;
  response_json["approve"] = success ? "true" : "false";

  co_return response_json.dump();
} catch (const std::exception& ec) {
  const std::string error_msg =
      fmt::format("/accept/propose error: {}", ec.what());
  spdlog::error(error_msg);
  co_return error_msg;
}
