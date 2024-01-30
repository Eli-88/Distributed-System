#pragma once

#include <boost/noncopyable.hpp>
#include <cstdint>
#include <optional>
#include <span>
#include <vector>
#include "addr.h"

class RaftContext : private boost::noncopyable {
 public:
  RaftContext(Addr local_addr, std::vector<Addr> peers);

  std::optional<Addr> CurrentLeader() const;
  Addr LocalAddr() const;
  bool IsLeader() const;
  bool IsLeaderAvailable() const;
  std::span<const Addr> PeerView() const;

  void CurrentLeader(Addr leader);
  void ResetLeader();

 private:
  const Addr local_addr_;
  const std::vector<Addr> peers_;
  std::optional<Addr> current_leader_{std::nullopt};
};
