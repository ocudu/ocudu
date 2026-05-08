// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "message_builder_helpers.h"
#include <random>

static std::mt19937 gen(0);

using namespace ocudu;
using namespace fapi;

static unsigned generate_sfn()
{
  std::uniform_int_distribution<unsigned> dist(0, 1023);
  return dist(gen);
}

static unsigned generate_slot()
{
  std::uniform_int_distribution<unsigned> dist(0, 159);

  return dist(gen);
}

static ofdm_symbol_range generate_symbols()
{
  std::uniform_int_distribution<unsigned> size_dist(0, 13);
  unsigned                                size = size_dist(gen);

  std::uniform_int_distribution<unsigned> start_dist(0, size);
  unsigned                                start = start_dist(gen);

  return {start, size};
}

static rnti_t generate_rnti()
{
  std::uniform_int_distribution<unsigned> dist(1, 65535);

  return to_rnti(dist(gen));
}

static unsigned generate_uint16()
{
  std::uniform_int_distribution<unsigned> dist(0, 65535);

  return dist(gen);
}

static unsigned generate_handle()
{
  std::uniform_int_distribution<unsigned> dist(0, std::numeric_limits<uint32_t>::max());

  return dist(gen);
}

static unsigned generate_bwp_start()
{
  std::uniform_int_distribution<unsigned> dist(0, 274);

  return dist(gen);
}

static crb_interval generate_crb_interval()
{
  std::uniform_int_distribution<unsigned> size_dist(0, 274);
  unsigned                                size = size_dist(gen);

  std::uniform_int_distribution<unsigned> start_dist(0, size);
  unsigned                                start = start_dist(gen);

  return {start, size};
}

static prb_interval generate_prb_interval()
{
  std::uniform_int_distribution<unsigned> size_dist(0, 274);
  unsigned                                size = size_dist(gen);

  std::uniform_int_distribution<unsigned> start_dist(0, size);
  unsigned                                start = start_dist(gen);

  return {start, size};
}

static vrb_interval generate_vrb_interval()
{
  std::uniform_int_distribution<unsigned> size_dist(0, 274);
  unsigned                                size = size_dist(gen);

  std::uniform_int_distribution<unsigned> start_dist(0, size);
  unsigned                                start = start_dist(gen);

  return {start, size};
}

static subcarrier_spacing generate_scs()
{
  std::uniform_int_distribution<unsigned> dist(0, 4);

  return static_cast<subcarrier_spacing>(dist(gen));
}

static cyclic_prefix generate_cyclic_prefix()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);

  return static_cast<cyclic_prefix::options>(dist(gen));
}

static pci_t generate_pci()
{
  std::uniform_int_distribution<unsigned> dist(0, 1007);

  return dist(gen);
}

static ssb_id_t generate_block_index()
{
  std::uniform_int_distribution<unsigned> dist(0, 63);

  return dist(gen);
}

static ssb_subcarrier_offset generate_subcarrier_offset()
{
  std::uniform_int_distribution<unsigned> dist(0, 23);

  return dist(gen);
}

static unsigned generate_offset_point_A()
{
  std::uniform_int_distribution<unsigned> dist(0, 2199);

  return dist(gen);
}

static ssb_pattern_case generate_case_pattern()
{
  std::uniform_int_distribution<unsigned> dist(0, 4);

  return static_cast<ssb_pattern_case>(dist(gen));
}

static bool generate_bool()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);
  return dist(gen);
}

static coreset_id generate_coreset_id()
{
  std::uniform_int_distribution<unsigned> dist(0, 11);

  return static_cast<coreset_id>(dist(gen));
}

dl_ssb_pdu unittest::build_valid_dl_ssb_pdu()
{
  dl_ssb_pdu pdu;

  pdu.phys_cell_id = generate_pci();

  auto& power    = pdu.power_config.emplace<dl_ssb_pdu::power_profile_nr>();
  power.beta_pss = ssb_pss_to_sss_epre::dB_0;

  pdu.ssb_block_index   = generate_block_index();
  pdu.subcarrier_offset = generate_subcarrier_offset();
  pdu.ssb_offset_pointA = generate_offset_point_A();
  pdu.bch_payload       = 0;
  pdu.case_type         = generate_case_pattern();
  pdu.scs               = subcarrier_spacing::kHz240;
  pdu.L_max             = 4;

  return pdu;
}

