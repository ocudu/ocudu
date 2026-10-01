// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/o_du_low/du_low_config.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config.h"
#include <variant>

namespace ocudu {

/// O-RAN DU low Split 6 unit configuration.
struct split6_o_du_low_unit_config {
  unsigned du_report_period = 1000;
  /// Start jitter in milliseconds.
  unsigned start_time_jitter_ms = 0;
  /// FAPI log level.
  ocudulog::basic_levels fapi_level = ocudulog::basic_levels::warning;
  /// DU low configuration.
  du_low_unit_config du_low_cfg;
  /// Radio Unit configuration.
  std::variant<ru_sdr_unit_config, ru_ofh_unit_parsed_config> ru_cfg = {ru_sdr_unit_config{}};
};

} // namespace ocudu
