// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../../phy_test_utils.h"
#include "ocudu/phy/upper/signal_processors/pdcch/factories.h"

namespace ocudu {

class dmrs_pdcch_processor_spy : public dmrs_pdcch_processor
{
private:
  struct entry_t {
    config_t              config;
    resource_grid_writer* grid;
  };
  std::vector<entry_t> entries;

public:
  void map(resource_grid_writer& grid, const config_t& config) override
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

PHY_SPY_FACTORY_TEMPLATE(dmrs_pdcch_processor);

} // namespace ocudu
