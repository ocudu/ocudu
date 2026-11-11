// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/pc5_rrc_definitions.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// AccessStratumReleaseSidelink-r16 ::= ENUMERATED
static const char* enum_names_access_stratum_release_sidelink_r16_e_values[] =
    {"rel16", "rel17", "rel18", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc access_stratum_release_sidelink_r16_opts::desc = {
    enum_names_access_stratum_release_sidelink_r16_e_values,
    8,
    "access_stratum_release_sidelink_r16_e"};
uint8_t access_stratum_release_sidelink_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {16, 17, 18};
  return map_enum_number(numbers, 3, value, "access_stratum_release_sidelink_r16_e");
}

// BandParametersSidelink-v1710 ::= SEQUENCE
static const asn1::table::seq_field seq_table_band_params_sidelink_v1710_s_fields[] = {
    {0,
     (int32_t)offsetof(band_params_sidelink_v1710_s, tx_iuc_scheme1_mode2_sidelink_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(band_params_sidelink_v1710_s, tx_iuc_scheme2_mode2_sidelink_r17),
     (int32_t)offsetof(band_params_sidelink_v1710_s, tx_iuc_scheme2_mode2_sidelink_r17_present),
     asn1::table::enum_op<decltype(band_params_sidelink_v1710_s::tx_iuc_scheme2_mode2_sidelink_r17)>(),
     decltype(band_params_sidelink_v1710_s::tx_iuc_scheme2_mode2_sidelink_r17)::has_ext,
     false,
     decltype(band_params_sidelink_v1710_s::tx_iuc_scheme2_mode2_sidelink_r17)::nof_types,
     decltype(band_params_sidelink_v1710_s::tx_iuc_scheme2_mode2_sidelink_r17)::nof_exts,
     nullptr,
     &band_params_sidelink_v1710_s::tx_iuc_scheme2_mode2_sidelink_r17_opts::desc}};
static const asn1::table::field_name seq_table_band_params_sidelink_v1710_s_names[] = {
    {"tx-IUC-Scheme1-Mode2Sidelink-r17", "supported"},
    {"tx-IUC-Scheme2-Mode2Sidelink-r17", nullptr}};
static const asn1::table::seq_opt seq_table_band_params_sidelink_v1710_s_opts[] = {
    {0, (int32_t)offsetof(band_params_sidelink_v1710_s, tx_iuc_scheme1_mode2_sidelink_r17_present)},
    {1, (int32_t)offsetof(band_params_sidelink_v1710_s, tx_iuc_scheme2_mode2_sidelink_r17_present)}};
const asn1::table::seq_desc band_params_sidelink_v1710_s::desc = {
    {seq_table_band_params_sidelink_v1710_s_fields, 2, UINT64_C(0x0), seq_table_band_params_sidelink_v1710_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_band_params_sidelink_v1710_s_names};

static const char* enum_names_band_params_sidelink_v1710_s_tx_iuc_scheme2_mode2_sidelink_r17_e__values[] = {"n4",
                                                                                                            "n8",
                                                                                                            "n16"};
const asn1::table::enum_desc band_params_sidelink_v1710_s::tx_iuc_scheme2_mode2_sidelink_r17_opts::desc  = {
    enum_names_band_params_sidelink_v1710_s_tx_iuc_scheme2_mode2_sidelink_r17_e__values,
    3,
    "band_params_sidelink_v1710_s::tx_iuc_scheme2_mode2_sidelink_r17_e_"};
uint8_t band_params_sidelink_v1710_s::tx_iuc_scheme2_mode2_sidelink_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 16};
  return map_enum_number(numbers, 3, value, "band_params_sidelink_v1710_s::tx_iuc_scheme2_mode2_sidelink_r17_e_");
}

// BandSidelinkPC5-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__fields[] = {
    {offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_, harq_rx_process_sidelink_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::harq_rx_process_sidelink_r16)>(),
     decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::harq_rx_process_sidelink_r16)::has_ext,
     false,
     decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::harq_rx_process_sidelink_r16)::nof_types,
     decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::harq_rx_process_sidelink_r16)::nof_exts,
     nullptr,
     &band_sidelink_pc5_r16_s::sl_reception_r16_s_::harq_rx_process_sidelink_r16_opts::desc},
    {offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_, pscch_rx_sidelink_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::pscch_rx_sidelink_r16)>(),
     decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::pscch_rx_sidelink_r16)::has_ext,
     false,
     decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::pscch_rx_sidelink_r16)::nof_types,
     decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::pscch_rx_sidelink_r16)::nof_exts,
     nullptr,
     &band_sidelink_pc5_r16_s::sl_reception_r16_s_::pscch_rx_sidelink_r16_opts::desc},
    {offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_, scs_cp_pattern_rx_sidelink_r16),
     (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_, scs_cp_pattern_rx_sidelink_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::desc},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_, extended_cp_rx_sidelink_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__names[] = {
    {"harq-RxProcessSidelink-r16", nullptr},
    {"pscch-RxSidelink-r16", nullptr},
    {"scs-CP-PatternRxSidelink-r16", nullptr},
    {"extendedCP-RxSidelink-r16", "supported"}};
static const asn1::table::seq_opt seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__opts[] = {
    {2, (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_, scs_cp_pattern_rx_sidelink_r16_present)},
    {3, (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_, extended_cp_rx_sidelink_r16_present)}};
static const asn1::table::seq_desc seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s_ = {
    {seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__fields,
     4,
     UINT64_C(0x3),
     seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__names};
static const asn1::table::seq_field seq_table_band_sidelink_pc5_r16_s_csi_report_sidelink_r16_s__fields[] = {
    {offsetof(band_sidelink_pc5_r16_s::csi_report_sidelink_r16_s_, csi_rs_ports_sidelink_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(band_sidelink_pc5_r16_s::csi_report_sidelink_r16_s_::csi_rs_ports_sidelink_r16)>(),
     decltype(band_sidelink_pc5_r16_s::csi_report_sidelink_r16_s_::csi_rs_ports_sidelink_r16)::has_ext,
     false,
     decltype(band_sidelink_pc5_r16_s::csi_report_sidelink_r16_s_::csi_rs_ports_sidelink_r16)::nof_types,
     decltype(band_sidelink_pc5_r16_s::csi_report_sidelink_r16_s_::csi_rs_ports_sidelink_r16)::nof_exts,
     nullptr,
     &band_sidelink_pc5_r16_s::csi_report_sidelink_r16_s_::csi_rs_ports_sidelink_r16_opts::desc}};
static const asn1::table::field_name seq_table_band_sidelink_pc5_r16_s_csi_report_sidelink_r16_s__names[] = {
    {"csi-RS-PortsSidelink-r16", nullptr}};
static const asn1::table::seq_desc seq_table_band_sidelink_pc5_r16_s_csi_report_sidelink_r16_s_ = {
    {seq_table_band_sidelink_pc5_r16_s_csi_report_sidelink_r16_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_band_sidelink_pc5_r16_s_csi_report_sidelink_r16_s__names};
static const asn1::table::seq_field seq_table_band_sidelink_pc5_r16_s_sl_ca_communication_r18_s__fields[] = {
    {offsetof(band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_, nof_carriers_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(2),
     UINT64_C(8),
     nullptr},
    {offsetof(band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_, nof_pscch_decode_value_z_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(2),
     nullptr},
    {offsetof(band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_, total_bw_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_::total_bw_r18)>(),
     decltype(band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_::total_bw_r18)::has_ext,
     false,
     decltype(band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_::total_bw_r18)::nof_types,
     decltype(band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_::total_bw_r18)::nof_exts,
     nullptr,
     &band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_::total_bw_r18_opts::desc}};
static const asn1::table::field_name seq_table_band_sidelink_pc5_r16_s_sl_ca_communication_r18_s__names[] = {
    {"numberOfCarriers-r18", nullptr},
    {"numberOfPSCCH-DecodeValueZ-r18", nullptr},
    {"totalBandwidth-r18", nullptr}};
static const asn1::table::seq_desc seq_table_band_sidelink_pc5_r16_s_sl_ca_communication_r18_s_ = {
    {seq_table_band_sidelink_pc5_r16_s_sl_ca_communication_r18_s__fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_band_sidelink_pc5_r16_s_sl_ca_communication_r18_s__names};
static const asn1::table::seq_field seq_table_band_sidelink_pc5_r16_s_fields[] = {
    {offsetof(band_sidelink_pc5_r16_s, freq_band_sidelink_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(1024),
     nullptr},
    {offsetof(band_sidelink_pc5_r16_s, sl_reception_r16),
     (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_reception_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s_},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_tx_256_qam_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, low_se_64_qam_mcs_table_sidelink_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_band_sidelink_pc5_r16_s_names[] = {
    {"freqBandSidelink-r16", nullptr},
    {"sl-Reception-r16", nullptr},
    {"sl-Tx-256QAM-r16", "supported"},
    {"lowSE-64QAM-MCS-TableSidelink-r16", "supported"}};
static const asn1::table::seq_opt seq_table_band_sidelink_pc5_r16_s_opts[] = {
    {1, (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_reception_r16_present)},
    {2, (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_tx_256_qam_r16_present)},
    {3, (int32_t)offsetof(band_sidelink_pc5_r16_s, low_se_64_qam_mcs_table_sidelink_r16_present)}};
static const asn1::table::seq_field seq_table_band_sidelink_pc5_r16_s_g0_fields[] = {
    {offsetof(band_sidelink_pc5_r16_s, csi_report_sidelink_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(band_sidelink_pc5_r16_s::csi_report_sidelink_r16)>::value,
     &seq_table_band_sidelink_pc5_r16_s_csi_report_sidelink_r16_s_},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, rank_two_reception_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_open_loop_pc_rsrp_report_sidelink_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_rx_256_qam_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_band_sidelink_pc5_r16_s_g0_names[] = {
    {"csi-ReportSidelink-r16", nullptr},
    {"rankTwoReception-r16", "supported"},
    {"sl-openLoopPC-RSRP-ReportSidelink-r16", "supported"},
    {"sl-Rx-256QAM-r16", "supported"}};
static const asn1::table::seq_opt seq_table_band_sidelink_pc5_r16_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(band_sidelink_pc5_r16_s, rank_two_reception_r16_present)},
    {2, (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_open_loop_pc_rsrp_report_sidelink_r16_present)},
    {3, (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_rx_256_qam_r16_present)}};
static const asn1::table::seq_field seq_table_band_sidelink_pc5_r16_s_g1_fields[] = {
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, rx_iuc_scheme1_preferred_mode2_sidelink_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, rx_iuc_scheme1_non_preferred_mode2_sidelink_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(band_sidelink_pc5_r16_s, rx_iuc_scheme2_mode2_sidelink_r17),
     (int32_t)offsetof(band_sidelink_pc5_r16_s, rx_iuc_scheme2_mode2_sidelink_r17_present),
     asn1::table::enum_op<decltype(band_sidelink_pc5_r16_s::rx_iuc_scheme2_mode2_sidelink_r17)>(),
     decltype(band_sidelink_pc5_r16_s::rx_iuc_scheme2_mode2_sidelink_r17)::has_ext,
     false,
     decltype(band_sidelink_pc5_r16_s::rx_iuc_scheme2_mode2_sidelink_r17)::nof_types,
     decltype(band_sidelink_pc5_r16_s::rx_iuc_scheme2_mode2_sidelink_r17)::nof_exts,
     nullptr,
     &band_sidelink_pc5_r16_s::rx_iuc_scheme2_mode2_sidelink_r17_opts::desc},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, rx_iuc_scheme1_sci_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, rx_iuc_scheme1_sci_explicit_req_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, scheme2_conflict_determination_rsrp_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_band_sidelink_pc5_r16_s_g1_names[] = {
    {"rx-IUC-Scheme1-PreferredMode2Sidelink-r17", "supported"},
    {"rx-IUC-Scheme1-NonPreferredMode2Sidelink-r17", "supported"},
    {"rx-IUC-Scheme2-Mode2Sidelink-r17", nullptr},
    {"rx-IUC-Scheme1-SCI-r17", "supported"},
    {"rx-IUC-Scheme1-SCI-ExplicitReq-r17", "supported"},
    {"scheme2-ConflictDeterminationRSRP-r17", "supported"}};
static const asn1::table::seq_opt seq_table_band_sidelink_pc5_r16_s_g1_opts[] = {
    {0, (int32_t)offsetof(band_sidelink_pc5_r16_s, rx_iuc_scheme1_preferred_mode2_sidelink_r17_present)},
    {1, (int32_t)offsetof(band_sidelink_pc5_r16_s, rx_iuc_scheme1_non_preferred_mode2_sidelink_r17_present)},
    {2, (int32_t)offsetof(band_sidelink_pc5_r16_s, rx_iuc_scheme2_mode2_sidelink_r17_present)},
    {3, (int32_t)offsetof(band_sidelink_pc5_r16_s, rx_iuc_scheme1_sci_r17_present)},
    {4, (int32_t)offsetof(band_sidelink_pc5_r16_s, rx_iuc_scheme1_sci_explicit_req_r17_present)},
    {5, (int32_t)offsetof(band_sidelink_pc5_r16_s, scheme2_conflict_determination_rsrp_r17_present)}};
static const asn1::table::seq_field seq_table_band_sidelink_pc5_r16_s_g2_fields[] = {
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_pathloss_based_olpc_sl_rsrp_report_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_ue_cot_sharing_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_psfch_multi_contiguous_rb_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_psfch_multi_non_contiguous_rb_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(band_sidelink_pc5_r16_s, sl_ca_communication_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(band_sidelink_pc5_r16_s::sl_ca_communication_r18)>::value,
     &seq_table_band_sidelink_pc5_r16_s_sl_ca_communication_r18_s_},
    {0,
     (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_reception_intra_carrier_guard_band_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(band_sidelink_pc5_r16_s, sl_pwr_class_unlicensed_r18),
     (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_pwr_class_unlicensed_r18_present),
     asn1::table::enum_op<decltype(band_sidelink_pc5_r16_s::sl_pwr_class_unlicensed_r18)>(),
     decltype(band_sidelink_pc5_r16_s::sl_pwr_class_unlicensed_r18)::has_ext,
     false,
     decltype(band_sidelink_pc5_r16_s::sl_pwr_class_unlicensed_r18)::nof_types,
     decltype(band_sidelink_pc5_r16_s::sl_pwr_class_unlicensed_r18)::nof_exts,
     nullptr,
     &band_sidelink_pc5_r16_s::sl_pwr_class_unlicensed_r18_opts::desc}};
static const asn1::table::field_name seq_table_band_sidelink_pc5_r16_s_g2_names[] = {
    {"sl-PathlossBasedOLPC-SL-RSRP-Report-r18", "supported"},
    {"sl-UE-COT-Sharing-r18", "supported"},
    {"sl-PSFCH-MultiContiguousRB-r18", "supported"},
    {"sl-PSFCH-MultiNonContiguousRB-r18", "supported"},
    {"sl-CA-Communication-r18", nullptr},
    {"sl-ReceptionIntraCarrierGuardBand-r18", "supported"},
    {"sl-PowerClassUnlicensed-r18", nullptr}};
static const asn1::table::seq_opt seq_table_band_sidelink_pc5_r16_s_g2_opts[] = {
    {0, (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_pathloss_based_olpc_sl_rsrp_report_r18_present)},
    {1, (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_ue_cot_sharing_r18_present)},
    {2, (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_psfch_multi_contiguous_rb_r18_present)},
    {3, (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_psfch_multi_non_contiguous_rb_r18_present)},
    {4, asn1::table::seq_field::ptr_presence},
    {5, (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_reception_intra_carrier_guard_band_r18_present)},
    {6, (int32_t)offsetof(band_sidelink_pc5_r16_s, sl_pwr_class_unlicensed_r18_present)}};
static const asn1::table::seq_group seq_table_band_sidelink_pc5_r16_s_groups[] = {
    {{seq_table_band_sidelink_pc5_r16_s_g0_fields, 4, UINT64_C(0x0), seq_table_band_sidelink_pc5_r16_s_g0_opts, 4},
     false,
     seq_table_band_sidelink_pc5_r16_s_g0_names},
    {{seq_table_band_sidelink_pc5_r16_s_g1_fields, 6, UINT64_C(0x0), seq_table_band_sidelink_pc5_r16_s_g1_opts, 6},
     false,
     seq_table_band_sidelink_pc5_r16_s_g1_names},
    {{seq_table_band_sidelink_pc5_r16_s_g2_fields, 7, UINT64_C(0x0), seq_table_band_sidelink_pc5_r16_s_g2_opts, 7},
     false,
     seq_table_band_sidelink_pc5_r16_s_g2_names}};
const asn1::table::seq_desc band_sidelink_pc5_r16_s::desc = {
    {seq_table_band_sidelink_pc5_r16_s_fields, 4, UINT64_C(0x1), seq_table_band_sidelink_pc5_r16_s_opts, 3},
    true,
    false,
    seq_table_band_sidelink_pc5_r16_s_groups,
    3,
    seq_table_band_sidelink_pc5_r16_s_names};

static const char* enum_names_band_sidelink_pc5_r16_s_sl_reception_r16_s__harq_rx_process_sidelink_r16_e__values[] =
    {"n16", "n24", "n32", "n64"};
const asn1::table::enum_desc band_sidelink_pc5_r16_s::sl_reception_r16_s_::harq_rx_process_sidelink_r16_opts::desc = {
    enum_names_band_sidelink_pc5_r16_s_sl_reception_r16_s__harq_rx_process_sidelink_r16_e__values,
    4,
    "band_sidelink_pc5_r16_s::sl_reception_r16_s_::harq_rx_process_sidelink_r16_e_"};
uint8_t band_sidelink_pc5_r16_s::sl_reception_r16_s_::harq_rx_process_sidelink_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {16, 24, 32, 64};
  return map_enum_number(
      numbers, 4, value, "band_sidelink_pc5_r16_s::sl_reception_r16_s_::harq_rx_process_sidelink_r16_e_");
}

static const char* enum_names_band_sidelink_pc5_r16_s_sl_reception_r16_s__pscch_rx_sidelink_r16_e__values[] = {
    "value1",
    "value2"};
const asn1::table::enum_desc band_sidelink_pc5_r16_s::sl_reception_r16_s_::pscch_rx_sidelink_r16_opts::desc = {
    enum_names_band_sidelink_pc5_r16_s_sl_reception_r16_s__pscch_rx_sidelink_r16_e__values,
    2,
    "band_sidelink_pc5_r16_s::sl_reception_r16_s_::pscch_rx_sidelink_r16_e_"};
uint8_t band_sidelink_pc5_r16_s::sl_reception_r16_s_::pscch_rx_sidelink_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "band_sidelink_pc5_r16_s::sl_reception_r16_s_::pscch_rx_sidelink_r16_e_");
}

static const asn1::table::storage_ops* const
    choice_storage_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__alternatives[] = {
        &asn1::table::alternative_ops<
            band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_,
            false>::value,
        &asn1::table::alternative_ops<
            band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr2_r16_s_,
            false>::value};
const asn1::table::choice_storage_desc
    band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::storage_desc = {
        choice_storage_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__alternatives,
        2,
        "band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_"};

band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_&
band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::set_fr1_r16()
{
  set(types::fr1_r16);
  return c.get<fr1_r16_s_>();
}
band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr2_r16_s_&
band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::set_fr2_r16()
{
  set(types::fr2_r16);
  return c.get<fr2_r16_s_>();
}
static const asn1::table::seq_field
    seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr1_r16_s__fields[] = {
        {offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_,
                  scs_15k_hz_r16),
         (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_,
                           scs_15k_hz_r16_present),
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::
                                                 scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_::scs_15k_hz_r16),
                                    asn1::table::struct_json>::value},
        {offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_,
                  scs_30k_hz_r16),
         (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_,
                           scs_30k_hz_r16_present),
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::
                                                 scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_::scs_30k_hz_r16),
                                    asn1::table::struct_json>::value},
        {offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_,
                  scs_60k_hz_r16),
         (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_,
                           scs_60k_hz_r16_present),
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::
                                                 scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_::scs_60k_hz_r16),
                                    asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr1_r16_s__names[] = {
        {"scs-15kHz-r16", nullptr},
        {"scs-30kHz-r16", nullptr},
        {"scs-60kHz-r16", nullptr}};
static const asn1::table::seq_opt
    seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr1_r16_s__opts[] = {
        {0,
         (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_,
                           scs_15k_hz_r16_present)},
        {1,
         (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_,
                           scs_30k_hz_r16_present)},
        {2,
         (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr1_r16_s_,
                           scs_60k_hz_r16_present)}};
static const asn1::table::seq_desc
    seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr1_r16_s_ = {
        {seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr1_r16_s__fields,
         3,
         UINT64_C(0x0),
         seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr1_r16_s__opts,
         3},
        false,
        false,
        nullptr,
        0,
        seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr1_r16_s__names};
static const asn1::table::seq_field
    seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr2_r16_s__fields[] = {
        {offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr2_r16_s_,
                  scs_60k_hz_r16),
         (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr2_r16_s_,
                           scs_60k_hz_r16_present),
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::
                                                 scs_cp_pattern_rx_sidelink_r16_c_::fr2_r16_s_::scs_60k_hz_r16),
                                    asn1::table::struct_json>::value},
        {offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr2_r16_s_,
                  scs_120k_hz_r16),
         (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr2_r16_s_,
                           scs_120k_hz_r16_present),
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(band_sidelink_pc5_r16_s::sl_reception_r16_s_::
                                                 scs_cp_pattern_rx_sidelink_r16_c_::fr2_r16_s_::scs_120k_hz_r16),
                                    asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr2_r16_s__names[] = {
        {"scs-60kHz-r16", nullptr},
        {"scs-120kHz-r16", nullptr}};
static const asn1::table::seq_opt
    seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr2_r16_s__opts[] = {
        {0,
         (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr2_r16_s_,
                           scs_60k_hz_r16_present)},
        {1,
         (int32_t)offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::fr2_r16_s_,
                           scs_120k_hz_r16_present)}};
static const asn1::table::seq_desc
    seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr2_r16_s_ = {
        {seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr2_r16_s__fields,
         2,
         UINT64_C(0x0),
         seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr2_r16_s__opts,
         2},
        false,
        false,
        nullptr,
        0,
        seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr2_r16_s__names};
static const asn1::table::seq_field
    choice_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr1_r16_s_},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &seq_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__fr2_r16_s_}};
