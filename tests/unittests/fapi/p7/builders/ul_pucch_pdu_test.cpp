// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu_test_requirements.h"
#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/builders/ul_pucch_pdu_builder.h"
#include "ocudu/support/units.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(ul_pucch_pdu_builder, valid_ue_specific_parameters_passes)
{
  rnti_t rnti = to_rnti(14);

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_ue_specific_parameters(rnti);

  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(0, pdu.handle);
}

TEST(ul_pucch_pdu_builder, valid_allocation_time_parameters_passes)
{
  ofdm_symbol_range symbols = {4, 6};

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_time_allocation_parameters(symbols);

  ASSERT_EQ(symbols, pdu.symbols);
}

TEST(ul_pucch_pdu_builder, valid_format0_parameters_passes)
{
  OCUDU_TEST_REQUIREMENTS("DU-PHY-1");

  uint16_t    nid_pucch_hopping    = 200;
  uint16_t    initial_cyclic_shift = 7;
  bool        sr_present           = true;
  units::bits bit_len_harq(190);

  ul_pucch_pdu_format_0         pdu;
  ul_pucch_format_0_pdu_builder builder(pdu);

  builder.set_hopping_parameters(nid_pucch_hopping)
      .set_cyclic_shift_parameters(initial_cyclic_shift)
      .set_payload_parameters(sr_present, bit_len_harq);

  ASSERT_EQ(nid_pucch_hopping, pdu.nid_pucch_hopping);
  ASSERT_EQ(initial_cyclic_shift, pdu.initial_cyclic_shift);
  ASSERT_EQ(sr_present, pdu.sr_present);
  ASSERT_EQ(bit_len_harq, pdu.bit_len_harq);
}

TEST(ul_pucch_pdu_builder, valid_format1_parameters_passes)
{
  OCUDU_TEST_REQUIREMENTS("DU-PHY-1");

  uint16_t    nid_pucch_hopping     = 200;
  uint16_t    initial_cyclic_shift  = 7;
  uint8_t     time_domain_occ_index = 3;
  bool        sr_present            = true;
  units::bits bit_len_harq(190);

  ul_pucch_pdu_format_1         pdu;
  ul_pucch_format_1_pdu_builder builder(pdu);

  builder.set_hopping_parameters(nid_pucch_hopping)
      .set_cyclic_shift_parameters(initial_cyclic_shift)
      .set_time_domain_parameters(time_domain_occ_index)
      .set_payload_parameters(sr_present, bit_len_harq);

  ASSERT_EQ(nid_pucch_hopping, pdu.nid_pucch_hopping);
  ASSERT_EQ(initial_cyclic_shift, pdu.initial_cyclic_shift);
  ASSERT_EQ(time_domain_occ_index, pdu.time_domain_occ_index);
  ASSERT_EQ(sr_present, pdu.sr_present);
  ASSERT_EQ(bit_len_harq, pdu.bit_len_harq);
}

TEST(ul_pucch_pdu_builder, valid_format2_parameters_passes)
{
  OCUDU_TEST_REQUIREMENTS("DU-PHY-1");

  uint16_t    nid_pucch_scrambling       = 500;
  uint16_t    nid0_pucch_dmrs_scrambling = 34567;
  sr_nof_bits sr_bit_len                 = sr_nof_bits::one;
  units::bits csi_part1_bit_length(1024);
  units::bits bit_len_harq(190);

  ul_pucch_pdu_format_2         pdu;
  ul_pucch_format_2_pdu_builder builder(pdu);

  builder.set_scrambling_parameters(nid_pucch_scrambling, nid0_pucch_dmrs_scrambling)
      .set_payload_parameters(sr_bit_len, csi_part1_bit_length, bit_len_harq);

  ASSERT_EQ(nid_pucch_scrambling, pdu.nid_pucch_scrambling);
  ASSERT_EQ(nid0_pucch_dmrs_scrambling, pdu.nid0_pucch_dmrs_scrambling);
  ASSERT_EQ(sr_bit_len, pdu.sr_bit_len);
  ASSERT_EQ(csi_part1_bit_length, pdu.csi_part1_bit_length);
  ASSERT_EQ(bit_len_harq, pdu.bit_len_harq);
}

