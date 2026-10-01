// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/generic_functions/transform_precoding/transform_precoding_factories.h"
#include "transform_precoder_dft_impl.h"
#include "ocudu/ran/transform_precoding/transform_precoding_helpers.h"

using namespace ocudu;

namespace {

/// Implements a transform precoder factory based on DFT algorithm.
class transform_precoder_dft_factory : public transform_precoder_factory
{
public:
  transform_precoder_dft_factory(std::shared_ptr<dft_processor_factory> dft_factory_, unsigned max_nof_prb_) :
    dft_factory(std::move(dft_factory_)), max_nof_prb(max_nof_prb_)
  {
    ocudu_assert(dft_factory, "Invalid DFT factory.");
  }

  // See interface for documentation.
  std::unique_ptr<transform_precoder> create() override
  {
    // Create DFT processors for each valid number of PRBs within the limit.
    transform_precoder_dft_impl::collection_dft_processors dft_processors;
    for (unsigned nof_prb = 0, nof_prb_end = max_nof_prb + 1; nof_prb != nof_prb_end; ++nof_prb) {
      if (transform_precoding::is_nof_prbs_valid(nof_prb)) {
        // Prepare DFT size.
        dft_processor::configuration dft_config;
        dft_config.dir  = dft_processor::direction::INVERSE;
        dft_config.size = NOF_SUBCARRIERS_PER_RB * nof_prb;

        dft_processors.emplace(nof_prb, dft_factory->create(dft_config));
      }
    }

    return std::make_unique<transform_precoder_dft_impl>(std::move(dft_processors));
  }

private:
  /// DFT processor factory.
  std::shared_ptr<dft_processor_factory> dft_factory;
  /// Maximum number of PRB.
  unsigned max_nof_prb;
};

} // namespace

std::shared_ptr<transform_precoder_factory>
ocudu::create_dft_transform_precoder_factory(std::shared_ptr<dft_processor_factory> dft_factory, unsigned max_nof_prb)
{
  return std::make_shared<transform_precoder_dft_factory>(std::move(dft_factory), max_nof_prb);
}
