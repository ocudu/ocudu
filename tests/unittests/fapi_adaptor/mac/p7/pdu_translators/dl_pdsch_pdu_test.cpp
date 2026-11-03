// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "pdsch.h"
#include "ocudu/fapi_adaptor/precoding_codebook_generator.h"
#include "ocudu/ran/precoding/precoding_codebooks.h"
#include "ocudu/ran/sch/sch_constants.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace fapi_adaptor;
using namespace unittests;

/// Number of PRBs of the cell, which is also the size of the precoding resource block groups.
static constexpr unsigned nof_prbs = 51;

using precoding_tools =
    std::pair<std::unique_ptr<precoding_codebook_mapper>, std::unique_ptr<precoding_codebook_repository>>;

/// Builds the precoding codebooks of a single port, used by SIB1 and paging.
static precoding_tools make_one_port_codebooks()
{
  return generate_precoding_codebooks(pmi_codebook_one_port{}, antenna_topology::one_port, 0);
}

/// Builds the precoding codebooks of two ports, used by RAR.
static precoding_tools make_two_port_codebooks()
{
  return generate_precoding_codebooks(pmi_codebook_two_port{}, antenna_topology::two_port, 0);
}

/// Builds the precoding codebooks of four ports with a Type I codebook, used by the DL messages.
static precoding_tools make_four_port_codebooks()
{
  return generate_precoding_codebooks(
      pmi_codebook_typeI_single_panel{.n1_n2 = pmi_codebook_single_panel_config::two_one,
                                      .mode  = pmi_codebook_typeI_mode::one},
      antenna_topology::four_ports,
      0);
}

/// Converts the given MAC PDU into a FAPI PDSCH PDU.
template <typename MacPdu>
static fapi::dl_pdsch_pdu
convert_pdsch(const MacPdu& mac_pdu, const precoding_codebook_mapper& pm_mapper, unsigned nof_csi_pdus = 0)
{
  fapi::dl_pdsch_pdu         fapi_pdu;
  fapi::dl_pdsch_pdu_builder builder(fapi_pdu);
  convert_pdsch_mac_to_fapi(builder, mac_pdu, nof_csi_pdus, pm_mapper, nof_prbs);
  return fapi_pdu;
}

/// Checks the fields of the PDU that come from the PDSCH configuration, for a PDSCH in CORESET#0.
static void validate_pdsch_information(const pdsch_information& pdsch_cfg, const fapi::dl_pdsch_pdu& fapi_pdu)
{
  // BWP.
  const bwp_configuration& bwp_cfg = *pdsch_cfg.bwp_cfg;
  EXPECT_EQ(pdsch_cfg.coreset_cfg->coreset0_crbs(), fapi_pdu.bwp);
  EXPECT_EQ(bwp_cfg.scs, fapi_pdu.scs);
  EXPECT_EQ(bwp_cfg.cp, fapi_pdu.cp);

  // Basic parameters.
  EXPECT_EQ(pdsch_cfg.rnti, fapi_pdu.rnti);
  EXPECT_EQ(pdsch_cfg.symbols, fapi_pdu.symbols);
  EXPECT_EQ(pdsch_cfg.n_id, fapi_pdu.nid_pdsch);
  EXPECT_EQ(pdsch_cfg.nof_layers, fapi_pdu.num_layers);

  // DMRS.
  const dmrs_information& dmrs_cfg = pdsch_cfg.dmrs;
  EXPECT_EQ(dmrs_cfg.dmrs_symb_pos, fapi_pdu.dl_dmrs_symb_pos);
  EXPECT_EQ(dmrs_cfg.config_type, fapi_pdu.dmrs_type);
  EXPECT_EQ(dmrs_cfg.dmrs_scrambling_id, fapi_pdu.pdsch_dmrs_scrambling_id);
  EXPECT_EQ(dmrs_cfg.n_scid, fapi_pdu.nscid);
  EXPECT_EQ(dmrs_cfg.num_dmrs_cdm_grps_no_data, fapi_pdu.num_dmrs_cdm_grps_no_data);
  EXPECT_EQ(dmrs_cfg.dmrs_ports, fapi_pdu.dmrs_ports);

  // Frequency allocation.
  const vrb_alloc& rbs = pdsch_cfg.rbs;
  EXPECT_EQ(vrb_to_prb::mapping_type::non_interleaved, fapi_pdu.vrb_to_prb_mapping);
  if (rbs.is_type0()) {
    const auto* alloc = std::get_if<fapi::resource_allocation_type_0>(&fapi_pdu.resource_alloc);
    ASSERT_NE(alloc, nullptr);
    EXPECT_EQ(rbs.type0(), alloc->vrbs);
  } else {
    const auto* alloc = std::get_if<fapi::resource_allocation_type_1>(&fapi_pdu.resource_alloc);
    ASSERT_NE(alloc, nullptr);
    EXPECT_EQ(rbs.type1(), alloc->vrbs);
  }

  // Power.
  const auto* power = std::get_if<fapi::dl_pdsch_pdu::power_profile_nr>(&fapi_pdu.power_config);
  ASSERT_NE(power, nullptr);
  EXPECT_EQ(pdsch_cfg.tx_pwr_info.pwr_ctrl_offset, power->pwr_control_offset_db);
  EXPECT_EQ(fapi::to_power_control_offset_ss(pdsch_cfg.tx_pwr_info.pwr_ctrl_offset_ss), power->pwr_control_offset_ss);

  // Codewords.
  ASSERT_EQ(1U, fapi_pdu.cws.size());
  const auto& fapi_cw = fapi_pdu.cws.front();
  const auto& mac_cw  = pdsch_cfg.codewords.front();
  EXPECT_EQ(pdsch_cfg.mcs_table, fapi_cw.mcs_table);
  EXPECT_EQ(mac_cw.rv_index, fapi_cw.rv_index);
  EXPECT_EQ(mac_cw.mcs_index, fapi_cw.mcs_index);
  EXPECT_EQ(mac_cw.mcs_descr.modulation, fapi_cw.qam_mod_order);
  EXPECT_EQ(mac_cw.tb_size_bytes, fapi_cw.tb_size);
  EXPECT_EQ(pdsch_cfg.tbs_lbrm, fapi_pdu.tb_size_lbrm);
  EXPECT_EQ(get_ldpc_base_graph(mac_cw.mcs_descr.get_normalised_target_code_rate(), mac_cw.tb_size_bytes.to_bits()),
            fapi_pdu.ldpc_base_graph);
}

