// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../support/paging_helpers.h"
#include "si_message_scheduler.h"
#include "sib1_scheduler.h"
#include "ocudu/adt/slotted_vector.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/scheduler/scheduler_sys_info_handler.h"
#include <memory>

namespace ocudu {

struct si_scheduling_update_request;

/// Class responsible for scheduling SIB1 and SI-messages given the cell configuration.
class si_scheduler
{
public:
  si_scheduler(const cell_configuration&                       cfg_,
               pdcch_resource_allocator&                       pdcch_sch,
               const sched_cell_configuration_request_message& msg);

  /// \param hyper_sfn_tx HyperSFN for the slot provided in the current slot indication.
  void run_slot(cell_resource_allocator& res_alloc, uint32_t hyper_sfn_tx);

  /// \note Must be called from the cell scheduler executor.
  void handle_si_update_request(const si_scheduling_update_request& req);

  /// \note Must be called from the cell scheduler executor.
  void handle_pws_si_update_request(const pws_si_scheduling_update_request& req);

  void stop();

private:
  void try_handle_pending_request(cell_resource_allocator& res_alloc, slot_point_extended sl_tx_ext);

  /// \param slot_sched Slot at which the SI change, if any, would take effect (already offset by
  /// max_dl_slot_alloc_delay).
  /// \return Returns true if a short message is due for transmission.
  bool try_handle_si_mod_request(slot_point_extended slot_sched);

  /// \brief Applies any pending ETWS/CMAS SI change requests
  /// \note The SI scheduler reverts to the normal operation one once every warning finished being
  /// broadcast.
  /// \param slot_sched Slot at which the epoch, if any, is being processed (already offset by
  /// max_dl_slot_alloc_delay).
  /// \return Returns true if a short message is due for transmission.
  bool try_handle_pending_pws_request(slot_point_extended slot_sched);

  void try_schedule_short_message(cell_slot_resource_allocator& slot_alloc,
                                  bool                          include_si_modification,
                                  bool                          include_pws_indication);
  void allocate_short_message(cell_slot_resource_allocator& slot_alloc,
                              bool                          include_si_modification,
                              bool                          include_pws_indication);

  const cell_configuration& cell_cfg;
  const subcarrier_spacing  scs_common;
  const paging_slot_helper  paging_helper;
  const unsigned            default_paging_cycle;
  const unsigned            si_change_mod_period;
  pdcch_resource_allocator& pdcch_sch;
  ocudulog::basic_logger&   logger;

  sib1_scheduler       sib1_sched;
  si_message_scheduler si_msg_sched;

  si_version_type last_version = 0;
  /// Newest SI change request pending to be handled. A request that arrives while another one is on-going replaces it.
  std::optional<si_scheduling_update_request> pending_req;

  std::optional<si_scheduling_update_request> on_going_req;
  /// Slot at which the on-going SI change modification window starts. Only meaningful while \c on_going_req has a
  /// value.
  slot_point_extended si_change_start_slot;

  /// Applies a pending ETWS/CMAS SI epoch, and reverts to the normal operation one once no warning is being broadcast.
  void handle_pws_epoch(slot_point_extended slot_sched);

  /// \brief Rearms the deadline of every warning that the given epoch (re)started the broadcast of.
  /// \return Whether any warning started one more broadcast.
  bool refresh_pws_deadlines(const pws_si_scheduling_update_request& epoch, slot_point_extended slot_sched);

  /// \brief Number of radio frames a warning must keep being broadcast for, counted from the start of its broadcast.
  /// \return \c std::nullopt if it must be broadcast indefinitely.
  std::optional<unsigned> compute_pws_broadcast_duration(const pws_broadcasting_si_message& warning,
                                                         const si_scheduling_config&        si_sched_cfg) const;

  /// Whether every warning of the ETWS/CMAS SI epoch in effect finished being broadcast.
  bool all_pws_broadcasts_ended(slot_point_extended slot_sched) const;

  /// Newest ETWS/CMAS SI epoch pending to be applied. A newer epoch replaces one not yet applied.
  std::optional<pws_si_scheduling_update_request> pending_pws_epoch;

  /// Deadline of the broadcast of one warning, held so that each warning is timed from its own broadcast.
  struct pws_broadcast_deadline {
    /// SI message carrying the warning.
    sib_type_set sib_set;
    /// Epoch version that last started a broadcast of this warning.
    si_version_type version = 0;
    /// Slot until which it must keep being broadcast. \c std::nullopt if indefinitely.
    std::optional<slot_point_extended> until;
  };

  /// \brief Deadline of each warning being broadcast in the ETWS/CMAS SI epoch.
  ///
  /// The epoch is only reverted once every warning elapsed, so a warning whose own broadcast completed keeps being
  /// broadcast until the last one does. That way the set of SI messages listed as broadcasting in SIB1 only ever grows
  /// while the epoch is in effect, and can never claim a warning that is no longer on air.
  static_vector<pws_broadcast_deadline, MAX_PWS_SI_MESSAGES> pws_deadlines;
  /// \brief Slot up to which the PWS (ETWS/CMAS) short-message notification must keep being transmitted at every
  /// paging occasion. \c std::nullopt if no notification is currently pending.
  std::optional<slot_point_extended> pws_notif_until_slot;

  slot_point last_sl_tx;
};

} // namespace ocudu
