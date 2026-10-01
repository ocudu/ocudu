// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/slot_point.h"
#include "ocudu/ran/sr_configuration.h"
#include "ocudu/scheduler/config/ue_bwp_config.h"

namespace ocudu::sr_helper {

/// \brief Checks if the slot is a candidate for SR reporting for a given user.
bool is_sr_opportunity_slot(const ue_uplink_bwp_config& ue_ul_cfg, sr_periodicity sr_period, slot_point sl_tx);

} // namespace ocudu::sr_helper
