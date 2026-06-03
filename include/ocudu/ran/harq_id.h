// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"

namespace ocudu {

/// Identification of an HARQ process.
enum harq_id_t : uint8_t {
  MAX_HARQ_ID           = 31,
  MAX_NOF_HARQS         = 32, ///< Maximum number of HARQ processes (NTN).
  MAX_NOF_HARQS_NON_NTN = 16, ///< Maximum number of HARQ processes for non-NTN cells.
  INVALID_HARQ_ID       = 32
};

constexpr harq_id_t to_harq_id(unsigned h_id)
{
  return static_cast<harq_id_t>(h_id);
}

/// Bitset mask for DL HARQ Feedback Disabled configuration.
using harq_dl_feedback_disabled_mask = bounded_bitset<MAX_NOF_HARQS, true>;

/// Bitset mask for UL HARQ mode configuration.
using harq_ul_mode_mask = bounded_bitset<MAX_NOF_HARQS, true>;

/// Outcomes of a HARQ-ACK report.
enum class mac_harq_ack_report_status : int8_t { nack = 0, ack, dtx };

/// FMT formatting function.
inline uint8_t format_as(harq_id_t harq_id)
{
  return static_cast<uint8_t>(harq_id);
}
} // namespace ocudu
