#include "rs.hpp"
#include "concept.h"
#include "executer.hpp"
#include "opcode.hpp"
#include "tools.h"
#include <cstdint>
#include <ostream>
#include <iomanip>

void RSModule::work() {
  int newly_freed_rd = -1, newly_append_ind = -1;
  Bit<32> new_val;
  bool reuse = 0;
  if (rob_in) {
    int ind = to_unsigned(rob_in_ind);
    if (revert == 0) {
      busy[ind] <= 0;
    }
    if (need_update) {
      update(ind, new_val, newly_freed_rd);
    }
  }
  if (revert) {
    flush();
    return;
  }
  if (in) {
    std::cerr << "RS: Issue PC: " << std::hex << std::setw(8) << std::setfill('0')
              << to_unsigned(pc) << std::dec << std::endl;
    max_size_t i = 0;
    for (; i < RS_MAX; ++i) {
      if (busy[i] == 0) {
        break;
      }
    }
    newly_append_ind = i;
    pos_in_rob <= pos_in_rob + 1;
    busy[i] <= 1;
    pcs[i] <= pc;
    jumps[i] <= jump;
    dispatched[i] <= 0;
    ops[i] <= op;
    rob_dests[i] <= to_unsigned(pos_in_rob);
    bool src1 = 1, src2 = 1, userd = 1;
    exec(i, src1, src2, userd);
    int rs1_ind = to_unsigned(rs1);
    int rs2_ind = to_unsigned(rs2);
    int rd_ind = to_unsigned(rd);
    if (src1) {
      if (reorder_busy[rs1_ind] && newly_freed_rd != rs1_ind) {
        vj[i] <= 0;
        qj[i] <= reorder[rs1_ind];
      } else if (newly_freed_rd == rs1_ind) {
        vj[i] <= newly_freed_rd;
        qj[i] <= Q_DEFAULT;
      } else {
        vj[i] <= regs[rs1_ind];
        qj[i] <= Q_DEFAULT;
      }
    } else {
      vj[i] <= 0;
      qj[i] <= Q_DEFAULT;
    }

    if (src2) {
      if (reorder_busy[rs2_ind] && newly_freed_rd != rs2_ind) {
        vk[i] <= 0;
        qk[i] <= reorder[rs2_ind];
      } else if (newly_freed_rd == rs2_ind) {
        vk[i] <= newly_freed_rd;
        qk[i] <= Q_DEFAULT;
      } else {
        vk[i] <= regs[rs2_ind];
        qk[i] <= Q_DEFAULT;
      }
    } else {
      vk[i] <= 0;
      qk[i] <= Q_DEFAULT;
    }

    if (userd) {
      reorder[rd_ind] <= i;
      reorder_busy[rd_ind] <= 1;
      rds[i] <= rd_ind;
      if (rd_ind == newly_freed_rd) {
        reuse = 1;
      }
    } else {
      rds[i] <= RD_DEFAULT;
    }
  }
  if (!reuse && newly_freed_rd != -1) {
    reorder_busy[newly_freed_rd] <= 0;
  }

  uint32_t cnt = 0;
  for (uint32_t i = 0; i < RS_MAX; ++i) {
    if (busy[i] == 0 && int(i) != newly_append_ind) {
      cnt++;
      if (cnt > MIN_AVAL) {
        break;
      }
    }
  }
  memory_flag <= (cnt > MIN_AVAL);
  bool rob_out_flag = 0, lsb_out_flag = 0;
  for (uint32_t i = 0; i < RS_MAX; ++i) {
    if (busy[i] && qj[i] == Q_DEFAULT && qk[i] == Q_DEFAULT &&
        dispatched[i] == 0 && to_unsigned(rob_dests[i]) < rob_pos + ROB_MAX) {
      std::cerr << "RS: Dispatching instruction at index " << i
                << " with PC: " << std::hex << std::setw(8) << std::setfill('0')
                << to_unsigned(pcs[i]) << std::dec << std::endl;
      trans(i, rob_out_flag, lsb_out_flag);
      break;
    }
  }

  if (!rob_out_flag) {
    rob_flag <= 0;
  }

  if (!lsb_out_flag) {
    lsb_flag <= 0;
  }
}

