// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <string_view>

namespace ocudu {

/// Transmission comb size, as per \c transmissionComb, in \c SRS-Config, TS 38.331, or \f$K_{TC}\f$, as per
/// Section 6.4.1.4.1, TS 38.211. Values {2, 4}.
enum class tx_comb_size : uint8_t { n2 = 2, n4 = 4 };

/// Transmission comb pos size, as per \c transmissionComb, in \c SRS-Config, TS 38.331, or \f$K_{TC}\f$, as per
/// Section 6.4.1.4.1, TS 38.211. Values {2, 4, 8}.
enum class tx_comb_pos_size : uint8_t { n2 = 2, n4 = 4, n8 = 8 };

/// \brief \c groupOrSequenceHopping, parameter for configuring group or sequence hopping.
/// \remark See TS 38.211, clause 6.4.1.4.2, or TS 38.331, "SRS-Resource".
enum class srs_group_or_sequence_hopping { neither, group_hopping, sequence_hopping };

/// \brief \c resourceType, as per TS 38.331, "SRS-Resource".
enum class srs_resource_type { aperiodic, semi_persistent, periodic };

/// \brief SRS resource usage.
/// \remark See TS 38.214, clause 6.2.1.
enum class srs_usage : uint8_t { beam_management, codebook, non_codebook, antenna_switching };

/// Convert SRS resource type to string.
inline std::string_view to_string(srs_resource_type res_type)
{
  switch (res_type) {
    case srs_resource_type::aperiodic:
      return "aperiodic";
    case srs_resource_type::semi_persistent:
      return "semi-persistent";
    case srs_resource_type::periodic:
      return "periodic";
    default:
      break;
  }
  return "Invalid srs resource type";
}

/// Definition of the SRS number of symbols.
enum srs_nof_symbols : uint8_t {
  n1 = 1,
  n2 = 2,
  n4 = 4,
};

enum class srs_periodicity : uint16_t {
  sl1    = 1,
  sl2    = 2,
  sl4    = 4,
  sl5    = 5,
  sl8    = 8,
  sl10   = 10,
  sl16   = 16,
  sl20   = 20,
  sl32   = 32,
  sl40   = 40,
  sl64   = 64,
  sl80   = 80,
  sl160  = 160,
  sl320  = 320,
  sl640  = 640,
  sl1280 = 1280,
  sl2560 = 2560
};

} // namespace ocudu
