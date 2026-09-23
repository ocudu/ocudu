// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "mac_test_mode_helpers.h"
#include "ocudu/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "ocudu/ran/csi_report/csi_report_packed.h"
#include "ocudu/ran/csi_report/csi_report_size.h"
#include "ocudu/ran/precoding/precoding_codebook_type1_helpers.h"
#include "ocudu/scheduler/result/pucch_info.h"
#include "ocudu/scheduler/result/pusch_info.h"
#include <algorithm>

using namespace ocudu;
using namespace odu;

expected<mac_rx_data_indication>
odu::create_test_pdu_with_bsr(du_cell_index_t cell_index, slot_point sl_rx, rnti_t test_rnti, harq_id_t harq_id)
{
  // - 8-bit R/LCID MAC subheader.
  // - MAC CE with Long BSR.
  //
  // |   |   |   |   |   |   |   |   |
  // | R | R |         LCID          |  Octet 1
  // |              L                |  Octet 2
  // | LCG7 | LCG6 |    ...   | LCG0 |  Octet 3
  // |         Buffer Size 1         |  Octet 4

  // We pass BSR=254, which according to TS38.321 is the maximum value for the LBSR size.
  auto buf = byte_buffer::create({0x3e, 0x02, 0x01, 254});
  if (not buf.has_value()) {
    return make_unexpected(default_error_t{});
  }
  return mac_rx_data_indication{
      sl_rx, cell_index, mac_rx_pdu_list{mac_rx_pdu{test_rnti, harq_id, std::move(buf.value())}}};
}

expected<mac_rx_data_indication> odu::create_test_pdu_with_rrc_setup_complete(du_cell_index_t cell_index,
                                                                              slot_point      sl_rx,
                                                                              rnti_t          test_rnti,
                                                                              harq_id_t       harq_id)
{
  auto buf = byte_buffer::create({0x01, 0x23, 0xc0, 0x00, 0x00, 0x00, 0x10, 0x00, 0x05, 0xdf, 0x80, 0x10, 0x5e,
                                  0x40, 0x03, 0x40, 0x40, 0x3c, 0x44, 0x3c, 0x3f, 0xc0, 0x00, 0x04, 0x0c, 0x95,
                                  0x1d, 0xa6, 0x0b, 0x80, 0xb8, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00});
  if (not buf.has_value()) {
    return make_unexpected(default_error_t{});
  }
  return mac_rx_data_indication{
      sl_rx, cell_index, mac_rx_pdu_list{mac_rx_pdu{test_rnti, harq_id, std::move(buf.value())}}};
}

/// \brief Selects the rank reported by the test mode UE.
///
/// Returns the highest rank allowed by the RI restriction that does not exceed the configured rank.
static unsigned select_reported_rank(const ri_restriction_type& ri_restriction, unsigned configured_rank)
{
  if (ri_restriction.none()) {
    return 1;
  }

  const int rank_bit = ri_restriction.find_highest(0, std::min<size_t>(configured_rank, ri_restriction.size()), true);
  return static_cast<unsigned>((rank_bit >= 0) ? rank_bit : ri_restriction.find_lowest(true)) + 1;
}

/// Gets the index of a rank within the allowed rank values, as per TS 38.214 Section 5.2.2.2.1.
static unsigned get_rank_index(const ri_restriction_type& ri_restriction, unsigned rank)
{
  // The rank is the number of layers, starting at one, whereas bit i of the RI restriction enables rank i + 1.
  const auto  allowed_ranks = ri_restriction.get_bit_positions();
  const auto* rank_it       = std::find(allowed_ranks.begin(), allowed_ranks.end(), static_cast<size_t>(rank - 1));
  return (rank_it != allowed_ranks.end()) ? static_cast<unsigned>(rank_it - allowed_ranks.begin()) : 0;
}

