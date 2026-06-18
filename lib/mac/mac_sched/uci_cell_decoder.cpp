// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "uci_cell_decoder.h"
#include "ocudu/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "ocudu/ran/csi_report/csi_report_on_pusch_helpers.h"
#include "ocudu/scheduler/resource_grid_util.h"
#include "ocudu/scheduler/result/pucch_info.h"
#include "ocudu/scheduler/result/pusch_info.h"

using namespace ocudu;

/// \brief Size, in number of slots, of the ring buffer used to store the pending UCIs to be decoded. This size
/// should account for potential latencies in the PHY in forwarding the decoded UCI to the MAC.
static size_t get_ring_size(const sched_cell_configuration_request_message& cell_cfg)
{
  const unsigned ntn_cs_koffset =
      cell_cfg.ran.ntn_params.has_value() && cell_cfg.ran.ntn_params->ntn_cfg.cell_specific_koffset.has_value()
          ? cell_cfg.ran.ntn_params->ntn_cfg.cell_specific_koffset->count() *
                get_nof_slots_per_subframe(cell_cfg.ran.dl_cfg_common.init_dl_bwp.generic_params.scs)
          : 0;

  // Estimation of the time it takes the UL lower-layers to process and forward CRC/UCI indications.
  // Note: The size of this ring has to be larger than that of the test mode internal buffer.
  static constexpr unsigned MAX_UL_PHY_DELAY = 80;
  // Note: The history ring size has to be a multiple of the TDD frame size in slots.
  // Number of slots managed by this container.
  return get_allocator_ring_size_gt_min(get_max_slot_ul_alloc_delay(ntn_cs_koffset) + MAX_UL_PHY_DELAY);
}

uci_cell_decoder::uci_cell_decoder(const sched_cell_configuration_request_message& cell_cfg,
                                   const du_rnti_table&                            rnti_table_,
                                   rlf_detector&                                   rlf_hdlr_) :
  rnti_table(rnti_table_),
  cell_index(cell_cfg.cell_index),
  aperiodic_csi_report(cell_cfg.ran.init_bwp.csi.has_value() and cell_cfg.ran.init_bwp.csi->enable_aperiodic_report),
  rlf_handler(rlf_hdlr_),
  logger(ocudulog::fetch_basic_logger("MAC")),
  expected_uci_report_grid(get_ring_size(cell_cfg))
{
}

static auto convert_mac_harq_bits_to_sched_harq_values(bool harq_status,
                                                       const bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>& payload)
{
  harq_ack_report_list harqs(payload.size(),
                             harq_status ? mac_harq_ack_report_status::nack : mac_harq_ack_report_status::dtx);
  if (harq_status) {
    for (unsigned i = 0, e = harqs.size(); i != e; ++i) {
      if (payload.test(i)) {
        harqs[i] = ocudu::mac_harq_ack_report_status::ack;
      }
    }
  }
  return harqs;
}

static csi_report_data decode_pucch_csi(const bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>& payload,
                                        const csi_report_configuration& csi_rep_cfg)
{
  // Convert UCI CSI1 bits to "csi_report_packed".
  csi_report_packed csi_bits(payload.size());
  for (unsigned k = 0; k != csi_bits.size(); ++k) {
    csi_bits.set(k, payload.test(k));
  }

  return csi_report_unpack_pucch(csi_bits, csi_rep_cfg);
}

static std::optional<csi_report_data> decode_csi_bits(const mac_uci_pdu::pucch_f2_or_f3_or_f4_type& pucch,
                                                      const csi_report_configuration&               csi_rep_cfg)
{
  // TODO: Handle CSI part 2.
  return decode_pucch_csi(pucch.csi_part1_info->payload, csi_rep_cfg);
}

static std::optional<csi_report_data>
decode_csi_bits(const mac_uci_pdu::pusch_type& pusch, const csi_report_configuration& csi_rep_cfg, bool is_aperiodic)
{
  if (not is_aperiodic) {
    // TODO: Handle CSI part 2.
    return decode_pucch_csi(pusch.csi_part1_info->payload, csi_rep_cfg);
  }

  // Convert UCI CSI1 and CSI2 bits to "csi_report_packed".
  csi_report_packed csi1_bits(pusch.csi_part1_info->payload.size());
  for (unsigned k = 0; k != csi1_bits.size(); ++k) {
    csi1_bits.set(k, pusch.csi_part1_info->payload.test(k));
  }
  csi_report_packed csi2_bits(pusch.csi_part2_info->payload.size());
  for (unsigned k = 0; k != csi2_bits.size(); ++k) {
    csi2_bits.set(k, pusch.csi_part2_info->payload.test(k));
  }

  return csi_report_unpack_pusch(csi1_bits, csi2_bits, csi_rep_cfg);
}

