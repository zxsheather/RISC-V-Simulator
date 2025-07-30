
#include "module.h"
#include "tools.h"

struct ALUInput {
  Wire<1> alu_enable;
  Wire<32> operand_1;
  Wire<32> operand_2;
  Wire<32> op;
  Wire<32> index_rob;
};

struct ALUOutput {
  Register<32> res;
  Register<1> done;
  Register<32> rob_index;
};

struct ALUModule : dark::Module<ALUInput, ALUOutput> {
  void work() override final;
};