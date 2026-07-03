// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "uci_indication_selector.h"
#include "ocudu/ocudulog/ocudulog.h"

using namespace ocudu;

uci_indication_selector::uci_indication_selector(uci_indication_timeout_notifier& timeout_notifier_,
                                                 unsigned                         uci_ack_timeout,
                                                 unsigned                         max_pucch_grants_per_slot,
                                                 std::optional<float>             pucch_sinr_threshold_dB_) :
  ack_timeout_slots(uci_ack_timeout),
  pucch_sinr_threshold_dB(pucch_sinr_threshold_dB_),
  timeout_notifier(timeout_notifier_),
  logger(ocudulog::fetch_basic_logger("SCHED")),
  uci_wheel(ack_timeout_slots),
  short_timeout_wheel(SHORT_PUCCH_TIMEOUT_SLOTS)
{
  uci_pool.reserve(
      std::min<unsigned>(ack_timeout_slots * std::min<unsigned>(max_pucch_grants_per_slot, MAX_PUCCH_PDUS_PER_SLOT),
                         MAX_NOF_DU_UES * MAX_NOF_HARQS));
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
    return create_action(pdu);
  }

  auto uci_r = uci_wheel[sl_rx.count()].get_list(uci_pool);
  for (auto prev = uci_r.before_begin(), it = uci_r.begin(); it != uci_r.end(); prev = it, ++it) {
    if (pdu.crnti != it->crnti or it->uci_slot != sl_rx) {
      continue;
    }
    // RNTIs match. The grant was found.
    stable_id_t id    = it.id();
    uci_entry&  entry = uci_pool[id];

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

  // Signal timeout to notifier.
  timeout_notifier.on_timeout(sl_rx, entry.crnti, entry.chosen_action);

  // Remove from short timeout wheel if present, then erase from pool.
  if (entry.short_timeout_slot.valid()) {
    short_timeout_wheel[entry.short_timeout_slot.count()].get_list(uci_pool).erase(id);
  }
  uci_pool.erase(id);
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

    // Check if there is another PUCCH (e.g. F1 SR + F1 HARQ-ACK case or during transition from fallback).
    // If so, increment uci_pdus_to_rx.
    {
      bool found = false;
      for (auto it = uci_r.begin(); it != uci_r.end(); ++it) {
        if (it->crnti == pucch.crnti) {
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
    entry.crnti          = pusch.pusch_cfg.rnti;
    entry.uci_pdus_to_rx = 1;
    entry.uci_slot       = sl_tx;
    entry.chosen_action.harq_ack_bits.resize(pusch.uci->harq->harq_ack_nof_bits);
    stable_id_t id = uci_pool.insert(entry);
    uci_r.push_front(id);
  }
}

void uci_indication_selector::handle_discarded_ucis(slot_point sl_tx)
{
  auto uci_r = uci_wheel[sl_tx.count()].get_list(uci_pool);
  while (not uci_r.empty()) {
    stable_id_t id    = uci_r.pop_front();
    uci_entry&  entry = uci_pool[id];

    // The lower layers will not attempt to decode the PUCCHs and PUSCH UCIs and will not send any UCI indication
    // feedback. To avoid a long DL HARQ timeout window (due to lack of UCI indication), it is important to force a DTX
    // for the DL HARQ processes with UCI falling in this slot.
    // Note: We don't use this cancellation to update the DL OLLA (UCI is invalid), as we shouldn't take lates into
    // account in link adaptation.
    timeout_notifier.on_timeout(sl_tx, entry.crnti, entry.chosen_action);

    // Remove from short timeout wheel if present, then erase from pool.
    if (entry.short_timeout_slot.valid()) {
      short_timeout_wheel[entry.short_timeout_slot.count()].get_list(uci_pool).erase(id);
    }
    uci_pool.erase(id);
  }
}
