// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "scheduler_config_helper.h"
#include "ocudu/ran/du_types.h"
#include "ocudu/scheduler/config/logical_channel_config_factory.h"
#include "ocudu/scheduler/config/ran_cell_config.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"
#include "ocudu/scheduler/config/serving_cell_config.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"

using namespace ocudu;

sched_cell_configuration_request_message
sched_config_helper::make_default_sched_cell_configuration_request(const cell_config_builder_params& params_input)
{
  config_helpers::cell_config_builder_params_extended params{params_input};
  sched_cell_configuration_request_message            sched_req{};
  sched_req.cell_index = to_du_cell_index(0);
  sched_req.ran        = config_helpers::make_default_ran_cell_config(params);

  // SIB1 parameters.
  sched_req.si_scheduling.sib1_payload_size = units::bytes{101}; // Random size.

  return sched_req;
}

sched_ue_creation_request_message
sched_config_helper::create_default_sched_ue_creation_request(const ran_cell_config& cell_cfg,
                                                              span<const lcid_t>     lcid_to_cfg)
{
  sched_ue_creation_request_message msg{};

  msg.ue_index = to_du_ue_index(0);
  msg.crnti    = to_rnti(0x4601);

  msg.cfg.cells.emplace();
  msg.cfg.cells->push_back(config_helpers::make_default_ue_cell_config(cell_cfg));

  msg.cfg.lc_config_list.emplace();
  msg.cfg.lc_config_list->resize(2);
  (*msg.cfg.lc_config_list)[0] = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB0);
  (*msg.cfg.lc_config_list)[1] = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB1);
  for (lcid_t lcid : lcid_to_cfg) {
    if (lcid >= lcid_t::LCID_SRB2) {
      msg.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid));
    }
  }

  return msg;
}

sched_ue_creation_request_message
sched_config_helper::create_default_sched_ue_creation_request(const ran_cell_config&               cell_cfg,
                                                              const std::initializer_list<lcid_t>& lcid_to_cfg)
{
  return create_default_sched_ue_creation_request(cell_cfg, span<const lcid_t>(lcid_to_cfg.begin(), lcid_to_cfg.end()));
}

sched_ue_creation_request_message
sched_config_helper::create_empty_spcell_cfg_sched_ue_creation_request(const ran_cell_config& cell_cfg)
{
  sched_ue_creation_request_message msg{};

  msg.ue_index = to_du_ue_index(0);
  msg.crnti    = to_rnti(0x4601);

  ue_cell_config       cfg;
  serving_cell_config& serv_cell = cfg.serv_cell_cfg;
  serv_cell.cell_index           = to_du_cell_index(0);
  cfg.bwps.emplace_back(ue_bwp_config{});

  msg.cfg.cells.emplace();
  msg.cfg.cells->push_back(cfg);

  msg.cfg.lc_config_list.emplace();
  msg.cfg.lc_config_list->resize(1);
  (*msg.cfg.lc_config_list)[0] = config_helpers::create_default_logical_channel_config(lcid_t::LCID_SRB0);

  return msg;
}

ue_creation_mode sched_config_helper::to_ue_creation_mode(const sched_ue_creation_request_message& req)
{
  if (not req.starts_in_fallback) {
    return ue_creation_mode::skip_fallback;
  }
  if (req.ul_ccch_slot_rx.has_value()) {
    return ue_creation_mode::msg3_rach;
  }
  return req.cfra_enabled ? ue_creation_mode::cfra : ue_creation_mode::high_layers;
}
