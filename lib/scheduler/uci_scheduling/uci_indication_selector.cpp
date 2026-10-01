// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "uci_indication_selector.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/scheduler/resource_grid_util.h"

using namespace ocudu;

uci_indication_selector::uci_indication_selector(uci_indication_timeout_notifier& timeout_notifier_,
                                                 unsigned                         uci_ack_timeout,
                                                 unsigned                         max_pucch_grants_per_slot,
                                                 unsigned                         max_nof_ue_contexts,
                                                 std::optional<float>             pucch_sinr_threshold_dB_) :
  ack_timeout_slots(uci_ack_timeout),
  pucch_sinr_threshold_dB(pucch_sinr_threshold_dB_),
  timeout_notifier(timeout_notifier_),
  logger(ocudulog::fetch_basic_logger("SCHED")),
  uci_wheel(get_pow2_allocator_ring_size_gt_min(ack_timeout_slots)),
  short_timeout_wheel(get_pow2_allocator_ring_size_gt_min(SHORT_PUCCH_TIMEOUT_SLOTS))
{
  uci_pool.reserve(
      std::min<unsigned>(ack_timeout_slots * std::min<unsigned>(max_pucch_grants_per_slot, MAX_PUCCH_PDUS_PER_SLOT),
                         max_nof_ue_contexts * MAX_NOF_HARQS));
  report_fatal_error_if_not(uci_ack_timeout > SHORT_PUCCH_TIMEOUT_SLOTS, "Invalid UCI ACK timeout");
}

uci_action uci_indication_selector::create_action(const uci_indication::uci_pdu& pdu) const
{
  uci_action ret;
  bool       is_dtx = false;
  if (const auto* f0f1 = std::get_if<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(&pdu.pdu)) {
    ret.type                = uci_action::pdu_type::pucch_f0f1;
    ret.ul_sinr_dB          = f0f1->ul_sinr_dB;
    ret.time_advance_offset = f0f1->time_advance_offset;
    ret.sr_detected         = f0f1->sr_detected;
    ret.harq_ack_bits.resize(f0f1->harqs.size());
    for (unsigned i = 0, e = f0f1->harqs.size(); i != e; ++i) {
      if (f0f1->harqs[i] == mac_harq_ack_report_status::ack) {
        ret.harq_ack_bits.set(i);
      }
      is_dtx |= f0f1->harqs[i] == mac_harq_ack_report_status::dtx;
    }
  } else if (const auto* f2f3f4 = std::get_if<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>(&pdu.pdu)) {
    ret.type                = uci_action::pdu_type::pucch_f2f3f4;
    ret.ul_sinr_dB          = f2f3f4->ul_sinr_dB;
    ret.time_advance_offset = f2f3f4->time_advance_offset;
    ret.sr_detected         = f2f3f4->sr_info.any();
    ret.harq_ack_bits.resize(f2f3f4->harqs.size());
    for (unsigned i = 0, e = f2f3f4->harqs.size(); i != e; ++i) {
      if (f2f3f4->harqs[i] == mac_harq_ack_report_status::ack) {
        ret.harq_ack_bits.set(i);
      }
      is_dtx |= f2f3f4->harqs[i] == mac_harq_ack_report_status::dtx;
    }
    ret.csi = f2f3f4->csi;
  } else {
    const auto& pusch = std::get<uci_indication::uci_pdu::uci_pusch_pdu>(pdu.pdu);
    ret.type          = uci_action::pdu_type::pusch;
    ret.harq_ack_bits.resize(pusch.harqs.size());
    for (unsigned i = 0, e = pusch.harqs.size(); i != e; ++i) {
      if (pusch.harqs[i] == mac_harq_ack_report_status::ack) {
        ret.harq_ack_bits.set(i);
      }
      is_dtx |= pusch.harqs[i] == mac_harq_ack_report_status::dtx;
    }
    ret.csi = pusch.csi;
  }

  // PUCCH PDUs whose estimated SINR is below the configured threshold are treated as if nothing was detected.
  const bool is_pucch = ret.type == uci_action::pdu_type::pucch_f0f1 or ret.type == uci_action::pdu_type::pucch_f2f3f4;
  if (is_pucch and pucch_sinr_threshold_dB.has_value() and ret.ul_sinr_dB.has_value() and
      ret.ul_sinr_dB.value() < pucch_sinr_threshold_dB.value()) {
    ret.sr_detected = false;
    ret.csi.reset();
    ret.uci_valid = false;
    return ret;
  }

  ret.uci_valid =
      (ret.sr_detected or (not ret.harq_ack_bits.empty() and not is_dtx) or (ret.csi.has_value() and ret.csi->valid));
  return ret;
}

