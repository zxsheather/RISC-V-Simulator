#include "memory.hpp"
#include "predictor.hpp"
#include "tools.h"
#include "util.hpp"
#include <cstdint>
#include <sstream>

void MemoryModule::load() {
  std::string line;
  max_size_t cur = 0;
  while (std::getline(std::cin, line)) {
    if (line.empty())
      continue;
    if (line[0] == '@') {
      std::string addr_str = line.substr(1);
      cur = std::stoul(addr_str, nullptr, 16);
      continue;
    }

    std::istringstream iss(line);
    std::string byte_str;

    while (iss >> byte_str) {
      memory[cur++] = std::stoul(byte_str, nullptr, 16);
    }
  }
}

void MemoryModule::work() {
  uint32_t updated_now_pc = to_unsigned(pc);
  if (rob_in) {
    if (!predictor->refresh_predictor(this)) {
      // std::cerr << "Predictor: Prediction Fail. Jump to "
      //           << to_unsigned(rob_pc) << std::endl;
      updated_now_pc = to_unsigned(rob_pc);
    }
  }
  old_pc <= updated_now_pc;
  if (rs_available && updated_now_pc < MEM_MAX) {
    out <= 1;
    // std::cerr << "Memory: Issue PC " << std::hex << std::setw(8)
    //           << std::setfill('0') << updated_now_pc << std::dec <<
    //           std::endl;
    decode_and_issue(updated_now_pc);
  } else {
    pc <= updated_now_pc;
    out <= 0;
  }
}

Bit<32> MemoryModule::read_memory(uint32_t addr, uint32_t width,
                                  bool is_unsigned) {
  Bit<32> rd_data;
  addr %= 0x10000;
  switch (width) {
  case 0: {
    Bit<8> b = memory[addr];
    if (is_unsigned) {
      rd_data = to_unsigned(b);
    } else {
      rd_data = to_signed(b);
    }
    break;
  }
  case 1: {
    Bit<8> b1 = memory[addr];
    Bit<8> b2 = memory[addr + 1];
    Bit<16> b = {b2, b1};
    if (is_unsigned) {
      rd_data = to_unsigned(b);
    } else {
      rd_data = to_signed(b);
    }
    break;
  }
  case 2: {
    Bit<8> b1 = memory[addr];
    Bit<8> b2 = memory[addr + 1];
    Bit<8> b3 = memory[addr + 2];
    Bit<8> b4 = memory[addr + 3];
    Bit<32> b = {b4, b3, b2, b1};
    rd_data = b;
    break;
  }
  }
  return rd_data;
}

void MemoryModule::write_memory(uint32_t addr, uint32_t width, uint32_t data) {
  addr %= 0x10000;
  switch (width) {
  case 0: {
    memory[addr] = data;
    break;
  }
  case 1: {
    memory[addr] = data & 0xFF;
    memory[addr + 1] = (data >> 8) & 0xFF;
    break;
  }
  case 2: {
    memory[addr] = data & 0xFF;
    memory[addr + 1] = (data >> 8) & 0xFF;
    memory[addr + 2] = (data >> 16) & 0xFF;
    memory[addr + 3] = (data >> 24) & 0xFF;
    break;
  }
  }
}

void MemoryModule::decode_and_issue(uint32_t pc_) {
  Bit<32> inst = read_memory(pc_, 2, false);
  uint32_t opcode = to_unsigned(inst.range<6, 0>());
  uint32_t rs1_ = to_unsigned(inst.range<19, 15>());
  uint32_t rs2_ = to_unsigned(inst.range<24, 20>());
  uint32_t rd_ = to_unsigned(inst.range<11, 7>());
  bool src1 = 1, src2 = 1, userd = 1;

  switch (opcode) {
  case 0b0110011: {
    decode_r_arith(inst, pc_);
    break;
  }

  case 0b0010011: {
    decode_i_arith(inst, pc_);
    src2 = 0;
    break;
  }

  case 0b0000011: {
    decode_load(inst, pc_);
    src2 = 0;
    break;
  }

  case 0b1100011: {
    decode_b_type(inst, pc_);
    userd = 0;
    break;
  }

  case 0b0100011: {
    decode_s_type(inst, pc_);
    break;
  }

  case 0b0110111: {
    decode_lui(inst, pc_);
    break;
  }

  case 0b0010111: {
    decode_auipc(inst, pc_);
    break;
  }

  case 0b1101111: {
    // std::cerr << "JAL" << std::endl;
    decode_jal(inst, pc_);
    src1 = 0;
    src2 = 0;
    break;
  }

  case 0b1100111: {
    decode_jalr(inst, pc_);
    src2 = 0;
    break;
  }

  default: {
    // std::cerr << "Unknown opcode: " << std::hex << std::setw(8)
    //           << std::setfill('0') << opcode << std::dec << std::endl;
    op <= static_cast<max_size_t>(Opcode::UNKNOWN);
    src1 = 0;
    src2 = 0;
    userd = 0;
    pc <= pc_ + 4;
  }
  }

  if (src1) {
    rs1 <= rs1_;
  } else {
    rs1 <= 32;
  }

  if (src2) {
    rs2 <= rs2_;
  } else {
    rs2 <= 32;
  }

  if (userd) {
    rd <= rd_;
  } else {
    rd <= 32;
  }
};

