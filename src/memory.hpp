#pragma once
#include "module.h"
#include "predictor.hpp"
#include "tools.h"
#include "util.hpp"
#include <cstdint>

struct BranchPredictor;
struct MemoryModule;
struct TwoBitPredictor;
struct AlwaysFalsePredictor;

struct MemoryModule;
struct MemoryInput {
  Wire<1> rs_available;
  Wire<1> rob_in;
  Wire<1> is_jump;
  Wire<32> rob_pc;
  Wire<32> predict_pc;
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
  Register<32> jump;
};

struct MemoryModule : dark::Module<MemoryInput, MemoryOutput> {
  uint8_t memory[MEM_MAX]{};
  BranchPredictor *predictor;

  void work() override final;
  void load();
  void decode_and_issue(uint32_t pc);
  void decode_i_arith(const Bit<32> &inst, uint32_t pc_);
  void decode_load(const Bit<32> &inst, uint32_t pc_);
  void decode_r_arith(const Bit<32> &inst, uint32_t pc_);
  void decode_b_type(const Bit<32> &inst, uint32_t pc_);
  void decode_s_type(const Bit<32> &inst, uint32_t pc_);
  void decode_lui(const Bit<32> &inst, uint32_t pc_);
  void decode_auipc(const Bit<32> &inst, uint32_t pc_);
  void decode_jal(const Bit<32> &inst, uint32_t pc_);
  void decode_jalr(const Bit<32> &inst, uint32_t pc_);
  Bit<32> read_memory(uint32_t addr, uint32_t width, bool is_unsigned);
  void write_memory(uint32_t addr, uint32_t width, uint32_t data);
  MemoryModule(BranchPredictor *predictor_) : predictor(predictor_) {}
  ~MemoryModule();
};