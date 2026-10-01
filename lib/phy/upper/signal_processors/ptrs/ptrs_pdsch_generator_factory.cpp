// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator_factory.h"
#include "ptrs_pdsch_generator_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/phy/support/support_factories.h"

using namespace ocudu;

namespace {

/// PT-RS for PDSCH generator generic factory.
class ptrs_pdsch_generator_generic_factory : public ptrs_pdsch_generator_factory
{
public:
  /// Creates a PT-RS for PDSCH generator generic factory.
  ptrs_pdsch_generator_generic_factory(std::shared_ptr<pseudo_random_generator_factory> pseudo_random_gen_factory_,
                                       std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory_) :
    pseudo_random_gen_factory(std::move(pseudo_random_gen_factory_)), rg_mapper_factory(std::move(rg_mapper_factory_))
  {
    ocudu_assert(pseudo_random_gen_factory, "Invalid factory.");
  }

  // See interface for documentation.
  std::unique_ptr<ptrs_pdsch_generator> create() override
  {
    return std::make_unique<ptrs_pdsch_generator_generic_impl>(pseudo_random_gen_factory->create(),
                                                               rg_mapper_factory->create());
  }

private:
  /// Pseudo-random sequence generator factory.
  std::shared_ptr<pseudo_random_generator_factory> pseudo_random_gen_factory;
  /// Resource grid mapper factory.
  std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory;
};

} // namespace

std::shared_ptr<ptrs_pdsch_generator_factory> ocudu::create_ptrs_pdsch_generator_generic_factory(
    std::shared_ptr<pseudo_random_generator_factory> pseudo_random_gen_factory,
    std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory)
{
  return std::make_shared<ptrs_pdsch_generator_generic_factory>(std::move(pseudo_random_gen_factory),
                                                                std::move(rg_mapper_factory));
}
