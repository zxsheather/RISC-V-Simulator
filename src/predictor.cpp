#include "predictor.hpp"
#include "memory.hpp"
#include <cstdint>

uint32_t TwoBitPredictor::total_predictions() { return total_predictions_; }

uint32_t TwoBitPredictor::correct_predictions() { return correct_predictions_; }

bool TwoBitPredictor::refresh_predictor(const MemoryModule *mem) {
  uint32_t pc_predict_ = to_unsigned(mem->predict_pc);
  total_predictions_++;
  if (mem->is_jump) {
    predict[pc_predict_] = std::min(predict[pc_predict_] + 1, 3u);
  } else {
    if (predict[pc_predict_] > 0) {
      predict[pc_predict_]--;
    }
  }
  if (jumps[pc_predict_] != mem->is_jump) {
    return false;
  } else {
    correct_predictions_++;
    return true;
  }
}

uint32_t TwoBitPredictor::opt(const MemoryModule *mem, uint32_t pc_now, int32_t imm) {
  if (predict[pc_now]>=2){
    jumps[pc_now] = pc_now + imm;
  } else {
    jumps[pc_now] = pc_now + 4;
  }
  return jumps[pc_now];
}


