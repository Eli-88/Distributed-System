#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

using Response = boost::beast::http::response<boost::beast::http::string_body>;
using Request = boost::beast::http::request<boost::beast::http::string_body>;
namespace http = boost::beast::http;

template <std::size_t T>
using static_buffer = boost::beast::static_buffer<T>;
