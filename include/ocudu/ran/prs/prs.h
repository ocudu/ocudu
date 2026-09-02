// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Positioning Reference Signals (PRS) type definitions.

#pragma once

#include <cstdint>
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

/// \brief Determines whether the combination of time domain duration and comb size is valid.
///
/// The valid combinations are given in TS38.211 Section 7.4.1.7.3.
inline bool prs_valid_num_symbols_and_comb_size(prs_num_symbols nsymb, prs_comb_size comb_sz)
{
  uint8_t nsymb_u8   = static_cast<uint8_t>(nsymb);
  uint8_t comb_sz_u8 = static_cast<uint8_t>(comb_sz);
  return (nsymb_u8 >= comb_sz_u8) && (nsymb_u8 % comb_sz_u8 == 0);
}

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
  // TODO: Muting (Options 1 and 2) and QCL information.
};

/// DL-PRS configuration of a cell.
struct prs_config {
  /// \brief Resource sets of the cell. Up to \ref prs_constants::MAX_NOF_RESOURCE_SETS.
  ///
  /// The PRS Resource Set ID of a resource set is its index in this list. DL-PRS is disabled when this list is empty.
  std::vector<prs_resource_set> resource_sets;
};

} // namespace ocudu
