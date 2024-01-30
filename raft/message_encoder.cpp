#include "message_encoder.h"
#include <fmt/format.h>
#include <nlohmann/json.hpp>

std::string Encode(const RequestVoteMsg& msg) {
  nlohmann::json result;
  result["message_type"] = RequestVoteMsg::MsgType;
  result["term"] = msg.term;
  result["index"] = msg.index;
  result["host"] = msg.addr.host;
  result["port"] = msg.addr.port;

  return result.dump();
}

std::string Encode(const VoteResultMsg& msg) {
  nlohmann::json result;
  result["message_type"] = VoteResultMsg::MsgType;
  result["approved"] = msg.approved ? "true" : "false";
  result["host"] = msg.addr.host;
  result["port"] = msg.addr.port;

  return result.dump();
}

std::string Encode(const LeaderAliveMsg& msg) {
  nlohmann::json result;
  result["message_type"] = LeaderAliveMsg::MsgType;
  result["host"] = msg.addr.host;
  result["port"] = msg.addr.port;

  return result.dump();
}
std::string Encode(const AnnounceNewLeader& msg) {
  nlohmann::json result;
  result["message_type"] = AnnounceNewLeader::MsgType;
  result["host"] = msg.addr.host;
  result["port"] = msg.addr.port;
  result["term"] = msg.term;
  result["index"] = msg.index;

  return result.dump();
}
std::string Encode(const AppendLogsMsg& msg) {
  nlohmann::json result;
  result["message_type"] = AppendLogsMsg::MsgType;
  result["index"] = msg.log_entry_item.index;
  result["term"] = msg.log_entry_item.term;
  result["data"] = msg.log_entry_item.data;
  result["host"] = msg.leader_addr.host;
  result["port"] = msg.leader_addr.port;

  return result.dump();
}

std::string Encode(const ClientRequest& msg) {
  nlohmann::json result;
  result["message_type"] = ClientRequest::MsgType;
  result["command"] = msg.command;

  return result.dump();
}

std::string Encode(const InvalidMsg& msg) {
  nlohmann::json result;
  result["result"] = fmt::format("invalid msg: {}", msg.msg);

  return result.dump();
}