/// Appends the PMI fields configured for the test mode UE, whose bit-widths are given in TS 38.212 Table 6.3.1.1.2-1.
static void fill_pmi_bits(csi_report_packed&                              packed,
                          const pmi_codebook_config&                      pmi_codebook,
                          unsigned                                        rank,
                          const du_test_mode_config::test_mode_ue_config& test_ue_cfg)
{
  if (std::holds_alternative<pmi_codebook_two_port>(pmi_codebook)) {
    packed.push_back(test_ue_cfg.pmi, (rank == 1) ? 2U : 1U);
    return;
  }

  const auto* single_panel = std::get_if<pmi_codebook_typeI_single_panel>(&pmi_codebook);
  if (single_panel == nullptr) {
    return;
  }

  // The fields that the panel topology and the rank do not report have a zero bit-width.
  const pmi_typeI_single_panel_param_sizes sizes =
      get_pmi_sizes_typeI_single_panel(get_single_panel_info(single_panel->n1_n2), rank);
  packed.push_back(test_ue_cfg.i_1_1, sizes.i_1_1);
  packed.push_back(test_ue_cfg.i_1_2.value_or(0), sizes.i_1_2);
  packed.push_back(test_ue_cfg.i_1_3.value_or(0), sizes.i_1_3);
  packed.push_back(test_ue_cfg.i_2, sizes.i_2);
}

/// Fills a CSI Part 1 payload with the values configured for the test mode UE, following TS 38.212 Table 6.3.1.1.2-7.
static void fill_csi_bits(csi_report_packed&                              payload,
                          const csi_report_configuration&                 csi_rep_cfg,
                          const du_test_mode_config::test_mode_ue_config& test_ue_cfg)
{
  payload.resize(0);

  const bool has_pmi = (csi_rep_cfg.quantities == csi_report_quantities::cri_ri_pmi_cqi) ||
                       (csi_rep_cfg.quantities == csi_report_quantities::cri_ri_li_pmi_cqi);
  const bool has_cqi = has_pmi || (csi_rep_cfg.quantities == csi_report_quantities::cri_ri_cqi);
  if (not has_cqi) {
    return;
  }

  const unsigned            rank  = select_reported_rank(csi_rep_cfg.ri_restriction, test_ue_cfg.ri);
  const ri_li_cqi_cri_sizes sizes = get_ri_li_cqi_cri_sizes(
      csi_rep_cfg.pmi_codebook, csi_rep_cfg.ri_restriction, rank, csi_rep_cfg.nof_csi_rs_resources);

  // CRI. The test mode UE always reports the first CSI-RS resource.
  payload.push_back(0U, sizes.cri);

  // RI.
  payload.push_back(get_rank_index(csi_rep_cfg.ri_restriction, rank), sizes.ri);

  // LI.
  if (csi_rep_cfg.quantities == csi_report_quantities::cri_ri_li_pmi_cqi) {
    payload.push_back(0U, sizes.li);
  }

  // Padding, which makes the report size independent of the reported rank, precedes the PMI.
  const unsigned nof_pmi_bits   = has_pmi ? csi_report_get_size_pmi(csi_rep_cfg.pmi_codebook, rank) : 0;
  const unsigned nof_cqi_bits   = sizes.wideband_cqi_first_tb + sizes.wideband_cqi_second_tb;
  const unsigned report_size    = get_csi_report_pucch_size(csi_rep_cfg).part1_size.value();
  const unsigned nof_field_bits = payload.size() + nof_pmi_bits + nof_cqi_bits;
  ocudu_assert(report_size >= nof_field_bits,
               "The CSI report size (i.e., {} bits) is smaller than the generated fields (i.e., {} bits).",
               report_size,
               nof_field_bits);
  payload.push_back(0U, report_size - nof_field_bits);

  // PMI.
  if (has_pmi) {
    fill_pmi_bits(payload, csi_rep_cfg.pmi_codebook, rank, test_ue_cfg);
  }

  // Wideband CQI for the first TB and, for a rank higher than four, for the second TB.
  payload.push_back(test_ue_cfg.cqi, sizes.wideband_cqi_first_tb);
  payload.push_back(test_ue_cfg.cqi, sizes.wideband_cqi_second_tb);
}

