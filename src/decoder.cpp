#include "decoder.hpp"
#include "bit.h"
#include "bit_impl.h"
#include "concept.h"
#include "opcode.hpp"
#include "tools.h"

void DecoderModule::init() {
  rd_index <= 0;
  rs1_index <= 0;
  rs2_index <= 0;
  immediate <= 0;
  opcode <= 0;
  alu_op <= 0;
  alu_enable <= 0;
  alu_src <= 0;
  branch <= 0;
  branch_op <= 0;
  mem_read <= 0;
  mem_write <= 0;
  reg_write <= 0;
  mem_width <= 0; 
  mem_unsigned <= 0; 
  pc_to_alu <= 0;
  jump <= 0;
  link <= 0;
  jalr <= 0;
}

void DecoderModule::work() {
  Bit<32> inst = instruction;
  opcode <= inst.range<6, 0>();
  init();

  switch (to_unsigned(opcode)) {
  case 0b0110011: {
    decode_r_arith(inst);
    break;
  }

  case 0b0010011: {
    decode_i_arith(inst);
    break;
  }

  case 0b0000011: {
    decode_load(inst);
    break;
  }

  case 0b1100011: {
    decode_b_type(inst);
    break;
  }

  case 0b0100011: {
    decode_s_type(inst);
    break;
  }

  case 0b0110111: {
    decode_lui(inst);
    break;
  }

  case 0b0010111: {
    decode_auipc(inst);
    break;
  }

  case 0b1101111: {
    decode_jal(inst);
    break;
  }

  case 0b1100111: {
    decode_jalr(inst);
    break;
  }
  }
}

void DecoderModule::decode_r_arith(const Bit<32> &inst) {
  rd_index <= inst.range<11, 7>();
  rs1_index <= inst.range<19, 15>();
  rs2_index <= inst.range<24, 20>();
  immediate <= 0;
  alu_src <= 0;
  alu_enable <= 1;
  branch <= 0;
  reg_write <= 1;

  max_size_t f3 = to_unsigned(inst.range<14, 12>());
  max_size_t f7 = to_unsigned(inst.range<31, 25>());
  switch (f3) {
  // ADD or SUB
  case 0b000: {
    switch (f7) {
    // ADD
    case 0b0000000: {
      alu_op <= static_cast<max_size_t>(Opcode::ADD);
      break;
    }
    // SUB
    case 0b0100000: {
      alu_op <= static_cast<max_size_t>(Opcode::SUB);
      break;
    }
    }
    break;
  }

  // SLL
  case 0b001: {
    alu_op <= static_cast<max_size_t>(Opcode::SLL);
    break;
  }

  // SLT
  case 0b010: {
    alu_op <= static_cast<max_size_t>(Opcode::SLT);
    break;
  }

  // SLTU
  case 0b011: {
    alu_op <= static_cast<max_size_t>(Opcode::SLTU);
    break;
  }

  // XOR
  case 0b100: {
    alu_op <= static_cast<max_size_t>(Opcode::XOR);
    break;
  }

  // SRL or SRA
  case 0b101: {
    switch (f7) {
    // SRL
    case 0b0000000: {
      alu_op <= static_cast<max_size_t>(Opcode::SRL);
      break;
    }
    // SRA
    case 0b0100000: {
      alu_op <= static_cast<max_size_t>(Opcode::SRA);
      break;
    }
    }
    break;
  }

  // OR
  case 0b110: {
    alu_op <= static_cast<max_size_t>(Opcode::OR);
    break;
  }

  // AND
  case 0b111: {
    alu_op <= static_cast<max_size_t>(Opcode::AND);
    break;
  }
  }
}

void DecoderModule::decode_i_arith(const Bit<32> &inst) {
  rd_index <= inst.range<11, 7>();
  rs1_index <= inst.range<19, 15>();
  rs2_index <= 0;
  alu_src <= 1;
  alu_enable <= 1;
  reg_write <= 1;
  branch <= 0;

  max_size_t f3 = to_unsigned(inst.range<14, 12>());
  switch (f3) {
  // ADDI
  case 0b000: {
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::ADD);
    break;
  }

  // SLTI
  case 0b010: {
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::SLT);
    break;
  }

  // SLTIU
  case 0b011: {
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::SLTU);
    break;
  }

  // XORI
  case 0b100: {
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::XOR);
    break;
  }

  // ORI
  case 0b110: {
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::OR);
    break;
  }

  // ANDI
  case 0b111: {
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::AND);
    break;
  }

  // SLLI
  case 0b001: {
    immediate <= dark::sign_extend<32>(inst.range<24, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::SLL);
    break;
  }

  // SRLI or SRAI
  case 0b101: {
    max_size_t f7 = to_unsigned(inst.range<31, 25>());
    immediate <= dark::sign_extend<32>(inst.range<24, 20>());
    switch (f7) {
    // SRLI
    case 0b0000000: {
      alu_op <= static_cast<max_size_t>(Opcode::SRL);
      break;
    }

    // SRAI
    case 0b0100000: {
      alu_op <= static_cast<max_size_t>(Opcode::SRA);
      break;
    }
    }
  }
  }
}

