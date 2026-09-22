// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "pucch.h"
#include "tests/ocudu_test_requirements.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

TEST(mac_fapi_ul_pucch_format1_pdu_conversor_test, ul_pucch_format1_pdu_valid_should_pass)
{
  const pucch_info_test_helper& pdu_test = build_valid_pucch_format_1_pdu();
  const pucch_info&             mac_pdu  = pdu_test.info;
  fapi::ul_pucch_pdu            fapi_pdu;
  fapi::ul_pucch_pdu_builder    builder(fapi_pdu);

  convert_pucch_mac_to_fapi(builder, mac_pdu);

  const auto* format1 = std::get_if<fapi::ul_pucch_pdu_format_1>(&fapi_pdu.format);
  ASSERT_TRUE(format1 != nullptr);

  // BWP.
  ASSERT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs, fapi_pdu.bwp);

  ASSERT_EQ(mac_pdu.crnti, fapi_pdu.rnti);

  // Resources.
  ASSERT_EQ(mac_pdu.grant_prbs(), fapi_pdu.prbs);
  ASSERT_EQ(mac_pdu.res->syms, fapi_pdu.symbols);
  ASSERT_TRUE(fapi_pdu.second_hop_prb.has_value());
  ASSERT_EQ(*mac_pdu.res->second_hop_prb, *fapi_pdu.second_hop_prb);

  // Format 1.
  const auto& res_f1 = std::get<pucch_resource::f1_config>(mac_pdu.res->format_params);
  ASSERT_EQ(res_f1.time_domain_occ, format1->time_domain_occ_index);
  ASSERT_EQ(res_f1.initial_cyclic_shift, format1->initial_cyclic_shift);
  const auto& f1 = std::get<pucch_info::f1_config>(mac_pdu.format_params);
  ASSERT_EQ(f1.n_id_hopping, format1->nid_pucch_hopping);

  // UCI bits.
  ASSERT_EQ(static_cast<unsigned>(mac_pdu.uci_bits.sr_bits), static_cast<uint8_t>(format1->sr_present));
  ASSERT_EQ(mac_pdu.uci_bits.harq_ack_nof_bits, format1->bit_len_harq.value());

  // Multi-slot TX indicator (common to all formats).
  ASSERT_EQ(fapi_pdu.multi_slot_tx_indicator,
            mac_pdu.repetition.has_value() ? mac_pdu.repetition->position : pucch_repetition_tx_slot::no_multi_slot);
}

TEST(mac_fapi_ul_pucch_format1_pdu_conversor_test, multi_slot_tx_indicator_is_forwarded)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-BW-17-1");

  // Repetition state used to be a per-format field that was silently dropped at the FAPI boundary; verify that it now
  // reaches the FAPI PDU's multi_slot_tx_indicator from the common part of pucch_info.
  pucch_info_test_helper pdu_test    = build_valid_pucch_format_1_pdu();
  pdu_test.info.repetition           = pucch_info::repetition_info{slot_point{}, pucch_repetition_tx_slot::starts};
  const pucch_info&          mac_pdu = pdu_test.info;
  fapi::ul_pucch_pdu         fapi_pdu;
  fapi::ul_pucch_pdu_builder builder(fapi_pdu);

  convert_pucch_mac_to_fapi(builder, mac_pdu);

  ASSERT_EQ(fapi_pdu.multi_slot_tx_indicator, pucch_repetition_tx_slot::starts);
}

TEST(mac_fapi_ul_pucch_format2_pdu_conversor_test, ul_pucch_format2_pdu_valid_should_pass)
{
  const pucch_info_test_helper& pdu_test = build_valid_pucch_format_2_pdu();
  const pucch_info&             mac_pdu  = pdu_test.info;
  fapi::ul_pucch_pdu            fapi_pdu;
  fapi::ul_pucch_pdu_builder    builder(fapi_pdu);

  convert_pucch_mac_to_fapi(builder, mac_pdu);

  const auto* format2 = std::get_if<fapi::ul_pucch_pdu_format_2>(&fapi_pdu.format);
  ASSERT_TRUE(format2 != nullptr);

  // BWP.
  ASSERT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs, fapi_pdu.bwp);

  ASSERT_EQ(mac_pdu.crnti, fapi_pdu.rnti);

  // Resources.
  ASSERT_TRUE(fapi_pdu.second_hop_prb.has_value());
  ASSERT_EQ(mac_pdu.grant_prbs(), fapi_pdu.prbs);
  ASSERT_EQ(mac_pdu.res->syms, fapi_pdu.symbols);
  ASSERT_EQ(mac_pdu.res->second_hop_prb.value(), fapi_pdu.second_hop_prb.value());

  // Format 2.
  const auto& f2 = std::get<pucch_info::f2_config>(mac_pdu.format_params);
  ASSERT_EQ(f2.n_id_0_scrambling, format2->nid0_pucch_dmrs_scrambling);
  ASSERT_EQ(f2.n_id_scrambling, format2->nid_pucch_scrambling);

  // UCI bits.
  ASSERT_EQ(static_cast<unsigned>(mac_pdu.uci_bits.sr_bits), static_cast<uint8_t>(format2->sr_bit_len));
  ASSERT_EQ(mac_pdu.uci_bits.harq_ack_nof_bits, format2->bit_len_harq.value());
  ASSERT_EQ(mac_pdu.uci_bits.csi_part1_nof_bits, format2->csi_part1_bit_length.value());
}

