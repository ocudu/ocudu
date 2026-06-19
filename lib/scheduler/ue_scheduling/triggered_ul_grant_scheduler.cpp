// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "triggered_ul_grant_scheduler.h"
#include "../config/ue_configuration.h"
#include "../ue_context/ue.h"
#include "ocudu/ran/logical_channel/lcid_dl_sch.h"
#include "ocudu/scheduler/result/pdsch_info.h"
#include "ocudu/scheduler/result/sched_result.h"

using namespace ocudu;

triggered_ul_grant_scheduler::triggered_ul_grant_scheduler(ue_repository&     ues_,
                                                           du_cell_index_t    cell_index_,
                                                           subcarrier_spacing scs_) :
  logger(ocudulog::fetch_basic_logger("SCHED")), ues(ues_), cell_index(cell_index_), scs(scs_)
{
  pending_grants.reserve(pending_grants_size);
}

void triggered_ul_grant_scheduler::process_dl_results(slot_point sl_tx, const sched_result& sched_result)
{
  for (const dl_msg_alloc& grant : sched_result.dl.ue_grants) {
    ue* u = ues.find(grant.context.ue_index);
    ocudu_assert(u != nullptr, "No UE context found for triggered UL grant");
    const ue_configuration* ue_cfg  = u->ue_cfg_dedicated();
    const auto              lc_list = ue_cfg->logical_channels();

    for (const dl_msg_tb_info& tb : grant.tb_list) {
      for (const dl_msg_lc_info& lc : tb.lc_chs_to_sched) {
        if (pending_grants.size() >= pending_grants_size) {
          logger.warning("Triggered UL grant queue full ({} entries) at slot={} — skipping DL results",
                         pending_grants.size(),
                         sl_tx);
          return;
        }
        if (not lc.lcid.is_sdu()) {
          continue;
        }
        const auto& lc_cfg = lc_list.value()[lc.lcid.to_lcid()];
        if (not lc_cfg->triggered_ul_grant.has_value()) {
          continue;
        }
        const auto&    trig        = *lc_cfg->triggered_ul_grant;
        const unsigned delay_slots = static_cast<unsigned>(trig.delay_ms.count()) * get_nof_slots_per_subframe(scs);
        pending_grants.emplace_back(
            sl_tx + delay_slots, grant.context.ue_index, lc_cfg->lc_group, units::bytes{trig.grant_size});
      }
    }
  }
}

void triggered_ul_grant_scheduler::run_slot(slot_point pdcch_slot)
{
  for (const pending_grant& g : pending_grants) {
    if (g.target_pdcch_slot > pdcch_slot) {
      continue;
    }

    ue* u = ues.find(g.ue_index);
    ocudu_assert(u != nullptr, "No UE context found for triggered UL grant");

    const unsigned ue_pending_bytes = u->logical_channels().pending_bytes(g.lcg_id);
    if (ue_pending_bytes >= g.bytes.value()) {
      // UE already has already more data pending than the triggered grant size; skip to avoid shrinking its BSR.
      continue;
    }
    ul_bsr_indication_message bsr{};
    bsr.cell_index = cell_index;
    bsr.ue_index   = g.ue_index;
    bsr.crnti      = u->crnti;
    bsr.type       = bsr_format::SHORT_BSR;
    bsr.reported_lcgs.push_back({g.lcg_id, g.bytes.value()});
    u->handle_bsr_indication(bsr);
  }
  // Remove all expired pending grants from the list.
  clean_pending_grants(pdcch_slot);
}

void triggered_ul_grant_scheduler::clean_pending_grants(slot_point pdcch_slot)
{
  pending_grants.erase(std::remove_if(pending_grants.begin(),
                                      pending_grants.end(),
                                      [&](const pending_grant& g) { return g.target_pdcch_slot <= pdcch_slot; }),
                       pending_grants.end());
}
