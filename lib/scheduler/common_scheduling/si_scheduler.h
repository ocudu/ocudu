// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../support/paging_helpers.h"
#include "si_message_scheduler.h"
#include "sib1_scheduler.h"
#include "ocudu/adt/lockfree_triple_buffer.h"
#include "ocudu/adt/slotted_vector.h"
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

  void run_slot(cell_resource_allocator& res_alloc);

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

  void try_handle_pending_request(cell_resource_allocator& res_alloc);

  /// \return Returns true if a short message is due for transmission.
  bool try_handle_si_mod_request(slot_point sl_tx, unsigned max_dl_slot_alloc_delay);

  /// \brief Checks for a new PWS broadcast request and, if found, activates the target SI-message for one broadcast.
  /// \return Returns true if a short message is due for transmission.
  bool try_handle_pending_pws_request(unsigned max_dl_slot_alloc_delay);

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
  unsigned                                    si_change_start_count = 0;

  si_version_type next_pws_version = 1;
  /// One pending-request slot per SI-message that requires activation, keyed by SI-message index.
  slotted_vector<pws_pending_entry> pending_pws_reqs;
  /// \brief Slot count (see \c slot_count) up to which the PWS (ETWS/CMAS) short-message notification must keep
  /// being transmitted at every paging occasion.
  /// \remark As per TS 38.304, ETWS/CMAS-capable UEs in RRC_IDLE/RRC_INACTIVE monitor for this notification only in
  /// their own paging occasion, once per DRX cycle; since the network does not know a given UE's UE_ID (hence its
  /// exact paging occasion), the notification must be repeated across a full default paging cycle to guarantee every
  /// UE is covered -- mirroring the systemInfoModification window (see \c si_change_start_count).
  unsigned pws_notif_until_count = 0;

  // Note: We use counts instead of slot_points because SI periods can be longer that 1024 * 10 msec.
  unsigned   slot_count = 0;
  slot_point last_sl_tx;
};

} // namespace ocudu
