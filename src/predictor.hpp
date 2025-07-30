
#pragma once
#include "util.hpp"
#include <cstdint>
#include <iostream>

struct MemoryModule;
struct BranchPredictor {
  bool jumps[MEM_MAX]{};
  virtual uint32_t total_predictions() = 0;
  virtual uint32_t correct_predictions() = 0;
  virtual bool refresh_predictor(const MemoryModule *mem) = 0;
  virtual bool opt(const MemoryModule *mem, uint32_t pc_now) { return true; }
  void update_jump(uint32_t pc_now, bool is_jump) { jumps[pc_now] = is_jump; }
  virtual ~BranchPredictor() = default;
  void print_stats() {
    std::cerr << "Total Predictions: " << total_predictions() << std::endl;
    std::cerr << "Correct Predictions: " << correct_predictions() << std::endl;
    std::cerr << "Accuracy: "
              << (static_cast<double>(correct_predictions()) /
                  total_predictions())
              << std::endl;
  }
};

struct TwoBitPredictor : public BranchPredictor {
  uint32_t total_predictions_ = 0;
  uint32_t correct_predictions_ = 0;
  uint32_t predict[MEM_MAX]{};

  uint32_t total_predictions() override;
  uint32_t correct_predictions() override;
  bool refresh_predictor(const MemoryModule *mem) override;
  bool opt(const MemoryModule *mem, uint32_t pc_now) override;
  ~TwoBitPredictor() override = default;
};

struct AlwaysFalsePredictor : public BranchPredictor {
  uint32_t total_predictions_ = 0;
  uint32_t correct_predictions_ = 0;

  uint32_t total_predictions() override;
  uint32_t correct_predictions() override;

  bool refresh_predictor(const MemoryModule *mem) override;
  bool opt(const MemoryModule *mem, uint32_t pc_now) override;
  ~AlwaysFalsePredictor() override = default;
};

struct AlwaysTruePredictor : public BranchPredictor {
  uint32_t total_predictions_ = 0;
  uint32_t correct_predictions_ = 0;

  uint32_t total_predictions() override;
  uint32_t correct_predictions() override;

  bool refresh_predictor(const MemoryModule *mem) override;
  bool opt(const MemoryModule *mem, uint32_t pc_now) override;
  ~AlwaysTruePredictor() override = default;
};