static bool has_harq_ack_bits(const uci_indication::uci_pdu& pdu)
{
  if (auto* f0f1 = std::get_if<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(&pdu.pdu)) {
    return not f0f1->harqs.empty();
  }
  if (auto* f2f3f4 = std::get_if<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>(&pdu.pdu)) {
    return not f2f3f4->harqs.empty();
  }
  return not std::get<uci_indication::uci_pdu::uci_pusch_pdu>(pdu.pdu).harqs.empty();
}

std::optional<uci_action> uci_indication_selector::handle_uci_ind_pdu(slot_point                     sl_rx,
                                                                      const uci_indication::uci_pdu& pdu)
{
  // If the PDU has no HARQ-ACK bits, it was not registered for timeout tracking (e.g. SR-only, CSI-only or SR-CSI-only
  // PUCCH). In this case, create and return an action directly.
  if (not has_harq_ack_bits(pdu)) {
    uci_action action = create_action(pdu);
    action.uci_slot   = sl_rx;
    return action;
  }

  auto uci_r = uci_wheel[sl_rx.count()].get_list(uci_pool);
  for (auto prev = uci_r.before_begin(), it = uci_r.begin(); it != uci_r.end(); prev = it, ++it) {
    if (pdu.crnti != it->crnti or it->uci_slot != sl_rx) {
      continue;
    }
    // RNTIs match. The grant was found.
    stable_id_t id = it.id();

    if (uci_pool[id].is_burst()) {
      // The PUCCH is one of the repetitions of a multi-slot PUCCH repetition burst, which is tracked as a whole by its
      // anchor entry.
      const slot_point anchor_slot = uci_pool[id].burst_anchor_slot();
      if (not uci_pool[id].is_burst_anchor()) {
        // This entry has served its only purpose of pointing at the anchor entry.
        uci_r.erase_after(prev);
        uci_pool.erase(id);
      }
      return handle_burst_uci_pdu(anchor_slot, pdu);
    }

    uci_entry& entry = uci_pool[id];

    // Generate an action.
    auto action = handle_uci_pdu(pdu, entry);

    if (action.has_value()) {
      // An action was generated. It means that all the combining is complete and we can erase the UCI entry.
      uci_r.erase_after(prev);
      if (entry.short_timeout_slot.valid()) {
        short_timeout_wheel[entry.short_timeout_slot.count()].get_list(uci_pool).erase(id);
      }
      uci_pool.erase(id);
    } else {
      // No action was generated. This means that the UCI entry is still expecting more UCI indications.
      if (not entry.short_timeout_slot.valid()) {
        // We add the UCI grant in another linked list in the short timeout wheel, if not added yet.
        entry.short_timeout_slot = last_sl_tx + SHORT_PUCCH_TIMEOUT_SLOTS;
        short_timeout_wheel[entry.short_timeout_slot.count()].get_list(uci_pool).push_front(id);
      }
    }

    return action;
  }

  logger.warning("rnti={}: Discarding UCI indication PDU. Cause: Respective UCI grant was not found (UCI slot={})",
                 pdu.crnti,
                 sl_rx);

  return std::nullopt;
}

void uci_indication_selector::handle_timeout_pending_uci_entry(stable_id_t id, slot_point sl_rx)
{
  const uci_entry& entry = uci_pool[id];

  // Signal timeout to notifier. Note: an entry whose outcome was already forwarded (only possible for a PUCCH
  // repetition burst, whose entry outlives the action it reported) must not be signalled twice.
  if (not entry.is_burst_reported()) {
    timeout_notifier.on_timeout(sl_rx, entry.crnti, entry.chosen_action);
  }

  // Remove from short timeout wheel if present, then erase from pool.
  if (entry.short_timeout_slot.valid()) {
    short_timeout_wheel[entry.short_timeout_slot.count()].get_list(uci_pool).erase(id);
  }
  uci_pool.erase(id);
}

void uci_indication_selector::erase_uci_entry(stable_id_t id)
{
  const slot_point uci_slot           = uci_pool[id].uci_slot;
  const slot_point short_timeout_slot = uci_pool[id].short_timeout_slot;

  uci_wheel[uci_slot.count()].get_list(uci_pool).erase(id);
  if (short_timeout_slot.valid()) {
    short_timeout_wheel[short_timeout_slot.count()].get_list(uci_pool).erase(id);
  }
  uci_pool.erase(id);
}

