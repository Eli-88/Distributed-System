#include "message_decoder.h"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

Message MessageDecoder::Decode(std::string_view request) {
  spdlog::debug("decode msg: {}", request);
  nlohmann::json req = nlohmann::json::parse(request);
  const int msg_type = req["message_type"].get<int>();

  switch (msg_type) {
    case RequestVoteMsg::MsgType:
      return DecodeRequestVoteMsg(request);
    case VoteResultMsg::MsgType:
      return DecodeVoteResultMsg(request);
    case LeaderAliveMsg::MsgType:
      return DecodeLeaderAliveMsg(request);
    case AnnounceNewLeader::MsgType:
      return DecodeAnnounceNewLeader(request);
    case AppendLogsMsg::MsgType:
      return DecodeAppendLogsMsg(request);
    case ClientRequest::MsgType:
      return DecodeClientRequest(request);
    default:
      return InvalidMsg{.msg = std::string(request)};
  }
}

RequestVoteMsg MessageDecoder::DecodeRequestVoteMsg(std::string_view request) {
  spdlog::debug("request vote msg decoded");
  auto req = nlohmann::json::parse(request);
  RequestVoteMsg msg;
  msg.index = req["index"].get<uint64_t>();
  msg.term = req["term"].get<uint64_t>();

  const std::string host = req["host"].get<std::string>();
  const unsigned short port = req["port"].get<unsigned short>();
  msg.addr = Addr{.host = host, .port = port};

  return msg;
}

VoteResultMsg MessageDecoder::DecodeVoteResultMsg(std::string_view request) {
  spdlog::debug("vote result msg decoded");

  auto req = nlohmann::json::parse(request);
  VoteResultMsg msg;
  msg.addr = Addr{.host = req["host"].get<std::string>(),
                  .port = req["port"].get<unsigned short>()};
  msg.approved = (req["approved"].get<std::string>() == "true");

  return msg;
}

LeaderAliveMsg MessageDecoder::DecodeLeaderAliveMsg(std::string_view request) {
  spdlog::debug("leader alive msg decoded");

  auto req = nlohmann::json::parse(request);
  LeaderAliveMsg msg;
  msg.addr = Addr{.host = req["host"].get<std::string>(),
                  .port = req["port"].get<unsigned short>()};

  return msg;
}

AnnounceNewLeader MessageDecoder::DecodeAnnounceNewLeader(
    std::string_view request) {
  spdlog::debug("announce new leader msg decoded");

  auto req = nlohmann::json::parse(request);
  AnnounceNewLeader msg;

  msg.addr = Addr{.host = req["host"].get<std::string>(),
                  .port = req["port"].get<unsigned short>()};

  msg.index = req["index"].get<int>();
  msg.term = req["term"].get<int>();

  return msg;
}

AppendLogsMsg MessageDecoder::DecodeAppendLogsMsg(std::string_view request) {
  spdlog::debug("append log msg decoded");

  auto req = nlohmann::json::parse(request);
  AppendLogsMsg msg;

  msg.leader_addr = Addr{.host = req["host"].get<std::string>(),
                         .port = req["port"].get<unsigned short>()};

  msg.log_entry_item = LogEntryItem{.index = req["index"].get<uint64_t>(),
                                    .term = req["term"].get<uint64_t>(),
                                    .data = req["data"].get<std::string>()};

  return msg;
}

ClientRequest MessageDecoder::DecodeClientRequest(std::string_view request) {
  spdlog::debug("client request msg decoded");

  auto req = nlohmann::json::parse(request);
  ClientRequest msg;

  msg.command = req["command"].get<std::string>();

  return msg;
}
