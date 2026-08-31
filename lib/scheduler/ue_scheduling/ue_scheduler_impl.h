// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../slicing/inter_slice_scheduler.h"
#include "../uci_scheduling/uci_indication_selector.h"
#include "../uci_scheduling/uci_scheduler_impl.h"
#include "../ue_context/ue_repository.h"
#include "cell_group_event_manager.h"
#include "intra_slice_scheduler.h"
#include "triggered_ul_grant_scheduler.h"
#include "ue_cell_grid_allocator.h"
#include "ue_fallback_scheduler.h"
#include "ue_scheduler.h"
#include "ocudu/scheduler/config/scheduler_expert_config.h"
#include <mutex>

namespace ocudu {

class configured_grant_scheduler_impl;

/// \brief Interface of data scheduler that is used to allocate UE DL and UL grants in a given slot.
/// The data_scheduler object will be common to all cells and slots.
class ue_scheduler_impl final : public ue_scheduler
{
public:
  explicit ue_scheduler_impl(const scheduler_ue_expert_config& expert_cfg_);

private:
  ue_cell_scheduler* do_add_cell(const ue_cell_scheduler_creation_request& params) override;

  void do_start_cell(du_cell_index_t cell_index);
  void do_stop_cell(du_cell_index_t cell_index);

  void do_rem_cell(du_cell_index_t cell_index) override;

  void run_slot_impl(slot_point sl_tx);

  void run_sched_strategy(du_cell_index_t cell_index);

  struct cell_context final : public ue_cell_scheduler {
    ue_scheduler_impl& parent;

    cell_resource_allocator* cell_res_alloc;

    /// Repository of UEs for this cell.
    ue_cell_repository& ue_cell_db;

    /// Fallback scheduler.
    ue_fallback_scheduler fallback_sched;

    /// Slice scheduler.
    inter_slice_scheduler slice_sched;

    /// Intra-slice scheduler.
    intra_slice_scheduler intra_slice_sched;

    /// Configured Grant scheduler.
    std::unique_ptr<configured_grant_scheduler_impl> cg_sched;

    /// Triggered UL grant sub-scheduler.
    triggered_ul_grant_scheduler trig_ul_sched;

    /// Handler of the events that this cell dispatches to the cell group.
    std::unique_ptr<cell_group_event_handler_impl> ev_handler;

    cell_context(ue_scheduler_impl& parent, const ue_cell_scheduler_creation_request& params);
    ~cell_context() override;

    cell_group_event_handler& get_event_handler() override { return *ev_handler; }

    void run_slot(slot_point sl_tx) override { parent.run_slot_impl(sl_tx); }

    void start() override { parent.do_start_cell(cell_res_alloc->cfg.cell_index); }

    void stop() override { parent.do_stop_cell(cell_res_alloc->cfg.cell_index); }
  };
  const scheduler_ue_expert_config& expert_cfg;
  ocudulog::basic_logger&           logger;

  // List of cells of the UE scheduler.
  slotted_array<cell_context, MAX_NOF_DU_CELLS> cells;

  /// Repository of created UEs.
  ue_repository ue_db;

  /// Processor of UE input events.
  cell_group_event_manager event_mng;

  // Mutex to lock cells of the same cell group (when CA enabled) for joint carrier scheduling
  std::mutex cell_group_mutex;

  // Last slot run.
  slot_point last_sl_ind;
};

} // namespace ocudu