stable_id_t uci_indication_selector::find_burst_anchor(rnti_t crnti, slot_point anchor_slot)
{
  if (not anchor_slot.valid()) {
    return invalid_entry_id;
  }
  auto anchor_r = uci_wheel[anchor_slot.count()].get_list(uci_pool);
  for (auto it = anchor_r.begin(); it != anchor_r.end(); ++it) {
    if (it->crnti == crnti and it->uci_slot == anchor_slot and it->is_burst_anchor()) {
      return it.id();
    }
  }
  return invalid_entry_id;
}

std::optional<uci_action> uci_indication_selector::handle_burst_uci_pdu(slot_point                     anchor_slot,
                                                                        const uci_indication::uci_pdu& pdu)
{
  const stable_id_t anchor_id = find_burst_anchor(pdu.crnti, anchor_slot);
  if (anchor_id == invalid_entry_id) {
    // The outcome of the burst was already determined (e.g. an earlier repetition was successfully decoded and all the
    // remaining ones were accounted for), so this repetition carries no new information.
    logger.debug("rnti={}: Discarding UCI indication PDU of a PUCCH repetition burst (first slot={}). Cause: The "
                 "outcome of the burst was already determined",
                 pdu.crnti,
                 anchor_slot);
    return std::nullopt;
  }

  uci_action action = create_action(pdu);
  action.uci_slot   = anchor_slot;

  return handle_burst_repetition(anchor_id, &action);
}

std::optional<uci_action> uci_indication_selector::handle_burst_repetition(stable_id_t       anchor_id,
                                                                           const uci_action* action)
{
  uci_entry& entry = uci_pool[anchor_id];
  if (entry.uci_pdus_to_rx > 0) {
    --entry.uci_pdus_to_rx;
  }

  auto& anchor = std::get<uci_entry::burst_anchor>(entry.burst);

  std::optional<uci_action> ret;
  if (not anchor.reported and action != nullptr and action->uci_valid) {
    // All the repetitions of a burst carry the same UCI, so the first one that is successfully decoded already
    // determines its outcome. Forward it without waiting for the remaining repetitions.
    anchor.reported = true;
    ret             = *action;
  }

  if (anchor.ended and entry.uci_pdus_to_rx == 0) {
    // The feedback of all the repetitions of the burst has been accounted for.
    if (not anchor.reported) {
      // None of the repetitions was successfully decoded. Propagate the outcome, so that the respective DL HARQ
      // processes do not have to wait for the full timeout.
      ret = entry.chosen_action;
    }
    erase_uci_entry(anchor_id);
  }

  return ret;
}

void uci_indication_selector::handle_large_slot_jump(unsigned slot_jump)
{
  if (uci_pool.empty()) {
    return;
  }

  logger.warning("Forcing timeout for {} pending UCI entries. Cause: Slot jump of {} exceeds UCI timeout wheel size "
                 "of {} slots",
                 uci_pool.size(),
                 slot_jump,
                 uci_wheel.size());

  for (const uci_entry& entry : uci_pool) {
    // The entries of the repetition slots that follow the first one of a burst, and the bursts whose outcome was
    // already forwarded, have nothing to report.
    if ((entry.is_burst() and not entry.is_burst_anchor()) or entry.is_burst_reported()) {
      continue;
    }
    timeout_notifier.on_timeout(entry.uci_slot, entry.crnti, entry.chosen_action);
  }

  uci_pool.clear();
  for (auto& list_head : uci_wheel) {
    list_head = {};
  }
  for (auto& list_head : short_timeout_wheel) {
    list_head = {};
  }
}

std::optional<uci_action> uci_indication_selector::handle_uci_pdu(const uci_indication::uci_pdu& pdu, uci_entry& entry)
{
  // Retrieve info from different PUCCH/PUSCH formats.
  uci_action ret = create_action(pdu);
  ret.uci_slot   = entry.uci_slot;

  // Case: If there was no previous UCI PDU decoded, it had lower SNR or was invalid, this UCI PDU is chosen.
  if (ret.uci_valid and
      (not entry.chosen_action.ul_sinr_dB.has_value() or
       (ret.ul_sinr_dB.has_value() and entry.chosen_action.ul_sinr_dB.value() < ret.ul_sinr_dB.value()))) {
    entry.chosen_action = ret;
  }

  if (entry.uci_pdus_to_rx <= 1) {
    // Case: This is the last PUCCH that is expected for this UCI grant.
    entry.uci_pdus_to_rx = 0;
    return entry.chosen_action;
  }

  // Case: This is not the last PUCCH that is expected for this UCI grant.
  entry.uci_pdus_to_rx--;

  return std::nullopt;
}

