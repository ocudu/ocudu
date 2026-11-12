// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/cell_group_config.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// AutonomousDenialParameters-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_autonomous_denial_params_r18_s_fields[] = {
    {offsetof(autonomous_denial_params_r18_s, autonomous_denial_slots_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(autonomous_denial_params_r18_s::autonomous_denial_slots_r18)>(),
     decltype(autonomous_denial_params_r18_s::autonomous_denial_slots_r18)::has_ext,
     false,
     decltype(autonomous_denial_params_r18_s::autonomous_denial_slots_r18)::nof_types,
     decltype(autonomous_denial_params_r18_s::autonomous_denial_slots_r18)::nof_exts,
     nullptr,
     &autonomous_denial_params_r18_s::autonomous_denial_slots_r18_opts::desc},
    {offsetof(autonomous_denial_params_r18_s, autonomous_denial_validity_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(autonomous_denial_params_r18_s::autonomous_denial_validity_r18)>(),
     decltype(autonomous_denial_params_r18_s::autonomous_denial_validity_r18)::has_ext,
     false,
     decltype(autonomous_denial_params_r18_s::autonomous_denial_validity_r18)::nof_types,
     decltype(autonomous_denial_params_r18_s::autonomous_denial_validity_r18)::nof_exts,
     nullptr,
     &autonomous_denial_params_r18_s::autonomous_denial_validity_r18_opts::desc}};
static const asn1::table::field_name seq_table_autonomous_denial_params_r18_s_names[] = {
    {"autonomousDenialSlots-r18", nullptr},
    {"autonomousDenialValidity-r18", nullptr}};
const asn1::table::seq_desc autonomous_denial_params_r18_s::desc = {{seq_table_autonomous_denial_params_r18_s_fields,
                                                                     uint64_t{0x3},
                                                                     nullptr,
                                                                     seq_table_autonomous_denial_params_r18_s_names,
                                                                     2,
                                                                     0},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0};

static const char* enum_names_autonomous_denial_params_r18_s_autonomous_denial_slots_r18_e__values[] =
    {"n2", "n5", "n10", "n15", "n20", "n30", "spare2", "spare1"};
const asn1::table::enum_desc autonomous_denial_params_r18_s::autonomous_denial_slots_r18_opts::desc = {
    enum_names_autonomous_denial_params_r18_s_autonomous_denial_slots_r18_e__values,
    8,
    "autonomous_denial_params_r18_s::autonomous_denial_slots_r18_e_"};
uint8_t autonomous_denial_params_r18_s::autonomous_denial_slots_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 5, 10, 15, 20, 30};
  return map_enum_number(numbers, 6, value, "autonomous_denial_params_r18_s::autonomous_denial_slots_r18_e_");
}

static const char* enum_names_autonomous_denial_params_r18_s_autonomous_denial_validity_r18_e__values[] = {"n200",
                                                                                                           "n500",
                                                                                                           "n1000",
                                                                                                           "n2000"};
const asn1::table::enum_desc autonomous_denial_params_r18_s::autonomous_denial_validity_r18_opts::desc  = {
    enum_names_autonomous_denial_params_r18_s_autonomous_denial_validity_r18_e__values,
    4,
    "autonomous_denial_params_r18_s::autonomous_denial_validity_r18_e_"};
uint16_t autonomous_denial_params_r18_s::autonomous_denial_validity_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {200, 500, 1000, 2000};
  return map_enum_number(numbers, 4, value, "autonomous_denial_params_r18_s::autonomous_denial_validity_r18_e_");
}

// BH-LogicalChannelIdentity-r16 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_bh_lc_ch_id_r16_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value};
const asn1::table::choice_storage_desc bh_lc_ch_id_r16_c::storage_desc = {choice_storage_bh_lc_ch_id_r16_c_alternatives,
                                                                          2,
                                                                          "bh_lc_ch_id_r16_c"};

uint8_t& bh_lc_ch_id_r16_c::set_bh_lc_ch_id_r16()
{
  set(types::bh_lc_ch_id_r16);
  return c.get<uint8_t>();
}
uint32_t& bh_lc_ch_id_r16_c::set_bh_lc_ch_id_ext_r16()
{
  set(types::bh_lc_ch_id_ext_r16);
  return c.get<uint32_t>();
}
static const asn1::table::seq_field  choice_table_bh_lc_ch_id_r16_c_alternatives[] = {{0,
                                                                                       asn1::table::seq_field::mandatory,
                                                                                       asn1::table::field_op::int_u8,
                                                                                       false,
                                                                                       false,
                                                                                       int64_t{1},
                                                                                       uint64_t{31},
                                                                                       nullptr},
                                                                                      {0,
                                                                                       asn1::table::seq_field::mandatory,
                                                                                       asn1::table::field_op::int_u32,
                                                                                       false,
                                                                                       false,
                                                                                       int64_t{320},
                                                                                       uint64_t{65535},
                                                                                       nullptr}};
static const asn1::table::field_name choice_table_bh_lc_ch_id_r16_c_names[]        = {
    {"bh-LogicalChannelIdentity-r16", nullptr},
    {"bh-LogicalChannelIdentityExt-r16", nullptr}};
const asn1::table::choice_desc bh_lc_ch_id_r16_c::desc = {asn1::table::enum_int_type<bh_lc_ch_id_r16_c::types>(),
                                                          bh_lc_ch_id_r16_c::types::nof_types,
                                                          bh_lc_ch_id_r16_c::types::nof_exts,
                                                          bh_lc_ch_id_r16_c::types::has_ext,
                                                          false,
                                                          choice_table_bh_lc_ch_id_r16_c_alternatives,
                                                          nullptr,
                                                          choice_table_bh_lc_ch_id_r16_c_names,
                                                          "bh_lc_ch_id_r16_c",
                                                          offsetof(bh_lc_ch_id_r16_c, c.type),
                                                          offsetof(bh_lc_ch_id_r16_c, c.value),
                                                          false,
                                                          &bh_lc_ch_id_r16_c::storage_desc};

static const char*           enum_names_bh_lc_ch_id_r16_c_types_values[] = {"bh-LogicalChannelIdentity-r16",
                                                                            "bh-LogicalChannelIdentityExt-r16"};
const asn1::table::enum_desc bh_lc_ch_id_r16_c::types_opts::desc         = {enum_names_bh_lc_ch_id_r16_c_types_values,
                                                                            2,
                                                                            "bh_lc_ch_id_r16_c::types"};

// UL-AM-RLC ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_am_rlc_s_fields[] = {
    {offsetof(ul_am_rlc_s, sn_field_len),
     (int32_t)offsetof(ul_am_rlc_s, sn_field_len_present),
     asn1::table::enum_op<decltype(ul_am_rlc_s::sn_field_len)>(),
     decltype(ul_am_rlc_s::sn_field_len)::has_ext,
     false,
     decltype(ul_am_rlc_s::sn_field_len)::nof_types,
     decltype(ul_am_rlc_s::sn_field_len)::nof_exts,
     nullptr,
     &sn_field_len_am_opts::desc},
    {offsetof(ul_am_rlc_s, t_poll_retx),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ul_am_rlc_s::t_poll_retx)>(),
     decltype(ul_am_rlc_s::t_poll_retx)::has_ext,
     false,
     decltype(ul_am_rlc_s::t_poll_retx)::nof_types,
     decltype(ul_am_rlc_s::t_poll_retx)::nof_exts,
     nullptr,
     &t_poll_retx_opts::desc},
    {offsetof(ul_am_rlc_s, poll_pdu),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ul_am_rlc_s::poll_pdu)>(),
     decltype(ul_am_rlc_s::poll_pdu)::has_ext,
     false,
     decltype(ul_am_rlc_s::poll_pdu)::nof_types,
     decltype(ul_am_rlc_s::poll_pdu)::nof_exts,
     nullptr,
     &poll_pdu_opts::desc},
    {offsetof(ul_am_rlc_s, poll_byte),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ul_am_rlc_s::poll_byte)>(),
     decltype(ul_am_rlc_s::poll_byte)::has_ext,
     false,
     decltype(ul_am_rlc_s::poll_byte)::nof_types,
     decltype(ul_am_rlc_s::poll_byte)::nof_exts,
     nullptr,
     &poll_byte_opts::desc},
    {offsetof(ul_am_rlc_s, max_retx_thres),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ul_am_rlc_s::max_retx_thres)>(),
     decltype(ul_am_rlc_s::max_retx_thres)::has_ext,
     false,
     decltype(ul_am_rlc_s::max_retx_thres)::nof_types,
     decltype(ul_am_rlc_s::max_retx_thres)::nof_exts,
     nullptr,
     &ul_am_rlc_s::max_retx_thres_opts::desc}};
static const asn1::table::field_name seq_table_ul_am_rlc_s_names[] = {{"sn-FieldLength", nullptr},
                                                                      {"t-PollRetransmit", nullptr},
                                                                      {"pollPDU", nullptr},
                                                                      {"pollByte", nullptr},
                                                                      {"maxRetxThreshold", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_am_rlc_s_opts[]  = {
    {0, (int32_t)offsetof(ul_am_rlc_s, sn_field_len_present)}};
const asn1::table::seq_desc ul_am_rlc_s::desc = {
    {seq_table_ul_am_rlc_s_fields, uint64_t{0x1e}, seq_table_ul_am_rlc_s_opts, seq_table_ul_am_rlc_s_names, 5, 1},
    false,
    false,
    nullptr,
    0};

static const char* enum_names_ul_am_rlc_s_max_retx_thres_e__values[] =
    {"t1", "t2", "t3", "t4", "t6", "t8", "t16", "t32"};
const asn1::table::enum_desc ul_am_rlc_s::max_retx_thres_opts::desc = {enum_names_ul_am_rlc_s_max_retx_thres_e__values,
                                                                       8,
                                                                       "ul_am_rlc_s::max_retx_thres_e_"};
uint8_t                      ul_am_rlc_s::max_retx_thres_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 8, value, "ul_am_rlc_s::max_retx_thres_e_");
}

// T-Reassembly ::= ENUMERATED
static const char* enum_names_t_reassembly_e_values[] = {
    "ms0",   "ms5",   "ms10",  "ms15",  "ms20",  "ms25",  "ms30",  "ms35",  "ms40",  "ms45",  "ms50",
    "ms55",  "ms60",  "ms65",  "ms70",  "ms75",  "ms80",  "ms85",  "ms90",  "ms95",  "ms100", "ms110",
    "ms120", "ms130", "ms140", "ms150", "ms160", "ms170", "ms180", "ms190", "ms200", "spare1"};
const asn1::table::enum_desc t_reassembly_opts::desc = {enum_names_t_reassembly_e_values, 32, "t_reassembly_e"};
uint8_t                      t_reassembly_opts::to_number() const
{
  static const uint8_t numbers[] = {0,  5,  10, 15, 20,  25,  30,  35,  40,  45,  50,  55,  60,  65,  70, 75,
                                    80, 85, 90, 95, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200};
  return map_enum_number(numbers, 31, value, "t_reassembly_e");
}

// T-StatusProhibit ::= ENUMERATED
static const char* enum_names_t_status_prohibit_e_values[] = {
    "ms0",   "ms5",   "ms10",   "ms15",   "ms20",   "ms25",   "ms30",   "ms35",   "ms40",  "ms45",  "ms50",
    "ms55",  "ms60",  "ms65",   "ms70",   "ms75",   "ms80",   "ms85",   "ms90",   "ms95",  "ms100", "ms105",
    "ms110", "ms115", "ms120",  "ms125",  "ms130",  "ms135",  "ms140",  "ms145",  "ms150", "ms155", "ms160",
    "ms165", "ms170", "ms175",  "ms180",  "ms185",  "ms190",  "ms195",  "ms200",  "ms205", "ms210", "ms215",
    "ms220", "ms225", "ms230",  "ms235",  "ms240",  "ms245",  "ms250",  "ms300",  "ms350", "ms400", "ms450",
    "ms500", "ms800", "ms1000", "ms1200", "ms1600", "ms2000", "ms2400", "spare2", "spare1"};
const asn1::table::enum_desc t_status_prohibit_opts::desc = {enum_names_t_status_prohibit_e_values,
                                                             64,
                                                             "t_status_prohibit_e"};
uint16_t                     t_status_prohibit_opts::to_number() const
{
  static const uint16_t numbers[] = {0,   5,   10,  15,  20,  25,  30,  35,  40,  45,   50,   55,   60,   65,  70,  75,
                                     80,  85,  90,  95,  100, 105, 110, 115, 120, 125,  130,  135,  140,  145, 150, 155,
                                     160, 165, 170, 175, 180, 185, 190, 195, 200, 205,  210,  215,  220,  225, 230, 235,
                                     240, 245, 250, 300, 350, 400, 450, 500, 800, 1000, 1200, 1600, 2000, 2400};
  return map_enum_number(numbers, 62, value, "t_status_prohibit_e");
}

// DL-AM-RLC ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_am_rlc_s_fields[] = {
    {offsetof(dl_am_rlc_s, sn_field_len),
     (int32_t)offsetof(dl_am_rlc_s, sn_field_len_present),
     asn1::table::enum_op<decltype(dl_am_rlc_s::sn_field_len)>(),
     decltype(dl_am_rlc_s::sn_field_len)::has_ext,
     false,
     decltype(dl_am_rlc_s::sn_field_len)::nof_types,
     decltype(dl_am_rlc_s::sn_field_len)::nof_exts,
     nullptr,
     &sn_field_len_am_opts::desc},
    {offsetof(dl_am_rlc_s, t_reassembly),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(dl_am_rlc_s::t_reassembly)>(),
     decltype(dl_am_rlc_s::t_reassembly)::has_ext,
     false,
     decltype(dl_am_rlc_s::t_reassembly)::nof_types,
     decltype(dl_am_rlc_s::t_reassembly)::nof_exts,
     nullptr,
     &t_reassembly_opts::desc},
    {offsetof(dl_am_rlc_s, t_status_prohibit),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(dl_am_rlc_s::t_status_prohibit)>(),
     decltype(dl_am_rlc_s::t_status_prohibit)::has_ext,
     false,
     decltype(dl_am_rlc_s::t_status_prohibit)::nof_types,
     decltype(dl_am_rlc_s::t_status_prohibit)::nof_exts,
     nullptr,
     &t_status_prohibit_opts::desc}};
static const asn1::table::field_name seq_table_dl_am_rlc_s_names[] = {{"sn-FieldLength", nullptr},
                                                                      {"t-Reassembly", nullptr},
                                                                      {"t-StatusProhibit", nullptr}};
static const asn1::table::seq_opt    seq_table_dl_am_rlc_s_opts[]  = {
    {0, (int32_t)offsetof(dl_am_rlc_s, sn_field_len_present)}};
const asn1::table::seq_desc dl_am_rlc_s::desc = {
    {seq_table_dl_am_rlc_s_fields, uint64_t{0x6}, seq_table_dl_am_rlc_s_opts, seq_table_dl_am_rlc_s_names, 3, 1},
    false,
    false,
    nullptr,
    0};

// UL-UM-RLC ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_um_rlc_s_fields[] = {
    {offsetof(ul_um_rlc_s, sn_field_len),
     (int32_t)offsetof(ul_um_rlc_s, sn_field_len_present),
     asn1::table::enum_op<decltype(ul_um_rlc_s::sn_field_len)>(),
     decltype(ul_um_rlc_s::sn_field_len)::has_ext,
     false,
     decltype(ul_um_rlc_s::sn_field_len)::nof_types,
     decltype(ul_um_rlc_s::sn_field_len)::nof_exts,
     nullptr,
     &sn_field_len_um_opts::desc}};
static const asn1::table::field_name seq_table_ul_um_rlc_s_names[] = {{"sn-FieldLength", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_um_rlc_s_opts[]  = {
    {0, (int32_t)offsetof(ul_um_rlc_s, sn_field_len_present)}};
const asn1::table::seq_desc ul_um_rlc_s::desc = {
    {seq_table_ul_um_rlc_s_fields, uint64_t{0x0}, seq_table_ul_um_rlc_s_opts, seq_table_ul_um_rlc_s_names, 1, 1},
    false,
    false,
    nullptr,
    0};

// DL-UM-RLC ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_um_rlc_s_fields[] = {
    {offsetof(dl_um_rlc_s, sn_field_len),
     (int32_t)offsetof(dl_um_rlc_s, sn_field_len_present),
     asn1::table::enum_op<decltype(dl_um_rlc_s::sn_field_len)>(),
     decltype(dl_um_rlc_s::sn_field_len)::has_ext,
     false,
     decltype(dl_um_rlc_s::sn_field_len)::nof_types,
     decltype(dl_um_rlc_s::sn_field_len)::nof_exts,
     nullptr,
     &sn_field_len_um_opts::desc},
    {offsetof(dl_um_rlc_s, t_reassembly),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(dl_um_rlc_s::t_reassembly)>(),
     decltype(dl_um_rlc_s::t_reassembly)::has_ext,
     false,
     decltype(dl_um_rlc_s::t_reassembly)::nof_types,
     decltype(dl_um_rlc_s::t_reassembly)::nof_exts,
     nullptr,
     &t_reassembly_opts::desc}};
static const asn1::table::field_name seq_table_dl_um_rlc_s_names[] = {{"sn-FieldLength", nullptr},
                                                                      {"t-Reassembly", nullptr}};
static const asn1::table::seq_opt    seq_table_dl_um_rlc_s_opts[]  = {
    {0, (int32_t)offsetof(dl_um_rlc_s, sn_field_len_present)}};
const asn1::table::seq_desc dl_um_rlc_s::desc = {
    {seq_table_dl_um_rlc_s_fields, uint64_t{0x2}, seq_table_dl_um_rlc_s_opts, seq_table_dl_um_rlc_s_names, 2, 1},
    false,
    false,
    nullptr,
    0};

// RLC-Config ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_rlc_cfg_c_alternatives[] = {
    &asn1::table::alternative_ops<rlc_cfg_c::am_s_, false>::value,
    &asn1::table::alternative_ops<rlc_cfg_c::um_bi_dir_s_, false>::value,
    &asn1::table::alternative_ops<rlc_cfg_c::um_uni_dir_ul_s_, false>::value,
    &asn1::table::alternative_ops<rlc_cfg_c::um_uni_dir_dl_s_, false>::value};
const asn1::table::choice_storage_desc rlc_cfg_c::storage_desc = {choice_storage_rlc_cfg_c_alternatives,
                                                                  4,
                                                                  "rlc_cfg_c"};

rlc_cfg_c::am_s_& rlc_cfg_c::set_am()
{
  set(types::am);
  return c.get<am_s_>();
}
rlc_cfg_c::um_bi_dir_s_& rlc_cfg_c::set_um_bi_dir()
{
  set(types::um_bi_dir);
  return c.get<um_bi_dir_s_>();
}
rlc_cfg_c::um_uni_dir_ul_s_& rlc_cfg_c::set_um_uni_dir_ul()
{
  set(types::um_uni_dir_ul);
  return c.get<um_uni_dir_ul_s_>();
}
rlc_cfg_c::um_uni_dir_dl_s_& rlc_cfg_c::set_um_uni_dir_dl()
{
  set(types::um_uni_dir_dl);
  return c.get<um_uni_dir_dl_s_>();
}
static const asn1::table::seq_field  seq_table_rlc_cfg_c_am_s__fields[] = {{offsetof(rlc_cfg_c::am_s_, ul_am_rlc),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::sequence,
                                                                            false,
                                                                            false,
                                                                            0,
                                                                            0,
                                                                            nullptr,
                                                                            &ul_am_rlc_s::desc},
                                                                           {offsetof(rlc_cfg_c::am_s_, dl_am_rlc),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::sequence,
                                                                            false,
                                                                            false,
                                                                            0,
                                                                            0,
                                                                            nullptr,
                                                                            &dl_am_rlc_s::desc}};
static const asn1::table::field_name seq_table_rlc_cfg_c_am_s__names[]  = {{"ul-AM-RLC", nullptr},
                                                                           {"dl-AM-RLC", nullptr}};
static const asn1::table::seq_desc   seq_table_rlc_cfg_c_am_s_          = {
    {seq_table_rlc_cfg_c_am_s__fields, uint64_t{0x3}, nullptr, seq_table_rlc_cfg_c_am_s__names, 2, 0},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_rlc_cfg_c_um_bi_dir_s__fields[] = {
    {offsetof(rlc_cfg_c::um_bi_dir_s_, ul_um_rlc),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_um_rlc_s::desc},
    {offsetof(rlc_cfg_c::um_bi_dir_s_, dl_um_rlc),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_um_rlc_s::desc}};
static const asn1::table::field_name seq_table_rlc_cfg_c_um_bi_dir_s__names[] = {{"ul-UM-RLC", nullptr},
                                                                                 {"dl-UM-RLC", nullptr}};
static const asn1::table::seq_desc   seq_table_rlc_cfg_c_um_bi_dir_s_         = {
    {seq_table_rlc_cfg_c_um_bi_dir_s__fields, uint64_t{0x3}, nullptr, seq_table_rlc_cfg_c_um_bi_dir_s__names, 2, 0},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_rlc_cfg_c_um_uni_dir_ul_s__fields[] = {
    {offsetof(rlc_cfg_c::um_uni_dir_ul_s_, ul_um_rlc),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_um_rlc_s::desc}};
static const asn1::table::field_name seq_table_rlc_cfg_c_um_uni_dir_ul_s__names[] = {{"ul-UM-RLC", nullptr}};
static const asn1::table::seq_desc seq_table_rlc_cfg_c_um_uni_dir_ul_s_ = {{seq_table_rlc_cfg_c_um_uni_dir_ul_s__fields,
                                                                            uint64_t{0x1},
                                                                            nullptr,
                                                                            seq_table_rlc_cfg_c_um_uni_dir_ul_s__names,
                                                                            1,
                                                                            0},
                                                                           false,
                                                                           false,
                                                                           nullptr,
                                                                           0};
static const asn1::table::seq_field seq_table_rlc_cfg_c_um_uni_dir_dl_s__fields[] = {
    {offsetof(rlc_cfg_c::um_uni_dir_dl_s_, dl_um_rlc),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_um_rlc_s::desc}};
static const asn1::table::field_name seq_table_rlc_cfg_c_um_uni_dir_dl_s__names[] = {{"dl-UM-RLC", nullptr}};
static const asn1::table::seq_desc seq_table_rlc_cfg_c_um_uni_dir_dl_s_ = {{seq_table_rlc_cfg_c_um_uni_dir_dl_s__fields,
                                                                            uint64_t{0x1},
                                                                            nullptr,
                                                                            seq_table_rlc_cfg_c_um_uni_dir_dl_s__names,
                                                                            1,
                                                                            0},
                                                                           false,
                                                                           false,
                                                                           nullptr,
                                                                           0};
static const asn1::table::seq_field  choice_table_rlc_cfg_c_alternatives[] = {{0,
                                                                               asn1::table::seq_field::mandatory,
                                                                               asn1::table::field_op::sequence,
                                                                               false,
                                                                               false,
                                                                               0,
                                                                               0,
                                                                               nullptr,
                                                                               &seq_table_rlc_cfg_c_am_s_},
                                                                              {0,
                                                                               asn1::table::seq_field::mandatory,
                                                                               asn1::table::field_op::sequence,
                                                                               false,
                                                                               false,
                                                                               0,
                                                                               0,
                                                                               nullptr,
                                                                               &seq_table_rlc_cfg_c_um_bi_dir_s_},
                                                                              {0,
                                                                               asn1::table::seq_field::mandatory,
                                                                               asn1::table::field_op::sequence,
                                                                               false,
                                                                               false,
                                                                               0,
                                                                               0,
                                                                               nullptr,
                                                                               &seq_table_rlc_cfg_c_um_uni_dir_ul_s_},
                                                                              {0,
                                                                               asn1::table::seq_field::mandatory,
                                                                               asn1::table::field_op::sequence,
                                                                               false,
                                                                               false,
                                                                               0,
                                                                               0,
                                                                               nullptr,
                                                                               &seq_table_rlc_cfg_c_um_uni_dir_dl_s_}};
static const asn1::table::field_name choice_table_rlc_cfg_c_names[]        = {{"am", nullptr},
                                                                              {"um-Bi-Directional", nullptr},
                                                                              {"um-Uni-Directional-UL", nullptr},
                                                                              {"um-Uni-Directional-DL", nullptr}};
const asn1::table::choice_desc       rlc_cfg_c::desc = {asn1::table::enum_int_type<rlc_cfg_c::types>(),
                                                        rlc_cfg_c::types::nof_types,
                                                        rlc_cfg_c::types::nof_exts,
                                                        rlc_cfg_c::types::has_ext,
                                                        false,
                                                        choice_table_rlc_cfg_c_alternatives,
                                                        nullptr,
                                                        choice_table_rlc_cfg_c_names,
                                                        "rlc_cfg_c",
                                                        offsetof(rlc_cfg_c, c.type),
                                                        offsetof(rlc_cfg_c, c.value),
                                                        false,
                                                        &rlc_cfg_c::storage_desc};

static const char*           enum_names_rlc_cfg_c_types_values[] = {"am",
                                                                    "um-Bi-Directional",
                                                                    "um-Uni-Directional-UL",
                                                                    "um-Uni-Directional-DL"};
const asn1::table::enum_desc rlc_cfg_c::types_opts::desc = {enum_names_rlc_cfg_c_types_values, 4, "rlc_cfg_c::types"};

// LogicalChannelConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_lc_ch_cfg_s_fields[] = {
    {offsetof(lc_ch_cfg_s, ul_specific_params),
     (int32_t)offsetof(lc_ch_cfg_s, ul_specific_params_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &lc_ch_cfg_s::ul_specific_params_s_::desc}};
static const asn1::table::field_name seq_table_lc_ch_cfg_s_names[] = {{"ul-SpecificParameters", nullptr}};
static const asn1::table::seq_opt    seq_table_lc_ch_cfg_s_opts[]  = {
    {0, (int32_t)offsetof(lc_ch_cfg_s, ul_specific_params_present)}};
static const asn1::table::seq_field seq_table_lc_ch_cfg_s_g0_fields[] = {
    {offsetof(lc_ch_cfg_s, ch_access_prio_r16),
     (int32_t)offsetof(lc_ch_cfg_s, ch_access_prio_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{3},
     nullptr},
    {offsetof(lc_ch_cfg_s, bit_rate_multiplier_r16),
     (int32_t)offsetof(lc_ch_cfg_s, bit_rate_multiplier_r16_present),
     asn1::table::enum_op<decltype(lc_ch_cfg_s::bit_rate_multiplier_r16)>(),
     decltype(lc_ch_cfg_s::bit_rate_multiplier_r16)::has_ext,
     false,
     decltype(lc_ch_cfg_s::bit_rate_multiplier_r16)::nof_types,
     decltype(lc_ch_cfg_s::bit_rate_multiplier_r16)::nof_exts,
     nullptr,
     &lc_ch_cfg_s::bit_rate_multiplier_r16_opts::desc}};
static const asn1::table::field_name seq_table_lc_ch_cfg_s_g0_names[] = {{"channelAccessPriority-r16", nullptr},
                                                                         {"bitRateMultiplier-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_lc_ch_cfg_s_g0_opts[]  = {
    {0, (int32_t)offsetof(lc_ch_cfg_s, ch_access_prio_r16_present)},
    {1, (int32_t)offsetof(lc_ch_cfg_s, bit_rate_multiplier_r16_present)}};
static const asn1::table::seq_group seq_table_lc_ch_cfg_s_groups[] = {{{seq_table_lc_ch_cfg_s_g0_fields,
                                                                        uint64_t{0x0},
                                                                        seq_table_lc_ch_cfg_s_g0_opts,
                                                                        seq_table_lc_ch_cfg_s_g0_names,
                                                                        2,
                                                                        2},
                                                                       false}};
const asn1::table::seq_desc         lc_ch_cfg_s::desc              = {
    {seq_table_lc_ch_cfg_s_fields, uint64_t{0x0}, seq_table_lc_ch_cfg_s_opts, seq_table_lc_ch_cfg_s_names, 1, 1},
    true,
    false,
    seq_table_lc_ch_cfg_s_groups,
    1};

static const asn1::table::seq_field seq_table_lc_ch_cfg_s_ul_specific_params_s__fields[] = {
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, prio),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{15},
     nullptr},
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, prioritised_bit_rate),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate)>(),
     decltype(lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate)::has_ext,
     false,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate)::nof_types,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate)::nof_exts,
     nullptr,
     &lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::desc},
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, bucket_size_dur),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur)>(),
     decltype(lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur)::has_ext,
     false,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur)::nof_types,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur)::nof_exts,
     nullptr,
     &lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::desc},
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, allowed_serving_cells),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     30,
     &asn1::table::list_codec<decltype(lc_ch_cfg_s::ul_specific_params_s_::allowed_serving_cells),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, allowed_scs_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<decltype(lc_ch_cfg_s::ul_specific_params_s_::allowed_scs_list),
                              asn1::table::string_json>::value},
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, max_pusch_dur),
     (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, max_pusch_dur_present),
     asn1::table::enum_op<decltype(lc_ch_cfg_s::ul_specific_params_s_::max_pusch_dur)>(),
     decltype(lc_ch_cfg_s::ul_specific_params_s_::max_pusch_dur)::has_ext,
     false,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::max_pusch_dur)::nof_types,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::max_pusch_dur)::nof_exts,
     nullptr,
     &lc_ch_cfg_s::ul_specific_params_s_::max_pusch_dur_opts::desc},
    {0,
     (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, cfg_grant_type1_allowed_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, lc_ch_group),
     (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, lc_ch_group_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, sched_request_id),
     (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, sched_request_id_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, lc_ch_sr_mask),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, lc_ch_sr_delay_timer_applied),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_lc_ch_cfg_s_ul_specific_params_s__names[] = {
    {"priority", nullptr},
    {"prioritisedBitRate", nullptr},
    {"bucketSizeDuration", nullptr},
    {"allowedServingCells", nullptr},
    {"allowedSCS-List", nullptr},
    {"maxPUSCH-Duration", nullptr},
    {"configuredGrantType1Allowed", "true"},
    {"logicalChannelGroup", nullptr},
    {"schedulingRequestID", nullptr},
    {"logicalChannelSR-Mask", nullptr},
    {"logicalChannelSR-DelayTimerApplied", nullptr}};
static const asn1::table::seq_opt seq_table_lc_ch_cfg_s_ul_specific_params_s__opts[] = {
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, max_pusch_dur_present)},
    {6, (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, cfg_grant_type1_allowed_present)},
    {7, (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, lc_ch_group_present)},
    {8, (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, sched_request_id_present)}};
static const asn1::table::seq_field seq_table_lc_ch_cfg_s_ul_specific_params_s__g0_fields[] = {
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, bit_rate_query_prohibit_timer),
     (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, bit_rate_query_prohibit_timer_present),
     asn1::table::enum_op<decltype(lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer)>(),
     decltype(lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer)::has_ext,
     false,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer)::nof_types,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer)::nof_exts,
     nullptr,
     &lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_opts::desc}};
static const asn1::table::field_name seq_table_lc_ch_cfg_s_ul_specific_params_s__g0_names[] = {
    {"bitRateQueryProhibitTimer", nullptr}};
static const asn1::table::seq_opt seq_table_lc_ch_cfg_s_ul_specific_params_s__g0_opts[] = {
    {0, (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, bit_rate_query_prohibit_timer_present)}};
static const asn1::table::seq_field seq_table_lc_ch_cfg_s_ul_specific_params_s__g1_fields[] = {
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, allowed_cg_list_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     31,
     &asn1::table::ptr_list_codec<decltype(lc_ch_cfg_s::ul_specific_params_s_::allowed_cg_list_r16),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, allowed_phy_prio_idx_r16),
     (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, allowed_phy_prio_idx_r16_present),
     asn1::table::enum_op<decltype(lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16)>(),
     decltype(lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16)::has_ext,
     false,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16)::nof_types,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16)::nof_exts,
     nullptr,
     &lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16_opts::desc}};
static const asn1::table::field_name seq_table_lc_ch_cfg_s_ul_specific_params_s__g1_names[] = {
    {"allowedCG-List-r16", nullptr},
    {"allowedPHY-PriorityIndex-r16", nullptr}};
static const asn1::table::seq_opt seq_table_lc_ch_cfg_s_ul_specific_params_s__g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, allowed_phy_prio_idx_r16_present)}};
static const asn1::table::seq_field seq_table_lc_ch_cfg_s_ul_specific_params_s__g2_fields[] = {
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, lc_ch_group_iab_ext_r17),
     (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, lc_ch_group_iab_ext_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(lc_ch_cfg_s::ul_specific_params_s_, allowed_harq_mode_r17),
     (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, allowed_harq_mode_r17_present),
     asn1::table::enum_op<decltype(lc_ch_cfg_s::ul_specific_params_s_::allowed_harq_mode_r17)>(),
     decltype(lc_ch_cfg_s::ul_specific_params_s_::allowed_harq_mode_r17)::has_ext,
     false,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::allowed_harq_mode_r17)::nof_types,
     decltype(lc_ch_cfg_s::ul_specific_params_s_::allowed_harq_mode_r17)::nof_exts,
     nullptr,
     &lc_ch_cfg_s::ul_specific_params_s_::allowed_harq_mode_r17_opts::desc}};
static const asn1::table::field_name seq_table_lc_ch_cfg_s_ul_specific_params_s__g2_names[] = {
    {"logicalChannelGroupIAB-Ext-r17", nullptr},
    {"allowedHARQ-mode-r17", nullptr}};
static const asn1::table::seq_opt seq_table_lc_ch_cfg_s_ul_specific_params_s__g2_opts[] = {
    {0, (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, lc_ch_group_iab_ext_r17_present)},
    {1, (int32_t)offsetof(lc_ch_cfg_s::ul_specific_params_s_, allowed_harq_mode_r17_present)}};
static const asn1::table::seq_group seq_table_lc_ch_cfg_s_ul_specific_params_s__groups[] = {
    {{seq_table_lc_ch_cfg_s_ul_specific_params_s__g0_fields,
      uint64_t{0x0},
      seq_table_lc_ch_cfg_s_ul_specific_params_s__g0_opts,
      seq_table_lc_ch_cfg_s_ul_specific_params_s__g0_names,
      1,
      1},
     true},
    {{seq_table_lc_ch_cfg_s_ul_specific_params_s__g1_fields,
      uint64_t{0x0},
      seq_table_lc_ch_cfg_s_ul_specific_params_s__g1_opts,
      seq_table_lc_ch_cfg_s_ul_specific_params_s__g1_names,
      2,
      2},
     false},
    {{seq_table_lc_ch_cfg_s_ul_specific_params_s__g2_fields,
      uint64_t{0x0},
      seq_table_lc_ch_cfg_s_ul_specific_params_s__g2_opts,
      seq_table_lc_ch_cfg_s_ul_specific_params_s__g2_names,
      2,
      2},
     false}};
const asn1::table::seq_desc lc_ch_cfg_s::ul_specific_params_s_::desc = {
    {seq_table_lc_ch_cfg_s_ul_specific_params_s__fields,
     uint64_t{0x607},
     seq_table_lc_ch_cfg_s_ul_specific_params_s__opts,
     seq_table_lc_ch_cfg_s_ul_specific_params_s__names,
     11,
     6},
    true,
    false,
    seq_table_lc_ch_cfg_s_ul_specific_params_s__groups,
    3};

static const char*           enum_names_lc_ch_cfg_s_ul_specific_params_s__prioritised_bit_rate_e__values[] = {"kBps0",
                                                                                                              "kBps8",
                                                                                                              "kBps16",
                                                                                                              "kBps32",
                                                                                                              "kBps64",
                                                                                                              "kBps128",
                                                                                                              "kBps256",
                                                                                                              "kBps512",
                                                                                                              "kBps1024",
                                                                                                              "kBps2048",
                                                                                                              "kBps4096",
                                                                                                              "kBps8192",
                                                                                                              "kBps16384",
                                                                                                              "kBps32768",
                                                                                                              "kBps65536",
                                                                                                              "infinity"};
const asn1::table::enum_desc lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::desc           = {
    enum_names_lc_ch_cfg_s_ul_specific_params_s__prioritised_bit_rate_e__values,
    16,
    "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_"};
int32_t lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_opts::to_number() const
{
  static const int32_t numbers[] = {0, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, -1};
  return map_enum_number(numbers, 16, value, "lc_ch_cfg_s::ul_specific_params_s_::prioritised_bit_rate_e_");
}

static const char*           enum_names_lc_ch_cfg_s_ul_specific_params_s__bucket_size_dur_e__values[] = {"ms5",
                                                                                                         "ms10",
                                                                                                         "ms20",
                                                                                                         "ms50",
                                                                                                         "ms100",
                                                                                                         "ms150",
                                                                                                         "ms300",
                                                                                                         "ms500",
                                                                                                         "ms1000",
                                                                                                         "spare7",
                                                                                                         "spare6",
                                                                                                         "spare5",
                                                                                                         "spare4",
                                                                                                         "spare3",
                                                                                                         "spare2",
                                                                                                         "spare1"};
const asn1::table::enum_desc lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::desc           = {
    enum_names_lc_ch_cfg_s_ul_specific_params_s__bucket_size_dur_e__values,
    16,
    "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_"};
uint16_t lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 50, 100, 150, 300, 500, 1000};
  return map_enum_number(numbers, 9, value, "lc_ch_cfg_s::ul_specific_params_s_::bucket_size_dur_e_");
}

static const char* enum_names_lc_ch_cfg_s_ul_specific_params_s__max_pusch_dur_e__values[] =
    {"ms0p02", "ms0p04", "ms0p0625", "ms0p125", "ms0p25", "ms0p5", "ms0p01-v1700", "spare1"};
const asn1::table::enum_desc lc_ch_cfg_s::ul_specific_params_s_::max_pusch_dur_opts::desc = {
    enum_names_lc_ch_cfg_s_ul_specific_params_s__max_pusch_dur_e__values,
    8,
    "lc_ch_cfg_s::ul_specific_params_s_::max_pusch_dur_e_"};

static const char* enum_names_lc_ch_cfg_s_ul_specific_params_s__bit_rate_query_prohibit_timer_e__values[] =
    {"s0", "s0dot4", "s0dot8", "s1dot6", "s3", "s6", "s12", "s30"};
const asn1::table::enum_desc lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_opts::desc = {
    enum_names_lc_ch_cfg_s_ul_specific_params_s__bit_rate_query_prohibit_timer_e__values,
    8,
    "lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_e_"};
float lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.4, 0.8, 1.6, 3.0, 6.0, 12.0, 30.0};
  return map_enum_number(numbers, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_e_");
}
const char* lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_opts::to_number_string() const
{
  static const char* number_strs[] = {"0", "0.4", "0.8", "1.6", "3", "6", "12", "30"};
  return convert_enum_idx(
      number_strs, 8, value, "lc_ch_cfg_s::ul_specific_params_s_::bit_rate_query_prohibit_timer_e_");
}

static const char* enum_names_lc_ch_cfg_s_ul_specific_params_s__allowed_phy_prio_idx_r16_e__values[] = {"p0", "p1"};
const asn1::table::enum_desc lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16_opts::desc = {
    enum_names_lc_ch_cfg_s_ul_specific_params_s__allowed_phy_prio_idx_r16_e__values,
    2,
    "lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16_e_"};
uint8_t lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "lc_ch_cfg_s::ul_specific_params_s_::allowed_phy_prio_idx_r16_e_");
}

static const char* enum_names_lc_ch_cfg_s_ul_specific_params_s__allowed_harq_mode_r17_e__values[] = {"harqModeA",
                                                                                                     "harqModeB"};
const asn1::table::enum_desc lc_ch_cfg_s::ul_specific_params_s_::allowed_harq_mode_r17_opts::desc = {
    enum_names_lc_ch_cfg_s_ul_specific_params_s__allowed_harq_mode_r17_e__values,
    2,
    "lc_ch_cfg_s::ul_specific_params_s_::allowed_harq_mode_r17_e_"};

static const char* enum_names_lc_ch_cfg_s_bit_rate_multiplier_r16_e__values[] = {"x40", "x70", "x100", "x200"};
const asn1::table::enum_desc lc_ch_cfg_s::bit_rate_multiplier_r16_opts::desc  = {
    enum_names_lc_ch_cfg_s_bit_rate_multiplier_r16_e__values,
    4,
    "lc_ch_cfg_s::bit_rate_multiplier_r16_e_"};
uint8_t lc_ch_cfg_s::bit_rate_multiplier_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {40, 70, 100, 200};
  return map_enum_number(numbers, 4, value, "lc_ch_cfg_s::bit_rate_multiplier_r16_e_");
}

// BH-RLC-ChannelConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_bh_rlc_ch_cfg_r16_s_fields[] = {
    {offsetof(bh_rlc_ch_cfg_r16_s, bh_lc_ch_id_r16),
     (int32_t)offsetof(bh_rlc_ch_cfg_r16_s, bh_lc_ch_id_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &bh_lc_ch_id_r16_c::desc},
    {offsetof(bh_rlc_ch_cfg_r16_s, bh_rlc_ch_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(bh_rlc_ch_cfg_r16_s::bh_rlc_ch_id_r16), asn1::table::struct_json>::value},
    {0,
     (int32_t)offsetof(bh_rlc_ch_cfg_r16_s, reestablish_rlc_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(bh_rlc_ch_cfg_r16_s, rlc_cfg_r16),
     (int32_t)offsetof(bh_rlc_ch_cfg_r16_s, rlc_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rlc_cfg_c::desc},
    {offsetof(bh_rlc_ch_cfg_r16_s, mac_lc_ch_cfg_r16),
     (int32_t)offsetof(bh_rlc_ch_cfg_r16_s, mac_lc_ch_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &lc_ch_cfg_s::desc}};
static const asn1::table::field_name seq_table_bh_rlc_ch_cfg_r16_s_names[] = {
    {"bh-LogicalChannelIdentity-r16", nullptr},
    {"bh-RLC-ChannelID-r16", nullptr},
    {"reestablishRLC-r16", "true"},
    {"rlc-Config-r16", nullptr},
    {"mac-LogicalChannelConfig-r16", nullptr}};
static const asn1::table::seq_opt seq_table_bh_rlc_ch_cfg_r16_s_opts[] = {
    {0, (int32_t)offsetof(bh_rlc_ch_cfg_r16_s, bh_lc_ch_id_r16_present)},
    {2, (int32_t)offsetof(bh_rlc_ch_cfg_r16_s, reestablish_rlc_r16_present)},
    {3, (int32_t)offsetof(bh_rlc_ch_cfg_r16_s, rlc_cfg_r16_present)},
    {4, (int32_t)offsetof(bh_rlc_ch_cfg_r16_s, mac_lc_ch_cfg_r16_present)}};
const asn1::table::seq_desc bh_rlc_ch_cfg_r16_s::desc = {{seq_table_bh_rlc_ch_cfg_r16_s_fields,
                                                          uint64_t{0x2},
                                                          seq_table_bh_rlc_ch_cfg_r16_s_opts,
                                                          seq_table_bh_rlc_ch_cfg_r16_s_names,
                                                          5,
                                                          4},
                                                         true,
                                                         false,
                                                         nullptr,
                                                         0};

// CarrierState-r17 ::= CHOICE
void carrier_state_r17_c::set(types::options e)
{
  type_ = e;
}
void carrier_state_r17_c::set_de_activ_r17()
{
  set(types::de_activ_r17);
}
uint8_t& carrier_state_r17_c::set_active_bwp_r17()
{
  set(types::active_bwp_r17);
  return c;
}
static const asn1::table::seq_field choice_table_carrier_state_r17_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{4},
     nullptr}};
static const asn1::table::field_name choice_table_carrier_state_r17_c_names[] = {{"deActivated-r17", nullptr},
                                                                                 {"activeBWP-r17", nullptr}};
const asn1::table::choice_desc carrier_state_r17_c::desc = {asn1::table::enum_int_type<carrier_state_r17_c::types>(),
                                                            carrier_state_r17_c::types::nof_types,
                                                            carrier_state_r17_c::types::nof_exts,
                                                            carrier_state_r17_c::types::has_ext,
                                                            false,
                                                            choice_table_carrier_state_r17_c_alternatives,
                                                            &asn1::table::choice_set<carrier_state_r17_c>,
                                                            choice_table_carrier_state_r17_c_names,
                                                            "carrier_state_r17_c",
                                                            offsetof(carrier_state_r17_c, type_),
                                                            offsetof(carrier_state_r17_c, c),
                                                            false};

static const char*           enum_names_carrier_state_r17_c_types_values[] = {"deActivated-r17", "activeBWP-r17"};
const asn1::table::enum_desc carrier_state_r17_c::types_opts::desc = {enum_names_carrier_state_r17_c_types_values,
                                                                      2,
                                                                      "carrier_state_r17_c::types"};

// CC-State-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cc_state_r17_s_fields[] = {
    {offsetof(cc_state_r17_s, dl_carrier_r17),
     (int32_t)offsetof(cc_state_r17_s, dl_carrier_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &carrier_state_r17_c::desc},
    {offsetof(cc_state_r17_s, ul_carrier_r17),
     (int32_t)offsetof(cc_state_r17_s, ul_carrier_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &carrier_state_r17_c::desc}};
static const asn1::table::field_name seq_table_cc_state_r17_s_names[] = {{"dlCarrier-r17", nullptr},
                                                                         {"ulCarrier-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_cc_state_r17_s_opts[]  = {
    {0, (int32_t)offsetof(cc_state_r17_s, dl_carrier_r17_present)},
    {1, (int32_t)offsetof(cc_state_r17_s, ul_carrier_r17_present)}};
const asn1::table::seq_desc cc_state_r17_s::desc = {{seq_table_cc_state_r17_s_fields,
                                                     uint64_t{0x0},
                                                     seq_table_cc_state_r17_s_opts,
                                                     seq_table_cc_state_r17_s_names,
                                                     2,
                                                     2},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0};

// CFRA-SSB-Resource ::= SEQUENCE
static const asn1::table::seq_field  seq_table_cfra_ssb_res_s_fields[]    = {{offsetof(cfra_ssb_res_s, ssb),
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::field_op::int_u8,
                                                                              false,
                                                                              false,
                                                                              int64_t{0},
                                                                              uint64_t{63},
                                                                              nullptr},
                                                                             {offsetof(cfra_ssb_res_s, ra_preamb_idx),
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::field_op::int_u8,
                                                                              false,
                                                                              false,
                                                                              int64_t{0},
                                                                              uint64_t{63},
                                                                              nullptr}};
static const asn1::table::field_name seq_table_cfra_ssb_res_s_names[]     = {{"ssb", nullptr},
                                                                             {"ra-PreambleIndex", nullptr}};
static const asn1::table::seq_field  seq_table_cfra_ssb_res_s_g0_fields[] = {
    {offsetof(cfra_ssb_res_s, msg_a_pusch_res_idx_r16),
      (int32_t)offsetof(cfra_ssb_res_s, msg_a_pusch_res_idx_r16_present),
      asn1::table::field_op::int_u16,
      false,
      false,
      int64_t{0},
      uint64_t{3071},
      nullptr}};
static const asn1::table::field_name seq_table_cfra_ssb_res_s_g0_names[] = {{"msgA-PUSCH-Resource-Index-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_cfra_ssb_res_s_g0_opts[]  = {
    {0, (int32_t)offsetof(cfra_ssb_res_s, msg_a_pusch_res_idx_r16_present)}};
static const asn1::table::seq_group seq_table_cfra_ssb_res_s_groups[] = {{{seq_table_cfra_ssb_res_s_g0_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_cfra_ssb_res_s_g0_opts,
                                                                           seq_table_cfra_ssb_res_s_g0_names,
                                                                           1,
                                                                           1},
                                                                          false}};
const asn1::table::seq_desc         cfra_ssb_res_s::desc              = {
    {seq_table_cfra_ssb_res_s_fields, uint64_t{0x3}, nullptr, seq_table_cfra_ssb_res_s_names, 2, 0},
    true,
    false,
    seq_table_cfra_ssb_res_s_groups,
    1};

// CFRA-CSIRS-Resource ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfra_csi_rs_res_s_fields[] = {
    {offsetof(cfra_csi_rs_res_s, csi_rs),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{95},
     nullptr},
    {offsetof(cfra_csi_rs_res_s, ra_occasion_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::list_codec<decltype(cfra_csi_rs_res_s::ra_occasion_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{511}, false, false>>::value},
    {offsetof(cfra_csi_rs_res_s, ra_preamb_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{63},
     nullptr}};
static const asn1::table::field_name seq_table_cfra_csi_rs_res_s_names[] = {{"csi-RS", nullptr},
                                                                            {"ra-OccasionList", nullptr},
                                                                            {"ra-PreambleIndex", nullptr}};
const asn1::table::seq_desc          cfra_csi_rs_res_s::desc             = {
    {seq_table_cfra_csi_rs_res_s_fields, uint64_t{0x7}, nullptr, seq_table_cfra_csi_rs_res_s_names, 3, 0},
    true,
    false,
    nullptr,
    0};

// CFRA ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfra_s_occasions_s__fields[] = {
    {offsetof(cfra_s::occasions_s_, rach_cfg_generic),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rach_cfg_generic_s::desc},
    {offsetof(cfra_s::occasions_s_, ssb_per_rach_occasion),
     (int32_t)offsetof(cfra_s::occasions_s_, ssb_per_rach_occasion_present),
     asn1::table::enum_op<decltype(cfra_s::occasions_s_::ssb_per_rach_occasion)>(),
     decltype(cfra_s::occasions_s_::ssb_per_rach_occasion)::has_ext,
     false,
     decltype(cfra_s::occasions_s_::ssb_per_rach_occasion)::nof_types,
     decltype(cfra_s::occasions_s_::ssb_per_rach_occasion)::nof_exts,
     nullptr,
     &cfra_s::occasions_s_::ssb_per_rach_occasion_opts::desc}};
static const asn1::table::field_name seq_table_cfra_s_occasions_s__names[] = {{"rach-ConfigGeneric", nullptr},
                                                                              {"ssb-perRACH-Occasion", nullptr}};
static const asn1::table::seq_opt    seq_table_cfra_s_occasions_s__opts[]  = {
    {1, (int32_t)offsetof(cfra_s::occasions_s_, ssb_per_rach_occasion_present)}};
static const asn1::table::seq_desc   seq_table_cfra_s_occasions_s_ = {{seq_table_cfra_s_occasions_s__fields,
                                                                       uint64_t{0x1},
                                                                       seq_table_cfra_s_occasions_s__opts,
                                                                       seq_table_cfra_s_occasions_s__names,
                                                                       2,
                                                                       1},
                                                                      false,
                                                                      false,
                                                                      nullptr,
                                                                      0};
static const asn1::table::seq_field  seq_table_cfra_s_fields[]     = {{offsetof(cfra_s, occasions),
                                                                       (int32_t)offsetof(cfra_s, occasions_present),
                                                                       asn1::table::field_op::sequence,
                                                                       false,
                                                                       false,
                                                                       0,
                                                                       0,
                                                                       nullptr,
                                                                       &seq_table_cfra_s_occasions_s_},
                                                                      {offsetof(cfra_s, res),
                                                                       asn1::table::seq_field::mandatory,
                                                                       asn1::table::field_op::choice,
                                                                       false,
                                                                       false,
                                                                       0,
                                                                       0,
                                                                       nullptr,
                                                                       &cfra_s::res_c_::desc}};
static const asn1::table::field_name seq_table_cfra_s_names[]      = {{"occasions", nullptr}, {"resources", nullptr}};
static const asn1::table::seq_opt    seq_table_cfra_s_opts[]      = {{0, (int32_t)offsetof(cfra_s, occasions_present)}};
static const asn1::table::seq_field  seq_table_cfra_s_g0_fields[] = {
    {offsetof(cfra_s, total_nof_ra_preambs),
      (int32_t)offsetof(cfra_s, total_nof_ra_preambs_present),
      asn1::table::field_op::int_u8,
      false,
      false,
      int64_t{1},
      uint64_t{62},
      nullptr}};
static const asn1::table::field_name seq_table_cfra_s_g0_names[] = {{"totalNumberOfRA-Preambles", nullptr}};
static const asn1::table::seq_opt    seq_table_cfra_s_g0_opts[]  = {
    {0, (int32_t)offsetof(cfra_s, total_nof_ra_preambs_present)}};
static const asn1::table::seq_field seq_table_cfra_s_g1_fields[] = {
    {offsetof(cfra_s, msg1_repeat_num_r18),
     (int32_t)offsetof(cfra_s, msg1_repeat_num_r18_present),
     asn1::table::enum_op<decltype(cfra_s::msg1_repeat_num_r18)>(),
     decltype(cfra_s::msg1_repeat_num_r18)::has_ext,
     false,
     decltype(cfra_s::msg1_repeat_num_r18)::nof_types,
     decltype(cfra_s::msg1_repeat_num_r18)::nof_exts,
     nullptr,
     &cfra_s::msg1_repeat_num_r18_opts::desc}};
static const asn1::table::field_name seq_table_cfra_s_g1_names[] = {{"msg1-RepetitionNum-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_cfra_s_g1_opts[]  = {
    {0, (int32_t)offsetof(cfra_s, msg1_repeat_num_r18_present)}};
static const asn1::table::seq_group seq_table_cfra_s_groups[] = {
    {{seq_table_cfra_s_g0_fields, uint64_t{0x0}, seq_table_cfra_s_g0_opts, seq_table_cfra_s_g0_names, 1, 1}, false},
    {{seq_table_cfra_s_g1_fields, uint64_t{0x0}, seq_table_cfra_s_g1_opts, seq_table_cfra_s_g1_names, 1, 1}, false}};
const asn1::table::seq_desc cfra_s::desc = {
    {seq_table_cfra_s_fields, uint64_t{0x2}, seq_table_cfra_s_opts, seq_table_cfra_s_names, 2, 1},
    true,
    false,
    seq_table_cfra_s_groups,
    2};

static const char* enum_names_cfra_s_occasions_s__ssb_per_rach_occasion_e__values[] =
    {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
const asn1::table::enum_desc cfra_s::occasions_s_::ssb_per_rach_occasion_opts::desc = {
    enum_names_cfra_s_occasions_s__ssb_per_rach_occasion_e__values,
    8,
    "cfra_s::occasions_s_::ssb_per_rach_occasion_e_"};
float cfra_s::occasions_s_::ssb_per_rach_occasion_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(numbers, 8, value, "cfra_s::occasions_s_::ssb_per_rach_occasion_e_");
}
const char* cfra_s::occasions_s_::ssb_per_rach_occasion_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(number_strs, 8, value, "cfra_s::occasions_s_::ssb_per_rach_occasion_e_");
}

static const asn1::table::storage_ops* const choice_storage_cfra_s_res_c__alternatives[] = {
    &asn1::table::alternative_ops<cfra_s::res_c_::ssb_s_, false>::value,
    &asn1::table::alternative_ops<cfra_s::res_c_::csirs_s_, false>::value};
const asn1::table::choice_storage_desc cfra_s::res_c_::storage_desc = {choice_storage_cfra_s_res_c__alternatives,
                                                                       2,
                                                                       "cfra_s::res_c_"};

cfra_s::res_c_::ssb_s_& cfra_s::res_c_::set_ssb()
{
  set(types::ssb);
  return c.get<ssb_s_>();
}
cfra_s::res_c_::csirs_s_& cfra_s::res_c_::set_csirs()
{
  set(types::csirs);
  return c.get<csirs_s_>();
}
static const asn1::table::seq_field seq_table_cfra_s_res_c__ssb_s__fields[] = {
    {offsetof(cfra_s::res_c_::ssb_s_, ssb_res_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::item_list_codec<decltype(cfra_s::res_c_::ssb_s_::ssb_res_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cfra_ssb_res_s::desc},
    {offsetof(cfra_s::res_c_::ssb_s_, ra_ssb_occasion_mask_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr}};
static const asn1::table::field_name seq_table_cfra_s_res_c__ssb_s__names[] = {{"ssb-ResourceList", nullptr},
                                                                               {"ra-ssb-OccasionMaskIndex", nullptr}};
static const asn1::table::seq_desc   seq_table_cfra_s_res_c__ssb_s_         = {
    {seq_table_cfra_s_res_c__ssb_s__fields, uint64_t{0x3}, nullptr, seq_table_cfra_s_res_c__ssb_s__names, 2, 0},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_cfra_s_res_c__csirs_s__fields[] = {
    {offsetof(cfra_s::res_c_::csirs_s_, csirs_res_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     95,
     &asn1::table::item_list_codec<decltype(cfra_s::res_c_::csirs_s_::csirs_res_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cfra_csi_rs_res_s::desc},
    {offsetof(cfra_s::res_c_::csirs_s_, rsrp_thres_csi_rs),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr}};
static const asn1::table::field_name seq_table_cfra_s_res_c__csirs_s__names[] = {{"csirs-ResourceList", nullptr},
                                                                                 {"rsrp-ThresholdCSI-RS", nullptr}};
static const asn1::table::seq_desc   seq_table_cfra_s_res_c__csirs_s_         = {
    {seq_table_cfra_s_res_c__csirs_s__fields, uint64_t{0x3}, nullptr, seq_table_cfra_s_res_c__csirs_s__names, 2, 0},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field  choice_table_cfra_s_res_c__alternatives[] = {{0,
                                                                                   asn1::table::seq_field::mandatory,
                                                                                   asn1::table::field_op::sequence,
                                                                                   false,
                                                                                   false,
                                                                                   0,
                                                                                   0,
                                                                                   nullptr,
                                                                                   &seq_table_cfra_s_res_c__ssb_s_},
                                                                                  {0,
                                                                                   asn1::table::seq_field::mandatory,
                                                                                   asn1::table::field_op::sequence,
                                                                                   false,
                                                                                   false,
                                                                                   0,
                                                                                   0,
                                                                                   nullptr,
                                                                                   &seq_table_cfra_s_res_c__csirs_s_}};
static const asn1::table::field_name choice_table_cfra_s_res_c__names[]        = {{"ssb", nullptr}, {"csirs", nullptr}};
const asn1::table::choice_desc       cfra_s::res_c_::desc = {asn1::table::enum_int_type<cfra_s::res_c_::types>(),
                                                             cfra_s::res_c_::types::nof_types,
                                                             cfra_s::res_c_::types::nof_exts,
                                                             cfra_s::res_c_::types::has_ext,
                                                             false,
                                                             choice_table_cfra_s_res_c__alternatives,
                                                             nullptr,
                                                             choice_table_cfra_s_res_c__names,
                                                             "cfra_s::res_c_",
                                                             offsetof(cfra_s::res_c_, c.type),
                                                             offsetof(cfra_s::res_c_, c.value),
                                                             false,
                                                             &cfra_s::res_c_::storage_desc};

static const char*           enum_names_cfra_s_res_c__types_values[] = {"ssb", "csirs"};
const asn1::table::enum_desc cfra_s::res_c_::types_opts::desc        = {enum_names_cfra_s_res_c__types_values,
                                                                        2,
                                                                        "cfra_s::res_c_::types"};

static const char*           enum_names_cfra_s_msg1_repeat_num_r18_e__values[] = {"n2", "n4", "n8", "spare1"};
const asn1::table::enum_desc cfra_s::msg1_repeat_num_r18_opts::desc = {enum_names_cfra_s_msg1_repeat_num_r18_e__values,
                                                                       4,
                                                                       "cfra_s::msg1_repeat_num_r18_e_"};
uint8_t                      cfra_s::msg1_repeat_num_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "cfra_s::msg1_repeat_num_r18_e_");
}

// CFRA-TwoStep-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfra_two_step_r16_s_occasions_two_step_ra_r16_s__fields[] = {
    {offsetof(cfra_two_step_r16_s::occasions_two_step_ra_r16_s_, rach_cfg_generic_two_step_ra_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rach_cfg_generic_two_step_ra_r16_s::desc},
    {offsetof(cfra_two_step_r16_s::occasions_two_step_ra_r16_s_, ssb_per_rach_occasion_two_step_ra_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<
         decltype(cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16)>(),
     decltype(cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16)::has_ext,
     false,
     decltype(cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16)::nof_types,
     decltype(cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16)::nof_exts,
     nullptr,
     &cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_opts::desc}};
static const asn1::table::field_name seq_table_cfra_two_step_r16_s_occasions_two_step_ra_r16_s__names[] = {
    {"rach-ConfigGenericTwoStepRA-r16", nullptr},
    {"ssb-PerRACH-OccasionTwoStepRA-r16", nullptr}};
static const asn1::table::seq_desc seq_table_cfra_two_step_r16_s_occasions_two_step_ra_r16_s_ = {
    {seq_table_cfra_two_step_r16_s_occasions_two_step_ra_r16_s__fields,
     uint64_t{0x3},
     nullptr,
     seq_table_cfra_two_step_r16_s_occasions_two_step_ra_r16_s__names,
     2,
     0},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_cfra_two_step_r16_s_res_two_step_r16_s__fields[] = {
    {offsetof(cfra_two_step_r16_s::res_two_step_r16_s_, ssb_res_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::item_list_codec<decltype(cfra_two_step_r16_s::res_two_step_r16_s_::ssb_res_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cfra_ssb_res_s::desc},
    {offsetof(cfra_two_step_r16_s::res_two_step_r16_s_, ra_ssb_occasion_mask_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr}};
static const asn1::table::field_name seq_table_cfra_two_step_r16_s_res_two_step_r16_s__names[] = {
    {"ssb-ResourceList", nullptr},
    {"ra-ssb-OccasionMaskIndex", nullptr}};
static const asn1::table::seq_desc seq_table_cfra_two_step_r16_s_res_two_step_r16_s_ = {
    {seq_table_cfra_two_step_r16_s_res_two_step_r16_s__fields,
     uint64_t{0x3},
     nullptr,
     seq_table_cfra_two_step_r16_s_res_two_step_r16_s__names,
     2,
     0},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_cfra_two_step_r16_s_fields[] = {
    {offsetof(cfra_two_step_r16_s, occasions_two_step_ra_r16),
     (int32_t)offsetof(cfra_two_step_r16_s, occasions_two_step_ra_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_cfra_two_step_r16_s_occasions_two_step_ra_r16_s_},
    {offsetof(cfra_two_step_r16_s, msg_a_cfra_pusch_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &msg_a_pusch_res_r16_s::desc},
    {offsetof(cfra_two_step_r16_s, msg_a_trans_max_r16),
     (int32_t)offsetof(cfra_two_step_r16_s, msg_a_trans_max_r16_present),
     asn1::table::enum_op<decltype(cfra_two_step_r16_s::msg_a_trans_max_r16)>(),
     decltype(cfra_two_step_r16_s::msg_a_trans_max_r16)::has_ext,
     false,
     decltype(cfra_two_step_r16_s::msg_a_trans_max_r16)::nof_types,
     decltype(cfra_two_step_r16_s::msg_a_trans_max_r16)::nof_exts,
     nullptr,
     &cfra_two_step_r16_s::msg_a_trans_max_r16_opts::desc},
    {offsetof(cfra_two_step_r16_s, res_two_step_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_cfra_two_step_r16_s_res_two_step_r16_s_}};
static const asn1::table::field_name seq_table_cfra_two_step_r16_s_names[] = {{"occasionsTwoStepRA-r16", nullptr},
                                                                              {"msgA-CFRA-PUSCH-r16", nullptr},
                                                                              {"msgA-TransMax-r16", nullptr},
                                                                              {"resourcesTwoStep-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_cfra_two_step_r16_s_opts[]  = {
    {0, (int32_t)offsetof(cfra_two_step_r16_s, occasions_two_step_ra_r16_present)},
    {2, (int32_t)offsetof(cfra_two_step_r16_s, msg_a_trans_max_r16_present)}};
const asn1::table::seq_desc cfra_two_step_r16_s::desc = {{seq_table_cfra_two_step_r16_s_fields,
                                                          uint64_t{0xa},
                                                          seq_table_cfra_two_step_r16_s_opts,
                                                          seq_table_cfra_two_step_r16_s_names,
                                                          4,
                                                          2},
                                                         true,
                                                         false,
                                                         nullptr,
                                                         0};

static const char*
    enum_names_cfra_two_step_r16_s_occasions_two_step_ra_r16_s__ssb_per_rach_occasion_two_step_ra_r16_e__values[] =
        {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
const asn1::table::enum_desc
    cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_opts::desc = {
        enum_names_cfra_two_step_r16_s_occasions_two_step_ra_r16_s__ssb_per_rach_occasion_two_step_ra_r16_e__values,
        8,
        "cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_e_"};
float cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(
      numbers, 8, value, "cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_e_");
}
const char*
cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(
      number_strs,
      8,
      value,
      "cfra_two_step_r16_s::occasions_two_step_ra_r16_s_::ssb_per_rach_occasion_two_step_ra_r16_e_");
}

static const char* enum_names_cfra_two_step_r16_s_msg_a_trans_max_r16_e__values[] =
    {"n1", "n2", "n4", "n6", "n8", "n10", "n20", "n50", "n100", "n200"};
const asn1::table::enum_desc cfra_two_step_r16_s::msg_a_trans_max_r16_opts::desc = {
    enum_names_cfra_two_step_r16_s_msg_a_trans_max_r16_e__values,
    10,
    "cfra_two_step_r16_s::msg_a_trans_max_r16_e_"};
uint8_t cfra_two_step_r16_s::msg_a_trans_max_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6, 8, 10, 20, 50, 100, 200};
  return map_enum_number(numbers, 10, value, "cfra_two_step_r16_s::msg_a_trans_max_r16_e_");
}

// CellDTRX-DCI-config-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_dtrx_dci_cfg_r18_s_fields[] = {
    {offsetof(cell_dtrx_dci_cfg_r18_s, cell_dtrx_rnti_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(cell_dtrx_dci_cfg_r18_s, size_dci_2_9_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{139},
     nullptr}};
static const asn1::table::field_name seq_table_cell_dtrx_dci_cfg_r18_s_names[] = {{"cellDTRX-RNTI-r18", nullptr},
                                                                                  {"sizeDCI-2-9-r18", nullptr}};
const asn1::table::seq_desc          cell_dtrx_dci_cfg_r18_s::desc             = {
    {seq_table_cell_dtrx_dci_cfg_r18_s_fields, uint64_t{0x3}, nullptr, seq_table_cell_dtrx_dci_cfg_r18_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};

// T-StatusProhibit-v1610 ::= ENUMERATED
static const char* enum_names_t_status_prohibit_v1610_e_values[] =
    {"ms1", "ms2", "ms3", "ms4", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc t_status_prohibit_v1610_opts::desc = {enum_names_t_status_prohibit_v1610_e_values,
                                                                   8,
                                                                   "t_status_prohibit_v1610_e"};
uint8_t                      t_status_prohibit_v1610_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "t_status_prohibit_v1610_e");
}

// DL-AM-RLC-v1610 ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_am_rlc_v1610_s_fields[] = {
    {offsetof(dl_am_rlc_v1610_s, t_status_prohibit_v1610),
     (int32_t)offsetof(dl_am_rlc_v1610_s, t_status_prohibit_v1610_present),
     asn1::table::enum_op<decltype(dl_am_rlc_v1610_s::t_status_prohibit_v1610)>(),
     decltype(dl_am_rlc_v1610_s::t_status_prohibit_v1610)::has_ext,
     false,
     decltype(dl_am_rlc_v1610_s::t_status_prohibit_v1610)::nof_types,
     decltype(dl_am_rlc_v1610_s::t_status_prohibit_v1610)::nof_exts,
     nullptr,
     &t_status_prohibit_v1610_opts::desc}};
static const asn1::table::field_name seq_table_dl_am_rlc_v1610_s_names[] = {{"t-StatusProhibit-v1610", nullptr}};
static const asn1::table::seq_opt    seq_table_dl_am_rlc_v1610_s_opts[]  = {
    {0, (int32_t)offsetof(dl_am_rlc_v1610_s, t_status_prohibit_v1610_present)}};
const asn1::table::seq_desc dl_am_rlc_v1610_s::desc = {{seq_table_dl_am_rlc_v1610_s_fields,
                                                        uint64_t{0x0},
                                                        seq_table_dl_am_rlc_v1610_s_opts,
                                                        seq_table_dl_am_rlc_v1610_s_names,
                                                        1,
                                                        1},
                                                       true,
                                                       false,
                                                       nullptr,
                                                       0};

// RLC-Config-v1610 ::= SEQUENCE
static const asn1::table::seq_field  seq_table_rlc_cfg_v1610_s_fields[] = {{offsetof(rlc_cfg_v1610_s, dl_am_rlc_v1610),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::sequence,
                                                                            false,
                                                                            false,
                                                                            0,
                                                                            0,
                                                                            nullptr,
                                                                            &dl_am_rlc_v1610_s::desc}};
static const asn1::table::field_name seq_table_rlc_cfg_v1610_s_names[]  = {{"dl-AM-RLC-v1610", nullptr}};
const asn1::table::seq_desc          rlc_cfg_v1610_s::desc              = {
    {seq_table_rlc_cfg_v1610_s_fields, uint64_t{0x1}, nullptr, seq_table_rlc_cfg_v1610_s_names, 1, 0},
    false,
    false,
    nullptr,
    0};

// T-ReassemblyExt-r17 ::= ENUMERATED
static const char* enum_names_t_reassembly_ext_r17_e_values[] =
    {"ms210", "ms220", "ms340", "ms350", "ms550", "ms1100", "ms1650", "ms2200"};
const asn1::table::enum_desc t_reassembly_ext_r17_opts::desc = {enum_names_t_reassembly_ext_r17_e_values,
                                                                8,
                                                                "t_reassembly_ext_r17_e"};
uint16_t                     t_reassembly_ext_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {210, 220, 340, 350, 550, 1100, 1650, 2200};
  return map_enum_number(numbers, 8, value, "t_reassembly_ext_r17_e");
}

// DL-AM-RLC-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_am_rlc_v1700_s_fields[] = {
    {offsetof(dl_am_rlc_v1700_s, t_reassembly_ext_r17),
     (int32_t)offsetof(dl_am_rlc_v1700_s, t_reassembly_ext_r17_present),
     asn1::table::enum_op<decltype(dl_am_rlc_v1700_s::t_reassembly_ext_r17)>(),
     decltype(dl_am_rlc_v1700_s::t_reassembly_ext_r17)::has_ext,
     false,
     decltype(dl_am_rlc_v1700_s::t_reassembly_ext_r17)::nof_types,
     decltype(dl_am_rlc_v1700_s::t_reassembly_ext_r17)::nof_exts,
     nullptr,
     &t_reassembly_ext_r17_opts::desc}};
static const asn1::table::field_name seq_table_dl_am_rlc_v1700_s_names[] = {{"t-ReassemblyExt-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_dl_am_rlc_v1700_s_opts[]  = {
    {0, (int32_t)offsetof(dl_am_rlc_v1700_s, t_reassembly_ext_r17_present)}};
const asn1::table::seq_desc dl_am_rlc_v1700_s::desc = {{seq_table_dl_am_rlc_v1700_s_fields,
                                                        uint64_t{0x0},
                                                        seq_table_dl_am_rlc_v1700_s_opts,
                                                        seq_table_dl_am_rlc_v1700_s_names,
                                                        1,
                                                        1},
                                                       false,
                                                       false,
                                                       nullptr,
                                                       0};

// DL-UM-RLC-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_um_rlc_v1700_s_fields[] = {
    {offsetof(dl_um_rlc_v1700_s, t_reassembly_ext_r17),
     (int32_t)offsetof(dl_um_rlc_v1700_s, t_reassembly_ext_r17_present),
     asn1::table::enum_op<decltype(dl_um_rlc_v1700_s::t_reassembly_ext_r17)>(),
     decltype(dl_um_rlc_v1700_s::t_reassembly_ext_r17)::has_ext,
     false,
     decltype(dl_um_rlc_v1700_s::t_reassembly_ext_r17)::nof_types,
     decltype(dl_um_rlc_v1700_s::t_reassembly_ext_r17)::nof_exts,
     nullptr,
     &t_reassembly_ext_r17_opts::desc}};
static const asn1::table::field_name seq_table_dl_um_rlc_v1700_s_names[] = {{"t-ReassemblyExt-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_dl_um_rlc_v1700_s_opts[]  = {
    {0, (int32_t)offsetof(dl_um_rlc_v1700_s, t_reassembly_ext_r17_present)}};
const asn1::table::seq_desc dl_um_rlc_v1700_s::desc = {{seq_table_dl_um_rlc_v1700_s_fields,
                                                        uint64_t{0x0},
                                                        seq_table_dl_um_rlc_v1700_s_opts,
                                                        seq_table_dl_um_rlc_v1700_s_names,
                                                        1,
                                                        1},
                                                       false,
                                                       false,
                                                       nullptr,
                                                       0};

// RLC-Config-v1700 ::= SEQUENCE
static const asn1::table::seq_field  seq_table_rlc_cfg_v1700_s_fields[] = {{offsetof(rlc_cfg_v1700_s, dl_am_rlc_v1700),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::sequence,
                                                                            false,
                                                                            false,
                                                                            0,
                                                                            0,
                                                                            nullptr,
                                                                            &dl_am_rlc_v1700_s::desc},
                                                                           {offsetof(rlc_cfg_v1700_s, dl_um_rlc_v1700),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::sequence,
                                                                            false,
                                                                            false,
                                                                            0,
                                                                            0,
                                                                            nullptr,
                                                                            &dl_um_rlc_v1700_s::desc}};
static const asn1::table::field_name seq_table_rlc_cfg_v1700_s_names[]  = {{"dl-AM-RLC-v1700", nullptr},
                                                                           {"dl-UM-RLC-v1700", nullptr}};
const asn1::table::seq_desc          rlc_cfg_v1700_s::desc              = {
    {seq_table_rlc_cfg_v1700_s_fields, uint64_t{0x3}, nullptr, seq_table_rlc_cfg_v1700_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};

// MulticastRLC-BearerConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_multicast_rlc_bearer_cfg_r17_s_fields[] = {
    {offsetof(multicast_rlc_bearer_cfg_r17_s, served_mbs_radio_bearer_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{1},
     uint64_t{511},
     nullptr},
    {0,
     (int32_t)offsetof(multicast_rlc_bearer_cfg_r17_s, is_ptm_entity_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_multicast_rlc_bearer_cfg_r17_s_names[] = {
    {"servedMBS-RadioBearer-r17", nullptr},
    {"isPTM-Entity-r17", "true"}};
static const asn1::table::seq_opt seq_table_multicast_rlc_bearer_cfg_r17_s_opts[] = {
    {1, (int32_t)offsetof(multicast_rlc_bearer_cfg_r17_s, is_ptm_entity_r17_present)}};
const asn1::table::seq_desc multicast_rlc_bearer_cfg_r17_s::desc = {{seq_table_multicast_rlc_bearer_cfg_r17_s_fields,
                                                                     uint64_t{0x1},
                                                                     seq_table_multicast_rlc_bearer_cfg_r17_s_opts,
                                                                     seq_table_multicast_rlc_bearer_cfg_r17_s_names,
                                                                     2,
                                                                     1},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0};

// RLC-BearerConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_rlc_bearer_cfg_s_fields[] = {
    {offsetof(rlc_bearer_cfg_s, lc_ch_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr},
    {offsetof(rlc_bearer_cfg_s, served_radio_bearer),
     (int32_t)offsetof(rlc_bearer_cfg_s, served_radio_bearer_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rlc_bearer_cfg_s::served_radio_bearer_c_::desc},
    {0,
     (int32_t)offsetof(rlc_bearer_cfg_s, reestablish_rlc_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(rlc_bearer_cfg_s, rlc_cfg),
     (int32_t)offsetof(rlc_bearer_cfg_s, rlc_cfg_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rlc_cfg_c::desc},
    {offsetof(rlc_bearer_cfg_s, mac_lc_ch_cfg),
     (int32_t)offsetof(rlc_bearer_cfg_s, mac_lc_ch_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &lc_ch_cfg_s::desc}};
static const asn1::table::field_name seq_table_rlc_bearer_cfg_s_names[] = {{"logicalChannelIdentity", nullptr},
                                                                           {"servedRadioBearer", nullptr},
                                                                           {"reestablishRLC", "true"},
                                                                           {"rlc-Config", nullptr},
                                                                           {"mac-LogicalChannelConfig", nullptr}};
static const asn1::table::seq_opt    seq_table_rlc_bearer_cfg_s_opts[]  = {
    {1, (int32_t)offsetof(rlc_bearer_cfg_s, served_radio_bearer_present)},
    {2, (int32_t)offsetof(rlc_bearer_cfg_s, reestablish_rlc_present)},
    {3, (int32_t)offsetof(rlc_bearer_cfg_s, rlc_cfg_present)},
    {4, (int32_t)offsetof(rlc_bearer_cfg_s, mac_lc_ch_cfg_present)}};
static const asn1::table::seq_field seq_table_rlc_bearer_cfg_s_g0_fields[] = {
    {offsetof(rlc_bearer_cfg_s, rlc_cfg_v1610),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(rlc_bearer_cfg_s::rlc_cfg_v1610)>::value,
     &rlc_cfg_v1610_s::desc}};
static const asn1::table::field_name seq_table_rlc_bearer_cfg_s_g0_names[] = {{"rlc-Config-v1610", nullptr}};
static const asn1::table::seq_opt    seq_table_rlc_bearer_cfg_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_rlc_bearer_cfg_s_g1_fields[] = {
    {offsetof(rlc_bearer_cfg_s, rlc_cfg_v1700),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(rlc_bearer_cfg_s::rlc_cfg_v1700)>::value,
      &rlc_cfg_v1700_s::desc},
    {offsetof(rlc_bearer_cfg_s, lc_ch_id_ext_r17),
      (int32_t)offsetof(rlc_bearer_cfg_s, lc_ch_id_ext_r17_present),
      asn1::table::field_op::int_u32,
      false,
      false,
      int64_t{320},
      uint64_t{65535},
      nullptr},
    {offsetof(rlc_bearer_cfg_s, multicast_rlc_bearer_cfg_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(rlc_bearer_cfg_s::multicast_rlc_bearer_cfg_r17)>::value,
      &multicast_rlc_bearer_cfg_r17_s::desc},
    {offsetof(rlc_bearer_cfg_s, served_radio_bearer_srb4_r17),
      (int32_t)offsetof(rlc_bearer_cfg_s, served_radio_bearer_srb4_r17_present),
      asn1::table::field_op::int_u8,
      false,
      false,
      int64_t{4},
      uint64_t{0},
      nullptr}};
static const asn1::table::field_name seq_table_rlc_bearer_cfg_s_g1_names[] = {
    {"rlc-Config-v1700", nullptr},
    {"logicalChannelIdentityExt-r17", nullptr},
    {"multicastRLC-BearerConfig-r17", nullptr},
    {"servedRadioBearerSRB4-r17", nullptr}};
static const asn1::table::seq_opt seq_table_rlc_bearer_cfg_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(rlc_bearer_cfg_s, lc_ch_id_ext_r17_present)},
    {2, asn1::table::seq_field::ptr_presence},
    {3, (int32_t)offsetof(rlc_bearer_cfg_s, served_radio_bearer_srb4_r17_present)}};
static const asn1::table::seq_field seq_table_rlc_bearer_cfg_s_g2_fields[] = {
    {offsetof(rlc_bearer_cfg_s, served_radio_bearer_srb5_r18),
     (int32_t)offsetof(rlc_bearer_cfg_s, served_radio_bearer_srb5_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{5},
     uint64_t{0},
     nullptr}};
static const asn1::table::field_name seq_table_rlc_bearer_cfg_s_g2_names[] = {{"servedRadioBearerSRB5-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_rlc_bearer_cfg_s_g2_opts[]  = {
    {0, (int32_t)offsetof(rlc_bearer_cfg_s, served_radio_bearer_srb5_r18_present)}};
static const asn1::table::seq_group seq_table_rlc_bearer_cfg_s_groups[] = {{{seq_table_rlc_bearer_cfg_s_g0_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_rlc_bearer_cfg_s_g0_opts,
                                                                             seq_table_rlc_bearer_cfg_s_g0_names,
                                                                             1,
                                                                             1},
                                                                            false},
                                                                           {{seq_table_rlc_bearer_cfg_s_g1_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_rlc_bearer_cfg_s_g1_opts,
                                                                             seq_table_rlc_bearer_cfg_s_g1_names,
                                                                             4,
                                                                             4},
                                                                            false},
                                                                           {{seq_table_rlc_bearer_cfg_s_g2_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_rlc_bearer_cfg_s_g2_opts,
                                                                             seq_table_rlc_bearer_cfg_s_g2_names,
                                                                             1,
                                                                             1},
                                                                            false}};
const asn1::table::seq_desc         rlc_bearer_cfg_s::desc              = {{seq_table_rlc_bearer_cfg_s_fields,
                                                                            uint64_t{0x1},
                                                                            seq_table_rlc_bearer_cfg_s_opts,
                                                                            seq_table_rlc_bearer_cfg_s_names,
                                                                            5,
                                                                            4},
                                                                           true,
                                                                           false,
                                                                           seq_table_rlc_bearer_cfg_s_groups,
                                                                           3};

static const asn1::table::storage_ops* const choice_storage_rlc_bearer_cfg_s_served_radio_bearer_c__alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc rlc_bearer_cfg_s::served_radio_bearer_c_::storage_desc = {
    choice_storage_rlc_bearer_cfg_s_served_radio_bearer_c__alternatives,
    2,
    "rlc_bearer_cfg_s::served_radio_bearer_c_"};

uint8_t& rlc_bearer_cfg_s::served_radio_bearer_c_::set_srb_id()
{
  set(types::srb_id);
  return c.get<uint8_t>();
}
uint8_t& rlc_bearer_cfg_s::served_radio_bearer_c_::set_drb_id()
{
  set(types::drb_id);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_rlc_bearer_cfg_s_served_radio_bearer_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{2},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name choice_table_rlc_bearer_cfg_s_served_radio_bearer_c__names[] = {
    {"srb-Identity", nullptr},
    {"drb-Identity", nullptr}};
const asn1::table::choice_desc rlc_bearer_cfg_s::served_radio_bearer_c_::desc = {
    asn1::table::enum_int_type<rlc_bearer_cfg_s::served_radio_bearer_c_::types>(),
    rlc_bearer_cfg_s::served_radio_bearer_c_::types::nof_types,
    rlc_bearer_cfg_s::served_radio_bearer_c_::types::nof_exts,
    rlc_bearer_cfg_s::served_radio_bearer_c_::types::has_ext,
    false,
    choice_table_rlc_bearer_cfg_s_served_radio_bearer_c__alternatives,
    nullptr,
    choice_table_rlc_bearer_cfg_s_served_radio_bearer_c__names,
    "rlc_bearer_cfg_s::served_radio_bearer_c_",
    offsetof(rlc_bearer_cfg_s::served_radio_bearer_c_, c.type),
    offsetof(rlc_bearer_cfg_s::served_radio_bearer_c_, c.value),
    false,
    &rlc_bearer_cfg_s::served_radio_bearer_c_::storage_desc};

static const char* enum_names_rlc_bearer_cfg_s_served_radio_bearer_c__types_values[] = {"srb-Identity", "drb-Identity"};
const asn1::table::enum_desc rlc_bearer_cfg_s::served_radio_bearer_c_::types_opts::desc = {
    enum_names_rlc_bearer_cfg_s_served_radio_bearer_c__types_values,
    2,
    "rlc_bearer_cfg_s::served_radio_bearer_c_::types"};

// SchedulingRequestToAddMod ::= SEQUENCE
static const asn1::table::seq_field seq_table_sched_request_to_add_mod_s_fields[] = {
    {offsetof(sched_request_to_add_mod_s, sched_request_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(sched_request_to_add_mod_s, sr_prohibit_timer),
     (int32_t)offsetof(sched_request_to_add_mod_s, sr_prohibit_timer_present),
     asn1::table::enum_op<decltype(sched_request_to_add_mod_s::sr_prohibit_timer)>(),
     decltype(sched_request_to_add_mod_s::sr_prohibit_timer)::has_ext,
     false,
     decltype(sched_request_to_add_mod_s::sr_prohibit_timer)::nof_types,
     decltype(sched_request_to_add_mod_s::sr_prohibit_timer)::nof_exts,
     nullptr,
     &sched_request_to_add_mod_s::sr_prohibit_timer_opts::desc},
    {offsetof(sched_request_to_add_mod_s, sr_trans_max),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sched_request_to_add_mod_s::sr_trans_max)>(),
     decltype(sched_request_to_add_mod_s::sr_trans_max)::has_ext,
     false,
     decltype(sched_request_to_add_mod_s::sr_trans_max)::nof_types,
     decltype(sched_request_to_add_mod_s::sr_trans_max)::nof_exts,
     nullptr,
     &sched_request_to_add_mod_s::sr_trans_max_opts::desc}};
static const asn1::table::field_name seq_table_sched_request_to_add_mod_s_names[] = {{"schedulingRequestId", nullptr},
                                                                                     {"sr-ProhibitTimer", nullptr},
                                                                                     {"sr-TransMax", nullptr}};
static const asn1::table::seq_opt    seq_table_sched_request_to_add_mod_s_opts[]  = {
    {1, (int32_t)offsetof(sched_request_to_add_mod_s, sr_prohibit_timer_present)}};
const asn1::table::seq_desc sched_request_to_add_mod_s::desc = {{seq_table_sched_request_to_add_mod_s_fields,
                                                                 uint64_t{0x5},
                                                                 seq_table_sched_request_to_add_mod_s_opts,
                                                                 seq_table_sched_request_to_add_mod_s_names,
                                                                 3,
                                                                 1},
                                                                false,
                                                                false,
                                                                nullptr,
                                                                0};

static const char* enum_names_sched_request_to_add_mod_s_sr_prohibit_timer_e__values[] =
    {"ms1", "ms2", "ms4", "ms8", "ms16", "ms32", "ms64", "ms128"};
const asn1::table::enum_desc sched_request_to_add_mod_s::sr_prohibit_timer_opts::desc = {
    enum_names_sched_request_to_add_mod_s_sr_prohibit_timer_e__values,
    8,
    "sched_request_to_add_mod_s::sr_prohibit_timer_e_"};
uint8_t sched_request_to_add_mod_s::sr_prohibit_timer_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32, 64, 128};
  return map_enum_number(numbers, 8, value, "sched_request_to_add_mod_s::sr_prohibit_timer_e_");
}

static const char* enum_names_sched_request_to_add_mod_s_sr_trans_max_e__values[] =
    {"n4", "n8", "n16", "n32", "n64", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sched_request_to_add_mod_s::sr_trans_max_opts::desc = {
    enum_names_sched_request_to_add_mod_s_sr_trans_max_e__values,
    8,
    "sched_request_to_add_mod_s::sr_trans_max_e_"};
uint8_t sched_request_to_add_mod_s::sr_trans_max_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 16, 32, 64};
  return map_enum_number(numbers, 5, value, "sched_request_to_add_mod_s::sr_trans_max_e_");
}

// SchedulingRequestConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_sched_request_cfg_s_fields[] = {
    {offsetof(sched_request_cfg_s, sched_request_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(sched_request_cfg_s::sched_request_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sched_request_to_add_mod_s::desc},
    {offsetof(sched_request_cfg_s, sched_request_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(sched_request_cfg_s::sched_request_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{7}, false, false>>::value}};
static const asn1::table::field_name seq_table_sched_request_cfg_s_names[] = {
    {"schedulingRequestToAddModList", nullptr},
    {"schedulingRequestToReleaseList", nullptr}};
static const asn1::table::seq_opt seq_table_sched_request_cfg_s_opts[] = {{0, asn1::table::seq_field::list_presence},
                                                                          {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       sched_request_cfg_s::desc            = {{seq_table_sched_request_cfg_s_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_sched_request_cfg_s_opts,
                                                                           seq_table_sched_request_cfg_s_names,
                                                                           2,
                                                                           2},
                                                                          false,
                                                                          false,
                                                                          nullptr,
                                                                          0};

// TAG ::= SEQUENCE
static const asn1::table::seq_field seq_table_tag_s_fields[] = {
    {offsetof(tag_s, tag_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{3},
     nullptr},
    {offsetof(tag_s, time_align_timer),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(tag_s::time_align_timer)>(),
     decltype(tag_s::time_align_timer)::has_ext,
     false,
     decltype(tag_s::time_align_timer)::nof_types,
     decltype(tag_s::time_align_timer)::nof_exts,
     nullptr,
     &time_align_timer_opts::desc}};
static const asn1::table::field_name seq_table_tag_s_names[] = {{"tag-Id", nullptr}, {"timeAlignmentTimer", nullptr}};
const asn1::table::seq_desc          tag_s::desc =
    {{seq_table_tag_s_fields, uint64_t{0x3}, nullptr, seq_table_tag_s_names, 2, 0}, true, false, nullptr, 0};

// TAG-Config ::= SEQUENCE
static const asn1::table::seq_field seq_table_tag_cfg_s_fields[] = {
    {offsetof(tag_cfg_s, tag_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::list_codec<decltype(tag_cfg_s::tag_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{3}, false, false>>::value},
    {offsetof(tag_cfg_s, tag_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::
         item_list_codec<decltype(tag_cfg_s::tag_to_add_mod_list), asn1::table::sequence_item_ops, false, true>::value,
     &tag_s::desc}};
static const asn1::table::field_name seq_table_tag_cfg_s_names[] = {{"tag-ToReleaseList", nullptr},
                                                                    {"tag-ToAddModList", nullptr}};
static const asn1::table::seq_opt    seq_table_tag_cfg_s_opts[]  = {{0, asn1::table::seq_field::list_presence},
                                                                    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          tag_cfg_s::desc             = {
    {seq_table_tag_cfg_s_fields, uint64_t{0x0}, seq_table_tag_cfg_s_opts, seq_table_tag_cfg_s_names, 2, 2},
    false,
    false,
    nullptr,
    0};

// MPE-Config-FR2-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mpe_cfg_fr2_r16_s_fields[] = {
    {offsetof(mpe_cfg_fr2_r16_s, mpe_prohibit_timer_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16)>(),
     decltype(mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16)::has_ext,
     false,
     decltype(mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16)::nof_types,
     decltype(mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16)::nof_exts,
     nullptr,
     &mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16_opts::desc},
    {offsetof(mpe_cfg_fr2_r16_s, mpe_thres_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mpe_cfg_fr2_r16_s::mpe_thres_r16)>(),
     decltype(mpe_cfg_fr2_r16_s::mpe_thres_r16)::has_ext,
     false,
     decltype(mpe_cfg_fr2_r16_s::mpe_thres_r16)::nof_types,
     decltype(mpe_cfg_fr2_r16_s::mpe_thres_r16)::nof_exts,
     nullptr,
     &mpe_cfg_fr2_r16_s::mpe_thres_r16_opts::desc}};
static const asn1::table::field_name seq_table_mpe_cfg_fr2_r16_s_names[] = {{"mpe-ProhibitTimer-r16", nullptr},
                                                                            {"mpe-Threshold-r16", nullptr}};
const asn1::table::seq_desc          mpe_cfg_fr2_r16_s::desc             = {
    {seq_table_mpe_cfg_fr2_r16_s_fields, uint64_t{0x3}, nullptr, seq_table_mpe_cfg_fr2_r16_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};

static const char* enum_names_mpe_cfg_fr2_r16_s_mpe_prohibit_timer_r16_e__values[] =
    {"sf0", "sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000"};
const asn1::table::enum_desc mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16_opts::desc = {
    enum_names_mpe_cfg_fr2_r16_s_mpe_prohibit_timer_r16_e__values,
    8,
    "mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16_e_"};
uint16_t mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 10, 20, 50, 100, 200, 500, 1000};
  return map_enum_number(numbers, 8, value, "mpe_cfg_fr2_r16_s::mpe_prohibit_timer_r16_e_");
}

static const char*           enum_names_mpe_cfg_fr2_r16_s_mpe_thres_r16_e__values[] = {"dB3", "dB6", "dB9", "dB12"};
const asn1::table::enum_desc mpe_cfg_fr2_r16_s::mpe_thres_r16_opts::desc            = {
    enum_names_mpe_cfg_fr2_r16_s_mpe_thres_r16_e__values,
    4,
    "mpe_cfg_fr2_r16_s::mpe_thres_r16_e_"};
uint8_t mpe_cfg_fr2_r16_s::mpe_thres_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 6, 9, 12};
  return map_enum_number(numbers, 4, value, "mpe_cfg_fr2_r16_s::mpe_thres_r16_e_");
}

// MPE-Config-FR2-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mpe_cfg_fr2_r17_s_fields[] = {
    {offsetof(mpe_cfg_fr2_r17_s, mpe_prohibit_timer_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17)>(),
     decltype(mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17)::has_ext,
     false,
     decltype(mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17)::nof_types,
     decltype(mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17)::nof_exts,
     nullptr,
     &mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17_opts::desc},
    {offsetof(mpe_cfg_fr2_r17_s, mpe_thres_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mpe_cfg_fr2_r17_s::mpe_thres_r17)>(),
     decltype(mpe_cfg_fr2_r17_s::mpe_thres_r17)::has_ext,
     false,
     decltype(mpe_cfg_fr2_r17_s::mpe_thres_r17)::nof_types,
     decltype(mpe_cfg_fr2_r17_s::mpe_thres_r17)::nof_exts,
     nullptr,
     &mpe_cfg_fr2_r17_s::mpe_thres_r17_opts::desc},
    {offsetof(mpe_cfg_fr2_r17_s, nof_n_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{3},
     nullptr}};
static const asn1::table::field_name seq_table_mpe_cfg_fr2_r17_s_names[] = {{"mpe-ProhibitTimer-r17", nullptr},
                                                                            {"mpe-Threshold-r17", nullptr},
                                                                            {"numberOfN-r17", nullptr}};
const asn1::table::seq_desc          mpe_cfg_fr2_r17_s::desc             = {
    {seq_table_mpe_cfg_fr2_r17_s_fields, uint64_t{0x7}, nullptr, seq_table_mpe_cfg_fr2_r17_s_names, 3, 0},
    true,
    false,
    nullptr,
    0};

static const char* enum_names_mpe_cfg_fr2_r17_s_mpe_prohibit_timer_r17_e__values[] =
    {"sf0", "sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000"};
const asn1::table::enum_desc mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17_opts::desc = {
    enum_names_mpe_cfg_fr2_r17_s_mpe_prohibit_timer_r17_e__values,
    8,
    "mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17_e_"};
uint16_t mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 10, 20, 50, 100, 200, 500, 1000};
  return map_enum_number(numbers, 8, value, "mpe_cfg_fr2_r17_s::mpe_prohibit_timer_r17_e_");
}

static const char*           enum_names_mpe_cfg_fr2_r17_s_mpe_thres_r17_e__values[] = {"dB3", "dB6", "dB9", "dB12"};
const asn1::table::enum_desc mpe_cfg_fr2_r17_s::mpe_thres_r17_opts::desc            = {
    enum_names_mpe_cfg_fr2_r17_s_mpe_thres_r17_e__values,
    4,
    "mpe_cfg_fr2_r17_s::mpe_thres_r17_e_"};
uint8_t mpe_cfg_fr2_r17_s::mpe_thres_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 6, 9, 12};
  return map_enum_number(numbers, 4, value, "mpe_cfg_fr2_r17_s::mpe_thres_r17_e_");
}

// PHR-Config ::= SEQUENCE
static const asn1::table::seq_field seq_table_phr_cfg_s_mpe_report_fr2_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mpe_cfg_fr2_r16_s::desc}};
static const asn1::table::field_name seq_table_phr_cfg_s_mpe_report_fr2_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_phr_cfg_s_mpe_report_fr2_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_phr_cfg_s_mpe_report_fr2_r16_setup_release_alternatives,
    nullptr,
    seq_table_phr_cfg_s_mpe_report_fr2_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<mpe_cfg_fr2_r16_s>::type_offset(),
    setup_release_c<mpe_cfg_fr2_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_phr_cfg_s_mpe_report_fr2_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mpe_cfg_fr2_r17_s::desc}};
static const asn1::table::field_name seq_table_phr_cfg_s_mpe_report_fr2_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_phr_cfg_s_mpe_report_fr2_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_phr_cfg_s_mpe_report_fr2_r17_setup_release_alternatives,
    nullptr,
    seq_table_phr_cfg_s_mpe_report_fr2_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<mpe_cfg_fr2_r17_s>::type_offset(),
    setup_release_c<mpe_cfg_fr2_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_phr_cfg_s_fields[] = {
    {offsetof(phr_cfg_s, phr_periodic_timer),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(phr_cfg_s::phr_periodic_timer)>(),
     decltype(phr_cfg_s::phr_periodic_timer)::has_ext,
     false,
     decltype(phr_cfg_s::phr_periodic_timer)::nof_types,
     decltype(phr_cfg_s::phr_periodic_timer)::nof_exts,
     nullptr,
     &phr_cfg_s::phr_periodic_timer_opts::desc},
    {offsetof(phr_cfg_s, phr_prohibit_timer),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(phr_cfg_s::phr_prohibit_timer)>(),
     decltype(phr_cfg_s::phr_prohibit_timer)::has_ext,
     false,
     decltype(phr_cfg_s::phr_prohibit_timer)::nof_types,
     decltype(phr_cfg_s::phr_prohibit_timer)::nof_exts,
     nullptr,
     &phr_cfg_s::phr_prohibit_timer_opts::desc},
    {offsetof(phr_cfg_s, phr_tx_pwr_factor_change),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(phr_cfg_s::phr_tx_pwr_factor_change)>(),
     decltype(phr_cfg_s::phr_tx_pwr_factor_change)::has_ext,
     false,
     decltype(phr_cfg_s::phr_tx_pwr_factor_change)::nof_types,
     decltype(phr_cfg_s::phr_tx_pwr_factor_change)::nof_exts,
     nullptr,
     &phr_cfg_s::phr_tx_pwr_factor_change_opts::desc},
    {offsetof(phr_cfg_s, multiple_phr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(phr_cfg_s, dummy),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(phr_cfg_s, phr_type2_other_cell),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(phr_cfg_s, phr_mode_other_cg),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(phr_cfg_s::phr_mode_other_cg)>(),
     decltype(phr_cfg_s::phr_mode_other_cg)::has_ext,
     false,
     decltype(phr_cfg_s::phr_mode_other_cg)::nof_types,
     decltype(phr_cfg_s::phr_mode_other_cg)::nof_exts,
     nullptr,
     &phr_cfg_s::phr_mode_other_cg_opts::desc}};
static const asn1::table::field_name seq_table_phr_cfg_s_names[]     = {{"phr-PeriodicTimer", nullptr},
                                                                        {"phr-ProhibitTimer", nullptr},
                                                                        {"phr-Tx-PowerFactorChange", nullptr},
                                                                        {"multiplePHR", nullptr},
                                                                        {"dummy", nullptr},
                                                                        {"phr-Type2OtherCell", nullptr},
                                                                        {"phr-ModeOtherCG", nullptr}};
static const asn1::table::seq_field  seq_table_phr_cfg_s_g0_fields[] = {
    {offsetof(phr_cfg_s, mpe_report_fr2_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(phr_cfg_s::mpe_report_fr2_r16)>::value,
      &seq_table_phr_cfg_s_mpe_report_fr2_r16_setup_release}};
static const asn1::table::field_name seq_table_phr_cfg_s_g0_names[]  = {{"mpe-Reporting-FR2-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_phr_cfg_s_g0_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_phr_cfg_s_g1_fields[] = {
    {offsetof(phr_cfg_s, mpe_report_fr2_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(phr_cfg_s::mpe_report_fr2_r17)>::value,
      &seq_table_phr_cfg_s_mpe_report_fr2_r17_setup_release},
    {0,
      (int32_t)offsetof(phr_cfg_s, two_phr_mode_r17_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr}};
static const asn1::table::field_name seq_table_phr_cfg_s_g1_names[] = {{"mpe-Reporting-FR2-r17", nullptr},
                                                                       {"twoPHRMode-r17", "enabled"}};
static const asn1::table::seq_opt    seq_table_phr_cfg_s_g1_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(phr_cfg_s, two_phr_mode_r17_present)}};
static const asn1::table::seq_field seq_table_phr_cfg_s_g2_fields[] = {
    {0,
     (int32_t)offsetof(phr_cfg_s, phr_assumed_pusch_report_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phr_cfg_s, dpc_report_fr1_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_phr_cfg_s_g2_names[] = {{"phr-AssumedPUSCH-Reporting-r18", "enabled"},
                                                                       {"dpc-Reporting-FR1-r18", "enabled"}};
static const asn1::table::seq_opt    seq_table_phr_cfg_s_g2_opts[]  = {
    {0, (int32_t)offsetof(phr_cfg_s, phr_assumed_pusch_report_r18_present)},
    {1, (int32_t)offsetof(phr_cfg_s, dpc_report_fr1_r18_present)}};
static const asn1::table::seq_group seq_table_phr_cfg_s_groups[] = {
    {{seq_table_phr_cfg_s_g0_fields, uint64_t{0x0}, seq_table_phr_cfg_s_g0_opts, seq_table_phr_cfg_s_g0_names, 1, 1},
     false},
    {{seq_table_phr_cfg_s_g1_fields, uint64_t{0x0}, seq_table_phr_cfg_s_g1_opts, seq_table_phr_cfg_s_g1_names, 2, 2},
     false},
    {{seq_table_phr_cfg_s_g2_fields, uint64_t{0x0}, seq_table_phr_cfg_s_g2_opts, seq_table_phr_cfg_s_g2_names, 2, 2},
     false}};
const asn1::table::seq_desc phr_cfg_s::desc = {
    {seq_table_phr_cfg_s_fields, uint64_t{0x7f}, nullptr, seq_table_phr_cfg_s_names, 7, 0},
    true,
    false,
    seq_table_phr_cfg_s_groups,
    3};

static const char* enum_names_phr_cfg_s_phr_periodic_timer_e__values[] =
    {"sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000", "infinity"};
const asn1::table::enum_desc phr_cfg_s::phr_periodic_timer_opts::desc = {
    enum_names_phr_cfg_s_phr_periodic_timer_e__values,
    8,
    "phr_cfg_s::phr_periodic_timer_e_"};
int16_t phr_cfg_s::phr_periodic_timer_opts::to_number() const
{
  static const int16_t numbers[] = {10, 20, 50, 100, 200, 500, 1000, -1};
  return map_enum_number(numbers, 8, value, "phr_cfg_s::phr_periodic_timer_e_");
}

static const char* enum_names_phr_cfg_s_phr_prohibit_timer_e__values[] =
    {"sf0", "sf10", "sf20", "sf50", "sf100", "sf200", "sf500", "sf1000"};
const asn1::table::enum_desc phr_cfg_s::phr_prohibit_timer_opts::desc = {
    enum_names_phr_cfg_s_phr_prohibit_timer_e__values,
    8,
    "phr_cfg_s::phr_prohibit_timer_e_"};
uint16_t phr_cfg_s::phr_prohibit_timer_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 10, 20, 50, 100, 200, 500, 1000};
  return map_enum_number(numbers, 8, value, "phr_cfg_s::phr_prohibit_timer_e_");
}

static const char* enum_names_phr_cfg_s_phr_tx_pwr_factor_change_e__values[] = {"dB1", "dB3", "dB6", "infinity"};
const asn1::table::enum_desc phr_cfg_s::phr_tx_pwr_factor_change_opts::desc  = {
    enum_names_phr_cfg_s_phr_tx_pwr_factor_change_e__values,
    4,
    "phr_cfg_s::phr_tx_pwr_factor_change_e_"};
int8_t phr_cfg_s::phr_tx_pwr_factor_change_opts::to_number() const
{
  static const int8_t numbers[] = {1, 3, 6, -1};
  return map_enum_number(numbers, 4, value, "phr_cfg_s::phr_tx_pwr_factor_change_e_");
}

static const char*           enum_names_phr_cfg_s_phr_mode_other_cg_e__values[] = {"real", "virtual"};
const asn1::table::enum_desc phr_cfg_s::phr_mode_other_cg_opts::desc            = {
    enum_names_phr_cfg_s_phr_mode_other_cg_e__values,
    2,
    "phr_cfg_s::phr_mode_other_cg_e_"};

// DataInactivityTimer ::= ENUMERATED
static const char* enum_names_data_inactivity_timer_e_values[] =
    {"s1", "s2", "s3", "s5", "s7", "s10", "s15", "s20", "s40", "s50", "s60", "s80", "s100", "s120", "s150", "s180"};
const asn1::table::enum_desc data_inactivity_timer_opts::desc = {enum_names_data_inactivity_timer_e_values,
                                                                 16,
                                                                 "data_inactivity_timer_e"};
uint8_t                      data_inactivity_timer_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 5, 7, 10, 15, 20, 40, 50, 60, 80, 100, 120, 150, 180};
  return map_enum_number(numbers, 16, value, "data_inactivity_timer_e");
}

// DRX-ConfigSecondaryGroup-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_drx_cfg_secondary_group_r16_s_fields[] = {
    {offsetof(drx_cfg_secondary_group_r16_s, drx_on_dur_timer_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::desc},
    {offsetof(drx_cfg_secondary_group_r16_s, drx_inactivity_timer_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16)>(),
     decltype(drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16)::has_ext,
     false,
     decltype(drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16)::nof_types,
     decltype(drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16)::nof_exts,
     nullptr,
     &drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16_opts::desc}};
static const asn1::table::field_name seq_table_drx_cfg_secondary_group_r16_s_names[] = {
    {"drx-onDurationTimer-r16", nullptr},
    {"drx-InactivityTimer-r16", nullptr}};
const asn1::table::seq_desc drx_cfg_secondary_group_r16_s::desc = {{seq_table_drx_cfg_secondary_group_r16_s_fields,
                                                                    uint64_t{0x3},
                                                                    nullptr,
                                                                    seq_table_drx_cfg_secondary_group_r16_s_names,
                                                                    2,
                                                                    0},
                                                                   false,
                                                                   false,
                                                                   nullptr,
                                                                   0};

static const asn1::table::storage_ops* const
    choice_storage_drx_cfg_secondary_group_r16_s_drx_on_dur_timer_r16_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_e_,
                                      true>::value};
const asn1::table::choice_storage_desc drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::storage_desc = {
    choice_storage_drx_cfg_secondary_group_r16_s_drx_on_dur_timer_r16_c__alternatives,
    2,
    "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_"};

uint8_t& drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_e_&
drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
static const asn1::table::seq_field choice_table_drx_cfg_secondary_group_r16_s_drx_on_dur_timer_r16_c__alternatives[] =
    {{0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::int_u8,
      false,
      false,
      int64_t{1},
      uint64_t{30},
      nullptr},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::enum_op<drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_e_>(),
      drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_e_::has_ext,
      false,
      drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_e_::nof_types,
      drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_e_::nof_exts,
      nullptr,
      &drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_opts::desc}};
static const asn1::table::field_name choice_table_drx_cfg_secondary_group_r16_s_drx_on_dur_timer_r16_c__names[] = {
    {"subMilliSeconds", nullptr},
    {"milliSeconds", nullptr}};
const asn1::table::choice_desc drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::desc = {
    asn1::table::enum_int_type<drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::types>(),
    drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::types::nof_types,
    drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::types::nof_exts,
    drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::types::has_ext,
    false,
    choice_table_drx_cfg_secondary_group_r16_s_drx_on_dur_timer_r16_c__alternatives,
    nullptr,
    choice_table_drx_cfg_secondary_group_r16_s_drx_on_dur_timer_r16_c__names,
    "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_",
    offsetof(drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_, c.type),
    offsetof(drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_, c.value),
    false,
    &drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::storage_desc};

static const char* enum_names_drx_cfg_secondary_group_r16_s_drx_on_dur_timer_r16_c__milli_seconds_e__values[] = {
    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",   "ms20",   "ms30",  "ms40",
    "ms50",   "ms60",   "ms80",   "ms100",  "ms200",  "ms300",  "ms400",  "ms500",  "ms600",  "ms800", "ms1000",
    "ms1200", "ms1600", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_opts::desc = {
    enum_names_drx_cfg_secondary_group_r16_s_drx_on_dur_timer_r16_c__milli_seconds_e__values,
    32,
    "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_e_"};
uint16_t drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(
      numbers, 24, value, "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::milli_seconds_e_");
}

static const char* enum_names_drx_cfg_secondary_group_r16_s_drx_on_dur_timer_r16_c__types_values[] = {"subMilliSeconds",
                                                                                                      "milliSeconds"};
const asn1::table::enum_desc drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::types_opts::desc = {
    enum_names_drx_cfg_secondary_group_r16_s_drx_on_dur_timer_r16_c__types_values,
    2,
    "drx_cfg_secondary_group_r16_s::drx_on_dur_timer_r16_c_::types"};

static const char* enum_names_drx_cfg_secondary_group_r16_s_drx_inactivity_timer_r16_e__values[] = {
    "ms0",    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",   "ms20",   "ms30",
    "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920",
    "ms2560", "spare9", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16_opts::desc = {
    enum_names_drx_cfg_secondary_group_r16_s_drx_inactivity_timer_r16_e__values,
    32,
    "drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16_e_"};
uint16_t drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {0,  1,  2,  3,   4,   5,   6,   8,   10,   20,   30,  40,
                                     50, 60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 23, value, "drx_cfg_secondary_group_r16_s::drx_inactivity_timer_r16_e_");
}

// DRX-ConfigSL-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_drx_cfg_sl_r17_s_fields[] = {
    {offsetof(drx_cfg_sl_r17_s, drx_harq_rtt_timer_sl_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{56},
     nullptr},
    {offsetof(drx_cfg_sl_r17_s, drx_retx_timer_sl_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(drx_cfg_sl_r17_s::drx_retx_timer_sl_r17)>(),
     decltype(drx_cfg_sl_r17_s::drx_retx_timer_sl_r17)::has_ext,
     false,
     decltype(drx_cfg_sl_r17_s::drx_retx_timer_sl_r17)::nof_types,
     decltype(drx_cfg_sl_r17_s::drx_retx_timer_sl_r17)::nof_exts,
     nullptr,
     &drx_cfg_sl_r17_s::drx_retx_timer_sl_r17_opts::desc}};
static const asn1::table::field_name seq_table_drx_cfg_sl_r17_s_names[] = {{"drx-HARQ-RTT-TimerSL-r17", nullptr},
                                                                           {"drx-RetransmissionTimerSL-r17", nullptr}};
const asn1::table::seq_desc          drx_cfg_sl_r17_s::desc             = {
    {seq_table_drx_cfg_sl_r17_s_fields, uint64_t{0x3}, nullptr, seq_table_drx_cfg_sl_r17_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};

static const char* enum_names_drx_cfg_sl_r17_s_drx_retx_timer_sl_r17_e__values[] = {
    "sl0",     "sl1",    "sl2",    "sl4",    "sl6",    "sl8",    "sl16",    "sl24",    "sl33",    "sl40",    "sl64",
    "sl80",    "sl96",   "sl112",  "sl128",  "sl160",  "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11",
    "spare10", "spare9", "spare8", "spare7", "spare6", "spare5", "spare4",  "spare3",  "spare2",  "spare1"};
const asn1::table::enum_desc drx_cfg_sl_r17_s::drx_retx_timer_sl_r17_opts::desc = {
    enum_names_drx_cfg_sl_r17_s_drx_retx_timer_sl_r17_e__values,
    32,
    "drx_cfg_sl_r17_s::drx_retx_timer_sl_r17_e_"};
uint16_t drx_cfg_sl_r17_s::drx_retx_timer_sl_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(numbers, 17, value, "drx_cfg_sl_r17_s::drx_retx_timer_sl_r17_e_");
}

// DRX-ConfigExt-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_drx_cfg_ext_v1700_s_fields[] = {
    {offsetof(drx_cfg_ext_v1700_s, drx_harq_rtt_timer_dl_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{448},
     nullptr},
    {offsetof(drx_cfg_ext_v1700_s, drx_harq_rtt_timer_ul_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{448},
     nullptr}};
static const asn1::table::field_name seq_table_drx_cfg_ext_v1700_s_names[] = {{"drx-HARQ-RTT-TimerDL-r17", nullptr},
                                                                              {"drx-HARQ-RTT-TimerUL-r17", nullptr}};
const asn1::table::seq_desc          drx_cfg_ext_v1700_s::desc             = {
    {seq_table_drx_cfg_ext_v1700_s_fields, uint64_t{0x3}, nullptr, seq_table_drx_cfg_ext_v1700_s_names, 2, 0},
    false,
    false,
    nullptr,
    0};

// SchedulingRequestToAddModExt-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sched_request_to_add_mod_ext_v1700_s_fields[] = {
    {offsetof(sched_request_to_add_mod_ext_v1700_s, sr_prohibit_timer_v1700),
     (int32_t)offsetof(sched_request_to_add_mod_ext_v1700_s, sr_prohibit_timer_v1700_present),
     asn1::table::enum_op<decltype(sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700)>(),
     decltype(sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700)::has_ext,
     false,
     decltype(sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700)::nof_types,
     decltype(sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700)::nof_exts,
     nullptr,
     &sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700_opts::desc}};
static const asn1::table::field_name seq_table_sched_request_to_add_mod_ext_v1700_s_names[] = {
    {"sr-ProhibitTimer-v1700", nullptr}};
static const asn1::table::seq_opt seq_table_sched_request_to_add_mod_ext_v1700_s_opts[] = {
    {0, (int32_t)offsetof(sched_request_to_add_mod_ext_v1700_s, sr_prohibit_timer_v1700_present)}};
const asn1::table::seq_desc sched_request_to_add_mod_ext_v1700_s::desc = {
    {seq_table_sched_request_to_add_mod_ext_v1700_s_fields,
     uint64_t{0x0},
     seq_table_sched_request_to_add_mod_ext_v1700_s_opts,
     seq_table_sched_request_to_add_mod_ext_v1700_s_names,
     1,
     1},
    false,
    false,
    nullptr,
    0};

static const char* enum_names_sched_request_to_add_mod_ext_v1700_s_sr_prohibit_timer_v1700_e__values[] = {"ms192",
                                                                                                          "ms256",
                                                                                                          "ms320",
                                                                                                          "ms384",
                                                                                                          "ms448",
                                                                                                          "ms512",
                                                                                                          "ms576",
                                                                                                          "ms640",
                                                                                                          "ms1082",
                                                                                                          "spare7",
                                                                                                          "spare6",
                                                                                                          "spare5",
                                                                                                          "spare4",
                                                                                                          "spare3",
                                                                                                          "spare2",
                                                                                                          "spare1"};
const asn1::table::enum_desc sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700_opts::desc  = {
    enum_names_sched_request_to_add_mod_ext_v1700_s_sr_prohibit_timer_v1700_e__values,
    16,
    "sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700_e_"};
uint16_t sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700_opts::to_number() const
{
  static const uint16_t numbers[] = {192, 256, 320, 384, 448, 512, 576, 640, 1082};
  return map_enum_number(numbers, 9, value, "sched_request_to_add_mod_ext_v1700_s::sr_prohibit_timer_v1700_e_");
}

// SchedulingRequestConfig-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sched_request_cfg_v1700_s_fields[] = {
    {offsetof(sched_request_cfg_v1700_s, sched_request_to_add_mod_list_ext_v1700),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(sched_request_cfg_v1700_s::sched_request_to_add_mod_list_ext_v1700),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sched_request_to_add_mod_ext_v1700_s::desc}};
static const asn1::table::field_name seq_table_sched_request_cfg_v1700_s_names[] = {
    {"schedulingRequestToAddModListExt-v1700", nullptr}};
static const asn1::table::seq_opt seq_table_sched_request_cfg_v1700_s_opts[] = {
    {0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sched_request_cfg_v1700_s::desc = {{seq_table_sched_request_cfg_v1700_s_fields,
                                                                uint64_t{0x0},
                                                                seq_table_sched_request_cfg_v1700_s_opts,
                                                                seq_table_sched_request_cfg_v1700_s_names,
                                                                1,
                                                                1},
                                                               false,
                                                               false,
                                                               nullptr,
                                                               0};

// TAR-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_tar_cfg_r17_s_fields[] = {
    {offsetof(tar_cfg_r17_s, offset_thres_ta_r17),
     (int32_t)offsetof(tar_cfg_r17_s, offset_thres_ta_r17_present),
     asn1::table::enum_op<decltype(tar_cfg_r17_s::offset_thres_ta_r17)>(),
     decltype(tar_cfg_r17_s::offset_thres_ta_r17)::has_ext,
     false,
     decltype(tar_cfg_r17_s::offset_thres_ta_r17)::nof_types,
     decltype(tar_cfg_r17_s::offset_thres_ta_r17)::nof_exts,
     nullptr,
     &tar_cfg_r17_s::offset_thres_ta_r17_opts::desc},
    {0,
     (int32_t)offsetof(tar_cfg_r17_s, timing_advance_sr_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_tar_cfg_r17_s_names[] = {{"offsetThresholdTA-r17", nullptr},
                                                                        {"timingAdvanceSR-r17", "enabled"}};
static const asn1::table::seq_opt    seq_table_tar_cfg_r17_s_opts[]  = {
    {0, (int32_t)offsetof(tar_cfg_r17_s, offset_thres_ta_r17_present)},
    {1, (int32_t)offsetof(tar_cfg_r17_s, timing_advance_sr_r17_present)}};
const asn1::table::seq_desc tar_cfg_r17_s::desc = {
    {seq_table_tar_cfg_r17_s_fields, uint64_t{0x0}, seq_table_tar_cfg_r17_s_opts, seq_table_tar_cfg_r17_s_names, 2, 2},
    true,
    false,
    nullptr,
    0};

static const char* enum_names_tar_cfg_r17_s_offset_thres_ta_r17_e__values[] = {
    "ms0dot5", "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",     "ms7",     "ms8",     "ms9",
    "ms10",    "ms11",   "ms12",   "ms13",   "ms14",   "ms15",   "spare13", "spare12", "spare11", "spare10",
    "spare9",  "spare8", "spare7", "spare6", "spare5", "spare4", "spare3",  "spare2",  "spare1"};
const asn1::table::enum_desc tar_cfg_r17_s::offset_thres_ta_r17_opts::desc = {
    enum_names_tar_cfg_r17_s_offset_thres_ta_r17_e__values,
    29,
    "tar_cfg_r17_s::offset_thres_ta_r17_e_"};
float tar_cfg_r17_s::offset_thres_ta_r17_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0};
  return map_enum_number(numbers, 16, value, "tar_cfg_r17_s::offset_thres_ta_r17_e_");
}
const char* tar_cfg_r17_s::offset_thres_ta_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0.5", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15"};
  return convert_enum_idx(number_strs, 29, value, "tar_cfg_r17_s::offset_thres_ta_r17_e_");
}

// DRX-ConfigPTM-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_drx_cfg_ptm_r17_s_fields[] = {
    {offsetof(drx_cfg_ptm_r17_s, drx_on_dur_timer_ptm_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::desc},
    {offsetof(drx_cfg_ptm_r17_s, drx_inactivity_timer_ptm_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17)>(),
     decltype(drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17)::has_ext,
     false,
     decltype(drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17)::nof_types,
     decltype(drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17)::nof_exts,
     nullptr,
     &drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17_opts::desc},
    {offsetof(drx_cfg_ptm_r17_s, drx_harq_rtt_timer_dl_ptm_r17),
     (int32_t)offsetof(drx_cfg_ptm_r17_s, drx_harq_rtt_timer_dl_ptm_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{56},
     nullptr},
    {offsetof(drx_cfg_ptm_r17_s, drx_retx_timer_dl_ptm_r17),
     (int32_t)offsetof(drx_cfg_ptm_r17_s, drx_retx_timer_dl_ptm_r17_present),
     asn1::table::enum_op<decltype(drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17)>(),
     decltype(drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17)::has_ext,
     false,
     decltype(drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17)::nof_types,
     decltype(drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17)::nof_exts,
     nullptr,
     &drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17_opts::desc},
    {offsetof(drx_cfg_ptm_r17_s, drx_long_cycle_start_offset_ptm_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::desc},
    {offsetof(drx_cfg_ptm_r17_s, drx_slot_offset_ptm_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name seq_table_drx_cfg_ptm_r17_s_names[] = {
    {"drx-onDurationTimerPTM-r17", nullptr},
    {"drx-InactivityTimerPTM-r17", nullptr},
    {"drx-HARQ-RTT-TimerDL-PTM-r17", nullptr},
    {"drx-RetransmissionTimerDL-PTM-r17", nullptr},
    {"drx-LongCycleStartOffsetPTM-r17", nullptr},
    {"drx-SlotOffsetPTM-r17", nullptr}};
static const asn1::table::seq_opt seq_table_drx_cfg_ptm_r17_s_opts[] = {
    {2, (int32_t)offsetof(drx_cfg_ptm_r17_s, drx_harq_rtt_timer_dl_ptm_r17_present)},
    {3, (int32_t)offsetof(drx_cfg_ptm_r17_s, drx_retx_timer_dl_ptm_r17_present)}};
const asn1::table::seq_desc drx_cfg_ptm_r17_s::desc = {{seq_table_drx_cfg_ptm_r17_s_fields,
                                                        uint64_t{0x33},
                                                        seq_table_drx_cfg_ptm_r17_s_opts,
                                                        seq_table_drx_cfg_ptm_r17_s_names,
                                                        6,
                                                        2},
                                                       false,
                                                       false,
                                                       nullptr,
                                                       0};

static const asn1::table::storage_ops* const
    choice_storage_drx_cfg_ptm_r17_s_drx_on_dur_timer_ptm_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_e_, true>::value};
const asn1::table::choice_storage_desc drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::storage_desc = {
    choice_storage_drx_cfg_ptm_r17_s_drx_on_dur_timer_ptm_r17_c__alternatives,
    2,
    "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_"};

uint8_t& drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_e_&
drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
static const asn1::table::seq_field choice_table_drx_cfg_ptm_r17_s_drx_on_dur_timer_ptm_r17_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{30},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_e_>(),
     drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_e_::has_ext,
     false,
     drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_e_::nof_types,
     drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_e_::nof_exts,
     nullptr,
     &drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_opts::desc}};
static const asn1::table::field_name choice_table_drx_cfg_ptm_r17_s_drx_on_dur_timer_ptm_r17_c__names[] = {
    {"subMilliSeconds", nullptr},
    {"milliSeconds", nullptr}};
const asn1::table::choice_desc drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::desc = {
    asn1::table::enum_int_type<drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::types>(),
    drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::types::nof_types,
    drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::types::nof_exts,
    drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::types::has_ext,
    false,
    choice_table_drx_cfg_ptm_r17_s_drx_on_dur_timer_ptm_r17_c__alternatives,
    nullptr,
    choice_table_drx_cfg_ptm_r17_s_drx_on_dur_timer_ptm_r17_c__names,
    "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_",
    offsetof(drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_, c.type),
    offsetof(drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_, c.value),
    false,
    &drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::storage_desc};

static const char* enum_names_drx_cfg_ptm_r17_s_drx_on_dur_timer_ptm_r17_c__milli_seconds_e__values[] = {
    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",   "ms20",   "ms30",  "ms40",
    "ms50",   "ms60",   "ms80",   "ms100",  "ms200",  "ms300",  "ms400",  "ms500",  "ms600",  "ms800", "ms1000",
    "ms1200", "ms1600", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_opts::desc = {
    enum_names_drx_cfg_ptm_r17_s_drx_on_dur_timer_ptm_r17_c__milli_seconds_e__values,
    32,
    "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_e_"};
uint16_t drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(numbers, 24, value, "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::milli_seconds_e_");
}

static const char* enum_names_drx_cfg_ptm_r17_s_drx_on_dur_timer_ptm_r17_c__types_values[]    = {"subMilliSeconds",
                                                                                                 "milliSeconds"};
const asn1::table::enum_desc drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::types_opts::desc = {
    enum_names_drx_cfg_ptm_r17_s_drx_on_dur_timer_ptm_r17_c__types_values,
    2,
    "drx_cfg_ptm_r17_s::drx_on_dur_timer_ptm_r17_c_::types"};

static const char* enum_names_drx_cfg_ptm_r17_s_drx_inactivity_timer_ptm_r17_e__values[] = {
    "ms0",    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",   "ms20",   "ms30",
    "ms40",   "ms50",   "ms60",   "ms80",   "ms100",  "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920",
    "ms2560", "spare9", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17_opts::desc = {
    enum_names_drx_cfg_ptm_r17_s_drx_inactivity_timer_ptm_r17_e__values,
    32,
    "drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17_e_"};
uint16_t drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0,  1,  2,  3,   4,   5,   6,   8,   10,   20,   30,  40,
                                     50, 60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 23, value, "drx_cfg_ptm_r17_s::drx_inactivity_timer_ptm_r17_e_");
}

static const char* enum_names_drx_cfg_ptm_r17_s_drx_retx_timer_dl_ptm_r17_e__values[] = {
    "sl0",     "sl1",    "sl2",    "sl4",    "sl6",    "sl8",    "sl16",    "sl24",    "sl33",    "sl40",    "sl64",
    "sl80",    "sl96",   "sl112",  "sl128",  "sl160",  "sl320",  "spare15", "spare14", "spare13", "spare12", "spare11",
    "spare10", "spare9", "spare8", "spare7", "spare6", "spare5", "spare4",  "spare3",  "spare2",  "spare1"};
const asn1::table::enum_desc drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17_opts::desc = {
    enum_names_drx_cfg_ptm_r17_s_drx_retx_timer_dl_ptm_r17_e__values,
    32,
    "drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17_e_"};
uint16_t drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 1, 2, 4, 6, 8, 16, 24, 33, 40, 64, 80, 96, 112, 128, 160, 320};
  return map_enum_number(numbers, 17, value, "drx_cfg_ptm_r17_s::drx_retx_timer_dl_ptm_r17_e_");
}

static const asn1::table::storage_ops* const
    choice_storage_drx_cfg_ptm_r17_s_drx_long_cycle_start_offset_ptm_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::storage_desc = {
    choice_storage_drx_cfg_ptm_r17_s_drx_long_cycle_start_offset_ptm_r17_c__alternatives,
    20,
    "drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_"};

uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms60()
{
  set(types::ms60);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms70()
{
  set(types::ms70);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms80()
{
  set(types::ms80);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms160()
{
  set(types::ms160);
  return c.get<uint8_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms320()
{
  set(types::ms320);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms512()
{
  set(types::ms512);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms640()
{
  set(types::ms640);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms1024()
{
  set(types::ms1024);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms1280()
{
  set(types::ms1280);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms2048()
{
  set(types::ms2048);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms2560()
{
  set(types::ms2560);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms5120()
{
  set(types::ms5120);
  return c.get<uint16_t>();
}
uint16_t& drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::set_ms10240()
{
  set(types::ms10240);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field
    choice_table_drx_cfg_ptm_r17_s_drx_long_cycle_start_offset_ptm_r17_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{9},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{19},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{31},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{39},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{59},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{63},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{69},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{79},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{127},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{159},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{255},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{319},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{511},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{639},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{1023},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{1279},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{2047},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{2559},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{5119},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         int64_t{0},
         uint64_t{10239},
         nullptr}};
static const asn1::table::field_name choice_table_drx_cfg_ptm_r17_s_drx_long_cycle_start_offset_ptm_r17_c__names[] = {
    {"ms10", nullptr},   {"ms20", nullptr},   {"ms32", nullptr},   {"ms40", nullptr},   {"ms60", nullptr},
    {"ms64", nullptr},   {"ms70", nullptr},   {"ms80", nullptr},   {"ms128", nullptr},  {"ms160", nullptr},
    {"ms256", nullptr},  {"ms320", nullptr},  {"ms512", nullptr},  {"ms640", nullptr},  {"ms1024", nullptr},
    {"ms1280", nullptr}, {"ms2048", nullptr}, {"ms2560", nullptr}, {"ms5120", nullptr}, {"ms10240", nullptr}};
const asn1::table::choice_desc drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::desc = {
    asn1::table::enum_int_type<drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types>(),
    drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types::nof_types,
    drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types::nof_exts,
    drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types::has_ext,
    false,
    choice_table_drx_cfg_ptm_r17_s_drx_long_cycle_start_offset_ptm_r17_c__alternatives,
    nullptr,
    choice_table_drx_cfg_ptm_r17_s_drx_long_cycle_start_offset_ptm_r17_c__names,
    "drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_",
    offsetof(drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_, c.type),
    offsetof(drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_, c.value),
    false,
    &drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::storage_desc};

static const char* enum_names_drx_cfg_ptm_r17_s_drx_long_cycle_start_offset_ptm_r17_c__types_values[] = {
    "ms10",  "ms20",  "ms32",  "ms40",  "ms60",   "ms64",   "ms70",   "ms80",   "ms128",  "ms160",
    "ms256", "ms320", "ms512", "ms640", "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
const asn1::table::enum_desc drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types_opts::desc = {
    enum_names_drx_cfg_ptm_r17_s_drx_long_cycle_start_offset_ptm_r17_c__types_values,
    20,
    "drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types"};
uint16_t drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "drx_cfg_ptm_r17_s::drx_long_cycle_start_offset_ptm_r17_c_::types");
}

// MBS-RNTI-SpecificConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_rnti_specific_cfg_r17_s_drx_cfg_ptm_r17_setup_release_alternatives[] =
    {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &drx_cfg_ptm_r17_s::desc}};
static const asn1::table::field_name seq_table_mbs_rnti_specific_cfg_r17_s_drx_cfg_ptm_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_mbs_rnti_specific_cfg_r17_s_drx_cfg_ptm_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_mbs_rnti_specific_cfg_r17_s_drx_cfg_ptm_r17_setup_release_alternatives,
    nullptr,
    seq_table_mbs_rnti_specific_cfg_r17_s_drx_cfg_ptm_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<drx_cfg_ptm_r17_s>::type_offset(),
    setup_release_c<drx_cfg_ptm_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_mbs_rnti_specific_cfg_r17_s_fields[] = {
    {offsetof(mbs_rnti_specific_cfg_r17_s, mbs_rnti_specific_cfg_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {offsetof(mbs_rnti_specific_cfg_r17_s, group_common_rnti_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::desc},
    {offsetof(mbs_rnti_specific_cfg_r17_s, drx_cfg_ptm_r17),
     (int32_t)offsetof(mbs_rnti_specific_cfg_r17_s, drx_cfg_ptm_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_mbs_rnti_specific_cfg_r17_s_drx_cfg_ptm_r17_setup_release},
    {offsetof(mbs_rnti_specific_cfg_r17_s, harq_feedback_enabler_multicast_r17),
     (int32_t)offsetof(mbs_rnti_specific_cfg_r17_s, harq_feedback_enabler_multicast_r17_present),
     asn1::table::enum_op<decltype(mbs_rnti_specific_cfg_r17_s::harq_feedback_enabler_multicast_r17)>(),
     decltype(mbs_rnti_specific_cfg_r17_s::harq_feedback_enabler_multicast_r17)::has_ext,
     false,
     decltype(mbs_rnti_specific_cfg_r17_s::harq_feedback_enabler_multicast_r17)::nof_types,
     decltype(mbs_rnti_specific_cfg_r17_s::harq_feedback_enabler_multicast_r17)::nof_exts,
     nullptr,
     &mbs_rnti_specific_cfg_r17_s::harq_feedback_enabler_multicast_r17_opts::desc},
    {offsetof(mbs_rnti_specific_cfg_r17_s, harq_feedback_option_multicast_r17),
     (int32_t)offsetof(mbs_rnti_specific_cfg_r17_s, harq_feedback_option_multicast_r17_present),
     asn1::table::enum_op<decltype(mbs_rnti_specific_cfg_r17_s::harq_feedback_option_multicast_r17)>(),
     decltype(mbs_rnti_specific_cfg_r17_s::harq_feedback_option_multicast_r17)::has_ext,
     false,
     decltype(mbs_rnti_specific_cfg_r17_s::harq_feedback_option_multicast_r17)::nof_types,
     decltype(mbs_rnti_specific_cfg_r17_s::harq_feedback_option_multicast_r17)::nof_exts,
     nullptr,
     &mbs_rnti_specific_cfg_r17_s::harq_feedback_option_multicast_r17_opts::desc},
    {offsetof(mbs_rnti_specific_cfg_r17_s, pdsch_aggregation_factor_r17),
     (int32_t)offsetof(mbs_rnti_specific_cfg_r17_s, pdsch_aggregation_factor_r17_present),
     asn1::table::enum_op<decltype(mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17)>(),
     decltype(mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17)::has_ext,
     false,
     decltype(mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17)::nof_types,
     decltype(mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17)::nof_exts,
     nullptr,
     &mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17_opts::desc}};
static const asn1::table::field_name seq_table_mbs_rnti_specific_cfg_r17_s_names[] = {
    {"mbs-RNTI-SpecificConfigId-r17", nullptr},
    {"groupCommon-RNTI-r17", nullptr},
    {"drx-ConfigPTM-r17", nullptr},
    {"harq-FeedbackEnablerMulticast-r17", nullptr},
    {"harq-FeedbackOptionMulticast-r17", nullptr},
    {"pdsch-AggregationFactor-r17", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_rnti_specific_cfg_r17_s_opts[] = {
    {2, (int32_t)offsetof(mbs_rnti_specific_cfg_r17_s, drx_cfg_ptm_r17_present)},
    {3, (int32_t)offsetof(mbs_rnti_specific_cfg_r17_s, harq_feedback_enabler_multicast_r17_present)},
    {4, (int32_t)offsetof(mbs_rnti_specific_cfg_r17_s, harq_feedback_option_multicast_r17_present)},
    {5, (int32_t)offsetof(mbs_rnti_specific_cfg_r17_s, pdsch_aggregation_factor_r17_present)}};
const asn1::table::seq_desc mbs_rnti_specific_cfg_r17_s::desc = {{seq_table_mbs_rnti_specific_cfg_r17_s_fields,
                                                                  uint64_t{0x3},
                                                                  seq_table_mbs_rnti_specific_cfg_r17_s_opts,
                                                                  seq_table_mbs_rnti_specific_cfg_r17_s_names,
                                                                  6,
                                                                  4},
                                                                 false,
                                                                 false,
                                                                 nullptr,
                                                                 0};

static const asn1::table::storage_ops* const
    choice_storage_mbs_rnti_specific_cfg_r17_s_group_common_rnti_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<uint32_t, true>::value,
        &asn1::table::alternative_ops<uint32_t, true>::value};
const asn1::table::choice_storage_desc mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::storage_desc = {
    choice_storage_mbs_rnti_specific_cfg_r17_s_group_common_rnti_r17_c__alternatives,
    2,
    "mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_"};

uint32_t& mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::set_g_rnti()
{
  set(types::g_rnti);
  return c.get<uint32_t>();
}
uint32_t& mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::set_g_cs_rnti()
{
  set(types::g_cs_rnti);
  return c.get<uint32_t>();
}
static const asn1::table::seq_field choice_table_mbs_rnti_specific_cfg_r17_s_group_common_rnti_r17_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name choice_table_mbs_rnti_specific_cfg_r17_s_group_common_rnti_r17_c__names[] = {
    {"g-RNTI", nullptr},
    {"g-CS-RNTI", nullptr}};
const asn1::table::choice_desc mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::desc = {
    asn1::table::enum_int_type<mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::types>(),
    mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::types::nof_types,
    mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::types::nof_exts,
    mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::types::has_ext,
    false,
    choice_table_mbs_rnti_specific_cfg_r17_s_group_common_rnti_r17_c__alternatives,
    nullptr,
    choice_table_mbs_rnti_specific_cfg_r17_s_group_common_rnti_r17_c__names,
    "mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_",
    offsetof(mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_, c.type),
    offsetof(mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_, c.value),
    false,
    &mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::storage_desc};

static const char*           enum_names_mbs_rnti_specific_cfg_r17_s_group_common_rnti_r17_c__types_values[] = {"g-RNTI",
                                                                                                               "g-CS-RNTI"};
const asn1::table::enum_desc mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::types_opts::desc        = {
    enum_names_mbs_rnti_specific_cfg_r17_s_group_common_rnti_r17_c__types_values,
    2,
    "mbs_rnti_specific_cfg_r17_s::group_common_rnti_r17_c_::types"};

static const char* enum_names_mbs_rnti_specific_cfg_r17_s_harq_feedback_enabler_multicast_r17_e__values[] = {
    "dci-enabler",
    "enabled"};
const asn1::table::enum_desc mbs_rnti_specific_cfg_r17_s::harq_feedback_enabler_multicast_r17_opts::desc = {
    enum_names_mbs_rnti_specific_cfg_r17_s_harq_feedback_enabler_multicast_r17_e__values,
    2,
    "mbs_rnti_specific_cfg_r17_s::harq_feedback_enabler_multicast_r17_e_"};

static const char* enum_names_mbs_rnti_specific_cfg_r17_s_harq_feedback_option_multicast_r17_e__values[] = {
    "ack-nack",
    "nack-only"};
const asn1::table::enum_desc mbs_rnti_specific_cfg_r17_s::harq_feedback_option_multicast_r17_opts::desc = {
    enum_names_mbs_rnti_specific_cfg_r17_s_harq_feedback_option_multicast_r17_e__values,
    2,
    "mbs_rnti_specific_cfg_r17_s::harq_feedback_option_multicast_r17_e_"};

static const char* enum_names_mbs_rnti_specific_cfg_r17_s_pdsch_aggregation_factor_r17_e__values[] = {"n2", "n4", "n8"};
const asn1::table::enum_desc mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17_opts::desc  = {
    enum_names_mbs_rnti_specific_cfg_r17_s_pdsch_aggregation_factor_r17_e__values,
    3,
    "mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17_e_"};
uint8_t mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "mbs_rnti_specific_cfg_r17_s::pdsch_aggregation_factor_r17_e_");
}

// DRX-ConfigExt2-v1800 ::= SEQUENCE
static const asn1::table::seq_field seq_table_drx_cfg_ext2_v1800_s_short_drx_r18_s__fields[] = {
    {offsetof(drx_cfg_ext2_v1800_s::short_drx_r18_s_, drx_non_integer_short_cycle_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(drx_cfg_ext2_v1800_s::short_drx_r18_s_::drx_non_integer_short_cycle_r18)>(),
     decltype(drx_cfg_ext2_v1800_s::short_drx_r18_s_::drx_non_integer_short_cycle_r18)::has_ext,
     false,
     decltype(drx_cfg_ext2_v1800_s::short_drx_r18_s_::drx_non_integer_short_cycle_r18)::nof_types,
     decltype(drx_cfg_ext2_v1800_s::short_drx_r18_s_::drx_non_integer_short_cycle_r18)::nof_exts,
     nullptr,
     &drx_cfg_ext2_v1800_s::short_drx_r18_s_::drx_non_integer_short_cycle_r18_opts::desc},
    {offsetof(drx_cfg_ext2_v1800_s::short_drx_r18_s_, drx_short_cycle_timer_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{15},
     nullptr}};
static const asn1::table::field_name seq_table_drx_cfg_ext2_v1800_s_short_drx_r18_s__names[] = {
    {"drx-NonIntegerShortCycle-r18", nullptr},
    {"drx-ShortCycleTimer-r18", nullptr}};
static const asn1::table::seq_desc seq_table_drx_cfg_ext2_v1800_s_short_drx_r18_s_ = {
    {seq_table_drx_cfg_ext2_v1800_s_short_drx_r18_s__fields,
     uint64_t{0x3},
     nullptr,
     seq_table_drx_cfg_ext2_v1800_s_short_drx_r18_s__names,
     2,
     0},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_drx_cfg_ext2_v1800_s_fields[] = {
    {offsetof(drx_cfg_ext2_v1800_s, drx_non_integer_long_cycle_start_offset_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::desc},
    {offsetof(drx_cfg_ext2_v1800_s, short_drx_r18),
     (int32_t)offsetof(drx_cfg_ext2_v1800_s, short_drx_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_drx_cfg_ext2_v1800_s_short_drx_r18_s_},
    {0,
     (int32_t)offsetof(drx_cfg_ext2_v1800_s, drx_time_ref_sfn_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_drx_cfg_ext2_v1800_s_names[] = {
    {"drx-NonIntegerLongCycleStartOffset-r18", nullptr},
    {"shortDRX-r18", nullptr},
    {"drx-TimeReferenceSFN-r18", "sfn512"}};
static const asn1::table::seq_opt seq_table_drx_cfg_ext2_v1800_s_opts[] = {
    {1, (int32_t)offsetof(drx_cfg_ext2_v1800_s, short_drx_r18_present)},
    {2, (int32_t)offsetof(drx_cfg_ext2_v1800_s, drx_time_ref_sfn_r18_present)}};
const asn1::table::seq_desc drx_cfg_ext2_v1800_s::desc = {{seq_table_drx_cfg_ext2_v1800_s_fields,
                                                           uint64_t{0x1},
                                                           seq_table_drx_cfg_ext2_v1800_s_opts,
                                                           seq_table_drx_cfg_ext2_v1800_s_names,
                                                           3,
                                                           2},
                                                          false,
                                                          false,
                                                          nullptr,
                                                          0};

static const asn1::table::storage_ops* const
    choice_storage_drx_cfg_ext2_v1800_s_drx_non_integer_long_cycle_start_offset_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc
    drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::storage_desc = {
        choice_storage_drx_cfg_ext2_v1800_s_drx_non_integer_long_cycle_start_offset_r18_c__alternatives,
        23,
        "drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_"};

uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms1001over240()
{
  set(types::ms1001over240);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms25over6()
{
  set(types::ms25over6);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms25over3()
{
  set(types::ms25over3);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms1001over120()
{
  set(types::ms1001over120);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms100over9()
{
  set(types::ms100over9);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms25over2()
{
  set(types::ms25over2);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms40over3()
{
  set(types::ms40over3);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms125over9()
{
  set(types::ms125over9);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms50over3()
{
  set(types::ms50over3);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms1001over60()
{
  set(types::ms1001over60);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms125over6()
{
  set(types::ms125over6);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms200over9()
{
  set(types::ms200over9);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms250over9()
{
  set(types::ms250over9);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms100over3()
{
  set(types::ms100over3);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms1001over30()
{
  set(types::ms1001over30);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms75over2()
{
  set(types::ms75over2);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms125over3()
{
  set(types::ms125over3);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms1001over24()
{
  set(types::ms1001over24);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms200over3()
{
  set(types::ms200over3);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms1001over15()
{
  set(types::ms1001over15);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms250over3()
{
  set(types::ms250over3);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms1001over12()
{
  set(types::ms1001over12);
  return c.get<uint8_t>();
}
uint8_t& drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::set_ms400over3()
{
  set(types::ms400over3);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field
    choice_table_drx_cfg_ext2_v1800_s_drx_non_integer_long_cycle_start_offset_r18_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{3},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{3},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{7},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{7},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{10},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{11},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{12},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{12},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{15},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{15},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{19},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{21},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{26},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{32},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{32},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{36},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{40},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{40},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{65},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{65},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{82},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{82},
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{132},
         nullptr}};
static const asn1::table::field_name
    choice_table_drx_cfg_ext2_v1800_s_drx_non_integer_long_cycle_start_offset_r18_c__names[] = {
        {"ms1001over240", nullptr}, {"ms25over6", nullptr},    {"ms25over3", nullptr},    {"ms1001over120", nullptr},
        {"ms100over9", nullptr},    {"ms25over2", nullptr},    {"ms40over3", nullptr},    {"ms125over9", nullptr},
        {"ms50over3", nullptr},     {"ms1001over60", nullptr}, {"ms125over6", nullptr},   {"ms200over9", nullptr},
        {"ms250over9", nullptr},    {"ms100over3", nullptr},   {"ms1001over30", nullptr}, {"ms75over2", nullptr},
        {"ms125over3", nullptr},    {"ms1001over24", nullptr}, {"ms200over3", nullptr},   {"ms1001over15", nullptr},
        {"ms250over3", nullptr},    {"ms1001over12", nullptr}, {"ms400over3", nullptr}};
const asn1::table::choice_desc drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::desc = {
    asn1::table::enum_int_type<drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::types>(),
    drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::types::nof_types,
    drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::types::nof_exts,
    drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::types::has_ext,
    false,
    choice_table_drx_cfg_ext2_v1800_s_drx_non_integer_long_cycle_start_offset_r18_c__alternatives,
    nullptr,
    choice_table_drx_cfg_ext2_v1800_s_drx_non_integer_long_cycle_start_offset_r18_c__names,
    "drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_",
    offsetof(drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_, c.type),
    offsetof(drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_, c.value),
    false,
    &drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::storage_desc};

static const char* enum_names_drx_cfg_ext2_v1800_s_drx_non_integer_long_cycle_start_offset_r18_c__types_values[] = {
    "ms1001over240", "ms25over6",    "ms25over3",    "ms1001over120", "ms100over9", "ms25over2",
    "ms40over3",     "ms125over9",   "ms50over3",    "ms1001over60",  "ms125over6", "ms200over9",
    "ms250over9",    "ms100over3",   "ms1001over30", "ms75over2",     "ms125over3", "ms1001over24",
    "ms200over3",    "ms1001over15", "ms250over3",   "ms1001over12",  "ms400over3"};
const asn1::table::enum_desc drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::types_opts::desc = {
    enum_names_drx_cfg_ext2_v1800_s_drx_non_integer_long_cycle_start_offset_r18_c__types_values,
    23,
    "drx_cfg_ext2_v1800_s::drx_non_integer_long_cycle_start_offset_r18_c_::types"};

static const char* enum_names_drx_cfg_ext2_v1800_s_short_drx_r18_s__drx_non_integer_short_cycle_r18_e__values[] = {
    "ms1001over240", "ms25over6",    "ms25over3",    "ms1001over120", "ms100over9", "ms25over2",  "ms40over3",
    "ms125over9",    "ms50over3",    "ms1001over60", "ms125over6",    "ms200over9", "ms100over3", "ms1001over30",
    "ms125over3",    "ms1001over24", "ms200over3",   "spare15",       "spare14",    "spare13",    "spare12",
    "spare11",       "spare10",      "spare9",       "spare8",        "spare7",     "spare6",     "spare5",
    "spare4",        "spare3",       "spare2",       "spare1"};
const asn1::table::enum_desc drx_cfg_ext2_v1800_s::short_drx_r18_s_::drx_non_integer_short_cycle_r18_opts::desc = {
    enum_names_drx_cfg_ext2_v1800_s_short_drx_r18_s__drx_non_integer_short_cycle_r18_e__values,
    32,
    "drx_cfg_ext2_v1800_s::short_drx_r18_s_::drx_non_integer_short_cycle_r18_e_"};

// LCG-DSR-Config-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_lcg_dsr_cfg_r18_s_fields[] = {
    {offsetof(lcg_dsr_cfg_r18_s, lcg_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(lcg_dsr_cfg_r18_s, remaining_time_thres_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{63},
     nullptr}};
static const asn1::table::field_name seq_table_lcg_dsr_cfg_r18_s_names[] = {{"lcg-Id-r18", nullptr},
                                                                            {"remainingTimeThreshold-r18", nullptr}};
const asn1::table::seq_desc          lcg_dsr_cfg_r18_s::desc             = {
    {seq_table_lcg_dsr_cfg_r18_s_fields, uint64_t{0x3}, nullptr, seq_table_lcg_dsr_cfg_r18_s_names, 2, 0},
    true,
    false,
    nullptr,
    0};

// TAR-Config-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_tar_cfg_r18_s_fields[] = {
    {offsetof(tar_cfg_r18_s, offset_thres_ta_r18),
     (int32_t)offsetof(tar_cfg_r18_s, offset_thres_ta_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{55},
     nullptr},
    {0,
     (int32_t)offsetof(tar_cfg_r18_s, timing_advance_sr_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_tar_cfg_r18_s_names[] = {{"offsetThresholdTA-r18", nullptr},
                                                                        {"timingAdvanceSR-r18", "enabled"}};
static const asn1::table::seq_opt    seq_table_tar_cfg_r18_s_opts[]  = {
    {0, (int32_t)offsetof(tar_cfg_r18_s, offset_thres_ta_r18_present)},
    {1, (int32_t)offsetof(tar_cfg_r18_s, timing_advance_sr_r18_present)}};
const asn1::table::seq_desc tar_cfg_r18_s::desc = {
    {seq_table_tar_cfg_r18_s_fields, uint64_t{0x0}, seq_table_tar_cfg_r18_s_opts, seq_table_tar_cfg_r18_s_names, 2, 2},
    true,
    false,
    nullptr,
    0};

// MAC-CellGroupConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_drx_cfg_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drx_cfg_s::desc}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_drx_cfg_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_mac_cell_group_cfg_s_drx_cfg_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_mac_cell_group_cfg_s_drx_cfg_setup_release_alternatives,
    nullptr,
    seq_table_mac_cell_group_cfg_s_drx_cfg_setup_release_names,
    "setup_release_c",
    setup_release_c<drx_cfg_s>::type_offset(),
    setup_release_c<drx_cfg_s>::value_offset()};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_phr_cfg_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &phr_cfg_s::desc}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_phr_cfg_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_mac_cell_group_cfg_s_phr_cfg_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_mac_cell_group_cfg_s_phr_cfg_setup_release_alternatives,
    nullptr,
    seq_table_mac_cell_group_cfg_s_phr_cfg_setup_release_names,
    "setup_release_c",
    setup_release_c<phr_cfg_s>::type_offset(),
    setup_release_c<phr_cfg_s>::value_offset()};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_data_inactivity_timer_setup_release_alternatives[] =
    {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::enum_op<setup_release_c<data_inactivity_timer_e>::value_type>(),
      setup_release_c<data_inactivity_timer_e>::value_type::has_ext,
      false,
      setup_release_c<data_inactivity_timer_e>::value_type::nof_types,
      setup_release_c<data_inactivity_timer_e>::value_type::nof_exts,
      nullptr,
      &data_inactivity_timer_opts::desc}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_data_inactivity_timer_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_mac_cell_group_cfg_s_data_inactivity_timer_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_mac_cell_group_cfg_s_data_inactivity_timer_setup_release_alternatives,
    nullptr,
    seq_table_mac_cell_group_cfg_s_data_inactivity_timer_setup_release_names,
    "setup_release_c",
    setup_release_c<data_inactivity_timer_e>::type_offset(),
    setup_release_c<data_inactivity_timer_e>::value_offset()};
static const asn1::table::seq_field
    seq_table_mac_cell_group_cfg_s_drx_cfg_secondary_group_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &drx_cfg_secondary_group_r16_s::desc}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_drx_cfg_secondary_group_r16_setup_release_names[] =
    {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_mac_cell_group_cfg_s_drx_cfg_secondary_group_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_mac_cell_group_cfg_s_drx_cfg_secondary_group_r16_setup_release_alternatives,
    nullptr,
    seq_table_mac_cell_group_cfg_s_drx_cfg_secondary_group_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<drx_cfg_secondary_group_r16_s>::type_offset(),
    setup_release_c<drx_cfg_secondary_group_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_drx_cfg_sl_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drx_cfg_sl_r17_s::desc}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_drx_cfg_sl_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_mac_cell_group_cfg_s_drx_cfg_sl_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_mac_cell_group_cfg_s_drx_cfg_sl_r17_setup_release_alternatives,
    nullptr,
    seq_table_mac_cell_group_cfg_s_drx_cfg_sl_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<drx_cfg_sl_r17_s>::type_offset(),
    setup_release_c<drx_cfg_sl_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_drx_cfg_ext_v1700_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drx_cfg_ext_v1700_s::desc}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_drx_cfg_ext_v1700_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_mac_cell_group_cfg_s_drx_cfg_ext_v1700_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_mac_cell_group_cfg_s_drx_cfg_ext_v1700_setup_release_alternatives,
    nullptr,
    seq_table_mac_cell_group_cfg_s_drx_cfg_ext_v1700_setup_release_names,
    "setup_release_c",
    setup_release_c<drx_cfg_ext_v1700_s>::type_offset(),
    setup_release_c<drx_cfg_ext_v1700_s>::value_offset()};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_tar_cfg_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tar_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_tar_cfg_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_mac_cell_group_cfg_s_tar_cfg_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_mac_cell_group_cfg_s_tar_cfg_r17_setup_release_alternatives,
    nullptr,
    seq_table_mac_cell_group_cfg_s_tar_cfg_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<tar_cfg_r17_s>::type_offset(),
    setup_release_c<tar_cfg_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_drx_cfg_ext2_v1800_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drx_cfg_ext2_v1800_s::desc}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_drx_cfg_ext2_v1800_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_mac_cell_group_cfg_s_drx_cfg_ext2_v1800_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_mac_cell_group_cfg_s_drx_cfg_ext2_v1800_setup_release_alternatives,
    nullptr,
    seq_table_mac_cell_group_cfg_s_drx_cfg_ext2_v1800_setup_release_names,
    "setup_release_c",
    setup_release_c<drx_cfg_ext2_v1800_s>::type_offset(),
    setup_release_c<drx_cfg_ext2_v1800_s>::value_offset()};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_tar_cfg_r18_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tar_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_tar_cfg_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_mac_cell_group_cfg_s_tar_cfg_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_mac_cell_group_cfg_s_tar_cfg_r18_setup_release_alternatives,
    nullptr,
    seq_table_mac_cell_group_cfg_s_tar_cfg_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<tar_cfg_r18_s>::type_offset(),
    setup_release_c<tar_cfg_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_fields[] = {
    {offsetof(mac_cell_group_cfg_s, drx_cfg),
     (int32_t)offsetof(mac_cell_group_cfg_s, drx_cfg_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_mac_cell_group_cfg_s_drx_cfg_setup_release},
    {offsetof(mac_cell_group_cfg_s, sched_request_cfg),
     (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sched_request_cfg_s::desc},
    {offsetof(mac_cell_group_cfg_s, bsr_cfg),
     (int32_t)offsetof(mac_cell_group_cfg_s, bsr_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bsr_cfg_s::desc},
    {offsetof(mac_cell_group_cfg_s, tag_cfg),
     (int32_t)offsetof(mac_cell_group_cfg_s, tag_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tag_cfg_s::desc},
    {offsetof(mac_cell_group_cfg_s, phr_cfg),
     (int32_t)offsetof(mac_cell_group_cfg_s, phr_cfg_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_mac_cell_group_cfg_s_phr_cfg_setup_release},
    {offsetof(mac_cell_group_cfg_s, skip_ul_tx_dyn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_names[] = {{"drx-Config", nullptr},
                                                                               {"schedulingRequestConfig", nullptr},
                                                                               {"bsr-Config", nullptr},
                                                                               {"tag-Config", nullptr},
                                                                               {"phr-Config", nullptr},
                                                                               {"skipUplinkTxDynamic", nullptr}};
static const asn1::table::seq_opt    seq_table_mac_cell_group_cfg_s_opts[]  = {
    {0, (int32_t)offsetof(mac_cell_group_cfg_s, drx_cfg_present)},
    {1, (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_cfg_present)},
    {2, (int32_t)offsetof(mac_cell_group_cfg_s, bsr_cfg_present)},
    {3, (int32_t)offsetof(mac_cell_group_cfg_s, tag_cfg_present)},
    {4, (int32_t)offsetof(mac_cell_group_cfg_s, phr_cfg_present)}};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_g0_fields[] = {
    {offsetof(mac_cell_group_cfg_s, csi_mask),
     (int32_t)offsetof(mac_cell_group_cfg_s, csi_mask_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(mac_cell_group_cfg_s, data_inactivity_timer),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(mac_cell_group_cfg_s::data_inactivity_timer)>::value,
     &seq_table_mac_cell_group_cfg_s_data_inactivity_timer_setup_release}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_g0_names[] = {{"csi-Mask", nullptr},
                                                                                  {"dataInactivityTimer", nullptr}};
static const asn1::table::seq_opt    seq_table_mac_cell_group_cfg_s_g0_opts[]  = {
    {0, (int32_t)offsetof(mac_cell_group_cfg_s, csi_mask_present)},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_g1_fields[] = {
    {0,
     (int32_t)offsetof(mac_cell_group_cfg_s, use_pre_bsr_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(mac_cell_group_cfg_s, sched_request_id_lbt_scell_r16),
     (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_id_lbt_scell_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {0,
     (int32_t)offsetof(mac_cell_group_cfg_s, lch_based_prioritization_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(mac_cell_group_cfg_s, sched_request_id_bfr_scell_r16),
     (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_id_bfr_scell_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(mac_cell_group_cfg_s, drx_cfg_secondary_group_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(mac_cell_group_cfg_s::drx_cfg_secondary_group_r16)>::value,
     &seq_table_mac_cell_group_cfg_s_drx_cfg_secondary_group_r16_setup_release}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_g1_names[] = {
    {"usePreBSR-r16", "true"},
    {"schedulingRequestID-LBT-SCell-r16", nullptr},
    {"lch-BasedPrioritization-r16", "enabled"},
    {"schedulingRequestID-BFR-SCell-r16", nullptr},
    {"drx-ConfigSecondaryGroup-r16", nullptr}};
static const asn1::table::seq_opt seq_table_mac_cell_group_cfg_s_g1_opts[] = {
    {0, (int32_t)offsetof(mac_cell_group_cfg_s, use_pre_bsr_r16_present)},
    {1, (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_id_lbt_scell_r16_present)},
    {2, (int32_t)offsetof(mac_cell_group_cfg_s, lch_based_prioritization_r16_present)},
    {3, (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_id_bfr_scell_r16_present)},
    {4, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_g2_fields[] = {
    {0,
     (int32_t)offsetof(mac_cell_group_cfg_s, enhanced_skip_ul_tx_dyn_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mac_cell_group_cfg_s, enhanced_skip_ul_tx_cfg_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_g2_names[] = {
    {"enhancedSkipUplinkTxDynamic-r16", "true"},
    {"enhancedSkipUplinkTxConfigured-r16", "true"}};
static const asn1::table::seq_opt seq_table_mac_cell_group_cfg_s_g2_opts[] = {
    {0, (int32_t)offsetof(mac_cell_group_cfg_s, enhanced_skip_ul_tx_dyn_r16_present)},
    {1, (int32_t)offsetof(mac_cell_group_cfg_s, enhanced_skip_ul_tx_cfg_r16_present)}};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_g3_fields[] = {
    {0,
     (int32_t)offsetof(mac_cell_group_cfg_s, intra_cg_prioritization_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(mac_cell_group_cfg_s, drx_cfg_sl_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(mac_cell_group_cfg_s::drx_cfg_sl_r17)>::value,
     &seq_table_mac_cell_group_cfg_s_drx_cfg_sl_r17_setup_release},
    {offsetof(mac_cell_group_cfg_s, drx_cfg_ext_v1700),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(mac_cell_group_cfg_s::drx_cfg_ext_v1700)>::value,
     &seq_table_mac_cell_group_cfg_s_drx_cfg_ext_v1700_setup_release},
    {offsetof(mac_cell_group_cfg_s, sched_request_id_bfr_r17),
     (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_id_bfr_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(mac_cell_group_cfg_s, sched_request_id_bfr2_r17),
     (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_id_bfr2_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(mac_cell_group_cfg_s, sched_request_cfg_v1700),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(mac_cell_group_cfg_s::sched_request_cfg_v1700)>::value,
     &sched_request_cfg_v1700_s::desc},
    {offsetof(mac_cell_group_cfg_s, tar_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(mac_cell_group_cfg_s::tar_cfg_r17)>::value,
     &seq_table_mac_cell_group_cfg_s_tar_cfg_r17_setup_release},
    {offsetof(mac_cell_group_cfg_s, g_rnti_cfg_to_add_mod_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::ptr_item_list_codec<decltype(mac_cell_group_cfg_s::g_rnti_cfg_to_add_mod_list_r17),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &mbs_rnti_specific_cfg_r17_s::desc},
    {offsetof(mac_cell_group_cfg_s, g_rnti_cfg_to_release_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     15,
     &asn1::table::ptr_list_codec<decltype(mac_cell_group_cfg_s::g_rnti_cfg_to_release_list_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{15}, false, false>>::value},
    {offsetof(mac_cell_group_cfg_s, g_cs_rnti_cfg_to_add_mod_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_item_list_codec<decltype(mac_cell_group_cfg_s::g_cs_rnti_cfg_to_add_mod_list_r17),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &mbs_rnti_specific_cfg_r17_s::desc},
    {offsetof(mac_cell_group_cfg_s, g_cs_rnti_cfg_to_release_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_list_codec<decltype(mac_cell_group_cfg_s::g_cs_rnti_cfg_to_release_list_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{15}, false, false>>::value},
    {offsetof(mac_cell_group_cfg_s, allow_csi_srs_tx_multicast_drx_active_r17),
     (int32_t)offsetof(mac_cell_group_cfg_s, allow_csi_srs_tx_multicast_drx_active_r17_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_g3_names[] = {
    {"intraCG-Prioritization-r17", "enabled"},
    {"drx-ConfigSL-r17", nullptr},
    {"drx-ConfigExt-v1700", nullptr},
    {"schedulingRequestID-BFR-r17", nullptr},
    {"schedulingRequestID-BFR2-r17", nullptr},
    {"schedulingRequestConfig-v1700", nullptr},
    {"tar-Config-r17", nullptr},
    {"g-RNTI-ConfigToAddModList-r17", nullptr},
    {"g-RNTI-ConfigToReleaseList-r17", nullptr},
    {"g-CS-RNTI-ConfigToAddModList-r17", nullptr},
    {"g-CS-RNTI-ConfigToReleaseList-r17", nullptr},
    {"allowCSI-SRS-Tx-MulticastDRX-Active-r17", nullptr}};
static const asn1::table::seq_opt seq_table_mac_cell_group_cfg_s_g3_opts[] = {
    {0, (int32_t)offsetof(mac_cell_group_cfg_s, intra_cg_prioritization_r17_present)},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_id_bfr_r17_present)},
    {4, (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_id_bfr2_r17_present)},
    {5, asn1::table::seq_field::ptr_presence},
    {6, asn1::table::seq_field::ptr_presence},
    {7, asn1::table::seq_field::ptr_presence},
    {8, asn1::table::seq_field::ptr_presence},
    {9, asn1::table::seq_field::ptr_presence},
    {10, asn1::table::seq_field::ptr_presence},
    {11, (int32_t)offsetof(mac_cell_group_cfg_s, allow_csi_srs_tx_multicast_drx_active_r17_present)}};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_g4_fields[] = {
    {offsetof(mac_cell_group_cfg_s, sched_request_id_pos_mg_request_r17),
     (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_id_pos_mg_request_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {0,
     (int32_t)offsetof(mac_cell_group_cfg_s, drx_last_tx_ul_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_g4_names[] = {
    {"schedulingRequestID-PosMG-Request-r17", nullptr},
    {"drx-LastTransmissionUL-r17", "enabled"}};
static const asn1::table::seq_opt seq_table_mac_cell_group_cfg_s_g4_opts[] = {
    {0, (int32_t)offsetof(mac_cell_group_cfg_s, sched_request_id_pos_mg_request_r17_present)},
    {1, (int32_t)offsetof(mac_cell_group_cfg_s, drx_last_tx_ul_r17_present)}};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_g5_fields[] = {
    {0,
     (int32_t)offsetof(mac_cell_group_cfg_s, pos_mg_request_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_g5_names[] = {{"posMG-Request-r17", "enabled"}};
static const asn1::table::seq_opt    seq_table_mac_cell_group_cfg_s_g5_opts[]  = {
    {0, (int32_t)offsetof(mac_cell_group_cfg_s, pos_mg_request_r17_present)}};
static const asn1::table::seq_field seq_table_mac_cell_group_cfg_s_g6_fields[] = {
    {offsetof(mac_cell_group_cfg_s, drx_cfg_ext2_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(mac_cell_group_cfg_s::drx_cfg_ext2_v1800)>::value,
     &seq_table_mac_cell_group_cfg_s_drx_cfg_ext2_v1800_setup_release},
    {offsetof(mac_cell_group_cfg_s, add_bs_table_allowed_r18),
     (int32_t)offsetof(mac_cell_group_cfg_s, add_bs_table_allowed_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mac_cell_group_cfg_s::add_bs_table_allowed_r18),
                                asn1::table::struct_json>::value},
    {offsetof(mac_cell_group_cfg_s, dsr_cfg_to_add_mod_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_item_list_codec<decltype(mac_cell_group_cfg_s::dsr_cfg_to_add_mod_list_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &lcg_dsr_cfg_r18_s::desc},
    {offsetof(mac_cell_group_cfg_s, dsr_cfg_to_release_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_list_codec<decltype(mac_cell_group_cfg_s::dsr_cfg_to_release_list_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{7}, false, false>>::value},
    {offsetof(mac_cell_group_cfg_s, tar_cfg_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(mac_cell_group_cfg_s::tar_cfg_r18)>::value,
     &seq_table_mac_cell_group_cfg_s_tar_cfg_r18_setup_release}};
static const asn1::table::field_name seq_table_mac_cell_group_cfg_s_g6_names[] = {
    {"drx-ConfigExt2-v1800", nullptr},
    {"additionalBS-TableAllowed-r18", nullptr},
    {"dsr-ConfigToAddModList-r18", nullptr},
    {"dsr-ConfigToReleaseList-r18", nullptr},
    {"tar-Config-r18", nullptr}};
static const asn1::table::seq_opt seq_table_mac_cell_group_cfg_s_g6_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(mac_cell_group_cfg_s, add_bs_table_allowed_r18_present)},
    {2, asn1::table::seq_field::ptr_presence},
    {3, asn1::table::seq_field::ptr_presence},
    {4, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_mac_cell_group_cfg_s_groups[] = {
    {{seq_table_mac_cell_group_cfg_s_g0_fields,
      uint64_t{0x0},
      seq_table_mac_cell_group_cfg_s_g0_opts,
      seq_table_mac_cell_group_cfg_s_g0_names,
      2,
      2},
     false},
    {{seq_table_mac_cell_group_cfg_s_g1_fields,
      uint64_t{0x0},
      seq_table_mac_cell_group_cfg_s_g1_opts,
      seq_table_mac_cell_group_cfg_s_g1_names,
      5,
      5},
     false},
    {{seq_table_mac_cell_group_cfg_s_g2_fields,
      uint64_t{0x0},
      seq_table_mac_cell_group_cfg_s_g2_opts,
      seq_table_mac_cell_group_cfg_s_g2_names,
      2,
      2},
     false},
    {{seq_table_mac_cell_group_cfg_s_g3_fields,
      uint64_t{0x0},
      seq_table_mac_cell_group_cfg_s_g3_opts,
      seq_table_mac_cell_group_cfg_s_g3_names,
      12,
      12},
     false},
    {{seq_table_mac_cell_group_cfg_s_g4_fields,
      uint64_t{0x0},
      seq_table_mac_cell_group_cfg_s_g4_opts,
      seq_table_mac_cell_group_cfg_s_g4_names,
      2,
      2},
     false},
    {{seq_table_mac_cell_group_cfg_s_g5_fields,
      uint64_t{0x0},
      seq_table_mac_cell_group_cfg_s_g5_opts,
      seq_table_mac_cell_group_cfg_s_g5_names,
      1,
      1},
     false},
    {{seq_table_mac_cell_group_cfg_s_g6_fields,
      uint64_t{0x0},
      seq_table_mac_cell_group_cfg_s_g6_opts,
      seq_table_mac_cell_group_cfg_s_g6_names,
      5,
      5},
     false}};
const asn1::table::seq_desc mac_cell_group_cfg_s::desc = {{seq_table_mac_cell_group_cfg_s_fields,
                                                           uint64_t{0x20},
                                                           seq_table_mac_cell_group_cfg_s_opts,
                                                           seq_table_mac_cell_group_cfg_s_names,
                                                           6,
                                                           5},
                                                          true,
                                                          false,
                                                          seq_table_mac_cell_group_cfg_s_groups,
                                                          7};

// DCP-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_d_cp_cfg_r16_s_fields[] = {
    {offsetof(d_cp_cfg_r16_s, ps_rnti_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(d_cp_cfg_r16_s, ps_offset_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{119},
     nullptr},
    {offsetof(d_cp_cfg_r16_s, size_dci_2_6_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{139},
     nullptr},
    {offsetof(d_cp_cfg_r16_s, ps_position_dci_2_6_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{139},
     nullptr},
    {0,
     (int32_t)offsetof(d_cp_cfg_r16_s, ps_wake_up_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(d_cp_cfg_r16_s, ps_tx_periodic_l1_rsrp_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(d_cp_cfg_r16_s, ps_tx_other_periodic_csi_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_d_cp_cfg_r16_s_names[] = {{"ps-RNTI-r16", nullptr},
                                                                         {"ps-Offset-r16", nullptr},
                                                                         {"sizeDCI-2-6-r16", nullptr},
                                                                         {"ps-PositionDCI-2-6-r16", nullptr},
                                                                         {"ps-WakeUp-r16", "true"},
                                                                         {"ps-TransmitPeriodicL1-RSRP-r16", "true"},
                                                                         {"ps-TransmitOtherPeriodicCSI-r16", "true"}};
static const asn1::table::seq_opt    seq_table_d_cp_cfg_r16_s_opts[]  = {
    {4, (int32_t)offsetof(d_cp_cfg_r16_s, ps_wake_up_r16_present)},
    {5, (int32_t)offsetof(d_cp_cfg_r16_s, ps_tx_periodic_l1_rsrp_r16_present)},
    {6, (int32_t)offsetof(d_cp_cfg_r16_s, ps_tx_other_periodic_csi_r16_present)}};
const asn1::table::seq_desc d_cp_cfg_r16_s::desc = {{seq_table_d_cp_cfg_r16_s_fields,
                                                     uint64_t{0xf},
                                                     seq_table_d_cp_cfg_r16_s_opts,
                                                     seq_table_d_cp_cfg_r16_s_names,
                                                     7,
                                                     3},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0};

static const char*           enum_names_pdsch_harq_ack_codebook_list_r16_item_e__values[] = {"semiStatic", "dynamic"};
const asn1::table::enum_desc pdsch_harq_ack_codebook_list_r16_item_opts::desc             = {
    enum_names_pdsch_harq_ack_codebook_list_r16_item_e__values,
    2,
    "pdsch_harq_ack_codebook_list_r16_item_e_"};

// PDCCH-BlindDetectionCA-CombIndicator-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdcch_blind_detection_ca_comb_ind_r16_s_fields[] = {
    {offsetof(pdcch_blind_detection_ca_comb_ind_r16_s, pdcch_blind_detection_ca1_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(pdcch_blind_detection_ca_comb_ind_r16_s, pdcch_blind_detection_ca2_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{14},
     nullptr}};
static const asn1::table::field_name seq_table_pdcch_blind_detection_ca_comb_ind_r16_s_names[] = {
    {"pdcch-BlindDetectionCA1-r16", nullptr},
    {"pdcch-BlindDetectionCA2-r16", nullptr}};
const asn1::table::seq_desc pdcch_blind_detection_ca_comb_ind_r16_s::desc = {
    {seq_table_pdcch_blind_detection_ca_comb_ind_r16_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_pdcch_blind_detection_ca_comb_ind_r16_s_names,
     2,
     0},
    false,
    false,
    nullptr,
    0};

// PDSCH-HARQ-ACK-EnhType3-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdsch_harq_ack_enh_type3_r17_s_fields[] = {
    {offsetof(pdsch_harq_ack_enh_type3_r17_s, pdsch_harq_ack_enh_type3_idx_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(pdsch_harq_ack_enh_type3_r17_s, applicable_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::desc},
    {0,
     (int32_t)offsetof(pdsch_harq_ack_enh_type3_r17_s, pdsch_harq_ack_enh_type3_ndi_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(pdsch_harq_ack_enh_type3_r17_s, pdsch_harq_ack_enh_type3_cbg_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdsch_harq_ack_enh_type3_r17_s_names[] = {
    {"pdsch-HARQ-ACK-EnhType3Index-r17", nullptr},
    {"applicable-r17", nullptr},
    {"pdsch-HARQ-ACK-EnhType3NDI-r17", "true"},
    {"pdsch-HARQ-ACK-EnhType3CBG-r17", "true"}};
static const asn1::table::seq_opt seq_table_pdsch_harq_ack_enh_type3_r17_s_opts[] = {
    {2, (int32_t)offsetof(pdsch_harq_ack_enh_type3_r17_s, pdsch_harq_ack_enh_type3_ndi_r17_present)},
    {3, (int32_t)offsetof(pdsch_harq_ack_enh_type3_r17_s, pdsch_harq_ack_enh_type3_cbg_r17_present)}};
static const asn1::table::seq_field seq_table_pdsch_harq_ack_enh_type3_r17_s_g0_fields[] = {
    {offsetof(pdsch_harq_ack_enh_type3_r17_s, per_harq_ext_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(pdsch_harq_ack_enh_type3_r17_s::per_harq_ext_r17),
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pdsch_harq_ack_enh_type3_r17_s_g0_names[] = {
    {"perHARQ-Ext-r17", nullptr}};
static const asn1::table::seq_opt seq_table_pdsch_harq_ack_enh_type3_r17_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_pdsch_harq_ack_enh_type3_r17_s_groups[] = {
    {{seq_table_pdsch_harq_ack_enh_type3_r17_s_g0_fields,
      uint64_t{0x0},
      seq_table_pdsch_harq_ack_enh_type3_r17_s_g0_opts,
      seq_table_pdsch_harq_ack_enh_type3_r17_s_g0_names,
      1,
      1},
     false}};
const asn1::table::seq_desc pdsch_harq_ack_enh_type3_r17_s::desc = {{seq_table_pdsch_harq_ack_enh_type3_r17_s_fields,
                                                                     uint64_t{0x3},
                                                                     seq_table_pdsch_harq_ack_enh_type3_r17_s_opts,
                                                                     seq_table_pdsch_harq_ack_enh_type3_r17_s_names,
                                                                     4,
                                                                     2},
                                                                    true,
                                                                    false,
                                                                    seq_table_pdsch_harq_ack_enh_type3_r17_s_groups,
                                                                    1};

static const asn1::table::storage_ops* const
    choice_storage_pdsch_harq_ack_enh_type3_r17_s_applicable_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::per_cc_l_, false>::value,
        &asn1::table::alternative_ops<pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::per_harq_l_, false>::value};
const asn1::table::choice_storage_desc pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::storage_desc = {
    choice_storage_pdsch_harq_ack_enh_type3_r17_s_applicable_r17_c__alternatives,
    2,
    "pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_"};

pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::per_cc_l_&
pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::set_per_cc()
{
  set(types::per_cc);
  return c.get<per_cc_l_>();
}
pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::per_harq_l_&
pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::set_per_harq()
{
  set(types::per_harq);
  return c.get<per_harq_l_>();
}
static const asn1::table::seq_field choice_table_pdsch_harq_ack_enh_type3_r17_s_applicable_r17_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::per_cc_l_,
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{1}, false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::per_harq_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_pdsch_harq_ack_enh_type3_r17_s_applicable_r17_c__names[] = {
    {"perCC", nullptr},
    {"perHARQ", nullptr}};
const asn1::table::choice_desc pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::desc = {
    asn1::table::enum_int_type<pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::types>(),
    pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::types::nof_types,
    pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::types::nof_exts,
    pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::types::has_ext,
    false,
    choice_table_pdsch_harq_ack_enh_type3_r17_s_applicable_r17_c__alternatives,
    nullptr,
    choice_table_pdsch_harq_ack_enh_type3_r17_s_applicable_r17_c__names,
    "pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_",
    offsetof(pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_, c.type),
    offsetof(pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_, c.value),
    false,
    &pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::storage_desc};

static const char* enum_names_pdsch_harq_ack_enh_type3_r17_s_applicable_r17_c__types_values[]    = {"perCC", "perHARQ"};
const asn1::table::enum_desc pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::types_opts::desc = {
    enum_names_pdsch_harq_ack_enh_type3_r17_s_applicable_r17_c__types_values,
    2,
    "pdsch_harq_ack_enh_type3_r17_s::applicable_r17_c_::types"};

// MulticastConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_multicast_cfg_r17_s_pdsch_harq_ack_codebook_list_multicast_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         1,
         &asn1::table::list_codec<
             setup_release_c<dyn_seq_of<pdsch_harq_ack_codebook_list_r16_item_e_, 1, 2>>::value_type,
             asn1::table::string_json>::value}};
static const asn1::table::field_name
    seq_table_multicast_cfg_r17_s_pdsch_harq_ack_codebook_list_multicast_r17_setup_release_names[] = {
        {"release", nullptr},
        {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_multicast_cfg_r17_s_pdsch_harq_ack_codebook_list_multicast_r17_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_multicast_cfg_r17_s_pdsch_harq_ack_codebook_list_multicast_r17_setup_release_alternatives,
        nullptr,
        seq_table_multicast_cfg_r17_s_pdsch_harq_ack_codebook_list_multicast_r17_setup_release_names,
        "setup_release_c",
        setup_release_c<dyn_seq_of<pdsch_harq_ack_codebook_list_r16_item_e_, 1, 2>>::type_offset(),
        setup_release_c<dyn_seq_of<pdsch_harq_ack_codebook_list_r16_item_e_, 1, 2>>::value_offset()};
static const asn1::table::seq_field seq_table_multicast_cfg_r17_s_fields[] = {
    {offsetof(multicast_cfg_r17_s, pdsch_harq_ack_codebook_list_multicast_r17),
     (int32_t)offsetof(multicast_cfg_r17_s, pdsch_harq_ack_codebook_list_multicast_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_multicast_cfg_r17_s_pdsch_harq_ack_codebook_list_multicast_r17_setup_release},
    {offsetof(multicast_cfg_r17_s, type1_codebook_generation_mode_r17),
     (int32_t)offsetof(multicast_cfg_r17_s, type1_codebook_generation_mode_r17_present),
     asn1::table::enum_op<decltype(multicast_cfg_r17_s::type1_codebook_generation_mode_r17)>(),
     decltype(multicast_cfg_r17_s::type1_codebook_generation_mode_r17)::has_ext,
     false,
     decltype(multicast_cfg_r17_s::type1_codebook_generation_mode_r17)::nof_types,
     decltype(multicast_cfg_r17_s::type1_codebook_generation_mode_r17)::nof_exts,
     nullptr,
     &multicast_cfg_r17_s::type1_codebook_generation_mode_r17_opts::desc}};
static const asn1::table::field_name seq_table_multicast_cfg_r17_s_names[] = {
    {"pdsch-HARQ-ACK-CodebookListMulticast-r17", nullptr},
    {"type1CodebookGenerationMode-r17", nullptr}};
static const asn1::table::seq_opt seq_table_multicast_cfg_r17_s_opts[] = {
    {0, (int32_t)offsetof(multicast_cfg_r17_s, pdsch_harq_ack_codebook_list_multicast_r17_present)},
    {1, (int32_t)offsetof(multicast_cfg_r17_s, type1_codebook_generation_mode_r17_present)}};
const asn1::table::seq_desc multicast_cfg_r17_s::desc = {{seq_table_multicast_cfg_r17_s_fields,
                                                          uint64_t{0x0},
                                                          seq_table_multicast_cfg_r17_s_opts,
                                                          seq_table_multicast_cfg_r17_s_names,
                                                          2,
                                                          2},
                                                         false,
                                                         false,
                                                         nullptr,
                                                         0};

static const char* enum_names_multicast_cfg_r17_s_type1_codebook_generation_mode_r17_e__values[] = {"mode1", "mode2"};
const asn1::table::enum_desc multicast_cfg_r17_s::type1_codebook_generation_mode_r17_opts::desc  = {
    enum_names_multicast_cfg_r17_s_type1_codebook_generation_mode_r17_e__values,
    2,
    "multicast_cfg_r17_s::type1_codebook_generation_mode_r17_e_"};
uint8_t multicast_cfg_r17_s::type1_codebook_generation_mode_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "multicast_cfg_r17_s::type1_codebook_generation_mode_r17_e_");
}

// PDCCH-BlindDetectionCA-CombIndicator-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdcch_blind_detection_ca_comb_ind_r17_s_fields[] = {
    {offsetof(pdcch_blind_detection_ca_comb_ind_r17_s, pdcch_blind_detection_ca1_r17),
     (int32_t)offsetof(pdcch_blind_detection_ca_comb_ind_r17_s, pdcch_blind_detection_ca1_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(pdcch_blind_detection_ca_comb_ind_r17_s, pdcch_blind_detection_ca2_r17),
     (int32_t)offsetof(pdcch_blind_detection_ca_comb_ind_r17_s, pdcch_blind_detection_ca2_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{14},
     nullptr},
    {offsetof(pdcch_blind_detection_ca_comb_ind_r17_s, pdcch_blind_detection_ca3_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{14},
     nullptr}};
static const asn1::table::field_name seq_table_pdcch_blind_detection_ca_comb_ind_r17_s_names[] = {
    {"pdcch-BlindDetectionCA1-r17", nullptr},
    {"pdcch-BlindDetectionCA2-r17", nullptr},
    {"pdcch-BlindDetectionCA3-r17", nullptr}};
static const asn1::table::seq_opt seq_table_pdcch_blind_detection_ca_comb_ind_r17_s_opts[] = {
    {0, (int32_t)offsetof(pdcch_blind_detection_ca_comb_ind_r17_s, pdcch_blind_detection_ca1_r17_present)},
    {1, (int32_t)offsetof(pdcch_blind_detection_ca_comb_ind_r17_s, pdcch_blind_detection_ca2_r17_present)}};
const asn1::table::seq_desc pdcch_blind_detection_ca_comb_ind_r17_s::desc = {
    {seq_table_pdcch_blind_detection_ca_comb_ind_r17_s_fields,
     uint64_t{0x4},
     seq_table_pdcch_blind_detection_ca_comb_ind_r17_s_opts,
     seq_table_pdcch_blind_detection_ca_comb_ind_r17_s_names,
     3,
     2},
    false,
    false,
    nullptr,
    0};

// PhysicalCellGroupConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_cs_rnti_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_cs_rnti_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_phys_cell_group_cfg_s_cs_rnti_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_phys_cell_group_cfg_s_cs_rnti_setup_release_alternatives,
    nullptr,
    seq_table_phys_cell_group_cfg_s_cs_rnti_setup_release_names,
    "setup_release_c",
    setup_release_c<integer<uint32_t, 0, 65535>>::type_offset(),
    setup_release_c<integer<uint32_t, 0, 65535>>::value_offset()};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_setup_release_alternatives[] =
    {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::int_u8,
      false,
      false,
      int64_t{1},
      uint64_t{14},
      nullptr}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_setup_release_alternatives,
    nullptr,
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_setup_release_names,
    "setup_release_c",
    setup_release_c<integer<uint8_t, 1, 15>>::type_offset(),
    setup_release_c<integer<uint8_t, 1, 15>>::value_offset()};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_dcp_cfg_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &d_cp_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_dcp_cfg_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_phys_cell_group_cfg_s_dcp_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_phys_cell_group_cfg_s_dcp_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_phys_cell_group_cfg_s_dcp_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<d_cp_cfg_r16_s>::type_offset(),
    setup_release_c<d_cp_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_phys_cell_group_cfg_s_pdsch_harq_ack_codebook_list_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         1,
         &asn1::table::list_codec<
             setup_release_c<dyn_seq_of<pdsch_harq_ack_codebook_list_r16_item_e_, 1, 2>>::value_type,
             asn1::table::string_json>::value}};
static const asn1::table::field_name
    seq_table_phys_cell_group_cfg_s_pdsch_harq_ack_codebook_list_r16_setup_release_names[] = {{"release", nullptr},
                                                                                              {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_phys_cell_group_cfg_s_pdsch_harq_ack_codebook_list_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_phys_cell_group_cfg_s_pdsch_harq_ack_codebook_list_r16_setup_release_alternatives,
    nullptr,
    seq_table_phys_cell_group_cfg_s_pdsch_harq_ack_codebook_list_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<dyn_seq_of<pdsch_harq_ack_codebook_list_r16_item_e_, 1, 2>>::type_offset(),
    setup_release_c<dyn_seq_of<pdsch_harq_ack_codebook_list_r16_item_e_, 1, 2>>::value_offset()};
static const asn1::table::seq_field
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &pdcch_blind_detection_ca_comb_ind_r16_s::desc}};
static const asn1::table::field_name
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r16_setup_release_names[] = {{"release", nullptr},
                                                                                                   {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r16_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r16_setup_release_alternatives,
        nullptr,
        seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r16_setup_release_names,
        "setup_release_c",
        setup_release_c<pdcch_blind_detection_ca_comb_ind_r16_s>::type_offset(),
        setup_release_c<pdcch_blind_detection_ca_comb_ind_r16_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection2_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{1},
         uint64_t{14},
         nullptr}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_pdcch_blind_detection2_r16_setup_release_names[] =
    {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_phys_cell_group_cfg_s_pdcch_blind_detection2_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection2_r16_setup_release_alternatives,
    nullptr,
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection2_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<integer<uint8_t, 1, 15>>::type_offset(),
    setup_release_c<integer<uint8_t, 1, 15>>::value_offset()};
static const asn1::table::seq_field
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection3_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{1},
         uint64_t{14},
         nullptr}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_pdcch_blind_detection3_r16_setup_release_names[] =
    {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_phys_cell_group_cfg_s_pdcch_blind_detection3_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection3_r16_setup_release_alternatives,
    nullptr,
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection3_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<integer<uint8_t, 1, 15>>::type_offset(),
    setup_release_c<integer<uint8_t, 1, 15>>::value_offset()};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_multicast_cfg_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &multicast_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_multicast_cfg_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_phys_cell_group_cfg_s_multicast_cfg_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_phys_cell_group_cfg_s_multicast_cfg_r17_setup_release_alternatives,
    nullptr,
    seq_table_phys_cell_group_cfg_s_multicast_cfg_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<multicast_cfg_r17_s>::type_offset(),
    setup_release_c<multicast_cfg_r17_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &pdcch_blind_detection_ca_comb_ind_r17_s::desc}};
static const asn1::table::field_name
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r17_setup_release_names[] = {{"release", nullptr},
                                                                                                   {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r17_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r17_setup_release_alternatives,
        nullptr,
        seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r17_setup_release_names,
        "setup_release_c",
        setup_release_c<pdcch_blind_detection_ca_comb_ind_r17_s>::type_offset(),
        setup_release_c<pdcch_blind_detection_ca_comb_ind_r17_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection4_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{1},
         uint64_t{14},
         nullptr}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_pdcch_blind_detection4_r17_setup_release_names[] =
    {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_phys_cell_group_cfg_s_pdcch_blind_detection4_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection4_r17_setup_release_alternatives,
    nullptr,
    seq_table_phys_cell_group_cfg_s_pdcch_blind_detection4_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<integer<uint8_t, 1, 15>>::type_offset(),
    setup_release_c<integer<uint8_t, 1, 15>>::value_offset()};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_cell_dtrx_dci_cfg_r18_setup_release_alternatives[] =
    {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &cell_dtrx_dci_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_cell_dtrx_dci_cfg_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_phys_cell_group_cfg_s_cell_dtrx_dci_cfg_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_phys_cell_group_cfg_s_cell_dtrx_dci_cfg_r18_setup_release_alternatives,
    nullptr,
    seq_table_phys_cell_group_cfg_s_cell_dtrx_dci_cfg_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<cell_dtrx_dci_cfg_r18_s>::type_offset(),
    setup_release_c<cell_dtrx_dci_cfg_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_fields[] = {
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, harq_ack_spatial_bundling_pucch_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, harq_ack_spatial_bundling_pusch_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(phys_cell_group_cfg_s, p_nr_fr1),
     (int32_t)offsetof(phys_cell_group_cfg_s, p_nr_fr1_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{63},
     nullptr},
    {offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_codebook),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(phys_cell_group_cfg_s::pdsch_harq_ack_codebook)>(),
     decltype(phys_cell_group_cfg_s::pdsch_harq_ack_codebook)::has_ext,
     false,
     decltype(phys_cell_group_cfg_s::pdsch_harq_ack_codebook)::nof_types,
     decltype(phys_cell_group_cfg_s::pdsch_harq_ack_codebook)::nof_exts,
     nullptr,
     &phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::desc},
    {offsetof(phys_cell_group_cfg_s, tpc_srs_rnti),
     (int32_t)offsetof(phys_cell_group_cfg_s, tpc_srs_rnti_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(phys_cell_group_cfg_s, tpc_pucch_rnti),
     (int32_t)offsetof(phys_cell_group_cfg_s, tpc_pucch_rnti_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(phys_cell_group_cfg_s, tpc_pusch_rnti),
     (int32_t)offsetof(phys_cell_group_cfg_s, tpc_pusch_rnti_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(phys_cell_group_cfg_s, sp_csi_rnti),
     (int32_t)offsetof(phys_cell_group_cfg_s, sp_csi_rnti_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(phys_cell_group_cfg_s, cs_rnti),
     (int32_t)offsetof(phys_cell_group_cfg_s, cs_rnti_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_phys_cell_group_cfg_s_cs_rnti_setup_release}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_names[] = {
    {"harq-ACK-SpatialBundlingPUCCH", "true"},
    {"harq-ACK-SpatialBundlingPUSCH", "true"},
    {"p-NR-FR1", nullptr},
    {"pdsch-HARQ-ACK-Codebook", nullptr},
    {"tpc-SRS-RNTI", nullptr},
    {"tpc-PUCCH-RNTI", nullptr},
    {"tpc-PUSCH-RNTI", nullptr},
    {"sp-CSI-RNTI", nullptr},
    {"cs-RNTI", nullptr}};
static const asn1::table::seq_opt seq_table_phys_cell_group_cfg_s_opts[] = {
    {0, (int32_t)offsetof(phys_cell_group_cfg_s, harq_ack_spatial_bundling_pucch_present)},
    {1, (int32_t)offsetof(phys_cell_group_cfg_s, harq_ack_spatial_bundling_pusch_present)},
    {2, (int32_t)offsetof(phys_cell_group_cfg_s, p_nr_fr1_present)},
    {4, (int32_t)offsetof(phys_cell_group_cfg_s, tpc_srs_rnti_present)},
    {5, (int32_t)offsetof(phys_cell_group_cfg_s, tpc_pucch_rnti_present)},
    {6, (int32_t)offsetof(phys_cell_group_cfg_s, tpc_pusch_rnti_present)},
    {7, (int32_t)offsetof(phys_cell_group_cfg_s, sp_csi_rnti_present)},
    {8, (int32_t)offsetof(phys_cell_group_cfg_s, cs_rnti_present)}};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_g0_fields[] = {
    {offsetof(phys_cell_group_cfg_s, mcs_c_rnti),
     (int32_t)offsetof(phys_cell_group_cfg_s, mcs_c_rnti_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(phys_cell_group_cfg_s, p_ue_fr1),
     (int32_t)offsetof(phys_cell_group_cfg_s, p_ue_fr1_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{63},
     nullptr}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_g0_names[] = {{"mcs-C-RNTI", nullptr},
                                                                                   {"p-UE-FR1", nullptr}};
static const asn1::table::seq_opt    seq_table_phys_cell_group_cfg_s_g0_opts[]  = {
    {0, (int32_t)offsetof(phys_cell_group_cfg_s, mcs_c_rnti_present)},
    {1, (int32_t)offsetof(phys_cell_group_cfg_s, p_ue_fr1_present)}};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_g1_fields[] = {
    {offsetof(phys_cell_group_cfg_s, xscale),
     (int32_t)offsetof(phys_cell_group_cfg_s, xscale_present),
     asn1::table::enum_op<decltype(phys_cell_group_cfg_s::xscale)>(),
     decltype(phys_cell_group_cfg_s::xscale)::has_ext,
     false,
     decltype(phys_cell_group_cfg_s::xscale)::nof_types,
     decltype(phys_cell_group_cfg_s::xscale)::nof_exts,
     nullptr,
     &phys_cell_group_cfg_s::xscale_opts::desc}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_g1_names[] = {{"xScale", nullptr}};
static const asn1::table::seq_opt    seq_table_phys_cell_group_cfg_s_g1_opts[]  = {
    {0, (int32_t)offsetof(phys_cell_group_cfg_s, xscale_present)}};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_g2_fields[] = {
    {offsetof(phys_cell_group_cfg_s, pdcch_blind_detection),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(phys_cell_group_cfg_s::pdcch_blind_detection)>::value,
     &seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_setup_release}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_g2_names[] = {{"pdcch-BlindDetection", nullptr}};
static const asn1::table::seq_opt    seq_table_phys_cell_group_cfg_s_g2_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_g3_fields[] = {
    {offsetof(phys_cell_group_cfg_s, dcp_cfg_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(phys_cell_group_cfg_s::dcp_cfg_r16)>::value,
     &seq_table_phys_cell_group_cfg_s_dcp_cfg_r16_setup_release},
    {offsetof(phys_cell_group_cfg_s, harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16),
     (int32_t)offsetof(phys_cell_group_cfg_s, harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_present),
     asn1::table::enum_op<decltype(phys_cell_group_cfg_s::harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16)>(),
     decltype(phys_cell_group_cfg_s::harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16)::has_ext,
     false,
     decltype(phys_cell_group_cfg_s::harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16)::nof_types,
     decltype(phys_cell_group_cfg_s::harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16)::nof_exts,
     nullptr,
     &phys_cell_group_cfg_s::harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_opts::desc},
    {offsetof(phys_cell_group_cfg_s, harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16),
     (int32_t)offsetof(phys_cell_group_cfg_s, harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_present),
     asn1::table::enum_op<decltype(phys_cell_group_cfg_s::harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16)>(),
     decltype(phys_cell_group_cfg_s::harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16)::has_ext,
     false,
     decltype(phys_cell_group_cfg_s::harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16)::nof_types,
     decltype(phys_cell_group_cfg_s::harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16)::nof_exts,
     nullptr,
     &phys_cell_group_cfg_s::harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_opts::desc},
    {offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_codebook_secondary_pucch_group_r16),
     (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_codebook_secondary_pucch_group_r16_present),
     asn1::table::enum_op<decltype(phys_cell_group_cfg_s::pdsch_harq_ack_codebook_secondary_pucch_group_r16)>(),
     decltype(phys_cell_group_cfg_s::pdsch_harq_ack_codebook_secondary_pucch_group_r16)::has_ext,
     false,
     decltype(phys_cell_group_cfg_s::pdsch_harq_ack_codebook_secondary_pucch_group_r16)::nof_types,
     decltype(phys_cell_group_cfg_s::pdsch_harq_ack_codebook_secondary_pucch_group_r16)::nof_exts,
     nullptr,
     &phys_cell_group_cfg_s::pdsch_harq_ack_codebook_secondary_pucch_group_r16_opts::desc},
    {offsetof(phys_cell_group_cfg_s, p_nr_fr2_r16),
     (int32_t)offsetof(phys_cell_group_cfg_s, p_nr_fr2_r16_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{63},
     nullptr},
    {offsetof(phys_cell_group_cfg_s, p_ue_fr2_r16),
     (int32_t)offsetof(phys_cell_group_cfg_s, p_ue_fr2_r16_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{63},
     nullptr},
    {offsetof(phys_cell_group_cfg_s, nrdc_p_cmode_fr1_r16),
     (int32_t)offsetof(phys_cell_group_cfg_s, nrdc_p_cmode_fr1_r16_present),
     asn1::table::enum_op<decltype(phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16)>(),
     decltype(phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16)::has_ext,
     false,
     decltype(phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16)::nof_types,
     decltype(phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16)::nof_exts,
     nullptr,
     &phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16_opts::desc},
    {offsetof(phys_cell_group_cfg_s, nrdc_p_cmode_fr2_r16),
     (int32_t)offsetof(phys_cell_group_cfg_s, nrdc_p_cmode_fr2_r16_present),
     asn1::table::enum_op<decltype(phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16)>(),
     decltype(phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16)::has_ext,
     false,
     decltype(phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16)::nof_types,
     decltype(phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16)::nof_exts,
     nullptr,
     &phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16_opts::desc},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_codebook_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, nfi_total_dai_included_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, ul_total_dai_included_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_one_shot_feedback_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_one_shot_feedback_ndi_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_one_shot_feedback_cbg_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, dl_assign_idx_dci_0_2_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(phys_cell_group_cfg_s, dl_assign_idx_dci_1_2_r16),
     (int32_t)offsetof(phys_cell_group_cfg_s, dl_assign_idx_dci_1_2_r16_present),
     asn1::table::enum_op<decltype(phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16)>(),
     decltype(phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16)::has_ext,
     false,
     decltype(phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16)::nof_types,
     decltype(phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16)::nof_exts,
     nullptr,
     &phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16_opts::desc},
    {offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_codebook_list_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(phys_cell_group_cfg_s::pdsch_harq_ack_codebook_list_r16)>::value,
     &seq_table_phys_cell_group_cfg_s_pdsch_harq_ack_codebook_list_r16_setup_release},
    {offsetof(phys_cell_group_cfg_s, ack_nack_feedback_mode_r16),
     (int32_t)offsetof(phys_cell_group_cfg_s, ack_nack_feedback_mode_r16_present),
     asn1::table::enum_op<decltype(phys_cell_group_cfg_s::ack_nack_feedback_mode_r16)>(),
     decltype(phys_cell_group_cfg_s::ack_nack_feedback_mode_r16)::has_ext,
     false,
     decltype(phys_cell_group_cfg_s::ack_nack_feedback_mode_r16)::nof_types,
     decltype(phys_cell_group_cfg_s::ack_nack_feedback_mode_r16)::nof_exts,
     nullptr,
     &phys_cell_group_cfg_s::ack_nack_feedback_mode_r16_opts::desc},
    {offsetof(phys_cell_group_cfg_s, pdcch_blind_detection_ca_comb_ind_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(phys_cell_group_cfg_s::pdcch_blind_detection_ca_comb_ind_r16)>::value,
     &seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r16_setup_release},
    {offsetof(phys_cell_group_cfg_s, pdcch_blind_detection2_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(phys_cell_group_cfg_s::pdcch_blind_detection2_r16)>::value,
     &seq_table_phys_cell_group_cfg_s_pdcch_blind_detection2_r16_setup_release},
    {offsetof(phys_cell_group_cfg_s, pdcch_blind_detection3_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(phys_cell_group_cfg_s::pdcch_blind_detection3_r16)>::value,
     &seq_table_phys_cell_group_cfg_s_pdcch_blind_detection3_r16_setup_release},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, bd_factor_r_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_g3_names[] = {
    {"dcp-Config-r16", nullptr},
    {"harq-ACK-SpatialBundlingPUCCH-secondaryPUCCHgroup-r16", nullptr},
    {"harq-ACK-SpatialBundlingPUSCH-secondaryPUCCHgroup-r16", nullptr},
    {"pdsch-HARQ-ACK-Codebook-secondaryPUCCHgroup-r16", nullptr},
    {"p-NR-FR2-r16", nullptr},
    {"p-UE-FR2-r16", nullptr},
    {"nrdc-PCmode-FR1-r16", nullptr},
    {"nrdc-PCmode-FR2-r16", nullptr},
    {"pdsch-HARQ-ACK-Codebook-r16", "enhancedDynamic"},
    {"nfi-TotalDAI-Included-r16", "true"},
    {"ul-TotalDAI-Included-r16", "true"},
    {"pdsch-HARQ-ACK-OneShotFeedback-r16", "true"},
    {"pdsch-HARQ-ACK-OneShotFeedbackNDI-r16", "true"},
    {"pdsch-HARQ-ACK-OneShotFeedbackCBG-r16", "true"},
    {"downlinkAssignmentIndexDCI-0-2-r16", "enabled"},
    {"downlinkAssignmentIndexDCI-1-2-r16", nullptr},
    {"pdsch-HARQ-ACK-CodebookList-r16", nullptr},
    {"ackNackFeedbackMode-r16", nullptr},
    {"pdcch-BlindDetectionCA-CombIndicator-r16", nullptr},
    {"pdcch-BlindDetection2-r16", nullptr},
    {"pdcch-BlindDetection3-r16", nullptr},
    {"bdFactorR-r16", "n1"}};
static const asn1::table::seq_opt seq_table_phys_cell_group_cfg_s_g3_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(phys_cell_group_cfg_s, harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_present)},
    {2, (int32_t)offsetof(phys_cell_group_cfg_s, harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_present)},
    {3, (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_codebook_secondary_pucch_group_r16_present)},
    {4, (int32_t)offsetof(phys_cell_group_cfg_s, p_nr_fr2_r16_present)},
    {5, (int32_t)offsetof(phys_cell_group_cfg_s, p_ue_fr2_r16_present)},
    {6, (int32_t)offsetof(phys_cell_group_cfg_s, nrdc_p_cmode_fr1_r16_present)},
    {7, (int32_t)offsetof(phys_cell_group_cfg_s, nrdc_p_cmode_fr2_r16_present)},
    {8, (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_codebook_r16_present)},
    {9, (int32_t)offsetof(phys_cell_group_cfg_s, nfi_total_dai_included_r16_present)},
    {10, (int32_t)offsetof(phys_cell_group_cfg_s, ul_total_dai_included_r16_present)},
    {11, (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_one_shot_feedback_r16_present)},
    {12, (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_one_shot_feedback_ndi_r16_present)},
    {13, (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_one_shot_feedback_cbg_r16_present)},
    {14, (int32_t)offsetof(phys_cell_group_cfg_s, dl_assign_idx_dci_0_2_r16_present)},
    {15, (int32_t)offsetof(phys_cell_group_cfg_s, dl_assign_idx_dci_1_2_r16_present)},
    {16, asn1::table::seq_field::ptr_presence},
    {17, (int32_t)offsetof(phys_cell_group_cfg_s, ack_nack_feedback_mode_r16_present)},
    {18, asn1::table::seq_field::ptr_presence},
    {19, asn1::table::seq_field::ptr_presence},
    {20, asn1::table::seq_field::ptr_presence},
    {21, (int32_t)offsetof(phys_cell_group_cfg_s, bd_factor_r_r16_present)}};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_g4_fields[] = {
    {offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_enh_type3_to_add_mod_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_item_list_codec<decltype(phys_cell_group_cfg_s::pdsch_harq_ack_enh_type3_to_add_mod_list_r17),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &pdsch_harq_ack_enh_type3_r17_s::desc},
    {offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_enh_type3_to_release_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_list_codec<decltype(phys_cell_group_cfg_s::pdsch_harq_ack_enh_type3_to_release_list_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{7}, false, false>>::value},
    {offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_item_list_codec<
         decltype(phys_cell_group_cfg_s::pdsch_harq_ack_enh_type3_secondary_to_add_mod_list_r17),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &pdsch_harq_ack_enh_type3_r17_s::desc},
    {offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_enh_type3_secondary_to_release_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_list_codec<
         decltype(phys_cell_group_cfg_s::pdsch_harq_ack_enh_type3_secondary_to_release_list_r17),
         asn1::table::int_json,
         asn1::table::int_items<false, int64_t{0}, uint64_t{7}, false, false>>::value},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_enh_type3_dci_field_secondary_pucch_group_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_enh_type3_dci_field_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_retx_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_retx_secondary_pucch_group_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(phys_cell_group_cfg_s, pucch_s_scell_r17),
     (int32_t)offsetof(phys_cell_group_cfg_s, pucch_s_scell_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{30},
     nullptr},
    {offsetof(phys_cell_group_cfg_s, pucch_s_scell_secondary_pucch_group_r17),
     (int32_t)offsetof(phys_cell_group_cfg_s, pucch_s_scell_secondary_pucch_group_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{30},
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, pucch_s_scell_dyn_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, pucch_s_scell_dyn_secondary_pucch_group_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(phys_cell_group_cfg_s, pucch_s_scell_pattern_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     319,
     &asn1::table::ptr_list_codec<decltype(phys_cell_group_cfg_s::pucch_s_scell_pattern_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{1}, false, false>>::value},
    {offsetof(phys_cell_group_cfg_s, pucch_s_scell_pattern_secondary_pucch_group_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     319,
     &asn1::table::ptr_list_codec<decltype(phys_cell_group_cfg_s::pucch_s_scell_pattern_secondary_pucch_group_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{1}, false, false>>::value},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, uci_mux_with_diff_prio_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, uci_mux_with_diff_prio_secondary_pucch_group_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, simul_pucch_pusch_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, simul_pucch_pusch_secondary_pucch_group_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, prio_low_dg_high_cg_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, prio_high_dg_low_cg_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, two_qcl_type_d_for_pdcch_repeat_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(phys_cell_group_cfg_s, multicast_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(phys_cell_group_cfg_s::multicast_cfg_r17)>::value,
     &seq_table_phys_cell_group_cfg_s_multicast_cfg_r17_setup_release},
    {offsetof(phys_cell_group_cfg_s, pdcch_blind_detection_ca_comb_ind_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(phys_cell_group_cfg_s::pdcch_blind_detection_ca_comb_ind_r17)>::value,
     &seq_table_phys_cell_group_cfg_s_pdcch_blind_detection_ca_comb_ind_r17_setup_release}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_g4_names[] = {
    {"pdsch-HARQ-ACK-EnhType3ToAddModList-r17", nullptr},
    {"pdsch-HARQ-ACK-EnhType3ToReleaseList-r17", nullptr},
    {"pdsch-HARQ-ACK-EnhType3SecondaryToAddModList-r17", nullptr},
    {"pdsch-HARQ-ACK-EnhType3SecondaryToReleaseList-r17", nullptr},
    {"pdsch-HARQ-ACK-EnhType3DCI-FieldSecondaryPUCCHgroup-r17", "enabled"},
    {"pdsch-HARQ-ACK-EnhType3DCI-Field-r17", "enabled"},
    {"pdsch-HARQ-ACK-Retx-r17", "enabled"},
    {"pdsch-HARQ-ACK-RetxSecondaryPUCCHgroup-r17", "enabled"},
    {"pucch-sSCell-r17", nullptr},
    {"pucch-sSCellSecondaryPUCCHgroup-r17", nullptr},
    {"pucch-sSCellDyn-r17", "enabled"},
    {"pucch-sSCellDynSecondaryPUCCHgroup-r17", "enabled"},
    {"pucch-sSCellPattern-r17", nullptr},
    {"pucch-sSCellPatternSecondaryPUCCHgroup-r17", nullptr},
    {"uci-MuxWithDiffPrio-r17", "enabled"},
    {"uci-MuxWithDiffPrioSecondaryPUCCHgroup-r17", "enabled"},
    {"simultaneousPUCCH-PUSCH-r17", "enabled"},
    {"simultaneousPUCCH-PUSCH-SecondaryPUCCHgroup-r17", "enabled"},
    {"prioLowDG-HighCG-r17", "enabled"},
    {"prioHighDG-LowCG-r17", "enabled"},
    {"twoQCLTypeDforPDCCHRepetition-r17", "enabled"},
    {"multicastConfig-r17", nullptr},
    {"pdcch-BlindDetectionCA-CombIndicator-r17", nullptr}};
static const asn1::table::seq_opt seq_table_phys_cell_group_cfg_s_g4_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, asn1::table::seq_field::ptr_presence},
    {4, (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_enh_type3_dci_field_secondary_pucch_group_r17_present)},
    {5, (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_enh_type3_dci_field_r17_present)},
    {6, (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_retx_r17_present)},
    {7, (int32_t)offsetof(phys_cell_group_cfg_s, pdsch_harq_ack_retx_secondary_pucch_group_r17_present)},
    {8, (int32_t)offsetof(phys_cell_group_cfg_s, pucch_s_scell_r17_present)},
    {9, (int32_t)offsetof(phys_cell_group_cfg_s, pucch_s_scell_secondary_pucch_group_r17_present)},
    {10, (int32_t)offsetof(phys_cell_group_cfg_s, pucch_s_scell_dyn_r17_present)},
    {11, (int32_t)offsetof(phys_cell_group_cfg_s, pucch_s_scell_dyn_secondary_pucch_group_r17_present)},
    {12, asn1::table::seq_field::ptr_presence},
    {13, asn1::table::seq_field::ptr_presence},
    {14, (int32_t)offsetof(phys_cell_group_cfg_s, uci_mux_with_diff_prio_r17_present)},
    {15, (int32_t)offsetof(phys_cell_group_cfg_s, uci_mux_with_diff_prio_secondary_pucch_group_r17_present)},
    {16, (int32_t)offsetof(phys_cell_group_cfg_s, simul_pucch_pusch_r17_present)},
    {17, (int32_t)offsetof(phys_cell_group_cfg_s, simul_pucch_pusch_secondary_pucch_group_r17_present)},
    {18, (int32_t)offsetof(phys_cell_group_cfg_s, prio_low_dg_high_cg_r17_present)},
    {19, (int32_t)offsetof(phys_cell_group_cfg_s, prio_high_dg_low_cg_r17_present)},
    {20, (int32_t)offsetof(phys_cell_group_cfg_s, two_qcl_type_d_for_pdcch_repeat_r17_present)},
    {21, asn1::table::seq_field::ptr_presence},
    {22, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_g5_fields[] = {
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, simul_sr_pusch_diff_pucch_groups_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_g5_names[] = {
    {"simultaneousSR-PUSCH-diffPUCCH-Groups-r17", "enabled"}};
static const asn1::table::seq_opt seq_table_phys_cell_group_cfg_s_g5_opts[] = {
    {0, (int32_t)offsetof(phys_cell_group_cfg_s, simul_sr_pusch_diff_pucch_groups_r17_present)}};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_g6_fields[] = {
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, intra_band_nc_prach_simul_tx_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_g6_names[] = {
    {"intraBandNC-PRACH-simulTx-r17", "enabled"}};
static const asn1::table::seq_opt seq_table_phys_cell_group_cfg_s_g6_opts[] = {
    {0, (int32_t)offsetof(phys_cell_group_cfg_s, intra_band_nc_prach_simul_tx_r17_present)}};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_g7_fields[] = {
    {offsetof(phys_cell_group_cfg_s, pdcch_blind_detection4_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(phys_cell_group_cfg_s::pdcch_blind_detection4_r17)>::value,
     &seq_table_phys_cell_group_cfg_s_pdcch_blind_detection4_r17_setup_release}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_g7_names[] = {
    {"pdcch-BlindDetection4-r17", nullptr}};
static const asn1::table::seq_opt seq_table_phys_cell_group_cfg_s_g7_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_g8_fields[] = {
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, simul_pucch_pusch_same_prio_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, simul_pucch_pusch_same_prio_secondary_pucch_group_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_g8_names[] = {
    {"simultaneousPUCCH-PUSCH-SamePriority-r17", "enabled"},
    {"simultaneousPUCCH-PUSCH-SamePriority-SecondaryPUCCHgroup-r17", "enabled"}};
static const asn1::table::seq_opt seq_table_phys_cell_group_cfg_s_g8_opts[] = {
    {0, (int32_t)offsetof(phys_cell_group_cfg_s, simul_pucch_pusch_same_prio_r17_present)},
    {1, (int32_t)offsetof(phys_cell_group_cfg_s, simul_pucch_pusch_same_prio_secondary_pucch_group_r17_present)}};
static const asn1::table::seq_field seq_table_phys_cell_group_cfg_s_g9_fields[] = {
    {offsetof(phys_cell_group_cfg_s, ncr_rnti_r18),
     (int32_t)offsetof(phys_cell_group_cfg_s, ncr_rnti_r18_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(phys_cell_group_cfg_s, cell_dtrx_dci_cfg_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(phys_cell_group_cfg_s::cell_dtrx_dci_cfg_r18)>::value,
     &seq_table_phys_cell_group_cfg_s_cell_dtrx_dci_cfg_r18_setup_release},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, two_qcl_type_d_for_multi_dci_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, enable_type1_harq_ack_mux_for_dl_assign_after_ul_grant_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, enable_type2_harq_ack_mux_for_dl_assign_after_ul_grant_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, enable_type3_harq_ack_mux_for_dl_assign_after_ul_grant_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, enable_diff_pucch_res_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(phys_cell_group_cfg_s, enable_diff_cb_size_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_phys_cell_group_cfg_s_g9_names[] = {
    {"ncr-RNTI-r18", nullptr},
    {"cellDTRX-DCI-config-r18", nullptr},
    {"twoQCL-TypeD-ForMultiDCI-r18", "enabled"},
    {"enableType1HARQ-ACK-MuxForDL-AssignmentAfterUL-Grant-r18", "enabled"},
    {"enableType2HARQ-ACK-MuxForDL-AssignmentAfterUL-Grant-r18", "enabled"},
    {"enableType3HARQ-ACK-MuxForDL-AssignmentAfterUL-Grant-r18", "enabled"},
    {"enableDiffPUCCH-Resource-r18", "enabled"},
    {"enableDiffCB-Size-r18", "enabled"}};
static const asn1::table::seq_opt seq_table_phys_cell_group_cfg_s_g9_opts[] = {
    {0, (int32_t)offsetof(phys_cell_group_cfg_s, ncr_rnti_r18_present)},
    {1, asn1::table::seq_field::ptr_presence},
    {2, (int32_t)offsetof(phys_cell_group_cfg_s, two_qcl_type_d_for_multi_dci_r18_present)},
    {3, (int32_t)offsetof(phys_cell_group_cfg_s, enable_type1_harq_ack_mux_for_dl_assign_after_ul_grant_r18_present)},
    {4, (int32_t)offsetof(phys_cell_group_cfg_s, enable_type2_harq_ack_mux_for_dl_assign_after_ul_grant_r18_present)},
    {5, (int32_t)offsetof(phys_cell_group_cfg_s, enable_type3_harq_ack_mux_for_dl_assign_after_ul_grant_r18_present)},
    {6, (int32_t)offsetof(phys_cell_group_cfg_s, enable_diff_pucch_res_r18_present)},
    {7, (int32_t)offsetof(phys_cell_group_cfg_s, enable_diff_cb_size_r18_present)}};
static const asn1::table::seq_group seq_table_phys_cell_group_cfg_s_groups[] = {
    {{seq_table_phys_cell_group_cfg_s_g0_fields,
      uint64_t{0x0},
      seq_table_phys_cell_group_cfg_s_g0_opts,
      seq_table_phys_cell_group_cfg_s_g0_names,
      2,
      2},
     false},
    {{seq_table_phys_cell_group_cfg_s_g1_fields,
      uint64_t{0x0},
      seq_table_phys_cell_group_cfg_s_g1_opts,
      seq_table_phys_cell_group_cfg_s_g1_names,
      1,
      1},
     false},
    {{seq_table_phys_cell_group_cfg_s_g2_fields,
      uint64_t{0x0},
      seq_table_phys_cell_group_cfg_s_g2_opts,
      seq_table_phys_cell_group_cfg_s_g2_names,
      1,
      1},
     false},
    {{seq_table_phys_cell_group_cfg_s_g3_fields,
      uint64_t{0x0},
      seq_table_phys_cell_group_cfg_s_g3_opts,
      seq_table_phys_cell_group_cfg_s_g3_names,
      22,
      22},
     false},
    {{seq_table_phys_cell_group_cfg_s_g4_fields,
      uint64_t{0x0},
      seq_table_phys_cell_group_cfg_s_g4_opts,
      seq_table_phys_cell_group_cfg_s_g4_names,
      23,
      23},
     false},
    {{seq_table_phys_cell_group_cfg_s_g5_fields,
      uint64_t{0x0},
      seq_table_phys_cell_group_cfg_s_g5_opts,
      seq_table_phys_cell_group_cfg_s_g5_names,
      1,
      1},
     false},
    {{seq_table_phys_cell_group_cfg_s_g6_fields,
      uint64_t{0x0},
      seq_table_phys_cell_group_cfg_s_g6_opts,
      seq_table_phys_cell_group_cfg_s_g6_names,
      1,
      1},
     false},
    {{seq_table_phys_cell_group_cfg_s_g7_fields,
      uint64_t{0x0},
      seq_table_phys_cell_group_cfg_s_g7_opts,
      seq_table_phys_cell_group_cfg_s_g7_names,
      1,
      1},
     false},
    {{seq_table_phys_cell_group_cfg_s_g8_fields,
      uint64_t{0x0},
      seq_table_phys_cell_group_cfg_s_g8_opts,
      seq_table_phys_cell_group_cfg_s_g8_names,
      2,
      2},
     false},
    {{seq_table_phys_cell_group_cfg_s_g9_fields,
      uint64_t{0x0},
      seq_table_phys_cell_group_cfg_s_g9_opts,
      seq_table_phys_cell_group_cfg_s_g9_names,
      8,
      8},
     false}};
const asn1::table::seq_desc phys_cell_group_cfg_s::desc = {{seq_table_phys_cell_group_cfg_s_fields,
                                                            uint64_t{0x8},
                                                            seq_table_phys_cell_group_cfg_s_opts,
                                                            seq_table_phys_cell_group_cfg_s_names,
                                                            9,
                                                            8},
                                                           true,
                                                           false,
                                                           seq_table_phys_cell_group_cfg_s_groups,
                                                           10};

static const char* enum_names_phys_cell_group_cfg_s_pdsch_harq_ack_codebook_e__values[] = {"semiStatic", "dynamic"};
const asn1::table::enum_desc phys_cell_group_cfg_s::pdsch_harq_ack_codebook_opts::desc  = {
    enum_names_phys_cell_group_cfg_s_pdsch_harq_ack_codebook_e__values,
    2,
    "phys_cell_group_cfg_s::pdsch_harq_ack_codebook_e_"};

static const char*           enum_names_phys_cell_group_cfg_s_xscale_e__values[] = {"dB0", "dB6", "spare2", "spare1"};
const asn1::table::enum_desc phys_cell_group_cfg_s::xscale_opts::desc            = {
    enum_names_phys_cell_group_cfg_s_xscale_e__values,
    4,
    "phys_cell_group_cfg_s::xscale_e_"};
uint8_t phys_cell_group_cfg_s::xscale_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 6};
  return map_enum_number(numbers, 2, value, "phys_cell_group_cfg_s::xscale_e_");
}

static const char*
    enum_names_phys_cell_group_cfg_s_harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_e__values[] = {
        "enabled",
        "disabled"};
const asn1::table::enum_desc
    phys_cell_group_cfg_s::harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_opts::desc = {
        enum_names_phys_cell_group_cfg_s_harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_e__values,
        2,
        "phys_cell_group_cfg_s::harq_ack_spatial_bundling_pucch_secondary_pucch_group_r16_e_"};

static const char*
    enum_names_phys_cell_group_cfg_s_harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_e__values[] = {
        "enabled",
        "disabled"};
const asn1::table::enum_desc
    phys_cell_group_cfg_s::harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_opts::desc = {
        enum_names_phys_cell_group_cfg_s_harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_e__values,
        2,
        "phys_cell_group_cfg_s::harq_ack_spatial_bundling_pusch_secondary_pucch_group_r16_e_"};

static const char* enum_names_phys_cell_group_cfg_s_pdsch_harq_ack_codebook_secondary_pucch_group_r16_e__values[] = {
    "semiStatic",
    "dynamic"};
const asn1::table::enum_desc phys_cell_group_cfg_s::pdsch_harq_ack_codebook_secondary_pucch_group_r16_opts::desc = {
    enum_names_phys_cell_group_cfg_s_pdsch_harq_ack_codebook_secondary_pucch_group_r16_e__values,
    2,
    "phys_cell_group_cfg_s::pdsch_harq_ack_codebook_secondary_pucch_group_r16_e_"};

static const char*           enum_names_phys_cell_group_cfg_s_nrdc_p_cmode_fr1_r16_e__values[] = {"semi-static-mode1",
                                                                                                  "semi-static-mode2",
                                                                                                  "dynamic"};
const asn1::table::enum_desc phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16_opts::desc            = {
    enum_names_phys_cell_group_cfg_s_nrdc_p_cmode_fr1_r16_e__values,
    3,
    "phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16_e_"};
uint8_t phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "phys_cell_group_cfg_s::nrdc_p_cmode_fr1_r16_e_");
}

static const char*           enum_names_phys_cell_group_cfg_s_nrdc_p_cmode_fr2_r16_e__values[] = {"semi-static-mode1",
                                                                                                  "semi-static-mode2",
                                                                                                  "dynamic"};
const asn1::table::enum_desc phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16_opts::desc            = {
    enum_names_phys_cell_group_cfg_s_nrdc_p_cmode_fr2_r16_e__values,
    3,
    "phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16_e_"};
uint8_t phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "phys_cell_group_cfg_s::nrdc_p_cmode_fr2_r16_e_");
}

static const char* enum_names_phys_cell_group_cfg_s_dl_assign_idx_dci_1_2_r16_e__values[] = {"n1", "n2", "n4"};
const asn1::table::enum_desc phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16_opts::desc  = {
    enum_names_phys_cell_group_cfg_s_dl_assign_idx_dci_1_2_r16_e__values,
    3,
    "phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16_e_"};
uint8_t phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "phys_cell_group_cfg_s::dl_assign_idx_dci_1_2_r16_e_");
}

static const char* enum_names_phys_cell_group_cfg_s_ack_nack_feedback_mode_r16_e__values[] = {"joint", "separate"};
const asn1::table::enum_desc phys_cell_group_cfg_s::ack_nack_feedback_mode_r16_opts::desc  = {
    enum_names_phys_cell_group_cfg_s_ack_nack_feedback_mode_r16_e__values,
    2,
    "phys_cell_group_cfg_s::ack_nack_feedback_mode_r16_e_"};

// FrequencyInfoDL ::= SEQUENCE
static const asn1::table::seq_field seq_table_freq_info_dl_s_fields[] = {
    {offsetof(freq_info_dl_s, absolute_freq_ssb),
     (int32_t)offsetof(freq_info_dl_s, absolute_freq_ssb_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{3279165},
     nullptr},
    {offsetof(freq_info_dl_s, freq_band_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(freq_info_dl_s::freq_band_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{1023}, false, false>>::value},
    {offsetof(freq_info_dl_s, absolute_freq_point_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{3279165},
     nullptr},
    {offsetof(freq_info_dl_s, scs_specific_carrier_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(freq_info_dl_s::scs_specific_carrier_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &scs_specific_carrier_s::desc}};
static const asn1::table::field_name seq_table_freq_info_dl_s_names[] = {{"absoluteFrequencySSB", nullptr},
                                                                         {"frequencyBandList", nullptr},
                                                                         {"absoluteFrequencyPointA", nullptr},
                                                                         {"scs-SpecificCarrierList", nullptr}};
static const asn1::table::seq_opt    seq_table_freq_info_dl_s_opts[]  = {
    {0, (int32_t)offsetof(freq_info_dl_s, absolute_freq_ssb_present)}};
static const asn1::table::seq_field seq_table_freq_info_dl_s_g0_fields[] = {
    {offsetof(freq_info_dl_s, ref_cell_r18),
     (int32_t)offsetof(freq_info_dl_s, ref_cell_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name seq_table_freq_info_dl_s_g0_names[] = {{"referenceCell-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_freq_info_dl_s_g0_opts[]  = {
    {0, (int32_t)offsetof(freq_info_dl_s, ref_cell_r18_present)}};
static const asn1::table::seq_group seq_table_freq_info_dl_s_groups[] = {{{seq_table_freq_info_dl_s_g0_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_freq_info_dl_s_g0_opts,
                                                                           seq_table_freq_info_dl_s_g0_names,
                                                                           1,
                                                                           1},
                                                                          false}};
const asn1::table::seq_desc         freq_info_dl_s::desc              = {{seq_table_freq_info_dl_s_fields,
                                                                          uint64_t{0xe},
                                                                          seq_table_freq_info_dl_s_opts,
                                                                          seq_table_freq_info_dl_s_names,
                                                                          4,
                                                                          1},
                                                                         true,
                                                                         false,
                                                                         seq_table_freq_info_dl_s_groups,
                                                                         1};

// DownlinkConfigCommon ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_cfg_common_s_fields[] = {
    {offsetof(dl_cfg_common_s, freq_info_dl),
     (int32_t)offsetof(dl_cfg_common_s, freq_info_dl_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &freq_info_dl_s::desc},
    {offsetof(dl_cfg_common_s, init_dl_bwp),
     (int32_t)offsetof(dl_cfg_common_s, init_dl_bwp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_dl_common_s::desc}};
static const asn1::table::field_name seq_table_dl_cfg_common_s_names[] = {{"frequencyInfoDL", nullptr},
                                                                          {"initialDownlinkBWP", nullptr}};
static const asn1::table::seq_opt    seq_table_dl_cfg_common_s_opts[]  = {
    {0, (int32_t)offsetof(dl_cfg_common_s, freq_info_dl_present)},
    {1, (int32_t)offsetof(dl_cfg_common_s, init_dl_bwp_present)}};
static const asn1::table::seq_field seq_table_dl_cfg_common_s_g0_fields[] = {
    {offsetof(dl_cfg_common_s, init_dl_bwp_red_cap_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(dl_cfg_common_s::init_dl_bwp_red_cap_r17)>::value,
     &bwp_dl_common_s::desc}};
static const asn1::table::field_name seq_table_dl_cfg_common_s_g0_names[] = {
    {"initialDownlinkBWP-RedCap-r17", nullptr}};
static const asn1::table::seq_opt   seq_table_dl_cfg_common_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_dl_cfg_common_s_groups[]  = {{{seq_table_dl_cfg_common_s_g0_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_dl_cfg_common_s_g0_opts,
                                                                             seq_table_dl_cfg_common_s_g0_names,
                                                                             1,
                                                                             1},
                                                                            false}};
const asn1::table::seq_desc         dl_cfg_common_s::desc               = {{seq_table_dl_cfg_common_s_fields,
                                                                            uint64_t{0x0},
                                                                            seq_table_dl_cfg_common_s_opts,
                                                                            seq_table_dl_cfg_common_s_names,
                                                                            2,
                                                                            2},
                                                                           true,
                                                                           false,
                                                                           seq_table_dl_cfg_common_s_groups,
                                                                           1};

// FrequencyInfoUL ::= SEQUENCE
static const asn1::table::seq_field seq_table_freq_info_ul_s_fields[] = {
    {offsetof(freq_info_ul_s, freq_band_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(freq_info_ul_s::freq_band_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{1023}, false, false>>::value},
    {offsetof(freq_info_ul_s, absolute_freq_point_a),
     (int32_t)offsetof(freq_info_ul_s, absolute_freq_point_a_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{3279165},
     nullptr},
    {offsetof(freq_info_ul_s, scs_specific_carrier_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(freq_info_ul_s::scs_specific_carrier_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &scs_specific_carrier_s::desc},
    {offsetof(freq_info_ul_s, add_spec_emission),
     (int32_t)offsetof(freq_info_ul_s, add_spec_emission_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(freq_info_ul_s, p_max),
     (int32_t)offsetof(freq_info_ul_s, p_max_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{63},
     nullptr},
    {0,
     (int32_t)offsetof(freq_info_ul_s, freq_shift7p5khz_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_freq_info_ul_s_names[] = {{"frequencyBandList", nullptr},
                                                                         {"absoluteFrequencyPointA", nullptr},
                                                                         {"scs-SpecificCarrierList", nullptr},
                                                                         {"additionalSpectrumEmission", nullptr},
                                                                         {"p-Max", nullptr},
                                                                         {"frequencyShift7p5khz", "true"}};
static const asn1::table::seq_opt    seq_table_freq_info_ul_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(freq_info_ul_s, absolute_freq_point_a_present)},
    {3, (int32_t)offsetof(freq_info_ul_s, add_spec_emission_present)},
    {4, (int32_t)offsetof(freq_info_ul_s, p_max_present)},
    {5, (int32_t)offsetof(freq_info_ul_s, freq_shift7p5khz_present)}};
static const asn1::table::seq_field seq_table_freq_info_ul_s_g0_fields[] = {
    {offsetof(freq_info_ul_s, add_spec_emission_v1760),
     (int32_t)offsetof(freq_info_ul_s, add_spec_emission_v1760_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{8},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name seq_table_freq_info_ul_s_g0_names[] = {
    {"additionalSpectrumEmission-v1760", nullptr}};
static const asn1::table::seq_opt seq_table_freq_info_ul_s_g0_opts[] = {
    {0, (int32_t)offsetof(freq_info_ul_s, add_spec_emission_v1760_present)}};
static const asn1::table::seq_field seq_table_freq_info_ul_s_g1_fields[] = {
    {offsetof(freq_info_ul_s, add_spec_emission_aerial_r18),
     (int32_t)offsetof(freq_info_ul_s, add_spec_emission_aerial_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{39},
     nullptr}};
static const asn1::table::field_name seq_table_freq_info_ul_s_g1_names[] = {
    {"additionalSpectrumEmissionAerial-r18", nullptr}};
static const asn1::table::seq_opt seq_table_freq_info_ul_s_g1_opts[] = {
    {0, (int32_t)offsetof(freq_info_ul_s, add_spec_emission_aerial_r18_present)}};
static const asn1::table::seq_group seq_table_freq_info_ul_s_groups[] = {{{seq_table_freq_info_ul_s_g0_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_freq_info_ul_s_g0_opts,
                                                                           seq_table_freq_info_ul_s_g0_names,
                                                                           1,
                                                                           1},
                                                                          false},
                                                                         {{seq_table_freq_info_ul_s_g1_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_freq_info_ul_s_g1_opts,
                                                                           seq_table_freq_info_ul_s_g1_names,
                                                                           1,
                                                                           1},
                                                                          false}};
const asn1::table::seq_desc         freq_info_ul_s::desc              = {{seq_table_freq_info_ul_s_fields,
                                                                          uint64_t{0x4},
                                                                          seq_table_freq_info_ul_s_opts,
                                                                          seq_table_freq_info_ul_s_names,
                                                                          6,
                                                                          5},
                                                                         true,
                                                                         false,
                                                                         seq_table_freq_info_ul_s_groups,
                                                                         2};

// UplinkConfigCommon ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_cfg_common_s_fields[] = {
    {offsetof(ul_cfg_common_s, freq_info_ul),
     (int32_t)offsetof(ul_cfg_common_s, freq_info_ul_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &freq_info_ul_s::desc},
    {offsetof(ul_cfg_common_s, init_ul_bwp),
     (int32_t)offsetof(ul_cfg_common_s, init_ul_bwp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_ul_common_s::desc},
    {offsetof(ul_cfg_common_s, dummy),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ul_cfg_common_s::dummy)>(),
     decltype(ul_cfg_common_s::dummy)::has_ext,
     false,
     decltype(ul_cfg_common_s::dummy)::nof_types,
     decltype(ul_cfg_common_s::dummy)::nof_exts,
     nullptr,
     &time_align_timer_opts::desc}};
static const asn1::table::field_name seq_table_ul_cfg_common_s_names[] = {{"frequencyInfoUL", nullptr},
                                                                          {"initialUplinkBWP", nullptr},
                                                                          {"dummy", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_cfg_common_s_opts[]  = {
    {0, (int32_t)offsetof(ul_cfg_common_s, freq_info_ul_present)},
    {1, (int32_t)offsetof(ul_cfg_common_s, init_ul_bwp_present)}};
const asn1::table::seq_desc ul_cfg_common_s::desc = {{seq_table_ul_cfg_common_s_fields,
                                                      uint64_t{0x4},
                                                      seq_table_ul_cfg_common_s_opts,
                                                      seq_table_ul_cfg_common_s_names,
                                                      3,
                                                      2},
                                                     false,
                                                     false,
                                                     nullptr,
                                                     0};

// HighSpeedConfig-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_high_speed_cfg_v1700_s_fields[] = {
    {0,
     (int32_t)offsetof(high_speed_cfg_v1700_s, high_speed_meas_ca_scell_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(high_speed_cfg_v1700_s, high_speed_meas_inter_freq_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(high_speed_cfg_v1700_s, high_speed_demod_ca_scell_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_high_speed_cfg_v1700_s_names[] = {
    {"highSpeedMeasCA-Scell-r17", "true"},
    {"highSpeedMeasInterFreq-r17", "true"},
    {"highSpeedDemodCA-Scell-r17", "true"}};
static const asn1::table::seq_opt seq_table_high_speed_cfg_v1700_s_opts[] = {
    {0, (int32_t)offsetof(high_speed_cfg_v1700_s, high_speed_meas_ca_scell_r17_present)},
    {1, (int32_t)offsetof(high_speed_cfg_v1700_s, high_speed_meas_inter_freq_r17_present)},
    {2, (int32_t)offsetof(high_speed_cfg_v1700_s, high_speed_demod_ca_scell_r17_present)}};
const asn1::table::seq_desc high_speed_cfg_v1700_s::desc = {{seq_table_high_speed_cfg_v1700_s_fields,
                                                             uint64_t{0x0},
                                                             seq_table_high_speed_cfg_v1700_s_opts,
                                                             seq_table_high_speed_cfg_v1700_s_names,
                                                             3,
                                                             3},
                                                            true,
                                                            false,
                                                            nullptr,
                                                            0};

// UplinkConfigCommon-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_cfg_common_v1700_s_fields[] = {
    {offsetof(ul_cfg_common_v1700_s, init_ul_bwp_red_cap_r17),
     (int32_t)offsetof(ul_cfg_common_v1700_s, init_ul_bwp_red_cap_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_ul_common_s::desc}};
static const asn1::table::field_name seq_table_ul_cfg_common_v1700_s_names[] = {
    {"initialUplinkBWP-RedCap-r17", nullptr}};
static const asn1::table::seq_opt seq_table_ul_cfg_common_v1700_s_opts[] = {
    {0, (int32_t)offsetof(ul_cfg_common_v1700_s, init_ul_bwp_red_cap_r17_present)}};
const asn1::table::seq_desc ul_cfg_common_v1700_s::desc = {{seq_table_ul_cfg_common_v1700_s_fields,
                                                            uint64_t{0x0},
                                                            seq_table_ul_cfg_common_v1700_s_opts,
                                                            seq_table_ul_cfg_common_v1700_s_names,
                                                            1,
                                                            1},
                                                           false,
                                                           false,
                                                           nullptr,
                                                           0};

// ServingCellConfigCommon ::= SEQUENCE
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_s_feature_priorities_r17_s__fields[] = {
    {offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, red_cap_prio_r17),
     (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, red_cap_prio_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, slicing_prio_r17),
     (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, slicing_prio_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, msg3_repeats_prio_r17),
     (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, msg3_repeats_prio_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, sdt_prio_r17),
     (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, sdt_prio_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_s_feature_priorities_r17_s__names[] = {
    {"redCapPriority-r17", nullptr},
    {"slicingPriority-r17", nullptr},
    {"msg3-Repetitions-Priority-r17", nullptr},
    {"sdt-Priority-r17", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_s_feature_priorities_r17_s__opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, red_cap_prio_r17_present)},
    {1, (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, slicing_prio_r17_present)},
    {2, (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, msg3_repeats_prio_r17_present)},
    {3, (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_r17_s_, sdt_prio_r17_present)}};
static const asn1::table::seq_desc seq_table_serving_cell_cfg_common_s_feature_priorities_r17_s_ = {
    {seq_table_serving_cell_cfg_common_s_feature_priorities_r17_s__fields,
     uint64_t{0x0},
     seq_table_serving_cell_cfg_common_s_feature_priorities_r17_s__opts,
     seq_table_serving_cell_cfg_common_s_feature_priorities_r17_s__names,
     4,
     4},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_s_feature_priorities_v1800_s__fields[] = {
    {offsetof(serving_cell_cfg_common_s::feature_priorities_v1800_s_, msg1_repeats_prio_r18),
     (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_v1800_s_, msg1_repeats_prio_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {offsetof(serving_cell_cfg_common_s::feature_priorities_v1800_s_, ered_cap_prio_r18),
     (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_v1800_s_, ered_cap_prio_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_s_feature_priorities_v1800_s__names[] = {
    {"msg1-Repetitions-Priority-r18", nullptr},
    {"eRedCapPriority-r18", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_s_feature_priorities_v1800_s__opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_v1800_s_, msg1_repeats_prio_r18_present)},
    {1, (int32_t)offsetof(serving_cell_cfg_common_s::feature_priorities_v1800_s_, ered_cap_prio_r18_present)}};
static const asn1::table::seq_desc seq_table_serving_cell_cfg_common_s_feature_priorities_v1800_s_ = {
    {seq_table_serving_cell_cfg_common_s_feature_priorities_v1800_s__fields,
     uint64_t{0x0},
     seq_table_serving_cell_cfg_common_s_feature_priorities_v1800_s__opts,
     seq_table_serving_cell_cfg_common_s_feature_priorities_v1800_s__names,
     2,
     2},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field
    seq_table_serving_cell_cfg_common_s_lte_crs_to_match_around_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &rate_match_pattern_lte_crs_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_s_lte_crs_to_match_around_setup_release_names[] =
    {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_common_s_lte_crs_to_match_around_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_common_s_lte_crs_to_match_around_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_common_s_lte_crs_to_match_around_setup_release_names,
    "setup_release_c",
    setup_release_c<rate_match_pattern_lte_crs_s>::type_offset(),
    setup_release_c<rate_match_pattern_lte_crs_s>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_s_fields[] = {
    {offsetof(serving_cell_cfg_common_s, pci),
     (int32_t)offsetof(serving_cell_cfg_common_s, pci_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1007},
     nullptr},
    {offsetof(serving_cell_cfg_common_s, dl_cfg_common),
     (int32_t)offsetof(serving_cell_cfg_common_s, dl_cfg_common_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_cfg_common_s::desc},
    {offsetof(serving_cell_cfg_common_s, ul_cfg_common),
     (int32_t)offsetof(serving_cell_cfg_common_s, ul_cfg_common_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_cfg_common_s::desc},
    {offsetof(serving_cell_cfg_common_s, supplementary_ul_cfg),
     (int32_t)offsetof(serving_cell_cfg_common_s, supplementary_ul_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_cfg_common_s::desc},
    {offsetof(serving_cell_cfg_common_s, n_timing_advance_offset),
     (int32_t)offsetof(serving_cell_cfg_common_s, n_timing_advance_offset_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_common_s::n_timing_advance_offset)>(),
     decltype(serving_cell_cfg_common_s::n_timing_advance_offset)::has_ext,
     false,
     decltype(serving_cell_cfg_common_s::n_timing_advance_offset)::nof_types,
     decltype(serving_cell_cfg_common_s::n_timing_advance_offset)::nof_exts,
     nullptr,
     &serving_cell_cfg_common_s::n_timing_advance_offset_opts::desc},
    {offsetof(serving_cell_cfg_common_s, ssb_positions_in_burst),
     (int32_t)offsetof(serving_cell_cfg_common_s, ssb_positions_in_burst_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &serving_cell_cfg_common_s::ssb_positions_in_burst_c_::desc},
    {offsetof(serving_cell_cfg_common_s, ssb_periodicity_serving_cell),
     (int32_t)offsetof(serving_cell_cfg_common_s, ssb_periodicity_serving_cell_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_common_s::ssb_periodicity_serving_cell)>(),
     decltype(serving_cell_cfg_common_s::ssb_periodicity_serving_cell)::has_ext,
     false,
     decltype(serving_cell_cfg_common_s::ssb_periodicity_serving_cell)::nof_types,
     decltype(serving_cell_cfg_common_s::ssb_periodicity_serving_cell)::nof_exts,
     nullptr,
     &serving_cell_cfg_common_s::ssb_periodicity_serving_cell_opts::desc},
    {offsetof(serving_cell_cfg_common_s, dmrs_type_a_position),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(serving_cell_cfg_common_s::dmrs_type_a_position)>(),
     decltype(serving_cell_cfg_common_s::dmrs_type_a_position)::has_ext,
     false,
     decltype(serving_cell_cfg_common_s::dmrs_type_a_position)::nof_types,
     decltype(serving_cell_cfg_common_s::dmrs_type_a_position)::nof_exts,
     nullptr,
     &serving_cell_cfg_common_s::dmrs_type_a_position_opts::desc},
    {offsetof(serving_cell_cfg_common_s, lte_crs_to_match_around),
     (int32_t)offsetof(serving_cell_cfg_common_s, lte_crs_to_match_around_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_serving_cell_cfg_common_s_lte_crs_to_match_around_setup_release},
    {offsetof(serving_cell_cfg_common_s, rate_match_pattern_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::item_list_codec<decltype(serving_cell_cfg_common_s::rate_match_pattern_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &rate_match_pattern_s::desc},
    {offsetof(serving_cell_cfg_common_s, rate_match_pattern_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::list_codec<decltype(serving_cell_cfg_common_s::rate_match_pattern_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{3}, false, false>>::value},
    {offsetof(serving_cell_cfg_common_s, ssb_subcarrier_spacing),
     (int32_t)offsetof(serving_cell_cfg_common_s, ssb_subcarrier_spacing_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_common_s::ssb_subcarrier_spacing)>(),
     decltype(serving_cell_cfg_common_s::ssb_subcarrier_spacing)::has_ext,
     false,
     decltype(serving_cell_cfg_common_s::ssb_subcarrier_spacing)::nof_types,
     decltype(serving_cell_cfg_common_s::ssb_subcarrier_spacing)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(serving_cell_cfg_common_s, tdd_ul_dl_cfg_common),
     (int32_t)offsetof(serving_cell_cfg_common_s, tdd_ul_dl_cfg_common_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tdd_ul_dl_cfg_common_s::desc},
    {offsetof(serving_cell_cfg_common_s, ss_pbch_block_pwr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-60},
     uint64_t{110},
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_s_names[] = {
    {"physCellId", nullptr},
    {"downlinkConfigCommon", nullptr},
    {"uplinkConfigCommon", nullptr},
    {"supplementaryUplinkConfig", nullptr},
    {"n-TimingAdvanceOffset", nullptr},
    {"ssb-PositionsInBurst", nullptr},
    {"ssb-periodicityServingCell", nullptr},
    {"dmrs-TypeA-Position", nullptr},
    {"lte-CRS-ToMatchAround", nullptr},
    {"rateMatchPatternToAddModList", nullptr},
    {"rateMatchPatternToReleaseList", nullptr},
    {"ssbSubcarrierSpacing", nullptr},
    {"tdd-UL-DL-ConfigurationCommon", nullptr},
    {"ss-PBCH-BlockPower", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_s_opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_common_s, pci_present)},
    {1, (int32_t)offsetof(serving_cell_cfg_common_s, dl_cfg_common_present)},
    {2, (int32_t)offsetof(serving_cell_cfg_common_s, ul_cfg_common_present)},
    {3, (int32_t)offsetof(serving_cell_cfg_common_s, supplementary_ul_cfg_present)},
    {4, (int32_t)offsetof(serving_cell_cfg_common_s, n_timing_advance_offset_present)},
    {5, (int32_t)offsetof(serving_cell_cfg_common_s, ssb_positions_in_burst_present)},
    {6, (int32_t)offsetof(serving_cell_cfg_common_s, ssb_periodicity_serving_cell_present)},
    {8, (int32_t)offsetof(serving_cell_cfg_common_s, lte_crs_to_match_around_present)},
    {9, asn1::table::seq_field::list_presence},
    {10, asn1::table::seq_field::list_presence},
    {11, (int32_t)offsetof(serving_cell_cfg_common_s, ssb_subcarrier_spacing_present)},
    {12, (int32_t)offsetof(serving_cell_cfg_common_s, tdd_ul_dl_cfg_common_present)}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_s_g0_fields[] = {
    {offsetof(serving_cell_cfg_common_s, ch_access_mode_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_s::ch_access_mode_r16)>::value,
     &serving_cell_cfg_common_s::ch_access_mode_r16_c_::desc},
    {offsetof(serving_cell_cfg_common_s, discovery_burst_win_len_r16),
     (int32_t)offsetof(serving_cell_cfg_common_s, discovery_burst_win_len_r16_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_common_s::discovery_burst_win_len_r16)>(),
     decltype(serving_cell_cfg_common_s::discovery_burst_win_len_r16)::has_ext,
     false,
     decltype(serving_cell_cfg_common_s::discovery_burst_win_len_r16)::nof_types,
     decltype(serving_cell_cfg_common_s::discovery_burst_win_len_r16)::nof_exts,
     nullptr,
     &serving_cell_cfg_common_s::discovery_burst_win_len_r16_opts::desc},
    {offsetof(serving_cell_cfg_common_s, ssb_position_qcl_r16),
     (int32_t)offsetof(serving_cell_cfg_common_s, ssb_position_qcl_r16_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_common_s::ssb_position_qcl_r16)>(),
     decltype(serving_cell_cfg_common_s::ssb_position_qcl_r16)::has_ext,
     false,
     decltype(serving_cell_cfg_common_s::ssb_position_qcl_r16)::nof_types,
     decltype(serving_cell_cfg_common_s::ssb_position_qcl_r16)::nof_exts,
     nullptr,
     &ssb_position_qcl_relation_r16_opts::desc},
    {offsetof(serving_cell_cfg_common_s, high_speed_cfg_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_s::high_speed_cfg_r16)>::value,
     &high_speed_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_s_g0_names[] = {
    {"channelAccessMode-r16", nullptr},
    {"discoveryBurstWindowLength-r16", nullptr},
    {"ssb-PositionQCL-r16", nullptr},
    {"highSpeedConfig-r16", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(serving_cell_cfg_common_s, discovery_burst_win_len_r16_present)},
    {2, (int32_t)offsetof(serving_cell_cfg_common_s, ssb_position_qcl_r16_present)},
    {3, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_s_g1_fields[] = {
    {offsetof(serving_cell_cfg_common_s, high_speed_cfg_v1700),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_s::high_speed_cfg_v1700)>::value,
     &high_speed_cfg_v1700_s::desc},
    {0,
     (int32_t)offsetof(serving_cell_cfg_common_s, ch_access_mode2_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(serving_cell_cfg_common_s, discovery_burst_win_len_r17),
     (int32_t)offsetof(serving_cell_cfg_common_s, discovery_burst_win_len_r17_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_common_s::discovery_burst_win_len_r17)>(),
     decltype(serving_cell_cfg_common_s::discovery_burst_win_len_r17)::has_ext,
     false,
     decltype(serving_cell_cfg_common_s::discovery_burst_win_len_r17)::nof_types,
     decltype(serving_cell_cfg_common_s::discovery_burst_win_len_r17)::nof_exts,
     nullptr,
     &serving_cell_cfg_common_s::discovery_burst_win_len_r17_opts::desc},
    {offsetof(serving_cell_cfg_common_s, ssb_position_qcl_r17),
     (int32_t)offsetof(serving_cell_cfg_common_s, ssb_position_qcl_r17_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_common_s::ssb_position_qcl_r17)>(),
     decltype(serving_cell_cfg_common_s::ssb_position_qcl_r17)::has_ext,
     false,
     decltype(serving_cell_cfg_common_s::ssb_position_qcl_r17)::nof_types,
     decltype(serving_cell_cfg_common_s::ssb_position_qcl_r17)::nof_exts,
     nullptr,
     &ssb_position_qcl_relation_r17_opts::desc},
    {offsetof(serving_cell_cfg_common_s, high_speed_cfg_fr2_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_s::high_speed_cfg_fr2_r17)>::value,
     &high_speed_cfg_fr2_r17_s::desc},
    {offsetof(serving_cell_cfg_common_s, ul_cfg_common_v1700),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_s::ul_cfg_common_v1700)>::value,
     &ul_cfg_common_v1700_s::desc},
    {offsetof(serving_cell_cfg_common_s, ntn_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_s::ntn_cfg_r17)>::value,
     &ntn_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_s_g1_names[] = {
    {"highSpeedConfig-v1700", nullptr},
    {"channelAccessMode2-r17", "enabled"},
    {"discoveryBurstWindowLength-r17", nullptr},
    {"ssb-PositionQCL-r17", nullptr},
    {"highSpeedConfigFR2-r17", nullptr},
    {"uplinkConfigCommon-v1700", nullptr},
    {"ntn-Config-r17", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(serving_cell_cfg_common_s, ch_access_mode2_r17_present)},
    {2, (int32_t)offsetof(serving_cell_cfg_common_s, discovery_burst_win_len_r17_present)},
    {3, (int32_t)offsetof(serving_cell_cfg_common_s, ssb_position_qcl_r17_present)},
    {4, asn1::table::seq_field::ptr_presence},
    {5, asn1::table::seq_field::ptr_presence},
    {6, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_s_g2_fields[] = {
    {offsetof(serving_cell_cfg_common_s, feature_priorities_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_s::feature_priorities_r17)>::value,
     &seq_table_serving_cell_cfg_common_s_feature_priorities_r17_s_}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_s_g2_names[] = {
    {"featurePriorities-r17", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_s_g2_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_s_g3_fields[] = {
    {0,
     (int32_t)offsetof(serving_cell_cfg_common_s, ra_ch_access_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_s_g3_names[] = {
    {"ra-ChannelAccess-r17", "enabled"}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_s_g3_opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_common_s, ra_ch_access_r17_present)}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_s_g4_fields[] = {
    {offsetof(serving_cell_cfg_common_s, feature_priorities_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_s::feature_priorities_v1800)>::value,
     &seq_table_serving_cell_cfg_common_s_feature_priorities_v1800_s_},
    {offsetof(serving_cell_cfg_common_s, atg_cfg_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_s::atg_cfg_r18)>::value,
     &atg_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_s_g4_names[] = {
    {"featurePriorities-v1800", nullptr},
    {"atg-Config-r18", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_s_g4_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_serving_cell_cfg_common_s_groups[] = {
    {{seq_table_serving_cell_cfg_common_s_g0_fields,
      uint64_t{0x0},
      seq_table_serving_cell_cfg_common_s_g0_opts,
      seq_table_serving_cell_cfg_common_s_g0_names,
      4,
      4},
     false},
    {{seq_table_serving_cell_cfg_common_s_g1_fields,
      uint64_t{0x0},
      seq_table_serving_cell_cfg_common_s_g1_opts,
      seq_table_serving_cell_cfg_common_s_g1_names,
      7,
      7},
     false},
    {{seq_table_serving_cell_cfg_common_s_g2_fields,
      uint64_t{0x0},
      seq_table_serving_cell_cfg_common_s_g2_opts,
      seq_table_serving_cell_cfg_common_s_g2_names,
      1,
      1},
     false},
    {{seq_table_serving_cell_cfg_common_s_g3_fields,
      uint64_t{0x0},
      seq_table_serving_cell_cfg_common_s_g3_opts,
      seq_table_serving_cell_cfg_common_s_g3_names,
      1,
      1},
     false},
    {{seq_table_serving_cell_cfg_common_s_g4_fields,
      uint64_t{0x0},
      seq_table_serving_cell_cfg_common_s_g4_opts,
      seq_table_serving_cell_cfg_common_s_g4_names,
      2,
      2},
     false}};
const asn1::table::seq_desc serving_cell_cfg_common_s::desc = {{seq_table_serving_cell_cfg_common_s_fields,
                                                                uint64_t{0x2080},
                                                                seq_table_serving_cell_cfg_common_s_opts,
                                                                seq_table_serving_cell_cfg_common_s_names,
                                                                14,
                                                                12},
                                                               true,
                                                               false,
                                                               seq_table_serving_cell_cfg_common_s_groups,
                                                               5};

static const char*           enum_names_serving_cell_cfg_common_s_n_timing_advance_offset_e__values[] = {"n0",
                                                                                                         "n25600",
                                                                                                         "n39936"};
const asn1::table::enum_desc serving_cell_cfg_common_s::n_timing_advance_offset_opts::desc            = {
    enum_names_serving_cell_cfg_common_s_n_timing_advance_offset_e__values,
    3,
    "serving_cell_cfg_common_s::n_timing_advance_offset_e_"};
uint16_t serving_cell_cfg_common_s::n_timing_advance_offset_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 25600, 39936};
  return map_enum_number(numbers, 3, value, "serving_cell_cfg_common_s::n_timing_advance_offset_e_");
}

static const asn1::table::storage_ops* const
    choice_storage_serving_cell_cfg_common_s_ssb_positions_in_burst_c__alternatives[] = {
        &asn1::table::alternative_ops<fixed_bitstring<4>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<8>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value};
const asn1::table::choice_storage_desc serving_cell_cfg_common_s::ssb_positions_in_burst_c_::storage_desc = {
    choice_storage_serving_cell_cfg_common_s_ssb_positions_in_burst_c__alternatives,
    3,
    "serving_cell_cfg_common_s::ssb_positions_in_burst_c_"};

fixed_bitstring<4>& serving_cell_cfg_common_s::ssb_positions_in_burst_c_::set_short_bitmap()
{
  set(types::short_bitmap);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<8>& serving_cell_cfg_common_s::ssb_positions_in_burst_c_::set_medium_bitmap()
{
  set(types::medium_bitmap);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& serving_cell_cfg_common_s::ssb_positions_in_burst_c_::set_long_bitmap()
{
  set(types::long_bitmap);
  return c.get<fixed_bitstring<64>>();
}
static const asn1::table::seq_field choice_table_serving_cell_cfg_common_s_ssb_positions_in_burst_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<4>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<8>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_serving_cell_cfg_common_s_ssb_positions_in_burst_c__names[] = {
    {"shortBitmap", nullptr},
    {"mediumBitmap", nullptr},
    {"longBitmap", nullptr}};
const asn1::table::choice_desc serving_cell_cfg_common_s::ssb_positions_in_burst_c_::desc = {
    asn1::table::enum_int_type<serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types>(),
    serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types::nof_types,
    serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types::nof_exts,
    serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types::has_ext,
    false,
    choice_table_serving_cell_cfg_common_s_ssb_positions_in_burst_c__alternatives,
    nullptr,
    choice_table_serving_cell_cfg_common_s_ssb_positions_in_burst_c__names,
    "serving_cell_cfg_common_s::ssb_positions_in_burst_c_",
    offsetof(serving_cell_cfg_common_s::ssb_positions_in_burst_c_, c.type),
    offsetof(serving_cell_cfg_common_s::ssb_positions_in_burst_c_, c.value),
    false,
    &serving_cell_cfg_common_s::ssb_positions_in_burst_c_::storage_desc};

static const char* enum_names_serving_cell_cfg_common_s_ssb_positions_in_burst_c__types_values[]    = {"shortBitmap",
                                                                                                       "mediumBitmap",
                                                                                                       "longBitmap"};
const asn1::table::enum_desc serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types_opts::desc = {
    enum_names_serving_cell_cfg_common_s_ssb_positions_in_burst_c__types_values,
    3,
    "serving_cell_cfg_common_s::ssb_positions_in_burst_c_::types"};

static const char* enum_names_serving_cell_cfg_common_s_ssb_periodicity_serving_cell_e__values[] =
    {"ms5", "ms10", "ms20", "ms40", "ms80", "ms160", "spare2", "spare1"};
const asn1::table::enum_desc serving_cell_cfg_common_s::ssb_periodicity_serving_cell_opts::desc = {
    enum_names_serving_cell_cfg_common_s_ssb_periodicity_serving_cell_e__values,
    8,
    "serving_cell_cfg_common_s::ssb_periodicity_serving_cell_e_"};
uint8_t serving_cell_cfg_common_s::ssb_periodicity_serving_cell_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_s::ssb_periodicity_serving_cell_e_");
}

static const char*           enum_names_serving_cell_cfg_common_s_dmrs_type_a_position_e__values[] = {"pos2", "pos3"};
const asn1::table::enum_desc serving_cell_cfg_common_s::dmrs_type_a_position_opts::desc            = {
    enum_names_serving_cell_cfg_common_s_dmrs_type_a_position_e__values,
    2,
    "serving_cell_cfg_common_s::dmrs_type_a_position_e_"};
uint8_t serving_cell_cfg_common_s::dmrs_type_a_position_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3};
  return map_enum_number(numbers, 2, value, "serving_cell_cfg_common_s::dmrs_type_a_position_e_");
}

void serving_cell_cfg_common_s::ch_access_mode_r16_c_::set(types::options e)
{
  type_ = e;
}
void serving_cell_cfg_common_s::ch_access_mode_r16_c_::set_dyn()
{
  set(types::dyn);
}
semi_static_ch_access_cfg_r16_s& serving_cell_cfg_common_s::ch_access_mode_r16_c_::set_semi_static()
{
  set(types::semi_static);
  return c;
}
static const asn1::table::seq_field choice_table_serving_cell_cfg_common_s_ch_access_mode_r16_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &semi_static_ch_access_cfg_r16_s::desc}};
static const asn1::table::field_name choice_table_serving_cell_cfg_common_s_ch_access_mode_r16_c__names[] = {
    {"dynamic", nullptr},
    {"semiStatic", nullptr}};
const asn1::table::choice_desc serving_cell_cfg_common_s::ch_access_mode_r16_c_::desc = {
    asn1::table::enum_int_type<serving_cell_cfg_common_s::ch_access_mode_r16_c_::types>(),
    serving_cell_cfg_common_s::ch_access_mode_r16_c_::types::nof_types,
    serving_cell_cfg_common_s::ch_access_mode_r16_c_::types::nof_exts,
    serving_cell_cfg_common_s::ch_access_mode_r16_c_::types::has_ext,
    false,
    choice_table_serving_cell_cfg_common_s_ch_access_mode_r16_c__alternatives,
    &asn1::table::choice_set<serving_cell_cfg_common_s::ch_access_mode_r16_c_>,
    choice_table_serving_cell_cfg_common_s_ch_access_mode_r16_c__names,
    "serving_cell_cfg_common_s::ch_access_mode_r16_c_",
    offsetof(serving_cell_cfg_common_s::ch_access_mode_r16_c_, type_),
    offsetof(serving_cell_cfg_common_s::ch_access_mode_r16_c_, c),
    false};

static const char*           enum_names_serving_cell_cfg_common_s_ch_access_mode_r16_c__types_values[] = {"dynamic",
                                                                                                          "semiStatic"};
const asn1::table::enum_desc serving_cell_cfg_common_s::ch_access_mode_r16_c_::types_opts::desc        = {
    enum_names_serving_cell_cfg_common_s_ch_access_mode_r16_c__types_values,
    2,
    "serving_cell_cfg_common_s::ch_access_mode_r16_c_::types"};

static const char* enum_names_serving_cell_cfg_common_s_discovery_burst_win_len_r16_e__values[] =
    {"ms0dot5", "ms1", "ms2", "ms3", "ms4", "ms5"};
const asn1::table::enum_desc serving_cell_cfg_common_s::discovery_burst_win_len_r16_opts::desc = {
    enum_names_serving_cell_cfg_common_s_discovery_burst_win_len_r16_e__values,
    6,
    "serving_cell_cfg_common_s::discovery_burst_win_len_r16_e_"};
float serving_cell_cfg_common_s::discovery_burst_win_len_r16_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 3.0, 4.0, 5.0};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_s::discovery_burst_win_len_r16_e_");
}
const char* serving_cell_cfg_common_s::discovery_burst_win_len_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "1", "2", "3", "4", "5"};
  return convert_enum_idx(number_strs, 6, value, "serving_cell_cfg_common_s::discovery_burst_win_len_r16_e_");
}

static const char* enum_names_serving_cell_cfg_common_s_discovery_burst_win_len_r17_e__values[] =
    {"ms0dot125", "ms0dot25", "ms0dot5", "ms0dot75", "ms1", "ms1dot25"};
const asn1::table::enum_desc serving_cell_cfg_common_s::discovery_burst_win_len_r17_opts::desc = {
    enum_names_serving_cell_cfg_common_s_discovery_burst_win_len_r17_e__values,
    6,
    "serving_cell_cfg_common_s::discovery_burst_win_len_r17_e_"};
float serving_cell_cfg_common_s::discovery_burst_win_len_r17_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 0.75, 1.0, 1.25};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_s::discovery_burst_win_len_r17_e_");
}
const char* serving_cell_cfg_common_s::discovery_burst_win_len_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.125", "0.25", "0.5", "0.75", "1", "1.25"};
  return convert_enum_idx(number_strs, 6, value, "serving_cell_cfg_common_s::discovery_burst_win_len_r17_e_");
}

// RACH-ConfigDedicated ::= SEQUENCE
static const asn1::table::seq_field seq_table_rach_cfg_ded_s_fields[] = {
    {offsetof(rach_cfg_ded_s, cfra),
     (int32_t)offsetof(rach_cfg_ded_s, cfra_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cfra_s::desc},
    {offsetof(rach_cfg_ded_s, ra_prioritization),
     (int32_t)offsetof(rach_cfg_ded_s, ra_prioritization_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ra_prioritization_s::desc}};
static const asn1::table::field_name seq_table_rach_cfg_ded_s_names[] = {{"cfra", nullptr},
                                                                         {"ra-Prioritization", nullptr}};
static const asn1::table::seq_opt    seq_table_rach_cfg_ded_s_opts[]  = {
    {0, (int32_t)offsetof(rach_cfg_ded_s, cfra_present)},
    {1, (int32_t)offsetof(rach_cfg_ded_s, ra_prioritization_present)}};
static const asn1::table::seq_field seq_table_rach_cfg_ded_s_g0_fields[] = {
    {offsetof(rach_cfg_ded_s, ra_prioritization_two_step_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(rach_cfg_ded_s::ra_prioritization_two_step_r16)>::value,
     &ra_prioritization_s::desc},
    {offsetof(rach_cfg_ded_s, cfra_two_step_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(rach_cfg_ded_s::cfra_two_step_r16)>::value,
     &cfra_two_step_r16_s::desc}};
static const asn1::table::field_name seq_table_rach_cfg_ded_s_g0_names[] = {{"ra-PrioritizationTwoStep-r16", nullptr},
                                                                            {"cfra-TwoStep-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_rach_cfg_ded_s_g0_opts[]  = {{0, asn1::table::seq_field::ptr_presence},
                                                                            {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_rach_cfg_ded_s_groups[]   = {{{seq_table_rach_cfg_ded_s_g0_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_rach_cfg_ded_s_g0_opts,
                                                                              seq_table_rach_cfg_ded_s_g0_names,
                                                                              2,
                                                                              2},
                                                                             false}};
const asn1::table::seq_desc          rach_cfg_ded_s::desc                = {{seq_table_rach_cfg_ded_s_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_rach_cfg_ded_s_opts,
                                                                             seq_table_rach_cfg_ded_s_names,
                                                                             2,
                                                                             2},
                                                                            true,
                                                                            false,
                                                                            seq_table_rach_cfg_ded_s_groups,
                                                                            1};

// DAPS-UplinkPowerConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_daps_ul_pwr_cfg_r16_s_fields[] = {
    {offsetof(daps_ul_pwr_cfg_r16_s, p_daps_source_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{63},
     nullptr},
    {offsetof(daps_ul_pwr_cfg_r16_s, p_daps_target_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     int64_t{-30},
     uint64_t{63},
     nullptr},
    {offsetof(daps_ul_pwr_cfg_r16_s, ul_pwr_sharing_daps_mode_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16)>(),
     decltype(daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16)::has_ext,
     false,
     decltype(daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16)::nof_types,
     decltype(daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16)::nof_exts,
     nullptr,
     &daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16_opts::desc}};
static const asn1::table::field_name seq_table_daps_ul_pwr_cfg_r16_s_names[] = {
    {"p-DAPS-Source-r16", nullptr},
    {"p-DAPS-Target-r16", nullptr},
    {"uplinkPowerSharingDAPS-Mode-r16", nullptr}};
const asn1::table::seq_desc daps_ul_pwr_cfg_r16_s::desc = {
    {seq_table_daps_ul_pwr_cfg_r16_s_fields, uint64_t{0x7}, nullptr, seq_table_daps_ul_pwr_cfg_r16_s_names, 3, 0},
    false,
    false,
    nullptr,
    0};

static const char* enum_names_daps_ul_pwr_cfg_r16_s_ul_pwr_sharing_daps_mode_r16_e__values[] = {"semi-static-mode1",
                                                                                                "semi-static-mode2",
                                                                                                "dynamic"};
const asn1::table::enum_desc daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16_opts::desc  = {
    enum_names_daps_ul_pwr_cfg_r16_s_ul_pwr_sharing_daps_mode_r16_e__values,
    3,
    "daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16_e_"};
uint8_t daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "daps_ul_pwr_cfg_r16_s::ul_pwr_sharing_daps_mode_r16_e_");
}

// SL-PathSwitchConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_path_switch_cfg_r17_s_fields[] = {
    {offsetof(sl_path_switch_cfg_r17_s, target_relay_ue_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sl_path_switch_cfg_r17_s::target_relay_ue_id_r17),
                                asn1::table::struct_json>::value},
    {offsetof(sl_path_switch_cfg_r17_s, t420_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sl_path_switch_cfg_r17_s::t420_r17)>(),
     decltype(sl_path_switch_cfg_r17_s::t420_r17)::has_ext,
     false,
     decltype(sl_path_switch_cfg_r17_s::t420_r17)::nof_types,
     decltype(sl_path_switch_cfg_r17_s::t420_r17)::nof_exts,
     nullptr,
     &sl_path_switch_cfg_r17_s::t420_r17_opts::desc}};
static const asn1::table::field_name seq_table_sl_path_switch_cfg_r17_s_names[] = {
    {"targetRelayUE-Identity-r17", nullptr},
    {"t420-r17", nullptr}};
const asn1::table::seq_desc sl_path_switch_cfg_r17_s::desc = {
    {seq_table_sl_path_switch_cfg_r17_s_fields, uint64_t{0x3}, nullptr, seq_table_sl_path_switch_cfg_r17_s_names, 2, 0},
    true,
    false,
    nullptr,
    0};

static const char* enum_names_sl_path_switch_cfg_r17_s_t420_r17_e__values[] =
    {"ms50", "ms100", "ms150", "ms200", "ms500", "ms1000", "ms2000", "ms10000"};
const asn1::table::enum_desc sl_path_switch_cfg_r17_s::t420_r17_opts::desc = {
    enum_names_sl_path_switch_cfg_r17_s_t420_r17_e__values,
    8,
    "sl_path_switch_cfg_r17_s::t420_r17_e_"};
uint16_t sl_path_switch_cfg_r17_s::t420_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {50, 100, 150, 200, 500, 1000, 2000, 10000};
  return map_enum_number(numbers, 8, value, "sl_path_switch_cfg_r17_s::t420_r17_e_");
}

// RACH-LessHO-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_rach_less_ho_r18_s_fields[] = {
    {offsetof(rach_less_ho_r18_s, target_nta_r18),
     (int32_t)offsetof(rach_less_ho_r18_s, target_nta_r18_present),
     asn1::table::enum_op<decltype(rach_less_ho_r18_s::target_nta_r18)>(),
     decltype(rach_less_ho_r18_s::target_nta_r18)::has_ext,
     false,
     decltype(rach_less_ho_r18_s::target_nta_r18)::nof_types,
     decltype(rach_less_ho_r18_s::target_nta_r18)::nof_exts,
     nullptr,
     &rach_less_ho_r18_s::target_nta_r18_opts::desc},
    {offsetof(rach_less_ho_r18_s, beam_ind_r18),
     (int32_t)offsetof(rach_less_ho_r18_s, beam_ind_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rach_less_ho_r18_s::beam_ind_r18_c_::desc}};
static const asn1::table::field_name seq_table_rach_less_ho_r18_s_names[] = {{"targetNTA-r18", nullptr},
                                                                             {"beamIndication-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_rach_less_ho_r18_s_opts[]  = {
    {0, (int32_t)offsetof(rach_less_ho_r18_s, target_nta_r18_present)},
    {1, (int32_t)offsetof(rach_less_ho_r18_s, beam_ind_r18_present)}};
const asn1::table::seq_desc rach_less_ho_r18_s::desc = {{seq_table_rach_less_ho_r18_s_fields,
                                                         uint64_t{0x0},
                                                         seq_table_rach_less_ho_r18_s_opts,
                                                         seq_table_rach_less_ho_r18_s_names,
                                                         2,
                                                         2},
                                                        true,
                                                        false,
                                                        nullptr,
                                                        0};

static const char*           enum_names_rach_less_ho_r18_s_target_nta_r18_e__values[] = {"zero", "source"};
const asn1::table::enum_desc rach_less_ho_r18_s::target_nta_r18_opts::desc            = {
    enum_names_rach_less_ho_r18_s_target_nta_r18_e__values,
    2,
    "rach_less_ho_r18_s::target_nta_r18_e_"};
uint8_t rach_less_ho_r18_s::target_nta_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {0};
  return map_enum_number(numbers, 1, value, "rach_less_ho_r18_s::target_nta_r18_e_");
}

static const asn1::table::storage_ops* const choice_storage_rach_less_ho_r18_s_beam_ind_r18_c__alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc rach_less_ho_r18_s::beam_ind_r18_c_::storage_desc = {
    choice_storage_rach_less_ho_r18_s_beam_ind_r18_c__alternatives,
    2,
    "rach_less_ho_r18_s::beam_ind_r18_c_"};

uint8_t& rach_less_ho_r18_s::beam_ind_r18_c_::set_tci_state_id_r18()
{
  set(types::tci_state_id_r18);
  return c.get<uint8_t>();
}
uint8_t& rach_less_ho_r18_s::beam_ind_r18_c_::set_ssb_idx_r18()
{
  set(types::ssb_idx_r18);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_rach_less_ho_r18_s_beam_ind_r18_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{63},
     nullptr}};
static const asn1::table::field_name choice_table_rach_less_ho_r18_s_beam_ind_r18_c__names[] = {
    {"tci-StateID-r18", nullptr},
    {"ssb-Index-r18", nullptr}};
const asn1::table::choice_desc rach_less_ho_r18_s::beam_ind_r18_c_::desc = {
    asn1::table::enum_int_type<rach_less_ho_r18_s::beam_ind_r18_c_::types>(),
    rach_less_ho_r18_s::beam_ind_r18_c_::types::nof_types,
    rach_less_ho_r18_s::beam_ind_r18_c_::types::nof_exts,
    rach_less_ho_r18_s::beam_ind_r18_c_::types::has_ext,
    false,
    choice_table_rach_less_ho_r18_s_beam_ind_r18_c__alternatives,
    nullptr,
    choice_table_rach_less_ho_r18_s_beam_ind_r18_c__names,
    "rach_less_ho_r18_s::beam_ind_r18_c_",
    offsetof(rach_less_ho_r18_s::beam_ind_r18_c_, c.type),
    offsetof(rach_less_ho_r18_s::beam_ind_r18_c_, c.value),
    false,
    &rach_less_ho_r18_s::beam_ind_r18_c_::storage_desc};

static const char* enum_names_rach_less_ho_r18_s_beam_ind_r18_c__types_values[] = {"tci-StateID-r18", "ssb-Index-r18"};
const asn1::table::enum_desc rach_less_ho_r18_s::beam_ind_r18_c_::types_opts::desc = {
    enum_names_rach_less_ho_r18_s_beam_ind_r18_c__types_values,
    2,
    "rach_less_ho_r18_s::beam_ind_r18_c_::types"};

// ReconfigurationWithSync ::= SEQUENCE
static const asn1::table::seq_field seq_table_recfg_with_sync_s_fields[] = {
    {offsetof(recfg_with_sync_s, sp_cell_cfg_common),
     (int32_t)offsetof(recfg_with_sync_s, sp_cell_cfg_common_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &serving_cell_cfg_common_s::desc},
    {offsetof(recfg_with_sync_s, new_ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(recfg_with_sync_s, t304),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(recfg_with_sync_s::t304)>(),
     decltype(recfg_with_sync_s::t304)::has_ext,
     false,
     decltype(recfg_with_sync_s::t304)::nof_types,
     decltype(recfg_with_sync_s::t304)::nof_exts,
     nullptr,
     &recfg_with_sync_s::t304_opts::desc},
    {offsetof(recfg_with_sync_s, rach_cfg_ded),
     (int32_t)offsetof(recfg_with_sync_s, rach_cfg_ded_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &recfg_with_sync_s::rach_cfg_ded_c_::desc}};
static const asn1::table::field_name seq_table_recfg_with_sync_s_names[] = {{"spCellConfigCommon", nullptr},
                                                                            {"newUE-Identity", nullptr},
                                                                            {"t304", nullptr},
                                                                            {"rach-ConfigDedicated", nullptr}};
static const asn1::table::seq_opt    seq_table_recfg_with_sync_s_opts[]  = {
    {0, (int32_t)offsetof(recfg_with_sync_s, sp_cell_cfg_common_present)},
    {3, (int32_t)offsetof(recfg_with_sync_s, rach_cfg_ded_present)}};
static const asn1::table::seq_field seq_table_recfg_with_sync_s_g0_fields[] = {
    {offsetof(recfg_with_sync_s, smtc),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(recfg_with_sync_s::smtc)>::value,
     &ssb_mtc_s::desc}};
static const asn1::table::field_name seq_table_recfg_with_sync_s_g0_names[] = {{"smtc", nullptr}};
static const asn1::table::seq_opt   seq_table_recfg_with_sync_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_recfg_with_sync_s_g1_fields[] = {
    {offsetof(recfg_with_sync_s, daps_ul_pwr_cfg_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(recfg_with_sync_s::daps_ul_pwr_cfg_r16)>::value,
     &daps_ul_pwr_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_recfg_with_sync_s_g1_names[] = {{"daps-UplinkPowerConfig-r16", nullptr}};
static const asn1::table::seq_opt   seq_table_recfg_with_sync_s_g1_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_recfg_with_sync_s_g2_fields[] = {
    {offsetof(recfg_with_sync_s, sl_path_switch_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(recfg_with_sync_s::sl_path_switch_cfg_r17)>::value,
     &sl_path_switch_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_recfg_with_sync_s_g2_names[] = {{"sl-PathSwitchConfig-r17", nullptr}};
static const asn1::table::seq_opt   seq_table_recfg_with_sync_s_g2_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_recfg_with_sync_s_g3_fields[] = {
    {offsetof(recfg_with_sync_s, rach_less_ho_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(recfg_with_sync_s::rach_less_ho_r18)>::value,
     &rach_less_ho_r18_s::desc},
    {0,
     (int32_t)offsetof(recfg_with_sync_s, sl_indirect_path_maintain_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_recfg_with_sync_s_g3_names[] = {{"rach-LessHO-r18", nullptr},
                                                                               {"sl-IndirectPathMaintain-r18", "true"}};
static const asn1::table::seq_opt    seq_table_recfg_with_sync_s_g3_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(recfg_with_sync_s, sl_indirect_path_maintain_r18_present)}};
static const asn1::table::seq_group seq_table_recfg_with_sync_s_groups[] = {{{seq_table_recfg_with_sync_s_g0_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_recfg_with_sync_s_g0_opts,
                                                                              seq_table_recfg_with_sync_s_g0_names,
                                                                              1,
                                                                              1},
                                                                             false},
                                                                            {{seq_table_recfg_with_sync_s_g1_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_recfg_with_sync_s_g1_opts,
                                                                              seq_table_recfg_with_sync_s_g1_names,
                                                                              1,
                                                                              1},
                                                                             false},
                                                                            {{seq_table_recfg_with_sync_s_g2_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_recfg_with_sync_s_g2_opts,
                                                                              seq_table_recfg_with_sync_s_g2_names,
                                                                              1,
                                                                              1},
                                                                             false},
                                                                            {{seq_table_recfg_with_sync_s_g3_fields,
                                                                              uint64_t{0x0},
                                                                              seq_table_recfg_with_sync_s_g3_opts,
                                                                              seq_table_recfg_with_sync_s_g3_names,
                                                                              2,
                                                                              2},
                                                                             false}};
const asn1::table::seq_desc         recfg_with_sync_s::desc              = {{seq_table_recfg_with_sync_s_fields,
                                                                             uint64_t{0x6},
                                                                             seq_table_recfg_with_sync_s_opts,
                                                                             seq_table_recfg_with_sync_s_names,
                                                                             4,
                                                                             2},
                                                                            true,
                                                                            false,
                                                                            seq_table_recfg_with_sync_s_groups,
                                                                            4};

static const char* enum_names_recfg_with_sync_s_t304_e__values[] =
    {"ms50", "ms100", "ms150", "ms200", "ms500", "ms1000", "ms2000", "ms10000"};
const asn1::table::enum_desc recfg_with_sync_s::t304_opts::desc = {enum_names_recfg_with_sync_s_t304_e__values,
                                                                   8,
                                                                   "recfg_with_sync_s::t304_e_"};
uint16_t                     recfg_with_sync_s::t304_opts::to_number() const
{
  static const uint16_t numbers[] = {50, 100, 150, 200, 500, 1000, 2000, 10000};
  return map_enum_number(numbers, 8, value, "recfg_with_sync_s::t304_e_");
}

static const asn1::table::storage_ops* const choice_storage_recfg_with_sync_s_rach_cfg_ded_c__alternatives[] = {
    &asn1::table::alternative_ops<rach_cfg_ded_s, false>::value,
    &asn1::table::alternative_ops<rach_cfg_ded_s, false>::value};
const asn1::table::choice_storage_desc recfg_with_sync_s::rach_cfg_ded_c_::storage_desc = {
    choice_storage_recfg_with_sync_s_rach_cfg_ded_c__alternatives,
    2,
    "recfg_with_sync_s::rach_cfg_ded_c_"};

rach_cfg_ded_s& recfg_with_sync_s::rach_cfg_ded_c_::set_ul()
{
  set(types::ul);
  return c.get<rach_cfg_ded_s>();
}
rach_cfg_ded_s& recfg_with_sync_s::rach_cfg_ded_c_::set_supplementary_ul()
{
  set(types::supplementary_ul);
  return c.get<rach_cfg_ded_s>();
}
static const asn1::table::seq_field choice_table_recfg_with_sync_s_rach_cfg_ded_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rach_cfg_ded_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rach_cfg_ded_s::desc}};
static const asn1::table::field_name choice_table_recfg_with_sync_s_rach_cfg_ded_c__names[] = {
    {"uplink", nullptr},
    {"supplementaryUplink", nullptr}};
const asn1::table::choice_desc recfg_with_sync_s::rach_cfg_ded_c_::desc = {
    asn1::table::enum_int_type<recfg_with_sync_s::rach_cfg_ded_c_::types>(),
    recfg_with_sync_s::rach_cfg_ded_c_::types::nof_types,
    recfg_with_sync_s::rach_cfg_ded_c_::types::nof_exts,
    recfg_with_sync_s::rach_cfg_ded_c_::types::has_ext,
    false,
    choice_table_recfg_with_sync_s_rach_cfg_ded_c__alternatives,
    nullptr,
    choice_table_recfg_with_sync_s_rach_cfg_ded_c__names,
    "recfg_with_sync_s::rach_cfg_ded_c_",
    offsetof(recfg_with_sync_s::rach_cfg_ded_c_, c.type),
    offsetof(recfg_with_sync_s::rach_cfg_ded_c_, c.value),
    false,
    &recfg_with_sync_s::rach_cfg_ded_c_::storage_desc};

static const char* enum_names_recfg_with_sync_s_rach_cfg_ded_c__types_values[]    = {"uplink", "supplementaryUplink"};
const asn1::table::enum_desc recfg_with_sync_s::rach_cfg_ded_c_::types_opts::desc = {
    enum_names_recfg_with_sync_s_rach_cfg_ded_c__types_values,
    2,
    "recfg_with_sync_s::rach_cfg_ded_c_::types"};

// RLF-TimersAndConstants ::= SEQUENCE
static const asn1::table::seq_field seq_table_rlf_timers_and_consts_s_fields[] = {
    {offsetof(rlf_timers_and_consts_s, t310),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rlf_timers_and_consts_s::t310)>(),
     decltype(rlf_timers_and_consts_s::t310)::has_ext,
     false,
     decltype(rlf_timers_and_consts_s::t310)::nof_types,
     decltype(rlf_timers_and_consts_s::t310)::nof_exts,
     nullptr,
     &rlf_timers_and_consts_s::t310_opts::desc},
    {offsetof(rlf_timers_and_consts_s, n310),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rlf_timers_and_consts_s::n310)>(),
     decltype(rlf_timers_and_consts_s::n310)::has_ext,
     false,
     decltype(rlf_timers_and_consts_s::n310)::nof_types,
     decltype(rlf_timers_and_consts_s::n310)::nof_exts,
     nullptr,
     &rlf_timers_and_consts_s::n310_opts::desc},
    {offsetof(rlf_timers_and_consts_s, n311),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rlf_timers_and_consts_s::n311)>(),
     decltype(rlf_timers_and_consts_s::n311)::has_ext,
     false,
     decltype(rlf_timers_and_consts_s::n311)::nof_types,
     decltype(rlf_timers_and_consts_s::n311)::nof_exts,
     nullptr,
     &rlf_timers_and_consts_s::n311_opts::desc}};
static const asn1::table::field_name seq_table_rlf_timers_and_consts_s_names[]     = {{"t310", nullptr},
                                                                                      {"n310", nullptr},
                                                                                      {"n311", nullptr}};
static const asn1::table::seq_field  seq_table_rlf_timers_and_consts_s_g0_fields[] = {
    {offsetof(rlf_timers_and_consts_s, t311),
      asn1::table::seq_field::ptr_mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_codec<decltype(rlf_timers_and_consts_s::t311), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_rlf_timers_and_consts_s_g0_names[] = {{"t311", nullptr}};
static const asn1::table::seq_group  seq_table_rlf_timers_and_consts_s_groups[]   = {
    {{seq_table_rlf_timers_and_consts_s_g0_fields,
         uint64_t{0x1},
         nullptr,
         seq_table_rlf_timers_and_consts_s_g0_names,
         1,
         0},
        false}};
const asn1::table::seq_desc rlf_timers_and_consts_s::desc = {
    {seq_table_rlf_timers_and_consts_s_fields, uint64_t{0x7}, nullptr, seq_table_rlf_timers_and_consts_s_names, 3, 0},
    true,
    false,
    seq_table_rlf_timers_and_consts_s_groups,
    1};

static const char* enum_names_rlf_timers_and_consts_s_t310_e__values[] =
    {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000", "ms4000", "ms6000"};
const asn1::table::enum_desc rlf_timers_and_consts_s::t310_opts::desc = {
    enum_names_rlf_timers_and_consts_s_t310_e__values,
    9,
    "rlf_timers_and_consts_s::t310_e_"};
uint16_t rlf_timers_and_consts_s::t310_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 50, 100, 200, 500, 1000, 2000, 4000, 6000};
  return map_enum_number(numbers, 9, value, "rlf_timers_and_consts_s::t310_e_");
}

static const char* enum_names_rlf_timers_and_consts_s_n310_e__values[] =
    {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
const asn1::table::enum_desc rlf_timers_and_consts_s::n310_opts::desc = {
    enum_names_rlf_timers_and_consts_s_n310_e__values,
    8,
    "rlf_timers_and_consts_s::n310_e_"};
uint8_t rlf_timers_and_consts_s::n310_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(numbers, 8, value, "rlf_timers_and_consts_s::n310_e_");
}

static const char* enum_names_rlf_timers_and_consts_s_n311_e__values[] =
    {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
const asn1::table::enum_desc rlf_timers_and_consts_s::n311_opts::desc = {
    enum_names_rlf_timers_and_consts_s_n311_e__values,
    8,
    "rlf_timers_and_consts_s::n311_e_"};
uint8_t rlf_timers_and_consts_s::n311_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(numbers, 8, value, "rlf_timers_and_consts_s::n311_e_");
}

static const char* enum_names_rlf_timers_and_consts_s_t311_e__values[] =
    {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
const asn1::table::enum_desc rlf_timers_and_consts_s::t311_opts::desc = {
    enum_names_rlf_timers_and_consts_s_t311_e__values,
    7,
    "rlf_timers_and_consts_s::t311_e_"};
uint16_t rlf_timers_and_consts_s::t311_opts::to_number() const
{
  static const uint16_t numbers[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(numbers, 7, value, "rlf_timers_and_consts_s::t311_e_");
}

// GoodServingCellEvaluation-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_good_serving_cell_eval_r17_s_fields[] = {
    {offsetof(good_serving_cell_eval_r17_s, offset_r17),
     (int32_t)offsetof(good_serving_cell_eval_r17_s, offset_r17_present),
     asn1::table::enum_op<decltype(good_serving_cell_eval_r17_s::offset_r17)>(),
     decltype(good_serving_cell_eval_r17_s::offset_r17)::has_ext,
     false,
     decltype(good_serving_cell_eval_r17_s::offset_r17)::nof_types,
     decltype(good_serving_cell_eval_r17_s::offset_r17)::nof_exts,
     nullptr,
     &good_serving_cell_eval_r17_s::offset_r17_opts::desc}};
static const asn1::table::field_name seq_table_good_serving_cell_eval_r17_s_names[] = {{"offset-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_good_serving_cell_eval_r17_s_opts[]  = {
    {0, (int32_t)offsetof(good_serving_cell_eval_r17_s, offset_r17_present)}};
const asn1::table::seq_desc good_serving_cell_eval_r17_s::desc = {{seq_table_good_serving_cell_eval_r17_s_fields,
                                                                   uint64_t{0x0},
                                                                   seq_table_good_serving_cell_eval_r17_s_opts,
                                                                   seq_table_good_serving_cell_eval_r17_s_names,
                                                                   1,
                                                                   1},
                                                                  false,
                                                                  false,
                                                                  nullptr,
                                                                  0};

static const char* enum_names_good_serving_cell_eval_r17_s_offset_r17_e__values[] = {"db2", "db4", "db6", "db8"};
const asn1::table::enum_desc good_serving_cell_eval_r17_s::offset_r17_opts::desc  = {
    enum_names_good_serving_cell_eval_r17_s_offset_r17_e__values,
    4,
    "good_serving_cell_eval_r17_s::offset_r17_e_"};
uint8_t good_serving_cell_eval_r17_s::offset_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8};
  return map_enum_number(numbers, 4, value, "good_serving_cell_eval_r17_s::offset_r17_e_");
}

// DeactivatedSCG-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_deactiv_scg_cfg_r17_s_fields[] = {
    {offsetof(deactiv_scg_cfg_r17_s, bfd_and_rlm_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_deactiv_scg_cfg_r17_s_names[] = {{"bfd-and-RLM-r17", nullptr}};
const asn1::table::seq_desc          deactiv_scg_cfg_r17_s::desc             = {
    {seq_table_deactiv_scg_cfg_r17_s_fields, uint64_t{0x1}, nullptr, seq_table_deactiv_scg_cfg_r17_s_names, 1, 0},
    true,
    false,
    nullptr,
    0};

// SpCellConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_sp_cell_cfg_s_low_mob_eval_connected_r17_s__fields[] = {
    {offsetof(sp_cell_cfg_s::low_mob_eval_connected_r17_s_, s_search_delta_p_connected_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17)>(),
     decltype(sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17)::has_ext,
     false,
     decltype(sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17)::nof_types,
     decltype(sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17)::nof_exts,
     nullptr,
     &sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17_opts::desc},
    {offsetof(sp_cell_cfg_s::low_mob_eval_connected_r17_s_, t_search_delta_p_connected_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17)>(),
     decltype(sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17)::has_ext,
     false,
     decltype(sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17)::nof_types,
     decltype(sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17)::nof_exts,
     nullptr,
     &sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17_opts::desc}};
static const asn1::table::field_name seq_table_sp_cell_cfg_s_low_mob_eval_connected_r17_s__names[] = {
    {"s-SearchDeltaP-Connected-r17", nullptr},
    {"t-SearchDeltaP-Connected-r17", nullptr}};
static const asn1::table::seq_desc seq_table_sp_cell_cfg_s_low_mob_eval_connected_r17_s_ = {
    {seq_table_sp_cell_cfg_s_low_mob_eval_connected_r17_s__fields,
     uint64_t{0x3},
     nullptr,
     seq_table_sp_cell_cfg_s_low_mob_eval_connected_r17_s__names,
     2,
     0},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_sp_cell_cfg_s_rlf_timers_and_consts_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rlf_timers_and_consts_s::desc}};
static const asn1::table::field_name seq_table_sp_cell_cfg_s_rlf_timers_and_consts_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sp_cell_cfg_s_rlf_timers_and_consts_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sp_cell_cfg_s_rlf_timers_and_consts_setup_release_alternatives,
    nullptr,
    seq_table_sp_cell_cfg_s_rlf_timers_and_consts_setup_release_names,
    "setup_release_c",
    setup_release_c<rlf_timers_and_consts_s>::type_offset(),
    setup_release_c<rlf_timers_and_consts_s>::value_offset()};
static const asn1::table::seq_field seq_table_sp_cell_cfg_s_deactiv_scg_cfg_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &deactiv_scg_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sp_cell_cfg_s_deactiv_scg_cfg_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sp_cell_cfg_s_deactiv_scg_cfg_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sp_cell_cfg_s_deactiv_scg_cfg_r17_setup_release_alternatives,
    nullptr,
    seq_table_sp_cell_cfg_s_deactiv_scg_cfg_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<deactiv_scg_cfg_r17_s>::type_offset(),
    setup_release_c<deactiv_scg_cfg_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_sp_cell_cfg_s_fields[] = {
    {offsetof(sp_cell_cfg_s, serv_cell_idx),
     (int32_t)offsetof(sp_cell_cfg_s, serv_cell_idx_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr},
    {offsetof(sp_cell_cfg_s, recfg_with_sync),
     (int32_t)offsetof(sp_cell_cfg_s, recfg_with_sync_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &recfg_with_sync_s::desc},
    {offsetof(sp_cell_cfg_s, rlf_timers_and_consts),
     (int32_t)offsetof(sp_cell_cfg_s, rlf_timers_and_consts_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sp_cell_cfg_s_rlf_timers_and_consts_setup_release},
    {0,
     (int32_t)offsetof(sp_cell_cfg_s, rlm_in_sync_out_of_sync_thres_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sp_cell_cfg_s, sp_cell_cfg_ded),
     (int32_t)offsetof(sp_cell_cfg_s, sp_cell_cfg_ded_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &serving_cell_cfg_s::desc}};
static const asn1::table::field_name seq_table_sp_cell_cfg_s_names[] = {{"servCellIndex", nullptr},
                                                                        {"reconfigurationWithSync", nullptr},
                                                                        {"rlf-TimersAndConstants", nullptr},
                                                                        {"rlmInSyncOutOfSyncThreshold", "n1"},
                                                                        {"spCellConfigDedicated", nullptr}};
static const asn1::table::seq_opt    seq_table_sp_cell_cfg_s_opts[]  = {
    {0, (int32_t)offsetof(sp_cell_cfg_s, serv_cell_idx_present)},
    {1, (int32_t)offsetof(sp_cell_cfg_s, recfg_with_sync_present)},
    {2, (int32_t)offsetof(sp_cell_cfg_s, rlf_timers_and_consts_present)},
    {3, (int32_t)offsetof(sp_cell_cfg_s, rlm_in_sync_out_of_sync_thres_present)},
    {4, (int32_t)offsetof(sp_cell_cfg_s, sp_cell_cfg_ded_present)}};
static const asn1::table::seq_field seq_table_sp_cell_cfg_s_g0_fields[] = {
    {offsetof(sp_cell_cfg_s, low_mob_eval_connected_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sp_cell_cfg_s::low_mob_eval_connected_r17)>::value,
     &seq_table_sp_cell_cfg_s_low_mob_eval_connected_r17_s_},
    {offsetof(sp_cell_cfg_s, good_serving_cell_eval_rlm_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sp_cell_cfg_s::good_serving_cell_eval_rlm_r17)>::value,
     &good_serving_cell_eval_r17_s::desc},
    {offsetof(sp_cell_cfg_s, good_serving_cell_eval_bfd_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sp_cell_cfg_s::good_serving_cell_eval_bfd_r17)>::value,
     &good_serving_cell_eval_r17_s::desc},
    {offsetof(sp_cell_cfg_s, deactiv_scg_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sp_cell_cfg_s::deactiv_scg_cfg_r17)>::value,
     &seq_table_sp_cell_cfg_s_deactiv_scg_cfg_r17_setup_release}};
static const asn1::table::field_name seq_table_sp_cell_cfg_s_g0_names[] = {
    {"lowMobilityEvaluationConnected-r17", nullptr},
    {"goodServingCellEvaluationRLM-r17", nullptr},
    {"goodServingCellEvaluationBFD-r17", nullptr},
    {"deactivatedSCG-Config-r17", nullptr}};
static const asn1::table::seq_opt   seq_table_sp_cell_cfg_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence},
                                                                         {1, asn1::table::seq_field::ptr_presence},
                                                                         {2, asn1::table::seq_field::ptr_presence},
                                                                         {3, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sp_cell_cfg_s_groups[]  = {{{seq_table_sp_cell_cfg_s_g0_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_sp_cell_cfg_s_g0_opts,
                                                                           seq_table_sp_cell_cfg_s_g0_names,
                                                                           4,
                                                                           4},
                                                                          false}};
const asn1::table::seq_desc         sp_cell_cfg_s::desc               = {
    {seq_table_sp_cell_cfg_s_fields, uint64_t{0x0}, seq_table_sp_cell_cfg_s_opts, seq_table_sp_cell_cfg_s_names, 5, 5},
    true,
    false,
    seq_table_sp_cell_cfg_s_groups,
    1};

static const char* enum_names_sp_cell_cfg_s_low_mob_eval_connected_r17_s__s_search_delta_p_connected_r17_e__values[] =
    {"dB3", "dB6", "dB9", "dB12", "dB15", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17_opts::desc = {
    enum_names_sp_cell_cfg_s_low_mob_eval_connected_r17_s__s_search_delta_p_connected_r17_e__values,
    8,
    "sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17_e_"};
uint8_t sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 6, 9, 12, 15};
  return map_enum_number(
      numbers, 5, value, "sp_cell_cfg_s::low_mob_eval_connected_r17_s_::s_search_delta_p_connected_r17_e_");
}

static const char* enum_names_sp_cell_cfg_s_low_mob_eval_connected_r17_s__t_search_delta_p_connected_r17_e__values[] = {
    "s5",
    "s10",
    "s20",
    "s30",
    "s60",
    "s120",
    "s180",
    "s240",
    "s300",
    "spare7",
    "spare6",
    "spare5",
    "spare4",
    "spare3",
    "spare2",
    "spare1"};
const asn1::table::enum_desc sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17_opts::desc = {
    enum_names_sp_cell_cfg_s_low_mob_eval_connected_r17_s__t_search_delta_p_connected_r17_e__values,
    16,
    "sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17_e_"};
uint16_t sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 30, 60, 120, 180, 240, 300};
  return map_enum_number(
      numbers, 9, value, "sp_cell_cfg_s::low_mob_eval_connected_r17_s_::t_search_delta_p_connected_r17_e_");
}

// SCellConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_scell_cfg_s_scell_sib20_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<setup_release_c<dyn_octstring>::value_type, asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_scell_cfg_s_scell_sib20_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_scell_cfg_s_scell_sib20_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_scell_cfg_s_scell_sib20_r17_setup_release_alternatives,
    nullptr,
    seq_table_scell_cfg_s_scell_sib20_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<dyn_octstring>::type_offset(),
    setup_release_c<dyn_octstring>::value_offset()};
static const asn1::table::seq_field seq_table_scell_cfg_s_plmn_id_info_list_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     11,
     &asn1::table::item_list_codec<setup_release_c<dyn_seq_of<plmn_id_info_s, 1, 12>>::value_type,
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &plmn_id_info_s::desc}};
static const asn1::table::field_name seq_table_scell_cfg_s_plmn_id_info_list_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_scell_cfg_s_plmn_id_info_list_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_scell_cfg_s_plmn_id_info_list_r17_setup_release_alternatives,
    nullptr,
    seq_table_scell_cfg_s_plmn_id_info_list_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<dyn_seq_of<plmn_id_info_s, 1, 12>>::type_offset(),
    setup_release_c<dyn_seq_of<plmn_id_info_s, 1, 12>>::value_offset()};
static const asn1::table::seq_field seq_table_scell_cfg_s_npn_id_info_list_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     11,
     &asn1::table::item_list_codec<setup_release_c<dyn_seq_of<npn_id_info_r16_s, 1, 12>>::value_type,
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &npn_id_info_r16_s::desc}};
static const asn1::table::field_name seq_table_scell_cfg_s_npn_id_info_list_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_scell_cfg_s_npn_id_info_list_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_scell_cfg_s_npn_id_info_list_r17_setup_release_alternatives,
    nullptr,
    seq_table_scell_cfg_s_npn_id_info_list_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<dyn_seq_of<npn_id_info_r16_s, 1, 12>>::type_offset(),
    setup_release_c<dyn_seq_of<npn_id_info_r16_s, 1, 12>>::value_offset()};
static const asn1::table::seq_field seq_table_scell_cfg_s_fields[] = {
    {offsetof(scell_cfg_s, scell_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{30},
     nullptr},
    {offsetof(scell_cfg_s, scell_cfg_common),
     (int32_t)offsetof(scell_cfg_s, scell_cfg_common_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &serving_cell_cfg_common_s::desc},
    {offsetof(scell_cfg_s, scell_cfg_ded),
     (int32_t)offsetof(scell_cfg_s, scell_cfg_ded_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &serving_cell_cfg_s::desc}};
static const asn1::table::field_name seq_table_scell_cfg_s_names[] = {{"sCellIndex", nullptr},
                                                                      {"sCellConfigCommon", nullptr},
                                                                      {"sCellConfigDedicated", nullptr}};
static const asn1::table::seq_opt    seq_table_scell_cfg_s_opts[]  = {
    {1, (int32_t)offsetof(scell_cfg_s, scell_cfg_common_present)},
    {2, (int32_t)offsetof(scell_cfg_s, scell_cfg_ded_present)}};
static const asn1::table::seq_field seq_table_scell_cfg_s_g0_fields[] = {
    {offsetof(scell_cfg_s, smtc),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(scell_cfg_s::smtc)>::value,
     &ssb_mtc_s::desc}};
static const asn1::table::field_name seq_table_scell_cfg_s_g0_names[]  = {{"smtc", nullptr}};
static const asn1::table::seq_opt    seq_table_scell_cfg_s_g0_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_scell_cfg_s_g1_fields[] = {
    {0,
      (int32_t)offsetof(scell_cfg_s, scell_state_r16_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr},
    {0,
      (int32_t)offsetof(scell_cfg_s, secondary_drx_group_cfg_r16_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr}};
static const asn1::table::field_name seq_table_scell_cfg_s_g1_names[] = {{"sCellState-r16", "activated"},
                                                                         {"secondaryDRX-GroupConfig-r16", "true"}};
static const asn1::table::seq_opt    seq_table_scell_cfg_s_g1_opts[]  = {
    {0, (int32_t)offsetof(scell_cfg_s, scell_state_r16_present)},
    {1, (int32_t)offsetof(scell_cfg_s, secondary_drx_group_cfg_r16_present)}};
static const asn1::table::seq_field seq_table_scell_cfg_s_g2_fields[] = {
    {offsetof(scell_cfg_s, pre_conf_gap_status_r17),
     (int32_t)offsetof(scell_cfg_s, pre_conf_gap_status_r17_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(scell_cfg_s::pre_conf_gap_status_r17), asn1::table::struct_json>::value},
    {offsetof(scell_cfg_s, good_serving_cell_eval_bfd_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(scell_cfg_s::good_serving_cell_eval_bfd_r17)>::value,
     &good_serving_cell_eval_r17_s::desc},
    {offsetof(scell_cfg_s, scell_sib20_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(scell_cfg_s::scell_sib20_r17)>::value,
     &seq_table_scell_cfg_s_scell_sib20_r17_setup_release}};
static const asn1::table::field_name seq_table_scell_cfg_s_g2_names[] = {{"preConfGapStatus-r17", nullptr},
                                                                         {"goodServingCellEvaluationBFD-r17", nullptr},
                                                                         {"sCellSIB20-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_scell_cfg_s_g2_opts[]  = {
    {0, (int32_t)offsetof(scell_cfg_s, pre_conf_gap_status_r17_present)},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_scell_cfg_s_g3_fields[] = {
    {offsetof(scell_cfg_s, plmn_id_info_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(scell_cfg_s::plmn_id_info_list_r17)>::value,
     &seq_table_scell_cfg_s_plmn_id_info_list_r17_setup_release},
    {offsetof(scell_cfg_s, npn_id_info_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(scell_cfg_s::npn_id_info_list_r17)>::value,
     &seq_table_scell_cfg_s_npn_id_info_list_r17_setup_release}};
static const asn1::table::field_name seq_table_scell_cfg_s_g3_names[] = {{"plmn-IdentityInfoList-r17", nullptr},
                                                                         {"npn-IdentityInfoList-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_scell_cfg_s_g3_opts[]  = {{0, asn1::table::seq_field::ptr_presence},
                                                                         {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_scell_cfg_s_groups[]   = {{{seq_table_scell_cfg_s_g0_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_scell_cfg_s_g0_opts,
                                                                           seq_table_scell_cfg_s_g0_names,
                                                                           1,
                                                                           1},
                                                                          false},
                                                                         {{seq_table_scell_cfg_s_g1_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_scell_cfg_s_g1_opts,
                                                                           seq_table_scell_cfg_s_g1_names,
                                                                           2,
                                                                           2},
                                                                          false},
                                                                         {{seq_table_scell_cfg_s_g2_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_scell_cfg_s_g2_opts,
                                                                           seq_table_scell_cfg_s_g2_names,
                                                                           3,
                                                                           3},
                                                                          false},
                                                                         {{seq_table_scell_cfg_s_g3_fields,
                                                                           uint64_t{0x0},
                                                                           seq_table_scell_cfg_s_g3_opts,
                                                                           seq_table_scell_cfg_s_g3_names,
                                                                           2,
                                                                           2},
                                                                          false}};
const asn1::table::seq_desc          scell_cfg_s::desc                = {
    {seq_table_scell_cfg_s_fields, uint64_t{0x1}, seq_table_scell_cfg_s_opts, seq_table_scell_cfg_s_names, 3, 2},
    true,
    false,
    seq_table_scell_cfg_s_groups,
    4};

// Uu-RelayRLC-ChannelConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_uu_relay_rlc_ch_cfg_r17_s_fields[] = {
    {offsetof(uu_relay_rlc_ch_cfg_r17_s, uu_lc_ch_id_r17),
     (int32_t)offsetof(uu_relay_rlc_ch_cfg_r17_s, uu_lc_ch_id_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr},
    {offsetof(uu_relay_rlc_ch_cfg_r17_s, uu_relay_rlc_ch_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr},
    {0,
     (int32_t)offsetof(uu_relay_rlc_ch_cfg_r17_s, reestablish_rlc_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(uu_relay_rlc_ch_cfg_r17_s, rlc_cfg_r17),
     (int32_t)offsetof(uu_relay_rlc_ch_cfg_r17_s, rlc_cfg_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rlc_cfg_c::desc},
    {offsetof(uu_relay_rlc_ch_cfg_r17_s, mac_lc_ch_cfg_r17),
     (int32_t)offsetof(uu_relay_rlc_ch_cfg_r17_s, mac_lc_ch_cfg_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &lc_ch_cfg_s::desc}};
static const asn1::table::field_name seq_table_uu_relay_rlc_ch_cfg_r17_s_names[] = {
    {"uu-LogicalChannelIdentity-r17", nullptr},
    {"uu-RelayRLC-ChannelID-r17", nullptr},
    {"reestablishRLC-r17", "true"},
    {"rlc-Config-r17", nullptr},
    {"mac-LogicalChannelConfig-r17", nullptr}};
static const asn1::table::seq_opt seq_table_uu_relay_rlc_ch_cfg_r17_s_opts[] = {
    {0, (int32_t)offsetof(uu_relay_rlc_ch_cfg_r17_s, uu_lc_ch_id_r17_present)},
    {2, (int32_t)offsetof(uu_relay_rlc_ch_cfg_r17_s, reestablish_rlc_r17_present)},
    {3, (int32_t)offsetof(uu_relay_rlc_ch_cfg_r17_s, rlc_cfg_r17_present)},
    {4, (int32_t)offsetof(uu_relay_rlc_ch_cfg_r17_s, mac_lc_ch_cfg_r17_present)}};
const asn1::table::seq_desc uu_relay_rlc_ch_cfg_r17_s::desc = {{seq_table_uu_relay_rlc_ch_cfg_r17_s_fields,
                                                                uint64_t{0x2},
                                                                seq_table_uu_relay_rlc_ch_cfg_r17_s_opts,
                                                                seq_table_uu_relay_rlc_ch_cfg_r17_s_names,
                                                                5,
                                                                4},
                                                               true,
                                                               false,
                                                               nullptr,
                                                               0};

// IAB-ResourceConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_iab_res_cfg_r17_s_fields[] = {
    {offsetof(iab_res_cfg_r17_s, iab_res_cfg_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(iab_res_cfg_r17_s, slot_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     5119,
     &asn1::table::list_codec<decltype(iab_res_cfg_r17_s::slot_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{5119}, false, false>>::value},
    {offsetof(iab_res_cfg_r17_s, periodicity_slot_list_r17),
     (int32_t)offsetof(iab_res_cfg_r17_s, periodicity_slot_list_r17_present),
     asn1::table::enum_op<decltype(iab_res_cfg_r17_s::periodicity_slot_list_r17)>(),
     decltype(iab_res_cfg_r17_s::periodicity_slot_list_r17)::has_ext,
     false,
     decltype(iab_res_cfg_r17_s::periodicity_slot_list_r17)::nof_types,
     decltype(iab_res_cfg_r17_s::periodicity_slot_list_r17)::nof_exts,
     nullptr,
     &iab_res_cfg_r17_s::periodicity_slot_list_r17_opts::desc},
    {offsetof(iab_res_cfg_r17_s, slot_list_subcarrier_spacing_r17),
     (int32_t)offsetof(iab_res_cfg_r17_s, slot_list_subcarrier_spacing_r17_present),
     asn1::table::enum_op<decltype(iab_res_cfg_r17_s::slot_list_subcarrier_spacing_r17)>(),
     decltype(iab_res_cfg_r17_s::slot_list_subcarrier_spacing_r17)::has_ext,
     false,
     decltype(iab_res_cfg_r17_s::slot_list_subcarrier_spacing_r17)::nof_types,
     decltype(iab_res_cfg_r17_s::slot_list_subcarrier_spacing_r17)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc}};
static const asn1::table::field_name seq_table_iab_res_cfg_r17_s_names[] = {{"iab-ResourceConfigID-r17", nullptr},
                                                                            {"slotList-r17", nullptr},
                                                                            {"periodicitySlotList-r17", nullptr},
                                                                            {"slotListSubcarrierSpacing-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_iab_res_cfg_r17_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(iab_res_cfg_r17_s, periodicity_slot_list_r17_present)},
    {3, (int32_t)offsetof(iab_res_cfg_r17_s, slot_list_subcarrier_spacing_r17_present)}};
const asn1::table::seq_desc iab_res_cfg_r17_s::desc = {{seq_table_iab_res_cfg_r17_s_fields,
                                                        uint64_t{0x1},
                                                        seq_table_iab_res_cfg_r17_s_opts,
                                                        seq_table_iab_res_cfg_r17_s_names,
                                                        4,
                                                        3},
                                                       true,
                                                       false,
                                                       nullptr,
                                                       0};

static const char* enum_names_iab_res_cfg_r17_s_periodicity_slot_list_r17_e__values[] =
    {"ms0p5", "ms0p625", "ms1", "ms1p25", "ms2", "ms2p5", "ms5", "ms10", "ms20", "ms40", "ms80", "ms160"};
const asn1::table::enum_desc iab_res_cfg_r17_s::periodicity_slot_list_r17_opts::desc = {
    enum_names_iab_res_cfg_r17_s_periodicity_slot_list_r17_e__values,
    12,
    "iab_res_cfg_r17_s::periodicity_slot_list_r17_e_"};

// IntraBandCC-CombinationReqList-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_intra_band_cc_combination_req_list_r17_s_fields[] = {
    {offsetof(intra_band_cc_combination_req_list_r17_s, serv_cell_idx_list_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(intra_band_cc_combination_req_list_r17_s::serv_cell_idx_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(intra_band_cc_combination_req_list_r17_s, cc_combination_list_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     127,
     &asn1::table::list_codec<decltype(intra_band_cc_combination_req_list_r17_s::cc_combination_list_r17),
                              asn1::table::list_json<asn1::table::struct_json>,
                              asn1::table::list_items<false, 1, 32, false, asn1::table::object_items<false>>>::value}};
static const asn1::table::field_name seq_table_intra_band_cc_combination_req_list_r17_s_names[] = {
    {"servCellIndexList-r17", nullptr},
    {"cc-CombinationList-r17", nullptr}};
const asn1::table::seq_desc intra_band_cc_combination_req_list_r17_s::desc = {
    {seq_table_intra_band_cc_combination_req_list_r17_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_intra_band_cc_combination_req_list_r17_s_names,
     2,
     0},
    false,
    false,
    nullptr,
    0};

// NCR-SlotPeriodicityAndSlotOffset-r18 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ncr_slot_periodicity_and_slot_offset_r18_c_alternatives[] =
    {nullptr,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint16_t, true>::value,
     &asn1::table::alternative_ops<uint16_t, true>::value,
     &asn1::table::alternative_ops<uint16_t, true>::value,
     &asn1::table::alternative_ops<uint16_t, true>::value,
     &asn1::table::alternative_ops<uint16_t, true>::value,
     &asn1::table::alternative_ops<uint16_t, true>::value,
     &asn1::table::alternative_ops<uint16_t, true>::value,
     &asn1::table::alternative_ops<uint16_t, true>::value,
     &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc ncr_slot_periodicity_and_slot_offset_r18_c::storage_desc = {
    choice_storage_ncr_slot_periodicity_and_slot_offset_r18_c_alternatives,
    23,
    "ncr_slot_periodicity_and_slot_offset_r18_c"};

void ncr_slot_periodicity_and_slot_offset_r18_c::set_sl1()
{
  set(types::sl1);
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl2()
{
  set(types::sl2);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl4()
{
  set(types::sl4);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl5()
{
  set(types::sl5);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl8()
{
  set(types::sl8);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl10()
{
  set(types::sl10);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl16()
{
  set(types::sl16);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl20()
{
  set(types::sl20);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl32()
{
  set(types::sl32);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl40()
{
  set(types::sl40);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl64()
{
  set(types::sl64);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl80()
{
  set(types::sl80);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl128()
{
  set(types::sl128);
  return c.get<uint8_t>();
}
uint8_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl160()
{
  set(types::sl160);
  return c.get<uint8_t>();
}
uint16_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl256()
{
  set(types::sl256);
  return c.get<uint16_t>();
}
uint16_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl320()
{
  set(types::sl320);
  return c.get<uint16_t>();
}
uint16_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl512()
{
  set(types::sl512);
  return c.get<uint16_t>();
}
uint16_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl640()
{
  set(types::sl640);
  return c.get<uint16_t>();
}
uint16_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl1024()
{
  set(types::sl1024);
  return c.get<uint16_t>();
}
uint16_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl1280()
{
  set(types::sl1280);
  return c.get<uint16_t>();
}
uint16_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl2560()
{
  set(types::sl2560);
  return c.get<uint16_t>();
}
uint16_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl5120()
{
  set(types::sl5120);
  return c.get<uint16_t>();
}
uint16_t& ncr_slot_periodicity_and_slot_offset_r18_c::set_sl10240()
{
  set(types::sl10240);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field choice_table_ncr_slot_periodicity_and_slot_offset_r18_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{1},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{3},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{4},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{7},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{9},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{19},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{39},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{63},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{79},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{159},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{319},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{511},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{639},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1023},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1279},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{2559},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{5119},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{10239},
     nullptr}};
static const asn1::table::field_name choice_table_ncr_slot_periodicity_and_slot_offset_r18_c_names[] = {
    {"sl1", nullptr},    {"sl2", nullptr},    {"sl4", nullptr},    {"sl5", nullptr},    {"sl8", nullptr},
    {"sl10", nullptr},   {"sl16", nullptr},   {"sl20", nullptr},   {"sl32", nullptr},   {"sl40", nullptr},
    {"sl64", nullptr},   {"sl80", nullptr},   {"sl128", nullptr},  {"sl160", nullptr},  {"sl256", nullptr},
    {"sl320", nullptr},  {"sl512", nullptr},  {"sl640", nullptr},  {"sl1024", nullptr}, {"sl1280", nullptr},
    {"sl2560", nullptr}, {"sl5120", nullptr}, {"sl10240", nullptr}};
const asn1::table::choice_desc ncr_slot_periodicity_and_slot_offset_r18_c::desc = {
    asn1::table::enum_int_type<ncr_slot_periodicity_and_slot_offset_r18_c::types>(),
    ncr_slot_periodicity_and_slot_offset_r18_c::types::nof_types,
    ncr_slot_periodicity_and_slot_offset_r18_c::types::nof_exts,
    ncr_slot_periodicity_and_slot_offset_r18_c::types::has_ext,
    false,
    choice_table_ncr_slot_periodicity_and_slot_offset_r18_c_alternatives,
    nullptr,
    choice_table_ncr_slot_periodicity_and_slot_offset_r18_c_names,
    "ncr_slot_periodicity_and_slot_offset_r18_c",
    offsetof(ncr_slot_periodicity_and_slot_offset_r18_c, c.type),
    offsetof(ncr_slot_periodicity_and_slot_offset_r18_c, c.value),
    false,
    &ncr_slot_periodicity_and_slot_offset_r18_c::storage_desc};

static const char* enum_names_ncr_slot_periodicity_and_slot_offset_r18_c_types_values[] = {
    "sl1",   "sl2",   "sl4",   "sl5",   "sl8",   "sl10",  "sl16",   "sl20",   "sl32",   "sl40",   "sl64",   "sl80",
    "sl128", "sl160", "sl256", "sl320", "sl512", "sl640", "sl1024", "sl1280", "sl2560", "sl5120", "sl10240"};
const asn1::table::enum_desc ncr_slot_periodicity_and_slot_offset_r18_c::types_opts::desc = {
    enum_names_ncr_slot_periodicity_and_slot_offset_r18_c_types_values,
    23,
    "ncr_slot_periodicity_and_slot_offset_r18_c::types"};
uint16_t ncr_slot_periodicity_and_slot_offset_r18_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {1,   2,   4,   5,   8,   10,  16,   20,   32,   40,   64,   80,
                                     128, 160, 256, 320, 512, 640, 1024, 1280, 2560, 5120, 10240};
  return map_enum_number(numbers, 23, value, "ncr_slot_periodicity_and_slot_offset_r18_c::types");
}

// NCR-MsPeriodicityAndSlotOffset-r18 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ncr_ms_periodicity_and_slot_offset_r18_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,  &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint16_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value, &asn1::table::alternative_ops<uint32_t, true>::value,
    &asn1::table::alternative_ops<uint32_t, true>::value};
const asn1::table::choice_storage_desc ncr_ms_periodicity_and_slot_offset_r18_c::storage_desc = {
    choice_storage_ncr_ms_periodicity_and_slot_offset_r18_c_alternatives,
    23,
    "ncr_ms_periodicity_and_slot_offset_r18_c"};

uint8_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms1()
{
  set(types::ms1);
  return c.get<uint8_t>();
}
uint8_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms2()
{
  set(types::ms2);
  return c.get<uint8_t>();
}
uint8_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms4()
{
  set(types::ms4);
  return c.get<uint8_t>();
}
uint8_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms5()
{
  set(types::ms5);
  return c.get<uint8_t>();
}
uint8_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms8()
{
  set(types::ms8);
  return c.get<uint8_t>();
}
uint8_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms16()
{
  set(types::ms16);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms20()
{
  set(types::ms20);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms32()
{
  set(types::ms32);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms40()
{
  set(types::ms40);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms64()
{
  set(types::ms64);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms80()
{
  set(types::ms80);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms128()
{
  set(types::ms128);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms160()
{
  set(types::ms160);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms320()
{
  set(types::ms320);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms512()
{
  set(types::ms512);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms640()
{
  set(types::ms640);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms1024()
{
  set(types::ms1024);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms1280()
{
  set(types::ms1280);
  return c.get<uint16_t>();
}
uint16_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms2560()
{
  set(types::ms2560);
  return c.get<uint16_t>();
}
uint32_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms5120()
{
  set(types::ms5120);
  return c.get<uint32_t>();
}
uint32_t& ncr_ms_periodicity_and_slot_offset_r18_c::set_ms10240()
{
  set(types::ms10240);
  return c.get<uint32_t>();
}
static const asn1::table::seq_field choice_table_ncr_ms_periodicity_and_slot_offset_r18_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{15},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{63},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{79},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{159},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{319},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{511},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{639},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1023},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1279},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{2047},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{2559},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{5119},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{8191},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{10239},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{16383},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{20479},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{40959},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{81919},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     int64_t{0},
     uint64_t{163839},
     nullptr}};
static const asn1::table::field_name choice_table_ncr_ms_periodicity_and_slot_offset_r18_c_names[] = {
    {"ms1", nullptr},    {"ms2", nullptr},    {"ms4", nullptr},    {"ms5", nullptr},    {"ms8", nullptr},
    {"ms10", nullptr},   {"ms16", nullptr},   {"ms20", nullptr},   {"ms32", nullptr},   {"ms40", nullptr},
    {"ms64", nullptr},   {"ms80", nullptr},   {"ms128", nullptr},  {"ms160", nullptr},  {"ms256", nullptr},
    {"ms320", nullptr},  {"ms512", nullptr},  {"ms640", nullptr},  {"ms1024", nullptr}, {"ms1280", nullptr},
    {"ms2560", nullptr}, {"ms5120", nullptr}, {"ms10240", nullptr}};
const asn1::table::choice_desc ncr_ms_periodicity_and_slot_offset_r18_c::desc = {
    asn1::table::enum_int_type<ncr_ms_periodicity_and_slot_offset_r18_c::types>(),
    ncr_ms_periodicity_and_slot_offset_r18_c::types::nof_types,
    ncr_ms_periodicity_and_slot_offset_r18_c::types::nof_exts,
    ncr_ms_periodicity_and_slot_offset_r18_c::types::has_ext,
    false,
    choice_table_ncr_ms_periodicity_and_slot_offset_r18_c_alternatives,
    nullptr,
    choice_table_ncr_ms_periodicity_and_slot_offset_r18_c_names,
    "ncr_ms_periodicity_and_slot_offset_r18_c",
    offsetof(ncr_ms_periodicity_and_slot_offset_r18_c, c.type),
    offsetof(ncr_ms_periodicity_and_slot_offset_r18_c, c.value),
    false,
    &ncr_ms_periodicity_and_slot_offset_r18_c::storage_desc};

static const char* enum_names_ncr_ms_periodicity_and_slot_offset_r18_c_types_values[] = {
    "ms1",   "ms2",   "ms4",   "ms5",   "ms8",   "ms10",  "ms16",   "ms20",   "ms32",   "ms40",   "ms64",   "ms80",
    "ms128", "ms160", "ms256", "ms320", "ms512", "ms640", "ms1024", "ms1280", "ms2560", "ms5120", "ms10240"};
const asn1::table::enum_desc ncr_ms_periodicity_and_slot_offset_r18_c::types_opts::desc = {
    enum_names_ncr_ms_periodicity_and_slot_offset_r18_c_types_values,
    23,
    "ncr_ms_periodicity_and_slot_offset_r18_c::types"};
uint16_t ncr_ms_periodicity_and_slot_offset_r18_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {1,   2,   4,   5,   8,   10,  16,   20,   32,   40,   64,   80,
                                     128, 160, 256, 320, 512, 640, 1024, 1280, 2560, 5120, 10240};
  return map_enum_number(numbers, 23, value, "ncr_ms_periodicity_and_slot_offset_r18_c::types");
}

// NCR-PeriodicityAndOffset-r18 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ncr_periodicity_and_offset_r18_c_alternatives[] = {
    &asn1::table::alternative_ops<ncr_slot_periodicity_and_slot_offset_r18_c, false>::value,
    &asn1::table::alternative_ops<ncr_ms_periodicity_and_slot_offset_r18_c, false>::value};
const asn1::table::choice_storage_desc ncr_periodicity_and_offset_r18_c::storage_desc = {
    choice_storage_ncr_periodicity_and_offset_r18_c_alternatives,
    2,
    "ncr_periodicity_and_offset_r18_c"};

ncr_slot_periodicity_and_slot_offset_r18_c& ncr_periodicity_and_offset_r18_c::set_slot()
{
  set(types::slot);
  return c.get<ncr_slot_periodicity_and_slot_offset_r18_c>();
}
ncr_ms_periodicity_and_slot_offset_r18_c& ncr_periodicity_and_offset_r18_c::set_ms()
{
  set(types::ms);
  return c.get<ncr_ms_periodicity_and_slot_offset_r18_c>();
}
static const asn1::table::seq_field choice_table_ncr_periodicity_and_offset_r18_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ncr_slot_periodicity_and_slot_offset_r18_c::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ncr_ms_periodicity_and_slot_offset_r18_c::desc}};
static const asn1::table::field_name choice_table_ncr_periodicity_and_offset_r18_c_names[] = {{"slot", nullptr},
                                                                                              {"ms", nullptr}};
const asn1::table::choice_desc       ncr_periodicity_and_offset_r18_c::desc                = {
    asn1::table::enum_int_type<ncr_periodicity_and_offset_r18_c::types>(),
    ncr_periodicity_and_offset_r18_c::types::nof_types,
    ncr_periodicity_and_offset_r18_c::types::nof_exts,
    ncr_periodicity_and_offset_r18_c::types::has_ext,
    false,
    choice_table_ncr_periodicity_and_offset_r18_c_alternatives,
    nullptr,
    choice_table_ncr_periodicity_and_offset_r18_c_names,
    "ncr_periodicity_and_offset_r18_c",
    offsetof(ncr_periodicity_and_offset_r18_c, c.type),
    offsetof(ncr_periodicity_and_offset_r18_c, c.value),
    false,
    &ncr_periodicity_and_offset_r18_c::storage_desc};

static const char*           enum_names_ncr_periodicity_and_offset_r18_c_types_values[] = {"slot", "ms"};
const asn1::table::enum_desc ncr_periodicity_and_offset_r18_c::types_opts::desc         = {
    enum_names_ncr_periodicity_and_offset_r18_c_types_values,
    2,
    "ncr_periodicity_and_offset_r18_c::types"};

// NCR-PeriodicFwdResource-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ncr_periodic_fwd_res_r18_s_periodic_time_rsrc_r18_s__fields[] = {
    {offsetof(ncr_periodic_fwd_res_r18_s::periodic_time_rsrc_r18_s_, periodicity_and_offset_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ncr_periodicity_and_offset_r18_c::desc},
    {offsetof(ncr_periodic_fwd_res_r18_s::periodic_time_rsrc_r18_s_, symbol_offset_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{13},
     nullptr},
    {offsetof(ncr_periodic_fwd_res_r18_s::periodic_time_rsrc_r18_s_, dur_in_symbols_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{111},
     nullptr}};
static const asn1::table::field_name seq_table_ncr_periodic_fwd_res_r18_s_periodic_time_rsrc_r18_s__names[] = {
    {"periodicityAndOffset-r18", nullptr},
    {"symbolOffset-r18", nullptr},
    {"durationInSymbols-r18", nullptr}};
static const asn1::table::seq_desc seq_table_ncr_periodic_fwd_res_r18_s_periodic_time_rsrc_r18_s_ = {
    {seq_table_ncr_periodic_fwd_res_r18_s_periodic_time_rsrc_r18_s__fields,
     uint64_t{0x7},
     nullptr,
     seq_table_ncr_periodic_fwd_res_r18_s_periodic_time_rsrc_r18_s__names,
     3,
     0},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_ncr_periodic_fwd_res_r18_s_fields[] = {
    {offsetof(ncr_periodic_fwd_res_r18_s, periodic_fwd_rsrc_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     int64_t{0},
     uint64_t{1023},
     nullptr},
    {offsetof(ncr_periodic_fwd_res_r18_s, beam_idx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{63},
     nullptr},
    {offsetof(ncr_periodic_fwd_res_r18_s, periodic_time_rsrc_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ncr_periodic_fwd_res_r18_s_periodic_time_rsrc_r18_s_}};
static const asn1::table::field_name seq_table_ncr_periodic_fwd_res_r18_s_names[] = {{"periodicFwdRsrcId-r18", nullptr},
                                                                                     {"beamIndex-r18", nullptr},
                                                                                     {"periodicTimeRsrc-r18", nullptr}};
const asn1::table::seq_desc          ncr_periodic_fwd_res_r18_s::desc = {{seq_table_ncr_periodic_fwd_res_r18_s_fields,
                                                                          uint64_t{0x7},
                                                                          nullptr,
                                                                          seq_table_ncr_periodic_fwd_res_r18_s_names,
                                                                          3,
                                                                          0},
                                                                         true,
                                                                         false,
                                                                         nullptr,
                                                                         0};

// NCR-PeriodicFwdResourceSet-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ncr_periodic_fwd_res_set_r18_s_fields[] = {
    {offsetof(ncr_periodic_fwd_res_set_r18_s, periodic_fwd_rsrc_set_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr},
    {offsetof(ncr_periodic_fwd_res_set_r18_s, periodic_fwd_rsrc_to_add_mod_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1023,
     &asn1::table::item_list_codec<decltype(ncr_periodic_fwd_res_set_r18_s::periodic_fwd_rsrc_to_add_mod_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ncr_periodic_fwd_res_r18_s::desc},
    {offsetof(ncr_periodic_fwd_res_set_r18_s, periodic_fwd_rsrc_to_release_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1023,
     &asn1::table::list_codec<decltype(ncr_periodic_fwd_res_set_r18_s::periodic_fwd_rsrc_to_release_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{1023}, false, false>>::value},
    {offsetof(ncr_periodic_fwd_res_set_r18_s, ref_scs_r18),
     (int32_t)offsetof(ncr_periodic_fwd_res_set_r18_s, ref_scs_r18_present),
     asn1::table::enum_op<decltype(ncr_periodic_fwd_res_set_r18_s::ref_scs_r18)>(),
     decltype(ncr_periodic_fwd_res_set_r18_s::ref_scs_r18)::has_ext,
     false,
     decltype(ncr_periodic_fwd_res_set_r18_s::ref_scs_r18)::nof_types,
     decltype(ncr_periodic_fwd_res_set_r18_s::ref_scs_r18)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {0,
     (int32_t)offsetof(ncr_periodic_fwd_res_set_r18_s, prio_flag_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ncr_periodic_fwd_res_set_r18_s_names[] = {
    {"periodicFwdRsrcSetId-r18", nullptr},
    {"periodicFwdRsrcToAddModList-r18", nullptr},
    {"periodicFwdRsrcToReleaseList-r18", nullptr},
    {"referenceSCS-r18", nullptr},
    {"priorityFlag-r18", "true"}};
static const asn1::table::seq_opt seq_table_ncr_periodic_fwd_res_set_r18_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(ncr_periodic_fwd_res_set_r18_s, ref_scs_r18_present)},
    {4, (int32_t)offsetof(ncr_periodic_fwd_res_set_r18_s, prio_flag_r18_present)}};
const asn1::table::seq_desc ncr_periodic_fwd_res_set_r18_s::desc = {{seq_table_ncr_periodic_fwd_res_set_r18_s_fields,
                                                                     uint64_t{0x1},
                                                                     seq_table_ncr_periodic_fwd_res_set_r18_s_opts,
                                                                     seq_table_ncr_periodic_fwd_res_set_r18_s_names,
                                                                     5,
                                                                     4},
                                                                    true,
                                                                    false,
                                                                    nullptr,
                                                                    0};

// NCR-AperiodicFwdTimeResource-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ncr_aperiodic_fwd_time_res_r18_s_fields[] = {
    {offsetof(ncr_aperiodic_fwd_time_res_r18_s, aperiodic_fwd_time_rsrc_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{111},
     nullptr},
    {offsetof(ncr_aperiodic_fwd_time_res_r18_s, slot_offset_aperiodic_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{14},
     nullptr},
    {offsetof(ncr_aperiodic_fwd_time_res_r18_s, symbol_offset_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{13},
     nullptr},
    {offsetof(ncr_aperiodic_fwd_time_res_r18_s, dur_in_symbols_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{27},
     nullptr}};
static const asn1::table::field_name seq_table_ncr_aperiodic_fwd_time_res_r18_s_names[] = {
    {"aperiodicFwdTimeRsrcId-r18", nullptr},
    {"slotOffsetAperiodic-r18", nullptr},
    {"symbolOffset-r18", nullptr},
    {"durationInSymbols-r18", nullptr}};
const asn1::table::seq_desc ncr_aperiodic_fwd_time_res_r18_s::desc = {
    {seq_table_ncr_aperiodic_fwd_time_res_r18_s_fields,
     uint64_t{0xf},
     nullptr,
     seq_table_ncr_aperiodic_fwd_time_res_r18_s_names,
     4,
     0},
    true,
    false,
    nullptr,
    0};

// NCR-AperiodicFwdConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ncr_aperiodic_fwd_cfg_r18_s_fields[] = {
    {offsetof(ncr_aperiodic_fwd_cfg_r18_s, aperiodic_fwd_time_rsrc_to_add_mod_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     111,
     &asn1::table::item_list_codec<decltype(ncr_aperiodic_fwd_cfg_r18_s::aperiodic_fwd_time_rsrc_to_add_mod_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ncr_aperiodic_fwd_time_res_r18_s::desc},
    {offsetof(ncr_aperiodic_fwd_cfg_r18_s, aperiodic_fwd_time_rsrc_to_release_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     111,
     &asn1::table::list_codec<decltype(ncr_aperiodic_fwd_cfg_r18_s::aperiodic_fwd_time_rsrc_to_release_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{111}, false, false>>::value},
    {offsetof(ncr_aperiodic_fwd_cfg_r18_s, ref_scs_r18),
     (int32_t)offsetof(ncr_aperiodic_fwd_cfg_r18_s, ref_scs_r18_present),
     asn1::table::enum_op<decltype(ncr_aperiodic_fwd_cfg_r18_s::ref_scs_r18)>(),
     decltype(ncr_aperiodic_fwd_cfg_r18_s::ref_scs_r18)::has_ext,
     false,
     decltype(ncr_aperiodic_fwd_cfg_r18_s::ref_scs_r18)::nof_types,
     decltype(ncr_aperiodic_fwd_cfg_r18_s::ref_scs_r18)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(ncr_aperiodic_fwd_cfg_r18_s, aperiodic_beam_field_width_r18),
     (int32_t)offsetof(ncr_aperiodic_fwd_cfg_r18_s, aperiodic_beam_field_width_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{5},
     nullptr},
    {offsetof(ncr_aperiodic_fwd_cfg_r18_s, nof_fields_r18),
     (int32_t)offsetof(ncr_aperiodic_fwd_cfg_r18_s, nof_fields_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name seq_table_ncr_aperiodic_fwd_cfg_r18_s_names[] = {
    {"aperiodicFwdTimeRsrcToAddModList-r18", nullptr},
    {"aperiodicFwdTimeRsrcToReleaseList-r18", nullptr},
    {"referenceSCS-r18", nullptr},
    {"aperiodicBeamFieldWidth-r18", nullptr},
    {"numberOfFields-r18", nullptr}};
static const asn1::table::seq_opt seq_table_ncr_aperiodic_fwd_cfg_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(ncr_aperiodic_fwd_cfg_r18_s, ref_scs_r18_present)},
    {3, (int32_t)offsetof(ncr_aperiodic_fwd_cfg_r18_s, aperiodic_beam_field_width_r18_present)},
    {4, (int32_t)offsetof(ncr_aperiodic_fwd_cfg_r18_s, nof_fields_r18_present)}};
const asn1::table::seq_desc ncr_aperiodic_fwd_cfg_r18_s::desc = {{seq_table_ncr_aperiodic_fwd_cfg_r18_s_fields,
                                                                  uint64_t{0x0},
                                                                  seq_table_ncr_aperiodic_fwd_cfg_r18_s_opts,
                                                                  seq_table_ncr_aperiodic_fwd_cfg_r18_s_names,
                                                                  5,
                                                                  5},
                                                                 true,
                                                                 false,
                                                                 nullptr,
                                                                 0};

// NCR-SemiPersistentFwdResource-r18 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_ncr_semi_persistent_fwd_res_r18_s_semi_persistent_time_rsrc_r18_s__fields[] = {
        {offsetof(ncr_semi_persistent_fwd_res_r18_s::semi_persistent_time_rsrc_r18_s_, periodicity_and_offset_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &ncr_periodicity_and_offset_r18_c::desc},
        {offsetof(ncr_semi_persistent_fwd_res_r18_s::semi_persistent_time_rsrc_r18_s_, symbol_offset_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{0},
         uint64_t{13},
         nullptr},
        {offsetof(ncr_semi_persistent_fwd_res_r18_s::semi_persistent_time_rsrc_r18_s_, dur_in_symbols_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         int64_t{1},
         uint64_t{111},
         nullptr}};
static const asn1::table::field_name
    seq_table_ncr_semi_persistent_fwd_res_r18_s_semi_persistent_time_rsrc_r18_s__names[] = {
        {"periodicityAndOffset-r18", nullptr},
        {"symbolOffset-r18", nullptr},
        {"durationInSymbols-r18", nullptr}};
static const asn1::table::seq_desc seq_table_ncr_semi_persistent_fwd_res_r18_s_semi_persistent_time_rsrc_r18_s_ = {
    {seq_table_ncr_semi_persistent_fwd_res_r18_s_semi_persistent_time_rsrc_r18_s__fields,
     uint64_t{0x7},
     nullptr,
     seq_table_ncr_semi_persistent_fwd_res_r18_s_semi_persistent_time_rsrc_r18_s__names,
     3,
     0},
    false,
    false,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_ncr_semi_persistent_fwd_res_r18_s_fields[] = {
    {offsetof(ncr_semi_persistent_fwd_res_r18_s, semi_persistent_fwd_rsrc_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{127},
     nullptr},
    {offsetof(ncr_semi_persistent_fwd_res_r18_s, beam_idx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{63},
     nullptr},
    {offsetof(ncr_semi_persistent_fwd_res_r18_s, semi_persistent_time_rsrc_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ncr_semi_persistent_fwd_res_r18_s_semi_persistent_time_rsrc_r18_s_}};
static const asn1::table::field_name seq_table_ncr_semi_persistent_fwd_res_r18_s_names[] = {
    {"semiPersistentFwdRsrcId-r18", nullptr},
    {"beamIndex-r18", nullptr},
    {"semiPersistentTimeRsrc-r18", nullptr}};
const asn1::table::seq_desc ncr_semi_persistent_fwd_res_r18_s::desc = {
    {seq_table_ncr_semi_persistent_fwd_res_r18_s_fields,
     uint64_t{0x7},
     nullptr,
     seq_table_ncr_semi_persistent_fwd_res_r18_s_names,
     3,
     0},
    true,
    false,
    nullptr,
    0};

// NCR-SemiPersistentFwdResourceSet-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ncr_semi_persistent_fwd_res_set_r18_s_fields[] = {
    {offsetof(ncr_semi_persistent_fwd_res_set_r18_s, semi_persistent_fwd_rsrc_set_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{31},
     nullptr},
    {offsetof(ncr_semi_persistent_fwd_res_set_r18_s, semi_persistent_fwd_rsrc_to_add_mod_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     127,
     &asn1::table::item_list_codec<
         decltype(ncr_semi_persistent_fwd_res_set_r18_s::semi_persistent_fwd_rsrc_to_add_mod_list_r18),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &ncr_semi_persistent_fwd_res_r18_s::desc},
    {offsetof(ncr_semi_persistent_fwd_res_set_r18_s, semi_persistent_fwd_rsrc_to_release_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     127,
     &asn1::table::list_codec<
         decltype(ncr_semi_persistent_fwd_res_set_r18_s::semi_persistent_fwd_rsrc_to_release_list_r18),
         asn1::table::int_json,
         asn1::table::int_items<false, int64_t{0}, uint64_t{127}, false, false>>::value},
    {offsetof(ncr_semi_persistent_fwd_res_set_r18_s, ref_scs_r18),
     (int32_t)offsetof(ncr_semi_persistent_fwd_res_set_r18_s, ref_scs_r18_present),
     asn1::table::enum_op<decltype(ncr_semi_persistent_fwd_res_set_r18_s::ref_scs_r18)>(),
     decltype(ncr_semi_persistent_fwd_res_set_r18_s::ref_scs_r18)::has_ext,
     false,
     decltype(ncr_semi_persistent_fwd_res_set_r18_s::ref_scs_r18)::nof_types,
     decltype(ncr_semi_persistent_fwd_res_set_r18_s::ref_scs_r18)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {0,
     (int32_t)offsetof(ncr_semi_persistent_fwd_res_set_r18_s, prio_flag_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ncr_semi_persistent_fwd_res_set_r18_s_names[] = {
    {"semiPersistentFwdRsrcSetId-r18", nullptr},
    {"semiPersistentFwdRsrcToAddModList-r18", nullptr},
    {"semiPersistentFwdRsrcToReleaseList-r18", nullptr},
    {"referenceSCS-r18", nullptr},
    {"priorityFlag-r18", "true"}};
static const asn1::table::seq_opt seq_table_ncr_semi_persistent_fwd_res_set_r18_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(ncr_semi_persistent_fwd_res_set_r18_s, ref_scs_r18_present)},
    {4, (int32_t)offsetof(ncr_semi_persistent_fwd_res_set_r18_s, prio_flag_r18_present)}};
const asn1::table::seq_desc ncr_semi_persistent_fwd_res_set_r18_s::desc = {
    {seq_table_ncr_semi_persistent_fwd_res_set_r18_s_fields,
     uint64_t{0x1},
     seq_table_ncr_semi_persistent_fwd_res_set_r18_s_opts,
     seq_table_ncr_semi_persistent_fwd_res_set_r18_s_names,
     5,
     4},
    true,
    false,
    nullptr,
    0};

// NCR-FwdConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ncr_fwd_cfg_r18_s_aperiodic_fwd_cfg_r18_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ncr_aperiodic_fwd_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_ncr_fwd_cfg_r18_s_aperiodic_fwd_cfg_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_ncr_fwd_cfg_r18_s_aperiodic_fwd_cfg_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_ncr_fwd_cfg_r18_s_aperiodic_fwd_cfg_r18_setup_release_alternatives,
    nullptr,
    seq_table_ncr_fwd_cfg_r18_s_aperiodic_fwd_cfg_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<ncr_aperiodic_fwd_cfg_r18_s>::type_offset(),
    setup_release_c<ncr_aperiodic_fwd_cfg_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_ncr_fwd_cfg_r18_s_fields[] = {
    {offsetof(ncr_fwd_cfg_r18_s, periodic_fwd_rsrc_set_to_add_mod_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::item_list_codec<decltype(ncr_fwd_cfg_r18_s::periodic_fwd_rsrc_set_to_add_mod_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ncr_periodic_fwd_res_set_r18_s::desc},
    {offsetof(ncr_fwd_cfg_r18_s, periodic_fwd_rsrc_set_to_release_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(ncr_fwd_cfg_r18_s::periodic_fwd_rsrc_set_to_release_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(ncr_fwd_cfg_r18_s, aperiodic_fwd_cfg_r18),
     (int32_t)offsetof(ncr_fwd_cfg_r18_s, aperiodic_fwd_cfg_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ncr_fwd_cfg_r18_s_aperiodic_fwd_cfg_r18_setup_release},
    {offsetof(ncr_fwd_cfg_r18_s, semi_persistent_fwd_rsrc_set_to_add_mod_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::item_list_codec<decltype(ncr_fwd_cfg_r18_s::semi_persistent_fwd_rsrc_set_to_add_mod_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ncr_semi_persistent_fwd_res_set_r18_s::desc},
    {offsetof(ncr_fwd_cfg_r18_s, semi_persistent_fwd_rsrc_set_to_release_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(ncr_fwd_cfg_r18_s::semi_persistent_fwd_rsrc_set_to_release_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value}};
static const asn1::table::field_name seq_table_ncr_fwd_cfg_r18_s_names[] = {
    {"periodicFwdRsrcSetToAddModList-r18", nullptr},
    {"periodicFwdRsrcSetToReleaseList-r18", nullptr},
    {"aperiodicFwdConfig-r18", nullptr},
    {"semiPersistentFwdRsrcSetToAddModList-r18", nullptr},
    {"semiPersistentFwdRsrcSetToReleaseList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_ncr_fwd_cfg_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(ncr_fwd_cfg_r18_s, aperiodic_fwd_cfg_r18_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ncr_fwd_cfg_r18_s::desc = {{seq_table_ncr_fwd_cfg_r18_s_fields,
                                                        uint64_t{0x0},
                                                        seq_table_ncr_fwd_cfg_r18_s_opts,
                                                        seq_table_ncr_fwd_cfg_r18_s_names,
                                                        5,
                                                        5},
                                                       true,
                                                       false,
                                                       nullptr,
                                                       0};

// UplinkTxSwitchingBandPairConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_tx_switching_band_pair_cfg_r18_s_fields[] = {
    {offsetof(ul_tx_switching_band_pair_cfg_r18_s, band_info_ul1_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr},
    {offsetof(ul_tx_switching_band_pair_cfg_r18_s, band_info_ul2_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr},
    {offsetof(ul_tx_switching_band_pair_cfg_r18_s, switching_option_cfg_for_band_pair_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ul_tx_switching_band_pair_cfg_r18_s::switching_option_cfg_for_band_pair_r18)>(),
     decltype(ul_tx_switching_band_pair_cfg_r18_s::switching_option_cfg_for_band_pair_r18)::has_ext,
     false,
     decltype(ul_tx_switching_band_pair_cfg_r18_s::switching_option_cfg_for_band_pair_r18)::nof_types,
     decltype(ul_tx_switching_band_pair_cfg_r18_s::switching_option_cfg_for_band_pair_r18)::nof_exts,
     nullptr,
     &ul_tx_switching_band_pair_cfg_r18_s::switching_option_cfg_for_band_pair_r18_opts::desc},
    {0,
     (int32_t)offsetof(ul_tx_switching_band_pair_cfg_r18_s, switching2_t_mode_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ul_tx_switching_band_pair_cfg_r18_s, switching_period_cfg_for_band_pair_r18),
     (int32_t)offsetof(ul_tx_switching_band_pair_cfg_r18_s, switching_period_cfg_for_band_pair_r18_present),
     asn1::table::enum_op<decltype(ul_tx_switching_band_pair_cfg_r18_s::switching_period_cfg_for_band_pair_r18)>(),
     decltype(ul_tx_switching_band_pair_cfg_r18_s::switching_period_cfg_for_band_pair_r18)::has_ext,
     false,
     decltype(ul_tx_switching_band_pair_cfg_r18_s::switching_period_cfg_for_band_pair_r18)::nof_types,
     decltype(ul_tx_switching_band_pair_cfg_r18_s::switching_period_cfg_for_band_pair_r18)::nof_exts,
     nullptr,
     &ul_tx_switching_band_pair_cfg_r18_s::switching_period_cfg_for_band_pair_r18_opts::desc}};
static const asn1::table::field_name seq_table_ul_tx_switching_band_pair_cfg_r18_s_names[] = {
    {"bandInfoUL1-r18", nullptr},
    {"bandInfoUL2-r18", nullptr},
    {"switchingOptionConfigForBandPair-r18", nullptr},
    {"switching2T-Mode-r18", "enabled"},
    {"switchingPeriodConfigForBandPair-r18", nullptr}};
static const asn1::table::seq_opt seq_table_ul_tx_switching_band_pair_cfg_r18_s_opts[] = {
    {3, (int32_t)offsetof(ul_tx_switching_band_pair_cfg_r18_s, switching2_t_mode_r18_present)},
    {4, (int32_t)offsetof(ul_tx_switching_band_pair_cfg_r18_s, switching_period_cfg_for_band_pair_r18_present)}};
const asn1::table::seq_desc ul_tx_switching_band_pair_cfg_r18_s::desc = {
    {seq_table_ul_tx_switching_band_pair_cfg_r18_s_fields,
     uint64_t{0x7},
     seq_table_ul_tx_switching_band_pair_cfg_r18_s_opts,
     seq_table_ul_tx_switching_band_pair_cfg_r18_s_names,
     5,
     2},
    true,
    false,
    nullptr,
    0};

static const char* enum_names_ul_tx_switching_band_pair_cfg_r18_s_switching_option_cfg_for_band_pair_r18_e__values[] = {
    "switchedUL",
    "dualUL"};
const asn1::table::enum_desc ul_tx_switching_band_pair_cfg_r18_s::switching_option_cfg_for_band_pair_r18_opts::desc = {
    enum_names_ul_tx_switching_band_pair_cfg_r18_s_switching_option_cfg_for_band_pair_r18_e__values,
    2,
    "ul_tx_switching_band_pair_cfg_r18_s::switching_option_cfg_for_band_pair_r18_e_"};

static const char* enum_names_ul_tx_switching_band_pair_cfg_r18_s_switching_period_cfg_for_band_pair_r18_e__values[] = {
    "n35us",
    "n140us"};
const asn1::table::enum_desc ul_tx_switching_band_pair_cfg_r18_s::switching_period_cfg_for_band_pair_r18_opts::desc = {
    enum_names_ul_tx_switching_band_pair_cfg_r18_s_switching_period_cfg_for_band_pair_r18_e__values,
    2,
    "ul_tx_switching_band_pair_cfg_r18_s::switching_period_cfg_for_band_pair_r18_e_"};
uint8_t ul_tx_switching_band_pair_cfg_r18_s::switching_period_cfg_for_band_pair_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {35, 140};
  return map_enum_number(
      numbers, 2, value, "ul_tx_switching_band_pair_cfg_r18_s::switching_period_cfg_for_band_pair_r18_e_");
}

// UplinkTxSwitchingAssociatedBandDualUL-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_tx_switching_associated_band_dual_ul_r18_s_fields[] = {
    {offsetof(ul_tx_switching_associated_band_dual_ul_r18_s, tx_band_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr},
    {offsetof(ul_tx_switching_associated_band_dual_ul_r18_s, associated_band_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{1},
     uint64_t{31},
     nullptr}};
static const asn1::table::field_name seq_table_ul_tx_switching_associated_band_dual_ul_r18_s_names[] = {
    {"transmitBand-r18", nullptr},
    {"associatedBand-r18", nullptr}};
const asn1::table::seq_desc ul_tx_switching_associated_band_dual_ul_r18_s::desc = {
    {seq_table_ul_tx_switching_associated_band_dual_ul_r18_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_ul_tx_switching_associated_band_dual_ul_r18_s_names,
     2,
     0},
    false,
    false,
    nullptr,
    0};

// UplinkTxSwitchingMoreBands-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_tx_switching_more_bands_r18_s_fields[] = {
    {offsetof(ul_tx_switching_more_bands_r18_s, ul_tx_switching_band_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(ul_tx_switching_more_bands_r18_s::ul_tx_switching_band_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{1023}, false, false>>::value},
    {offsetof(ul_tx_switching_more_bands_r18_s, ul_tx_switching_band_pair_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::item_list_codec<decltype(ul_tx_switching_more_bands_r18_s::ul_tx_switching_band_pair_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ul_tx_switching_band_pair_cfg_r18_s::desc},
    {offsetof(ul_tx_switching_more_bands_r18_s, ul_tx_switching_associated_band_dual_ul_list_r18),
     (int32_t)offsetof(ul_tx_switching_more_bands_r18_s, ul_tx_switching_associated_band_dual_ul_list_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     32,
     &asn1::table::item_list_codec<
         decltype(ul_tx_switching_more_bands_r18_s::ul_tx_switching_associated_band_dual_ul_list_r18),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &ul_tx_switching_associated_band_dual_ul_r18_s::desc}};
static const asn1::table::field_name seq_table_ul_tx_switching_more_bands_r18_s_names[] = {
    {"uplinkTxSwitchingBandList-r18", nullptr},
    {"uplinkTxSwitchingBandPairList-r18", nullptr},
    {"uplinkTxSwitchingAssociatedBandDualUL-List-r18", nullptr}};
static const asn1::table::seq_opt seq_table_ul_tx_switching_more_bands_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(ul_tx_switching_more_bands_r18_s, ul_tx_switching_associated_band_dual_ul_list_r18_present)}};
const asn1::table::seq_desc ul_tx_switching_more_bands_r18_s::desc = {
    {seq_table_ul_tx_switching_more_bands_r18_s_fields,
     uint64_t{0x0},
     seq_table_ul_tx_switching_more_bands_r18_s_opts,
     seq_table_ul_tx_switching_more_bands_r18_s_names,
     3,
     3},
    true,
    false,
    nullptr,
    0};

// CellGroupConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_group_cfg_s_ncr_fwd_cfg_r18_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ncr_fwd_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_cell_group_cfg_s_ncr_fwd_cfg_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_cell_group_cfg_s_ncr_fwd_cfg_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_cell_group_cfg_s_ncr_fwd_cfg_r18_setup_release_alternatives,
    nullptr,
    seq_table_cell_group_cfg_s_ncr_fwd_cfg_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<ncr_fwd_cfg_r18_s>::type_offset(),
    setup_release_c<ncr_fwd_cfg_r18_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_cell_group_cfg_s_autonomous_denial_params_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &autonomous_denial_params_r18_s::desc}};
static const asn1::table::field_name seq_table_cell_group_cfg_s_autonomous_denial_params_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_cell_group_cfg_s_autonomous_denial_params_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_cell_group_cfg_s_autonomous_denial_params_r18_setup_release_alternatives,
    nullptr,
    seq_table_cell_group_cfg_s_autonomous_denial_params_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<autonomous_denial_params_r18_s>::type_offset(),
    setup_release_c<autonomous_denial_params_r18_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_cell_group_cfg_s_ul_tx_switching_more_bands_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &ul_tx_switching_more_bands_r18_s::desc}};
static const asn1::table::field_name seq_table_cell_group_cfg_s_ul_tx_switching_more_bands_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_cell_group_cfg_s_ul_tx_switching_more_bands_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_cell_group_cfg_s_ul_tx_switching_more_bands_r18_setup_release_alternatives,
    nullptr,
    seq_table_cell_group_cfg_s_ul_tx_switching_more_bands_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<ul_tx_switching_more_bands_r18_s>::type_offset(),
    setup_release_c<ul_tx_switching_more_bands_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_cell_group_cfg_s_fields[] = {
    {offsetof(cell_group_cfg_s, cell_group_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     int64_t{0},
     uint64_t{3},
     nullptr},
    {offsetof(cell_group_cfg_s, rlc_bearer_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::item_list_codec<decltype(cell_group_cfg_s::rlc_bearer_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &rlc_bearer_cfg_s::desc},
    {offsetof(cell_group_cfg_s, rlc_bearer_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(cell_group_cfg_s::rlc_bearer_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{31}, false, false>>::value},
    {offsetof(cell_group_cfg_s, mac_cell_group_cfg),
     (int32_t)offsetof(cell_group_cfg_s, mac_cell_group_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mac_cell_group_cfg_s::desc},
    {offsetof(cell_group_cfg_s, phys_cell_group_cfg),
     (int32_t)offsetof(cell_group_cfg_s, phys_cell_group_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &phys_cell_group_cfg_s::desc},
    {offsetof(cell_group_cfg_s, sp_cell_cfg),
     (int32_t)offsetof(cell_group_cfg_s, sp_cell_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sp_cell_cfg_s::desc},
    {offsetof(cell_group_cfg_s, scell_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     30,
     &asn1::table::item_list_codec<decltype(cell_group_cfg_s::scell_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &scell_cfg_s::desc},
    {offsetof(cell_group_cfg_s, scell_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     30,
     &asn1::table::list_codec<decltype(cell_group_cfg_s::scell_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{1}, uint64_t{30}, false, false>>::value}};
static const asn1::table::field_name seq_table_cell_group_cfg_s_names[] = {{"cellGroupId", nullptr},
                                                                           {"rlc-BearerToAddModList", nullptr},
                                                                           {"rlc-BearerToReleaseList", nullptr},
                                                                           {"mac-CellGroupConfig", nullptr},
                                                                           {"physicalCellGroupConfig", nullptr},
                                                                           {"spCellConfig", nullptr},
                                                                           {"sCellToAddModList", nullptr},
                                                                           {"sCellToReleaseList", nullptr}};
static const asn1::table::seq_opt    seq_table_cell_group_cfg_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(cell_group_cfg_s, mac_cell_group_cfg_present)},
    {4, (int32_t)offsetof(cell_group_cfg_s, phys_cell_group_cfg_present)},
    {5, (int32_t)offsetof(cell_group_cfg_s, sp_cell_cfg_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_cell_group_cfg_s_g0_fields[] = {
    {0,
     (int32_t)offsetof(cell_group_cfg_s, report_ul_tx_direct_current_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_cell_group_cfg_s_g0_names[] = {{"reportUplinkTxDirectCurrent", "true"}};
static const asn1::table::seq_opt    seq_table_cell_group_cfg_s_g0_opts[]  = {
    {0, (int32_t)offsetof(cell_group_cfg_s, report_ul_tx_direct_current_present)}};
static const asn1::table::seq_field seq_table_cell_group_cfg_s_g1_fields[] = {
    {offsetof(cell_group_cfg_s, bap_address_r16),
     (int32_t)offsetof(cell_group_cfg_s, bap_address_r16_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cell_group_cfg_s::bap_address_r16), asn1::table::struct_json>::value},
    {offsetof(cell_group_cfg_s, bh_rlc_ch_to_add_mod_list_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65535,
     &asn1::table::ptr_item_list_codec<decltype(cell_group_cfg_s::bh_rlc_ch_to_add_mod_list_r16),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &bh_rlc_ch_cfg_r16_s::desc},
    {offsetof(cell_group_cfg_s, bh_rlc_ch_to_release_list_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65535,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::bh_rlc_ch_to_release_list_r16),
                                  asn1::table::struct_json>::value},
    {offsetof(cell_group_cfg_s, f1c_transfer_path_r16),
     (int32_t)offsetof(cell_group_cfg_s, f1c_transfer_path_r16_present),
     asn1::table::enum_op<decltype(cell_group_cfg_s::f1c_transfer_path_r16)>(),
     decltype(cell_group_cfg_s::f1c_transfer_path_r16)::has_ext,
     false,
     decltype(cell_group_cfg_s::f1c_transfer_path_r16)::nof_types,
     decltype(cell_group_cfg_s::f1c_transfer_path_r16)::nof_exts,
     nullptr,
     &cell_group_cfg_s::f1c_transfer_path_r16_opts::desc},
    {offsetof(cell_group_cfg_s, simul_tci_upd_list1_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::simul_tci_upd_list1_r16),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(cell_group_cfg_s, simul_tci_upd_list2_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::simul_tci_upd_list2_r16),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(cell_group_cfg_s, simul_spatial_upd_list1_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::simul_spatial_upd_list1_r16),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(cell_group_cfg_s, simul_spatial_upd_list2_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::simul_spatial_upd_list2_r16),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(cell_group_cfg_s, ul_tx_switching_option_r16),
     (int32_t)offsetof(cell_group_cfg_s, ul_tx_switching_option_r16_present),
     asn1::table::enum_op<decltype(cell_group_cfg_s::ul_tx_switching_option_r16)>(),
     decltype(cell_group_cfg_s::ul_tx_switching_option_r16)::has_ext,
     false,
     decltype(cell_group_cfg_s::ul_tx_switching_option_r16)::nof_types,
     decltype(cell_group_cfg_s::ul_tx_switching_option_r16)::nof_exts,
     nullptr,
     &cell_group_cfg_s::ul_tx_switching_option_r16_opts::desc},
    {0,
     (int32_t)offsetof(cell_group_cfg_s, ul_tx_switching_pwr_boosting_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_cell_group_cfg_s_g1_names[] = {
    {"bap-Address-r16", nullptr},
    {"bh-RLC-ChannelToAddModList-r16", nullptr},
    {"bh-RLC-ChannelToReleaseList-r16", nullptr},
    {"f1c-TransferPath-r16", nullptr},
    {"simultaneousTCI-UpdateList1-r16", nullptr},
    {"simultaneousTCI-UpdateList2-r16", nullptr},
    {"simultaneousSpatial-UpdatedList1-r16", nullptr},
    {"simultaneousSpatial-UpdatedList2-r16", nullptr},
    {"uplinkTxSwitchingOption-r16", nullptr},
    {"uplinkTxSwitchingPowerBoosting-r16", "enabled"}};
static const asn1::table::seq_opt seq_table_cell_group_cfg_s_g1_opts[] = {
    {0, (int32_t)offsetof(cell_group_cfg_s, bap_address_r16_present)},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, (int32_t)offsetof(cell_group_cfg_s, f1c_transfer_path_r16_present)},
    {4, asn1::table::seq_field::ptr_presence},
    {5, asn1::table::seq_field::ptr_presence},
    {6, asn1::table::seq_field::ptr_presence},
    {7, asn1::table::seq_field::ptr_presence},
    {8, (int32_t)offsetof(cell_group_cfg_s, ul_tx_switching_option_r16_present)},
    {9, (int32_t)offsetof(cell_group_cfg_s, ul_tx_switching_pwr_boosting_r16_present)}};
static const asn1::table::seq_field seq_table_cell_group_cfg_s_g2_fields[] = {
    {0,
     (int32_t)offsetof(cell_group_cfg_s, report_ul_tx_direct_current_two_carrier_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_cell_group_cfg_s_g2_names[] = {
    {"reportUplinkTxDirectCurrentTwoCarrier-r16", "true"}};
static const asn1::table::seq_opt seq_table_cell_group_cfg_s_g2_opts[] = {
    {0, (int32_t)offsetof(cell_group_cfg_s, report_ul_tx_direct_current_two_carrier_r16_present)}};
static const asn1::table::seq_field seq_table_cell_group_cfg_s_g3_fields[] = {
    {offsetof(cell_group_cfg_s, f1c_transfer_path_nrdc_r17),
     (int32_t)offsetof(cell_group_cfg_s, f1c_transfer_path_nrdc_r17_present),
     asn1::table::enum_op<decltype(cell_group_cfg_s::f1c_transfer_path_nrdc_r17)>(),
     decltype(cell_group_cfg_s::f1c_transfer_path_nrdc_r17)::has_ext,
     false,
     decltype(cell_group_cfg_s::f1c_transfer_path_nrdc_r17)::nof_types,
     decltype(cell_group_cfg_s::f1c_transfer_path_nrdc_r17)::nof_exts,
     nullptr,
     &cell_group_cfg_s::f1c_transfer_path_nrdc_r17_opts::desc},
    {0,
     (int32_t)offsetof(cell_group_cfg_s, ul_tx_switching_2_t_mode_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(cell_group_cfg_s, ul_tx_switching_dual_ul_tx_state_r17),
     (int32_t)offsetof(cell_group_cfg_s, ul_tx_switching_dual_ul_tx_state_r17_present),
     asn1::table::enum_op<decltype(cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17)>(),
     decltype(cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17)::has_ext,
     false,
     decltype(cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17)::nof_types,
     decltype(cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17)::nof_exts,
     nullptr,
     &cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17_opts::desc},
    {offsetof(cell_group_cfg_s, uu_relay_rlc_ch_to_add_mod_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_item_list_codec<decltype(cell_group_cfg_s::uu_relay_rlc_ch_to_add_mod_list_r17),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &uu_relay_rlc_ch_cfg_r17_s::desc},
    {offsetof(cell_group_cfg_s, uu_relay_rlc_ch_to_release_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::uu_relay_rlc_ch_to_release_list_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{1}, uint64_t{31}, false, false>>::value},
    {offsetof(cell_group_cfg_s, simul_u_tci_upd_list1_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::simul_u_tci_upd_list1_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(cell_group_cfg_s, simul_u_tci_upd_list2_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::simul_u_tci_upd_list2_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(cell_group_cfg_s, simul_u_tci_upd_list3_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::simul_u_tci_upd_list3_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(cell_group_cfg_s, simul_u_tci_upd_list4_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::simul_u_tci_upd_list4_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{31}, false, false>>::value},
    {offsetof(cell_group_cfg_s, rlc_bearer_to_release_list_ext_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::rlc_bearer_to_release_list_ext_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{320}, uint64_t{65535}, false, false>>::value},
    {offsetof(cell_group_cfg_s, iab_res_cfg_to_add_mod_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65535,
     &asn1::table::ptr_item_list_codec<decltype(cell_group_cfg_s::iab_res_cfg_to_add_mod_list_r17),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &iab_res_cfg_r17_s::desc},
    {offsetof(cell_group_cfg_s, iab_res_cfg_to_release_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65535,
     &asn1::table::ptr_list_codec<decltype(cell_group_cfg_s::iab_res_cfg_to_release_list_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, int64_t{0}, uint64_t{65535}, false, false>>::value}};
static const asn1::table::field_name seq_table_cell_group_cfg_s_g3_names[] = {
    {"f1c-TransferPathNRDC-r17", nullptr},
    {"uplinkTxSwitching-2T-Mode-r17", "enabled"},
    {"uplinkTxSwitching-DualUL-TxState-r17", nullptr},
    {"uu-RelayRLC-ChannelToAddModList-r17", nullptr},
    {"uu-RelayRLC-ChannelToReleaseList-r17", nullptr},
    {"simultaneousU-TCI-UpdateList1-r17", nullptr},
    {"simultaneousU-TCI-UpdateList2-r17", nullptr},
    {"simultaneousU-TCI-UpdateList3-r17", nullptr},
    {"simultaneousU-TCI-UpdateList4-r17", nullptr},
    {"rlc-BearerToReleaseListExt-r17", nullptr},
    {"iab-ResourceConfigToAddModList-r17", nullptr},
    {"iab-ResourceConfigToReleaseList-r17", nullptr}};
static const asn1::table::seq_opt seq_table_cell_group_cfg_s_g3_opts[] = {
    {0, (int32_t)offsetof(cell_group_cfg_s, f1c_transfer_path_nrdc_r17_present)},
    {1, (int32_t)offsetof(cell_group_cfg_s, ul_tx_switching_2_t_mode_r17_present)},
    {2, (int32_t)offsetof(cell_group_cfg_s, ul_tx_switching_dual_ul_tx_state_r17_present)},
    {3, asn1::table::seq_field::ptr_presence},
    {4, asn1::table::seq_field::ptr_presence},
    {5, asn1::table::seq_field::ptr_presence},
    {6, asn1::table::seq_field::ptr_presence},
    {7, asn1::table::seq_field::ptr_presence},
    {8, asn1::table::seq_field::ptr_presence},
    {9, asn1::table::seq_field::ptr_presence},
    {10, asn1::table::seq_field::ptr_presence},
    {11, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_cell_group_cfg_s_g4_fields[] = {
    {offsetof(cell_group_cfg_s, report_ul_tx_direct_current_more_carrier_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::ptr_item_list_codec<decltype(cell_group_cfg_s::report_ul_tx_direct_current_more_carrier_r17),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &intra_band_cc_combination_req_list_r17_s::desc}};
static const asn1::table::field_name seq_table_cell_group_cfg_s_g4_names[] = {
    {"reportUplinkTxDirectCurrentMoreCarrier-r17", nullptr}};
static const asn1::table::seq_opt   seq_table_cell_group_cfg_s_g4_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_cell_group_cfg_s_g5_fields[] = {
    {0,
     (int32_t)offsetof(cell_group_cfg_s, prio_scell_prach_over_sp_periodic_srs_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_cell_group_cfg_s_g5_names[] = {
    {"prioSCellPRACH-OverSP-PeriodicSRS-r17", "enabled"}};
static const asn1::table::seq_opt seq_table_cell_group_cfg_s_g5_opts[] = {
    {0, (int32_t)offsetof(cell_group_cfg_s, prio_scell_prach_over_sp_periodic_srs_r17_present)}};
static const asn1::table::seq_field seq_table_cell_group_cfg_s_g6_fields[] = {
    {offsetof(cell_group_cfg_s, ncr_fwd_cfg_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(cell_group_cfg_s::ncr_fwd_cfg_r18)>::value,
     &seq_table_cell_group_cfg_s_ncr_fwd_cfg_r18_setup_release},
    {offsetof(cell_group_cfg_s, autonomous_denial_params_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(cell_group_cfg_s::autonomous_denial_params_r18)>::value,
     &seq_table_cell_group_cfg_s_autonomous_denial_params_r18_setup_release},
    {0,
     (int32_t)offsetof(cell_group_cfg_s, non_collocated_type_mrdc_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(cell_group_cfg_s, non_collocated_type_nr_ca_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(cell_group_cfg_s, ul_tx_switching_more_bands_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(cell_group_cfg_s::ul_tx_switching_more_bands_r18)>::value,
     &seq_table_cell_group_cfg_s_ul_tx_switching_more_bands_r18_setup_release}};
static const asn1::table::field_name seq_table_cell_group_cfg_s_g6_names[] = {
    {"ncr-FwdConfig-r18", nullptr},
    {"autonomousDenialParameters-r18", nullptr},
    {"nonCollocatedTypeMRDC-r18", "true"},
    {"nonCollocatedTypeNR-CA-r18", "true"},
    {"uplinkTxSwitchingMoreBands-r18", nullptr}};
static const asn1::table::seq_opt seq_table_cell_group_cfg_s_g6_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, (int32_t)offsetof(cell_group_cfg_s, non_collocated_type_mrdc_r18_present)},
    {3, (int32_t)offsetof(cell_group_cfg_s, non_collocated_type_nr_ca_r18_present)},
    {4, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_cell_group_cfg_s_groups[] = {{{seq_table_cell_group_cfg_s_g0_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_cell_group_cfg_s_g0_opts,
                                                                             seq_table_cell_group_cfg_s_g0_names,
                                                                             1,
                                                                             1},
                                                                            false},
                                                                           {{seq_table_cell_group_cfg_s_g1_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_cell_group_cfg_s_g1_opts,
                                                                             seq_table_cell_group_cfg_s_g1_names,
                                                                             10,
                                                                             10},
                                                                            false},
                                                                           {{seq_table_cell_group_cfg_s_g2_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_cell_group_cfg_s_g2_opts,
                                                                             seq_table_cell_group_cfg_s_g2_names,
                                                                             1,
                                                                             1},
                                                                            false},
                                                                           {{seq_table_cell_group_cfg_s_g3_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_cell_group_cfg_s_g3_opts,
                                                                             seq_table_cell_group_cfg_s_g3_names,
                                                                             12,
                                                                             12},
                                                                            false},
                                                                           {{seq_table_cell_group_cfg_s_g4_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_cell_group_cfg_s_g4_opts,
                                                                             seq_table_cell_group_cfg_s_g4_names,
                                                                             1,
                                                                             1},
                                                                            false},
                                                                           {{seq_table_cell_group_cfg_s_g5_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_cell_group_cfg_s_g5_opts,
                                                                             seq_table_cell_group_cfg_s_g5_names,
                                                                             1,
                                                                             1},
                                                                            false},
                                                                           {{seq_table_cell_group_cfg_s_g6_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_cell_group_cfg_s_g6_opts,
                                                                             seq_table_cell_group_cfg_s_g6_names,
                                                                             5,
                                                                             5},
                                                                            false}};
const asn1::table::seq_desc         cell_group_cfg_s::desc              = {{seq_table_cell_group_cfg_s_fields,
                                                                            uint64_t{0x1},
                                                                            seq_table_cell_group_cfg_s_opts,
                                                                            seq_table_cell_group_cfg_s_names,
                                                                            8,
                                                                            7},
                                                                           true,
                                                                           false,
                                                                           seq_table_cell_group_cfg_s_groups,
                                                                           7};

static const char*           enum_names_cell_group_cfg_s_f1c_transfer_path_r16_e__values[] = {"lte", "nr", "both"};
const asn1::table::enum_desc cell_group_cfg_s::f1c_transfer_path_r16_opts::desc            = {
    enum_names_cell_group_cfg_s_f1c_transfer_path_r16_e__values,
    3,
    "cell_group_cfg_s::f1c_transfer_path_r16_e_"};

static const char* enum_names_cell_group_cfg_s_ul_tx_switching_option_r16_e__values[] = {"switchedUL", "dualUL"};
const asn1::table::enum_desc cell_group_cfg_s::ul_tx_switching_option_r16_opts::desc  = {
    enum_names_cell_group_cfg_s_ul_tx_switching_option_r16_e__values,
    2,
    "cell_group_cfg_s::ul_tx_switching_option_r16_e_"};

static const char* enum_names_cell_group_cfg_s_f1c_transfer_path_nrdc_r17_e__values[] = {"mcg", "scg", "both"};
const asn1::table::enum_desc cell_group_cfg_s::f1c_transfer_path_nrdc_r17_opts::desc  = {
    enum_names_cell_group_cfg_s_f1c_transfer_path_nrdc_r17_e__values,
    3,
    "cell_group_cfg_s::f1c_transfer_path_nrdc_r17_e_"};

static const char* enum_names_cell_group_cfg_s_ul_tx_switching_dual_ul_tx_state_r17_e__values[] = {"oneT", "twoT"};
const asn1::table::enum_desc cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17_opts::desc  = {
    enum_names_cell_group_cfg_s_ul_tx_switching_dual_ul_tx_state_r17_e__values,
    2,
    "cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17_e_"};
uint8_t cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "cell_group_cfg_s::ul_tx_switching_dual_ul_tx_state_r17_e_");
}