static bool grid_access_required(const mac_uci_indication_message& msg)
{
  return std::any_of(msg.ucis.begin(), msg.ucis.end(), [](const mac_uci_pdu& pdu) {
    if (const auto* puschuci = std::get_if<mac_uci_pdu::pusch_type>(&pdu.pdu)) {
      return puschuci->csi_part1_info.has_value() and puschuci->csi_part1_info->is_valid;
    }
    if (const auto* f2uci = std::get_if<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(&pdu.pdu)) {
      return f2uci->csi_part1_info.has_value() and f2uci->csi_part1_info->is_valid;
    }
    return false;
  });
}

uci_indication uci_cell_decoder::decode_uci(const mac_uci_indication_message& msg)
{
  // Convert MAC UCI indication to OCUDU scheduler UCI indication.
  uci_indication ind{};
  ind.slot_rx    = msg.sl_rx;
  ind.cell_index = cell_index;

  // Acquire shared (reader) access to the grid entry holding the expected UCI report configs for this slot. Multiple
  // PHY worker threads may decode the same slot's UCIs concurrently, so reader access is shared. A failed acquisition,
  // or an entry that has been recycled for a different slot, means the slot-indication writer wrapped around the ring
  // and caught up with this reader, i.e. the ring buffer overflowed; the CSI report configs cannot be read and the
  // associated CSI reports are discarded.
  slot_ucis_entry* slot_entry = nullptr;
  if (grid_access_required(msg)) {
    slot_ucis_entry& entry = expected_uci_report_grid[to_grid_index(msg.sl_rx)];
    if (not entry.try_acquire_read()) {
      logger.warning("cell={}: Discarding CSI reports for slot_rx={}. Cause: UCI report ring buffer overflow.",
                     cell_index,
                     msg.sl_rx);
    } else if (entry.slot != msg.sl_rx) {
      entry.release_read();
      logger.warning("cell={}: Discarding CSI reports for slot_rx={}. Cause: UCI report ring buffer overflow.",
                     cell_index,
                     msg.sl_rx);
    } else {
      slot_entry = &entry;
    }
  }

  for (const auto& mac_uci : msg.ucis) {
    uci_indication::uci_pdu& uci_pdu = ind.ucis.emplace_back();
    uci_pdu.crnti                    = mac_uci.rnti;
    uci_pdu.ue_index                 = rnti_table[mac_uci.rnti];
    if (uci_pdu.ue_index == INVALID_DU_UE_INDEX) {
      ind.ucis.pop_back();
      logger.info("rnti={}: Discarding UCI PDU. Cause: No UE with provided RNTI exists.", uci_pdu.crnti);
      continue;
    }

    if (const auto* pucch_f0f1 = std::get_if<mac_uci_pdu::pucch_f0_or_f1_type>(&mac_uci.pdu)) {
      uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu pdu{};

      pdu.ul_sinr_dB          = pucch_f0f1->ul_sinr_dB;
      pdu.time_advance_offset = pucch_f0f1->time_advance_offset;
      pdu.sr_detected         = false;
      if (pucch_f0f1->sr_info.has_value()) {
        pdu.sr_detected = pucch_f0f1->sr_info.value().detected;
      }
      if (pucch_f0f1->harq_info.has_value()) {
        // NOTES:
        // - We report to the scheduler only the UCI HARQ-ACKs that contain either an ACK or NACK; we ignore the
        // UCIs with DTX. In that case, the scheduler will not receive the notification and the HARQ will eventually
        // retransmit the packet.
        // - This is to handle the case of simultaneous SR + HARQ UCI, for which we receive 2 UCI PDUs from the PHY,
        // 1 for SR + HARQ, 1 for HARQ only; note that only the SR + HARQ UCI is filled by the UE, meaning that we
        // expect the received HARQ-only UCI to be DTX. If reported to the scheduler, the UCI with HARQ-ACK only would
        // be erroneously treated as a NACK (as the scheduler only accepts ACK or NACK).

        // NOTE: There is a potential error that need to be handled below, which occurs when there's the 2-bit report
        // {DTX, (N)ACK}; if this were reported, we would skip the first bit (i.e. DTX) and report the second (i.e.
        // (N)ACK). Since in the scheduler the HARQ-ACK bits for a given UCI are processed in sequence, the
        // notification of the second bit of {DTX, (N)ACK} would be seen by the scheduler as the first bit of the
        // expected 2-bit reporting. To prevent this, we assume that PUCCH Format 0 or 1 UCI is valid if none of the 1
        // or 2 bits report is DTX (not detected).

        const auto& harq_pdus = pucch_f0f1->harq_info.value().harqs;
        pdu.harqs.resize(harq_pdus.size());
        for (unsigned i = 0, e = pdu.harqs.size(); i != e; ++i) {
          switch (harq_pdus[i]) {
            case uci_pucch_f0_or_f1_harq_values::ack:
              pdu.harqs[i] = mac_harq_ack_report_status::ack;
              break;
            case uci_pucch_f0_or_f1_harq_values::nack:
              pdu.harqs[i] = mac_harq_ack_report_status::nack;
              break;
            default:
              pdu.harqs[i] = mac_harq_ack_report_status::dtx;
          }

          // Report ACK for RLF detection purposes.
          rlf_handler.handle_ack(uci_pdu.ue_index, cell_index, pdu.harqs[i] == mac_harq_ack_report_status::ack);
        }
      }
      uci_pdu.pdu.emplace<uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu>(pdu);
    } else if (const auto* pusch = std::get_if<mac_uci_pdu::pusch_type>(&mac_uci.pdu)) {
      auto& pdu = uci_pdu.pdu.emplace<uci_indication::uci_pdu::uci_pusch_pdu>();
      if (pusch->harq_info.has_value()) {
        pdu.harqs =
            convert_mac_harq_bits_to_sched_harq_values(pusch->harq_info.value().is_valid, pusch->harq_info->payload);

        // Report ACK for RLF detection purposes.
        for (mac_harq_ack_report_status harq : pdu.harqs) {
          rlf_handler.handle_ack(uci_pdu.ue_index, cell_index, harq == mac_harq_ack_report_status::ack);
        }
      }

      if (pusch->csi_part1_info.has_value()) {
        if (pusch->csi_part1_info->is_valid and slot_entry != nullptr) {
          // Decode CSI bits given the CSI report config previously stored in the grid.
          // Search for CSI report config with matching RNTI.
          for (const auto& expected_slot_uci : slot_entry->ucis) {
            if (expected_slot_uci.rnti == uci_pdu.crnti) {
              pdu.csi = decode_csi_bits(*pusch, expected_slot_uci.csi_rep_cfg, aperiodic_csi_report);
              break;
            }
          }
          if (not pdu.csi.has_value()) {
            logger.warning("cell={} ue={} rnti={}: Discarding CSI report. Cause: Unable to find CSI report config.",
                           cell_index,
                           uci_pdu.ue_index,
                           uci_pdu.crnti);
          }
        } else if (not pusch->csi_part1_info->is_valid) {
          pdu.csi = csi_report_data{.valid = false};
        }

        // NOTE: The RLF detection based on CSI is used when the UE only transmits PUCCHs; if the UE transmit PUSCHs,
        // the RLF detection will be based on the PUSCH CRC. However, if the PUSCH UCI has a correctly decoded CSI, we
        // need to reset the CSI KOs counter.
        if (pusch->csi_part1_info->is_valid) {
          rlf_handler.handle_csi(uci_pdu.ue_index, cell_index, true);
        }
      }

    } else if (const auto* pucch_f2f3f4 = std::get_if<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(&mac_uci.pdu)) {
      auto& pdu = uci_pdu.pdu.emplace<uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu>();

      pdu.ul_sinr_dB          = pucch_f2f3f4->ul_sinr_dB;
      pdu.time_advance_offset = pucch_f2f3f4->time_advance_offset;
      if (pucch_f2f3f4->sr_info.has_value()) {
        pdu.sr_info = pucch_f2f3f4->sr_info.value();
      }
      if (pucch_f2f3f4->harq_info.has_value()) {
        pdu.harqs = convert_mac_harq_bits_to_sched_harq_values(pucch_f2f3f4->harq_info.value().is_valid,
                                                               pucch_f2f3f4->harq_info->payload);

        // Report ACK for RLF detection purposes.
        for (mac_harq_ack_report_status harq_st : pdu.harqs) {
          rlf_handler.handle_ack(uci_pdu.ue_index, cell_index, harq_st == mac_harq_ack_report_status::ack);
        }
      }

      // Check if the UCI has been correctly decoded.
      if (pucch_f2f3f4->csi_part1_info.has_value()) {
        if (pucch_f2f3f4->csi_part1_info->is_valid and slot_entry != nullptr) {
          // Decode CSI bits given the CSI report config previously stored in the grid.
          // Search for CSI report config with matching RNTI.
          for (const auto& expected_slot_uci : slot_entry->ucis) {
            if (expected_slot_uci.rnti == uci_pdu.crnti) {
              pdu.csi = decode_csi_bits(*pucch_f2f3f4, expected_slot_uci.csi_rep_cfg);
              break;
            }
          }
          if (not pdu.csi.has_value()) {
            logger.warning("cell={} ue={} rnti={}: Discarding CSI report. Cause: Unable to find CSI report config.",
                           cell_index,
                           uci_pdu.ue_index,
                           uci_pdu.crnti);
          }
        } else if (not pucch_f2f3f4->csi_part1_info->is_valid) {
          pdu.csi = csi_report_data{.valid = false};
        }

        // We consider any status other than "crc_pass" as non-decoded CSI.
        rlf_handler.handle_csi(uci_pdu.ue_index, cell_index, pucch_f2f3f4->csi_part1_info->is_valid);
      }
    }
  }

  if (slot_entry != nullptr) {
    // Do not clear the entry: the PHY may deliver the UCIs for a given slot across several indications, all sharing the
    // same sl_rx, so the stored configs must remain available until store_uci overwrites the entry on ring wrap-around.
    slot_entry->release_read();
  }

  return ind;
}

