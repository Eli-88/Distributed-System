#include <spdlog/spdlog.h>
#include <algorithm>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/rolling_count.hpp>
#include <boost/accumulators/statistics/rolling_mean.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/asio.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <thread>

namespace asio = boost::asio;
namespace ip = asio::ip;
namespace beast = boost::beast;
namespace http = beast::http;

using io_context = boost::asio::io_context;

using tcp = asio::ip::tcp;
using asio::experimental::as_tuple;
using namespace asio::experimental::awaitable_operators;

using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;

using namespace boost::accumulators;

int64_t SendRequest(io_context& ctx, tcp::endpoint ep, std::string_view json) {
  const auto start_time = std::chrono::steady_clock::now();

  beast::tcp_stream stream(ctx);

  beast::error_code ec;

  stream.connect(ep, ec);

  if (ec) {
    spdlog::error("connection error for {}:{}: {}", ep.address().to_string(),
                  ep.port(), ec.message());
  }

  Request request;
  request.keep_alive(false);
  request.method(http::verb::post);
  request.target("/proposer/request");
  request.body() = json;
  request.prepare_payload();

  http::write(stream, request, ec);

  Response response;
  beast::static_buffer<100> buffer;

  http::read(stream, buffer, response, ec);

  if (ec) {
    spdlog::error("read error: {}", ec.message());
  }

  const auto end_time = std::chrono::steady_clock::now();

  const int64_t time_diff =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end_time -
                                                           start_time)
          .count();

  return time_diff;
}

void Task(io_context& ctx,
          tcp::endpoint ep,
          int iteration,
          std::string request) {
  accumulator_set<int, stats<tag::rolling_mean>> acc(
      tag::rolling_window::window_size = 1);

  for (int i = 0; i < iteration; ++i) {
    acc(SendRequest(ctx, ep, request));
  }

  spdlog::info("rolling mean for {}:{} -> {}ns", ep.address().to_string(),
               ep.port(), rolling_mean(acc));
}

int main() {
  nlohmann::json json;
  json["value"] = "hello world";
  const std::string request = json.dump();

  io_context ctx;

  std::vector<tcp::endpoint> endpoints;

  tcp::resolver resolver(ctx);
  endpoints.emplace_back(*resolver.resolve("127.0.0.1", "7000"));
  endpoints.emplace_back(*resolver.resolve("127.0.0.1", "7001"));
  endpoints.emplace_back(*resolver.resolve("127.0.0.1", "7002"));
  endpoints.emplace_back(*resolver.resolve("127.0.0.1", "7003"));

  std::vector<std::thread> grp;
  for (const auto& ep : endpoints) {
    grp.emplace_back(
        std::thread([&ctx, ep, request] { Task(ctx, ep, 1000000, request); }));
  }

  for (auto& t : grp) {
    t.join();
  }
}
