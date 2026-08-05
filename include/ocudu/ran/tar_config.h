// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <chrono>

namespace ocudu {

/// \brief \c TAR-Config, TS 38.331. Controls the UE Timing Advance reporting procedure of TS 38.321, 5.4.8.
///
/// Only configured for NTN cells whose UE supports uplink-TA-Reporting-r17. Note that this governs the
/// variation-triggered reports; the reports at random access and handover are enabled separately, by ta-Report in
/// SIB19 and in ServingCellConfigCommon respectively.
struct tar_config {
  /// \brief \c offsetThresholdTA. The UE reports whenever its T_TA has moved by at least this much since the last
  /// report. Held in microseconds because the smallest value the field can carry is 0.5ms.
  std::chrono::microseconds offset_threshold_ta = std::chrono::microseconds{0};
  /// \brief \c timingAdvanceSR. Lets the UE raise a Scheduling Request when a report is triggered and no UL-SCH
  /// resource is available. Requires the UE to support sr-TriggeredBy-TA-Report-r17.
  bool sr_enabled = false;

  bool operator==(const tar_config& other) const
  {
    return offset_threshold_ta == other.offset_threshold_ta and sr_enabled == other.sr_enabled;
  }
  bool operator!=(const tar_config& other) const { return !(*this == other); }
};

} // namespace ocudu