dl_pdcch_pdu unittest::build_valid_dl_pdcch_pdu()
{
  dl_pdcch_pdu pdu;

  pdu.coreset_bwp = generate_crb_interval();
  pdu.scs         = subcarrier_spacing::kHz240;
  pdu.cp          = generate_cyclic_prefix();
  pdu.symbols     = generate_symbols();
  pdu.freq_domain_resource.resize(pdu.freq_domain_resource.max_size());
  for (unsigned i = 0, e = pdu.freq_domain_resource.max_size(); i != e; ++i) {
    pdu.freq_domain_resource.set(i, generate_bool());
  }
  uint8_t    reg_bundle_size  = 2;
  uint8_t    interleaver_size = 3;
  uint16_t   shift_index      = 129;
  coreset_id id               = generate_coreset_id();
  if (id == to_coreset_id(0)) {
    pdu.mapping = dl_pdcch_pdu::mapping_coreset_0{reg_bundle_size, interleaver_size, shift_index};
  } else {
    pdu.mapping = dl_pdcch_pdu::mapping_interleaved{reg_bundle_size, interleaver_size, shift_index};
  }
  pdu.precoder_granularity = static_cast<coreset_configuration::precoder_granularity_type>(generate_bool());

  // Add the DCI.
  pdu.dl_dci.rnti                  = generate_rnti();
  pdu.dl_dci.nid_pdcch_data        = generate_uint16();
  pdu.dl_dci.nid_pdcch_dmrs        = generate_uint16();
  pdu.dl_dci.nrnti_pdcch_data      = generate_uint16();
  pdu.dl_dci.cce_index             = 0;
  pdu.dl_dci.dci_aggregation_level = aggregation_level::n2;
  pdu.dl_dci.power_config.emplace<fapi::dl_dci_pdu::power_profile_nr>().power_control_offset_ss_db = 0;
  pdu.dl_dci.payload                   = {1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0};
  pdu.dl_dci.precoding_and_beamforming = build_valid_tx_precoding_and_beamforming_pdu();

  return pdu;
}

dl_pdsch_pdu unittest::build_valid_dl_pdsch_pdu()
{
  dl_pdsch_pdu pdu;
  pdu.rnti             = to_rnti(3);
  pdu.bwp              = {3, 4};
  pdu.scs              = subcarrier_spacing::kHz15;
  pdu.cp               = cyclic_prefix::NORMAL;
  pdu.cws              = {{modulation_scheme::QPSK, sch_mcs_index(3), pdsch_mcs_table(1), 3, units::bytes{12}}};
  pdu.nid_pdsch        = 65;
  pdu.num_layers       = 6;
  pdu.ref_point        = pdsch_ref_point_type::point_a;
  pdu.dl_dmrs_symb_pos = dmrs_symbol_mask(13);
  pdu.dl_dmrs_symb_pos.from_uint64(0);
  pdu.pdsch_dmrs_scrambling_id  = 31;
  pdu.dmrs_type                 = dmrs_config_type::type1;
  pdu.nscid                     = 0;
  pdu.num_dmrs_cdm_grps_no_data = 2;
  pdu.dmrs_ports                = dmrs_ports_mask(12);
  pdu.dmrs_ports.from_uint64(0);
  pdu.resource_alloc.vrbs = {42, 89};
  pdu.vrb_to_prb_mapping  = vrb_to_prb::mapping_type::interleaved_n2;
  pdu.symbols             = generate_symbols();

  auto& power                 = pdu.power_config.emplace<dl_pdsch_pdu::power_profile_nr>();
  power.pwr_control_offset_db = 6;
  power.pwr_control_offset_ss = fapi::power_control_offset_ss::dB3;

  pdu.precoding_and_beamforming = build_valid_tx_precoding_and_beamforming_pdu();
  pdu.tb_size_lbrm              = units::bytes(0);
  pdu.ldpc_base_graph           = ocudu::ldpc_base_graph_type::BG1;
  pdu.nof_csi_pdus_for_rm       = 0;

  auto& pdu_vrb_to_prb_mapping           = pdu.mapping.emplace<dl_pdsch_pdu::non_interleaved_common_ss>();
  pdu_vrb_to_prb_mapping.N_start_coreset = 0U;

  return pdu;
}

