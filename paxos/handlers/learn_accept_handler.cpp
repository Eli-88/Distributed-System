#include "learn_accept_handler.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

LearnAcceptHandler::LearnAcceptHandler(std::shared_ptr<Learner> learner)
    : learner_{learner} {}

awaitable<std::string> LearnAcceptHandler::operator()(Request request) try {
  spdlog::debug("LearnAcceptHandler recv: {}", request.body());

  auto json = nlohmann::json::parse(request.body());
  const uint64_t number = json["number"].get<uint64_t>();
  const std::string value = json["value"].get<std::string>();

  const bool success = learner_->Learn(number, value);

  nlohmann::json response_json;
  response_json["number"] = number;
  response_json["approve"] = success ? "true" : "false";
  co_return response_json.dump();
} catch (const std::exception& ec) {
  const std::string error_msg =
      fmt::format("/learn/accept error: {}", ec.what());
  co_return error_msg;
}
