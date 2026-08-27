// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../config/sched_config_manager.h"
#include "../configured_grant/configured_grant_scheduler.h"
#include "../slicing/inter_slice_scheduler.h"
#include "ue_fallback_scheduler.h"
#include "ue_scheduler.h"
#include "ocudu/adt/mpmc_queue.h"
#include "ocudu/adt/unique_function.h"
#include "ocudu/ran/du_types.h"

namespace ocudu {

class cell_metrics_handler;
class scheduler_event_logger;
class uci_scheduler_impl;
class srs_scheduler;
class pdu_indication_pool;
class uci_indication_selector;
class ra_ue_repository;
class ue_cell_repository;

struct cell_creation_event {
  cell_resource_allocator& cell_res_grid;
  ue_cell_repository&      ue_cell_db;
  ue_fallback_scheduler&   fallback_sched;
  uci_scheduler_impl&      uci_sched;
  inter_slice_scheduler&   slice_sched;
  srs_scheduler&           srs_sched;
  /// Configured Grant scheduler. Nullptr if CG is not configured for the cell.
  configured_grant_scheduler* cg_sched;
  cell_metrics_handler&       metrics;
  scheduler_event_logger&     ev_logger;
  ra_ue_repository&           ra_ue_repo;
};

class ue_event_manager;

/// Handler of UE events for a given cell.
class ue_cell_event_manager final : public sched_ue_configuration_handler,
                                    public ue_feedback_handler,
                                    public cell_ue_event_notifier,
                                    public scheduler_dl_buffer_state_indication_handler
{
public:
  ue_cell_event_manager(ue_event_manager&          parent_,
                        const cell_creation_event& cell_ev,
                        ue_repository&             ue_db,
                        ocudulog::basic_logger&    logger);
  ~ue_cell_event_manager() override;

  /// Activate cell event processing.
  void start() { active.store(true, std::memory_order_release); }

  /// Deactivate cell event processing and clear any pending events.
  void stop();

  /// Process pending events when a slot indication is received for the given cell.
  void run_slot(slot_point sl_tx);

  /// UE Add/Mod/Remove interface.
  void handle_ue_creation(ue_config_update_event ev) override;
  void handle_ue_reconfiguration(ue_config_update_event ev) override;
  void handle_ue_deletion(ue_config_delete_event ev) override;
  void handle_ue_config_applied(du_cell_index_t pcell_idx, du_ue_index_t ue_idx) override;
  void handle_ue_deactivation_request(du_cell_index_t pcell_idx, du_ue_index_t ue_idx) override;

  // scheduler_feedback_handler methods.
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override;
  void handle_ul_phr_indication(const ul_phr_indication_message& phr_ind) override;
  void handle_ul_ta_report_indication(const ul_ta_report_indication_message& ta_report_ind) override;
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce) override;
  void handle_crnti_ce_received(du_ue_index_t ue_index) override;

  // cell_ue_event_notifier methods.
  void on_cfra_msg3_acked(du_ue_index_t ue_index) override;
  void on_conres_ce_acked(du_ue_index_t ue_index) override;
  void on_sr_detected(du_ue_index_t ue_index, slot_point uci_slot) override;
  void on_ul_n_ta_update(du_ue_index_t              ue_index,
                         time_alignment_group::id_t tag_id,
                         phy_time_unit              n_ta_diff,
                         float                      ul_sinr) override;

  // scheduler_dl_buffer_state_indication_handler methods.
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs) override;

  // Handle slice reconfiguration request.
  void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& req);

private:
  class ue_dl_buffer_occupancy_manager;

  /// Result of processing a cell event.
  enum class event_result { processed, invalid_ue, invalid_ue_cc };

  /// Type of event enqueued and handled by the scheduler.
  struct event_t {
    static constexpr size_t callback_capacity = 64;
    using callback_type                       = unique_function<event_result(), callback_capacity, true>;

    callback_type callback;
    const char*   ev_name = "invalid";
    /// UE index associated with the event. If INVALID_DU_UE_INDEX, the event is not associated with any UE.
    du_ue_index_t ue_index        = INVALID_DU_UE_INDEX;
    bool          warn_if_ignored = true;

    event_t() = default;
    template <typename Callable>
    event_t(const char* ev_name_, Callable&& callable, bool warn_if_ignored_ = true) :
      callback(std::forward<Callable>(callable)), ev_name(ev_name_), warn_if_ignored(warn_if_ignored_)
    {
    }
    template <typename Callable>
    event_t(const char* ev_name_, du_ue_index_t ue_index_, Callable&& callable, bool warn_if_ignored_ = true) :
      callback(std::forward<Callable>(callable)),
      ev_name(ev_name_),
      ue_index(ue_index_),
      warn_if_ignored(warn_if_ignored_)
    {
    }
  };

  /// Type used for the queue of pending events for a given cell.
  using event_queue = concurrent_queue<event_t, concurrent_queue_policy::lockfree_mpmc>;

  /// Enqueue a new cell event to be processed by the scheduler.
  void push_event(du_cell_index_t cell_index, event_t event);

  /// Log event with invalid UE index.
  void log_invalid_ue_index(du_ue_index_t ue_index, const char* event_name, bool warn_if_ignored = true) const;

  /// Log event when UE does not have a carrier for this cell.
  void log_invalid_cc(du_ue_index_t ue_idx, const char* event_name, bool warn_if_ignored = true) const;

  // shared parameters.
  ue_event_manager&       parent;
  ue_repository&          ue_db;
  ocudulog::basic_logger& logger;
  // cell parameters.
  const cell_configuration& cfg;
  ue_fallback_scheduler&    fallback_sched;
  uci_scheduler_impl&       uci_sched;
  inter_slice_scheduler&    slice_sched;
  srs_scheduler&            srs_sched;
  /// Configured Grant scheduler. Nullptr if CG is not configured for the cell.
  configured_grant_scheduler* cg_sched;
  cell_metrics_handler&       metrics;
  scheduler_event_logger&     ev_logger;
  ra_ue_repository&           ra_ue_repo;

  std::unique_ptr<pdu_indication_pool> ind_pdu_pool;

  std::unique_ptr<ue_dl_buffer_occupancy_manager> dl_bo_mng;

  event_queue pending_events;

  slot_point last_sl_tx;

  /// Whether the cell is currently processing events.
  std::atomic<bool> active = true;
};

/// \brief Class used to manage events that arrive to the scheduler and are directed at UEs.
/// This class acts as a facade for several of the ue_scheduler subcomponents, managing the asynchronous configuration
/// of the UEs and logging in a thread-safe manner.
class ue_event_manager
{
public:
  ue_event_manager(ue_repository& ue_db);

  std::unique_ptr<ue_cell_event_manager> add_cell(const cell_creation_event& cell_ev);

private:
  friend class ue_cell_event_manager;

  bool cell_exists(du_cell_index_t cell_index) const;

  ue_repository&          ue_db;
  ocudulog::basic_logger& logger;

  std::array<ue_cell_event_manager*, MAX_NOF_DU_CELLS> cells;
};

} // namespace ocudu
