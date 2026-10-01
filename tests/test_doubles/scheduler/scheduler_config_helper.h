// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/scheduler/ue_context/ue_fsm_states.h"
#include "ocudu/ran/logical_channel/lcid.h"
#include "ocudu/scheduler/config/cell_config_builder_params.h"
#include "ocudu/scheduler/scheduler_configurator.h"

namespace ocudu {
struct ran_cell_config;
} // namespace ocudu

namespace ocudu::sched_config_helper {

/// Create a default cell configuration request.
sched_cell_configuration_request_message
make_default_sched_cell_configuration_request(const cell_config_builder_params& params = {});

/// Create a default UE creation request.
sched_ue_creation_request_message create_default_sched_ue_creation_request(const ran_cell_config& cell_cfg,
                                                                           span<const lcid_t>     lcid_to_cfg = {});
sched_ue_creation_request_message
create_default_sched_ue_creation_request(const ran_cell_config&               cell_cfg,
                                         const std::initializer_list<lcid_t>& lcid_to_cfg);

/// Create a UE creation request with no UE-dedicated config.
sched_ue_creation_request_message create_empty_spcell_cfg_sched_ue_creation_request(const ran_cell_config& cell_cfg);

/// \brief Derive the \c ue_creation_mode matching a request's starts_in_fallback/ul_ccch_slot_rx/cfra_enabled
/// fields, for tests that don't exercise the 2-step RACH successRAR path.
ue_creation_mode to_ue_creation_mode(const sched_ue_creation_request_message& req);

} // namespace ocudu::sched_config_helper
