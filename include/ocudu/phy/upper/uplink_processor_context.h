// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/pucch/pucch_mapping.h"
#include "ocudu/ran/rnti.h"
#include "ocudu/ran/slot_point.h"

namespace ocudu {

/// PUCCH context for Format 0 or Format 1.
struct ul_pucch_f0_f1_context {
  /// Scheduling request opportunity indicator.
  bool is_sr_opportunity;
};

/// PUCCH context.
struct ul_pucch_context {
  /// Slot description (also specifies the numerology).
  slot_point slot;
  /// Radio Network Temporary Identifier (RNTI).
  rnti_t rnti;
  /// PUCCH format type.
  pucch_format format;
  /// Format 0 or Format 1 context.
  std::optional<ul_pucch_f0_f1_context> context_f0_f1;
};

/// Sounding Reference Signals context.
struct ul_srs_context {
  /// Slot description (also specifies the numerology).
  slot_point slot;
  /// Radio Network Temporary Identifier (RNTI).
  rnti_t rnti;
  /// Normalized channel IQ matrix report requested.
  bool is_normalized_channel_iq_matrix_report_requested;
  /// Positioning report requested.
  bool is_positioning_report_requested;
};

} // namespace ocudu