void DecoderModule::decode_load(const Bit<32> &inst) {
  rd_index <= inst.range<11, 7>();
  rs1_index <= inst.range<19, 15>();
  rs2_index <= 0;

  Bit<12> imm12 = inst.range<31, 20>();
  immediate <= dark::sign_extend<32>(inst.range<31, 25>());
  alu_src <= 1;
  alu_enable <= 1;
  alu_op <= static_cast<max_size_t>(Opcode::ADD);
  branch <= 0;
  mem_read <= 1;
  mem_write <= 0;
  reg_write <= 1;

  max_size_t f3 = to_unsigned(inst.range<14, 12>());
  switch (f3) {
  // LB
  case 0b000: {
    mem_width <= 0;
    mem_unsigned <= 0;
    break;
  }
  // LH
  case 0b001: {
    mem_width <= 1;
    mem_unsigned <= 0;
    break;
  }

  // LW
  case 0b010: {
    mem_width <= 2;
    mem_unsigned <= 0;
    break;
  }

  // LBU
  case 0b100: {
    mem_width <= 0;
    mem_unsigned <= 1;
    break;
  }

  // LHU
  case 0b101: {
    mem_width <= 1;
    mem_unsigned <= 1;
    break;
  }
  }
}

void DecoderModule::decode_s_type(const Bit<32>& inst) {
  rs1_index <= inst.range<19, 15>();
  rs2_index <= inst.range<24, 20>();
  Bit<5> imm1 = inst.range<11, 7>();
  Bit<7> imm2 = inst.range<31, 25>();
  immediate <= dark::sign_extend<32>((dark::zero_extend<12>(imm1) | (dark::zero_extend<12>(imm2) << 5)));
  max_size_t f3 = to_unsigned(inst.range<11, 7>());
  alu_src <= 1;
  alu_enable <= 1;
  alu_op <= static_cast<max_size_t>(Opcode::ADD);
  branch <= 0;
  mem_read <= 0;
  mem_write <= 1;
  reg_write <= 0;
  switch (f3) {
  // SB
  case 0b000:{
    mem_width <= 0;
    break;
  }

  case 0b001:{
    mem_width <= 1;
    break;
  }

  case 0b010: {
    mem_width <= 2;
    break;
  }
  }

  
}

void DecoderModule::decode_lui(const Bit<32> &inst){
  rd_index <= inst.range<11, 7>();
  immediate <= (dark::zero_extend(inst.range<31, 12>()) << 12);
  rs1_index <= 0;
  rs2_index <= 0;
  alu_enable <= 0;
  mem_read <= 0;
  mem_write <= 0;
  reg_write <= 1;
}

void DecoderModule::decode_auipc(const Bit<32> &inst) {
  rd_index <= inst.range<11, 7>();
  immediate <= (dark::zero_extend(inst.range<31, 12>()) << 12);
  rs1_index <= 0;
  rs2_index <= 0;

  alu_enable <= 1;
  alu_op <= static_cast<max_size_t>(Opcode::ADD);
  alu_src <= 1;
  branch <= 0;
  mem_read <= 0;
  mem_write <= 0;
  reg_write <= 1;
  pc_to_alu <= 1;
}

void DecoderModule::decode_jal(const Bit<32> &inst) {
  rd_index <= inst.range<11, 7>();
  rs1_index <= 0;  
  rs2_index <= 0; 
  
  Bit<1> imm20 = inst.range<31, 31>();
  Bit<10> imm10_1 = inst.range<30, 21>();
  Bit<1> imm11 = inst.range<20, 20>();
  Bit<8> imm19_12 = inst.range<19, 12>();

  Bit<21> offset = {imm20, imm19_12, imm11, imm10_1, Bit<1>()};
  immediate <= dark::sign_extend<32>(offset);
  alu_enable <= 1;
  alu_src <= 1;
  alu_op <= static_cast<max_size_t>(Opcode::ADD);
  branch <= 1;
  jump <= 1;
  link <= 1;
  jalr <= 0;
}

void DecoderModule::decode_jalr(const Bit<32> &inst) {
  rd_index <= inst.range<11, 7>();
  rs1_index <= inst.range<19, 15>();
  rs2_index <= 0;

  immediate <= dark::sign_extend<32>(inst.range<31, 20>());

  alu_enable <= 1;
  branch <= 0;
  alu_op <= static_cast<max_size_t>(Opcode::ADD);
  alu_src <= 1;
  branch <= 0;
  mem_read <= 0;
  reg_write <= 1;
  jump <= 1;
  link <= 1;
  jalr <= 1;
}

void DecoderModule::decode_b_type(const Bit<32> &inst) {
  rs1_index <= inst.range<19, 15>();
  rs2_index <= inst.range<24, 20>();
  rd_index <= 0;
  Bit<1> imm12 = inst.range<31, 31>();
  Bit<1> imm11 = inst.range<7, 7>();
  Bit<6> imm10_5 = inst.range<30, 25>();
  Bit<4> imm4_1 = inst.range<11, 8>();

  Bit<13> offset = {imm12, imm11, imm10_5, imm4_1, Bit<1>()};
  immediate <= to_signed(offset);

  alu_enable <= 0;
  alu_src <= 0;
  branch <= 1;
  mem_read <= 0;
  reg_write <= 0;
  branch_op <= inst.range<14, 12>();
}