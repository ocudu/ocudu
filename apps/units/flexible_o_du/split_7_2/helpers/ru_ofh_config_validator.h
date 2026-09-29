// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/services/worker_manager/os_sched_affinity_manager.h"
#include "ru_ofh_config.h"
#include "ocudu/ran/antenna_topology.h"
#include "ocudu/ran/subcarrier_spacing.h"

namespace ocudu {

/// Open Fronthaul cell configuration for validation.
struct ru_ofh_cell_validation_config {
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// Downlink antenna topology.
  antenna_topology tx_ant_topology;
  /// Downlink number of antennas.
  unsigned nof_antennas_dl;
  /// Uplink number of antennas.
  unsigned nof_antennas_ul;
  /// PRACH number of ports.
  unsigned nof_prach_ports;
};

/// Validates the given Open Fronthaul Radio Unit configuration. Returns true on success, false otherwise.
bool validate_ru_ofh_config(const ru_ofh_unit_config& config, span<const ru_ofh_cell_validation_config> cell_config);

} // namespace ocudu