TEST(mac_fapi_ul_pucch_format3_pdu_conversor_test, ul_pucch_format3_pdu_valid_should_pass)
{
  const pucch_info_test_helper& pdu_test = build_valid_pucch_format_3_pdu();
  const pucch_info&             mac_pdu  = pdu_test.info;
  fapi::ul_pucch_pdu            fapi_pdu;
  fapi::ul_pucch_pdu_builder    builder(fapi_pdu);

  convert_pucch_mac_to_fapi(builder, mac_pdu);

  const auto* format3 = std::get_if<fapi::ul_pucch_pdu_format_3>(&fapi_pdu.format);
  ASSERT_TRUE(format3 != nullptr);

  // BWP.
  ASSERT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs, fapi_pdu.bwp);

  ASSERT_EQ(mac_pdu.crnti, fapi_pdu.rnti);

  // Resources.
  ASSERT_TRUE(fapi_pdu.second_hop_prb.has_value());
  ASSERT_EQ(mac_pdu.grant_prbs(), fapi_pdu.prbs);
  ASSERT_EQ(mac_pdu.res->syms, fapi_pdu.symbols);
  ASSERT_EQ(mac_pdu.res->second_hop_prb.value(), fapi_pdu.second_hop_prb.value());

  // Format 3.
  const auto& res_f3 = std::get<pucch_resource::f3_config>(mac_pdu.res->format_params);
  ASSERT_EQ(res_f3.additional_dmrs, format3->add_dmrs_flag);
  ASSERT_EQ(res_f3.pi_2_bpsk, format3->pi2_bpsk);
  const auto& f3 = std::get<pucch_info::f3_config>(mac_pdu.format_params);
  ASSERT_EQ(f3.n_id_0_scrambling, format3->nid0_pucch_dmrs_scrambling);
  ASSERT_EQ(f3.n_id_scrambling, format3->nid_pucch_scrambling);
  ASSERT_EQ(f3.n_id_hopping, format3->nid_pucch_hopping);

  // UCI bits.
  ASSERT_EQ(static_cast<unsigned>(mac_pdu.uci_bits.sr_bits), static_cast<uint8_t>(format3->sr_bit_len));
  ASSERT_EQ(mac_pdu.uci_bits.harq_ack_nof_bits, format3->bit_len_harq.value());
  ASSERT_EQ(mac_pdu.uci_bits.csi_part1_nof_bits, format3->csi_part1_bit_length.value());
}

TEST(mac_fapi_ul_pucch_format4_pdu_conversor_test, ul_pucch_format4_pdu_valid_should_pass)
{
  const pucch_info_test_helper& pdu_test = build_valid_pucch_format_4_pdu();
  const pucch_info&             mac_pdu  = pdu_test.info;
  fapi::ul_pucch_pdu            fapi_pdu;
  fapi::ul_pucch_pdu_builder    builder(fapi_pdu);

  convert_pucch_mac_to_fapi(builder, mac_pdu);

  const auto* format4 = std::get_if<fapi::ul_pucch_pdu_format_4>(&fapi_pdu.format);
  ASSERT_TRUE(format4 != nullptr);

  // BWP.
  ASSERT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs, fapi_pdu.bwp);

  ASSERT_EQ(mac_pdu.crnti, fapi_pdu.rnti);

  // Resources.
  ASSERT_TRUE(fapi_pdu.second_hop_prb.has_value());
  ASSERT_EQ(mac_pdu.grant_prbs(), fapi_pdu.prbs);
  ASSERT_EQ(mac_pdu.res->syms, fapi_pdu.symbols);
  ASSERT_EQ(mac_pdu.res->second_hop_prb.value(), fapi_pdu.second_hop_prb.value());

  // Format 4.
  const auto& res_f4 = std::get<pucch_resource::f4_config>(mac_pdu.res->format_params);
  ASSERT_EQ(res_f4.additional_dmrs, format4->add_dmrs_flag);
  ASSERT_EQ(res_f4.pi_2_bpsk, format4->pi2_bpsk);
  ASSERT_EQ(static_cast<unsigned>(res_f4.occ_length), format4->pre_dft_occ_len);
  ASSERT_EQ(static_cast<unsigned>(res_f4.occ_index), format4->pre_dft_occ_idx);
  const auto& f4 = std::get<pucch_info::f4_config>(mac_pdu.format_params);
  ASSERT_EQ(f4.n_id_0_scrambling, format4->nid0_pucch_dmrs_scrambling);
  ASSERT_EQ(f4.n_id_scrambling, format4->nid_pucch_scrambling);
  ASSERT_EQ(f4.n_id_hopping, format4->nid_pucch_hopping);

  // UCI bits.
  ASSERT_EQ(static_cast<unsigned>(mac_pdu.uci_bits.sr_bits), static_cast<uint8_t>(format4->sr_bit_len));
  ASSERT_EQ(mac_pdu.uci_bits.harq_ack_nof_bits, format4->bit_len_harq.value());
  ASSERT_EQ(mac_pdu.uci_bits.csi_part1_nof_bits, format4->csi_part1_bit_length.value());
}
