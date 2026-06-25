// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ntn_orbital_compute_module.h"
#include "ocudu/adt/span.h"
#include "ocudu/ntn/ntn_configuration_manager_config.h"
#include "ocudu/ran/ntn.h"
#include "ocudu/ran/sib/system_info_config.h"
#include "ocudu/ran/slot_point.h"
#include <optional>

namespace ocudu {
namespace ocudu_ntn {

/// \brief Assembles sib19_info from OCM results and cell configuration.
///
/// \param cell_cfg      Per-cell NTN configuration.
/// \param epoch_slot    SIB19 epoch Tx slot (used to populate ntn_cfg epoch_time sfn/subframe).
/// \param serving_reply OCM result for the serving satellite.
/// \param sat_sw_reply  OCM result for the sat-switch target satellite, or nullptr if not applicable.
/// \param ncell_replies OCM results for each neighbor cell, indexed to match cell_cfg.ncells.
/// \return Assembled sib19_info.
sib19_info generate_sib19_info(const ntn_cell_config&        cell_cfg,
                               slot_point                    epoch_slot,
                               const ntn_orbital_state&      serving_reply,
                               const ntn_orbital_state*      sat_sw_reply,
                               span<const ntn_orbital_state> ncell_replies);

/// \brief Returns true when any SIB1 value-tag-tracked SIB19 field differs between \p prev and \p curr,
/// or when \p prev has no value (first update).
///
/// Exempt fields (not compared): moving_ref_location, ntn_config::{epoch_time, ntn_ul_sync_validity_dur,
/// ta_info, ephemeris_info}.
bool sib19_tracked_fields_changed(const std::optional<sib19_info>& prev, const sib19_info& curr_sib19);

} // namespace ocudu_ntn
} // namespace ocudu
