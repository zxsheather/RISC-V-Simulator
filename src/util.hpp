#pragma once
#include <cstdint>

const uint32_t ROB_MAX = 120;
const uint32_t RS_MAX = 120;
const uint32_t LSB_MAX = 120;
const uint32_t MEM_MAX = 0x10000;
const uint32_t MIN_AVAL = 5;
const uint32_t RD_DEFAULT = 33;
const uint32_t Q_DEFAULT = RS_MAX + 1;

enum Opcode {
  ADD,
  SUB,
  SLL,
  SRL,
  SRA,
  AND,
  OR,
  XOR,
  SLT,
  SLTU,
  ADDI,
  SLLI,
  SRLI,
  SRAI,
  ANDI,
  ORI,
  XORI,
  SLTI,
  SLTIU,
  BEQ,
  BNE,
  BLT,
  BGE,
  BLTU,
  BGEU,
  LB,
  LH,
  LW,
  LBU,
  LHU,
  SB,
  SH,
  SW,
  JAL,
  JALR,
  AUIPC,
  LUI,
  UNKNOWN,
};