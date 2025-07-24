#include "alu.hpp"
#include "bc.hpp"
#include "cpu.h"
#include "decoder.hpp"
#include "dmem.hpp"
#include "fetch.hpp"
#include "reg.hpp"
#include "tools.h"
#include <cstdio>

int main() {
  freopen("/home/zx/local_repo/RISC-V-Simulator/testcases/array_test1.data",
          "r", stdin);
  freopen("output.txt", "w", stdout);

  dark::CPU cpu;
  ALUModule alu;
  DecoderModule decoder;
  DMEMModule dmem;
  FetchUnitModule fetch_unit;
  BranchCompModule bc;
  RegFileModule reg;

  cpu.add_module(&alu);
  cpu.add_module(&decoder);
  cpu.add_module(&dmem);
  cpu.add_module(&fetch_unit);
  cpu.add_module(&bc);
  cpu.add_module(&reg);

  // IMEM -> Decoder
  decoder.instruction = [&]() -> auto & { return fetch_unit.inst; };

  // Decoder -> RegFire
  reg.rs1_index = [&]() -> auto & { return decoder.rs1_index; };
  reg.rs2_index = [&]() -> auto & { return decoder.rs2_index; };
  reg.wb_index = [&]() -> auto & { return decoder.rd_index; };
  reg.wb_enable = [&]() -> auto & { return decoder.reg_write; };

  // RegFile -> ALU
  alu.rs1 = [&]() -> auto & {
    return decoder.alu_src == 2 ? fetch_unit.pc : reg.rs1_data;
  };

  alu.rs2 = [&]() -> auto & {
    return decoder.alu_src == 0 ? reg.rs2_data : decoder.immediate;
  };

  alu.alu_op = [&]() -> auto & { return decoder.alu_op; };
  alu.issue = [&]() -> auto & { return decoder.alu_enable; };

  // ALU/DMEM -> RegFile
  reg.wb_data = [&]() -> auto & {
    if (decoder.mem_op == 1) {
      return dmem.rd_data;
    } else if (decoder.jump_link == 1) {
      return fetch_unit.pc_plus_4;
    } else {
      return alu.out;
    }
  };

  // Decoder/ALU/RegFile -> DMEM
  dmem.mem_addr = [&]() -> auto & { return alu.out; };
  dmem.write_data = [&]() -> auto & { return reg.rs2_data; };
  dmem.mem_op = [&]() -> auto & { return decoder.mem_op; };
  dmem.mem_width = [&]() -> auto & { return decoder.mem_width; };
  dmem.mem_unsigned = [&]() -> auto & { return decoder.mem_unsigned; };

  // BranchComp
  bc.rs1 = [&]() -> auto & { return reg.rs1_data; };
  bc.rs2 = [&]() -> auto & { return reg.rs2_data; };
  bc.branch = [&]() -> auto & { return decoder.branch; };
  bc.branch_op = [&]() -> auto & { return decoder.branch_op; };

  // fetch_unit
  fetch_unit.branch_taken = [&]() -> auto & { return bc.branch_taken; };
  fetch_unit.branch_target = [&]() -> auto & { return alu.out; };
  fetch_unit.alu_done = [&]() -> auto & { return alu.done; };
  fetch_unit.jump = [&]() -> auto & { return decoder.jump_link; };
  fetch_unit.exit_value = [&]() -> auto & { return reg.regs[10]; };

  fetch_unit.load();
  cpu.run(10000000, false);
  return 0;
}