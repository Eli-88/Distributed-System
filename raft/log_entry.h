#pragma once

#include <cstdint>
#include <span>
#include <string>

struct LogEntryItem {
  uint64_t index;
  uint64_t term;
  std::string data;
};

class LogEntry {
 public:
  virtual bool Append(const LogEntryItem& item) = 0;
  virtual std::span<const LogEntryItem> Slice(uint64_t start,
                                              uint64_t end) const = 0;
  virtual uint64_t LatestIndex() const = 0;
};
