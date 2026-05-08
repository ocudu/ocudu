// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/ran/ssb/ssb_properties.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <array>

namespace ocudu {

/// Maximum number of SS/PBCH beams or SSB occasions in a SS/PBCH period as per TS 38.213 Section 4.1.
constexpr size_t NOF_SSB_BEAMS = 64;

/// SSB-Index identifies an SS-Block within an SS-Burst.
/// \remark See TS 38.331, "SSB-Index" and "maxNrofSSBs". See also, TS 38.213, clause 4.1.
using ssb_id_t = bounded_integer<uint8_t, 0, 63>;

/// Implements \c ssb-PositionsInBurst, as per TS 38.331.
class ssb_bitmap_t : public bounded_bitset<NOF_SSB_BEAMS, true>
{
public:
  /// \brief Build a default bitmap with L_max 64, with all zero bits.
  ssb_bitmap_t() : bounded_bitset(NOF_SSB_BEAMS) { reset(); }
  /// \brief Build a bitmap with specified L_max. L_max possible values: {4, 8, 64}.
  /// \ref set_bitmap for the details.
  ssb_bitmap_t(uint64_t bitmap, uint8_t l_max) { set_bitmap(bitmap, l_max); }

  /// \brief Set a bitmap with specified L_max. L_max possible values: {4, 8, 64}.
  /// The bitmap is expected to be L_max bits, where the SSB_idx 0 corresponds to the left most bit, SSB_idx L_max-1 is
  /// the right most bit. For L_max = 8, provide an 8-bit bitmap (e.g, 0b10010110), for L_max = 4, provide a 4-bit
  /// bitmap (e.g, 0b1001).
  void set_bitmap(uint64_t bitmap, uint8_t l_max)
  {
    ocudu_assert(l_max == 4 or l_max == 8 or l_max == 64, "L_max must be 4, 8 or 64");
    resize(NOF_SSB_BEAMS);
    if (l_max == NOF_SSB_BEAMS) {
      from_uint64(bitmap);
    } else {
      ocudu_assert(bitmap < static_cast<uint64_t>(0b1 << l_max), "SSB bitmap exceeds the max size for L_max={}", l_max);
      from_uint64(bitmap << (NOF_SSB_BEAMS - l_max));
    }
    resize(l_max);
  }

  /// Getter for L_max.
  uint8_t get_L_max() const { return size(); }

  /// Set L_max after the ssb bitmap has been constructed, and resize it to the specified L_max.
  void set_L_max(uint8_t l_max)
  {
    ocudu_assert(l_max == 4 or l_max == 8 or l_max == 64, "L_max must be 4, 8 or 64");
    resize(l_max);
  }
};

/// SSB Configuration.
struct ssb_configuration {
  /// SSB subcarrier spacing.
  subcarrier_spacing scs;
  /// Represents the offset to Point A in PRBs as per TS 38.331 Section 6.3.2 IE offsetToPointA.
  /// Possible values: {0, ..., 2199}.
  ssb_offset_to_pointA offset_to_point_A;
  /// SSB periodicity.
  ssb_periodicity ssb_period;
  /// k_ssb or SSB SubcarrierOffest, as per TS38.211 Section 7.4.3.1. Possible values: {0, ..., 23}.
  ssb_subcarrier_offset k_ssb;
  /// Each bit in this bitmap represents whether a beam is active or not as per TS 38.331 Section 6.3.2 IE
  /// ssb-PositionsInBurst.
  ssb_bitmap_t ssb_bitmap;
  /// The n-th element of the array indicates what Beam ID to use for the n-th SSB occasion in \c ssb_bitmap. Only
  /// relevant if the n-th bit of \c ssb_bitmap is set.
  std::array<uint8_t, NOF_SSB_BEAMS> beam_ids;
  /// PSS EPRE to SSS EPRE for SSB. TS 38.213, Section 4.1 gives an explanation of this measure, but doesn't provide a
  /// name for this parameter. Nor is there a name in the TS 38.331.
  ssb_pss_to_sss_epre pss_to_sss_epre;
  /// Average EPRE of the resources elements that carry secondary synchronization signals in dBm, as per
  /// ServingCellConfigCommonSIB, TS 38.331. Possible values: {-60, ..., 50}
  int ssb_block_power;
};

} // namespace ocudu
