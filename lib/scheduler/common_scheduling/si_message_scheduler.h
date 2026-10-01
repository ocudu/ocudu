// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../pdcch_scheduling/pdcch_resource_allocator.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/scheduler/scheduler_sys_info_handler.h"
#include "ocudu/support/units.h"
#include <optional>

namespace ocudu {

/// Version and SIB1 payload size of an SI epoch.
struct si_epoch_info {
  unsigned     version = 0;
  units::bytes sib1_payload_size{0};
};

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

  /// \brief Makes the ETWS/CMAS SI epoch the one in effect, activating only the SI messages it lists as broadcasting.
  ///
  /// The SI epoch of the normal operation is kept aside, and keeps being updated, so that it can be reverted to.
  void apply_pws_epoch(unsigned                                version,
                       const si_scheduling_config&             pws_si_sched_cfg,
                       span<const pws_broadcasting_si_message> broadcasting);

  /// \brief Makes the SI epoch of the normal operation the one in effect again, with every warning back to dormant.
  /// \return The epoch that is in effect afterwards.
  si_epoch_info revert_pws_epoch();

  /// Whether the ETWS/CMAS SI epoch is the one in effect.
  bool pws_epoch_in_effect() const { return baseline.has_value(); }

  /// SI epoch currently in effect.
  /// \brief Applies the PDSCH grant sizing (msg_len) from a new SI scheduling config, immediately and without touching
  /// window/active state or bumping version, but only for the NTN SI-message.
  /// \remark si_scheduling_update_request/handle_si_message_update_indication defer taking effect until a future SI
  /// change modification window (at least one full default paging cycle away, per TS 38.331, so idle UEs get advance
  /// notice via the short-message before SIB1's valueTag actually changes). For exempt SI-messages (e.g. NTN SIB19)
  /// MAC pushes the new content immediately at the next SI window, so their grant sizing must track it immediately too.
  void update_msg_lens(const si_scheduling_config& new_si_sched_cfg);

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
    /// \remark Initialized from \c si_message_scheduling_config::msg_len, and overridden by the ETWS/CMAS SI epoch
    /// while a warning is on air, since real content length is only known once the warning is pushed.
    units::bytes msg_len{0};
  };

  /// Slot until which a broadcast starting at the given slot must keep being transmitted.

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

  /// SI epoch of the normal operation, kept aside while the ETWS/CMAS one is in effect.
  struct baseline_epoch {
    unsigned             version = 0;
    si_scheduling_config si_sched_cfg;
  };

  std::vector<message_window_context> pending_messages;
  unsigned                            version = 0;
  std::optional<baseline_epoch>       baseline;
};

} // namespace ocudu