void MemoryModule::decode_r_arith(const Bit<32> &inst, uint32_t pc_) {
  uint32_t f3 = to_unsigned(inst.range<14, 12>());
  uint32_t f7 = to_unsigned(inst.range<31, 25>());
  pc <= pc_ + 4;
  switch (f3) {
  // ADD or SUB
  case 0b000: {
    switch (f7) {
    // ADD
    case 0b0000000: {
      // std::cerr << "ALU ADD" << std::endl;
      op <= static_cast<max_size_t>(Opcode::ADD);
      break;
    }
    // SUB
    case 0b0100000: {
      // std::cerr << "ALU SUB" << std::endl;
      op <= static_cast<max_size_t>(Opcode::SUB);
      break;
    }
    }
    break;
  }

  // SLL
  case 0b001: {
    // std::cerr << "ALU SLL" << std::endl;
    op <= static_cast<max_size_t>(Opcode::SLL);
    break;
  }

  // SLT
  case 0b010: {
    // std::cerr << "ALU SLT" << std::endl;
    op <= static_cast<max_size_t>(Opcode::SLT);
    break;
  }

  // SLTU
  case 0b011: {
    // std::cerr << "ALU SLTU" << std::endl;
    op <= static_cast<max_size_t>(Opcode::SLTU);
    break;
  }

  // XOR
  case 0b100: {
    // std::cerr << "ALU XOR" << std::endl;
    op <= static_cast<max_size_t>(Opcode::XOR);
    break;
  }

  // SRL or SRA
  case 0b101: {
    switch (f7) {
    // SRL
    case 0b0000000: {
      // std::cerr << "ALU SRL" << std::endl;
      op <= static_cast<max_size_t>(Opcode::SRL);
      break;
    }
    // SRA
    case 0b0100000: {
      // std::cerr << "ALU SRA" << std::endl;
      op <= static_cast<max_size_t>(Opcode::SRA);
      break;
    }
    }
    break;
  }

  // OR
  case 0b110: {
    // std::cerr << "ALU OR" << std::endl;
    op <= static_cast<max_size_t>(Opcode::OR);
    break;
  }

  // AND
  case 0b111: {
    // std::cerr << "ALU AND" << std::endl;
    op <= static_cast<max_size_t>(Opcode::AND);
    break;
  }
  }
}

void MemoryModule::decode_i_arith(const Bit<32> &inst, uint32_t pc_) {
  uint32_t f3 = to_unsigned(inst.range<14, 12>());
  pc <= pc_ + 4;

  switch (f3) {
  // ADDI
  case 0b000: {
    // std::cerr << "ALU ADDI" << std::endl;
    a <= dark::sign_extend<32>(inst.range<31, 20>());
    op <= static_cast<max_size_t>(Opcode::ADDI);
    break;
  }

  // SLTI
  case 0b010: {
    // std::cerr << "ALU SLTI" << std::endl;
    a <= dark::sign_extend<32>(inst.range<31, 20>());
    op <= static_cast<max_size_t>(Opcode::SLTI);
    break;
  }

  // SLTIU
  case 0b011: {
    // std::cerr << "ALU SLTIU" << std::endl;
    a <= dark::sign_extend<32>(inst.range<31, 20>());
    op <= static_cast<max_size_t>(Opcode::SLTIU);
    break;
  }

  // XORI
  case 0b100: {
    // std::cerr << "ALU XORI" << std::endl;
    a <= dark::sign_extend<32>(inst.range<31, 20>());
    op <= static_cast<max_size_t>(Opcode::XORI);
    break;
  }

  // ORI
  case 0b110: {
    // std::cerr << "ALU ORI" << std::endl;
    a <= dark::sign_extend<32>(inst.range<31, 20>());
    op <= static_cast<max_size_t>(Opcode::ORI);
    break;
  }

  // ANDI
  case 0b111: {
    // std::cerr << "ALU ANDI" << std::endl;
    a <= dark::sign_extend<32>(inst.range<31, 20>());
    op <= static_cast<max_size_t>(Opcode::ANDI);
    break;
  }

  // SLLI
  case 0b001: {
    // std::cerr << "ALU SLLI" << std::endl;
    a <= dark::sign_extend<32>(inst.range<24, 20>());
    op <= static_cast<max_size_t>(Opcode::SLLI);
    break;
  }

  // SRLI or SRAI
  case 0b101: {
    max_size_t f7 = to_unsigned(inst.range<31, 25>());
    a <= dark::sign_extend<32>(inst.range<24, 20>());
    switch (f7) {
    // SRLI
    case 0b0000000: {
      // std::cerr << "ALU SRLI" << std::endl;
      op <= static_cast<max_size_t>(Opcode::SRLI);
      break;
    }

    // SRAI
    case 0b0100000: {
      // std::cerr << "ALU SRAI" << std::endl;
      op <= static_cast<max_size_t>(Opcode::SRAI);
      break;
    }
    }
  }
  }
}

