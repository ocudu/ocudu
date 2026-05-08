// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/fapi/p7/builders/dl_ssb_pdu_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(dl_ssb_pdu_builder, valid_basic_parameters_passes)
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  pci_t                 pci               = 106;
  ssb_pss_to_sss_epre   pss_profile       = ssb_pss_to_sss_epre::dB_0;
  ssb_id_t              block_index       = 3;
  ssb_subcarrier_offset subcarrier_offset = 2;
  unsigned              offset_pointA     = 39;
  ssb_pattern_case      case_type         = ssb_pattern_case::A;
  subcarrier_spacing    scs               = subcarrier_spacing::kHz15;
  unsigned              L_max             = 8;

  builder.set_carrier_parameters(scs)
      .set_cell_parameters(pci)
      .set_nr_power_parameters(pss_profile)
      .set_ssb_parameters(block_index, subcarrier_offset, offset_pointA, case_type, L_max);

  ASSERT_EQ(pci, pdu.phys_cell_id);
  const auto* profile_nr = std::get_if<dl_ssb_pdu::power_profile_nr>(&pdu.power_config);
  ASSERT_TRUE(profile_nr != nullptr);
  ASSERT_EQ(pss_profile, profile_nr->beta_pss);
  ASSERT_EQ(block_index, pdu.ssb_block_index);
  ASSERT_EQ(subcarrier_offset, pdu.subcarrier_offset);
  ASSERT_EQ(offset_pointA, pdu.ssb_offset_pointA.value());
  ASSERT_EQ(case_type, pdu.case_type);
  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(L_max, pdu.L_max);
}

TEST(dl_ssb_pdu_builder, valid_bch_payload_mixed_passes)
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  unsigned payload = 15453423;

  builder.set_bch_payload_phy_timing_info(payload);

  ASSERT_EQ(payload, pdu.bch_payload);
}
