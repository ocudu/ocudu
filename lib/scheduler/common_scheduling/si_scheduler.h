// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../support/paging_helpers.h"
#include "si_message_scheduler.h"
#include "sib1_scheduler.h"
#include "ocudu/adt/lockfree_triple_buffer.h"
#include "ocudu/scheduler/scheduler_sys_info_handler.h"

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
  /// Request written into \c pending_pws_req, tagged with a locally-generated version for newness detection.
  struct pws_pending_request {
    si_version_type         version;
    unsigned                si_msg_idx;
    std::optional<unsigned> nof_segments;
  };

  void try_handle_pending_request(cell_resource_allocator& res_alloc);

  /// \return Returns true if a short message is due for transmission.
  bool try_handle_si_mod_request(slot_point sl_tx, unsigned max_dl_slot_alloc_delay);

  /// \brief Checks for a new PWS broadcast request and, if found, activates the target SI-message for one broadcast.
  /// \return Returns true if a short message is due for transmission.
  bool try_handle_pending_pws_request();

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

  si_version_type                             next_pws_version = 1;
  si_version_type                             last_pws_version = 0;
  lockfree_triple_buffer<pws_pending_request> pending_pws_req{pws_pending_request{0, 0, std::nullopt}};
  /// Whether a PWS (ETWS/CMAS) short-message notification is still pending transmission at a paging occasion.
  bool pws_short_msg_pending = false;

  // Note: We use counts instead of slot_points because SI periods can be longer that 1024 * 10 msec.
  unsigned   slot_count = 0;
  slot_point last_sl_tx;
};

} // namespace ocudu
