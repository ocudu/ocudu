// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/support/ocudu_assert.h"
#include <chrono>

namespace ocudu {

/// Number of bits of the Timing Advance field of the Timing Advance Report MAC CE.
static constexpr unsigned NOF_TA_REPORT_BITS = 14;
/// Maximum value the Timing Advance field can carry, in slots of 15kHz SCS.
static constexpr unsigned MAX_TA_REPORT_SLOTS = (1U << NOF_TA_REPORT_BITS) - 1U;

/// \brief Decodes the Timing Advance Report MAC CE.
///
/// The CE is two octets: two reserved bits followed by a 14-bit Timing Advance field. Per TS 38.321, 6.1.3.56, the
/// field carries "the least integer number of slots, using subcarrier spacing of 15 kHz, greater than or equal to the
/// Timing Advance value", i.e. T_TA as defined in TS 38.211, 4.3.1, rounded up to a whole millisecond. The reported
/// value is therefore an upper bound on T_TA, coarser than the estimate the gNB derives from the ephemeris; callers
/// must not assume better than millisecond accuracy.
///
/// \remark ATG in FR1 encodes the field in symbols rather than slots. Not supported here, as this gNB does not serve
/// ATG cells.
inline std::chrono::microseconds decode_ta_report(byte_buffer_view payload)
{
  ocudu_sanity_check(payload.length() == 2, "Invalid payload length={} while decoding TA report.", payload.length());

  const unsigned ta_slots = ((static_cast<unsigned>(payload[0]) & 0b111111U) << 8U) | static_cast<unsigned>(payload[1]);
  ocudu_sanity_check(ta_slots <= MAX_TA_REPORT_SLOTS, "Invalid TA report value={}.", ta_slots);

  // Uses slot at 15kHz SCS (i.e., 1ms) as reference, independently of the SCS the cell actually uses.
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::milliseconds{ta_slots});
}

} // namespace ocudu