ocudu::fapi::dl_prs_pdu unittest::build_valid_dl_prs_pdu()
{
  dl_prs_pdu pdu;

  pdu.scs                 = subcarrier_spacing::kHz240;
  pdu.cp                  = cyclic_prefix::NORMAL;
  pdu.nid_prs             = 1;
  pdu.comb_size           = prs_comb_size::two;
  pdu.comb_offset         = 0;
  pdu.num_symbols         = prs_num_symbols::four;
  pdu.first_symbol        = 8;
  pdu.crbs                = {24, 28};
  pdu.prs_power_offset_db = -13.3;

  // Precoding.
  pdu.precoding_and_beamforming.prg_size = 276;
  // Use identity matrix
  pdu.precoding_and_beamforming.prg.pm_index = 0;

  return pdu;
}

dl_csi_rs_pdu unittest::build_valid_dl_csi_pdu()
{
  dl_csi_rs_pdu pdu;

  pdu.scs                     = subcarrier_spacing::kHz15;
  pdu.cp                      = cyclic_prefix::NORMAL;
  pdu.crbs                    = {23, 28};
  pdu.type                    = csi_rs_type::CSI_RS_NZP;
  pdu.row                     = 1;
  pdu.freq_domain             = {1, 0, 0, 0, 0, 0};
  pdu.symb_L0                 = 8;
  pdu.symb_L1                 = 7;
  pdu.cdm_type                = csi_rs_cdm_type::no_CDM;
  pdu.freq_density            = csi_rs_freq_density_type::three;
  pdu.scramb_id               = 123;
  auto& power                 = pdu.power_config.emplace<dl_csi_rs_pdu::power_profile_nr>();
  power.pwr_control_offset_db = 6;
  power.pwr_control_offset_ss = fapi::power_control_offset_ss::dB3;
  pdu.bwp                     = {56U, 60U};

  return pdu;
}

dl_tti_request unittest::build_valid_dl_tti_request()
{
  dl_tti_request msg;

  auto     scs        = subcarrier_spacing::kHz240;
  unsigned sfn        = 4U;
  auto     slot_index = 0U;
  msg.slot            = slot_point(scs, sfn, slot_index);

  // Manually add the SSB PDU to reuse the functions above.
  auto& last_ssb_pdu = msg.pdus.emplace_back();
  auto& ssb_pdu      = last_ssb_pdu.pdu.emplace<dl_ssb_pdu>();
  ssb_pdu            = build_valid_dl_ssb_pdu();

  // Manually add the PDCCH PDU to reuse the functions above.
  auto& last_pdcch_pdu = msg.pdus.emplace_back();
  auto& pdcch_pdu      = last_pdcch_pdu.pdu.emplace<dl_pdcch_pdu>();
  pdcch_pdu            = build_valid_dl_pdcch_pdu();

  // Manually add the PDSCH PDU to reuse the functions above.
  auto& last_pdsch_pdu = msg.pdus.emplace_back();
  auto& pdsch_pdu      = last_pdsch_pdu.pdu.emplace<dl_pdsch_pdu>();
  pdsch_pdu            = build_valid_dl_pdsch_pdu();

  // Manually add the CSI PDU to reuse the functions above.
  auto& last_csi_rs_pdu = msg.pdus.emplace_back();
  auto& csi_rs_pdu      = last_csi_rs_pdu.pdu.emplace<dl_csi_rs_pdu>();
  csi_rs_pdu            = build_valid_dl_csi_pdu();

  return msg;
}

static uint8_t generate_index_fd_ra()
{
  std::uniform_int_distribution<unsigned> dist(0, 7);

  return dist(gen);
}

