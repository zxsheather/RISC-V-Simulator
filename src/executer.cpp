#include "executer.hpp"
#include "concept.h"
#include "tools.h"
#include <cstdint>

void ExecuterModule::work() {
  uint32_t head = pos % ROB_MAX;
  if (busy[head] && ready[head]) {
    exec(head);
  }
  if (in) {
    append(to_unsigned(dest_rs) % ROB_MAX);
  }
}