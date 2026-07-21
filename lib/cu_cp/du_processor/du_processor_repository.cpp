// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_processor_repository.h"
#include "du_processor_config.h"
#include "du_processor_factory.h"
#include "ocudu/adt/format.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/cu_cp/cu_cp_configuration_helpers.h"
#include "ocudu/rrc/rrc_config.h"
#include "ocudu/support/executors/sync_task_executor.h"

using namespace ocudu;
using namespace ocucp;

du_processor_repository::du_processor_repository(const du_repository_config&       cfg_,
                                                 const du_repository_dependencies& dependencies) :
  cfg(cfg_),
  cu_cp_executor(dependencies.cu_cp_executor),
  timers(dependencies.timers),
  cu_cp_du_handler(dependencies.cu_cp_du_handler),
  meas_config_handler(dependencies.meas_config_handler),
  ue_removal_handler(dependencies.ue_removal_handler),
  ue_context_handler(dependencies.ue_context_handler),
  common_task_sched(dependencies.common_task_sched),
  ue_mng(dependencies.ue_mng),
  du_conn_notif(dependencies.du_conn_notif),
  ref_time_report_notifier(dependencies.ref_time_report_notifier),
  logger(dependencies.logger),
  du_cfg_mng(cfg.gnb_id, config_helpers::get_supported_plmns(cfg.ngaps))
{
}

cu_cp_du_index_t du_processor_repository::add_du(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier)
{
  cu_cp_du_index_t du_index = get_next_du_index();
  if (du_index == cu_cp_du_index_t::invalid) {
    logger.warning("DU connection failed. Cause: Maximum number of DUs connected ({})", cfg.max_nof_dus);
    fmt::print("DU connection failed. Cause: Maximum number of DUs connected ({}). To increase the number of allowed "
               "DUs change the \"--max_nof_dus\" in the CU-CP configuration\n",
               cfg.max_nof_dus);
    return cu_cp_du_index_t::invalid;
  }

  // Create DU object
  auto it = du_db.insert(std::make_pair(du_index, du_context{}));
  ocudu_assert(it.second, "Unable to insert DU in map");
  du_context& du_ctxt = it.first->second;
  du_ctxt.du_to_cu_cp_notifier.connect_cu_cp(
      cu_cp_du_handler, meas_config_handler, ue_removal_handler, ue_context_handler);
  du_ctxt.f1ap_tx_pdu_notifier = std::move(f1ap_tx_pdu_notifier);

  du_processor_config           du_processor_cfg{.du_index                       = du_index,
                                                 .gnb_id                         = cfg.gnb_id,
                                                 .ran_node_name                  = cfg.ran_node_name,
                                                 .srb2_cfg                       = cfg.srb2_cfg,
                                                 .drb_config                     = cfg.drb_config,
                                                 .int_algo_pref_list             = cfg.int_algo_pref_list,
                                                 .enc_algo_pref_list             = cfg.enc_algo_pref_list,
                                                 .force_reestablishment_fallback = cfg.force_reestablishment_fallback,
                                                 .force_resume_fallback          = cfg.force_resume_fallback,
                                                 .rrc_procedure_guard_time_ms    = cfg.rrc_procedure_guard_time_ms,
                                                 .rrc_reject_wait_time           = cfg.rrc_reject_wait_time,
                                                 .rrc_version                    = cfg.rrc_version,
                                                 .f1ap                           = cfg.f1ap};
  du_processor_dependencies     du_processor_deps{.cu_cp_executor           = cu_cp_executor,
                                                  .timers                   = timers,
                                                  .du_setup_notif           = du_conn_notif,
                                                  .du_cfg_hdlr              = du_cfg_mng.create_du_handler(),
                                                  .cu_cp_notifier           = du_ctxt.du_to_cu_cp_notifier,
                                                  .f1ap_pdu_notifier        = *du_ctxt.f1ap_tx_pdu_notifier,
                                                  .common_task_sched        = common_task_sched,
                                                  .ue_mng                   = ue_mng,
                                                  .ref_time_report_notifier = ref_time_report_notifier,
                                                  .logger                   = logger};
  std::unique_ptr<du_processor> du = create_du_processor(du_processor_cfg, std::move(du_processor_deps));

  ocudu_assert(du != nullptr, "Failed to create DU processor");
  du_ctxt.processor = std::move(du);

  return du_index;
}

