// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/format.h"
#include "ocudu/fapi/p7/builders/dl_pdcch_pdu_builder.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi;

TEST(dl_pdcch_pdu_builder, valid_bwp_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  crb_interval       coreset_bwp = {100, 100};
  subcarrier_spacing scs         = subcarrier_spacing::kHz60;
  cyclic_prefix      cp          = cyclic_prefix::NORMAL;

  builder.set_bwp_parameters(coreset_bwp, scs, cp);

  ASSERT_EQ(coreset_bwp, pdu.coreset_bwp);
  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cp, pdu.cp);
}

TEST(dl_pdcch_pdu_builder, valid_coreset_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  ofdm_symbol_range                                symbols = {2, 7};
  coreset_configuration::precoder_granularity_type granularity =
      coreset_configuration::precoder_granularity_type::same_as_reg_bundle;

  builder.set_coreset_parameters(symbols, granularity);

  ASSERT_EQ(symbols, pdu.symbols);
  ASSERT_EQ(granularity, pdu.precoder_granularity);
}

TEST(dl_pdcch_pdu_builder, valid_coreset_0_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  uint8_t                                         reg_size         = 3;
  uint8_t                                         interleaver_size = 3;
  uint16_t                                        shift_index      = 100;
  coreset_configuration::interleaved_mapping_type interleaved      = {reg_size, interleaver_size, shift_index};

  freq_resource_bitmap freq_domain = {1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1,
                                      1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1};

  builder.set_coreset_0_parameters(interleaved, freq_domain);

  ASSERT_TRUE(std::holds_alternative<dl_pdcch_pdu::mapping_coreset_0>(pdu.mapping));
  ASSERT_EQ(interleaved, std::get<dl_pdcch_pdu::mapping_coreset_0>(pdu.mapping).interleaved);
  ASSERT_EQ(freq_domain, pdu.freq_domain_resource);
}

TEST(dl_pdcch_pdu_builder, valid_interleaver_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  uint8_t                                         reg_size         = 3;
  uint8_t                                         interleaver_size = 3;
  uint16_t                                        shift_index      = 100;
  coreset_configuration::interleaved_mapping_type interleaved      = {reg_size, interleaver_size, shift_index};

  freq_resource_bitmap freq_domain = {1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1,
                                      1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1};

  builder.set_interleaver_parameters(interleaved, freq_domain);

  ASSERT_TRUE(std::holds_alternative<dl_pdcch_pdu::mapping_interleaved>(pdu.mapping));
  ASSERT_EQ(interleaved, std::get<dl_pdcch_pdu::mapping_interleaved>(pdu.mapping).interleaved);
  ASSERT_EQ(freq_domain, pdu.freq_domain_resource);
}

TEST(dl_pdcch_pdu_builder, valid_non_interleaver_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);

  unsigned reg_size = 6U;

  freq_resource_bitmap freq_domain = {1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1,
                                      1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1};

  builder.set_non_interleaver_parameters(freq_domain);

  ASSERT_TRUE(std::holds_alternative<dl_pdcch_pdu::mapping_non_interleaved>(pdu.mapping));
  ASSERT_EQ(reg_size, std::get<dl_pdcch_pdu::mapping_non_interleaved>(pdu.mapping).reg_bundle_sz);
  ASSERT_EQ(freq_domain, pdu.freq_domain_resource);
}

TEST(dl_pdcch_pdu_builder, valid_dci_ue_specific_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);
  dl_dci_pdu_builder   builder_dci = builder.get_dl_dci_pdu_builder();

  rnti_t rnti = to_rnti(100);
  builder_dci.set_ue_specific_parameters(rnti);

  ASSERT_EQ(rnti, pdu.dl_dci.rnti);
}

TEST(dl_pdcch_pdu_builder, valid_dci_control_channel_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);
  dl_dci_pdu_builder   builder_dci = builder.get_dl_dci_pdu_builder();

  unsigned          cce_index             = 100;
  aggregation_level dci_aggregation_level = aggregation_level::n4;
  builder_dci.set_control_channel_parameters(cce_index, dci_aggregation_level);

  ASSERT_EQ(cce_index, pdu.dl_dci.cce_index);
  ASSERT_EQ(dci_aggregation_level, pdu.dl_dci.dci_aggregation_level);
}

TEST(dl_pdcch_pdu_builder, valid_dci_data_scrambling_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);
  dl_dci_pdu_builder   builder_dci = builder.get_dl_dci_pdu_builder();

  unsigned nid_pdcch_data   = 200;
  unsigned nrnti_pdcch_data = 200;
  builder_dci.set_data_scrambling_parameters(nid_pdcch_data, nrnti_pdcch_data);

  ASSERT_EQ(nid_pdcch_data, pdu.dl_dci.nid_pdcch_data);
  ASSERT_EQ(nrnti_pdcch_data, pdu.dl_dci.nrnti_pdcch_data);
}

TEST(dl_pdcch_pdu_builder, valid_dci_tx_power_parameters_passes)
{
  for (int i = -8; i != 9; ++i) {
    dl_pdcch_pdu         pdu;
    dl_pdcch_pdu_builder builder(pdu);
    dl_dci_pdu_builder   builder_dci = builder.get_dl_dci_pdu_builder();

    builder_dci.set_profile_nr_tx_power_info_parameters(i);

    const auto* profile = std::get_if<dl_dci_pdu::power_profile_nr>(&pdu.dl_dci.power_config);
    ASSERT_TRUE(profile);
    ASSERT_EQ(i, profile->power_control_offset_ss_db);
  }
}

TEST(dl_pdcch_pdu_builder, valid_dci_profile_sss_tx_power_parameters_passes)
{
  for (int i = -8; i != 9; ++i) {
    dl_pdcch_pdu         pdu;
    dl_pdcch_pdu_builder builder(pdu);
    dl_dci_pdu_builder   builder_dci = builder.get_dl_dci_pdu_builder();

    builder_dci.set_profile_sss_tx_power_info_parameters(i, i - 10);

    const auto* profile = std::get_if<dl_dci_pdu::power_profile_sss>(&pdu.dl_dci.power_config);
    ASSERT_TRUE(profile);
    ASSERT_FLOAT_EQ(i, profile->dmrs_power_offset_db);
    ASSERT_FLOAT_EQ(i - 10, profile->data_power_offset_db);
  }
}

TEST(dl_pdcch_pdu_builder, valid_dmrs_parameters_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);
  dl_dci_pdu_builder   builder_dci = builder.get_dl_dci_pdu_builder();

  unsigned nid = 100;
  builder_dci.set_dmrs_parameters(nid);

  ASSERT_EQ(nid, pdu.dl_dci.nid_pdcch_dmrs);
}

TEST(dl_pdcch_pdu_builder, add_valid_dci_payload_passes)
{
  dl_pdcch_pdu         pdu;
  dl_pdcch_pdu_builder builder(pdu);
  dl_dci_pdu_builder   builder_dci = builder.get_dl_dci_pdu_builder();

  dci_payload payload = {1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1};
  builder_dci.set_payload(payload);

  ASSERT_EQ(payload, pdu.dl_dci.payload);
}
