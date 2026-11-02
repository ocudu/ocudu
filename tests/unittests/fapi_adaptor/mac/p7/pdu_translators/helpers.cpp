// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "ocudu/ran/pucch/pucch_constants.h"

using namespace ocudu;
using namespace unittests;

/// Physical cell identifier used by the PDUs.
static constexpr pci_t test_pci = 500;

dl_ssb_pdu unittests::build_valid_dl_ssb_pdu()
{
  // FR2 SS/PBCH block with 240 kHz SCS, which uses the case E pattern of up to 64 blocks.
  return dl_ssb_pdu{.pci                       = test_pci,
                    .pss_to_sss_epre           = ssb_pss_to_sss_epre::dB_3,
                    .ssb_index                 = 37,
                    .subcarrier_offset         = 7,
                    .offset_to_pointA          = 1200,
                    .ssb_case                  = ssb_pattern_case::E,
                    .L_max                     = 64,
                    .scs                       = subcarrier_spacing::kHz240,
                    .precoding_and_beamforming = make_single_beam_precoding(to_beam_id(3)),
                    .mib_data                  = ssb_mib_data_pdu{.dmrs_typeA_pos         = dmrs_typeA_position::pos3,
                                                                  .pdcch_config_sib1      = 0x5a,
                                                                  .cell_barred            = false,
                                                                  .intra_freq_reselection = true}};
}

static dci_payload generate_dci_payload()
{
  dci_payload payload;
  // Mixed pattern of ones and zeros.
  for (unsigned i = 0, e = payload.max_size(); i != e; ++i) {
    payload.push_back(i % 3 == 0);
  }

  return payload;
}

static bwp_configuration generate_bwp_configuration()
{
  return bwp_configuration{
      .cp = cyclic_prefix::NORMAL, .scs = subcarrier_spacing::kHz240, .crbs = crb_interval{10, 34}};
}

coreset_configuration unittests::build_valid_coreset0_configuration()
{
  return coreset_configuration{nr_band::n3,
                               subcarrier_spacing::kHz15,
                               subcarrier_spacing::kHz15,
                               coreset0_index{0},
                               ssb_subcarrier_offset{0},
                               ssb_offset_to_pointA{48},
                               pci_t{10},
                               std::nullopt};
}

