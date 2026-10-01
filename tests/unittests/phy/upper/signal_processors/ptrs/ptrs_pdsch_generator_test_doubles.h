// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../../phy_test_utils.h"
#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "ocudu/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "ocudu/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator_factory.h"

namespace ocudu {

class ptrs_pdsch_generator_spy : public ptrs_pdsch_generator
{
private:
  struct entry_t {
    configuration         config;
    resource_grid_writer* grid;
  };
  std::vector<entry_t> entries;

public:
  void generate(resource_grid_writer& grid, const configuration& config) override
  {
    entry_t entry = {};
    entry.config  = config;
    entry.grid    = &grid;
    entries.emplace_back(entry);
  }
  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

PHY_SPY_FACTORY_TEMPLATE(ptrs_pdsch_generator);

} // namespace ocudu
