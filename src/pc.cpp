#include "pc.hpp"
#include "tools.h"

void PCModule::work() {
    if (to_unsigned(stall) == 0){
        if (to_unsigned(branch_taken)) {
            pc <= branch_target;
        } else {
            pc <= pc + 4;
        }
    }
}