static uint8_t generate_prach_start_symbol()
{
  std::uniform_int_distribution<unsigned> dist(0, 13);

  return dist(gen);
}

static uint8_t generate_num_cs()
{
  std::uniform_int_distribution<unsigned> dist(0, 419);

  return dist(gen);
}

static prach_format_type generate_prach_format()
{
  std::uniform_int_distribution<unsigned> dist(0, 3);

  return static_cast<prach_format_type>(dist(gen));
}

static ul_prach_pdu::preambles_interval generate_preambles()
{
  std::uniform_int_distribution<unsigned> num_preambles_dist(1, 64);

  unsigned num_preambles = num_preambles_dist(gen);

  unsigned start_preamble = std::min(static_cast<unsigned>(generate_num_cs()), num_preambles);

  return {num_preambles, start_preamble};
}

ul_prach_pdu unittest::build_valid_ul_prach_pdu()
{
  ul_prach_pdu pdu;

  pdu.num_prach_ocas     = 1;
  pdu.prach_format       = generate_prach_format();
  pdu.index_fd_ra        = generate_index_fd_ra();
  pdu.prach_start_symbol = generate_prach_start_symbol();
  pdu.num_cs             = generate_num_cs();
  pdu.num_fd_ra          = generate_index_fd_ra() + 1U;
  pdu.preambles          = generate_preambles();

  return pdu;
}

static int generate_ul_sinr_metric()
{
  std::uniform_int_distribution<int> dist(-32768, 32767);
  return dist(gen);
}

static phy_time_unit generate_timing_advance_offset()
{
  std::uniform_int_distribution<unsigned> dist(0, 100);
  unsigned                                value = dist(gen);

  return phy_time_unit::from_seconds((value < 64U) ? value : 65535U);
}

static unsigned generate_rssi_or_rsrp()
{
  std::uniform_int_distribution<unsigned> dist(0, 2500);
  unsigned                                value = dist(gen);

  return (value <= 1280U) ? value : 65535;
}

static units::bits generate_bit_length()
{
  std::uniform_int_distribution<unsigned> dist(1, 1706);
  return units::bits(dist(gen));
}

static uci_harq_pdu generate_harq_pdu()
{
  uci_harq_pdu pdu;

  pdu.detection_status    = uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
  pdu.expected_bit_length = generate_bit_length();
  pdu.payload.resize(pdu.expected_bit_length.value());

  return pdu;
}

static uci_csi_part1 generate_csi_part1_pdu()
{
  uci_csi_part1 pdu;

  pdu.detection_status    = uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
  pdu.expected_bit_length = generate_bit_length();
  pdu.payload.resize(pdu.expected_bit_length.value());

  return pdu;
}

static uci_csi_part2 generate_csi_part2_pdu()
{
  uci_csi_part2 pdu;

  pdu.detection_status    = uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
  pdu.expected_bit_length = generate_bit_length();
  pdu.payload.resize(pdu.expected_bit_length.value());

  return pdu;
}

uci_pusch_pdu unittest::build_valid_uci_pusch_pdu()
{
  uci_pusch_pdu pdu;

  pdu.handle                = generate_handle();
  pdu.rnti                  = generate_rnti();
  pdu.ul_sinr_metric        = static_cast<int16_t>(generate_ul_sinr_metric());
  pdu.timing_advance_offset = generate_timing_advance_offset();
  pdu.rssi                  = generate_rssi_or_rsrp();
  pdu.rsrp                  = generate_rssi_or_rsrp();

  // Enable HARQ, CSI Part 1 and CSI Part 2.
  pdu.harq      = generate_harq_pdu();
  pdu.csi_part1 = generate_csi_part1_pdu();
  pdu.csi_part2 = generate_csi_part2_pdu();

  return pdu;
}

static sr_pdu_format_0_1 generate_sr_format01_pdu()
{
  sr_pdu_format_0_1 pdu;

  std::uniform_int_distribution<unsigned> dist(0, 3);
  pdu.sr_detected = std::min(1U, dist(gen));

  return pdu;
}

