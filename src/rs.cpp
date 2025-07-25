#include "rs.hpp"
#include "concept.h"
#include <cstdint>

void RSModule::work() {
  if (in) {

    max_size_t i = 0;
    for (; i < RS_MAX; ++i) {
      if (busy[i] == 0) {
        break;
      }
    }
    reorder[i] <= pos_in_rob;
    pos_in_rob <= pos_in_rob + 1;
    busy[i] <= 1;
    pcs[i] <= pc;
    jumps[i] <= jump;
    dispatched[i] <= 1;

  }
}