static const asn1::table::field_name
    choice_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__names[] = {
        {"fr1-r16", nullptr},
        {"fr2-r16", nullptr}};
const asn1::table::choice_desc band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::desc = {
    asn1::table::enum_int_type<
        band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types>(),
    band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types::nof_types,
    band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types::nof_exts,
    band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types::has_ext,
    false,
    choice_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__alternatives,
    nullptr,
    choice_table_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__names,
    "band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_",
    offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_, c.type),
    offsetof(band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_, c.value),
    false,
    &band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::storage_desc};

static const char*
    enum_names_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__types_values[] = {
        "fr1-r16",
        "fr2-r16"};
const asn1::table::enum_desc
    band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types_opts::desc = {
        enum_names_band_sidelink_pc5_r16_s_sl_reception_r16_s__scs_cp_pattern_rx_sidelink_r16_c__types_values,
        2,
        "band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types"};
uint8_t band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers, 2, value, "band_sidelink_pc5_r16_s::sl_reception_r16_s_::scs_cp_pattern_rx_sidelink_r16_c_::types");
}

static const char* enum_names_band_sidelink_pc5_r16_s_csi_report_sidelink_r16_s__csi_rs_ports_sidelink_r16_e__values[] =
    {"p1", "p2"};
const asn1::table::enum_desc band_sidelink_pc5_r16_s::csi_report_sidelink_r16_s_::csi_rs_ports_sidelink_r16_opts::desc =
    {enum_names_band_sidelink_pc5_r16_s_csi_report_sidelink_r16_s__csi_rs_ports_sidelink_r16_e__values,
     2,
     "band_sidelink_pc5_r16_s::csi_report_sidelink_r16_s_::csi_rs_ports_sidelink_r16_e_"};
uint8_t band_sidelink_pc5_r16_s::csi_report_sidelink_r16_s_::csi_rs_ports_sidelink_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers, 2, value, "band_sidelink_pc5_r16_s::csi_report_sidelink_r16_s_::csi_rs_ports_sidelink_r16_e_");
}

static const char* enum_names_band_sidelink_pc5_r16_s_rx_iuc_scheme2_mode2_sidelink_r17_e__values[] =
    {"n5", "n15", "n25", "n32", "n35", "n45", "n50", "n64"};
const asn1::table::enum_desc band_sidelink_pc5_r16_s::rx_iuc_scheme2_mode2_sidelink_r17_opts::desc = {
    enum_names_band_sidelink_pc5_r16_s_rx_iuc_scheme2_mode2_sidelink_r17_e__values,
    8,
    "band_sidelink_pc5_r16_s::rx_iuc_scheme2_mode2_sidelink_r17_e_"};
uint8_t band_sidelink_pc5_r16_s::rx_iuc_scheme2_mode2_sidelink_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 15, 25, 32, 35, 45, 50, 64};
  return map_enum_number(numbers, 8, value, "band_sidelink_pc5_r16_s::rx_iuc_scheme2_mode2_sidelink_r17_e_");
}

static const char* enum_names_band_sidelink_pc5_r16_s_sl_ca_communication_r18_s__total_bw_r18_e__values[] =
    {"mhz20", "mhz30", "mhz40", "mhz50", "mhz60", "mhz70"};
const asn1::table::enum_desc band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_::total_bw_r18_opts::desc = {
    enum_names_band_sidelink_pc5_r16_s_sl_ca_communication_r18_s__total_bw_r18_e__values,
    6,
    "band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_::total_bw_r18_e_"};
uint8_t band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_::total_bw_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {20, 30, 40, 50, 60, 70};
  return map_enum_number(numbers, 6, value, "band_sidelink_pc5_r16_s::sl_ca_communication_r18_s_::total_bw_r18_e_");
}

static const char* enum_names_band_sidelink_pc5_r16_s_sl_pwr_class_unlicensed_r18_e__values[] =
    {"pc5", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc band_sidelink_pc5_r16_s::sl_pwr_class_unlicensed_r18_opts::desc = {
    enum_names_band_sidelink_pc5_r16_s_sl_pwr_class_unlicensed_r18_e__values,
    8,
    "band_sidelink_pc5_r16_s::sl_pwr_class_unlicensed_r18_e_"};
uint8_t band_sidelink_pc5_r16_s::sl_pwr_class_unlicensed_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {5};
  return map_enum_number(numbers, 1, value, "band_sidelink_pc5_r16_s::sl_pwr_class_unlicensed_r18_e_");
}

