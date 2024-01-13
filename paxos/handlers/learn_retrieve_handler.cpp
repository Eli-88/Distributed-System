#include "learn_retrieve_handler.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

LearnRetrieveHandler::LearnRetrieveHandler(std::shared_ptr<Learner> learner)
    : learner_{learner} {}

awaitable<std::string> LearnRetrieveHandler::operator()(Request request) try {
  spdlog::debug("LearnRetrieveHandler recv: {}", request.body());

  auto json = nlohmann::json::parse(request.body());

  const uint64_t number = json["number"].get<uint64_t>();
  std::optional<std::string> value = learner_->Retrieve(number);

  nlohmann::json response;
  response["result"] = value.has_value() ? value.value() : "";
  co_return response.dump();
} catch (const std::exception& ec) {
  const std::string error_msg =
      fmt::format("learner/retrieve error: {}", ec.what());
  spdlog::error(error_msg);
  co_return error_msg;
}
