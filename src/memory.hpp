#include "module.h"
#include "register.h"
#include "tools.h"
#include <cstdint>

const int MEM_MAX = 0x11000;
struct MemoryInput {
  Wire<1> rs_available;
};

struct MemoryOutput {
  Register<32> pc;
  Register<32> old_pc;
  Register<1> out;
  Register<32> op;
  Register<32> rd;
  Register<32> rs1;
  Register<32> rs2;
  Register<32> a;
  Register<1> rs_out;
  Register<1> jump;
};

struct MemoryModule : dark::Module<MemoryInput, MemoryOutput> {
  uint32_t memory[MEM_MAX]{};
  bool jump[MEM_MAX]{};
  
  void work() override final;
  void load();
  void decode_and_issue();
};