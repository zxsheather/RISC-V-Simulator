#pragma once
#include "module.h"
#include "register.h"
#include "tools.h"
#include <cstdint>

const int MEM_MAX = 0x11000;
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
  Register<1> jump;
};

struct BranchPredictor {
  bool jumps[MEM_MAX]{};
  virtual uint32_t total_predictions() = 0;
  virtual uint32_t correct_predictions() = 0;
  virtual bool refresh_predictor(const MemoryModule *mem) = 0;
  virtual bool opt(const MemoryModule *mem, uint32_t pc_now) {
    return true; 
  } 
  void update_jump(uint32_t pc_now, bool is_jump) {
    jumps[pc_now] = is_jump;
  }
  virtual ~BranchPredictor() = default;
};

struct TwoBitPredictor : public BranchPredictor {
  uint32_t total_predictions_ = 0;
  uint32_t correct_predictions_ = 0;
  uint32_t predict[MEM_MAX]{};
  

  uint32_t total_predictions() override;
  uint32_t correct_predictions() override;
  bool refresh_predictor(const MemoryModule *mem) override;
  bool opt(const MemoryModule *mem, uint32_t pc_now) override;
};

struct MemoryModule : dark::Module<MemoryInput, MemoryOutput> {
  friend struct TwoBitPredictor;
  uint8_t memory[MEM_MAX]{};
  BranchPredictor *predictor = new TwoBitPredictor();

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
  ~MemoryModule();
};