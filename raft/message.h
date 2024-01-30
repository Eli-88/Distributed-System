#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include "addr.h"
#include "log_entry.h"

struct RequestVoteMsg {
  static constexpr int MsgType = 0x01;
  Addr addr;
  uint64_t term;
  uint64_t index;
};

struct VoteResultMsg {
  static constexpr int MsgType = 0x02;
  Addr addr;
  bool approved;
};

struct LeaderAliveMsg {
  static constexpr int MsgType = 0x03;
  Addr addr;
};

struct AnnounceNewLeader {
  static constexpr int MsgType = 0x04;
  Addr addr;
  uint64_t term;
  uint64_t index;
};

struct AppendLogsMsg {
  static constexpr int MsgType = 0x05;
  Addr leader_addr;
  LogEntryItem log_entry_item;
};

struct ClientRequest {
  static constexpr int MsgType = 0x06;
  std::string command;
};

struct InvalidMsg {
  std::string msg;
};

using Message = std::variant<RequestVoteMsg,
                             VoteResultMsg,
                             LeaderAliveMsg,
                             AnnounceNewLeader,
                             AppendLogsMsg,
                             ClientRequest,
                             InvalidMsg>;
