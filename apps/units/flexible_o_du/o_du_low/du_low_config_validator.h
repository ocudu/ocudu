// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_low_config.h"
#include "ocudu/adt/span.h"
#include "ocudu/ran/prach/prach_format_type.h"
#include "ocudu/ran/prach/prach_subcarrier_spacing.h"

namespace ocudu {

/// DU low PRACH configuration for validation.
struct du_low_prach_validation_config {
  /// PRACH format.
  prach_format_type format;
  /// PRACH subcarrier spacing.
  prach_subcarrier_spacing prach_scs;
  /// PRACH zero correlation zone.
  unsigned zero_correlation_zone;
  /// PRACH number of ports.
  unsigned nof_prach_ports;
  /// Number of uplink antennas.
  unsigned nof_antennas_ul;
};

/// Validates the given DU low configuration. Returns true on success, false otherwise.
bool validate_du_low_config(const du_low_unit_config&                  config,
                            span<const du_low_prach_validation_config> prach_cells_config);

} // namespace ocudu
