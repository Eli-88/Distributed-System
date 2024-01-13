#include "accept_prepare_handler.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

AcceptPrepareHandler::AcceptPrepareHandler(std::shared_ptr<Acceptor> acceptor)
    : acceptor_{acceptor} {}

awaitable<std::string> AcceptPrepareHandler::operator()(Request request) try {
  spdlog::debug("AcceptPrepareHandler recv: {}", request.body());

  auto json = nlohmann::json::parse(request.body());
  const uint64_t number = json["number"].get<uint64_t>();

  const bool success = acceptor_->Prepare(number);

  nlohmann::json response_json;
  response_json["number"] = number;
  response_json["approve"] = success ? "true" : "false";

  const std::string response = response_json.dump();
  spdlog::debug("AcceptPrepareHandler response: {}", response);

  co_return response;
} catch (const std::exception& ec) {
  const std::string error_msg =
      fmt::format("/accept/prepare error: {}", ec.what());
  spdlog::error(error_msg);
  co_return error_msg;
}
