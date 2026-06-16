// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ue_cell_repository.h"
#include "../logging/cell_metrics_handler.h"

using namespace ocudu;

namespace {

class harq_manager_timeout_notifier : public harq_timeout_notifier
{
public:
  explicit harq_manager_timeout_notifier(cell_metrics_handler& metrics_handler_) : metrics_handler(metrics_handler_) {}

  void on_harq_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override
  {
    metrics_handler.handle_harq_timeout(ue_idx, is_dl);
  }

  void on_feedback_disabled_harq_timeout(du_ue_index_t ue_idx, bool is_dl, units::bytes tbs) override
  {
    if (is_dl) {
      metrics_handler.handle_dl_harq_ack(ue_idx, true, tbs);
    }
  }

private:
  cell_metrics_handler& metrics_handler;
};

} // namespace

ue_cell_repository::ue_cell_repository(const cell_configuration& cell_cfg, cell_metrics_handler* cell_metrics) :
  cell_idx(cell_cfg.cell_index),
  metrics(cell_metrics),
  logger(ocudulog::fetch_basic_logger("SCHED")),
  cell_harqs(MAX_NOF_DU_UES,
             cell_cfg.ntn_cs_koffset > 0 ? MAX_NOF_HARQS : MAX_NOF_HARQS_NON_NTN,
             cell_metrics != nullptr ? std::make_unique<harq_manager_timeout_notifier>(*cell_metrics) : nullptr,
             cell_metrics != nullptr ? std::make_unique<harq_manager_timeout_notifier>(*cell_metrics) : nullptr,
             cell_cfg.expert_cfg.ue.dl_harq_retx_timeout.count() * get_nof_slots_per_subframe(cell_cfg.scs_common()),
             cell_cfg.expert_cfg.ue.ul_harq_retx_timeout.count() * get_nof_slots_per_subframe(cell_cfg.scs_common()),
             cell_harq_manager::DEFAULT_ACK_TIMEOUT_SLOTS,
             cell_cfg.ntn_cs_koffset,
             cell_cfg.params.ntn_params.has_value() && cell_cfg.params.ntn_params->ul_harq_mode_b)
{
  rnti_to_ue_index_lookup.reserve(MAX_NOF_DU_UES);
}

void ue_cell_repository::slot_indication(slot_point sl_tx)
{
  // Process pending HARQ timeouts.
  cell_harqs.slot_indication(sl_tx);
}

void ue_cell_repository::deactivate()
{
  cell_harqs.stop();
}

ue_cell& ue_cell_repository::add_ue(const ue_configuration& ue_cfg,
                                    serv_cell_index_t       serv_cell_index,
                                    ue_pcell_state*         ue_pcell_fsm,
                                    ue_drx_controller&      drx)
{
  ocudu_assert(not ues.contains(ue_cfg.ue_index), "UE with duplicate index being added to the cell UE repository");
  const auto& ue_cell_cfg = ue_cfg.ue_cell_cfg(serv_cell_index);

  // Create UE cell components.
  channel_states.emplace(ue_cfg.ue_index, ue_cell_cfg.cell_cfg_common.expert_cfg.ue, ue_cell_cfg.get_nof_dl_ports());
  ue_mcs_calculators.emplace(ue_cfg.ue_index, ue_cell_cfg.cell_cfg_common, channel_states[ue_cfg.ue_index]);
  pusch_pwr_controllers.emplace(ue_cfg.ue_index, ue_cell_cfg, channel_states[ue_cfg.ue_index], logger);
  pucch_pwr_controllers.emplace(ue_cfg.ue_index, ue_cell_cfg, logger);

  // Add UE in the repository.
  ues.emplace(ue_cfg.ue_index,
              ue_cfg.ue_index,
              ue_cfg.crnti,
              ue_cell_cfg,
              cell_harqs,
              ue_shared_context{drx},
              ue_cell_components{ue_pcell_fsm,
                                 &channel_states[ue_cfg.ue_index],
                                 &ue_mcs_calculators[ue_cfg.ue_index],
                                 &pusch_pwr_controllers[ue_cfg.ue_index],
                                 &pucch_pwr_controllers[ue_cfg.ue_index]},
              logger);
  auto res = rnti_to_ue_index_lookup.insert(std::make_pair(ue_cfg.crnti, ue_cfg.ue_index));
  ocudu_assert(res.second, "UE with duplicate RNTI being added to the cell UE repository");
  return ues[ue_cfg.ue_index];
}

void ue_cell_repository::rem_ue(du_ue_index_t ue_index)
{
  if (not ues.contains(ue_index)) {
    logger.error("ue={} : UE not found in the cell UE repository", fmt::underlying(ue_index));
  }
  const ue_cell&      u      = ues[ue_index];
  const rnti_t        crnti  = u.rnti();
  const du_ue_index_t ue_idx = u.ue_index;

  // Remove UE from lookup.
  auto it = rnti_to_ue_index_lookup.find(crnti);
  if (it != rnti_to_ue_index_lookup.end()) {
    rnti_to_ue_index_lookup.erase(it);
  } else {
    logger.error("ue={} rnti={}: UE with provided c-rnti not found in RNTI-to-UE-index lookup table.",
                 fmt::underlying(ue_idx),
                 crnti);
  }

  pucch_pwr_controllers.erase(ue_idx);
  pusch_pwr_controllers.erase(ue_idx);
  ue_mcs_calculators.erase(ue_idx);
  channel_states.erase(ue_idx);

  // Take the ue cell from the repository.
  ues.erase(ue_idx);
}