async_task<void> du_processor_repository::remove_du(cu_cp_du_index_t du_index)
{
  ocudu_assert(du_index != cu_cp_du_index_t::invalid, "Invalid du_index={}", du_index);
  logger.debug("Removing DU {}...", du_index);

  return launch_async([this, du_index](coro_context<async_task<void>>& ctx) {
    CORO_BEGIN(ctx);

    // Remove DU
    if (du_db.find(du_index) == du_db.end()) {
      logger.warning("Remove DU called for inexistent du_index={}", du_index);
      return;
    }

    // Stop DU activity, eliminating pending transactions for the DU and respective UEs.
    CORO_AWAIT(du_db.find(du_index)->second.processor->get_f1ap_handler().stop());

    // De-realize the DU's logical cells, keeping their operator intent (admin lock/barring) so it can be
    // re-applied when the DU reconnects.
    cu_cp_du_handler.handle_du_removed(du_index);

    // Remove DU
    du_db.erase(du_index);
    logger.info("Removed DU {}", du_index);

    // Notify the CU-CP that the cells served by the connected DUs changed.
    cu_cp_du_handler.handle_served_cells_updated();

    CORO_RETURN();
  });
}

cu_cp_du_index_t du_processor_repository::get_next_du_index()
{
  for (unsigned du_idx_int = cu_cp_du_index_to_uint(cu_cp_du_index_t::min), e = cfg.max_nof_dus; du_idx_int != e;
       ++du_idx_int) {
    cu_cp_du_index_t du_idx = uint_to_cu_cp_du_index(du_idx_int);
    if (du_db.find(du_idx) == du_db.end()) {
      return du_idx;
    }
  }
  return cu_cp_du_index_t::invalid;
}

cu_cp_du_index_t du_processor_repository::find_du(pci_t pci) const
{
  cu_cp_du_index_t index = cu_cp_du_index_t::invalid;
  for (const auto& du : du_db) {
    if (du.second.processor->has_cell(pci)) {
      return du.first;
    }
  }

  return index;
}

cu_cp_du_index_t du_processor_repository::find_du(const nr_cell_global_id_t& cgi) const
{
  cu_cp_du_index_t index = cu_cp_du_index_t::invalid;
  for (const auto& du : du_db) {
    if (du.second.processor->has_cell(cgi)) {
      return du.first;
    }
  }

  return index;
}

cu_cp_du_index_t du_processor_repository::find_du_any_state(const nr_cell_global_id_t& cgi)
{
  for (const auto& du : du_db) {
    if (du.second.processor->has_cell_any_state(cgi)) {
      return du.first;
    }
  }
  return cu_cp_du_index_t::invalid;
}

du_processor* du_processor_repository::find_du_processor(cu_cp_du_index_t du_index)
{
  if (du_db.find(du_index) == du_db.end()) {
    return nullptr;
  }
  return du_db.at(du_index).processor.get();
}

du_processor& du_processor_repository::get_du_processor(cu_cp_du_index_t du_index)
{
  ocudu_assert(du_index != cu_cp_du_index_t::invalid, "Invalid du_index={}", du_index);
  ocudu_assert(du_db.find(du_index) != du_db.end(), "DU not found du_index={}", du_index);
  return *du_db.at(du_index).processor;
}

std::vector<cu_cp_du_index_t> du_processor_repository::get_du_processor_indexes() const
{
  std::vector<cu_cp_du_index_t> du_indexes;
  du_indexes.reserve(du_db.size());
  for (const auto& du : du_db) {
    du_indexes.push_back(du.first);
  }

  return du_indexes;
}

std::vector<cu_cp_served_cell_info> du_processor_repository::get_served_cells()
{
  std::vector<cu_cp_served_cell_info> served_cells;
  for (const auto& [du_index, du_ctxt] : du_db) {
    const du_configuration_context* du_cfg = du_ctxt.processor->get_context();
    if (du_cfg == nullptr) {
      // DU has not completed F1 setup.
      continue;
    }
    for (const du_cell_configuration& cell : du_cfg->served_cells) {
      cu_cp_served_cell_info& served_cell = served_cells.emplace_back();
      served_cell.nr_cgi                  = cell.cgi;
      served_cell.nr_pci                  = cell.pci;
      served_cell.five_gs_tac             = cell.tac;
      served_cell.served_plmns            = cell.served_plmns;
      served_cell.nr_mode_info            = cell.nr_mode_info;
      served_cell.meas_timing_cfg         = cell.meas_timing_cfg.copy();
    }
  }

  return served_cells;
}

std::vector<cu_cp_metrics_report::du_info> du_processor_repository::handle_du_metrics_report_request() const
{
  if (!cfg.enable_rrc_metrics) {
    return {};
  }

  std::vector<cu_cp_metrics_report::du_info> du_reports;
  du_reports.reserve(du_db.size());
  for (const auto& du : du_db) {
    du_reports.emplace_back(du.second.processor->get_metrics_handler().handle_du_metrics_report_request());
  }
  return du_reports;
}

size_t du_processor_repository::get_nof_f1ap_ues() const
{
  size_t nof_ues = 0;
  for (auto& du : du_db) {
    nof_ues += du.second.processor->get_f1ap_handler().get_nof_ues();
  }
  return nof_ues;
}

size_t du_processor_repository::get_nof_rrc_ues() const
{
  size_t nof_ues = 0;
  for (auto& du : du_db) {
    nof_ues += du.second.processor->get_rrc_du_handler().get_nof_ues();
  }
  return nof_ues;
}
