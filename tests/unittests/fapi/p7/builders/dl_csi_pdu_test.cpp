// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/builders/dl_csi_rs_pdu_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(dl_csi_pdu_builder, valid_resource_config_parameters_passes)
{
  dl_csi_rs_pdu         pdu;
  dl_csi_rs_pdu_builder builder(pdu);

  csi_rs_type     type     = csi_rs_type::CSI_RS_ZP;
  unsigned        row      = 10;
  csi_rs_cdm_type cdm      = csi_rs_cdm_type::cdm8_FD2_TD4;
  unsigned        scram_id = 523;

  builder.set_csi_resource_config_parameters(type, row, cdm, scram_id);

  ASSERT_EQ(type, pdu.type);
  ASSERT_EQ(row, pdu.row);
  ASSERT_EQ(cdm, pdu.cdm_type);
  ASSERT_EQ(scram_id, pdu.scramb_id);
}

TEST(dl_csi_pdu_builder, valid_frequency_domain_parameters_passes)
{
  dl_csi_rs_pdu         pdu;
  dl_csi_rs_pdu_builder builder(pdu);

  bounded_bitset<12, false> freq_domain  = {0, 0, 1};
  csi_rs_freq_density_type  freq_density = csi_rs_freq_density_type::one;

  builder.set_frequency_domain_parameters(freq_domain, freq_density);

  ASSERT_EQ(freq_domain, pdu.freq_domain);
  ASSERT_EQ(freq_density, pdu.freq_density);
}

TEST(dl_csi_pdu_builder, valid_time_domain_parameters_passes)
{
  dl_csi_rs_pdu         pdu;
  dl_csi_rs_pdu_builder builder(pdu);

  unsigned sym_l0 = 2;
  unsigned sym_l1 = 3;

  builder.set_time_domain_parameters(sym_l0, sym_l1);

  ASSERT_EQ(sym_l0, pdu.symb_L0);
  ASSERT_EQ(sym_l1, pdu.symb_L1);
}

TEST(dl_csi_pdu_builder, valid_resource_block_parameters_passes)
{
  dl_csi_rs_pdu         pdu;
  dl_csi_rs_pdu_builder builder(pdu);

  crb_interval crbs = {150, 200};

  builder.set_resource_block_parameters(crbs);

  ASSERT_EQ(crbs, pdu.crbs);
}

TEST(dl_csi_pdu_builder, valid_bwp_parameters_passes)
{
  dl_csi_rs_pdu         pdu;
  dl_csi_rs_pdu_builder builder(pdu);

  subcarrier_spacing scs      = subcarrier_spacing::kHz60;
  cyclic_prefix      cyclic_p = cyclic_prefix::NORMAL;

  crb_interval bwp = {56U, 60U};

  builder.set_bwp_parameters(scs, cyclic_p, bwp);

  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cyclic_p, pdu.cp);
  ASSERT_EQ(bwp, pdu.bwp);
}

TEST(dl_csi_pdu_builder, valid_nr_tx_power_info_parameters_passes)
{
  for (auto power : {0, -8}) {
    dl_csi_rs_pdu         pdu;
    dl_csi_rs_pdu_builder builder(pdu);

    power_control_offset_ss ss = power_control_offset_ss::dB3;

    builder.set_profile_nr_tx_power_info_parameters(power, ss);

    const auto* profile_nr = std::get_if<fapi::dl_csi_rs_pdu::power_profile_nr>(&pdu.power_config);
    ASSERT_TRUE(profile_nr != nullptr);
    ASSERT_EQ(ss, profile_nr->pwr_control_offset_ss);
    ASSERT_EQ(power, profile_nr->pwr_control_offset_db);
  }
}

TEST(dl_csi_pdu_builder, valid_sss_tx_power_info_parameters_passes)
{
  const float power_offset_db = 10;

  dl_csi_rs_pdu         pdu;
  dl_csi_rs_pdu_builder builder(pdu);

  builder.set_profile_sss_tx_power_info_parameters(power_offset_db);

  const auto* profile_sss = std::get_if<fapi::dl_csi_rs_pdu::power_profile_sss>(&pdu.power_config);
  ASSERT_TRUE(profile_sss != nullptr);
  ASSERT_EQ(power_offset_db, profile_sss->pwr_offset_db);
}