coreset_configuration unittests::build_valid_non_coreset0_configuration(bool interleaved)
{
  static constexpr unsigned nof_freq_resources = 8;
  static constexpr unsigned duration           = 2;

  freq_resource_bitmap freq_resources(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
  freq_resources.fill(0, nof_freq_resources);

  std::optional<coreset_configuration::interleaved_mapping_type> interleaved_mapping;
  if (interleaved) {
    interleaved_mapping =
        coreset_configuration::interleaved_mapping_type{.reg_bundle_sz = 6, .interleaver_sz = 2, .shift_index = 10};
  }

  static constexpr unsigned pdcch_dmrs_scrambling_id = 10;

  return coreset_configuration{to_coreset_id(1),
                               freq_resources,
                               duration,
                               interleaved_mapping,
                               coreset_configuration::precoder_granularity_type::same_as_reg_bundle,
                               pdcch_dmrs_scrambling_id};
}

/// Returns DL DCI information that alternates between the RA-RNTI and SI-RNTI formats with the given index.
static dci_dl_info generate_dci_dl_info(unsigned index)
{
  dci_dl_info info;
  if (index % 2 == 0) {
    info.set_ra_rnti_f1_0() = dci_1_0_ra_rnti_configuration{.N_rb_dl_bwp                 = 1,
                                                            .frequency_resource          = 2,
                                                            .time_resource               = 3,
                                                            .interleaved_vrb_prb_mapping = false,
                                                            .modulation_coding_scheme    = 1,
                                                            .tb_scaling                  = 2,
                                                            .lsb_sfn                     = 0};
  } else {
    info.set_si_rnti_f1_0() = dci_1_0_si_rnti_configuration{.N_rb_dl_bwp                  = 1,
                                                            .frequency_resource           = 2,
                                                            .time_resource                = 3,
                                                            .interleaved_vrb_prb_mapping  = false,
                                                            .modulation_coding_scheme     = 0,
                                                            .redundancy_version           = 2,
                                                            .system_information_indicator = 1};
  }
  return info;
}

/// Returns the DCI context of the PDCCH PDU with the given index, which has its own RNTI.
static dci_context_information
generate_dci_context(const bwp_configuration& bwp, const coreset_configuration& coreset, unsigned index)
{
  return dci_context_information{.bwp_cfg           = &bwp,
                                 .coreset_cfg       = &coreset,
                                 .rnti              = to_rnti(0x4601 + index),
                                 .n_id_pdcch_data   = 2,
                                 .n_rnti_pdcch_data = 4,
                                 .cces              = cce_position{.ncce = 2, .aggr_lvl = aggregation_level::n4},
                                 .starting_symbol   = 2,
                                 // NOTE: add the precoding information when the struct is defined, now is empty.
                                 .precoding_and_beamforming = {},
                                 .tx_pwr                    = {},
                                 .n_id_pdcch_dmrs           = 8,
                                 // NOTE: decision context is not filled as all the members are private in FAPI.
                                 .context = {}};
}

/// Returns the element of the list at the given index, or its last element when the index is out of range.
template <typename T>
static const T& get_clamped(span<const T> list, unsigned index)
{
  return list[std::min<size_t>(index, list.size() - 1)];
}

/// Adds the given number of DL PDCCH PDUs and their DCI payloads, using the given BWPs and the CORESETs of the helper.
static void
add_dl_pdcch_pdus(mac_dl_sched_result_test_helper& helper, span<const bwp_configuration> bwps, unsigned nof_pdus)
{
  span<const coreset_configuration> coresets = *helper.coreset_cfg;
  for (unsigned i = 0; i != nof_pdus; ++i) {
    helper.builder.dl_pdcchs().emplace_back(
        pdcch_dl_information{.ctx = generate_dci_context(get_clamped(bwps, i), get_clamped(coresets, i), i),
                             .dci = generate_dci_dl_info(i)});
    helper.result.dl_pdcch_pdus.push_back(generate_dci_payload());
  }
}

/// Adds the given number of UL PDCCH PDUs and their DCI payloads, using the given BWPs and the CORESETs of the helper.
static void
add_ul_pdcch_pdus(mac_dl_sched_result_test_helper& helper, span<const bwp_configuration> bwps, unsigned nof_pdus)
{
  span<const coreset_configuration> coresets = *helper.coreset_cfg;
  for (unsigned i = 0; i != nof_pdus; ++i) {
    helper.builder.ul_pdcchs().emplace_back(pdcch_ul_information{
        .ctx = generate_dci_context(get_clamped(bwps, i), get_clamped(coresets, i), i), .dci = {}});
    helper.result.ul_pdcch_pdus.push_back(generate_dci_payload());
  }
}

/// Returns a single layer DMRS configuration, transmitted in port 0 and in the given symbol.
static dmrs_information make_dmrs_information(unsigned dmrs_symbol)
{
  dmrs_symbol_mask symbols(MAX_NSYMB_PER_SLOT);
  symbols.set(dmrs_symbol);
  bounded_bitset<12> ports(12);
  ports.set(0);
  return dmrs_information{.dmrs_symb_pos                 = symbols,
                          .config_type                   = dmrs_config_type::type1,
                          .dmrs_scrambling_id            = 2,
                          .dmrs_scrambling_id_complement = 3,
                          .low_papr_dmrs                 = false,
                          .n_scid                        = false,
                          .num_dmrs_cdm_grps_no_data     = 2,
                          .dmrs_ports                    = ports};
}

/// Returns the PDSCH precoding for the given number of ports.
static precoding_and_beamforming_info make_pdsch_precoding(unsigned nof_ports)
{
  if (nof_ports == 2) {
    return precoding_and_beamforming_info{precoding_matrix_indicator{pmi_two_antenna_port{.pmi = 1}}};
  }
  if (nof_ports == 4) {
    return precoding_and_beamforming_info{precoding_matrix_indicator{pmi_typeI_single_panel{
        .panel_config = pmi_codebook_typeI_single_panel{.n1_n2 = pmi_codebook_single_panel_config::two_one,
                                                        .mode  = pmi_codebook_typeI_mode::one},
        .i_1_1        = 1,
        .i_1_2        = std::nullopt,
        .i_1_3        = std::nullopt,
        .i_2          = 1}}};
  }
  // A single port uses the default precoding.
  return make_default_precoding();
}

/// Creates the BWP and CORESET configurations of the given helper.
template <typename Pdu>
static void create_pdsch_configurations(pdsch_pdu_test_helper<Pdu>& helper)
{
  helper.bwp_cfg     = std::make_unique<bwp_configuration>(generate_bwp_configuration());
  helper.coreset_cfg = std::make_unique<coreset_configuration>(build_valid_coreset0_configuration());
}

/// Returns a valid PDSCH that uses the given BWP and CORESET, precoded for the given number of ports.
static pdsch_information
make_valid_pdsch_information(const bwp_configuration& bwp, const coreset_configuration& coreset, unsigned nof_ports)
{
  const ofdm_symbol_range symbols{3, 10};

  return pdsch_information{
      .rnti                      = to_rnti(0x4444),
      .bwp_cfg                   = &bwp,
      .coreset_cfg               = &coreset,
      .rbs                       = vrb_interval{4, 20},
      .symbols                   = symbols,
      .mcs_table                 = pdsch_mcs_table::qam64,
      .codewords                 = static_vector<pdsch_codeword, MAX_CODEWORDS_PER_PDSCH>{pdsch_codeword{
                          .mcs_descr     = sch_mcs_description{.modulation = modulation_scheme::QAM16, .target_code_rate = 220.F},
                          .mcs_index     = 5,
                          .rv_index      = 2,
                          .tb_size_bytes = units::bytes{128},
                          .new_data      = false}},
      .dmrs                      = make_dmrs_information(symbols.start()),
      .n_id                      = 507,
      .nof_layers                = 1U,
      .vrb_prb_mapping           = vrb_to_prb::mapping_type::non_interleaved,
      .ss_set_type               = search_space_set_type::type0,
      .dci_fmt                   = dci_dl_format::f1_0,
      .harq_id                   = to_harq_id(4),
      .precoding_and_beamforming = make_pdsch_precoding(nof_ports),
      .tx_pwr_info               = tx_power_pdsch_information{.pwr_ctrl_offset = 4, .pwr_ctrl_offset_ss = 3}};
}

/// Adds a single UL PDCCH PDU, in the first BWP and CORESET of the helper.
static void add_single_ul_pdcch_pdu(mac_dl_sched_result_test_helper& helper)
{
  helper.builder.ul_pdcchs().emplace_back(pdcch_ul_information{
      .ctx = dci_context_information{.bwp_cfg           = &helper.bwp_cfg->front(),
                                     .coreset_cfg       = &helper.coreset_cfg->front(),
                                     .rnti              = to_rnti(0x4444),
                                     .n_id_pdcch_data   = 1,
                                     .n_rnti_pdcch_data = 1,
                                     .cces              = cce_position{.ncce = 1, .aggr_lvl = aggregation_level::n1},
                                     .starting_symbol   = 0,
                                     .precoding_and_beamforming = {},
                                     .tx_pwr                    = {},
                                     .n_id_pdcch_dmrs           = 1,
                                     .context                   = {}},
      .dci = {}});
  helper.result.ul_pdcch_pdus.push_back(generate_dci_payload());
}

sib_information_test_helper unittests::build_valid_sib1_information_pdu()
{
  sib_information_test_helper helper;
  create_pdsch_configurations(helper);
  helper.pdu = sib_information{.si_indicator  = sib_information::si_indicator_type::sib1,
                               .si_msg_index  = std::nullopt,
                               .sibs          = {},
                               .version       = 0,
                               .nof_txs       = 0,
                               .is_repetition = false,
                               .pdsch_cfg     = make_valid_pdsch_information(*helper.bwp_cfg, *helper.coreset_cfg, 1)};

  return helper;
}

rar_information_test_helper unittests::build_valid_rar_information_pdu()
{
  rar_information_test_helper helper;
  create_pdsch_configurations(helper);
  helper.pdu = rar_information{
      .pdsch_cfg         = make_valid_pdsch_information(*helper.bwp_cfg, *helper.coreset_cfg, 2),
      .grants            = static_vector<rar_ul_grant, MAX_GRANTS_PER_RAR>{rar_ul_grant{.rapid         = 2,
                                                                                        .ta            = 0,
                                                                                        .temp_crnti    = to_rnti(0x0003),
                                                                                        .freq_hop_flag = false,
                                                                                        .time_resource_assignment = 0,
                                                                                        .freq_resource_assignment = 0,
                                                                                        .mcs                      = 25,
                                                                                        .tpc                      = 3,
                                                                                        .csi_req                  = false,
                                                                                        .type = rar_ul_grant::four_step_info{}}},
      .backoff_indicator = std::nullopt};

  return helper;
}

dl_paging_allocation_test_helper unittests::build_valid_dl_paging_pdu()
{
  dl_paging_allocation_test_helper helper;
  create_pdsch_configurations(helper);
  helper.pdu = dl_paging_allocation{.paging_ue_list = {},
                                    .pdsch_cfg = make_valid_pdsch_information(*helper.bwp_cfg, *helper.coreset_cfg, 1)};

  return helper;
}

dl_msg_allocation_test_helper unittests::build_valid_dl_msg_alloc_pdu()
{
  dl_msg_allocation_test_helper helper;
  create_pdsch_configurations(helper);
  helper.pdu = dl_msg_alloc{.pdsch_cfg = make_valid_pdsch_information(*helper.bwp_cfg, *helper.coreset_cfg, 4),
                            .tb_list   = {},
                            .context   = dl_msg_alloc::decision_context{.ue_index        = to_du_ue_index(5),
                                                                        .k1              = 3,
                                                                        .ss_id           = to_search_space_id(1),
                                                                        .nof_retxs       = 2,
                                                                        .nof_repetitions = 1,
                                                                        .nof_remaining_repetitions = 0,
                                                                        .buffer_occupancy          = 0,
                                                                        .olla_offset               = 4}};

  return helper;
}

/// Creates the configurations of the helper and initializes its result with no PDUs.
static void initialize_dl_sched_result(mac_dl_sched_result_test_helper& helper)
{
  helper.bwp_cfg       = std::make_unique<std::array<bwp_configuration, 2>>();
  helper.bwp_cfg_pdcch = std::make_unique<std::array<bwp_configuration, MAX_DL_PDCCH_PDUS_PER_SLOT>>();
  helper.coreset_cfg   = std::make_unique<std::array<coreset_configuration, 2>>();

  // Mix a CORESET#0 and a non-CORESET#0 so that both kinds are present in the result.
  (*helper.coreset_cfg)[0] = build_valid_coreset0_configuration();
  (*helper.coreset_cfg)[1] = build_valid_non_coreset0_configuration(true);
  for (auto& bwp : *helper.bwp_cfg) {
    bwp = generate_bwp_configuration();
  }
  for (unsigned i = 0, e = helper.bwp_cfg_pdcch->size(); i != e; ++i) {
    (*helper.bwp_cfg_pdcch)[i] = bwp_configuration{
        .cp = cyclic_prefix::NORMAL, .scs = subcarrier_spacing::kHz240, .crbs = crb_interval{i, 20 + i}};
  }

  helper.result.slot   = slot_point(4, 512, 37);
  helper.result.dl_res = helper.sched_result;
}

mac_dl_sched_result_test_helper unittests::build_valid_mac_dl_sched_result()
{
  static constexpr unsigned nof_dl_pdcch_pdus = 4;
  static constexpr unsigned nof_ssb_pdus      = 2;

  mac_dl_sched_result_test_helper helper;
  initialize_dl_sched_result(helper);

  add_dl_pdcch_pdus(helper, *helper.bwp_cfg, nof_dl_pdcch_pdus);
  add_single_ul_pdcch_pdu(helper);

  helper.sib1 = build_valid_sib1_information_pdu();
  helper.sched_result.bc.sibs.push_back(helper.sib1.pdu);

  for (unsigned i = 0; i != nof_ssb_pdus; ++i) {
    helper.result.ssb_pdus.push_back(build_valid_dl_ssb_pdu());
  }

  return helper;
}

static csi_rs_info build_valid_csi_pdu(mac_dl_sched_result_test_helper& helper)
{
  return csi_rs_info{.bwp_cfg                   = &helper.bwp_cfg->front(),
                     .crbs                      = crb_interval{40, 120},
                     .type                      = csi_rs_type::CSI_RS_ZP,
                     .row                       = 1,
                     .freq_domain               = bounded_bitset<12, false>(12),
                     .symbol0                   = 0,
                     .symbol1                   = 4,
                     .cdm_type                  = csi_rs_cdm_type::no_CDM,
                     .freq_density              = csi_rs_freq_density_type::one,
                     .scrambling_id             = 0,
                     .power_ctrl_offset         = 0,
                     .power_ctrl_offset_ss      = 0,
                     .precoding_and_beamforming = {}};
}

mac_dl_sched_result_test_helper unittests::build_valid_mac_dl_sched_result_with_all_supported_pdus()
{
  mac_dl_sched_result_test_helper helper;
  initialize_dl_sched_result(helper);

  add_dl_pdcch_pdus(helper, *helper.bwp_cfg_pdcch, MAX_DL_PDCCH_PDUS_PER_SLOT);
  add_ul_pdcch_pdus(helper, *helper.bwp_cfg_pdcch, MAX_UL_PDCCH_PDUS_PER_SLOT);

  // Add SIB1 PDU.
  helper.sib1 = build_valid_sib1_information_pdu();
  for (unsigned i = 0; i != MAX_SI_PDUS_PER_SLOT; ++i) {
    helper.sched_result.bc.sibs.push_back(helper.sib1.pdu);
  }

  // Add RAR PDU.
  helper.rar = build_valid_rar_information_pdu();
  for (unsigned i = 0; i != MAX_RAR_PDUS_PER_SLOT; ++i) {
    helper.builder.rar_grants().emplace_back(helper.rar.pdu);
  }

  // Add paging PDU.
  helper.dl_paging = build_valid_dl_paging_pdu();
  for (unsigned i = 0; i != MAX_PAGING_PDUS_PER_SLOT; ++i) {
    helper.sched_result.paging_grants.push_back(helper.dl_paging.pdu);
  }

  // Add downlink UE PDU.
  helper.dl_msg_alloc = build_valid_dl_msg_alloc_pdu();
  for (unsigned i = 0; i != MAX_UE_PDUS_PER_SLOT; ++i) {
    helper.builder.ue_grants().emplace_back(helper.dl_msg_alloc.pdu);
  }

  // Add CSI PDU.
  for (unsigned i = 0; i != MAX_CSI_RS_PDUS_PER_SLOT; ++i) {
    helper.builder.csi_rs().emplace_back(build_valid_csi_pdu(helper));
  }

  // Add DL-PRS PDUs.
  for (unsigned i = 0; i != MAX_PRS_PDUS_PER_SLOT; ++i) {
    helper.sched_result.prs.push_back(build_valid_prs_pdu());
  }

  // Add SSBs.
  for (unsigned i = 0; i != MAX_SSB_PER_SLOT; ++i) {
    helper.result.ssb_pdus.push_back(build_valid_dl_ssb_pdu());
  }

  return helper;
}

prach_occasion_info unittests::build_valid_prach_occasion()
{
  // NOTE: Used random values from the property range, FAPI document SCF-222 v4.0 section 3.4.3.1 in table PRACH PDU.
  return prach_occasion_info{.pci                  = test_pci,
                             .nof_prach_occasions  = 5,
                             .format               = prach_format_type::one,
                             .index_fd_ra          = 2,
                             .start_symbol         = 2,
                             .nof_cs               = 3,
                             .nof_fd_ra            = 4,
                             .start_preamble_index = 11,
                             .nof_preamble_indexes = 12};
}

prs_info unittests::build_valid_prs_pdu()
{
  return prs_info{.scs             = subcarrier_spacing::kHz30,
                  .cp              = cyclic_prefix::NORMAL,
                  .n_id_prs        = 1023,
                  .comb_size       = prs_comb_size::four,
                  .comb_offset     = 3,
                  .nof_symbols     = prs_num_symbols::twelve,
                  .symbols         = ofdm_symbol_range{2, 14},
                  .crbs            = crb_interval{12, 60},
                  .power_offset_db = -3};
}

ul_sched_info_test_helper unittests::build_valid_pusch_pdu()
{
  ul_sched_info_test_helper helper;
  helper.bwp_cfg = std::make_unique<bwp_configuration>(
      bwp_configuration{.cp = cyclic_prefix::NORMAL, .scs = subcarrier_spacing::kHz15, .crbs = crb_interval{10, 20}});

  const ofdm_symbol_range symbols{2, 12};

  helper.info = ul_sched_info{
      .pusch_cfg = pusch_information{.rnti                       = to_rnti(0x4444),
                                     .bwp_cfg                    = helper.bwp_cfg.get(),
                                     .rbs                        = vrb_interval(10, 20),
                                     .symbols                    = symbols,
                                     .is_cg                      = false,
                                     .intra_slot_freq_hopping    = false,
                                     .pusch_second_hop_prb       = 3,
                                     .tx_direct_current_location = 1,
                                     .ul_freq_shift_7p5khz       = false,
                                     .mcs_descr           = sch_mcs_description{.modulation       = modulation_scheme::QAM256,
                                                                                .target_code_rate = 193},
                                     .mcs_index           = 3,
                                     .mcs_table           = pusch_mcs_table::qam256,
                                     .transform_precoding = false,
                                     .n_id                = 69,
                                     .nof_layers          = 1,
                                     .dmrs                = make_dmrs_information(symbols.start()),
                                     .pusch_dmrs_id       = 18,
                                     .dmrs_hopping_mode   = pusch_information::dmrs_hopping_mode::no_hopping,
                                     .rv_index            = 1,
                                     .harq_id             = to_harq_id(2),
                                     .new_data            = true,
                                     .tb_size_bytes       = units::bytes{11},
                                     .nof_cb              = 0,
                                     .repetitions         = std::nullopt},
      // UCI.
      .uci =
          uci_info{.harq  = uci_info::harq_info{.harq_ack_nof_bits = 1, .beta_offset_harq_ack = 4},
                   .csi   = uci_info::csi_info{.csi_rep_cfg =
                                                 csi_report_configuration{.nof_csi_rs_resources = 1,
                                                                            .nof_reported_rs      = 1,
                                                                            .pmi_codebook =
                                                                              pmi_codebook_typeI_single_panel{
                                                                                    .n1_n2 = pmi_codebook_single_panel_config::two_one, .mode = pmi_codebook_typeI_mode::one},
                                                                            .ri_restriction = ri_restriction_type(
                                                                              {true, true, true, true}),
                                                                            .quantities =
                                                                              csi_report_quantities::cri_ri_li_pmi_cqi,
                                                                            .subband = std::nullopt},
                                               .csi_part1_nof_bits = 2,
                                               .beta_offset_csi_1  = 13,
                                               .beta_offset_csi_2  = 6},
                   .alpha = alpha_scaling_opt::f0p5},
      .context = ul_sched_info::decision_context{.ue_index   = to_du_ue_index(0),
                                                 .ss_id      = to_search_space_id(0),
                                                 .k2         = 0,
                                                 .nof_retxs  = 0,
                                                 .msg3_delay = std::nullopt,
                                                 // Rapid.
                                                 .rapid       = 123,
                                                 .olla_offset = std::nullopt,
                                                 .nof_oh_prb  = 0}};

  return helper;
}

srs_info_helper unittests::build_valid_srs_pdu()
{
  srs_info_helper helper;
  helper.bwp_cfg = std::make_unique<bwp_configuration>(
      bwp_configuration{.cp = cyclic_prefix::NORMAL, .scs = subcarrier_spacing::kHz15, .crbs = crb_interval{10, 20}});

  helper.pdu = srs_info{.crnti                                  = to_rnti(23),
                        .bwp_cfg                                = helper.bwp_cfg.get(),
                        .nof_antenna_ports                      = 4,
                        .symbols                                = ofdm_symbol_range{0, 1},
                        .nof_repetitions                        = n1,
                        .config_index                           = 4,
                        .sequence_id                            = 4,
                        .bw_index                               = 2,
                        .tx_comb                                = tx_comb_size::n2,
                        .comb_offset                            = 1,
                        .cyclic_shift                           = 2,
                        .freq_position                          = 32,
                        .freq_shift                             = 21,
                        .freq_hopping                           = 3,
                        .group_or_seq_hopping                   = srs_group_or_sequence_hopping::neither,
                        .resource_type                          = srs_resource_type::aperiodic,
                        .t_srs_period                           = srs_periodicity::sl8,
                        .t_offset                               = 5,
                        .normalized_channel_iq_matrix_requested = true,
                        .positioning_report_requested           = true};

  return helper;
}

/// \brief Builds a PUCCH PDU with the given format parameters and UCI bits.
///
/// The resource uses all the symbols that the format allows and starts at PRB 1.
static pucch_info_test_helper build_pucch_pdu(unsigned                                       nof_symbols,
                                              unsigned                                       second_hop_prb,
                                              const decltype(pucch_resource::format_params)& res_format_params,
                                              const decltype(pucch_info::format_params)&     info_format_params,
                                              const pucch_uci_bits&                          uci_bits)
{
  pucch_info_test_helper helper;
  helper.bwp_cfg = std::make_unique<bwp_configuration>(
      bwp_configuration{.cp = cyclic_prefix::NORMAL, .scs = subcarrier_spacing::kHz15, .crbs = crb_interval{2, 10}});
  helper.res = std::make_unique<pucch_resource>(pucch_resource{.res_id         = pucch_res_id_t::make_ded(0, 0),
                                                               .starting_prb   = 1,
                                                               .syms           = ofdm_symbol_range{0, nof_symbols},
                                                               .second_hop_prb = second_hop_prb,
                                                               .format_params  = res_format_params,
                                                               .rep_factor     = pucch_repetition_factor::n1});

  helper.info = pucch_info{.crnti         = to_rnti(0x4444),
                           .bwp_cfg       = helper.bwp_cfg.get(),
                           .res           = helper.res.get(),
                           .format_params = info_format_params,
                           .uci_bits      = uci_bits,
                           .csi_rep_cfg   = std::nullopt,
                           .repetition    = std::nullopt};

  return helper;
}

pucch_info_test_helper unittests::build_valid_pucch_format_0_pdu()
{
  return build_pucch_pdu(pucch_constants::f0::MAX_NOF_SYMS,
                         3,
                         pucch_resource::f0_config{.initial_cyclic_shift = 7},
                         pucch_info::f0_config{.group_hopping = pucch_group_hopping::NEITHER, .n_id_hopping = 5},
                         pucch_uci_bits{.harq_ack_nof_bits = 2, .sr_bits = sr_nof_bits::one, .csi_part1_nof_bits = 0});
}

pucch_info_test_helper unittests::build_valid_pucch_format_1_pdu()
{
  return build_pucch_pdu(
      pucch_constants::f1::MAX_NOF_SYMS,
      2,
      pucch_resource::f1_config{.initial_cyclic_shift = 9, .time_domain_occ = 3},
      pucch_info::f1_config{.group_hopping = pucch_group_hopping::NEITHER, .n_id_hopping = 2},
      pucch_uci_bits{.harq_ack_nof_bits = 2, .sr_bits = sr_nof_bits::no_sr, .csi_part1_nof_bits = 0});
}

/// UCI bits shared by the PUCCH formats 2, 3 and 4 PDUs.
static constexpr pucch_uci_bits pucch_f2_f3_f4_uci_bits{.harq_ack_nof_bits  = 100,
                                                        .sr_bits            = sr_nof_bits::one,
                                                        .csi_part1_nof_bits = 102};

/// \brief Number of PRBs of the PUCCH formats 2 and 3 resources, and of the grants allocated in them.
///
/// The grant uses fewer PRBs than its resource, as the scheduler does when the UCI fits in fewer PRBs, so that the PRBs
/// reported to FAPI can only come from the grant.
static constexpr unsigned pucch_f2_f3_resource_nof_prbs = 3;
static constexpr unsigned pucch_f2_f3_grant_nof_prbs    = 2;

pucch_info_test_helper unittests::build_valid_pucch_format_2_pdu()
{
  return build_pucch_pdu(
      pucch_constants::f2::MAX_NOF_SYMS,
      1,
      pucch_resource::f2_config{.nof_prbs = pucch_f2_f3_resource_nof_prbs},
      pucch_info::f2_config{.n_id_scrambling = 382, .n_id_0_scrambling = 256, .nof_prbs = pucch_f2_f3_grant_nof_prbs},
      pucch_f2_f3_f4_uci_bits);
}

pucch_info_test_helper unittests::build_valid_pucch_format_3_pdu()
{
  return build_pucch_pdu(
      pucch_constants::f3::MAX_NOF_SYMS,
      1,
      pucch_resource::f3_config{.nof_prbs = pucch_f2_f3_resource_nof_prbs, .pi_2_bpsk = true, .additional_dmrs = false},
      pucch_info::f3_config{.group_hopping     = pucch_group_hopping::NEITHER,
                            .n_id_hopping      = 180,
                            .n_id_scrambling   = 382,
                            .n_id_0_scrambling = 256,
                            .nof_prbs          = pucch_f2_f3_grant_nof_prbs},
      pucch_f2_f3_f4_uci_bits);
}

pucch_info_test_helper unittests::build_valid_pucch_format_4_pdu()
{
  return build_pucch_pdu(pucch_constants::f4::MAX_NOF_SYMS,
                         5,
                         pucch_resource::f4_config{.occ_index       = pucch_f4_occ_idx::n1,
                                                   .occ_length      = pucch_f4_occ_len::n2,
                                                   .pi_2_bpsk       = true,
                                                   .additional_dmrs = false},
                         pucch_info::f4_config{.group_hopping     = pucch_group_hopping::NEITHER,
                                               .n_id_hopping      = 180,
                                               .n_id_scrambling   = 382,
                                               .n_id_0_scrambling = 256},
                         pucch_f2_f3_f4_uci_bits);
}

/// Initializes the result of the helper with no PDUs.
static void initialize_ul_sched_result(mac_ul_sched_result_test_helper& helper)
{
  helper.result.slot   = slot_point(4, 512, 41);
  helper.result.ul_res = helper.sched_result;
}

mac_ul_sched_result_test_helper unittests::build_valid_mac_ul_sched_result()
{
  mac_ul_sched_result_test_helper helper;
  initialize_ul_sched_result(helper);

  helper.pucch_f1 = build_valid_pucch_format_1_pdu();
  helper.sched_result.pucchs.emplace(helper.pucch_f1.info);
  helper.pucch_f2 = build_valid_pucch_format_2_pdu();
  helper.sched_result.pucchs.emplace(helper.pucch_f2.info);

  helper.pusch = build_valid_pusch_pdu();
  helper.builder.puschs().emplace_back(helper.pusch.info);

  helper.sched_result.prachs.push_back(build_valid_prach_occasion());

  return helper;
}

mac_ul_sched_result_test_helper unittests::build_valid_mac_ul_sched_result_with_all_supported_pdus()
{
  mac_ul_sched_result_test_helper helper;
  initialize_ul_sched_result(helper);

  // All the PDUs of a kind share the BWP and resource owned by the helper.
  helper.pucch_f1 = build_valid_pucch_format_1_pdu();
  for (unsigned i = 0; i != MAX_PUCCH_PDUS_PER_SLOT; ++i) {
    helper.sched_result.pucchs.emplace(helper.pucch_f1.info);
  }

  helper.pusch = build_valid_pusch_pdu();
  for (unsigned i = 0; i != MAX_PUSCH_PDUS_PER_SLOT; ++i) {
    helper.builder.puschs().emplace_back(helper.pusch.info);
  }

  for (unsigned i = 0; i != MAX_PRACH_OCCASIONS_PER_SLOT; ++i) {
    helper.sched_result.prachs.push_back(build_valid_prach_occasion());
  }

  helper.srs = build_valid_srs_pdu();
  for (unsigned i = 0; i != MAX_SRS_PDUS_PER_SLOT; ++i) {
    helper.sched_result.srss.push_back(helper.srs.pdu);
  }

  return helper;
}

/// Creates a data result without PDUs, whose PDUs will all carry the same payload.
static mac_dl_data_result_test_helper create_data_result()
{
  return mac_dl_data_result_test_helper{.data = std::make_unique<std::array<uint8_t, 1>>(),
                                        .result =
                                            mac_dl_data_result{.slot     = slot_point(subcarrier_spacing::kHz240, 0, 0),
                                                               .si_pdus  = {},
                                                               .rar_pdus = {},
                                                               .ue_pdus  = {},
                                                               .paging_pdus = {}}};
}

/// Adds the given number of single codeword PDUs to the list, carrying the payload of the helper.
template <typename PduList>
static void add_data_pdus(PduList& pdus, const mac_dl_data_result_test_helper& helper, unsigned nof_pdus)
{
  for (unsigned i = 0; i != nof_pdus; ++i) {
    pdus.emplace_back(0, shared_transport_block(*helper.data));
  }
}

mac_dl_data_result_test_helper unittests::build_valid_mac_data_result()
{
  mac_dl_data_result_test_helper helper = create_data_result();
  add_data_pdus(helper.result.si_pdus, helper, 1);
  return helper;
}

mac_dl_data_result_test_helper unittests::build_valid_mac_data_result_with_all_supported_pdu()
{
  mac_dl_data_result_test_helper helper = create_data_result();
  add_data_pdus(helper.result.si_pdus, helper, MAX_SI_PDUS_PER_SLOT);
  add_data_pdus(helper.result.rar_pdus, helper, MAX_RAR_PDUS_PER_SLOT);
  add_data_pdus(helper.result.ue_pdus, helper, MAX_UE_PDUS_PER_SLOT);
  add_data_pdus(helper.result.paging_pdus, helper, MAX_PAGING_PDUS_PER_SLOT);
  return helper;
}
