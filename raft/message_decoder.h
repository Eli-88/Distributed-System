#pragma once

#include "message.h"

struct MessageDecoder {
 public:
  static Message Decode(std::string_view request);
  static RequestVoteMsg DecodeRequestVoteMsg(std::string_view request);
  static VoteResultMsg DecodeVoteResultMsg(std::string_view request);
  static LeaderAliveMsg DecodeLeaderAliveMsg(std::string_view request);
  static AnnounceNewLeader DecodeAnnounceNewLeader(std::string_view request);
  static AppendLogsMsg DecodeAppendLogsMsg(std::string_view request);
  static ClientRequest DecodeClientRequest(std::string_view request);
};
