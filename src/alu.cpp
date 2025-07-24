#include "alu.hpp"
#include "opcode.hpp"
#include "tools.h"

void ALUModule::work() {
  if (issue) {
    switch (static_cast<Opcode>(static_cast<unsigned>(alu_op))) {
      using enum Opcode;
    case ADD:
      out <= (rs1 + rs2);
      std::cerr << "ALU ADD: " << to_signed(rs1) << " + " << to_signed(rs2) << " = " << std::endl;
      break;
    case SUB:
      out <= (rs1 - rs2);
      std::cerr << "ALU SUB: " << to_signed(rs1) << " - " << to_signed(rs2) << " = " << std::endl;
      break;
    case SLL:
      out <= (rs1 << rs2);
      std::cerr << "ALU SLL: " << to_signed(rs1) << " << " << to_signed(rs2) << " = " << std::endl;
      break;
    case SRL:
      out <= (rs1 >> rs2);
      std::cerr << "ALU SRL: " << to_signed(rs1) << " >> " << to_signed(rs2) << " = " << std::endl;
      break;
    case SRA:
      out <= (to_signed(rs1) >> to_signed(rs2));
      std::cerr << "ALU SRA: " << to_signed(rs1) << " >> " << to_signed(rs2) << " = " << std::endl;
      break;
    case SLT:
      out <= (to_signed(rs1) < to_signed(rs2));
      std::cerr << "ALU SLT: " << to_signed(rs1) << " < " << to_signed(rs2) << " = " << std::endl;
      break;
    case SLTU:
      out <= (rs1 < rs2);
      std::cerr << "ALU SLTU: " << to_unsigned(rs1) << " < " << to_unsigned(rs2) << " = " << std::endl;
      break;
    default:
      break;
    }
    done <= 1;
  } else {
    done <= 0;
  }
}