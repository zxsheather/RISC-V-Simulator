#pragma once

#include "module.h"
#include "tools.h"
#include <array>

const max_size_t MAX_MEM = 0x11000;

struct IMEMInput {
    Wire<32> pc;
};

struct IMEMOutput {
    Register<32> inst;
};

struct IMEMPrivate {
    std::array<Register<32>, MAX_MEM> memory;
};

struct IMEMModule : dark::Module<IMEMInput, IMEMOutput, IMEMPrivate> {
    void work() override;
    void load();
};
