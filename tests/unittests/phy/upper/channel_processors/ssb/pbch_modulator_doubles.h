// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_processors/ssb/pbch_modulator.h"
#include <vector>

namespace ocudu {

class pbch_modulator_spy : public pbch_modulator
{
private:
  struct entry_t {
    config_t              config;
    std::vector<uint8_t>  bits;
    resource_grid_writer* grid_ptr;
  };
  std::vector<entry_t> entries;

public:
  void put(span<const uint8_t> bits, resource_grid_writer& grid, const config_t& config) override
  {
    entry_t e = {};
    e.config  = config;
    e.bits.resize(bits.size());
    e.grid_ptr = &grid;
    std::copy(bits.begin(), bits.end(), e.bits.begin());
    entries.emplace_back(e);
  }
  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

PHY_SPY_FACTORY_TEMPLATE(pbch_modulator);

} // namespace ocudu
