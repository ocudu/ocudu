// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/nr_cgi.h"
#include <cstdint>
#include <optional>
#include <vector>

namespace ocudu::ocucp {

/// PWS System Information IE (TS 38.473 section 9.3.1.58).
/// Carries one encoded SIB message and optional additional segments for large payloads.
struct f1ap_pws_sys_info {
  /// SIB type (6 = SIB6/ETWS primary, 7 = SIB7/ETWS secondary, 8 = SIB8/CMAS).
  uint8_t sib_type = 6;
  /// Primary encoded SIB message (ASN.1 PER, TS 38.331).
  byte_buffer sib_msg;
  /// Additional SIB message segments (TS 38.473 section 9.3.1.86).
  /// Each element is one additional segment following the primary sib_msg.
  std::vector<byte_buffer> additional_sib_segments;
};

/// Common type for F1AP WRITE-REPLACE WARNING REQUEST (TS 38.473 section 8.5.1.1).
struct f1ap_write_replace_warning_request {
  /// PWS System Information IE (mandatory).
  f1ap_pws_sys_info pws_sys_info;
  /// Repetition Period in seconds (mandatory).
  uint32_t repeat_period = 0;
  /// Number of Broadcasts Requested (mandatory).
  uint32_t nof_broadcasts_requested = 0;
  /// Cells to broadcast to (optional; absent = all served cells).
  std::optional<std::vector<nr_cell_global_id_t>> cells_to_be_broadcast;
};

/// Common type for F1AP WRITE-REPLACE WARNING RESPONSE (TS 38.473 section 8.9.1.2).
struct f1ap_write_replace_warning_response {
  bool success = false;
  /// Cells that completed broadcasting (optional, reported by DU).
  std::vector<nr_cell_global_id_t> cells_broadcast_completed;
};

} // namespace ocudu::ocucp
