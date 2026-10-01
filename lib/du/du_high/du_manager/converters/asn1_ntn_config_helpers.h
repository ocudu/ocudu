// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/du/du_cell_config.h"

namespace ocudu {
namespace odu {

/// Creates and fills an ASN.1 SIB19 message structure for NTN cell configuration.
/// \param[in] sib19_params The SIB19 configuration parameters to encode.
/// \return The filled ASN.1 SIB19 structure.
asn1::rrc_nr::sib19_r17_s make_asn1_rrc_cell_sib19(const sib19_info& sib19_params);

/// Creates and fills an ASN.1 NTN-Config-R17 structure for NTN cell configuration.
/// \param[in] sib19_params The NTN cell configuration parameters to encode.
/// \return The filled ASN.1 NTN-Config-R17 structure.
asn1::rrc_nr::ntn_cfg_r17_s make_asn1_rrc_cell_ntn_cfg(const ntn_config& ntn_cfg);

/// Fills advanced cell NTN-config parameters in ASN.1 SIB19 message.
/// \param[in]  sib19_params content of SIB19 msg.
/// \param[out] out The ASN.1 SIB19 struct to fill.
/// \return A buffer with packed SIB19 as BCCH-DL-SCH message.
void make_asn1_rrc_advanced_cell_sib19(const sib19_info& sib19_params, asn1::rrc_nr::sib19_r17_s& out);

} // namespace odu
} // namespace ocudu
