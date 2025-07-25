#include "lsb.hpp"
#include "concept.h"
#include "tools.h"

void LSBModule::work() {
    if(busy[head]) {
        if(ticker!=3){
            ticker+=1;
        }else{
            ticker=0;
            exec(head);
        }
    }
    if (in) {
        max_size_t index = to_unsigned(pos_rs) % LSB_MAX;
        append(index);
    }
}