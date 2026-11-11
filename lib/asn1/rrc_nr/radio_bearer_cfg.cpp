// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/radio_bearer_cfg.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// CipheringAlgorithm ::= ENUMERATED
static const char* enum_names_ciphering_algorithm_e_values[] =
    {"nea0", "nea1", "nea2", "nea3", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc ciphering_algorithm_opts::desc = {enum_names_ciphering_algorithm_e_values,
                                                               8,
                                                               "ciphering_algorithm_e"};
uint8_t                      ciphering_algorithm_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3};
  return map_enum_number(numbers, 4, value, "ciphering_algorithm_e");
}

// UL-DataSplitThreshold ::= ENUMERATED
static const char* enum_names_ul_data_split_thres_e_values[] = {
    "b0",       "b100",     "b200",     "b400",     "b800",     "b1600",    "b3200",    "b6400",
    "b12800",   "b25600",   "b51200",   "b102400",  "b204800",  "b409600",  "b819200",  "b1228800",
    "b1638400", "b2457600", "b3276800", "b4096000", "b4915200", "b5734400", "b6553600", "infinity",
    "spare8",   "spare7",   "spare6",   "spare5",   "spare4",   "spare3",   "spare2",   "spare1"};
const asn1::table::enum_desc ul_data_split_thres_opts::desc = {enum_names_ul_data_split_thres_e_values,
                                                               32,
                                                               "ul_data_split_thres_e"};
int32_t                      ul_data_split_thres_opts::to_number() const
{
  static const int32_t numbers[] = {0,       100,     200,     400,     800,     1600,    3200,    6400,
                                    12800,   25600,   51200,   102400,  204800,  409600,  819200,  1228800,
                                    1638400, 2457600, 3276800, 4096000, 4915200, 5734400, 6553600, -1};
  return map_enum_number(numbers, 24, value, "ul_data_split_thres_e");
}

// DiscardTimerExt-r16 ::= ENUMERATED
static const char* enum_names_discard_timer_ext_r16_e_values[] =
    {"ms0dot5", "ms1", "ms2", "ms4", "ms6", "ms8", "spare2", "spare1"};
const asn1::table::enum_desc discard_timer_ext_r16_opts::desc = {enum_names_discard_timer_ext_r16_e_values,
                                                                 8,
                                                                 "discard_timer_ext_r16_e"};
float                        discard_timer_ext_r16_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 4.0, 6.0, 8.0};
  return map_enum_number(numbers, 6, value, "discard_timer_ext_r16_e");
}
const char* discard_timer_ext_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "1", "2", "4", "6", "8"};
  return convert_enum_idx(number_strs, 8, value, "discard_timer_ext_r16_e");
}

// EthernetHeaderCompression-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ethernet_hdr_compress_r16_s_ehc_common_r16_s__fields[] = {
    {offsetof(ethernet_hdr_compress_r16_s::ehc_common_r16_s_, ehc_c_id_len_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16)>(),
     decltype(ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16)::has_ext,
     false,
     decltype(ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16)::nof_types,
     decltype(ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16)::nof_exts,
     nullptr,
     &ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_opts::desc}};
static const asn1::table::field_name seq_table_ethernet_hdr_compress_r16_s_ehc_common_r16_s__names[] = {
    {"ehc-CID-Length-r16", nullptr}};