void MemoryModule::decode_load(const Bit<32> &inst, uint32_t pc_) {
  uint32_t f3 = to_unsigned(inst.range<14, 12>());
  pc <= pc_ + 4;
  a <= dark::sign_extend<32>(inst.range<31, 20>());
  switch (f3) {

  case 0b000: {
    // std::cerr << "LB" << std::endl;
    op <= static_cast<max_size_t>(Opcode::LB);
  }

  case 0b001: {
    // std::cerr << "LH" << std::endl;
    op <= static_cast<max_size_t>(Opcode::LH);
    break;
  }

  case 0b010: {
    // std::cerr << "LW" << std::endl;
    op <= static_cast<max_size_t>(Opcode::LW);
    break;
  }

  case 0b100: {
    // std::cerr << "LBU" << std::endl;
    op <= static_cast<max_size_t>(Opcode::LBU);
    break;
  }

  case 0b101: {
    // std::cerr << "LHU" << std::endl;
    op <= static_cast<max_size_t>(Opcode::LHU);
    break;
  }
  }
}

void MemoryModule::decode_b_type(const Bit<32> &inst, uint32_t pc_) {
  max_size_t f3 = to_unsigned(inst.range<14, 12>());
  Bit<13> imm = {inst.range<31, 31>(), inst.range<7, 7>(), inst.range<30, 25>(),
                 inst.range<11, 8>(), Bit<1>()};
  int32_t imm_data = to_signed(imm);
  a <= imm_data;
  if (predictor->opt(this, pc_)) {
    pc <= pc_ + imm_data;
    predictor->update_jump(pc_, true);
    jump <= 1;
  } else {
    pc <= pc_ + 4;
    predictor->update_jump(pc_, false);
    jump <= 0;
  }
  switch (f3) {
  case 0b000: {
    // std::cerr << "BEQ" << std::endl;
    op <= static_cast<max_size_t>(Opcode::BEQ);
    break;
  }

  case 0b001: {
    // std::cerr << "BNE" << std::endl;
    op <= static_cast<max_size_t>(Opcode::BNE);
    break;
  }

  case 0b100: {
    // std::cerr << "BLT" << std::endl;
    op <= static_cast<max_size_t>(Opcode::BLT);
    break;
  }

  case 0b101: {
    // std::cerr << "BGE" << std::endl;
    op <= static_cast<max_size_t>(Opcode::BGE);
    break;
  }

  case 0b110: {
    // std::cerr << "BLTU" << std::endl;
    op <= static_cast<max_size_t>(Opcode::BLTU);
    break;
  }

  case 0b111: {
    // std::cerr << "BGEU" << std::endl;
    op <= static_cast<max_size_t>(Opcode::BGEU);
    break;
  }
  }
}

void MemoryModule::decode_s_type(const Bit<32> &inst, uint32_t pc_) {
  max_size_t f3 = to_unsigned(inst.range<14, 12>());
  Bit<12> imm = {inst.range<31, 25>(), inst.range<11, 7>()};
  a <= to_signed(imm);
  pc <= pc_ + 4;
  switch (f3) {
  case 0b000: {
    // std::cerr << "SB" << std::endl;
    op <= static_cast<max_size_t>(Opcode::SB);
    break;
  }

  case 0b001: {
    // std::cerr << "SH" << std::endl;
    op <= static_cast<max_size_t>(Opcode::SH);
    break;
  }

  case 0b010: {
    // std::cerr << "SW" << std::endl;
    op <= static_cast<max_size_t>(Opcode::SW);
    break;
  }
  }
}

void MemoryModule::decode_lui(const Bit<32> &inst, uint32_t pc_) {
  Bit<32> imm = {inst.range<31, 12>(), Bit<12>()};
  pc <= pc_ + 4;
  op <= static_cast<max_size_t>(Opcode::LUI);
  a <= to_signed(imm);
}

void MemoryModule::decode_auipc(const Bit<32> &inst, uint32_t pc_) {
  Bit<32> imm = {inst.range<31, 12>(), Bit<12>()};
  pc <= pc_ + 4;
  op <= static_cast<max_size_t>(Opcode::AUIPC);
  a <= to_signed(imm);
}

void MemoryModule::decode_jal(const Bit<32> &inst, uint32_t pc_) {
  Bit<21> imm = {inst.range<31, 31>(), inst.range<19, 12>(),
                 inst.range<20, 20>(), inst.range<30, 21>(), Bit<1>()};
  pc <= pc_ + to_signed(imm);
  op <= static_cast<max_size_t>(Opcode::JAL);
  a <= to_signed(imm);
}

void MemoryModule::decode_jalr(const Bit<32> &inst, uint32_t pc_) {
  Bit<12> imm = inst.range<31, 20>();
  pc <= pc_ + 4;
  jump <= 0;
  predictor->update_jump(pc_, false);
  a <= to_signed(imm);
  op <= static_cast<max_size_t>(Opcode::JALR);
}

MemoryModule::~MemoryModule() { delete predictor; }
