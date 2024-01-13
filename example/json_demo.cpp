#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>

int main()
try
{
    nlohmann::json json;
    json["hello"] = 1;
    json["world"] = "two";

    spdlog::info("result: {}", json.dump());

    nlohmann::json decoded_json = nlohmann::json::parse(json.dump());
    spdlog::info("hello: {}", decoded_json["hello"].get<int>());
    spdlog::info("world: {}", decoded_json["world"].get<std::string>());
}
catch (const std::exception &ex)
{
    spdlog::error("exception caught: {}", ex.what());
}