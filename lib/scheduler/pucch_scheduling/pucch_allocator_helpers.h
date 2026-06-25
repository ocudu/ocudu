// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/ue_configuration.h"
#include "ocudu/ran/pucch/pucch_configuration.h"
#include "ocudu/scheduler/result/pdcch_info.h"

namespace ocudu::pucch_helper {

/// Returns the PUCCH resource from Resource Set \c ResourceSetId indexed by the given PUCCH Resource Indicator.
template <unsigned ResourceSetId>
const pucch_resource& get_harq_resource(const ue_cell_configuration& ue_cfg, unsigned pri);

/// Returns the SR PUCCH resource configured for the given UE.
const pucch_resource& get_sr_resource(const ue_cell_configuration& ue_cfg);

/// Returns the CSI PUCCH resource configured for the given UE.
const pucch_resource& get_csi_resource(const ue_cell_configuration& ue_cfg);

/// Returns the common PUCCH resource indexed by the given DCI info and PRI.
const pucch_resource&
get_common_resource(const cell_configuration& cell_cfg, const dci_context_information& dci_info, unsigned d_pri);

} // namespace ocudu::pucch_helper
