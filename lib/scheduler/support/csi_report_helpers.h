// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/csi_rs/csi_meas_config.h"
#include "ocudu/ran/slot_point.h"
#include "ocudu/scheduler/config/ue_bwp_config.h"

namespace ocudu::csi_helper {

/// \brief Checks if the slot is a candidate for periodic CSI reporting on PUCCH for a given user.
bool is_csi_reporting_slot(const ue_periodic_csi_config& periodic_csi,
                           csi_resource_periodicity      csi_period,
                           slot_point                    sl_tx);

} // namespace ocudu::csi_helper
