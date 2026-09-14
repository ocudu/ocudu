// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
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

/// UL HARQ mode of a HARQ process, as per \c uplinkHARQ-mode, TS 38.331, Section 6.3.2.
enum class ul_harq_mode : uint8_t { mode_a, mode_b };

inline const char* format_as(ul_harq_mode mode)
{
  return mode == ul_harq_mode::mode_a ? "mode_a" : "mode_b";
}

/// Returns whether any of the first \c nof_harqs processes of the given mask operates in the given UL HARQ mode.
inline bool is_ul_harq_mode_available(const harq_ul_mode_mask& mode_mask, unsigned nof_harqs, ul_harq_mode mode)
{
  const size_t nof_checked = std::min(static_cast<size_t>(nof_harqs), mode_mask.size());
  // A bit set to one identifies a process in mode A, so mode B is left wherever a bit is still zero.
  return mode == ul_harq_mode::mode_a ? mode_mask.any(0, nof_checked) : not mode_mask.all(0, nof_checked);
}

/// Outcomes of a HARQ-ACK report.
enum class mac_harq_ack_report_status : int8_t { nack = 0, ack, dtx };

/// FMT formatting function.
inline uint8_t format_as(harq_id_t harq_id)
{
  return static_cast<uint8_t>(harq_id);
}
} // namespace ocudu
