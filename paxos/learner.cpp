#include "learner.h"
#include <algorithm>

bool Learner::Learn(uint64_t number, std::string_view value) {
  if (number < next_accepted_number_) {
    return false;
  }

  next_accepted_number_ = number + 1;

  values_.emplace_back(Record{.number = number, .value = std::string(value)});
  return true;
}

std::optional<std::string> Learner::Retrieve(uint64_t number) const {
  auto result_iter =
      std::lower_bound(std::begin(values_), std::end(values_), number);
  if (result_iter == std::end(values_) || result_iter->number != number) {
    return std::nullopt;
  }

  return result_iter->value;
}