static uci_harq_format_0_1 generate_harq_format01_pdu()
{
  uci_harq_format_0_1 pdu;

  std::uniform_int_distribution<unsigned> dist(0, 2);

  std::uniform_int_distribution<unsigned> nof_dist(1, 2);
  for (unsigned i = 0, e = nof_dist(gen); i != e; ++i) {
    pdu.harq_values.emplace_back(static_cast<uci_pucch_f0_or_f1_harq_values>(dist(gen)));
  }

  return pdu;
}

uci_pucch_pdu_format_0_1 unittest::build_valid_uci_pucch_format01_pdu()
{
  uci_pucch_pdu_format_0_1 pdu;

  pdu.handle                = generate_handle();
  pdu.rnti                  = generate_rnti();
  pdu.ul_sinr_metric        = static_cast<int16_t>(generate_ul_sinr_metric());
  pdu.timing_advance_offset = generate_timing_advance_offset();
  pdu.rssi                  = generate_rssi_or_rsrp();
  pdu.rsrp                  = generate_rssi_or_rsrp();

  std::uniform_int_distribution<unsigned> dist(0, 1);
  pdu.pucch_format = static_cast<uci_pucch_pdu_format_0_1::format_type>(dist(gen));

  // Enable SR and HARQ for PUCCH format 0/1.
  pdu.sr   = generate_sr_format01_pdu();
  pdu.harq = generate_harq_format01_pdu();

  return pdu;
}

static sr_pdu_format_2_3_4 generate_sr_format234_pdu()
{
  sr_pdu_format_2_3_4 pdu;

  std::uniform_int_distribution<unsigned> dist(1, 4);
  pdu.sr_payload.resize(std::ceil(static_cast<float>(dist(gen)) / 8.F));

  return pdu;
}

uci_pucch_pdu_format_2_3_4 unittest::build_valid_uci_pucch_format234_pdu()
{
  uci_pucch_pdu_format_2_3_4 pdu;

  pdu.handle                = generate_handle();
  pdu.rnti                  = generate_rnti();
  pdu.ul_sinr_metric        = static_cast<int16_t>(generate_ul_sinr_metric());
  pdu.timing_advance_offset = generate_timing_advance_offset();
  pdu.rssi                  = generate_rssi_or_rsrp();
  pdu.rsrp                  = generate_rssi_or_rsrp();

  std::uniform_int_distribution<unsigned> dist(0, 2);
  pdu.pucch_format = static_cast<uci_pucch_pdu_format_2_3_4::format_type>(dist(gen));

  // Enable SR, HARQ, CSI Part 1. CSI Part 2, UCI payload Part 1 and UCI payload Part 2.
  pdu.sr        = generate_sr_format234_pdu();
  pdu.harq      = generate_harq_pdu();
  pdu.csi_part1 = generate_csi_part1_pdu();
  pdu.csi_part2 = generate_csi_part2_pdu();

  return pdu;
}

static unsigned generate_sr_bit_len(ul_pucch_pdu::ul_pucch_pdu_format format)
{
  if (std::holds_alternative<ul_pucch_pdu_format_0>(format) || std::holds_alternative<ul_pucch_pdu_format_1>(format)) {
    std::uniform_int_distribution<unsigned> dist(0, 1);
    return dist(gen);
  }

  std::uniform_int_distribution<unsigned> dist(0, 4);
  return dist(gen);
}

static units::bits generate_harq_bit_len(ul_pucch_pdu::ul_pucch_pdu_format format)
{
  if (std::holds_alternative<ul_pucch_pdu_format_0>(format) || std::holds_alternative<ul_pucch_pdu_format_1>(format)) {
    std::uniform_int_distribution<unsigned> dist(1, 2);
    return units::bits(dist(gen));
  }

  std::uniform_int_distribution<unsigned> dist(2, 1706);
  return units::bits(dist(gen));
}

static unsigned generate_nid_pucch_hopping()
{
  std::uniform_int_distribution<unsigned> dist(0, 1023);
  return dist(gen);
}

static unsigned generate_initial_cyclic_shift()
{
  std::uniform_int_distribution<unsigned> dist(0, 11);
  return dist(gen);
}

