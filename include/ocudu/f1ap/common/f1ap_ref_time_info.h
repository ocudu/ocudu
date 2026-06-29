// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {

/// Event type for Reference Time Information Reporting Control as per TS 38.473 section 9.3.1.147.
enum class f1ap_ref_time_event_type { on_demand, periodic, stop };

/// Request sent by the CU to control reference time information reporting as per TS 38.473 section 8.12.1.
struct f1ap_ref_time_report_ctrl_request {
  f1ap_ref_time_event_type event_type;
  /// Reporting periodicity in radio frames (1 radio frame = 10 ms). Present only when event_type is periodic.
  std::optional<uint16_t> report_periodicity_rf;
};

/// Reference time information reported by the DU as per TS 38.473 section 9.3.1.148.
struct f1ap_time_ref_info {
  /// PER-encoded ReferenceTime-r16 (TS 38.331 section 6.3.2). Opaque to F1AP; packed/unpacked by the DU/CU
  /// adapters that already depend on the RRC ASN.1 library.
  byte_buffer ref_time_r16;
  /// Slot point whose SFN (.sfn()) is the reference SFN as per TS 38.473 section 9.3.1.148.
  slot_point ref_slot;
  /// Timing uncertainty in units of 25 ns as per TS 38.331 section 6.3.2. Absent means unspecified.
  std::optional<uint16_t> uncertainty;
  /// When true, timeInfoType is set to localClock; otherwise GPS time (origin: 6 Jan 1980) is assumed.
  bool is_local_clock = false;
};

} // namespace ocudu
