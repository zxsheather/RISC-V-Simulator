#pragma once
#include "module.h"
#include "tools.h"

struct ALUInput {
    Wire<8> alu_op;
    Wire<1> issue;
    Wire<32> rs1;
    Wire<32> rs2;
};

struct ALUOutput {
    Register<32> out;
    Register<1> done;
};

struct ALUModule : dark::Module<ALUInput, ALUOutput> {
    void work() override;
};