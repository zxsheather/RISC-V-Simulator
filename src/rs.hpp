#include "module.h"
#include "register.h"
#include "tools.h"
#include <array>

const max_size_t RS_MAX = 120;

struct RSInput {
  Wire<1> in;
  Wire<32> op;
  Wire<32> rs1;
  Wire<32> rs2;
  Wire<32> rd;
  Wire<32> a;
  Wire<32> pc;
  Wire<1> jump;

  Wire<32> rob_pos;
};

struct RSOutput {
  Register<1> memory_flag;
};

struct RSPrivate {
  Register<32> pos_in_rob;
  std::array<Register<1>, RS_MAX> busy;
  std::array<Register<32>, RS_MAX> op;
  std::array<Register<32>, RS_MAX> vj;
  std::array<Register<32>, RS_MAX> vk;
  std::array<Register<32>, RS_MAX> qj;
  std::array<Register<32>, RS_MAX> qk;
  std::array<Register<32>, RS_MAX> reorder;
  std::array<Register<32>, RS_MAX> a;
  std::array<Register<32>, RS_MAX> pcs;  // Pcs of insts. The unique identifier.
  std::array<Register<1>, RS_MAX> jumps;
  std::array<Register<1>, RS_MAX> dispatched;
  
  std::array<Register<32>, 32> regs;
};

struct RSModule : dark::Module<RSInput, RSOutput, RSPrivate> {
  void work() override final;
  void exec();
};