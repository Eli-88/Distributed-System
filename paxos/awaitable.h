#pragma once

#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/experimental/as_tuple.hpp>
#include <boost/asio/experimental/awaitable_operators.hpp>
#include <boost/asio/this_coro.hpp>

constexpr auto use_awaitable = boost::asio::use_awaitable;
constexpr auto use_nothrow_awaitable =
    boost::asio::as_tuple(boost::asio::use_awaitable);
constexpr auto detached = boost::asio::detached;
constexpr auto deferred = boost::asio::deferred;
namespace this_coro = boost::asio::this_coro;
using namespace boost::asio::experimental::awaitable_operators;

template <typename T>
using awaitable = boost::asio::awaitable<T>;
