#pragma once

#include <boost/noncopyable.hpp>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

class Learner : private boost::noncopyable {
 private:
  struct Record {
    uint64_t number{};
    std::string value;

    bool operator<(uint64_t other) const { return number < other; }

    bool operator==(uint64_t other) const { return number == other; }
  };

 public:
  bool Learn(uint64_t number, std::string_view value);
  std::optional<std::string> Retrieve(uint64_t number) const;

 private:
  uint64_t next_accepted_number_{};
  std::vector<Record> values_;
};
