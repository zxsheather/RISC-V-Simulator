#pragma once
#include "module.h"
#include "register.h"
#include "tools.h"
#include <cstdint>
#include "memory.hpp"


struct LSBInput {
  Wire<1> revert;
  Wire<1> rob_in;
  Wire<32> commit_index;

  Wire<1> in;
  Wire<32> op_rs;
  Wire<32> rs1_rs;
  Wire<32> rs2_rs;
  Wire<32> a_rs;
  Wire<32> dest_rs;
  Wire<32> pos_rs;
};

struct LSBOutput {
  Register<1> rob_out;
  Register<32> value;
  Register<32> rob_dest;
};


struct LSBModule : dark::Module<LSBInput, LSBOutput> {
  LSBModule(MemoryModule& mem_module_): mem_module(mem_module_){};
  void work() override final;
  void exec(uint32_t pos);
  void append(uint32_t pos);

  uint32_t ticker{}; 
  int32_t commit_pos = -1;
  int32_t head{};
  bool busy[LSB_MAX]{};
  uint32_t ops[LSB_MAX]{};
  uint32_t dests[LSB_MAX]{};
  uint32_t address[LSB_MAX]{};
  uint32_t data[LSB_MAX]{};

  MemoryModule& mem_module;
};