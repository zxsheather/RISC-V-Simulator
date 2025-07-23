#include "alu.hpp"
#include "bc.hpp"
#include "cpu.h"
#include "decoder.hpp"
#include "dmem.hpp"
#include "imem.hpp"
#include "pc.hpp"
#include "reg.hpp"
#include "tools.h"

int main() {
  dark::CPU cpu;
  ALUModule alu;
  DecoderModule decoder;
  PCModule pc;
  DMEMModule dmem;
  IMEMModule imem;
  BranchCompModule bc;
  RegFileModule reg;

  cpu.add_module(&alu);
  cpu.add_module(&decoder);
  cpu.add_module(&pc);
  cpu.add_module(&dmem);
  cpu.add_module(&imem);
  cpu.add_module(&bc);
  cpu.add_module(&reg);

  // PC -> IMEM
  imem.pc = [&]() -> auto & { return pc.pc; };

  // IMEM -> Decoder
  decoder.instruction = [&]() -> auto & { return imem.inst; };

  // Decoder -> RegFire
  reg.rs1_index = [&]() -> auto & { return decoder.rs1_index; };
  reg.rs2_index = [&]() -> auto & { return decoder.rs2_index; };
  reg.wb_index = [&]() -> auto & { return decoder.rd_index; };
  reg.wb_enable = [&]() -> auto & { return decoder.reg_write; };

  // RegFile -> ALU
  alu.rs1 = [&]() -> auto & {
    return decoder.pc_to_alu ? pc.pc : reg.rs1_data;
  };

  alu.rs2 = [&]() -> auto & {
    return decoder.alu_src ? decoder.immediate : reg.rs2_data;
  };

  alu.alu_op = [&]() -> auto & { return decoder.alu_op; };
  alu.issue = [&]() -> auto & { return decoder.alu_enable; };

  // ALU/DMEM -> RegFile
  reg.wb_data = [&]() -> auto & {
    if (decoder.mem_read) {
      return dmem.rd_data;
    } else if (decoder.link) {
      return pc.old_pc_plus4;
    } else {
      return alu.out;
    }
  };

  // Decoder/ALU/RegFile -> DMEM
  dmem.mem_addr = [&]() -> auto & { return alu.out; };
  dmem.write_data = [&]() -> auto & { return reg.rs2_data; };
  dmem.mem_read = [&]() -> auto & { return decoder.mem_read; };
  dmem.mem_write = [&]() -> auto & { return decoder.mem_write; };
  dmem.mem_width = [&]() -> auto & { return decoder.mem_width; };
  dmem.mem_unsigned = [&]() -> auto & { return decoder.mem_unsigned; };

  // BranchComp
  bc.rs1 = [&]() -> auto & { return reg.rs1_data; };
  bc.rs2 = [&]() -> auto & { return reg.rs2_data; };
  bc.branch = [&]() -> auto & { return decoder.branch; };
  bc.branch_op = [&]() -> auto & { return decoder.branch_op; };

  // PC
  pc.branch_taken = [&]() -> auto & { return bc.branch_taken; };
  pc.branch_target = [&]() -> auto & { return alu.out; };
  pc.old_pc = [&]() -> auto & {return pc.pc;};
}