void uci_indication_selector::handle_timeouts(slot_point sl_tx)
{
  // Handle UCI entries that reach their timeout and never received any UCI PDU.
  slot_point sl_rx = sl_tx - ack_timeout_slots;
  {
    auto uci_r = uci_wheel[sl_rx.count()].get_list(uci_pool);
    while (not uci_r.empty()) {
      stable_id_t id    = uci_r.pop_front();
      uci_entry&  entry = uci_pool[id];
      if (entry.is_burst() and not entry.is_burst_anchor()) {
        // The entries of the repetition slots that follow the first one report nothing on their own: the outcome of
        // the burst is handled by its anchor entry, which expires earlier.
        uci_pool.erase(id);
        continue;
      }
      if (entry.is_burst_reported()) {
        // The outcome of this PUCCH repetition burst was already forwarded; the entry only lingered because the
        // feedback of some of its repetitions never reached the scheduler.
        logger.debug("rnti={}: Discarding PUCCH repetition burst entry (first slot={}). Cause: Timeout was reached "
                     "({} slots) but its outcome had already been forwarded",
                     entry.crnti,
                     entry.uci_slot,
                     ack_timeout_slots);
        handle_timeout_pending_uci_entry(id, sl_rx);
        continue;
      }
      if (OCUDU_UNLIKELY(entry.uci_slot != sl_rx)) {
        logger.warning("rnti={}: Forcing timeout for stale UCI entry. Cause: expected UCI slot={} but found slot={}",
                       entry.crnti,
                       sl_rx,
                       entry.uci_slot);
        handle_timeout_pending_uci_entry(id, entry.uci_slot);
        continue;
      }

      logger.warning("rnti={}: Forcing \"NACK\" for {} DL HARQ processes. Cause: Timeout was reached ({} slots) "
                     "but no UCI indication feedback has been received yet from lower layers (UCI slot={})",
                     entry.crnti,
                     entry.chosen_action.harq_ack_bits.size(),
                     ack_timeout_slots,
                     entry.uci_slot);

      // Handle UCI timeout if there were still pending UCI indications.
      handle_timeout_pending_uci_entry(id, sl_rx);
    }
  }
  ocudu_sanity_check(uci_wheel[sl_rx.count()].empty(), "Unexpected state for UCI time wheel");

  // Handle UCI entries that received at least one but not all the expected UCI indications (within the short timeout
  // window).
  {
    auto short_r = short_timeout_wheel[sl_tx.count()].get_list(uci_pool);
    while (not short_r.empty()) {
      stable_id_t id    = short_r.pop_front();
      uci_entry&  entry = uci_pool[id];

      if (entry.is_burst_reported()) {
        // The outcome of this PUCCH repetition burst was already forwarded, and the feedback of the repetitions that
        // are still pending would not change it.
        logger.debug("rnti={}: Closing PUCCH repetition burst (first slot={}) without the feedback of all its "
                     "repetitions. Cause: Timeout was reached ({} slots) after the last repetition",
                     entry.crnti,
                     entry.uci_slot,
                     SHORT_PUCCH_TIMEOUT_SLOTS);
        uci_wheel[entry.uci_slot.count()].get_list(uci_pool).erase(id);
        uci_pool.erase(id);
        continue;
      }

      // Handle UCI timeout.
      if (entry.chosen_action.uci_valid) {
        logger.debug("rnti={}: Forwarding HARQ-ACK bits=0b{:b} to UE DL HARQ processes without all UCI indication "
                     "feedback having been received. Cause: Timeout was reached ({} slots), but at least a valid UCI "
                     "PDU was received (UCI slot={}).",
                     entry.crnti,
                     entry.chosen_action.harq_ack_bits,
                     SHORT_PUCCH_TIMEOUT_SLOTS,
                     entry.uci_slot);
      } else {
        // At least one of the expected ACKs went missing and we haven't received any valid UCI.
        logger.warning("rnti={}: Forcing \"NACK\" for {} DL HARQ processes. Cause: Timeout was reached ({} slots) "
                       "to receive the respective UCI indication feedback and no valid UCI PDU has been received yet "
                       "(UCI slot={})",
                       entry.crnti,
                       entry.chosen_action.harq_ack_bits.size(),
                       SHORT_PUCCH_TIMEOUT_SLOTS,
                       entry.uci_slot);
      }

      // Propagate timeout.
      timeout_notifier.on_timeout(entry.uci_slot, entry.crnti, entry.chosen_action);

      // Remove from main wheel and erase from pool. The short-timeout list entry was already popped above.
      uci_wheel[entry.uci_slot.count()].get_list(uci_pool).erase(id);
      uci_pool.erase(id);
    }
  }
  ocudu_sanity_check(short_timeout_wheel[sl_tx.count()].empty(), "Unexpected state for short UCI timeout wheel");
}

