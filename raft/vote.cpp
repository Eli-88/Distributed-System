#include "vote.h"
#include <algorithm>
#include <functional>
#include <numeric>

size_t Votes::ApprovedCount() const {
  return std::accumulate(std::begin(votes), std::end(votes), 0,
                         [](std::size_t acc, const auto& item) {
                           const auto& [_, approved] = item;
                           return acc += approved ? 1 : 0;
                         });
}
