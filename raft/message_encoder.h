#pragma once

#include <string>
#include "message.h"

std::string Encode(const RequestVoteMsg& msg);
std::string Encode(const VoteResultMsg& msg);
std::string Encode(const LeaderAliveMsg& msg);
std::string Encode(const AnnounceNewLeader& msg);
std::string Encode(const AppendLogsMsg& msg);
std::string Encode(const ClientRequest& msg);
std::string Encode(const InvalidMsg& msg);