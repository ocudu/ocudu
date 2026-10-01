// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/rrc_nr/serving_cell.h"
#include "ocudu/ran/csi_rs/csi_meas_config.h"

namespace ocudu {
namespace odu {

/// \brief Fills ASN.1 ZP-CSI-RS-Resource struct.
/// \param[in] cfg ZP-CSI-RS-Resource configuration.
/// \return ASN.1 ZP-CSI-RS-Resource struct.
asn1::rrc_nr::zp_csi_rs_res_s make_asn1_zp_csi_rs_resource(const zp_csi_rs_resource& cfg);

/// \brief Fills ASN.1 ZP-CSI-RS-ResourceSet struct.
/// \param[in] cfg ZP-CSI-RS-ResourceSet configuration.
/// \return ASN.1 ZP-CSI-RS-ResourceSet struct.
asn1::rrc_nr::zp_csi_rs_res_set_s make_asn1_zp_csi_rs_resource_set(const zp_csi_rs_resource_set& cfg);

/// \brief Fills ASN.1 CSI-MeasConfig struct.
/// \param[out] out The ASN.1 CSI-MeasConfig struct to fill.
/// \param[in] src Previous CSI Measurement configuration of UE.
/// \param[in] dest Next CSI Measurement configuration of UE.
void calculate_csi_meas_config_diff(asn1::rrc_nr::csi_meas_cfg_s& out,
                                    const csi_meas_config&        src,
                                    const csi_meas_config&        dest);

} // namespace odu
} // namespace ocudu
