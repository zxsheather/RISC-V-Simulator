#include "alu.hpp"
#include "opcode.hpp"
#include "tools.h"

void ALUModule::work() {
  if (issue) {
    switch (static_cast<Opcode>(static_cast<unsigned>(alu_op))) {
      using enum Opcode;
    case ADD:
      out <= (rs1 + rs2);
      break;
    case SUB:
      out <= (rs1 - rs2);
      break;
    case SLL:
      out <= (rs1 << rs2);
      break;
    case SRL:
      out <= (rs1 >> rs2);
      break;
    case SRA:
      out <= (to_signed(rs1) >> to_signed(rs2));
      break;
    case SLT:
      out <= (to_signed(rs1) < to_signed(rs2));
      break;
    case SLTU:
      out <= (rs1 < rs2);
      break;
    default:
      break;
    }
    done <= 1;
  } else {
    done <= 0;
  }
}