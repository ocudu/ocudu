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

  void on_feedback_timeout(du_ue_index_t ue_idx, bool is_dl, bool ack) override
  {
    metrics_handler.handle_harq_timeout(ue_idx, is_dl);
  }

  void on_retx_timeout(du_ue_index_t ue_idx, bool is_dl) override
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
             cell_cfg.max_nof_ue_contexts,
             cell_cfg.ntn_cs_koffset > 0 ? MAX_NOF_HARQS : MAX_NOF_HARQS_NON_NTN,
             cell_metrics != nullptr ? std::make_unique<harq_manager_timeout_notifier>(*cell_metrics) : nullptr,
             cell_metrics != nullptr ? std::make_unique<harq_manager_timeout_notifier>(*cell_metrics) : nullptr,
             cell_cfg.expert_cfg.ue.dl_harq_retx_timeout.count() * get_nof_slots_per_subframe(cell_cfg.scs_common()),
             cell_cfg.expert_cfg.ue.ul_harq_retx_timeout.count() * get_nof_slots_per_subframe(cell_cfg.scs_common()),
             cell_harq_manager::DEFAULT_ACK_TIMEOUT_SLOTS,
             cell_cfg.ntn_cs_koffset,
             cell_cfg.params.ntn_params.has_value() && cell_cfg.params.ntn_params->ul_harq_mode_b),
  ue_pool(cell_cfg.max_nof_ue_contexts),
  channel_state_pool(cell_cfg.max_nof_ue_contexts),
  mcs_calculator_pool(cell_cfg.max_nof_ue_contexts),
  pusch_pwr_controller_pool(cell_cfg.max_nof_ue_contexts),
  pucch_pwr_controller_pool(cell_cfg.max_nof_ue_contexts)
{
  // Pre-reserve the UE storage and the range of DU UE indexes, so that no allocation is needed to add a UE.
  ues.reserve(cell_cfg.max_nof_ue_contexts, MAX_NOF_DU_UES);
  rnti_to_ue_index_lookup.reserve(cell_cfg.max_nof_ue_contexts);
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
                                    ue_shared_context       shared_ctx)
{
  ocudu_assert(not ues.contains(ue_cfg.ue_index), "UE with duplicate index being added to the cell UE repository");
  const auto& ue_cell_cfg = ue_cfg.ue_cell_cfg(serv_cell_index);

  report_fatal_error_if_not(not ue_pool.full(),
                            "cell={}: No resources left to add ue={}. The cell was dimensioned for {} UEs",
                            cell_idx,
                            ue_cfg.ue_index,
                            ue_pool.nof_objects());

  // Create UE cell components. The UE takes their ownership, returning them to the pools on removal.
  ue_cell_components components;
  components.pcell_state = ue_pcell_fsm;
  components.channel_state =
      channel_state_pool.get(ue_cell_cfg.cell_cfg_common.expert_cfg.ue, ue_cell_cfg.get_nof_dl_ports());
  components.ue_mcs_calculator    = mcs_calculator_pool.get(ue_cell_cfg.cell_cfg_common, *components.channel_state);
  components.pusch_pwr_controller = pusch_pwr_controller_pool.get(ue_cell_cfg, *components.channel_state, logger);
  components.pucch_pwr_controller = pucch_pwr_controller_pool.get(ue_cell_cfg, logger);

  // Add UE in the repository.
  ues.emplace(
      ue_cfg.ue_index,
      ue_pool.get(ue_cfg.ue_index, ue_cfg.crnti, ue_cell_cfg, cell_harqs, shared_ctx, std::move(components), logger));
  auto res = rnti_to_ue_index_lookup.insert(std::make_pair(ue_cfg.crnti, ue_cfg.ue_index));
  ocudu_assert(res.second, "UE with duplicate RNTI being added to the cell UE repository");
  return *ues[ue_cfg.ue_index];
}

void ue_cell_repository::rem_ue(du_ue_index_t ue_index)
{
  if (not ues.contains(ue_index)) {
    logger.error("ue={} : UE not found in the cell UE repository", ue_index);
  }
  const ue_cell&      u      = *ues[ue_index];
  const rnti_t        crnti  = u.rnti();
  const du_ue_index_t ue_idx = u.ue_index;

  // Remove UE from lookup.
  auto it = rnti_to_ue_index_lookup.find(crnti);
  if (it != rnti_to_ue_index_lookup.end()) {
    rnti_to_ue_index_lookup.erase(it);
  } else {
    logger.error("ue={} rnti={}: UE with provided c-rnti not found in RNTI-to-UE-index lookup table.", ue_idx, crnti);
  }

  // Take the UE cell from the repository. This returns its components back to the pools.
  ues.erase(ue_idx);
}
