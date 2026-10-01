// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>

namespace ocudu {

/// CSI-RS types.
enum class csi_rs_type : uint8_t {
  /// Tracking Reference signals.
  TRS,
  /// Non-Zero-Power CSI Reference Signal.
  CSI_RS_NZP,
  /// Zero-Power CSI Reference Signal.
  CSI_RS_ZP
};

/// Converts a CSI-RS type to a string representation.
inline const char* to_string(csi_rs_type value)
{
  switch (value) {
    case csi_rs_type::TRS:
      return "TRS";
    case csi_rs_type::CSI_RS_NZP:
      return "CSI_RS_NZP";
    case csi_rs_type::CSI_RS_ZP:
      return "CSI_RS_ZP";
    default:
      return "";
  }
}

/// Code-Division Multiplexing (CDM) strategies for CSI-RS.
enum class csi_rs_cdm_type {
  /// No CDM, i.e. CDM group of size 1.
  no_CDM = 0,
  /// Frequency-domain CDM group of size 2.
  fd_CDM2,
  /// CDM group of size 4 (two frequency-domain locations and two time-domain locations).
  cdm4_FD2_TD2,
  /// CDM group of size 8 (two frequency-domain locations and four time-domain locations).
  cdm8_FD2_TD4
};

/// Converts a CSI-RS CDM type to a string representation.
inline const char* to_string(csi_rs_cdm_type value)
{
  switch (value) {
    case csi_rs_cdm_type::no_CDM:
      return "no_CDM";
    case csi_rs_cdm_type::fd_CDM2:
      return "fd_CDM2";
    case csi_rs_cdm_type::cdm4_FD2_TD2:
      return "cdm4_FD2_TD2";
    case csi_rs_cdm_type::cdm8_FD2_TD4:
    default:
      return "cdm8_FD2_TD4";
  }
}

/// Strategies regarding Resource Element (RE) density in the frequency domain for CSI-RS signals.
enum class csi_rs_freq_density_type {
  /// One RE occupied in even-numbered PRB.
  dot5_even_RB = 0,
  /// One RE occupied in odd-numbered PRB.
  dot5_odd_RB,
  /// One RE occupied in every PRB.
  one,
  /// Three RE occupied in every PRB.
  three
};

/// Converts a CSI-RS density type to a string representation.
inline const char* to_string(csi_rs_freq_density_type value)
{
  switch (value) {
    case csi_rs_freq_density_type::dot5_even_RB:
      return "dot5_even";
    case csi_rs_freq_density_type::dot5_odd_RB:
      return "dot5_odd";
    case csi_rs_freq_density_type::one:
      return "one";
    case csi_rs_freq_density_type::three:
    default:
      return "three";
  }
}

/// Computes the CDM group size.
constexpr unsigned get_cdm_group_size(csi_rs_cdm_type cdm)
{
  // Determine the CDM group size.
  unsigned cdm_group_size = 1;
  if (cdm == csi_rs_cdm_type::fd_CDM2) {
    cdm_group_size = 2;
  } else if (cdm == csi_rs_cdm_type::cdm4_FD2_TD2) {
    cdm_group_size = 4;
  } else if (cdm == csi_rs_cdm_type::cdm8_FD2_TD4) {
    cdm_group_size = 8;
  }
  return cdm_group_size;
}

} // namespace ocudu