// MAC-ParametersSidelink-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mac_params_sidelink_r17_s_fields[] = {
    {0,
     (int32_t)offsetof(mac_params_sidelink_r17_s, drx_on_sidelink_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_mac_params_sidelink_r17_s_names[] = {
    {"drx-OnSidelink-r17", "supported"}};
static const asn1::table::seq_opt seq_table_mac_params_sidelink_r17_s_opts[] = {
    {0, (int32_t)offsetof(mac_params_sidelink_r17_s, drx_on_sidelink_r17_present)}};
const asn1::table::seq_desc mac_params_sidelink_r17_s::desc = {
    {seq_table_mac_params_sidelink_r17_s_fields, 1, UINT64_C(0x0), seq_table_mac_params_sidelink_r17_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_mac_params_sidelink_r17_s_names};

// MasterInformationBlockSidelink ::= SEQUENCE
static const asn1::table::seq_field seq_table_mib_sidelink_s_fields[] = {
    {offsetof(mib_sidelink_s, sl_tdd_cfg_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mib_sidelink_s::sl_tdd_cfg_r16), asn1::table::struct_json>::value},
    {offsetof(mib_sidelink_s, in_coverage_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(mib_sidelink_s, direct_frame_num_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mib_sidelink_s::direct_frame_num_r16), asn1::table::struct_json>::value},
    {offsetof(mib_sidelink_s, slot_idx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mib_sidelink_s::slot_idx_r16), asn1::table::struct_json>::value},
    {offsetof(mib_sidelink_s, reserved_bits_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mib_sidelink_s::reserved_bits_r16), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_mib_sidelink_s_names[] = {{"sl-TDD-Config-r16", nullptr},
                                                                         {"inCoverage-r16", nullptr},
                                                                         {"directFrameNumber-r16", nullptr},
                                                                         {"slotIndex-r16", nullptr},
                                                                         {"reservedBits-r16", nullptr}};
const asn1::table::seq_desc mib_sidelink_s::desc = {{seq_table_mib_sidelink_s_fields, 5, UINT64_C(0x1f), nullptr, 0},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0,
                                                    seq_table_mib_sidelink_s_names};

// SL-MeasQuantityResult-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_meas_quant_result_r16_s_fields[] = {
    {offsetof(sl_meas_quant_result_r16_s, sl_rsrp_r16),
     (int32_t)offsetof(sl_meas_quant_result_r16_s, sl_rsrp_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr}};
static const asn1::table::field_name seq_table_sl_meas_quant_result_r16_s_names[] = {{"sl-RSRP-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_meas_quant_result_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_meas_quant_result_r16_s, sl_rsrp_r16_present)}};
static const asn1::table::seq_field seq_table_sl_meas_quant_result_r16_s_g0_fields[] = {
    {offsetof(sl_meas_quant_result_r16_s, sl_rsrp_ded_sl_prs_rp_r18),
     (int32_t)offsetof(sl_meas_quant_result_r16_s, sl_rsrp_ded_sl_prs_rp_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(13),
     nullptr}};
static const asn1::table::field_name seq_table_sl_meas_quant_result_r16_s_g0_names[] = {
    {"sl-RSRP-DedicatedSL-PRS-RP-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_meas_quant_result_r16_s_g0_opts[] = {
    {0, (int32_t)offsetof(sl_meas_quant_result_r16_s, sl_rsrp_ded_sl_prs_rp_r18_present)}};
static const asn1::table::seq_group seq_table_sl_meas_quant_result_r16_s_groups[] = {
    {{seq_table_sl_meas_quant_result_r16_s_g0_fields,
      1,
      UINT64_C(0x0),
      seq_table_sl_meas_quant_result_r16_s_g0_opts,
      1},
     false,
     seq_table_sl_meas_quant_result_r16_s_g0_names}};
const asn1::table::seq_desc sl_meas_quant_result_r16_s::desc = {
    {seq_table_sl_meas_quant_result_r16_s_fields, 1, UINT64_C(0x0), seq_table_sl_meas_quant_result_r16_s_opts, 1},
    true,
    false,
    seq_table_sl_meas_quant_result_r16_s_groups,
    1,
    seq_table_sl_meas_quant_result_r16_s_names};

// SL-MeasResult-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_meas_result_r16_s_fields[] = {
    {offsetof(sl_meas_result_r16_s, sl_result_dmrs_r16),
     (int32_t)offsetof(sl_meas_result_r16_s, sl_result_dmrs_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_quant_result_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_meas_result_r16_s_names[] = {{"sl-ResultDMRS-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_meas_result_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_meas_result_r16_s, sl_result_dmrs_r16_present)}};
static const asn1::table::seq_field seq_table_sl_meas_result_r16_s_g0_fields[] = {
    {offsetof(sl_meas_result_r16_s, sl_result_sl_prs_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sl_meas_result_r16_s::sl_result_sl_prs_r18)>::value,
     &sl_meas_quant_result_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_meas_result_r16_s_g0_names[] = {{"sl-Result-SL-PRS-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_meas_result_r16_s_g0_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sl_meas_result_r16_s_groups[] = {
    {{seq_table_sl_meas_result_r16_s_g0_fields, 1, UINT64_C(0x0), seq_table_sl_meas_result_r16_s_g0_opts, 1},
     false,
     seq_table_sl_meas_result_r16_s_g0_names}};
const asn1::table::seq_desc sl_meas_result_r16_s::desc = {
    {seq_table_sl_meas_result_r16_s_fields, 1, UINT64_C(0x0), seq_table_sl_meas_result_r16_s_opts, 1},
    true,
    false,
    seq_table_sl_meas_result_r16_s_groups,
    1,
    seq_table_sl_meas_result_r16_s_names};

// SL-MeasResults-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_meas_results_r16_s_fields[] = {
    {offsetof(sl_meas_results_r16_s, sl_meas_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(64),
     nullptr},
    {offsetof(sl_meas_results_r16_s, sl_meas_result_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_result_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_meas_results_r16_s_names[] = {{"sl-MeasId-r16", nullptr},
                                                                                {"sl-MeasResult-r16", nullptr}};
const asn1::table::seq_desc          sl_meas_results_r16_s::desc             = {
    {seq_table_sl_meas_results_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_meas_results_r16_s_names};

// MeasurementReportSidelink-r16-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_report_sidelink_r16_ies_s_fields[] = {
    {offsetof(meas_report_sidelink_r16_ies_s, sl_meas_results_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_results_r16_s::desc},
    {offsetof(meas_report_sidelink_r16_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(meas_report_sidelink_r16_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(meas_report_sidelink_r16_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_meas_report_sidelink_r16_ies_s_names[] = {
    {"sl-MeasResults-r16", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_meas_report_sidelink_r16_ies_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(meas_report_sidelink_r16_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc meas_report_sidelink_r16_ies_s::desc = {{seq_table_meas_report_sidelink_r16_ies_s_fields,
                                                                     3,
                                                                     UINT64_C(0x1),
                                                                     seq_table_meas_report_sidelink_r16_ies_s_opts,
                                                                     2},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_meas_report_sidelink_r16_ies_s_names};

// MeasurementReportSidelink ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_report_sidelink_s_fields[] = {
    {offsetof(meas_report_sidelink_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_report_sidelink_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_meas_report_sidelink_s_names[] = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          meas_report_sidelink_s::desc             = {
    {seq_table_meas_report_sidelink_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_report_sidelink_s_names};

void meas_report_sidelink_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
meas_report_sidelink_r16_ies_s& meas_report_sidelink_s::crit_exts_c_::set_meas_report_sidelink_r16()
{
  set(types::meas_report_sidelink_r16);
  return c;
}
void meas_report_sidelink_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_meas_report_sidelink_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_report_sidelink_r16_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_meas_report_sidelink_s_crit_exts_c__names[] = {
    {"measurementReportSidelink-r16", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc meas_report_sidelink_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<meas_report_sidelink_s::crit_exts_c_::types>(),
    meas_report_sidelink_s::crit_exts_c_::types::nof_types,
    meas_report_sidelink_s::crit_exts_c_::types::nof_exts,
    meas_report_sidelink_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_meas_report_sidelink_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<meas_report_sidelink_s::crit_exts_c_>,
    choice_table_meas_report_sidelink_s_crit_exts_c__names,
    "meas_report_sidelink_s::crit_exts_c_",
    offsetof(meas_report_sidelink_s::crit_exts_c_, type_),
    offsetof(meas_report_sidelink_s::crit_exts_c_, c),
    false};

static const char* enum_names_meas_report_sidelink_s_crit_exts_c__types_values[]    = {"measurementReportSidelink-r16",
                                                                                       "criticalExtensionsFuture"};
const asn1::table::enum_desc meas_report_sidelink_s::crit_exts_c_::types_opts::desc = {
    enum_names_meas_report_sidelink_s_crit_exts_c__types_values,
    2,
    "meas_report_sidelink_s::crit_exts_c_::types"};

// NotificationMessageSidelink-v1800-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_notif_msg_sidelink_v1800_ies_s_fields[] = {
    {offsetof(notif_msg_sidelink_v1800_ies_s, sl_ind_type_r18),
     (int32_t)offsetof(notif_msg_sidelink_v1800_ies_s, sl_ind_type_r18_present),
     asn1::table::enum_op<decltype(notif_msg_sidelink_v1800_ies_s::sl_ind_type_r18)>(),
     decltype(notif_msg_sidelink_v1800_ies_s::sl_ind_type_r18)::has_ext,
     false,
     decltype(notif_msg_sidelink_v1800_ies_s::sl_ind_type_r18)::nof_types,
     decltype(notif_msg_sidelink_v1800_ies_s::sl_ind_type_r18)::nof_exts,
     nullptr,
     &notif_msg_sidelink_v1800_ies_s::sl_ind_type_r18_opts::desc},
    {offsetof(notif_msg_sidelink_v1800_ies_s, sl_dest_id_remote_ue_r18),
     (int32_t)offsetof(notif_msg_sidelink_v1800_ies_s, sl_dest_id_remote_ue_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(notif_msg_sidelink_v1800_ies_s::sl_dest_id_remote_ue_r18),
                                asn1::table::struct_json>::value},
    {0,
     (int32_t)offsetof(notif_msg_sidelink_v1800_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_notif_msg_sidelink_v1800_ies_s_names[] = {
    {"sl-IndicationType-r18", nullptr},
    {"sl-DestinationIdentityRemoteUE-r18", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_notif_msg_sidelink_v1800_ies_s_opts[] = {
    {0, (int32_t)offsetof(notif_msg_sidelink_v1800_ies_s, sl_ind_type_r18_present)},
    {1, (int32_t)offsetof(notif_msg_sidelink_v1800_ies_s, sl_dest_id_remote_ue_r18_present)},
    {2, (int32_t)offsetof(notif_msg_sidelink_v1800_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc notif_msg_sidelink_v1800_ies_s::desc = {{seq_table_notif_msg_sidelink_v1800_ies_s_fields,
                                                                     3,
                                                                     UINT64_C(0x0),
                                                                     seq_table_notif_msg_sidelink_v1800_ies_s_opts,
                                                                     3},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_notif_msg_sidelink_v1800_ies_s_names};

static const char*           enum_names_notif_msg_sidelink_v1800_ies_s_sl_ind_type_r18_e__values[] = {"relayUE-PC5-RLF",
                                                                                                      "spare1"};
const asn1::table::enum_desc notif_msg_sidelink_v1800_ies_s::sl_ind_type_r18_opts::desc            = {
    enum_names_notif_msg_sidelink_v1800_ies_s_sl_ind_type_r18_e__values,
    2,
    "notif_msg_sidelink_v1800_ies_s::sl_ind_type_r18_e_"};
uint8_t notif_msg_sidelink_v1800_ies_s::sl_ind_type_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {5};
  return map_enum_number(numbers, 1, value, "notif_msg_sidelink_v1800_ies_s::sl_ind_type_r18_e_");
}

// NotificationMessageSidelink-r17-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_notif_msg_sidelink_r17_ies_s_fields[] = {
    {offsetof(notif_msg_sidelink_r17_ies_s, ind_type_r17),
     (int32_t)offsetof(notif_msg_sidelink_r17_ies_s, ind_type_r17_present),
     asn1::table::enum_op<decltype(notif_msg_sidelink_r17_ies_s::ind_type_r17)>(),
     decltype(notif_msg_sidelink_r17_ies_s::ind_type_r17)::has_ext,
     false,
     decltype(notif_msg_sidelink_r17_ies_s::ind_type_r17)::nof_types,
     decltype(notif_msg_sidelink_r17_ies_s::ind_type_r17)::nof_exts,
     nullptr,
     &notif_msg_sidelink_r17_ies_s::ind_type_r17_opts::desc},
    {offsetof(notif_msg_sidelink_r17_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(notif_msg_sidelink_r17_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {offsetof(notif_msg_sidelink_r17_ies_s, non_crit_ext),
     (int32_t)offsetof(notif_msg_sidelink_r17_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &notif_msg_sidelink_v1800_ies_s::desc}};
static const asn1::table::field_name seq_table_notif_msg_sidelink_r17_ies_s_names[] = {
    {"indicationType-r17", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_notif_msg_sidelink_r17_ies_s_opts[] = {
    {0, (int32_t)offsetof(notif_msg_sidelink_r17_ies_s, ind_type_r17_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(notif_msg_sidelink_r17_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc notif_msg_sidelink_r17_ies_s::desc = {
    {seq_table_notif_msg_sidelink_r17_ies_s_fields, 3, UINT64_C(0x0), seq_table_notif_msg_sidelink_r17_ies_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_notif_msg_sidelink_r17_ies_s_names};

static const char*           enum_names_notif_msg_sidelink_r17_ies_s_ind_type_r17_e__values[] = {"relayUE-Uu-RLF",
                                                                                                 "relayUE-HO",
                                                                                                 "relayUE-CellReselection",
                                                                                                 "relayUE-Uu-RRC-Failure"};
const asn1::table::enum_desc notif_msg_sidelink_r17_ies_s::ind_type_r17_opts::desc            = {
    enum_names_notif_msg_sidelink_r17_ies_s_ind_type_r17_e__values,
    4,
    "notif_msg_sidelink_r17_ies_s::ind_type_r17_e_"};

// NotificationMessageSidelink-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_notif_msg_sidelink_r17_s_fields[] = {
    {offsetof(notif_msg_sidelink_r17_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &notif_msg_sidelink_r17_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_notif_msg_sidelink_r17_s_names[] = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          notif_msg_sidelink_r17_s::desc             = {
    {seq_table_notif_msg_sidelink_r17_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_notif_msg_sidelink_r17_s_names};

void notif_msg_sidelink_r17_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
notif_msg_sidelink_r17_ies_s& notif_msg_sidelink_r17_s::crit_exts_c_::set_notif_msg_sidelink_r17()
{
  set(types::notif_msg_sidelink_r17);
  return c;
}
void notif_msg_sidelink_r17_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_notif_msg_sidelink_r17_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &notif_msg_sidelink_r17_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_notif_msg_sidelink_r17_s_crit_exts_c__names[] = {
    {"notificationMessageSidelink-r17", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc notif_msg_sidelink_r17_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<notif_msg_sidelink_r17_s::crit_exts_c_::types>(),
    notif_msg_sidelink_r17_s::crit_exts_c_::types::nof_types,
    notif_msg_sidelink_r17_s::crit_exts_c_::types::nof_exts,
    notif_msg_sidelink_r17_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_notif_msg_sidelink_r17_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<notif_msg_sidelink_r17_s::crit_exts_c_>,
    choice_table_notif_msg_sidelink_r17_s_crit_exts_c__names,
    "notif_msg_sidelink_r17_s::crit_exts_c_",
    offsetof(notif_msg_sidelink_r17_s::crit_exts_c_, type_),
    offsetof(notif_msg_sidelink_r17_s::crit_exts_c_, c),
    false};

static const char* enum_names_notif_msg_sidelink_r17_s_crit_exts_c__types_values[] = {"notificationMessageSidelink-r17",
                                                                                      "criticalExtensionsFuture"};
const asn1::table::enum_desc notif_msg_sidelink_r17_s::crit_exts_c_::types_opts::desc = {
    enum_names_notif_msg_sidelink_r17_s_crit_exts_c__types_values,
    2,
    "notif_msg_sidelink_r17_s::crit_exts_c_::types"};

// PDCP-ParametersSidelink-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdcp_params_sidelink_r16_s_fields[] = {
    {0,
     (int32_t)offsetof(pdcp_params_sidelink_r16_s, out_of_order_delivery_sidelink_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdcp_params_sidelink_r16_s_names[] = {
    {"outOfOrderDeliverySidelink-r16", "supported"}};
static const asn1::table::seq_opt seq_table_pdcp_params_sidelink_r16_s_opts[] = {
    {0, (int32_t)offsetof(pdcp_params_sidelink_r16_s, out_of_order_delivery_sidelink_r16_present)}};
static const asn1::table::seq_field seq_table_pdcp_params_sidelink_r16_s_g0_fields[] = {
    {0,
     (int32_t)offsetof(pdcp_params_sidelink_r16_s, pdcp_dupl_srb_sidelink_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(pdcp_params_sidelink_r16_s, pdcp_dupl_drb_sidelink_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdcp_params_sidelink_r16_s_g0_names[] = {
    {"pdcp-DuplicationSRB-sidelink-r18", "supported"},
    {"pdcp-DuplicationDRB-sidelink-r18", "supported"}};
static const asn1::table::seq_opt seq_table_pdcp_params_sidelink_r16_s_g0_opts[] = {
    {0, (int32_t)offsetof(pdcp_params_sidelink_r16_s, pdcp_dupl_srb_sidelink_r18_present)},
    {1, (int32_t)offsetof(pdcp_params_sidelink_r16_s, pdcp_dupl_drb_sidelink_r18_present)}};
static const asn1::table::seq_group seq_table_pdcp_params_sidelink_r16_s_groups[] = {
    {{seq_table_pdcp_params_sidelink_r16_s_g0_fields,
      2,
      UINT64_C(0x0),
      seq_table_pdcp_params_sidelink_r16_s_g0_opts,
      2},
     false,
     seq_table_pdcp_params_sidelink_r16_s_g0_names}};
const asn1::table::seq_desc pdcp_params_sidelink_r16_s::desc = {
    {seq_table_pdcp_params_sidelink_r16_s_fields, 1, UINT64_C(0x0), seq_table_pdcp_params_sidelink_r16_s_opts, 1},
    true,
    false,
    seq_table_pdcp_params_sidelink_r16_s_groups,
    1,
    seq_table_pdcp_params_sidelink_r16_s_names};

// RRCReconfigurationCompleteSidelink-v1720-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_recfg_complete_sidelink_v1720_ies_s_fields[] = {
    {0,
     (int32_t)offsetof(rrc_recfg_complete_sidelink_v1720_ies_s, sl_drx_cfg_reject_v1720_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(rrc_recfg_complete_sidelink_v1720_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_rrc_recfg_complete_sidelink_v1720_ies_s_names[] = {
    {"sl-DRX-ConfigReject-v1720", "true"},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_rrc_recfg_complete_sidelink_v1720_ies_s_opts[] = {
    {0, (int32_t)offsetof(rrc_recfg_complete_sidelink_v1720_ies_s, sl_drx_cfg_reject_v1720_present)},
    {1, (int32_t)offsetof(rrc_recfg_complete_sidelink_v1720_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_recfg_complete_sidelink_v1720_ies_s::desc = {
    {seq_table_rrc_recfg_complete_sidelink_v1720_ies_s_fields,
     2,
     UINT64_C(0x0),
     seq_table_rrc_recfg_complete_sidelink_v1720_ies_s_opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_recfg_complete_sidelink_v1720_ies_s_names};

// RRCReconfigurationCompleteSidelink-v1710-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_recfg_complete_sidelink_v1710_ies_s_fields[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {offsetof(rrc_recfg_complete_sidelink_v1710_ies_s, non_crit_ext),
     (int32_t)offsetof(rrc_recfg_complete_sidelink_v1710_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_complete_sidelink_v1720_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_recfg_complete_sidelink_v1710_ies_s_names[] = {
    {"dummy", "true"},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_rrc_recfg_complete_sidelink_v1710_ies_s_opts[] = {
    {1, (int32_t)offsetof(rrc_recfg_complete_sidelink_v1710_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_recfg_complete_sidelink_v1710_ies_s::desc = {
    {seq_table_rrc_recfg_complete_sidelink_v1710_ies_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_rrc_recfg_complete_sidelink_v1710_ies_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_recfg_complete_sidelink_v1710_ies_s_names};

// RRCReconfigurationCompleteSidelink-r16-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_recfg_complete_sidelink_r16_ies_s_fields[] = {
    {offsetof(rrc_recfg_complete_sidelink_r16_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(rrc_recfg_complete_sidelink_r16_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {offsetof(rrc_recfg_complete_sidelink_r16_ies_s, non_crit_ext),
     (int32_t)offsetof(rrc_recfg_complete_sidelink_r16_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_complete_sidelink_v1710_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_recfg_complete_sidelink_r16_ies_s_names[] = {
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_rrc_recfg_complete_sidelink_r16_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(rrc_recfg_complete_sidelink_r16_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_recfg_complete_sidelink_r16_ies_s::desc = {
    {seq_table_rrc_recfg_complete_sidelink_r16_ies_s_fields,
     2,
     UINT64_C(0x0),
     seq_table_rrc_recfg_complete_sidelink_r16_ies_s_opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_recfg_complete_sidelink_r16_ies_s_names};

// RRCReconfigurationCompleteSidelink ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_recfg_complete_sidelink_s_fields[] = {
    {offsetof(rrc_recfg_complete_sidelink_s, rrc_transaction_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(rrc_recfg_complete_sidelink_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_complete_sidelink_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_rrc_recfg_complete_sidelink_s_names[] = {
    {"rrc-TransactionIdentifier-r16", nullptr},
    {"criticalExtensions", nullptr}};
const asn1::table::seq_desc rrc_recfg_complete_sidelink_s::desc = {
    {seq_table_rrc_recfg_complete_sidelink_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_recfg_complete_sidelink_s_names};

void rrc_recfg_complete_sidelink_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_recfg_complete_sidelink_r16_ies_s&
rrc_recfg_complete_sidelink_s::crit_exts_c_::set_rrc_recfg_complete_sidelink_r16()
{
  set(types::rrc_recfg_complete_sidelink_r16);
  return c;
}
void rrc_recfg_complete_sidelink_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_rrc_recfg_complete_sidelink_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_complete_sidelink_r16_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_rrc_recfg_complete_sidelink_s_crit_exts_c__names[] = {
    {"rrcReconfigurationCompleteSidelink-r16", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc rrc_recfg_complete_sidelink_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<rrc_recfg_complete_sidelink_s::crit_exts_c_::types>(),
    rrc_recfg_complete_sidelink_s::crit_exts_c_::types::nof_types,
    rrc_recfg_complete_sidelink_s::crit_exts_c_::types::nof_exts,
    rrc_recfg_complete_sidelink_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_rrc_recfg_complete_sidelink_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<rrc_recfg_complete_sidelink_s::crit_exts_c_>,
    choice_table_rrc_recfg_complete_sidelink_s_crit_exts_c__names,
    "rrc_recfg_complete_sidelink_s::crit_exts_c_",
    offsetof(rrc_recfg_complete_sidelink_s::crit_exts_c_, type_),
    offsetof(rrc_recfg_complete_sidelink_s::crit_exts_c_, c),
    false};

static const char* enum_names_rrc_recfg_complete_sidelink_s_crit_exts_c__types_values[] = {
    "rrcReconfigurationCompleteSidelink-r16",
    "criticalExtensionsFuture"};
const asn1::table::enum_desc rrc_recfg_complete_sidelink_s::crit_exts_c_::types_opts::desc = {
    enum_names_rrc_recfg_complete_sidelink_s_crit_exts_c__types_values,
    2,
    "rrc_recfg_complete_sidelink_s::crit_exts_c_::types"};

// RRCReconfigurationFailureSidelink-r16-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_recfg_fail_sidelink_r16_ies_s_fields[] = {
    {offsetof(rrc_recfg_fail_sidelink_r16_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(rrc_recfg_fail_sidelink_r16_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(rrc_recfg_fail_sidelink_r16_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_rrc_recfg_fail_sidelink_r16_ies_s_names[] = {
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_rrc_recfg_fail_sidelink_r16_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(rrc_recfg_fail_sidelink_r16_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_recfg_fail_sidelink_r16_ies_s::desc = {
    {seq_table_rrc_recfg_fail_sidelink_r16_ies_s_fields,
     2,
     UINT64_C(0x0),
     seq_table_rrc_recfg_fail_sidelink_r16_ies_s_opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_recfg_fail_sidelink_r16_ies_s_names};

// RRCReconfigurationFailureSidelink ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_recfg_fail_sidelink_s_fields[] = {
    {offsetof(rrc_recfg_fail_sidelink_s, rrc_transaction_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(rrc_recfg_fail_sidelink_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_fail_sidelink_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_rrc_recfg_fail_sidelink_s_names[] = {
    {"rrc-TransactionIdentifier-r16", nullptr},
    {"criticalExtensions", nullptr}};
const asn1::table::seq_desc rrc_recfg_fail_sidelink_s::desc = {
    {seq_table_rrc_recfg_fail_sidelink_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_recfg_fail_sidelink_s_names};

void rrc_recfg_fail_sidelink_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_recfg_fail_sidelink_r16_ies_s& rrc_recfg_fail_sidelink_s::crit_exts_c_::set_rrc_recfg_fail_sidelink_r16()
{
  set(types::rrc_recfg_fail_sidelink_r16);
  return c;
}
void rrc_recfg_fail_sidelink_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_rrc_recfg_fail_sidelink_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_fail_sidelink_r16_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_rrc_recfg_fail_sidelink_s_crit_exts_c__names[] = {
    {"rrcReconfigurationFailureSidelink-r16", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc rrc_recfg_fail_sidelink_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<rrc_recfg_fail_sidelink_s::crit_exts_c_::types>(),
    rrc_recfg_fail_sidelink_s::crit_exts_c_::types::nof_types,
    rrc_recfg_fail_sidelink_s::crit_exts_c_::types::nof_exts,
    rrc_recfg_fail_sidelink_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_rrc_recfg_fail_sidelink_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<rrc_recfg_fail_sidelink_s::crit_exts_c_>,
    choice_table_rrc_recfg_fail_sidelink_s_crit_exts_c__names,
    "rrc_recfg_fail_sidelink_s::crit_exts_c_",
    offsetof(rrc_recfg_fail_sidelink_s::crit_exts_c_, type_),
    offsetof(rrc_recfg_fail_sidelink_s::crit_exts_c_, c),
    false};

static const char* enum_names_rrc_recfg_fail_sidelink_s_crit_exts_c__types_values[] = {
    "rrcReconfigurationFailureSidelink-r16",
    "criticalExtensionsFuture"};
const asn1::table::enum_desc rrc_recfg_fail_sidelink_s::crit_exts_c_::types_opts::desc = {
    enum_names_rrc_recfg_fail_sidelink_s_crit_exts_c__types_values,
    2,
    "rrc_recfg_fail_sidelink_s::crit_exts_c_::types"};

// SL-SDAP-ConfigPC5-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_sdap_cfg_pc5_r16_s_fields[] = {
    {offsetof(sl_sdap_cfg_pc5_r16_s, sl_mapped_qos_flows_to_add_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(sl_sdap_cfg_pc5_r16_s::sl_mapped_qos_flows_to_add_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(64), false, false>>::value},
    {offsetof(sl_sdap_cfg_pc5_r16_s, sl_mapped_qos_flows_to_release_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(sl_sdap_cfg_pc5_r16_s::sl_mapped_qos_flows_to_release_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(64), false, false>>::value},
    {offsetof(sl_sdap_cfg_pc5_r16_s, sl_sdap_hdr_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_sdap_cfg_pc5_r16_s::sl_sdap_hdr_r16)>(),
     decltype(sl_sdap_cfg_pc5_r16_s::sl_sdap_hdr_r16)::has_ext,
     false,
     decltype(sl_sdap_cfg_pc5_r16_s::sl_sdap_hdr_r16)::nof_types,
     decltype(sl_sdap_cfg_pc5_r16_s::sl_sdap_hdr_r16)::nof_exts,
     nullptr,
     &sl_sdap_cfg_pc5_r16_s::sl_sdap_hdr_r16_opts::desc}};
static const asn1::table::field_name seq_table_sl_sdap_cfg_pc5_r16_s_names[] = {
    {"sl-MappedQoS-FlowsToAddList-r16", nullptr},
    {"sl-MappedQoS-FlowsToReleaseList-r16", nullptr},
    {"sl-SDAP-Header-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_sdap_cfg_pc5_r16_s_opts[] = {{0, asn1::table::seq_field::list_presence},
                                                                            {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       sl_sdap_cfg_pc5_r16_s::desc            = {
    {seq_table_sl_sdap_cfg_pc5_r16_s_fields, 3, UINT64_C(0x4), seq_table_sl_sdap_cfg_pc5_r16_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_sdap_cfg_pc5_r16_s_names};

static const char*           enum_names_sl_sdap_cfg_pc5_r16_s_sl_sdap_hdr_r16_e__values[] = {"present", "absent"};
const asn1::table::enum_desc sl_sdap_cfg_pc5_r16_s::sl_sdap_hdr_r16_opts::desc            = {
    enum_names_sl_sdap_cfg_pc5_r16_s_sl_sdap_hdr_r16_e__values,
    2,
    "sl_sdap_cfg_pc5_r16_s::sl_sdap_hdr_r16_e_"};

// SL-PDCP-ConfigPC5-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pdcp_cfg_pc5_r16_s_fields[] = {
    {offsetof(sl_pdcp_cfg_pc5_r16_s, sl_pdcp_sn_size_r16),
     (int32_t)offsetof(sl_pdcp_cfg_pc5_r16_s, sl_pdcp_sn_size_r16_present),
     asn1::table::enum_op<decltype(sl_pdcp_cfg_pc5_r16_s::sl_pdcp_sn_size_r16)>(),
     decltype(sl_pdcp_cfg_pc5_r16_s::sl_pdcp_sn_size_r16)::has_ext,
     false,
     decltype(sl_pdcp_cfg_pc5_r16_s::sl_pdcp_sn_size_r16)::nof_types,
     decltype(sl_pdcp_cfg_pc5_r16_s::sl_pdcp_sn_size_r16)::nof_exts,
     nullptr,
     &sl_pdcp_cfg_pc5_r16_s::sl_pdcp_sn_size_r16_opts::desc},
    {0,
     (int32_t)offsetof(sl_pdcp_cfg_pc5_r16_s, sl_out_of_order_delivery_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sl_pdcp_cfg_pc5_r16_s_names[] = {{"sl-PDCP-SN-Size-r16", nullptr},
                                                                                {"sl-OutOfOrderDelivery-r16", "true"}};
static const asn1::table::seq_opt    seq_table_sl_pdcp_cfg_pc5_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_pdcp_cfg_pc5_r16_s, sl_pdcp_sn_size_r16_present)},
    {1, (int32_t)offsetof(sl_pdcp_cfg_pc5_r16_s, sl_out_of_order_delivery_r16_present)}};
const asn1::table::seq_desc sl_pdcp_cfg_pc5_r16_s::desc = {
    {seq_table_sl_pdcp_cfg_pc5_r16_s_fields, 2, UINT64_C(0x0), seq_table_sl_pdcp_cfg_pc5_r16_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_pdcp_cfg_pc5_r16_s_names};

static const char* enum_names_sl_pdcp_cfg_pc5_r16_s_sl_pdcp_sn_size_r16_e__values[] = {"len12bits", "len18bits"};
const asn1::table::enum_desc sl_pdcp_cfg_pc5_r16_s::sl_pdcp_sn_size_r16_opts::desc  = {
    enum_names_sl_pdcp_cfg_pc5_r16_s_sl_pdcp_sn_size_r16_e__values,
    2,
    "sl_pdcp_cfg_pc5_r16_s::sl_pdcp_sn_size_r16_e_"};
uint8_t sl_pdcp_cfg_pc5_r16_s::sl_pdcp_sn_size_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "sl_pdcp_cfg_pc5_r16_s::sl_pdcp_sn_size_r16_e_");
}

// SL-RLC-ConfigPC5-r16 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sl_rlc_cfg_pc5_r16_c_alternatives[] = {
    &asn1::table::alternative_ops<sl_rlc_cfg_pc5_r16_c::sl_am_rlc_r16_s_, false>::value,
    &asn1::table::alternative_ops<sl_rlc_cfg_pc5_r16_c::sl_um_bi_dir_rlc_r16_s_, false>::value,
    &asn1::table::alternative_ops<sl_rlc_cfg_pc5_r16_c::sl_um_uni_dir_rlc_r16_s_, false>::value};
const asn1::table::choice_storage_desc sl_rlc_cfg_pc5_r16_c::storage_desc = {
    choice_storage_sl_rlc_cfg_pc5_r16_c_alternatives,
    3,
    "sl_rlc_cfg_pc5_r16_c"};

sl_rlc_cfg_pc5_r16_c::sl_am_rlc_r16_s_& sl_rlc_cfg_pc5_r16_c::set_sl_am_rlc_r16()
{
  set(types::sl_am_rlc_r16);
  return c.get<sl_am_rlc_r16_s_>();
}
sl_rlc_cfg_pc5_r16_c::sl_um_bi_dir_rlc_r16_s_& sl_rlc_cfg_pc5_r16_c::set_sl_um_bi_dir_rlc_r16()
{
  set(types::sl_um_bi_dir_rlc_r16);
  return c.get<sl_um_bi_dir_rlc_r16_s_>();
}
sl_rlc_cfg_pc5_r16_c::sl_um_uni_dir_rlc_r16_s_& sl_rlc_cfg_pc5_r16_c::set_sl_um_uni_dir_rlc_r16()
{
  set(types::sl_um_uni_dir_rlc_r16);
  return c.get<sl_um_uni_dir_rlc_r16_s_>();
}
static const asn1::table::seq_field seq_table_sl_rlc_cfg_pc5_r16_c_sl_am_rlc_r16_s__fields[] = {
    {offsetof(sl_rlc_cfg_pc5_r16_c::sl_am_rlc_r16_s_, sl_sn_field_len_am_r16),
     (int32_t)offsetof(sl_rlc_cfg_pc5_r16_c::sl_am_rlc_r16_s_, sl_sn_field_len_am_r16_present),
     asn1::table::enum_op<decltype(sl_rlc_cfg_pc5_r16_c::sl_am_rlc_r16_s_::sl_sn_field_len_am_r16)>(),
     decltype(sl_rlc_cfg_pc5_r16_c::sl_am_rlc_r16_s_::sl_sn_field_len_am_r16)::has_ext,
     false,
     decltype(sl_rlc_cfg_pc5_r16_c::sl_am_rlc_r16_s_::sl_sn_field_len_am_r16)::nof_types,
     decltype(sl_rlc_cfg_pc5_r16_c::sl_am_rlc_r16_s_::sl_sn_field_len_am_r16)::nof_exts,
     nullptr,
     &sn_field_len_am_opts::desc}};
static const asn1::table::field_name seq_table_sl_rlc_cfg_pc5_r16_c_sl_am_rlc_r16_s__names[] = {
    {"sl-SN-FieldLengthAM-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_rlc_cfg_pc5_r16_c_sl_am_rlc_r16_s__opts[] = {
    {0, (int32_t)offsetof(sl_rlc_cfg_pc5_r16_c::sl_am_rlc_r16_s_, sl_sn_field_len_am_r16_present)}};
static const asn1::table::seq_desc seq_table_sl_rlc_cfg_pc5_r16_c_sl_am_rlc_r16_s_ = {
    {seq_table_sl_rlc_cfg_pc5_r16_c_sl_am_rlc_r16_s__fields,
     1,
     UINT64_C(0x0),
     seq_table_sl_rlc_cfg_pc5_r16_c_sl_am_rlc_r16_s__opts,
     1},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_rlc_cfg_pc5_r16_c_sl_am_rlc_r16_s__names};
static const asn1::table::seq_field seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_bi_dir_rlc_r16_s__fields[] = {
    {offsetof(sl_rlc_cfg_pc5_r16_c::sl_um_bi_dir_rlc_r16_s_, sl_sn_field_len_um_r16),
     (int32_t)offsetof(sl_rlc_cfg_pc5_r16_c::sl_um_bi_dir_rlc_r16_s_, sl_sn_field_len_um_r16_present),
     asn1::table::enum_op<decltype(sl_rlc_cfg_pc5_r16_c::sl_um_bi_dir_rlc_r16_s_::sl_sn_field_len_um_r16)>(),
     decltype(sl_rlc_cfg_pc5_r16_c::sl_um_bi_dir_rlc_r16_s_::sl_sn_field_len_um_r16)::has_ext,
     false,
     decltype(sl_rlc_cfg_pc5_r16_c::sl_um_bi_dir_rlc_r16_s_::sl_sn_field_len_um_r16)::nof_types,
     decltype(sl_rlc_cfg_pc5_r16_c::sl_um_bi_dir_rlc_r16_s_::sl_sn_field_len_um_r16)::nof_exts,
     nullptr,
     &sn_field_len_um_opts::desc}};
static const asn1::table::field_name seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_bi_dir_rlc_r16_s__names[] = {
    {"sl-SN-FieldLengthUM-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_bi_dir_rlc_r16_s__opts[] = {
    {0, (int32_t)offsetof(sl_rlc_cfg_pc5_r16_c::sl_um_bi_dir_rlc_r16_s_, sl_sn_field_len_um_r16_present)}};
static const asn1::table::seq_desc seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_bi_dir_rlc_r16_s_ = {
    {seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_bi_dir_rlc_r16_s__fields,
     1,
     UINT64_C(0x0),
     seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_bi_dir_rlc_r16_s__opts,
     1},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_bi_dir_rlc_r16_s__names};
static const asn1::table::seq_field seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_uni_dir_rlc_r16_s__fields[] = {
    {offsetof(sl_rlc_cfg_pc5_r16_c::sl_um_uni_dir_rlc_r16_s_, sl_sn_field_len_um_r16),
     (int32_t)offsetof(sl_rlc_cfg_pc5_r16_c::sl_um_uni_dir_rlc_r16_s_, sl_sn_field_len_um_r16_present),
     asn1::table::enum_op<decltype(sl_rlc_cfg_pc5_r16_c::sl_um_uni_dir_rlc_r16_s_::sl_sn_field_len_um_r16)>(),
     decltype(sl_rlc_cfg_pc5_r16_c::sl_um_uni_dir_rlc_r16_s_::sl_sn_field_len_um_r16)::has_ext,
     false,
     decltype(sl_rlc_cfg_pc5_r16_c::sl_um_uni_dir_rlc_r16_s_::sl_sn_field_len_um_r16)::nof_types,
     decltype(sl_rlc_cfg_pc5_r16_c::sl_um_uni_dir_rlc_r16_s_::sl_sn_field_len_um_r16)::nof_exts,
     nullptr,
     &sn_field_len_um_opts::desc}};
static const asn1::table::field_name seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_uni_dir_rlc_r16_s__names[] = {
    {"sl-SN-FieldLengthUM-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_uni_dir_rlc_r16_s__opts[] = {
    {0, (int32_t)offsetof(sl_rlc_cfg_pc5_r16_c::sl_um_uni_dir_rlc_r16_s_, sl_sn_field_len_um_r16_present)}};
static const asn1::table::seq_desc seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_uni_dir_rlc_r16_s_ = {
    {seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_uni_dir_rlc_r16_s__fields,
     1,
     UINT64_C(0x0),
     seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_uni_dir_rlc_r16_s__opts,
     1},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_uni_dir_rlc_r16_s__names};
static const asn1::table::seq_field choice_table_sl_rlc_cfg_pc5_r16_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_rlc_cfg_pc5_r16_c_sl_am_rlc_r16_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_bi_dir_rlc_r16_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_rlc_cfg_pc5_r16_c_sl_um_uni_dir_rlc_r16_s_}};
static const asn1::table::field_name choice_table_sl_rlc_cfg_pc5_r16_c_names[] = {
    {"sl-AM-RLC-r16", nullptr},
    {"sl-UM-Bi-Directional-RLC-r16", nullptr},
    {"sl-UM-Uni-Directional-RLC-r16", nullptr}};
const asn1::table::choice_desc sl_rlc_cfg_pc5_r16_c::desc = {asn1::table::enum_int_type<sl_rlc_cfg_pc5_r16_c::types>(),
                                                             sl_rlc_cfg_pc5_r16_c::types::nof_types,
                                                             sl_rlc_cfg_pc5_r16_c::types::nof_exts,
                                                             sl_rlc_cfg_pc5_r16_c::types::has_ext,
                                                             false,
                                                             choice_table_sl_rlc_cfg_pc5_r16_c_alternatives,
                                                             nullptr,
                                                             choice_table_sl_rlc_cfg_pc5_r16_c_names,
                                                             "sl_rlc_cfg_pc5_r16_c",
                                                             offsetof(sl_rlc_cfg_pc5_r16_c, c.type),
                                                             offsetof(sl_rlc_cfg_pc5_r16_c, c.value),
                                                             false,
                                                             &sl_rlc_cfg_pc5_r16_c::storage_desc};

static const char*           enum_names_sl_rlc_cfg_pc5_r16_c_types_values[] = {"sl-AM-RLC-r16",
                                                                               "sl-UM-Bi-Directional-RLC-r16",
                                                                               "sl-UM-Uni-Directional-RLC-r16"};
const asn1::table::enum_desc sl_rlc_cfg_pc5_r16_c::types_opts::desc = {enum_names_sl_rlc_cfg_pc5_r16_c_types_values,
                                                                       3,
                                                                       "sl_rlc_cfg_pc5_r16_c::types"};

// SL-LogicalChannelConfigPC5-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_lc_ch_cfg_pc5_r16_s_fields[] = {
    {offsetof(sl_lc_ch_cfg_pc5_r16_s, sl_lc_ch_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(32),
     nullptr}};
static const asn1::table::field_name seq_table_sl_lc_ch_cfg_pc5_r16_s_names[] = {
    {"sl-LogicalChannelIdentity-r16", nullptr}};
static const asn1::table::seq_field seq_table_sl_lc_ch_cfg_pc5_r16_s_g0_fields[] = {
    {offsetof(sl_lc_ch_cfg_pc5_r16_s, sl_lc_ch_id_v1800),
     (int32_t)offsetof(sl_lc_ch_cfg_pc5_r16_s, sl_lc_ch_id_v1800_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(33),
     UINT64_C(38),
     nullptr}};
static const asn1::table::field_name seq_table_sl_lc_ch_cfg_pc5_r16_s_g0_names[] = {
    {"sl-LogicalChannelIdentity-v1800", nullptr}};
static const asn1::table::seq_opt seq_table_sl_lc_ch_cfg_pc5_r16_s_g0_opts[] = {
    {0, (int32_t)offsetof(sl_lc_ch_cfg_pc5_r16_s, sl_lc_ch_id_v1800_present)}};
static const asn1::table::seq_group seq_table_sl_lc_ch_cfg_pc5_r16_s_groups[] = {
    {{seq_table_sl_lc_ch_cfg_pc5_r16_s_g0_fields, 1, UINT64_C(0x0), seq_table_sl_lc_ch_cfg_pc5_r16_s_g0_opts, 1},
     false,
     seq_table_sl_lc_ch_cfg_pc5_r16_s_g0_names}};
const asn1::table::seq_desc sl_lc_ch_cfg_pc5_r16_s::desc = {
    {seq_table_sl_lc_ch_cfg_pc5_r16_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    seq_table_sl_lc_ch_cfg_pc5_r16_s_groups,
    1,
    seq_table_sl_lc_ch_cfg_pc5_r16_s_names};

// SLRB-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_slrb_cfg_r16_s_fields[] = {
    {offsetof(slrb_cfg_r16_s, slrb_pc5_cfg_idx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(slrb_cfg_r16_s, sl_sdap_cfg_pc5_r16),
     (int32_t)offsetof(slrb_cfg_r16_s, sl_sdap_cfg_pc5_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_sdap_cfg_pc5_r16_s::desc},
    {offsetof(slrb_cfg_r16_s, sl_pdcp_cfg_pc5_r16),
     (int32_t)offsetof(slrb_cfg_r16_s, sl_pdcp_cfg_pc5_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_pdcp_cfg_pc5_r16_s::desc},
    {offsetof(slrb_cfg_r16_s, sl_rlc_cfg_pc5_r16),
     (int32_t)offsetof(slrb_cfg_r16_s, sl_rlc_cfg_pc5_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_rlc_cfg_pc5_r16_c::desc},
    {offsetof(slrb_cfg_r16_s, sl_mac_lc_ch_cfg_pc5_r16),
     (int32_t)offsetof(slrb_cfg_r16_s, sl_mac_lc_ch_cfg_pc5_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_lc_ch_cfg_pc5_r16_s::desc}};
static const asn1::table::field_name seq_table_slrb_cfg_r16_s_names[] = {
    {"slrb-PC5-ConfigIndex-r16", nullptr},
    {"sl-SDAP-ConfigPC5-r16", nullptr},
    {"sl-PDCP-ConfigPC5-r16", nullptr},
    {"sl-RLC-ConfigPC5-r16", nullptr},
    {"sl-MAC-LogicalChannelConfigPC5-r16", nullptr}};
static const asn1::table::seq_opt seq_table_slrb_cfg_r16_s_opts[] = {
    {1, (int32_t)offsetof(slrb_cfg_r16_s, sl_sdap_cfg_pc5_r16_present)},
    {2, (int32_t)offsetof(slrb_cfg_r16_s, sl_pdcp_cfg_pc5_r16_present)},
    {3, (int32_t)offsetof(slrb_cfg_r16_s, sl_rlc_cfg_pc5_r16_present)},
    {4, (int32_t)offsetof(slrb_cfg_r16_s, sl_mac_lc_ch_cfg_pc5_r16_present)}};
const asn1::table::seq_desc slrb_cfg_r16_s::desc = {
    {seq_table_slrb_cfg_r16_s_fields, 5, UINT64_C(0x1), seq_table_slrb_cfg_r16_s_opts, 4},
    true,
    false,
    nullptr,
    0,
    seq_table_slrb_cfg_r16_s_names};

// SL-CSI-RS-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_csi_rs_cfg_r16_s_fields[] = {
    {offsetof(sl_csi_rs_cfg_r16_s, sl_csi_rs_freq_alloc_r16),
     (int32_t)offsetof(sl_csi_rs_cfg_r16_s, sl_csi_rs_freq_alloc_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::desc},
    {offsetof(sl_csi_rs_cfg_r16_s, sl_csi_rs_first_symbol_r16),
     (int32_t)offsetof(sl_csi_rs_cfg_r16_s, sl_csi_rs_first_symbol_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(3),
     UINT64_C(12),
     nullptr}};
static const asn1::table::field_name seq_table_sl_csi_rs_cfg_r16_s_names[] = {{"sl-CSI-RS-FreqAllocation-r16", nullptr},
                                                                              {"sl-CSI-RS-FirstSymbol-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_csi_rs_cfg_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sl_csi_rs_cfg_r16_s, sl_csi_rs_freq_alloc_r16_present)},
    {1, (int32_t)offsetof(sl_csi_rs_cfg_r16_s, sl_csi_rs_first_symbol_r16_present)}};
const asn1::table::seq_desc sl_csi_rs_cfg_r16_s::desc = {
    {seq_table_sl_csi_rs_cfg_r16_s_fields, 2, UINT64_C(0x0), seq_table_sl_csi_rs_cfg_r16_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_csi_rs_cfg_r16_s_names};

static const asn1::table::storage_ops* const
    choice_storage_sl_csi_rs_cfg_r16_s_sl_csi_rs_freq_alloc_r16_c__alternatives[] = {
        &asn1::table::alternative_ops<fixed_bitstring<12>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<6>, false>::value};
const asn1::table::choice_storage_desc sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::storage_desc = {
    choice_storage_sl_csi_rs_cfg_r16_s_sl_csi_rs_freq_alloc_r16_c__alternatives,
    2,
    "sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_"};

fixed_bitstring<12>& sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::set_sl_one_ant_port_r16()
{
  set(types::sl_one_ant_port_r16);
  return c.get<fixed_bitstring<12>>();
}
fixed_bitstring<6>& sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::set_sl_two_ant_port_r16()
{
  set(types::sl_two_ant_port_r16);
  return c.get<fixed_bitstring<6>>();
}
static const asn1::table::seq_field choice_table_sl_csi_rs_cfg_r16_s_sl_csi_rs_freq_alloc_r16_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<12>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<6>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_sl_csi_rs_cfg_r16_s_sl_csi_rs_freq_alloc_r16_c__names[] = {
    {"sl-OneAntennaPort-r16", nullptr},
    {"sl-TwoAntennaPort-r16", nullptr}};
const asn1::table::choice_desc sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::desc = {
    asn1::table::enum_int_type<sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::types>(),
    sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::types::nof_types,
    sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::types::nof_exts,
    sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::types::has_ext,
    false,
    choice_table_sl_csi_rs_cfg_r16_s_sl_csi_rs_freq_alloc_r16_c__alternatives,
    nullptr,
    choice_table_sl_csi_rs_cfg_r16_s_sl_csi_rs_freq_alloc_r16_c__names,
    "sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_",
    offsetof(sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_, c.type),
    offsetof(sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_, c.value),
    false,
    &sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::storage_desc};

static const char* enum_names_sl_csi_rs_cfg_r16_s_sl_csi_rs_freq_alloc_r16_c__types_values[] = {
    "sl-OneAntennaPort-r16",
    "sl-TwoAntennaPort-r16"};
const asn1::table::enum_desc sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::types_opts::desc = {
    enum_names_sl_csi_rs_cfg_r16_s_sl_csi_rs_freq_alloc_r16_c__types_values,
    2,
    "sl_csi_rs_cfg_r16_s::sl_csi_rs_freq_alloc_r16_c_::types"};

// SL-RLC-ChannelConfigPC5-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_rlc_ch_cfg_pc5_r17_s_fields[] = {
    {offsetof(sl_rlc_ch_cfg_pc5_r17_s, sl_rlc_ch_id_pc5_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(sl_rlc_ch_cfg_pc5_r17_s, sl_rlc_cfg_pc5_r17),
     (int32_t)offsetof(sl_rlc_ch_cfg_pc5_r17_s, sl_rlc_cfg_pc5_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_rlc_cfg_pc5_r16_c::desc},
    {offsetof(sl_rlc_ch_cfg_pc5_r17_s, sl_mac_lc_ch_cfg_pc5_r17),
     (int32_t)offsetof(sl_rlc_ch_cfg_pc5_r17_s, sl_mac_lc_ch_cfg_pc5_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_lc_ch_cfg_pc5_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_rlc_ch_cfg_pc5_r17_s_names[] = {
    {"sl-RLC-ChannelID-PC5-r17", nullptr},
    {"sl-RLC-ConfigPC5-r17", nullptr},
    {"sl-MAC-LogicalChannelConfigPC5-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_rlc_ch_cfg_pc5_r17_s_opts[] = {
    {1, (int32_t)offsetof(sl_rlc_ch_cfg_pc5_r17_s, sl_rlc_cfg_pc5_r17_present)},
    {2, (int32_t)offsetof(sl_rlc_ch_cfg_pc5_r17_s, sl_mac_lc_ch_cfg_pc5_r17_present)}};
const asn1::table::seq_desc sl_rlc_ch_cfg_pc5_r17_s::desc = {
    {seq_table_sl_rlc_ch_cfg_pc5_r17_s_fields, 3, UINT64_C(0x1), seq_table_sl_rlc_ch_cfg_pc5_r17_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_rlc_ch_cfg_pc5_r17_s_names};

// SL-SFN-DFN-Offset-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_sfn_dfn_offset_r18_s_fields[] = {
    {offsetof(sl_sfn_dfn_offset_r18_s, sl_frame_offset_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(sl_sfn_dfn_offset_r18_s, sl_sf_offset_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(sl_sfn_dfn_offset_r18_s, sl_slot_offset_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr}};
static const asn1::table::field_name seq_table_sl_sfn_dfn_offset_r18_s_names[] = {{"sl-FrameOffset-r18", nullptr},
                                                                                  {"sl-SubframeOffset-r18", nullptr},
                                                                                  {"sl-SlotOffset-r18", nullptr}};
const asn1::table::seq_desc          sl_sfn_dfn_offset_r18_s::desc             = {
    {seq_table_sl_sfn_dfn_offset_r18_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_sfn_dfn_offset_r18_s_names};

// SL-CarrierConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_carrier_cfg_r18_s_fields[] = {
    {offsetof(sl_carrier_cfg_r18_s, sl_carrier_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(7),
     nullptr},
    {offsetof(sl_carrier_cfg_r18_s, sl_offset_to_carrier_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(2199),
     nullptr},
    {offsetof(sl_carrier_cfg_r18_s, subcarrier_spacing_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_carrier_cfg_r18_s::subcarrier_spacing_r18)>(),
     decltype(sl_carrier_cfg_r18_s::subcarrier_spacing_r18)::has_ext,
     false,
     decltype(sl_carrier_cfg_r18_s::subcarrier_spacing_r18)::nof_types,
     decltype(sl_carrier_cfg_r18_s::subcarrier_spacing_r18)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(sl_carrier_cfg_r18_s, carrier_bw_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(275),
     nullptr},
    {offsetof(sl_carrier_cfg_r18_s, sl_absolute_freq_point_a_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr}};
static const asn1::table::field_name seq_table_sl_carrier_cfg_r18_s_names[] = {
    {"sl-CarrierId-r18", nullptr},
    {"sl-OffsetToCarrier-r18", nullptr},
    {"subcarrierSpacing-r18", nullptr},
    {"carrierBandwidth-r18", nullptr},
    {"sl-AbsoluteFrequencyPointA-r18", nullptr}};
const asn1::table::seq_desc sl_carrier_cfg_r18_s::desc = {
    {seq_table_sl_carrier_cfg_r18_s_fields, 5, UINT64_C(0x1f), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_carrier_cfg_r18_s_names};

// SL-RLC-BearerConfig-r18 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_sl_rlc_bearer_cfg_r18_c_alternatives[] = {
    &asn1::table::alternative_ops<sl_rlc_bearer_cfg_r18_c::srb_s_, false>::value,
    &asn1::table::alternative_ops<sl_rlc_bearer_cfg_r18_c::drb_s_, false>::value};
const asn1::table::choice_storage_desc sl_rlc_bearer_cfg_r18_c::storage_desc = {
    choice_storage_sl_rlc_bearer_cfg_r18_c_alternatives,
    2,
    "sl_rlc_bearer_cfg_r18_c"};

sl_rlc_bearer_cfg_r18_c::srb_s_& sl_rlc_bearer_cfg_r18_c::set_srb()
{
  set(types::srb);
  return c.get<srb_s_>();
}
sl_rlc_bearer_cfg_r18_c::drb_s_& sl_rlc_bearer_cfg_r18_c::set_drb()
{
  set(types::drb);
  return c.get<drb_s_>();
}
static const asn1::table::seq_field seq_table_sl_rlc_bearer_cfg_r18_c_srb_s__fields[] = {
    {offsetof(sl_rlc_bearer_cfg_r18_c::srb_s_, sl_srb_id_with_dupl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(3),
     nullptr},
    {offsetof(sl_rlc_bearer_cfg_r18_c::srb_s_, sl_rlc_bearer_cfg_idx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(512),
     nullptr}};
static const asn1::table::field_name seq_table_sl_rlc_bearer_cfg_r18_c_srb_s__names[] = {
    {"sl-SRB-IdentityWithDuplication", nullptr},
    {"sL-RLC-BearerConfigIndex-r18", nullptr}};
static const asn1::table::seq_desc seq_table_sl_rlc_bearer_cfg_r18_c_srb_s_ = {
    {seq_table_sl_rlc_bearer_cfg_r18_c_srb_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_rlc_bearer_cfg_r18_c_srb_s__names};
static const asn1::table::seq_field seq_table_sl_rlc_bearer_cfg_r18_c_drb_s__fields[] = {
    {offsetof(sl_rlc_bearer_cfg_r18_c::drb_s_, slrb_pc5_cfg_idx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(sl_rlc_bearer_cfg_r18_c::drb_s_, sl_rlc_bearer_cfg_idx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(sl_rlc_bearer_cfg_r18_c::drb_s_, sl_rlc_cfg_pc5_r18),
     (int32_t)offsetof(sl_rlc_bearer_cfg_r18_c::drb_s_, sl_rlc_cfg_pc5_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_rlc_cfg_pc5_r16_c::desc},
    {offsetof(sl_rlc_bearer_cfg_r18_c::drb_s_, sl_mac_lc_ch_cfg_pc5_r18),
     (int32_t)offsetof(sl_rlc_bearer_cfg_r18_c::drb_s_, sl_mac_lc_ch_cfg_pc5_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_lc_ch_cfg_pc5_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_rlc_bearer_cfg_r18_c_drb_s__names[] = {
    {"slrb-PC5-ConfigIndex-r18", nullptr},
    {"sL-RLC-BearerConfigIndex-r18", nullptr},
    {"sl-RLC-ConfigPC5-r18", nullptr},
    {"sl-MAC-LogicalChannelConfigPC5-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_rlc_bearer_cfg_r18_c_drb_s__opts[] = {
    {2, (int32_t)offsetof(sl_rlc_bearer_cfg_r18_c::drb_s_, sl_rlc_cfg_pc5_r18_present)},
    {3, (int32_t)offsetof(sl_rlc_bearer_cfg_r18_c::drb_s_, sl_mac_lc_ch_cfg_pc5_r18_present)}};
static const asn1::table::seq_desc seq_table_sl_rlc_bearer_cfg_r18_c_drb_s_ = {
    {seq_table_sl_rlc_bearer_cfg_r18_c_drb_s__fields,
     4,
     UINT64_C(0x3),
     seq_table_sl_rlc_bearer_cfg_r18_c_drb_s__opts,
     2},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_rlc_bearer_cfg_r18_c_drb_s__names};
static const asn1::table::seq_field choice_table_sl_rlc_bearer_cfg_r18_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_rlc_bearer_cfg_r18_c_srb_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sl_rlc_bearer_cfg_r18_c_drb_s_}};
static const asn1::table::field_name choice_table_sl_rlc_bearer_cfg_r18_c_names[] = {{"srb", nullptr},
                                                                                     {"drb", nullptr}};
const asn1::table::choice_desc       sl_rlc_bearer_cfg_r18_c::desc                = {
    asn1::table::enum_int_type<sl_rlc_bearer_cfg_r18_c::types>(),
    sl_rlc_bearer_cfg_r18_c::types::nof_types,
    sl_rlc_bearer_cfg_r18_c::types::nof_exts,
    sl_rlc_bearer_cfg_r18_c::types::has_ext,
    false,
    choice_table_sl_rlc_bearer_cfg_r18_c_alternatives,
    nullptr,
    choice_table_sl_rlc_bearer_cfg_r18_c_names,
    "sl_rlc_bearer_cfg_r18_c",
    offsetof(sl_rlc_bearer_cfg_r18_c, c.type),
    offsetof(sl_rlc_bearer_cfg_r18_c, c.value),
    false,
    &sl_rlc_bearer_cfg_r18_c::storage_desc};

static const char*           enum_names_sl_rlc_bearer_cfg_r18_c_types_values[] = {"srb", "drb"};
const asn1::table::enum_desc sl_rlc_bearer_cfg_r18_c::types_opts::desc         = {
    enum_names_sl_rlc_bearer_cfg_r18_c_types_values,
    2,
    "sl_rlc_bearer_cfg_r18_c::types"};

// SL-SRAP-ConfigPC5-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_srap_cfg_pc5_r18_s_fields[] = {
    {offsetof(sl_srap_cfg_pc5_r18_s, sl_peer_remote_ue_l2_id_r18),
     (int32_t)offsetof(sl_srap_cfg_pc5_r18_s, sl_peer_remote_ue_l2_id_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_srap_cfg_pc5_r18_s::sl_peer_remote_ue_l2_id_r18),
                                asn1::table::struct_json>::value},
    {offsetof(sl_srap_cfg_pc5_r18_s, sl_peer_remote_ue_local_id_r18),
     (int32_t)offsetof(sl_srap_cfg_pc5_r18_s, sl_peer_remote_ue_local_id_r18_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(sl_srap_cfg_pc5_r18_s, sl_remote_ue_l2_id_r18),
     (int32_t)offsetof(sl_srap_cfg_pc5_r18_s, sl_remote_ue_l2_id_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_srap_cfg_pc5_r18_s::sl_remote_ue_l2_id_r18),
                                asn1::table::struct_json>::value},
    {offsetof(sl_srap_cfg_pc5_r18_s, sl_remote_ue_local_id_r18),
     (int32_t)offsetof(sl_srap_cfg_pc5_r18_s, sl_remote_ue_local_id_r18_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(255),
     nullptr}};
static const asn1::table::field_name seq_table_sl_srap_cfg_pc5_r18_s_names[] = {
    {"sl-PeerRemoteUE-L2Identity-r18", nullptr},
    {"sl-PeerRemoteUE-LocalIdentity-r18", nullptr},
    {"sl-RemoteUE-L2Identity-r18", nullptr},
    {"sl-RemoteUE-LocalIdentity-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_srap_cfg_pc5_r18_s_opts[] = {
    {0, (int32_t)offsetof(sl_srap_cfg_pc5_r18_s, sl_peer_remote_ue_l2_id_r18_present)},
    {1, (int32_t)offsetof(sl_srap_cfg_pc5_r18_s, sl_peer_remote_ue_local_id_r18_present)},
    {2, (int32_t)offsetof(sl_srap_cfg_pc5_r18_s, sl_remote_ue_l2_id_r18_present)},
    {3, (int32_t)offsetof(sl_srap_cfg_pc5_r18_s, sl_remote_ue_local_id_r18_present)}};
const asn1::table::seq_desc sl_srap_cfg_pc5_r18_s::desc = {
    {seq_table_sl_srap_cfg_pc5_r18_s_fields, 4, UINT64_C(0x0), seq_table_sl_srap_cfg_pc5_r18_s_opts, 4},
    true,
    false,
    nullptr,
    0,
    seq_table_sl_srap_cfg_pc5_r18_s_names};

// RRCReconfigurationSidelink-v1800-IEs ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_rrc_recfg_sidelink_v1800_ies_s_sl_sfn_dfn_offset_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_sfn_dfn_offset_r18_s::desc}};
static const asn1::table::field_name
    seq_table_rrc_recfg_sidelink_v1800_ies_s_sl_sfn_dfn_offset_r18_setup_release_names[] = {{"release", nullptr},
                                                                                            {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_rrc_recfg_sidelink_v1800_ies_s_sl_sfn_dfn_offset_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_rrc_recfg_sidelink_v1800_ies_s_sl_sfn_dfn_offset_r18_setup_release_alternatives,
    nullptr,
    seq_table_rrc_recfg_sidelink_v1800_ies_s_sl_sfn_dfn_offset_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_sfn_dfn_offset_r18_s>::type_offset(),
    setup_release_c<sl_sfn_dfn_offset_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_rrc_recfg_sidelink_v1800_ies_s_fields[] = {
    {offsetof(rrc_recfg_sidelink_v1800_ies_s, sl_sfn_dfn_offset_r18),
     (int32_t)offsetof(rrc_recfg_sidelink_v1800_ies_s, sl_sfn_dfn_offset_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_rrc_recfg_sidelink_v1800_ies_s_sl_sfn_dfn_offset_r18_setup_release},
    {offsetof(rrc_recfg_sidelink_v1800_ies_s, sl_carrier_to_add_mod_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(rrc_recfg_sidelink_v1800_ies_s::sl_carrier_to_add_mod_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_carrier_cfg_r18_s::desc},
    {offsetof(rrc_recfg_sidelink_v1800_ies_s, sl_carrier_to_release_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(rrc_recfg_sidelink_v1800_ies_s::sl_carrier_to_release_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(7), false, false>>::value},
    {offsetof(rrc_recfg_sidelink_v1800_ies_s, sl_rlc_bearer_to_add_mod_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::item_list_codec<decltype(rrc_recfg_sidelink_v1800_ies_s::sl_rlc_bearer_to_add_mod_list_r18),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &sl_rlc_bearer_cfg_r18_c::desc},
    {offsetof(rrc_recfg_sidelink_v1800_ies_s, sl_rlc_bearer_to_release_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::list_codec<decltype(rrc_recfg_sidelink_v1800_ies_s::sl_rlc_bearer_to_release_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(512), false, false>>::value},
    {offsetof(rrc_recfg_sidelink_v1800_ies_s, sl_local_id_pair_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(rrc_recfg_sidelink_v1800_ies_s::sl_local_id_pair_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_srap_cfg_pc5_r18_s::desc},
    {0,
     (int32_t)offsetof(rrc_recfg_sidelink_v1800_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_rrc_recfg_sidelink_v1800_ies_s_names[] = {
    {"sl-SFN-DFN-Offset-r18", nullptr},
    {"sl-CarrierToAddModList-r18", nullptr},
    {"sl-CarrierToReleaseList-r18", nullptr},
    {"sl-RLC-BearerToAddModList-r18", nullptr},
    {"sl-RLC-BearerToReleaseList-r18", nullptr},
    {"sl-LocalID-PairList-r18", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_rrc_recfg_sidelink_v1800_ies_s_opts[] = {
    {0, (int32_t)offsetof(rrc_recfg_sidelink_v1800_ies_s, sl_sfn_dfn_offset_r18_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(rrc_recfg_sidelink_v1800_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_recfg_sidelink_v1800_ies_s::desc = {{seq_table_rrc_recfg_sidelink_v1800_ies_s_fields,
                                                                     7,
                                                                     UINT64_C(0x0),
                                                                     seq_table_rrc_recfg_sidelink_v1800_ies_s_opts,
                                                                     7},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_rrc_recfg_sidelink_v1800_ies_s_names};

// RRCReconfigurationSidelink-v1700-IEs ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_drx_cfg_uc_pc5_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_drx_cfg_uc_r17_s::desc}};
static const asn1::table::field_name
    seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_drx_cfg_uc_pc5_r17_setup_release_names[] = {{"release", nullptr},
                                                                                            {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_drx_cfg_uc_pc5_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_drx_cfg_uc_pc5_r17_setup_release_alternatives,
    nullptr,
    seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_drx_cfg_uc_pc5_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_drx_cfg_uc_r17_s>::type_offset(),
    setup_release_c<sl_drx_cfg_uc_r17_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_latency_bound_iuc_report_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(3),
         UINT64_C(160),
         nullptr}};
static const asn1::table::field_name
    seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_latency_bound_iuc_report_r17_setup_release_names[] = {
        {"release", nullptr},
        {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_latency_bound_iuc_report_r17_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_latency_bound_iuc_report_r17_setup_release_alternatives,
        nullptr,
        seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_latency_bound_iuc_report_r17_setup_release_names,
        "setup_release_c",
        setup_release_c<integer<uint8_t, 3, 160>>::type_offset(),
        setup_release_c<integer<uint8_t, 3, 160>>::value_offset()};
static const asn1::table::seq_field seq_table_rrc_recfg_sidelink_v1700_ies_s_fields[] = {
    {offsetof(rrc_recfg_sidelink_v1700_ies_s, sl_drx_cfg_uc_pc5_r17),
     (int32_t)offsetof(rrc_recfg_sidelink_v1700_ies_s, sl_drx_cfg_uc_pc5_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_drx_cfg_uc_pc5_r17_setup_release},
    {offsetof(rrc_recfg_sidelink_v1700_ies_s, sl_latency_bound_iuc_report_r17),
     (int32_t)offsetof(rrc_recfg_sidelink_v1700_ies_s, sl_latency_bound_iuc_report_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_rrc_recfg_sidelink_v1700_ies_s_sl_latency_bound_iuc_report_r17_setup_release},
    {offsetof(rrc_recfg_sidelink_v1700_ies_s, sl_rlc_ch_to_release_list_pc5_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::list_codec<decltype(rrc_recfg_sidelink_v1700_ies_s::sl_rlc_ch_to_release_list_pc5_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(512), false, false>>::value},
    {offsetof(rrc_recfg_sidelink_v1700_ies_s, sl_rlc_ch_to_add_mod_list_pc5_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::item_list_codec<decltype(rrc_recfg_sidelink_v1700_ies_s::sl_rlc_ch_to_add_mod_list_pc5_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_rlc_ch_cfg_pc5_r17_s::desc},
    {offsetof(rrc_recfg_sidelink_v1700_ies_s, non_crit_ext),
     (int32_t)offsetof(rrc_recfg_sidelink_v1700_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_sidelink_v1800_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_recfg_sidelink_v1700_ies_s_names[] = {
    {"sl-DRX-ConfigUC-PC5-r17", nullptr},
    {"sl-LatencyBoundIUC-Report-r17", nullptr},
    {"sl-RLC-ChannelToReleaseListPC5-r17", nullptr},
    {"sl-RLC-ChannelToAddModListPC5-r17", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_rrc_recfg_sidelink_v1700_ies_s_opts[] = {
    {0, (int32_t)offsetof(rrc_recfg_sidelink_v1700_ies_s, sl_drx_cfg_uc_pc5_r17_present)},
    {1, (int32_t)offsetof(rrc_recfg_sidelink_v1700_ies_s, sl_latency_bound_iuc_report_r17_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(rrc_recfg_sidelink_v1700_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_recfg_sidelink_v1700_ies_s::desc = {{seq_table_rrc_recfg_sidelink_v1700_ies_s_fields,
                                                                     5,
                                                                     UINT64_C(0x0),
                                                                     seq_table_rrc_recfg_sidelink_v1700_ies_s_opts,
                                                                     5},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_rrc_recfg_sidelink_v1700_ies_s_names};

// RRCReconfigurationSidelink-r16-IEs ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_rrc_recfg_sidelink_r16_ies_s_sl_meas_cfg_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_meas_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_rrc_recfg_sidelink_r16_ies_s_sl_meas_cfg_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_rrc_recfg_sidelink_r16_ies_s_sl_meas_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_rrc_recfg_sidelink_r16_ies_s_sl_meas_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_rrc_recfg_sidelink_r16_ies_s_sl_meas_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_meas_cfg_r16_s>::type_offset(),
    setup_release_c<sl_meas_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_rrc_recfg_sidelink_r16_ies_s_sl_csi_rs_cfg_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_csi_rs_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_rrc_recfg_sidelink_r16_ies_s_sl_csi_rs_cfg_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_rrc_recfg_sidelink_r16_ies_s_sl_csi_rs_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_rrc_recfg_sidelink_r16_ies_s_sl_csi_rs_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_rrc_recfg_sidelink_r16_ies_s_sl_csi_rs_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_csi_rs_cfg_r16_s>::type_offset(),
    setup_release_c<sl_csi_rs_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_rrc_recfg_sidelink_r16_ies_s_fields[] = {
    {offsetof(rrc_recfg_sidelink_r16_ies_s, slrb_cfg_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::item_list_codec<decltype(rrc_recfg_sidelink_r16_ies_s::slrb_cfg_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &slrb_cfg_r16_s::desc},
    {offsetof(rrc_recfg_sidelink_r16_ies_s, slrb_cfg_to_release_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::list_codec<decltype(rrc_recfg_sidelink_r16_ies_s::slrb_cfg_to_release_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(512), false, false>>::value},
    {offsetof(rrc_recfg_sidelink_r16_ies_s, sl_meas_cfg_r16),
     (int32_t)offsetof(rrc_recfg_sidelink_r16_ies_s, sl_meas_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_rrc_recfg_sidelink_r16_ies_s_sl_meas_cfg_r16_setup_release},
    {offsetof(rrc_recfg_sidelink_r16_ies_s, sl_csi_rs_cfg_r16),
     (int32_t)offsetof(rrc_recfg_sidelink_r16_ies_s, sl_csi_rs_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_rrc_recfg_sidelink_r16_ies_s_sl_csi_rs_cfg_r16_setup_release},
    {0,
     (int32_t)offsetof(rrc_recfg_sidelink_r16_ies_s, sl_reset_cfg_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(rrc_recfg_sidelink_r16_ies_s, sl_latency_bound_csi_report_r16),
     (int32_t)offsetof(rrc_recfg_sidelink_r16_ies_s, sl_latency_bound_csi_report_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(3),
     UINT64_C(160),
     nullptr},
    {offsetof(rrc_recfg_sidelink_r16_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(rrc_recfg_sidelink_r16_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {offsetof(rrc_recfg_sidelink_r16_ies_s, non_crit_ext),
     (int32_t)offsetof(rrc_recfg_sidelink_r16_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_sidelink_v1700_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_recfg_sidelink_r16_ies_s_names[] = {
    {"slrb-ConfigToAddModList-r16", nullptr},
    {"slrb-ConfigToReleaseList-r16", nullptr},
    {"sl-MeasConfig-r16", nullptr},
    {"sl-CSI-RS-Config-r16", nullptr},
    {"sl-ResetConfig-r16", "true"},
    {"sl-LatencyBoundCSI-Report-r16", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_rrc_recfg_sidelink_r16_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(rrc_recfg_sidelink_r16_ies_s, sl_meas_cfg_r16_present)},
    {3, (int32_t)offsetof(rrc_recfg_sidelink_r16_ies_s, sl_csi_rs_cfg_r16_present)},
    {4, (int32_t)offsetof(rrc_recfg_sidelink_r16_ies_s, sl_reset_cfg_r16_present)},
    {5, (int32_t)offsetof(rrc_recfg_sidelink_r16_ies_s, sl_latency_bound_csi_report_r16_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, (int32_t)offsetof(rrc_recfg_sidelink_r16_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_recfg_sidelink_r16_ies_s::desc = {
    {seq_table_rrc_recfg_sidelink_r16_ies_s_fields, 8, UINT64_C(0x0), seq_table_rrc_recfg_sidelink_r16_ies_s_opts, 8},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_recfg_sidelink_r16_ies_s_names};

// RRCReconfigurationSidelink ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_recfg_sidelink_s_fields[] = {
    {offsetof(rrc_recfg_sidelink_s, rrc_transaction_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(rrc_recfg_sidelink_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_sidelink_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_rrc_recfg_sidelink_s_names[] = {
    {"rrc-TransactionIdentifier-r16", nullptr},
    {"criticalExtensions", nullptr}};
const asn1::table::seq_desc rrc_recfg_sidelink_s::desc = {
    {seq_table_rrc_recfg_sidelink_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_recfg_sidelink_s_names};

void rrc_recfg_sidelink_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
rrc_recfg_sidelink_r16_ies_s& rrc_recfg_sidelink_s::crit_exts_c_::set_rrc_recfg_sidelink_r16()
{
  set(types::rrc_recfg_sidelink_r16);
  return c;
}
void rrc_recfg_sidelink_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_rrc_recfg_sidelink_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_sidelink_r16_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_rrc_recfg_sidelink_s_crit_exts_c__names[] = {
    {"rrcReconfigurationSidelink-r16", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc rrc_recfg_sidelink_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<rrc_recfg_sidelink_s::crit_exts_c_::types>(),
    rrc_recfg_sidelink_s::crit_exts_c_::types::nof_types,
    rrc_recfg_sidelink_s::crit_exts_c_::types::nof_exts,
    rrc_recfg_sidelink_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_rrc_recfg_sidelink_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<rrc_recfg_sidelink_s::crit_exts_c_>,
    choice_table_rrc_recfg_sidelink_s_crit_exts_c__names,
    "rrc_recfg_sidelink_s::crit_exts_c_",
    offsetof(rrc_recfg_sidelink_s::crit_exts_c_, type_),
    offsetof(rrc_recfg_sidelink_s::crit_exts_c_, c),
    false};

static const char* enum_names_rrc_recfg_sidelink_s_crit_exts_c__types_values[]    = {"rrcReconfigurationSidelink-r16",
                                                                                     "criticalExtensionsFuture"};
const asn1::table::enum_desc rrc_recfg_sidelink_s::crit_exts_c_::types_opts::desc = {
    enum_names_rrc_recfg_sidelink_s_crit_exts_c__types_values,
    2,
    "rrc_recfg_sidelink_s::crit_exts_c_::types"};

// SL-SIB-ReqInfo-r17 ::= ENUMERATED
static const char* enum_names_sl_sib_req_info_r17_e_values[] = {
    "sib1",        "sib2",        "sib3",       "sib4",       "sib5",          "sib6",       "sib7",
    "sib8",        "sib9",        "sib10",      "sib11",      "sib12",         "sib13",      "sib14",
    "sib15",       "sib16",       "sib17",      "sib18",      "sib19",         "sib20",      "sib21",
    "sibNotReq11", "sibNotReq10", "sibNotReq9", "sibNotReq8", "sibNotReq7",    "sibNotReq6", "sibNotReq5",
    "sibNotReq4",  "sibNotReq3",  "sibNotReq2", "sibNotReq1", "sib17bis-v1820"};
const asn1::table::enum_desc sl_sib_req_info_r17_opts::desc = {enum_names_sl_sib_req_info_r17_e_values,
                                                               33,
                                                               "sl_sib_req_info_r17_e"};

// SL-PagingInfo-RemoteUE-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_paging_info_remote_ue_r17_s_fields[] = {
    {offsetof(sl_paging_info_remote_ue_r17_s, sl_paging_id_remote_ue_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_paging_id_remote_ue_r17_s::desc},
    {offsetof(sl_paging_info_remote_ue_r17_s, sl_paging_cycle_remote_ue_r17),
     (int32_t)offsetof(sl_paging_info_remote_ue_r17_s, sl_paging_cycle_remote_ue_r17_present),
     asn1::table::enum_op<decltype(sl_paging_info_remote_ue_r17_s::sl_paging_cycle_remote_ue_r17)>(),
     decltype(sl_paging_info_remote_ue_r17_s::sl_paging_cycle_remote_ue_r17)::has_ext,
     false,
     decltype(sl_paging_info_remote_ue_r17_s::sl_paging_cycle_remote_ue_r17)::nof_types,
     decltype(sl_paging_info_remote_ue_r17_s::sl_paging_cycle_remote_ue_r17)::nof_exts,
     nullptr,
     &paging_cycle_opts::desc}};
static const asn1::table::field_name seq_table_sl_paging_info_remote_ue_r17_s_names[] = {
    {"sl-PagingIdentityRemoteUE-r17", nullptr},
    {"sl-PagingCycleRemoteUE-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sl_paging_info_remote_ue_r17_s_opts[] = {
    {1, (int32_t)offsetof(sl_paging_info_remote_ue_r17_s, sl_paging_cycle_remote_ue_r17_present)}};
const asn1::table::seq_desc sl_paging_info_remote_ue_r17_s::desc = {{seq_table_sl_paging_info_remote_ue_r17_s_fields,
                                                                     2,
                                                                     UINT64_C(0x1),
                                                                     seq_table_sl_paging_info_remote_ue_r17_s_opts,
                                                                     1},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_sl_paging_info_remote_ue_r17_s_names};

// SL-PosSIB-ReqInfo-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pos_sib_req_info_r18_s_fields[] = {
    {offsetof(sl_pos_sib_req_info_r18_s, gnss_id_r18),
     (int32_t)offsetof(sl_pos_sib_req_info_r18_s, gnss_id_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnss_id_r16_s::desc},
    {offsetof(sl_pos_sib_req_info_r18_s, sbas_id_r18),
     (int32_t)offsetof(sl_pos_sib_req_info_r18_s, sbas_id_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sbas_id_r16_s::desc},
    {offsetof(sl_pos_sib_req_info_r18_s, pos_sib_type_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_pos_sib_req_info_r18_s::pos_sib_type_r18)>(),
     decltype(sl_pos_sib_req_info_r18_s::pos_sib_type_r18)::has_ext,
     false,
     decltype(sl_pos_sib_req_info_r18_s::pos_sib_type_r18)::nof_types,
     decltype(sl_pos_sib_req_info_r18_s::pos_sib_type_r18)::nof_exts,
     nullptr,
     &sl_pos_sib_req_info_r18_s::pos_sib_type_r18_opts::desc}};
static const asn1::table::field_name seq_table_sl_pos_sib_req_info_r18_s_names[] = {{"gnss-id-r18", nullptr},
                                                                                    {"sbas-id-r18", nullptr},
                                                                                    {"posSibType-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_sl_pos_sib_req_info_r18_s_opts[]  = {
    {0, (int32_t)offsetof(sl_pos_sib_req_info_r18_s, gnss_id_r18_present)},
    {1, (int32_t)offsetof(sl_pos_sib_req_info_r18_s, sbas_id_r18_present)}};
const asn1::table::seq_desc sl_pos_sib_req_info_r18_s::desc = {
    {seq_table_sl_pos_sib_req_info_r18_s_fields, 3, UINT64_C(0x4), seq_table_sl_pos_sib_req_info_r18_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_pos_sib_req_info_r18_s_names};

static const char* enum_names_sl_pos_sib_req_info_r18_s_pos_sib_type_r18_e__values[] = {
    "posSibType1-1",  "posSibType1-2",   "posSibType1-3",  "posSibType1-4",  "posSibType1-5",   "posSibType1-6",
    "posSibType1-7",  "posSibType1-8",   "posSibType1-9",  "posSibType1-10", "posSibType1-11",  "posSibType1-12",
    "posSibType2-1",  "posSibType2-2",   "posSibType2-3",  "posSibType2-4",  "posSibType2-5",   "posSibType2-6",
    "posSibType2-7",  "posSibType2-8",   "posSibType2-9",  "posSibType2-10", "posSibType2-11",  "posSibType2-12",
    "posSibType2-13", "posSibType2-14",  "posSibType2-15", "posSibType2-16", "posSibType2-17",  "posSibType2-17a",
    "posSibType2-18", "posSibType2-18a", "posSibType2-19", "posSibType2-20", "posSibType2-20a", "posSibType2-21",
    "posSibType2-22", "posSibType2-23",  "posSibType2-24", "posSibType2-25", "posSibType2-26",  "posSibType2-27",
    "posSibType3-1",  "posSibType4-1",   "posSibType5-1",  "posSibType6-1",  "posSibType6-2",   "posSibType6-3",
    "posSibType6-4",  "posSibType6-5",   "posSibType6-6",  "posSibType6-7",  "posSibType7-1",   "posSibType7-2",
    "posSibType7-3",  "posSibType7-4",   "spare9",         "spare8",         "spare7",          "spare6",
    "spare5",         "spare4",          "spare3",         "spare2",         "spare1"};
const asn1::table::enum_desc sl_pos_sib_req_info_r18_s::pos_sib_type_r18_opts::desc = {
    enum_names_sl_pos_sib_req_info_r18_s_pos_sib_type_r18_e__values,
    65,
    "sl_pos_sib_req_info_r18_s::pos_sib_type_r18_e_"};

// RemoteUEInformationSidelink-v1800-IEs ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_remote_ue_info_sidelink_v1800_ies_s_sl_requested_pos_sib_list_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         32,
         &asn1::table::item_list_codec<setup_release_c<dyn_seq_of<sl_pos_sib_req_info_r18_s, 1, 32>>::value_type,
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
         &sl_pos_sib_req_info_r18_s::desc}};
static const asn1::table::field_name
    seq_table_remote_ue_info_sidelink_v1800_ies_s_sl_requested_pos_sib_list_r18_setup_release_names[] = {
        {"release", nullptr},
        {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_remote_ue_info_sidelink_v1800_ies_s_sl_requested_pos_sib_list_r18_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_remote_ue_info_sidelink_v1800_ies_s_sl_requested_pos_sib_list_r18_setup_release_alternatives,
        nullptr,
        seq_table_remote_ue_info_sidelink_v1800_ies_s_sl_requested_pos_sib_list_r18_setup_release_names,
        "setup_release_c",
        setup_release_c<dyn_seq_of<sl_pos_sib_req_info_r18_s, 1, 32>>::type_offset(),
        setup_release_c<dyn_seq_of<sl_pos_sib_req_info_r18_s, 1, 32>>::value_offset()};
static const asn1::table::seq_field seq_table_remote_ue_info_sidelink_v1800_ies_s_fields[] = {
    {offsetof(remote_ue_info_sidelink_v1800_ies_s, sl_requested_pos_sib_list_r18),
     (int32_t)offsetof(remote_ue_info_sidelink_v1800_ies_s, sl_requested_pos_sib_list_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_remote_ue_info_sidelink_v1800_ies_s_sl_requested_pos_sib_list_r18_setup_release},
    {0,
     (int32_t)offsetof(remote_ue_info_sidelink_v1800_ies_s, sl_sfn_dfn_offset_requested_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(remote_ue_info_sidelink_v1800_ies_s, conn_for_mp_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(remote_ue_info_sidelink_v1800_ies_s, sl_dest_id_remote_ue_r18),
     (int32_t)offsetof(remote_ue_info_sidelink_v1800_ies_s, sl_dest_id_remote_ue_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(remote_ue_info_sidelink_v1800_ies_s::sl_dest_id_remote_ue_r18),
                                asn1::table::struct_json>::value},
    {0,
     (int32_t)offsetof(remote_ue_info_sidelink_v1800_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_remote_ue_info_sidelink_v1800_ies_s_names[] = {
    {"sl-RequestedPosSIB-List-r18", nullptr},
    {"sl-SFN-DFN-OffsetRequested-r18", "true"},
    {"connectionForMP-r18", "true"},
    {"sl-DestinationIdentityRemoteUE-r18", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_remote_ue_info_sidelink_v1800_ies_s_opts[] = {
    {0, (int32_t)offsetof(remote_ue_info_sidelink_v1800_ies_s, sl_requested_pos_sib_list_r18_present)},
    {1, (int32_t)offsetof(remote_ue_info_sidelink_v1800_ies_s, sl_sfn_dfn_offset_requested_r18_present)},
    {2, (int32_t)offsetof(remote_ue_info_sidelink_v1800_ies_s, conn_for_mp_r18_present)},
    {3, (int32_t)offsetof(remote_ue_info_sidelink_v1800_ies_s, sl_dest_id_remote_ue_r18_present)},
    {4, (int32_t)offsetof(remote_ue_info_sidelink_v1800_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc remote_ue_info_sidelink_v1800_ies_s::desc = {
    {seq_table_remote_ue_info_sidelink_v1800_ies_s_fields,
     5,
     UINT64_C(0x0),
     seq_table_remote_ue_info_sidelink_v1800_ies_s_opts,
     5},
    false,
    false,
    nullptr,
    0,
    seq_table_remote_ue_info_sidelink_v1800_ies_s_names};

// RemoteUEInformationSidelink-r17-IEs ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_remote_ue_info_sidelink_r17_ies_s_sl_requested_sib_list_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         33,
         33,
         &asn1::table::list_codec<setup_release_c<sl_requested_sib_list_r17_l>::value_type,
                                  asn1::table::string_json,
                                  asn1::table::object_items<true>>::value}};
static const asn1::table::field_name
    seq_table_remote_ue_info_sidelink_r17_ies_s_sl_requested_sib_list_r17_setup_release_names[] = {{"release", nullptr},
                                                                                                   {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_remote_ue_info_sidelink_r17_ies_s_sl_requested_sib_list_r17_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_remote_ue_info_sidelink_r17_ies_s_sl_requested_sib_list_r17_setup_release_alternatives,
        nullptr,
        seq_table_remote_ue_info_sidelink_r17_ies_s_sl_requested_sib_list_r17_setup_release_names,
        "setup_release_c",
        setup_release_c<sl_requested_sib_list_r17_l>::type_offset(),
        setup_release_c<sl_requested_sib_list_r17_l>::value_offset()};
static const asn1::table::seq_field
    seq_table_remote_ue_info_sidelink_r17_ies_s_sl_paging_info_remote_ue_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sl_paging_info_remote_ue_r17_s::desc}};
static const asn1::table::field_name
    seq_table_remote_ue_info_sidelink_r17_ies_s_sl_paging_info_remote_ue_r17_setup_release_names[] = {
        {"release", nullptr},
        {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_remote_ue_info_sidelink_r17_ies_s_sl_paging_info_remote_ue_r17_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_remote_ue_info_sidelink_r17_ies_s_sl_paging_info_remote_ue_r17_setup_release_alternatives,
        nullptr,
        seq_table_remote_ue_info_sidelink_r17_ies_s_sl_paging_info_remote_ue_r17_setup_release_names,
        "setup_release_c",
        setup_release_c<sl_paging_info_remote_ue_r17_s>::type_offset(),
        setup_release_c<sl_paging_info_remote_ue_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_remote_ue_info_sidelink_r17_ies_s_fields[] = {
    {offsetof(remote_ue_info_sidelink_r17_ies_s, sl_requested_sib_list_r17),
     (int32_t)offsetof(remote_ue_info_sidelink_r17_ies_s, sl_requested_sib_list_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_remote_ue_info_sidelink_r17_ies_s_sl_requested_sib_list_r17_setup_release},
    {offsetof(remote_ue_info_sidelink_r17_ies_s, sl_paging_info_remote_ue_r17),
     (int32_t)offsetof(remote_ue_info_sidelink_r17_ies_s, sl_paging_info_remote_ue_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_remote_ue_info_sidelink_r17_ies_s_sl_paging_info_remote_ue_r17_setup_release},
    {offsetof(remote_ue_info_sidelink_r17_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(remote_ue_info_sidelink_r17_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {offsetof(remote_ue_info_sidelink_r17_ies_s, non_crit_ext),
     (int32_t)offsetof(remote_ue_info_sidelink_r17_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &remote_ue_info_sidelink_v1800_ies_s::desc}};
static const asn1::table::field_name seq_table_remote_ue_info_sidelink_r17_ies_s_names[] = {
    {"sl-RequestedSIB-List-r17", nullptr},
    {"sl-PagingInfo-RemoteUE-r17", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_remote_ue_info_sidelink_r17_ies_s_opts[] = {
    {0, (int32_t)offsetof(remote_ue_info_sidelink_r17_ies_s, sl_requested_sib_list_r17_present)},
    {1, (int32_t)offsetof(remote_ue_info_sidelink_r17_ies_s, sl_paging_info_remote_ue_r17_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(remote_ue_info_sidelink_r17_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc remote_ue_info_sidelink_r17_ies_s::desc = {
    {seq_table_remote_ue_info_sidelink_r17_ies_s_fields,
     4,
     UINT64_C(0x0),
     seq_table_remote_ue_info_sidelink_r17_ies_s_opts,
     4},
    false,
    false,
    nullptr,
    0,
    seq_table_remote_ue_info_sidelink_r17_ies_s_names};

// RemoteUEInformationSidelink-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_remote_ue_info_sidelink_r17_s_fields[] = {
    {offsetof(remote_ue_info_sidelink_r17_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &remote_ue_info_sidelink_r17_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_remote_ue_info_sidelink_r17_s_names[] = {
    {"criticalExtensions", nullptr}};
const asn1::table::seq_desc remote_ue_info_sidelink_r17_s::desc = {
    {seq_table_remote_ue_info_sidelink_r17_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_remote_ue_info_sidelink_r17_s_names};

void remote_ue_info_sidelink_r17_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
remote_ue_info_sidelink_r17_ies_s& remote_ue_info_sidelink_r17_s::crit_exts_c_::set_remote_ue_info_sidelink_r17()
{
  set(types::remote_ue_info_sidelink_r17);
  return c;
}
void remote_ue_info_sidelink_r17_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_remote_ue_info_sidelink_r17_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &remote_ue_info_sidelink_r17_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_remote_ue_info_sidelink_r17_s_crit_exts_c__names[] = {
    {"remoteUEInformationSidelink-r17", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc remote_ue_info_sidelink_r17_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<remote_ue_info_sidelink_r17_s::crit_exts_c_::types>(),
    remote_ue_info_sidelink_r17_s::crit_exts_c_::types::nof_types,
    remote_ue_info_sidelink_r17_s::crit_exts_c_::types::nof_exts,
    remote_ue_info_sidelink_r17_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_remote_ue_info_sidelink_r17_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<remote_ue_info_sidelink_r17_s::crit_exts_c_>,
    choice_table_remote_ue_info_sidelink_r17_s_crit_exts_c__names,
    "remote_ue_info_sidelink_r17_s::crit_exts_c_",
    offsetof(remote_ue_info_sidelink_r17_s::crit_exts_c_, type_),
    offsetof(remote_ue_info_sidelink_r17_s::crit_exts_c_, c),
    false};

static const char* enum_names_remote_ue_info_sidelink_r17_s_crit_exts_c__types_values[] = {
    "remoteUEInformationSidelink-r17",
    "criticalExtensionsFuture"};
const asn1::table::enum_desc remote_ue_info_sidelink_r17_s::crit_exts_c_::types_opts::desc = {
    enum_names_remote_ue_info_sidelink_r17_s_crit_exts_c__types_values,
    2,
    "remote_ue_info_sidelink_r17_s::crit_exts_c_::types"};

// SBCCH-SL-BCH-MessageType ::= CHOICE
void sbcch_sl_bch_msg_type_c::set(types::options e)
{
  type_ = e;
}
sbcch_sl_bch_msg_type_c::c1_c_& sbcch_sl_bch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void sbcch_sl_bch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
static const asn1::table::seq_field choice_table_sbcch_sl_bch_msg_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sbcch_sl_bch_msg_type_c::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_sbcch_sl_bch_msg_type_c_names[] = {
    {"c1", nullptr},
    {"messageClassExtension", nullptr}};
const asn1::table::choice_desc sbcch_sl_bch_msg_type_c::desc = {
    asn1::table::enum_int_type<sbcch_sl_bch_msg_type_c::types>(),
    sbcch_sl_bch_msg_type_c::types::nof_types,
    sbcch_sl_bch_msg_type_c::types::nof_exts,
    sbcch_sl_bch_msg_type_c::types::has_ext,
    false,
    choice_table_sbcch_sl_bch_msg_type_c_alternatives,
    &asn1::table::choice_set<sbcch_sl_bch_msg_type_c>,
    choice_table_sbcch_sl_bch_msg_type_c_names,
    "sbcch_sl_bch_msg_type_c",
    offsetof(sbcch_sl_bch_msg_type_c, type_),
    offsetof(sbcch_sl_bch_msg_type_c, c),
    false};

void sbcch_sl_bch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
}
mib_sidelink_s& sbcch_sl_bch_msg_type_c::c1_c_::set_mib_sidelink()
{
  set(types::mib_sidelink);
  return c;
}
void sbcch_sl_bch_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_sbcch_sl_bch_msg_type_c_c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mib_sidelink_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_sbcch_sl_bch_msg_type_c_c1_c__names[] = {
    {"masterInformationBlockSidelink", nullptr},
    {"spare1", nullptr}};
const asn1::table::choice_desc sbcch_sl_bch_msg_type_c::c1_c_::desc = {
    asn1::table::enum_int_type<sbcch_sl_bch_msg_type_c::c1_c_::types>(),
    sbcch_sl_bch_msg_type_c::c1_c_::types::nof_types,
    sbcch_sl_bch_msg_type_c::c1_c_::types::nof_exts,
    sbcch_sl_bch_msg_type_c::c1_c_::types::has_ext,
    false,
    choice_table_sbcch_sl_bch_msg_type_c_c1_c__alternatives,
    &asn1::table::choice_set<sbcch_sl_bch_msg_type_c::c1_c_>,
    choice_table_sbcch_sl_bch_msg_type_c_c1_c__names,
    "sbcch_sl_bch_msg_type_c::c1_c_",
    offsetof(sbcch_sl_bch_msg_type_c::c1_c_, type_),
    offsetof(sbcch_sl_bch_msg_type_c::c1_c_, c),
    false};

static const char* enum_names_sbcch_sl_bch_msg_type_c_c1_c__types_values[]    = {"masterInformationBlockSidelink",
                                                                                 "spare1"};
const asn1::table::enum_desc sbcch_sl_bch_msg_type_c::c1_c_::types_opts::desc = {
    enum_names_sbcch_sl_bch_msg_type_c_c1_c__types_values,
    2,
    "sbcch_sl_bch_msg_type_c::c1_c_::types"};

static const char*           enum_names_sbcch_sl_bch_msg_type_c_types_values[] = {"c1", "messageClassExtension"};
const asn1::table::enum_desc sbcch_sl_bch_msg_type_c::types_opts::desc         = {
    enum_names_sbcch_sl_bch_msg_type_c_types_values,
    2,
    "sbcch_sl_bch_msg_type_c::types"};
uint8_t sbcch_sl_bch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "sbcch_sl_bch_msg_type_c::types");
}

// SBCCH-SL-BCH-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_sbcch_sl_bch_msg_s_fields[] = {{offsetof(sbcch_sl_bch_msg_s, msg),
                                                                               asn1::table::seq_field::mandatory,
                                                                               asn1::table::field_op::choice,
                                                                               false,
                                                                               false,
                                                                               0,
                                                                               0,
                                                                               nullptr,
                                                                               &sbcch_sl_bch_msg_type_c::desc}};
static const asn1::table::field_name seq_table_sbcch_sl_bch_msg_s_names[]  = {{"message", nullptr}};
const asn1::table::seq_desc          sbcch_sl_bch_msg_s::desc              = {
    {seq_table_sbcch_sl_bch_msg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sbcch_sl_bch_msg_s_names,
    "SBCCH-SL-BCH-Message"};

// UECapabilityEnquirySidelink-r16-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_enquiry_sidelink_r16_ies_s_fields[] = {
    {offsetof(ue_cap_enquiry_sidelink_r16_ies_s, freq_band_list_filt_sidelink_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1280,
     &asn1::table::item_list_codec<decltype(ue_cap_enquiry_sidelink_r16_ies_s::freq_band_list_filt_sidelink_r16),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &freq_band_info_c::desc},
    {offsetof(ue_cap_enquiry_sidelink_r16_ies_s, ue_cap_info_sidelink_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(ue_cap_enquiry_sidelink_r16_ies_s::ue_cap_info_sidelink_r16),
                                      asn1::table::string_json>::value},
    {offsetof(ue_cap_enquiry_sidelink_r16_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(ue_cap_enquiry_sidelink_r16_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(ue_cap_enquiry_sidelink_r16_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_cap_enquiry_sidelink_r16_ies_s_names[] = {
    {"frequencyBandListFilterSidelink-r16", nullptr},
    {"ue-CapabilityInformationSidelink-r16", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_cap_enquiry_sidelink_r16_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(ue_cap_enquiry_sidelink_r16_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_cap_enquiry_sidelink_r16_ies_s::desc = {
    {seq_table_ue_cap_enquiry_sidelink_r16_ies_s_fields,
     4,
     UINT64_C(0x0),
     seq_table_ue_cap_enquiry_sidelink_r16_ies_s_opts,
     4},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_enquiry_sidelink_r16_ies_s_names};

// UECapabilityEnquirySidelink ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_enquiry_sidelink_s_fields[] = {
    {offsetof(ue_cap_enquiry_sidelink_s, rrc_transaction_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(ue_cap_enquiry_sidelink_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_enquiry_sidelink_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_ue_cap_enquiry_sidelink_s_names[] = {
    {"rrc-TransactionIdentifier-r16", nullptr},
    {"criticalExtensions", nullptr}};
const asn1::table::seq_desc ue_cap_enquiry_sidelink_s::desc = {
    {seq_table_ue_cap_enquiry_sidelink_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_enquiry_sidelink_s_names};

void ue_cap_enquiry_sidelink_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_cap_enquiry_sidelink_r16_ies_s& ue_cap_enquiry_sidelink_s::crit_exts_c_::set_ue_cap_enquiry_sidelink_r16()
{
  set(types::ue_cap_enquiry_sidelink_r16);
  return c;
}
void ue_cap_enquiry_sidelink_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_ue_cap_enquiry_sidelink_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_enquiry_sidelink_r16_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ue_cap_enquiry_sidelink_s_crit_exts_c__names[] = {
    {"ueCapabilityEnquirySidelink-r16", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc ue_cap_enquiry_sidelink_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<ue_cap_enquiry_sidelink_s::crit_exts_c_::types>(),
    ue_cap_enquiry_sidelink_s::crit_exts_c_::types::nof_types,
    ue_cap_enquiry_sidelink_s::crit_exts_c_::types::nof_exts,
    ue_cap_enquiry_sidelink_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_ue_cap_enquiry_sidelink_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<ue_cap_enquiry_sidelink_s::crit_exts_c_>,
    choice_table_ue_cap_enquiry_sidelink_s_crit_exts_c__names,
    "ue_cap_enquiry_sidelink_s::crit_exts_c_",
    offsetof(ue_cap_enquiry_sidelink_s::crit_exts_c_, type_),
    offsetof(ue_cap_enquiry_sidelink_s::crit_exts_c_, c),
    false};

static const char* enum_names_ue_cap_enquiry_sidelink_s_crit_exts_c__types_values[] = {
    "ueCapabilityEnquirySidelink-r16",
    "criticalExtensionsFuture"};
const asn1::table::enum_desc ue_cap_enquiry_sidelink_s::crit_exts_c_::types_opts::desc = {
    enum_names_ue_cap_enquiry_sidelink_s_crit_exts_c__types_values,
    2,
    "ue_cap_enquiry_sidelink_s::crit_exts_c_::types"};

// UECapabilityInformationSidelink-v1800-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_info_sidelink_v1800_ies_s_fields[] = {
    {0,
     (int32_t)offsetof(ue_cap_info_sidelink_v1800_ies_s, sfn_dfn_offset_supported_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_cap_info_sidelink_v1800_ies_s, pos_sib_forwarding_supported_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_cap_info_sidelink_v1800_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_cap_info_sidelink_v1800_ies_s_names[] = {
    {"sfn-DFN-OffsetSupported-r18", "supported"},
    {"posSIB-ForwardingSupported-r18", "supported"},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_cap_info_sidelink_v1800_ies_s_opts[] = {
    {0, (int32_t)offsetof(ue_cap_info_sidelink_v1800_ies_s, sfn_dfn_offset_supported_r18_present)},
    {1, (int32_t)offsetof(ue_cap_info_sidelink_v1800_ies_s, pos_sib_forwarding_supported_r18_present)},
    {2, (int32_t)offsetof(ue_cap_info_sidelink_v1800_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_cap_info_sidelink_v1800_ies_s::desc = {
    {seq_table_ue_cap_info_sidelink_v1800_ies_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_ue_cap_info_sidelink_v1800_ies_s_opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_info_sidelink_v1800_ies_s_names};

// UECapabilityInformationSidelink-v1700-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_info_sidelink_v1700_ies_s_fields[] = {
    {offsetof(ue_cap_info_sidelink_v1700_ies_s, mac_params_sidelink_r17),
     (int32_t)offsetof(ue_cap_info_sidelink_v1700_ies_s, mac_params_sidelink_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mac_params_sidelink_r17_s::desc},
    {offsetof(ue_cap_info_sidelink_v1700_ies_s, supported_band_combination_list_sidelink_nr_v1710),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65536,
     &asn1::table::list_codec<
         decltype(ue_cap_info_sidelink_v1700_ies_s::supported_band_combination_list_sidelink_nr_v1710),
         asn1::table::list_json<asn1::table::struct_json>,
         asn1::table::list_items<false, 1, 32, false, asn1::table::object_items<false>>>::value},
    {offsetof(ue_cap_info_sidelink_v1700_ies_s, non_crit_ext),
     (int32_t)offsetof(ue_cap_info_sidelink_v1700_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_info_sidelink_v1800_ies_s::desc}};
static const asn1::table::field_name seq_table_ue_cap_info_sidelink_v1700_ies_s_names[] = {
    {"mac-ParametersSidelink-r17", nullptr},
    {"supportedBandCombinationListSidelinkNR-v1710", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_cap_info_sidelink_v1700_ies_s_opts[] = {
    {0, (int32_t)offsetof(ue_cap_info_sidelink_v1700_ies_s, mac_params_sidelink_r17_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(ue_cap_info_sidelink_v1700_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_cap_info_sidelink_v1700_ies_s::desc = {
    {seq_table_ue_cap_info_sidelink_v1700_ies_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_ue_cap_info_sidelink_v1700_ies_s_opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_info_sidelink_v1700_ies_s_names};

// UECapabilityInformationSidelink-r16-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_info_sidelink_r16_ies_s_fields[] = {
    {offsetof(ue_cap_info_sidelink_r16_ies_s, access_stratum_release_sidelink_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ue_cap_info_sidelink_r16_ies_s::access_stratum_release_sidelink_r16)>(),
     decltype(ue_cap_info_sidelink_r16_ies_s::access_stratum_release_sidelink_r16)::has_ext,
     false,
     decltype(ue_cap_info_sidelink_r16_ies_s::access_stratum_release_sidelink_r16)::nof_types,
     decltype(ue_cap_info_sidelink_r16_ies_s::access_stratum_release_sidelink_r16)::nof_exts,
     nullptr,
     &access_stratum_release_sidelink_r16_opts::desc},
    {offsetof(ue_cap_info_sidelink_r16_ies_s, pdcp_params_sidelink_r16),
     (int32_t)offsetof(ue_cap_info_sidelink_r16_ies_s, pdcp_params_sidelink_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_params_sidelink_r16_s::desc},
    {offsetof(ue_cap_info_sidelink_r16_ies_s, rlc_params_sidelink_r16),
     (int32_t)offsetof(ue_cap_info_sidelink_r16_ies_s, rlc_params_sidelink_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rlc_params_sidelink_r16_s::desc},
    {offsetof(ue_cap_info_sidelink_r16_ies_s, supported_band_combination_list_sidelink_nr_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65536,
     &asn1::table::list_codec<decltype(ue_cap_info_sidelink_r16_ies_s::supported_band_combination_list_sidelink_nr_r16),
                              asn1::table::list_json<asn1::table::struct_json>,
                              asn1::table::list_items<false, 1, 32, false, asn1::table::object_items<false>>>::value},
    {offsetof(ue_cap_info_sidelink_r16_ies_s, supported_band_list_sidelink_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::item_list_codec<decltype(ue_cap_info_sidelink_r16_ies_s::supported_band_list_sidelink_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &band_sidelink_pc5_r16_s::desc},
    {offsetof(ue_cap_info_sidelink_r16_ies_s, applied_freq_band_list_filt_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1280,
     &asn1::table::item_list_codec<decltype(ue_cap_info_sidelink_r16_ies_s::applied_freq_band_list_filt_r16),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &freq_band_info_c::desc},
    {offsetof(ue_cap_info_sidelink_r16_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(ue_cap_info_sidelink_r16_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {offsetof(ue_cap_info_sidelink_r16_ies_s, non_crit_ext),
     (int32_t)offsetof(ue_cap_info_sidelink_r16_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_info_sidelink_v1700_ies_s::desc}};
static const asn1::table::field_name seq_table_ue_cap_info_sidelink_r16_ies_s_names[] = {
    {"accessStratumReleaseSidelink-r16", nullptr},
    {"pdcp-ParametersSidelink-r16", nullptr},
    {"rlc-ParametersSidelink-r16", nullptr},
    {"supportedBandCombinationListSidelinkNR-r16", nullptr},
    {"supportedBandListSidelink-r16", nullptr},
    {"appliedFreqBandListFilter-r16", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_cap_info_sidelink_r16_ies_s_opts[] = {
    {1, (int32_t)offsetof(ue_cap_info_sidelink_r16_ies_s, pdcp_params_sidelink_r16_present)},
    {2, (int32_t)offsetof(ue_cap_info_sidelink_r16_ies_s, rlc_params_sidelink_r16_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, asn1::table::seq_field::list_presence},
    {7, (int32_t)offsetof(ue_cap_info_sidelink_r16_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_cap_info_sidelink_r16_ies_s::desc = {{seq_table_ue_cap_info_sidelink_r16_ies_s_fields,
                                                                     8,
                                                                     UINT64_C(0x1),
                                                                     seq_table_ue_cap_info_sidelink_r16_ies_s_opts,
                                                                     7},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_ue_cap_info_sidelink_r16_ies_s_names};

// UECapabilityInformationSidelink ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_info_sidelink_s_fields[] = {
    {offsetof(ue_cap_info_sidelink_s, rrc_transaction_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(ue_cap_info_sidelink_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_info_sidelink_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_ue_cap_info_sidelink_s_names[] = {
    {"rrc-TransactionIdentifier-r16", nullptr},
    {"criticalExtensions", nullptr}};
const asn1::table::seq_desc ue_cap_info_sidelink_s::desc = {
    {seq_table_ue_cap_info_sidelink_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_info_sidelink_s_names};

void ue_cap_info_sidelink_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_cap_info_sidelink_r16_ies_s& ue_cap_info_sidelink_s::crit_exts_c_::set_ue_cap_info_sidelink_r16()
{
  set(types::ue_cap_info_sidelink_r16);
  return c;
}
void ue_cap_info_sidelink_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_ue_cap_info_sidelink_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_info_sidelink_r16_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ue_cap_info_sidelink_s_crit_exts_c__names[] = {
    {"ueCapabilityInformationSidelink-r16", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc ue_cap_info_sidelink_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<ue_cap_info_sidelink_s::crit_exts_c_::types>(),
    ue_cap_info_sidelink_s::crit_exts_c_::types::nof_types,
    ue_cap_info_sidelink_s::crit_exts_c_::types::nof_exts,
    ue_cap_info_sidelink_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_ue_cap_info_sidelink_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<ue_cap_info_sidelink_s::crit_exts_c_>,
    choice_table_ue_cap_info_sidelink_s_crit_exts_c__names,
    "ue_cap_info_sidelink_s::crit_exts_c_",
    offsetof(ue_cap_info_sidelink_s::crit_exts_c_, type_),
    offsetof(ue_cap_info_sidelink_s::crit_exts_c_, c),
    false};

static const char* enum_names_ue_cap_info_sidelink_s_crit_exts_c__types_values[] = {
    "ueCapabilityInformationSidelink-r16",
    "criticalExtensionsFuture"};
const asn1::table::enum_desc ue_cap_info_sidelink_s::crit_exts_c_::types_opts::desc = {
    enum_names_ue_cap_info_sidelink_s_crit_exts_c__types_values,
    2,
    "ue_cap_info_sidelink_s::crit_exts_c_::types"};

// UuMessageTransferSidelink-v1800-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_uu_msg_transfer_sidelink_v1800_ies_s_fields[] = {
    {offsetof(uu_msg_transfer_sidelink_v1800_ies_s, sl_paging_delivery_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(uu_msg_transfer_sidelink_v1800_ies_s::sl_paging_delivery_r18),
                                      asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(uu_msg_transfer_sidelink_v1800_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_uu_msg_transfer_sidelink_v1800_ies_s_names[] = {
    {"sl-PagingDelivery-r18", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_uu_msg_transfer_sidelink_v1800_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(uu_msg_transfer_sidelink_v1800_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc uu_msg_transfer_sidelink_v1800_ies_s::desc = {
    {seq_table_uu_msg_transfer_sidelink_v1800_ies_s_fields,
     2,
     UINT64_C(0x0),
     seq_table_uu_msg_transfer_sidelink_v1800_ies_s_opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_uu_msg_transfer_sidelink_v1800_ies_s_names};

// UuMessageTransferSidelink-r17-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_uu_msg_transfer_sidelink_r17_ies_s_fields[] = {
    {offsetof(uu_msg_transfer_sidelink_r17_ies_s, sl_paging_delivery_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(uu_msg_transfer_sidelink_r17_ies_s::sl_paging_delivery_r17),
                                      asn1::table::string_json>::value},
    {offsetof(uu_msg_transfer_sidelink_r17_ies_s, sl_sib1_delivery_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(uu_msg_transfer_sidelink_r17_ies_s::sl_sib1_delivery_r17),
                                      asn1::table::string_json>::value},
    {offsetof(uu_msg_transfer_sidelink_r17_ies_s, sl_sys_info_delivery_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(uu_msg_transfer_sidelink_r17_ies_s::sl_sys_info_delivery_r17),
                                      asn1::table::string_json>::value},
    {offsetof(uu_msg_transfer_sidelink_r17_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(uu_msg_transfer_sidelink_r17_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {offsetof(uu_msg_transfer_sidelink_r17_ies_s, non_crit_ext),
     (int32_t)offsetof(uu_msg_transfer_sidelink_r17_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &uu_msg_transfer_sidelink_v1800_ies_s::desc}};
static const asn1::table::field_name seq_table_uu_msg_transfer_sidelink_r17_ies_s_names[] = {
    {"sl-PagingDelivery-r17", nullptr},
    {"sl-SIB1-Delivery-r17", nullptr},
    {"sl-SystemInformationDelivery-r17", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_uu_msg_transfer_sidelink_r17_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(uu_msg_transfer_sidelink_r17_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc uu_msg_transfer_sidelink_r17_ies_s::desc = {
    {seq_table_uu_msg_transfer_sidelink_r17_ies_s_fields,
     5,
     UINT64_C(0x0),
     seq_table_uu_msg_transfer_sidelink_r17_ies_s_opts,
     5},
    false,
    false,
    nullptr,
    0,
    seq_table_uu_msg_transfer_sidelink_r17_ies_s_names};

// UuMessageTransferSidelink-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_uu_msg_transfer_sidelink_r17_s_fields[] = {
    {offsetof(uu_msg_transfer_sidelink_r17_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &uu_msg_transfer_sidelink_r17_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_uu_msg_transfer_sidelink_r17_s_names[] = {
    {"criticalExtensions", nullptr}};
const asn1::table::seq_desc uu_msg_transfer_sidelink_r17_s::desc = {
    {seq_table_uu_msg_transfer_sidelink_r17_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_uu_msg_transfer_sidelink_r17_s_names};

void uu_msg_transfer_sidelink_r17_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
uu_msg_transfer_sidelink_r17_ies_s& uu_msg_transfer_sidelink_r17_s::crit_exts_c_::set_uu_msg_transfer_sidelink_r17()
{
  set(types::uu_msg_transfer_sidelink_r17);
  return c;
}
void uu_msg_transfer_sidelink_r17_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_uu_msg_transfer_sidelink_r17_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &uu_msg_transfer_sidelink_r17_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_uu_msg_transfer_sidelink_r17_s_crit_exts_c__names[] = {
    {"uuMessageTransferSidelink-r17", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc uu_msg_transfer_sidelink_r17_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<uu_msg_transfer_sidelink_r17_s::crit_exts_c_::types>(),
    uu_msg_transfer_sidelink_r17_s::crit_exts_c_::types::nof_types,
    uu_msg_transfer_sidelink_r17_s::crit_exts_c_::types::nof_exts,
    uu_msg_transfer_sidelink_r17_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_uu_msg_transfer_sidelink_r17_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<uu_msg_transfer_sidelink_r17_s::crit_exts_c_>,
    choice_table_uu_msg_transfer_sidelink_r17_s_crit_exts_c__names,
    "uu_msg_transfer_sidelink_r17_s::crit_exts_c_",
    offsetof(uu_msg_transfer_sidelink_r17_s::crit_exts_c_, type_),
    offsetof(uu_msg_transfer_sidelink_r17_s::crit_exts_c_, c),
    false};

static const char* enum_names_uu_msg_transfer_sidelink_r17_s_crit_exts_c__types_values[] = {
    "uuMessageTransferSidelink-r17",
    "criticalExtensionsFuture"};
const asn1::table::enum_desc uu_msg_transfer_sidelink_r17_s::crit_exts_c_::types_opts::desc = {
    enum_names_uu_msg_transfer_sidelink_r17_s_crit_exts_c__types_values,
    2,
    "uu_msg_transfer_sidelink_r17_s::crit_exts_c_::types"};

// UEAssistanceInformationSidelink-r17-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_assist_info_sidelink_r17_ies_s_fields[] = {
    {offsetof(ue_assist_info_sidelink_r17_ies_s, sl_preferred_drx_cfg_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(ue_assist_info_sidelink_r17_ies_s::sl_preferred_drx_cfg_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_drx_cfg_uc_semi_static_r17_s::desc},
    {offsetof(ue_assist_info_sidelink_r17_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(ue_assist_info_sidelink_r17_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(ue_assist_info_sidelink_r17_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_assist_info_sidelink_r17_ies_s_names[] = {
    {"sl-PreferredDRX-ConfigList-r17", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_assist_info_sidelink_r17_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(ue_assist_info_sidelink_r17_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_assist_info_sidelink_r17_ies_s::desc = {
    {seq_table_ue_assist_info_sidelink_r17_ies_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_ue_assist_info_sidelink_r17_ies_s_opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_assist_info_sidelink_r17_ies_s_names};

// UEAssistanceInformationSidelink-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_assist_info_sidelink_r17_s_fields[] = {
    {offsetof(ue_assist_info_sidelink_r17_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_assist_info_sidelink_r17_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_ue_assist_info_sidelink_r17_s_names[] = {
    {"criticalExtensions", nullptr}};
const asn1::table::seq_desc ue_assist_info_sidelink_r17_s::desc = {
    {seq_table_ue_assist_info_sidelink_r17_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_assist_info_sidelink_r17_s_names};

void ue_assist_info_sidelink_r17_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_assist_info_sidelink_r17_ies_s& ue_assist_info_sidelink_r17_s::crit_exts_c_::set_ue_assist_info_sidelink_r17()
{
  set(types::ue_assist_info_sidelink_r17);
  return c;
}
void ue_assist_info_sidelink_r17_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_ue_assist_info_sidelink_r17_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_assist_info_sidelink_r17_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ue_assist_info_sidelink_r17_s_crit_exts_c__names[] = {
    {"ueAssistanceInformationSidelink-r17", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc ue_assist_info_sidelink_r17_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<ue_assist_info_sidelink_r17_s::crit_exts_c_::types>(),
    ue_assist_info_sidelink_r17_s::crit_exts_c_::types::nof_types,
    ue_assist_info_sidelink_r17_s::crit_exts_c_::types::nof_exts,
    ue_assist_info_sidelink_r17_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_ue_assist_info_sidelink_r17_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<ue_assist_info_sidelink_r17_s::crit_exts_c_>,
    choice_table_ue_assist_info_sidelink_r17_s_crit_exts_c__names,
    "ue_assist_info_sidelink_r17_s::crit_exts_c_",
    offsetof(ue_assist_info_sidelink_r17_s::crit_exts_c_, type_),
    offsetof(ue_assist_info_sidelink_r17_s::crit_exts_c_, c),
    false};

static const char* enum_names_ue_assist_info_sidelink_r17_s_crit_exts_c__types_values[] = {
    "ueAssistanceInformationSidelink-r17",
    "criticalExtensionsFuture"};
const asn1::table::enum_desc ue_assist_info_sidelink_r17_s::crit_exts_c_::types_opts::desc = {
    enum_names_ue_assist_info_sidelink_r17_s_crit_exts_c__types_values,
    2,
    "ue_assist_info_sidelink_r17_s::crit_exts_c_::types"};

// SL-E2E-QoS-InfoPC5-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_e2_e_qos_info_pc5_r18_s_fields[] = {
    {offsetof(sl_e2_e_qos_info_pc5_r18_s, sl_dest_id_remote_ue_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_e2_e_qos_info_pc5_r18_s::sl_dest_id_remote_ue_r18),
                                asn1::table::struct_json>::value},
    {offsetof(sl_e2_e_qos_info_pc5_r18_s, sl_e2_e_slrb_idx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(sl_e2_e_qos_info_pc5_r18_s, sl_qos_info_list_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(sl_e2_e_qos_info_pc5_r18_s::sl_qos_info_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_qos_info_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_e2_e_qos_info_pc5_r18_s_names[] = {
    {"sl-DestinationIdentityRemoteUE-r18", nullptr},
    {"sl-E2E-SLRB-Index-r18", nullptr},
    {"sl-QoS-InfoList-r18", nullptr}};
const asn1::table::seq_desc sl_e2_e_qos_info_pc5_r18_s::desc = {
    {seq_table_sl_e2_e_qos_info_pc5_r18_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_e2_e_qos_info_pc5_r18_s_names};

// UEInformationRequestSidelink-r18-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_info_request_sidelink_r18_ies_s_fields[] = {
    {offsetof(ue_info_request_sidelink_r18_ies_s, sl_e2_e_qos_info_list_pc5_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::item_list_codec<decltype(ue_info_request_sidelink_r18_ies_s::sl_e2_e_qos_info_list_pc5_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_e2_e_qos_info_pc5_r18_s::desc},
    {offsetof(ue_info_request_sidelink_r18_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(ue_info_request_sidelink_r18_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(ue_info_request_sidelink_r18_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_info_request_sidelink_r18_ies_s_names[] = {
    {"sl-E2E-QoS-InfoListPC5-r18", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_info_request_sidelink_r18_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(ue_info_request_sidelink_r18_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_info_request_sidelink_r18_ies_s::desc = {
    {seq_table_ue_info_request_sidelink_r18_ies_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_ue_info_request_sidelink_r18_ies_s_opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_info_request_sidelink_r18_ies_s_names};

// UEInformationRequestSidelink-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_info_request_sidelink_r18_s_fields[] = {
    {offsetof(ue_info_request_sidelink_r18_s, rrc_transaction_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(ue_info_request_sidelink_r18_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_info_request_sidelink_r18_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_ue_info_request_sidelink_r18_s_names[] = {
    {"rrc-TransactionIdentifier-r18", nullptr},
    {"criticalExtensions", nullptr}};
const asn1::table::seq_desc ue_info_request_sidelink_r18_s::desc = {
    {seq_table_ue_info_request_sidelink_r18_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_info_request_sidelink_r18_s_names};

void ue_info_request_sidelink_r18_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_info_request_sidelink_r18_ies_s& ue_info_request_sidelink_r18_s::crit_exts_c_::set_ue_info_request_sidelink_r18()
{
  set(types::ue_info_request_sidelink_r18);
  return c;
}
void ue_info_request_sidelink_r18_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_ue_info_request_sidelink_r18_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_info_request_sidelink_r18_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ue_info_request_sidelink_r18_s_crit_exts_c__names[] = {
    {"ueInformationRequestSidelink-r18", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc ue_info_request_sidelink_r18_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<ue_info_request_sidelink_r18_s::crit_exts_c_::types>(),
    ue_info_request_sidelink_r18_s::crit_exts_c_::types::nof_types,
    ue_info_request_sidelink_r18_s::crit_exts_c_::types::nof_exts,
    ue_info_request_sidelink_r18_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_ue_info_request_sidelink_r18_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<ue_info_request_sidelink_r18_s::crit_exts_c_>,
    choice_table_ue_info_request_sidelink_r18_s_crit_exts_c__names,
    "ue_info_request_sidelink_r18_s::crit_exts_c_",
    offsetof(ue_info_request_sidelink_r18_s::crit_exts_c_, type_),
    offsetof(ue_info_request_sidelink_r18_s::crit_exts_c_, c),
    false};

static const char* enum_names_ue_info_request_sidelink_r18_s_crit_exts_c__types_values[] = {
    "ueInformationRequestSidelink-r18",
    "criticalExtensionsFuture"};
const asn1::table::enum_desc ue_info_request_sidelink_r18_s::crit_exts_c_::types_opts::desc = {
    enum_names_ue_info_request_sidelink_r18_s_crit_exts_c__types_values,
    2,
    "ue_info_request_sidelink_r18_s::crit_exts_c_::types"};

// SL-SplitQoS-InfoPC5-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_split_qos_info_pc5_r18_s_fields[] = {
    {offsetof(sl_split_qos_info_pc5_r18_s, sl_qos_flow_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(2048),
     nullptr},
    {offsetof(sl_split_qos_info_pc5_r18_s, sl_split_packet_delay_budget_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr}};
static const asn1::table::field_name seq_table_sl_split_qos_info_pc5_r18_s_names[] = {
    {"sl-QoS-FlowIdentity-r18", nullptr},
    {"sl-SplitPacketDelayBudget-r18", nullptr}};
const asn1::table::seq_desc sl_split_qos_info_pc5_r18_s::desc = {
    {seq_table_sl_split_qos_info_pc5_r18_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_split_qos_info_pc5_r18_s_names};

// UEInformationResponseSidelink-r18-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_info_resp_sidelink_r18_ies_s_fields[] = {
    {offsetof(ue_info_resp_sidelink_r18_ies_s, sl_split_qos_info_list_pc5_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2048,
     &asn1::table::item_list_codec<decltype(ue_info_resp_sidelink_r18_ies_s::sl_split_qos_info_list_pc5_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_split_qos_info_pc5_r18_s::desc},
    {offsetof(ue_info_resp_sidelink_r18_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(ue_info_resp_sidelink_r18_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(ue_info_resp_sidelink_r18_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_info_resp_sidelink_r18_ies_s_names[] = {
    {"sl-SplitQoS-InfoListPC5-r18", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_info_resp_sidelink_r18_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(ue_info_resp_sidelink_r18_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_info_resp_sidelink_r18_ies_s::desc = {{seq_table_ue_info_resp_sidelink_r18_ies_s_fields,
                                                                      3,
                                                                      UINT64_C(0x0),
                                                                      seq_table_ue_info_resp_sidelink_r18_ies_s_opts,
                                                                      3},
                                                                     false,
                                                                     false,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_ue_info_resp_sidelink_r18_ies_s_names};

// UEInformationResponseSidelink-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_info_resp_sidelink_r18_s_fields[] = {
    {offsetof(ue_info_resp_sidelink_r18_s, rrc_transaction_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(ue_info_resp_sidelink_r18_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_info_resp_sidelink_r18_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_ue_info_resp_sidelink_r18_s_names[] = {
    {"rrc-TransactionIdentifier-r18", nullptr},
    {"criticalExtensions", nullptr}};
const asn1::table::seq_desc ue_info_resp_sidelink_r18_s::desc = {
    {seq_table_ue_info_resp_sidelink_r18_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_info_resp_sidelink_r18_s_names};

void ue_info_resp_sidelink_r18_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_info_resp_sidelink_r18_ies_s& ue_info_resp_sidelink_r18_s::crit_exts_c_::set_ue_info_resp_sidelink_r18()
{
  set(types::ue_info_resp_sidelink_r18);
  return c;
}
void ue_info_resp_sidelink_r18_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_ue_info_resp_sidelink_r18_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_info_resp_sidelink_r18_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ue_info_resp_sidelink_r18_s_crit_exts_c__names[] = {
    {"ueInformationResponseSidelink-r18", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc ue_info_resp_sidelink_r18_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<ue_info_resp_sidelink_r18_s::crit_exts_c_::types>(),
    ue_info_resp_sidelink_r18_s::crit_exts_c_::types::nof_types,
    ue_info_resp_sidelink_r18_s::crit_exts_c_::types::nof_exts,
    ue_info_resp_sidelink_r18_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_ue_info_resp_sidelink_r18_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<ue_info_resp_sidelink_r18_s::crit_exts_c_>,
    choice_table_ue_info_resp_sidelink_r18_s_crit_exts_c__names,
    "ue_info_resp_sidelink_r18_s::crit_exts_c_",
    offsetof(ue_info_resp_sidelink_r18_s::crit_exts_c_, type_),
    offsetof(ue_info_resp_sidelink_r18_s::crit_exts_c_, c),
    false};

static const char* enum_names_ue_info_resp_sidelink_r18_s_crit_exts_c__types_values[] = {
    "ueInformationResponseSidelink-r18",
    "criticalExtensionsFuture"};
const asn1::table::enum_desc ue_info_resp_sidelink_r18_s::crit_exts_c_::types_opts::desc = {
    enum_names_ue_info_resp_sidelink_r18_s_crit_exts_c__types_values,
    2,
    "ue_info_resp_sidelink_r18_s::crit_exts_c_::types"};

// SCCH-MessageType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_s_cch_msg_type_c_alternatives[] = {
    &asn1::table::alternative_ops<s_cch_msg_type_c::c1_c_, false>::value,
    &asn1::table::alternative_ops<s_cch_msg_type_c::msg_class_ext_c_, false>::value};
const asn1::table::choice_storage_desc s_cch_msg_type_c::storage_desc = {choice_storage_s_cch_msg_type_c_alternatives,
                                                                         2,
                                                                         "s_cch_msg_type_c"};

s_cch_msg_type_c::c1_c_& s_cch_msg_type_c::set_c1()
{
  set(types::c1);
  return c.get<c1_c_>();
}
s_cch_msg_type_c::msg_class_ext_c_& s_cch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
  return c.get<msg_class_ext_c_>();
}
static const asn1::table::seq_field choice_table_s_cch_msg_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &s_cch_msg_type_c::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &s_cch_msg_type_c::msg_class_ext_c_::desc}};
static const asn1::table::field_name choice_table_s_cch_msg_type_c_names[] = {{"c1", nullptr},
                                                                              {"messageClassExtension", nullptr}};
const asn1::table::choice_desc       s_cch_msg_type_c::desc = {asn1::table::enum_int_type<s_cch_msg_type_c::types>(),
                                                               s_cch_msg_type_c::types::nof_types,
                                                               s_cch_msg_type_c::types::nof_exts,
                                                               s_cch_msg_type_c::types::has_ext,
                                                               false,
                                                               choice_table_s_cch_msg_type_c_alternatives,
                                                               nullptr,
                                                               choice_table_s_cch_msg_type_c_names,
                                                               "s_cch_msg_type_c",
                                                               offsetof(s_cch_msg_type_c, c.type),
                                                               offsetof(s_cch_msg_type_c, c.value),
                                                               false,
                                                               &s_cch_msg_type_c::storage_desc};

static const asn1::table::storage_ops* const choice_storage_s_cch_msg_type_c_c1_c__alternatives[] = {
    &asn1::table::alternative_ops<meas_report_sidelink_s, false>::value,
    &asn1::table::alternative_ops<rrc_recfg_sidelink_s, false>::value,
    &asn1::table::alternative_ops<rrc_recfg_complete_sidelink_s, false>::value,
    &asn1::table::alternative_ops<rrc_recfg_fail_sidelink_s, false>::value,
    &asn1::table::alternative_ops<ue_cap_enquiry_sidelink_s, false>::value,
    &asn1::table::alternative_ops<ue_cap_info_sidelink_s, false>::value,
    &asn1::table::alternative_ops<uu_msg_transfer_sidelink_r17_s, false>::value,
    &asn1::table::alternative_ops<remote_ue_info_sidelink_r17_s, false>::value};
const asn1::table::choice_storage_desc s_cch_msg_type_c::c1_c_::storage_desc = {
    choice_storage_s_cch_msg_type_c_c1_c__alternatives,
    8,
    "s_cch_msg_type_c::c1_c_"};

meas_report_sidelink_s& s_cch_msg_type_c::c1_c_::set_meas_report_sidelink()
{
  set(types::meas_report_sidelink);
  return c.get<meas_report_sidelink_s>();
}
rrc_recfg_sidelink_s& s_cch_msg_type_c::c1_c_::set_rrc_recfg_sidelink()
{
  set(types::rrc_recfg_sidelink);
  return c.get<rrc_recfg_sidelink_s>();
}
rrc_recfg_complete_sidelink_s& s_cch_msg_type_c::c1_c_::set_rrc_recfg_complete_sidelink()
{
  set(types::rrc_recfg_complete_sidelink);
  return c.get<rrc_recfg_complete_sidelink_s>();
}
rrc_recfg_fail_sidelink_s& s_cch_msg_type_c::c1_c_::set_rrc_recfg_fail_sidelink()
{
  set(types::rrc_recfg_fail_sidelink);
  return c.get<rrc_recfg_fail_sidelink_s>();
}
ue_cap_enquiry_sidelink_s& s_cch_msg_type_c::c1_c_::set_ue_cap_enquiry_sidelink()
{
  set(types::ue_cap_enquiry_sidelink);
  return c.get<ue_cap_enquiry_sidelink_s>();
}
ue_cap_info_sidelink_s& s_cch_msg_type_c::c1_c_::set_ue_cap_info_sidelink()
{
  set(types::ue_cap_info_sidelink);
  return c.get<ue_cap_info_sidelink_s>();
}
uu_msg_transfer_sidelink_r17_s& s_cch_msg_type_c::c1_c_::set_uu_msg_transfer_sidelink_r17()
{
  set(types::uu_msg_transfer_sidelink_r17);
  return c.get<uu_msg_transfer_sidelink_r17_s>();
}
remote_ue_info_sidelink_r17_s& s_cch_msg_type_c::c1_c_::set_remote_ue_info_sidelink_r17()
{
  set(types::remote_ue_info_sidelink_r17);
  return c.get<remote_ue_info_sidelink_r17_s>();
}
static const asn1::table::seq_field choice_table_s_cch_msg_type_c_c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_report_sidelink_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_sidelink_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_complete_sidelink_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_fail_sidelink_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_enquiry_sidelink_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_info_sidelink_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &uu_msg_transfer_sidelink_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &remote_ue_info_sidelink_r17_s::desc}};
static const asn1::table::field_name choice_table_s_cch_msg_type_c_c1_c__names[] = {
    {"measurementReportSidelink", nullptr},
    {"rrcReconfigurationSidelink", nullptr},
    {"rrcReconfigurationCompleteSidelink", nullptr},
    {"rrcReconfigurationFailureSidelink", nullptr},
    {"ueCapabilityEnquirySidelink", nullptr},
    {"ueCapabilityInformationSidelink", nullptr},
    {"uuMessageTransferSidelink-r17", nullptr},
    {"remoteUEInformationSidelink-r17", nullptr}};
const asn1::table::choice_desc s_cch_msg_type_c::c1_c_::desc = {
    asn1::table::enum_int_type<s_cch_msg_type_c::c1_c_::types>(),
    s_cch_msg_type_c::c1_c_::types::nof_types,
    s_cch_msg_type_c::c1_c_::types::nof_exts,
    s_cch_msg_type_c::c1_c_::types::has_ext,
    false,
    choice_table_s_cch_msg_type_c_c1_c__alternatives,
    nullptr,
    choice_table_s_cch_msg_type_c_c1_c__names,
    "s_cch_msg_type_c::c1_c_",
    offsetof(s_cch_msg_type_c::c1_c_, c.type),
    offsetof(s_cch_msg_type_c::c1_c_, c.value),
    false,
    &s_cch_msg_type_c::c1_c_::storage_desc};

static const char*           enum_names_s_cch_msg_type_c_c1_c__types_values[] = {"measurementReportSidelink",
                                                                                 "rrcReconfigurationSidelink",
                                                                                 "rrcReconfigurationCompleteSidelink",
                                                                                 "rrcReconfigurationFailureSidelink",
                                                                                 "ueCapabilityEnquirySidelink",
                                                                                 "ueCapabilityInformationSidelink",
                                                                                 "uuMessageTransferSidelink-r17",
                                                                                 "remoteUEInformationSidelink-r17"};
const asn1::table::enum_desc s_cch_msg_type_c::c1_c_::types_opts::desc        = {
    enum_names_s_cch_msg_type_c_c1_c__types_values,
    8,
    "s_cch_msg_type_c::c1_c_::types"};

void s_cch_msg_type_c::msg_class_ext_c_::set(types::options e)
{
  type_ = e;
}
s_cch_msg_type_c::msg_class_ext_c_::c2_c_& s_cch_msg_type_c::msg_class_ext_c_::set_c2()
{
  set(types::c2);
  return c;
}
void s_cch_msg_type_c::msg_class_ext_c_::set_msg_class_ext_future_r17()
{
  set(types::msg_class_ext_future_r17);
}
static const asn1::table::seq_field choice_table_s_cch_msg_type_c_msg_class_ext_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &s_cch_msg_type_c::msg_class_ext_c_::c2_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_s_cch_msg_type_c_msg_class_ext_c__names[] = {
    {"c2", nullptr},
    {"messageClassExtensionFuture-r17", nullptr}};
const asn1::table::choice_desc s_cch_msg_type_c::msg_class_ext_c_::desc = {
    asn1::table::enum_int_type<s_cch_msg_type_c::msg_class_ext_c_::types>(),
    s_cch_msg_type_c::msg_class_ext_c_::types::nof_types,
    s_cch_msg_type_c::msg_class_ext_c_::types::nof_exts,
    s_cch_msg_type_c::msg_class_ext_c_::types::has_ext,
    false,
    choice_table_s_cch_msg_type_c_msg_class_ext_c__alternatives,
    &asn1::table::choice_set<s_cch_msg_type_c::msg_class_ext_c_>,
    choice_table_s_cch_msg_type_c_msg_class_ext_c__names,
    "s_cch_msg_type_c::msg_class_ext_c_",
    offsetof(s_cch_msg_type_c::msg_class_ext_c_, type_),
    offsetof(s_cch_msg_type_c::msg_class_ext_c_, c),
    false};

static const asn1::table::storage_ops* const choice_storage_s_cch_msg_type_c_msg_class_ext_c__c2_c__alternatives[] = {
    &asn1::table::alternative_ops<notif_msg_sidelink_r17_s, false>::value,
    &asn1::table::alternative_ops<ue_assist_info_sidelink_r17_s, false>::value,
    &asn1::table::alternative_ops<ue_info_request_sidelink_r18_s, false>::value,
    &asn1::table::alternative_ops<ue_info_resp_sidelink_r18_s, false>::value,
    nullptr,
    nullptr,
    nullptr,
    nullptr};
const asn1::table::choice_storage_desc s_cch_msg_type_c::msg_class_ext_c_::c2_c_::storage_desc = {
    choice_storage_s_cch_msg_type_c_msg_class_ext_c__c2_c__alternatives,
    8,
    "s_cch_msg_type_c::msg_class_ext_c_::c2_c_"};

notif_msg_sidelink_r17_s& s_cch_msg_type_c::msg_class_ext_c_::c2_c_::set_notif_msg_sidelink_r17()
{
  set(types::notif_msg_sidelink_r17);
  return c.get<notif_msg_sidelink_r17_s>();
}
ue_assist_info_sidelink_r17_s& s_cch_msg_type_c::msg_class_ext_c_::c2_c_::set_ue_assist_info_sidelink_r17()
{
  set(types::ue_assist_info_sidelink_r17);
  return c.get<ue_assist_info_sidelink_r17_s>();
}
ue_info_request_sidelink_r18_s& s_cch_msg_type_c::msg_class_ext_c_::c2_c_::set_ue_info_request_sidelink_r18()
{
  set(types::ue_info_request_sidelink_r18);
  return c.get<ue_info_request_sidelink_r18_s>();
}
ue_info_resp_sidelink_r18_s& s_cch_msg_type_c::msg_class_ext_c_::c2_c_::set_ue_info_resp_sidelink_r18()
{
  set(types::ue_info_resp_sidelink_r18);
  return c.get<ue_info_resp_sidelink_r18_s>();
}
void s_cch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare4()
{
  set(types::spare4);
}
void s_cch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare3()
{
  set(types::spare3);
}
void s_cch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare2()
{
  set(types::spare2);
}
void s_cch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_s_cch_msg_type_c_msg_class_ext_c__c2_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &notif_msg_sidelink_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_assist_info_sidelink_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_info_request_sidelink_r18_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_info_resp_sidelink_r18_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_s_cch_msg_type_c_msg_class_ext_c__c2_c__names[] = {
    {"notificationMessageSidelink-r17", nullptr},
    {"ueAssistanceInformationSidelink-r17", nullptr},
    {"ueInformationRequestSidelink-r18", nullptr},
    {"ueInformationResponseSidelink-r18", nullptr},
    {"spare4", nullptr},
    {"spare3", nullptr},
    {"spare2", nullptr},
    {"spare1", nullptr}};
const asn1::table::choice_desc s_cch_msg_type_c::msg_class_ext_c_::c2_c_::desc = {
    asn1::table::enum_int_type<s_cch_msg_type_c::msg_class_ext_c_::c2_c_::types>(),
    s_cch_msg_type_c::msg_class_ext_c_::c2_c_::types::nof_types,
    s_cch_msg_type_c::msg_class_ext_c_::c2_c_::types::nof_exts,
    s_cch_msg_type_c::msg_class_ext_c_::c2_c_::types::has_ext,
    false,
    choice_table_s_cch_msg_type_c_msg_class_ext_c__c2_c__alternatives,
    nullptr,
    choice_table_s_cch_msg_type_c_msg_class_ext_c__c2_c__names,
    "s_cch_msg_type_c::msg_class_ext_c_::c2_c_",
    offsetof(s_cch_msg_type_c::msg_class_ext_c_::c2_c_, c.type),
    offsetof(s_cch_msg_type_c::msg_class_ext_c_::c2_c_, c.value),
    false,
    &s_cch_msg_type_c::msg_class_ext_c_::c2_c_::storage_desc};

static const char* enum_names_s_cch_msg_type_c_msg_class_ext_c__c2_c__types_values[] = {
    "notificationMessageSidelink-r17",
    "ueAssistanceInformationSidelink-r17",
    "ueInformationRequestSidelink-r18",
    "ueInformationResponseSidelink-r18",
    "spare4",
    "spare3",
    "spare2",
    "spare1"};
const asn1::table::enum_desc s_cch_msg_type_c::msg_class_ext_c_::c2_c_::types_opts::desc = {
    enum_names_s_cch_msg_type_c_msg_class_ext_c__c2_c__types_values,
    8,
    "s_cch_msg_type_c::msg_class_ext_c_::c2_c_::types"};

static const char*           enum_names_s_cch_msg_type_c_msg_class_ext_c__types_values[] = {"c2",
                                                                                            "messageClassExtensionFuture-r17"};
const asn1::table::enum_desc s_cch_msg_type_c::msg_class_ext_c_::types_opts::desc        = {
    enum_names_s_cch_msg_type_c_msg_class_ext_c__types_values,
    2,
    "s_cch_msg_type_c::msg_class_ext_c_::types"};
uint8_t s_cch_msg_type_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "s_cch_msg_type_c::msg_class_ext_c_::types");
}

static const char*           enum_names_s_cch_msg_type_c_types_values[] = {"c1", "messageClassExtension"};
const asn1::table::enum_desc s_cch_msg_type_c::types_opts::desc         = {enum_names_s_cch_msg_type_c_types_values,
                                                                           2,
                                                                           "s_cch_msg_type_c::types"};
uint8_t                      s_cch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "s_cch_msg_type_c::types");
}

// SCCH-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_s_cch_msg_s_fields[] = {{offsetof(s_cch_msg_s, msg),
                                                                        asn1::table::seq_field::mandatory,
                                                                        asn1::table::field_op::choice,
                                                                        false,
                                                                        false,
                                                                        0,
                                                                        0,
                                                                        nullptr,
                                                                        &s_cch_msg_type_c::desc}};
static const asn1::table::field_name seq_table_s_cch_msg_s_names[]  = {{"message", nullptr}};
const asn1::table::seq_desc          s_cch_msg_s::desc = {{seq_table_s_cch_msg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                                          false,
                                                          false,
                                                          nullptr,
                                                          0,
                                                          seq_table_s_cch_msg_s_names,
                                                          "SCCH-Message"};

// SL-MeasResultRelay-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_meas_result_relay_r17_s_fields[] = {
    {offsetof(sl_meas_result_relay_r17_s, cell_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_access_related_info_s::desc},
    {offsetof(sl_meas_result_relay_r17_s, sl_relay_ue_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_meas_result_relay_r17_s::sl_relay_ue_id_r17),
                                asn1::table::struct_json>::value},
    {offsetof(sl_meas_result_relay_r17_s, sl_meas_result_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_result_r16_s::desc}};
static const asn1::table::field_name seq_table_sl_meas_result_relay_r17_s_names[] = {
    {"cellIdentity-r17", nullptr},
    {"sl-RelayUE-Identity-r17", nullptr},
    {"sl-MeasResult-r17", nullptr}};
static const asn1::table::seq_field seq_table_sl_meas_result_relay_r17_s_g0_fields[] = {
    {offsetof(sl_meas_result_relay_r17_s, sl_meas_quant_r18),
     (int32_t)offsetof(sl_meas_result_relay_r17_s, sl_meas_quant_r18_present),
     asn1::table::enum_op<decltype(sl_meas_result_relay_r17_s::sl_meas_quant_r18)>(),
     decltype(sl_meas_result_relay_r17_s::sl_meas_quant_r18)::has_ext,
     false,
     decltype(sl_meas_result_relay_r17_s::sl_meas_quant_r18)::nof_types,
     decltype(sl_meas_result_relay_r17_s::sl_meas_quant_r18)::nof_exts,
     nullptr,
     &sl_meas_result_relay_r17_s::sl_meas_quant_r18_opts::desc},
    {0,
     (int32_t)offsetof(sl_meas_result_relay_r17_s, sl_relay_ind_mp_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sl_meas_result_relay_r17_s_g0_names[] = {
    {"sl-MeasQuantity-r18", nullptr},
    {"sl-RelayIndicationMP-r18", "support"}};
static const asn1::table::seq_opt seq_table_sl_meas_result_relay_r17_s_g0_opts[] = {
    {0, (int32_t)offsetof(sl_meas_result_relay_r17_s, sl_meas_quant_r18_present)},
    {1, (int32_t)offsetof(sl_meas_result_relay_r17_s, sl_relay_ind_mp_r18_present)}};
static const asn1::table::seq_group seq_table_sl_meas_result_relay_r17_s_groups[] = {
    {{seq_table_sl_meas_result_relay_r17_s_g0_fields,
      2,
      UINT64_C(0x0),
      seq_table_sl_meas_result_relay_r17_s_g0_opts,
      2},
     false,
     seq_table_sl_meas_result_relay_r17_s_g0_names}};
const asn1::table::seq_desc sl_meas_result_relay_r17_s::desc = {
    {seq_table_sl_meas_result_relay_r17_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    true,
    false,
    seq_table_sl_meas_result_relay_r17_s_groups,
    1,
    seq_table_sl_meas_result_relay_r17_s_names};

static const char* enum_names_sl_meas_result_relay_r17_s_sl_meas_quant_r18_e__values[] = {"sl-rsrp", "sd-rsrp"};
const asn1::table::enum_desc sl_meas_result_relay_r17_s::sl_meas_quant_r18_opts::desc  = {
    enum_names_sl_meas_result_relay_r17_s_sl_meas_quant_r18_e__values,
    2,
    "sl_meas_result_relay_r17_s::sl_meas_quant_r18_e_"};