static units::bits generate_csi_bit_len()
{
  std::uniform_int_distribution<unsigned> dist(0, 1706);
  return units::bits(dist(gen));
}

static ul_pucch_pdu generate_generic_ul_pucch_pdu()
{
  ul_pucch_pdu pdu;

  pdu.rnti           = generate_rnti();
  pdu.handle         = generate_handle();
  pdu.bwp            = generate_crb_interval();
  pdu.scs            = generate_scs();
  pdu.cp             = generate_cyclic_prefix();
  pdu.prbs           = generate_prb_interval();
  pdu.symbols        = generate_symbols();
  pdu.second_hop_prb = generate_bwp_start();

  return pdu;
}

ul_pucch_pdu unittest::build_valid_ul_pucch_f0_pdu()
{
  auto  pdu                   = generate_generic_ul_pucch_pdu();
  auto& format                = pdu.format.emplace<ul_pucch_pdu_format_0>();
  format.sr_present           = generate_sr_bit_len(pdu.format);
  format.nid_pucch_hopping    = generate_nid_pucch_hopping();
  format.initial_cyclic_shift = generate_initial_cyclic_shift();
  format.bit_len_harq         = generate_harq_bit_len(pdu.format);

  return pdu;
}

ul_pucch_pdu unittest::build_valid_ul_pucch_f1_pdu()
{
  auto  pdu                    = generate_generic_ul_pucch_pdu();
  auto& format                 = pdu.format.emplace<ul_pucch_pdu_format_1>();
  format.nid_pucch_hopping     = generate_nid_pucch_hopping();
  format.initial_cyclic_shift  = generate_initial_cyclic_shift();
  format.time_domain_occ_index = 5;
  format.sr_present            = generate_sr_bit_len(pdu.format);
  format.bit_len_harq          = generate_harq_bit_len(pdu.format);

  return pdu;
}

ul_pucch_pdu unittest::build_valid_ul_pucch_f2_pdu()
{
  auto  pdu                         = generate_generic_ul_pucch_pdu();
  auto& format                      = pdu.format.emplace<ul_pucch_pdu_format_2>();
  format.sr_bit_len                 = static_cast<sr_nof_bits>(generate_sr_bit_len(pdu.format));
  format.nid_pucch_scrambling       = generate_nid_pucch_hopping();
  format.nid0_pucch_dmrs_scrambling = 10200;
  format.csi_part1_bit_length       = generate_csi_bit_len();
  format.bit_len_harq               = generate_harq_bit_len(pdu.format);

  return pdu;
}

ul_pucch_pdu unittest::build_valid_ul_pucch_f3_pdu()
{
  auto  pdu                         = generate_generic_ul_pucch_pdu();
  auto& format                      = pdu.format.emplace<ul_pucch_pdu_format_3>();
  format.pi2_bpsk                   = true;
  format.nid_pucch_hopping          = generate_nid_pucch_hopping();
  format.nid0_pucch_dmrs_scrambling = 10200;
  format.m0_pucch_dmrs_cyclic_shift = 8;
  format.sr_bit_len                 = static_cast<sr_nof_bits>(generate_sr_bit_len(pdu.format));
  format.add_dmrs_flag              = true;
  format.csi_part1_bit_length       = generate_csi_bit_len();
  format.bit_len_harq               = generate_harq_bit_len(pdu.format);

  return pdu;
}

ul_pucch_pdu unittest::build_valid_ul_pucch_f4_pdu()
{
  auto  pdu                         = generate_generic_ul_pucch_pdu();
  auto& format                      = pdu.format.emplace<ul_pucch_pdu_format_4>();
  format.pi2_bpsk                   = true;
  format.nid_pucch_hopping          = generate_nid_pucch_hopping();
  format.nid0_pucch_dmrs_scrambling = 10200;
  format.pre_dft_occ_len            = 2;
  format.pre_dft_occ_idx            = 2;
  format.add_dmrs_flag              = true;
  format.nid0_pucch_dmrs_scrambling = 10200;
  format.m0_pucch_dmrs_cyclic_shift = 8;
  format.sr_bit_len                 = static_cast<sr_nof_bits>(generate_sr_bit_len(pdu.format));
  format.csi_part1_bit_length       = generate_csi_bit_len();
  format.bit_len_harq               = generate_harq_bit_len(pdu.format);

  return pdu;
}

