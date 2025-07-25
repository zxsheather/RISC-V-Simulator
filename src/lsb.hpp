#pragma once
#include "module.h"
#include "tools.h"
#include <cstdint>

const max_size_t LSB_MAX = 120;

struct LSBInput {
  Wire<1> in;
  Wire<32> op;
  Wire<32> rs1;
  Wire<32> rs2;
  Wire<32> a;
  Wire<32> pos_rs;
};

struct LSBOutput {
  
};

struct LSBModule : dark::Module<LSBInput, LSBOutput> {
  void work() override final;
  void exec(uint32_t pos);
  void append(uint32_t pos);

  uint32_t ticker{}; 
  uint32_t commit_pos{};
  uint32_t head{};
  bool busy[LSB_MAX]{};
  uint32_t op[LSB_MAX]{};

};