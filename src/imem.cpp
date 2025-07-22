#include "imem.hpp"
#include "concept.h"
#include "tools.h"
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

void IMEMModule::work() {
  max_size_t addr = to_unsigned(pc) >> 2;
  if (addr <= MAX_MEM) {
    inst <= memory[addr];
  } else {
    inst <= 0;
  }
}

void IMEMModule::load() {
  for (auto &mem : memory) {
    mem <= 0;
  }

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
      if (mem_index < MAX_MEM) {
        memory[mem_index] <= inst;
      }
      cur += 4;
    }
  }
}