static modulation_scheme generate_qam_mod_order(bool transform_precoding)
{
  static constexpr std::array<unsigned, 5> values = {1, 2, 4, 6, 8};

  std::uniform_int_distribution<unsigned> dist(0, 4);
  unsigned                                index = dist(gen);

  if (!transform_precoding && index == 0) {
    ++index;
  }

  return static_cast<modulation_scheme>(values[index]);
}

static unsigned generate_mcs_index()
{
  std::uniform_int_distribution<unsigned> dist(0, 31);
  return dist(gen);
}

static pusch_mcs_table generate_mcs_table()
{
  std::uniform_int_distribution<unsigned> dist(0, 4);
  return static_cast<pusch_mcs_table>(dist(gen));
}

static unsigned generate_num_layers()
{
  std::uniform_int_distribution<unsigned> dist(1, 4);
  return dist(gen);
}

static dmrs_config_type generate_dmrs_type()
{
  std::uniform_int_distribution<unsigned> dist(0, 1);
  return static_cast<dmrs_config_type>(dist(gen));
}

static unsigned generate_pusch_dmrs_identity()
{
  std::uniform_int_distribution<unsigned> dist(0, 1007);
  return dist(gen);
}

static unsigned generate_num_dmrs_cdm_no_data()
{
  std::uniform_int_distribution<unsigned> dist(1, 3);
  return dist(gen);
}

static unsigned generate_tx_direct_current_location()
{
  std::uniform_int_distribution<unsigned> dist(0, 4095);
  return dist(gen);
}

static ldpc_base_graph_type generate_ldpc_graph_type()
{
  std::uniform_int_distribution<unsigned> dist(1, 2);
  return static_cast<ldpc_base_graph_type>(dist(gen));
}

ul_pusch_pdu unittest::build_valid_ul_pusch_pdu()
{
  ul_pusch_pdu pdu;

  pdu.rnti             = generate_rnti();
  pdu.handle           = generate_handle();
  pdu.bwp              = generate_crb_interval();
  pdu.scs              = generate_scs();
  pdu.cp               = generate_cyclic_prefix();
  pdu.target_code_rate = 1982U;

  bool transform_precoding = generate_bool();
  if (transform_precoding) {
    auto& tp_enabled               = pdu.transform_precoding.emplace<ul_pusch_pdu::transform_precoding_enabled>();
    tp_enabled.pusch_dmrs_identity = generate_pusch_dmrs_identity();
  } else {
    auto& tp_disabled = pdu.transform_precoding.emplace<ul_pusch_pdu::transform_precoding_disabled>();
    tp_disabled.num_dmrs_cdm_grps_no_data = generate_num_dmrs_cdm_no_data();
  }

  pdu.qam_mod_order    = generate_qam_mod_order(transform_precoding);
  pdu.mcs_index        = generate_mcs_index();
  pdu.mcs_table        = generate_mcs_table();
  pdu.nid_pusch        = generate_nid_pucch_hopping();
  pdu.num_layers       = generate_num_layers();
  pdu.ul_dmrs_symb_pos = dmrs_symbol_mask(13);
  pdu.ul_dmrs_symb_pos.from_uint64(3);
  pdu.dmrs_type                = generate_dmrs_type();
  pdu.pusch_dmrs_scrambling_id = 32421;
  pdu.nscid                    = generate_bool();
  pdu.dmrs_ports               = dmrs_ports_mask(11);
  pdu.dmrs_ports.from_uint64(4);
  pdu.resource_allocation_1.vrbs = generate_vrb_interval();
  pdu.tx_direct_current_location = generate_tx_direct_current_location();
  pdu.symbols                    = generate_symbols();

  pdu.pusch_data = std::make_optional(ul_pusch_data{
      .rv_index = 2, .harq_process_id = to_harq_id(2), .new_data = false, .tb_size = units::bytes{213131}});

  pdu.pusch_uci = std::make_optional(ul_pusch_uci{.harq_ack_bit         = units::bits(3),
                                                  .csi_part1_bit        = units::bits(4),
                                                  .alpha_scaling        = alpha_scaling_opt::f0p5,
                                                  .beta_offset_harq_ack = 12,
                                                  .beta_offset_csi1     = 16,
                                                  .beta_offset_csi2     = 17});

  // Add 1 part1 to part2 correspondence.
  pdu.uci_correspondence = std::make_optional(uci_part1_to_part2_correspondence());
  pdu.uci_correspondence->part2.emplace_back();
  auto& corr                = pdu.uci_correspondence->part2.back();
  corr.param_offsets        = {1, 2};
  corr.param_sizes          = {1, 2};
  corr.part2_size_map_index = 0;

  pdu.ldpc_base_graph    = generate_ldpc_graph_type();
  pdu.tb_size_lbrm_bytes = units::bytes{32323242};

  return pdu;
}

