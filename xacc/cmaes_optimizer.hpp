#pragma once
#include <type_traits>
#include <utility>

#include "Optimizer.hpp"

namespace xacc {


class CMAESOptimizer : public Optimizer {
public:
  OptResult optimize(OptFunction &function) override;
  const bool isGradientBased() const override;
  const std::string get_algorithm() const override;

  const std::string name() const override { return "cmaes"; }
  const std::string description() const override { return ""; }
};
} // namespace xacc

