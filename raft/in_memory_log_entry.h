#include <vector>
#include "log_entry.h"

class InMemoryLogEntry : public LogEntry {
 public:
  bool Append(const LogEntryItem& item) override;
  std::span<const LogEntryItem> Slice(uint64_t start,
                                      uint64_t end) const override;
  uint64_t LatestIndex() const override;

 private:
  uint64_t latest_index_{0};
  std::vector<LogEntryItem> entries_;
};
