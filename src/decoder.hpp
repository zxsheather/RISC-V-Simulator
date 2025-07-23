#pragma once
#include "module.h"
#include "register.h"
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
  Register<3> branch_op;

  Register<1> mem_read;
  Register<1> mem_write;
  Register<1> reg_write;
  Register<2> mem_width; // 0: Byte 1: Halfword 2: Word
  Register<1> mem_unsigned;

  Register<1> pc_to_alu;
  Register<1> jump;
  Register<1> link;
  Register<1> jalr;
};

struct DecoderModule : dark::Module<DecoderInput, DecoderOutput> {
  void work() override;
  void init();
  void decode_i_arith(const Bit<32> &inst);
  void decode_load(const Bit<32> &inst);
  void decode_r_arith(const Bit<32> &inst);
  void decode_b_type(const Bit<32> &inst);
  void decode_s_type(const Bit<32> &inst);
  void decode_lui(const Bit<32> &inst);
  void decode_auipc(const Bit<32> &inst);
  void decode_jal(const Bit<32> &inst);
  void decode_jalr(const Bit<32> &inst);
};
