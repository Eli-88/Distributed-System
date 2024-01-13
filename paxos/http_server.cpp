#include "http_server.h"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <utility>

void HttpServer::RegisterCallback(
    std::string target,
    const std::function<awaitable<std::string>(Request)>& callback) {
  callback_mappings_[target] = callback;
}

awaitable<void> HttpServer::Run(tcp::endpoint endpoint) {
  auto executor = co_await this_coro::executor;
  tcp::acceptor acceptor(executor, endpoint);

  for (;;) {
    tcp::socket conn = co_await acceptor.async_accept(use_awaitable);
    tcp_stream stream(std::move(conn));
    co_spawn(executor, HandleConn(std::move(stream)), detached);
  }
}

awaitable<void> HttpServer::HandleConn(tcp_stream stream) {
  static_buffer<1024> buffer{};
  for (;;) {
    Request request;
    const auto read_result = co_await (
        http::async_read(stream, buffer, request, use_nothrow_awaitable) ||
        Timeout(10s));
    if (read_result.index() == 1) {
      spdlog::debug("connection timeout");
      break;
    }

    const auto [read_err, byte_recv] = std::get<0>(read_result);
    if (read_err) {
      spdlog::debug("connection read error: {}", read_err.message());
      break;
    }

    const std::string target_url =
        std::string(boost::beast::to_string_view(request.target()));

    auto cb_iter = callback_mappings_.find(target_url);
    if (cb_iter == callback_mappings_.end()) {
      spdlog::debug("invalid target url: {}", target_url);

      Response response;
      response.result(http::status::bad_request);
      response.body() = fmt::format("invalid target url: {}", target_url);
      response.prepare_payload();

      co_await http::async_write(stream, response, use_nothrow_awaitable);
      break;
    }

    std::string str_response = co_await cb_iter->second(request);

    Response response;
    response.result(http::status::ok);
    response.body() = std::move(str_response);
    response.prepare_payload();

    std::ostringstream os;
    os << response;
    spdlog::debug("http response:\n{}", os.str());

    const auto [write_err, _] =
        co_await http::async_write(stream, response, use_nothrow_awaitable);
    if (write_err) {
      spdlog::debug("connection write error: {}", write_err.message());
      break;
    }

    if (!request.keep_alive()) {
      spdlog::debug("not require to keep alive");
      break;
    }
  }

  co_return;
}

awaitable<void> HttpServer::Timeout(duration seconds) {
  steady_timer timer(co_await this_coro::executor);
  timer.expires_after(seconds);
  co_await timer.async_wait(use_nothrow_awaitable);
}
