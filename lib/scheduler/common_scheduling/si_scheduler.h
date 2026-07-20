// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../support/paging_helpers.h"
#include "si_message_scheduler.h"
#include "sib1_scheduler.h"
#include "ocudu/adt/lockfree_triple_buffer.h"
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

  void handle_si_update_request(const si_scheduling_update_request& req);

  void handle_pws_broadcast_indication(const pws_broadcast_request& req);

  void stop();

private:
  /// Request written into a \c pending_pws_reqs slot, tagged with a locally-generated version for newness detection.
  struct pws_pending_request {
    si_version_type         version = 0;
    std::optional<unsigned> nof_segments;
    units::bytes            msg_len{0};
  };

  /// \brief Per-SI-message pending-request slot, keyed by SI-message index.
  /// \remark \c buffer is stored as \c unique_ptr because \c lockfree_triple_buffer is non-movable, and slot values
  /// must be movable to live inside a \c slotted_vector.
  struct pws_pending_entry {
    unsigned                                                     si_msg_idx        = 0;
    si_version_type                                              last_seen_version = 0;
    std::unique_ptr<lockfree_triple_buffer<pws_pending_request>> buffer =
        std::make_unique<lockfree_triple_buffer<pws_pending_request>>();

    explicit pws_pending_entry(unsigned si_msg_idx_) : si_msg_idx(si_msg_idx_) {}
  };

  void try_handle_pending_request(cell_resource_allocator& res_alloc, slot_point_extended sl_tx_ext);

  /// \param slot_sched Slot at which the SI change, if any, would take effect (already offset by
  /// max_dl_slot_alloc_delay).
  /// \return Returns true if a short message is due for transmission.
  bool try_handle_si_mod_request(slot_point_extended slot_sched);

  /// \brief Checks for a new PWS broadcast request and, if found, activates the target SI-message for one broadcast.
  /// \param slot_sched Slot at which the SI-message activation, if any, is being processed (already offset by
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

  si_version_type                                      last_version = 0;
  lockfree_triple_buffer<si_scheduling_update_request> pending_req;

  std::optional<si_scheduling_update_request> on_going_req;
  /// Slot at which the on-going SI change modification window starts. Only meaningful while \c on_going_req has a
  /// value.
  slot_point_extended si_change_start_slot;

  si_version_type next_pws_version = 1;
  /// One pending-request slot per SI-message that requires activation, keyed by SI-message index.
  slotted_vector<pws_pending_entry> pending_pws_reqs;
  /// \brief Slot up to which the PWS (ETWS/CMAS) short-message notification must keep being transmitted at every
  /// paging occasion. \c std::nullopt if no notification is currently pending.
  std::optional<slot_point_extended> pws_notif_until_slot;

  slot_point last_sl_tx;
};

} // namespace ocudu
