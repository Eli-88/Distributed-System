#pragma once

#include <boost/beast/http.hpp>
#include <boost/noncopyable.hpp>
#include <functional>
#include <string>
#include <unordered_map>
#include "awaitable.h"
#include "http.h"
#include "net.h"
#include "timer.h"

// ignore http method for this server
class HttpServer : private boost::noncopyable {
 public:
  void RegisterCallback(
      std::string target,
      const std::function<awaitable<std::string>(Request)>& callback);
  awaitable<void> Run(tcp::endpoint endpoint);

 private:
  std::unordered_map<std::string,
                     std::function<awaitable<std::string>(Request)>>
      callback_mappings_;

  awaitable<void> HandleConn(tcp_stream stream);
  awaitable<void> Timeout(duration seconds);
};
