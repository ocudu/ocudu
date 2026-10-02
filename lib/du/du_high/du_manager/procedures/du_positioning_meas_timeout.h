// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/srs/srs_configuration.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <algorithm>
#include <chrono>

namespace ocudu::odu {

/// Number of SRS periods that the MAC waits for a measurement. A second period covers a missed SRS occasion.
inline constexpr unsigned NOF_SRS_PERIODS_PER_POSITIONING_MEAS = 2;

/// Shortest time that the MAC waits for a measurement, for SRS configurations with a very short period.
inline constexpr std::chrono::milliseconds MIN_POSITIONING_MEAS_TIMEOUT{20};

/// \brief Returns the time that the MAC waits for the SRS measurements of the given SRS configuration.
///
/// The measurement completes on an SRS occasion, so the time follows the SRS period. A value below one period makes
/// the measurement fail before the UE transmits.
inline std::chrono::milliseconds get_positioning_meas_timeout(const srs_config& srs_cfg, subcarrier_spacing scs)
{
  unsigned max_period_slots = 0;
  for (const auto& srs_res : srs_cfg.srs_res_list) {
    if (srs_res.periodicity_and_offset.has_value()) {
      max_period_slots = std::max(max_period_slots, static_cast<unsigned>(srs_res.periodicity_and_offset->period));
    }
  }

  // Slots per millisecond, as per TS 38.211 section 4.3.1.
  const unsigned slots_per_ms = get_nof_slots_per_subframe(scs);
  const unsigned timeout_ms   = (max_period_slots * NOF_SRS_PERIODS_PER_POSITIONING_MEAS) / slots_per_ms;

  return std::max(MIN_POSITIONING_MEAS_TIMEOUT, std::chrono::milliseconds{timeout_ms});
}

} // namespace ocudu::odu