TEST(ul_pucch_pdu_builder, valid_format3_parameters_passes)
{
  OCUDU_TEST_REQUIREMENTS("DU-PHY-1");

  bool        pi2                        = false;
  uint16_t    nid_pucch_hopping          = 200;
  uint16_t    nid_pucch_scrambling       = 500;
  bool        add_dmrs_flag              = false;
  uint16_t    nid0_pucch_dmrs_scrambling = 0;
  uint8_t     m0_pucch_dmrs_cyclic_shift = 8;
  sr_nof_bits sr_bit_len                 = sr_nof_bits::one;
  units::bits csi_part1_bit_length(1024);
  units::bits bit_len_harq(190);

  ul_pucch_pdu_format_3         pdu;
  ul_pucch_format_3_pdu_builder builder(pdu);

  builder.set_modulation_parameters(pi2)
      .set_hopping_parameters(nid_pucch_hopping)
      .set_scrambling_parameters(nid_pucch_scrambling)
      .set_dmrs_parameters(add_dmrs_flag, nid0_pucch_dmrs_scrambling, m0_pucch_dmrs_cyclic_shift)
      .set_payload_parameters(sr_bit_len, csi_part1_bit_length, bit_len_harq);

  ASSERT_EQ(pi2, pdu.pi2_bpsk);
  ASSERT_EQ(nid_pucch_hopping, pdu.nid_pucch_hopping);
  ASSERT_EQ(nid_pucch_scrambling, pdu.nid_pucch_scrambling);
  ASSERT_EQ(add_dmrs_flag, pdu.add_dmrs_flag);
  ASSERT_EQ(nid0_pucch_dmrs_scrambling, pdu.nid0_pucch_dmrs_scrambling);
  ASSERT_EQ(m0_pucch_dmrs_cyclic_shift, pdu.m0_pucch_dmrs_cyclic_shift);
  ASSERT_EQ(sr_bit_len, pdu.sr_bit_len);
  ASSERT_EQ(csi_part1_bit_length, pdu.csi_part1_bit_length);
  ASSERT_EQ(bit_len_harq, pdu.bit_len_harq);
}

TEST(ul_pucch_pdu_builder, valid_format4_parameters_passes)
{
  OCUDU_TEST_REQUIREMENTS("DU-PHY-1");

  bool        pi2                        = false;
  uint16_t    nid_pucch_hopping          = 200;
  uint8_t     pre_dft_occ_idx            = 1;
  uint8_t     pre_dft_occ_len            = 2;
  uint16_t    nid_pucch_scrambling       = 500;
  bool        add_dmrs_flag              = false;
  uint16_t    nid0_pucch_dmrs_scrambling = 0;
  uint8_t     m0_pucch_dmrs_cyclic_shift = 8;
  sr_nof_bits sr_bit_len                 = sr_nof_bits::one;
  units::bits csi_part1_bit_length(1024);
  units::bits bit_len_harq(190);

  ul_pucch_pdu_format_4         pdu;
  ul_pucch_format_4_pdu_builder format_4_builder(pdu);

  format_4_builder.set_modulation_parameters(pi2)
      .set_hopping_parameters(nid_pucch_hopping)
      .set_occ_parameters(pre_dft_occ_idx, pre_dft_occ_len)
      .set_scrambling_parameters(nid_pucch_scrambling)
      .set_dmrs_parameters(add_dmrs_flag, nid0_pucch_dmrs_scrambling, m0_pucch_dmrs_cyclic_shift)
      .set_payload_parameters(sr_bit_len, csi_part1_bit_length, bit_len_harq);

  ASSERT_EQ(pi2, pdu.pi2_bpsk);
  ASSERT_EQ(nid_pucch_hopping, pdu.nid_pucch_hopping);
  ASSERT_EQ(pre_dft_occ_idx, pdu.pre_dft_occ_idx);
  ASSERT_EQ(pre_dft_occ_len, pdu.pre_dft_occ_len);
  ASSERT_EQ(nid_pucch_scrambling, pdu.nid_pucch_scrambling);
  ASSERT_EQ(add_dmrs_flag, pdu.add_dmrs_flag);
  ASSERT_EQ(nid0_pucch_dmrs_scrambling, pdu.nid0_pucch_dmrs_scrambling);
  ASSERT_EQ(m0_pucch_dmrs_cyclic_shift, pdu.m0_pucch_dmrs_cyclic_shift);
  ASSERT_EQ(sr_bit_len, pdu.sr_bit_len);
  ASSERT_EQ(csi_part1_bit_length, pdu.csi_part1_bit_length);
  ASSERT_EQ(bit_len_harq, pdu.bit_len_harq);
}

TEST(ul_pucch_pdu_builder, valid_bwp_parameters_passes)
{
  crb_interval       bwp = {34, 35};
  subcarrier_spacing scs = subcarrier_spacing::kHz120;
  cyclic_prefix      cp  = cyclic_prefix::NORMAL;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_bwp_parameters(bwp, scs, cp);

  ASSERT_EQ(bwp, pdu.bwp);
  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cp, pdu.cp);
}

TEST(ul_pucch_pdu_builder, valid_frequency_allocation_parameters_passes)
{
  prb_interval prbs{8, 10};

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_frequency_allocation_parameters(prbs);

  ASSERT_EQ(prbs, pdu.prbs);
}

TEST(ul_pucch_pdu_builder, valid_hopping_information_parameters_passes)
{
  unsigned second_hop_prb = 100;

  ul_pucch_pdu         pdu;
  ul_pucch_pdu_builder builder(pdu);

  builder.set_hopping_information_parameters(second_hop_prb);

  ASSERT_TRUE(pdu.second_hop_prb.has_value());
  ASSERT_EQ(second_hop_prb, *pdu.second_hop_prb);
}
