#include "alu.hpp"
#include "cpu.h"
#include "lsb.hpp"
#include "memory.hpp"
#include "predictor.hpp"
#include "rob.hpp"
#include "rs.hpp"
#include "tools.h"
#include <cstdio>

int main() {
  // freopen("/home/zx/local_repo/RISC-V-Simulator/testcases/pi.data", "r",
  //         stdin);
  // freopen("error.txt", "w", stderr);
  // freopen("output.txt", "w", stdout);
  dark::CPU cpu;
  TwoBitPredictor two_bit_predictor;
  // AlwaysFalsePredictor always_false_predictor;
  // AlwaysTruePredictor always_true_predictor;
  RSModule rs(&two_bit_predictor);
  MemoryModule memory(&two_bit_predictor);
  LSBModule lsb(memory);
  RobModule executer;
  ALUModule alu;
  cpu.add_module(&memory);
  cpu.add_module(&rs);
  cpu.add_module(&lsb);
  cpu.add_module(&executer);
  cpu.add_module(&alu);

  memory.load();

  // rs -> memory
  rs.op = [&]() -> auto & { return memory.op; };
  rs.rs1 = [&]() -> auto & { return memory.rs1; };
  rs.rs2 = [&]() -> auto & { return memory.rs2; };
  rs.a_in = [&]() -> auto & { return memory.a; };
  rs.pc = [&]() -> auto & { return memory.old_pc; };
  rs.rd = [&]() -> auto & { return memory.rd; };
  rs.jump = [&]() -> auto & { return memory.jump; };
  rs.in = [&]() -> auto & { return memory.out; };

  // memory -> rs
  memory.rs_available = [&]() -> auto & { return rs.memory_flag; };

  // rs -> executer
  rs.rob_in = [&]() -> auto & { return executer.rs_out; };
  rs.rob_pos = [&]() -> auto & { return executer.rs_pos; };
  rs.rob_in_ind = [&]() -> auto & { return executer.rs_ind; };
  rs.need_update = [&]() -> auto & { return executer.rs_update; };
  rs.rob_value = [&]() -> auto & { return executer.rs_value; };
  rs.revert = [&]() -> auto & { return executer.revert; };

  // executer -> rs
  executer.in_rs = [&]() -> auto & { return rs.rob_flag; };
  executer.op_rs = [&]() -> auto & { return rs.rob_op; };
  executer.rs1_rs = [&]() -> auto & { return rs.rob_rs1; };
  executer.rs2_rs = [&]() -> auto & { return rs.rob_rs2; };
  executer.dest_rs = [&]() -> auto & { return rs.rob_dest; };
  executer.a_rs = [&]() -> auto & { return rs.rob_a; };
  executer.pc_rs = [&]() -> auto & { return rs.rob_pc; };
  executer.ind_rs = [&]() -> auto & { return rs.rs_index; };
  executer.jump_rs = [&]() -> auto & { return rs.rob_jump; };

  // alu -> executer
  alu.alu_enable = [&]() -> auto & { return executer.alu_out; };
  alu.operand_1 = [&]() -> auto & { return executer.operand_1; };
  alu.operand_2 = [&]() -> auto & { return executer.operand_2; };
  alu.op = [&]() -> auto & { return executer.alu_op; };
  alu.index_rob = [&]() -> auto & { return executer.alu_index; };

  // executer -> alu
  executer.in_alu = [&]() -> auto & { return alu.done; };
  executer.index_alu = [&]() -> auto & { return alu.rob_index; };
  executer.res_alu = [&]() -> auto & { return alu.res; };

  // memory -> executer
  memory.rob_in = [&]() -> auto & { return executer.memory_out; };
  memory.rob_pc = [&]() -> auto & { return executer.memory_pc; };
  memory.predict_pc = [&]() -> auto & { return executer.memory_predict; };
  memory.is_jump = [&]() -> auto & { return executer.memory_jump; };

  // executer -> lsb
  executer.in_lsb = [&]() -> auto & { return lsb.rob_out; };
  executer.dest_lsb = [&]() -> auto & { return lsb.rob_dest; };
  executer.val_lsb = [&]() -> auto & { return lsb.value; };

  // lsb -> executer
  lsb.revert = [&]() -> auto & { return executer.revert; };
  lsb.rob_in = [&]() -> auto & { return executer.lsb_out; };
  lsb.commit_index = [&]() -> auto & { return executer.lsb_dest; };

  // lsb -> rs
  lsb.in = [&]() -> auto & { return rs.lsb_flag; };
  lsb.op_rs = [&]() -> auto & { return rs.lsb_op; };
  lsb.rs1_rs = [&]() -> auto & { return rs.lsb_rs1; };
  lsb.rs2_rs = [&]() -> auto & { return rs.lsb_rs2; };
  lsb.a_rs = [&]() -> auto & { return rs.lsb_a; };
  lsb.dest_rs = [&]() -> auto & { return rs.lsb_dest; };
  lsb.pos_rs = [&]() -> auto & { return rs.to_lsb_pos; };

  cpu.run(1000000000, true);
  return 0;
}