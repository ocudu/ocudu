// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "scheduler_configuration_helpers.h"
#include "../du_ue/du_ue.h"
#include "ocudu/du/du_cell_config.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/ran/csi_report/csi_report_config_helpers.h"
#include "ocudu/ran/qos/five_qi_qos_mapping.h"
#include "ocudu/scheduler/config/logical_channel_config_factory.h"
#include "ocudu/scheduler/config/sched_cell_config_helpers.h"
#include <algorithm>

using namespace ocudu;
using namespace odu;

si_scheduling_config ocudu::odu::make_si_scheduling_info_config(const du_cell_config&    du_cfg,
                                                                units::bytes             sib1_len,
                                                                span<const units::bytes> si_message_lens)
{
  ocudu_assert(si_message_lens.size() ==
                   (du_cfg.si.si_config.has_value() ? du_cfg.si.si_config->si_sched_info.size() : 0),
               "Number of SI messages does not match the number of SI payload sizes");

  si_scheduling_config sched_req{};
  sched_req.sib1_payload_size = sib1_len;

  if (du_cfg.si.si_config.has_value()) {
    sched_req.si_window_len_slots = du_cfg.si.si_config->si_window_len_slots;
    sched_req.si_messages.resize(du_cfg.si.si_config->si_sched_info.size());
    for (unsigned i = 0, sz = du_cfg.si.si_config->si_sched_info.size(); i != sz; ++i) {
      const auto& sib_mapping_info = du_cfg.si.si_config->si_sched_info[i].sib_mapping_info;

      sched_req.si_messages[i].period_radio_frames = du_cfg.si.si_config->si_sched_info[i].si_period_radio_frames;
      sched_req.si_messages[i].msg_len             = si_message_lens[i];
      sched_req.si_messages[i].si_window_position  = du_cfg.si.si_config->si_sched_info[i].si_window_position;
      // SI-messages carrying only PWS (ETWS/CMAS) SIBs stay dormant until a Write-Replace Warning activates them.
      sched_req.si_messages[i].requires_activation =
          not sib_mapping_info.empty() and
          std::all_of(sib_mapping_info.begin(), sib_mapping_info.end(), [](sib_type t) {
            return t == sib_type::sib6 || t == sib_type::sib7 || t == sib_type::sib8;
          });
    }
  }

  return sched_req;
}

/// Derives Scheduler Cell Configuration from DU Cell Configuration.
sched_cell_configuration_request_message
ocudu::odu::make_sched_cell_config_req(du_cell_index_t             cell_index,
                                       const odu::du_cell_config&  du_cfg,
                                       const si_scheduling_config& si_sched_cfg)
{
  ocudu_assert(si_sched_cfg.sib1_payload_size.value() > 0, "SIB1 payload size needs to be set");
  ocudu_assert(si_sched_cfg.si_messages.size() ==
                   (du_cfg.si.si_config.has_value() ? du_cfg.si.si_config->si_sched_info.size() : 0),
               "Number of SI messages does not match the number of SI payload sizes");

  sched_cell_configuration_request_message sched_req{};
  sched_req.cell_index       = cell_index;
  sched_req.cell_group_index = (du_cell_group_index_t)cell_index; // No CA by default.
  sched_req.ran              = du_cfg.ran;

  // Convert SIB1 and SI message info scheduling config.
  sched_req.si_scheduling = si_sched_cfg;

  sched_req.rrm_policy_members = du_cfg.rrm_policy_members;

  return sched_req;
}

sched_ue_config_request ocudu::odu::create_scheduler_ue_config_request(const du_ue_context&         ue_ctx,
                                                                       const du_ue_resource_config& ue_res_cfg)
{
  sched_ue_config_request sched_cfg;

  sched_cfg.cells.emplace();
  sched_cfg.cells->resize(1);
  (*sched_cfg.cells)[0] = ue_res_cfg.cell_group.cells.at(SERVING_PCELL_IDX);
  // Add SRB and DRB logical channels.
  sched_cfg.lc_config_list.emplace();
  for (const auto& srb : ue_res_cfg.srbs) {
    auto& sched_lc_ch = sched_cfg.lc_config_list->emplace_back(
        config_helpers::create_default_logical_channel_config(srb_id_to_lcid(srb.srb_id)));
    sched_lc_ch.lc_group                  = srb.mac_cfg.lcg_id;
    sched_lc_ch.lc_sr_mask                = srb.mac_cfg.lc_sr_mask;
    sched_lc_ch.lc_sr_delay_timer_applied = srb.mac_cfg.lc_sr_delay_applied;
    sched_lc_ch.sr_id.emplace(srb.mac_cfg.sr_id);
    sched_lc_ch.triggered_ul_grant = srb.mac_cfg.triggered_ul_grant;
  }
  for (const auto& drb : ue_res_cfg.drbs) {
    auto& sched_lc_ch =
        sched_cfg.lc_config_list->emplace_back(config_helpers::create_default_logical_channel_config(drb.lcid));
    sched_lc_ch.lc_group                  = drb.mac_cfg.lcg_id;
    sched_lc_ch.lc_sr_mask                = drb.mac_cfg.lc_sr_mask;
    sched_lc_ch.lc_sr_delay_timer_applied = drb.mac_cfg.lc_sr_delay_applied;
    sched_lc_ch.sr_id.emplace(drb.mac_cfg.sr_id);
    sched_lc_ch.triggered_ul_grant = drb.mac_cfg.triggered_ul_grant;
    sched_lc_ch.rrm_policy.s_nssai = drb.s_nssai;
    sched_lc_ch.rrm_policy.plmn_id = ue_ctx.nr_cgi.plmn_id;

    const standardized_qos_characteristics* mapping =
        get_5qi_to_qos_characteristics_mapping(drb.qos.qos_desc.get_5qi());
    if (mapping != nullptr) {
      sched_lc_ch.qos.emplace();
      sched_lc_ch.qos->qos          = *mapping;
      sched_lc_ch.qos->arp_priority = drb.qos.alloc_retention_prio.prio_level_arp;
      sched_lc_ch.qos->gbr_qos_info = drb.qos.gbr_qos_info;
    }
  }
  sched_cfg.drx_cfg      = ue_res_cfg.cell_group.mcg_cfg.drx_cfg;
  sched_cfg.meas_gap_cfg = ue_res_cfg.meas_gap;

  return sched_cfg;
}
