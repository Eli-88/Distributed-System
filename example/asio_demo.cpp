#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

int main()
{
    boost::asio::io_context ctx;
    ctx.post([]()
             { spdlog::info("hello asio"); });

    ctx.run();
}