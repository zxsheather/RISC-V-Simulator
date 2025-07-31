
#include "tools.h"

struct RegFileInput {
  Wire<32> read_index_1;
  Wire<32> read_index_2;
  Wire<32> write_index;
  Wire<32> read_index_in_rs_1;
  Wire<32> read_index_in_rs_2;
  Wire<32> write_index_in_rs;
  Wire<1> read_enable_1;
  Wire<1> read_enable_2;
  Wire<1> write_enable;
};

struct RegFileOutput {
  Register<1> read_1;
  Register<1> read_2;
  Register<32> read_res_1;
  Register<32> read_res_2;
  Register<32> read_1_index;
  Register<32> read_2_index;
};