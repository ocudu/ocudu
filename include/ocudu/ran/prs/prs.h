// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Positioning Reference Signals (PRS) type definitions.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/ran/prs/prs_constants.h"
#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>

namespace ocudu {

/// PRS transmission comb size.
enum class prs_comb_size : uint8_t { two = 2, four = 4, six = 6, twelve = 12 };

/// PRS resource repetition factor.
enum class prs_repetition_factor : uint8_t {
  one       = 1,
  two       = 2,
  four      = 4,
  six       = 6,
  eight     = 8,
  sixteen   = 16,
  thirtytwo = 32
};

/// PRS transmission time domain duration.
enum class prs_num_symbols : uint8_t { two = 2, four = 4, six = 6, twelve = 12 };

/// PRS resource time gap between repetitions, in slots.
enum class prs_time_gap : uint8_t { one = 1, two = 2, four = 4, eight = 8, sixteen = 16, thirtytwo = 32 };

/// \brief Muting bit repetition factor of a DL-PRS muting Option 1 pattern, or \f$T_{muting}^{PRS}\f$.
enum class prs_muting_bit_repetition_factor : uint8_t { one = 1, two = 2, four = 4, eight = 8 };

/// \brief DL-PRS Muting Option 1 configuration of a PRS resource set.
///
/// Muting Option 1 mutes whole instances of the resource set. Each bit of \c muting_pattern corresponds to \c
/// muting_bit_repetition_factor consecutive resource set instances, with a bit value of 0 indicating that those
/// instances are muted.
///
/// \remark See TS 38.211, Section 7.4.1.7.4, and TS 37.355, Section 6.4.3, "DL-PRS-MutingOption1".
struct prs_muting_option1 {
  /// Muting pattern bitmap. Size: {2, 4, 6, 8, 16, 32}.
  bounded_bitset<prs_constants::VALID_MUTING_PATTERN_SIZES.back()> muting_pattern;
  /// Muting bit repetition factor, or \f$T_{muting}^{PRS}\f$.
  prs_muting_bit_repetition_factor muting_bit_repetition_factor = prs_muting_bit_repetition_factor::one;
};

/// \brief DL-PRS Muting Option 2 configuration of a PRS resource set.
///
/// Muting Option 2 mutes selected repetitions within a resource set instance. Each bit of \c muting_pattern corresponds
/// to a repetition index, with a bit value of 0 indicating that the repetition is muted.
///
/// \remark See TS 38.211, Section 7.4.1.7.4, and TS 37.355, Section 6.4.3, "DL-PRS-MutingOption2".
struct prs_muting_option2 {
  /// \brief Muting pattern bitmap.
  ///
  /// Its size equals \ref prs_resource_set::repetition_factor.
  bounded_bitset<prs_constants::VALID_MUTING_PATTERN_SIZES.back()> muting_pattern;
};

/// \brief Determines whether the combination of time domain duration and comb size is valid.
///
/// The valid combinations are given in TS38.211 Section 7.4.1.7.3.
bool prs_valid_num_symbols_and_comb_size(prs_num_symbols nsymb, prs_comb_size comb_sz);

/// \brief Determines whether a PRS resource set periodicity, in slots, is valid for the given numerology.
bool prs_valid_periodicity(unsigned periodicity_slots, unsigned numerology);

/// \brief Frequency offset \f$k^{\prime}\f$ of a downlink PRS resource, as a function of the symbol index within the
/// resource, \f$l - l_{start}^{PRS}\f$.
///
/// \remark See TS 38.211, Table 7.4.1.7.3-1. The pattern is periodic with period \c comb_sz, so it is defined here
/// only for one period and indexed modulo it.
unsigned get_prs_freq_offset(prs_comb_size comb_sz, unsigned l_minus_lstart);

/// \brief Configuration of a single DL-PRS resource within a PRS resource set.
///
/// \remark See TS 38.455, Section 9.2.44, and TS 38.211, Section 7.4.1.7.
struct prs_resource {
  /// \brief Sequence ID seeding the PRS pseudo-random sequence, or \f$n_{ID,seq}^{PRS}\f$.
  ///
  /// Values: {0,...,\ref prs_constants::MAX_SEQUENCE_ID}.
  uint16_t sequence_id;
  /// RE offset, or comb offset, of the resource. Values: {0,...,comb size - 1}.
  uint8_t re_offset;
  /// \brief Slot offset of the resource, on top of the slot offset of the resource set.
  ///
  /// Values: {0,...,\ref prs_constants::MAX_RES_SLOT_OFFSET}.
  uint16_t slot_offset;
  /// First OFDM symbol of the resource within the slot. Values: {0,...,12}.
  uint8_t symbol_offset;
};

/// \brief Configuration of a DL-PRS resource set.
///
/// \remark See TS 38.455, Section 9.2.44, and TS 38.211, Section 7.4.1.7.
struct prs_resource_set {
  /// \brief PRS bandwidth, in PRBs. It is a multiple of \ref prs_constants::PRB_GRANULARITY.
  ///
  /// Values: {\ref prs_constants::MIN_PRBS,...,\ref prs_constants::MAX_PRBS}.
  uint16_t bandwidth_prbs;
  /// Start PRB of the resource set, relative to Point A. Values: {0,...,\ref prs_constants::MAX_START_PRB}.
  uint16_t start_prb;
  /// Comb size, or \f$K_{comb}^{PRS}\f$.
  prs_comb_size comb_size;
  /// \brief Resource set periodicity, or \f$T_{per}^{PRS}\f$, in slots.
  ///
  /// Valid values are given by \ref prs_constants::VALID_PERIODICITIES.
  unsigned periodicity_slots;
  /// Resource set slot offset within the period, or \f$T_{offset}^{PRS}\f$. Values: {0,...,periodicity - 1}.
  unsigned slot_offset;
  /// Resource repetition factor, or \f$T_{rep}^{PRS}\f$.
  prs_repetition_factor repetition_factor;
  /// Resource time gap between repetitions, or \f$T_{gap}^{PRS}\f$.
  prs_time_gap time_gap;
  /// Number of OFDM symbols of each resource, or \f$L_{PRS}\f$.
  prs_num_symbols nof_symbols;
  /// \brief Transmission power offset of the resource set, in dB.
  ///
  /// Values: {\ref prs_constants::MIN_POWER_OFFSET_DB,...,\ref prs_constants::MAX_POWER_OFFSET_DB}.
  int8_t power_offset_db;
  /// \brief Resources of the resource set. Up to \ref prs_constants::MAX_NOF_RESOURCES_PER_SET.
  ///
  /// The PRS Resource ID of a resource is its index in this list.
  std::vector<prs_resource> resources;
  /// \brief DL-PRS Muting Option 1 configuration, if enabled.
  std::optional<prs_muting_option1> muting_option1;
  /// \brief DL-PRS Muting Option 2 configuration, if enabled.
  std::optional<prs_muting_option2> muting_option2;
  // TODO: QCL information.
};

/// DL-PRS configuration of a cell.
struct prs_config {
  /// \brief Resource sets of the cell. Up to \ref prs_constants::MAX_NOF_RESOURCE_SETS.
  ///
  /// The PRS Resource Set ID of a resource set is its index in this list. DL-PRS is disabled when this list is empty.
  std::vector<prs_resource_set> resource_sets;
};

} // namespace ocudu
