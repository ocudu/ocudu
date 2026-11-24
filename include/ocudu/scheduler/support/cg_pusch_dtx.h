// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cmath>
#include <optional>

namespace ocudu {

/// \brief Returns true if the CRC of a configured grant PUSCH indicates that the UE did not transmit it (DTX).
///
/// A CRC KO is a DTX when its SINR is non-finite or below \c sinr_threshold_dB. A CRC KO without SINR is not a DTX.
inline bool is_cg_pusch_dtx(bool tb_crc_success, std::optional<float> ul_sinr_dB, float sinr_threshold_dB)
{
  if (tb_crc_success or not ul_sinr_dB.has_value()) {
    return false;
  }
  return not std::isfinite(*ul_sinr_dB) or *ul_sinr_dB < sinr_threshold_dB;
}

} // namespace ocudu
