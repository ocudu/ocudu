// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/pdcch/pdcch_modulator.h"
#include <vector>

namespace ocudu {

class pdcch_modulator_spy : public pdcch_modulator
{
private:
  struct entry_t {
    config_t              config;
    std::vector<uint8_t>  bits;
    resource_grid_writer* grid;
  };
  std::vector<entry_t> entries;

public:
  void modulate(resource_grid_writer& grid, span<const uint8_t> data, const config_t& config) override
  {
    entry_t e = {};
    e.config  = config;
    e.bits.resize(data.size());
    e.grid = &grid;
    std::copy(data.begin(), data.end(), e.bits.begin());
    entries.emplace_back(e);
  }
  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

PHY_SPY_FACTORY_TEMPLATE(pdcch_modulator);

} // namespace ocudu