/// Returns the PMI report carried by the PDSCH configuration, if any.
static std::optional<precoding_matrix_indicator> get_pmi_report(const pdsch_information& pdsch_cfg)
{
  const auto& pmi = std::get<precoding_matrix_indicator>(pdsch_cfg.precoding_and_beamforming);
  if (std::holds_alternative<std::monostate>(pmi)) {
    return std::nullopt;
  }
  return pmi;
}

/// \brief Checks that the PDU carries the precoding matrix the mapper selects for the PMI report and the number of
/// layers of the PDSCH configuration, and that this matrix has that number of layers.
///
/// Without a PMI report, the PDU must carry the omnidirectional precoding matrix.
static void validate_precoding_matrix(const fapi::dl_pdsch_pdu& fapi_pdu,
                                      const precoding_tools&    pm_tools,
                                      const pdsch_information&  pdsch_cfg)
{
  const auto& precoding = fapi_pdu.precoding_and_beamforming;
  EXPECT_EQ(nof_prbs, precoding.prg_size);

  const auto* pmi = std::get_if<fapi::precoding_matrix_index>(&precoding.prg.precoding);
  ASSERT_NE(pmi, nullptr);

  mac_pdsch_precoding_info info{.report = get_pmi_report(pdsch_cfg)};
  EXPECT_EQ(pm_tools.first->map(info, pdsch_cfg.nof_layers), *pmi);
  EXPECT_EQ(pdsch_cfg.nof_layers, pm_tools.second->get_precoding_config(*pmi).mimo.get_nof_layers());
}

