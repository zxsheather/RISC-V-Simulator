#pragma once
#include "concept.h"
#include "module.h"
#include "register.h"
#include "tools.h"
#include <array>
#include <cstdint>

const max_size_t MAX_DMEM = 0x11000;

struct DMEMInput {
    Wire<32> mem_addr;
    Wire<32> write_data;
    Wire<2> mem_op; // 0: None 1: Read 2: Write
    Wire<2> mem_width;
    Wire<1> mem_unsigned;
};

struct DMEMOutput {
    Register<32> rd_data;
};

struct DMEMPrivate {
    std::array< Register<8>, MAX_DMEM> memory;
};

struct DMEMModule : dark::Module<DMEMInput, DMEMOutput, DMEMPrivate> {
    void work() override;
    void read_memory(int addr, int width, bool is_unsigned);
    void write_memory(int addr, int width, max_size_t data);
    void init_memory();
};