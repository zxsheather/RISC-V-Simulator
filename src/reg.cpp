
#include "reg.hpp"
#include "tools.h"

void RegFileModule::work() {
    rs1_data <= regs[to_unsigned(rs1_index)];
    rs2_data <= regs[to_unsigned(rs2_index)];

    if(wb_enable && wb_index!=0){
        regs[to_unsigned(wb_index)] <= wb_data;
    }
}