static bool uci_storage_required(span<const pucch_info> scheduled_pucchs, span<const ul_sched_info> scheduled_puschs)
{
  return std::any_of(scheduled_pucchs.begin(),
                     scheduled_pucchs.end(),
                     [](const pucch_info& pucch) { return pucch.csi_rep_cfg.has_value(); }) or
         std::any_of(scheduled_puschs.begin(), scheduled_puschs.end(), [](const ul_sched_info& pusch) {
           return pusch.uci.has_value() and pusch.uci->csi.has_value();
         });
}

void uci_cell_decoder::store_uci(slot_point                uci_sl,
                                 span<const pucch_info>    scheduled_pucchs,
                                 span<const ul_sched_info> scheduled_puschs)
{
  if (not uci_storage_required(scheduled_pucchs, scheduled_puschs)) {
    // Early exit.
    return;
  }

  // Acquire exclusive (writer) access to the grid entry. A failed acquisition means one or more readers are still
  // decoding an aliasing slot, i.e. the ring buffer overflowed; skip the write to avoid racing on the underlying
  // container.
  slot_ucis_entry& slot_entry = expected_uci_report_grid[to_grid_index(uci_sl)];
  if (not slot_entry.try_acquire_write()) {
    logger.warning("cell={}: Skipping storage of expected UCIs for slot={}. Cause: UCI report ring buffer overflow.",
                   cell_index,
                   uci_sl);
    return;
  }

  slot_entry.slot = uci_sl;
  auto& slot_ucis = slot_entry.ucis;
  slot_ucis.clear();

  for (const pucch_info& pucch : scheduled_pucchs) {
    if (pucch.csi_rep_cfg.has_value()) {
      uci_context& uci_ctx = slot_ucis.emplace_back();
      uci_ctx.rnti         = pucch.crnti;
      uci_ctx.csi_rep_cfg  = *pucch.csi_rep_cfg;
    }
  }
  for (const ul_sched_info& pusch : scheduled_puschs) {
    if (pusch.uci.has_value() and pusch.uci->csi.has_value()) {
      uci_context& uci_ctx = slot_ucis.emplace_back();
      uci_ctx.rnti         = pusch.pusch_cfg.rnti;
      uci_ctx.csi_rep_cfg  = pusch.uci->csi->csi_rep_cfg;
    }
  }

  slot_entry.release_write();
}
