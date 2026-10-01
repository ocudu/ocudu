// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../../phy_test_utils.h"
#include "ocudu/phy/upper/signal_processors/pusch/factories.h"

namespace ocudu {

// Does nothing.
class dmrs_pusch_estimator_results_dummy : public dmrs_pusch_estimator_results
{
public:
  float get_noise_variance(unsigned rx_port) const override { return {}; }

  float get_rsrp(unsigned rx_port, unsigned tx_layer = 0) const override { return {}; }

  static_vector<float, MAX_PORTS> get_rsrp_all_ports(unsigned tx_layer = 0) const override { return {}; }

  float get_epre(unsigned rx_port) const override { return {}; }

  float get_snr(unsigned rx_port) const override { return {}; }

  float get_layer_average_snr(unsigned tx_layer = 0) const override { return {}; }

  phy_time_unit get_time_alignment(unsigned rx_port) const override { return {}; }

  std::optional<float> get_cfo_Hz(unsigned rx_port) const override { return {}; }

  void
  get_symbol_ch_estimate(span<cbf16_t> estimates, unsigned i_symbol, unsigned rx_port, unsigned tx_layer) const override
  {
  }

  void get_symbol_ch_estimate(span<cbf16_t>                              estimates,
                              unsigned                                   i_symbol,
                              unsigned                                   rx_port,
                              unsigned                                   tx_layer,
                              const bounded_bitset<MAX_NOF_SUBCARRIERS>& re_mask) const override
  {
  }

  void get_channel_state_information(channel_state_information& csi) const override {}
};

class dmrs_pusch_estimator_spy : public dmrs_pusch_estimator
{
public:
  struct entry_t {
    const resource_grid_reader* grid;
    configuration               config;
  };

  void estimate(dmrs_pusch_estimator_notifier& notifier,
                const resource_grid_reader&    grid,
                const configuration&           config) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.grid     = &grid;
    entry.config   = config;

    notifier.on_estimation_complete(results_dummy);
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

  void clear() { entries.clear(); }

private:
  std::vector<entry_t>               entries;
  dmrs_pusch_estimator_results_dummy results_dummy;
};

PHY_SPY_FACTORY_TEMPLATE(dmrs_pusch_estimator);

} // namespace ocudu
