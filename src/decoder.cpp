#include "decoder.hpp"
#include "tools.h"
#include "opcode.hpp"

void DecoderModule::work() {
  Bit<32> inst = instruction;
  opcode <= inst.range<6, 0>();
  rd_index <= inst.range<11, 7>();
  funct3 <= inst.range<14, 12>();
  rs1_index <= inst.range<19, 15>();
  rs2_index <= inst.range<24, 20>();
  funct7 <= inst.range<31, 25>();

  switch (to_unsigned(opcode)) {
  case 0b0110011: {
    decode_r_type(inst);
    break;
  }

  case 0b0010011: {
    decode_i_type(inst);
    break;
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

void DecoderModule::decode_i_type(const Bit<32>& inst) {
  immediate <= 0;
  alu_src <= 0;
  reg_write <= 1;
  branch <= 0;

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