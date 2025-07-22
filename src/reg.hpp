#pragma once
#include "module.h"
#include "tools.h"
#include <array>

struct RegFileInput {
  Wire<5> rs1_index;
  Wire<5> rs2_index;

  Wire<5> wb_index;
  Wire<32> wb_data;
  Wire<1> wb_enable;
};

struct RegFileOutput {
  Register<32> rs1_data;
  Register<32> rs2_data;
};

struct RegFilePrivate {
  std::array< Register<32>, 32> regs;
};

struct RegFileModule : dark::Module<RegFileInput, RegFileOutput, RegFilePrivate> {
  void work() override;
};
