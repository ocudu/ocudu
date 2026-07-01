// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "o_du_low_unit_factory_config.h"
#include "ocudu/adt/span.h"
#include "ocudu/du/du_low/o_du_low_config.h"

namespace ocudu {

namespace odu {
struct du_cell_config;
struct du_low_config;
} // namespace odu

struct du_low_unit_config;
struct worker_manager_config;

/// Generates O-DU low configuration from the given parameters.
void generate_o_du_low_config(odu::o_du_low_config&                           out_config,
                              const du_low_unit_config&                       du_low_unit_cfg,
                              span<const o_du_low_unit_config::du_low_config> cells);

/// Per-cell parameters used by the DU low to auto-derive PUSCH/SRS concurrency.
struct du_low_cell_config {
  /// Number of downlink antenna ports.
  unsigned nof_dl_antennas = 1;
  /// Number of uplink antenna ports.
  unsigned nof_ul_antennas = 1;
  /// Channel bandwidth in MHz.
  unsigned channel_bw_mhz = 100;
  /// Maximum number of PUSCH transmission layers.
  unsigned pusch_max_nof_layers = 1;
  /// Ratio between the number of UL slots and the TDD period. Set to 1 for FDD, i.e. all slots are UL.
  double ul_ratio = 1.0;
};

/// Fills the DU low worker manager parameters of the given worker manager configuration.
void fill_du_low_worker_manager_config(worker_manager_config&         config,
                                       const du_low_unit_config&      unit_cfg,
                                       unsigned                       is_blocking_mode_active,
                                       span<const du_low_cell_config> cell_params);

} // namespace ocudu