static const asn1::table::seq_desc seq_table_ethernet_hdr_compress_r16_s_ehc_common_r16_s_ = {
    {seq_table_ethernet_hdr_compress_r16_s_ehc_common_r16_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_ethernet_hdr_compress_r16_s_ehc_common_r16_s__names};
static const asn1::table::seq_field seq_table_ethernet_hdr_compress_r16_s_ehc_dl_r16_s__fields[] = {
    {0,
     (int32_t)offsetof(ethernet_hdr_compress_r16_s::ehc_dl_r16_s_, drb_continue_ehc_dl_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ethernet_hdr_compress_r16_s_ehc_dl_r16_s__names[] = {
    {"drb-ContinueEHC-DL-r16", "true"}};
static const asn1::table::seq_opt seq_table_ethernet_hdr_compress_r16_s_ehc_dl_r16_s__opts[] = {
    {0, (int32_t)offsetof(ethernet_hdr_compress_r16_s::ehc_dl_r16_s_, drb_continue_ehc_dl_r16_present)}};
static const asn1::table::seq_desc seq_table_ethernet_hdr_compress_r16_s_ehc_dl_r16_s_ = {
    {seq_table_ethernet_hdr_compress_r16_s_ehc_dl_r16_s__fields,
     1,
     UINT64_C(0x0),
     seq_table_ethernet_hdr_compress_r16_s_ehc_dl_r16_s__opts,
     1},
    true,
    false,
    nullptr,
    0,
    seq_table_ethernet_hdr_compress_r16_s_ehc_dl_r16_s__names};
static const asn1::table::seq_field seq_table_ethernet_hdr_compress_r16_s_ehc_ul_r16_s__fields[] = {
    {offsetof(ethernet_hdr_compress_r16_s::ehc_ul_r16_s_, max_c_id_ehc_ul_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(32767),
     nullptr},
    {0,
     (int32_t)offsetof(ethernet_hdr_compress_r16_s::ehc_ul_r16_s_, drb_continue_ehc_ul_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ethernet_hdr_compress_r16_s_ehc_ul_r16_s__names[] = {
    {"maxCID-EHC-UL-r16", nullptr},
    {"drb-ContinueEHC-UL-r16", "true"}};
static const asn1::table::seq_opt seq_table_ethernet_hdr_compress_r16_s_ehc_ul_r16_s__opts[] = {
    {1, (int32_t)offsetof(ethernet_hdr_compress_r16_s::ehc_ul_r16_s_, drb_continue_ehc_ul_r16_present)}};
static const asn1::table::seq_desc seq_table_ethernet_hdr_compress_r16_s_ehc_ul_r16_s_ = {
    {seq_table_ethernet_hdr_compress_r16_s_ehc_ul_r16_s__fields,
     2,
     UINT64_C(0x1),
     seq_table_ethernet_hdr_compress_r16_s_ehc_ul_r16_s__opts,
     1},
    true,
    false,
    nullptr,
    0,
    seq_table_ethernet_hdr_compress_r16_s_ehc_ul_r16_s__names};
static const asn1::table::seq_field seq_table_ethernet_hdr_compress_r16_s_fields[] = {
    {offsetof(ethernet_hdr_compress_r16_s, ehc_common_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ethernet_hdr_compress_r16_s_ehc_common_r16_s_},
    {offsetof(ethernet_hdr_compress_r16_s, ehc_dl_r16),
     (int32_t)offsetof(ethernet_hdr_compress_r16_s, ehc_dl_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ethernet_hdr_compress_r16_s_ehc_dl_r16_s_},
    {offsetof(ethernet_hdr_compress_r16_s, ehc_ul_r16),
     (int32_t)offsetof(ethernet_hdr_compress_r16_s, ehc_ul_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ethernet_hdr_compress_r16_s_ehc_ul_r16_s_}};
static const asn1::table::field_name seq_table_ethernet_hdr_compress_r16_s_names[] = {{"ehc-Common-r16", nullptr},
                                                                                      {"ehc-Downlink-r16", nullptr},
                                                                                      {"ehc-Uplink-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_ethernet_hdr_compress_r16_s_opts[]  = {
    {1, (int32_t)offsetof(ethernet_hdr_compress_r16_s, ehc_dl_r16_present)},
    {2, (int32_t)offsetof(ethernet_hdr_compress_r16_s, ehc_ul_r16_present)}};
const asn1::table::seq_desc ethernet_hdr_compress_r16_s::desc = {
    {seq_table_ethernet_hdr_compress_r16_s_fields, 3, UINT64_C(0x1), seq_table_ethernet_hdr_compress_r16_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ethernet_hdr_compress_r16_s_names};

static const char* enum_names_ethernet_hdr_compress_r16_s_ehc_common_r16_s__ehc_c_id_len_r16_e__values[] = {"bits7",
                                                                                                            "bits15"};
const asn1::table::enum_desc ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_opts::desc = {
    enum_names_ethernet_hdr_compress_r16_s_ehc_common_r16_s__ehc_c_id_len_r16_e__values,
    2,
    "ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_e_"};
uint8_t ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {7, 15};
  return map_enum_number(numbers, 2, value, "ethernet_hdr_compress_r16_s::ehc_common_r16_s_::ehc_c_id_len_r16_e_");
}

// UplinkDataCompression-r17 ::= CHOICE
void ul_data_compress_r17_c::set(types::options e)
{
  type_ = e;
}
ul_data_compress_r17_c::new_setup_s_& ul_data_compress_r17_c::set_new_setup()
{
  set(types::new_setup);
  return c;
}
void ul_data_compress_r17_c::set_drb_continue_udc()
{
  set(types::drb_continue_udc);
}
static const asn1::table::seq_field seq_table_ul_data_compress_r17_c_new_setup_s__fields[] = {
    {offsetof(ul_data_compress_r17_c::new_setup_s_, buffer_size_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ul_data_compress_r17_c::new_setup_s_::buffer_size_r17)>(),
     decltype(ul_data_compress_r17_c::new_setup_s_::buffer_size_r17)::has_ext,
     false,
     decltype(ul_data_compress_r17_c::new_setup_s_::buffer_size_r17)::nof_types,
     decltype(ul_data_compress_r17_c::new_setup_s_::buffer_size_r17)::nof_exts,
     nullptr,
     &ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_opts::desc},
    {offsetof(ul_data_compress_r17_c::new_setup_s_, dictionary_r17),
     (int32_t)offsetof(ul_data_compress_r17_c::new_setup_s_, dictionary_r17_present),
     asn1::table::enum_op<decltype(ul_data_compress_r17_c::new_setup_s_::dictionary_r17)>(),
     decltype(ul_data_compress_r17_c::new_setup_s_::dictionary_r17)::has_ext,
     false,
     decltype(ul_data_compress_r17_c::new_setup_s_::dictionary_r17)::nof_types,
     decltype(ul_data_compress_r17_c::new_setup_s_::dictionary_r17)::nof_exts,
     nullptr,
     &ul_data_compress_r17_c::new_setup_s_::dictionary_r17_opts::desc}};
static const asn1::table::field_name seq_table_ul_data_compress_r17_c_new_setup_s__names[] = {
    {"bufferSize-r17", nullptr},
    {"dictionary-r17", nullptr}};
static const asn1::table::seq_opt seq_table_ul_data_compress_r17_c_new_setup_s__opts[] = {
    {1, (int32_t)offsetof(ul_data_compress_r17_c::new_setup_s_, dictionary_r17_present)}};
static const asn1::table::seq_desc seq_table_ul_data_compress_r17_c_new_setup_s_ = {
    {seq_table_ul_data_compress_r17_c_new_setup_s__fields,
     2,
     UINT64_C(0x1),
     seq_table_ul_data_compress_r17_c_new_setup_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_ul_data_compress_r17_c_new_setup_s__names};
static const asn1::table::seq_field choice_table_ul_data_compress_r17_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ul_data_compress_r17_c_new_setup_s_},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_ul_data_compress_r17_c_names[] = {{"newSetup", nullptr},
                                                                                    {"drb-ContinueUDC", nullptr}};
const asn1::table::choice_desc       ul_data_compress_r17_c::desc                = {
    asn1::table::enum_int_type<ul_data_compress_r17_c::types>(),
    ul_data_compress_r17_c::types::nof_types,
    ul_data_compress_r17_c::types::nof_exts,
    ul_data_compress_r17_c::types::has_ext,
    false,
    choice_table_ul_data_compress_r17_c_alternatives,
    &asn1::table::choice_set<ul_data_compress_r17_c>,
    choice_table_ul_data_compress_r17_c_names,
    "ul_data_compress_r17_c",
    offsetof(ul_data_compress_r17_c, type_),
    offsetof(ul_data_compress_r17_c, c),
    false};

static const char*           enum_names_ul_data_compress_r17_c_new_setup_s__buffer_size_r17_e__values[] = {"kbyte2",
                                                                                                           "kbyte4",
                                                                                                           "kbyte8",
                                                                                                           "spare1"};
const asn1::table::enum_desc ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_opts::desc           = {
    enum_names_ul_data_compress_r17_c_new_setup_s__buffer_size_r17_e__values,
    4,
    "ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_e_"};
uint8_t ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "ul_data_compress_r17_c::new_setup_s_::buffer_size_r17_e_");
}

static const char* enum_names_ul_data_compress_r17_c_new_setup_s__dictionary_r17_e__values[] = {"sip-SDP", "operator"};
const asn1::table::enum_desc ul_data_compress_r17_c::new_setup_s_::dictionary_r17_opts::desc = {
    enum_names_ul_data_compress_r17_c_new_setup_s__dictionary_r17_e__values,
    2,
    "ul_data_compress_r17_c::new_setup_s_::dictionary_r17_e_"};

static const char*           enum_names_ul_data_compress_r17_c_types_values[] = {"newSetup", "drb-ContinueUDC"};
const asn1::table::enum_desc ul_data_compress_r17_c::types_opts::desc = {enum_names_ul_data_compress_r17_c_types_values,
                                                                         2,
                                                                         "ul_data_compress_r17_c::types"};

// DiscardTimerExt2-r17 ::= ENUMERATED
static const char*           enum_names_discard_timer_ext2_r17_e_values[] = {"ms2000", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc discard_timer_ext2_r17_opts::desc            = {enum_names_discard_timer_ext2_r17_e_values,
                                                                             4,
                                                                             "discard_timer_ext2_r17_e"};
uint16_t                     discard_timer_ext2_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {2000};
  return map_enum_number(numbers, 1, value, "discard_timer_ext2_r17_e");
}

// DiscardTimerForLowImportance-r18 ::= ENUMERATED
static const char*           enum_names_discard_timer_for_low_importance_r18_e_values[] = {"ms0",
                                                                                           "ms2",
                                                                                           "ms4",
                                                                                           "ms6",
                                                                                           "ms8",
                                                                                           "ms10",
                                                                                           "ms12",
                                                                                           "ms14",
                                                                                           "ms18",
                                                                                           "ms22",
                                                                                           "ms26",
                                                                                           "ms30",
                                                                                           "ms40",
                                                                                           "ms50",
                                                                                           "ms75",
                                                                                           "ms100"};
const asn1::table::enum_desc discard_timer_for_low_importance_r18_opts::desc            = {
    enum_names_discard_timer_for_low_importance_r18_e_values,
    16,
    "discard_timer_for_low_importance_r18_e"};
uint8_t discard_timer_for_low_importance_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 2, 4, 6, 8, 10, 12, 14, 18, 22, 26, 30, 40, 50, 75, 100};
  return map_enum_number(numbers, 16, value, "discard_timer_for_low_importance_r18_e");
}

// PDCP-Config ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdcp_cfg_s_drb_s__fields[] = {
    {offsetof(pdcp_cfg_s::drb_s_, discard_timer),
     (int32_t)offsetof(pdcp_cfg_s::drb_s_, discard_timer_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::drb_s_::discard_timer)>(),
     decltype(pdcp_cfg_s::drb_s_::discard_timer)::has_ext,
     false,
     decltype(pdcp_cfg_s::drb_s_::discard_timer)::nof_types,
     decltype(pdcp_cfg_s::drb_s_::discard_timer)::nof_exts,
     nullptr,
     &pdcp_cfg_s::drb_s_::discard_timer_opts::desc},
    {offsetof(pdcp_cfg_s::drb_s_, pdcp_sn_size_ul),
     (int32_t)offsetof(pdcp_cfg_s::drb_s_, pdcp_sn_size_ul_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::drb_s_::pdcp_sn_size_ul)>(),
     decltype(pdcp_cfg_s::drb_s_::pdcp_sn_size_ul)::has_ext,
     false,
     decltype(pdcp_cfg_s::drb_s_::pdcp_sn_size_ul)::nof_types,
     decltype(pdcp_cfg_s::drb_s_::pdcp_sn_size_ul)::nof_exts,
     nullptr,
     &pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::desc},
    {offsetof(pdcp_cfg_s::drb_s_, pdcp_sn_size_dl),
     (int32_t)offsetof(pdcp_cfg_s::drb_s_, pdcp_sn_size_dl_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::drb_s_::pdcp_sn_size_dl)>(),
     decltype(pdcp_cfg_s::drb_s_::pdcp_sn_size_dl)::has_ext,
     false,
     decltype(pdcp_cfg_s::drb_s_::pdcp_sn_size_dl)::nof_types,
     decltype(pdcp_cfg_s::drb_s_::pdcp_sn_size_dl)::nof_exts,
     nullptr,
     &pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::desc},
    {offsetof(pdcp_cfg_s::drb_s_, hdr_compress),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::drb_s_::hdr_compress_c_::desc},
    {0,
     (int32_t)offsetof(pdcp_cfg_s::drb_s_, integrity_protection_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(pdcp_cfg_s::drb_s_, status_report_required_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(pdcp_cfg_s::drb_s_, out_of_order_delivery_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_drb_s__names[] = {{"discardTimer", nullptr},
                                                                            {"pdcp-SN-SizeUL", nullptr},
                                                                            {"pdcp-SN-SizeDL", nullptr},
                                                                            {"headerCompression", nullptr},
                                                                            {"integrityProtection", "enabled"},
                                                                            {"statusReportRequired", "true"},
                                                                            {"outOfOrderDelivery", "true"}};
static const asn1::table::seq_opt    seq_table_pdcp_cfg_s_drb_s__opts[]  = {
    {0, (int32_t)offsetof(pdcp_cfg_s::drb_s_, discard_timer_present)},
    {1, (int32_t)offsetof(pdcp_cfg_s::drb_s_, pdcp_sn_size_ul_present)},
    {2, (int32_t)offsetof(pdcp_cfg_s::drb_s_, pdcp_sn_size_dl_present)},
    {4, (int32_t)offsetof(pdcp_cfg_s::drb_s_, integrity_protection_present)},
    {5, (int32_t)offsetof(pdcp_cfg_s::drb_s_, status_report_required_present)},
    {6, (int32_t)offsetof(pdcp_cfg_s::drb_s_, out_of_order_delivery_present)}};
static const asn1::table::seq_desc seq_table_pdcp_cfg_s_drb_s_ = {
    {seq_table_pdcp_cfg_s_drb_s__fields, 7, UINT64_C(0x8), seq_table_pdcp_cfg_s_drb_s__opts, 6},
    false,
    false,
    nullptr,
    0,
    seq_table_pdcp_cfg_s_drb_s__names};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_more_than_one_rlc_s__primary_path_s__fields[] = {
    {offsetof(pdcp_cfg_s::more_than_one_rlc_s_::primary_path_s_, cell_group),
     (int32_t)offsetof(pdcp_cfg_s::more_than_one_rlc_s_::primary_path_s_, cell_group_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(pdcp_cfg_s::more_than_one_rlc_s_::primary_path_s_, lc_ch),
     (int32_t)offsetof(pdcp_cfg_s::more_than_one_rlc_s_::primary_path_s_, lc_ch_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(32),
     nullptr}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_more_than_one_rlc_s__primary_path_s__names[] = {
    {"cellGroup", nullptr},
    {"logicalChannel", nullptr}};
static const asn1::table::seq_opt seq_table_pdcp_cfg_s_more_than_one_rlc_s__primary_path_s__opts[] = {
    {0, (int32_t)offsetof(pdcp_cfg_s::more_than_one_rlc_s_::primary_path_s_, cell_group_present)},
    {1, (int32_t)offsetof(pdcp_cfg_s::more_than_one_rlc_s_::primary_path_s_, lc_ch_present)}};
static const asn1::table::seq_desc seq_table_pdcp_cfg_s_more_than_one_rlc_s__primary_path_s_ = {
    {seq_table_pdcp_cfg_s_more_than_one_rlc_s__primary_path_s__fields,
     2,
     UINT64_C(0x0),
     seq_table_pdcp_cfg_s_more_than_one_rlc_s__primary_path_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_pdcp_cfg_s_more_than_one_rlc_s__primary_path_s__names};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_more_than_one_rlc_s__fields[] = {
    {offsetof(pdcp_cfg_s::more_than_one_rlc_s_, primary_path),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_pdcp_cfg_s_more_than_one_rlc_s__primary_path_s_},
    {offsetof(pdcp_cfg_s::more_than_one_rlc_s_, ul_data_split_thres),
     (int32_t)offsetof(pdcp_cfg_s::more_than_one_rlc_s_, ul_data_split_thres_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::more_than_one_rlc_s_::ul_data_split_thres)>(),
     decltype(pdcp_cfg_s::more_than_one_rlc_s_::ul_data_split_thres)::has_ext,
     false,
     decltype(pdcp_cfg_s::more_than_one_rlc_s_::ul_data_split_thres)::nof_types,
     decltype(pdcp_cfg_s::more_than_one_rlc_s_::ul_data_split_thres)::nof_exts,
     nullptr,
     &ul_data_split_thres_opts::desc},
    {offsetof(pdcp_cfg_s::more_than_one_rlc_s_, pdcp_dupl),
     (int32_t)offsetof(pdcp_cfg_s::more_than_one_rlc_s_, pdcp_dupl_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_more_than_one_rlc_s__names[] = {
    {"primaryPath", nullptr},
    {"ul-DataSplitThreshold", nullptr},
    {"pdcp-Duplication", nullptr}};
static const asn1::table::seq_opt seq_table_pdcp_cfg_s_more_than_one_rlc_s__opts[] = {
    {1, (int32_t)offsetof(pdcp_cfg_s::more_than_one_rlc_s_, ul_data_split_thres_present)},
    {2, (int32_t)offsetof(pdcp_cfg_s::more_than_one_rlc_s_, pdcp_dupl_present)}};
static const asn1::table::seq_desc seq_table_pdcp_cfg_s_more_than_one_rlc_s_ = {
    {seq_table_pdcp_cfg_s_more_than_one_rlc_s__fields,
     3,
     UINT64_C(0x1),
     seq_table_pdcp_cfg_s_more_than_one_rlc_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_pdcp_cfg_s_more_than_one_rlc_s__names};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_more_than_two_rlc_drb_r16_s__fields[] = {
    {offsetof(pdcp_cfg_s::more_than_two_rlc_drb_r16_s_, split_secondary_path_r16),
     (int32_t)offsetof(pdcp_cfg_s::more_than_two_rlc_drb_r16_s_, split_secondary_path_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(pdcp_cfg_s::more_than_two_rlc_drb_r16_s_, dupl_state_r16),
     (int32_t)offsetof(pdcp_cfg_s::more_than_two_rlc_drb_r16_s_, dupl_state_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     3,
     3,
     &asn1::table::list_codec<decltype(pdcp_cfg_s::more_than_two_rlc_drb_r16_s_::dupl_state_r16),
                              asn1::table::bool_json,
                              asn1::table::bool_items<true>>::value}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_more_than_two_rlc_drb_r16_s__names[] = {
    {"splitSecondaryPath-r16", nullptr},
    {"duplicationState-r16", nullptr}};
static const asn1::table::seq_opt seq_table_pdcp_cfg_s_more_than_two_rlc_drb_r16_s__opts[] = {
    {0, (int32_t)offsetof(pdcp_cfg_s::more_than_two_rlc_drb_r16_s_, split_secondary_path_r16_present)},
    {1, (int32_t)offsetof(pdcp_cfg_s::more_than_two_rlc_drb_r16_s_, dupl_state_r16_present)}};
static const asn1::table::seq_desc seq_table_pdcp_cfg_s_more_than_two_rlc_drb_r16_s_ = {
    {seq_table_pdcp_cfg_s_more_than_two_rlc_drb_r16_s__fields,
     2,
     UINT64_C(0x0),
     seq_table_pdcp_cfg_s_more_than_two_rlc_drb_r16_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_pdcp_cfg_s_more_than_two_rlc_drb_r16_s__names};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_discard_timer_ext_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<setup_release_c<discard_timer_ext_r16_e>::value_type>(),
     setup_release_c<discard_timer_ext_r16_e>::value_type::has_ext,
     false,
     setup_release_c<discard_timer_ext_r16_e>::value_type::nof_types,
     setup_release_c<discard_timer_ext_r16_e>::value_type::nof_exts,
     nullptr,
     &discard_timer_ext_r16_opts::desc}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_discard_timer_ext_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pdcp_cfg_s_discard_timer_ext_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pdcp_cfg_s_discard_timer_ext_r16_setup_release_alternatives,
    nullptr,
    seq_table_pdcp_cfg_s_discard_timer_ext_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<discard_timer_ext_r16_e>::type_offset(),
    setup_release_c<discard_timer_ext_r16_e>::value_offset()};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_ethernet_hdr_compress_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ethernet_hdr_compress_r16_s::desc}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_ethernet_hdr_compress_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pdcp_cfg_s_ethernet_hdr_compress_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pdcp_cfg_s_ethernet_hdr_compress_r16_setup_release_alternatives,
    nullptr,
    seq_table_pdcp_cfg_s_ethernet_hdr_compress_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<ethernet_hdr_compress_r16_s>::type_offset(),
    setup_release_c<ethernet_hdr_compress_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_ul_data_compress_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_data_compress_r17_c::desc}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_ul_data_compress_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pdcp_cfg_s_ul_data_compress_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pdcp_cfg_s_ul_data_compress_r17_setup_release_alternatives,
    nullptr,
    seq_table_pdcp_cfg_s_ul_data_compress_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<ul_data_compress_r17_c>::type_offset(),
    setup_release_c<ul_data_compress_r17_c>::value_offset()};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_discard_timer_ext2_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<setup_release_c<discard_timer_ext2_r17_e>::value_type>(),
     setup_release_c<discard_timer_ext2_r17_e>::value_type::has_ext,
     false,
     setup_release_c<discard_timer_ext2_r17_e>::value_type::nof_types,
     setup_release_c<discard_timer_ext2_r17_e>::value_type::nof_exts,
     nullptr,
     &discard_timer_ext2_r17_opts::desc}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_discard_timer_ext2_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pdcp_cfg_s_discard_timer_ext2_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pdcp_cfg_s_discard_timer_ext2_r17_setup_release_alternatives,
    nullptr,
    seq_table_pdcp_cfg_s_discard_timer_ext2_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<discard_timer_ext2_r17_e>::type_offset(),
    setup_release_c<discard_timer_ext2_r17_e>::value_offset()};
static const asn1::table::seq_field
    seq_table_pdcp_cfg_s_discard_timer_for_low_importance_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<setup_release_c<discard_timer_for_low_importance_r18_e>::value_type>(),
         setup_release_c<discard_timer_for_low_importance_r18_e>::value_type::has_ext,
         false,
         setup_release_c<discard_timer_for_low_importance_r18_e>::value_type::nof_types,
         setup_release_c<discard_timer_for_low_importance_r18_e>::value_type::nof_exts,
         nullptr,
         &discard_timer_for_low_importance_r18_opts::desc}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_discard_timer_for_low_importance_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pdcp_cfg_s_discard_timer_for_low_importance_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pdcp_cfg_s_discard_timer_for_low_importance_r18_setup_release_alternatives,
    nullptr,
    seq_table_pdcp_cfg_s_discard_timer_for_low_importance_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<discard_timer_for_low_importance_r18_e>::type_offset(),
    setup_release_c<discard_timer_for_low_importance_r18_e>::value_offset()};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_fields[] = {
    {offsetof(pdcp_cfg_s, drb),
     (int32_t)offsetof(pdcp_cfg_s, drb_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_pdcp_cfg_s_drb_s_},
    {offsetof(pdcp_cfg_s, more_than_one_rlc),
     (int32_t)offsetof(pdcp_cfg_s, more_than_one_rlc_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_pdcp_cfg_s_more_than_one_rlc_s_},
    {offsetof(pdcp_cfg_s, t_reordering),
     (int32_t)offsetof(pdcp_cfg_s, t_reordering_present),
     asn1::table::enum_op<decltype(pdcp_cfg_s::t_reordering)>(),
     decltype(pdcp_cfg_s::t_reordering)::has_ext,
     false,
     decltype(pdcp_cfg_s::t_reordering)::nof_types,
     decltype(pdcp_cfg_s::t_reordering)::nof_exts,
     nullptr,
     &pdcp_cfg_s::t_reordering_opts::desc}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_names[] = {{"drb", nullptr},
                                                                     {"moreThanOneRLC", nullptr},
                                                                     {"t-Reordering", nullptr}};
static const asn1::table::seq_opt    seq_table_pdcp_cfg_s_opts[]  = {
    {0, (int32_t)offsetof(pdcp_cfg_s, drb_present)},
    {1, (int32_t)offsetof(pdcp_cfg_s, more_than_one_rlc_present)},
    {2, (int32_t)offsetof(pdcp_cfg_s, t_reordering_present)}};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_g0_fields[] = {
    {0,
     (int32_t)offsetof(pdcp_cfg_s, ciphering_disabled_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_g0_names[] = {{"cipheringDisabled", "true"}};
static const asn1::table::seq_opt    seq_table_pdcp_cfg_s_g0_opts[]  = {
    {0, (int32_t)offsetof(pdcp_cfg_s, ciphering_disabled_present)}};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_g1_fields[] = {
    {offsetof(pdcp_cfg_s, discard_timer_ext_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pdcp_cfg_s::discard_timer_ext_r16)>::value,
     &seq_table_pdcp_cfg_s_discard_timer_ext_r16_setup_release},
    {offsetof(pdcp_cfg_s, more_than_two_rlc_drb_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pdcp_cfg_s::more_than_two_rlc_drb_r16)>::value,
     &seq_table_pdcp_cfg_s_more_than_two_rlc_drb_r16_s_},
    {offsetof(pdcp_cfg_s, ethernet_hdr_compress_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pdcp_cfg_s::ethernet_hdr_compress_r16)>::value,
     &seq_table_pdcp_cfg_s_ethernet_hdr_compress_r16_setup_release}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_g1_names[]  = {{"discardTimerExt-r16", nullptr},
                                                                         {"moreThanTwoRLC-DRB-r16", nullptr},
                                                                         {"ethernetHeaderCompression-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_pdcp_cfg_s_g1_opts[]   = {{0, asn1::table::seq_field::ptr_presence},
                                                                         {1, asn1::table::seq_field::ptr_presence},
                                                                         {2, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_pdcp_cfg_s_g2_fields[] = {
    {0,
      (int32_t)offsetof(pdcp_cfg_s, survival_time_state_support_r17_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr},
    {offsetof(pdcp_cfg_s, ul_data_compress_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(pdcp_cfg_s::ul_data_compress_r17)>::value,
      &seq_table_pdcp_cfg_s_ul_data_compress_r17_setup_release},
    {offsetof(pdcp_cfg_s, discard_timer_ext2_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(pdcp_cfg_s::discard_timer_ext2_r17)>::value,
      &seq_table_pdcp_cfg_s_discard_timer_ext2_r17_setup_release},
    {offsetof(pdcp_cfg_s, init_rx_deliv_r17),
      (int32_t)offsetof(pdcp_cfg_s, init_rx_deliv_r17_present),
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<decltype(pdcp_cfg_s::init_rx_deliv_r17), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_g2_names[] = {{"survivalTimeStateSupport-r17", "true"},
                                                                        {"uplinkDataCompression-r17", nullptr},
                                                                        {"discardTimerExt2-r17", nullptr},
                                                                        {"initialRX-DELIV-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_pdcp_cfg_s_g2_opts[]  = {
    {0, (int32_t)offsetof(pdcp_cfg_s, survival_time_state_support_r17_present)},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, (int32_t)offsetof(pdcp_cfg_s, init_rx_deliv_r17_present)}};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_g3_fields[] = {
    {0,
     (int32_t)offsetof(pdcp_cfg_s, pdu_set_discard_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pdcp_cfg_s, discard_timer_for_low_importance_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pdcp_cfg_s::discard_timer_for_low_importance_r18)>::value,
     &seq_table_pdcp_cfg_s_discard_timer_for_low_importance_r18_setup_release},
    {0,
     (int32_t)offsetof(pdcp_cfg_s, primary_path_on_indirect_path_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(pdcp_cfg_s, sn_gap_report_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_g3_names[] = {{"pdu-SetDiscard-r18", "true"},
                                                                        {"discardTimerForLowImportance-r18", nullptr},
                                                                        {"primaryPathOnIndirectPath-r18", "true"},
                                                                        {"sn-GapReport-r18", "true"}};
static const asn1::table::seq_opt    seq_table_pdcp_cfg_s_g3_opts[]  = {
    {0, (int32_t)offsetof(pdcp_cfg_s, pdu_set_discard_r18_present)},
    {1, asn1::table::seq_field::ptr_presence},
    {2, (int32_t)offsetof(pdcp_cfg_s, primary_path_on_indirect_path_r18_present)},
    {3, (int32_t)offsetof(pdcp_cfg_s, sn_gap_report_r18_present)}};
static const asn1::table::seq_group seq_table_pdcp_cfg_s_groups[] = {
    {{seq_table_pdcp_cfg_s_g0_fields, 1, UINT64_C(0x0), seq_table_pdcp_cfg_s_g0_opts, 1},
     false,
     seq_table_pdcp_cfg_s_g0_names},
    {{seq_table_pdcp_cfg_s_g1_fields, 3, UINT64_C(0x0), seq_table_pdcp_cfg_s_g1_opts, 3},
     false,
     seq_table_pdcp_cfg_s_g1_names},
    {{seq_table_pdcp_cfg_s_g2_fields, 4, UINT64_C(0x0), seq_table_pdcp_cfg_s_g2_opts, 4},
     false,
     seq_table_pdcp_cfg_s_g2_names},
    {{seq_table_pdcp_cfg_s_g3_fields, 4, UINT64_C(0x0), seq_table_pdcp_cfg_s_g3_opts, 4},
     false,
     seq_table_pdcp_cfg_s_g3_names}};
const asn1::table::seq_desc pdcp_cfg_s::desc = {
    {seq_table_pdcp_cfg_s_fields, 3, UINT64_C(0x0), seq_table_pdcp_cfg_s_opts, 3},
    true,
    false,
    seq_table_pdcp_cfg_s_groups,
    4,
    seq_table_pdcp_cfg_s_names};

static const char*           enum_names_pdcp_cfg_s_drb_s__discard_timer_e__values[] = {"ms10",
                                                                                       "ms20",
                                                                                       "ms30",
                                                                                       "ms40",
                                                                                       "ms50",
                                                                                       "ms60",
                                                                                       "ms75",
                                                                                       "ms100",
                                                                                       "ms150",
                                                                                       "ms200",
                                                                                       "ms250",
                                                                                       "ms300",
                                                                                       "ms500",
                                                                                       "ms750",
                                                                                       "ms1500",
                                                                                       "infinity"};
const asn1::table::enum_desc pdcp_cfg_s::drb_s_::discard_timer_opts::desc           = {
    enum_names_pdcp_cfg_s_drb_s__discard_timer_e__values,
    16,
    "pdcp_cfg_s::drb_s_::discard_timer_e_"};
int16_t pdcp_cfg_s::drb_s_::discard_timer_opts::to_number() const
{
  static const int16_t numbers[] = {10, 20, 30, 40, 50, 60, 75, 100, 150, 200, 250, 300, 500, 750, 1500, -1};
  return map_enum_number(numbers, 16, value, "pdcp_cfg_s::drb_s_::discard_timer_e_");
}

static const char*           enum_names_pdcp_cfg_s_drb_s__pdcp_sn_size_ul_e__values[] = {"len12bits", "len18bits"};
const asn1::table::enum_desc pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::desc           = {
    enum_names_pdcp_cfg_s_drb_s__pdcp_sn_size_ul_e__values,
    2,
    "pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_e_"};
uint8_t pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_ul_e_");
}

static const char*           enum_names_pdcp_cfg_s_drb_s__pdcp_sn_size_dl_e__values[] = {"len12bits", "len18bits"};
const asn1::table::enum_desc pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::desc           = {
    enum_names_pdcp_cfg_s_drb_s__pdcp_sn_size_dl_e__values,
    2,
    "pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_e_"};
uint8_t pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "pdcp_cfg_s::drb_s_::pdcp_sn_size_dl_e_");
}

static const asn1::table::storage_ops* const choice_storage_pdcp_cfg_s_drb_s__hdr_compress_c__alternatives[] = {
    nullptr,
    &asn1::table::alternative_ops<pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_, false>::value,
    &asn1::table::alternative_ops<pdcp_cfg_s::drb_s_::hdr_compress_c_::ul_only_rohc_s_, false>::value};
const asn1::table::choice_storage_desc pdcp_cfg_s::drb_s_::hdr_compress_c_::storage_desc = {
    choice_storage_pdcp_cfg_s_drb_s__hdr_compress_c__alternatives,
    3,
    "pdcp_cfg_s::drb_s_::hdr_compress_c_"};

void pdcp_cfg_s::drb_s_::hdr_compress_c_::set_not_used()
{
  set(types::not_used);
}
pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_& pdcp_cfg_s::drb_s_::hdr_compress_c_::set_rohc()
{
  set(types::rohc);
  return c.get<rohc_s_>();
}
pdcp_cfg_s::drb_s_::hdr_compress_c_::ul_only_rohc_s_& pdcp_cfg_s::drb_s_::hdr_compress_c_::set_ul_only_rohc()
{
  set(types::ul_only_rohc);
  return c.get<ul_only_rohc_s_>();
}
static const asn1::table::seq_field seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__profiles_s__fields[] = {
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_::profiles_s_, profile0x0001),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_::profiles_s_, profile0x0002),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_::profiles_s_, profile0x0003),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_::profiles_s_, profile0x0004),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_::profiles_s_, profile0x0006),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_::profiles_s_, profile0x0101),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_::profiles_s_, profile0x0102),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_::profiles_s_, profile0x0103),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_::profiles_s_, profile0x0104),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__profiles_s__names[] = {
    {"profile0x0001", nullptr},
    {"profile0x0002", nullptr},
    {"profile0x0003", nullptr},
    {"profile0x0004", nullptr},
    {"profile0x0006", nullptr},
    {"profile0x0101", nullptr},
    {"profile0x0102", nullptr},
    {"profile0x0103", nullptr},
    {"profile0x0104", nullptr}};
static const asn1::table::seq_desc seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__profiles_s_ = {
    {seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__profiles_s__fields, 9, UINT64_C(0x1ff), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__profiles_s__names};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__fields[] = {
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_, max_c_id),
     (int32_t)offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_, max_c_id_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(16383),
     nullptr},
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_, profiles),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__profiles_s_},
    {0,
     (int32_t)offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_, drb_continue_rohc_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__names[] = {
    {"maxCID", nullptr},
    {"profiles", nullptr},
    {"drb-ContinueROHC", "true"}};
static const asn1::table::seq_opt seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__opts[] = {
    {0, (int32_t)offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_, max_c_id_present)},
    {2, (int32_t)offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::rohc_s_, drb_continue_rohc_present)}};
static const asn1::table::seq_desc seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s_ = {
    {seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__fields,
     3,
     UINT64_C(0x2),
     seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s__names};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__profiles_s__fields[] = {
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::ul_only_rohc_s_::profiles_s_, profile0x0006),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__profiles_s__names[] = {
    {"profile0x0006", nullptr}};
static const asn1::table::seq_desc seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__profiles_s_ = {
    {seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__profiles_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__profiles_s__names};
static const asn1::table::seq_field seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__fields[] = {
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::ul_only_rohc_s_, max_c_id),
     (int32_t)offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::ul_only_rohc_s_, max_c_id_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(16383),
     nullptr},
    {offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::ul_only_rohc_s_, profiles),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__profiles_s_},
    {0,
     (int32_t)offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::ul_only_rohc_s_, drb_continue_rohc_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__names[] = {
    {"maxCID", nullptr},
    {"profiles", nullptr},
    {"drb-ContinueROHC", "true"}};
static const asn1::table::seq_opt seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__opts[] = {
    {0, (int32_t)offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::ul_only_rohc_s_, max_c_id_present)},
    {2, (int32_t)offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_::ul_only_rohc_s_, drb_continue_rohc_present)}};
static const asn1::table::seq_desc seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s_ = {
    {seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__fields,
     3,
     UINT64_C(0x2),
     seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s__names};
static const asn1::table::seq_field choice_table_pdcp_cfg_s_drb_s__hdr_compress_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__rohc_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_pdcp_cfg_s_drb_s__hdr_compress_c__ul_only_rohc_s_}};
static const asn1::table::field_name choice_table_pdcp_cfg_s_drb_s__hdr_compress_c__names[] = {
    {"notUsed", nullptr},
    {"rohc", nullptr},
    {"uplinkOnlyROHC", nullptr}};
const asn1::table::choice_desc pdcp_cfg_s::drb_s_::hdr_compress_c_::desc = {
    asn1::table::enum_int_type<pdcp_cfg_s::drb_s_::hdr_compress_c_::types>(),
    pdcp_cfg_s::drb_s_::hdr_compress_c_::types::nof_types,
    pdcp_cfg_s::drb_s_::hdr_compress_c_::types::nof_exts,
    pdcp_cfg_s::drb_s_::hdr_compress_c_::types::has_ext,
    false,
    choice_table_pdcp_cfg_s_drb_s__hdr_compress_c__alternatives,
    nullptr,
    choice_table_pdcp_cfg_s_drb_s__hdr_compress_c__names,
    "pdcp_cfg_s::drb_s_::hdr_compress_c_",
    offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_, c.type),
    offsetof(pdcp_cfg_s::drb_s_::hdr_compress_c_, c.value),
    false,
    &pdcp_cfg_s::drb_s_::hdr_compress_c_::storage_desc};

static const char* enum_names_pdcp_cfg_s_drb_s__hdr_compress_c__types_values[] = {"notUsed", "rohc", "uplinkOnlyROHC"};
const asn1::table::enum_desc pdcp_cfg_s::drb_s_::hdr_compress_c_::types_opts::desc = {
    enum_names_pdcp_cfg_s_drb_s__hdr_compress_c__types_values,
    3,
    "pdcp_cfg_s::drb_s_::hdr_compress_c_::types"};

static const char* enum_names_pdcp_cfg_s_t_reordering_e__values[] = {
    "ms0",     "ms1",     "ms2",     "ms4",     "ms5",     "ms8",     "ms10",    "ms15",    "ms20",    "ms30",
    "ms40",    "ms50",    "ms60",    "ms80",    "ms100",   "ms120",   "ms140",   "ms160",   "ms180",   "ms200",
    "ms220",   "ms240",   "ms260",   "ms280",   "ms300",   "ms500",   "ms750",   "ms1000",  "ms1250",  "ms1500",
    "ms1750",  "ms2000",  "ms2250",  "ms2500",  "ms2750",  "ms3000",  "spare28", "spare27", "spare26", "spare25",
    "spare24", "spare23", "spare22", "spare21", "spare20", "spare19", "spare18", "spare17", "spare16", "spare15",
    "spare14", "spare13", "spare12", "spare11", "spare10", "spare09", "spare08", "spare07", "spare06", "spare05",
    "spare04", "spare03", "spare02", "spare01"};
const asn1::table::enum_desc pdcp_cfg_s::t_reordering_opts::desc = {enum_names_pdcp_cfg_s_t_reordering_e__values,
                                                                    64,
                                                                    "pdcp_cfg_s::t_reordering_e_"};
uint16_t                     pdcp_cfg_s::t_reordering_opts::to_number() const
{
  static const uint16_t numbers[] = {0,   1,   2,   4,    5,    8,    10,   15,   20,   30,   40,   50,
                                     60,  80,  100, 120,  140,  160,  180,  200,  220,  240,  260,  280,
                                     300, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000};
  return map_enum_number(numbers, 36, value, "pdcp_cfg_s::t_reordering_e_");
}

// SRB-ToAddMod ::= SEQUENCE
static const asn1::table::seq_field seq_table_srb_to_add_mod_s_fields[] = {
    {offsetof(srb_to_add_mod_s, srb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(3),
     nullptr},
    {0,
     (int32_t)offsetof(srb_to_add_mod_s, reestablish_pdcp_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(srb_to_add_mod_s, discard_on_pdcp_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(srb_to_add_mod_s, pdcp_cfg),
     (int32_t)offsetof(srb_to_add_mod_s, pdcp_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc}};
static const asn1::table::field_name seq_table_srb_to_add_mod_s_names[] = {{"srb-Identity", nullptr},
                                                                           {"reestablishPDCP", "true"},
                                                                           {"discardOnPDCP", "true"},
                                                                           {"pdcp-Config", nullptr}};
static const asn1::table::seq_opt    seq_table_srb_to_add_mod_s_opts[]  = {
    {1, (int32_t)offsetof(srb_to_add_mod_s, reestablish_pdcp_present)},
    {2, (int32_t)offsetof(srb_to_add_mod_s, discard_on_pdcp_present)},
    {3, (int32_t)offsetof(srb_to_add_mod_s, pdcp_cfg_present)}};
static const asn1::table::seq_field seq_table_srb_to_add_mod_s_g0_fields[] = {
    {offsetof(srb_to_add_mod_s, srb_id_v1700),
     (int32_t)offsetof(srb_to_add_mod_s, srb_id_v1700_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(4),
     UINT64_C(4),
     nullptr}};
static const asn1::table::field_name seq_table_srb_to_add_mod_s_g0_names[] = {{"srb-Identity-v1700", nullptr}};
static const asn1::table::seq_opt    seq_table_srb_to_add_mod_s_g0_opts[]  = {
    {0, (int32_t)offsetof(srb_to_add_mod_s, srb_id_v1700_present)}};
static const asn1::table::seq_field seq_table_srb_to_add_mod_s_g1_fields[] = {
    {offsetof(srb_to_add_mod_s, srb_id_v1800),
     (int32_t)offsetof(srb_to_add_mod_s, srb_id_v1800_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(5),
     UINT64_C(5),
     nullptr},
    {0,
     (int32_t)offsetof(srb_to_add_mod_s, n3c_bearer_associated_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_srb_to_add_mod_s_g1_names[] = {{"srb-Identity-v1800", nullptr},
                                                                              {"n3c-BearerAssociated-r18", "true"}};
static const asn1::table::seq_opt    seq_table_srb_to_add_mod_s_g1_opts[]  = {
    {0, (int32_t)offsetof(srb_to_add_mod_s, srb_id_v1800_present)},
    {1, (int32_t)offsetof(srb_to_add_mod_s, n3c_bearer_associated_r18_present)}};
static const asn1::table::seq_group seq_table_srb_to_add_mod_s_groups[] = {
    {{seq_table_srb_to_add_mod_s_g0_fields, 1, UINT64_C(0x0), seq_table_srb_to_add_mod_s_g0_opts, 1},
     false,
     seq_table_srb_to_add_mod_s_g0_names},
    {{seq_table_srb_to_add_mod_s_g1_fields, 2, UINT64_C(0x0), seq_table_srb_to_add_mod_s_g1_opts, 2},
     false,
     seq_table_srb_to_add_mod_s_g1_names}};
const asn1::table::seq_desc srb_to_add_mod_s::desc = {
    {seq_table_srb_to_add_mod_s_fields, 4, UINT64_C(0x1), seq_table_srb_to_add_mod_s_opts, 3},
    true,
    false,
    seq_table_srb_to_add_mod_s_groups,
    2,
    seq_table_srb_to_add_mod_s_names};

// SDAP-Config ::= SEQUENCE
static const asn1::table::seq_field seq_table_sdap_cfg_s_fields[] = {
    {offsetof(sdap_cfg_s, pdu_session),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(sdap_cfg_s, sdap_hdr_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sdap_cfg_s::sdap_hdr_dl)>(),
     decltype(sdap_cfg_s::sdap_hdr_dl)::has_ext,
     false,
     decltype(sdap_cfg_s::sdap_hdr_dl)::nof_types,
     decltype(sdap_cfg_s::sdap_hdr_dl)::nof_exts,
     nullptr,
     &sdap_cfg_s::sdap_hdr_dl_opts::desc},
    {offsetof(sdap_cfg_s, sdap_hdr_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sdap_cfg_s::sdap_hdr_ul)>(),
     decltype(sdap_cfg_s::sdap_hdr_ul)::has_ext,
     false,
     decltype(sdap_cfg_s::sdap_hdr_ul)::nof_types,
     decltype(sdap_cfg_s::sdap_hdr_ul)::nof_exts,
     nullptr,
     &sdap_cfg_s::sdap_hdr_ul_opts::desc},
    {offsetof(sdap_cfg_s, default_drb),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sdap_cfg_s, mapped_qos_flows_to_add),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(sdap_cfg_s::mapped_qos_flows_to_add),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, false>>::value},
    {offsetof(sdap_cfg_s, mapped_qos_flows_to_release),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(sdap_cfg_s::mapped_qos_flows_to_release),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, false>>::value}};
static const asn1::table::field_name seq_table_sdap_cfg_s_names[] = {{"pdu-Session", nullptr},
                                                                     {"sdap-HeaderDL", nullptr},
                                                                     {"sdap-HeaderUL", nullptr},
                                                                     {"defaultDRB", nullptr},
                                                                     {"mappedQoS-FlowsToAdd", nullptr},
                                                                     {"mappedQoS-FlowsToRelease", nullptr}};
static const asn1::table::seq_opt    seq_table_sdap_cfg_s_opts[]  = {{4, asn1::table::seq_field::list_presence},
                                                                     {5, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          sdap_cfg_s::desc             = {
    {seq_table_sdap_cfg_s_fields, 6, UINT64_C(0xf), seq_table_sdap_cfg_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sdap_cfg_s_names};

static const char*           enum_names_sdap_cfg_s_sdap_hdr_dl_e__values[] = {"present", "absent"};
const asn1::table::enum_desc sdap_cfg_s::sdap_hdr_dl_opts::desc = {enum_names_sdap_cfg_s_sdap_hdr_dl_e__values,
                                                                   2,
                                                                   "sdap_cfg_s::sdap_hdr_dl_e_"};

static const char*           enum_names_sdap_cfg_s_sdap_hdr_ul_e__values[] = {"present", "absent"};
const asn1::table::enum_desc sdap_cfg_s::sdap_hdr_ul_opts::desc = {enum_names_sdap_cfg_s_sdap_hdr_ul_e__values,
                                                                   2,
                                                                   "sdap_cfg_s::sdap_hdr_ul_e_"};

// DRB-ToAddMod ::= SEQUENCE
static const asn1::table::seq_field seq_table_drb_to_add_mod_s_fields[] = {
    {offsetof(drb_to_add_mod_s, cn_assoc),
     (int32_t)offsetof(drb_to_add_mod_s, cn_assoc_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &drb_to_add_mod_s::cn_assoc_c_::desc},
    {offsetof(drb_to_add_mod_s, drb_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {0,
     (int32_t)offsetof(drb_to_add_mod_s, reestablish_pdcp_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(drb_to_add_mod_s, recover_pdcp_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(drb_to_add_mod_s, pdcp_cfg),
     (int32_t)offsetof(drb_to_add_mod_s, pdcp_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc}};
static const asn1::table::field_name seq_table_drb_to_add_mod_s_names[] = {{"cnAssociation", nullptr},
                                                                           {"drb-Identity", nullptr},
                                                                           {"reestablishPDCP", "true"},
                                                                           {"recoverPDCP", "true"},
                                                                           {"pdcp-Config", nullptr}};
static const asn1::table::seq_opt    seq_table_drb_to_add_mod_s_opts[]  = {
    {0, (int32_t)offsetof(drb_to_add_mod_s, cn_assoc_present)},
    {2, (int32_t)offsetof(drb_to_add_mod_s, reestablish_pdcp_present)},
    {3, (int32_t)offsetof(drb_to_add_mod_s, recover_pdcp_present)},
    {4, (int32_t)offsetof(drb_to_add_mod_s, pdcp_cfg_present)}};
static const asn1::table::seq_field seq_table_drb_to_add_mod_s_g0_fields[] = {
    {0,
     (int32_t)offsetof(drb_to_add_mod_s, daps_cfg_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_drb_to_add_mod_s_g0_names[] = {{"daps-Config-r16", "true"}};
static const asn1::table::seq_opt    seq_table_drb_to_add_mod_s_g0_opts[]  = {
    {0, (int32_t)offsetof(drb_to_add_mod_s, daps_cfg_r16_present)}};
static const asn1::table::seq_field seq_table_drb_to_add_mod_s_g1_fields[] = {
    {0,
     (int32_t)offsetof(drb_to_add_mod_s, n3c_bearer_associated_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_drb_to_add_mod_s_g1_names[] = {{"n3c-BearerAssociated-r18", "true"}};
static const asn1::table::seq_opt    seq_table_drb_to_add_mod_s_g1_opts[]  = {
    {0, (int32_t)offsetof(drb_to_add_mod_s, n3c_bearer_associated_r18_present)}};
static const asn1::table::seq_group seq_table_drb_to_add_mod_s_groups[] = {
    {{seq_table_drb_to_add_mod_s_g0_fields, 1, UINT64_C(0x0), seq_table_drb_to_add_mod_s_g0_opts, 1},
     false,
     seq_table_drb_to_add_mod_s_g0_names},
    {{seq_table_drb_to_add_mod_s_g1_fields, 1, UINT64_C(0x0), seq_table_drb_to_add_mod_s_g1_opts, 1},
     false,
     seq_table_drb_to_add_mod_s_g1_names}};
const asn1::table::seq_desc drb_to_add_mod_s::desc = {
    {seq_table_drb_to_add_mod_s_fields, 5, UINT64_C(0x2), seq_table_drb_to_add_mod_s_opts, 4},
    true,
    false,
    seq_table_drb_to_add_mod_s_groups,
    2,
    seq_table_drb_to_add_mod_s_names};

static const asn1::table::storage_ops* const choice_storage_drb_to_add_mod_s_cn_assoc_c__alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<sdap_cfg_s, false>::value};
const asn1::table::choice_storage_desc drb_to_add_mod_s::cn_assoc_c_::storage_desc = {
    choice_storage_drb_to_add_mod_s_cn_assoc_c__alternatives,
    2,
    "drb_to_add_mod_s::cn_assoc_c_"};

uint8_t& drb_to_add_mod_s::cn_assoc_c_::set_eps_bearer_id()
{
  set(types::eps_bearer_id);
  return c.get<uint8_t>();
}
sdap_cfg_s& drb_to_add_mod_s::cn_assoc_c_::set_sdap_cfg()
{
  set(types::sdap_cfg);
  return c.get<sdap_cfg_s>();
}
static const asn1::table::seq_field choice_table_drb_to_add_mod_s_cn_assoc_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sdap_cfg_s::desc}};
static const asn1::table::field_name choice_table_drb_to_add_mod_s_cn_assoc_c__names[] = {
    {"eps-BearerIdentity", nullptr},
    {"sdap-Config", nullptr}};
const asn1::table::choice_desc drb_to_add_mod_s::cn_assoc_c_::desc = {
    asn1::table::enum_int_type<drb_to_add_mod_s::cn_assoc_c_::types>(),
    drb_to_add_mod_s::cn_assoc_c_::types::nof_types,
    drb_to_add_mod_s::cn_assoc_c_::types::nof_exts,
    drb_to_add_mod_s::cn_assoc_c_::types::has_ext,
    false,
    choice_table_drb_to_add_mod_s_cn_assoc_c__alternatives,
    nullptr,
    choice_table_drb_to_add_mod_s_cn_assoc_c__names,
    "drb_to_add_mod_s::cn_assoc_c_",
    offsetof(drb_to_add_mod_s::cn_assoc_c_, c.type),
    offsetof(drb_to_add_mod_s::cn_assoc_c_, c.value),
    false,
    &drb_to_add_mod_s::cn_assoc_c_::storage_desc};

static const char* enum_names_drb_to_add_mod_s_cn_assoc_c__types_values[]    = {"eps-BearerIdentity", "sdap-Config"};
const asn1::table::enum_desc drb_to_add_mod_s::cn_assoc_c_::types_opts::desc = {
    enum_names_drb_to_add_mod_s_cn_assoc_c__types_values,
    2,
    "drb_to_add_mod_s::cn_assoc_c_::types"};

// IntegrityProtAlgorithm ::= ENUMERATED
static const char* enum_names_integrity_prot_algorithm_e_values[] =
    {"nia0", "nia1", "nia2", "nia3", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc integrity_prot_algorithm_opts::desc = {enum_names_integrity_prot_algorithm_e_values,
                                                                    8,
                                                                    "integrity_prot_algorithm_e"};
uint8_t                      integrity_prot_algorithm_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3};
  return map_enum_number(numbers, 4, value, "integrity_prot_algorithm_e");
}

// SecurityAlgorithmConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_security_algorithm_cfg_s_fields[] = {
    {offsetof(security_algorithm_cfg_s, ciphering_algorithm),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(security_algorithm_cfg_s::ciphering_algorithm)>(),
     decltype(security_algorithm_cfg_s::ciphering_algorithm)::has_ext,
     false,
     decltype(security_algorithm_cfg_s::ciphering_algorithm)::nof_types,
     decltype(security_algorithm_cfg_s::ciphering_algorithm)::nof_exts,
     nullptr,
     &ciphering_algorithm_opts::desc},
    {offsetof(security_algorithm_cfg_s, integrity_prot_algorithm),
     (int32_t)offsetof(security_algorithm_cfg_s, integrity_prot_algorithm_present),
     asn1::table::enum_op<decltype(security_algorithm_cfg_s::integrity_prot_algorithm)>(),
     decltype(security_algorithm_cfg_s::integrity_prot_algorithm)::has_ext,
     false,
     decltype(security_algorithm_cfg_s::integrity_prot_algorithm)::nof_types,
     decltype(security_algorithm_cfg_s::integrity_prot_algorithm)::nof_exts,
     nullptr,
     &integrity_prot_algorithm_opts::desc}};
static const asn1::table::field_name seq_table_security_algorithm_cfg_s_names[] = {{"cipheringAlgorithm", nullptr},
                                                                                   {"integrityProtAlgorithm", nullptr}};
static const asn1::table::seq_opt    seq_table_security_algorithm_cfg_s_opts[]  = {
    {1, (int32_t)offsetof(security_algorithm_cfg_s, integrity_prot_algorithm_present)}};
const asn1::table::seq_desc security_algorithm_cfg_s::desc = {
    {seq_table_security_algorithm_cfg_s_fields, 2, UINT64_C(0x1), seq_table_security_algorithm_cfg_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_security_algorithm_cfg_s_names};

// SecurityConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_security_cfg_s_fields[] = {
    {offsetof(security_cfg_s, security_algorithm_cfg),
     (int32_t)offsetof(security_cfg_s, security_algorithm_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_algorithm_cfg_s::desc},
    {offsetof(security_cfg_s, key_to_use),
     (int32_t)offsetof(security_cfg_s, key_to_use_present),
     asn1::table::enum_op<decltype(security_cfg_s::key_to_use)>(),
     decltype(security_cfg_s::key_to_use)::has_ext,
     false,
     decltype(security_cfg_s::key_to_use)::nof_types,
     decltype(security_cfg_s::key_to_use)::nof_exts,
     nullptr,
     &security_cfg_s::key_to_use_opts::desc}};
static const asn1::table::field_name seq_table_security_cfg_s_names[] = {{"securityAlgorithmConfig", nullptr},
                                                                         {"keyToUse", nullptr}};
static const asn1::table::seq_opt    seq_table_security_cfg_s_opts[]  = {
    {0, (int32_t)offsetof(security_cfg_s, security_algorithm_cfg_present)},
    {1, (int32_t)offsetof(security_cfg_s, key_to_use_present)}};
const asn1::table::seq_desc security_cfg_s::desc = {
    {seq_table_security_cfg_s_fields, 2, UINT64_C(0x0), seq_table_security_cfg_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_security_cfg_s_names};

static const char*           enum_names_security_cfg_s_key_to_use_e__values[] = {"master", "secondary"};
const asn1::table::enum_desc security_cfg_s::key_to_use_opts::desc = {enum_names_security_cfg_s_key_to_use_e__values,
                                                                      2,
                                                                      "security_cfg_s::key_to_use_e_"};

// TMGI-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_tmgi_r17_s_fields[] = {
    {offsetof(tmgi_r17_s, plmn_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &tmgi_r17_s::plmn_id_r17_c_::desc},
    {offsetof(tmgi_r17_s, service_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tmgi_r17_s::service_id_r17), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_tmgi_r17_s_names[] = {{"plmn-Id-r17", nullptr},
                                                                     {"serviceId-r17", nullptr}};
const asn1::table::seq_desc          tmgi_r17_s::desc =
    {{seq_table_tmgi_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0}, false, false, nullptr, 0, seq_table_tmgi_r17_s_names};

static const asn1::table::storage_ops* const choice_storage_tmgi_r17_s_plmn_id_r17_c__alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<plmn_id_s, false>::value};
const asn1::table::choice_storage_desc tmgi_r17_s::plmn_id_r17_c_::storage_desc = {
    choice_storage_tmgi_r17_s_plmn_id_r17_c__alternatives,
    2,
    "tmgi_r17_s::plmn_id_r17_c_"};

uint8_t& tmgi_r17_s::plmn_id_r17_c_::set_plmn_idx()
{
  set(types::plmn_idx);
  return c.get<uint8_t>();
}
plmn_id_s& tmgi_r17_s::plmn_id_r17_c_::set_explicit_value()
{
  set(types::explicit_value);
  return c.get<plmn_id_s>();
}
static const asn1::table::seq_field choice_table_tmgi_r17_s_plmn_id_r17_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(12),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc}};
static const asn1::table::field_name choice_table_tmgi_r17_s_plmn_id_r17_c__names[] = {{"plmn-Index", nullptr},
                                                                                       {"explicitValue", nullptr}};
const asn1::table::choice_desc       tmgi_r17_s::plmn_id_r17_c_::desc               = {
    asn1::table::enum_int_type<tmgi_r17_s::plmn_id_r17_c_::types>(),
    tmgi_r17_s::plmn_id_r17_c_::types::nof_types,
    tmgi_r17_s::plmn_id_r17_c_::types::nof_exts,
    tmgi_r17_s::plmn_id_r17_c_::types::has_ext,
    false,
    choice_table_tmgi_r17_s_plmn_id_r17_c__alternatives,
    nullptr,
    choice_table_tmgi_r17_s_plmn_id_r17_c__names,
    "tmgi_r17_s::plmn_id_r17_c_",
    offsetof(tmgi_r17_s::plmn_id_r17_c_, c.type),
    offsetof(tmgi_r17_s::plmn_id_r17_c_, c.value),
    false,
    &tmgi_r17_s::plmn_id_r17_c_::storage_desc};

static const char*           enum_names_tmgi_r17_s_plmn_id_r17_c__types_values[] = {"plmn-Index", "explicitValue"};
const asn1::table::enum_desc tmgi_r17_s::plmn_id_r17_c_::types_opts::desc        = {
    enum_names_tmgi_r17_s_plmn_id_r17_c__types_values,
    2,
    "tmgi_r17_s::plmn_id_r17_c_::types"};

// MRB-ToAddMod-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrb_to_add_mod_r17_s_fields[] = {
    {offsetof(mrb_to_add_mod_r17_s, mbs_session_id_r17),
     (int32_t)offsetof(mrb_to_add_mod_r17_s, mbs_session_id_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tmgi_r17_s::desc},
    {offsetof(mrb_to_add_mod_r17_s, mrb_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(mrb_to_add_mod_r17_s, mrb_id_new_r17),
     (int32_t)offsetof(mrb_to_add_mod_r17_s, mrb_id_new_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {0,
     (int32_t)offsetof(mrb_to_add_mod_r17_s, reestablish_pdcp_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mrb_to_add_mod_r17_s, recover_pdcp_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(mrb_to_add_mod_r17_s, pdcp_cfg_r17),
     (int32_t)offsetof(mrb_to_add_mod_r17_s, pdcp_cfg_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcp_cfg_s::desc}};
static const asn1::table::field_name seq_table_mrb_to_add_mod_r17_s_names[] = {{"mbs-SessionId-r17", nullptr},
                                                                               {"mrb-Identity-r17", nullptr},
                                                                               {"mrb-IdentityNew-r17", nullptr},
                                                                               {"reestablishPDCP-r17", "true"},
                                                                               {"recoverPDCP-r17", "true"},
                                                                               {"pdcp-Config-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_mrb_to_add_mod_r17_s_opts[]  = {
    {0, (int32_t)offsetof(mrb_to_add_mod_r17_s, mbs_session_id_r17_present)},
    {2, (int32_t)offsetof(mrb_to_add_mod_r17_s, mrb_id_new_r17_present)},
    {3, (int32_t)offsetof(mrb_to_add_mod_r17_s, reestablish_pdcp_r17_present)},
    {4, (int32_t)offsetof(mrb_to_add_mod_r17_s, recover_pdcp_r17_present)},
    {5, (int32_t)offsetof(mrb_to_add_mod_r17_s, pdcp_cfg_r17_present)}};
const asn1::table::seq_desc mrb_to_add_mod_r17_s::desc = {
    {seq_table_mrb_to_add_mod_r17_s_fields, 6, UINT64_C(0x2), seq_table_mrb_to_add_mod_r17_s_opts, 5},
    true,
    false,
    nullptr,
    0,
    seq_table_mrb_to_add_mod_r17_s_names};

// RadioBearerConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_radio_bearer_cfg_s_fields[] = {
    {offsetof(radio_bearer_cfg_s, srb_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2,
     &asn1::table::item_list_codec<decltype(radio_bearer_cfg_s::srb_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &srb_to_add_mod_s::desc},
    {0,
     (int32_t)offsetof(radio_bearer_cfg_s, srb3_to_release_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(radio_bearer_cfg_s, drb_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     29,
     &asn1::table::item_list_codec<decltype(radio_bearer_cfg_s::drb_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drb_to_add_mod_s::desc},
    {offsetof(radio_bearer_cfg_s, drb_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     29,
     &asn1::table::list_codec<decltype(radio_bearer_cfg_s::drb_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(32), false, false>>::value},
    {offsetof(radio_bearer_cfg_s, security_cfg),
     (int32_t)offsetof(radio_bearer_cfg_s, security_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &security_cfg_s::desc}};
static const asn1::table::field_name seq_table_radio_bearer_cfg_s_names[] = {{"srb-ToAddModList", nullptr},
                                                                             {"srb3-ToRelease", "true"},
                                                                             {"drb-ToAddModList", nullptr},
                                                                             {"drb-ToReleaseList", nullptr},
                                                                             {"securityConfig", nullptr}};
static const asn1::table::seq_opt    seq_table_radio_bearer_cfg_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(radio_bearer_cfg_s, srb3_to_release_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(radio_bearer_cfg_s, security_cfg_present)}};
static const asn1::table::seq_field seq_table_radio_bearer_cfg_s_g0_fields[] = {
    {offsetof(radio_bearer_cfg_s, mrb_to_add_mod_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::ptr_item_list_codec<decltype(radio_bearer_cfg_s::mrb_to_add_mod_list_r17),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &mrb_to_add_mod_r17_s::desc},
    {offsetof(radio_bearer_cfg_s, mrb_to_release_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::ptr_list_codec<decltype(radio_bearer_cfg_s::mrb_to_release_list_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(1), UINT64_C(512), false, false>>::value},
    {offsetof(radio_bearer_cfg_s, srb4_to_add_mod_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(radio_bearer_cfg_s::srb4_to_add_mod_r17)>::value,
     &srb_to_add_mod_s::desc},
    {0,
     (int32_t)offsetof(radio_bearer_cfg_s, srb4_to_release_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_radio_bearer_cfg_s_g0_names[] = {{"mrb-ToAddModList-r17", nullptr},
                                                                                {"mrb-ToReleaseList-r17", nullptr},
                                                                                {"srb4-ToAddMod-r17", nullptr},
                                                                                {"srb4-ToRelease-r17", "true"}};
static const asn1::table::seq_opt    seq_table_radio_bearer_cfg_s_g0_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, (int32_t)offsetof(radio_bearer_cfg_s, srb4_to_release_r17_present)}};
static const asn1::table::seq_field seq_table_radio_bearer_cfg_s_g1_fields[] = {
    {offsetof(radio_bearer_cfg_s, srb5_to_add_mod_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(radio_bearer_cfg_s::srb5_to_add_mod_r18)>::value,
     &srb_to_add_mod_s::desc},
    {0,
     (int32_t)offsetof(radio_bearer_cfg_s, srb5_to_release_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_radio_bearer_cfg_s_g1_names[] = {{"srb5-ToAddMod-r18", nullptr},
                                                                                {"srb5-ToRelease-r18", "true"}};
static const asn1::table::seq_opt    seq_table_radio_bearer_cfg_s_g1_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(radio_bearer_cfg_s, srb5_to_release_r18_present)}};
static const asn1::table::seq_group seq_table_radio_bearer_cfg_s_groups[] = {
    {{seq_table_radio_bearer_cfg_s_g0_fields, 4, UINT64_C(0x0), seq_table_radio_bearer_cfg_s_g0_opts, 4},
     false,
     seq_table_radio_bearer_cfg_s_g0_names},
    {{seq_table_radio_bearer_cfg_s_g1_fields, 2, UINT64_C(0x0), seq_table_radio_bearer_cfg_s_g1_opts, 2},
     false,
     seq_table_radio_bearer_cfg_s_g1_names}};
const asn1::table::seq_desc radio_bearer_cfg_s::desc = {
    {seq_table_radio_bearer_cfg_s_fields, 5, UINT64_C(0x0), seq_table_radio_bearer_cfg_s_opts, 5},
    true,
    false,
    seq_table_radio_bearer_cfg_s_groups,
    2,
    seq_table_radio_bearer_cfg_s_names};
