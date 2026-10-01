// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <chrono>
#include <optional>

namespace ocudu {
namespace ocudu_ntn {

/// Structure used to send NTN Doppler Compensation Request to RU.
struct doppler_compensation_request {
  using time_point = std::chrono::system_clock::time_point;
  /// Sector Id to which the request applies.
  unsigned sector_id = 0;
  /// Carrier Frequency Offset in Hz.
  float cfo_hz = 0.0;
  /// Carrier Frequency Offset drift in Hz/s relative to the start time. Set it to 0 for no drift.
  float cfo_drift_hz_s = 0.0;
  /// Optional timestamp at which the CFO command is applied. Set to \c std::nullopt for applying it immediately.
  std::optional<time_point> start_timestamp;
};

/// \brief Interface for handling NTN Doppler compensation requests.
///
/// This interface decouples the NTN module from direct RU controller dependencies.
class ntn_doppler_compensation_handler
{
public:
  virtual ~ntn_doppler_compensation_handler() = default;

  /// \brief Handle DL Doppler compensation request.
  ///
  /// \param request Pre-calculated DL Doppler compensation request containing time, frequency, and drift values.
  /// \return True if the request was successfully handled; false otherwise.
  virtual bool handle_dl_doppler_compensation(const doppler_compensation_request& request) = 0;

  /// \brief Handle UL Doppler compensation request.
  ///
  /// \param request Pre-calculated UL Doppler compensation request containing time, frequency, and drift values.
  /// \return True if the request was successfully handled; false otherwise.
  virtual bool handle_ul_doppler_compensation(const doppler_compensation_request& request) = 0;
};

} // namespace ocudu_ntn
} // namespace ocudu
