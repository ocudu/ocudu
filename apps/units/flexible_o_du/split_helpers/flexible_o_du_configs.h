// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/o_du_high/o_du_high_unit_config.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config.h"
#include "ocudu/ran/cyclic_prefix.h"
#include "ocudu/ran/tdd/tdd_ul_dl_config.h"

namespace ocudu {

class ru_error_notifier;
class ru_timing_notifier;
class ru_uplink_plane_rx_symbol_notifier;
struct worker_manager;

namespace odu {
struct du_cell_config;
}

/// Flexible O-RAN DU unit configuration.
struct flexible_o_du_unit_config {
  struct ru_config {
    app_helpers::metrics_config config;
    bool                        enable_ru_metrics;
    float                       rx_gain_dB;
  };

  /// O-RAN DU high configuration.
  const o_du_high_unit_config& odu_high_cfg;
  /// DU low configuration.
  const du_low_unit_config& du_low_cfg;
  /// RU configuration.
  ru_config ru_cfg;
};

/// Radio Unit dependencies of the flexible O-RAN DU.
struct flexible_o_du_ru_config {
  struct cell_config {
    subcarrier_spacing                     scs;
    cyclic_prefix                          cp;
    bs_channel_bandwidth                   bw;
    unsigned                               nof_rx_antennas;
    unsigned                               nof_tx_antennas;
    antenna_topology                       tx_ant_topology = antenna_topology::one_port;
    std::optional<tdd_ul_dl_config_common> tdd_config;
    arfcn_t                                dl_arfcn;
    arfcn_t                                ul_arfcn;
    frequency_range                        freq_range;
  };

  std::vector<cell_config> cells;
  unsigned                 max_processing_delay;
  unsigned                 prach_nof_ports;
  bool                     is_rt_mode_enabled;
};

/// Radio Unit dependencies of the flexible O-RAN DU.
struct flexible_o_du_ru_dependencies {
  worker_manager&                     workers;
  ru_uplink_plane_rx_symbol_notifier& symbol_notifier;
  ru_timing_notifier&                 timing_notifier;
  ru_error_notifier&                  error_notifier;
};

} // namespace ocudu
