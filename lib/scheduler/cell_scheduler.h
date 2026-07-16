// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cell/resource_grid.h"
#include "common_scheduling/csi_rs_scheduler.h"
#include "common_scheduling/paging_scheduler.h"
#include "common_scheduling/prach_scheduler.h"
#include "common_scheduling/ra_scheduler.h"
#include "common_scheduling/si_scheduler.h"
#include "common_scheduling/ssb_scheduler.h"
#include "config/cell_configuration.h"
#include "logging/cell_event_tracer.h"
#include "logging/cell_metrics_handler.h"
#include "logging/scheduler_event_logger.h"
#include "logging/scheduler_result_logger.h"
#include "pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "pucch_scheduling/pucch_allocator_impl.h"
#include "srs/srs_allocator_impl.h"
#include "uci_scheduling/uci_allocator_impl.h"
#include "ue_scheduling/ue_scheduler.h"

namespace ocudu {

/// \brief This class holds all the resources that are specific to a cell.
/// This includes the SIB and RA scheduler objects, PDCCH scheduler object, the cell resource grid, etc.
class cell_scheduler
{
public:
  explicit cell_scheduler(const scheduler_expert_config&                  sched_cfg,
                          const sched_cell_configuration_request_message& msg,
                          const cell_configuration&                       cell_cfg,
                          ue_scheduler&                                   ue_sched);

  /// Handle a slot indication for this cell.
  void run_slot(slot_point_extended sl_tx);

  /// Handle an error indication for this cell.
  void handle_error_indication(slot_point sl_tx, scheduler_slot_handler::error_outcome event);

  /// Activate cell.
  void start();

  /// Deactivate cell.
  void stop();

  const sched_result& last_result() const { return res_grid[0].result; }

  void handle_si_update_request(const si_scheduling_update_request& msg);

  void handle_pws_broadcast_indication(const pws_broadcast_request& req);

  void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& slice_reconf_req);

  void handle_cfra_mapping(du_ue_index_t ue_index, rnti_t crnti) { ra_sch.handle_cfra_mapping_update(ue_index, crnti); }
  void handle_rach_indication(const rach_indication_message& msg) { ra_sch.handle_rach_indication(msg); }

  void handle_crc_indication(const ul_crc_indication& crc_ind);

  void handle_paging_information(const sched_paging_information& pi) { pg_sch.handle_paging_information(pi); }

  scheduler_feedback_handler&         get_feedback_handler() { return ue_sched->get_feedback_handler(); }
  scheduler_cell_positioning_handler& get_positioning_handler() { return ue_sched->get_positioning_handler(); }
  scheduler_dl_buffer_state_indication_handler& get_dl_buffer_state_indication_handler()
  {
    return ue_sched->get_dl_buffer_state_indication_handler();
  }
  sched_ue_configuration_handler& get_ue_configurator() { return ue_sched->get_ue_configurator(); }

  const cell_configuration& cell_cfg;

private:
  void reset_resource_grid(slot_point sl_tx);

  /// Resource grid of this cell.
  cell_resource_allocator res_grid;

  /// Logger of cell events and scheduling results.
  scheduler_event_logger event_logger;
  /// Sink for the slot metrics of this cell. Owned here so that its lifetime matches the cell.
  cell_metrics_handler                           metrics;
  scheduler_result_logger                        result_logger;
  std::unique_ptr<schedtrace::cell_event_tracer> cell_tracer;
  ocudulog::basic_logger&                        logger;

  ssb_scheduler                 ssb_sch;
  pdcch_resource_allocator_impl pdcch_sch;
  si_scheduler                  si_sch;
  csi_rs_scheduler              csi_sch;
  ra_scheduler                  ra_sch;
  prach_scheduler               prach_sch;
  pucch_allocator_impl          pucch_alloc;
  uci_allocator_impl            uci_alloc;
  srs_allocator_impl            srs_alloc;
  paging_scheduler              pg_sch;

  /// Reference to UE scheduler whose DU cell group contains this cell.
  ue_scheduler::unique_cell_ptr ue_sched;

  /// Current state of the cell.
  bool active = false;
};

} // namespace ocudu
