#pragma once

#include <memory>
#include <string>
#include "awaitable.h"
#include "http.h"
#include "learner.h"

class LearnAcceptHandler {
 public:
  explicit LearnAcceptHandler(std::shared_ptr<Learner> learner);
  awaitable<std::string> operator()(Request request);

 private:
  std::shared_ptr<Learner> learner_;
};
