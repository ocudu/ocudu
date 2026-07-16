// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "si_scheduler.h"
#include "../support/dci_builder.h"
#include "ocudu/ran/pdcch/dci_packing.h"

using namespace ocudu;

si_scheduler::si_scheduler(const cell_configuration&                       cfg_,
                           pdcch_resource_allocator&                       pdcch_sch_,
                           const sched_cell_configuration_request_message& msg) :
  cell_cfg(cfg_),
  scs_common(cell_cfg.scs_common()),
  paging_helper(cfg_),
  default_paging_cycle(static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.default_paging_cycle)),
  si_change_mod_period(default_paging_cycle *
                       static_cast<unsigned>(cell_cfg.params.dl_cfg_common.bcch_cfg.mod_period_coeff)),
  pdcch_sch(pdcch_sch_),
  logger(ocudulog::fetch_basic_logger("SCHED")),
  sib1_sched(cell_cfg, pdcch_sch, msg.si_scheduling.sib1_payload_size),
  si_msg_sched(cell_cfg, pdcch_sch, msg.si_scheduling),
  pending_req(si_scheduling_update_request{INVALID_DU_CELL_INDEX, last_version, {}})
{
  // One slot per SI-message that requires activation (i.e. may carry PWS content).
  const unsigned nof_si_messages = msg.si_scheduling.si_messages.size();
  for (unsigned i = 0; i != nof_si_messages; ++i) {
    if (msg.si_scheduling.si_messages[i].requires_activation) {
      pending_pws_reqs.emplace(i, i);
    }
  }
}

void si_scheduler::run_slot(cell_resource_allocator& res_alloc, uint32_t hyper_sfn_tx)
{
  const slot_point_extended sl_tx_ext{res_alloc[0].slot, hyper_sfn_tx};

  if (OCUDU_UNLIKELY(not last_sl_tx.valid())) {
    // First call to run_slot.

    // Fill resource grid up to max_dl_slot_alloc_delay - 1, which corresponds to the grid DL size.
    for (unsigned i = 0, e = res_alloc.max_dl_slot_alloc_delay; i != e; ++i) {
      sib1_sched.run_slot(res_alloc[i]);
      si_msg_sched.run_slot(res_alloc[i], sl_tx_ext + i);
    }
  }
  last_sl_tx = res_alloc[0].slot;

  // If no on-going request is being handled, try to fetch a new request.
  try_handle_pending_request(res_alloc, sl_tx_ext);

  auto& slot_res_alloc = res_alloc[res_alloc.max_dl_slot_alloc_delay];

  // Run SIB1 scheduling
  sib1_sched.run_slot(slot_res_alloc);

  // Run SI-message scheduling.
  si_msg_sched.run_slot(slot_res_alloc, sl_tx_ext + res_alloc.max_dl_slot_alloc_delay);
}

void si_scheduler::stop()
{
  last_sl_tx = {};

  // Reset on-going requests.
  const auto& next = pending_req.read();
  last_version     = next.version;
  on_going_req.reset();

  sib1_sched.stop();
  si_msg_sched.stop();
}

void si_scheduler::try_handle_pending_request(cell_resource_allocator& res_alloc, slot_point_extended sl_tx_ext)
{
  // The SI is scheduled ahead of time.
  slot_point_extended slot_sched = sl_tx_ext + res_alloc.max_dl_slot_alloc_delay;

  // Handle any request to change the SI sched info.
  const bool si_modification_due = try_handle_si_mod_request(slot_sched);

  // Determine whether a PWS (ETWS/CMAS) short-message notification is due this slot.
  const bool pws_notif_due = try_handle_pending_pws_request(slot_sched);

  if (si_modification_due or pws_notif_due) {
    try_schedule_short_message(res_alloc[slot_sched.without_hyper_sfn()], si_modification_due, pws_notif_due);
  }
}

