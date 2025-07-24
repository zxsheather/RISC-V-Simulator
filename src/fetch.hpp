#pragma once

#include "module.h"
#include "register.h"
#include "tools.h"
#include <array>
#include <cstdint>


const max_size_t MAX_MEM = 0x11000;

struct FetchUnitInput {
    Wire<1> branch_taken;
    Wire<32> branch_target;
    Wire<1> alu_done;
    Wire<2> jump;
    Wire<32> exit_value;
};

struct FetchUnitOutput {
    Register<32> pc;
    Register<32> old_pc;
    Register<32> inst;
    Register<32> pc_plus_4;
    Register<1> wait;
};

struct FetchUnitModule : dark::Module<FetchUnitInput, FetchUnitOutput> {
    void work() override;
    void load();
    uint32_t memory[MAX_MEM];
};
