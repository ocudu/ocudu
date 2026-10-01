// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/strong_type.h"
#include <chrono>
#include <type_traits>

namespace ocudu {

/// \c maxNrofTAGs, TS 38.331.
const unsigned MAX_NOF_TIME_ALIGNMENT_GROUPS = 4;

/// \c TimeAlignmentTimer, TS 38.331.
enum class time_alignment_timer {
  ms500    = 500,
  ms750    = 750,
  ms1280   = 1280,
  ms1920   = 1920,
  ms2560   = 2560,
  ms5120   = 5120,
  ms10240  = 10240,
  infinity = 0
};

/// Return the value of \ref time_alignment_timer.
inline std::chrono::milliseconds time_alignment_timer_to_msec(time_alignment_timer timer)
{
  return std::chrono::milliseconds{static_cast<std::underlying_type_t<time_alignment_timer>>(timer)};
}

/// Time Alignment Group (TAG), as per TS 38.331.
struct time_alignment_group {
  using id_t = strong_type<uint8_t, time_alignment_group, strong_equality>;

  id_t                 tag_id;
  time_alignment_timer ta_timer;

  bool operator==(const time_alignment_group& rhs) const { return tag_id == rhs.tag_id && ta_timer == rhs.ta_timer; }
  bool operator!=(const time_alignment_group& rhs) const { return !(rhs == *this); }
};

} // namespace ocudu
