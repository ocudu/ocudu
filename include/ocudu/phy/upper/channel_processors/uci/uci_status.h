// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// Labels for the UCI decoding/detection status, common for both PUCCH and PUSCH UCI.
enum class uci_status {
  /// Unknown status.
  unknown = 0,
  /// \brief The UCI message is valid.
  ///
  /// Messages longer than 11 bits are considered as \c valid when the CRC has passed. For shorter messages, the \c
  /// valid state means that the detection metric is above the threshold.
  valid,
  /// \brief The UCI message is invalid.
  ///
  /// Messages longer than 11 bits are considered as \c invalid when the CRC failed. For shorter messages, the \c
  /// invalid state means that all detection metrics are below the threshold.
  invalid
};

/// Gets a string representing a \c uci_status value.
inline const char* to_string(uci_status status)
{
  switch (status) {
    case uci_status::valid:
      return "valid";
    case uci_status::unknown:
      return "unknown";
    case uci_status::invalid:
    default:
      return "invalid";
  }
}

} // namespace ocudu
