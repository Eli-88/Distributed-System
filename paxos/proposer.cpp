#include "proposer.h"

uint64_t Proposer::Propose() {
  return proposed_number++;
}
