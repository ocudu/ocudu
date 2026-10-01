// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/cell_configuration.h"
#include "ocudu/mac/mac_lc_config.h"

namespace ocudu {

namespace odu {
struct du_cell_config;
}

/// \brief Generates default SRB MAC Logical Channel configuration for SRBs other than SRB0.
mac_lc_config make_default_srb_mac_lc_config(lcid_t lcid);

/// \brief Generates default DRB MAC Logical Channel configuration for DRBs.
mac_lc_config make_default_drb_mac_lc_config();

/// Returns the \c prioritized_bit_rate enum value greater than or equal to given bitrate in bit per second.
prioritized_bit_rate get_pbr_ceil(uint64_t bitrate_bps);

/// \brief Generates DRB MAC Logical Channel configuration for GBR DRBs.
mac_lc_config make_gbr_drb_mac_lc_config(const gbr_qos_flow_information& gbr_qos_flow_info);

/// \brief Generates DRB MAC Logical Channel configuration for non-GBR DRBs.
mac_lc_config make_non_gbr_drb_mac_lc_config();

} // namespace ocudu
