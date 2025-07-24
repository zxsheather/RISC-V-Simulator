#include "fetch.hpp"
#include "tools.h"
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>


void FetchUnitModule::work() {
  old_pc <= pc;
  pc_plus_4 <= pc + 4;
  max_size_t addr = to_unsigned(pc) >> 2;
  if (addr < MAX_MEM) {
    if((branch_taken || jump) && alu_done == 0){
      wait <= 1;
      inst <= 0;
    } else {
      uint32_t current_inst = memory[addr];
      inst <= current_inst;
      if (current_inst == 0x0ff00513) {
        std::cerr << "Detected exit instruction (li a0,255)" << std::endl;
        std::cout << (to_unsigned(exit_value) & 0xFF) << std::endl;
      }
      std::cerr << "Fetched instruction: " << std::hex << std::setw(8) << std::setfill('0') << memory[addr] << " from address: " << addr * 4 << std::dec << std::endl;
    }
  } else {
    inst <= 0;
  }

  if ((branch_taken || jump) && to_unsigned(alu_done) == 0) {
        pc <= pc;
    } else if (wait && alu_done) {
        pc <= branch_target;
        wait <= 0;
    } else {
        pc <= pc + 4;
    }
}

void FetchUnitModule::load() {
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
      std::vector<uint8_t> bytes;
      bytes.push_back(std::stoul(byte_str, nullptr, 16));

      int count = 3;
      while (count-- && iss >> byte_str) {
        bytes.push_back(std::stoul(byte_str, nullptr, 16));
      }

      while (bytes.size() < 4) {
        bytes.push_back(0);
      }

      uint32_t inst = (static_cast<uint32_t>(bytes[3]) << 24) |
                      (static_cast<uint32_t>(bytes[2]) << 16) |
                      (static_cast<uint32_t>(bytes[1]) << 8) |
                      (static_cast<uint32_t>(bytes[0]));
      max_size_t mem_index = cur / 4;
      std::cerr << "Loading instruction: " << std::hex << std::setw(8) << std::setfill('0') << inst << " to address: " << mem_index * 4 << std::dec << std::endl;
      if (mem_index < MAX_MEM) {
        memory[mem_index] = inst;
      }
      cur += 4;
    }
  }
}