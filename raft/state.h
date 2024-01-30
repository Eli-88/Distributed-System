#pragma once

#include "message.h"

class State {
 public:
  virtual ~State() = default;
  virtual void Start() = 0;
  virtual std::string OnRequest(const Message&) = 0;
};
