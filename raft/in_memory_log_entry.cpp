#include "in_memory_log_entry.h"

bool InMemoryLogEntry::Append(const LogEntryItem& item) {
  if (item.index != latest_index_ + 1) {
    return false;
  }

  entries_.emplace_back(item);
  latest_index_ += 1;
  return true;
}

std::span<const LogEntryItem> InMemoryLogEntry::Slice(uint64_t start,
                                                      uint64_t end) const {
  return std::span<const LogEntryItem>(std::begin(entries_) + start, end);
}

uint64_t InMemoryLogEntry::LatestIndex() const {
  return latest_index_;
}
