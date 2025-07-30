#include "rob.hpp"
#include "tools.h"
#include "util.hpp"
#include <cstdint>

void RobModule::work() {
  if (in_alu) {
    uint32_t ind = to_unsigned(index_alu);
    ready[ind] = 1;
    value[ind] = to_signed(res_alu);
  }
  uint32_t head = pos % ROB_MAX;
  if (busy[head] && ready[head]) {
    if (!exec(head)) {
      return;
    }
  } else {
    rs_out <= 0;
    revert <= 0;
    lsb_out <= 0;
    memory_out <= 0;
  }
  if (in_rs && revert == 0) {
    // std::cerr << "Executer: Appending PC " << std::hex << std::setw(8)
    //           << std::setfill('0') << to_unsigned(pc_rs) << std::dec
    //           << std::endl;
    append(to_unsigned(dest_rs) % ROB_MAX);
  }

  if (in_lsb && revert == 0) {
    max_size_t index = to_unsigned(dest_lsb) % ROB_MAX;
    ready[index] = 1;
    value[index] = to_signed(val_lsb);
  }
}

bool RobModule::exec(max_size_t head) {
  // std::cerr << "Executer: Executing instruction at index " << head
  //           << " with PC: " << std::hex << std::setw(8) << std::setfill('0')
  //           << pcs[head] << std::dec << std::endl;
  bool revert_flag = 0, to_memory_flag = 0, to_lsb_flag = 0, to_rs_flag = 1;
  busy[head] = 0;
  pos++;
  if (ops[head] >= ADD && ops[head] <= SLTIU) {
    rs_value <= value[head];
  } else if (ops[head] >= BEQ && ops[head] <= BGEU) {
    to_memory_flag = 1;
    to_rs_flag = 0;
    memory_predict <= pcs[head];
    if (value[head]) {
      if (jumps[head] == 0) {
        revert_flag = 1;
      }
      memory_jump <= 1;
      memory_pc <= pcs[head] + as[head];
    } else {
      if (jumps[head] == 1) {
        revert_flag = 1;
      }
      memory_jump <= 0;
      memory_pc <= pcs[head] + 4;
    }
  } else if (ops[head] >= LB && ops[head] <= LHU) {
    rs_value <= value[head];
  } else if (ops[head] >= SB && ops[head] <= SW) {
    to_rs_flag = 0;
    to_lsb_flag = 1;
    lsb_dest <= dests[head];
  } else if (ops[head] == JAL) {
    rs_value <= pcs[head] + 4;
  } else if (ops[head] == JALR) {
    rs_value <= pcs[head] + 4;
    to_memory_flag = 1;
    memory_predict <= pcs[head];
    revert_flag = 1;
    memory_pc <= (rs1s[head] + as[head]);
    memory_jump <= 1;
  } else if (ops[head] == AUIPC) {
    rs_value <= pcs[head] + as[head];
  } else if (ops[head] == LUI) {
    rs_value <= as[head];
  }

  rs_out <= 1;
  rs_ind <= rs_inds[head];
  rs_update <= to_rs_flag;
  memory_out <= to_memory_flag;
  if (revert_flag) {
    revert <= 1;
    lsb_out <= 0;
    pos = 0;
    for (uint32_t i = 0; i < ROB_MAX; i++) {
      if (i == head) {
        continue;
      }
      busy[i] = 0;
    }
    return false;
  }
  lsb_out <= to_lsb_flag;
  revert <= 0;
  rs_pos <= pos;
  return true;
}

void RobModule::append(max_size_t index) {
  busy[index] = 1;
  ready[index] = to_signed(ready_rs);
  rs_inds[index] = to_signed(ind_rs);
  dests[index] = to_signed(dest_rs);
  ops[index] = to_signed(op_rs);
  pcs[index] = to_signed(pc_rs);
  rs1s[index] = to_signed(rs1_rs);
  rs2s[index] = to_signed(rs2_rs);
  as[index] = to_signed(a_rs);
  jumps[index] = to_signed(jump_rs);
  if (ops[index] >= ADD && ops[index] <= BGEU) {
    alu_out <= 1;
    ready[index] = 0;
    alu_op <= ops[index];
    operand_1 <= rs1s[index];
    alu_index <= index;
    if (ops[index] <= SLTU || ops[index] >= BEQ) {
      operand_2 <= rs2s[index];
    } else {
      operand_2 <= as[index];
    }
  } else if (ops[index] >= LB && ops[index] <= LHU) {
    ready[index] = 0;
    alu_out <= 0;
  } else {
    ready[index] = 1;
    alu_out <= 0;
  }
}