TEST(mac_fapi_pdsch_pdu_conversor_test, valid_sib1_pdu_should_pass)
{
  static constexpr unsigned nof_csi_pdus = 2;

  sib_information_test_helper pdu_test = build_valid_sib1_information_pdu();
  precoding_tools             pm_tools = make_one_port_codebooks();

  fapi::dl_pdsch_pdu fapi_pdu = convert_pdsch(pdu_test.pdu, *pm_tools.first, nof_csi_pdus);

  validate_pdsch_information(pdu_test.pdu.pdsch_cfg, fapi_pdu);
  EXPECT_EQ(nof_csi_pdus, fapi_pdu.nof_csi_pdus_for_rm);
  validate_precoding_matrix(fapi_pdu, pm_tools, pdu_test.pdu.pdsch_cfg);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, valid_rar_pdu_should_pass)
{
  static constexpr unsigned nof_csi_pdus = 2;

  rar_information_test_helper pdu_test = build_valid_rar_information_pdu();
  precoding_tools             pm_tools = make_two_port_codebooks();

  fapi::dl_pdsch_pdu fapi_pdu = convert_pdsch(pdu_test.pdu, *pm_tools.first, nof_csi_pdus);

  validate_pdsch_information(pdu_test.pdu.pdsch_cfg, fapi_pdu);
  EXPECT_EQ(nof_csi_pdus, fapi_pdu.nof_csi_pdus_for_rm);
  validate_precoding_matrix(fapi_pdu, pm_tools, pdu_test.pdu.pdsch_cfg);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, valid_dl_paging_pdu_should_pass)
{
  static constexpr unsigned nof_csi_pdus = 2;

  dl_paging_allocation_test_helper pdu_test = build_valid_dl_paging_pdu();
  precoding_tools                  pm_tools = make_one_port_codebooks();

  fapi::dl_pdsch_pdu fapi_pdu = convert_pdsch(pdu_test.pdu, *pm_tools.first, nof_csi_pdus);

  validate_pdsch_information(pdu_test.pdu.pdsch_cfg, fapi_pdu);
  EXPECT_EQ(nof_csi_pdus, fapi_pdu.nof_csi_pdus_for_rm);
  validate_precoding_matrix(fapi_pdu, pm_tools, pdu_test.pdu.pdsch_cfg);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, valid_dl_msg_alloc_pdu_should_pass)
{
  static constexpr unsigned nof_csi_pdus = 2;

  dl_msg_allocation_test_helper pdu_test = build_valid_dl_msg_alloc_pdu();
  const dl_msg_alloc&           pdu      = pdu_test.pdu;
  precoding_tools               pm_tools = make_four_port_codebooks();

  fapi::dl_pdsch_pdu fapi_pdu = convert_pdsch(pdu, *pm_tools.first, nof_csi_pdus);

  validate_pdsch_information(pdu.pdsch_cfg, fapi_pdu);
  EXPECT_EQ(nof_csi_pdus, fapi_pdu.nof_csi_pdus_for_rm);
  validate_precoding_matrix(fapi_pdu, pm_tools, pdu.pdsch_cfg);

  ASSERT_TRUE(fapi_pdu.context.has_value());
  EXPECT_EQ(pdu.pdsch_cfg.harq_id, fapi_pdu.context->get_h_id());
  EXPECT_EQ(pdu.context.nof_retxs != 0, !fapi_pdu.context->is_new_data());
}

TEST(mac_fapi_pdsch_pdu_conversor_test, dl_msg_alloc_pdu_with_type0_allocation_carries_its_vrb_bitmap)
{
  dl_msg_allocation_test_helper pdu_test = build_valid_dl_msg_alloc_pdu();
  pdsch_information&            pdsch    = pdu_test.pdu.pdsch_cfg;
  pdu_test.bwp_cfg->crbs                 = {0, 52};
  vrb_bitmap vrbs(pdu_test.bwp_cfg->crbs.length());
  vrbs.fill(0, 4);
  vrbs.fill(12, 20);
  pdsch.rbs             = vrbs;
  pdsch.vrb_prb_mapping = vrb_to_prb::mapping_type::non_interleaved;

  fapi::dl_pdsch_pdu fapi_pdu = convert_pdsch(pdu_test.pdu, *make_four_port_codebooks().first);

  EXPECT_TRUE(std::holds_alternative<fapi::resource_allocation_type_0>(fapi_pdu.resource_alloc));
  validate_pdsch_information(pdsch, fapi_pdu);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, beamformed_rar_carries_its_beam)
{
  beam_identifier beam_id = to_beam_id(3);

  rar_information_test_helper pdu_test             = build_valid_rar_information_pdu();
  pdu_test.pdu.pdsch_cfg.nof_layers                = 1;
  pdu_test.pdu.pdsch_cfg.precoding_and_beamforming = make_single_beam_precoding(beam_id);

  fapi::dl_pdsch_pdu fapi_pdu = convert_pdsch(pdu_test.pdu, *make_two_port_codebooks().first);

  EXPECT_EQ(nof_prbs, fapi_pdu.precoding_and_beamforming.prg_size);
  EXPECT_EQ(precoding_beam_list({beam_id}), fapi_pdu.precoding_and_beamforming.prg.beams);
}

