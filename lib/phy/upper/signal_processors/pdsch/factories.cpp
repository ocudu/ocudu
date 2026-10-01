// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/phy/upper/signal_processors/pdsch/factories.h"
#include "dmrs_pdsch_processor_impl.h"
#include "ocudu/adt/format.h"

using namespace ocudu;

namespace {

class dmrs_pdsch_processor_sw_factory : public dmrs_pdsch_processor_factory
{
private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory;

public:
  explicit dmrs_pdsch_processor_sw_factory(std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                                           std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory_) :
    prg_factory(std::move(prg_factory_)), rg_mapper_factory(std::move(rg_mapper_factory_))
  {
    ocudu_assert(prg_factory, "Invalid PRG factory.");
    ocudu_assert(rg_mapper_factory, "Invalid resource grid mapper factory.");
  }

  std::unique_ptr<dmrs_pdsch_processor> create() override
  {
    return std::make_unique<dmrs_pdsch_processor_impl>(prg_factory->create(), rg_mapper_factory->create());
  }
};

} // namespace

std::shared_ptr<dmrs_pdsch_processor_factory>
ocudu::create_dmrs_pdsch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                              std::shared_ptr<resource_grid_mapper_factory>    rg_mapper_factory)
{
  return std::make_shared<dmrs_pdsch_processor_sw_factory>(std::move(prg_factory), std::move(rg_mapper_factory));
}