void RSModule::exec(uint32_t pos, bool &src1, bool &src2, bool &userd) {
  if (op >= ADD && op <= SLTU) {
    ready[pos] <= 1;
  } else if (op >= ADDI && op <= SLTIU) {
    ready[pos] <= 1;
    src2 = 0;
    as[pos] <= a_in;
  } else if (op >= LB && op <= LHU) {
    src2 = 0;
    ready[pos] <= 0;
    as[pos] <= a_in;
    lsb_poses[pos] <= lsb_pos;
    lsb_pos++;
  } else if (op >= SB && op <= SW) {
    userd = 0;
    ready[pos] <= 1;
    as[pos] <= a_in;
    lsb_poses[pos] <= lsb_pos;
    lsb_pos++;
  } else if (op >= BEQ && op <= BGEU) {
    userd = 0;
    ready[pos] <= 1;
    as[pos] <= a_in;
  } else if (op == JALR) {
    src2 = 0;
    ready[pos] <= 1;
    as[pos] <= a_in;
  } else if (op == JAL || op == AUIPC || op == LUI) {
    src1 = 0;
    src2 = 0;
    ready[pos] <= 1;
    as[pos] <= a_in;
  }
}

void RSModule::print_res() {
  Bit<32> res = regs[10];
  std::cout << to_unsigned(res.range<7, 0>()) << std::endl;
  exit(0);
}

void RSModule::update(uint32_t ind, Bit<32> &new_val, int &newly_freed_rd) {
  if (ops[ind] == ADDI && rds[ind] == 10 && vj[ind] == 0 && as[ind] == 255) {
    print_res();
  }
  new_val = rob_value;
  // For register x0, set the value to zero.
  if (rds[ind] == 0) {
    new_val = 0;
  }
  for (uint32_t i = 0; i < RS_MAX; ++i) {
    if (busy[i] == 0) {
      continue;
    }
    if (qj[i] == ind) {
      qj[i] <= Q_DEFAULT;
      vj[i] <= new_val;
    }
    if (qk[i] == ind) {
      qk[i] <= Q_DEFAULT;
      vk[i] <= new_val;
    }
  }
  for (uint32_t i = 0; i < 32; ++i) {
    if (reorder_busy[i] && reorder[i] == ind && revert == 0) {
      newly_freed_rd = i;
      break;
    }
  }
  regs[to_unsigned(rds[ind])] <= new_val;
}

void RSModule::flush() {
  for (max_size_t i = 0; i < 32; ++i) {
    reorder_busy[i] <= 0;
  }
  for (max_size_t i = 0; i < RS_MAX; ++i) {
    busy[i] <= 0;
  }
  rob_flag <= 0;
  lsb_flag <= 0;
  lsb_pos = 0;
  pos_in_rob <= 0;
  memory_flag <= 1;
  return;
}

void RSModule::trans(uint32_t i, bool &rob_out_flag, bool &lsb_out_flag) {
  dispatched[i] <= 1;
  rob_flag <= 1;
  rob_out_flag = 1;
  rob_op <= ops[i];
  rob_rs1 <= vj[i];
  rob_rs2 <= vk[i];
  rob_dest <= rob_dests[i];
  rob_a <= as[i];
  rob_pc <= pcs[i];
  rs_index <= i;
  rob_ready <= ready[i];
  rob_jump <= jumps[i];

  uint32_t oper = to_unsigned(ops[i]);
  if (oper >= LB && oper <= SW) {
    lsb_out_flag = 1;
    lsb_flag <= 1;
    lsb_op <= ops[i];
    lsb_rs1 <= vj[i];
    lsb_rs2 <= vk[i];
    to_lsb_pos <= lsb_poses[i];
    lsb_a <= as[i];
    lsb_dest <= rob_dests[i];
  }
}
