#pragma once

#include "addr.h"

class StateMediator {
 public:
  virtual void SwitchToFollower() = 0;
  virtual void SwitchToLeader(const Addr& addr) = 0;
  virtual void SwitchToCandidate() = 0;
};
