// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdsch.h"
#include "ocudu/fapi/p7/builders/dl_pdsch_pdu_builder.h"
#include "ocudu/fapi_adaptor/precoding_matrix_table_generator.h"
#include "ocudu/ran/pdcch/dci_format.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;
using namespace fapi_adaptor;

static std::mt19937 gen(0);

static float calculate_ratio_pdsch_data_to_sss_dB(int profile_nr, fapi::power_control_offset_ss profile_ss_nr)
{
  float power_control_offset_ss_dB = 0.0F;
  switch (profile_ss_nr) {
    case fapi::power_control_offset_ss::dB_minus_3:
      power_control_offset_ss_dB = -3.F;
      break;
    case fapi::power_control_offset_ss::dB0:
      power_control_offset_ss_dB = .0F;
      break;
    case fapi::power_control_offset_ss::dB3:
      power_control_offset_ss_dB = 3.F;
      break;
    case fapi::power_control_offset_ss::dB6:
    default:
      power_control_offset_ss_dB = 6.F;
      break;
  }

  return static_cast<float>(profile_nr) + power_control_offset_ss_dB;
}

static float calculate_ratio_pdsch_dmrs_to_sss_dB(float ratio_pdsch_data_to_sss_dB, unsigned num_dmrs_cdm_grps_no_data)
{
  static const std::array<float, 4> beta_dmrs_values = {NAN, 0, -3, -4.77};

  return ratio_pdsch_data_to_sss_dB + beta_dmrs_values[num_dmrs_cdm_grps_no_data];
}

static rb_allocation make_freq_allocation(fapi::dl_pdsch_pdu::vrb_to_prb_mapping_t pdu_mapping,
                                          unsigned                                 bwp_start,
                                          unsigned                                 bwp_size,
                                          unsigned                                 coreset_start,
                                          unsigned                                 initial_bwp_size,
                                          vrb_to_prb::mapping_type                 vrb_prb_mapping,
                                          unsigned                                 rb_start,
                                          unsigned                                 rb_size)
{
  // Make VRB-to-PRB mapping.
  vrb_to_prb::configuration vrb_to_prb_configuration;

  if (std::holds_alternative<fapi::dl_pdsch_pdu::non_interleaved_common_ss>(pdu_mapping)) {
    vrb_to_prb_configuration = vrb_to_prb::create_non_interleaved_common_ss(0U);
  } else if (std::holds_alternative<fapi::dl_pdsch_pdu::non_interleaved_other>(pdu_mapping)) {
    vrb_to_prb_configuration = vrb_to_prb::create_non_interleaved_other();
  } else if (std::holds_alternative<fapi::dl_pdsch_pdu::interleaved_common_type0_coreset0>(pdu_mapping)) {
    vrb_to_prb_configuration = vrb_to_prb::create_interleaved_coreset0(coreset_start, initial_bwp_size);
  } else if (std::holds_alternative<fapi::dl_pdsch_pdu::interleaved_common_any_coreset0_present>(pdu_mapping)) {
    vrb_to_prb_configuration = vrb_to_prb::create_interleaved_common_ss(coreset_start, bwp_start, initial_bwp_size);

  } else if (std::holds_alternative<fapi::dl_pdsch_pdu::interleaved_other>(pdu_mapping)) {
    vrb_to_prb_configuration = vrb_to_prb::create_interleaved_other(bwp_start, bwp_size, vrb_prb_mapping);
  }

  return rb_allocation::make_type1(rb_start, rb_size, vrb_to_prb_configuration);
}

static void set_vrb_to_prb_mapping_into_fapi_pdu(fapi::dl_pdsch_pdu_builder& builder,
                                                 vrb_to_prb::mapping_type    vrb_prb_mapping,
                                                 dci_dl_format               dci_format,
                                                 search_space_set_type       ss,
                                                 unsigned                    coreset_start,
                                                 fapi::pdsch_ref_point_type  ref_point,
                                                 unsigned                    initial_bwp_size)
{
  // Non-interleaved cases.
  if (vrb_prb_mapping == vrb_to_prb::mapping_type::non_interleaved) {
    if (dci_format == dci_dl_format::f1_0 && is_common_search_space(ss)) {
      builder.set_vrb_to_prb_non_interleaved_common_ss_parameters();
      return;
    }

    builder.set_vrb_to_prb_non_interleaved_other_parameters();
    return;
  }

  // Interleaved cases for DCI 1_0, CORESET0 and in Common Search Space.
  if (ref_point == fapi::pdsch_ref_point_type::point_a && is_common_search_space(ss) &&
      dci_format == dci_dl_format::f1_0) {
    if (ss == search_space_set_type::type0) {
      builder.set_vrb_to_prb_interleaved_common_type0_coreset0_parameters(coreset_start, initial_bwp_size);
      return;
    }

    builder.set_vrb_to_prb_interleaved_common_any_coreset0_present_parameters(coreset_start, initial_bwp_size);
    return;
  }

  builder.set_vrb_to_prb_interleaved_other_parameters();
}

