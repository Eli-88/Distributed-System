#pragma once

#include <boost/noncopyable.hpp>
#include <cstdint>

class Acceptor : private boost::noncopyable {
 public:
  bool Prepare(uint64_t number);
  bool Accept(uint64_t number);

 private:
  uint64_t next_promised_number{};
  uint64_t next_proposed_number{};
};
