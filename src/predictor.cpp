#include "predictor.hpp"
#include "memory.hpp"
#include <iostream>

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

bool TwoBitPredictor::opt(const MemoryModule *mem, uint32_t pc_now) {
  return predict[pc_now] >= 2;
}

uint32_t AlwaysFalsePredictor::total_predictions() {
  return total_predictions_;
}

uint32_t AlwaysFalsePredictor::correct_predictions() {
  return correct_predictions_;
}

bool AlwaysFalsePredictor::refresh_predictor(const MemoryModule *mem) {
  uint32_t pc_predict_ = to_unsigned(mem->predict_pc);
  total_predictions_++;
  if (jumps[pc_predict_] != mem->is_jump) {
    return false;
  } else {
    correct_predictions_++;
    return true;
  }
}

bool AlwaysFalsePredictor::opt(const MemoryModule *mem, uint32_t pc_now) {
  return false;
}

uint32_t AlwaysTruePredictor::total_predictions() { return total_predictions_; }

uint32_t AlwaysTruePredictor::correct_predictions() {
  return correct_predictions_;
}

bool AlwaysTruePredictor::refresh_predictor(const MemoryModule *mem) {
  uint32_t pc_predict_ = to_unsigned(mem->predict_pc);
  total_predictions_++;
  if (jumps[pc_predict_] != mem->is_jump) {
    return true;
  } else {
    correct_predictions_++;
    return true;
  }
}

bool AlwaysTruePredictor::opt(const MemoryModule *mem, uint32_t pc_now) {
  return true;
}