TEST(fapi_to_phy_pdsch_conversion_test, valid_pdu_conversion_success)
{
  // Random generators.
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> slot_dist(0, 159);
  std::uniform_int_distribution<unsigned> rnti_dist(1, 65535);
  std::uniform_int_distribution<unsigned> bwp_size_dist(1, 275);
  std::uniform_int_distribution<unsigned> bwp_start_dist(0, 274);
  std::uniform_int_distribution<unsigned> nid_pdsch_dist(0, 1023);
  std::uniform_int_distribution<unsigned> dmrs_scrambling_dist(0, 65535);
  std::uniform_int_distribution<unsigned> binary_dist(0, 1);
  std::uniform_int_distribution<unsigned> dmrs_cdm_grps_no_data_dist(1, 3);
  std::uniform_int_distribution<unsigned> nr_of_symbols_dist(1, 14);
  std::uniform_int_distribution<unsigned> start_symbol_index_dist(0, 13);

  auto                               pm_tools = generate_precoding_matrix_tables(1, 0);
  const precoding_matrix_repository& pm_repo  = *std::get<std::unique_ptr<precoding_matrix_repository>>(pm_tools);

  for (auto cyclic_p : {cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED}) {
    for (auto ref_point : {fapi::pdsch_ref_point_type::point_a, fapi::pdsch_ref_point_type::subcarrier_0}) {
      for (auto config_type : {dmrs_config_type::type1, dmrs_config_type::type2}) {
        // Iterate possible VRB-to PRB mapping. As transmission type is enabled
        // vrb_to_prb_mapping_type::non_interleaved value is irrelevant.
        for (auto vrb_prb_mapping : {vrb_to_prb::mapping_type::non_interleaved,
                                     vrb_to_prb::mapping_type::interleaved_n4,
                                     vrb_to_prb::mapping_type::interleaved_n2}) {
          // Iterate all possible NZP-CSI-RS to SSS ratios. L1_use_profile_sss means SSS profile mode.
          for (auto power_ss_profile_nr : {fapi::power_control_offset_ss::dB_minus_3,
                                           fapi::power_control_offset_ss::dB0,
                                           fapi::power_control_offset_ss::dB3,
                                           fapi::power_control_offset_ss::dB6}) {
            // Iterate possible PDSCH data to NZP-CSI-RS ratios for Profile NR. It is ignored when
            // power_ss_profile_nr is L1_use_profile_sss.
            for (int power_profile_nr = -8; power_profile_nr != -7; ++power_profile_nr) {
              for (auto ldpc_graph : {ldpc_base_graph_type::BG1, ldpc_base_graph_type::BG2}) {
                for (auto dci_format : {dci_dl_format::f1_0, dci_dl_format::f1_1}) {
                  for (auto ss : {search_space_set_type::type0, search_space_set_type::ue_specific}) {
                    unsigned         sfn                   = sfn_dist(gen);
                    unsigned         slot_index            = slot_dist(gen);
                    auto             scs                   = subcarrier_spacing::kHz240;
                    slot_point       slot                  = slot_point(scs, sfn, slot_index);
                    rnti_t           rnti                  = to_rnti(rnti_dist(gen));
                    unsigned         bwp_size              = bwp_size_dist(gen);
                    unsigned         bwp_start             = std::min(bwp_start_dist(gen), bwp_size);
                    unsigned         nid_pdsch             = nid_pdsch_dist(gen);
                    unsigned         scrambling_id         = dmrs_scrambling_dist(gen);
                    bool             n_scid                = binary_dist(gen);
                    unsigned         dmrs_cdm_grps_no_data = dmrs_cdm_grps_no_data_dist(gen);
                    unsigned         nr_of_symbols         = nr_of_symbols_dist(gen);
                    unsigned         start_symbol_index    = std::min(start_symbol_index_dist(gen), nr_of_symbols);
                    unsigned         coreset_start         = bwp_size_dist(gen);
                    unsigned         initial_bwp_size      = bwp_size_dist(gen);
                    units::bytes     tb_size_lbrm_bytes{50};
                    dmrs_symbol_mask dl_dmrs_symbol(13);
                    dl_dmrs_symbol.from_uint64(start_symbol_index_dist(gen));
                    unsigned              rb_size  = nr_of_symbols_dist(gen);
                    unsigned              rb_start = std::min(start_symbol_index_dist(gen), rb_size);
                    fapi::dmrs_ports_mask dmrs_ports(11);
                    dmrs_ports.from_uint64(0);
                    unsigned nof_csi_pdus_for_rm = 0U;

                    fapi::dl_pdsch_pdu         fapi_pdu;
                    fapi::dl_pdsch_pdu_builder builder(fapi_pdu);

                    builder.set_ue_specific_parameters(rnti);

                    // Always work with the biggest numerology.
                    builder.set_bwp_parameters(crb_interval::start_and_len(bwp_start, bwp_size), scs, cyclic_p)
                        .set_codeword_generation_parameters(nid_pdsch, 0, ref_point)
                        .set_dmrs_parameters(
                            dl_dmrs_symbol, config_type, scrambling_id, n_scid, dmrs_cdm_grps_no_data, dmrs_ports)
                        .set_codeword_parameters(tb_size_lbrm_bytes)
                        .set_number_of_csi_puds(nof_csi_pdus_for_rm);

                    auto builder_cw = builder.add_codeword();

                    modulation_scheme qam_mod = modulation_scheme::QAM64;
                    sch_mcs_index     mcs(20);
                    pdsch_mcs_table   mcs_table = pdsch_mcs_table::qam64;
                    unsigned          rv_index  = 0;
                    units::bytes      tb_size{42};

                    builder_cw.set_codeword_parameters(qam_mod, mcs, mcs_table, rv_index, tb_size);

                    builder.set_frequency_allocation_type_1(vrb_interval::start_and_len(rb_start, rb_size),
                                                            vrb_prb_mapping);

                    builder.set_time_allocation_parameters(
                        ofdm_symbol_range::start_and_len(start_symbol_index, nr_of_symbols));
                    builder.set_profile_nr_tx_power_info_parameters(power_profile_nr, power_ss_profile_nr);

                    builder.set_ldpc_base_graph(ldpc_graph);
                    builder.get_tx_precoding_and_beamforming_pdu_builder().set_pmi(0).set_prg_parameters(51);

                    set_vrb_to_prb_mapping_into_fapi_pdu(
                        builder, vrb_prb_mapping, dci_format, ss, coreset_start, ref_point, initial_bwp_size);

                    pdsch_processor::pdu_t proc_pdu;
                    convert_pdsch_fapi_to_phy(proc_pdu, fapi_pdu, slot, {}, pm_repo);

                    // Test basic parameters.
                    ASSERT_EQ(slot, proc_pdu.slot);
                    ASSERT_EQ(static_cast<unsigned>(cyclic_p), static_cast<unsigned>(proc_pdu.cp.value));

                    ASSERT_EQ(to_value(rnti), proc_pdu.rnti);
                    ASSERT_EQ(bwp_size, proc_pdu.bwp_size_rb);
                    ASSERT_EQ(bwp_start, proc_pdu.bwp_start_rb);

                    // Codeword.
                    ASSERT_EQ(static_cast<modulation_scheme>(qam_mod), proc_pdu.codewords[0].modulation);
                    ASSERT_EQ(rv_index, proc_pdu.codewords[0].rv);

                    ASSERT_EQ(nid_pdsch, proc_pdu.n_id);
                    ASSERT_EQ(dl_dmrs_symbol, proc_pdu.dmrs_symbol_mask);

                    ASSERT_EQ(static_cast<unsigned>(ref_point), static_cast<unsigned>(proc_pdu.ref_point));
                    ASSERT_TRUE(config_type == proc_pdu.dmrs);
                    ASSERT_EQ(scrambling_id, proc_pdu.scrambling_id);
                    ASSERT_EQ(n_scid, proc_pdu.n_scid);
                    ASSERT_EQ(dmrs_cdm_grps_no_data, proc_pdu.nof_cdm_groups_without_data);
                    ASSERT_EQ(start_symbol_index, proc_pdu.start_symbol_index);
                    ASSERT_EQ(nr_of_symbols, proc_pdu.nof_symbols);

                    // Powers.
                    float ratio_pdsch_data_to_sss_dB =
                        calculate_ratio_pdsch_data_to_sss_dB(power_profile_nr, power_ss_profile_nr);
                    float ratio_pdsch_dmrs_to_sss_dB =
                        calculate_ratio_pdsch_dmrs_to_sss_dB(ratio_pdsch_data_to_sss_dB, dmrs_cdm_grps_no_data);

                    ASSERT_FLOAT_EQ(ratio_pdsch_dmrs_to_sss_dB, proc_pdu.ratio_pdsch_dmrs_to_sss_dB);
                    ASSERT_FLOAT_EQ(ratio_pdsch_data_to_sss_dB, proc_pdu.ratio_pdsch_data_to_sss_dB);

                    // Frequency domain allocation.
                    rb_allocation freq_allocation = make_freq_allocation(fapi_pdu.mapping,
                                                                         bwp_start,
                                                                         bwp_size,
                                                                         coreset_start,
                                                                         initial_bwp_size,
                                                                         vrb_prb_mapping,
                                                                         rb_start,
                                                                         rb_size);

                    ASSERT_TRUE(freq_allocation == proc_pdu.freq_alloc);

                    ASSERT_EQ(ldpc_graph, proc_pdu.ldpc_base_graph);
                    ASSERT_EQ(tb_size_lbrm_bytes, proc_pdu.tbs_lbrm);
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
