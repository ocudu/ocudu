// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcch.h"
#include "ocudu/fapi/p7/builders/dl_pdcch_pdu_builder.h"
#include "ocudu/fapi_adaptor/precoding_matrix_table_generator.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;
using namespace fapi_adaptor;

static std::mt19937 gen(0);

TEST(fapi_to_phy_pdcch_conversion_test, valid_pdu_conversion_success)
{
  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> bwp_size_dist(1, MAX_NOF_PRBS);
  std::uniform_int_distribution<unsigned> bwp_start_dist(0, MAX_NOF_PRBS - 1);
  std::uniform_int_distribution<unsigned> start_symbol_index_dist(0, 13);
  std::uniform_int_distribution<unsigned> duration_symbol_dist(0, 3);
  std::uniform_int_distribution<unsigned> shift_index_dist(0, MAX_NOF_PRBS);
  std::uniform_int_distribution<unsigned> n_rnti_dist(0, 65535);
  std::uniform_int_distribution<unsigned> cce_dist(0, 135);
  std::uniform_int_distribution<unsigned> aggregation_dist(0, 4);
  std::uniform_int_distribution<unsigned> nid_dmrs_dist(0, 65535);
  std::uniform_int_distribution<unsigned> nid_data_dist(0, 65535);

  auto                               pm_tools = generate_precoding_matrix_tables(pmi_codebook_one_port{}, 0);
  const precoding_matrix_repository& pm_repo  = *std::get<std::unique_ptr<precoding_matrix_repository>>(pm_tools);

  for (auto cp : {cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED}) {
    for (auto interleaved : {0U, 1U}) {
      for (auto reg_bundle : {2U, 3U, 6U}) {
        for (auto interleaver_size : {2U, 3U, 6U}) {
          for (auto id : {to_coreset_id(0), to_coreset_id(1U)}) {
            for (auto precoder : {coreset_configuration::precoder_granularity_type::same_as_reg_bundle,
                                  coreset_configuration::precoder_granularity_type::all_contiguous_rbs}) {
              for (int power_nr = -8; power_nr != -7; ++power_nr) {
                for (int power = -33; power != 3; power += 3) {
                  auto     scs                = subcarrier_spacing::kHz240;
                  unsigned sfn                = sfn_dist(gen);
                  unsigned slot_index         = slot_dist(gen);
                  auto     slot               = slot_point(scs, sfn, slot_index);
                  unsigned bwp_size           = bwp_size_dist(gen);
                  unsigned bwp_start          = std::min(bwp_start_dist(gen), bwp_size);
                  unsigned duration_symbol    = duration_symbol_dist(gen);
                  unsigned start_symbol_index = std::min(start_symbol_index_dist(gen), duration_symbol);
                  unsigned shift_index        = shift_index_dist(gen);
                  unsigned n_rnti             = n_rnti_dist(gen);
                  unsigned cce                = cce_dist(gen);
                  auto     aggregation        = aggregation_level::n2;
                  unsigned nid_dmrs           = nid_dmrs_dist(gen);
                  unsigned nid_data           = nid_data_dist(gen);

                  fapi::dl_pdcch_pdu         pdu;
                  fapi::dl_pdcch_pdu_builder builder(pdu);

                  freq_resource_bitmap freq_domain = {1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0,
                                                      0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
                                                      0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1};

                  // Always work with the biggest numerology.
                  builder.set_bwp_parameters(crb_interval::start_and_len(bwp_start, bwp_size), scs, cp);

                  builder.set_coreset_parameters(ofdm_symbol_range::start_and_len(start_symbol_index, duration_symbol),
                                                 precoder);

                  if (id == to_coreset_id(0)) {
                    builder.set_coreset_0_parameters(
                        coreset_configuration::interleaved_mapping_type{
                            (uint8_t)reg_bundle, (uint8_t)interleaver_size, (uint16_t)shift_index},
                        freq_domain);
                  } else if (interleaved == 0U) {
                    builder.set_non_interleaver_parameters(freq_domain);
                  } else {
                    builder.set_interleaver_parameters(
                        coreset_configuration::interleaved_mapping_type{
                            (uint8_t)reg_bundle, (uint8_t)interleaver_size, (uint16_t)shift_index},
                        freq_domain);
                  }

                  // Add DCI.
                  auto builder_dci = builder.get_dl_dci_pdu_builder();

                  builder_dci.set_ue_specific_parameters(to_rnti(0))
                      .set_data_scrambling_parameters(nid_data, n_rnti)
                      .set_control_channel_parameters(cce, aggregation);

                  builder_dci.set_profile_nr_tx_power_info_parameters(power_nr);
                  builder_dci.get_tx_precoding_and_beamforming_pdu_builder()
                      .set_prg_parameters(MAX_NOF_PRBS)
                      .set_pmi(0);

                  // Payload.
                  dci_payload payload = {1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1};
                  builder_dci.set_payload(payload);
                  builder_dci.set_dmrs_parameters(nid_dmrs);

                  pdcch_processor::pdu_t proc_pdu;

                  convert_pdcch_fapi_to_phy(proc_pdu, pdu, slot, pm_repo);

                  // Test basic parameters.
                  ASSERT_EQ(slot, proc_pdu.slot);
                  ASSERT_EQ(cp, proc_pdu.cp.value);

                  // Test coreset parameters.
                  ASSERT_EQ(bwp_size, proc_pdu.coreset.bwp_size_rb);
                  ASSERT_EQ(bwp_start, proc_pdu.coreset.bwp_start_rb);
                  ASSERT_EQ(start_symbol_index, proc_pdu.coreset.start_symbol_index);
                  ASSERT_EQ(duration_symbol, proc_pdu.coreset.duration);

                  if (id == to_coreset_id(0)) {
                    ASSERT_TRUE(proc_pdu.coreset.cce_to_reg_mapping ==
                                pdcch_processor::cce_to_reg_mapping_type::CORESET0);
                    ASSERT_EQ(0, proc_pdu.coreset.reg_bundle_size);
                    ASSERT_EQ(0, proc_pdu.coreset.interleaver_size);
                    ASSERT_EQ(shift_index, proc_pdu.coreset.shift_index);
                  } else {
                    if (interleaved == 0U) {
                      ASSERT_TRUE(proc_pdu.coreset.cce_to_reg_mapping ==
                                  pdcch_processor::cce_to_reg_mapping_type::NON_INTERLEAVED);
                      ASSERT_EQ(0, proc_pdu.coreset.reg_bundle_size);
                      ASSERT_EQ(0, proc_pdu.coreset.interleaver_size);
                      ASSERT_EQ(0, proc_pdu.coreset.shift_index);
                    } else {
                      ASSERT_TRUE(proc_pdu.coreset.cce_to_reg_mapping ==
                                  pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED);
                      ASSERT_EQ(reg_bundle, proc_pdu.coreset.reg_bundle_size);
                      ASSERT_EQ(interleaver_size, proc_pdu.coreset.interleaver_size);
                      ASSERT_EQ(shift_index, proc_pdu.coreset.shift_index);
                    }
                  }

                  // Test DCI.
                  ASSERT_EQ(n_rnti, proc_pdu.dci.n_rnti);
                  ASSERT_EQ(cce, proc_pdu.dci.cce_index);
                  ASSERT_EQ(aggregation, proc_pdu.dci.dci_aggregation_level);
                  ASSERT_EQ(nid_data, proc_pdu.dci.n_id_pdcch_data);
                  ASSERT_EQ(nid_dmrs, proc_pdu.dci.n_id_pdcch_dmrs);

                  // Test powers.
                  ASSERT_TRUE(std::abs(power_nr - proc_pdu.dci.dmrs_power_offset_dB) < 0.001F);
                  ASSERT_TRUE(std::abs(power_nr - proc_pdu.dci.data_power_offset_dB) < 0.001F);

                  // Test vectors.
                  for (unsigned i = 0, e = payload.size(); i != e; ++i) {
                    ASSERT_EQ(payload.test(i), bool(proc_pdu.dci.payload[i]));
                  }

                  // Test frequency domain resources.
                  for (unsigned i = 0, e = 45; i != e; ++i) {
                    ASSERT_EQ(freq_domain.test(i), bool(proc_pdu.coreset.frequency_resources.test(i)));
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}
