// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/srs/srs_constants.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include "ocudu/ran/time/radio_frame_constants.h"
#include "ocudu/scheduler/sched_consts.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/math/math_utils.h"
#include "ocudu/support/math/pow2_utils.h"
#include <algorithm>

namespace ocudu {

/// \brief Returns the largest ring size that is a power of 2 and divides the number of slots per hyper system frame.
constexpr unsigned get_max_pow2_allocator_ring_size(subcarrier_spacing scs = subcarrier_spacing::kHz15)
{
  const unsigned nof_slots_per_hyper_sfn = 10U * get_nof_slots_per_subframe(scs) * radio_frame_constants::NOF_SFNS;
  // The largest power of 2 that divides a value is given by its lowest set bit.
  return nof_slots_per_hyper_sfn & (~nof_slots_per_hyper_sfn + 1U);
}

/// \brief Derives a ring size for the resource grid allocator that is equal or larger than the given minimum value.
/// \remark 1. The ring size must satisfy the condition NOF_SLOTS_PER_HYPER_SYSTEM_FRAME % ring_size = 0, for
/// the used numerology. Otherwise, misalignments may occur close to the slot point wrap around.
/// Misalignment example: Assume NOF_SLOTS_PER_HYPER_SYSTEM_FRAME = 10240 and ring_size = 37
/// At the slot 1023.9, the ring index 10239 % 37 = 27 is accessed. At slot point 0.0 (once slot point wraps around),
/// the ring index 0 % 37 = 0 would be accessed.
/// \remark 2. If the condition NOF_SLOTS_PER_HYPER_SYSTEM_FRAME % ring_size = 0 is satisfied for
/// the numerology mu=0 (SCS=15kHz), it will be also satisfied for the same ring_size and larger numerologies.
/// This means that in contexts where mu is not known (e.g. compile time), mu=0 can be used for generality sake,
/// at the expense of more memory overhead.
constexpr unsigned get_allocator_ring_size_gt_min(unsigned           minimum_value,
                                                  subcarrier_spacing scs = subcarrier_spacing::kHz15)
{
  // The number of slots per hyper system frame is 5 * 2^(11 + mu). Hence, its divisors are the powers of 2 and the
  // powers of 2 multiplied by 5, in both cases up to the largest power of 2 that divides it.
  const unsigned max_pow2     = get_max_pow2_allocator_ring_size(scs);
  const unsigned min_value    = std::max(minimum_value, 1U);
  const unsigned pow2_size    = to_next_pow2(min_value);
  const unsigned pow2_x5_size = 5U * to_next_pow2(divide_ceil(min_value, 5U));

  if (pow2_size <= max_pow2) {
    return std::min(pow2_size, pow2_x5_size);
  }
  return pow2_x5_size;
}

/// \brief Derives a ring size for the resource grid allocator that is a power of 2 and equal or larger than the given
/// minimum value.
/// \remark The same remarks of \c get_allocator_ring_size_gt_min apply. A power of 2 ring size satisfies remark 1 as
/// long as it does not exceed \c get_max_pow2_allocator_ring_size.
constexpr unsigned get_pow2_allocator_ring_size_gt_min(unsigned           minimum_value,
                                                       subcarrier_spacing scs = subcarrier_spacing::kHz15)
{
  const unsigned ring_size = to_next_pow2(std::max(minimum_value, 1U));
  report_fatal_error_if_not(ring_size <= get_max_pow2_allocator_ring_size(scs),
                            "Ring size={} does not fit an integer number of times in a hyper system frame",
                            ring_size);
  return ring_size;
}

/// \brief Retrieves how far in advance the scheduler can allocate resources in the UL resource grid.
constexpr unsigned get_max_slot_ul_alloc_delay(unsigned ntn_cs_koffset)
{
  return std::max(SCHEDULER_MAX_K0 + std::max(SCHEDULER_MAX_K1, SCHEDULER_MAX_K2 + MAX_MSG3_DELTA),
                  srs_constants::MAX_SRS_SLOT_OFFSET) +
         ntn_cs_koffset;
}

} // namespace ocudu
