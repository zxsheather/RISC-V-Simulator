#include "decoder.hpp"
#include "bit.h"
#include "bit_impl.h"
#include "opcode.hpp"
#include "tools.h"


void DecoderModule::work() {
  Bit<32> inst = instruction;
  Bit<7> opcode = inst.range<6, 0>();

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
      std::cerr << "ALU ADD" << std::endl;
      alu_op <= static_cast<max_size_t>(Opcode::ADD);
      break;
    }
    // SUB
    case 0b0100000: {
      std::cerr << "ALU SUB" << std::endl;
      alu_op <= static_cast<max_size_t>(Opcode::SUB);
      break;
    }
    }
    break;
  }

  // SLL
  case 0b001: {
    std::cerr << "ALU SLL" << std::endl;
    alu_op <= static_cast<max_size_t>(Opcode::SLL);
    break;
  }

  // SLT
  case 0b010: {
    std::cerr << "ALU SLT" << std::endl;
    alu_op <= static_cast<max_size_t>(Opcode::SLT);
    break;
  }

  // SLTU
  case 0b011: {
    std::cerr << "ALU SLTU" << std::endl;
    alu_op <= static_cast<max_size_t>(Opcode::SLTU);
    break;
  }

  // XOR
  case 0b100: {
    std::cerr << "ALU XOR" << std::endl;
    alu_op <= static_cast<max_size_t>(Opcode::XOR);
    break;
  }

  // SRL or SRA
  case 0b101: {
    switch (f7) {
    // SRL
    case 0b0000000: {
      std::cerr << "ALU SRL" << std::endl;
      alu_op <= static_cast<max_size_t>(Opcode::SRL);
      break;
    }
    // SRA
    case 0b0100000: {
      std::cerr << "ALU SRA" << std::endl;
      alu_op <= static_cast<max_size_t>(Opcode::SRA);
      break;
    }
    }
    break;
  }

  // OR
  case 0b110: {
    std::cerr << "ALU OR" << std::endl;
    alu_op <= static_cast<max_size_t>(Opcode::OR);
    break;
  }

  // AND
  case 0b111: {
    std::cerr << "ALU AND" << std::endl;
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
    std::cerr << "ALU ADDI" << std::endl;
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::ADD);
    break;
  }

  // SLTI
  case 0b010: {
    std::cerr << "ALU SLTI" << std::endl;
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::SLT);
    break;
  }

  // SLTIU
  case 0b011: {
    std::cerr << "ALU SLTIU" << std::endl;
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::SLTU);
    break;
  }

  // XORI
  case 0b100: {
    std::cerr << "ALU XORI" << std::endl;
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::XOR);
    break;
  }

  // ORI
  case 0b110: {
    std::cerr << "ALU ORI" << std::endl;
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::OR);
    break;
  }

  // ANDI
  case 0b111: {
    std::cerr << "ALU ANDI" << std::endl;
    immediate <= dark::sign_extend<32>(inst.range<31, 20>());
    alu_op <= static_cast<max_size_t>(Opcode::AND);
    break;
  }

  // SLLI
  case 0b001: {
    std::cerr << "ALU SLLI" << std::endl;
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
      std::cerr << "ALU SRLI" << std::endl;
      alu_op <= static_cast<max_size_t>(Opcode::SRL);
      break;
    }

    // SRAI
    case 0b0100000: {
      std::cerr << "ALU SRAI" << std::endl;
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
  immediate <= dark::sign_extend<32>(imm12);
  alu_src <= 1;
  alu_enable <= 1;
  alu_op <= static_cast<max_size_t>(Opcode::ADD);
  branch <= 0;
  mem_op <= 1;
  reg_write <= 1;

  max_size_t f3 = to_unsigned(inst.range<14, 12>());
  switch (f3) {
  // LB
  case 0b000: {
    std::cerr << "LB" << std::endl;
    mem_width <= 0;
    mem_unsigned <= 0;
    break;
  }
  // LH
  case 0b001: {
    std::cerr << "LH" << std::endl;
    mem_width <= 1;
    mem_unsigned <= 0;
    break;
  }

  // LW
  case 0b010: {
    std::cerr << "LW" << std::endl;
    mem_width <= 2;
    mem_unsigned <= 0;
    break;
  }

  // LBU
  case 0b100: {
    std::cerr << "LBU" << std::endl;
    mem_width <= 0;
    mem_unsigned <= 1;
    break;
  }

  // LHU
  case 0b101: {
    std::cerr << "LHU" << std::endl;
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
  mem_op <= 2;
  reg_write <= 0;
  switch (f3) {
  // SB
  case 0b000:{
    std::cerr << "SB" << std::endl;
    mem_width <= 0;
    break;
  }

  case 0b001:{
    std::cerr << "SH" << std::endl;
    mem_width <= 1;
    break;
  }

  case 0b010: {
    std::cerr << "SW" << std::endl;
    mem_width <= 2;
    break;
  }
  }

  
}

void DecoderModule::decode_lui(const Bit<32> &inst){
  std::cerr << "LUI" << std::endl;
  rd_index <= inst.range<11, 7>();
  immediate <= (dark::zero_extend(inst.range<31, 12>()) << 12);
  rs1_index <= 0;
  rs2_index <= 0;
  alu_enable <= 0;
  mem_op <= 0;
  reg_write <= 1;
}

void DecoderModule::decode_auipc(const Bit<32> &inst) {
  std::cerr << "AUIPC" << std::endl;
  rd_index <= inst.range<11, 7>();
  immediate <= (dark::zero_extend(inst.range<31, 12>()) << 12);
  rs1_index <= 0;
  rs2_index <= 0;

  alu_enable <= 1;
  alu_op <= static_cast<max_size_t>(Opcode::ADD);
  alu_src <= 2;
  branch <= 0;
  reg_write <= 1;
}

void DecoderModule::decode_jal(const Bit<32> &inst) {
  std::cerr << "JAL" << std::endl;
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
  reg_write <= 1;
  jump_link <= 1;
}

void DecoderModule::decode_jalr(const Bit<32> &inst) {
  std::cerr << "JALR" << std::endl;
  rd_index <= inst.range<11, 7>();
  rs1_index <= inst.range<19, 15>();
  rs2_index <= 0;

  immediate <= dark::sign_extend<32>(inst.range<31, 20>());

  alu_enable <= 1;
  branch <= 0;
  alu_op <= static_cast<max_size_t>(Opcode::ADD);
  alu_src <= 1;
  branch <= 0;
  reg_write <= 1;
  jump_link <= 2;
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
  branch_op <= inst.range<14, 12>();
  jump_link <= 0;
  reg_write <= 0;
}