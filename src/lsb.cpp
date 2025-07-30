#include "lsb.hpp"
#include "concept.h"
#include "tools.h"
#include "util.hpp"
#include <cstdint>

void LSBModule::work() {
  if (rob_in) {
    commit_pos = to_unsigned(commit_index);
  }
  if (revert) {
    while (busy[head]) {
      // Already committed;
      if (commit_pos >= int32_t(dests[head])) {
        busy[head] = 0;
        switch (ops[head]) {
        case Opcode::SB: {
          mem_module.write_memory(address[head], 0, data[head]);
          break;
        }

        case Opcode::SH: {
          mem_module.write_memory(address[head], 1, data[head]);
          break;
        }

        case Opcode::SW: {
          mem_module.write_memory(address[head], 2, data[head]);
          break;
        }
        }
      }
      head = (head + 1) % LSB_MAX;
    }

    head = 0;
    commit_pos = -1;
    for (uint32_t i = 0; i < LSB_MAX; i++) {
      busy[i] = 0;
    }
    rob_out <= 0;
    return;
  }
  if (busy[head]) {
    if (ticker != 3) {
      ticker += 1;
      rob_out <= 0;
    } else {
      ticker = 0;
      exec(head);
    }
  } else {
    rob_out <= 0;
  }
  if (in) {
    max_size_t index = to_unsigned(pos_rs) % LSB_MAX;
    append(index);
  }
}

void LSBModule::exec(uint32_t pos) {
  if (ops[pos] >= Opcode::LB && ops[pos] <= Opcode::LHU) {
    busy[pos] = 0;
    head = (head + 1) % LSB_MAX;
    rob_out <= 1;
    rob_dest <= dests[pos];
    switch (ops[pos]) {
    case Opcode::LB: {
      value <= mem_module.read_memory(address[pos], 0, false);
      break;
    }
    case Opcode::LH: {
      value <= mem_module.read_memory(address[pos], 1, false);
      break;
    }
    case Opcode::LW: {
      value <= mem_module.read_memory(address[pos], 2, false);
      break;
    }
    case Opcode::LBU: {
      value <= mem_module.read_memory(address[pos], 0, true);
      break;
    }
    case Opcode::LHU: {
      value <= mem_module.read_memory(address[pos], 1, true);
      break;
    }
    }
  } else if (ops[pos] >= Opcode::SB && ops[pos] <= Opcode::SW) {
    if (commit_pos >= int32_t(dests[pos])) {
      head = (head + 1) % LSB_MAX;
      busy[pos] = 0;
      rob_out <= 0;
      switch (ops[pos]) {
      case Opcode::SB: {
        mem_module.write_memory(address[pos], 0, data[pos]);
        break;
      }
      case Opcode::SH: {
        mem_module.write_memory(address[pos], 1, data[pos]);
        break;
      }
      case Opcode::SW: {
        mem_module.write_memory(address[pos], 2, data[pos]);
        break;
      }
      }
    }
  }
}

void LSBModule::append(uint32_t pos) {
  busy[pos] = 1;
  ops[pos] = to_unsigned(op_rs);
  dests[pos] = to_unsigned(dest_rs);
  address[pos] = to_unsigned(rs1_rs + a_rs);
  if (ops[pos] >= Opcode::SB && ops[pos] <= Opcode::SW) {
    data[pos] = to_unsigned(rs2_rs);
  }
}
