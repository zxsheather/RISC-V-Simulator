#include "module.h"
#include "predictor.hpp"
#include "tools.h"
#include "util.hpp"
#include <array>
#include <cstdint>

struct RSInput {
  Wire<1> in;
  Wire<32> op;
  Wire<32> rs1;
  Wire<32> rs2;
  Wire<32> rd;
  Wire<32> a_in;
  Wire<32> pc;
  Wire<1> jump;

  Wire<1> rob_in;
  Wire<1> revert;
  Wire<32> rob_pos;
  Wire<32> rob_in_ind;
  Wire<1> need_update;
  Wire<32> rob_value;
};

struct RSOutput {
  Register<1> memory_flag;
  Register<1> rob_flag;
  Register<32> rob_op;
  Register<32> rob_rs1;
  Register<32> rob_rs2;
  Register<32> rob_dest;
  Register<32> rob_pc;
  Register<32> rob_a;
  Register<1> rob_jump;
  Register<1> rob_ready;
  Register<32> rs_index;

  Register<1> lsb_flag;
  Register<32> lsb_op;
  Register<32> lsb_rs1;
  Register<32> lsb_rs2;
  Register<32> lsb_a;
  Register<32> lsb_dest;
  Register<32> to_lsb_pos;
};

struct RSPrivate {
  Register<32> pos_in_rob;
  std::array<Register<1>, RS_MAX> busy;
  std::array<Register<32>, RS_MAX> ops;
  std::array<Register<32>, RS_MAX> vj;
  std::array<Register<32>, RS_MAX> vk;
  std::array<Register<32>, RS_MAX> qj;
  std::array<Register<32>, RS_MAX> qk;
  std::array<Register<32>, RS_MAX> rds;
  std::array<Register<32>, RS_MAX> rob_dests;
  std::array<Register<32>, RS_MAX> as;
  std::array<Register<32>, RS_MAX> pcs; // Pcs of insts. The unique identifier.
  std::array<Register<1>, RS_MAX> jumps;
  std::array<Register<1>, RS_MAX> dispatched;
  std::array<Register<1>, RS_MAX> ready;
  std::array<Register<32>, RS_MAX> lsb_poses;
  std::array<Register<32>, 32> regs;
  std::array<Register<1>, 32> reorder_busy; // Reorder buffer busy flags.
  std::array<Register<32>, RS_MAX> reorder;
};

struct RSModule : dark::Module<RSInput, RSOutput, RSPrivate> {
  BranchPredictor *predictor;
  uint32_t cycle = 0;
  void work() override final;
  void exec(uint32_t pos, bool &src1, bool &src2, bool &userd);
  uint32_t lsb_pos = 0;
  void print_res();
  void update(uint32_t ind, Bit<32> &new_val, int &newly_freed_rd);
  void flush();
  void trans(uint32_t i, bool &rob_out_flag, bool &lsb_out_flag);
  RSModule(BranchPredictor *predictor) : predictor(predictor) {}
};