static mac_uci_pdu::pucch_f0_or_f1_type make_f0f1_uci_pdu(const pucch_info&                               pucch,
                                                          const du_test_mode_config::test_mode_ue_config& test_ue_cfg)
{
  mac_uci_pdu::pucch_f0_or_f1_type pucch_ind;

  pucch_ind.ul_sinr_dB = 100;
  ocudu_assert(pucch.format() == pucch_format::FORMAT_0 or pucch.format() == pucch_format::FORMAT_1,
               "Expected PUCCH Format is F0 or F1");
  if (pucch.format() == pucch_format::FORMAT_0) {
    // In case of Format 0, unlike with Format 0, the GNB only schedules 1 PUCCH per slot; this PUCCH (and the
    // corresponding UCI indication) can have HARQ-ACK bits or SR bits, or both.
    if (pucch.uci_bits.sr_bits != sr_nof_bits::no_sr) {
      // In test mode, SRs are never detected, and instead BSR is injected.
      pucch_ind.sr_info.emplace();
      pucch_ind.sr_info.value().detected = false;
    }
    if (pucch.uci_bits.harq_ack_nof_bits > 0) {
      pucch_ind.harq_info.emplace();
      pucch_ind.harq_info->harqs.resize(pucch.uci_bits.harq_ack_nof_bits, uci_pucch_f0_or_f1_harq_values::ack);
    }
  } else {
    if (pucch.uci_bits.sr_bits != sr_nof_bits::no_sr) {
      // In test mode, SRs are never detected, and instead BSR is injected.
      pucch_ind.sr_info.emplace();
      pucch_ind.sr_info.value().detected = false;
    }
    if (pucch.uci_bits.harq_ack_nof_bits > 0) {
      pucch_ind.harq_info.emplace();
      // In case of PUCCH F1 with only HARQ-ACK bits, set all HARQ-ACK bits to ACK. If SR is included, then we
      // consider that the PUCCH is not detected.
      auto ack_val = pucch.uci_bits.sr_bits == sr_nof_bits::no_sr ? uci_pucch_f0_or_f1_harq_values::ack
                                                                  : uci_pucch_f0_or_f1_harq_values::dtx;
      pucch_ind.harq_info->harqs.resize(pucch.uci_bits.harq_ack_nof_bits, ack_val);
    }
  }
  return pucch_ind;
}

static mac_uci_pdu::pucch_f2_or_f3_or_f4_type
make_f2f3f4_uci_pdu(const pucch_info& pucch, const du_test_mode_config::test_mode_ue_config& test_ue_cfg)
{
  ocudu_assert(pucch.format() == pucch_format::FORMAT_2 or pucch.format() == pucch_format::FORMAT_3 or
                   pucch.format() == pucch_format::FORMAT_4,
               "Expected PUCCH Format is F2, F3 or F4");

  const sr_nof_bits sr_bits           = pucch.uci_bits.sr_bits;
  const unsigned    harq_ack_nof_bits = pucch.uci_bits.harq_ack_nof_bits;

  mac_uci_pdu::pucch_f2_or_f3_or_f4_type pucch_ind;
  pucch_ind.ul_sinr_dB = 100;
  if (sr_bits != sr_nof_bits::no_sr) {
    // Set SR to not detected.
    pucch_ind.sr_info.emplace();
    pucch_ind.sr_info->resize(to_underlying(sr_bits));
  }
  if (harq_ack_nof_bits > 0) {
    // Set all HARQ-ACK bits to ACK.
    pucch_ind.harq_info.emplace();
    pucch_ind.harq_info->is_valid = true;
    pucch_ind.harq_info->payload.resize(harq_ack_nof_bits);
    pucch_ind.harq_info->payload.fill();
  }
  if (pucch.csi_rep_cfg.has_value()) {
    pucch_ind.csi_part1_info.emplace();
    pucch_ind.csi_part1_info->is_valid = true;
    fill_csi_bits(pucch_ind.csi_part1_info->payload, pucch.csi_rep_cfg.value(), test_ue_cfg);
  }
  return pucch_ind;
}

