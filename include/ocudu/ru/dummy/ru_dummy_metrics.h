// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/gnb_constants.h"

namespace ocudu {

/// Dummy Radio Unit sector metrics.
struct ru_dummy_sector_metrics {
  /// Sector radio identifier.
  unsigned sector_id;
  /// Total number of UL receive requests.
  uint64_t total_ul_request_count;
  /// Total number of DL transmit requests.
  uint64_t total_dl_request_count;
  /// Total number of PRACH receive requests.
  uint64_t total_prach_request_count;
  /// Number of late UL receive request.
  uint64_t late_ul_request_count;
  /// Number of late DL transmit request.
  uint64_t late_dl_request_count;
  /// Number of late PRACH receive request.
  uint64_t late_prach_request_count;
};

struct ru_dummy_metrics {
  /// Dummy Radio Unit sector metrics.
  static_vector<ru_dummy_sector_metrics, MAX_CELLS_PER_DU> sectors;
};

} // namespace ocudu