bool si_scheduler::try_handle_si_mod_request(slot_point_extended slot_sched)
{
  const unsigned slots_per_frame = get_nof_slots_per_subframe(scs_common) * NOF_SUBFRAMES_PER_FRAME;

  if (not on_going_req.has_value()) {
    // Not handling any new request at the moment. Check if there is any pending SI change request to handle.
    const auto& next = pending_req.read();
    if (next.version != last_version) {
      // New pushed request. Save request to be handled in the following slots.
      on_going_req = next;
      last_version = on_going_req->version;

      // Determine the start of SI change modification window.
      const unsigned nof_sfns_until_mod_window = si_change_mod_period - (slot_sched.sfn() % si_change_mod_period);
      si_change_start_slot = slot_sched + nof_sfns_until_mod_window * slots_per_frame - slot_sched.slot_index();
      if (si_change_start_slot < slot_sched + default_paging_cycle * slots_per_frame) {
        // The next modification window is too close to the current slot to leave enough time to broadcast short
        // messages to all UEs (assuming that we need at least one full default paging cycle to notify all UEs). Thus,
        // we delay the SI change by one full SI change period.
        si_change_start_slot += si_change_mod_period * slots_per_frame;
      }
    }
  }

  // Determine whether a systemInfoModification short-message notification is due this slot.
  if (not on_going_req.has_value()) {
    // No pending request to handle.
    return false;
  }

  if (slot_sched < si_change_start_slot - default_paging_cycle * slots_per_frame) {
    // The SI change indication (short message) signalling window has not started yet.
    return false;
  }

  if (slot_sched < si_change_start_slot) {
    // We are inside the SI change indication signalling window. Short message is due.
    return true;
  }

  // We are inside the SI change window.
  logger.debug("SI change with version {} starting after slot {}", on_going_req->version, slot_sched);

  // Apply the SIB1 and SI message changes.
  sib1_sched.handle_sib1_update_indication(on_going_req->version, on_going_req->si_sched_cfg.sib1_payload_size);
  si_msg_sched.handle_si_message_update_indication(on_going_req->version, on_going_req->si_sched_cfg);

  // Delete the on-going request.
  on_going_req = std::nullopt;

  return false;
}

void si_scheduler::handle_si_update_request(const si_scheduling_update_request& req)
{
  pending_req.write_and_commit(req);
}

void si_scheduler::handle_pws_broadcast_indication(const pws_broadcast_request& req)
{
  ocudu_assert(pending_pws_reqs.contains(req.si_msg_idx), "SI-message {} does not require activation", req.si_msg_idx);
  pending_pws_reqs[req.si_msg_idx].buffer->write_and_commit(
      pws_pending_request{next_pws_version++, req.nof_segments, req.msg_len});
}

bool si_scheduler::try_handle_pending_pws_request(slot_point_extended slot_sched)
{
  const unsigned slots_per_frame = get_nof_slots_per_subframe(scs_common) * NOF_SUBFRAMES_PER_FRAME;

  // Check every SI-message slot for a new request.
  for (auto& pws_entry : pending_pws_reqs) {
    const auto& next = pws_entry.buffer->read();
    if (next.version == pws_entry.last_seen_version) {
      continue;
    }
    // New request. Activate the target SI-message for one broadcast.
    pws_entry.last_seen_version = next.version;
    si_msg_sched.activate_si_message(pws_entry.si_msg_idx, slot_sched, next.nof_segments, next.msg_len);

    // As per TS 38.304, ETWS/CMAS-capable UEs monitor for this notification only in their own paging occasion, once
    // per DRX cycle. Since we don't know a given UE's UE_ID (hence its exact paging occasion), extend the
    // notification window to cover at least one full default paging cycle from now.
    const slot_point_extended new_until = slot_sched + default_paging_cycle * slots_per_frame;
    if (not pws_notif_until_slot.has_value() or pws_notif_until_slot.value() < new_until) {
      pws_notif_until_slot = new_until;
    }
  }

  if (not pws_notif_until_slot.has_value()) {
    return false;
  }
  if (slot_sched < pws_notif_until_slot.value()) {
    return true;
  }
  // Deadline has passed. Clear it rather than leave a stale value behind.
  pws_notif_until_slot.reset();
  return false;
}

