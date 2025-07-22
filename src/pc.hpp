#pragma once
#include "tools.h"

struct PCInput {
    Wire<1> branch_taken;
    Wire<32> branch_target;
    Wire<1> stall;
};

struct PCOutput {
    Register<32> pc;
};

struct PCModule : dark::Module<PCInput, PCOutput> {
    void work() override;
};
