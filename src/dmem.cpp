#include "dmem.hpp"
#include "concept.h"
#include "tools.h"

void DMEMModule::work() {
  if (mem_write) {
    max_size_t addr = to_unsigned(mem_addr);
    max_size_t width = to_unsigned(mem_width);
    max_size_t is_unsigned = to_unsigned(mem_unsigned);
    write_memory(addr, width, is_unsigned);
  }
  if (mem_read) {
    max_size_t addr = to_unsigned(mem_addr);
    max_size_t width = to_unsigned(mem_width);
    bool is_unsigned = to_unsigned(mem_unsigned);
    read_memory(addr, width, is_unsigned);
  }
}

void DMEMModule::read_memory(int addr, int width, bool is_unsigned) {
  switch (width) {
  case 0: {
    Bit<8> b = memory[addr];
    if (is_unsigned) {
      rd_data <= to_unsigned(b);
    } else {
      rd_data <= to_signed(b);
    }
    break;
  }
  case 1: {
    Bit<8> b1 = memory[addr];
    Bit<8> b2 = memory[addr + 1];
    Bit<16> b = {b2, b1};
    if (is_unsigned) {
      rd_data <= to_unsigned(b);
    } else {
      rd_data <= to_signed(b);
    }
    break;
  }
  case 2: {
    Bit<8> b1 = memory[addr];
    Bit<8> b2 = memory[addr + 1];
    Bit<8> b3 = memory[addr + 2];
    Bit<8> b4 = memory[addr + 3];
    Bit<32> b = {b4, b3, b2, b1};
    rd_data <= b;
    break;
  }
  }
}

void DMEMModule::write_memory(int addr, int width, max_size_t data) {
  switch (width) {
  case 0: {
    memory[addr] = data & 0xFF;
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

void DMEMModule::init_memory() {
  for(auto &byte: memory){
    byte = 0;
  }
}