// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <chrono>
#include <optional>

namespace ocudu {

/// \brief Tracks the uplink timing advance T_TA a UE reports for its uplink (TS 38.321, Section 5.4.8).
///
/// Held as UE runtime state rather than in \c ue_cell_configuration, for the same reason the DRX active time is: the
/// report arrives on the scheduler thread on every TA Report MAC CE, whereas UE configurations are rebuilt on the DU
/// control executor. Keeping it here lets the value survive a UE reconfiguration without being copied across
/// configurations, and keeps the scheduler out of the configuration lifecycle.
class ue_ta_report_tracker
{
public:
  /// Stores the T_TA reported by the UE.
  void handle_ta_report(std::chrono::microseconds ul_ta) { reported_ul_ta = ul_ta; }

  /// \brief Discards the stored T_TA, so the uplink measurement gap window stops being placed with it.
  ///
  /// Called when the UE re-acquires its uplink timing, which leaves any earlier report describing a timing the UE no
  /// longer applies.
  void reset() { reported_ul_ta.reset(); }

  /// \brief Retrieves the T_TA last reported by the UE, if any.
  ///
  /// Absent until the UE reports one, which requires ta-Report or TAR-Config to be configured and the UE to support TA
  /// reporting.
  std::optional<std::chrono::microseconds> last_reported_ul_ta() const { return reported_ul_ta; }

private:
  std::optional<std::chrono::microseconds> reported_ul_ta;
};

} // namespace ocudu
