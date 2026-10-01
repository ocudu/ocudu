// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/cell_configuration.h"

namespace ocudu {
namespace csi_helper {

/// \brief Helpers that checks if the slot is a candidate for CSI RS signalling.
bool is_csi_rs_slot(const cell_configuration& cell_cfg, slot_point sl_tx);

} // namespace csi_helper
} // namespace ocudu
