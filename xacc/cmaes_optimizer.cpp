
#include "cmaes_optimizer.hpp"
#include "cmaes.h"

#include "xacc.hpp"
#include <iostream>

#include "cppmicroservices/BundleActivator.h"
#include "cppmicroservices/BundleContext.h"
#include "cppmicroservices/ServiceProperties.h"

using namespace std::placeholders;
using namespace libcmaes;

namespace xacc {

const std::string CMAESOptimizer::get_algorithm() const { return "cmaes"; }

const bool CMAESOptimizer::isGradientBased() const { return false; }

OptResult CMAESOptimizer::optimize(OptFunction &function) {

  auto dim = function.dimensions();
  std::vector<double> x(dim);
  if (options.keyExists<std::vector<double>>("initial-parameters")) {
    x = options.get_with_throw<std::vector<double>>("initial-parameters");
  } else if (options.keyExists<std::vector<int>>("initial-parameters")) {
    auto tmpx = options.get<std::vector<int>>("initial-parameters");
    x = std::vector<double>(tmpx.begin(), tmpx.end());
  }

  FitFunc cmaes_objective = [&](const double *x, const int N) {
    std::vector<double> x_values(x, x + N), dx;
    return function(x_values, dx);
  };

  double sigma = 0.1;
  // int lambda = 100; // offsprings at each generation.
  CMAParameters<> cmaparams(x, sigma);

  if (options.keyExists<int>("cmaes-max-feval")) {
    cmaparams.set_max_fevals(options.get<int>("cmaes-max-feval"));
  }

  if (options.keyExists<int>("cmaes-max-iter")) {
    cmaparams.set_max_iter(options.get<int>("cmaes-max-iter"));
  }

  // cmaparams.set_algo(BIPOP_CMAES);
  CMASolutions cmasols = cmaes<>(cmaes_objective, cmaparams);
  std::stringstream ss;
  ss << "best solution: " << cmasols << std::endl;
  ss << "optimization took " << cmasols.elapsed_time() / 1000.0 << " seconds\n";

  xacc::info(ss.str());
  Candidate bcand = cmasols.best_candidate();
  double fmin = bcand.get_fvalue();

  std::vector<double> x_stdv = bcand.get_x();
  return OptResult{fmin, x_stdv};
}

} // namespace xacc

namespace {
using namespace cppmicroservices;

/**
 */
class US_ABI_LOCAL CMAESActivator : public BundleActivator {

public:
  CMAESActivator() {}

  /**
   */
  void Start(BundleContext context) {
    auto xt = std::make_shared<xacc::CMAESOptimizer>();
    context.RegisterService<xacc::Optimizer>(xt);
  }

  /**
   */
  void Stop(BundleContext /*context*/) {}
};

} // namespace

CPPMICROSERVICES_EXPORT_BUNDLE_ACTIVATOR(CMAESActivator)
