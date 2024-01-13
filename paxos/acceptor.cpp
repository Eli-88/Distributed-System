#include "acceptor.h"

bool Acceptor::Prepare(uint64_t number) {
  if (number < next_promised_number) {
    return false;
  }

  next_promised_number = number + 1;
  return true;
}

bool Acceptor::Accept(uint64_t number) {
  if (number < next_proposed_number) {
    return false;
  }

  next_proposed_number = number + 1;
  return true;
}
