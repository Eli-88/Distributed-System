#pragma once

#include <string_view>
#include "addr.h"
#include "util/event_loop.h"
#include "util/tcp_client.h"
#include "util/tcp_server.h"
#include "util/tcp_stream.h"

util::EventLoop& GetLoop();

void SendToClient(Addr addr,
                  std::string_view msg,
                  std::function<void(util::TcpStream)> callback);

void CallAfter(int delay_ms, const std::function<void()>& handle);