void uci_indication_selector::handle_result(slot_point sl_tx, const sched_result& result)
{
  // Handle UCI grant timeouts accounting for potential slot indication skips.
  unsigned skipped_slots = 1;
  if (OCUDU_LIKELY(last_sl_tx.valid())) {
    const unsigned slot_jump = sl_tx - last_sl_tx;
    if (OCUDU_UNLIKELY(slot_jump > uci_wheel.size())) {
      handle_large_slot_jump(slot_jump);
    } else {
      skipped_slots = slot_jump;
    }
  }
  last_sl_tx = sl_tx;

  // Handle timeouts of past allocated UCIs.
  for (unsigned i = 0; i != skipped_slots; ++i) {
    handle_timeouts(sl_tx + 1 - skipped_slots + i);
  }

  // Handle new PUCCH grants scheduled in this slot.
  ocudu_sanity_check(uci_wheel[sl_tx.count()].empty(), "The wheel should be empty for slot tx");
  auto uci_r = uci_wheel[sl_tx.count()].get_list(uci_pool);
  for (const pucch_info& pucch : result.ul.pucchs) {
    if (pucch.uci_bits.harq_ack_nof_bits == 0) {
      // Only PUCCHs with HARQ-ACK bits need to be buffered for timeout handling.
      continue;
    }

    if (pucch.repetition.has_value()) {
      // The PUCCH is one of the repetitions of a multi-slot PUCCH repetition burst, which is tracked as a whole.
      handle_burst_pucch_grant(sl_tx, pucch);
      continue;
    }

    // Check if there is another PUCCH (e.g. F1 SR + F1 HARQ-ACK case or during transition from fallback).
    // If so, increment uci_pdus_to_rx.
    // Note: a PUCCH repetition burst reserves the slot for itself, so its entries are never combined with another
    // PUCCH of the same UE.
    {
      bool found = false;
      for (auto it = uci_r.begin(); it != uci_r.end(); ++it) {
        if (it->crnti == pucch.crnti and not it->is_burst()) {
          uci_pool[it.id()].uci_pdus_to_rx++;
          found = true;
          break;
        }
      }
      if (found) {
        // Another PUCCH F1 was found in this slot. Avoid adding more than one UCI grant for the same RNTI in the wheel.
        continue;
      }
    }

    // Create new UCI entry and save it in the UCI wheel.
    uci_entry entry;
    entry.crnti          = pucch.crnti;
    entry.uci_pdus_to_rx = 1;
    entry.uci_slot       = sl_tx;
    // The chosen action set here is what will be propagated in case of timeout.
    entry.chosen_action.uci_slot = sl_tx;
    entry.chosen_action.harq_ack_bits.resize(pucch.uci_bits.harq_ack_nof_bits);
    stable_id_t id = uci_pool.insert(entry);
    uci_r.push_front(id);
  }

  // Handle new PUSCH with UCI grants scheduled in this slot.
  for (const ul_sched_info& pusch : result.ul.puschs) {
    if (not pusch.uci.has_value() or not pusch.uci->harq.has_value() or pusch.uci->harq->harq_ack_nof_bits == 0) {
      // Only UCI with HARQ-ACK bits need to be buffered for timeout handling.
      continue;
    }

    // Create new UCI entry and save it in the UCI wheel.
    uci_entry entry;
    entry.crnti                  = pusch.pusch_cfg.rnti;
    entry.uci_pdus_to_rx         = 1;
    entry.uci_slot               = sl_tx;
    entry.chosen_action.uci_slot = sl_tx;
    entry.chosen_action.harq_ack_bits.resize(pusch.uci->harq->harq_ack_nof_bits);
    stable_id_t id = uci_pool.insert(entry);
    uci_r.push_front(id);
  }
}

