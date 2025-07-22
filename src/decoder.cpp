#include "decoder.hpp"
#include "bit_impl.h"
#include "concept.h"
#include "opcode.hpp"
#include "tools.h"

void DecoderModule::work() {
  Bit<32> inst = instruction;
  opcode <= inst.range<6, 0>();

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
  }

  case 0b0100000: {
    decode_b_type(inst);
    break;
  }

  case 0b0100011: {
    decode_s_type(inst);
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

  case 0b101: {
    mem_width <= 1;
    mem_unsigned <= 1;
    break;
  }
  }
}