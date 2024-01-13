#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/this_coro.hpp>
#include <spdlog/spdlog.h>

namespace asio = boost::asio;
namespace ip = asio::ip;
namespace beast = boost::beast;
namespace http = beast::http;

using tcp = asio::ip::tcp;
using asio::experimental::as_tuple;
using namespace asio::experimental::awaitable_operators;

constexpr auto use_nothrow_awaitable = as_tuple(asio::use_awaitable);

asio::awaitable<void> HandleConn(beast::tcp_stream stream)
{
    beast::static_buffer<8192> buffer;

    for (;;)
    {
        http::request<http::string_body> request;
        const auto [read_err, byte_recv] = co_await http::async_read(stream, buffer, request, use_nothrow_awaitable);
        if (read_err)
        {
            spdlog::error("read error: {}", read_err.message());
            break;
        }

        std::string_view target = beast::to_string_view(request.target());
        spdlog::info("url target: {}", target);
        spdlog::info("body: {}", request.body());

        http::response<http::string_body> response;
        response.result(http::status::ok);
        response.body() = "hello world";
        response.prepare_payload();

        const auto [write_err, _] = co_await http::async_write(stream, response, use_nothrow_awaitable);
        if (write_err)
        {
            spdlog::error("write error {}", write_err.message());
            break;
        }

        if (!request.keep_alive())
        {
            spdlog::info("request no need to keep alive");
            break;
        }
    }
}

asio::awaitable<void> Listen()
{
    auto executor = co_await asio::this_coro::executor;
    tcp::acceptor acceptor(executor, {tcp::v4(), 7000});

    for (;;)
    {
        tcp::socket conn = co_await acceptor.async_accept(asio::use_awaitable);
        beast::tcp_stream stream(std::move(conn));
        asio::co_spawn(co_await asio::this_coro::executor, HandleConn(std::move(stream)), asio::detached);
    }
}

int main()
{
    asio::io_context ctx;

    asio::co_spawn(ctx, Listen(), asio::detached);
    ctx.run();
}
