#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/system/error_code.hpp>

using io_context = boost::asio::io_context;
using tcp = boost::asio::ip::tcp;
using error_code = boost::system::error_code;
using tcp_stream = boost::beast::tcp_stream;
namespace ip = boost::asio::ip;
