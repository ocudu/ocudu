// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/asn1/rrc_nr/cell_group_config.h"

namespace ocudu {
namespace odu {

/// \brief Adds to a cellGroupConfig the release of the elements that only the source cellGroupConfig configures.
///
/// The UE keeps every element of the source configuration that a delta configuration does not release. This function
/// adds a release for each list element and each SetupRelease field that \c src sets up and \c out does not set up.
/// It covers the lists and SetupRelease fields of the structures that the gNB-DU fills, with the BWP and SCell lists.
/// Of the extension fields, it covers only the fields that the gNB-DU fills.
/// \param[in,out] out The cellGroupConfig with the complete configuration of the UE.
/// \param[in] src The cellGroupConfig of the source cell.
void calculate_cell_group_config_release(asn1::rrc_nr::cell_group_cfg_s&       out,
                                         const asn1::rrc_nr::cell_group_cfg_s& src);

} // namespace odu
} // namespace ocudu
