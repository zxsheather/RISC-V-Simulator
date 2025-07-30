#include "module.h"
#include "register.h"
#include "tools.h"
#include "util.hpp"
#include <cstdint>

struct RobInput {
  Wire<1> in_rs;
  Wire<32> op_rs;
  Wire<32> rs1_rs;
  Wire<32> rs2_rs;
  Wire<32> dest_rs;
  Wire<32> a_rs;
  Wire<32> pc_rs;
  Wire<32> ind_rs;
  Wire<1> jump_rs;

  Wire<1> in_alu;
  Wire<32> index_alu;
  Wire<32> res_alu;

  Wire<1> in_lsb;
  Wire<32> dest_lsb;
  Wire<32> val_lsb;
};

struct RobOutput {
  Register<1> rs_out;
  Register<1> revert;
  Register<32> rs_pos;
  Register<32> rs_value;
  Register<32> rs_ind;
  Register<1> rs_update;
  Register<1> memory_out;
  Register<1> memory_jump;
  Register<32> memory_predict;
  Register<32> memory_pc;
  Register<1> lsb_out;
  Register<32> lsb_dest;

  Register<1> alu_out;
  Register<32> operand_1;
  Register<32> operand_2;
  Register<32> alu_op;
  Register<32> alu_index;
};

struct RobModule : dark::Module<RobInput, RobOutput> {
  uint32_t pos;

  // ROB table
  bool busy[ROB_MAX] = {};
  bool ready[ROB_MAX] = {};
  int32_t rs_inds[ROB_MAX] = {};
  int32_t dests[ROB_MAX] = {};
  int32_t ops[ROB_MAX] = {};
  int32_t pcs[ROB_MAX] = {};
  int32_t rs1s[ROB_MAX] = {};
  int32_t rs2s[ROB_MAX] = {};
  int32_t as[ROB_MAX] = {};
  int32_t value[ROB_MAX] = {};
  bool jumps[ROB_MAX] = {};

  void work() override final;
  bool exec(max_size_t head);
  void append(max_size_t index);
};
