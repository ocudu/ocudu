// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Positioning Reference Signals (PRS) parameters.

#pragma once

#include "ocudu/adt/to_array.h"
#include <cstdint>
#include <vector>

namespace ocudu {

/// PRS transmission comb size.
enum class prs_comb_size : uint8_t { two = 2, four = 4, six = 6, twelve = 12 };

/// Valid comb size values, as per TS 38.455, Section 9.2.44, "Comb Size".
inline constexpr auto PRS_VALID_COMB_SIZES = to_array<unsigned>({2, 4, 6, 12});

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

/// Valid resource repetition factor values, as per TS 38.455, Section 9.2.44, "Resource Repetition Factor".
inline constexpr auto PRS_VALID_REPETITION_FACTORS = to_array<unsigned>({1, 2, 4, 6, 8, 16, 32});

/// PRS transmission time domain duration.
enum class prs_num_symbols : uint8_t { two = 2, four = 4, six = 6, twelve = 12 };

/// Valid time domain duration values, in OFDM symbols, as per TS 38.455, Section 9.2.44, "Resource Number of Symbols".
inline constexpr auto PRS_VALID_NUM_SYMBOLS = to_array<unsigned>({2, 4, 6, 12});

/// Valid resource set periodicity values, in slots, as per TS 38.455, Section 9.2.44, "Resource Set Periodicity".
inline constexpr auto PRS_VALID_PERIODICITIES = to_array<unsigned>(
    {4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560, 5120, 10240, 20480, 40960, 81920, 128, 256, 512});

/// PRS resource time gap between repetitions, in slots.
enum class prs_time_gap : uint8_t { one = 1, two = 2, four = 4, eight = 8, sixteen = 16, thirtytwo = 32 };

/// Valid resource time gap values, in slots, as per TS 38.455, Section 9.2.44, "Resource Time Gap".
inline constexpr auto PRS_VALID_TIME_GAPS = to_array<unsigned>({1, 2, 4, 8, 16, 32});

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
  /// Sequence ID seeding the PRS pseudo-random sequence, or \f$n_{ID,seq}^{PRS}\f$. Values: {0,...,4095}.
  unsigned sequence_id;
  /// RE offset, or comb offset, of the resource. Values: {0,...,comb size - 1}.
  unsigned re_offset;
  /// Slot offset of the resource, on top of the slot offset of the resource set. Values: {0,...,511}.
  unsigned slot_offset;
  /// First OFDM symbol of the resource within the slot. Values: {0,...,12}.
  unsigned symbol_offset;
};

/// \brief Configuration of a DL-PRS resource set.
///
/// \remark See TS 38.455, Section 9.2.44, and TS 38.211, Section 7.4.1.7.
struct prs_resource_set {
  /// PRS bandwidth, in PRBs. It is a multiple of 4. Values: {24,...,272}.
  unsigned bandwidth_prbs;
  /// Start PRB of the resource set, relative to Point A. Values: {0,...,2176}.
  unsigned start_prb;
  /// Comb size, or \f$K_{comb}^{PRS}\f$.
  prs_comb_size comb_size;
  /// Resource set periodicity, or \f$T_{per}^{PRS}\f$, in slots. Valid values are given by \ref
  /// PRS_VALID_PERIODICITIES.
  unsigned periodicity_slots;
  /// Resource set slot offset within the period, or \f$T_{offset}^{PRS}\f$. Values: {0,...,periodicity - 1}.
  unsigned slot_offset;
  /// Resource repetition factor, or \f$T_{rep}^{PRS}\f$.
  prs_repetition_factor repetition_factor;
  /// Resource time gap between repetitions, or \f$T_{gap}^{PRS}\f$.
  prs_time_gap time_gap;
  /// Number of OFDM symbols of each resource, or \f$L_{PRS}\f$.
  prs_num_symbols nof_symbols;
  /// Transmission power offset of the resource set, in dB. Values: {-60,...,50}.
  int power_offset_db;
  /// \brief Resources of the resource set. TS 38.455, Section 9.2.44, allows up to 64 resources per set.
  ///
  /// The PRS Resource ID of a resource is its index in this list.
  std::vector<prs_resource> resources;
  // TODO: Muting (Options 1 and 2) and QCL information.
};

/// DL-PRS configuration of a cell.
struct prs_config {
  /// \brief Resource sets of the cell. TS 38.455, Section 9.2.44, allows up to 8 resource sets per TRP.
  ///
  /// The PRS Resource Set ID of a resource set is its index in this list. DL-PRS is disabled when this list is empty.
  std::vector<prs_resource_set> resource_sets;
};

} // namespace ocudu
