// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_cell_config.h"
#include "ocudu/ran/band_helper.h"

namespace ocudu::odu {

/// Returns whether the cell supports the single-antenna RedCap admission profile.
inline bool supports_redcap_admission(const du_cell_config& cell)
{
  const auto& ran = cell.ran;
  return cell.redcap.enabled and not cell.redcap.cell_barred and ran.dl_carrier.nof_ant == 1 and
         ran.ul_carrier.nof_ant == 1 and band_helper::get_freq_range(ran.dl_carrier.band) == frequency_range::FR1 and
         band_helper::get_freq_range(ran.ul_carrier.band) == frequency_range::FR1 and
         ran.dl_carrier.carrier_bw <= bs_channel_bandwidth::MHz20 and
         ran.ul_carrier.carrier_bw <= bs_channel_bandwidth::MHz20;
}

} // namespace ocudu::odu
