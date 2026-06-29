// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pdsch/pdsch_modulator.h"
#include <vector>

namespace ocudu {

class pdsch_modulator_spy : public pdsch_modulator
{
private:
  struct entry_t {
    config_t                config;
    std::vector<bit_buffer> codewords;
    resource_grid_writer*   grid;
  };
  std::vector<entry_t> entries;

public:
  void modulate(resource_grid_writer& grid, span<const bit_buffer> codewords, const config_t& config) override
  {
    entry_t e = {.config = config};
    for (unsigned cw = 0; cw != codewords.size(); ++cw) {
      e.codewords.emplace_back(codewords[cw]);
    }
    e.grid = &grid;
    entries.emplace_back(e);
  }
  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

PHY_SPY_FACTORY_TEMPLATE(pdsch_modulator);

} // namespace ocudu