void uci_indication_selector::handle_burst_pucch_grant(slot_point sl_tx, const pucch_info& pucch)
{
  auto uci_r = uci_wheel[sl_tx.count()].get_list(uci_pool);

  if (pucch.repetition->position == pucch_repetition_tx_slot::starts) {
    // First repetition of the burst. Its entry tracks the burst as a whole.
    uci_entry entry;
    entry.crnti          = pucch.crnti;
    entry.uci_pdus_to_rx = 1;
    entry.uci_slot       = sl_tx;
    entry.burst          = uci_entry::burst_anchor{};
    // The chosen action set here is what will be propagated if none of the repetitions is successfully decoded.
    entry.chosen_action.uci_slot = sl_tx;
    entry.chosen_action.harq_ack_bits.resize(pucch.uci_bits.harq_ack_nof_bits);
    uci_r.push_front(uci_pool.insert(entry));
    return;
  }

  const stable_id_t anchor_id = find_burst_anchor(pucch.crnti, pucch.repetition->anchor_slot);
  if (anchor_id == invalid_entry_id) {
    logger.warning("rnti={}: Discarding PUCCH repetition scheduled in slot={}. Cause: The UCI grant of its burst "
                   "(first slot={}) was not found",
                   pucch.crnti,
                   sl_tx,
                   pucch.repetition->anchor_slot);
    return;
  }

  // The remaining repetitions only point at the anchor entry, so that the UCI PDUs received in their slot reach it.
  uci_entry entry;
  entry.crnti    = pucch.crnti;
  entry.uci_slot = sl_tx;
  entry.burst    = uci_entry::burst_repetition{pucch.repetition->anchor_slot};
  uci_r.push_front(uci_pool.insert(entry));

  uci_entry& anchor = uci_pool[anchor_id];
  ++anchor.uci_pdus_to_rx;
  if (pucch.repetition->position == pucch_repetition_tx_slot::ends) {
    std::get<uci_entry::burst_anchor>(anchor.burst).ended = true;
    // Bound the wait for the feedback of the last repetitions, so that the outcome of the burst does not have to wait
    // for the full UCI timeout in case some of it never reaches the scheduler.
    anchor.short_timeout_slot = sl_tx + SHORT_PUCCH_TIMEOUT_SLOTS;
    short_timeout_wheel[anchor.short_timeout_slot.count()].get_list(uci_pool).push_front(anchor_id);
  }
}

void uci_indication_selector::handle_discarded_ucis(slot_point sl_tx)
{
  auto uci_r = uci_wheel[sl_tx.count()].get_list(uci_pool);
  for (auto it = uci_r.begin(); it != uci_r.end();) {
    const stable_id_t id = it.id();
    // Advance before the entry is potentially unlinked from the list.
    ++it;

    if (uci_pool[id].is_burst()) {
      // Only one repetition of a PUCCH repetition burst is discarded; the remaining ones may still be decoded. Account
      // for the lost feedback and let the burst conclude once the rest of it is accounted for. Note that the entry
      // tracking the burst (the anchor entry) is only removed by that conclusion, so that the repetitions in the
      // slots that follow keep being tracked.
      const rnti_t crnti     = uci_pool[id].crnti;
      stable_id_t  anchor_id = id;
      if (not uci_pool[id].is_burst_anchor()) {
        anchor_id = find_burst_anchor(crnti, uci_pool[id].burst_anchor_slot());
        uci_r.erase(id);
        uci_pool.erase(id);
        if (anchor_id == invalid_entry_id) {
          continue;
        }
      }
      const slot_point                anchor_slot = uci_pool[anchor_id].uci_slot;
      const std::optional<uci_action> action      = handle_burst_repetition(anchor_id, nullptr);
      if (action.has_value()) {
        timeout_notifier.on_timeout(anchor_slot, crnti, *action);
      }
      continue;
    }

    const uci_entry& entry = uci_pool[id];

    // The lower layers will not attempt to decode the PUCCHs and PUSCH UCIs and will not send any UCI indication
    // feedback. To avoid a long DL HARQ timeout window (due to lack of UCI indication), it is important to force a DTX
    // for the DL HARQ processes with UCI falling in this slot.
    // Note: We don't use this cancellation to update the DL OLLA (UCI is invalid), as we shouldn't take lates into
    // account in link adaptation.
    timeout_notifier.on_timeout(sl_tx, entry.crnti, entry.chosen_action);

    // Remove from the wheels and erase from pool.
    uci_r.erase(id);
    if (entry.short_timeout_slot.valid()) {
      short_timeout_wheel[entry.short_timeout_slot.count()].get_list(uci_pool).erase(id);
    }
    uci_pool.erase(id);
  }
}
