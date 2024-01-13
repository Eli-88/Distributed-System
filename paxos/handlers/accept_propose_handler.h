#pragma once

#include <memory>
#include <string>
#include "acceptor.h"
#include "awaitable.h"
#include "http.h"

class AcceptProposeHandler {
 public:
  explicit AcceptProposeHandler(std::shared_ptr<Acceptor> acceptor);
  awaitable<std::string> operator()(Request request);

 private:
  std::shared_ptr<Acceptor> acceptor_;
};
