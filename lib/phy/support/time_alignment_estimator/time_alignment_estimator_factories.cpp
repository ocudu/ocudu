// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/support/time_alignment_estimator/time_alignment_estimator_factories.h"
#include "time_alignment_estimator_dft_impl.h"
#include "ocudu/phy/generic_functions/dft_processor.h"
#include "ocudu/phy/generic_functions/generic_functions_factories.h"
#include "ocudu/support/ocudu_assert.h"
#include <memory>

using namespace ocudu;

namespace {

class time_alignment_estimator_dft_factory : public time_alignment_estimator_factory
{
public:
  time_alignment_estimator_dft_factory(std::shared_ptr<dft_processor_factory> dft_proc_factory_) :
    dft_proc_factory(std::move(dft_proc_factory_))
  {
    ocudu_assert(dft_proc_factory, "Invalid DFT processor factory.");
  }

  std::unique_ptr<time_alignment_estimator> create() override
  {
    time_alignment_estimator_dft_impl::collection_dft_processors dft_processors;
    for (unsigned dft_size     = time_alignment_estimator_dft_impl::min_dft_size,
                  dft_size_max = time_alignment_estimator_dft_impl::max_dft_size;
         dft_size <= dft_size_max;
         dft_size *= 2) {
      dft_processor::configuration dft_proc_config;
      dft_proc_config.size = dft_size;
      dft_proc_config.dir  = time_alignment_estimator_dft_impl::dft_direction;

      dft_processors.emplace(dft_size, dft_proc_factory->create(dft_proc_config));
    }

    return std::make_unique<time_alignment_estimator_dft_impl>(std::move(dft_processors));
  }

private:
  std::shared_ptr<dft_processor_factory> dft_proc_factory;
};

} // namespace

std::shared_ptr<time_alignment_estimator_factory>
ocudu::create_time_alignment_estimator_dft_factory(std::shared_ptr<dft_processor_factory> dft_proc_factory)
{
  return std::make_shared<time_alignment_estimator_dft_factory>(std::move(dft_proc_factory));
}