namespace {

/// Kind of VRB-to-PRB mapping that the translator must select.
enum class vrb_mapping_kind {
  non_interleaved_common_ss,
  non_interleaved_other,
  interleaved_common_type0_coreset0,
  interleaved_common_any_coreset0_present,
  interleaved_other
};

/// Parameters of a PDSCH transmission that determine its VRB-to-PRB mapping, and the mapping expected for them.
struct vrb_mapping_param {
  vrb_to_prb::mapping_type mapping;
  dci_dl_format            dci_format;
  search_space_set_type    search_space;
  bool                     in_coreset0;
  vrb_mapping_kind         expected;
};

/// Formatter for test params (the default raw byte dump would read the uninitialized padding bytes).
void PrintTo(const vrb_mapping_param& param, std::ostream* os)
{
  *os << "{mapping=" << static_cast<unsigned>(param.mapping)
      << ", dci_format=" << static_cast<unsigned>(param.dci_format)
      << ", search_space=" << static_cast<unsigned>(param.search_space) << ", in_coreset0=" << param.in_coreset0
      << ", expected=" << static_cast<unsigned>(param.expected) << "}";
}

class mac_fapi_pdsch_vrb_mapping_fixture : public ::testing::TestWithParam<vrb_mapping_param>
{};

} // namespace

/// Returns the kind of VRB-to-PRB mapping of the given FAPI PDSCH PDU.
static vrb_mapping_kind get_vrb_mapping_kind(const fapi::dl_pdsch_pdu& fapi_pdu)
{
  if (std::holds_alternative<fapi::dl_pdsch_pdu::non_interleaved_common_ss>(fapi_pdu.mapping)) {
    return vrb_mapping_kind::non_interleaved_common_ss;
  }
  if (std::holds_alternative<fapi::dl_pdsch_pdu::non_interleaved_other>(fapi_pdu.mapping)) {
    return vrb_mapping_kind::non_interleaved_other;
  }
  if (std::holds_alternative<fapi::dl_pdsch_pdu::interleaved_common_type0_coreset0>(fapi_pdu.mapping)) {
    return vrb_mapping_kind::interleaved_common_type0_coreset0;
  }
  if (std::holds_alternative<fapi::dl_pdsch_pdu::interleaved_common_any_coreset0_present>(fapi_pdu.mapping)) {
    return vrb_mapping_kind::interleaved_common_any_coreset0_present;
  }
  return vrb_mapping_kind::interleaved_other;
}

TEST_P(mac_fapi_pdsch_vrb_mapping_fixture, vrb_to_prb_mapping_is_selected_from_the_dci_and_the_coreset)
{
  dl_msg_allocation_test_helper pdu_test = build_valid_dl_msg_alloc_pdu();
  if (!GetParam().in_coreset0) {
    *pdu_test.coreset_cfg = build_valid_non_coreset0_configuration(true);
  }
  pdsch_information& pdsch = pdu_test.pdu.pdsch_cfg;
  pdsch.vrb_prb_mapping    = GetParam().mapping;
  pdsch.dci_fmt            = GetParam().dci_format;
  pdsch.ss_set_type        = GetParam().search_space;

  fapi::dl_pdsch_pdu fapi_pdu = convert_pdsch(pdu_test.pdu, *make_four_port_codebooks().first);

  EXPECT_EQ(GetParam().mapping, fapi_pdu.vrb_to_prb_mapping);
  ASSERT_EQ(GetParam().expected, get_vrb_mapping_kind(fapi_pdu));
  // The BWP of a PDSCH in CORESET 0 starts at the CORESET, so the CORESET starts at the first RB of the BWP.
  if (const auto* type0 = std::get_if<fapi::dl_pdsch_pdu::interleaved_common_type0_coreset0>(&fapi_pdu.mapping)) {
    EXPECT_EQ(0U, type0->N_start_coreset);
    EXPECT_EQ(pdu_test.coreset_cfg->coreset0_crbs().length(), type0->N_bwp_init_size);
  }
  if (const auto* any = std::get_if<fapi::dl_pdsch_pdu::interleaved_common_any_coreset0_present>(&fapi_pdu.mapping)) {
    EXPECT_EQ(0U, any->N_start_coreset);
    EXPECT_EQ(pdu_test.coreset_cfg->coreset0_crbs().length(), any->N_bwp_init_size);
  }
}

