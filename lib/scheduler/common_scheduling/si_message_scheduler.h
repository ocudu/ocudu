// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "ocudu/ocudulog/logger.h"
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
  void run_slot(cell_slot_resource_allocator& res_grid);

  /// \brief Update the SI messages.
  void handle_si_message_update_indication(unsigned version, const si_scheduling_config& new_si_sched_cfg);

  /// \brief Activates an SI-message that requires explicit activation (see \c si_message_scheduling_config).
  ///
  /// If \c nof_segments has a value, the SI-message is activated for exactly that many consecutive window
  /// transmissions, after which it automatically goes back to dormant until the next activation. If \c nof_segments
  /// is \c std::nullopt, the SI-message is activated indefinitely and never automatically deactivates (used for
  /// test_mode-configured content that should broadcast forever).
  /// \param msg_len Length, in bytes, of the largest segment of the content being activated.
  void activate_si_message(unsigned si_msg_idx, std::optional<unsigned> nof_segments, units::bytes msg_len);

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
    /// \brief Number of segments to transmit before the on-going activation goes back to dormant.
    /// \remark If \c std::nullopt, the activation never automatically goes back to dormant (broadcast forever).
    std::optional<unsigned> nof_segments;
    /// Number of window transmissions completed since the current activation.
    unsigned nof_tx = 0;
    /// \brief Length, in bytes, used to size the PDSCH grant for this SI-message.
    /// \remark Initialized from \c si_message_scheduling_config::msg_len, and overridden by \c activate_si_message
    /// while a PWS activation is on-going, since real content length is only known at activation time.
    units::bytes msg_len{0};
  };

  void update_si_message_windows(slot_point sl_tx);

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
