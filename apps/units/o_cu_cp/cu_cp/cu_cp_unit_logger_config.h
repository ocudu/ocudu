// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"

namespace ocudu {

/// Configuration of logging functionalities.
struct cu_cp_unit_logger_config {
  ocudulog::basic_levels cu_level    = ocudulog::basic_levels::warning;
  ocudulog::basic_levels e1ap_level  = ocudulog::basic_levels::warning;
  ocudulog::basic_levels f1ap_level  = ocudulog::basic_levels::warning;
  ocudulog::basic_levels pdcp_level  = ocudulog::basic_levels::warning;
  ocudulog::basic_levels rrc_level   = ocudulog::basic_levels::warning;
  ocudulog::basic_levels ngap_level  = ocudulog::basic_levels::warning;
  ocudulog::basic_levels xnap_level  = ocudulog::basic_levels::warning;
  ocudulog::basic_levels nrppa_level = ocudulog::basic_levels::warning;
  ocudulog::basic_levels sec_level   = ocudulog::basic_levels::warning;
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Enable JSON generation for the F1AP Tx and Rx PDUs.
  bool f1ap_json_enabled = false;
  /// Enable JSON generation for the E1AP Tx and Rx PDUs.
  bool e1ap_json_enabled = false;
};

} // namespace ocudu