INSTANTIATE_TEST_SUITE_P(mappings,
                         mac_fapi_pdsch_vrb_mapping_fixture,
                         ::testing::Values(
                             // Non-interleaved: common SS with DCI 1_0 is special, everything else is the same.
                             vrb_mapping_param{.mapping      = vrb_to_prb::mapping_type::non_interleaved,
                                               .dci_format   = dci_dl_format::f1_0,
                                               .search_space = search_space_set_type::type0,
                                               .in_coreset0  = true,
                                               .expected     = vrb_mapping_kind::non_interleaved_common_ss},
                             vrb_mapping_param{.mapping      = vrb_to_prb::mapping_type::non_interleaved,
                                               .dci_format   = dci_dl_format::f1_1,
                                               .search_space = search_space_set_type::type0,
                                               .in_coreset0  = true,
                                               .expected     = vrb_mapping_kind::non_interleaved_other},
                             vrb_mapping_param{.mapping      = vrb_to_prb::mapping_type::non_interleaved,
                                               .dci_format   = dci_dl_format::f1_0,
                                               .search_space = search_space_set_type::ue_specific,
                                               .in_coreset0  = true,
                                               .expected     = vrb_mapping_kind::non_interleaved_other},
                             // Interleaved in CORESET 0 with DCI 1_0 in a common SS depends on the SS type.
                             vrb_mapping_param{.mapping      = vrb_to_prb::mapping_type::interleaved_n2,
                                               .dci_format   = dci_dl_format::f1_0,
                                               .search_space = search_space_set_type::type0,
                                               .in_coreset0  = true,
                                               .expected     = vrb_mapping_kind::interleaved_common_type0_coreset0},
                             vrb_mapping_param{.mapping      = vrb_to_prb::mapping_type::interleaved_n2,
                                               .dci_format   = dci_dl_format::f1_0,
                                               .search_space = search_space_set_type::type1,
                                               .in_coreset0  = true,
                                               .expected = vrb_mapping_kind::interleaved_common_any_coreset0_present},
                             // Any other interleaved case.
                             vrb_mapping_param{.mapping      = vrb_to_prb::mapping_type::interleaved_n2,
                                               .dci_format   = dci_dl_format::f1_0,
                                               .search_space = search_space_set_type::type0,
                                               .in_coreset0  = false,
                                               .expected     = vrb_mapping_kind::interleaved_other},
                             vrb_mapping_param{.mapping      = vrb_to_prb::mapping_type::interleaved_n2,
                                               .dci_format   = dci_dl_format::f1_0,
                                               .search_space = search_space_set_type::ue_specific,
                                               .in_coreset0  = true,
                                               .expected     = vrb_mapping_kind::interleaved_other},
                             vrb_mapping_param{.mapping      = vrb_to_prb::mapping_type::interleaved_n2,
                                               .dci_format   = dci_dl_format::f1_1,
                                               .search_space = search_space_set_type::type0,
                                               .in_coreset0  = true,
                                               .expected     = vrb_mapping_kind::interleaved_other}),
                         [](const ::testing::TestParamInfo<vrb_mapping_param>& param_info) {
                           return fmt::format("mapping_{}_dci_{}_ss_{}_coreset0_{}",
                                              static_cast<unsigned>(param_info.param.mapping),
                                              static_cast<unsigned>(param_info.param.dci_format),
                                              static_cast<unsigned>(param_info.param.search_space),
                                              param_info.param.in_coreset0);
                         });

