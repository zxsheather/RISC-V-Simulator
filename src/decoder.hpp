#pragma once
#include "module.h"
#include "tools.h"

struct DecoderInput {
  Wire<32> instruction;
};

struct DecoderOutput {
  Register<5> rs1_index;
  Register<5> rs2_index;
  Register<5> rd_index;
  Register<32> immediate;
  Register<7> opcode;
  Register<8> alu_op;
  Register<1> alu_enable;
  Register<1> alu_src;
  Register<1> branch;

  Register<1> mem_read;
  Register<1> mem_write;
  Register<1> reg_write;
  Register<2> mem_width; // 0: Byte 1: Harfword 2: Word
  Register<1> mem_unsigned;
};

struct DecoderModule : dark::Module<DecoderInput, DecoderOutput> {
  void work() override;
  void decode_i_arith(const Bit<32> &inst);
  void decode_load(const Bit<32> &inst);
  void decode_r_arith(const Bit<32> &inst);
  void decode_b_type(const Bit<32> &inst);
  void decode_s_type(const Bit<32> &inst);
  void decode_u_type(const Bit<32> &inst);
  void decode_jalr(const Bit<32> &inst);
};
