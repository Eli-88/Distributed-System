#include <boost/asio.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/awaitable.hpp>
#include <spdlog/spdlog.h>
#include <array>
#include <chrono>

namespace asio = boost::asio;
namespace ip = asio::ip;
using tcp = ip::tcp;
using error_code = boost::system::error_code;
using steady_clock = std::chrono::steady_clock;
using namespace std::literals::chrono_literals;
using namespace asio::experimental::awaitable_operators;

constexpr auto use_nothrow_awaitable = asio::as_tuple(asio::use_awaitable);

asio::awaitable<void> Timeout(steady_clock::duration duration)
{
    auto executor = co_await asio::this_coro::executor;
    asio::steady_timer timer(executor);
    timer.expires_after(duration);
    co_await timer.async_wait(use_nothrow_awaitable);
}

asio::awaitable<void> HandleConn(tcp::socket conn)
{
    std::array<char, 1024> buffer{};
    for (;;)
    {
        const auto read_result = co_await (conn.async_read_some(asio::buffer(buffer, buffer.size()), use_nothrow_awaitable) || Timeout(5s));
        if (read_result.index() == 1)
        {
            spdlog::error("read timeout");
            break;
        }

        const auto [err, byte_recv] = std::get<0>(read_result);

        if (err)
        {
            spdlog::error("read error: {}", err.message());
            break;
        }

        spdlog::info("message recv: {}", std::string_view(buffer.data(), byte_recv));

        co_await conn.async_write_some(asio::buffer(buffer, byte_recv), use_nothrow_awaitable);
    }
}

asio::awaitable<void> Listen(tcp::acceptor acceptor)
{
    auto executor = co_await asio::this_coro::executor;
    for (;;)
    {
        tcp::socket new_conn = co_await acceptor.async_accept(asio::use_awaitable);
        asio::co_spawn(executor, HandleConn(std::move(new_conn)), asio::detached);
    }
}

int main()
{
    asio::io_context ctx;
    tcp::acceptor acceptor(ctx, {tcp::v4(), 7000});

    asio::co_spawn(ctx, Listen(std::move(acceptor)), asio::detached);

    ctx.run();
}