void si_scheduler::try_schedule_short_message(cell_slot_resource_allocator& slot_alloc,
                                              bool                          include_si_modification,
                                              bool                          include_pws_indication)
{
  slot_point pdcch_slot = slot_alloc.slot;
  if (not cell_cfg.is_dl_enabled(pdcch_slot)) {
    // Skip UL slots.
    return;
  }
  // Verify there is space in PDSCH and PDCCH result lists for new allocations.
  if (slot_alloc.result.dl.dl_pdcchs.full()) {
    return;
  }

  // Check if this is a paging frame. Paging frames are computed according to the following formula:
  // (SFN + PF_offset) mod T = (T div N)*(UE_ID mod N). See TS 38.304, clause 7.1.
  // Note: We need to notify all UEs. Since we don't know the UE ID of UEs in RRC_IDLE, we send the change notification
  // in all paging occasions.
  // DRX cycle in radio frames.
  const unsigned drx_cycle            = default_paging_cycle;
  const unsigned nof_pf_per_drx_cycle = static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.nof_pf);
  const unsigned paging_frame_offset  = cell_cfg.params.dl_cfg_common.pcch_cfg.paging_frame_offset;
  // Number of total paging frames in a drx_cycle.
  const unsigned N = drx_cycle / nof_pf_per_drx_cycle;

  // If t_div_n doesn't evenly divide paging_offset_mod, there is no integer solution for UE_ID, so this is not a PF.
  const unsigned paging_offset_mod = (pdcch_slot.sfn() + paging_frame_offset) % drx_cycle;
  const unsigned t_div_n           = drx_cycle / N;
  if (paging_offset_mod % t_div_n != 0) {
    return;
  }

  // Number of paging occasions in a paging frame.
  const unsigned Ns = static_cast<unsigned>(cell_cfg.params.dl_cfg_common.pcch_cfg.ns);

  // Traverse all possible PO indices (i_s).
  // i_s = floor (UE_ID/N) mod Ns.
  // NOTE:
  //   - UE_ID [0, 1024).
  //   - Since T {32, 64, 128, 256} and nof_pf_per_drx_cycle {1, 2, 4, 8, 16}: N <= 256.
  //   => There will be *at least* 4 possible UE IDs (UE_ID mod N) with paging occasions on each PF.
  // Since Ns is at most 4, it is always guaranteed that all PO indices correspond to a valid UE_ID on each PF.
  for (unsigned i_s = 0; i_s != Ns; ++i_s) {
    // Determine if this slot is used by any PO index.
    if (paging_helper.is_paging_slot(pdcch_slot, i_s)) {
      logger.debug("Scheduling SI change notification Short Message at slot {}", slot_alloc.slot);
      allocate_short_message(slot_alloc, include_si_modification, include_pws_indication);
      break;
    }
  }
}

void si_scheduler::allocate_short_message(cell_slot_resource_allocator& slot_alloc,
                                          bool                          include_si_modification,
                                          bool                          include_pws_indication)
{
  const auto ss_id = cell_cfg.params.dl_cfg_common.init_dl_bwp.pdcch_common.paging_search_space_id.value();

  // > Allocate DCI_1_0 for Paging on PDCCH.
  pdcch_dl_information* pdcch =
      pdcch_sch.alloc_dl_pdcch_common(slot_alloc, rnti_t::P_RNTI, ss_id, cell_cfg.expert_cfg.pg.paging_dci_aggr_lev);
  if (pdcch == nullptr) {
    logger.warning("Could not allocate SI change notification Short Message in PDCCH");
    return;
  }

  // Fill Paging DCI, as per TS 38.331 Table 6.5-1.
  static constexpr unsigned si_modification_short_message = 0b10000000;
  static constexpr unsigned etws_cmas_short_message       = 0b01000000;

  unsigned short_message = 0;
  if (include_si_modification) {
    short_message |= si_modification_short_message;
  }
  if (include_pws_indication) {
    short_message |= etws_cmas_short_message;
  }
  build_dci_f1_0_p_rnti(pdcch->dci, cell_cfg.params.dl_cfg_common.init_dl_bwp, short_message);
}
