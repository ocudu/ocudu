// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/static_vector.h"
#include "ocudu/ran/beamforming/beam_identifier.h"
#include "ocudu/ran/ssb/ssb_properties.h"
#include "ocudu/ran/subcarrier_spacing.h"
#include <algorithm>

namespace ocudu {

/// Maximum number of candidate SS/PBCH blocks in a SS/PBCH period as per TS 38.213 Section 4.1.
constexpr size_t MAX_NOF_SSB_CANDIDATES = 64;

/// SSB-Index identifies an SS-Block within an SS-Burst.
/// \remark See TS 38.331, "SSB-Index" and "maxNrofSSBs". See also, TS 38.213, clause 4.1.
using ssb_id_t = bounded_integer<uint8_t, 0, 63>;

/// Implements \c ssb-PositionsInBurst, as per TS 38.331.
class ssb_bitmap_t : public bounded_bitset<MAX_NOF_SSB_CANDIDATES, true>
{
public:
  /// \brief Build a default bitmap with L_max 64, with all zero bits.
  ssb_bitmap_t() : bounded_bitset(MAX_NOF_SSB_CANDIDATES) { reset(); }
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
    resize(MAX_NOF_SSB_CANDIDATES);
    if (l_max == MAX_NOF_SSB_CANDIDATES) {
      from_uint64(bitmap);
    } else {
      ocudu_assert(bitmap < static_cast<uint64_t>(0b1 << l_max), "SSB bitmap exceeds the max size for L_max={}", l_max);
      from_uint64(bitmap << (MAX_NOF_SSB_CANDIDATES - l_max));
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

/// Maps each transmitted SSB candidate onto the beam that carries it.
class ssb_beam_mapping
{
public:
  /// Indexes of the SSB candidates that are transmitted.
  using ssb_index_list = static_vector<uint8_t, MAX_NOF_SSB_CANDIDATES>;

  ssb_beam_mapping() = default;

  /// Builds a mapping with the given L_max, in which no SSB candidate is transmitted.
  explicit ssb_beam_mapping(uint8_t l_max) { beams.assign(l_max, beam_identifier::invalid); }

  /// Builds a mapping in which the SSB candidates set in the bitmap are transmitted on the given beam.
  explicit ssb_beam_mapping(const ssb_bitmap_t& bitmap, beam_identifier beam_id = beam_identifier::n0) :
    ssb_beam_mapping(bitmap.get_L_max())
  {
    for (size_t ssb_idx : bitmap.get_bit_positions()) {
      beams[ssb_idx] = beam_id;
    }
  }

  /// L_max, the number of SSB candidates in an SSB period. Possible values: {0, 4, 8, 64}.
  uint8_t get_L_max() const { return beams.size(); }

  /// Assigns a beam to an SSB candidate, marking it as transmitted.
  void set_beam(unsigned ssb_idx, beam_identifier beam_id)
  {
    ocudu_assert(ssb_idx < beams.size(), "SSB index={} exceeds L_max={}", ssb_idx, beams.size());
    beams[ssb_idx] = beam_id;
  }

  /// Marks every SSB candidate as not transmitted, keeping L_max.
  void reset() { beams.assign(beams.size(), beam_identifier::invalid); }

  /// Returns true if the given SSB candidate is transmitted.
  bool is_transmitted(unsigned ssb_idx) const
  {
    ocudu_assert(ssb_idx < beams.size(), "SSB index={} exceeds L_max={}", ssb_idx, beams.size());
    return is_beam_id_valid(beams[ssb_idx]);
  }

  /// Beam that carries the given SSB candidate. Invalid if the candidate is not transmitted.
  beam_identifier get_beam(unsigned ssb_idx) const
  {
    ocudu_assert(ssb_idx < beams.size(), "SSB index={} exceeds L_max={}", ssb_idx, beams.size());
    return beams[ssb_idx];
  }

  /// Number of transmitted SSB candidates.
  unsigned nof_transmitted() const
  {
    return static_cast<unsigned>(std::count_if(beams.begin(), beams.end(), is_beam_id_valid));
  }

  /// Returns true if no SSB candidate is transmitted.
  bool empty() const { return std::none_of(beams.begin(), beams.end(), is_beam_id_valid); }

  /// Indexes of the transmitted SSB candidates, in ascending order.
  ssb_index_list transmitted_indexes() const
  {
    ssb_index_list indexes;
    for (uint8_t ssb_idx = 0, l_max = beams.size(); ssb_idx != l_max; ++ssb_idx) {
      if (is_beam_id_valid(beams[ssb_idx])) {
        indexes.push_back(ssb_idx);
      }
    }
    return indexes;
  }

  /// Implements \c ssb-PositionsInBurst, as per TS 38.331.
  ssb_bitmap_t get_ssb_bitmap() const
  {
    ocudu_assert(not beams.empty(), "The SSB beam mapping has no L_max set");
    ssb_bitmap_t bitmap;
    bitmap.set_L_max(beams.size());
    bitmap.reset();
    for (uint8_t ssb_idx : transmitted_indexes()) {
      bitmap.set(ssb_idx);
    }
    return bitmap;
  }

  bool operator==(const ssb_beam_mapping& rhs) const { return beams == rhs.beams; }
  bool operator!=(const ssb_beam_mapping& rhs) const { return !(*this == rhs); }

private:
  /// Beam of each SSB candidate. Its size is L_max, and the candidates that are not transmitted are invalid.
  static_vector<beam_identifier, MAX_NOF_SSB_CANDIDATES> beams;
};

/// Bounds of ss-PBCH-BlockPower (average SSS EPRE, in dBm) as per ServingCellConfigCommonSIB, TS 38.331:
/// INTEGER (-60..50).
constexpr int MIN_SS_PBCH_BLOCK_POWER = -60;
constexpr int MAX_SS_PBCH_BLOCK_POWER = 50;

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
  /// Transmitted SSB candidates and the beam that carries each of them. Determines \c ssb-PositionsInBurst.
  /// \note The beams are not honoured by the PHY yet: every SSB is transmitted on the same beam.
  ssb_beam_mapping ssb_beams;
  /// PSS EPRE to SSS EPRE for SSB. TS 38.213, Section 4.1 gives an explanation of this measure, but doesn't provide a
  /// name for this parameter. Nor is there a name in the TS 38.331.
  ssb_pss_to_sss_epre pss_to_sss_epre;
  /// Average EPRE of the resources elements that carry secondary synchronization signals in dBm, as per
  /// ServingCellConfigCommonSIB, TS 38.331. Possible values: {-60, ..., 50}
  int ssb_block_power;
};

} // namespace ocudu
