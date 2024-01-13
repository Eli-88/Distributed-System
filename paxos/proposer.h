#pragma once

#include <boost/noncopyable.hpp>
#include <cstdint>

class Proposer : private boost::noncopyable {
 public:
  uint64_t Propose();

 private:
  uint64_t proposed_number{};
};