ul_srs_pdu unittest::build_valid_ul_srs_pdu()
{
  ul_srs_pdu pdu;
  pdu.rnti                               = to_rnti(23);
  pdu.handle                             = 8;
  pdu.bwp                                = {230, 10};
  pdu.scs                                = subcarrier_spacing::kHz240;
  pdu.cp                                 = cyclic_prefix::NORMAL;
  pdu.num_ant_ports                      = srs_resource_configuration::one_two_four_enum::two;
  pdu.ofdm_symbols                       = ofdm_symbol_range();
  pdu.num_repetitions                    = n1;
  pdu.time_start_position                = 3;
  pdu.config_index                       = 4;
  pdu.sequence_id                        = 6;
  pdu.bandwidth_index                    = 1;
  pdu.comb_size                          = tx_comb_size::n2;
  pdu.comb_offset                        = 1;
  pdu.cyclic_shift                       = 0;
  pdu.frequency_position                 = 3;
  pdu.frequency_shift                    = 10;
  pdu.frequency_hopping                  = 2;
  pdu.group_or_sequence_hopping          = srs_group_or_sequence_hopping::neither;
  pdu.resource_type                      = srs_resource_type::periodic;
  pdu.t_srs                              = srs_periodicity::sl4;
  pdu.t_offset                           = 2;
  pdu.enable_normalized_iq_matrix_report = false;
  pdu.enable_positioning_report          = false;

  return pdu;
}

ul_tti_request unittest::build_valid_ul_tti_request()
{
  ul_tti_request msg;

  auto     scs        = subcarrier_spacing::kHz240;
  unsigned sfn        = generate_sfn();
  auto     slot_index = generate_slot();
  msg.slot            = slot_point(scs, sfn, slot_index);

  {
    auto& pdu       = msg.pdus.emplace_back();
    auto& prach_pdu = pdu.pdu.emplace<ul_prach_pdu>();
    prach_pdu       = build_valid_ul_prach_pdu();
  }
  {
    auto& pdu          = msg.pdus.emplace_back();
    auto& pucch_f0_pdu = pdu.pdu.emplace<ul_pucch_pdu>();
    pucch_f0_pdu       = build_valid_ul_pucch_f0_pdu();
  }
  {
    auto& pdu          = msg.pdus.emplace_back();
    auto& pucch_f3_pdu = pdu.pdu.emplace<ul_pucch_pdu>();
    pucch_f3_pdu       = build_valid_ul_pucch_f3_pdu();
  }
  {
    auto& pdu       = msg.pdus.emplace_back();
    auto& pusch_pdu = pdu.pdu.emplace<ul_pusch_pdu>();
    pusch_pdu       = build_valid_ul_pusch_pdu();
  }

  return msg;
}

ocudu::fapi::tx_precoding_and_beamforming_pdu unittest::build_valid_tx_precoding_and_beamforming_pdu()
{
  ocudu::fapi::tx_precoding_and_beamforming_pdu pdu;

  pdu.prg_size = 1U;
  pdu.prg      = {3U};

  return pdu;
}
