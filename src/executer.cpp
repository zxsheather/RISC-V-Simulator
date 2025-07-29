#include "executer.hpp"
#include "util.hpp"
#include "tools.h"
#include <cstdint>

void ExecuterModule::work() {
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
    as[index] = to_signed(val_lsb);
  }
}

bool ExecuterModule::exec(max_size_t head) {
  // std::cerr << "Executer: Executing instruction at index " << head
  //           << " with PC: " << std::hex << std::setw(8) << std::setfill('0')
  //           << pcs[head] << std::dec << std::endl;
  bool revert_flag = 0, to_memory_flag = 0, to_lsb_flag = 0, to_rs_flag = 1;
  busy[head] = 0;
  pos++;
  switch (ops[head]) {
  case Opcode::ADD: {
    // std::cerr << "Executer: ADD" << std::endl;
    rs_value <= (rs1s[head] + rs2s[head]);
    break;
  }

  case Opcode::SUB: {
    // std::cerr << "Executer: SUB" << std::endl;
    rs_value <= (rs1s[head] - rs2s[head]);
    break;
  }

  case Opcode::SLL: {
    // std::cerr << "Executer: SLL" << std::endl;
    int shift = rs2s[head];
    int d = rs1s[head];
    int b = d << shift;
    if (shift > 31) {
      b = 0;
    }
    rs_value <= b;
    break;
  }

  case Opcode::SRL: {
    // std::cerr << "Executer: SRL" << std::endl;
    int shift = rs2s[head];
    unsigned int d = rs1s[head];
    int b = d >> shift;
    if (shift > 31) {
      b = 0;
    }
    rs_value <= b;
    break;
  }

  case Opcode::SRA: {
    int shift = rs2s[head];
    int d = rs1s[head];
    int b = d >> shift;
    if (shift > 31) {
      if (d < 0) {
        b = 0xffffffff;
      } else {
        b = 0;
      }
    }
    rs_value <= b;
    break;
  }

  case Opcode::SLT: {
    // std::cerr << "Executer: SLT" << std::endl;
    rs_value <= (rs1s[head] < rs2s[head]);
    break;
  }

  case Opcode::SLTU: {
    // std::cerr << "Executer: SLTU" << std::endl;
    rs_value <= ((unsigned)rs1s[head] < (unsigned)rs2s[head]);
    break;
  }

  case Opcode::XOR: {
    // std::cerr << "Executer: XOR" << std::endl;
    rs_value <= (rs1s[head] ^ rs2s[head]);
    break;
  }

  case Opcode::OR: {
    // std::cerr << "Executer: OR" << std::endl;
    rs_value <= (rs1s[head] | rs2s[head]);
    break;
  }

  case Opcode::AND: {
    // std::cerr << "Executer: AND" << std::endl;
    rs_value <= (rs1s[head] & rs2s[head]);
    break;
  }

  case Opcode::ADDI: {
    // std::cerr << "Executer: ADDI" << std::endl;
    rs_value <= (rs1s[head] + as[head]);
    break;
  }

  case Opcode::SLLI: {
    // std::cerr << "Executer: SLLI" << std::endl;
    int shift = as[head];
    int c = rs1s[head];
    int b = c << shift;
    if (shift > 31) {
      b = 0;
    }
    rs_value <= b;
    break;
  }

  case Opcode::SRLI: {
    // std::cerr << "Executer: SRLI" << std::endl;
    int shift = as[head];
    unsigned int c = rs1s[head];
    int b = c >> shift;
    if (shift > 31) {
      b = 0;
    }
    rs_value <= b;
    break;
  }

  case Opcode::SRAI: {
    int shift = as[head];
    int c = rs1s[head];
    int b = c >> shift;
    if (shift > 31) {
      if (c < 0) {
        b = 0xffffffff;
      } else {
        b = 0;
      }
    }
    rs_value <= b;
    break;
  }

  case Opcode::ANDI: {
    // std::cerr << "Executer: ANDI" << std::endl;
    rs_value <= (rs1s[head] & as[head]);
    break;
  }

  case Opcode::ORI: {
    // std::cerr << "Executer: ORI" << std::endl;
    rs_value <= (rs1s[head] | as[head]);
    break;
  }

  case Opcode::XORI: {
    // std::cerr << "Executer: XORI" << std::endl;
    rs_value <= (rs1s[head] ^ as[head]);
    break;
  }

  case Opcode::SLTI: {
    // std::cerr << "Executer: SLTI" << std::endl;
    rs_value <= (rs1s[head] < as[head]);
    break;
  }

  case Opcode::SLTIU: {
    // std::cerr << "Executer: SLTIU" << std::endl;
    rs_value <= ((unsigned)rs1s[head] < (unsigned)as[head]);
    break;
  }

  case Opcode::BEQ: {
    // std::cerr << "Executer: BEQ" << std::endl;
    to_memory_flag = 1;
    to_rs_flag = 0;
    memory_predict <= pcs[head];
    if (rs1s[head] == rs2s[head]) {
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
    break;
  }

  case Opcode::BNE: {
    // std::cerr << "Executer: BNE" << std::endl;
    to_memory_flag = 1;
    to_rs_flag = 0;
    memory_predict <= pcs[head];
    if (rs1s[head] != rs2s[head]) {
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
    break;
  }

  case Opcode::BLT: {
    // std::cerr << "Executer: BLT" << std::endl;
    to_memory_flag = 1;
    to_rs_flag = 0;
    memory_predict <= pcs[head];
    if (rs1s[head] < rs2s[head]) {
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
    break;
  }

  case Opcode::BGE: {
    // std::cerr << "Executer: BGE" << std::endl;
    to_memory_flag = 1;
    to_rs_flag = 0;
    memory_predict <= pcs[head];
    if (rs1s[head] >= rs2s[head]) {
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
    break;
  }

  case Opcode::BLTU: {
    // std::cerr << "Executer: BLTU" << std::endl;
    to_memory_flag = 1;
    to_rs_flag = 0;
    memory_predict <= pcs[head];
    if ((unsigned)rs1s[head] < (unsigned)rs2s[head]) {
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
    break;
  }

  case Opcode::BGEU: {
    // std::cerr << "Executer: BGEU" << std::endl;
    to_memory_flag = 1;
    to_rs_flag = 0;
    memory_predict <= pcs[head];
    if ((unsigned)rs1s[head] >= (unsigned)rs2s[head]) {
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
    break;
  }

  case Opcode::LB: {
    // std::cerr << "Executer: LB" << std::endl;
    rs_value <= as[head];
    break;
  }

  case Opcode::LH: {
    // std::cerr << "Executer: LH" << std::endl;
    rs_value <= as[head];
    break;
  }

  case Opcode::LW: {
    // std::cerr << "Executer: LW" << std::endl;
    rs_value <= as[head];
    break;
  }

  case Opcode::LBU: {
    // std::cerr << "Executer: LBU" << std::endl;
    rs_value <= as[head];
    break;
  }

  case Opcode::LHU: {
    // std::cerr << "Executer: LHU" << std::endl;
    rs_value <= as[head];
    break;
  }

  case Opcode::SB: {
    // std::cerr << "Executer: SB" << std::endl;
    to_rs_flag = 0;
    to_lsb_flag = 1;
    lsb_dest <= dests[head];
    break;
  }

  case Opcode::SH: {
    // std::cerr << "Executer: SH" << std::endl;
    to_rs_flag = 0;
    to_lsb_flag = 1;
    lsb_dest <= dests[head];
    break;
  }

  case Opcode::SW: {
    // std::cerr << "Executer: SW" << std::endl;
    to_rs_flag = 0;
    to_lsb_flag = 1;
    lsb_dest <= dests[head];
    break;
  }

  case Opcode::JAL: {
    // std::cerr << "Executer: JAL" << std::endl;
    rs_value <= pcs[head] + 4;
    break;
  }

  case Opcode::JALR: {
    // std::cerr << "Executer: JALR" << std::endl;
    rs_value <= pcs[head] + 4;
    to_memory_flag = 1;
    memory_predict <= pcs[head];
    revert_flag = 1;
    memory_pc <= (rs1s[head] + as[head]);
    memory_jump <= 1;
    break;
  }

  case Opcode::AUIPC: {
    // std::cerr << "Executer: AUIPC" << std::endl;
    rs_value <= pcs[head] + as[head];
    break;
  }

  case Opcode::LUI: {
    // std::cerr << "Executer: LUI" << std::endl;
    rs_value <= as[head];
    break;
  }

  default: {
    to_rs_flag = 0;
    break;
  }
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

void ExecuterModule::append(max_size_t index) {
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
}