// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "indication_generators.h"
#include "lib/scheduler/config/cell_configuration.h"
#include "ocudu/ran/band_helper.h"
#include "ocudu/ran/prach/prach_configuration.h"
#include "ocudu/ran/prach/ra_helper.h"
#include "ocudu/scheduler/result/pucch_info.h"

using namespace ocudu;

rach_indication_message::preamble ocudu::test_helper::create_preamble(unsigned preamble_id, rnti_t tc_rnti)
{
  rach_indication_message::preamble preamble{};
  preamble.preamble_id  = preamble_id;
  preamble.time_advance = phy_time_unit::from_seconds(0);
  preamble.tc_rnti      = tc_rnti;
  return preamble;
}

rnti_t ocudu::test_helper::compute_ra_rnti(const cell_configuration& cell_cfg,
                                           slot_point                prach_slot_rx,
                                           unsigned                  start_symbol,
                                           unsigned                  frequency_index)
{
  const prach_configuration prach_cfg = prach_configuration_get(
      band_helper::get_freq_range(cell_cfg.band()),
      band_helper::get_duplex_mode(cell_cfg.band()),
      cell_cfg.params.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index);
  // As per TS 38.321, 5.1.3, and TS 38.211, 5.3.2, slot_idx uses 15kHz numerology (subframe index) for long PRACH
  // formats, and the SCS common numerology (actual slot index) for short formats.
  const unsigned slot_idx =
      is_long_preamble(prach_cfg.format) ? prach_slot_rx.subframe_index() : prach_slot_rx.slot_index();
  return ra_helper::get_ra_rnti(slot_idx, start_symbol, frequency_index);
}

rach_indication_message
ocudu::test_helper::create_rach_indication(slot_point                                            slot_rx,
                                           const std::vector<rach_indication_message::preamble>& preambles)
{
  rach_indication_message rach_ind{};
  rach_ind.cell_index = to_du_cell_index(0);
  rach_ind.slot_rx    = slot_rx;
  if (preambles.empty()) {
    return rach_ind;
  }
  rach_ind.occasions.emplace_back();
  rach_ind.occasions.back().start_symbol    = 0;
  rach_ind.occasions.back().frequency_index = 0;

  for (const auto& preamble : preambles) {
    rach_ind.occasions.back().preambles.push_back(preamble);
  }
  return rach_ind;
}

uci_indication::uci_pdu test_helper::create_uci_indication_pdu(du_ue_index_t                             ue_idx,
                                                               const pucch_info&                         pucch_pdu,
                                                               std::optional<mac_harq_ack_report_status> ack_set)
{
  const mac_harq_ack_report_status ack_status = ack_set.value_or(mac_harq_ack_report_status::ack);

  uci_indication::uci_pdu pdu;
  pdu.crnti    = pucch_pdu.crnti;
  pdu.ue_index = ue_idx;
  switch (pucch_pdu.format()) {
    case pucch_format::FORMAT_1: {
      uci_indication::uci_pdu::uci_pucch_f0_or_f1_pdu f1{};
      f1.harqs.resize(pucch_pdu.uci_bits.harq_ack_nof_bits);
      // We set DTX if it is a PUCCH F1 for SR and ack_set was not set.
      const auto f1_status = pucch_pdu.uci_bits.sr_bits != sr_nof_bits::no_sr and not ack_set.has_value()
                                 ? mac_harq_ack_report_status::dtx
                                 : ack_status;
      std::fill(f1.harqs.begin(), f1.harqs.end(), f1_status);
      pdu.pdu = f1;
    } break;
    case pucch_format::FORMAT_2: {
      uci_indication::uci_pdu::uci_pucch_f2_or_f3_or_f4_pdu f2{};
      f2.harqs.resize(pucch_pdu.uci_bits.harq_ack_nof_bits);
      std::fill(f2.harqs.begin(), f2.harqs.end(), ack_status);
      if (pucch_pdu.uci_bits.csi_part1_nof_bits > 0) {
        f2.csi.emplace();
        f2.csi->first_tb_wideband_cqi = 15;
      }
      pdu.pdu = f2;
    } break;
    default:
      report_fatal_error("Unsupported PUCCH format");
  }
  return pdu;
}

uci_indication
ocudu::test_helper::create_uci_indication(slot_point uci_sl, du_ue_index_t ue_idx, const pucch_info& pucch_pdu)
{
  uci_indication uci_ind{};
  uci_ind.cell_index = to_du_cell_index(0);
  uci_ind.slot_rx    = uci_sl;
  uci_ind.ucis.emplace_back(create_uci_indication_pdu(ue_idx, pucch_pdu));
  return uci_ind;
}

uci_indication::uci_pdu
ocudu::test_helper::create_uci_indication_pdu(rnti_t rnti, du_ue_index_t ue_idx, const uci_info& uci)
{
  uci_indication::uci_pdu pdu;
  pdu.crnti      = rnti;
  pdu.ue_index   = ue_idx;
  auto& puschpdu = pdu.pdu.emplace<uci_indication::uci_pdu::uci_pusch_pdu>();
  if (uci.csi.has_value()) {
    puschpdu.csi.emplace();
    if (uci.csi.value().csi_part1_nof_bits > 0) {
      puschpdu.csi.value().first_tb_wideband_cqi = 15;
    }
  }
  if (uci.harq.has_value()) {
    puschpdu.harqs.resize(uci.harq.value().harq_ack_nof_bits, mac_harq_ack_report_status::ack);
  }
  return pdu;
}

ul_crc_pdu_indication ocudu::test_helper::create_crc_pdu_indication(const ul_sched_info& ul_grant)
{
  ul_crc_pdu_indication pdu{};

  pdu.rnti     = ul_grant.pusch_cfg.rnti;
  pdu.ue_index = ul_grant.context.ue_index;
  pdu.harq_id  = ul_grant.pusch_cfg.harq_id;
  pdu.rapid    = ul_grant.context.rapid.has_value() ? std::optional<uint8_t>(*ul_grant.context.rapid) : std::nullopt;
  pdu.tb_crc_success = true;
  pdu.ul_sinr_dB     = 100;

  return pdu;
}

ul_crc_indication
ocudu::test_helper::create_crc_indication(slot_point sl_rx, span<const ul_sched_info> puschs, bool ack)
{
  ul_crc_indication crc_ind;
  crc_ind.cell_index = to_du_cell_index(0);
  crc_ind.sl_rx      = sl_rx;
  for (const ul_sched_info& ul : puschs) {
    ul_crc_pdu_indication pdu = create_crc_pdu_indication(ul);
    pdu.tb_crc_success        = ack;
    crc_ind.crcs.push_back(pdu);
  }
  return crc_ind;
}
