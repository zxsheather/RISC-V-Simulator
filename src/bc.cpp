#include "bc.hpp"
#include "tools.h"

void BranchCompModule::work() {
  branch_taken <= 0;
  if (branch) {
    max_size_t op = to_unsigned(branch_op);

    switch (op) {
    // BEQ
    case 0b000:
      branch_taken <= (rs1 == rs2 ? 1 : 0);
      break;

    // BNE
    case 0b001:
      branch_taken <= (rs1 != rs2 ? 1 : 0);
      break;

    // BLT
    case 0b100:
      branch_taken <= (to_signed(rs1) < to_signed(rs2) ? 1 : 0);
      break; 

    // BGE
    case 0b101:
      branch_taken <= (to_signed(rs1) >= to_signed(rs2) ? 1 : 0);
      break;

    // BLTU
    case 0b110:
      branch_taken <= (rs1 < rs2 ? 1 : 0);
      break;

    // BGEU
    case 0b111:
      branch_taken <= (rs1 >= rs2 ? 1 : 0);
      break;
    }
  }
}