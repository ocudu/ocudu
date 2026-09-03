// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/support/math/math_utils.h"
#include <array>
#include <chrono>
#include <optional>

namespace ocudu {

/// Measurement Gap Repetition Period (MGRP) in msec, as per TS 38.331.
enum class meas_gap_repetition_period : uint8_t { ms20 = 20, ms40 = 40, ms80 = 80, ms160 = 160 };

/// Measurement Gap Length (MGL) in msec, as per TS 38.331.
enum class meas_gap_length : uint8_t { ms1dot5, ms3, ms3dot5, ms4, ms5dot5, ms6 };

/// Configuration of a Measurement Gap as per TS 38.331, GapConfig.
struct meas_gap_config {
  /// Gap offset of the pattern in msec. Value must be between 0 and gap repetition period - 1.
  unsigned offset;
  /// Measurement Gap Length (MGL).
  meas_gap_length mgl;
  /// Measurement Gap Repetition Period (MGRP).
  meas_gap_repetition_period mgrp;

  bool operator==(const meas_gap_config& other) const
  {
    return offset == other.offset && mgl == other.mgl && mgrp == other.mgrp;
  }
  bool operator!=(const meas_gap_config& other) const { return !(*this == other); }
};

/// Convert measurement gap length into a float in milliseconds.
inline float meas_gap_length_to_msec(meas_gap_length len)
{
  static constexpr std::array<float, 6> vals{1.5, 3, 3.5, 4, 5.5, 6};
  return vals[static_cast<unsigned>(len)];
}

struct meas_gap_pattern {
  meas_gap_length            mgl;
  meas_gap_repetition_period mgrp;
};

static constexpr unsigned nof_meas_gap_patterns = 24;

/// Measurement gap pattern configurations as per TS 38.133, Table 9.1.2-1 - indexed by "Gap Pattern Id".
/// The supportedGapPattern-r16 extension patterns 24 and 25 are ignored as those use MGL/MGRP values outside the
/// current range of \ref meas_gap_length and \ref meas_gap_repetition_period enums.
inline constexpr std::array<meas_gap_pattern, nof_meas_gap_patterns> meas_gap_pattern_list = {{
    {meas_gap_length::ms6, meas_gap_repetition_period::ms40},
    {meas_gap_length::ms6, meas_gap_repetition_period::ms80},
    {meas_gap_length::ms3, meas_gap_repetition_period::ms40},
    {meas_gap_length::ms3, meas_gap_repetition_period::ms80},
    {meas_gap_length::ms6, meas_gap_repetition_period::ms20},
    {meas_gap_length::ms6, meas_gap_repetition_period::ms160},
    {meas_gap_length::ms4, meas_gap_repetition_period::ms20},
    {meas_gap_length::ms4, meas_gap_repetition_period::ms40},
    {meas_gap_length::ms4, meas_gap_repetition_period::ms80},
    {meas_gap_length::ms4, meas_gap_repetition_period::ms160},
    {meas_gap_length::ms3, meas_gap_repetition_period::ms20},
    {meas_gap_length::ms3, meas_gap_repetition_period::ms160},
    {meas_gap_length::ms5dot5, meas_gap_repetition_period::ms20},
    {meas_gap_length::ms5dot5, meas_gap_repetition_period::ms40},
    {meas_gap_length::ms5dot5, meas_gap_repetition_period::ms80},
    {meas_gap_length::ms5dot5, meas_gap_repetition_period::ms160},
    {meas_gap_length::ms3dot5, meas_gap_repetition_period::ms20},
    {meas_gap_length::ms3dot5, meas_gap_repetition_period::ms40},
    {meas_gap_length::ms3dot5, meas_gap_repetition_period::ms80},
    {meas_gap_length::ms3dot5, meas_gap_repetition_period::ms160},
    {meas_gap_length::ms1dot5, meas_gap_repetition_period::ms20},
    {meas_gap_length::ms1dot5, meas_gap_repetition_period::ms40},
    {meas_gap_length::ms1dot5, meas_gap_repetition_period::ms80},
    {meas_gap_length::ms1dot5, meas_gap_repetition_period::ms160},
}};

/// \brief Set of measurement gap patterns supported by a UE, based on  \c supportedGapPattern UE capability
/// (TS 38.306, 4.2.9 MeasAndMobParameters). Each bit maps to a Gap Pattern Id as defined in TS 38.133, Table 9.1.2-1.
///  A default-constructed instance (used when no UE capability information is available) only assumes the always
///  supported gap patterns of 0 and 1.
class supported_meas_gap_patterns
{
public:
  /// Bitset of gap pattern Ids (0..23), as per TS 38.133, Table 9.1.2-1.
  using pattern_bitset = bounded_bitset<nof_meas_gap_patterns>;

  /// Creates a set with only the default always supported gap patterns 0 and 1 marked as supported.
  supported_meas_gap_patterns()
  {
    // Add always supported gap pattern 0 and 1.
    supported.push_back(true);
    supported.push_back(true);
    // Set remaining gap patterns 2..23 default to unsupported.
    supported.resize(nof_meas_gap_patterns);
  }

