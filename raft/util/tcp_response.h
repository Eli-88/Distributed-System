#pragma once

#include <span>
#include <string>
#include <string_view>
#include "util/bytes.h"

namespace util {

// a simple wrapper for response
class TcpResponse {
 public:
  explicit TcpResponse(std::string_view msg) : msg_{msg} {}

  std::span<const std::byte> Buffer() const {
    return ConvertToBytesConst(msg_);
  }

 private:
  std::string msg_;
};

}  // namespace util
