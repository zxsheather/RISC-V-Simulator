#include "alu.hpp"
#include "tools.h"
#include "util.hpp"
#include <cassert>
#include <cstdint>

void ALUModule::work() {
  if (alu_enable) {
    done <= 1;
    rob_index <= index_rob;
    uint32_t oper = to_unsigned(op);
    int32_t op1 = to_signed(operand_1);
    int32_t op2 = to_signed(operand_2);
    if (oper == Opcode::ADD || oper == Opcode::ADDI) {
      res <= (operand_1 + operand_2);
    } else if (oper == Opcode::SUB) {
      res <= (op1 - op2);
    } else if (oper == Opcode::SLL || oper == Opcode::SLLI) {
      int shift = op2;
      int d = op1;
      int b = d << shift;
      if (shift > 31) {
        b = 0;
      }
      res <= b;
    } else if (oper == Opcode::SRL || oper == Opcode::SRLI) {
      int shift = op2;
      unsigned int c = op1;
      int b = c >> shift;
      if (shift > 31) {
        b = 0;
      }
      res <= b;
    } else if (oper == Opcode::SRA || oper == Opcode::SRAI) {
      int shift = op2;
      int c = op1;
      int b = c >> shift;
      if (shift > 31) {
        if (c < 0) {
          b = 0xffffffff;
        } else {
          b = 0;
        }
      }
      res <= b;
    } else if (oper == Opcode::AND || oper == Opcode::ANDI) {
      res <= (op1 & op2);
    } else if (oper == Opcode::OR || oper == Opcode::ORI) {
      res <= (op1 | op2);
    } else if (oper == Opcode::XOR || oper == Opcode::XORI) {
      res <= (op1 ^ op2);
    } else if (oper == Opcode::SLT || oper == Opcode::SLTI) {
      res <= (op1 < op2);
    } else if (oper == Opcode::SLTU || oper == Opcode::SLTIU) {
      res <= ((unsigned)op1 < (unsigned)op2);
    } else if (oper == BEQ) {
      res <= (op1 == op2);
    } else if (oper == BNE) {
      res <= (op1 != op2);
    } else if (oper == BLT) {
      res <= (op1 < op2);
    } else if (oper == BGE) {
      res <= (op1 >= op2);
    } else if (oper == BLTU) {
      res <= ((unsigned)op1 < (unsigned)op2);
    } else if (oper == BGEU) {
      res <= ((unsigned)op1 >= (unsigned)op2);
    } else {
      assert(false && "Unknown ALU operation");
    }
  } else {
    done <= 0;
  }
}