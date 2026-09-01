// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/scheduler/scheduler_configurator.h"

namespace ocudu {
namespace odu {

struct du_cell_config;
struct du_ue_context;
struct du_ue_resource_config;

/// \brief Derive Scheduler SI Scheduling Configuration from DU Cell Configuration.
/// \param si_message_lens Payload size of each SI message of the normal operation.
/// \param pws_si_message_lens Payload size of the content each SI message carrying a warning starts with, 0 bytes if
/// it has none.
si_scheduling_config make_si_scheduling_info_config(const du_cell_config&    du_cfg,
                                                    units::bytes             sib1_len,
                                                    span<const units::bytes> si_message_lens,
                                                    span<const units::bytes> pws_si_message_lens);

/// Derives Scheduler Cell Configuration from DU Cell Configuration.
sched_cell_configuration_request_message make_sched_cell_config_req(du_cell_index_t             cell_index,
                                                                    const odu::du_cell_config&  du_cfg,
                                                                    const si_scheduling_config& si_sched_cfg,
                                                                    unsigned                    max_nof_ue_contexts);

/// Create scheduler UE Configuration Request based on DU UE configuration context.
sched_ue_config_request create_scheduler_ue_config_request(const du_ue_context&         ue_ctx,
                                                           const du_ue_resource_config& ue_res_cfg);

} // namespace odu
} // namespace ocudu