  /// \brief Creates a set from the \c supportedGapPattern ASN.1 bitstring.
  /// Leading / leftmost bit (bit 0) of supportedGapPattern corresponds to the gap pattern 2, the next bit corresponds
  /// to the gap pattern 3, and so on. The mandatory gap patterns 0 and 1 are always supported.
  template <typename Asn1Bitstring>
  explicit supported_meas_gap_patterns(const Asn1Bitstring& supported_gap_pattern)
  {
    ocudu_assert(supported_gap_pattern.length() == nof_meas_gap_patterns - 2,
                 "Unexpected supportedGapPattern size (got {} bits, expected {})",
                 supported_gap_pattern.length(),
                 nof_meas_gap_patterns - 2);
    // Add always supported gap pattern 0 and 1.
    supported.push_back(true);
    supported.push_back(true);
    // Set remaining gap patterns 2..23.
    supported.push_back(supported_gap_pattern.to_number(), supported_gap_pattern.length());
  }

  /// Returns a set where all gap patterns (0..23) are supported.
  static supported_meas_gap_patterns all()
  {
    supported_meas_gap_patterns patterns;
    patterns.supported.fill();
    return patterns;
  }

  /// Marks the gap pattern with the given Gap Pattern Id (0..23) as supported.
  void mark_supported(unsigned pattern_id) { supported.set(pattern_id); }

  /// Returns true if the gap pattern with the given Gap Pattern Id (0..23) is supported.
  bool is_supported(unsigned pattern_id) const { return supported.test(pattern_id); }

  /// Returns true if a gap pattern with the given (MGL, MGRP) is supported.
  bool is_supported(meas_gap_length mgl, meas_gap_repetition_period mgrp) const
  {
    for (unsigned i = 0; i != nof_meas_gap_patterns; ++i) {
      if (meas_gap_pattern_list[i].mgl == mgl and meas_gap_pattern_list[i].mgrp == mgrp) {
        return is_supported(i);
      }
    }
    return false;
  }

  /// Returns the underlying bitset, where bit \c i flags support for Gap Pattern Id \c i.
  const pattern_bitset& bits() const { return supported; }

  bool operator==(const supported_meas_gap_patterns& other) const { return supported == other.supported; }
  bool operator!=(const supported_meas_gap_patterns& other) const { return !(*this == other); }

private:
  pattern_bitset supported;
};

/// Determines whether a slot of the UE downlink timing grid is inside the measurement gap.
/// \note mgta is not modelled, as the DU always signals mgta=ms0; otherwise the window would start mgta earlier.
inline bool is_inside_meas_gap(const meas_gap_config& gap, slot_point sl)
{
  const unsigned slot_per_sf  = sl.nof_slots_per_subframe();
  const unsigned period_slots = static_cast<uint8_t>(gap.mgrp) * slot_per_sf;
  const unsigned length_slots = std::ceil(meas_gap_length_to_msec(gap.mgl) * slot_per_sf);
  const unsigned slot_mod     = (sl - gap.offset * slot_per_sf).count() % period_slots;
  // The gap starts at the gap offset and spans MGL, hence slot_mod in [0, length_slots).
  return slot_mod < length_slots;
}

/// \brief Determines whether an uplink transmission the gNB receives in \c ul_slot falls inside the UE measurement gap.
///
/// The gap is anchored to the UE downlink timing (see \ref is_inside_meas_gap), while its uplink runs T_TA ahead of it
/// (TS 38.211, Section 4.3.1), so the position to test is <tt>ul_slot - T_TA</tt>. T_TA is negligible in a terrestrial
/// cell but reaches tens of slots in an NTN one.
///
/// \param ul_ta T_TA applied by the UE. Absent when not tracked, in which case the window is neither shifted nor
/// guarded.
inline bool
is_inside_ul_meas_gap(const meas_gap_config& gap, slot_point ul_slot, std::optional<std::chrono::microseconds> ul_ta)
{
  const unsigned slot_per_sf  = ul_slot.nof_slots_per_subframe();
  const unsigned period_slots = static_cast<uint8_t>(gap.mgrp) * slot_per_sf;
  // Only T_TA modulo the gap period matters, so a multi-period T_TA (e.g. GEO cell) does not widen the window.
  const unsigned ta_slots =
      ul_ta.has_value() ? static_cast<unsigned>(ul_ta->count() * slot_per_sf / 1000) % period_slots : 0;
  // [Implementation-defined] One 15kHz slot: exactly bounds a UE-reported T_TA, rounded up to a whole 15kHz slot
  // (TS 38.321, Section 6.1.3.56), but covers only 150km from the reference location at 0 deg elevation (and more at
  // higher elevations).
  constexpr std::chrono::microseconds ul_meas_gap_guard{1000};
  const unsigned                      guard_slots =
      ul_ta.has_value() ? divide_ceil(static_cast<unsigned>(ul_meas_gap_guard.count()) * slot_per_sf, 1000U) : 0;
  // The trailing slot covers the true edge reaching into the next UL slot, which partially overlaps and is interrupted
  // too (TS 38.133, Section 9.1C.2). Unconditional: where T_TA is not tracked it is the only margin, so it holds only
  // while N_TA stays below one slot, i.e. up to a 75km cell radius at 30kHz.
  const unsigned length_slots = std::ceil(meas_gap_length_to_msec(gap.mgl) * slot_per_sf) + 2 * guard_slots + 1;
  // A whole period is added so that the start stays non-negative when the guard reaches below the gap offset.
  const unsigned window_start = gap.offset * slot_per_sf + period_slots + ta_slots - guard_slots;
  return (ul_slot - window_start).count() % period_slots < length_slots;
}

} // namespace ocudu
