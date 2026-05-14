// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "cell_scheduler.h"
#include "logging/scheduler_metrics_handler.h"

using namespace ocudu;

cell_scheduler::cell_scheduler(const scheduler_expert_config&                  sched_cfg,
                               const sched_cell_configuration_request_message& msg,
                               const cell_configuration&                       cell_cfg_,
                               ue_scheduler&                                   ue_sched_,
                               cell_metrics_handler&                           metrics_handler) :
  cell_cfg(cell_cfg_),
  res_grid(cell_cfg),
  event_logger(cell_cfg.cell_index, cell_cfg.params.pci),
  metrics(metrics_handler),
  result_logger(sched_cfg.log_broadcast_messages, cell_cfg.params.pci),
  logger(ocudulog::fetch_basic_logger("SCHED")),
  ssb_sch(cell_cfg),
  pdcch_sch(cell_cfg),
  si_sch(cell_cfg, pdcch_sch, msg),
  csi_sch(cell_cfg),
  ra_sch(cell_cfg, pdcch_sch, event_logger, metrics),
  prach_sch(cell_cfg),
  pucch_alloc(cell_cfg, sched_cfg.ue.max_pucchs_per_slot, sched_cfg.ue.max_ul_grants_per_slot),
  uci_alloc(cell_cfg, pucch_alloc),
  // The SRS allocator is only used if srs_prohibit_time is set.
  srs_alloc(cell_cfg, sched_cfg.ue.srs_prohibit_time),
  pg_sch(cell_cfg, pdcch_sch)
{
  // Register new cell in the UE scheduler.
  ue_sched = ue_sched_.add_cell(ue_cell_scheduler_creation_request{
      msg.cell_index, &pdcch_sch, &pucch_alloc, &uci_alloc, &srs_alloc, &res_grid, &metrics, &event_logger});
}

void cell_scheduler::handle_si_update_request(const si_scheduling_update_request& msg)
{
  si_sch.handle_si_update_request(msg);
}

void cell_scheduler::handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& slice_reconf_req)
{
  ue_sched->handle_slice_reconfiguration_request(slice_reconf_req);
}

void cell_scheduler::handle_crc_indication(const ul_crc_indication& crc_ind)
{
  // Forward CRCs to RA scheduler. RA scheduler will auto-select the ones associated with the RA procedure.
  ra_sch.handle_crc_indication(crc_ind);
  // Forward CRCs to UE scheduler.
  ue_sched->get_feedback_handler().handle_crc_indication(crc_ind);
}

void cell_scheduler::run_slot(slot_point_extended sl_tx_ext)
{
  // Mark the start of the slot.
  slot_point sl_tx         = sl_tx_ext.without_hyper_sfn();
  auto       slot_start_tp = std::chrono::high_resolution_clock::now();

  // If there are skipped slots, handle them. Otherwise, the cell grid and cached results are not correctly cleared.
  if (OCUDU_LIKELY(res_grid.slot_tx().valid())) {
    while (OCUDU_UNLIKELY(res_grid.slot_tx() + 1 != sl_tx)) {
      slot_point skipped_slot = res_grid.slot_tx() + 1;
      logger.info("cell={}: Detected skipped slot={}.", cell_cfg.cell_index, skipped_slot);
      reset_resource_grid(skipped_slot);
    }
  } else {
    if (OCUDU_UNLIKELY(not active)) {
      // Implicitly activate cell on slot_indication.
      start();
    }
  }

  // > Start with clearing old allocations from the grid.
  reset_resource_grid(sl_tx);

  // > SSB scheduling.
  ssb_sch.run_slot(res_grid, sl_tx);

  // > Schedule CSI-RS.
  csi_sch.run_slot(res_grid[0]);

  // > Schedule SIB1 and SI-message signalling.
  si_sch.run_slot(res_grid);

  // > Schedule PRACH PDUs.
  prach_sch.run_slot(res_grid);

  // > Schedule RARs and Msg3.
  ra_sch.run_slot(res_grid);

  // > Schedule Paging.
  pg_sch.run_slot(res_grid, sl_tx_ext.hyper_sfn());

  // > Schedule UE DL and UL data.
  ue_sched->run_slot(sl_tx);

  // > Mark stop of the slot processing
  auto slot_stop_tp = std::chrono::high_resolution_clock::now();
  auto slot_dur     = std::chrono::duration_cast<std::chrono::microseconds>(slot_stop_tp - slot_start_tp);

  // > Log processed events.
  event_logger.log();

  // > Log the scheduler results.
  result_logger.on_scheduler_result(last_result(), slot_dur);

  // > Push the scheduler results to the metrics handler.
  metrics.push_result(sl_tx_ext, last_result(), slot_dur);
}

void cell_scheduler::handle_error_indication(slot_point sl_tx, scheduler_slot_handler::error_outcome event)
{
  ue_sched->handle_error_indication(sl_tx, event);
}

void cell_scheduler::reset_resource_grid(slot_point sl_tx)
{
  // Reset cell resource grid.
  res_grid.slot_indication(sl_tx);

  // Reset PDCCH slot context.
  pdcch_sch.slot_indication(sl_tx);

  // Reset PUCCH slot context.
  pucch_alloc.slot_indication(sl_tx);

  // Reset UCI slot context.
  uci_alloc.slot_indication(sl_tx);

  // Reset SRS slot context.
  srs_alloc.slot_indication(sl_tx);
}

void cell_scheduler::start()
{
  if (active) {
    return;
  }
  active = true;
  logger.info("cell={}: Cell scheduling was activated.", fmt::underlying(cell_cfg.cell_index));

  ue_sched->start();
}

void cell_scheduler::stop()
{
  // From this point onwards, no slot indications are expected until the cell is reenabled.

  if (not active) {
    // Do nothing.
    return;
  }
  active = false;
  logger.info("cell={}: Cell scheduling was deactivated.", fmt::underlying(cell_cfg.cell_index));

  // Stop sub-schedulers.
  ssb_sch.stop();
  si_sch.stop();
  prach_sch.stop();
  ra_sch.stop();
  pg_sch.stop();
  ue_sched->stop();

  // Reset resource grid and sub-allocators.
  res_grid.stop();
  pdcch_sch.stop();
  pucch_alloc.stop();
  uci_alloc.stop();

  // Report last metrics.
  metrics.handle_cell_deactivation();
}