TEST(mac_fapi_pdsch_pdu_conversor_test, pdsch_in_a_non_coreset0_uses_the_bwp_as_its_bandwidth)
{
  dl_msg_allocation_test_helper pdu_test = build_valid_dl_msg_alloc_pdu();
  *pdu_test.coreset_cfg                  = build_valid_non_coreset0_configuration(false);

  fapi::dl_pdsch_pdu fapi_pdu = convert_pdsch(pdu_test.pdu, *make_four_port_codebooks().first);

  EXPECT_EQ(pdu_test.bwp_cfg->crbs, fapi_pdu.bwp);
  EXPECT_EQ(pdu_test.bwp_cfg->scs, fapi_pdu.scs);
  EXPECT_EQ(pdu_test.bwp_cfg->cp, fapi_pdu.cp);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, sib1_uses_subcarrier_0_as_reference_point_and_other_si_uses_point_a)
{
  precoding_tools             pm_tools = make_one_port_codebooks();
  sib_information_test_helper pdu_test = build_valid_sib1_information_pdu();

  pdu_test.pdu.si_indicator = sib_information::sib1;
  EXPECT_EQ(fapi::pdsch_ref_point_type::subcarrier_0, convert_pdsch(pdu_test.pdu, *pm_tools.first).ref_point);

  pdu_test.pdu.si_indicator = sib_information::other_si;
  EXPECT_EQ(fapi::pdsch_ref_point_type::point_a, convert_pdsch(pdu_test.pdu, *pm_tools.first).ref_point);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, rar_paging_and_dl_message_use_point_a_as_reference_point)
{
  rar_information_test_helper      rar_test    = build_valid_rar_information_pdu();
  dl_paging_allocation_test_helper paging_test = build_valid_dl_paging_pdu();
  dl_msg_allocation_test_helper    dl_msg_test = build_valid_dl_msg_alloc_pdu();

  EXPECT_EQ(fapi::pdsch_ref_point_type::point_a,
            convert_pdsch(rar_test.pdu, *make_two_port_codebooks().first).ref_point);
  EXPECT_EQ(fapi::pdsch_ref_point_type::point_a,
            convert_pdsch(paging_test.pdu, *make_one_port_codebooks().first).ref_point);
  EXPECT_EQ(fapi::pdsch_ref_point_type::point_a,
            convert_pdsch(dl_msg_test.pdu, *make_four_port_codebooks().first).ref_point);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, dl_message_without_pmi_selects_the_omnidirectional_precoding_matrix)
{
  dl_msg_allocation_test_helper pdu_test = build_valid_dl_msg_alloc_pdu();
  pdsch_information&            pdsch    = pdu_test.pdu.pdsch_cfg;
  pdsch.nof_layers                       = 1;
  pdsch.precoding_and_beamforming        = precoding_and_beamforming_info{precoding_matrix_indicator{std::monostate{}}};

  precoding_tools pm_tools = make_four_port_codebooks();

  fapi::dl_pdsch_pdu fapi_pdu = convert_pdsch(pdu_test.pdu, *pm_tools.first);

  validate_precoding_matrix(fapi_pdu, pm_tools, pdsch);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, dl_message_with_a_type_ii_pmi_carries_the_precoding_weights)
{
  pmi_typeII pmi{.config = pmi_codebook_typeII{.n1_n2               = pmi_codebook_single_panel_config::two_one,
                                               .nof_beams           = bounded_integer<unsigned, 2, 4>(2),
                                               .phase_alphabet_size = pmi_codebook_typeII_phase_size::qpsk,
                                               .subband_amplitude   = false},
                 .i_1_1  = 0,
                 .i_1_2  = 0,
                 .layers = {pmi_typeII::layer_coefficients{
                     .i_1_3 = 0, .i_1_4 = {7, 3, 2, 1}, .i_2_1 = {0, 1, 2, 3}, .i_2_2 = {}}}};

  dl_msg_allocation_test_helper pdu_test           = build_valid_dl_msg_alloc_pdu();
  pdu_test.pdu.pdsch_cfg.nof_layers                = 1;
  pdu_test.pdu.pdsch_cfg.precoding_and_beamforming = precoding_and_beamforming_info{precoding_matrix_indicator{pmi}};

  fapi::dl_pdsch_pdu fapi_pdu = convert_pdsch(pdu_test.pdu, *make_four_port_codebooks().first);

  EXPECT_EQ(nof_prbs, fapi_pdu.precoding_and_beamforming.prg_size);
  const auto* weights = std::get_if<fapi::prg_precoding_weights>(&fapi_pdu.precoding_and_beamforming.prg.precoding);
  ASSERT_NE(weights, nullptr) << "The Type II precoding does not fit the precoding matrix table, so weights are sent";
  precoding_weight_matrix expected = make_precoding(precoding_matrix_indicator{pmi}, 1);
  EXPECT_EQ(expected.get_nof_layers(), weights->get_nof_layers());
  EXPECT_EQ(expected.get_nof_ports(), weights->get_nof_ports());
}

TEST(mac_fapi_pdsch_pdu_conversor_test, dl_message_context_carries_the_harq_process_k1_and_retransmissions)
{
  static constexpr unsigned k1        = 5;
  static constexpr unsigned nof_retxs = 2;

  dl_msg_allocation_test_helper pdu_test = build_valid_dl_msg_alloc_pdu();
  pdu_test.pdu.context.k1                = k1;
  pdu_test.pdu.context.nof_retxs         = nof_retxs;

  fapi::dl_pdsch_pdu fapi_pdu = convert_pdsch(pdu_test.pdu, *make_four_port_codebooks().first);

  EXPECT_EQ(pdsch_context(pdu_test.pdu.pdsch_cfg.harq_id, k1, nof_retxs), fapi_pdu.context);
}
