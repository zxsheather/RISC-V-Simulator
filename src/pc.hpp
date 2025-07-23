#pragma once
#include "register.h"
#include "tools.h"

struct PCInput {
    Wire<1> branch_taken;
    Wire<32> branch_target;
    Wire<1> stall;
    Wire<32> old_pc;
};

struct PCOutput {
    Register<32> pc;
    Register<32> old_pc_plus4;
};

struct PCModule : dark::Module<PCInput, PCOutput> {
    void work() override;
};
