#include "pc.hpp"
#include "tools.h"

void PCModule::work() {
    if (stall==0){
        if (branch_taken) {
            pc <= branch_target;
        } else {
            pc <= old_pc + 4;
        }
    }
    old_pc_plus4 <= old_pc + 4;
}