#include "concept.h"
#include "module.h"
#include "tools.h"
#include <cstdint>

const max_size_t ROB_MAX = 200;

struct ExecuterInput {
  Wire<1> in;
  Wire<32> op_rs;
  Wire<32> rs1_rs;
  Wire<32> rs2_rs;
  Wire<32> dest_rs;
  Wire<32> value_rs;
  Wire<1> ready_rs;
};

struct ExecuterOutput {
  Register<1> out_rs;
  Register<32> pos_rs;
  Register<32> value_rs;
  Register<1> memory_jump;
  Register<32> pc;
};

struct ExecuterModule : dark::Module<ExecuterInput, ExecuterOutput> {
  uint32_t pos;

  // ROB table
  bool busy[ROB_MAX] = {};
  bool ready[ROB_MAX] = {};
  int32_t dest[ROB_MAX] = {};
  int32_t op[ROB_MAX] = {};

  void work() override final;
  void exec(max_size_t pos);
  void append(max_size_t pos);
};
