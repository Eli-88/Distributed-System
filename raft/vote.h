#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include "addr.h"

struct Votes {
  std::unordered_map<Addr, bool> votes{};
  size_t ApprovedCount() const;
};
