
#pragma once
#include "module.h"
#include "tools.h"

struct BranchCompInput {
    Wire<32> rs1;
    Wire<32> rs2;
    Wire<3> branch_op;
    Wire<1> branch;
};

struct BranchCompOutput {
    Register<1> branch_taken;
};

struct BranchCompModule : dark::Module<BranchCompInput, BranchCompOutput> {
    void work() override;
};