mac_uci_pdu odu::create_uci_pdu(const pucch_info& pucch, const du_test_mode_config::test_mode_ue_config& test_ue_cfg)
{
  mac_uci_pdu pdu;
  pdu.rnti = pucch.crnti;
  switch (pucch.format()) {
    case pucch_format::FORMAT_0:
    case pucch_format::FORMAT_1:
      pdu.pdu = make_f0f1_uci_pdu(pucch, test_ue_cfg);
      break;
    case pucch_format::FORMAT_2:
    case pucch_format::FORMAT_3:
    case pucch_format::FORMAT_4:
      pdu.pdu = make_f2f3f4_uci_pdu(pucch, test_ue_cfg);
      break;
    default:
      report_fatal_error("Invalid format");
  }
  return pdu;
}

mac_uci_pdu odu::create_uci_pdu(const ul_sched_info& pusch, const du_test_mode_config::test_mode_ue_config& test_ue_cfg)
{
  mac_uci_pdu pdu;
  pdu.rnti                  = pusch.pusch_cfg.rnti;
  auto&           pusch_ind = pdu.pdu.emplace<mac_uci_pdu::pusch_type>();
  const uci_info& uci_info  = *pusch.uci;
  pusch_ind.ul_sinr_dB      = 100;
  if (uci_info.harq.has_value() and uci_info.harq->harq_ack_nof_bits > 0) {
    // If it has HARQ-ACK bits.
    pusch_ind.harq_info.emplace();
    pusch_ind.harq_info->is_valid = true;
    pusch_ind.harq_info->payload.resize(uci_info.harq.value().harq_ack_nof_bits);
    pusch_ind.harq_info->payload.fill();
  }
  if (uci_info.csi.has_value() and uci_info.csi->csi_part1_nof_bits > 0) {
    pusch_ind.csi_part1_info.emplace();
    pusch_ind.csi_part1_info->is_valid = true;
    fill_csi_bits(pusch_ind.csi_part1_info->payload, uci_info.csi->csi_rep_cfg, test_ue_cfg);
  }
  return pdu;
}

bool odu::pucch_info_and_uci_ind_match(const pucch_info& pucch, const mac_uci_pdu& uci_ind)
{
  if (pucch.crnti != uci_ind.rnti) {
    return false;
  }
  if ((pucch.format() == pucch_format::FORMAT_0 or pucch.format() == pucch_format::FORMAT_1) and
      std::holds_alternative<mac_uci_pdu::pucch_f0_or_f1_type>(uci_ind.pdu)) {
    const auto  pucch_pdu_sr_bits = pucch.uci_bits.sr_bits;
    const auto& f0f1_ind          = std::get<mac_uci_pdu::pucch_f0_or_f1_type>(uci_ind.pdu);
    if (f0f1_ind.sr_info.has_value() != (pucch_pdu_sr_bits != sr_nof_bits::no_sr)) {
      return false;
    }
    const auto pucch_pdu_harq_bits = pucch.uci_bits.harq_ack_nof_bits;
    if (f0f1_ind.harq_info.has_value() != (pucch_pdu_harq_bits > 0)) {
      return false;
    }
    return true;
  }
  if ((pucch.format() == pucch_format::FORMAT_2 or pucch.format() == pucch_format::FORMAT_3 or
       pucch.format() == pucch_format::FORMAT_4) and
      std::holds_alternative<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(uci_ind.pdu)) {
    const auto& f2f3f4_ind = std::get<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(uci_ind.pdu);

    const sr_nof_bits sr_bits           = pucch.uci_bits.sr_bits;
    const unsigned    harq_ack_nof_bits = pucch.uci_bits.harq_ack_nof_bits;

    if (f2f3f4_ind.sr_info.has_value() != (sr_bits != sr_nof_bits::no_sr)) {
      return false;
    }
    if (f2f3f4_ind.harq_info.has_value() != (harq_ack_nof_bits > 0)) {
      return false;
    }
    if (f2f3f4_ind.csi_part1_info.has_value() != pucch.csi_rep_cfg.has_value()) {
      return false;
    }
    return true;
  }
  return false;
}
