// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/support/units.h"
#include <optional>

namespace ocudu {

class si_message_scheduler
{
public:
  si_message_scheduler(const cell_configuration&   cfg_,
                       pdcch_resource_allocator&   pdcch_sch,
                       const si_scheduling_config& si_sched_cfg_);

  /// \brief Performs broadcast SI message scheduling.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param sl_tx_ext Extended (HyperSFN-aware) representation of \c res_grid.slot.
  void run_slot(cell_slot_resource_allocator& res_grid, slot_point_extended sl_tx_ext);

  /// \brief Update the SI messages.
  void handle_si_message_update_indication(unsigned version, const si_scheduling_config& new_si_sched_cfg);

  /// \brief Activates an SI-message that requires explicit activation (see \c si_message_scheduling_config).
  ///
  /// If \c nof_segments has a value, the SI-message is kept active for at least one full default paging cycle plus
  /// one full segment cycle (see \c message_window_context::active_until), after which it automatically goes back
  /// to dormant until the next activation. This -- rather than deactivating after exactly \c nof_segments window
  /// transmissions -- ensures single-round PWS delivery reaches UEs that are only notified (via the P-RNTI short
  /// message) near the end of the notification window (see \c si_scheduler::try_handle_pending_pws_request), since
  /// such UEs must still have a chance to observe a full cycle of segments afterwards. If \c nof_segments is
  /// \c std::nullopt, the SI-message is activated indefinitely and never automatically deactivates (used for
  /// test_mode-configured content that should broadcast forever).
  /// \param activation_slot Slot at which this activation is being processed, used as the origin for the
  /// aforementioned deadline.
  /// \param msg_len Length, in bytes, of the largest segment of the content being activated.
  void activate_si_message(unsigned                si_msg_idx,
                           slot_point_extended     activation_slot,
                           std::optional<unsigned> nof_segments,
                           units::bytes            msg_len);

  /// Called when cell is deactivated.
  void stop();

private:
  struct message_window_context {
    /// SI message window.
    interval<slot_point> window;
    /// Number of SI message transmissions within the current window.
    unsigned nof_tx_in_current_window = 0;
    /// Total number of SI message transmissions.
    unsigned long total_nof_tx = 0;
    /// \brief Whether this SI-message is currently active (i.e. allowed to be scheduled).
    /// \remark Always true for SI-messages that do not require explicit activation.
    bool active = false;
    /// \brief Slot at which the on-going activation must go back to dormant.
    /// \remark If \c std::nullopt, the activation never automatically goes back to dormant (broadcast forever). Not
    /// used for SI-messages that do not require explicit activation.
    std::optional<slot_point_extended> active_until;
    /// \brief Length, in bytes, used to size the PDSCH grant for this SI-message.
    /// \remark Initialized from \c si_message_scheduling_config::msg_len, and overridden by \c activate_si_message
    /// while a PWS activation is on-going, since real content length is only known at activation time.
    units::bytes msg_len{0};
  };

  void update_si_message_windows(slot_point_extended sl_tx_ext);

  void schedule_pending_si_messages(cell_slot_resource_allocator& res_grid);

  bool allocate_si_message(unsigned si_message, cell_slot_resource_allocator& res_grid);

  void fill_si_grant(cell_slot_resource_allocator& res_grid,
                     unsigned                      si_message,
                     crb_interval                  si_crbs_grant,
                     uint8_t                       time_resource,
                     const dmrs_information&       dmrs_info,
                     units::bytes                  tbs,
                     const message_window_context& message_context);

  // Configuration of the broadcast SI messages.
  const scheduler_si_expert_config& expert_cfg;
  const cell_configuration&         cell_cfg;
  pdcch_resource_allocator&         pdcch_sch;
  si_scheduling_config              si_sched_cfg;
  ocudulog::basic_logger&           logger;

  std::vector<message_window_context> pending_messages;
  unsigned                            version = 0;
};

} // namespace ocudu
