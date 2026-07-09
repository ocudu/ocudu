// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ntn/ntn_configuration_manager_config.h"
#include <optional>

namespace ocudu {
namespace ocudu_ntn {

/// \brief Derives the post-switch serving-cell config from \p current's sat_switch, to be applied at the serving
/// cell's t_service -- the moment the UE executes the switch per TS 38.331 clause 5.7.19, until which
/// satSwitchWithReSync must stay broadcast.
///
/// The result is a copy of \p current with the serving satellite switched to the sat-switch target, sat-switch
/// overrides applied where set (serving values otherwise), and sat_switch and the now-stale t_service cleared.
///
/// \return std::nullopt if the switch does not apply: no sat_switch, TN cell, or no ntn_cfg->t_service.
std::optional<ntn_cell_config> derive_post_switch_config(const ntn_cell_config& current);

} // namespace ocudu_ntn
} // namespace ocudu
