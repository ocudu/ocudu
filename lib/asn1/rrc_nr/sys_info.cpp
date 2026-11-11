// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PLMN-IdentityInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_plmn_id_info_s_fields[] = {
    {offsetof(plmn_id_info_s, plmn_id_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::
         item_list_codec<decltype(plmn_id_info_s::plmn_id_list), asn1::table::sequence_item_ops, false, true>::value,
     &plmn_id_s::desc},
    {offsetof(plmn_id_info_s, tac),
     (int32_t)offsetof(plmn_id_info_s, tac_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(plmn_id_info_s::tac), asn1::table::struct_json>::value},
    {offsetof(plmn_id_info_s, ranac),
     (int32_t)offsetof(plmn_id_info_s, ranac_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(plmn_id_info_s, cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(plmn_id_info_s::cell_id), asn1::table::struct_json>::value},
    {offsetof(plmn_id_info_s, cell_reserved_for_oper),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(plmn_id_info_s::cell_reserved_for_oper)>(),
     decltype(plmn_id_info_s::cell_reserved_for_oper)::has_ext,
     false,
     decltype(plmn_id_info_s::cell_reserved_for_oper)::nof_types,
     decltype(plmn_id_info_s::cell_reserved_for_oper)::nof_exts,
     nullptr,
     &plmn_id_info_s::cell_reserved_for_oper_opts::desc}};
static const asn1::table::field_name seq_table_plmn_id_info_s_names[] = {{"plmn-IdentityList", nullptr},
                                                                         {"trackingAreaCode", nullptr},
                                                                         {"ranac", nullptr},
                                                                         {"cellIdentity", nullptr},
                                                                         {"cellReservedForOperatorUse", nullptr}};
static const asn1::table::seq_opt    seq_table_plmn_id_info_s_opts[]  = {
    {1, (int32_t)offsetof(plmn_id_info_s, tac_present)},
    {2, (int32_t)offsetof(plmn_id_info_s, ranac_present)}};
static const asn1::table::seq_field seq_table_plmn_id_info_s_g0_fields[] = {
    {0,
     (int32_t)offsetof(plmn_id_info_s, iab_support_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_plmn_id_info_s_g0_names[] = {{"iab-Support-r16", "true"}};
static const asn1::table::seq_opt    seq_table_plmn_id_info_s_g0_opts[]  = {
    {0, (int32_t)offsetof(plmn_id_info_s, iab_support_r16_present)}};
static const asn1::table::seq_field seq_table_plmn_id_info_s_g1_fields[] = {
    {offsetof(plmn_id_info_s, tracking_area_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::ptr_list_codec<decltype(plmn_id_info_s::tracking_area_list_r17), asn1::table::struct_json>::value},
    {offsetof(plmn_id_info_s, gnb_id_len_r17),
     (int32_t)offsetof(plmn_id_info_s, gnb_id_len_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(22),
     UINT64_C(32),
     nullptr}};
static const asn1::table::field_name seq_table_plmn_id_info_s_g1_names[] = {{"trackingAreaList-r17", nullptr},
                                                                            {"gNB-ID-Length-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_plmn_id_info_s_g1_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(plmn_id_info_s, gnb_id_len_r17_present)}};
static const asn1::table::seq_field seq_table_plmn_id_info_s_g2_fields[] = {
    {0,
     (int32_t)offsetof(plmn_id_info_s, mobile_iab_support_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_plmn_id_info_s_g2_names[] = {{"mobileIAB-Support-r18", "true"}};
static const asn1::table::seq_opt    seq_table_plmn_id_info_s_g2_opts[]  = {
    {0, (int32_t)offsetof(plmn_id_info_s, mobile_iab_support_r18_present)}};
static const asn1::table::seq_group seq_table_plmn_id_info_s_groups[] = {
    {{seq_table_plmn_id_info_s_g0_fields, 1, UINT64_C(0x0), seq_table_plmn_id_info_s_g0_opts, 1},
     false,
     seq_table_plmn_id_info_s_g0_names},
    {{seq_table_plmn_id_info_s_g1_fields, 2, UINT64_C(0x0), seq_table_plmn_id_info_s_g1_opts, 2},
     false,
     seq_table_plmn_id_info_s_g1_names},
    {{seq_table_plmn_id_info_s_g2_fields, 1, UINT64_C(0x0), seq_table_plmn_id_info_s_g2_opts, 1},
     false,
     seq_table_plmn_id_info_s_g2_names}};
const asn1::table::seq_desc plmn_id_info_s::desc = {
    {seq_table_plmn_id_info_s_fields, 5, UINT64_C(0x19), seq_table_plmn_id_info_s_opts, 2},
    true,
    false,
    seq_table_plmn_id_info_s_groups,
    3,
    seq_table_plmn_id_info_s_names};

static const char*           enum_names_plmn_id_info_s_cell_reserved_for_oper_e__values[] = {"reserved", "notReserved"};
const asn1::table::enum_desc plmn_id_info_s::cell_reserved_for_oper_opts::desc            = {
    enum_names_plmn_id_info_s_cell_reserved_for_oper_e__values,
    2,
    "plmn_id_info_s::cell_reserved_for_oper_e_"};

// CAG-IdentityInfo-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cag_id_info_r16_s_fields[] = {
    {offsetof(cag_id_info_r16_s, cag_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cag_id_info_r16_s::cag_id_r16), asn1::table::struct_json>::value},
    {0,
     (int32_t)offsetof(cag_id_info_r16_s, manual_ca_gsel_allowed_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_cag_id_info_r16_s_names[] = {{"cag-Identity-r16", nullptr},
                                                                            {"manualCAGselectionAllowed-r16", "true"}};
static const asn1::table::seq_opt    seq_table_cag_id_info_r16_s_opts[]  = {
    {1, (int32_t)offsetof(cag_id_info_r16_s, manual_ca_gsel_allowed_r16_present)}};
const asn1::table::seq_desc cag_id_info_r16_s::desc = {
    {seq_table_cag_id_info_r16_s_fields, 2, UINT64_C(0x1), seq_table_cag_id_info_r16_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_cag_id_info_r16_s_names};

// NPN-Identity-r16 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_npn_id_r16_c_alternatives[] = {
    &asn1::table::alternative_ops<npn_id_r16_c::pni_npn_r16_s_, false>::value,
    &asn1::table::alternative_ops<npn_id_r16_c::snpn_r16_s_, false>::value};
const asn1::table::choice_storage_desc npn_id_r16_c::storage_desc = {choice_storage_npn_id_r16_c_alternatives,
                                                                     2,
                                                                     "npn_id_r16_c"};

npn_id_r16_c::pni_npn_r16_s_& npn_id_r16_c::set_pni_npn_r16()
{
  set(types::pni_npn_r16);
  return c.get<pni_npn_r16_s_>();
}
npn_id_r16_c::snpn_r16_s_& npn_id_r16_c::set_snpn_r16()
{
  set(types::snpn_r16);
  return c.get<snpn_r16_s_>();
}
static const asn1::table::seq_field seq_table_npn_id_r16_c_pni_npn_r16_s__fields[] = {
    {offsetof(npn_id_r16_c::pni_npn_r16_s_, plmn_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc},
    {offsetof(npn_id_r16_c::pni_npn_r16_s_, cag_id_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::item_list_codec<decltype(npn_id_r16_c::pni_npn_r16_s_::cag_id_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cag_id_info_r16_s::desc}};
static const asn1::table::field_name seq_table_npn_id_r16_c_pni_npn_r16_s__names[] = {
    {"plmn-Identity-r16", nullptr},
    {"cag-IdentityList-r16", nullptr}};
static const asn1::table::seq_desc seq_table_npn_id_r16_c_pni_npn_r16_s_ = {
    {seq_table_npn_id_r16_c_pni_npn_r16_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_npn_id_r16_c_pni_npn_r16_s__names};
static const asn1::table::seq_field seq_table_npn_id_r16_c_snpn_r16_s__fields[] = {
    {offsetof(npn_id_r16_c::snpn_r16_s_, plmn_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc},
    {offsetof(npn_id_r16_c::snpn_r16_s_, nid_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::list_codec<decltype(npn_id_r16_c::snpn_r16_s_::nid_list_r16), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_npn_id_r16_c_snpn_r16_s__names[] = {{"plmn-Identity-r16", nullptr},
                                                                                   {"nid-List-r16", nullptr}};
static const asn1::table::seq_desc   seq_table_npn_id_r16_c_snpn_r16_s_         = {
    {seq_table_npn_id_r16_c_snpn_r16_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_npn_id_r16_c_snpn_r16_s__names};
static const asn1::table::seq_field choice_table_npn_id_r16_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_npn_id_r16_c_pni_npn_r16_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_npn_id_r16_c_snpn_r16_s_}};
static const asn1::table::field_name choice_table_npn_id_r16_c_names[] = {{"pni-npn-r16", nullptr},
                                                                          {"snpn-r16", nullptr}};
const asn1::table::choice_desc       npn_id_r16_c::desc = {asn1::table::enum_int_type<npn_id_r16_c::types>(),
                                                           npn_id_r16_c::types::nof_types,
                                                           npn_id_r16_c::types::nof_exts,
                                                           npn_id_r16_c::types::has_ext,
                                                           false,
                                                           choice_table_npn_id_r16_c_alternatives,
                                                           nullptr,
                                                           choice_table_npn_id_r16_c_names,
                                                           "npn_id_r16_c",
                                                           offsetof(npn_id_r16_c, c.type),
                                                           offsetof(npn_id_r16_c, c.value),
                                                           false,
                                                           &npn_id_r16_c::storage_desc};

static const char*           enum_names_npn_id_r16_c_types_values[] = {"pni-npn-r16", "snpn-r16"};
const asn1::table::enum_desc npn_id_r16_c::types_opts::desc         = {enum_names_npn_id_r16_c_types_values,
                                                                       2,
                                                                       "npn_id_r16_c::types"};

// NPN-IdentityInfo-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_npn_id_info_r16_s_fields[] = {
    {offsetof(npn_id_info_r16_s, npn_id_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::item_list_codec<decltype(npn_id_info_r16_s::npn_id_list_r16),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &npn_id_r16_c::desc},
    {offsetof(npn_id_info_r16_s, tac_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(npn_id_info_r16_s::tac_r16), asn1::table::struct_json>::value},
    {offsetof(npn_id_info_r16_s, ranac_r16),
     (int32_t)offsetof(npn_id_info_r16_s, ranac_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(255),
     nullptr},
    {offsetof(npn_id_info_r16_s, cell_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(npn_id_info_r16_s::cell_id_r16), asn1::table::struct_json>::value},
    {offsetof(npn_id_info_r16_s, cell_reserved_for_oper_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(npn_id_info_r16_s::cell_reserved_for_oper_r16)>(),
     decltype(npn_id_info_r16_s::cell_reserved_for_oper_r16)::has_ext,
     false,
     decltype(npn_id_info_r16_s::cell_reserved_for_oper_r16)::nof_types,
     decltype(npn_id_info_r16_s::cell_reserved_for_oper_r16)::nof_exts,
     nullptr,
     &npn_id_info_r16_s::cell_reserved_for_oper_r16_opts::desc},
    {0,
     (int32_t)offsetof(npn_id_info_r16_s, iab_support_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_npn_id_info_r16_s_names[] = {{"npn-IdentityList-r16", nullptr},
                                                                            {"trackingAreaCode-r16", nullptr},
                                                                            {"ranac-r16", nullptr},
                                                                            {"cellIdentity-r16", nullptr},
                                                                            {"cellReservedForOperatorUse-r16", nullptr},
                                                                            {"iab-Support-r16", "true"}};
static const asn1::table::seq_opt    seq_table_npn_id_info_r16_s_opts[]  = {
    {2, (int32_t)offsetof(npn_id_info_r16_s, ranac_r16_present)},
    {5, (int32_t)offsetof(npn_id_info_r16_s, iab_support_r16_present)}};
static const asn1::table::seq_field seq_table_npn_id_info_r16_s_g0_fields[] = {
    {offsetof(npn_id_info_r16_s, gnb_id_len_r17),
     (int32_t)offsetof(npn_id_info_r16_s, gnb_id_len_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(22),
     UINT64_C(32),
     nullptr}};
static const asn1::table::field_name seq_table_npn_id_info_r16_s_g0_names[] = {{"gNB-ID-Length-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_npn_id_info_r16_s_g0_opts[]  = {
    {0, (int32_t)offsetof(npn_id_info_r16_s, gnb_id_len_r17_present)}};
static const asn1::table::seq_field seq_table_npn_id_info_r16_s_g1_fields[] = {
    {0,
     (int32_t)offsetof(npn_id_info_r16_s, mobile_iab_support_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_npn_id_info_r16_s_g1_names[] = {{"mobileIAB-Support-r18", "true"}};
static const asn1::table::seq_opt    seq_table_npn_id_info_r16_s_g1_opts[]  = {
    {0, (int32_t)offsetof(npn_id_info_r16_s, mobile_iab_support_r18_present)}};
static const asn1::table::seq_group seq_table_npn_id_info_r16_s_groups[] = {
    {{seq_table_npn_id_info_r16_s_g0_fields, 1, UINT64_C(0x0), seq_table_npn_id_info_r16_s_g0_opts, 1},
     false,
     seq_table_npn_id_info_r16_s_g0_names},
    {{seq_table_npn_id_info_r16_s_g1_fields, 1, UINT64_C(0x0), seq_table_npn_id_info_r16_s_g1_opts, 1},
     false,
     seq_table_npn_id_info_r16_s_g1_names}};
const asn1::table::seq_desc npn_id_info_r16_s::desc = {
    {seq_table_npn_id_info_r16_s_fields, 6, UINT64_C(0x1b), seq_table_npn_id_info_r16_s_opts, 2},
    true,
    false,
    seq_table_npn_id_info_r16_s_groups,
    2,
    seq_table_npn_id_info_r16_s_names};

static const char* enum_names_npn_id_info_r16_s_cell_reserved_for_oper_r16_e__values[] = {"reserved", "notReserved"};
const asn1::table::enum_desc npn_id_info_r16_s::cell_reserved_for_oper_r16_opts::desc  = {
    enum_names_npn_id_info_r16_s_cell_reserved_for_oper_r16_e__values,
    2,
    "npn_id_info_r16_s::cell_reserved_for_oper_r16_e_"};

// Q-OffsetRange ::= ENUMERATED
static const char* enum_names_q_offset_range_e_values[] = {
    "dB-24", "dB-22", "dB-20", "dB-18", "dB-16", "dB-14", "dB-12", "dB-10", "dB-8", "dB-6", "dB-5",
    "dB-4",  "dB-3",  "dB-2",  "dB-1",  "dB0",   "dB1",   "dB2",   "dB3",   "dB4",  "dB5",  "dB6",
    "dB8",   "dB10",  "dB12",  "dB14",  "dB16",  "dB18",  "dB20",  "dB22",  "dB24"};
const asn1::table::enum_desc q_offset_range_opts::desc = {enum_names_q_offset_range_e_values, 31, "q_offset_range_e"};
int8_t                       q_offset_range_opts::to_number() const
{
  static const int8_t numbers[] = {-24, -22, -20, -18, -16, -14, -12, -10, -8, -6, -5, -4, -3, -2, -1, 0,
                                   1,   2,   3,   4,   5,   6,   8,   10,  12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(numbers, 31, value, "q_offset_range_e");
}

// SSB-MTC ::= SEQUENCE
static const asn1::table::seq_field  seq_table_ssb_mtc_s_fields[] = {{offsetof(ssb_mtc_s, periodicity_and_offset),
                                                                      asn1::table::seq_field::mandatory,
                                                                      asn1::table::field_op::choice,
                                                                      false,
                                                                      false,
                                                                      0,
                                                                      0,
                                                                      nullptr,
                                                                      &ssb_mtc_s::periodicity_and_offset_c_::desc},
                                                                     {offsetof(ssb_mtc_s, dur),
                                                                      asn1::table::seq_field::mandatory,
                                                                      asn1::table::enum_op<decltype(ssb_mtc_s::dur)>(),
                                                                      decltype(ssb_mtc_s::dur)::has_ext,
                                                                      false,
                                                                      decltype(ssb_mtc_s::dur)::nof_types,
                                                                      decltype(ssb_mtc_s::dur)::nof_exts,
                                                                      nullptr,
                                                                      &ssb_mtc_s::dur_opts::desc}};
static const asn1::table::field_name seq_table_ssb_mtc_s_names[]  = {{"periodicityAndOffset", nullptr},
                                                                     {"duration", nullptr}};
const asn1::table::seq_desc          ssb_mtc_s::desc =
    {{seq_table_ssb_mtc_s_fields, 2, UINT64_C(0x3), nullptr, 0}, false, false, nullptr, 0, seq_table_ssb_mtc_s_names};

static const asn1::table::storage_ops* const choice_storage_ssb_mtc_s_periodicity_and_offset_c__alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc ssb_mtc_s::periodicity_and_offset_c_::storage_desc = {
    choice_storage_ssb_mtc_s_periodicity_and_offset_c__alternatives,
    6,
    "ssb_mtc_s::periodicity_and_offset_c_"};

uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf5()
{
  set(types::sf5);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf10()
{
  set(types::sf10);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf20()
{
  set(types::sf20);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf40()
{
  set(types::sf40);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf80()
{
  set(types::sf80);
  return c.get<uint8_t>();
}
uint8_t& ssb_mtc_s::periodicity_and_offset_c_::set_sf160()
{
  set(types::sf160);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_ssb_mtc_s_periodicity_and_offset_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(19),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(39),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(79),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(159),
     nullptr}};
static const asn1::table::field_name choice_table_ssb_mtc_s_periodicity_and_offset_c__names[] =
    {{"sf5", nullptr}, {"sf10", nullptr}, {"sf20", nullptr}, {"sf40", nullptr}, {"sf80", nullptr}, {"sf160", nullptr}};
const asn1::table::choice_desc ssb_mtc_s::periodicity_and_offset_c_::desc = {
    asn1::table::enum_int_type<ssb_mtc_s::periodicity_and_offset_c_::types>(),
    ssb_mtc_s::periodicity_and_offset_c_::types::nof_types,
    ssb_mtc_s::periodicity_and_offset_c_::types::nof_exts,
    ssb_mtc_s::periodicity_and_offset_c_::types::has_ext,
    false,
    choice_table_ssb_mtc_s_periodicity_and_offset_c__alternatives,
    nullptr,
    choice_table_ssb_mtc_s_periodicity_and_offset_c__names,
    "ssb_mtc_s::periodicity_and_offset_c_",
    offsetof(ssb_mtc_s::periodicity_and_offset_c_, c.type),
    offsetof(ssb_mtc_s::periodicity_and_offset_c_, c.value),
    false,
    &ssb_mtc_s::periodicity_and_offset_c_::storage_desc};

static const char* enum_names_ssb_mtc_s_periodicity_and_offset_c__types_values[] =
    {"sf5", "sf10", "sf20", "sf40", "sf80", "sf160"};
const asn1::table::enum_desc ssb_mtc_s::periodicity_and_offset_c_::types_opts::desc = {
    enum_names_ssb_mtc_s_periodicity_and_offset_c__types_values,
    6,
    "ssb_mtc_s::periodicity_and_offset_c_::types"};
uint8_t ssb_mtc_s::periodicity_and_offset_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "ssb_mtc_s::periodicity_and_offset_c_::types");
}

static const char*           enum_names_ssb_mtc_s_dur_e__values[] = {"sf1", "sf2", "sf3", "sf4", "sf5"};
const asn1::table::enum_desc ssb_mtc_s::dur_opts::desc = {enum_names_ssb_mtc_s_dur_e__values, 5, "ssb_mtc_s::dur_e_"};
uint8_t                      ssb_mtc_s::dur_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5};
  return map_enum_number(numbers, 5, value, "ssb_mtc_s::dur_e_");
}

// SS-RSSI-Measurement ::= SEQUENCE
static const asn1::table::seq_field seq_table_ss_rssi_meas_s_fields[] = {
    {offsetof(ss_rssi_meas_s, meas_slots),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ss_rssi_meas_s::meas_slots), asn1::table::struct_json>::value},
    {offsetof(ss_rssi_meas_s, end_symbol),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr}};
static const asn1::table::field_name seq_table_ss_rssi_meas_s_names[] = {{"measurementSlots", nullptr},
                                                                         {"endSymbol", nullptr}};
const asn1::table::seq_desc ss_rssi_meas_s::desc = {{seq_table_ss_rssi_meas_s_fields, 2, UINT64_C(0x3), nullptr, 0},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0,
                                                    seq_table_ss_rssi_meas_s_names};

// SSB-ToMeasure ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ssb_to_measure_c_alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<4>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<8>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value};
const asn1::table::choice_storage_desc ssb_to_measure_c::storage_desc = {choice_storage_ssb_to_measure_c_alternatives,
                                                                         3,
                                                                         "ssb_to_measure_c"};

fixed_bitstring<4>& ssb_to_measure_c::set_short_bitmap()
{
  set(types::short_bitmap);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<8>& ssb_to_measure_c::set_medium_bitmap()
{
  set(types::medium_bitmap);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& ssb_to_measure_c::set_long_bitmap()
{
  set(types::long_bitmap);
  return c.get<fixed_bitstring<64>>();
}
static const asn1::table::seq_field choice_table_ssb_to_measure_c_alternatives[] = {
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
static const asn1::table::field_name choice_table_ssb_to_measure_c_names[] = {{"shortBitmap", nullptr},
                                                                              {"mediumBitmap", nullptr},
                                                                              {"longBitmap", nullptr}};
const asn1::table::choice_desc       ssb_to_measure_c::desc = {asn1::table::enum_int_type<ssb_to_measure_c::types>(),
                                                               ssb_to_measure_c::types::nof_types,
                                                               ssb_to_measure_c::types::nof_exts,
                                                               ssb_to_measure_c::types::has_ext,
                                                               false,
                                                               choice_table_ssb_to_measure_c_alternatives,
                                                               nullptr,
                                                               choice_table_ssb_to_measure_c_names,
                                                               "ssb_to_measure_c",
                                                               offsetof(ssb_to_measure_c, c.type),
                                                               offsetof(ssb_to_measure_c, c.value),
                                                               false,
                                                               &ssb_to_measure_c::storage_desc};

static const char*           enum_names_ssb_to_measure_c_types_values[] = {"shortBitmap", "mediumBitmap", "longBitmap"};
const asn1::table::enum_desc ssb_to_measure_c::types_opts::desc         = {enum_names_ssb_to_measure_c_types_values,
                                                                           3,
                                                                           "ssb_to_measure_c::types"};

// ATG-Config-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_atg_cfg_r18_s_fields[] = {
    {offsetof(atg_cfg_r18_s, atg_gnb_location_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(atg_cfg_r18_s::atg_gnb_location_r18), asn1::table::string_json>::value},
    {offsetof(atg_cfg_r18_s, height_gnb_r18),
     (int32_t)offsetof(atg_cfg_r18_s, height_gnb_r18_present),
     asn1::table::field_op::int_i16,
     false,
     false,
     INT64_C(-16384),
     UINT64_C(16383),
     nullptr},
    {offsetof(atg_cfg_r18_s, cell_specific_koffset_r18),
     (int32_t)offsetof(atg_cfg_r18_s, cell_specific_koffset_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(3),
     nullptr},
    {0,
     (int32_t)offsetof(atg_cfg_r18_s, ta_report_atg_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_atg_cfg_r18_s_names[] = {{"atg-gNB-Location-r18", nullptr},
                                                                        {"height-gNB-r18", nullptr},
                                                                        {"cellSpecificKoffset-r18", nullptr},
                                                                        {"ta-ReportATG-r18", "enabled"}};
static const asn1::table::seq_opt    seq_table_atg_cfg_r18_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(atg_cfg_r18_s, height_gnb_r18_present)},
    {2, (int32_t)offsetof(atg_cfg_r18_s, cell_specific_koffset_r18_present)},
    {3, (int32_t)offsetof(atg_cfg_r18_s, ta_report_atg_r18_present)}};
const asn1::table::seq_desc atg_cfg_r18_s::desc = {
    {seq_table_atg_cfg_r18_s_fields, 4, UINT64_C(0x0), seq_table_atg_cfg_r18_s_opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_atg_cfg_r18_s_names};

// ATG-NeighCellConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_atg_neigh_cell_cfg_r18_s_fields[] = {
    {offsetof(atg_neigh_cell_cfg_r18_s, atg_gnb_location_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(atg_neigh_cell_cfg_r18_s::atg_gnb_location_r18),
                                      asn1::table::string_json>::value},
    {offsetof(atg_neigh_cell_cfg_r18_s, height_gnb_r18),
     (int32_t)offsetof(atg_neigh_cell_cfg_r18_s, height_gnb_r18_present),
     asn1::table::field_op::int_i16,
     false,
     false,
     INT64_C(-16384),
     UINT64_C(16383),
     nullptr},
    {offsetof(atg_neigh_cell_cfg_r18_s, carrier_freq_r18),
     (int32_t)offsetof(atg_neigh_cell_cfg_r18_s, carrier_freq_r18_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(atg_neigh_cell_cfg_r18_s, pci_r18),
     (int32_t)offsetof(atg_neigh_cell_cfg_r18_s, pci_r18_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr}};
static const asn1::table::field_name seq_table_atg_neigh_cell_cfg_r18_s_names[] = {{"atg-gNB-Location-r18", nullptr},
                                                                                   {"height-gNB-r18", nullptr},
                                                                                   {"carrierFreq-r18", nullptr},
                                                                                   {"physCellId-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_atg_neigh_cell_cfg_r18_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(atg_neigh_cell_cfg_r18_s, height_gnb_r18_present)},
    {2, (int32_t)offsetof(atg_neigh_cell_cfg_r18_s, carrier_freq_r18_present)},
    {3, (int32_t)offsetof(atg_neigh_cell_cfg_r18_s, pci_r18_present)}};
const asn1::table::seq_desc atg_neigh_cell_cfg_r18_s::desc = {
    {seq_table_atg_neigh_cell_cfg_r18_s_fields, 4, UINT64_C(0x0), seq_table_atg_neigh_cell_cfg_r18_s_opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_atg_neigh_cell_cfg_r18_s_names};

// ApplicableDisasterInfo-r17 ::= CHOICE
void applicable_disaster_info_r17_c::set(types::options e)
{
  type_ = e;
}
void applicable_disaster_info_r17_c::set_no_disaster_roaming_r17()
{
  set(types::no_disaster_roaming_r17);
}
void applicable_disaster_info_r17_c::set_disaster_related_ind_r17()
{
  set(types::disaster_related_ind_r17);
}
void applicable_disaster_info_r17_c::set_common_plmns_r17()
{
  set(types::common_plmns_r17);
}
applicable_disaster_info_r17_c::ded_plmns_r17_l_& applicable_disaster_info_r17_c::set_ded_plmns_r17()
{
  set(types::ded_plmns_r17);
  return c;
}
static const asn1::table::seq_field choice_table_applicable_disaster_info_r17_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::list_codec<applicable_disaster_info_r17_c::ded_plmns_r17_l_, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_applicable_disaster_info_r17_c_names[] = {
    {"noDisasterRoaming-r17", nullptr},
    {"disasterRelatedIndication-r17", nullptr},
    {"commonPLMNs-r17", nullptr},
    {"dedicatedPLMNs-r17", nullptr}};
const asn1::table::choice_desc applicable_disaster_info_r17_c::desc = {
    asn1::table::enum_int_type<applicable_disaster_info_r17_c::types>(),
    applicable_disaster_info_r17_c::types::nof_types,
    applicable_disaster_info_r17_c::types::nof_exts,
    applicable_disaster_info_r17_c::types::has_ext,
    false,
    choice_table_applicable_disaster_info_r17_c_alternatives,
    &asn1::table::choice_set<applicable_disaster_info_r17_c>,
    choice_table_applicable_disaster_info_r17_c_names,
    "applicable_disaster_info_r17_c",
    offsetof(applicable_disaster_info_r17_c, type_),
    offsetof(applicable_disaster_info_r17_c, c),
    false};

static const char*           enum_names_applicable_disaster_info_r17_c_types_values[] = {"noDisasterRoaming-r17",
                                                                                         "disasterRelatedIndication-r17",
                                                                                         "commonPLMNs-r17",
                                                                                         "dedicatedPLMNs-r17"};
const asn1::table::enum_desc applicable_disaster_info_r17_c::types_opts::desc         = {
    enum_names_applicable_disaster_info_r17_c_types_values,
    4,
    "applicable_disaster_info_r17_c::types"};

// ThresholdNR ::= SEQUENCE
static const asn1::table::seq_field  seq_table_thres_nr_s_fields[] = {{offsetof(thres_nr_s, thres_rsrp),
                                                                       (int32_t)offsetof(thres_nr_s, thres_rsrp_present),
                                                                       asn1::table::field_op::int_u8,
                                                                       false,
                                                                       false,
                                                                       INT64_C(0),
                                                                       UINT64_C(127),
                                                                       nullptr},
                                                                      {offsetof(thres_nr_s, thres_rsrq),
                                                                       (int32_t)offsetof(thres_nr_s, thres_rsrq_present),
                                                                       asn1::table::field_op::int_u8,
                                                                       false,
                                                                       false,
                                                                       INT64_C(0),
                                                                       UINT64_C(127),
                                                                       nullptr},
                                                                      {offsetof(thres_nr_s, thres_sinr),
                                                                       (int32_t)offsetof(thres_nr_s, thres_sinr_present),
                                                                       asn1::table::field_op::int_u8,
                                                                       false,
                                                                       false,
                                                                       INT64_C(0),
                                                                       UINT64_C(127),
                                                                       nullptr}};
static const asn1::table::field_name seq_table_thres_nr_s_names[]  = {{"thresholdRSRP", nullptr},
                                                                      {"thresholdRSRQ", nullptr},
                                                                      {"thresholdSINR", nullptr}};
static const asn1::table::seq_opt    seq_table_thres_nr_s_opts[]   = {
    {0, (int32_t)offsetof(thres_nr_s, thres_rsrp_present)},
    {1, (int32_t)offsetof(thres_nr_s, thres_rsrq_present)},
    {2, (int32_t)offsetof(thres_nr_s, thres_sinr_present)}};
const asn1::table::seq_desc thres_nr_s::desc = {
    {seq_table_thres_nr_s_fields, 3, UINT64_C(0x0), seq_table_thres_nr_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_thres_nr_s_names};

// MobilityStateParameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_mob_state_params_s_fields[] = {
    {offsetof(mob_state_params_s, t_eval),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mob_state_params_s::t_eval)>(),
     decltype(mob_state_params_s::t_eval)::has_ext,
     false,
     decltype(mob_state_params_s::t_eval)::nof_types,
     decltype(mob_state_params_s::t_eval)::nof_exts,
     nullptr,
     &mob_state_params_s::t_eval_opts::desc},
    {offsetof(mob_state_params_s, t_hyst_normal),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mob_state_params_s::t_hyst_normal)>(),
     decltype(mob_state_params_s::t_hyst_normal)::has_ext,
     false,
     decltype(mob_state_params_s::t_hyst_normal)::nof_types,
     decltype(mob_state_params_s::t_hyst_normal)::nof_exts,
     nullptr,
     &mob_state_params_s::t_hyst_normal_opts::desc},
    {offsetof(mob_state_params_s, n_cell_change_medium),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(16),
     nullptr},
    {offsetof(mob_state_params_s, n_cell_change_high),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(16),
     nullptr}};
static const asn1::table::field_name seq_table_mob_state_params_s_names[] = {{"t-Evaluation", nullptr},
                                                                             {"t-HystNormal", nullptr},
                                                                             {"n-CellChangeMedium", nullptr},
                                                                             {"n-CellChangeHigh", nullptr}};
const asn1::table::seq_desc          mob_state_params_s::desc             = {
    {seq_table_mob_state_params_s_fields, 4, UINT64_C(0xf), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_mob_state_params_s_names};

static const char* enum_names_mob_state_params_s_t_eval_e__values[] =
    {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc mob_state_params_s::t_eval_opts::desc = {enum_names_mob_state_params_s_t_eval_e__values,
                                                                      8,
                                                                      "mob_state_params_s::t_eval_e_"};
uint8_t                      mob_state_params_s::t_eval_opts::to_number() const
{
  static const uint8_t numbers[] = {30, 60, 120, 180, 240};
  return map_enum_number(numbers, 5, value, "mob_state_params_s::t_eval_e_");
}

static const char* enum_names_mob_state_params_s_t_hyst_normal_e__values[] =
    {"s30", "s60", "s120", "s180", "s240", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc mob_state_params_s::t_hyst_normal_opts::desc = {
    enum_names_mob_state_params_s_t_hyst_normal_e__values,
    8,
    "mob_state_params_s::t_hyst_normal_e_"};
uint8_t mob_state_params_s::t_hyst_normal_opts::to_number() const
{
  static const uint8_t numbers[] = {30, 60, 120, 180, 240};
  return map_enum_number(numbers, 5, value, "mob_state_params_s::t_hyst_normal_e_");
}

// CellReselectionSubPriority ::= ENUMERATED
static const char*           enum_names_cell_resel_sub_prio_e_values[] = {"oDot2", "oDot4", "oDot6", "oDot8"};
const asn1::table::enum_desc cell_resel_sub_prio_opts::desc            = {enum_names_cell_resel_sub_prio_e_values,
                                                                          4,
                                                                          "cell_resel_sub_prio_e"};
float                        cell_resel_sub_prio_opts::to_number() const
{
  static const float numbers[] = {0.2, 0.4, 0.6, 0.8};
  return map_enum_number(numbers, 4, value, "cell_resel_sub_prio_e");
}
const char* cell_resel_sub_prio_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.2", "0.4", "0.6", "0.8"};
  return convert_enum_idx(number_strs, 4, value, "cell_resel_sub_prio_e");
}

// SpeedStateScaleFactors ::= SEQUENCE
static const asn1::table::seq_field seq_table_speed_state_scale_factors_s_fields[] = {
    {offsetof(speed_state_scale_factors_s, sf_medium),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(speed_state_scale_factors_s::sf_medium)>(),
     decltype(speed_state_scale_factors_s::sf_medium)::has_ext,
     false,
     decltype(speed_state_scale_factors_s::sf_medium)::nof_types,
     decltype(speed_state_scale_factors_s::sf_medium)::nof_exts,
     nullptr,
     &speed_state_scale_factors_s::sf_medium_opts::desc},
    {offsetof(speed_state_scale_factors_s, sf_high),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(speed_state_scale_factors_s::sf_high)>(),
     decltype(speed_state_scale_factors_s::sf_high)::has_ext,
     false,
     decltype(speed_state_scale_factors_s::sf_high)::nof_types,
     decltype(speed_state_scale_factors_s::sf_high)::nof_exts,
     nullptr,
     &speed_state_scale_factors_s::sf_high_opts::desc}};
static const asn1::table::field_name seq_table_speed_state_scale_factors_s_names[] = {{"sf-Medium", nullptr},
                                                                                      {"sf-High", nullptr}};
const asn1::table::seq_desc          speed_state_scale_factors_s::desc             = {
    {seq_table_speed_state_scale_factors_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_speed_state_scale_factors_s_names};

static const char*           enum_names_speed_state_scale_factors_s_sf_medium_e__values[] = {"oDot25",
                                                                                             "oDot5",
                                                                                             "oDot75",
                                                                                             "lDot0"};
const asn1::table::enum_desc speed_state_scale_factors_s::sf_medium_opts::desc            = {
    enum_names_speed_state_scale_factors_s_sf_medium_e__values,
    4,
    "speed_state_scale_factors_s::sf_medium_e_"};
float speed_state_scale_factors_s::sf_medium_opts::to_number() const
{
  static const float numbers[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(numbers, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}
const char* speed_state_scale_factors_s::sf_medium_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(number_strs, 4, value, "speed_state_scale_factors_s::sf_medium_e_");
}

static const char* enum_names_speed_state_scale_factors_s_sf_high_e__values[] = {"oDot25", "oDot5", "oDot75", "lDot0"};
const asn1::table::enum_desc speed_state_scale_factors_s::sf_high_opts::desc  = {
    enum_names_speed_state_scale_factors_s_sf_high_e__values,
    4,
    "speed_state_scale_factors_s::sf_high_e_"};
float speed_state_scale_factors_s::sf_high_opts::to_number() const
{
  static const float numbers[] = {0.25, 0.5, 0.75, 1.0};
  return map_enum_number(numbers, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}
const char* speed_state_scale_factors_s::sf_high_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.25", "0.5", "0.75", "1.0"};
  return convert_enum_idx(number_strs, 4, value, "speed_state_scale_factors_s::sf_high_e_");
}

// SSB-MTC2-LP-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ssb_mtc2_lp_r16_s_fields[] = {
    {offsetof(ssb_mtc2_lp_r16_s, pci_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(ssb_mtc2_lp_r16_s::pci_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(1007), false, false>>::value},
    {offsetof(ssb_mtc2_lp_r16_s, periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ssb_mtc2_lp_r16_s::periodicity)>(),
     decltype(ssb_mtc2_lp_r16_s::periodicity)::has_ext,
     false,
     decltype(ssb_mtc2_lp_r16_s::periodicity)::nof_types,
     decltype(ssb_mtc2_lp_r16_s::periodicity)::nof_exts,
     nullptr,
     &ssb_mtc2_lp_r16_s::periodicity_opts::desc}};
static const asn1::table::field_name seq_table_ssb_mtc2_lp_r16_s_names[] = {{"pci-List", nullptr},
                                                                            {"periodicity", nullptr}};
static const asn1::table::seq_opt    seq_table_ssb_mtc2_lp_r16_s_opts[]  = {{0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          ssb_mtc2_lp_r16_s::desc             = {
    {seq_table_ssb_mtc2_lp_r16_s_fields, 2, UINT64_C(0x2), seq_table_ssb_mtc2_lp_r16_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_ssb_mtc2_lp_r16_s_names};

static const char* enum_names_ssb_mtc2_lp_r16_s_periodicity_e__values[] =
    {"sf10", "sf20", "sf40", "sf80", "sf160", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc ssb_mtc2_lp_r16_s::periodicity_opts::desc = {
    enum_names_ssb_mtc2_lp_r16_s_periodicity_e__values,
    8,
    "ssb_mtc2_lp_r16_s::periodicity_e_"};
uint8_t ssb_mtc2_lp_r16_s::periodicity_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 20, 40, 80, 160};
  return map_enum_number(numbers, 5, value, "ssb_mtc2_lp_r16_s::periodicity_e_");
}

// SSB-PositionQCL-Relation-r16 ::= ENUMERATED
static const char*           enum_names_ssb_position_qcl_relation_r16_e_values[] = {"n1", "n2", "n4", "n8"};
const asn1::table::enum_desc ssb_position_qcl_relation_r16_opts::desc            = {
    enum_names_ssb_position_qcl_relation_r16_e_values,
    4,
    "ssb_position_qcl_relation_r16_e"};
uint8_t ssb_position_qcl_relation_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "ssb_position_qcl_relation_r16_e");
}

// SSB-PositionQCL-Relation-r17 ::= ENUMERATED
static const char*           enum_names_ssb_position_qcl_relation_r17_e_values[] = {"n32", "n64"};
const asn1::table::enum_desc ssb_position_qcl_relation_r17_opts::desc            = {
    enum_names_ssb_position_qcl_relation_r17_e_values,
    2,
    "ssb_position_qcl_relation_r17_e"};
uint8_t ssb_position_qcl_relation_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {32, 64};
  return map_enum_number(numbers, 2, value, "ssb_position_qcl_relation_r17_e");
}

// SSB-MTC4-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ssb_mtc4_r17_s_fields[] = {
    {offsetof(ssb_mtc4_r17_s, pci_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(ssb_mtc4_r17_s::pci_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(1007), false, false>>::value},
    {offsetof(ssb_mtc4_r17_s, offset_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(159),
     nullptr}};
static const asn1::table::field_name seq_table_ssb_mtc4_r17_s_names[] = {{"pci-List-r17", nullptr},
                                                                         {"offset-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_ssb_mtc4_r17_s_opts[]  = {{0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          ssb_mtc4_r17_s::desc             = {
    {seq_table_ssb_mtc4_r17_s_fields, 2, UINT64_C(0x2), seq_table_ssb_mtc4_r17_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_ssb_mtc4_r17_s_names};

// SIB2 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__q_hyst_sf_s__fields[] = {
        {offsetof(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_, sf_medium),
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<
             decltype(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium)>(),
         decltype(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium)::has_ext,
         false,
         decltype(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium)::nof_types,
         decltype(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium)::nof_exts,
         nullptr,
         &sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::desc},
        {offsetof(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_, sf_high),
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<
             decltype(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high)>(),
         decltype(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high)::has_ext,
         false,
         decltype(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high)::nof_types,
         decltype(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high)::nof_exts,
         nullptr,
         &sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::desc}};
static const asn1::table::field_name
    seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__q_hyst_sf_s__names[] = {{"sf-Medium", nullptr},
                                                                                                 {"sf-High", nullptr}};
static const asn1::table::seq_desc seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__q_hyst_sf_s_ = {
    {seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__q_hyst_sf_s__fields,
     2,
     UINT64_C(0x3),
     nullptr,
     0},
    false,
    false,
    nullptr,
    0,
    seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__q_hyst_sf_s__names};
static const asn1::table::seq_field seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__fields[] = {
    {offsetof(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_, mob_state_params),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mob_state_params_s::desc},
    {offsetof(sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_, q_hyst_sf),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__q_hyst_sf_s_}};
static const asn1::table::field_name seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__names[] = {
    {"mobilityStateParameters", nullptr},
    {"q-HystSF", nullptr}};
static const asn1::table::seq_desc seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s_ = {
    {seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__names};
static const asn1::table::seq_field seq_table_sib2_s_cell_resel_info_common_s__fields[] = {
    {offsetof(sib2_s::cell_resel_info_common_s_, nrof_ss_blocks_to_average),
     (int32_t)offsetof(sib2_s::cell_resel_info_common_s_, nrof_ss_blocks_to_average_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(2),
     UINT64_C(16),
     nullptr},
    {offsetof(sib2_s::cell_resel_info_common_s_, abs_thresh_ss_blocks_consolidation),
     (int32_t)offsetof(sib2_s::cell_resel_info_common_s_, abs_thresh_ss_blocks_consolidation_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &thres_nr_s::desc},
    {offsetof(sib2_s::cell_resel_info_common_s_, range_to_best_cell),
     (int32_t)offsetof(sib2_s::cell_resel_info_common_s_, range_to_best_cell_present),
     asn1::table::enum_op<decltype(sib2_s::cell_resel_info_common_s_::range_to_best_cell)>(),
     decltype(sib2_s::cell_resel_info_common_s_::range_to_best_cell)::has_ext,
     false,
     decltype(sib2_s::cell_resel_info_common_s_::range_to_best_cell)::nof_types,
     decltype(sib2_s::cell_resel_info_common_s_::range_to_best_cell)::nof_exts,
     nullptr,
     &q_offset_range_opts::desc},
    {offsetof(sib2_s::cell_resel_info_common_s_, q_hyst),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sib2_s::cell_resel_info_common_s_::q_hyst)>(),
     decltype(sib2_s::cell_resel_info_common_s_::q_hyst)::has_ext,
     false,
     decltype(sib2_s::cell_resel_info_common_s_::q_hyst)::nof_types,
     decltype(sib2_s::cell_resel_info_common_s_::q_hyst)::nof_exts,
     nullptr,
     &sib2_s::cell_resel_info_common_s_::q_hyst_opts::desc},
    {offsetof(sib2_s::cell_resel_info_common_s_, speed_state_resel_pars),
     (int32_t)offsetof(sib2_s::cell_resel_info_common_s_, speed_state_resel_pars_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s_}};
static const asn1::table::field_name seq_table_sib2_s_cell_resel_info_common_s__names[] = {
    {"nrofSS-BlocksToAverage", nullptr},
    {"absThreshSS-BlocksConsolidation", nullptr},
    {"rangeToBestCell", nullptr},
    {"q-Hyst", nullptr},
    {"speedStateReselectionPars", nullptr}};
static const asn1::table::seq_opt seq_table_sib2_s_cell_resel_info_common_s__opts[] = {
    {0, (int32_t)offsetof(sib2_s::cell_resel_info_common_s_, nrof_ss_blocks_to_average_present)},
    {1, (int32_t)offsetof(sib2_s::cell_resel_info_common_s_, abs_thresh_ss_blocks_consolidation_present)},
    {2, (int32_t)offsetof(sib2_s::cell_resel_info_common_s_, range_to_best_cell_present)},
    {4, (int32_t)offsetof(sib2_s::cell_resel_info_common_s_, speed_state_resel_pars_present)}};
static const asn1::table::seq_desc seq_table_sib2_s_cell_resel_info_common_s_ = {
    {seq_table_sib2_s_cell_resel_info_common_s__fields,
     5,
     UINT64_C(0x8),
     seq_table_sib2_s_cell_resel_info_common_s__opts,
     4},
    true,
    false,
    nullptr,
    0,
    seq_table_sib2_s_cell_resel_info_common_s__names};
static const asn1::table::seq_field seq_table_sib2_s_cell_resel_serving_freq_info_s__fields[] = {
    {offsetof(sib2_s::cell_resel_serving_freq_info_s_, s_non_intra_search_p),
     (int32_t)offsetof(sib2_s::cell_resel_serving_freq_info_s_, s_non_intra_search_p_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(sib2_s::cell_resel_serving_freq_info_s_, s_non_intra_search_q),
     (int32_t)offsetof(sib2_s::cell_resel_serving_freq_info_s_, s_non_intra_search_q_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(sib2_s::cell_resel_serving_freq_info_s_, thresh_serving_low_p),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(sib2_s::cell_resel_serving_freq_info_s_, thresh_serving_low_q),
     (int32_t)offsetof(sib2_s::cell_resel_serving_freq_info_s_, thresh_serving_low_q_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(sib2_s::cell_resel_serving_freq_info_s_, cell_resel_prio),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(sib2_s::cell_resel_serving_freq_info_s_, cell_resel_sub_prio),
     (int32_t)offsetof(sib2_s::cell_resel_serving_freq_info_s_, cell_resel_sub_prio_present),
     asn1::table::enum_op<decltype(sib2_s::cell_resel_serving_freq_info_s_::cell_resel_sub_prio)>(),
     decltype(sib2_s::cell_resel_serving_freq_info_s_::cell_resel_sub_prio)::has_ext,
     false,
     decltype(sib2_s::cell_resel_serving_freq_info_s_::cell_resel_sub_prio)::nof_types,
     decltype(sib2_s::cell_resel_serving_freq_info_s_::cell_resel_sub_prio)::nof_exts,
     nullptr,
     &cell_resel_sub_prio_opts::desc}};
static const asn1::table::field_name seq_table_sib2_s_cell_resel_serving_freq_info_s__names[] = {
    {"s-NonIntraSearchP", nullptr},
    {"s-NonIntraSearchQ", nullptr},
    {"threshServingLowP", nullptr},
    {"threshServingLowQ", nullptr},
    {"cellReselectionPriority", nullptr},
    {"cellReselectionSubPriority", nullptr}};
static const asn1::table::seq_opt seq_table_sib2_s_cell_resel_serving_freq_info_s__opts[] = {
    {0, (int32_t)offsetof(sib2_s::cell_resel_serving_freq_info_s_, s_non_intra_search_p_present)},
    {1, (int32_t)offsetof(sib2_s::cell_resel_serving_freq_info_s_, s_non_intra_search_q_present)},
    {3, (int32_t)offsetof(sib2_s::cell_resel_serving_freq_info_s_, thresh_serving_low_q_present)},
    {5, (int32_t)offsetof(sib2_s::cell_resel_serving_freq_info_s_, cell_resel_sub_prio_present)}};
static const asn1::table::seq_desc seq_table_sib2_s_cell_resel_serving_freq_info_s_ = {
    {seq_table_sib2_s_cell_resel_serving_freq_info_s__fields,
     6,
     UINT64_C(0x14),
     seq_table_sib2_s_cell_resel_serving_freq_info_s__opts,
     4},
    true,
    false,
    nullptr,
    0,
    seq_table_sib2_s_cell_resel_serving_freq_info_s__names};
static const asn1::table::seq_field seq_table_sib2_s_relaxed_meas_r16_s__low_mob_eval_r16_s__fields[] = {
    {offsetof(sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_, s_search_delta_p_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16)>(),
     decltype(sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16)::has_ext,
     false,
     decltype(sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16)::nof_types,
     decltype(sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16)::nof_exts,
     nullptr,
     &sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_opts::desc},
    {offsetof(sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_, t_search_delta_p_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16)>(),
     decltype(sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16)::has_ext,
     false,
     decltype(sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16)::nof_types,
     decltype(sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16)::nof_exts,
     nullptr,
     &sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_opts::desc}};
static const asn1::table::field_name seq_table_sib2_s_relaxed_meas_r16_s__low_mob_eval_r16_s__names[] = {
    {"s-SearchDeltaP-r16", nullptr},
    {"t-SearchDeltaP-r16", nullptr}};
static const asn1::table::seq_desc seq_table_sib2_s_relaxed_meas_r16_s__low_mob_eval_r16_s_ = {
    {seq_table_sib2_s_relaxed_meas_r16_s__low_mob_eval_r16_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sib2_s_relaxed_meas_r16_s__low_mob_eval_r16_s__names};
static const asn1::table::seq_field seq_table_sib2_s_relaxed_meas_r16_s__cell_edge_eval_r16_s__fields[] = {
    {offsetof(sib2_s::relaxed_meas_r16_s_::cell_edge_eval_r16_s_, s_search_thres_p_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(sib2_s::relaxed_meas_r16_s_::cell_edge_eval_r16_s_, s_search_thres_q_r16),
     (int32_t)offsetof(sib2_s::relaxed_meas_r16_s_::cell_edge_eval_r16_s_, s_search_thres_q_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr}};
static const asn1::table::field_name seq_table_sib2_s_relaxed_meas_r16_s__cell_edge_eval_r16_s__names[] = {
    {"s-SearchThresholdP-r16", nullptr},
    {"s-SearchThresholdQ-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sib2_s_relaxed_meas_r16_s__cell_edge_eval_r16_s__opts[] = {
    {1, (int32_t)offsetof(sib2_s::relaxed_meas_r16_s_::cell_edge_eval_r16_s_, s_search_thres_q_r16_present)}};
static const asn1::table::seq_desc seq_table_sib2_s_relaxed_meas_r16_s__cell_edge_eval_r16_s_ = {
    {seq_table_sib2_s_relaxed_meas_r16_s__cell_edge_eval_r16_s__fields,
     2,
     UINT64_C(0x1),
     seq_table_sib2_s_relaxed_meas_r16_s__cell_edge_eval_r16_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_sib2_s_relaxed_meas_r16_s__cell_edge_eval_r16_s__names};
static const asn1::table::seq_field seq_table_sib2_s_relaxed_meas_r16_s__fields[] = {
    {offsetof(sib2_s::relaxed_meas_r16_s_, low_mob_eval_r16),
     (int32_t)offsetof(sib2_s::relaxed_meas_r16_s_, low_mob_eval_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib2_s_relaxed_meas_r16_s__low_mob_eval_r16_s_},
    {offsetof(sib2_s::relaxed_meas_r16_s_, cell_edge_eval_r16),
     (int32_t)offsetof(sib2_s::relaxed_meas_r16_s_, cell_edge_eval_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib2_s_relaxed_meas_r16_s__cell_edge_eval_r16_s_},
    {0,
     (int32_t)offsetof(sib2_s::relaxed_meas_r16_s_, combine_relaxed_meas_condition_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sib2_s::relaxed_meas_r16_s_, high_prio_meas_relax_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sib2_s_relaxed_meas_r16_s__names[] = {
    {"lowMobilityEvaluation-r16", nullptr},
    {"cellEdgeEvaluation-r16", nullptr},
    {"combineRelaxedMeasCondition-r16", "true"},
    {"highPriorityMeasRelax-r16", "true"}};
static const asn1::table::seq_opt seq_table_sib2_s_relaxed_meas_r16_s__opts[] = {
    {0, (int32_t)offsetof(sib2_s::relaxed_meas_r16_s_, low_mob_eval_r16_present)},
    {1, (int32_t)offsetof(sib2_s::relaxed_meas_r16_s_, cell_edge_eval_r16_present)},
    {2, (int32_t)offsetof(sib2_s::relaxed_meas_r16_s_, combine_relaxed_meas_condition_r16_present)},
    {3, (int32_t)offsetof(sib2_s::relaxed_meas_r16_s_, high_prio_meas_relax_r16_present)}};
static const asn1::table::seq_desc seq_table_sib2_s_relaxed_meas_r16_s_ = {
    {seq_table_sib2_s_relaxed_meas_r16_s__fields, 4, UINT64_C(0x0), seq_table_sib2_s_relaxed_meas_r16_s__opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_sib2_s_relaxed_meas_r16_s__names};
static const asn1::table::seq_field seq_table_sib2_s_relaxed_meas_r17_s__stationary_mob_eval_r17_s__fields[] = {
    {offsetof(sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_, s_search_delta_p_stationary_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<
         decltype(sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17)>(),
     decltype(sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17)::has_ext,
     false,
     decltype(sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17)::nof_types,
     decltype(sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17)::nof_exts,
     nullptr,
     &sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_opts::desc},
    {offsetof(sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_, t_search_delta_p_stationary_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<
         decltype(sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17)>(),
     decltype(sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17)::has_ext,
     false,
     decltype(sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17)::nof_types,
     decltype(sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17)::nof_exts,
     nullptr,
     &sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_opts::desc}};
static const asn1::table::field_name seq_table_sib2_s_relaxed_meas_r17_s__stationary_mob_eval_r17_s__names[] = {
    {"s-SearchDeltaP-Stationary-r17", nullptr},
    {"t-SearchDeltaP-Stationary-r17", nullptr}};
static const asn1::table::seq_desc seq_table_sib2_s_relaxed_meas_r17_s__stationary_mob_eval_r17_s_ = {
    {seq_table_sib2_s_relaxed_meas_r17_s__stationary_mob_eval_r17_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sib2_s_relaxed_meas_r17_s__stationary_mob_eval_r17_s__names};
static const asn1::table::seq_field
    seq_table_sib2_s_relaxed_meas_r17_s__cell_edge_eval_while_stationary_r17_s__fields[] = {
        {offsetof(sib2_s::relaxed_meas_r17_s_::cell_edge_eval_while_stationary_r17_s_, s_search_thres_p2_r17),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(0),
         UINT64_C(31),
         nullptr},
        {offsetof(sib2_s::relaxed_meas_r17_s_::cell_edge_eval_while_stationary_r17_s_, s_search_thres_q2_r17),
         (int32_t)offsetof(sib2_s::relaxed_meas_r17_s_::cell_edge_eval_while_stationary_r17_s_,
                           s_search_thres_q2_r17_present),
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(0),
         UINT64_C(31),
         nullptr}};
static const asn1::table::field_name
    seq_table_sib2_s_relaxed_meas_r17_s__cell_edge_eval_while_stationary_r17_s__names[] = {
        {"s-SearchThresholdP2-r17", nullptr},
        {"s-SearchThresholdQ2-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sib2_s_relaxed_meas_r17_s__cell_edge_eval_while_stationary_r17_s__opts[] = {
    {1,
     (int32_t)offsetof(sib2_s::relaxed_meas_r17_s_::cell_edge_eval_while_stationary_r17_s_,
                       s_search_thres_q2_r17_present)}};
static const asn1::table::seq_desc seq_table_sib2_s_relaxed_meas_r17_s__cell_edge_eval_while_stationary_r17_s_ = {
    {seq_table_sib2_s_relaxed_meas_r17_s__cell_edge_eval_while_stationary_r17_s__fields,
     2,
     UINT64_C(0x1),
     seq_table_sib2_s_relaxed_meas_r17_s__cell_edge_eval_while_stationary_r17_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_sib2_s_relaxed_meas_r17_s__cell_edge_eval_while_stationary_r17_s__names};
static const asn1::table::seq_field seq_table_sib2_s_relaxed_meas_r17_s__fields[] = {
    {offsetof(sib2_s::relaxed_meas_r17_s_, stationary_mob_eval_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib2_s_relaxed_meas_r17_s__stationary_mob_eval_r17_s_},
    {offsetof(sib2_s::relaxed_meas_r17_s_, cell_edge_eval_while_stationary_r17),
     (int32_t)offsetof(sib2_s::relaxed_meas_r17_s_, cell_edge_eval_while_stationary_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib2_s_relaxed_meas_r17_s__cell_edge_eval_while_stationary_r17_s_},
    {0,
     (int32_t)offsetof(sib2_s::relaxed_meas_r17_s_, combine_relaxed_meas_condition2_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sib2_s_relaxed_meas_r17_s__names[] = {
    {"stationaryMobilityEvaluation-r17", nullptr},
    {"cellEdgeEvaluationWhileStationary-r17", nullptr},
    {"combineRelaxedMeasCondition2-r17", "true"}};
static const asn1::table::seq_opt seq_table_sib2_s_relaxed_meas_r17_s__opts[] = {
    {1, (int32_t)offsetof(sib2_s::relaxed_meas_r17_s_, cell_edge_eval_while_stationary_r17_present)},
    {2, (int32_t)offsetof(sib2_s::relaxed_meas_r17_s_, combine_relaxed_meas_condition2_r17_present)}};
static const asn1::table::seq_desc seq_table_sib2_s_relaxed_meas_r17_s_ = {
    {seq_table_sib2_s_relaxed_meas_r17_s__fields, 3, UINT64_C(0x1), seq_table_sib2_s_relaxed_meas_r17_s__opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_sib2_s_relaxed_meas_r17_s__names};
static const asn1::table::seq_field  seq_table_sib2_s_fields[]    = {{offsetof(sib2_s, cell_resel_info_common),
                                                                      asn1::table::seq_field::mandatory,
                                                                      asn1::table::field_op::sequence,
                                                                      false,
                                                                      false,
                                                                      0,
                                                                      0,
                                                                      nullptr,
                                                                      &seq_table_sib2_s_cell_resel_info_common_s_},
                                                                     {offsetof(sib2_s, cell_resel_serving_freq_info),
                                                                      asn1::table::seq_field::mandatory,
                                                                      asn1::table::field_op::sequence,
                                                                      false,
                                                                      false,
                                                                      0,
                                                                      0,
                                                                      nullptr,
                                                                      &seq_table_sib2_s_cell_resel_serving_freq_info_s_},
                                                                     {offsetof(sib2_s, intra_freq_cell_resel_info),
                                                                      asn1::table::seq_field::mandatory,
                                                                      asn1::table::field_op::sequence,
                                                                      false,
                                                                      false,
                                                                      0,
                                                                      0,
                                                                      nullptr,
                                                                      &sib2_s::intra_freq_cell_resel_info_s_::desc}};
static const asn1::table::field_name seq_table_sib2_s_names[]     = {{"cellReselectionInfoCommon", nullptr},
                                                                     {"cellReselectionServingFreqInfo", nullptr},
                                                                     {"intraFreqCellReselectionInfo", nullptr}};
static const asn1::table::seq_field  seq_table_sib2_s_g0_fields[] = {
    {offsetof(sib2_s, relaxed_meas_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(sib2_s::relaxed_meas_r16)>::value,
      &seq_table_sib2_s_relaxed_meas_r16_s_}};
static const asn1::table::field_name seq_table_sib2_s_g0_names[]  = {{"relaxedMeasurement-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sib2_s_g0_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sib2_s_g1_fields[] = {
    {offsetof(sib2_s, cell_equivalent_size_r17),
      (int32_t)offsetof(sib2_s, cell_equivalent_size_r17_present),
      asn1::table::field_op::int_u8,
      false,
      false,
      INT64_C(2),
      UINT64_C(16),
      nullptr},
    {offsetof(sib2_s, relaxed_meas_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(sib2_s::relaxed_meas_r17)>::value,
      &seq_table_sib2_s_relaxed_meas_r17_s_}};
static const asn1::table::field_name seq_table_sib2_s_g1_names[] = {{"cellEquivalentSize-r17", nullptr},
                                                                    {"relaxedMeasurement-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_sib2_s_g1_opts[]  = {
    {0, (int32_t)offsetof(sib2_s, cell_equivalent_size_r17_present)},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sib2_s_groups[] = {
    {{seq_table_sib2_s_g0_fields, 1, UINT64_C(0x0), seq_table_sib2_s_g0_opts, 1}, false, seq_table_sib2_s_g0_names},
    {{seq_table_sib2_s_g1_fields, 2, UINT64_C(0x0), seq_table_sib2_s_g1_opts, 2}, false, seq_table_sib2_s_g1_names}};
const asn1::table::seq_desc sib2_s::desc = {{seq_table_sib2_s_fields, 3, UINT64_C(0x7), nullptr, 0},
                                            true,
                                            false,
                                            seq_table_sib2_s_groups,
                                            2,
                                            seq_table_sib2_s_names};

static const char*           enum_names_sib2_s_cell_resel_info_common_s__q_hyst_e__values[] = {"dB0",
                                                                                               "dB1",
                                                                                               "dB2",
                                                                                               "dB3",
                                                                                               "dB4",
                                                                                               "dB5",
                                                                                               "dB6",
                                                                                               "dB8",
                                                                                               "dB10",
                                                                                               "dB12",
                                                                                               "dB14",
                                                                                               "dB16",
                                                                                               "dB18",
                                                                                               "dB20",
                                                                                               "dB22",
                                                                                               "dB24"};
const asn1::table::enum_desc sib2_s::cell_resel_info_common_s_::q_hyst_opts::desc           = {
    enum_names_sib2_s_cell_resel_info_common_s__q_hyst_e__values,
    16,
    "sib2_s::cell_resel_info_common_s_::q_hyst_e_"};
uint8_t sib2_s::cell_resel_info_common_s_::q_hyst_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(numbers, 16, value, "sib2_s::cell_resel_info_common_s_::q_hyst_e_");
}

static const char*
    enum_names_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__q_hyst_sf_s__sf_medium_e__values[] = {"dB-6",
                                                                                                                "dB-4",
                                                                                                                "dB-2",
                                                                                                                "dB0"};
const asn1::table::enum_desc
    sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::desc = {
        enum_names_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__q_hyst_sf_s__sf_medium_e__values,
        4,
        "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_"};
int8_t sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_opts::to_number() const
{
  static const int8_t numbers[] = {-6, -4, -2, 0};
  return map_enum_number(
      numbers, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_medium_e_");
}

static const char*
    enum_names_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__q_hyst_sf_s__sf_high_e__values[] = {"dB-6",
                                                                                                              "dB-4",
                                                                                                              "dB-2",
                                                                                                              "dB0"};
const asn1::table::enum_desc
    sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::desc = {
        enum_names_sib2_s_cell_resel_info_common_s__speed_state_resel_pars_s__q_hyst_sf_s__sf_high_e__values,
        4,
        "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_"};
int8_t sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_opts::to_number() const
{
  static const int8_t numbers[] = {-6, -4, -2, 0};
  return map_enum_number(
      numbers, 4, value, "sib2_s::cell_resel_info_common_s_::speed_state_resel_pars_s_::q_hyst_sf_s_::sf_high_e_");
}

static const asn1::table::seq_field seq_table_sib2_s_intra_freq_cell_resel_info_s__fields[] = {
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, q_rx_lev_min),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-70),
     UINT64_C(-22),
     nullptr},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, q_rx_lev_min_sul),
     (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, q_rx_lev_min_sul_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-70),
     UINT64_C(-22),
     nullptr},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, q_qual_min),
     (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, q_qual_min_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-43),
     UINT64_C(-12),
     nullptr},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, s_intra_search_p),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, s_intra_search_q),
     (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, s_intra_search_q_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, t_resel_nr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, freq_band_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sib2_s::intra_freq_cell_resel_info_s_::freq_band_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_s::desc},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, freq_band_list_sul),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sib2_s::intra_freq_cell_resel_info_s_::freq_band_list_sul),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_s::desc},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, p_max),
     (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, p_max_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, smtc),
     (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, smtc_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_mtc_s::desc},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, ss_rssi_meas),
     (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, ss_rssi_meas_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ss_rssi_meas_s::desc},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, ssb_to_measure),
     (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, ssb_to_measure_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_to_measure_c::desc},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, derive_ssb_idx_from_cell),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sib2_s_intra_freq_cell_resel_info_s__names[] = {
    {"q-RxLevMin", nullptr},
    {"q-RxLevMinSUL", nullptr},
    {"q-QualMin", nullptr},
    {"s-IntraSearchP", nullptr},
    {"s-IntraSearchQ", nullptr},
    {"t-ReselectionNR", nullptr},
    {"frequencyBandList", nullptr},
    {"frequencyBandListSUL", nullptr},
    {"p-Max", nullptr},
    {"smtc", nullptr},
    {"ss-RSSI-Measurement", nullptr},
    {"ssb-ToMeasure", nullptr},
    {"deriveSSB-IndexFromCell", nullptr}};
static const asn1::table::seq_opt seq_table_sib2_s_intra_freq_cell_resel_info_s__opts[] = {
    {1, (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, q_rx_lev_min_sul_present)},
    {2, (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, q_qual_min_present)},
    {4, (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, s_intra_search_q_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, asn1::table::seq_field::list_presence},
    {8, (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, p_max_present)},
    {9, (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, smtc_present)},
    {10, (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, ss_rssi_meas_present)},
    {11, (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, ssb_to_measure_present)}};
static const asn1::table::seq_field seq_table_sib2_s_intra_freq_cell_resel_info_s__g0_fields[] = {
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, t_resel_nr_sf),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sib2_s::intra_freq_cell_resel_info_s_::t_resel_nr_sf)>::value,
     &speed_state_scale_factors_s::desc}};
static const asn1::table::field_name seq_table_sib2_s_intra_freq_cell_resel_info_s__g0_names[] = {
    {"t-ReselectionNR-SF", nullptr}};
static const asn1::table::seq_opt seq_table_sib2_s_intra_freq_cell_resel_info_s__g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sib2_s_intra_freq_cell_resel_info_s__g1_fields[] = {
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, smtc2_lp_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sib2_s::intra_freq_cell_resel_info_s_::smtc2_lp_r16)>::value,
     &ssb_mtc2_lp_r16_s::desc},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, ssb_position_qcl_common_r16),
     (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, ssb_position_qcl_common_r16_present),
     asn1::table::enum_op<decltype(sib2_s::intra_freq_cell_resel_info_s_::ssb_position_qcl_common_r16)>(),
     decltype(sib2_s::intra_freq_cell_resel_info_s_::ssb_position_qcl_common_r16)::has_ext,
     false,
     decltype(sib2_s::intra_freq_cell_resel_info_s_::ssb_position_qcl_common_r16)::nof_types,
     decltype(sib2_s::intra_freq_cell_resel_info_s_::ssb_position_qcl_common_r16)::nof_exts,
     nullptr,
     &ssb_position_qcl_relation_r16_opts::desc}};
static const asn1::table::field_name seq_table_sib2_s_intra_freq_cell_resel_info_s__g1_names[] = {
    {"smtc2-LP-r16", nullptr},
    {"ssb-PositionQCL-Common-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sib2_s_intra_freq_cell_resel_info_s__g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, ssb_position_qcl_common_r16_present)}};
static const asn1::table::seq_field seq_table_sib2_s_intra_freq_cell_resel_info_s__g2_fields[] = {
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, ssb_position_qcl_common_r17),
     (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, ssb_position_qcl_common_r17_present),
     asn1::table::enum_op<decltype(sib2_s::intra_freq_cell_resel_info_s_::ssb_position_qcl_common_r17)>(),
     decltype(sib2_s::intra_freq_cell_resel_info_s_::ssb_position_qcl_common_r17)::has_ext,
     false,
     decltype(sib2_s::intra_freq_cell_resel_info_s_::ssb_position_qcl_common_r17)::nof_types,
     decltype(sib2_s::intra_freq_cell_resel_info_s_::ssb_position_qcl_common_r17)::nof_exts,
     nullptr,
     &ssb_position_qcl_relation_r17_opts::desc}};
static const asn1::table::field_name seq_table_sib2_s_intra_freq_cell_resel_info_s__g2_names[] = {
    {"ssb-PositionQCL-Common-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sib2_s_intra_freq_cell_resel_info_s__g2_opts[] = {
    {0, (int32_t)offsetof(sib2_s::intra_freq_cell_resel_info_s_, ssb_position_qcl_common_r17_present)}};
static const asn1::table::seq_field seq_table_sib2_s_intra_freq_cell_resel_info_s__g3_fields[] = {
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, smtc4list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::ptr_item_list_codec<decltype(sib2_s::intra_freq_cell_resel_info_s_::smtc4list_r17),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &ssb_mtc4_r17_s::desc}};
static const asn1::table::field_name seq_table_sib2_s_intra_freq_cell_resel_info_s__g3_names[] = {
    {"smtc4list-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sib2_s_intra_freq_cell_resel_info_s__g3_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sib2_s_intra_freq_cell_resel_info_s__g4_fields[] = {
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, freq_band_list_v1760),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_item_list_codec<decltype(sib2_s::intra_freq_cell_resel_info_s_::freq_band_list_v1760),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &nr_multi_band_info_v1760_s::desc},
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, freq_band_list_sul_v1760),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_item_list_codec<decltype(sib2_s::intra_freq_cell_resel_info_s_::freq_band_list_sul_v1760),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &nr_multi_band_info_v1760_s::desc}};
static const asn1::table::field_name seq_table_sib2_s_intra_freq_cell_resel_info_s__g4_names[] = {
    {"frequencyBandList-v1760", nullptr},
    {"frequencyBandListSUL-v1760", nullptr}};
static const asn1::table::seq_opt seq_table_sib2_s_intra_freq_cell_resel_info_s__g4_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sib2_s_intra_freq_cell_resel_info_s__g5_fields[] = {
    {offsetof(sib2_s::intra_freq_cell_resel_info_s_, freq_band_list_aerial_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_item_list_codec<decltype(sib2_s::intra_freq_cell_resel_info_s_::freq_band_list_aerial_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &nr_multi_band_info_aerial_r18_s::desc}};
static const asn1::table::field_name seq_table_sib2_s_intra_freq_cell_resel_info_s__g5_names[] = {
    {"frequencyBandListAerial-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sib2_s_intra_freq_cell_resel_info_s__g5_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sib2_s_intra_freq_cell_resel_info_s__groups[] = {
    {{seq_table_sib2_s_intra_freq_cell_resel_info_s__g0_fields,
      1,
      UINT64_C(0x0),
      seq_table_sib2_s_intra_freq_cell_resel_info_s__g0_opts,
      1},
     false,
     seq_table_sib2_s_intra_freq_cell_resel_info_s__g0_names},
    {{seq_table_sib2_s_intra_freq_cell_resel_info_s__g1_fields,
      2,
      UINT64_C(0x0),
      seq_table_sib2_s_intra_freq_cell_resel_info_s__g1_opts,
      2},
     false,
     seq_table_sib2_s_intra_freq_cell_resel_info_s__g1_names},
    {{seq_table_sib2_s_intra_freq_cell_resel_info_s__g2_fields,
      1,
      UINT64_C(0x0),
      seq_table_sib2_s_intra_freq_cell_resel_info_s__g2_opts,
      1},
     false,
     seq_table_sib2_s_intra_freq_cell_resel_info_s__g2_names},
    {{seq_table_sib2_s_intra_freq_cell_resel_info_s__g3_fields,
      1,
      UINT64_C(0x0),
      seq_table_sib2_s_intra_freq_cell_resel_info_s__g3_opts,
      1},
     false,
     seq_table_sib2_s_intra_freq_cell_resel_info_s__g3_names},
    {{seq_table_sib2_s_intra_freq_cell_resel_info_s__g4_fields,
      2,
      UINT64_C(0x0),
      seq_table_sib2_s_intra_freq_cell_resel_info_s__g4_opts,
      2},
     false,
     seq_table_sib2_s_intra_freq_cell_resel_info_s__g4_names},
    {{seq_table_sib2_s_intra_freq_cell_resel_info_s__g5_fields,
      1,
      UINT64_C(0x0),
      seq_table_sib2_s_intra_freq_cell_resel_info_s__g5_opts,
      1},
     false,
     seq_table_sib2_s_intra_freq_cell_resel_info_s__g5_names}};
const asn1::table::seq_desc sib2_s::intra_freq_cell_resel_info_s_::desc = {
    {seq_table_sib2_s_intra_freq_cell_resel_info_s__fields,
     13,
     UINT64_C(0x1029),
     seq_table_sib2_s_intra_freq_cell_resel_info_s__opts,
     9},
    true,
    false,
    seq_table_sib2_s_intra_freq_cell_resel_info_s__groups,
    6,
    seq_table_sib2_s_intra_freq_cell_resel_info_s__names};

static const char* enum_names_sib2_s_relaxed_meas_r16_s__low_mob_eval_r16_s__s_search_delta_p_r16_e__values[] =
    {"dB3", "dB6", "dB9", "dB12", "dB15", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_opts::desc = {
    enum_names_sib2_s_relaxed_meas_r16_s__low_mob_eval_r16_s__s_search_delta_p_r16_e__values,
    8,
    "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_e_"};
uint8_t sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 6, 9, 12, 15};
  return map_enum_number(
      numbers, 5, value, "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::s_search_delta_p_r16_e_");
}

static const char* enum_names_sib2_s_relaxed_meas_r16_s__low_mob_eval_r16_s__t_search_delta_p_r16_e__values[] = {
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
const asn1::table::enum_desc sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_opts::desc = {
    enum_names_sib2_s_relaxed_meas_r16_s__low_mob_eval_r16_s__t_search_delta_p_r16_e__values,
    16,
    "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_e_"};
uint16_t sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 30, 60, 120, 180, 240, 300};
  return map_enum_number(
      numbers, 9, value, "sib2_s::relaxed_meas_r16_s_::low_mob_eval_r16_s_::t_search_delta_p_r16_e_");
}

static const char*
    enum_names_sib2_s_relaxed_meas_r17_s__stationary_mob_eval_r17_s__s_search_delta_p_stationary_r17_e__values[] =
        {"dB2", "dB3", "dB6", "dB9", "dB12", "dB15", "spare2", "spare1"};
const asn1::table::enum_desc
    sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_opts::desc = {
        enum_names_sib2_s_relaxed_meas_r17_s__stationary_mob_eval_r17_s__s_search_delta_p_stationary_r17_e__values,
        8,
        "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_e_"};
uint8_t sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 6, 9, 12, 15};
  return map_enum_number(
      numbers, 6, value, "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::s_search_delta_p_stationary_r17_e_");
}

static const char*
    enum_names_sib2_s_relaxed_meas_r17_s__stationary_mob_eval_r17_s__t_search_delta_p_stationary_r17_e__values[] = {
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
const asn1::table::enum_desc
    sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_opts::desc = {
        enum_names_sib2_s_relaxed_meas_r17_s__stationary_mob_eval_r17_s__t_search_delta_p_stationary_r17_e__values,
        16,
        "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_e_"};
uint16_t
sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 30, 60, 120, 180, 240, 300};
  return map_enum_number(
      numbers, 9, value, "sib2_s::relaxed_meas_r17_s_::stationary_mob_eval_r17_s_::t_search_delta_p_stationary_r17_e_");
}

// IntraFreqNeighCellInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_intra_freq_neigh_cell_info_s_fields[] = {
    {offsetof(intra_freq_neigh_cell_info_s, pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(intra_freq_neigh_cell_info_s, q_offset_cell),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(intra_freq_neigh_cell_info_s::q_offset_cell)>(),
     decltype(intra_freq_neigh_cell_info_s::q_offset_cell)::has_ext,
     false,
     decltype(intra_freq_neigh_cell_info_s::q_offset_cell)::nof_types,
     decltype(intra_freq_neigh_cell_info_s::q_offset_cell)::nof_exts,
     nullptr,
     &q_offset_range_opts::desc},
    {offsetof(intra_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell),
     (int32_t)offsetof(intra_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(intra_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_sul),
     (int32_t)offsetof(intra_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_sul_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(intra_freq_neigh_cell_info_s, q_qual_min_offset_cell),
     (int32_t)offsetof(intra_freq_neigh_cell_info_s, q_qual_min_offset_cell_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr}};
static const asn1::table::field_name seq_table_intra_freq_neigh_cell_info_s_names[] = {
    {"physCellId", nullptr},
    {"q-OffsetCell", nullptr},
    {"q-RxLevMinOffsetCell", nullptr},
    {"q-RxLevMinOffsetCellSUL", nullptr},
    {"q-QualMinOffsetCell", nullptr}};
static const asn1::table::seq_opt seq_table_intra_freq_neigh_cell_info_s_opts[] = {
    {2, (int32_t)offsetof(intra_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_present)},
    {3, (int32_t)offsetof(intra_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_sul_present)},
    {4, (int32_t)offsetof(intra_freq_neigh_cell_info_s, q_qual_min_offset_cell_present)}};
const asn1::table::seq_desc intra_freq_neigh_cell_info_s::desc = {
    {seq_table_intra_freq_neigh_cell_info_s_fields, 5, UINT64_C(0x3), seq_table_intra_freq_neigh_cell_info_s_opts, 3},
    true,
    false,
    nullptr,
    0,
    seq_table_intra_freq_neigh_cell_info_s_names};

// PCI-Range ::= SEQUENCE
static const asn1::table::seq_field seq_table_pci_range_s_fields[] = {
    {offsetof(pci_range_s, start),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(pci_range_s, range),
     (int32_t)offsetof(pci_range_s, range_present),
     asn1::table::enum_op<decltype(pci_range_s::range)>(),
     decltype(pci_range_s::range)::has_ext,
     false,
     decltype(pci_range_s::range)::nof_types,
     decltype(pci_range_s::range)::nof_exts,
     nullptr,
     &pci_range_s::range_opts::desc}};
static const asn1::table::field_name seq_table_pci_range_s_names[] = {{"start", nullptr}, {"range", nullptr}};
static const asn1::table::seq_opt seq_table_pci_range_s_opts[] = {{1, (int32_t)offsetof(pci_range_s, range_present)}};
const asn1::table::seq_desc       pci_range_s::desc            = {
    {seq_table_pci_range_s_fields, 2, UINT64_C(0x1), seq_table_pci_range_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_pci_range_s_names};

static const char*           enum_names_pci_range_s_range_e__values[] = {"n4",
                                                                         "n8",
                                                                         "n12",
                                                                         "n16",
                                                                         "n24",
                                                                         "n32",
                                                                         "n48",
                                                                         "n64",
                                                                         "n84",
                                                                         "n96",
                                                                         "n128",
                                                                         "n168",
                                                                         "n252",
                                                                         "n504",
                                                                         "n1008",
                                                                         "spare1"};
const asn1::table::enum_desc pci_range_s::range_opts::desc            = {enum_names_pci_range_s_range_e__values,
                                                                         16,
                                                                         "pci_range_s::range_e_"};
uint16_t                     pci_range_s::range_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504, 1008};
  return map_enum_number(numbers, 15, value, "pci_range_s::range_e_");
}

// IntraFreqNeighCellInfo-v1610 ::= SEQUENCE
static const asn1::table::seq_field seq_table_intra_freq_neigh_cell_info_v1610_s_fields[] = {
    {offsetof(intra_freq_neigh_cell_info_v1610_s, ssb_position_qcl_r16),
     (int32_t)offsetof(intra_freq_neigh_cell_info_v1610_s, ssb_position_qcl_r16_present),
     asn1::table::enum_op<decltype(intra_freq_neigh_cell_info_v1610_s::ssb_position_qcl_r16)>(),
     decltype(intra_freq_neigh_cell_info_v1610_s::ssb_position_qcl_r16)::has_ext,
     false,
     decltype(intra_freq_neigh_cell_info_v1610_s::ssb_position_qcl_r16)::nof_types,
     decltype(intra_freq_neigh_cell_info_v1610_s::ssb_position_qcl_r16)::nof_exts,
     nullptr,
     &ssb_position_qcl_relation_r16_opts::desc}};
static const asn1::table::field_name seq_table_intra_freq_neigh_cell_info_v1610_s_names[] = {
    {"ssb-PositionQCL-r16", nullptr}};
static const asn1::table::seq_opt seq_table_intra_freq_neigh_cell_info_v1610_s_opts[] = {
    {0, (int32_t)offsetof(intra_freq_neigh_cell_info_v1610_s, ssb_position_qcl_r16_present)}};
const asn1::table::seq_desc intra_freq_neigh_cell_info_v1610_s::desc = {
    {seq_table_intra_freq_neigh_cell_info_v1610_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_intra_freq_neigh_cell_info_v1610_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_intra_freq_neigh_cell_info_v1610_s_names};

// IntraFreqCAG-CellListPerPLMN-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_intra_freq_cag_cell_list_per_plmn_r16_s_fields[] = {
    {offsetof(intra_freq_cag_cell_list_per_plmn_r16_s, plmn_id_idx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(12),
     nullptr},
    {offsetof(intra_freq_cag_cell_list_per_plmn_r16_s, cag_cell_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(intra_freq_cag_cell_list_per_plmn_r16_s::cag_cell_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pci_range_s::desc}};
static const asn1::table::field_name seq_table_intra_freq_cag_cell_list_per_plmn_r16_s_names[] = {
    {"plmn-IdentityIndex-r16", nullptr},
    {"cag-CellList-r16", nullptr}};
const asn1::table::seq_desc intra_freq_cag_cell_list_per_plmn_r16_s::desc = {
    {seq_table_intra_freq_cag_cell_list_per_plmn_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_intra_freq_cag_cell_list_per_plmn_r16_s_names};

// IntraFreqNeighCellInfo-v1710 ::= SEQUENCE
static const asn1::table::seq_field seq_table_intra_freq_neigh_cell_info_v1710_s_fields[] = {
    {offsetof(intra_freq_neigh_cell_info_v1710_s, ssb_position_qcl_r17),
     (int32_t)offsetof(intra_freq_neigh_cell_info_v1710_s, ssb_position_qcl_r17_present),
     asn1::table::enum_op<decltype(intra_freq_neigh_cell_info_v1710_s::ssb_position_qcl_r17)>(),
     decltype(intra_freq_neigh_cell_info_v1710_s::ssb_position_qcl_r17)::has_ext,
     false,
     decltype(intra_freq_neigh_cell_info_v1710_s::ssb_position_qcl_r17)::nof_types,
     decltype(intra_freq_neigh_cell_info_v1710_s::ssb_position_qcl_r17)::nof_exts,
     nullptr,
     &ssb_position_qcl_relation_r17_opts::desc}};
static const asn1::table::field_name seq_table_intra_freq_neigh_cell_info_v1710_s_names[] = {
    {"ssb-PositionQCL-r17", nullptr}};
static const asn1::table::seq_opt seq_table_intra_freq_neigh_cell_info_v1710_s_opts[] = {
    {0, (int32_t)offsetof(intra_freq_neigh_cell_info_v1710_s, ssb_position_qcl_r17_present)}};
const asn1::table::seq_desc intra_freq_neigh_cell_info_v1710_s::desc = {
    {seq_table_intra_freq_neigh_cell_info_v1710_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_intra_freq_neigh_cell_info_v1710_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_intra_freq_neigh_cell_info_v1710_s_names};

// SIB3 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib3_s_fields[] = {
    {offsetof(sib3_s, intra_freq_neigh_cell_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(sib3_s::intra_freq_neigh_cell_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &intra_freq_neigh_cell_info_s::desc},
    {offsetof(sib3_s, intra_freq_excluded_cell_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(sib3_s::intra_freq_excluded_cell_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pci_range_s::desc},
    {offsetof(sib3_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib3_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib3_s_names[]     = {{"intraFreqNeighCellList", nullptr},
                                                                     {"intraFreqExcludedCellList", nullptr},
                                                                     {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib3_s_opts[]      = {{0, asn1::table::seq_field::list_presence},
                                                                     {1, asn1::table::seq_field::list_presence},
                                                                     {2, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field  seq_table_sib3_s_g0_fields[] = {
    {offsetof(sib3_s, intra_freq_neigh_cell_list_v1610),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      16,
      &asn1::table::ptr_item_list_codec<decltype(sib3_s::intra_freq_neigh_cell_list_v1610),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &intra_freq_neigh_cell_info_v1610_s::desc},
    {offsetof(sib3_s, intra_freq_allowed_cell_list_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      16,
      &asn1::table::ptr_item_list_codec<decltype(sib3_s::intra_freq_allowed_cell_list_r16),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &pci_range_s::desc},
    {offsetof(sib3_s, intra_freq_cag_cell_list_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      12,
      &asn1::table::ptr_item_list_codec<decltype(sib3_s::intra_freq_cag_cell_list_r16),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &intra_freq_cag_cell_list_per_plmn_r16_s::desc}};
static const asn1::table::field_name seq_table_sib3_s_g0_names[]  = {{"intraFreqNeighCellList-v1610", nullptr},
                                                                     {"intraFreqAllowedCellList-r16", nullptr},
                                                                     {"intraFreqCAG-CellList-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sib3_s_g0_opts[]   = {{0, asn1::table::seq_field::ptr_presence},
                                                                     {1, asn1::table::seq_field::ptr_presence},
                                                                     {2, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sib3_s_g1_fields[] = {
    {offsetof(sib3_s, intra_freq_neigh_hsdn_cell_list_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      16,
      &asn1::table::ptr_item_list_codec<decltype(sib3_s::intra_freq_neigh_hsdn_cell_list_r17),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &pci_range_s::desc},
    {offsetof(sib3_s, intra_freq_neigh_cell_list_v1710),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      16,
      &asn1::table::ptr_item_list_codec<decltype(sib3_s::intra_freq_neigh_cell_list_v1710),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &intra_freq_neigh_cell_info_v1710_s::desc}};
static const asn1::table::field_name seq_table_sib3_s_g1_names[]  = {{"intraFreqNeighHSDN-CellList-r17", nullptr},
                                                                     {"intraFreqNeighCellList-v1710", nullptr}};
static const asn1::table::seq_opt    seq_table_sib3_s_g1_opts[]   = {{0, asn1::table::seq_field::ptr_presence},
                                                                     {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sib3_s_g2_fields[] = {
    {0,
      (int32_t)offsetof(sib3_s, ch_access_mode2_r17_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr}};
static const asn1::table::field_name seq_table_sib3_s_g2_names[] = {{"channelAccessMode2-r17", "enabled"}};
static const asn1::table::seq_opt    seq_table_sib3_s_g2_opts[]  = {
    {0, (int32_t)offsetof(sib3_s, ch_access_mode2_r17_present)}};
static const asn1::table::seq_group seq_table_sib3_s_groups[] = {
    {{seq_table_sib3_s_g0_fields, 3, UINT64_C(0x0), seq_table_sib3_s_g0_opts, 3}, false, seq_table_sib3_s_g0_names},
    {{seq_table_sib3_s_g1_fields, 2, UINT64_C(0x0), seq_table_sib3_s_g1_opts, 2}, false, seq_table_sib3_s_g1_names},
    {{seq_table_sib3_s_g2_fields, 1, UINT64_C(0x0), seq_table_sib3_s_g2_opts, 1}, false, seq_table_sib3_s_g2_names}};
const asn1::table::seq_desc sib3_s::desc = {{seq_table_sib3_s_fields, 3, UINT64_C(0x0), seq_table_sib3_s_opts, 3},
                                            true,
                                            false,
                                            seq_table_sib3_s_groups,
                                            3,
                                            seq_table_sib3_s_names};

// InterFreqNeighCellInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_inter_freq_neigh_cell_info_s_fields[] = {
    {offsetof(inter_freq_neigh_cell_info_s, pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(inter_freq_neigh_cell_info_s, q_offset_cell),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(inter_freq_neigh_cell_info_s::q_offset_cell)>(),
     decltype(inter_freq_neigh_cell_info_s::q_offset_cell)::has_ext,
     false,
     decltype(inter_freq_neigh_cell_info_s::q_offset_cell)::nof_types,
     decltype(inter_freq_neigh_cell_info_s::q_offset_cell)::nof_exts,
     nullptr,
     &q_offset_range_opts::desc},
    {offsetof(inter_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell),
     (int32_t)offsetof(inter_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(inter_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_sul),
     (int32_t)offsetof(inter_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_sul_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(inter_freq_neigh_cell_info_s, q_qual_min_offset_cell),
     (int32_t)offsetof(inter_freq_neigh_cell_info_s, q_qual_min_offset_cell_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr}};
static const asn1::table::field_name seq_table_inter_freq_neigh_cell_info_s_names[] = {
    {"physCellId", nullptr},
    {"q-OffsetCell", nullptr},
    {"q-RxLevMinOffsetCell", nullptr},
    {"q-RxLevMinOffsetCellSUL", nullptr},
    {"q-QualMinOffsetCell", nullptr}};
static const asn1::table::seq_opt seq_table_inter_freq_neigh_cell_info_s_opts[] = {
    {2, (int32_t)offsetof(inter_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_present)},
    {3, (int32_t)offsetof(inter_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_sul_present)},
    {4, (int32_t)offsetof(inter_freq_neigh_cell_info_s, q_qual_min_offset_cell_present)}};
const asn1::table::seq_desc inter_freq_neigh_cell_info_s::desc = {
    {seq_table_inter_freq_neigh_cell_info_s_fields, 5, UINT64_C(0x3), seq_table_inter_freq_neigh_cell_info_s_opts, 3},
    true,
    false,
    nullptr,
    0,
    seq_table_inter_freq_neigh_cell_info_s_names};

// InterFreqCarrierFreqInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_inter_freq_carrier_freq_info_s_thresh_x_q_s__fields[] = {
    {offsetof(inter_freq_carrier_freq_info_s::thresh_x_q_s_, thresh_x_high_q),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s::thresh_x_q_s_, thresh_x_low_q),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr}};
static const asn1::table::field_name seq_table_inter_freq_carrier_freq_info_s_thresh_x_q_s__names[] = {
    {"threshX-HighQ", nullptr},
    {"threshX-LowQ", nullptr}};
static const asn1::table::seq_desc seq_table_inter_freq_carrier_freq_info_s_thresh_x_q_s_ = {
    {seq_table_inter_freq_carrier_freq_info_s_thresh_x_q_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_inter_freq_carrier_freq_info_s_thresh_x_q_s__names};
static const asn1::table::seq_field seq_table_inter_freq_carrier_freq_info_s_fields[] = {
    {offsetof(inter_freq_carrier_freq_info_s, dl_carrier_freq),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s, freq_band_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_s::freq_band_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_s::desc},
    {offsetof(inter_freq_carrier_freq_info_s, freq_band_list_sul),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_s::freq_band_list_sul),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_s::desc},
    {offsetof(inter_freq_carrier_freq_info_s, nrof_ss_blocks_to_average),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, nrof_ss_blocks_to_average_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(2),
     UINT64_C(16),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s, abs_thresh_ss_blocks_consolidation),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, abs_thresh_ss_blocks_consolidation_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &thres_nr_s::desc},
    {offsetof(inter_freq_carrier_freq_info_s, smtc),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, smtc_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_mtc_s::desc},
    {offsetof(inter_freq_carrier_freq_info_s, ssb_subcarrier_spacing),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(inter_freq_carrier_freq_info_s::ssb_subcarrier_spacing)>(),
     decltype(inter_freq_carrier_freq_info_s::ssb_subcarrier_spacing)::has_ext,
     false,
     decltype(inter_freq_carrier_freq_info_s::ssb_subcarrier_spacing)::nof_types,
     decltype(inter_freq_carrier_freq_info_s::ssb_subcarrier_spacing)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(inter_freq_carrier_freq_info_s, ssb_to_measure),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, ssb_to_measure_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_to_measure_c::desc},
    {offsetof(inter_freq_carrier_freq_info_s, derive_ssb_idx_from_cell),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s, ss_rssi_meas),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, ss_rssi_meas_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ss_rssi_meas_s::desc},
    {offsetof(inter_freq_carrier_freq_info_s, q_rx_lev_min),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-70),
     UINT64_C(-22),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s, q_rx_lev_min_sul),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, q_rx_lev_min_sul_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-70),
     UINT64_C(-22),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s, q_qual_min),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, q_qual_min_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-43),
     UINT64_C(-12),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s, p_max),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, p_max_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s, t_resel_nr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s, t_resel_nr_sf),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, t_resel_nr_sf_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &speed_state_scale_factors_s::desc},
    {offsetof(inter_freq_carrier_freq_info_s, thresh_x_high_p),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s, thresh_x_low_p),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s, thresh_x_q),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, thresh_x_q_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_inter_freq_carrier_freq_info_s_thresh_x_q_s_},
    {offsetof(inter_freq_carrier_freq_info_s, cell_resel_prio),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, cell_resel_prio_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_s, cell_resel_sub_prio),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, cell_resel_sub_prio_present),
     asn1::table::enum_op<decltype(inter_freq_carrier_freq_info_s::cell_resel_sub_prio)>(),
     decltype(inter_freq_carrier_freq_info_s::cell_resel_sub_prio)::has_ext,
     false,
     decltype(inter_freq_carrier_freq_info_s::cell_resel_sub_prio)::nof_types,
     decltype(inter_freq_carrier_freq_info_s::cell_resel_sub_prio)::nof_exts,
     nullptr,
     &cell_resel_sub_prio_opts::desc},
    {offsetof(inter_freq_carrier_freq_info_s, q_offset_freq),
     (int32_t)offsetof(inter_freq_carrier_freq_info_s, q_offset_freq_present),
     asn1::table::enum_op<decltype(inter_freq_carrier_freq_info_s::q_offset_freq)>(),
     decltype(inter_freq_carrier_freq_info_s::q_offset_freq)::has_ext,
     false,
     decltype(inter_freq_carrier_freq_info_s::q_offset_freq)::nof_types,
     decltype(inter_freq_carrier_freq_info_s::q_offset_freq)::nof_exts,
     nullptr,
     &q_offset_range_opts::desc},
    {offsetof(inter_freq_carrier_freq_info_s, inter_freq_neigh_cell_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_s::inter_freq_neigh_cell_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &inter_freq_neigh_cell_info_s::desc},
    {offsetof(inter_freq_carrier_freq_info_s, inter_freq_excluded_cell_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_s::inter_freq_excluded_cell_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pci_range_s::desc}};
static const asn1::table::field_name seq_table_inter_freq_carrier_freq_info_s_names[] = {
    {"dl-CarrierFreq", nullptr},
    {"frequencyBandList", nullptr},
    {"frequencyBandListSUL", nullptr},
    {"nrofSS-BlocksToAverage", nullptr},
    {"absThreshSS-BlocksConsolidation", nullptr},
    {"smtc", nullptr},
    {"ssbSubcarrierSpacing", nullptr},
    {"ssb-ToMeasure", nullptr},
    {"deriveSSB-IndexFromCell", nullptr},
    {"ss-RSSI-Measurement", nullptr},
    {"q-RxLevMin", nullptr},
    {"q-RxLevMinSUL", nullptr},
    {"q-QualMin", nullptr},
    {"p-Max", nullptr},
    {"t-ReselectionNR", nullptr},
    {"t-ReselectionNR-SF", nullptr},
    {"threshX-HighP", nullptr},
    {"threshX-LowP", nullptr},
    {"threshX-Q", nullptr},
    {"cellReselectionPriority", nullptr},
    {"cellReselectionSubPriority", nullptr},
    {"q-OffsetFreq", nullptr},
    {"interFreqNeighCellList", nullptr},
    {"interFreqExcludedCellList", nullptr}};
static const asn1::table::seq_opt seq_table_inter_freq_carrier_freq_info_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(inter_freq_carrier_freq_info_s, nrof_ss_blocks_to_average_present)},
    {4, (int32_t)offsetof(inter_freq_carrier_freq_info_s, abs_thresh_ss_blocks_consolidation_present)},
    {5, (int32_t)offsetof(inter_freq_carrier_freq_info_s, smtc_present)},
    {7, (int32_t)offsetof(inter_freq_carrier_freq_info_s, ssb_to_measure_present)},
    {9, (int32_t)offsetof(inter_freq_carrier_freq_info_s, ss_rssi_meas_present)},
    {11, (int32_t)offsetof(inter_freq_carrier_freq_info_s, q_rx_lev_min_sul_present)},
    {12, (int32_t)offsetof(inter_freq_carrier_freq_info_s, q_qual_min_present)},
    {13, (int32_t)offsetof(inter_freq_carrier_freq_info_s, p_max_present)},
    {15, (int32_t)offsetof(inter_freq_carrier_freq_info_s, t_resel_nr_sf_present)},
    {18, (int32_t)offsetof(inter_freq_carrier_freq_info_s, thresh_x_q_present)},
    {19, (int32_t)offsetof(inter_freq_carrier_freq_info_s, cell_resel_prio_present)},
    {20, (int32_t)offsetof(inter_freq_carrier_freq_info_s, cell_resel_sub_prio_present)},
    {21, (int32_t)offsetof(inter_freq_carrier_freq_info_s, q_offset_freq_present)},
    {22, asn1::table::seq_field::list_presence},
    {23, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc inter_freq_carrier_freq_info_s::desc = {{seq_table_inter_freq_carrier_freq_info_s_fields,
                                                                     24,
                                                                     UINT64_C(0x34541),
                                                                     seq_table_inter_freq_carrier_freq_info_s_opts,
                                                                     17},
                                                                    true,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_inter_freq_carrier_freq_info_s_names};

// InterFreqNeighCellInfo-v1610 ::= SEQUENCE
static const asn1::table::seq_field seq_table_inter_freq_neigh_cell_info_v1610_s_fields[] = {
    {offsetof(inter_freq_neigh_cell_info_v1610_s, ssb_position_qcl_r16),
     (int32_t)offsetof(inter_freq_neigh_cell_info_v1610_s, ssb_position_qcl_r16_present),
     asn1::table::enum_op<decltype(inter_freq_neigh_cell_info_v1610_s::ssb_position_qcl_r16)>(),
     decltype(inter_freq_neigh_cell_info_v1610_s::ssb_position_qcl_r16)::has_ext,
     false,
     decltype(inter_freq_neigh_cell_info_v1610_s::ssb_position_qcl_r16)::nof_types,
     decltype(inter_freq_neigh_cell_info_v1610_s::ssb_position_qcl_r16)::nof_exts,
     nullptr,
     &ssb_position_qcl_relation_r16_opts::desc}};
static const asn1::table::field_name seq_table_inter_freq_neigh_cell_info_v1610_s_names[] = {
    {"ssb-PositionQCL-r16", nullptr}};
static const asn1::table::seq_opt seq_table_inter_freq_neigh_cell_info_v1610_s_opts[] = {
    {0, (int32_t)offsetof(inter_freq_neigh_cell_info_v1610_s, ssb_position_qcl_r16_present)}};
const asn1::table::seq_desc inter_freq_neigh_cell_info_v1610_s::desc = {
    {seq_table_inter_freq_neigh_cell_info_v1610_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_inter_freq_neigh_cell_info_v1610_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_inter_freq_neigh_cell_info_v1610_s_names};

// InterFreqCAG-CellListPerPLMN-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_inter_freq_cag_cell_list_per_plmn_r16_s_fields[] = {
    {offsetof(inter_freq_cag_cell_list_per_plmn_r16_s, plmn_id_idx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(12),
     nullptr},
    {offsetof(inter_freq_cag_cell_list_per_plmn_r16_s, cag_cell_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(inter_freq_cag_cell_list_per_plmn_r16_s::cag_cell_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pci_range_s::desc}};
static const asn1::table::field_name seq_table_inter_freq_cag_cell_list_per_plmn_r16_s_names[] = {
    {"plmn-IdentityIndex-r16", nullptr},
    {"cag-CellList-r16", nullptr}};
const asn1::table::seq_desc inter_freq_cag_cell_list_per_plmn_r16_s::desc = {
    {seq_table_inter_freq_cag_cell_list_per_plmn_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_inter_freq_cag_cell_list_per_plmn_r16_s_names};

// InterFreqCarrierFreqInfo-v1610 ::= SEQUENCE
static const asn1::table::seq_field seq_table_inter_freq_carrier_freq_info_v1610_s_fields[] = {
    {offsetof(inter_freq_carrier_freq_info_v1610_s, inter_freq_neigh_cell_list_v1610),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_v1610_s::inter_freq_neigh_cell_list_v1610),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &inter_freq_neigh_cell_info_v1610_s::desc},
    {offsetof(inter_freq_carrier_freq_info_v1610_s, smtc2_lp_r16),
     (int32_t)offsetof(inter_freq_carrier_freq_info_v1610_s, smtc2_lp_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_mtc2_lp_r16_s::desc},
    {offsetof(inter_freq_carrier_freq_info_v1610_s, inter_freq_allowed_cell_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_v1610_s::inter_freq_allowed_cell_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pci_range_s::desc},
    {offsetof(inter_freq_carrier_freq_info_v1610_s, ssb_position_qcl_common_r16),
     (int32_t)offsetof(inter_freq_carrier_freq_info_v1610_s, ssb_position_qcl_common_r16_present),
     asn1::table::enum_op<decltype(inter_freq_carrier_freq_info_v1610_s::ssb_position_qcl_common_r16)>(),
     decltype(inter_freq_carrier_freq_info_v1610_s::ssb_position_qcl_common_r16)::has_ext,
     false,
     decltype(inter_freq_carrier_freq_info_v1610_s::ssb_position_qcl_common_r16)::nof_types,
     decltype(inter_freq_carrier_freq_info_v1610_s::ssb_position_qcl_common_r16)::nof_exts,
     nullptr,
     &ssb_position_qcl_relation_r16_opts::desc},
    {offsetof(inter_freq_carrier_freq_info_v1610_s, inter_freq_cag_cell_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_v1610_s::inter_freq_cag_cell_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &inter_freq_cag_cell_list_per_plmn_r16_s::desc}};
static const asn1::table::field_name seq_table_inter_freq_carrier_freq_info_v1610_s_names[] = {
    {"interFreqNeighCellList-v1610", nullptr},
    {"smtc2-LP-r16", nullptr},
    {"interFreqAllowedCellList-r16", nullptr},
    {"ssb-PositionQCL-Common-r16", nullptr},
    {"interFreqCAG-CellList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_inter_freq_carrier_freq_info_v1610_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(inter_freq_carrier_freq_info_v1610_s, smtc2_lp_r16_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(inter_freq_carrier_freq_info_v1610_s, ssb_position_qcl_common_r16_present)},
    {4, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc inter_freq_carrier_freq_info_v1610_s::desc = {
    {seq_table_inter_freq_carrier_freq_info_v1610_s_fields,
     5,
     UINT64_C(0x0),
     seq_table_inter_freq_carrier_freq_info_v1610_s_opts,
     5},
    false,
    false,
    nullptr,
    0,
    seq_table_inter_freq_carrier_freq_info_v1610_s_names};

// InterFreqNeighCellInfo-v1710 ::= SEQUENCE
static const asn1::table::seq_field seq_table_inter_freq_neigh_cell_info_v1710_s_fields[] = {
    {offsetof(inter_freq_neigh_cell_info_v1710_s, ssb_position_qcl_r17),
     (int32_t)offsetof(inter_freq_neigh_cell_info_v1710_s, ssb_position_qcl_r17_present),
     asn1::table::enum_op<decltype(inter_freq_neigh_cell_info_v1710_s::ssb_position_qcl_r17)>(),
     decltype(inter_freq_neigh_cell_info_v1710_s::ssb_position_qcl_r17)::has_ext,
     false,
     decltype(inter_freq_neigh_cell_info_v1710_s::ssb_position_qcl_r17)::nof_types,
     decltype(inter_freq_neigh_cell_info_v1710_s::ssb_position_qcl_r17)::nof_exts,
     nullptr,
     &ssb_position_qcl_relation_r17_opts::desc}};
static const asn1::table::field_name seq_table_inter_freq_neigh_cell_info_v1710_s_names[] = {
    {"ssb-PositionQCL-r17", nullptr}};
static const asn1::table::seq_opt seq_table_inter_freq_neigh_cell_info_v1710_s_opts[] = {
    {0, (int32_t)offsetof(inter_freq_neigh_cell_info_v1710_s, ssb_position_qcl_r17_present)}};
const asn1::table::seq_desc inter_freq_neigh_cell_info_v1710_s::desc = {
    {seq_table_inter_freq_neigh_cell_info_v1710_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_inter_freq_neigh_cell_info_v1710_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_inter_freq_neigh_cell_info_v1710_s_names};

// InterFreqCarrierFreqInfo-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_inter_freq_carrier_freq_info_v1700_s_fields[] = {
    {offsetof(inter_freq_carrier_freq_info_v1700_s, inter_freq_neigh_hsdn_cell_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_v1700_s::inter_freq_neigh_hsdn_cell_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pci_range_s::desc},
    {0,
     (int32_t)offsetof(inter_freq_carrier_freq_info_v1700_s, high_speed_meas_inter_freq_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(inter_freq_carrier_freq_info_v1700_s, red_cap_access_allowed_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_v1700_s, ssb_position_qcl_common_r17),
     (int32_t)offsetof(inter_freq_carrier_freq_info_v1700_s, ssb_position_qcl_common_r17_present),
     asn1::table::enum_op<decltype(inter_freq_carrier_freq_info_v1700_s::ssb_position_qcl_common_r17)>(),
     decltype(inter_freq_carrier_freq_info_v1700_s::ssb_position_qcl_common_r17)::has_ext,
     false,
     decltype(inter_freq_carrier_freq_info_v1700_s::ssb_position_qcl_common_r17)::nof_types,
     decltype(inter_freq_carrier_freq_info_v1700_s::ssb_position_qcl_common_r17)::nof_exts,
     nullptr,
     &ssb_position_qcl_relation_r17_opts::desc},
    {offsetof(inter_freq_carrier_freq_info_v1700_s, inter_freq_neigh_cell_list_v1710),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_v1700_s::inter_freq_neigh_cell_list_v1710),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &inter_freq_neigh_cell_info_v1710_s::desc}};
static const asn1::table::field_name seq_table_inter_freq_carrier_freq_info_v1700_s_names[] = {
    {"interFreqNeighHSDN-CellList-r17", nullptr},
    {"highSpeedMeasInterFreq-r17", "true"},
    {"redCapAccessAllowed-r17", "true"},
    {"ssb-PositionQCL-Common-r17", nullptr},
    {"interFreqNeighCellList-v1710", nullptr}};
static const asn1::table::seq_opt seq_table_inter_freq_carrier_freq_info_v1700_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(inter_freq_carrier_freq_info_v1700_s, high_speed_meas_inter_freq_r17_present)},
    {2, (int32_t)offsetof(inter_freq_carrier_freq_info_v1700_s, red_cap_access_allowed_r17_present)},
    {3, (int32_t)offsetof(inter_freq_carrier_freq_info_v1700_s, ssb_position_qcl_common_r17_present)},
    {4, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc inter_freq_carrier_freq_info_v1700_s::desc = {
    {seq_table_inter_freq_carrier_freq_info_v1700_s_fields,
     5,
     UINT64_C(0x0),
     seq_table_inter_freq_carrier_freq_info_v1700_s_opts,
     5},
    false,
    false,
    nullptr,
    0,
    seq_table_inter_freq_carrier_freq_info_v1700_s_names};

// InterFreqCarrierFreqInfo-v1720 ::= SEQUENCE
static const asn1::table::seq_field seq_table_inter_freq_carrier_freq_info_v1720_s_fields[] = {
    {offsetof(inter_freq_carrier_freq_info_v1720_s, smtc4list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     3,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_v1720_s::smtc4list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ssb_mtc4_r17_s::desc}};
static const asn1::table::field_name seq_table_inter_freq_carrier_freq_info_v1720_s_names[] = {
    {"smtc4list-r17", nullptr}};
static const asn1::table::seq_opt seq_table_inter_freq_carrier_freq_info_v1720_s_opts[] = {
    {0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc inter_freq_carrier_freq_info_v1720_s::desc = {
    {seq_table_inter_freq_carrier_freq_info_v1720_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_inter_freq_carrier_freq_info_v1720_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_inter_freq_carrier_freq_info_v1720_s_names};

// InterFreqCarrierFreqInfo-v1730 ::= SEQUENCE
static const asn1::table::seq_field seq_table_inter_freq_carrier_freq_info_v1730_s_fields[] = {
    {0,
     (int32_t)offsetof(inter_freq_carrier_freq_info_v1730_s, ch_access_mode2_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_inter_freq_carrier_freq_info_v1730_s_names[] = {
    {"channelAccessMode2-r17", "enabled"}};
static const asn1::table::seq_opt seq_table_inter_freq_carrier_freq_info_v1730_s_opts[] = {
    {0, (int32_t)offsetof(inter_freq_carrier_freq_info_v1730_s, ch_access_mode2_r17_present)}};
const asn1::table::seq_desc inter_freq_carrier_freq_info_v1730_s::desc = {
    {seq_table_inter_freq_carrier_freq_info_v1730_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_inter_freq_carrier_freq_info_v1730_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_inter_freq_carrier_freq_info_v1730_s_names};

// InterFreqCarrierFreqInfo-v1760 ::= SEQUENCE
static const asn1::table::seq_field seq_table_inter_freq_carrier_freq_info_v1760_s_fields[] = {
    {offsetof(inter_freq_carrier_freq_info_v1760_s, freq_band_list_v1760),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_v1760_s::freq_band_list_v1760),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_v1760_s::desc},
    {offsetof(inter_freq_carrier_freq_info_v1760_s, freq_band_list_sul_v1760),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_v1760_s::freq_band_list_sul_v1760),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_v1760_s::desc}};
static const asn1::table::field_name seq_table_inter_freq_carrier_freq_info_v1760_s_names[] = {
    {"frequencyBandList-v1760", nullptr},
    {"frequencyBandListSUL-v1760", nullptr}};
static const asn1::table::seq_opt seq_table_inter_freq_carrier_freq_info_v1760_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc inter_freq_carrier_freq_info_v1760_s::desc = {
    {seq_table_inter_freq_carrier_freq_info_v1760_s_fields,
     2,
     UINT64_C(0x0),
     seq_table_inter_freq_carrier_freq_info_v1760_s_opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_inter_freq_carrier_freq_info_v1760_s_names};

// InterFreqCarrierFreqInfo-v1800 ::= SEQUENCE
static const asn1::table::seq_field seq_table_inter_freq_carrier_freq_info_v1800_s_fields[] = {
    {offsetof(inter_freq_carrier_freq_info_v1800_s, dl_carrier_freq_r18),
     (int32_t)offsetof(inter_freq_carrier_freq_info_v1800_s, dl_carrier_freq_r18_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_v1800_s, freq_band_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_v1800_s::freq_band_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_s::desc},
    {offsetof(inter_freq_carrier_freq_info_v1800_s, freq_band_list_aerial_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(inter_freq_carrier_freq_info_v1800_s::freq_band_list_aerial_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_aerial_r18_s::desc},
    {offsetof(inter_freq_carrier_freq_info_v1800_s, mobile_iab_cell_list_r18),
     (int32_t)offsetof(inter_freq_carrier_freq_info_v1800_s, mobile_iab_cell_list_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pci_range_s::desc},
    {0,
     (int32_t)offsetof(inter_freq_carrier_freq_info_v1800_s, mobile_iab_freq_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(inter_freq_carrier_freq_info_v1800_s, ered_cap_access_allowed_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(inter_freq_carrier_freq_info_v1800_s, tn_area_id_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::list_codec<decltype(inter_freq_carrier_freq_info_v1800_s::tn_area_id_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(32), false, false>>::value},
    {0,
     (int32_t)offsetof(inter_freq_carrier_freq_info_v1800_s, access_allowed2_rx_xr_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_inter_freq_carrier_freq_info_v1800_s_names[] = {
    {"dl-CarrierFreq-r18", nullptr},
    {"frequencyBandList-r18", nullptr},
    {"frequencyBandListAerial-r18", nullptr},
    {"mobileIAB-CellList-r18", nullptr},
    {"mobileIAB-Freq-r18", "true"},
    {"eRedCapAccessAllowed-r18", "true"},
    {"tn-AreaIdList-r18", nullptr},
    {"accessAllowed2RxXR-r18", "true"}};
static const asn1::table::seq_opt seq_table_inter_freq_carrier_freq_info_v1800_s_opts[] = {
    {0, (int32_t)offsetof(inter_freq_carrier_freq_info_v1800_s, dl_carrier_freq_r18_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(inter_freq_carrier_freq_info_v1800_s, mobile_iab_cell_list_r18_present)},
    {4, (int32_t)offsetof(inter_freq_carrier_freq_info_v1800_s, mobile_iab_freq_r18_present)},
    {5, (int32_t)offsetof(inter_freq_carrier_freq_info_v1800_s, ered_cap_access_allowed_r18_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, (int32_t)offsetof(inter_freq_carrier_freq_info_v1800_s, access_allowed2_rx_xr_r18_present)}};
const asn1::table::seq_desc inter_freq_carrier_freq_info_v1800_s::desc = {
    {seq_table_inter_freq_carrier_freq_info_v1800_s_fields,
     8,
     UINT64_C(0x0),
     seq_table_inter_freq_carrier_freq_info_v1800_s_opts,
     8},
    false,
    false,
    nullptr,
    0,
    seq_table_inter_freq_carrier_freq_info_v1800_s_names};

// SIB4 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib4_s_fields[] = {
    {offsetof(sib4_s, inter_freq_carrier_freq_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sib4_s::inter_freq_carrier_freq_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &inter_freq_carrier_freq_info_s::desc},
    {offsetof(sib4_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib4_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib4_s_names[]     = {{"interFreqCarrierFreqList", nullptr},
                                                                     {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib4_s_opts[]      = {{1, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field  seq_table_sib4_s_g0_fields[] = {
    {offsetof(sib4_s, inter_freq_carrier_freq_list_v1610),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      8,
      &asn1::table::ptr_item_list_codec<decltype(sib4_s::inter_freq_carrier_freq_list_v1610),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &inter_freq_carrier_freq_info_v1610_s::desc}};
static const asn1::table::field_name seq_table_sib4_s_g0_names[]  = {{"interFreqCarrierFreqList-v1610", nullptr}};
static const asn1::table::seq_opt    seq_table_sib4_s_g0_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sib4_s_g1_fields[] = {
    {offsetof(sib4_s, inter_freq_carrier_freq_list_v1700),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      8,
      &asn1::table::ptr_item_list_codec<decltype(sib4_s::inter_freq_carrier_freq_list_v1700),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &inter_freq_carrier_freq_info_v1700_s::desc}};
static const asn1::table::field_name seq_table_sib4_s_g1_names[]  = {{"interFreqCarrierFreqList-v1700", nullptr}};
static const asn1::table::seq_opt    seq_table_sib4_s_g1_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sib4_s_g2_fields[] = {
    {offsetof(sib4_s, inter_freq_carrier_freq_list_v1720),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      8,
      &asn1::table::ptr_item_list_codec<decltype(sib4_s::inter_freq_carrier_freq_list_v1720),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &inter_freq_carrier_freq_info_v1720_s::desc}};
static const asn1::table::field_name seq_table_sib4_s_g2_names[]  = {{"interFreqCarrierFreqList-v1720", nullptr}};
static const asn1::table::seq_opt    seq_table_sib4_s_g2_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sib4_s_g3_fields[] = {
    {offsetof(sib4_s, inter_freq_carrier_freq_list_v1730),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      8,
      &asn1::table::ptr_item_list_codec<decltype(sib4_s::inter_freq_carrier_freq_list_v1730),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &inter_freq_carrier_freq_info_v1730_s::desc}};
static const asn1::table::field_name seq_table_sib4_s_g3_names[]  = {{"interFreqCarrierFreqList-v1730", nullptr}};
static const asn1::table::seq_opt    seq_table_sib4_s_g3_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sib4_s_g4_fields[] = {
    {offsetof(sib4_s, inter_freq_carrier_freq_list_v1760),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      8,
      &asn1::table::ptr_item_list_codec<decltype(sib4_s::inter_freq_carrier_freq_list_v1760),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &inter_freq_carrier_freq_info_v1760_s::desc}};
static const asn1::table::field_name seq_table_sib4_s_g4_names[]  = {{"interFreqCarrierFreqList-v1760", nullptr}};
static const asn1::table::seq_opt    seq_table_sib4_s_g4_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sib4_s_g5_fields[] = {
    {offsetof(sib4_s, inter_freq_carrier_freq_list_v1800),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      8,
      &asn1::table::ptr_item_list_codec<decltype(sib4_s::inter_freq_carrier_freq_list_v1800),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &inter_freq_carrier_freq_info_v1800_s::desc}};
static const asn1::table::field_name seq_table_sib4_s_g5_names[] = {{"interFreqCarrierFreqList-v1800", nullptr}};
static const asn1::table::seq_opt    seq_table_sib4_s_g5_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_sib4_s_groups[]   = {
    {{seq_table_sib4_s_g0_fields, 1, UINT64_C(0x0), seq_table_sib4_s_g0_opts, 1}, false, seq_table_sib4_s_g0_names},
    {{seq_table_sib4_s_g1_fields, 1, UINT64_C(0x0), seq_table_sib4_s_g1_opts, 1}, false, seq_table_sib4_s_g1_names},
    {{seq_table_sib4_s_g2_fields, 1, UINT64_C(0x0), seq_table_sib4_s_g2_opts, 1}, false, seq_table_sib4_s_g2_names},
    {{seq_table_sib4_s_g3_fields, 1, UINT64_C(0x0), seq_table_sib4_s_g3_opts, 1}, false, seq_table_sib4_s_g3_names},
    {{seq_table_sib4_s_g4_fields, 1, UINT64_C(0x0), seq_table_sib4_s_g4_opts, 1}, false, seq_table_sib4_s_g4_names},
    {{seq_table_sib4_s_g5_fields, 1, UINT64_C(0x0), seq_table_sib4_s_g5_opts, 1}, false, seq_table_sib4_s_g5_names}};
const asn1::table::seq_desc sib4_s::desc = {{seq_table_sib4_s_fields, 2, UINT64_C(0x1), seq_table_sib4_s_opts, 1},
                                            true,
                                            false,
                                            seq_table_sib4_s_groups,
                                            6,
                                            seq_table_sib4_s_names};

// EUTRA-NS-PmaxValue ::= SEQUENCE
static const asn1::table::seq_field seq_table_eutra_ns_pmax_value_s_fields[] = {
    {offsetof(eutra_ns_pmax_value_s, add_pmax),
     (int32_t)offsetof(eutra_ns_pmax_value_s, add_pmax_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr},
    {offsetof(eutra_ns_pmax_value_s, add_spec_emission),
     (int32_t)offsetof(eutra_ns_pmax_value_s, add_spec_emission_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(288),
     nullptr}};
static const asn1::table::field_name seq_table_eutra_ns_pmax_value_s_names[] = {
    {"additionalPmax", nullptr},
    {"additionalSpectrumEmission", nullptr}};
static const asn1::table::seq_opt seq_table_eutra_ns_pmax_value_s_opts[] = {
    {0, (int32_t)offsetof(eutra_ns_pmax_value_s, add_pmax_present)},
    {1, (int32_t)offsetof(eutra_ns_pmax_value_s, add_spec_emission_present)}};
const asn1::table::seq_desc eutra_ns_pmax_value_s::desc = {
    {seq_table_eutra_ns_pmax_value_s_fields, 2, UINT64_C(0x0), seq_table_eutra_ns_pmax_value_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_eutra_ns_pmax_value_s_names};

// EUTRA-MultiBandInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_eutra_multi_band_info_s_fields[] = {
    {offsetof(eutra_multi_band_info_s, eutra_freq_band_ind),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(256),
     nullptr},
    {offsetof(eutra_multi_band_info_s, eutra_ns_pmax_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(eutra_multi_band_info_s::eutra_ns_pmax_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &eutra_ns_pmax_value_s::desc}};
static const asn1::table::field_name seq_table_eutra_multi_band_info_s_names[] = {{"eutra-FreqBandIndicator", nullptr},
                                                                                  {"eutra-NS-PmaxList", nullptr}};
static const asn1::table::seq_opt    seq_table_eutra_multi_band_info_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc eutra_multi_band_info_s::desc = {
    {seq_table_eutra_multi_band_info_s_fields, 2, UINT64_C(0x1), seq_table_eutra_multi_band_info_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_eutra_multi_band_info_s_names};

// EUTRA-Q-OffsetRange ::= ENUMERATED
static const char* enum_names_eutra_q_offset_range_e_values[] = {
    "dB-24", "dB-22", "dB-20", "dB-18", "dB-16", "dB-14", "dB-12", "dB-10", "dB-8", "dB-6", "dB-5",
    "dB-4",  "dB-3",  "dB-2",  "dB-1",  "dB0",   "dB1",   "dB2",   "dB3",   "dB4",  "dB5",  "dB6",
    "dB8",   "dB10",  "dB12",  "dB14",  "dB16",  "dB18",  "dB20",  "dB22",  "dB24"};
const asn1::table::enum_desc eutra_q_offset_range_opts::desc = {enum_names_eutra_q_offset_range_e_values,
                                                                31,
                                                                "eutra_q_offset_range_e"};
int8_t                       eutra_q_offset_range_opts::to_number() const
{
  static const int8_t numbers[] = {-24, -22, -20, -18, -16, -14, -12, -10, -8, -6, -5, -4, -3, -2, -1, 0,
                                   1,   2,   3,   4,   5,   6,   8,   10,  12, 14, 16, 18, 20, 22, 24};
  return map_enum_number(numbers, 31, value, "eutra_q_offset_range_e");
}

// EUTRA-FreqNeighCellInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_eutra_freq_neigh_cell_info_s_fields[] = {
    {offsetof(eutra_freq_neigh_cell_info_s, pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(503),
     nullptr},
    {offsetof(eutra_freq_neigh_cell_info_s, dummy),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(eutra_freq_neigh_cell_info_s::dummy)>(),
     decltype(eutra_freq_neigh_cell_info_s::dummy)::has_ext,
     false,
     decltype(eutra_freq_neigh_cell_info_s::dummy)::nof_types,
     decltype(eutra_freq_neigh_cell_info_s::dummy)::nof_exts,
     nullptr,
     &eutra_q_offset_range_opts::desc},
    {offsetof(eutra_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell),
     (int32_t)offsetof(eutra_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(eutra_freq_neigh_cell_info_s, q_qual_min_offset_cell),
     (int32_t)offsetof(eutra_freq_neigh_cell_info_s, q_qual_min_offset_cell_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr}};
static const asn1::table::field_name seq_table_eutra_freq_neigh_cell_info_s_names[] =
    {{"physCellId", nullptr}, {"dummy", nullptr}, {"q-RxLevMinOffsetCell", nullptr}, {"q-QualMinOffsetCell", nullptr}};
static const asn1::table::seq_opt seq_table_eutra_freq_neigh_cell_info_s_opts[] = {
    {2, (int32_t)offsetof(eutra_freq_neigh_cell_info_s, q_rx_lev_min_offset_cell_present)},
    {3, (int32_t)offsetof(eutra_freq_neigh_cell_info_s, q_qual_min_offset_cell_present)}};
const asn1::table::seq_desc eutra_freq_neigh_cell_info_s::desc = {
    {seq_table_eutra_freq_neigh_cell_info_s_fields, 4, UINT64_C(0x3), seq_table_eutra_freq_neigh_cell_info_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_eutra_freq_neigh_cell_info_s_names};

// EUTRA-PhysCellIdRange ::= SEQUENCE
static const asn1::table::seq_field seq_table_eutra_pci_range_s_fields[] = {
    {offsetof(eutra_pci_range_s, start),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(503),
     nullptr},
    {offsetof(eutra_pci_range_s, range),
     (int32_t)offsetof(eutra_pci_range_s, range_present),
     asn1::table::enum_op<decltype(eutra_pci_range_s::range)>(),
     decltype(eutra_pci_range_s::range)::has_ext,
     false,
     decltype(eutra_pci_range_s::range)::nof_types,
     decltype(eutra_pci_range_s::range)::nof_exts,
     nullptr,
     &eutra_pci_range_s::range_opts::desc}};
static const asn1::table::field_name seq_table_eutra_pci_range_s_names[] = {{"start", nullptr}, {"range", nullptr}};
static const asn1::table::seq_opt    seq_table_eutra_pci_range_s_opts[]  = {
    {1, (int32_t)offsetof(eutra_pci_range_s, range_present)}};
const asn1::table::seq_desc eutra_pci_range_s::desc = {
    {seq_table_eutra_pci_range_s_fields, 2, UINT64_C(0x1), seq_table_eutra_pci_range_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_eutra_pci_range_s_names};

static const char*           enum_names_eutra_pci_range_s_range_e__values[] = {"n4",
                                                                               "n8",
                                                                               "n12",
                                                                               "n16",
                                                                               "n24",
                                                                               "n32",
                                                                               "n48",
                                                                               "n64",
                                                                               "n84",
                                                                               "n96",
                                                                               "n128",
                                                                               "n168",
                                                                               "n252",
                                                                               "n504",
                                                                               "spare2",
                                                                               "spare1"};
const asn1::table::enum_desc eutra_pci_range_s::range_opts::desc = {enum_names_eutra_pci_range_s_range_e__values,
                                                                    16,
                                                                    "eutra_pci_range_s::range_e_"};
uint16_t                     eutra_pci_range_s::range_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 8, 12, 16, 24, 32, 48, 64, 84, 96, 128, 168, 252, 504};
  return map_enum_number(numbers, 14, value, "eutra_pci_range_s::range_e_");
}

// EUTRA-AllowedMeasBandwidth ::= ENUMERATED
static const char* enum_names_eutra_allowed_meas_bw_e_values[] = {"mbw6", "mbw15", "mbw25", "mbw50", "mbw75", "mbw100"};
const asn1::table::enum_desc eutra_allowed_meas_bw_opts::desc  = {enum_names_eutra_allowed_meas_bw_e_values,
                                                                  6,
                                                                  "eutra_allowed_meas_bw_e"};
uint8_t                      eutra_allowed_meas_bw_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(numbers, 6, value, "eutra_allowed_meas_bw_e");
}

// CarrierFreqEUTRA ::= SEQUENCE
static const asn1::table::seq_field seq_table_carrier_freq_eutra_s_thresh_x_q_s__fields[] = {
    {offsetof(carrier_freq_eutra_s::thresh_x_q_s_, thresh_x_high_q),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(carrier_freq_eutra_s::thresh_x_q_s_, thresh_x_low_q),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr}};
static const asn1::table::field_name seq_table_carrier_freq_eutra_s_thresh_x_q_s__names[] = {{"threshX-HighQ", nullptr},
                                                                                             {"threshX-LowQ", nullptr}};
static const asn1::table::seq_desc   seq_table_carrier_freq_eutra_s_thresh_x_q_s_         = {
    {seq_table_carrier_freq_eutra_s_thresh_x_q_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_carrier_freq_eutra_s_thresh_x_q_s__names};
static const asn1::table::seq_field seq_table_carrier_freq_eutra_s_fields[] = {
    {offsetof(carrier_freq_eutra_s, carrier_freq),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(262143),
     nullptr},
    {offsetof(carrier_freq_eutra_s, eutra_multi_band_info_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(carrier_freq_eutra_s::eutra_multi_band_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &eutra_multi_band_info_s::desc},
    {offsetof(carrier_freq_eutra_s, eutra_freq_neigh_cell_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(carrier_freq_eutra_s::eutra_freq_neigh_cell_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &eutra_freq_neigh_cell_info_s::desc},
    {offsetof(carrier_freq_eutra_s, eutra_excluded_cell_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(carrier_freq_eutra_s::eutra_excluded_cell_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &eutra_pci_range_s::desc},
    {offsetof(carrier_freq_eutra_s, allowed_meas_bw),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(carrier_freq_eutra_s::allowed_meas_bw)>(),
     decltype(carrier_freq_eutra_s::allowed_meas_bw)::has_ext,
     false,
     decltype(carrier_freq_eutra_s::allowed_meas_bw)::nof_types,
     decltype(carrier_freq_eutra_s::allowed_meas_bw)::nof_exts,
     nullptr,
     &eutra_allowed_meas_bw_opts::desc},
    {offsetof(carrier_freq_eutra_s, presence_ant_port1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(carrier_freq_eutra_s, cell_resel_prio),
     (int32_t)offsetof(carrier_freq_eutra_s, cell_resel_prio_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(carrier_freq_eutra_s, cell_resel_sub_prio),
     (int32_t)offsetof(carrier_freq_eutra_s, cell_resel_sub_prio_present),
     asn1::table::enum_op<decltype(carrier_freq_eutra_s::cell_resel_sub_prio)>(),
     decltype(carrier_freq_eutra_s::cell_resel_sub_prio)::has_ext,
     false,
     decltype(carrier_freq_eutra_s::cell_resel_sub_prio)::nof_types,
     decltype(carrier_freq_eutra_s::cell_resel_sub_prio)::nof_exts,
     nullptr,
     &cell_resel_sub_prio_opts::desc},
    {offsetof(carrier_freq_eutra_s, thresh_x_high),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(carrier_freq_eutra_s, thresh_x_low),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(carrier_freq_eutra_s, q_rx_lev_min),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-70),
     UINT64_C(-22),
     nullptr},
    {offsetof(carrier_freq_eutra_s, q_qual_min),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-34),
     UINT64_C(-3),
     nullptr},
    {offsetof(carrier_freq_eutra_s, p_max_eutra),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr},
    {offsetof(carrier_freq_eutra_s, thresh_x_q),
     (int32_t)offsetof(carrier_freq_eutra_s, thresh_x_q_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_carrier_freq_eutra_s_thresh_x_q_s_}};
static const asn1::table::field_name seq_table_carrier_freq_eutra_s_names[] = {{"carrierFreq", nullptr},
                                                                               {"eutra-multiBandInfoList", nullptr},
                                                                               {"eutra-FreqNeighCellList", nullptr},
                                                                               {"eutra-ExcludedCellList", nullptr},
                                                                               {"allowedMeasBandwidth", nullptr},
                                                                               {"presenceAntennaPort1", nullptr},
                                                                               {"cellReselectionPriority", nullptr},
                                                                               {"cellReselectionSubPriority", nullptr},
                                                                               {"threshX-High", nullptr},
                                                                               {"threshX-Low", nullptr},
                                                                               {"q-RxLevMin", nullptr},
                                                                               {"q-QualMin", nullptr},
                                                                               {"p-MaxEUTRA", nullptr},
                                                                               {"threshX-Q", nullptr}};
static const asn1::table::seq_opt    seq_table_carrier_freq_eutra_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(carrier_freq_eutra_s, cell_resel_prio_present)},
    {7, (int32_t)offsetof(carrier_freq_eutra_s, cell_resel_sub_prio_present)},
    {13, (int32_t)offsetof(carrier_freq_eutra_s, thresh_x_q_present)}};
const asn1::table::seq_desc carrier_freq_eutra_s::desc = {
    {seq_table_carrier_freq_eutra_s_fields, 14, UINT64_C(0x1f31), seq_table_carrier_freq_eutra_s_opts, 6},
    false,
    false,
    nullptr,
    0,
    seq_table_carrier_freq_eutra_s_names};

// CarrierFreqEUTRA-v1610 ::= SEQUENCE
static const asn1::table::seq_field seq_table_carrier_freq_eutra_v1610_s_fields[] = {
    {0,
     (int32_t)offsetof(carrier_freq_eutra_v1610_s, high_speed_eutra_carrier_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_carrier_freq_eutra_v1610_s_names[] = {
    {"highSpeedEUTRACarrier-r16", "true"}};
static const asn1::table::seq_opt seq_table_carrier_freq_eutra_v1610_s_opts[] = {
    {0, (int32_t)offsetof(carrier_freq_eutra_v1610_s, high_speed_eutra_carrier_r16_present)}};
const asn1::table::seq_desc carrier_freq_eutra_v1610_s::desc = {
    {seq_table_carrier_freq_eutra_v1610_s_fields, 1, UINT64_C(0x0), seq_table_carrier_freq_eutra_v1610_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_carrier_freq_eutra_v1610_s_names};

// CarrierFreqEUTRA-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_carrier_freq_eutra_v1700_s_fields[] = {
    {offsetof(carrier_freq_eutra_v1700_s, eutra_freq_neigh_hsdn_cell_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(carrier_freq_eutra_v1700_s::eutra_freq_neigh_hsdn_cell_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &eutra_pci_range_s::desc}};
static const asn1::table::field_name seq_table_carrier_freq_eutra_v1700_s_names[] = {
    {"eutra-FreqNeighHSDN-CellList-r17", nullptr}};
static const asn1::table::seq_opt seq_table_carrier_freq_eutra_v1700_s_opts[] = {
    {0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc carrier_freq_eutra_v1700_s::desc = {
    {seq_table_carrier_freq_eutra_v1700_s_fields, 1, UINT64_C(0x0), seq_table_carrier_freq_eutra_v1700_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_carrier_freq_eutra_v1700_s_names};

// EUTRA-MultiBandInfoAerial-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_eutra_multi_band_info_aerial_r18_s_fields[] = {
    {offsetof(eutra_multi_band_info_aerial_r18_s, eutra_freq_band_ind_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(256),
     nullptr},
    {offsetof(eutra_multi_band_info_aerial_r18_s, eutra_ns_pmax_list_aerial_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(eutra_multi_band_info_aerial_r18_s::eutra_ns_pmax_list_aerial_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &eutra_ns_pmax_value_s::desc}};
static const asn1::table::field_name seq_table_eutra_multi_band_info_aerial_r18_s_names[] = {
    {"eutra-FreqBandIndicator-r18", nullptr},
    {"eutra-NS-PmaxListAerial-r18", nullptr}};
static const asn1::table::seq_opt seq_table_eutra_multi_band_info_aerial_r18_s_opts[] = {
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc eutra_multi_band_info_aerial_r18_s::desc = {
    {seq_table_eutra_multi_band_info_aerial_r18_s_fields,
     2,
     UINT64_C(0x1),
     seq_table_eutra_multi_band_info_aerial_r18_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_eutra_multi_band_info_aerial_r18_s_names};

// CarrierFreqEUTRA-v1800 ::= SEQUENCE
static const asn1::table::seq_field seq_table_carrier_freq_eutra_v1800_s_fields[] = {
    {offsetof(carrier_freq_eutra_v1800_s, eutra_multi_band_info_list_aerial_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(carrier_freq_eutra_v1800_s::eutra_multi_band_info_list_aerial_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &eutra_multi_band_info_aerial_r18_s::desc},
    {offsetof(carrier_freq_eutra_v1800_s, tn_area_id_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::list_codec<decltype(carrier_freq_eutra_v1800_s::tn_area_id_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(32), false, false>>::value}};
static const asn1::table::field_name seq_table_carrier_freq_eutra_v1800_s_names[] = {
    {"eutra-MultiBandInfoListAerial-r18", nullptr},
    {"tn-AreaIdList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_carrier_freq_eutra_v1800_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc carrier_freq_eutra_v1800_s::desc = {
    {seq_table_carrier_freq_eutra_v1800_s_fields, 2, UINT64_C(0x0), seq_table_carrier_freq_eutra_v1800_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_carrier_freq_eutra_v1800_s_names};

// SIB5 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib5_s_fields[] = {
    {offsetof(sib5_s, carrier_freq_list_eutra),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::
         item_list_codec<decltype(sib5_s::carrier_freq_list_eutra), asn1::table::sequence_item_ops, false, true>::value,
     &carrier_freq_eutra_s::desc},
    {offsetof(sib5_s, t_resel_eutra),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(sib5_s, t_resel_eutra_sf),
     (int32_t)offsetof(sib5_s, t_resel_eutra_sf_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &speed_state_scale_factors_s::desc},
    {offsetof(sib5_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib5_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib5_s_names[]     = {{"carrierFreqListEUTRA", nullptr},
                                                                     {"t-ReselectionEUTRA", nullptr},
                                                                     {"t-ReselectionEUTRA-SF", nullptr},
                                                                     {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib5_s_opts[]      = {{0, asn1::table::seq_field::list_presence},
                                                                     {2, (int32_t)offsetof(sib5_s, t_resel_eutra_sf_present)},
                                                                     {3, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field  seq_table_sib5_s_g0_fields[] = {
    {offsetof(sib5_s, carrier_freq_list_eutra_v1610),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      8,
      &asn1::table::ptr_item_list_codec<decltype(sib5_s::carrier_freq_list_eutra_v1610),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &carrier_freq_eutra_v1610_s::desc}};
static const asn1::table::field_name seq_table_sib5_s_g0_names[]  = {{"carrierFreqListEUTRA-v1610", nullptr}};
static const asn1::table::seq_opt    seq_table_sib5_s_g0_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sib5_s_g1_fields[] = {
    {offsetof(sib5_s, carrier_freq_list_eutra_v1700),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      8,
      &asn1::table::ptr_item_list_codec<decltype(sib5_s::carrier_freq_list_eutra_v1700),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &carrier_freq_eutra_v1700_s::desc},
    {0,
      (int32_t)offsetof(sib5_s, idle_mode_meas_voice_fallback_r17_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr}};
static const asn1::table::field_name seq_table_sib5_s_g1_names[] = {{"carrierFreqListEUTRA-v1700", nullptr},
                                                                    {"idleModeMeasVoiceFallback-r17", "true"}};
static const asn1::table::seq_opt    seq_table_sib5_s_g1_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(sib5_s, idle_mode_meas_voice_fallback_r17_present)}};
static const asn1::table::seq_field seq_table_sib5_s_g2_fields[] = {
    {offsetof(sib5_s, carrier_freq_list_eutra_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_item_list_codec<decltype(sib5_s::carrier_freq_list_eutra_v1800),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &carrier_freq_eutra_v1800_s::desc}};
static const asn1::table::field_name seq_table_sib5_s_g2_names[] = {{"carrierFreqListEUTRA-v1800", nullptr}};
static const asn1::table::seq_opt    seq_table_sib5_s_g2_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_sib5_s_groups[]   = {
    {{seq_table_sib5_s_g0_fields, 1, UINT64_C(0x0), seq_table_sib5_s_g0_opts, 1}, false, seq_table_sib5_s_g0_names},
    {{seq_table_sib5_s_g1_fields, 2, UINT64_C(0x0), seq_table_sib5_s_g1_opts, 2}, false, seq_table_sib5_s_g1_names},
    {{seq_table_sib5_s_g2_fields, 1, UINT64_C(0x0), seq_table_sib5_s_g2_opts, 1}, false, seq_table_sib5_s_g2_names}};
const asn1::table::seq_desc sib5_s::desc = {{seq_table_sib5_s_fields, 4, UINT64_C(0x2), seq_table_sib5_s_opts, 3},
                                            true,
                                            false,
                                            seq_table_sib5_s_groups,
                                            3,
                                            seq_table_sib5_s_names};

// SIB6 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib6_s_fields[] = {
    {offsetof(sib6_s, msg_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib6_s::msg_id), asn1::table::struct_json>::value},
    {offsetof(sib6_s, serial_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib6_s::serial_num), asn1::table::struct_json>::value},
    {offsetof(sib6_s, warning_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib6_s::warning_type), asn1::table::string_json>::value},
    {offsetof(sib6_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib6_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib6_s_names[] = {{"messageIdentifier", nullptr},
                                                                 {"serialNumber", nullptr},
                                                                 {"warningType", nullptr},
                                                                 {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib6_s_opts[]  = {{3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sib6_s::desc = {{seq_table_sib6_s_fields, 4, UINT64_C(0x7), seq_table_sib6_s_opts, 1},
                                            true,
                                            false,
                                            nullptr,
                                            0,
                                            seq_table_sib6_s_names};

// SIB7 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib7_s_fields[] = {
    {offsetof(sib7_s, msg_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib7_s::msg_id), asn1::table::struct_json>::value},
    {offsetof(sib7_s, serial_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib7_s::serial_num), asn1::table::struct_json>::value},
    {offsetof(sib7_s, warning_msg_segment_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sib7_s::warning_msg_segment_type)>(),
     decltype(sib7_s::warning_msg_segment_type)::has_ext,
     false,
     decltype(sib7_s::warning_msg_segment_type)::nof_types,
     decltype(sib7_s::warning_msg_segment_type)::nof_exts,
     nullptr,
     &sib7_s::warning_msg_segment_type_opts::desc},
    {offsetof(sib7_s, warning_msg_segment_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(sib7_s, warning_msg_segment),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib7_s::warning_msg_segment), asn1::table::string_json>::value},
    {offsetof(sib7_s, data_coding_scheme),
     (int32_t)offsetof(sib7_s, data_coding_scheme_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib7_s::data_coding_scheme), asn1::table::string_json>::value},
    {offsetof(sib7_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib7_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib7_s_names[] = {{"messageIdentifier", nullptr},
                                                                 {"serialNumber", nullptr},
                                                                 {"warningMessageSegmentType", nullptr},
                                                                 {"warningMessageSegmentNumber", nullptr},
                                                                 {"warningMessageSegment", nullptr},
                                                                 {"dataCodingScheme", nullptr},
                                                                 {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_sib7_s_opts[] = {{5, (int32_t)offsetof(sib7_s, data_coding_scheme_present)},
                                                             {6, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sib7_s::desc = {{seq_table_sib7_s_fields, 7, UINT64_C(0x1f), seq_table_sib7_s_opts, 2},
                                            true,
                                            false,
                                            nullptr,
                                            0,
                                            seq_table_sib7_s_names};

static const char*           enum_names_sib7_s_warning_msg_segment_type_e__values[] = {"notLastSegment", "lastSegment"};
const asn1::table::enum_desc sib7_s::warning_msg_segment_type_opts::desc            = {
    enum_names_sib7_s_warning_msg_segment_type_e__values,
    2,
    "sib7_s::warning_msg_segment_type_e_"};

// SIB8 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib8_s_fields[] = {
    {offsetof(sib8_s, msg_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib8_s::msg_id), asn1::table::struct_json>::value},
    {offsetof(sib8_s, serial_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib8_s::serial_num), asn1::table::struct_json>::value},
    {offsetof(sib8_s, warning_msg_segment_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sib8_s::warning_msg_segment_type)>(),
     decltype(sib8_s::warning_msg_segment_type)::has_ext,
     false,
     decltype(sib8_s::warning_msg_segment_type)::nof_types,
     decltype(sib8_s::warning_msg_segment_type)::nof_exts,
     nullptr,
     &sib8_s::warning_msg_segment_type_opts::desc},
    {offsetof(sib8_s, warning_msg_segment_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(sib8_s, warning_msg_segment),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib8_s::warning_msg_segment), asn1::table::string_json>::value},
    {offsetof(sib8_s, data_coding_scheme),
     (int32_t)offsetof(sib8_s, data_coding_scheme_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib8_s::data_coding_scheme), asn1::table::string_json>::value},
    {offsetof(sib8_s, warning_area_coordinates_segment),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib8_s::warning_area_coordinates_segment),
                                      asn1::table::string_json>::value},
    {offsetof(sib8_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib8_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib8_s_names[] = {{"messageIdentifier", nullptr},
                                                                 {"serialNumber", nullptr},
                                                                 {"warningMessageSegmentType", nullptr},
                                                                 {"warningMessageSegmentNumber", nullptr},
                                                                 {"warningMessageSegment", nullptr},
                                                                 {"dataCodingScheme", nullptr},
                                                                 {"warningAreaCoordinatesSegment", nullptr},
                                                                 {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_sib8_s_opts[] = {{5, (int32_t)offsetof(sib8_s, data_coding_scheme_present)},
                                                             {6, asn1::table::seq_field::list_presence},
                                                             {7, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sib8_s::desc = {{seq_table_sib8_s_fields, 8, UINT64_C(0x1f), seq_table_sib8_s_opts, 3},
                                            true,
                                            false,
                                            nullptr,
                                            0,
                                            seq_table_sib8_s_names};

static const char*           enum_names_sib8_s_warning_msg_segment_type_e__values[] = {"notLastSegment", "lastSegment"};
const asn1::table::enum_desc sib8_s::warning_msg_segment_type_opts::desc            = {
    enum_names_sib8_s_warning_msg_segment_type_e__values,
    2,
    "sib8_s::warning_msg_segment_type_e_"};

// ReferenceTime-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ref_time_r16_s_fields[] = {
    {offsetof(ref_time_r16_s, ref_days_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(72999),
     nullptr},
    {offsetof(ref_time_r16_s, ref_seconds_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(86399),
     nullptr},
    {offsetof(ref_time_r16_s, ref_milli_seconds_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(999),
     nullptr},
    {offsetof(ref_time_r16_s, ref_ten_nano_seconds_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(99999),
     nullptr}};
static const asn1::table::field_name seq_table_ref_time_r16_s_names[] = {{"refDays-r16", nullptr},
                                                                         {"refSeconds-r16", nullptr},
                                                                         {"refMilliSeconds-r16", nullptr},
                                                                         {"refTenNanoSeconds-r16", nullptr}};
const asn1::table::seq_desc ref_time_r16_s::desc = {{seq_table_ref_time_r16_s_fields, 4, UINT64_C(0xf), nullptr, 0},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0,
                                                    seq_table_ref_time_r16_s_names};

// ReferenceTimeInfo-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ref_time_info_r16_s_fields[] = {
    {offsetof(ref_time_info_r16_s, time_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ref_time_r16_s::desc},
    {offsetof(ref_time_info_r16_s, uncertainty_r16),
     (int32_t)offsetof(ref_time_info_r16_s, uncertainty_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(32767),
     nullptr},
    {0,
     (int32_t)offsetof(ref_time_info_r16_s, time_info_type_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ref_time_info_r16_s, ref_sfn_r16),
     (int32_t)offsetof(ref_time_info_r16_s, ref_sfn_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr}};
static const asn1::table::field_name seq_table_ref_time_info_r16_s_names[] = {{"time-r16", nullptr},
                                                                              {"uncertainty-r16", nullptr},
                                                                              {"timeInfoType-r16", "localClock"},
                                                                              {"referenceSFN-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_ref_time_info_r16_s_opts[]  = {
    {1, (int32_t)offsetof(ref_time_info_r16_s, uncertainty_r16_present)},
    {2, (int32_t)offsetof(ref_time_info_r16_s, time_info_type_r16_present)},
    {3, (int32_t)offsetof(ref_time_info_r16_s, ref_sfn_r16_present)}};
const asn1::table::seq_desc ref_time_info_r16_s::desc = {
    {seq_table_ref_time_info_r16_s_fields, 4, UINT64_C(0x1), seq_table_ref_time_info_r16_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_ref_time_info_r16_s_names};

// SIB9 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib9_s_time_info_s__fields[] = {
    {offsetof(sib9_s::time_info_s_, time_info_utc),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     false,
     INT64_C(0),
     UINT64_C(549755813887),
     nullptr},
    {offsetof(sib9_s::time_info_s_, day_light_saving_time),
     (int32_t)offsetof(sib9_s::time_info_s_, day_light_saving_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib9_s::time_info_s_::day_light_saving_time),
                                asn1::table::struct_json>::value},
    {offsetof(sib9_s::time_info_s_, leap_seconds),
     (int32_t)offsetof(sib9_s::time_info_s_, leap_seconds_present),
     asn1::table::field_op::int_i16,
     false,
     false,
     INT64_C(-127),
     UINT64_C(128),
     nullptr},
    {offsetof(sib9_s::time_info_s_, local_time_offset),
     (int32_t)offsetof(sib9_s::time_info_s_, local_time_offset_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-63),
     UINT64_C(64),
     nullptr}};
static const asn1::table::field_name seq_table_sib9_s_time_info_s__names[] = {{"timeInfoUTC", nullptr},
                                                                              {"dayLightSavingTime", nullptr},
                                                                              {"leapSeconds", nullptr},
                                                                              {"localTimeOffset", nullptr}};
static const asn1::table::seq_opt    seq_table_sib9_s_time_info_s__opts[]  = {
    {1, (int32_t)offsetof(sib9_s::time_info_s_, day_light_saving_time_present)},
    {2, (int32_t)offsetof(sib9_s::time_info_s_, leap_seconds_present)},
    {3, (int32_t)offsetof(sib9_s::time_info_s_, local_time_offset_present)}};
static const asn1::table::seq_desc seq_table_sib9_s_time_info_s_ = {
    {seq_table_sib9_s_time_info_s__fields, 4, UINT64_C(0x1), seq_table_sib9_s_time_info_s__opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_sib9_s_time_info_s__names};
static const asn1::table::seq_field seq_table_sib9_s_fields[] = {
    {offsetof(sib9_s, time_info),
     (int32_t)offsetof(sib9_s, time_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib9_s_time_info_s_},
    {offsetof(sib9_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib9_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib9_s_names[]     = {{"timeInfo", nullptr},
                                                                     {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib9_s_opts[]      = {{0, (int32_t)offsetof(sib9_s, time_info_present)},
                                                                     {1, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field  seq_table_sib9_s_g0_fields[] = {
    {offsetof(sib9_s, ref_time_info_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(sib9_s::ref_time_info_r16)>::value,
      &ref_time_info_r16_s::desc}};
static const asn1::table::field_name seq_table_sib9_s_g0_names[]  = {{"referenceTimeInfo-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_sib9_s_g0_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sib9_s_g1_fields[] = {
    {offsetof(sib9_s, event_id_tss_r18),
      (int32_t)offsetof(sib9_s, event_id_tss_r18_present),
      asn1::table::field_op::int_u8,
      false,
      false,
      INT64_C(0),
      UINT64_C(63),
      nullptr}};
static const asn1::table::field_name seq_table_sib9_s_g1_names[] = {{"eventID-TSS-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_sib9_s_g1_opts[]  = {
    {0, (int32_t)offsetof(sib9_s, event_id_tss_r18_present)}};
static const asn1::table::seq_group seq_table_sib9_s_groups[] = {
    {{seq_table_sib9_s_g0_fields, 1, UINT64_C(0x0), seq_table_sib9_s_g0_opts, 1}, false, seq_table_sib9_s_g0_names},
    {{seq_table_sib9_s_g1_fields, 1, UINT64_C(0x0), seq_table_sib9_s_g1_opts, 1}, false, seq_table_sib9_s_g1_names}};
const asn1::table::seq_desc sib9_s::desc = {{seq_table_sib9_s_fields, 2, UINT64_C(0x0), seq_table_sib9_s_opts, 2},
                                            true,
                                            false,
                                            seq_table_sib9_s_groups,
                                            2,
                                            seq_table_sib9_s_names};

// HRNN-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_hrnn_r16_s_fields[] = {
    {offsetof(hrnn_r16_s, hrnn_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(hrnn_r16_s::hrnn_r16), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_hrnn_r16_s_names[] = {{"hrnn-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_hrnn_r16_s_opts[]  = {{0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          hrnn_r16_s::desc             = {
    {seq_table_hrnn_r16_s_fields, 1, UINT64_C(0x0), seq_table_hrnn_r16_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_hrnn_r16_s_names};

// SIB10-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib10_r16_s_fields[] = {
    {offsetof(sib10_r16_s, hrnn_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::item_list_codec<decltype(sib10_r16_s::hrnn_list_r16), asn1::table::sequence_item_ops, false, true>::
         value,
     &hrnn_r16_s::desc},
    {offsetof(sib10_r16_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib10_r16_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib10_r16_s_names[] = {{"hrnn-List-r16", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib10_r16_s_opts[]  = {{0, asn1::table::seq_field::list_presence},
                                                                      {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          sib10_r16_s::desc             = {
    {seq_table_sib10_r16_s_fields, 2, UINT64_C(0x0), seq_table_sib10_r16_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sib10_r16_s_names};

// BeamMeasConfigIdle-NR-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_beam_meas_cfg_idle_nr_r16_s_fields[] = {
    {offsetof(beam_meas_cfg_idle_nr_r16_s, report_quant_rs_idxes_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(beam_meas_cfg_idle_nr_r16_s::report_quant_rs_idxes_r16)>(),
     decltype(beam_meas_cfg_idle_nr_r16_s::report_quant_rs_idxes_r16)::has_ext,
     false,
     decltype(beam_meas_cfg_idle_nr_r16_s::report_quant_rs_idxes_r16)::nof_types,
     decltype(beam_meas_cfg_idle_nr_r16_s::report_quant_rs_idxes_r16)::nof_exts,
     nullptr,
     &beam_meas_cfg_idle_nr_r16_s::report_quant_rs_idxes_r16_opts::desc},
    {offsetof(beam_meas_cfg_idle_nr_r16_s, max_nrof_rs_idxes_to_report_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(beam_meas_cfg_idle_nr_r16_s, include_beam_meass_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_beam_meas_cfg_idle_nr_r16_s_names[] = {
    {"reportQuantityRS-Indexes-r16", nullptr},
    {"maxNrofRS-IndexesToReport-r16", nullptr},
    {"includeBeamMeasurements-r16", nullptr}};
const asn1::table::seq_desc beam_meas_cfg_idle_nr_r16_s::desc = {
    {seq_table_beam_meas_cfg_idle_nr_r16_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_beam_meas_cfg_idle_nr_r16_s_names};

static const char*           enum_names_beam_meas_cfg_idle_nr_r16_s_report_quant_rs_idxes_r16_e__values[] = {"rsrp",
                                                                                                             "rsrq",
                                                                                                             "both"};
const asn1::table::enum_desc beam_meas_cfg_idle_nr_r16_s::report_quant_rs_idxes_r16_opts::desc            = {
    enum_names_beam_meas_cfg_idle_nr_r16_s_report_quant_rs_idxes_r16_e__values,
    3,
    "beam_meas_cfg_idle_nr_r16_s::report_quant_rs_idxes_r16_e_"};

// MeasIdleCarrierNR-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_idle_carrier_nr_r16_s_quality_thres_r16_s__fields[] = {
    {offsetof(meas_idle_carrier_nr_r16_s::quality_thres_r16_s_, idle_rsrp_thres_nr_r16),
     (int32_t)offsetof(meas_idle_carrier_nr_r16_s::quality_thres_r16_s_, idle_rsrp_thres_nr_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(meas_idle_carrier_nr_r16_s::quality_thres_r16_s_, idle_rsrq_thres_nr_r16),
     (int32_t)offsetof(meas_idle_carrier_nr_r16_s::quality_thres_r16_s_, idle_rsrq_thres_nr_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr}};
static const asn1::table::field_name seq_table_meas_idle_carrier_nr_r16_s_quality_thres_r16_s__names[] = {
    {"idleRSRP-Threshold-NR-r16", nullptr},
    {"idleRSRQ-Threshold-NR-r16", nullptr}};
static const asn1::table::seq_opt seq_table_meas_idle_carrier_nr_r16_s_quality_thres_r16_s__opts[] = {
    {0, (int32_t)offsetof(meas_idle_carrier_nr_r16_s::quality_thres_r16_s_, idle_rsrp_thres_nr_r16_present)},
    {1, (int32_t)offsetof(meas_idle_carrier_nr_r16_s::quality_thres_r16_s_, idle_rsrq_thres_nr_r16_present)}};
static const asn1::table::seq_desc seq_table_meas_idle_carrier_nr_r16_s_quality_thres_r16_s_ = {
    {seq_table_meas_idle_carrier_nr_r16_s_quality_thres_r16_s__fields,
     2,
     UINT64_C(0x0),
     seq_table_meas_idle_carrier_nr_r16_s_quality_thres_r16_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_idle_carrier_nr_r16_s_quality_thres_r16_s__names};
static const asn1::table::seq_field seq_table_meas_idle_carrier_nr_r16_s_ssb_meas_cfg_r16_s__fields[] = {
    {offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, nrof_ss_blocks_to_average_r16),
     (int32_t)offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, nrof_ss_blocks_to_average_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(2),
     UINT64_C(16),
     nullptr},
    {offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, abs_thresh_ss_blocks_consolidation_r16),
     (int32_t)offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, abs_thresh_ss_blocks_consolidation_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &thres_nr_s::desc},
    {offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, smtc_r16),
     (int32_t)offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, smtc_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_mtc_s::desc},
    {offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, ssb_to_measure_r16),
     (int32_t)offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, ssb_to_measure_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_to_measure_c::desc},
    {offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, derive_ssb_idx_from_cell_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, ss_rssi_meas_r16),
     (int32_t)offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, ss_rssi_meas_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ss_rssi_meas_s::desc}};
static const asn1::table::field_name seq_table_meas_idle_carrier_nr_r16_s_ssb_meas_cfg_r16_s__names[] = {
    {"nrofSS-BlocksToAverage-r16", nullptr},
    {"absThreshSS-BlocksConsolidation-r16", nullptr},
    {"smtc-r16", nullptr},
    {"ssb-ToMeasure-r16", nullptr},
    {"deriveSSB-IndexFromCell-r16", nullptr},
    {"ss-RSSI-Measurement-r16", nullptr}};
static const asn1::table::seq_opt seq_table_meas_idle_carrier_nr_r16_s_ssb_meas_cfg_r16_s__opts[] = {
    {0, (int32_t)offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, nrof_ss_blocks_to_average_r16_present)},
    {1,
     (int32_t)offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_,
                       abs_thresh_ss_blocks_consolidation_r16_present)},
    {2, (int32_t)offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, smtc_r16_present)},
    {3, (int32_t)offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, ssb_to_measure_r16_present)},
    {5, (int32_t)offsetof(meas_idle_carrier_nr_r16_s::ssb_meas_cfg_r16_s_, ss_rssi_meas_r16_present)}};
static const asn1::table::seq_desc seq_table_meas_idle_carrier_nr_r16_s_ssb_meas_cfg_r16_s_ = {
    {seq_table_meas_idle_carrier_nr_r16_s_ssb_meas_cfg_r16_s__fields,
     6,
     UINT64_C(0x10),
     seq_table_meas_idle_carrier_nr_r16_s_ssb_meas_cfg_r16_s__opts,
     5},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_idle_carrier_nr_r16_s_ssb_meas_cfg_r16_s__names};
static const asn1::table::seq_field seq_table_meas_idle_carrier_nr_r16_s_fields[] = {
    {offsetof(meas_idle_carrier_nr_r16_s, carrier_freq_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(meas_idle_carrier_nr_r16_s, ssb_subcarrier_spacing_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(meas_idle_carrier_nr_r16_s::ssb_subcarrier_spacing_r16)>(),
     decltype(meas_idle_carrier_nr_r16_s::ssb_subcarrier_spacing_r16)::has_ext,
     false,
     decltype(meas_idle_carrier_nr_r16_s::ssb_subcarrier_spacing_r16)::nof_types,
     decltype(meas_idle_carrier_nr_r16_s::ssb_subcarrier_spacing_r16)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(meas_idle_carrier_nr_r16_s, freq_band_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<decltype(meas_idle_carrier_nr_r16_s::freq_band_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(1024), false, false>>::value},
    {offsetof(meas_idle_carrier_nr_r16_s, meas_cell_list_nr_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(meas_idle_carrier_nr_r16_s::meas_cell_list_nr_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pci_range_s::desc},
    {offsetof(meas_idle_carrier_nr_r16_s, report_quantities_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(meas_idle_carrier_nr_r16_s::report_quantities_r16)>(),
     decltype(meas_idle_carrier_nr_r16_s::report_quantities_r16)::has_ext,
     false,
     decltype(meas_idle_carrier_nr_r16_s::report_quantities_r16)::nof_types,
     decltype(meas_idle_carrier_nr_r16_s::report_quantities_r16)::nof_exts,
     nullptr,
     &meas_idle_carrier_nr_r16_s::report_quantities_r16_opts::desc},
    {offsetof(meas_idle_carrier_nr_r16_s, quality_thres_r16),
     (int32_t)offsetof(meas_idle_carrier_nr_r16_s, quality_thres_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_meas_idle_carrier_nr_r16_s_quality_thres_r16_s_},
    {offsetof(meas_idle_carrier_nr_r16_s, ssb_meas_cfg_r16),
     (int32_t)offsetof(meas_idle_carrier_nr_r16_s, ssb_meas_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_meas_idle_carrier_nr_r16_s_ssb_meas_cfg_r16_s_},
    {offsetof(meas_idle_carrier_nr_r16_s, beam_meas_cfg_idle_r16),
     (int32_t)offsetof(meas_idle_carrier_nr_r16_s, beam_meas_cfg_idle_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &beam_meas_cfg_idle_nr_r16_s::desc}};
static const asn1::table::field_name seq_table_meas_idle_carrier_nr_r16_s_names[] = {
    {"carrierFreq-r16", nullptr},
    {"ssbSubcarrierSpacing-r16", nullptr},
    {"frequencyBandList", nullptr},
    {"measCellListNR-r16", nullptr},
    {"reportQuantities-r16", nullptr},
    {"qualityThreshold-r16", nullptr},
    {"ssb-MeasConfig-r16", nullptr},
    {"beamMeasConfigIdle-r16", nullptr}};
static const asn1::table::seq_opt seq_table_meas_idle_carrier_nr_r16_s_opts[] = {
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(meas_idle_carrier_nr_r16_s, quality_thres_r16_present)},
    {6, (int32_t)offsetof(meas_idle_carrier_nr_r16_s, ssb_meas_cfg_r16_present)},
    {7, (int32_t)offsetof(meas_idle_carrier_nr_r16_s, beam_meas_cfg_idle_r16_present)}};
const asn1::table::seq_desc meas_idle_carrier_nr_r16_s::desc = {
    {seq_table_meas_idle_carrier_nr_r16_s_fields, 8, UINT64_C(0x13), seq_table_meas_idle_carrier_nr_r16_s_opts, 5},
    true,
    false,
    nullptr,
    0,
    seq_table_meas_idle_carrier_nr_r16_s_names};

static const char* enum_names_meas_idle_carrier_nr_r16_s_report_quantities_r16_e__values[] = {"rsrp", "rsrq", "both"};
const asn1::table::enum_desc meas_idle_carrier_nr_r16_s::report_quantities_r16_opts::desc  = {
    enum_names_meas_idle_carrier_nr_r16_s_report_quantities_r16_e__values,
    3,
    "meas_idle_carrier_nr_r16_s::report_quantities_r16_e_"};

// MeasIdleCarrierEUTRA-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_idle_carrier_eutra_r16_s_quality_thres_eutra_r16_s__fields[] = {
    {offsetof(meas_idle_carrier_eutra_r16_s::quality_thres_eutra_r16_s_, idle_rsrp_thres_eutra_r16),
     (int32_t)offsetof(meas_idle_carrier_eutra_r16_s::quality_thres_eutra_r16_s_, idle_rsrp_thres_eutra_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(97),
     nullptr},
    {offsetof(meas_idle_carrier_eutra_r16_s::quality_thres_eutra_r16_s_, idle_rsrq_thres_eutra_r16),
     (int32_t)offsetof(meas_idle_carrier_eutra_r16_s::quality_thres_eutra_r16_s_, idle_rsrq_thres_eutra_r16_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(46),
     nullptr}};
static const asn1::table::field_name seq_table_meas_idle_carrier_eutra_r16_s_quality_thres_eutra_r16_s__names[] = {
    {"idleRSRP-Threshold-EUTRA-r16", nullptr},
    {"idleRSRQ-Threshold-EUTRA-r16", nullptr}};
static const asn1::table::seq_opt seq_table_meas_idle_carrier_eutra_r16_s_quality_thres_eutra_r16_s__opts[] = {
    {0,
     (int32_t)offsetof(meas_idle_carrier_eutra_r16_s::quality_thres_eutra_r16_s_, idle_rsrp_thres_eutra_r16_present)},
    {1,
     (int32_t)offsetof(meas_idle_carrier_eutra_r16_s::quality_thres_eutra_r16_s_, idle_rsrq_thres_eutra_r16_present)}};
static const asn1::table::seq_desc seq_table_meas_idle_carrier_eutra_r16_s_quality_thres_eutra_r16_s_ = {
    {seq_table_meas_idle_carrier_eutra_r16_s_quality_thres_eutra_r16_s__fields,
     2,
     UINT64_C(0x0),
     seq_table_meas_idle_carrier_eutra_r16_s_quality_thres_eutra_r16_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_idle_carrier_eutra_r16_s_quality_thres_eutra_r16_s__names};
static const asn1::table::seq_field seq_table_meas_idle_carrier_eutra_r16_s_fields[] = {
    {offsetof(meas_idle_carrier_eutra_r16_s, carrier_freq_eutra_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(262143),
     nullptr},
    {offsetof(meas_idle_carrier_eutra_r16_s, allowed_meas_bw_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(meas_idle_carrier_eutra_r16_s::allowed_meas_bw_r16)>(),
     decltype(meas_idle_carrier_eutra_r16_s::allowed_meas_bw_r16)::has_ext,
     false,
     decltype(meas_idle_carrier_eutra_r16_s::allowed_meas_bw_r16)::nof_types,
     decltype(meas_idle_carrier_eutra_r16_s::allowed_meas_bw_r16)::nof_exts,
     nullptr,
     &eutra_allowed_meas_bw_opts::desc},
    {offsetof(meas_idle_carrier_eutra_r16_s, meas_cell_list_eutra_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(meas_idle_carrier_eutra_r16_s::meas_cell_list_eutra_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &eutra_pci_range_s::desc},
    {offsetof(meas_idle_carrier_eutra_r16_s, report_quantities_eutra_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(meas_idle_carrier_eutra_r16_s::report_quantities_eutra_r16)>(),
     decltype(meas_idle_carrier_eutra_r16_s::report_quantities_eutra_r16)::has_ext,
     false,
     decltype(meas_idle_carrier_eutra_r16_s::report_quantities_eutra_r16)::nof_types,
     decltype(meas_idle_carrier_eutra_r16_s::report_quantities_eutra_r16)::nof_exts,
     nullptr,
     &meas_idle_carrier_eutra_r16_s::report_quantities_eutra_r16_opts::desc},
    {offsetof(meas_idle_carrier_eutra_r16_s, quality_thres_eutra_r16),
     (int32_t)offsetof(meas_idle_carrier_eutra_r16_s, quality_thres_eutra_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_meas_idle_carrier_eutra_r16_s_quality_thres_eutra_r16_s_}};
static const asn1::table::field_name seq_table_meas_idle_carrier_eutra_r16_s_names[] = {
    {"carrierFreqEUTRA-r16", nullptr},
    {"allowedMeasBandwidth-r16", nullptr},
    {"measCellListEUTRA-r16", nullptr},
    {"reportQuantitiesEUTRA-r16", nullptr},
    {"qualityThresholdEUTRA-r16", nullptr}};
static const asn1::table::seq_opt seq_table_meas_idle_carrier_eutra_r16_s_opts[] = {
    {2, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(meas_idle_carrier_eutra_r16_s, quality_thres_eutra_r16_present)}};
const asn1::table::seq_desc meas_idle_carrier_eutra_r16_s::desc = {
    {seq_table_meas_idle_carrier_eutra_r16_s_fields, 5, UINT64_C(0xb), seq_table_meas_idle_carrier_eutra_r16_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_meas_idle_carrier_eutra_r16_s_names};

static const char*           enum_names_meas_idle_carrier_eutra_r16_s_report_quantities_eutra_r16_e__values[] = {"rsrp",
                                                                                                                 "rsrq",
                                                                                                                 "both"};
const asn1::table::enum_desc meas_idle_carrier_eutra_r16_s::report_quantities_eutra_r16_opts::desc            = {
    enum_names_meas_idle_carrier_eutra_r16_s_report_quantities_eutra_r16_e__values,
    3,
    "meas_idle_carrier_eutra_r16_s::report_quantities_eutra_r16_e_"};

// MeasReselectionCarrierNR-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_resel_carrier_nr_r18_s_fields[] = {
    {offsetof(meas_resel_carrier_nr_r18_s, carrier_freq_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr}};
static const asn1::table::field_name seq_table_meas_resel_carrier_nr_r18_s_names[] = {{"carrierFreq-r18", nullptr}};
const asn1::table::seq_desc          meas_resel_carrier_nr_r18_s::desc             = {
    {seq_table_meas_resel_carrier_nr_r18_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_meas_resel_carrier_nr_r18_s_names};

// MeasurementValidityDuration-r18 ::= ENUMERATED
static const char* enum_names_meas_validity_dur_r18_e_values[] =
    {"s5", "s10", "s20", "s50", "s100", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc meas_validity_dur_r18_opts::desc = {enum_names_meas_validity_dur_r18_e_values,
                                                                 8,
                                                                 "meas_validity_dur_r18_e"};
uint8_t                      meas_validity_dur_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 50, 100};
  return map_enum_number(numbers, 5, value, "meas_validity_dur_r18_e");
}

// MeasIdleConfigSIB-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_idle_cfg_sib_r16_s_fields[] = {
    {offsetof(meas_idle_cfg_sib_r16_s, meas_idle_carrier_list_nr_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(meas_idle_cfg_sib_r16_s::meas_idle_carrier_list_nr_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_idle_carrier_nr_r16_s::desc},
    {offsetof(meas_idle_cfg_sib_r16_s, meas_idle_carrier_list_eutra_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(meas_idle_cfg_sib_r16_s::meas_idle_carrier_list_eutra_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_idle_carrier_eutra_r16_s::desc}};
static const asn1::table::field_name seq_table_meas_idle_cfg_sib_r16_s_names[] = {
    {"measIdleCarrierListNR-r16", nullptr},
    {"measIdleCarrierListEUTRA-r16", nullptr}};
static const asn1::table::seq_opt seq_table_meas_idle_cfg_sib_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_meas_idle_cfg_sib_r16_s_g0_fields[] = {
    {offsetof(meas_idle_cfg_sib_r16_s, meas_idle_carrier_list_nr_less_than5_m_hz_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_item_list_codec<decltype(meas_idle_cfg_sib_r16_s::meas_idle_carrier_list_nr_less_than5_m_hz_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &meas_idle_carrier_nr_r16_s::desc},
    {offsetof(meas_idle_cfg_sib_r16_s, meas_resel_carrier_list_nr_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_item_list_codec<decltype(meas_idle_cfg_sib_r16_s::meas_resel_carrier_list_nr_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &meas_resel_carrier_nr_r18_s::desc},
    {offsetof(meas_idle_cfg_sib_r16_s, meas_resel_carrier_list_nr_less_than5_m_hz_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_item_list_codec<
         decltype(meas_idle_cfg_sib_r16_s::meas_resel_carrier_list_nr_less_than5_m_hz_r18),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &meas_resel_carrier_nr_r18_s::desc},
    {offsetof(meas_idle_cfg_sib_r16_s, meas_idle_validity_dur_r18),
     (int32_t)offsetof(meas_idle_cfg_sib_r16_s, meas_idle_validity_dur_r18_present),
     asn1::table::enum_op<decltype(meas_idle_cfg_sib_r16_s::meas_idle_validity_dur_r18)>(),
     decltype(meas_idle_cfg_sib_r16_s::meas_idle_validity_dur_r18)::has_ext,
     false,
     decltype(meas_idle_cfg_sib_r16_s::meas_idle_validity_dur_r18)::nof_types,
     decltype(meas_idle_cfg_sib_r16_s::meas_idle_validity_dur_r18)::nof_exts,
     nullptr,
     &meas_validity_dur_r18_opts::desc},
    {offsetof(meas_idle_cfg_sib_r16_s, meas_resel_validity_dur_r18),
     (int32_t)offsetof(meas_idle_cfg_sib_r16_s, meas_resel_validity_dur_r18_present),
     asn1::table::enum_op<decltype(meas_idle_cfg_sib_r16_s::meas_resel_validity_dur_r18)>(),
     decltype(meas_idle_cfg_sib_r16_s::meas_resel_validity_dur_r18)::has_ext,
     false,
     decltype(meas_idle_cfg_sib_r16_s::meas_resel_validity_dur_r18)::nof_types,
     decltype(meas_idle_cfg_sib_r16_s::meas_resel_validity_dur_r18)::nof_exts,
     nullptr,
     &meas_validity_dur_r18_opts::desc}};
static const asn1::table::field_name seq_table_meas_idle_cfg_sib_r16_s_g0_names[] = {
    {"measIdleCarrierListNR-LessThan5MHz-r18", nullptr},
    {"measReselectionCarrierListNR-r18", nullptr},
    {"measReselectionCarrierListNR-LessThan5MHz-r18", nullptr},
    {"measIdleValidityDuration-r18", nullptr},
    {"measReselectionValidityDuration-r18", nullptr}};
static const asn1::table::seq_opt seq_table_meas_idle_cfg_sib_r16_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, (int32_t)offsetof(meas_idle_cfg_sib_r16_s, meas_idle_validity_dur_r18_present)},
    {4, (int32_t)offsetof(meas_idle_cfg_sib_r16_s, meas_resel_validity_dur_r18_present)}};
static const asn1::table::seq_group seq_table_meas_idle_cfg_sib_r16_s_groups[] = {
    {{seq_table_meas_idle_cfg_sib_r16_s_g0_fields, 5, UINT64_C(0x0), seq_table_meas_idle_cfg_sib_r16_s_g0_opts, 5},
     false,
     seq_table_meas_idle_cfg_sib_r16_s_g0_names}};
const asn1::table::seq_desc meas_idle_cfg_sib_r16_s::desc = {
    {seq_table_meas_idle_cfg_sib_r16_s_fields, 2, UINT64_C(0x0), seq_table_meas_idle_cfg_sib_r16_s_opts, 2},
    true,
    false,
    seq_table_meas_idle_cfg_sib_r16_s_groups,
    1,
    seq_table_meas_idle_cfg_sib_r16_s_names};

// SIB11-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib11_r16_s_fields[] = {
    {offsetof(sib11_r16_s, meas_idle_cfg_sib_r16),
     (int32_t)offsetof(sib11_r16_s, meas_idle_cfg_sib_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_idle_cfg_sib_r16_s::desc},
    {offsetof(sib11_r16_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib11_r16_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib11_r16_s_names[] = {{"measIdleConfigSIB-r16", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib11_r16_s_opts[]  = {
    {0, (int32_t)offsetof(sib11_r16_s, meas_idle_cfg_sib_r16_present)},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sib11_r16_s::desc = {
    {seq_table_sib11_r16_s_fields, 2, UINT64_C(0x0), seq_table_sib11_r16_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sib11_r16_s_names};

// SIB12-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib12_r16_s_fields[] = {
    {offsetof(sib12_r16_s, segment_num_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(sib12_r16_s, segment_type_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sib12_r16_s::segment_type_r16)>(),
     decltype(sib12_r16_s::segment_type_r16)::has_ext,
     false,
     decltype(sib12_r16_s::segment_type_r16)::nof_types,
     decltype(sib12_r16_s::segment_type_r16)::nof_exts,
     nullptr,
     &sib12_r16_s::segment_type_r16_opts::desc},
    {offsetof(sib12_r16_s, segment_container_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib12_r16_s::segment_container_r16), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib12_r16_s_names[] = {{"segmentNumber-r16", nullptr},
                                                                      {"segmentType-r16", nullptr},
                                                                      {"segmentContainer-r16", nullptr}};
const asn1::table::seq_desc          sib12_r16_s::desc = {{seq_table_sib12_r16_s_fields, 3, UINT64_C(0x7), nullptr, 0},
                                                          false,
                                                          false,
                                                          nullptr,
                                                          0,
                                                          seq_table_sib12_r16_s_names};

static const char*           enum_names_sib12_r16_s_segment_type_r16_e__values[] = {"notLastSegment", "lastSegment"};
const asn1::table::enum_desc sib12_r16_s::segment_type_r16_opts::desc            = {
    enum_names_sib12_r16_s_segment_type_r16_e__values,
    2,
    "sib12_r16_s::segment_type_r16_e_"};

// SIB13-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib13_r16_s_fields[] = {
    {offsetof(sib13_r16_s, sl_v2x_cfg_common_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib13_r16_s::sl_v2x_cfg_common_r16), asn1::table::string_json>::value},
    {offsetof(sib13_r16_s, dummy),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib13_r16_s::dummy), asn1::table::string_json>::value},
    {offsetof(sib13_r16_s, tdd_cfg_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib13_r16_s::tdd_cfg_r16), asn1::table::string_json>::value},
    {offsetof(sib13_r16_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib13_r16_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib13_r16_s_names[] = {{"sl-V2X-ConfigCommon-r16", nullptr},
                                                                      {"dummy", nullptr},
                                                                      {"tdd-Config-r16", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib13_r16_s_opts[]  = {{3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          sib13_r16_s::desc             = {
    {seq_table_sib13_r16_s_fields, 4, UINT64_C(0x7), seq_table_sib13_r16_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_sib13_r16_s_names};

// SIB14-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib14_r16_s_fields[] = {
    {offsetof(sib14_r16_s, sl_v2x_cfg_common_ext_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib14_r16_s::sl_v2x_cfg_common_ext_r16), asn1::table::string_json>::value},
    {offsetof(sib14_r16_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib14_r16_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib14_r16_s_names[] = {{"sl-V2X-ConfigCommonExt-r16", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib14_r16_s_opts[]  = {{1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          sib14_r16_s::desc             = {
    {seq_table_sib14_r16_s_fields, 2, UINT64_C(0x1), seq_table_sib14_r16_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_sib14_r16_s_names};

// SIB15-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib15_r17_s_fields[] = {
    {offsetof(sib15_r17_s, common_plmns_with_disaster_condition_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::item_list_codec<decltype(sib15_r17_s::common_plmns_with_disaster_condition_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &plmn_id_s::desc},
    {offsetof(sib15_r17_s, applicable_disaster_info_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::item_list_codec<decltype(sib15_r17_s::applicable_disaster_info_list_r17),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &applicable_disaster_info_r17_c::desc},
    {offsetof(sib15_r17_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib15_r17_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib15_r17_s_names[] = {{"commonPLMNsWithDisasterCondition-r17", nullptr},
                                                                      {"applicableDisasterInfoList-r17", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib15_r17_s_opts[]  = {{0, asn1::table::seq_field::list_presence},
                                                                      {1, asn1::table::seq_field::list_presence},
                                                                      {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          sib15_r17_s::desc             = {
    {seq_table_sib15_r17_s_fields, 3, UINT64_C(0x0), seq_table_sib15_r17_s_opts, 3},
    true,
    false,
    nullptr,
    0,
    seq_table_sib15_r17_s_names};

// NSAG-IdentityInfo-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_nsag_id_info_r17_s_fields[] = {
    {offsetof(nsag_id_info_r17_s, nsag_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nsag_id_info_r17_s::nsag_id_r17), asn1::table::struct_json>::value},
    {offsetof(nsag_id_info_r17_s, tac_r17),
     (int32_t)offsetof(nsag_id_info_r17_s, tac_r17_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nsag_id_info_r17_s::tac_r17), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_nsag_id_info_r17_s_names[] = {{"nsag-ID-r17", nullptr},
                                                                             {"trackingAreaCode-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_nsag_id_info_r17_s_opts[]  = {
    {1, (int32_t)offsetof(nsag_id_info_r17_s, tac_r17_present)}};
const asn1::table::seq_desc nsag_id_info_r17_s::desc = {
    {seq_table_nsag_id_info_r17_s_fields, 2, UINT64_C(0x1), seq_table_nsag_id_info_r17_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_nsag_id_info_r17_s_names};

// SliceInfo-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_slice_info_r17_s_fields[] = {
    {offsetof(slice_info_r17_s, nsag_id_info_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nsag_id_info_r17_s::desc},
    {offsetof(slice_info_r17_s, nsag_cell_resel_prio_r17),
     (int32_t)offsetof(slice_info_r17_s, nsag_cell_resel_prio_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(slice_info_r17_s, nsag_cell_resel_sub_prio_r17),
     (int32_t)offsetof(slice_info_r17_s, nsag_cell_resel_sub_prio_r17_present),
     asn1::table::enum_op<decltype(slice_info_r17_s::nsag_cell_resel_sub_prio_r17)>(),
     decltype(slice_info_r17_s::nsag_cell_resel_sub_prio_r17)::has_ext,
     false,
     decltype(slice_info_r17_s::nsag_cell_resel_sub_prio_r17)::nof_types,
     decltype(slice_info_r17_s::nsag_cell_resel_sub_prio_r17)::nof_exts,
     nullptr,
     &cell_resel_sub_prio_opts::desc},
    {offsetof(slice_info_r17_s, slice_cell_list_nr_r17),
     (int32_t)offsetof(slice_info_r17_s, slice_cell_list_nr_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &slice_info_r17_s::slice_cell_list_nr_r17_c_::desc}};
static const asn1::table::field_name seq_table_slice_info_r17_s_names[] = {
    {"nsag-IdentityInfo-r17", nullptr},
    {"nsag-CellReselectionPriority-r17", nullptr},
    {"nsag-CellReselectionSubPriority-r17", nullptr},
    {"sliceCellListNR-r17", nullptr}};
static const asn1::table::seq_opt seq_table_slice_info_r17_s_opts[] = {
    {1, (int32_t)offsetof(slice_info_r17_s, nsag_cell_resel_prio_r17_present)},
    {2, (int32_t)offsetof(slice_info_r17_s, nsag_cell_resel_sub_prio_r17_present)},
    {3, (int32_t)offsetof(slice_info_r17_s, slice_cell_list_nr_r17_present)}};
const asn1::table::seq_desc slice_info_r17_s::desc = {
    {seq_table_slice_info_r17_s_fields, 4, UINT64_C(0x1), seq_table_slice_info_r17_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_slice_info_r17_s_names};

static const asn1::table::storage_ops* const choice_storage_slice_info_r17_s_slice_cell_list_nr_r17_c__alternatives[] =
    {&asn1::table::alternative_ops<slice_cell_list_nr_r17_l, false>::value,
     &asn1::table::alternative_ops<slice_cell_list_nr_r17_l, false>::value};
const asn1::table::choice_storage_desc slice_info_r17_s::slice_cell_list_nr_r17_c_::storage_desc = {
    choice_storage_slice_info_r17_s_slice_cell_list_nr_r17_c__alternatives,
    2,
    "slice_info_r17_s::slice_cell_list_nr_r17_c_"};

slice_cell_list_nr_r17_l& slice_info_r17_s::slice_cell_list_nr_r17_c_::set_slice_allowed_cell_list_nr_r17()
{
  set(types::slice_allowed_cell_list_nr_r17);
  return c.get<slice_cell_list_nr_r17_l>();
}
slice_cell_list_nr_r17_l& slice_info_r17_s::slice_cell_list_nr_r17_c_::set_slice_excluded_cell_list_nr_r17()
{
  set(types::slice_excluded_cell_list_nr_r17);
  return c.get<slice_cell_list_nr_r17_l>();
}
static const asn1::table::seq_field choice_table_slice_info_r17_s_slice_cell_list_nr_r17_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<slice_cell_list_nr_r17_l, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<slice_cell_list_nr_r17_l, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_slice_info_r17_s_slice_cell_list_nr_r17_c__names[] = {
    {"sliceAllowedCellListNR-r17", nullptr},
    {"sliceExcludedCellListNR-r17", nullptr}};
const asn1::table::choice_desc slice_info_r17_s::slice_cell_list_nr_r17_c_::desc = {
    asn1::table::enum_int_type<slice_info_r17_s::slice_cell_list_nr_r17_c_::types>(),
    slice_info_r17_s::slice_cell_list_nr_r17_c_::types::nof_types,
    slice_info_r17_s::slice_cell_list_nr_r17_c_::types::nof_exts,
    slice_info_r17_s::slice_cell_list_nr_r17_c_::types::has_ext,
    false,
    choice_table_slice_info_r17_s_slice_cell_list_nr_r17_c__alternatives,
    nullptr,
    choice_table_slice_info_r17_s_slice_cell_list_nr_r17_c__names,
    "slice_info_r17_s::slice_cell_list_nr_r17_c_",
    offsetof(slice_info_r17_s::slice_cell_list_nr_r17_c_, c.type),
    offsetof(slice_info_r17_s::slice_cell_list_nr_r17_c_, c.value),
    false,
    &slice_info_r17_s::slice_cell_list_nr_r17_c_::storage_desc};

static const char* enum_names_slice_info_r17_s_slice_cell_list_nr_r17_c__types_values[] = {
    "sliceAllowedCellListNR-r17",
    "sliceExcludedCellListNR-r17"};
const asn1::table::enum_desc slice_info_r17_s::slice_cell_list_nr_r17_c_::types_opts::desc = {
    enum_names_slice_info_r17_s_slice_cell_list_nr_r17_c__types_values,
    2,
    "slice_info_r17_s::slice_cell_list_nr_r17_c_::types"};

// FreqPrioritySlicing-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_freq_prio_slicing_r17_s_fields[] = {
    {offsetof(freq_prio_slicing_r17_s, dl_implicit_carrier_freq_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(8),
     nullptr},
    {offsetof(freq_prio_slicing_r17_s, slice_info_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(freq_prio_slicing_r17_s::slice_info_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &slice_info_r17_s::desc}};
static const asn1::table::field_name seq_table_freq_prio_slicing_r17_s_names[] = {
    {"dl-ImplicitCarrierFreq-r17", nullptr},
    {"sliceInfoList-r17", nullptr}};
static const asn1::table::seq_opt seq_table_freq_prio_slicing_r17_s_opts[] = {
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc freq_prio_slicing_r17_s::desc = {
    {seq_table_freq_prio_slicing_r17_s_fields, 2, UINT64_C(0x1), seq_table_freq_prio_slicing_r17_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_freq_prio_slicing_r17_s_names};

// SIB16-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib16_r17_s_fields[] = {
    {offsetof(sib16_r17_s, freq_prio_list_slicing_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     9,
     &asn1::table::item_list_codec<decltype(sib16_r17_s::freq_prio_list_slicing_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &freq_prio_slicing_r17_s::desc},
    {offsetof(sib16_r17_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib16_r17_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib16_r17_s_names[] = {{"freqPriorityListSlicing-r17", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib16_r17_s_opts[]  = {{0, asn1::table::seq_field::list_presence},
                                                                      {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          sib16_r17_s::desc             = {
    {seq_table_sib16_r17_s_fields, 2, UINT64_C(0x0), seq_table_sib16_r17_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sib16_r17_s_names};

// SIB17-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib17_r17_s_fields[] = {
    {offsetof(sib17_r17_s, segment_num_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(sib17_r17_s, segment_type_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sib17_r17_s::segment_type_r17)>(),
     decltype(sib17_r17_s::segment_type_r17)::has_ext,
     false,
     decltype(sib17_r17_s::segment_type_r17)::nof_types,
     decltype(sib17_r17_s::segment_type_r17)::nof_exts,
     nullptr,
     &sib17_r17_s::segment_type_r17_opts::desc},
    {offsetof(sib17_r17_s, segment_container_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib17_r17_s::segment_container_r17), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib17_r17_s_names[] = {{"segmentNumber-r17", nullptr},
                                                                      {"segmentType-r17", nullptr},
                                                                      {"segmentContainer-r17", nullptr}};
const asn1::table::seq_desc          sib17_r17_s::desc = {{seq_table_sib17_r17_s_fields, 3, UINT64_C(0x7), nullptr, 0},
                                                          false,
                                                          false,
                                                          nullptr,
                                                          0,
                                                          seq_table_sib17_r17_s_names};

static const char*           enum_names_sib17_r17_s_segment_type_r17_e__values[] = {"notLastSegment", "lastSegment"};
const asn1::table::enum_desc sib17_r17_s::segment_type_r17_opts::desc            = {
    enum_names_sib17_r17_s_segment_type_r17_e__values,
    2,
    "sib17_r17_s::segment_type_r17_e_"};

// GIN-Element-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_gin_elem_r17_s_fields[] = {
    {offsetof(gin_elem_r17_s, plmn_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc},
    {offsetof(gin_elem_r17_s, nid_list_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     24,
     &asn1::table::list_codec<decltype(gin_elem_r17_s::nid_list_r17), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gin_elem_r17_s_names[] = {{"plmn-Identity-r17", nullptr},
                                                                         {"nid-List-r17", nullptr}};
const asn1::table::seq_desc gin_elem_r17_s::desc = {{seq_table_gin_elem_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0,
                                                    seq_table_gin_elem_r17_s_names};

// GINs-PerSNPN-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_gi_ns_per_sn_pn_r17_s_fields[] = {
    {offsetof(gi_ns_per_sn_pn_r17_s, supported_gi_ns_r17),
     (int32_t)offsetof(gi_ns_per_sn_pn_r17_s, supported_gi_ns_r17_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(gi_ns_per_sn_pn_r17_s::supported_gi_ns_r17),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_gi_ns_per_sn_pn_r17_s_names[] = {{"supportedGINs-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_gi_ns_per_sn_pn_r17_s_opts[]  = {
    {0, (int32_t)offsetof(gi_ns_per_sn_pn_r17_s, supported_gi_ns_r17_present)}};
const asn1::table::seq_desc gi_ns_per_sn_pn_r17_s::desc = {
    {seq_table_gi_ns_per_sn_pn_r17_s_fields, 1, UINT64_C(0x0), seq_table_gi_ns_per_sn_pn_r17_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_gi_ns_per_sn_pn_r17_s_names};

// SIB18-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib18_r17_s_fields[] = {
    {offsetof(sib18_r17_s, gin_elem_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     24,
     &asn1::table::
         item_list_codec<decltype(sib18_r17_s::gin_elem_list_r17), asn1::table::sequence_item_ops, false, true>::value,
     &gin_elem_r17_s::desc},
    {offsetof(sib18_r17_s, gins_per_sn_pn_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::item_list_codec<decltype(sib18_r17_s::gins_per_sn_pn_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &gi_ns_per_sn_pn_r17_s::desc},
    {offsetof(sib18_r17_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib18_r17_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib18_r17_s_names[] = {{"gin-ElementList-r17", nullptr},
                                                                      {"gins-PerSNPN-List-r17", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib18_r17_s_opts[]  = {{0, asn1::table::seq_field::list_presence},
                                                                      {1, asn1::table::seq_field::list_presence},
                                                                      {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          sib18_r17_s::desc             = {
    {seq_table_sib18_r17_s_fields, 3, UINT64_C(0x0), seq_table_sib18_r17_s_opts, 3},
    true,
    false,
    nullptr,
    0,
    seq_table_sib18_r17_s_names};

// EpochTime-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_epoch_time_r17_s_fields[] = {
    {offsetof(epoch_time_r17_s, sfn_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(epoch_time_r17_s, sub_frame_nr_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(9),
     nullptr}};
static const asn1::table::field_name seq_table_epoch_time_r17_s_names[] = {{"sfn-r17", nullptr},
                                                                           {"subFrameNR-r17", nullptr}};
const asn1::table::seq_desc epoch_time_r17_s::desc = {{seq_table_epoch_time_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
                                                      false,
                                                      false,
                                                      nullptr,
                                                      0,
                                                      seq_table_epoch_time_r17_s_names};

// TA-Info-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ta_info_r17_s_fields[] = {
    {offsetof(ta_info_r17_s, ta_common_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(66485757),
     nullptr},
    {offsetof(ta_info_r17_s, ta_common_drift_r17),
     (int32_t)offsetof(ta_info_r17_s, ta_common_drift_r17_present),
     asn1::table::field_op::int_i32,
     false,
     false,
     INT64_C(-257303),
     UINT64_C(257303),
     nullptr},
    {offsetof(ta_info_r17_s, ta_common_drift_variant_r17),
     (int32_t)offsetof(ta_info_r17_s, ta_common_drift_variant_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(28949),
     nullptr}};
static const asn1::table::field_name seq_table_ta_info_r17_s_names[] = {{"ta-Common-r17", nullptr},
                                                                        {"ta-CommonDrift-r17", nullptr},
                                                                        {"ta-CommonDriftVariant-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_ta_info_r17_s_opts[]  = {
    {1, (int32_t)offsetof(ta_info_r17_s, ta_common_drift_r17_present)},
    {2, (int32_t)offsetof(ta_info_r17_s, ta_common_drift_variant_r17_present)}};
const asn1::table::seq_desc ta_info_r17_s::desc = {
    {seq_table_ta_info_r17_s_fields, 3, UINT64_C(0x1), seq_table_ta_info_r17_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ta_info_r17_s_names};

// PositionVelocity-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_position_velocity_r17_s_fields[] = {
    {offsetof(position_velocity_r17_s, position_x_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i32,
     false,
     false,
     INT64_C(-33554432),
     UINT64_C(33554431),
     nullptr},
    {offsetof(position_velocity_r17_s, position_y_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i32,
     false,
     false,
     INT64_C(-33554432),
     UINT64_C(33554431),
     nullptr},
    {offsetof(position_velocity_r17_s, position_z_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i32,
     false,
     false,
     INT64_C(-33554432),
     UINT64_C(33554431),
     nullptr},
    {offsetof(position_velocity_r17_s, velocity_vx_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i32,
     false,
     false,
     INT64_C(-131072),
     UINT64_C(131071),
     nullptr},
    {offsetof(position_velocity_r17_s, velocity_vy_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i32,
     false,
     false,
     INT64_C(-131072),
     UINT64_C(131071),
     nullptr},
    {offsetof(position_velocity_r17_s, velocity_vz_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i32,
     false,
     false,
     INT64_C(-131072),
     UINT64_C(131071),
     nullptr}};
static const asn1::table::field_name seq_table_position_velocity_r17_s_names[] = {{"positionX-r17", nullptr},
                                                                                  {"positionY-r17", nullptr},
                                                                                  {"positionZ-r17", nullptr},
                                                                                  {"velocityVX-r17", nullptr},
                                                                                  {"velocityVY-r17", nullptr},
                                                                                  {"velocityVZ-r17", nullptr}};
const asn1::table::seq_desc          position_velocity_r17_s::desc             = {
    {seq_table_position_velocity_r17_s_fields, 6, UINT64_C(0x3f), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_position_velocity_r17_s_names};

// Orbital-r17 ::= SEQUENCE
static const asn1::table::seq_field  seq_table_orbital_r17_s_fields[] = {{offsetof(orbital_r17_s, semi_major_axis_r17),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::int_u64,
                                                                          false,
                                                                          false,
                                                                          INT64_C(0),
                                                                          UINT64_C(8589934591),
                                                                          nullptr},
                                                                         {offsetof(orbital_r17_s, eccentricity_r17),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::int_u32,
                                                                          false,
                                                                          false,
                                                                          INT64_C(0),
                                                                          UINT64_C(1048575),
                                                                          nullptr},
                                                                         {offsetof(orbital_r17_s, periapsis_r17),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::int_u32,
                                                                          false,
                                                                          false,
                                                                          INT64_C(0),
                                                                          UINT64_C(268435455),
                                                                          nullptr},
                                                                         {offsetof(orbital_r17_s, longitude_r17),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::int_u32,
                                                                          false,
                                                                          false,
                                                                          INT64_C(0),
                                                                          UINT64_C(268435455),
                                                                          nullptr},
                                                                         {offsetof(orbital_r17_s, inclination_r17),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::int_i32,
                                                                          false,
                                                                          false,
                                                                          INT64_C(-67108864),
                                                                          UINT64_C(67108863),
                                                                          nullptr},
                                                                         {offsetof(orbital_r17_s, mean_anomaly_r17),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::int_u32,
                                                                          false,
                                                                          false,
                                                                          INT64_C(0),
                                                                          UINT64_C(268435455),
                                                                          nullptr}};
static const asn1::table::field_name seq_table_orbital_r17_s_names[]  = {{"semiMajorAxis-r17", nullptr},
                                                                         {"eccentricity-r17", nullptr},
                                                                         {"periapsis-r17", nullptr},
                                                                         {"longitude-r17", nullptr},
                                                                         {"inclination-r17", nullptr},
                                                                         {"meanAnomaly-r17", nullptr}};
const asn1::table::seq_desc orbital_r17_s::desc = {{seq_table_orbital_r17_s_fields, 6, UINT64_C(0x3f), nullptr, 0},
                                                   false,
                                                   false,
                                                   nullptr,
                                                   0,
                                                   seq_table_orbital_r17_s_names};

// EphemerisInfo-r17 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ephemeris_info_r17_c_alternatives[] = {
    &asn1::table::alternative_ops<position_velocity_r17_s, false>::value,
    &asn1::table::alternative_ops<orbital_r17_s, false>::value};
const asn1::table::choice_storage_desc ephemeris_info_r17_c::storage_desc = {
    choice_storage_ephemeris_info_r17_c_alternatives,
    2,
    "ephemeris_info_r17_c"};

position_velocity_r17_s& ephemeris_info_r17_c::set_position_velocity_r17()
{
  set(types::position_velocity_r17);
  return c.get<position_velocity_r17_s>();
}
orbital_r17_s& ephemeris_info_r17_c::set_orbital_r17()
{
  set(types::orbital_r17);
  return c.get<orbital_r17_s>();
}
static const asn1::table::seq_field choice_table_ephemeris_info_r17_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &position_velocity_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &orbital_r17_s::desc}};
static const asn1::table::field_name choice_table_ephemeris_info_r17_c_names[] = {{"positionVelocity-r17", nullptr},
                                                                                  {"orbital-r17", nullptr}};
const asn1::table::choice_desc ephemeris_info_r17_c::desc = {asn1::table::enum_int_type<ephemeris_info_r17_c::types>(),
                                                             ephemeris_info_r17_c::types::nof_types,
                                                             ephemeris_info_r17_c::types::nof_exts,
                                                             ephemeris_info_r17_c::types::has_ext,
                                                             false,
                                                             choice_table_ephemeris_info_r17_c_alternatives,
                                                             nullptr,
                                                             choice_table_ephemeris_info_r17_c_names,
                                                             "ephemeris_info_r17_c",
                                                             offsetof(ephemeris_info_r17_c, c.type),
                                                             offsetof(ephemeris_info_r17_c, c.value),
                                                             false,
                                                             &ephemeris_info_r17_c::storage_desc};

static const char*           enum_names_ephemeris_info_r17_c_types_values[] = {"positionVelocity-r17", "orbital-r17"};
const asn1::table::enum_desc ephemeris_info_r17_c::types_opts::desc = {enum_names_ephemeris_info_r17_c_types_values,
                                                                       2,
                                                                       "ephemeris_info_r17_c::types"};

// NTN-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ntn_cfg_r17_s_fields[] = {
    {offsetof(ntn_cfg_r17_s, epoch_time_r17),
     (int32_t)offsetof(ntn_cfg_r17_s, epoch_time_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &epoch_time_r17_s::desc},
    {offsetof(ntn_cfg_r17_s, ntn_ul_sync_validity_dur_r17),
     (int32_t)offsetof(ntn_cfg_r17_s, ntn_ul_sync_validity_dur_r17_present),
     asn1::table::enum_op<decltype(ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17)>(),
     decltype(ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17)::has_ext,
     false,
     decltype(ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17)::nof_types,
     decltype(ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17)::nof_exts,
     nullptr,
     &ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::desc},
    {offsetof(ntn_cfg_r17_s, cell_specific_koffset_r17),
     (int32_t)offsetof(ntn_cfg_r17_s, cell_specific_koffset_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(1023),
     nullptr},
    {offsetof(ntn_cfg_r17_s, kmac_r17),
     (int32_t)offsetof(ntn_cfg_r17_s, kmac_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(512),
     nullptr},
    {offsetof(ntn_cfg_r17_s, ta_info_r17),
     (int32_t)offsetof(ntn_cfg_r17_s, ta_info_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ta_info_r17_s::desc},
    {offsetof(ntn_cfg_r17_s, ntn_polarization_dl_r17),
     (int32_t)offsetof(ntn_cfg_r17_s, ntn_polarization_dl_r17_present),
     asn1::table::enum_op<decltype(ntn_cfg_r17_s::ntn_polarization_dl_r17)>(),
     decltype(ntn_cfg_r17_s::ntn_polarization_dl_r17)::has_ext,
     false,
     decltype(ntn_cfg_r17_s::ntn_polarization_dl_r17)::nof_types,
     decltype(ntn_cfg_r17_s::ntn_polarization_dl_r17)::nof_exts,
     nullptr,
     &ntn_cfg_r17_s::ntn_polarization_dl_r17_opts::desc},
    {offsetof(ntn_cfg_r17_s, ntn_polarization_ul_r17),
     (int32_t)offsetof(ntn_cfg_r17_s, ntn_polarization_ul_r17_present),
     asn1::table::enum_op<decltype(ntn_cfg_r17_s::ntn_polarization_ul_r17)>(),
     decltype(ntn_cfg_r17_s::ntn_polarization_ul_r17)::has_ext,
     false,
     decltype(ntn_cfg_r17_s::ntn_polarization_ul_r17)::nof_types,
     decltype(ntn_cfg_r17_s::ntn_polarization_ul_r17)::nof_exts,
     nullptr,
     &ntn_cfg_r17_s::ntn_polarization_ul_r17_opts::desc},
    {offsetof(ntn_cfg_r17_s, ephemeris_info_r17),
     (int32_t)offsetof(ntn_cfg_r17_s, ephemeris_info_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ephemeris_info_r17_c::desc},
    {0,
     (int32_t)offsetof(ntn_cfg_r17_s, ta_report_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ntn_cfg_r17_s_names[] = {{"epochTime-r17", nullptr},
                                                                        {"ntn-UlSyncValidityDuration-r17", nullptr},
                                                                        {"cellSpecificKoffset-r17", nullptr},
                                                                        {"kmac-r17", nullptr},
                                                                        {"ta-Info-r17", nullptr},
                                                                        {"ntn-PolarizationDL-r17", nullptr},
                                                                        {"ntn-PolarizationUL-r17", nullptr},
                                                                        {"ephemerisInfo-r17", nullptr},
                                                                        {"ta-Report-r17", "enabled"}};
static const asn1::table::seq_opt    seq_table_ntn_cfg_r17_s_opts[]  = {
    {0, (int32_t)offsetof(ntn_cfg_r17_s, epoch_time_r17_present)},
    {1, (int32_t)offsetof(ntn_cfg_r17_s, ntn_ul_sync_validity_dur_r17_present)},
    {2, (int32_t)offsetof(ntn_cfg_r17_s, cell_specific_koffset_r17_present)},
    {3, (int32_t)offsetof(ntn_cfg_r17_s, kmac_r17_present)},
    {4, (int32_t)offsetof(ntn_cfg_r17_s, ta_info_r17_present)},
    {5, (int32_t)offsetof(ntn_cfg_r17_s, ntn_polarization_dl_r17_present)},
    {6, (int32_t)offsetof(ntn_cfg_r17_s, ntn_polarization_ul_r17_present)},
    {7, (int32_t)offsetof(ntn_cfg_r17_s, ephemeris_info_r17_present)},
    {8, (int32_t)offsetof(ntn_cfg_r17_s, ta_report_r17_present)}};
const asn1::table::seq_desc ntn_cfg_r17_s::desc = {
    {seq_table_ntn_cfg_r17_s_fields, 9, UINT64_C(0x0), seq_table_ntn_cfg_r17_s_opts, 9},
    true,
    false,
    nullptr,
    0,
    seq_table_ntn_cfg_r17_s_names};

static const char* enum_names_ntn_cfg_r17_s_ntn_ul_sync_validity_dur_r17_e__values[] =
    {"s5", "s10", "s15", "s20", "s25", "s30", "s35", "s40", "s45", "s50", "s55", "s60", "s120", "s180", "s240", "s900"};
const asn1::table::enum_desc ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::desc = {
    enum_names_ntn_cfg_r17_s_ntn_ul_sync_validity_dur_r17_e__values,
    16,
    "ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_e_"};
uint16_t ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 120, 180, 240, 900};
  return map_enum_number(numbers, 16, value, "ntn_cfg_r17_s::ntn_ul_sync_validity_dur_r17_e_");
}

static const char*           enum_names_ntn_cfg_r17_s_ntn_polarization_dl_r17_e__values[] = {"rhcp", "lhcp", "linear"};
const asn1::table::enum_desc ntn_cfg_r17_s::ntn_polarization_dl_r17_opts::desc            = {
    enum_names_ntn_cfg_r17_s_ntn_polarization_dl_r17_e__values,
    3,
    "ntn_cfg_r17_s::ntn_polarization_dl_r17_e_"};

static const char*           enum_names_ntn_cfg_r17_s_ntn_polarization_ul_r17_e__values[] = {"rhcp", "lhcp", "linear"};
const asn1::table::enum_desc ntn_cfg_r17_s::ntn_polarization_ul_r17_opts::desc            = {
    enum_names_ntn_cfg_r17_s_ntn_polarization_ul_r17_e__values,
    3,
    "ntn_cfg_r17_s::ntn_polarization_ul_r17_e_"};

// NTN-NeighCellConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ntn_neigh_cell_cfg_r17_s_fields[] = {
    {offsetof(ntn_neigh_cell_cfg_r17_s, ntn_cfg_r17),
     (int32_t)offsetof(ntn_neigh_cell_cfg_r17_s, ntn_cfg_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ntn_cfg_r17_s::desc},
    {offsetof(ntn_neigh_cell_cfg_r17_s, carrier_freq_r17),
     (int32_t)offsetof(ntn_neigh_cell_cfg_r17_s, carrier_freq_r17_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(ntn_neigh_cell_cfg_r17_s, pci_r17),
     (int32_t)offsetof(ntn_neigh_cell_cfg_r17_s, pci_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr}};
static const asn1::table::field_name seq_table_ntn_neigh_cell_cfg_r17_s_names[] = {{"ntn-Config-r17", nullptr},
                                                                                   {"carrierFreq-r17", nullptr},
                                                                                   {"physCellId-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_ntn_neigh_cell_cfg_r17_s_opts[]  = {
    {0, (int32_t)offsetof(ntn_neigh_cell_cfg_r17_s, ntn_cfg_r17_present)},
    {1, (int32_t)offsetof(ntn_neigh_cell_cfg_r17_s, carrier_freq_r17_present)},
    {2, (int32_t)offsetof(ntn_neigh_cell_cfg_r17_s, pci_r17_present)}};
const asn1::table::seq_desc ntn_neigh_cell_cfg_r17_s::desc = {
    {seq_table_ntn_neigh_cell_cfg_r17_s_fields, 3, UINT64_C(0x0), seq_table_ntn_neigh_cell_cfg_r17_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_ntn_neigh_cell_cfg_r17_s_names};

// NTN-CovEnh-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ntn_cov_enh_r18_s_fields[] = {
    {offsetof(ntn_cov_enh_r18_s, nof_msg4_harq_ack_repeats_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ntn_cov_enh_r18_s::nof_msg4_harq_ack_repeats_r18),
                                asn1::table::struct_json>::value},
    {offsetof(ntn_cov_enh_r18_s, rsrp_thres_msg4_harq_ack_r18),
     (int32_t)offsetof(ntn_cov_enh_r18_s, rsrp_thres_msg4_harq_ack_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr}};
static const asn1::table::field_name seq_table_ntn_cov_enh_r18_s_names[] = {
    {"numberOfMsg4HARQ-ACK-Repetitions-r18", nullptr},
    {"rsrp-ThresholdMsg4HARQ-ACK-r18", nullptr}};
static const asn1::table::seq_opt seq_table_ntn_cov_enh_r18_s_opts[] = {
    {1, (int32_t)offsetof(ntn_cov_enh_r18_s, rsrp_thres_msg4_harq_ack_r18_present)}};
const asn1::table::seq_desc ntn_cov_enh_r18_s::desc = {
    {seq_table_ntn_cov_enh_r18_s_fields, 2, UINT64_C(0x1), seq_table_ntn_cov_enh_r18_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_ntn_cov_enh_r18_s_names};

// SatSwitchWithReSync-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sat_switch_with_re_sync_r18_s_fields[] = {
    {offsetof(sat_switch_with_re_sync_r18_s, ntn_cfg_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ntn_cfg_r17_s::desc},
    {offsetof(sat_switch_with_re_sync_r18_s, t_service_start_r18),
     (int32_t)offsetof(sat_switch_with_re_sync_r18_s, t_service_start_r18_present),
     asn1::table::field_op::int_u64,
     false,
     false,
     INT64_C(0),
     UINT64_C(549755813887),
     nullptr},
    {offsetof(sat_switch_with_re_sync_r18_s, ssb_time_offset_r18),
     (int32_t)offsetof(sat_switch_with_re_sync_r18_s, ssb_time_offset_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(159),
     nullptr}};
static const asn1::table::field_name seq_table_sat_switch_with_re_sync_r18_s_names[] = {
    {"ntn-Config-r18", nullptr},
    {"t-ServiceStart-r18", nullptr},
    {"ssb-TimeOffset-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sat_switch_with_re_sync_r18_s_opts[] = {
    {1, (int32_t)offsetof(sat_switch_with_re_sync_r18_s, t_service_start_r18_present)},
    {2, (int32_t)offsetof(sat_switch_with_re_sync_r18_s, ssb_time_offset_r18_present)}};
const asn1::table::seq_desc sat_switch_with_re_sync_r18_s::desc = {
    {seq_table_sat_switch_with_re_sync_r18_s_fields, 3, UINT64_C(0x1), seq_table_sat_switch_with_re_sync_r18_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_sat_switch_with_re_sync_r18_s_names};

// SIB19-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib19_r17_s_fields[] = {
    {offsetof(sib19_r17_s, ntn_cfg_r17),
     (int32_t)offsetof(sib19_r17_s, ntn_cfg_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ntn_cfg_r17_s::desc},
    {offsetof(sib19_r17_s, t_service_r17),
     (int32_t)offsetof(sib19_r17_s, t_service_r17_present),
     asn1::table::field_op::int_u64,
     false,
     false,
     INT64_C(0),
     UINT64_C(549755813887),
     nullptr},
    {offsetof(sib19_r17_s, ref_location_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib19_r17_s::ref_location_r17), asn1::table::string_json>::value},
    {offsetof(sib19_r17_s, distance_thresh_r17),
     (int32_t)offsetof(sib19_r17_s, distance_thresh_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(65525),
     nullptr},
    {offsetof(sib19_r17_s, ntn_neigh_cell_cfg_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(sib19_r17_s::ntn_neigh_cell_cfg_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ntn_neigh_cell_cfg_r17_s::desc},
    {offsetof(sib19_r17_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib19_r17_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib19_r17_s_names[] = {{"ntn-Config-r17", nullptr},
                                                                      {"t-Service-r17", nullptr},
                                                                      {"referenceLocation-r17", nullptr},
                                                                      {"distanceThresh-r17", nullptr},
                                                                      {"ntn-NeighCellConfigList-r17", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib19_r17_s_opts[]  = {
    {0, (int32_t)offsetof(sib19_r17_s, ntn_cfg_r17_present)},
    {1, (int32_t)offsetof(sib19_r17_s, t_service_r17_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(sib19_r17_s, distance_thresh_r17_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_sib19_r17_s_g0_fields[] = {
    {offsetof(sib19_r17_s, ntn_neigh_cell_cfg_list_ext_v1720),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::ptr_item_list_codec<decltype(sib19_r17_s::ntn_neigh_cell_cfg_list_ext_v1720),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &ntn_neigh_cell_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sib19_r17_s_g0_names[] = {{"ntn-NeighCellConfigListExt-v1720", nullptr}};
static const asn1::table::seq_opt    seq_table_sib19_r17_s_g0_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_sib19_r17_s_g1_fields[] = {
    {offsetof(sib19_r17_s, moving_ref_location_r18),
      asn1::table::seq_field::list_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::sized_object_codec<decltype(sib19_r17_s::moving_ref_location_r18), asn1::table::string_json>::value},
    {offsetof(sib19_r17_s, ntn_cov_enh_r18),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(sib19_r17_s::ntn_cov_enh_r18)>::value,
      &ntn_cov_enh_r18_s::desc},
    {offsetof(sib19_r17_s, sat_switch_with_re_sync_r18),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(sib19_r17_s::sat_switch_with_re_sync_r18)>::value,
      &sat_switch_with_re_sync_r18_s::desc}};
static const asn1::table::field_name seq_table_sib19_r17_s_g1_names[] = {{"movingReferenceLocation-r18", nullptr},
                                                                         {"ntn-CovEnh-r18", nullptr},
                                                                         {"satSwitchWithReSync-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_sib19_r17_s_g1_opts[]  = {{0, asn1::table::seq_field::list_presence},
                                                                         {1, asn1::table::seq_field::ptr_presence},
                                                                         {2, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_sib19_r17_s_groups[]   = {
    {{seq_table_sib19_r17_s_g0_fields, 1, UINT64_C(0x0), seq_table_sib19_r17_s_g0_opts, 1},
        false,
        seq_table_sib19_r17_s_g0_names},
    {{seq_table_sib19_r17_s_g1_fields, 3, UINT64_C(0x0), seq_table_sib19_r17_s_g1_opts, 3},
        false,
        seq_table_sib19_r17_s_g1_names}};
const asn1::table::seq_desc sib19_r17_s::desc = {
    {seq_table_sib19_r17_s_fields, 6, UINT64_C(0x0), seq_table_sib19_r17_s_opts, 6},
    true,
    false,
    seq_table_sib19_r17_s_groups,
    2,
    seq_table_sib19_r17_s_names};

// MCCH-RepetitionPeriodAndOffset-r17 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_mcch_repeat_period_and_offset_r17_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc mcch_repeat_period_and_offset_r17_c::storage_desc = {
    choice_storage_mcch_repeat_period_and_offset_r17_c_alternatives,
    9,
    "mcch_repeat_period_and_offset_r17_c"};

uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf1_r17()
{
  set(types::rf1_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf2_r17()
{
  set(types::rf2_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf4_r17()
{
  set(types::rf4_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf8_r17()
{
  set(types::rf8_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf16_r17()
{
  set(types::rf16_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf32_r17()
{
  set(types::rf32_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf64_r17()
{
  set(types::rf64_r17);
  return c.get<uint8_t>();
}
uint8_t& mcch_repeat_period_and_offset_r17_c::set_rf128_r17()
{
  set(types::rf128_r17);
  return c.get<uint8_t>();
}
uint16_t& mcch_repeat_period_and_offset_r17_c::set_rf256_r17()
{
  set(types::rf256_r17);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field choice_table_mcch_repeat_period_and_offset_r17_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(0),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(1),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
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
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(255),
     nullptr}};
static const asn1::table::field_name choice_table_mcch_repeat_period_and_offset_r17_c_names[] = {
    {"rf1-r17", nullptr},
    {"rf2-r17", nullptr},
    {"rf4-r17", nullptr},
    {"rf8-r17", nullptr},
    {"rf16-r17", nullptr},
    {"rf32-r17", nullptr},
    {"rf64-r17", nullptr},
    {"rf128-r17", nullptr},
    {"rf256-r17", nullptr}};
const asn1::table::choice_desc mcch_repeat_period_and_offset_r17_c::desc = {
    asn1::table::enum_int_type<mcch_repeat_period_and_offset_r17_c::types>(),
    mcch_repeat_period_and_offset_r17_c::types::nof_types,
    mcch_repeat_period_and_offset_r17_c::types::nof_exts,
    mcch_repeat_period_and_offset_r17_c::types::has_ext,
    false,
    choice_table_mcch_repeat_period_and_offset_r17_c_alternatives,
    nullptr,
    choice_table_mcch_repeat_period_and_offset_r17_c_names,
    "mcch_repeat_period_and_offset_r17_c",
    offsetof(mcch_repeat_period_and_offset_r17_c, c.type),
    offsetof(mcch_repeat_period_and_offset_r17_c, c.value),
    false,
    &mcch_repeat_period_and_offset_r17_c::storage_desc};

static const char* enum_names_mcch_repeat_period_and_offset_r17_c_types_values[] =
    {"rf1-r17", "rf2-r17", "rf4-r17", "rf8-r17", "rf16-r17", "rf32-r17", "rf64-r17", "rf128-r17", "rf256-r17"};
const asn1::table::enum_desc mcch_repeat_period_and_offset_r17_c::types_opts::desc = {
    enum_names_mcch_repeat_period_and_offset_r17_c_types_values,
    9,
    "mcch_repeat_period_and_offset_r17_c::types"};
uint16_t mcch_repeat_period_and_offset_r17_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 8, 16, 32, 64, 128, 256};
  return map_enum_number(numbers, 9, value, "mcch_repeat_period_and_offset_r17_c::types");
}

// MCCH-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mcch_cfg_r17_s_fields[] = {
    {offsetof(mcch_cfg_r17_s, mcch_repeat_period_and_offset_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mcch_repeat_period_and_offset_r17_c::desc},
    {offsetof(mcch_cfg_r17_s, mcch_win_start_slot_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(79),
     nullptr},
    {offsetof(mcch_cfg_r17_s, mcch_win_dur_r17),
     (int32_t)offsetof(mcch_cfg_r17_s, mcch_win_dur_r17_present),
     asn1::table::enum_op<decltype(mcch_cfg_r17_s::mcch_win_dur_r17)>(),
     decltype(mcch_cfg_r17_s::mcch_win_dur_r17)::has_ext,
     false,
     decltype(mcch_cfg_r17_s::mcch_win_dur_r17)::nof_types,
     decltype(mcch_cfg_r17_s::mcch_win_dur_r17)::nof_exts,
     nullptr,
     &mcch_cfg_r17_s::mcch_win_dur_r17_opts::desc},
    {offsetof(mcch_cfg_r17_s, mcch_mod_period_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mcch_cfg_r17_s::mcch_mod_period_r17)>(),
     decltype(mcch_cfg_r17_s::mcch_mod_period_r17)::has_ext,
     false,
     decltype(mcch_cfg_r17_s::mcch_mod_period_r17)::nof_types,
     decltype(mcch_cfg_r17_s::mcch_mod_period_r17)::nof_exts,
     nullptr,
     &mcch_cfg_r17_s::mcch_mod_period_r17_opts::desc}};
static const asn1::table::field_name seq_table_mcch_cfg_r17_s_names[] = {
    {"mcch-RepetitionPeriodAndOffset-r17", nullptr},
    {"mcch-WindowStartSlot-r17", nullptr},
    {"mcch-WindowDuration-r17", nullptr},
    {"mcch-ModificationPeriod-r17", nullptr}};
static const asn1::table::seq_opt seq_table_mcch_cfg_r17_s_opts[] = {
    {2, (int32_t)offsetof(mcch_cfg_r17_s, mcch_win_dur_r17_present)}};
const asn1::table::seq_desc mcch_cfg_r17_s::desc = {
    {seq_table_mcch_cfg_r17_s_fields, 4, UINT64_C(0xb), seq_table_mcch_cfg_r17_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_mcch_cfg_r17_s_names};

static const char* enum_names_mcch_cfg_r17_s_mcch_win_dur_r17_e__values[] =
    {"sl2", "sl4", "sl8", "sl10", "sl20", "sl40", "sl80", "sl160"};
const asn1::table::enum_desc mcch_cfg_r17_s::mcch_win_dur_r17_opts::desc = {
    enum_names_mcch_cfg_r17_s_mcch_win_dur_r17_e__values,
    8,
    "mcch_cfg_r17_s::mcch_win_dur_r17_e_"};
uint8_t mcch_cfg_r17_s::mcch_win_dur_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 8, value, "mcch_cfg_r17_s::mcch_win_dur_r17_e_");
}

static const char*           enum_names_mcch_cfg_r17_s_mcch_mod_period_r17_e__values[] = {"rf2",
                                                                                          "rf4",
                                                                                          "rf8",
                                                                                          "rf16",
                                                                                          "rf32",
                                                                                          "rf64",
                                                                                          "rf128",
                                                                                          "rf256",
                                                                                          "rf512",
                                                                                          "rf1024",
                                                                                          "rf2048",
                                                                                          "rf4096",
                                                                                          "rf8192",
                                                                                          "rf16384",
                                                                                          "rf32768",
                                                                                          "rf65536"};
const asn1::table::enum_desc mcch_cfg_r17_s::mcch_mod_period_r17_opts::desc            = {
    enum_names_mcch_cfg_r17_s_mcch_mod_period_r17_e__values,
    16,
    "mcch_cfg_r17_s::mcch_mod_period_r17_e_"};
uint32_t mcch_cfg_r17_s::mcch_mod_period_r17_opts::to_number() const
{
  static const uint32_t numbers[] = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536};
  return map_enum_number(numbers, 16, value, "mcch_cfg_r17_s::mcch_mod_period_r17_e_");
}

// LocationAndBandwidthBroadcast-r17 ::= CHOICE
void location_and_bw_broadcast_r17_c::set(types::options e)
{
  type_ = e;
}
void location_and_bw_broadcast_r17_c::set_same_as_sib1_cfg_location_and_bw()
{
  set(types::same_as_sib1_cfg_location_and_bw);
}
uint16_t& location_and_bw_broadcast_r17_c::set_location_and_bw()
{
  set(types::location_and_bw);
  return c;
}
static const asn1::table::seq_field choice_table_location_and_bw_broadcast_r17_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(37949),
     nullptr}};
static const asn1::table::field_name choice_table_location_and_bw_broadcast_r17_c_names[] = {
    {"sameAsSib1ConfiguredLocationAndBW", nullptr},
    {"locationAndBandwidth", nullptr}};
const asn1::table::choice_desc location_and_bw_broadcast_r17_c::desc = {
    asn1::table::enum_int_type<location_and_bw_broadcast_r17_c::types>(),
    location_and_bw_broadcast_r17_c::types::nof_types,
    location_and_bw_broadcast_r17_c::types::nof_exts,
    location_and_bw_broadcast_r17_c::types::has_ext,
    false,
    choice_table_location_and_bw_broadcast_r17_c_alternatives,
    &asn1::table::choice_set<location_and_bw_broadcast_r17_c>,
    choice_table_location_and_bw_broadcast_r17_c_names,
    "location_and_bw_broadcast_r17_c",
    offsetof(location_and_bw_broadcast_r17_c, type_),
    offsetof(location_and_bw_broadcast_r17_c, c),
    false};

static const char* enum_names_location_and_bw_broadcast_r17_c_types_values[]   = {"sameAsSib1ConfiguredLocationAndBW",
                                                                                  "locationAndBandwidth"};
const asn1::table::enum_desc location_and_bw_broadcast_r17_c::types_opts::desc = {
    enum_names_location_and_bw_broadcast_r17_c_types_values,
    2,
    "location_and_bw_broadcast_r17_c::types"};
uint8_t location_and_bw_broadcast_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "location_and_bw_broadcast_r17_c::types");
}

// PDSCH-ConfigPTM-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdsch_cfg_ptm_r17_s_fields[] = {
    {offsetof(pdsch_cfg_ptm_r17_s, data_scrambling_id_pdsch_r17),
     (int32_t)offsetof(pdsch_cfg_ptm_r17_s, data_scrambling_id_pdsch_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(pdsch_cfg_ptm_r17_s, dmrs_scrambling_id0_r17),
     (int32_t)offsetof(pdsch_cfg_ptm_r17_s, dmrs_scrambling_id0_r17_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(pdsch_cfg_ptm_r17_s, pdsch_aggregation_factor_r17),
     (int32_t)offsetof(pdsch_cfg_ptm_r17_s, pdsch_aggregation_factor_r17_present),
     asn1::table::enum_op<decltype(pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17)>(),
     decltype(pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17)::has_ext,
     false,
     decltype(pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17)::nof_types,
     decltype(pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17)::nof_exts,
     nullptr,
     &pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_opts::desc}};
static const asn1::table::field_name seq_table_pdsch_cfg_ptm_r17_s_names[] = {
    {"dataScramblingIdentityPDSCH-r17", nullptr},
    {"dmrs-ScramblingID0-r17", nullptr},
    {"pdsch-AggregationFactor-r17", nullptr}};
static const asn1::table::seq_opt seq_table_pdsch_cfg_ptm_r17_s_opts[] = {
    {0, (int32_t)offsetof(pdsch_cfg_ptm_r17_s, data_scrambling_id_pdsch_r17_present)},
    {1, (int32_t)offsetof(pdsch_cfg_ptm_r17_s, dmrs_scrambling_id0_r17_present)},
    {2, (int32_t)offsetof(pdsch_cfg_ptm_r17_s, pdsch_aggregation_factor_r17_present)}};
const asn1::table::seq_desc pdsch_cfg_ptm_r17_s::desc = {
    {seq_table_pdsch_cfg_ptm_r17_s_fields, 3, UINT64_C(0x0), seq_table_pdsch_cfg_ptm_r17_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_pdsch_cfg_ptm_r17_s_names};

static const char* enum_names_pdsch_cfg_ptm_r17_s_pdsch_aggregation_factor_r17_e__values[] = {"n2", "n4", "n8"};
const asn1::table::enum_desc pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_opts::desc  = {
    enum_names_pdsch_cfg_ptm_r17_s_pdsch_aggregation_factor_r17_e__values,
    3,
    "pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_e_"};
uint8_t pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "pdsch_cfg_ptm_r17_s::pdsch_aggregation_factor_r17_e_");
}

// PDSCH-ConfigBroadcast-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdsch_cfg_broadcast_r17_s_fields[] = {
    {offsetof(pdsch_cfg_broadcast_r17_s, pdsch_cfg_list_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(pdsch_cfg_broadcast_r17_s::pdsch_cfg_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdsch_cfg_ptm_r17_s::desc},
    {offsetof(pdsch_cfg_broadcast_r17_s, pdsch_time_domain_alloc_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(pdsch_cfg_broadcast_r17_s::pdsch_time_domain_alloc_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdsch_time_domain_res_alloc_r16_s::desc},
    {offsetof(pdsch_cfg_broadcast_r17_s, rate_match_pattern_to_add_mod_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(pdsch_cfg_broadcast_r17_s::rate_match_pattern_to_add_mod_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &rate_match_pattern_s::desc},
    {offsetof(pdsch_cfg_broadcast_r17_s, lte_crs_to_match_around_r17),
     (int32_t)offsetof(pdsch_cfg_broadcast_r17_s, lte_crs_to_match_around_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rate_match_pattern_lte_crs_s::desc},
    {offsetof(pdsch_cfg_broadcast_r17_s, mcs_table_r17),
     (int32_t)offsetof(pdsch_cfg_broadcast_r17_s, mcs_table_r17_present),
     asn1::table::enum_op<decltype(pdsch_cfg_broadcast_r17_s::mcs_table_r17)>(),
     decltype(pdsch_cfg_broadcast_r17_s::mcs_table_r17)::has_ext,
     false,
     decltype(pdsch_cfg_broadcast_r17_s::mcs_table_r17)::nof_types,
     decltype(pdsch_cfg_broadcast_r17_s::mcs_table_r17)::nof_exts,
     nullptr,
     &pdsch_cfg_broadcast_r17_s::mcs_table_r17_opts::desc},
    {offsetof(pdsch_cfg_broadcast_r17_s, xoverhead_r17),
     (int32_t)offsetof(pdsch_cfg_broadcast_r17_s, xoverhead_r17_present),
     asn1::table::enum_op<decltype(pdsch_cfg_broadcast_r17_s::xoverhead_r17)>(),
     decltype(pdsch_cfg_broadcast_r17_s::xoverhead_r17)::has_ext,
     false,
     decltype(pdsch_cfg_broadcast_r17_s::xoverhead_r17)::nof_types,
     decltype(pdsch_cfg_broadcast_r17_s::xoverhead_r17)::nof_exts,
     nullptr,
     &pdsch_cfg_broadcast_r17_s::xoverhead_r17_opts::desc}};
static const asn1::table::field_name seq_table_pdsch_cfg_broadcast_r17_s_names[] = {
    {"pdschConfigList-r17", nullptr},
    {"pdsch-TimeDomainAllocationList-r17", nullptr},
    {"rateMatchPatternToAddModList-r17", nullptr},
    {"lte-CRS-ToMatchAround-r17", nullptr},
    {"mcs-Table-r17", nullptr},
    {"xOverhead-r17", nullptr}};
static const asn1::table::seq_opt seq_table_pdsch_cfg_broadcast_r17_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(pdsch_cfg_broadcast_r17_s, lte_crs_to_match_around_r17_present)},
    {4, (int32_t)offsetof(pdsch_cfg_broadcast_r17_s, mcs_table_r17_present)},
    {5, (int32_t)offsetof(pdsch_cfg_broadcast_r17_s, xoverhead_r17_present)}};
const asn1::table::seq_desc pdsch_cfg_broadcast_r17_s::desc = {
    {seq_table_pdsch_cfg_broadcast_r17_s_fields, 6, UINT64_C(0x1), seq_table_pdsch_cfg_broadcast_r17_s_opts, 5},
    false,
    false,
    nullptr,
    0,
    seq_table_pdsch_cfg_broadcast_r17_s_names};

static const char*           enum_names_pdsch_cfg_broadcast_r17_s_mcs_table_r17_e__values[] = {"qam256", "qam64LowSE"};
const asn1::table::enum_desc pdsch_cfg_broadcast_r17_s::mcs_table_r17_opts::desc            = {
    enum_names_pdsch_cfg_broadcast_r17_s_mcs_table_r17_e__values,
    2,
    "pdsch_cfg_broadcast_r17_s::mcs_table_r17_e_"};
uint16_t pdsch_cfg_broadcast_r17_s::mcs_table_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {256, 64};
  return map_enum_number(numbers, 2, value, "pdsch_cfg_broadcast_r17_s::mcs_table_r17_e_");
}

static const char* enum_names_pdsch_cfg_broadcast_r17_s_xoverhead_r17_e__values[] = {"xOh6", "xOh12", "xOh18"};
const asn1::table::enum_desc pdsch_cfg_broadcast_r17_s::xoverhead_r17_opts::desc  = {
    enum_names_pdsch_cfg_broadcast_r17_s_xoverhead_r17_e__values,
    3,
    "pdsch_cfg_broadcast_r17_s::xoverhead_r17_e_"};
uint8_t pdsch_cfg_broadcast_r17_s::xoverhead_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 12, 18};
  return map_enum_number(numbers, 3, value, "pdsch_cfg_broadcast_r17_s::xoverhead_r17_e_");
}

// CFR-ConfigMCCH-MTCH-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfr_cfg_mcch_mtch_r17_s_fields[] = {
    {offsetof(cfr_cfg_mcch_mtch_r17_s, location_and_bw_broadcast_r17),
     (int32_t)offsetof(cfr_cfg_mcch_mtch_r17_s, location_and_bw_broadcast_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &location_and_bw_broadcast_r17_c::desc},
    {offsetof(cfr_cfg_mcch_mtch_r17_s, pdsch_cfg_mcch_r17),
     (int32_t)offsetof(cfr_cfg_mcch_mtch_r17_s, pdsch_cfg_mcch_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdsch_cfg_broadcast_r17_s::desc},
    {offsetof(cfr_cfg_mcch_mtch_r17_s, common_coreset_ext_r17),
     (int32_t)offsetof(cfr_cfg_mcch_mtch_r17_s, common_coreset_ext_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &coreset_s::desc}};
static const asn1::table::field_name seq_table_cfr_cfg_mcch_mtch_r17_s_names[] = {
    {"locationAndBandwidthBroadcast-r17", nullptr},
    {"pdsch-ConfigMCCH-r17", nullptr},
    {"commonControlResourceSetExt-r17", nullptr}};
static const asn1::table::seq_opt seq_table_cfr_cfg_mcch_mtch_r17_s_opts[] = {
    {0, (int32_t)offsetof(cfr_cfg_mcch_mtch_r17_s, location_and_bw_broadcast_r17_present)},
    {1, (int32_t)offsetof(cfr_cfg_mcch_mtch_r17_s, pdsch_cfg_mcch_r17_present)},
    {2, (int32_t)offsetof(cfr_cfg_mcch_mtch_r17_s, common_coreset_ext_r17_present)}};
const asn1::table::seq_desc cfr_cfg_mcch_mtch_r17_s::desc = {
    {seq_table_cfr_cfg_mcch_mtch_r17_s_fields, 3, UINT64_C(0x0), seq_table_cfr_cfg_mcch_mtch_r17_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_cfr_cfg_mcch_mtch_r17_s_names};

// SIB20-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib20_r17_s_fields[] = {
    {offsetof(sib20_r17_s, mcch_cfg_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mcch_cfg_r17_s::desc},
    {offsetof(sib20_r17_s, cfr_cfg_mcch_mtch_r17),
     (int32_t)offsetof(sib20_r17_s, cfr_cfg_mcch_mtch_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cfr_cfg_mcch_mtch_r17_s::desc},
    {offsetof(sib20_r17_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib20_r17_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib20_r17_s_names[] = {{"mcch-Config-r17", nullptr},
                                                                      {"cfr-ConfigMCCH-MTCH-r17", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib20_r17_s_opts[]  = {
    {1, (int32_t)offsetof(sib20_r17_s, cfr_cfg_mcch_mtch_r17_present)},
    {2, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_sib20_r17_s_g0_fields[] = {
    {offsetof(sib20_r17_s, cfr_cfg_mcch_mtch_red_cap_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sib20_r17_s::cfr_cfg_mcch_mtch_red_cap_r18)>::value,
     &cfr_cfg_mcch_mtch_r17_s::desc},
    {offsetof(sib20_r17_s, mcch_cfg_red_cap_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sib20_r17_s::mcch_cfg_red_cap_r18)>::value,
     &mcch_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sib20_r17_s_g0_names[] = {{"cfr-ConfigMCCH-MTCH-RedCap-r18", nullptr},
                                                                         {"mcch-ConfigRedCap-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_sib20_r17_s_g0_opts[]  = {{0, asn1::table::seq_field::ptr_presence},
                                                                         {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_sib20_r17_s_groups[]   = {
    {{seq_table_sib20_r17_s_g0_fields, 2, UINT64_C(0x0), seq_table_sib20_r17_s_g0_opts, 2},
        false,
        seq_table_sib20_r17_s_g0_names}};
const asn1::table::seq_desc sib20_r17_s::desc = {
    {seq_table_sib20_r17_s_fields, 3, UINT64_C(0x1), seq_table_sib20_r17_s_opts, 2},
    true,
    false,
    seq_table_sib20_r17_s_groups,
    1,
    seq_table_sib20_r17_s_names};

// MBS-FSAI-InterFreq-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_fsai_inter_freq_r17_s_fields[] = {
    {offsetof(mbs_fsai_inter_freq_r17_s, dl_carrier_freq_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(mbs_fsai_inter_freq_r17_s, mbs_fsai_list_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(mbs_fsai_inter_freq_r17_s::mbs_fsai_list_r17),
                              asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_mbs_fsai_inter_freq_r17_s_names[] = {{"dl-CarrierFreq-r17", nullptr},
                                                                                    {"mbs-FSAI-List-r17", nullptr}};
const asn1::table::seq_desc          mbs_fsai_inter_freq_r17_s::desc             = {
    {seq_table_mbs_fsai_inter_freq_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_mbs_fsai_inter_freq_r17_s_names};

// SIB21-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib21_r17_s_fields[] = {
    {offsetof(sib21_r17_s, mbs_fsai_intra_freq_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(sib21_r17_s::mbs_fsai_intra_freq_r17), asn1::table::string_json>::value},
    {offsetof(sib21_r17_s, mbs_fsai_inter_freq_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sib21_r17_s::mbs_fsai_inter_freq_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mbs_fsai_inter_freq_r17_s::desc},
    {offsetof(sib21_r17_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib21_r17_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib21_r17_s_names[] = {{"mbs-FSAI-IntraFreq-r17", nullptr},
                                                                      {"mbs-FSAI-InterFreqList-r17", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib21_r17_s_opts[]  = {{0, asn1::table::seq_field::list_presence},
                                                                      {1, asn1::table::seq_field::list_presence},
                                                                      {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          sib21_r17_s::desc             = {
    {seq_table_sib21_r17_s_fields, 3, UINT64_C(0x0), seq_table_sib21_r17_s_opts, 3},
    true,
    false,
    nullptr,
    0,
    seq_table_sib21_r17_s_names};

// SIB22-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib22_r18_s_fields[] = {
    {offsetof(sib22_r18_s, atg_cfg_r18),
     (int32_t)offsetof(sib22_r18_s, atg_cfg_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &atg_cfg_r18_s::desc},
    {0,
     (int32_t)offsetof(sib22_r18_s, hs_atg_cell_resel_set_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib22_r18_s, atg_neigh_cell_cfg_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sib22_r18_s::atg_neigh_cell_cfg_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &atg_neigh_cell_cfg_r18_s::desc},
    {offsetof(sib22_r18_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib22_r18_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib22_r18_s_names[] = {{"atg-Config-r18", nullptr},
                                                                      {"hs-ATG-CellReselectionSet-r18", "true"},
                                                                      {"atg-NeighCellConfigList-r18", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib22_r18_s_opts[]  = {
    {0, (int32_t)offsetof(sib22_r18_s, atg_cfg_r18_present)},
    {1, (int32_t)offsetof(sib22_r18_s, hs_atg_cell_resel_set_r18_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sib22_r18_s::desc = {
    {seq_table_sib22_r18_s_fields, 4, UINT64_C(0x0), seq_table_sib22_r18_s_opts, 4},
    true,
    false,
    nullptr,
    0,
    seq_table_sib22_r18_s_names};

// SIB23-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib23_r18_s_fields[] = {
    {offsetof(sib23_r18_s, segment_num_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(sib23_r18_s, segment_type_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sib23_r18_s::segment_type_r18)>(),
     decltype(sib23_r18_s::segment_type_r18)::has_ext,
     false,
     decltype(sib23_r18_s::segment_type_r18)::nof_types,
     decltype(sib23_r18_s::segment_type_r18)::nof_exts,
     nullptr,
     &sib23_r18_s::segment_type_r18_opts::desc},
    {offsetof(sib23_r18_s, segment_container_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib23_r18_s::segment_container_r18), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib23_r18_s_names[] = {{"segmentNumber-r18", nullptr},
                                                                      {"segmentType-r18", nullptr},
                                                                      {"segmentContainer-r18", nullptr}};
const asn1::table::seq_desc          sib23_r18_s::desc = {{seq_table_sib23_r18_s_fields, 3, UINT64_C(0x7), nullptr, 0},
                                                          false,
                                                          false,
                                                          nullptr,
                                                          0,
                                                          seq_table_sib23_r18_s_names};

static const char*           enum_names_sib23_r18_s_segment_type_r18_e__values[] = {"notLastSegment", "lastSegment"};
const asn1::table::enum_desc sib23_r18_s::segment_type_r18_opts::desc            = {
    enum_names_sib23_r18_s_segment_type_r18_e__values,
    2,
    "sib23_r18_s::segment_type_r18_e_"};

// SIB24-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib24_r18_s_fields[] = {
    {offsetof(sib24_r18_s, multicast_mcch_cfg_r18),
     (int32_t)offsetof(sib24_r18_s, multicast_mcch_cfg_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mcch_cfg_r17_s::desc},
    {offsetof(sib24_r18_s, cfr_cfg_mcch_mtch_r18),
     (int32_t)offsetof(sib24_r18_s, cfr_cfg_mcch_mtch_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cfr_cfg_mcch_mtch_r17_s::desc},
    {offsetof(sib24_r18_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib24_r18_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib24_r18_s_names[] = {{"multicastMCCH-Config-r18", nullptr},
                                                                      {"cfr-ConfigMCCH-MTCH-r18", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib24_r18_s_opts[]  = {
    {0, (int32_t)offsetof(sib24_r18_s, multicast_mcch_cfg_r18_present)},
    {1, (int32_t)offsetof(sib24_r18_s, cfr_cfg_mcch_mtch_r18_present)},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sib24_r18_s::desc = {
    {seq_table_sib24_r18_s_fields, 3, UINT64_C(0x0), seq_table_sib24_r18_s_opts, 3},
    true,
    false,
    nullptr,
    0,
    seq_table_sib24_r18_s_names};

// CoverageAreaInfo-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_coverage_area_info_r18_s_fields[] = {
    {offsetof(coverage_area_info_r18_s, tn_area_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(coverage_area_info_r18_s, tn_ref_location_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(coverage_area_info_r18_s::tn_ref_location_r18),
                                asn1::table::string_json>::value},
    {offsetof(coverage_area_info_r18_s, tn_distance_radius_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(65535),
     nullptr}};
static const asn1::table::field_name seq_table_coverage_area_info_r18_s_names[] = {
    {"tn-AreaId-r18", nullptr},
    {"tn-ReferenceLocation-r18", nullptr},
    {"tn-DistanceRadius-r18", nullptr}};
const asn1::table::seq_desc coverage_area_info_r18_s::desc = {
    {seq_table_coverage_area_info_r18_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_coverage_area_info_r18_s_names};

// SIB25-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib25_r18_s_fields[] = {
    {offsetof(sib25_r18_s, coverage_area_info_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(sib25_r18_s::coverage_area_info_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &coverage_area_info_r18_s::desc},
    {offsetof(sib25_r18_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib25_r18_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib25_r18_s_names[] = {{"coverageAreaInfoList-r18", nullptr},
                                                                      {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib25_r18_s_opts[]  = {{0, asn1::table::seq_field::list_presence},
                                                                      {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          sib25_r18_s::desc             = {
    {seq_table_sib25_r18_s_fields, 2, UINT64_C(0x0), seq_table_sib25_r18_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sib25_r18_s_names};

// SIB17bis-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib17bis_r18_s_fields[] = {
    {offsetof(sib17bis_r18_s, segment_num_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(sib17bis_r18_s, segment_type_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sib17bis_r18_s::segment_type_r18)>(),
     decltype(sib17bis_r18_s::segment_type_r18)::has_ext,
     false,
     decltype(sib17bis_r18_s::segment_type_r18)::nof_types,
     decltype(sib17bis_r18_s::segment_type_r18)::nof_exts,
     nullptr,
     &sib17bis_r18_s::segment_type_r18_opts::desc},
    {offsetof(sib17bis_r18_s, segment_container_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib17bis_r18_s::segment_container_r18), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib17bis_r18_s_names[] = {{"segmentNumber-r18", nullptr},
                                                                         {"segmentType-r18", nullptr},
                                                                         {"segmentContainer-r18", nullptr}};
const asn1::table::seq_desc sib17bis_r18_s::desc = {{seq_table_sib17bis_r18_s_fields, 3, UINT64_C(0x7), nullptr, 0},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0,
                                                    seq_table_sib17bis_r18_s_names};

static const char*           enum_names_sib17bis_r18_s_segment_type_r18_e__values[] = {"notLastSegment", "lastSegment"};
const asn1::table::enum_desc sib17bis_r18_s::segment_type_r18_opts::desc            = {
    enum_names_sib17bis_r18_s_segment_type_r18_e__values,
    2,
    "sib17bis_r18_s::segment_type_r18_e_"};

// SystemInformation-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_sys_info_ies_s_fields[] = {
    {offsetof(sys_info_ies_s, sib_type_and_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::
         item_list_codec<decltype(sys_info_ies_s::sib_type_and_info), asn1::table::choice_item_ops, false, true>::value,
     &sys_info_ies_s::sib_type_and_info_l_item_c_::desc},
    {offsetof(sys_info_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sys_info_ies_s::late_non_crit_ext), asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(sys_info_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_sys_info_ies_s_names[] = {{"sib-TypeAndInfo", nullptr},
                                                                         {"lateNonCriticalExtension", nullptr},
                                                                         {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sys_info_ies_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(sys_info_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc sys_info_ies_s::desc = {
    {seq_table_sys_info_ies_s_fields, 3, UINT64_C(0x1), seq_table_sys_info_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_sys_info_ies_s_names};

static const asn1::table::storage_ops* const choice_storage_sys_info_ies_s_sib_type_and_info_l_item_c__alternatives[] =
    {&asn1::table::alternative_ops<sib2_s, false>::value,
     &asn1::table::alternative_ops<sib3_s, false>::value,
     &asn1::table::alternative_ops<sib4_s, false>::value,
     &asn1::table::alternative_ops<sib5_s, false>::value,
     &asn1::table::alternative_ops<sib6_s, false>::value,
     &asn1::table::alternative_ops<sib7_s, false>::value,
     &asn1::table::alternative_ops<sib8_s, false>::value,
     &asn1::table::alternative_ops<sib9_s, false>::value,
     &asn1::table::alternative_ops<sib10_r16_s, false>::value,
     &asn1::table::alternative_ops<sib11_r16_s, false>::value,
     &asn1::table::alternative_ops<sib12_r16_s, false>::value,
     &asn1::table::alternative_ops<sib13_r16_s, false>::value,
     &asn1::table::alternative_ops<sib14_r16_s, false>::value,
     &asn1::table::alternative_ops<sib15_r17_s, false>::value,
     &asn1::table::alternative_ops<sib16_r17_s, false>::value,
     &asn1::table::alternative_ops<sib17_r17_s, false>::value,
     &asn1::table::alternative_ops<sib18_r17_s, false>::value,
     &asn1::table::alternative_ops<sib19_r17_s, false>::value,
     &asn1::table::alternative_ops<sib20_r17_s, false>::value,
     &asn1::table::alternative_ops<sib21_r17_s, false>::value,
     &asn1::table::alternative_ops<sib22_r18_s, false>::value,
     &asn1::table::alternative_ops<sib23_r18_s, false>::value,
     &asn1::table::alternative_ops<sib24_r18_s, false>::value,
     &asn1::table::alternative_ops<sib25_r18_s, false>::value,
     &asn1::table::alternative_ops<sib17bis_r18_s, false>::value};
const asn1::table::choice_storage_desc sys_info_ies_s::sib_type_and_info_l_item_c_::storage_desc = {
    choice_storage_sys_info_ies_s_sib_type_and_info_l_item_c__alternatives,
    25,
    "sys_info_ies_s::sib_type_and_info_l_item_c_"};

sib2_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib2()
{
  set(types::sib2);
  return c.get<sib2_s>();
}
sib3_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib3()
{
  set(types::sib3);
  return c.get<sib3_s>();
}
sib4_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib4()
{
  set(types::sib4);
  return c.get<sib4_s>();
}
sib5_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib5()
{
  set(types::sib5);
  return c.get<sib5_s>();
}
sib6_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib6()
{
  set(types::sib6);
  return c.get<sib6_s>();
}
sib7_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib7()
{
  set(types::sib7);
  return c.get<sib7_s>();
}
sib8_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib8()
{
  set(types::sib8);
  return c.get<sib8_s>();
}
sib9_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib9()
{
  set(types::sib9);
  return c.get<sib9_s>();
}
sib10_r16_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib10_v1610()
{
  set(types::sib10_v1610);
  return c.get<sib10_r16_s>();
}
sib11_r16_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib11_v1610()
{
  set(types::sib11_v1610);
  return c.get<sib11_r16_s>();
}
sib12_r16_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib12_v1610()
{
  set(types::sib12_v1610);
  return c.get<sib12_r16_s>();
}
sib13_r16_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib13_v1610()
{
  set(types::sib13_v1610);
  return c.get<sib13_r16_s>();
}
sib14_r16_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib14_v1610()
{
  set(types::sib14_v1610);
  return c.get<sib14_r16_s>();
}
sib15_r17_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib15_v1700()
{
  set(types::sib15_v1700);
  return c.get<sib15_r17_s>();
}
sib16_r17_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib16_v1700()
{
  set(types::sib16_v1700);
  return c.get<sib16_r17_s>();
}
sib17_r17_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib17_v1700()
{
  set(types::sib17_v1700);
  return c.get<sib17_r17_s>();
}
sib18_r17_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib18_v1700()
{
  set(types::sib18_v1700);
  return c.get<sib18_r17_s>();
}
sib19_r17_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib19_v1700()
{
  set(types::sib19_v1700);
  return c.get<sib19_r17_s>();
}
sib20_r17_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib20_v1700()
{
  set(types::sib20_v1700);
  return c.get<sib20_r17_s>();
}
sib21_r17_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib21_v1700()
{
  set(types::sib21_v1700);
  return c.get<sib21_r17_s>();
}
sib22_r18_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib22_v1800()
{
  set(types::sib22_v1800);
  return c.get<sib22_r18_s>();
}
sib23_r18_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib23_v1800()
{
  set(types::sib23_v1800);
  return c.get<sib23_r18_s>();
}
sib24_r18_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib24_v1800()
{
  set(types::sib24_v1800);
  return c.get<sib24_r18_s>();
}
sib25_r18_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib25_v1800()
{
  set(types::sib25_v1800);
  return c.get<sib25_r18_s>();
}
sib17bis_r18_s& sys_info_ies_s::sib_type_and_info_l_item_c_::set_sib17bis_v1820()
{
  set(types::sib17bis_v1820);
  return c.get<sib17bis_r18_s>();
}
static const asn1::table::seq_field choice_table_sys_info_ies_s_sib_type_and_info_l_item_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &sib2_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &sib3_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &sib4_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &sib5_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &sib6_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &sib7_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &sib8_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::sequence, false, false, 0, 0, nullptr, &sib9_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib10_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib11_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib12_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib13_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib14_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib15_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib16_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib17_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib18_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib19_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib20_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib21_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib22_r18_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib23_r18_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib24_r18_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib25_r18_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib17bis_r18_s::desc}};
static const asn1::table::field_name choice_table_sys_info_ies_s_sib_type_and_info_l_item_c__names[] = {
    {"sib2", nullptr},          {"sib3", nullptr},        {"sib4", nullptr},        {"sib5", nullptr},
    {"sib6", nullptr},          {"sib7", nullptr},        {"sib8", nullptr},        {"sib9", nullptr},
    {"sib10-v1610", nullptr},   {"sib11-v1610", nullptr}, {"sib12-v1610", nullptr}, {"sib13-v1610", nullptr},
    {"sib14-v1610", nullptr},   {"sib15-v1700", nullptr}, {"sib16-v1700", nullptr}, {"sib17-v1700", nullptr},
    {"sib18-v1700", nullptr},   {"sib19-v1700", nullptr}, {"sib20-v1700", nullptr}, {"sib21-v1700", nullptr},
    {"sib22-v1800", nullptr},   {"sib23-v1800", nullptr}, {"sib24-v1800", nullptr}, {"sib25-v1800", nullptr},
    {"sib17bis-v1820", nullptr}};
const asn1::table::choice_desc sys_info_ies_s::sib_type_and_info_l_item_c_::desc = {
    asn1::table::enum_int_type<sys_info_ies_s::sib_type_and_info_l_item_c_::types>(),
    sys_info_ies_s::sib_type_and_info_l_item_c_::types::nof_types,
    sys_info_ies_s::sib_type_and_info_l_item_c_::types::nof_exts,
    sys_info_ies_s::sib_type_and_info_l_item_c_::types::has_ext,
    false,
    choice_table_sys_info_ies_s_sib_type_and_info_l_item_c__alternatives,
    nullptr,
    choice_table_sys_info_ies_s_sib_type_and_info_l_item_c__names,
    "sys_info_ies_s::sib_type_and_info_l_item_c_",
    offsetof(sys_info_ies_s::sib_type_and_info_l_item_c_, c.type),
    offsetof(sys_info_ies_s::sib_type_and_info_l_item_c_, c.value),
    false,
    &sys_info_ies_s::sib_type_and_info_l_item_c_::storage_desc};

static const char* enum_names_sys_info_ies_s_sib_type_and_info_l_item_c__types_values[] = {
    "sib2",        "sib3",        "sib4",        "sib5",          "sib6",        "sib7",        "sib8",
    "sib9",        "sib10-v1610", "sib11-v1610", "sib12-v1610",   "sib13-v1610", "sib14-v1610", "sib15-v1700",
    "sib16-v1700", "sib17-v1700", "sib18-v1700", "sib19-v1700",   "sib20-v1700", "sib21-v1700", "sib22-v1800",
    "sib23-v1800", "sib24-v1800", "sib25-v1800", "sib17bis-v1820"};
const asn1::table::enum_desc sys_info_ies_s::sib_type_and_info_l_item_c_::types_opts::desc = {
    enum_names_sys_info_ies_s_sib_type_and_info_l_item_c__types_values,
    25,
    "sys_info_ies_s::sib_type_and_info_l_item_c_::types"};

// SIBpos-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib_pos_r16_s_fields[] = {
    {offsetof(sib_pos_r16_s, assist_data_sib_elem_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib_pos_r16_s::assist_data_sib_elem_r16), asn1::table::string_json>::value},
    {offsetof(sib_pos_r16_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib_pos_r16_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib_pos_r16_s_names[] = {{"assistanceDataSIB-Element-r16", nullptr},
                                                                        {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib_pos_r16_s_opts[]  = {{1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          sib_pos_r16_s::desc             = {
    {seq_table_sib_pos_r16_s_fields, 2, UINT64_C(0x1), seq_table_sib_pos_r16_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_sib_pos_r16_s_names};

// PosSystemInformation-r16-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_pos_sys_info_r16_ies_s_fields[] = {
    {offsetof(pos_sys_info_r16_ies_s, pos_sib_type_and_info_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::desc},
    {offsetof(pos_sys_info_r16_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(pos_sys_info_r16_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(pos_sys_info_r16_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_pos_sys_info_r16_ies_s_names[] = {{"posSIB-TypeAndInfo-r16", nullptr},
                                                                                 {"lateNonCriticalExtension", nullptr},
                                                                                 {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_pos_sys_info_r16_ies_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(pos_sys_info_r16_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc pos_sys_info_r16_ies_s::desc = {
    {seq_table_pos_sys_info_r16_ies_s_fields, 3, UINT64_C(0x1), seq_table_pos_sys_info_r16_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_pos_sys_info_r16_ies_s_names};

static const asn1::table::storage_ops* const
    choice_storage_pos_sys_info_r16_ies_s_pos_sib_type_and_info_r16_l_item_c__alternatives[] = {
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value,
        &asn1::table::alternative_ops<sib_pos_r16_s, false>::value};
const asn1::table::choice_storage_desc pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::storage_desc = {
    choice_storage_pos_sys_info_r16_ies_s_pos_sib_type_and_info_r16_l_item_c__alternatives,
    56,
    "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_"};

sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_1_r16()
{
  set(types::pos_sib1_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_2_r16()
{
  set(types::pos_sib1_2_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_3_r16()
{
  set(types::pos_sib1_3_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_4_r16()
{
  set(types::pos_sib1_4_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_5_r16()
{
  set(types::pos_sib1_5_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_6_r16()
{
  set(types::pos_sib1_6_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_7_r16()
{
  set(types::pos_sib1_7_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_8_r16()
{
  set(types::pos_sib1_8_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_1_r16()
{
  set(types::pos_sib2_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_2_r16()
{
  set(types::pos_sib2_2_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_3_r16()
{
  set(types::pos_sib2_3_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_4_r16()
{
  set(types::pos_sib2_4_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_5_r16()
{
  set(types::pos_sib2_5_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_6_r16()
{
  set(types::pos_sib2_6_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_7_r16()
{
  set(types::pos_sib2_7_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_8_r16()
{
  set(types::pos_sib2_8_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_9_r16()
{
  set(types::pos_sib2_9_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_10_r16()
{
  set(types::pos_sib2_10_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_11_r16()
{
  set(types::pos_sib2_11_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_12_r16()
{
  set(types::pos_sib2_12_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_13_r16()
{
  set(types::pos_sib2_13_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_14_r16()
{
  set(types::pos_sib2_14_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_15_r16()
{
  set(types::pos_sib2_15_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_16_r16()
{
  set(types::pos_sib2_16_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_17_r16()
{
  set(types::pos_sib2_17_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_18_r16()
{
  set(types::pos_sib2_18_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_19_r16()
{
  set(types::pos_sib2_19_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_20_r16()
{
  set(types::pos_sib2_20_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_21_r16()
{
  set(types::pos_sib2_21_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_22_r16()
{
  set(types::pos_sib2_22_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_23_r16()
{
  set(types::pos_sib2_23_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib3_1_r16()
{
  set(types::pos_sib3_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib4_1_r16()
{
  set(types::pos_sib4_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib5_1_r16()
{
  set(types::pos_sib5_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib6_1_r16()
{
  set(types::pos_sib6_1_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib6_2_r16()
{
  set(types::pos_sib6_2_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib6_3_r16()
{
  set(types::pos_sib6_3_r16);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_9_v1700()
{
  set(types::pos_sib1_9_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_10_v1700()
{
  set(types::pos_sib1_10_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_24_v1700()
{
  set(types::pos_sib2_24_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_25_v1700()
{
  set(types::pos_sib2_25_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib6_4_v1700()
{
  set(types::pos_sib6_4_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib6_5_v1700()
{
  set(types::pos_sib6_5_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib6_6_v1700()
{
  set(types::pos_sib6_6_v1700);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_17a_v1770()
{
  set(types::pos_sib2_17a_v1770);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_18a_v1770()
{
  set(types::pos_sib2_18a_v1770);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_20a_v1770()
{
  set(types::pos_sib2_20a_v1770);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_11_v1800()
{
  set(types::pos_sib1_11_v1800);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib1_12_v1800()
{
  set(types::pos_sib1_12_v1800);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_26_v1800()
{
  set(types::pos_sib2_26_v1800);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib2_27_v1800()
{
  set(types::pos_sib2_27_v1800);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib6_7_v1800()
{
  set(types::pos_sib6_7_v1800);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib7_1_v1800()
{
  set(types::pos_sib7_1_v1800);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib7_2_v1800()
{
  set(types::pos_sib7_2_v1800);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib7_3_v1800()
{
  set(types::pos_sib7_3_v1800);
  return c.get<sib_pos_r16_s>();
}
sib_pos_r16_s& pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::set_pos_sib7_4_v1800()
{
  set(types::pos_sib7_4_v1800);
  return c.get<sib_pos_r16_s>();
}
static const asn1::table::seq_field
    choice_table_pos_sys_info_r16_ies_s_pos_sib_type_and_info_r16_l_item_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &sib_pos_r16_s::desc}};
static const asn1::table::field_name choice_table_pos_sys_info_r16_ies_s_pos_sib_type_and_info_r16_l_item_c__names[] = {
    {"posSib1-1-r16", nullptr},     {"posSib1-2-r16", nullptr},     {"posSib1-3-r16", nullptr},
    {"posSib1-4-r16", nullptr},     {"posSib1-5-r16", nullptr},     {"posSib1-6-r16", nullptr},
    {"posSib1-7-r16", nullptr},     {"posSib1-8-r16", nullptr},     {"posSib2-1-r16", nullptr},
    {"posSib2-2-r16", nullptr},     {"posSib2-3-r16", nullptr},     {"posSib2-4-r16", nullptr},
    {"posSib2-5-r16", nullptr},     {"posSib2-6-r16", nullptr},     {"posSib2-7-r16", nullptr},
    {"posSib2-8-r16", nullptr},     {"posSib2-9-r16", nullptr},     {"posSib2-10-r16", nullptr},
    {"posSib2-11-r16", nullptr},    {"posSib2-12-r16", nullptr},    {"posSib2-13-r16", nullptr},
    {"posSib2-14-r16", nullptr},    {"posSib2-15-r16", nullptr},    {"posSib2-16-r16", nullptr},
    {"posSib2-17-r16", nullptr},    {"posSib2-18-r16", nullptr},    {"posSib2-19-r16", nullptr},
    {"posSib2-20-r16", nullptr},    {"posSib2-21-r16", nullptr},    {"posSib2-22-r16", nullptr},
    {"posSib2-23-r16", nullptr},    {"posSib3-1-r16", nullptr},     {"posSib4-1-r16", nullptr},
    {"posSib5-1-r16", nullptr},     {"posSib6-1-r16", nullptr},     {"posSib6-2-r16", nullptr},
    {"posSib6-3-r16", nullptr},     {"posSib1-9-v1700", nullptr},   {"posSib1-10-v1700", nullptr},
    {"posSib2-24-v1700", nullptr},  {"posSib2-25-v1700", nullptr},  {"posSib6-4-v1700", nullptr},
    {"posSib6-5-v1700", nullptr},   {"posSib6-6-v1700", nullptr},   {"posSib2-17a-v1770", nullptr},
    {"posSib2-18a-v1770", nullptr}, {"posSib2-20a-v1770", nullptr}, {"posSib1-11-v1800", nullptr},
    {"posSib1-12-v1800", nullptr},  {"posSib2-26-v1800", nullptr},  {"posSib2-27-v1800", nullptr},
    {"posSib6-7-v1800", nullptr},   {"posSib7-1-v1800", nullptr},   {"posSib7-2-v1800", nullptr},
    {"posSib7-3-v1800", nullptr},   {"posSib7-4-v1800", nullptr}};
const asn1::table::choice_desc pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::desc = {
    asn1::table::enum_int_type<pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::types>(),
    pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::types::nof_types,
    pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::types::nof_exts,
    pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::types::has_ext,
    false,
    choice_table_pos_sys_info_r16_ies_s_pos_sib_type_and_info_r16_l_item_c__alternatives,
    nullptr,
    choice_table_pos_sys_info_r16_ies_s_pos_sib_type_and_info_r16_l_item_c__names,
    "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_",
    offsetof(pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_, c.type),
    offsetof(pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_, c.value),
    false,
    &pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::storage_desc};

static const char* enum_names_pos_sys_info_r16_ies_s_pos_sib_type_and_info_r16_l_item_c__types_values[] = {
    "posSib1-1-r16",     "posSib1-2-r16",     "posSib1-3-r16",    "posSib1-4-r16",    "posSib1-5-r16",
    "posSib1-6-r16",     "posSib1-7-r16",     "posSib1-8-r16",    "posSib2-1-r16",    "posSib2-2-r16",
    "posSib2-3-r16",     "posSib2-4-r16",     "posSib2-5-r16",    "posSib2-6-r16",    "posSib2-7-r16",
    "posSib2-8-r16",     "posSib2-9-r16",     "posSib2-10-r16",   "posSib2-11-r16",   "posSib2-12-r16",
    "posSib2-13-r16",    "posSib2-14-r16",    "posSib2-15-r16",   "posSib2-16-r16",   "posSib2-17-r16",
    "posSib2-18-r16",    "posSib2-19-r16",    "posSib2-20-r16",   "posSib2-21-r16",   "posSib2-22-r16",
    "posSib2-23-r16",    "posSib3-1-r16",     "posSib4-1-r16",    "posSib5-1-r16",    "posSib6-1-r16",
    "posSib6-2-r16",     "posSib6-3-r16",     "posSib1-9-v1700",  "posSib1-10-v1700", "posSib2-24-v1700",
    "posSib2-25-v1700",  "posSib6-4-v1700",   "posSib6-5-v1700",  "posSib6-6-v1700",  "posSib2-17a-v1770",
    "posSib2-18a-v1770", "posSib2-20a-v1770", "posSib1-11-v1800", "posSib1-12-v1800", "posSib2-26-v1800",
    "posSib2-27-v1800",  "posSib6-7-v1800",   "posSib7-1-v1800",  "posSib7-2-v1800",  "posSib7-3-v1800",
    "posSib7-4-v1800"};
const asn1::table::enum_desc pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::types_opts::desc = {
    enum_names_pos_sys_info_r16_ies_s_pos_sib_type_and_info_r16_l_item_c__types_values,
    56,
    "pos_sys_info_r16_ies_s::pos_sib_type_and_info_r16_l_item_c_::types"};

// SystemInformation ::= SEQUENCE
static const asn1::table::seq_field  seq_table_sys_info_s_fields[] = {{offsetof(sys_info_s, crit_exts),
                                                                       asn1::table::seq_field::mandatory,
                                                                       asn1::table::field_op::choice,
                                                                       false,
                                                                       false,
                                                                       0,
                                                                       0,
                                                                       nullptr,
                                                                       &sys_info_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_sys_info_s_names[]  = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          sys_info_s::desc =
    {{seq_table_sys_info_s_fields, 1, UINT64_C(0x1), nullptr, 0}, false, false, nullptr, 0, seq_table_sys_info_s_names};

static const asn1::table::storage_ops* const choice_storage_sys_info_s_crit_exts_c__alternatives[] = {
    &asn1::table::alternative_ops<sys_info_ies_s, false>::value,
    &asn1::table::alternative_ops<sys_info_s::crit_exts_c_::crit_exts_future_r16_c_, false>::value};
const asn1::table::choice_storage_desc sys_info_s::crit_exts_c_::storage_desc = {
    choice_storage_sys_info_s_crit_exts_c__alternatives,
    2,
    "sys_info_s::crit_exts_c_"};

sys_info_ies_s& sys_info_s::crit_exts_c_::set_sys_info()
{
  set(types::sys_info);
  return c.get<sys_info_ies_s>();
}
sys_info_s::crit_exts_c_::crit_exts_future_r16_c_& sys_info_s::crit_exts_c_::set_crit_exts_future_r16()
{
  set(types::crit_exts_future_r16);
  return c.get<crit_exts_future_r16_c_>();
}
static const asn1::table::seq_field choice_table_sys_info_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sys_info_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::desc}};
static const asn1::table::field_name choice_table_sys_info_s_crit_exts_c__names[] = {
    {"systemInformation", nullptr},
    {"criticalExtensionsFuture-r16", nullptr}};
const asn1::table::choice_desc sys_info_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<sys_info_s::crit_exts_c_::types>(),
    sys_info_s::crit_exts_c_::types::nof_types,
    sys_info_s::crit_exts_c_::types::nof_exts,
    sys_info_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_sys_info_s_crit_exts_c__alternatives,
    nullptr,
    choice_table_sys_info_s_crit_exts_c__names,
    "sys_info_s::crit_exts_c_",
    offsetof(sys_info_s::crit_exts_c_, c.type),
    offsetof(sys_info_s::crit_exts_c_, c.value),
    false,
    &sys_info_s::crit_exts_c_::storage_desc};

void sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::set(types::options e)
{
  type_ = e;
}
pos_sys_info_r16_ies_s& sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::set_pos_sys_info_r16()
{
  set(types::pos_sys_info_r16);
  return c;
}
void sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_sys_info_s_crit_exts_c__crit_exts_future_r16_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pos_sys_info_r16_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_sys_info_s_crit_exts_c__crit_exts_future_r16_c__names[] = {
    {"posSystemInformation-r16", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::desc = {
    asn1::table::enum_int_type<sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::types>(),
    sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::types::nof_types,
    sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::types::nof_exts,
    sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::types::has_ext,
    false,
    choice_table_sys_info_s_crit_exts_c__crit_exts_future_r16_c__alternatives,
    &asn1::table::choice_set<sys_info_s::crit_exts_c_::crit_exts_future_r16_c_>,
    choice_table_sys_info_s_crit_exts_c__crit_exts_future_r16_c__names,
    "sys_info_s::crit_exts_c_::crit_exts_future_r16_c_",
    offsetof(sys_info_s::crit_exts_c_::crit_exts_future_r16_c_, type_),
    offsetof(sys_info_s::crit_exts_c_::crit_exts_future_r16_c_, c),
    false};

static const char* enum_names_sys_info_s_crit_exts_c__crit_exts_future_r16_c__types_values[] = {
    "posSystemInformation-r16",
    "criticalExtensionsFuture"};
const asn1::table::enum_desc sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::types_opts::desc = {
    enum_names_sys_info_s_crit_exts_c__crit_exts_future_r16_c__types_values,
    2,
    "sys_info_s::crit_exts_c_::crit_exts_future_r16_c_::types"};

static const char*           enum_names_sys_info_s_crit_exts_c__types_values[] = {"systemInformation",
                                                                                  "criticalExtensionsFuture-r16"};
const asn1::table::enum_desc sys_info_s::crit_exts_c_::types_opts::desc        = {
    enum_names_sys_info_s_crit_exts_c__types_values,
    2,
    "sys_info_s::crit_exts_c_::types"};

// SNPN-AccessInfo-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sn_pn_access_info_r17_s_fields[] = {
    {0,
     (int32_t)offsetof(sn_pn_access_info_r17_s, ext_ch_supported_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sn_pn_access_info_r17_s, ext_ch_without_cfg_allowed_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sn_pn_access_info_r17_s, onboarding_enabled_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sn_pn_access_info_r17_s, ims_emergency_support_for_sn_pn_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sn_pn_access_info_r17_s_names[] = {
    {"extCH-Supported-r17", "true"},
    {"extCH-WithoutConfigAllowed-r17", "true"},
    {"onboardingEnabled-r17", "true"},
    {"imsEmergencySupportForSNPN-r17", "true"}};
static const asn1::table::seq_opt seq_table_sn_pn_access_info_r17_s_opts[] = {
    {0, (int32_t)offsetof(sn_pn_access_info_r17_s, ext_ch_supported_r17_present)},
    {1, (int32_t)offsetof(sn_pn_access_info_r17_s, ext_ch_without_cfg_allowed_r17_present)},
    {2, (int32_t)offsetof(sn_pn_access_info_r17_s, onboarding_enabled_r17_present)},
    {3, (int32_t)offsetof(sn_pn_access_info_r17_s, ims_emergency_support_for_sn_pn_r17_present)}};
const asn1::table::seq_desc sn_pn_access_info_r17_s::desc = {
    {seq_table_sn_pn_access_info_r17_s_fields, 4, UINT64_C(0x0), seq_table_sn_pn_access_info_r17_s_opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_sn_pn_access_info_r17_s_names};

// CellAccessRelatedInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_access_related_info_s_fields[] = {
    {offsetof(cell_access_related_info_s, plmn_id_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::item_list_codec<decltype(cell_access_related_info_s::plmn_id_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &plmn_id_info_s::desc},
    {0,
     (int32_t)offsetof(cell_access_related_info_s, cell_reserved_for_other_use_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_cell_access_related_info_s_names[] = {
    {"plmn-IdentityInfoList", nullptr},
    {"cellReservedForOtherUse", "true"}};
static const asn1::table::seq_opt seq_table_cell_access_related_info_s_opts[] = {
    {1, (int32_t)offsetof(cell_access_related_info_s, cell_reserved_for_other_use_present)}};
static const asn1::table::seq_field seq_table_cell_access_related_info_s_g0_fields[] = {
    {0,
     (int32_t)offsetof(cell_access_related_info_s, cell_reserved_for_future_use_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(cell_access_related_info_s, npn_id_info_list_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::ptr_item_list_codec<decltype(cell_access_related_info_s::npn_id_info_list_r16),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &npn_id_info_r16_s::desc}};
static const asn1::table::field_name seq_table_cell_access_related_info_s_g0_names[] = {
    {"cellReservedForFutureUse-r16", "true"},
    {"npn-IdentityInfoList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_cell_access_related_info_s_g0_opts[] = {
    {0, (int32_t)offsetof(cell_access_related_info_s, cell_reserved_for_future_use_r16_present)},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_cell_access_related_info_s_g1_fields[] = {
    {offsetof(cell_access_related_info_s, snpn_access_info_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::ptr_item_list_codec<decltype(cell_access_related_info_s::snpn_access_info_list_r17),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sn_pn_access_info_r17_s::desc}};
static const asn1::table::field_name seq_table_cell_access_related_info_s_g1_names[] = {
    {"snpn-AccessInfoList-r17", nullptr}};
static const asn1::table::seq_opt seq_table_cell_access_related_info_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_cell_access_related_info_s_groups[] = {
    {{seq_table_cell_access_related_info_s_g0_fields,
      2,
      UINT64_C(0x0),
      seq_table_cell_access_related_info_s_g0_opts,
      2},
     false,
     seq_table_cell_access_related_info_s_g0_names},
    {{seq_table_cell_access_related_info_s_g1_fields,
      1,
      UINT64_C(0x0),
      seq_table_cell_access_related_info_s_g1_opts,
      1},
     false,
     seq_table_cell_access_related_info_s_g1_names}};
const asn1::table::seq_desc cell_access_related_info_s::desc = {
    {seq_table_cell_access_related_info_s_fields, 2, UINT64_C(0x1), seq_table_cell_access_related_info_s_opts, 1},
    true,
    false,
    seq_table_cell_access_related_info_s_groups,
    2,
    seq_table_cell_access_related_info_s_names};

// ConnEstFailureControl ::= SEQUENCE
static const asn1::table::seq_field seq_table_conn_est_fail_ctrl_s_fields[] = {
    {offsetof(conn_est_fail_ctrl_s, conn_est_fail_count),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(conn_est_fail_ctrl_s::conn_est_fail_count)>(),
     decltype(conn_est_fail_ctrl_s::conn_est_fail_count)::has_ext,
     false,
     decltype(conn_est_fail_ctrl_s::conn_est_fail_count)::nof_types,
     decltype(conn_est_fail_ctrl_s::conn_est_fail_count)::nof_exts,
     nullptr,
     &conn_est_fail_ctrl_s::conn_est_fail_count_opts::desc},
    {offsetof(conn_est_fail_ctrl_s, conn_est_fail_offset_validity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(conn_est_fail_ctrl_s::conn_est_fail_offset_validity)>(),
     decltype(conn_est_fail_ctrl_s::conn_est_fail_offset_validity)::has_ext,
     false,
     decltype(conn_est_fail_ctrl_s::conn_est_fail_offset_validity)::nof_types,
     decltype(conn_est_fail_ctrl_s::conn_est_fail_offset_validity)::nof_exts,
     nullptr,
     &conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::desc},
    {offsetof(conn_est_fail_ctrl_s, conn_est_fail_offset),
     (int32_t)offsetof(conn_est_fail_ctrl_s, conn_est_fail_offset_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(15),
     nullptr}};
static const asn1::table::field_name seq_table_conn_est_fail_ctrl_s_names[] = {{"connEstFailCount", nullptr},
                                                                               {"connEstFailOffsetValidity", nullptr},
                                                                               {"connEstFailOffset", nullptr}};
static const asn1::table::seq_opt    seq_table_conn_est_fail_ctrl_s_opts[]  = {
    {2, (int32_t)offsetof(conn_est_fail_ctrl_s, conn_est_fail_offset_present)}};
const asn1::table::seq_desc conn_est_fail_ctrl_s::desc = {
    {seq_table_conn_est_fail_ctrl_s_fields, 3, UINT64_C(0x3), seq_table_conn_est_fail_ctrl_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_conn_est_fail_ctrl_s_names};

static const char*           enum_names_conn_est_fail_ctrl_s_conn_est_fail_count_e__values[] = {"n1", "n2", "n3", "n4"};
const asn1::table::enum_desc conn_est_fail_ctrl_s::conn_est_fail_count_opts::desc            = {
    enum_names_conn_est_fail_ctrl_s_conn_est_fail_count_e__values,
    4,
    "conn_est_fail_ctrl_s::conn_est_fail_count_e_"};
uint8_t conn_est_fail_ctrl_s::conn_est_fail_count_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "conn_est_fail_ctrl_s::conn_est_fail_count_e_");
}

static const char* enum_names_conn_est_fail_ctrl_s_conn_est_fail_offset_validity_e__values[] =
    {"s30", "s60", "s120", "s240", "s300", "s420", "s600", "s900"};
const asn1::table::enum_desc conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::desc = {
    enum_names_conn_est_fail_ctrl_s_conn_est_fail_offset_validity_e__values,
    8,
    "conn_est_fail_ctrl_s::conn_est_fail_offset_validity_e_"};
uint16_t conn_est_fail_ctrl_s::conn_est_fail_offset_validity_opts::to_number() const
{
  static const uint16_t numbers[] = {30, 60, 120, 240, 300, 420, 600, 900};
  return map_enum_number(numbers, 8, value, "conn_est_fail_ctrl_s::conn_est_fail_offset_validity_e_");
}

// SIB-TypeInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib_type_info_s_fields[] = {
    {offsetof(sib_type_info_s, type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sib_type_info_s::type)>(),
     decltype(sib_type_info_s::type)::has_ext,
     false,
     decltype(sib_type_info_s::type)::nof_types,
     decltype(sib_type_info_s::type)::nof_exts,
     nullptr,
     &sib_type_info_s::type_opts::desc},
    {offsetof(sib_type_info_s, value_tag),
     (int32_t)offsetof(sib_type_info_s, value_tag_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {0,
     (int32_t)offsetof(sib_type_info_s, area_scope_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sib_type_info_s_names[] = {{"type", nullptr},
                                                                          {"valueTag", nullptr},
                                                                          {"areaScope", "true"}};
static const asn1::table::seq_opt    seq_table_sib_type_info_s_opts[]  = {
    {1, (int32_t)offsetof(sib_type_info_s, value_tag_present)},
    {2, (int32_t)offsetof(sib_type_info_s, area_scope_present)}};
const asn1::table::seq_desc sib_type_info_s::desc = {
    {seq_table_sib_type_info_s_fields, 3, UINT64_C(0x1), seq_table_sib_type_info_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_sib_type_info_s_names};

static const char*           enum_names_sib_type_info_s_type_e__values[] = {"sibType2",
                                                                            "sibType3",
                                                                            "sibType4",
                                                                            "sibType5",
                                                                            "sibType6",
                                                                            "sibType7",
                                                                            "sibType8",
                                                                            "sibType9",
                                                                            "sibType10-v1610",
                                                                            "sibType11-v1610",
                                                                            "sibType12-v1610",
                                                                            "sibType13-v1610",
                                                                            "sibType14-v1610",
                                                                            "spare3",
                                                                            "spare2",
                                                                            "spare1"};
const asn1::table::enum_desc sib_type_info_s::type_opts::desc            = {enum_names_sib_type_info_s_type_e__values,
                                                                            16,
                                                                            "sib_type_info_s::type_e_"};
uint8_t                      sib_type_info_s::type_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
  return map_enum_number(numbers, 13, value, "sib_type_info_s::type_e_");
}

// SchedulingInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_sched_info_s_fields[] = {
    {offsetof(sched_info_s, si_broadcast_status),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sched_info_s::si_broadcast_status)>(),
     decltype(sched_info_s::si_broadcast_status)::has_ext,
     false,
     decltype(sched_info_s::si_broadcast_status)::nof_types,
     decltype(sched_info_s::si_broadcast_status)::nof_exts,
     nullptr,
     &sched_info_s::si_broadcast_status_opts::desc},
    {offsetof(sched_info_s, si_periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sched_info_s::si_periodicity)>(),
     decltype(sched_info_s::si_periodicity)::has_ext,
     false,
     decltype(sched_info_s::si_periodicity)::nof_types,
     decltype(sched_info_s::si_periodicity)::nof_exts,
     nullptr,
     &sched_info_s::si_periodicity_opts::desc},
    {offsetof(sched_info_s, sib_map_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(sched_info_s::sib_map_info), asn1::table::sequence_item_ops, false, true>::
         value,
     &sib_type_info_s::desc}};
static const asn1::table::field_name seq_table_sched_info_s_names[] = {{"si-BroadcastStatus", nullptr},
                                                                       {"si-Periodicity", nullptr},
                                                                       {"sib-MappingInfo", nullptr}};
const asn1::table::seq_desc sched_info_s::desc = {{seq_table_sched_info_s_fields, 3, UINT64_C(0x7), nullptr, 0},
                                                  false,
                                                  false,
                                                  nullptr,
                                                  0,
                                                  seq_table_sched_info_s_names};

static const char* enum_names_sched_info_s_si_broadcast_status_e__values[] = {"broadcasting", "notBroadcasting"};
const asn1::table::enum_desc sched_info_s::si_broadcast_status_opts::desc  = {
    enum_names_sched_info_s_si_broadcast_status_e__values,
    2,
    "sched_info_s::si_broadcast_status_e_"};

static const char* enum_names_sched_info_s_si_periodicity_e__values[] =
    {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
const asn1::table::enum_desc sched_info_s::si_periodicity_opts::desc = {
    enum_names_sched_info_s_si_periodicity_e__values,
    7,
    "sched_info_s::si_periodicity_e_"};
uint16_t sched_info_s::si_periodicity_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 7, value, "sched_info_s::si_periodicity_e_");
}

// SI-RequestResources ::= SEQUENCE
static const asn1::table::seq_field seq_table_si_request_res_s_fields[] = {
    {offsetof(si_request_res_s, ra_preamb_start_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(si_request_res_s, ra_assoc_period_idx),
     (int32_t)offsetof(si_request_res_s, ra_assoc_period_idx_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {offsetof(si_request_res_s, ra_ssb_occasion_mask_idx),
     (int32_t)offsetof(si_request_res_s, ra_ssb_occasion_mask_idx_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(15),
     nullptr}};
static const asn1::table::field_name seq_table_si_request_res_s_names[] = {{"ra-PreambleStartIndex", nullptr},
                                                                           {"ra-AssociationPeriodIndex", nullptr},
                                                                           {"ra-ssb-OccasionMaskIndex", nullptr}};
static const asn1::table::seq_opt    seq_table_si_request_res_s_opts[]  = {
    {1, (int32_t)offsetof(si_request_res_s, ra_assoc_period_idx_present)},
    {2, (int32_t)offsetof(si_request_res_s, ra_ssb_occasion_mask_idx_present)}};
const asn1::table::seq_desc si_request_res_s::desc = {
    {seq_table_si_request_res_s_fields, 3, UINT64_C(0x1), seq_table_si_request_res_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_si_request_res_s_names};

// SI-RequestConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_si_request_cfg_s_rach_occasions_si_s__fields[] = {
    {offsetof(si_request_cfg_s::rach_occasions_si_s_, rach_cfg_si),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rach_cfg_generic_s::desc},
    {offsetof(si_request_cfg_s::rach_occasions_si_s_, ssb_per_rach_occasion),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion)>(),
     decltype(si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion)::has_ext,
     false,
     decltype(si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion)::nof_types,
     decltype(si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion)::nof_exts,
     nullptr,
     &si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::desc}};
static const asn1::table::field_name seq_table_si_request_cfg_s_rach_occasions_si_s__names[] = {
    {"rach-ConfigSI", nullptr},
    {"ssb-perRACH-Occasion", nullptr}};
static const asn1::table::seq_desc seq_table_si_request_cfg_s_rach_occasions_si_s_ = {
    {seq_table_si_request_cfg_s_rach_occasions_si_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_si_request_cfg_s_rach_occasions_si_s__names};
static const asn1::table::seq_field seq_table_si_request_cfg_s_fields[] = {
    {offsetof(si_request_cfg_s, rach_occasions_si),
     (int32_t)offsetof(si_request_cfg_s, rach_occasions_si_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_si_request_cfg_s_rach_occasions_si_s_},
    {offsetof(si_request_cfg_s, si_request_period),
     (int32_t)offsetof(si_request_cfg_s, si_request_period_present),
     asn1::table::enum_op<decltype(si_request_cfg_s::si_request_period)>(),
     decltype(si_request_cfg_s::si_request_period)::has_ext,
     false,
     decltype(si_request_cfg_s::si_request_period)::nof_types,
     decltype(si_request_cfg_s::si_request_period)::nof_exts,
     nullptr,
     &si_request_cfg_s::si_request_period_opts::desc},
    {offsetof(si_request_cfg_s, si_request_res),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(si_request_cfg_s::si_request_res),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &si_request_res_s::desc}};
static const asn1::table::field_name seq_table_si_request_cfg_s_names[] = {{"rach-OccasionsSI", nullptr},
                                                                           {"si-RequestPeriod", nullptr},
                                                                           {"si-RequestResources", nullptr}};
static const asn1::table::seq_opt    seq_table_si_request_cfg_s_opts[]  = {
    {0, (int32_t)offsetof(si_request_cfg_s, rach_occasions_si_present)},
    {1, (int32_t)offsetof(si_request_cfg_s, si_request_period_present)}};
const asn1::table::seq_desc si_request_cfg_s::desc = {
    {seq_table_si_request_cfg_s_fields, 3, UINT64_C(0x4), seq_table_si_request_cfg_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_si_request_cfg_s_names};

static const char* enum_names_si_request_cfg_s_rach_occasions_si_s__ssb_per_rach_occasion_e__values[] =
    {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
const asn1::table::enum_desc si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::desc = {
    enum_names_si_request_cfg_s_rach_occasions_si_s__ssb_per_rach_occasion_e__values,
    8,
    "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_"};
float si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(numbers, 8, value, "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_");
}
const char* si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(number_strs, 8, value, "si_request_cfg_s::rach_occasions_si_s_::ssb_per_rach_occasion_e_");
}

static const char* enum_names_si_request_cfg_s_si_request_period_e__values[] =
    {"one", "two", "four", "six", "eight", "ten", "twelve", "sixteen"};
const asn1::table::enum_desc si_request_cfg_s::si_request_period_opts::desc = {
    enum_names_si_request_cfg_s_si_request_period_e__values,
    8,
    "si_request_cfg_s::si_request_period_e_"};

// SI-SchedulingInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_si_sched_info_s_fields[] = {
    {offsetof(si_sched_info_s, sched_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(si_sched_info_s::sched_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sched_info_s::desc},
    {offsetof(si_sched_info_s, si_win_len),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(si_sched_info_s::si_win_len)>(),
     decltype(si_sched_info_s::si_win_len)::has_ext,
     false,
     decltype(si_sched_info_s::si_win_len)::nof_types,
     decltype(si_sched_info_s::si_win_len)::nof_exts,
     nullptr,
     &si_sched_info_s::si_win_len_opts::desc},
    {offsetof(si_sched_info_s, si_request_cfg),
     (int32_t)offsetof(si_sched_info_s, si_request_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_request_cfg_s::desc},
    {offsetof(si_sched_info_s, si_request_cfg_sul),
     (int32_t)offsetof(si_sched_info_s, si_request_cfg_sul_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_request_cfg_s::desc},
    {offsetof(si_sched_info_s, sys_info_area_id),
     (int32_t)offsetof(si_sched_info_s, sys_info_area_id_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(si_sched_info_s::sys_info_area_id), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_si_sched_info_s_names[] = {{"schedulingInfoList", nullptr},
                                                                          {"si-WindowLength", nullptr},
                                                                          {"si-RequestConfig", nullptr},
                                                                          {"si-RequestConfigSUL", nullptr},
                                                                          {"systemInformationAreaID", nullptr}};
static const asn1::table::seq_opt    seq_table_si_sched_info_s_opts[]  = {
    {2, (int32_t)offsetof(si_sched_info_s, si_request_cfg_present)},
    {3, (int32_t)offsetof(si_sched_info_s, si_request_cfg_sul_present)},
    {4, (int32_t)offsetof(si_sched_info_s, sys_info_area_id_present)}};
const asn1::table::seq_desc si_sched_info_s::desc = {
    {seq_table_si_sched_info_s_fields, 5, UINT64_C(0x3), seq_table_si_sched_info_s_opts, 3},
    true,
    false,
    nullptr,
    0,
    seq_table_si_sched_info_s_names};

static const char* enum_names_si_sched_info_s_si_win_len_e__values[] =
    {"s5", "s10", "s20", "s40", "s80", "s160", "s320", "s640", "s1280", "s2560-v1710", "s5120-v1710"};
const asn1::table::enum_desc si_sched_info_s::si_win_len_opts::desc = {enum_names_si_sched_info_s_si_win_len_e__values,
                                                                       11,
                                                                       "si_sched_info_s::si_win_len_e_"};
uint16_t                     si_sched_info_s::si_win_len_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 40, 80, 160, 320, 640, 1280, 2560, 5120};
  return map_enum_number(numbers, 11, value, "si_sched_info_s::si_win_len_e_");
}

// UE-TimersAndConstants ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_timers_and_consts_s_fields[] = {
    {offsetof(ue_timers_and_consts_s, t300),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ue_timers_and_consts_s::t300)>(),
     decltype(ue_timers_and_consts_s::t300)::has_ext,
     false,
     decltype(ue_timers_and_consts_s::t300)::nof_types,
     decltype(ue_timers_and_consts_s::t300)::nof_exts,
     nullptr,
     &ue_timers_and_consts_s::t300_opts::desc},
    {offsetof(ue_timers_and_consts_s, t301),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ue_timers_and_consts_s::t301)>(),
     decltype(ue_timers_and_consts_s::t301)::has_ext,
     false,
     decltype(ue_timers_and_consts_s::t301)::nof_types,
     decltype(ue_timers_and_consts_s::t301)::nof_exts,
     nullptr,
     &ue_timers_and_consts_s::t301_opts::desc},
    {offsetof(ue_timers_and_consts_s, t310),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ue_timers_and_consts_s::t310)>(),
     decltype(ue_timers_and_consts_s::t310)::has_ext,
     false,
     decltype(ue_timers_and_consts_s::t310)::nof_types,
     decltype(ue_timers_and_consts_s::t310)::nof_exts,
     nullptr,
     &ue_timers_and_consts_s::t310_opts::desc},
    {offsetof(ue_timers_and_consts_s, n310),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ue_timers_and_consts_s::n310)>(),
     decltype(ue_timers_and_consts_s::n310)::has_ext,
     false,
     decltype(ue_timers_and_consts_s::n310)::nof_types,
     decltype(ue_timers_and_consts_s::n310)::nof_exts,
     nullptr,
     &ue_timers_and_consts_s::n310_opts::desc},
    {offsetof(ue_timers_and_consts_s, t311),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ue_timers_and_consts_s::t311)>(),
     decltype(ue_timers_and_consts_s::t311)::has_ext,
     false,
     decltype(ue_timers_and_consts_s::t311)::nof_types,
     decltype(ue_timers_and_consts_s::t311)::nof_exts,
     nullptr,
     &ue_timers_and_consts_s::t311_opts::desc},
    {offsetof(ue_timers_and_consts_s, n311),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ue_timers_and_consts_s::n311)>(),
     decltype(ue_timers_and_consts_s::n311)::has_ext,
     false,
     decltype(ue_timers_and_consts_s::n311)::nof_types,
     decltype(ue_timers_and_consts_s::n311)::nof_exts,
     nullptr,
     &ue_timers_and_consts_s::n311_opts::desc},
    {offsetof(ue_timers_and_consts_s, t319),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ue_timers_and_consts_s::t319)>(),
     decltype(ue_timers_and_consts_s::t319)::has_ext,
     false,
     decltype(ue_timers_and_consts_s::t319)::nof_types,
     decltype(ue_timers_and_consts_s::t319)::nof_exts,
     nullptr,
     &ue_timers_and_consts_s::t319_opts::desc}};
static const asn1::table::field_name seq_table_ue_timers_and_consts_s_names[] = {{"t300", nullptr},
                                                                                 {"t301", nullptr},
                                                                                 {"t310", nullptr},
                                                                                 {"n310", nullptr},
                                                                                 {"t311", nullptr},
                                                                                 {"n311", nullptr},
                                                                                 {"t319", nullptr}};
const asn1::table::seq_desc          ue_timers_and_consts_s::desc             = {
    {seq_table_ue_timers_and_consts_s_fields, 7, UINT64_C(0x7f), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_ue_timers_and_consts_s_names};

static const char* enum_names_ue_timers_and_consts_s_t300_e__values[] =
    {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
const asn1::table::enum_desc ue_timers_and_consts_s::t300_opts::desc = {
    enum_names_ue_timers_and_consts_s_t300_e__values,
    8,
    "ue_timers_and_consts_s::t300_e_"};
uint16_t ue_timers_and_consts_s::t300_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::t300_e_");
}

static const char* enum_names_ue_timers_and_consts_s_t301_e__values[] =
    {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
const asn1::table::enum_desc ue_timers_and_consts_s::t301_opts::desc = {
    enum_names_ue_timers_and_consts_s_t301_e__values,
    8,
    "ue_timers_and_consts_s::t301_e_"};
uint16_t ue_timers_and_consts_s::t301_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::t301_e_");
}

static const char* enum_names_ue_timers_and_consts_s_t310_e__values[] =
    {"ms0", "ms50", "ms100", "ms200", "ms500", "ms1000", "ms2000"};
const asn1::table::enum_desc ue_timers_and_consts_s::t310_opts::desc = {
    enum_names_ue_timers_and_consts_s_t310_e__values,
    7,
    "ue_timers_and_consts_s::t310_e_"};
uint16_t ue_timers_and_consts_s::t310_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 50, 100, 200, 500, 1000, 2000};
  return map_enum_number(numbers, 7, value, "ue_timers_and_consts_s::t310_e_");
}

static const char* enum_names_ue_timers_and_consts_s_n310_e__values[] =
    {"n1", "n2", "n3", "n4", "n6", "n8", "n10", "n20"};
const asn1::table::enum_desc ue_timers_and_consts_s::n310_opts::desc = {
    enum_names_ue_timers_and_consts_s_n310_e__values,
    8,
    "ue_timers_and_consts_s::n310_e_"};
uint8_t ue_timers_and_consts_s::n310_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 10, 20};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::n310_e_");
}

static const char* enum_names_ue_timers_and_consts_s_t311_e__values[] =
    {"ms1000", "ms3000", "ms5000", "ms10000", "ms15000", "ms20000", "ms30000"};
const asn1::table::enum_desc ue_timers_and_consts_s::t311_opts::desc = {
    enum_names_ue_timers_and_consts_s_t311_e__values,
    7,
    "ue_timers_and_consts_s::t311_e_"};
uint16_t ue_timers_and_consts_s::t311_opts::to_number() const
{
  static const uint16_t numbers[] = {1000, 3000, 5000, 10000, 15000, 20000, 30000};
  return map_enum_number(numbers, 7, value, "ue_timers_and_consts_s::t311_e_");
}

static const char* enum_names_ue_timers_and_consts_s_n311_e__values[] =
    {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10"};
const asn1::table::enum_desc ue_timers_and_consts_s::n311_opts::desc = {
    enum_names_ue_timers_and_consts_s_n311_e__values,
    8,
    "ue_timers_and_consts_s::n311_e_"};
uint8_t ue_timers_and_consts_s::n311_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::n311_e_");
}

static const char* enum_names_ue_timers_and_consts_s_t319_e__values[] =
    {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
const asn1::table::enum_desc ue_timers_and_consts_s::t319_opts::desc = {
    enum_names_ue_timers_and_consts_s_t319_e__values,
    8,
    "ue_timers_and_consts_s::t319_e_"};
uint16_t ue_timers_and_consts_s::t319_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_s::t319_e_");
}

// UAC-BarringPerCat ::= SEQUENCE
static const asn1::table::seq_field seq_table_uac_barr_per_cat_s_fields[] = {
    {offsetof(uac_barr_per_cat_s, access_category),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(63),
     nullptr},
    {offsetof(uac_barr_per_cat_s, uac_barr_info_set_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr}};
static const asn1::table::field_name seq_table_uac_barr_per_cat_s_names[] = {{"accessCategory", nullptr},
                                                                             {"uac-barringInfoSetIndex", nullptr}};
const asn1::table::seq_desc          uac_barr_per_cat_s::desc             = {
    {seq_table_uac_barr_per_cat_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_uac_barr_per_cat_s_names};

// UAC-BarringPerPLMN ::= SEQUENCE
static const asn1::table::seq_field seq_table_uac_barr_per_plmn_s_fields[] = {
    {offsetof(uac_barr_per_plmn_s, plmn_id_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(12),
     nullptr},
    {offsetof(uac_barr_per_plmn_s, uac_ac_barr_list_type),
     (int32_t)offsetof(uac_barr_per_plmn_s, uac_ac_barr_list_type_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::desc}};
static const asn1::table::field_name seq_table_uac_barr_per_plmn_s_names[] = {{"plmn-IdentityIndex", nullptr},
                                                                              {"uac-ACBarringListType", nullptr}};
static const asn1::table::seq_opt    seq_table_uac_barr_per_plmn_s_opts[]  = {
    {1, (int32_t)offsetof(uac_barr_per_plmn_s, uac_ac_barr_list_type_present)}};
const asn1::table::seq_desc uac_barr_per_plmn_s::desc = {
    {seq_table_uac_barr_per_plmn_s_fields, 2, UINT64_C(0x1), seq_table_uac_barr_per_plmn_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_uac_barr_per_plmn_s_names};

static const asn1::table::storage_ops* const
    choice_storage_uac_barr_per_plmn_s_uac_ac_barr_list_type_c__alternatives[] = {
        &asn1::table::alternative_ops<uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::uac_implicit_ac_barr_list_l_,
                                      false>::value,
        &asn1::table::alternative_ops<uac_barr_per_cat_list_l, false>::value};
const asn1::table::choice_storage_desc uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::storage_desc = {
    choice_storage_uac_barr_per_plmn_s_uac_ac_barr_list_type_c__alternatives,
    2,
    "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_"};

uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::uac_implicit_ac_barr_list_l_&
uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::set_uac_implicit_ac_barr_list()
{
  set(types::uac_implicit_ac_barr_list);
  return c.get<uac_implicit_ac_barr_list_l_>();
}
uac_barr_per_cat_list_l& uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::set_uac_explicit_ac_barr_list()
{
  set(types::uac_explicit_ac_barr_list);
  return c.get<uac_barr_per_cat_list_l>();
}
static const asn1::table::seq_field choice_table_uac_barr_per_plmn_s_uac_ac_barr_list_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     63,
     63,
     &asn1::table::list_codec<uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::uac_implicit_ac_barr_list_l_,
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(1), UINT64_C(8), false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::list_codec<uac_barr_per_cat_list_l, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_uac_barr_per_plmn_s_uac_ac_barr_list_type_c__names[] = {
    {"uac-ImplicitACBarringList", nullptr},
    {"uac-ExplicitACBarringList", nullptr}};
const asn1::table::choice_desc uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::desc = {
    asn1::table::enum_int_type<uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::types>(),
    uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::types::nof_types,
    uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::types::nof_exts,
    uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::types::has_ext,
    false,
    choice_table_uac_barr_per_plmn_s_uac_ac_barr_list_type_c__alternatives,
    nullptr,
    choice_table_uac_barr_per_plmn_s_uac_ac_barr_list_type_c__names,
    "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_",
    offsetof(uac_barr_per_plmn_s::uac_ac_barr_list_type_c_, c.type),
    offsetof(uac_barr_per_plmn_s::uac_ac_barr_list_type_c_, c.value),
    false,
    &uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::storage_desc};

static const char* enum_names_uac_barr_per_plmn_s_uac_ac_barr_list_type_c__types_values[] = {
    "uac-ImplicitACBarringList",
    "uac-ExplicitACBarringList"};
const asn1::table::enum_desc uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::types_opts::desc = {
    enum_names_uac_barr_per_plmn_s_uac_ac_barr_list_type_c__types_values,
    2,
    "uac_barr_per_plmn_s::uac_ac_barr_list_type_c_::types"};

// UAC-BarringInfoSet ::= SEQUENCE
static const asn1::table::seq_field seq_table_uac_barr_info_set_s_fields[] = {
    {offsetof(uac_barr_info_set_s, uac_barr_factor),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(uac_barr_info_set_s::uac_barr_factor)>(),
     decltype(uac_barr_info_set_s::uac_barr_factor)::has_ext,
     false,
     decltype(uac_barr_info_set_s::uac_barr_factor)::nof_types,
     decltype(uac_barr_info_set_s::uac_barr_factor)::nof_exts,
     nullptr,
     &uac_barr_info_set_s::uac_barr_factor_opts::desc},
    {offsetof(uac_barr_info_set_s, uac_barr_time),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(uac_barr_info_set_s::uac_barr_time)>(),
     decltype(uac_barr_info_set_s::uac_barr_time)::has_ext,
     false,
     decltype(uac_barr_info_set_s::uac_barr_time)::nof_types,
     decltype(uac_barr_info_set_s::uac_barr_time)::nof_exts,
     nullptr,
     &uac_barr_info_set_s::uac_barr_time_opts::desc},
    {offsetof(uac_barr_info_set_s, uac_barr_for_access_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(uac_barr_info_set_s::uac_barr_for_access_id),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_uac_barr_info_set_s_names[] = {
    {"uac-BarringFactor", nullptr},
    {"uac-BarringTime", nullptr},
    {"uac-BarringForAccessIdentity", nullptr}};
const asn1::table::seq_desc uac_barr_info_set_s::desc = {
    {seq_table_uac_barr_info_set_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_uac_barr_info_set_s_names};

static const char* enum_names_uac_barr_info_set_s_uac_barr_factor_e__values[] =
    {"p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
const asn1::table::enum_desc uac_barr_info_set_s::uac_barr_factor_opts::desc = {
    enum_names_uac_barr_info_set_s_uac_barr_factor_e__values,
    16,
    "uac_barr_info_set_s::uac_barr_factor_e_"};
float uac_barr_info_set_s::uac_barr_factor_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(numbers, 16, value, "uac_barr_info_set_s::uac_barr_factor_e_");
}
const char* uac_barr_info_set_s::uac_barr_factor_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(number_strs, 16, value, "uac_barr_info_set_s::uac_barr_factor_e_");
}

static const char* enum_names_uac_barr_info_set_s_uac_barr_time_e__values[] =
    {"s4", "s8", "s16", "s32", "s64", "s128", "s256", "s512"};
const asn1::table::enum_desc uac_barr_info_set_s::uac_barr_time_opts::desc = {
    enum_names_uac_barr_info_set_s_uac_barr_time_e__values,
    8,
    "uac_barr_info_set_s::uac_barr_time_e_"};
uint16_t uac_barr_info_set_s::uac_barr_time_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 8, value, "uac_barr_info_set_s::uac_barr_time_e_");
}

// UAC-AccessCategory1-SelectionAssistanceInfo ::= ENUMERATED
static const char*           enum_names_uac_access_category1_sel_assist_info_e_values[] = {"a", "b", "c"};
const asn1::table::enum_desc uac_access_category1_sel_assist_info_opts::desc            = {
    enum_names_uac_access_category1_sel_assist_info_e_values,
    3,
    "uac_access_category1_sel_assist_info_e"};

// GNSS-ID-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_gnss_id_r16_s_fields[] = {
    {offsetof(gnss_id_r16_s, gnss_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(gnss_id_r16_s::gnss_id_r16)>(),
     decltype(gnss_id_r16_s::gnss_id_r16)::has_ext,
     false,
     decltype(gnss_id_r16_s::gnss_id_r16)::nof_types,
     decltype(gnss_id_r16_s::gnss_id_r16)::nof_exts,
     nullptr,
     &gnss_id_r16_s::gnss_id_r16_opts::desc}};
static const asn1::table::field_name seq_table_gnss_id_r16_s_names[] = {{"gnss-id-r16", nullptr}};
const asn1::table::seq_desc gnss_id_r16_s::desc = {{seq_table_gnss_id_r16_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                                   true,
                                                   false,
                                                   nullptr,
                                                   0,
                                                   seq_table_gnss_id_r16_s_names};

static const char* enum_names_gnss_id_r16_s_gnss_id_r16_e__values[] =
    {"gps", "sbas", "qzss", "galileo", "glonass", "bds", "navic-v1760"};
const asn1::table::enum_desc gnss_id_r16_s::gnss_id_r16_opts::desc = {enum_names_gnss_id_r16_s_gnss_id_r16_e__values,
                                                                      7,
                                                                      "gnss_id_r16_s::gnss_id_r16_e_"};

// SBAS-ID-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sbas_id_r16_s_fields[] = {
    {offsetof(sbas_id_r16_s, sbas_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sbas_id_r16_s::sbas_id_r16)>(),
     decltype(sbas_id_r16_s::sbas_id_r16)::has_ext,
     false,
     decltype(sbas_id_r16_s::sbas_id_r16)::nof_types,
     decltype(sbas_id_r16_s::sbas_id_r16)::nof_exts,
     nullptr,
     &sbas_id_r16_s::sbas_id_r16_opts::desc}};
static const asn1::table::field_name seq_table_sbas_id_r16_s_names[] = {{"sbas-id-r16", nullptr}};
const asn1::table::seq_desc sbas_id_r16_s::desc = {{seq_table_sbas_id_r16_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                                   true,
                                                   false,
                                                   nullptr,
                                                   0,
                                                   seq_table_sbas_id_r16_s_names};

static const char*           enum_names_sbas_id_r16_s_sbas_id_r16_e__values[] = {"waas", "egnos", "msas", "gagan"};
const asn1::table::enum_desc sbas_id_r16_s::sbas_id_r16_opts::desc = {enum_names_sbas_id_r16_s_sbas_id_r16_e__values,
                                                                      4,
                                                                      "sbas_id_r16_s::sbas_id_r16_e_"};

// PosSIB-Type-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_pos_sib_type_r16_s_fields[] = {
    {0,
     (int32_t)offsetof(pos_sib_type_r16_s, encrypted_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pos_sib_type_r16_s, gnss_id_r16),
     (int32_t)offsetof(pos_sib_type_r16_s, gnss_id_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnss_id_r16_s::desc},
    {offsetof(pos_sib_type_r16_s, sbas_id_r16),
     (int32_t)offsetof(pos_sib_type_r16_s, sbas_id_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sbas_id_r16_s::desc},
    {offsetof(pos_sib_type_r16_s, pos_sib_type_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pos_sib_type_r16_s::pos_sib_type_r16)>(),
     decltype(pos_sib_type_r16_s::pos_sib_type_r16)::has_ext,
     false,
     decltype(pos_sib_type_r16_s::pos_sib_type_r16)::nof_types,
     decltype(pos_sib_type_r16_s::pos_sib_type_r16)::nof_exts,
     nullptr,
     &pos_sib_type_r16_s::pos_sib_type_r16_opts::desc},
    {0,
     (int32_t)offsetof(pos_sib_type_r16_s, area_scope_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pos_sib_type_r16_s_names[] = {{"encrypted-r16", "true"},
                                                                             {"gnss-id-r16", nullptr},
                                                                             {"sbas-id-r16", nullptr},
                                                                             {"posSibType-r16", nullptr},
                                                                             {"areaScope-r16", "true"}};
static const asn1::table::seq_opt    seq_table_pos_sib_type_r16_s_opts[]  = {
    {0, (int32_t)offsetof(pos_sib_type_r16_s, encrypted_r16_present)},
    {1, (int32_t)offsetof(pos_sib_type_r16_s, gnss_id_r16_present)},
    {2, (int32_t)offsetof(pos_sib_type_r16_s, sbas_id_r16_present)},
    {4, (int32_t)offsetof(pos_sib_type_r16_s, area_scope_r16_present)}};
const asn1::table::seq_desc pos_sib_type_r16_s::desc = {
    {seq_table_pos_sib_type_r16_s_fields, 5, UINT64_C(0x8), seq_table_pos_sib_type_r16_s_opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_pos_sib_type_r16_s_names};

static const char* enum_names_pos_sib_type_r16_s_pos_sib_type_r16_e__values[] = {
    "posSibType1-1",  "posSibType1-2",  "posSibType1-3",  "posSibType1-4",  "posSibType1-5",  "posSibType1-6",
    "posSibType1-7",  "posSibType1-8",  "posSibType2-1",  "posSibType2-2",  "posSibType2-3",  "posSibType2-4",
    "posSibType2-5",  "posSibType2-6",  "posSibType2-7",  "posSibType2-8",  "posSibType2-9",  "posSibType2-10",
    "posSibType2-11", "posSibType2-12", "posSibType2-13", "posSibType2-14", "posSibType2-15", "posSibType2-16",
    "posSibType2-17", "posSibType2-18", "posSibType2-19", "posSibType2-20", "posSibType2-21", "posSibType2-22",
    "posSibType2-23", "posSibType3-1",  "posSibType4-1",  "posSibType5-1",  "posSibType6-1",  "posSibType6-2",
    "posSibType6-3"};
const asn1::table::enum_desc pos_sib_type_r16_s::pos_sib_type_r16_opts::desc = {
    enum_names_pos_sib_type_r16_s_pos_sib_type_r16_e__values,
    37,
    "pos_sib_type_r16_s::pos_sib_type_r16_e_"};

// PosSchedulingInfo-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_pos_sched_info_r16_s_fields[] = {
    {0,
     (int32_t)offsetof(pos_sched_info_r16_s, offset_to_si_used_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pos_sched_info_r16_s, pos_si_periodicity_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pos_sched_info_r16_s::pos_si_periodicity_r16)>(),
     decltype(pos_sched_info_r16_s::pos_si_periodicity_r16)::has_ext,
     false,
     decltype(pos_sched_info_r16_s::pos_si_periodicity_r16)::nof_types,
     decltype(pos_sched_info_r16_s::pos_si_periodicity_r16)::nof_exts,
     nullptr,
     &pos_sched_info_r16_s::pos_si_periodicity_r16_opts::desc},
    {offsetof(pos_sched_info_r16_s, pos_si_broadcast_status_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pos_sched_info_r16_s::pos_si_broadcast_status_r16)>(),
     decltype(pos_sched_info_r16_s::pos_si_broadcast_status_r16)::has_ext,
     false,
     decltype(pos_sched_info_r16_s::pos_si_broadcast_status_r16)::nof_types,
     decltype(pos_sched_info_r16_s::pos_si_broadcast_status_r16)::nof_exts,
     nullptr,
     &pos_sched_info_r16_s::pos_si_broadcast_status_r16_opts::desc},
    {offsetof(pos_sched_info_r16_s, pos_sib_map_info_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pos_sched_info_r16_s::pos_sib_map_info_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pos_sib_type_r16_s::desc}};
static const asn1::table::field_name seq_table_pos_sched_info_r16_s_names[] = {{"offsetToSI-Used-r16", "true"},
                                                                               {"posSI-Periodicity-r16", nullptr},
                                                                               {"posSI-BroadcastStatus-r16", nullptr},
                                                                               {"posSIB-MappingInfo-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_pos_sched_info_r16_s_opts[]  = {
    {0, (int32_t)offsetof(pos_sched_info_r16_s, offset_to_si_used_r16_present)}};
const asn1::table::seq_desc pos_sched_info_r16_s::desc = {
    {seq_table_pos_sched_info_r16_s_fields, 4, UINT64_C(0xe), seq_table_pos_sched_info_r16_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_pos_sched_info_r16_s_names};

static const char* enum_names_pos_sched_info_r16_s_pos_si_periodicity_r16_e__values[] =
    {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
const asn1::table::enum_desc pos_sched_info_r16_s::pos_si_periodicity_r16_opts::desc = {
    enum_names_pos_sched_info_r16_s_pos_si_periodicity_r16_e__values,
    7,
    "pos_sched_info_r16_s::pos_si_periodicity_r16_e_"};
uint16_t pos_sched_info_r16_s::pos_si_periodicity_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 7, value, "pos_sched_info_r16_s::pos_si_periodicity_r16_e_");
}

static const char*           enum_names_pos_sched_info_r16_s_pos_si_broadcast_status_r16_e__values[] = {"broadcasting",
                                                                                                        "notBroadcasting"};
const asn1::table::enum_desc pos_sched_info_r16_s::pos_si_broadcast_status_r16_opts::desc            = {
    enum_names_pos_sched_info_r16_s_pos_si_broadcast_status_r16_e__values,
    2,
    "pos_sched_info_r16_s::pos_si_broadcast_status_r16_e_"};

// SI-RequestResourcesRepetition-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_si_request_res_repeat_r18_s_fields[] = {
    {offsetof(si_request_res_repeat_r18_s, ra_preamb_start_idx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr}};
static const asn1::table::field_name seq_table_si_request_res_repeat_r18_s_names[] = {
    {"ra-PreambleStartIndex-r18", nullptr}};
const asn1::table::seq_desc si_request_res_repeat_r18_s::desc = {
    {seq_table_si_request_res_repeat_r18_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_si_request_res_repeat_r18_s_names};

// SI-RequestConfigRepetition-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_si_request_cfg_repeat_r18_s_rach_occasions_si_r18_s__fields[] = {
    {offsetof(si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_, rach_cfg_si_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rach_cfg_generic_s::desc},
    {offsetof(si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_, ssb_per_rach_occasion_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_::ssb_per_rach_occasion_r18)>(),
     decltype(si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_::ssb_per_rach_occasion_r18)::has_ext,
     false,
     decltype(si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_::ssb_per_rach_occasion_r18)::nof_types,
     decltype(si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_::ssb_per_rach_occasion_r18)::nof_exts,
     nullptr,
     &si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_::ssb_per_rach_occasion_r18_opts::desc}};
static const asn1::table::field_name seq_table_si_request_cfg_repeat_r18_s_rach_occasions_si_r18_s__names[] = {
    {"rach-ConfigSI-r18", nullptr},
    {"ssb-perRACH-Occasion-r18", nullptr}};
static const asn1::table::seq_desc seq_table_si_request_cfg_repeat_r18_s_rach_occasions_si_r18_s_ = {
    {seq_table_si_request_cfg_repeat_r18_s_rach_occasions_si_r18_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_si_request_cfg_repeat_r18_s_rach_occasions_si_r18_s__names};
static const asn1::table::seq_field seq_table_si_request_cfg_repeat_r18_s_fields[] = {
    {offsetof(si_request_cfg_repeat_r18_s, rach_occasions_si_r18),
     (int32_t)offsetof(si_request_cfg_repeat_r18_s, rach_occasions_si_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_si_request_cfg_repeat_r18_s_rach_occasions_si_r18_s_},
    {offsetof(si_request_cfg_repeat_r18_s, si_request_res_repeat_num2_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(si_request_cfg_repeat_r18_s::si_request_res_repeat_num2_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &si_request_res_repeat_r18_s::desc},
    {offsetof(si_request_cfg_repeat_r18_s, si_request_res_repeat_num4_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(si_request_cfg_repeat_r18_s::si_request_res_repeat_num4_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &si_request_res_repeat_r18_s::desc},
    {offsetof(si_request_cfg_repeat_r18_s, si_request_res_repeat_num8_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(si_request_cfg_repeat_r18_s::si_request_res_repeat_num8_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &si_request_res_repeat_r18_s::desc}};
static const asn1::table::field_name seq_table_si_request_cfg_repeat_r18_s_names[] = {
    {"rach-OccasionsSI-r18", nullptr},
    {"si-RequestResourcesRepetitionNum2-r18", nullptr},
    {"si-RequestResourcesRepetitionNum4-r18", nullptr},
    {"si-RequestResourcesRepetitionNum8-r18", nullptr}};
static const asn1::table::seq_opt seq_table_si_request_cfg_repeat_r18_s_opts[] = {
    {0, (int32_t)offsetof(si_request_cfg_repeat_r18_s, rach_occasions_si_r18_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc si_request_cfg_repeat_r18_s::desc = {
    {seq_table_si_request_cfg_repeat_r18_s_fields, 4, UINT64_C(0x0), seq_table_si_request_cfg_repeat_r18_s_opts, 4},
    true,
    false,
    nullptr,
    0,
    seq_table_si_request_cfg_repeat_r18_s_names};

static const char*
    enum_names_si_request_cfg_repeat_r18_s_rach_occasions_si_r18_s__ssb_per_rach_occasion_r18_e__values[] =
        {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
const asn1::table::enum_desc
    si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_::ssb_per_rach_occasion_r18_opts::desc = {
        enum_names_si_request_cfg_repeat_r18_s_rach_occasions_si_r18_s__ssb_per_rach_occasion_r18_e__values,
        8,
        "si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_::ssb_per_rach_occasion_r18_e_"};
float si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_::ssb_per_rach_occasion_r18_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(
      numbers, 8, value, "si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_::ssb_per_rach_occasion_r18_e_");
}
const char*
si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_::ssb_per_rach_occasion_r18_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(
      number_strs, 8, value, "si_request_cfg_repeat_r18_s::rach_occasions_si_r18_s_::ssb_per_rach_occasion_r18_e_");
}

// PosSI-SchedulingInfo-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_pos_si_sched_info_r16_s_fields[] = {
    {offsetof(pos_si_sched_info_r16_s, pos_sched_info_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(pos_si_sched_info_r16_s::pos_sched_info_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pos_sched_info_r16_s::desc},
    {offsetof(pos_si_sched_info_r16_s, pos_si_request_cfg_r16),
     (int32_t)offsetof(pos_si_sched_info_r16_s, pos_si_request_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_request_cfg_s::desc},
    {offsetof(pos_si_sched_info_r16_s, pos_si_request_cfg_sul_r16),
     (int32_t)offsetof(pos_si_sched_info_r16_s, pos_si_request_cfg_sul_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_request_cfg_s::desc}};
static const asn1::table::field_name seq_table_pos_si_sched_info_r16_s_names[] = {
    {"posSchedulingInfoList-r16", nullptr},
    {"posSI-RequestConfig-r16", nullptr},
    {"posSI-RequestConfigSUL-r16", nullptr}};
static const asn1::table::seq_opt seq_table_pos_si_sched_info_r16_s_opts[] = {
    {1, (int32_t)offsetof(pos_si_sched_info_r16_s, pos_si_request_cfg_r16_present)},
    {2, (int32_t)offsetof(pos_si_sched_info_r16_s, pos_si_request_cfg_sul_r16_present)}};
static const asn1::table::seq_field seq_table_pos_si_sched_info_r16_s_g0_fields[] = {
    {offsetof(pos_si_sched_info_r16_s, pos_si_request_cfg_red_cap_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pos_si_sched_info_r16_s::pos_si_request_cfg_red_cap_r17)>::value,
     &si_request_cfg_s::desc}};
static const asn1::table::field_name seq_table_pos_si_sched_info_r16_s_g0_names[] = {
    {"posSI-RequestConfigRedCap-r17", nullptr}};
static const asn1::table::seq_opt seq_table_pos_si_sched_info_r16_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_pos_si_sched_info_r16_s_g1_fields[] = {
    {offsetof(pos_si_sched_info_r16_s, pos_si_request_cfg_msg1_repeat_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pos_si_sched_info_r16_s::pos_si_request_cfg_msg1_repeat_r18)>::value,
     &si_request_cfg_repeat_r18_s::desc},
    {offsetof(pos_si_sched_info_r16_s, pos_si_request_cfg_sul_msg1_repeat_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pos_si_sched_info_r16_s::pos_si_request_cfg_sul_msg1_repeat_r18)>::value,
     &si_request_cfg_repeat_r18_s::desc},
    {offsetof(pos_si_sched_info_r16_s, pos_si_request_cfg_red_cap_msg1_repeat_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<
         decltype(pos_si_sched_info_r16_s::pos_si_request_cfg_red_cap_msg1_repeat_r18)>::value,
     &si_request_cfg_repeat_r18_s::desc}};
static const asn1::table::field_name seq_table_pos_si_sched_info_r16_s_g1_names[] = {
    {"posSI-RequestConfigMSG1-Repetition-r18", nullptr},
    {"posSI-RequestConfigSUL-MSG1-Repetition-r18", nullptr},
    {"posSI-RequestConfigRedCap-MSG1-Repetition-r18", nullptr}};
static const asn1::table::seq_opt seq_table_pos_si_sched_info_r16_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_pos_si_sched_info_r16_s_groups[] = {
    {{seq_table_pos_si_sched_info_r16_s_g0_fields, 1, UINT64_C(0x0), seq_table_pos_si_sched_info_r16_s_g0_opts, 1},
     false,
     seq_table_pos_si_sched_info_r16_s_g0_names},
    {{seq_table_pos_si_sched_info_r16_s_g1_fields, 3, UINT64_C(0x0), seq_table_pos_si_sched_info_r16_s_g1_opts, 3},
     false,
     seq_table_pos_si_sched_info_r16_s_g1_names}};
const asn1::table::seq_desc pos_si_sched_info_r16_s::desc = {
    {seq_table_pos_si_sched_info_r16_s_fields, 3, UINT64_C(0x1), seq_table_pos_si_sched_info_r16_s_opts, 2},
    true,
    false,
    seq_table_pos_si_sched_info_r16_s_groups,
    2,
    seq_table_pos_si_sched_info_r16_s_names};

// UAC-AC1-SelectAssistInfo-r16 ::= ENUMERATED
static const char*           enum_names_uac_ac1_select_assist_info_r16_e_values[] = {"a", "b", "c", "notConfigured"};
const asn1::table::enum_desc uac_ac1_select_assist_info_r16_opts::desc            = {
    enum_names_uac_ac1_select_assist_info_r16_e_values,
    4,
    "uac_ac1_select_assist_info_r16_e"};

// UAC-BarringInfoSet-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_uac_barr_info_set_v1700_s_fields[] = {
    {offsetof(uac_barr_info_set_v1700_s, uac_barr_factor_for_ai3_r17),
     (int32_t)offsetof(uac_barr_info_set_v1700_s, uac_barr_factor_for_ai3_r17_present),
     asn1::table::enum_op<decltype(uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17)>(),
     decltype(uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17)::has_ext,
     false,
     decltype(uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17)::nof_types,
     decltype(uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17)::nof_exts,
     nullptr,
     &uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::desc}};
static const asn1::table::field_name seq_table_uac_barr_info_set_v1700_s_names[] = {
    {"uac-BarringFactorForAI3-r17", nullptr}};
static const asn1::table::seq_opt seq_table_uac_barr_info_set_v1700_s_opts[] = {
    {0, (int32_t)offsetof(uac_barr_info_set_v1700_s, uac_barr_factor_for_ai3_r17_present)}};
const asn1::table::seq_desc uac_barr_info_set_v1700_s::desc = {
    {seq_table_uac_barr_info_set_v1700_s_fields, 1, UINT64_C(0x0), seq_table_uac_barr_info_set_v1700_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_uac_barr_info_set_v1700_s_names};

static const char* enum_names_uac_barr_info_set_v1700_s_uac_barr_factor_for_ai3_r17_e__values[] =
    {"p00", "p05", "p10", "p15", "p20", "p25", "p30", "p40", "p50", "p60", "p70", "p75", "p80", "p85", "p90", "p95"};
const asn1::table::enum_desc uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::desc = {
    enum_names_uac_barr_info_set_v1700_s_uac_barr_factor_for_ai3_r17_e__values,
    16,
    "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_"};
float uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::to_number() const
{
  static const float numbers[] = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0, 7.5, 8.0, 8.5, 9.0, 9.5};
  return map_enum_number(numbers, 16, value, "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_");
}
const char* uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {
      "0.0", "0.5", "1.0", "1.5", "2.0", "2.5", "3.0", "4.0", "5.0", "6.0", "7.0", "7.5", "8.0", "8.5", "9.0", "9.5"};
  return convert_enum_idx(number_strs, 16, value, "uac_barr_info_set_v1700_s::uac_barr_factor_for_ai3_r17_e_");
}

// SDT-ConfigCommonSIB-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sdt_cfg_common_sib_r17_s_fields[] = {
    {offsetof(sdt_cfg_common_sib_r17_s, sdt_rsrp_thres_r17),
     (int32_t)offsetof(sdt_cfg_common_sib_r17_s, sdt_rsrp_thres_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(sdt_cfg_common_sib_r17_s, sdt_lc_ch_sr_delay_timer_r17),
     (int32_t)offsetof(sdt_cfg_common_sib_r17_s, sdt_lc_ch_sr_delay_timer_r17_present),
     asn1::table::enum_op<decltype(sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17)>(),
     decltype(sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17)::has_ext,
     false,
     decltype(sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17)::nof_types,
     decltype(sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17)::nof_exts,
     nullptr,
     &sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_opts::desc},
    {offsetof(sdt_cfg_common_sib_r17_s, sdt_data_volume_thres_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17)>(),
     decltype(sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17)::has_ext,
     false,
     decltype(sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17)::nof_types,
     decltype(sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17)::nof_exts,
     nullptr,
     &sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_opts::desc},
    {offsetof(sdt_cfg_common_sib_r17_s, t319a_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sdt_cfg_common_sib_r17_s::t319a_r17)>(),
     decltype(sdt_cfg_common_sib_r17_s::t319a_r17)::has_ext,
     false,
     decltype(sdt_cfg_common_sib_r17_s::t319a_r17)::nof_types,
     decltype(sdt_cfg_common_sib_r17_s::t319a_r17)::nof_exts,
     nullptr,
     &sdt_cfg_common_sib_r17_s::t319a_r17_opts::desc}};
static const asn1::table::field_name seq_table_sdt_cfg_common_sib_r17_s_names[] = {
    {"sdt-RSRP-Threshold-r17", nullptr},
    {"sdt-LogicalChannelSR-DelayTimer-r17", nullptr},
    {"sdt-DataVolumeThreshold-r17", nullptr},
    {"t319a-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sdt_cfg_common_sib_r17_s_opts[] = {
    {0, (int32_t)offsetof(sdt_cfg_common_sib_r17_s, sdt_rsrp_thres_r17_present)},
    {1, (int32_t)offsetof(sdt_cfg_common_sib_r17_s, sdt_lc_ch_sr_delay_timer_r17_present)}};
const asn1::table::seq_desc sdt_cfg_common_sib_r17_s::desc = {
    {seq_table_sdt_cfg_common_sib_r17_s_fields, 4, UINT64_C(0xc), seq_table_sdt_cfg_common_sib_r17_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_sdt_cfg_common_sib_r17_s_names};

static const char* enum_names_sdt_cfg_common_sib_r17_s_sdt_lc_ch_sr_delay_timer_r17_e__values[] =
    {"sf20", "sf40", "sf64", "sf128", "sf512", "sf1024", "sf2560", "spare1"};
const asn1::table::enum_desc sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_opts::desc = {
    enum_names_sdt_cfg_common_sib_r17_s_sdt_lc_ch_sr_delay_timer_r17_e__values,
    8,
    "sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_e_"};
uint16_t sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {20, 40, 64, 128, 512, 1024, 2560};
  return map_enum_number(numbers, 7, value, "sdt_cfg_common_sib_r17_s::sdt_lc_ch_sr_delay_timer_r17_e_");
}

static const char*           enum_names_sdt_cfg_common_sib_r17_s_sdt_data_volume_thres_r17_e__values[] = {"byte32",
                                                                                                          "byte100",
                                                                                                          "byte200",
                                                                                                          "byte400",
                                                                                                          "byte600",
                                                                                                          "byte800",
                                                                                                          "byte1000",
                                                                                                          "byte2000",
                                                                                                          "byte4000",
                                                                                                          "byte8000",
                                                                                                          "byte9000",
                                                                                                          "byte10000",
                                                                                                          "byte12000",
                                                                                                          "byte24000",
                                                                                                          "byte48000",
                                                                                                          "byte96000"};
const asn1::table::enum_desc sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_opts::desc            = {
    enum_names_sdt_cfg_common_sib_r17_s_sdt_data_volume_thres_r17_e__values,
    16,
    "sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_e_"};
uint32_t sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_opts::to_number() const
{
  static const uint32_t numbers[] = {
      32, 100, 200, 400, 600, 800, 1000, 2000, 4000, 8000, 9000, 10000, 12000, 24000, 48000, 96000};
  return map_enum_number(numbers, 16, value, "sdt_cfg_common_sib_r17_s::sdt_data_volume_thres_r17_e_");
}

static const char*           enum_names_sdt_cfg_common_sib_r17_s_t319a_r17_e__values[] = {"ms100",
                                                                                          "ms200",
                                                                                          "ms300",
                                                                                          "ms400",
                                                                                          "ms600",
                                                                                          "ms1000",
                                                                                          "ms2000",
                                                                                          "ms3000",
                                                                                          "ms4000",
                                                                                          "spare7",
                                                                                          "spare6",
                                                                                          "spare5",
                                                                                          "spare4",
                                                                                          "spare3",
                                                                                          "spare2",
                                                                                          "spare1"};
const asn1::table::enum_desc sdt_cfg_common_sib_r17_s::t319a_r17_opts::desc            = {
    enum_names_sdt_cfg_common_sib_r17_s_t319a_r17_e__values,
    16,
    "sdt_cfg_common_sib_r17_s::t319a_r17_e_"};
uint16_t sdt_cfg_common_sib_r17_s::t319a_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 2000, 3000, 4000};
  return map_enum_number(numbers, 9, value, "sdt_cfg_common_sib_r17_s::t319a_r17_e_");
}

// RedCap-ConfigCommonSIB-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_red_cap_cfg_common_sib_r17_s_cell_barred_red_cap_r17_s__fields[] = {
    {offsetof(red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_, cell_barred_red_cap1_rx_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<
         decltype(red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap1_rx_r17)>(),
     decltype(red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap1_rx_r17)::has_ext,
     false,
     decltype(red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap1_rx_r17)::nof_types,
     decltype(red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap1_rx_r17)::nof_exts,
     nullptr,
     &red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap1_rx_r17_opts::desc},
    {offsetof(red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_, cell_barred_red_cap2_rx_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<
         decltype(red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap2_rx_r17)>(),
     decltype(red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap2_rx_r17)::has_ext,
     false,
     decltype(red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap2_rx_r17)::nof_types,
     decltype(red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap2_rx_r17)::nof_exts,
     nullptr,
     &red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap2_rx_r17_opts::desc}};
static const asn1::table::field_name seq_table_red_cap_cfg_common_sib_r17_s_cell_barred_red_cap_r17_s__names[] = {
    {"cellBarredRedCap1Rx-r17", nullptr},
    {"cellBarredRedCap2Rx-r17", nullptr}};
static const asn1::table::seq_desc seq_table_red_cap_cfg_common_sib_r17_s_cell_barred_red_cap_r17_s_ = {
    {seq_table_red_cap_cfg_common_sib_r17_s_cell_barred_red_cap_r17_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_red_cap_cfg_common_sib_r17_s_cell_barred_red_cap_r17_s__names};
static const asn1::table::seq_field seq_table_red_cap_cfg_common_sib_r17_s_fields[] = {
    {0,
     (int32_t)offsetof(red_cap_cfg_common_sib_r17_s, half_duplex_red_cap_allowed_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(red_cap_cfg_common_sib_r17_s, cell_barred_red_cap_r17),
     (int32_t)offsetof(red_cap_cfg_common_sib_r17_s, cell_barred_red_cap_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_red_cap_cfg_common_sib_r17_s_cell_barred_red_cap_r17_s_}};
static const asn1::table::field_name seq_table_red_cap_cfg_common_sib_r17_s_names[] = {
    {"halfDuplexRedCapAllowed-r17", "true"},
    {"cellBarredRedCap-r17", nullptr}};
static const asn1::table::seq_opt seq_table_red_cap_cfg_common_sib_r17_s_opts[] = {
    {0, (int32_t)offsetof(red_cap_cfg_common_sib_r17_s, half_duplex_red_cap_allowed_r17_present)},
    {1, (int32_t)offsetof(red_cap_cfg_common_sib_r17_s, cell_barred_red_cap_r17_present)}};
const asn1::table::seq_desc red_cap_cfg_common_sib_r17_s::desc = {
    {seq_table_red_cap_cfg_common_sib_r17_s_fields, 2, UINT64_C(0x0), seq_table_red_cap_cfg_common_sib_r17_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_red_cap_cfg_common_sib_r17_s_names};

static const char*
    enum_names_red_cap_cfg_common_sib_r17_s_cell_barred_red_cap_r17_s__cell_barred_red_cap1_rx_r17_e__values[] = {
        "barred",
        "notBarred"};
const asn1::table::enum_desc
    red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap1_rx_r17_opts::desc = {
        enum_names_red_cap_cfg_common_sib_r17_s_cell_barred_red_cap_r17_s__cell_barred_red_cap1_rx_r17_e__values,
        2,
        "red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap1_rx_r17_e_"};

static const char*
    enum_names_red_cap_cfg_common_sib_r17_s_cell_barred_red_cap_r17_s__cell_barred_red_cap2_rx_r17_e__values[] = {
        "barred",
        "notBarred"};
const asn1::table::enum_desc
    red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap2_rx_r17_opts::desc = {
        enum_names_red_cap_cfg_common_sib_r17_s_cell_barred_red_cap_r17_s__cell_barred_red_cap2_rx_r17_e__values,
        2,
        "red_cap_cfg_common_sib_r17_s::cell_barred_red_cap_r17_s_::cell_barred_red_cap2_rx_r17_e_"};

// SIB-TypeInfo-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib_type_info_v1700_s_fields[] = {
    {offsetof(sib_type_info_v1700_s, sib_type_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sib_type_info_v1700_s::sib_type_r17_c_::desc},
    {offsetof(sib_type_info_v1700_s, value_tag_r17),
     (int32_t)offsetof(sib_type_info_v1700_s, value_tag_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {0,
     (int32_t)offsetof(sib_type_info_v1700_s, area_scope_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sib_type_info_v1700_s_names[] = {{"sibType-r17", nullptr},
                                                                                {"valueTag-r17", nullptr},
                                                                                {"areaScope-r17", "true"}};
static const asn1::table::seq_opt    seq_table_sib_type_info_v1700_s_opts[]  = {
    {1, (int32_t)offsetof(sib_type_info_v1700_s, value_tag_r17_present)},
    {2, (int32_t)offsetof(sib_type_info_v1700_s, area_scope_r17_present)}};
const asn1::table::seq_desc sib_type_info_v1700_s::desc = {
    {seq_table_sib_type_info_v1700_s_fields, 3, UINT64_C(0x1), seq_table_sib_type_info_v1700_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_sib_type_info_v1700_s_names};

static const asn1::table::storage_ops* const choice_storage_sib_type_info_v1700_s_sib_type_r17_c__alternatives[] = {
    &asn1::table::alternative_ops<sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_, true>::value,
    &asn1::table::alternative_ops<sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_, false>::value};
const asn1::table::choice_storage_desc sib_type_info_v1700_s::sib_type_r17_c_::storage_desc = {
    choice_storage_sib_type_info_v1700_s_sib_type_r17_c__alternatives,
    2,
    "sib_type_info_v1700_s::sib_type_r17_c_"};

sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_& sib_type_info_v1700_s::sib_type_r17_c_::set_type1_r17()
{
  set(types::type1_r17);
  return c.get<type1_r17_e_>();
}
sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_& sib_type_info_v1700_s::sib_type_r17_c_::set_type2_r17()
{
  set(types::type2_r17);
  return c.get<type2_r17_s_>();
}
static const asn1::table::seq_field seq_table_sib_type_info_v1700_s_sib_type_r17_c__type2_r17_s__fields[] = {
    {offsetof(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_, pos_sib_type_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_::pos_sib_type_r17)>(),
     decltype(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_::pos_sib_type_r17)::has_ext,
     false,
     decltype(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_::pos_sib_type_r17)::nof_types,
     decltype(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_::pos_sib_type_r17)::nof_exts,
     nullptr,
     &sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_::pos_sib_type_r17_opts::desc},
    {0,
     (int32_t)offsetof(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_, encrypted_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_, gnss_id_r17),
     (int32_t)offsetof(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_, gnss_id_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &gnss_id_r16_s::desc},
    {offsetof(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_, sbas_id_r17),
     (int32_t)offsetof(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_, sbas_id_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sbas_id_r16_s::desc}};
static const asn1::table::field_name seq_table_sib_type_info_v1700_s_sib_type_r17_c__type2_r17_s__names[] =
    {{"posSibType-r17", nullptr}, {"encrypted-r17", "true"}, {"gnss-id-r17", nullptr}, {"sbas-id-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sib_type_info_v1700_s_sib_type_r17_c__type2_r17_s__opts[] = {
    {1, (int32_t)offsetof(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_, encrypted_r17_present)},
    {2, (int32_t)offsetof(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_, gnss_id_r17_present)},
    {3, (int32_t)offsetof(sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_, sbas_id_r17_present)}};
static const asn1::table::seq_desc seq_table_sib_type_info_v1700_s_sib_type_r17_c__type2_r17_s_ = {
    {seq_table_sib_type_info_v1700_s_sib_type_r17_c__type2_r17_s__fields,
     4,
     UINT64_C(0x1),
     seq_table_sib_type_info_v1700_s_sib_type_r17_c__type2_r17_s__opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_sib_type_info_v1700_s_sib_type_r17_c__type2_r17_s__names};
static const asn1::table::seq_field choice_table_sib_type_info_v1700_s_sib_type_r17_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_>(),
     sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_::has_ext,
     false,
     sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_::nof_types,
     sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_::nof_exts,
     nullptr,
     &sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib_type_info_v1700_s_sib_type_r17_c__type2_r17_s_}};
static const asn1::table::field_name choice_table_sib_type_info_v1700_s_sib_type_r17_c__names[] = {
    {"type1-r17", nullptr},
    {"type2-r17", nullptr}};
const asn1::table::choice_desc sib_type_info_v1700_s::sib_type_r17_c_::desc = {
    asn1::table::enum_int_type<sib_type_info_v1700_s::sib_type_r17_c_::types>(),
    sib_type_info_v1700_s::sib_type_r17_c_::types::nof_types,
    sib_type_info_v1700_s::sib_type_r17_c_::types::nof_exts,
    sib_type_info_v1700_s::sib_type_r17_c_::types::has_ext,
    false,
    choice_table_sib_type_info_v1700_s_sib_type_r17_c__alternatives,
    nullptr,
    choice_table_sib_type_info_v1700_s_sib_type_r17_c__names,
    "sib_type_info_v1700_s::sib_type_r17_c_",
    offsetof(sib_type_info_v1700_s::sib_type_r17_c_, c.type),
    offsetof(sib_type_info_v1700_s::sib_type_r17_c_, c.value),
    false,
    &sib_type_info_v1700_s::sib_type_r17_c_::storage_desc};

static const char*           enum_names_sib_type_info_v1700_s_sib_type_r17_c__type1_r17_e__values[] = {"sibType15",
                                                                                                       "sibType16",
                                                                                                       "sibType17",
                                                                                                       "sibType18",
                                                                                                       "sibType19",
                                                                                                       "sibType20",
                                                                                                       "sibType21",
                                                                                                       "sibType22-v1800",
                                                                                                       "sibType23-v1800",
                                                                                                       "sibType24-v1800",
                                                                                                       "sibType25-v1800",
                                                                                                       "sibType17bis-v1820",
                                                                                                       "spare4",
                                                                                                       "spare3",
                                                                                                       "spare2",
                                                                                                       "spare1"};
const asn1::table::enum_desc sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_opts::desc           = {
    enum_names_sib_type_info_v1700_s_sib_type_r17_c__type1_r17_e__values,
    16,
    "sib_type_info_v1700_s::sib_type_r17_c_::type1_r17_e_"};

static const char* enum_names_sib_type_info_v1700_s_sib_type_r17_c__type2_r17_s__pos_sib_type_r17_e__values[] = {
    "posSibType1-9",
    "posSibType1-10",
    "posSibType2-24",
    "posSibType2-25",
    "posSibType6-4",
    "posSibType6-5",
    "posSibType6-6",
    "posSibType2-17a-v1770",
    "posSibType2-18a-v1770",
    "posSibType2-20a-v1770",
    "posSibType1-11-v1800",
    "posSibType1-12-v1800",
    "posSibType2-26-v1800",
    "posSibType2-27-v1800",
    "posSibType6-7-v1800",
    "posSibType7-1-v1800",
    "posSibType7-2-v1800",
    "posSibType7-3-v1800",
    "posSibType7-4-v1800"};
const asn1::table::enum_desc sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_::pos_sib_type_r17_opts::desc = {
    enum_names_sib_type_info_v1700_s_sib_type_r17_c__type2_r17_s__pos_sib_type_r17_e__values,
    19,
    "sib_type_info_v1700_s::sib_type_r17_c_::type2_r17_s_::pos_sib_type_r17_e_"};

static const char* enum_names_sib_type_info_v1700_s_sib_type_r17_c__types_values[]    = {"type1-r17", "type2-r17"};
const asn1::table::enum_desc sib_type_info_v1700_s::sib_type_r17_c_::types_opts::desc = {
    enum_names_sib_type_info_v1700_s_sib_type_r17_c__types_values,
    2,
    "sib_type_info_v1700_s::sib_type_r17_c_::types"};
uint8_t sib_type_info_v1700_s::sib_type_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "sib_type_info_v1700_s::sib_type_r17_c_::types");
}

// SchedulingInfo2-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sched_info2_r17_s_fields[] = {
    {offsetof(sched_info2_r17_s, si_broadcast_status_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sched_info2_r17_s::si_broadcast_status_r17)>(),
     decltype(sched_info2_r17_s::si_broadcast_status_r17)::has_ext,
     false,
     decltype(sched_info2_r17_s::si_broadcast_status_r17)::nof_types,
     decltype(sched_info2_r17_s::si_broadcast_status_r17)::nof_exts,
     nullptr,
     &sched_info2_r17_s::si_broadcast_status_r17_opts::desc},
    {offsetof(sched_info2_r17_s, si_win_position_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(256),
     nullptr},
    {offsetof(sched_info2_r17_s, si_periodicity_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sched_info2_r17_s::si_periodicity_r17)>(),
     decltype(sched_info2_r17_s::si_periodicity_r17)::has_ext,
     false,
     decltype(sched_info2_r17_s::si_periodicity_r17)::nof_types,
     decltype(sched_info2_r17_s::si_periodicity_r17)::nof_exts,
     nullptr,
     &sched_info2_r17_s::si_periodicity_r17_opts::desc},
    {offsetof(sched_info2_r17_s, sib_map_info_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(sched_info2_r17_s::sib_map_info_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sib_type_info_v1700_s::desc}};
static const asn1::table::field_name seq_table_sched_info2_r17_s_names[] = {{"si-BroadcastStatus-r17", nullptr},
                                                                            {"si-WindowPosition-r17", nullptr},
                                                                            {"si-Periodicity-r17", nullptr},
                                                                            {"sib-MappingInfo-r17", nullptr}};
const asn1::table::seq_desc          sched_info2_r17_s::desc             = {
    {seq_table_sched_info2_r17_s_fields, 4, UINT64_C(0xf), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sched_info2_r17_s_names};

static const char*           enum_names_sched_info2_r17_s_si_broadcast_status_r17_e__values[] = {"broadcasting",
                                                                                                 "notBroadcasting"};
const asn1::table::enum_desc sched_info2_r17_s::si_broadcast_status_r17_opts::desc            = {
    enum_names_sched_info2_r17_s_si_broadcast_status_r17_e__values,
    2,
    "sched_info2_r17_s::si_broadcast_status_r17_e_"};

static const char* enum_names_sched_info2_r17_s_si_periodicity_r17_e__values[] =
    {"rf8", "rf16", "rf32", "rf64", "rf128", "rf256", "rf512"};
const asn1::table::enum_desc sched_info2_r17_s::si_periodicity_r17_opts::desc = {
    enum_names_sched_info2_r17_s_si_periodicity_r17_e__values,
    7,
    "sched_info2_r17_s::si_periodicity_r17_e_"};
uint16_t sched_info2_r17_s::si_periodicity_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 16, 32, 64, 128, 256, 512};
  return map_enum_number(numbers, 7, value, "sched_info2_r17_s::si_periodicity_r17_e_");
}

// SI-SchedulingInfo-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_si_sched_info_v1700_s_fields[] = {
    {offsetof(si_sched_info_v1700_s, sched_info_list2_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(si_sched_info_v1700_s::sched_info_list2_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sched_info2_r17_s::desc},
    {offsetof(si_sched_info_v1700_s, dummy),
     (int32_t)offsetof(si_sched_info_v1700_s, dummy_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_request_cfg_s::desc}};
static const asn1::table::field_name seq_table_si_sched_info_v1700_s_names[] = {{"schedulingInfoList2-r17", nullptr},
                                                                                {"dummy", nullptr}};
static const asn1::table::seq_opt    seq_table_si_sched_info_v1700_s_opts[]  = {
    {1, (int32_t)offsetof(si_sched_info_v1700_s, dummy_present)}};
const asn1::table::seq_desc si_sched_info_v1700_s::desc = {
    {seq_table_si_sched_info_v1700_s_fields, 2, UINT64_C(0x1), seq_table_si_sched_info_v1700_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_si_sched_info_v1700_s_names};

// SI-SchedulingInfo-v1740 ::= SEQUENCE
static const asn1::table::seq_field seq_table_si_sched_info_v1740_s_fields[] = {
    {offsetof(si_sched_info_v1740_s, si_request_cfg_red_cap_r17),
     (int32_t)offsetof(si_sched_info_v1740_s, si_request_cfg_red_cap_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_request_cfg_s::desc}};
static const asn1::table::field_name seq_table_si_sched_info_v1740_s_names[] = {
    {"si-RequestConfigRedCap-r17", nullptr}};
static const asn1::table::seq_opt seq_table_si_sched_info_v1740_s_opts[] = {
    {0, (int32_t)offsetof(si_sched_info_v1740_s, si_request_cfg_red_cap_r17_present)}};
const asn1::table::seq_desc si_sched_info_v1740_s::desc = {
    {seq_table_si_sched_info_v1740_s_fields, 1, UINT64_C(0x0), seq_table_si_sched_info_v1740_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_si_sched_info_v1740_s_names};

// MT-SDT-ConfigCommonSIB-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mt_sdt_cfg_common_sib_r18_s_fields[] = {
    {offsetof(mt_sdt_cfg_common_sib_r18_s, mt_sdt_rsrp_thres_r18),
     (int32_t)offsetof(mt_sdt_cfg_common_sib_r18_s, mt_sdt_rsrp_thres_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(mt_sdt_cfg_common_sib_r18_s, sdt_lc_ch_sr_delay_timer_r18),
     (int32_t)offsetof(mt_sdt_cfg_common_sib_r18_s, sdt_lc_ch_sr_delay_timer_r18_present),
     asn1::table::enum_op<decltype(mt_sdt_cfg_common_sib_r18_s::sdt_lc_ch_sr_delay_timer_r18)>(),
     decltype(mt_sdt_cfg_common_sib_r18_s::sdt_lc_ch_sr_delay_timer_r18)::has_ext,
     false,
     decltype(mt_sdt_cfg_common_sib_r18_s::sdt_lc_ch_sr_delay_timer_r18)::nof_types,
     decltype(mt_sdt_cfg_common_sib_r18_s::sdt_lc_ch_sr_delay_timer_r18)::nof_exts,
     nullptr,
     &mt_sdt_cfg_common_sib_r18_s::sdt_lc_ch_sr_delay_timer_r18_opts::desc},
    {offsetof(mt_sdt_cfg_common_sib_r18_s, t319a_r18),
     (int32_t)offsetof(mt_sdt_cfg_common_sib_r18_s, t319a_r18_present),
     asn1::table::enum_op<decltype(mt_sdt_cfg_common_sib_r18_s::t319a_r18)>(),
     decltype(mt_sdt_cfg_common_sib_r18_s::t319a_r18)::has_ext,
     false,
     decltype(mt_sdt_cfg_common_sib_r18_s::t319a_r18)::nof_types,
     decltype(mt_sdt_cfg_common_sib_r18_s::t319a_r18)::nof_exts,
     nullptr,
     &mt_sdt_cfg_common_sib_r18_s::t319a_r18_opts::desc}};
static const asn1::table::field_name seq_table_mt_sdt_cfg_common_sib_r18_s_names[] = {
    {"mt-SDT-RSRP-Threshold-r18", nullptr},
    {"sdt-LogicalChannelSR-DelayTimer-r18", nullptr},
    {"t319a-r18", nullptr}};
static const asn1::table::seq_opt seq_table_mt_sdt_cfg_common_sib_r18_s_opts[] = {
    {0, (int32_t)offsetof(mt_sdt_cfg_common_sib_r18_s, mt_sdt_rsrp_thres_r18_present)},
    {1, (int32_t)offsetof(mt_sdt_cfg_common_sib_r18_s, sdt_lc_ch_sr_delay_timer_r18_present)},
    {2, (int32_t)offsetof(mt_sdt_cfg_common_sib_r18_s, t319a_r18_present)}};
const asn1::table::seq_desc mt_sdt_cfg_common_sib_r18_s::desc = {
    {seq_table_mt_sdt_cfg_common_sib_r18_s_fields, 3, UINT64_C(0x0), seq_table_mt_sdt_cfg_common_sib_r18_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_mt_sdt_cfg_common_sib_r18_s_names};

static const char* enum_names_mt_sdt_cfg_common_sib_r18_s_sdt_lc_ch_sr_delay_timer_r18_e__values[] =
    {"sf20", "sf40", "sf64", "sf128", "sf512", "sf1024", "sf2560", "spare1"};
const asn1::table::enum_desc mt_sdt_cfg_common_sib_r18_s::sdt_lc_ch_sr_delay_timer_r18_opts::desc = {
    enum_names_mt_sdt_cfg_common_sib_r18_s_sdt_lc_ch_sr_delay_timer_r18_e__values,
    8,
    "mt_sdt_cfg_common_sib_r18_s::sdt_lc_ch_sr_delay_timer_r18_e_"};
uint16_t mt_sdt_cfg_common_sib_r18_s::sdt_lc_ch_sr_delay_timer_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {20, 40, 64, 128, 512, 1024, 2560};
  return map_enum_number(numbers, 7, value, "mt_sdt_cfg_common_sib_r18_s::sdt_lc_ch_sr_delay_timer_r18_e_");
}

static const char*           enum_names_mt_sdt_cfg_common_sib_r18_s_t319a_r18_e__values[] = {"ms100",
                                                                                             "ms200",
                                                                                             "ms300",
                                                                                             "ms400",
                                                                                             "ms600",
                                                                                             "ms1000",
                                                                                             "ms2000",
                                                                                             "ms3000",
                                                                                             "ms4000",
                                                                                             "spare7",
                                                                                             "spare6",
                                                                                             "spare5",
                                                                                             "spare4",
                                                                                             "spare3",
                                                                                             "spare2",
                                                                                             "spare1"};
const asn1::table::enum_desc mt_sdt_cfg_common_sib_r18_s::t319a_r18_opts::desc            = {
    enum_names_mt_sdt_cfg_common_sib_r18_s_t319a_r18_e__values,
    16,
    "mt_sdt_cfg_common_sib_r18_s::t319a_r18_e_"};
uint16_t mt_sdt_cfg_common_sib_r18_s::t319a_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 2000, 3000, 4000};
  return map_enum_number(numbers, 9, value, "mt_sdt_cfg_common_sib_r18_s::t319a_r18_e_");
}

// SI-SchedulingInfo-v1800 ::= SEQUENCE
static const asn1::table::seq_field seq_table_si_sched_info_v1800_s_fields[] = {
    {offsetof(si_sched_info_v1800_s, si_request_cfg_msg1_repeat_r18),
     (int32_t)offsetof(si_sched_info_v1800_s, si_request_cfg_msg1_repeat_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_request_cfg_repeat_r18_s::desc},
    {offsetof(si_sched_info_v1800_s, si_request_cfg_red_cap_msg1_repeat_r18),
     (int32_t)offsetof(si_sched_info_v1800_s, si_request_cfg_red_cap_msg1_repeat_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_request_cfg_repeat_r18_s::desc},
    {offsetof(si_sched_info_v1800_s, si_request_cfg_sul_msg1_repeat_r18),
     (int32_t)offsetof(si_sched_info_v1800_s, si_request_cfg_sul_msg1_repeat_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_request_cfg_repeat_r18_s::desc}};
static const asn1::table::field_name seq_table_si_sched_info_v1800_s_names[] = {
    {"si-RequestConfigMSG1-Repetition-r18", nullptr},
    {"si-RequestConfigRedCap-MSG1-Repetition-r18", nullptr},
    {"si-RequestConfigSUL-MSG1-Repetition-r18", nullptr}};
static const asn1::table::seq_opt seq_table_si_sched_info_v1800_s_opts[] = {
    {0, (int32_t)offsetof(si_sched_info_v1800_s, si_request_cfg_msg1_repeat_r18_present)},
    {1, (int32_t)offsetof(si_sched_info_v1800_s, si_request_cfg_red_cap_msg1_repeat_r18_present)},
    {2, (int32_t)offsetof(si_sched_info_v1800_s, si_request_cfg_sul_msg1_repeat_r18_present)}};
const asn1::table::seq_desc si_sched_info_v1800_s::desc = {
    {seq_table_si_sched_info_v1800_s_fields, 3, UINT64_C(0x0), seq_table_si_sched_info_v1800_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_si_sched_info_v1800_s_names};

// ERedCap-ConfigCommonSIB-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e_red_cap_cfg_common_sib_r18_s_cell_barred_e_red_cap_r18_s__fields[] = {
    {offsetof(e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_, cell_barred_e_red_cap1_rx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<
         decltype(e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap1_rx_r18)>(),
     decltype(e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap1_rx_r18)::has_ext,
     false,
     decltype(e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap1_rx_r18)::nof_types,
     decltype(e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap1_rx_r18)::nof_exts,
     nullptr,
     &e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap1_rx_r18_opts::desc},
    {offsetof(e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_, cell_barred_e_red_cap2_rx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<
         decltype(e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap2_rx_r18)>(),
     decltype(e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap2_rx_r18)::has_ext,
     false,
     decltype(e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap2_rx_r18)::nof_types,
     decltype(e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap2_rx_r18)::nof_exts,
     nullptr,
     &e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap2_rx_r18_opts::desc}};
static const asn1::table::field_name seq_table_e_red_cap_cfg_common_sib_r18_s_cell_barred_e_red_cap_r18_s__names[] = {
    {"cellBarred-eRedCap1Rx-r18", nullptr},
    {"cellBarred-eRedCap2Rx-r18", nullptr}};
static const asn1::table::seq_desc seq_table_e_red_cap_cfg_common_sib_r18_s_cell_barred_e_red_cap_r18_s_ = {
    {seq_table_e_red_cap_cfg_common_sib_r18_s_cell_barred_e_red_cap_r18_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_e_red_cap_cfg_common_sib_r18_s_cell_barred_e_red_cap_r18_s__names};
static const asn1::table::seq_field seq_table_e_red_cap_cfg_common_sib_r18_s_fields[] = {
    {offsetof(e_red_cap_cfg_common_sib_r18_s, cell_barred_e_red_cap_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_e_red_cap_cfg_common_sib_r18_s_cell_barred_e_red_cap_r18_s_}};
static const asn1::table::field_name seq_table_e_red_cap_cfg_common_sib_r18_s_names[] = {
    {"cellBarred-eRedCap-r18", nullptr}};
const asn1::table::seq_desc e_red_cap_cfg_common_sib_r18_s::desc = {
    {seq_table_e_red_cap_cfg_common_sib_r18_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_e_red_cap_cfg_common_sib_r18_s_names};

static const char*
    enum_names_e_red_cap_cfg_common_sib_r18_s_cell_barred_e_red_cap_r18_s__cell_barred_e_red_cap1_rx_r18_e__values[] = {
        "barred",
        "notBarred"};
const asn1::table::enum_desc
    e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap1_rx_r18_opts::desc = {
        enum_names_e_red_cap_cfg_common_sib_r18_s_cell_barred_e_red_cap_r18_s__cell_barred_e_red_cap1_rx_r18_e__values,
        2,
        "e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap1_rx_r18_e_"};

static const char*
    enum_names_e_red_cap_cfg_common_sib_r18_s_cell_barred_e_red_cap_r18_s__cell_barred_e_red_cap2_rx_r18_e__values[] = {
        "barred",
        "notBarred"};
const asn1::table::enum_desc
    e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap2_rx_r18_opts::desc = {
        enum_names_e_red_cap_cfg_common_sib_r18_s_cell_barred_e_red_cap_r18_s__cell_barred_e_red_cap2_rx_r18_e__values,
        2,
        "e_red_cap_cfg_common_sib_r18_s::cell_barred_e_red_cap_r18_s_::cell_barred_e_red_cap2_rx_r18_e_"};

// SIB1-v1800-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib1_v1800_ies_s_feature_priorities_v1800_s__fields[] = {
    {offsetof(sib1_v1800_ies_s::feature_priorities_v1800_s_, msg1_repeats_prio_r18),
     (int32_t)offsetof(sib1_v1800_ies_s::feature_priorities_v1800_s_, msg1_repeats_prio_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(sib1_v1800_ies_s::feature_priorities_v1800_s_, ered_cap_prio_r18),
     (int32_t)offsetof(sib1_v1800_ies_s::feature_priorities_v1800_s_, ered_cap_prio_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr}};
static const asn1::table::field_name seq_table_sib1_v1800_ies_s_feature_priorities_v1800_s__names[] = {
    {"msg1-Repetitions-Priority-r18", nullptr},
    {"eRedCapPriority-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sib1_v1800_ies_s_feature_priorities_v1800_s__opts[] = {
    {0, (int32_t)offsetof(sib1_v1800_ies_s::feature_priorities_v1800_s_, msg1_repeats_prio_r18_present)},
    {1, (int32_t)offsetof(sib1_v1800_ies_s::feature_priorities_v1800_s_, ered_cap_prio_r18_present)}};
static const asn1::table::seq_desc seq_table_sib1_v1800_ies_s_feature_priorities_v1800_s_ = {
    {seq_table_sib1_v1800_ies_s_feature_priorities_v1800_s__fields,
     2,
     UINT64_C(0x0),
     seq_table_sib1_v1800_ies_s_feature_priorities_v1800_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_sib1_v1800_ies_s_feature_priorities_v1800_s__names};
static const asn1::table::seq_field seq_table_sib1_v1800_ies_s_fields[] = {
    {0,
     (int32_t)offsetof(sib1_v1800_ies_s, ncr_support_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib1_v1800_ies_s, mt_sdt_cfg_common_sib_r18),
     (int32_t)offsetof(sib1_v1800_ies_s, mt_sdt_cfg_common_sib_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mt_sdt_cfg_common_sib_r18_s::desc},
    {0,
     (int32_t)offsetof(sib1_v1800_ies_s, musim_cap_restrict_allowed_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib1_v1800_ies_s, feature_priorities_v1800),
     (int32_t)offsetof(sib1_v1800_ies_s, feature_priorities_v1800_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib1_v1800_ies_s_feature_priorities_v1800_s_},
    {offsetof(sib1_v1800_ies_s, si_sched_info_v1800),
     (int32_t)offsetof(sib1_v1800_ies_s, si_sched_info_v1800_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_sched_info_v1800_s::desc},
    {offsetof(sib1_v1800_ies_s, cell_barred_atg_r18),
     (int32_t)offsetof(sib1_v1800_ies_s, cell_barred_atg_r18_present),
     asn1::table::enum_op<decltype(sib1_v1800_ies_s::cell_barred_atg_r18)>(),
     decltype(sib1_v1800_ies_s::cell_barred_atg_r18)::has_ext,
     false,
     decltype(sib1_v1800_ies_s::cell_barred_atg_r18)::nof_types,
     decltype(sib1_v1800_ies_s::cell_barred_atg_r18)::nof_exts,
     nullptr,
     &sib1_v1800_ies_s::cell_barred_atg_r18_opts::desc},
    {0,
     (int32_t)offsetof(sib1_v1800_ies_s, cell_barred_nes_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sib1_v1800_ies_s, mobile_iab_cell_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sib1_v1800_ies_s, edrx_allowed_inactive_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib1_v1800_ies_s, intra_freq_resel_e_red_cap_r18),
     (int32_t)offsetof(sib1_v1800_ies_s, intra_freq_resel_e_red_cap_r18_present),
     asn1::table::enum_op<decltype(sib1_v1800_ies_s::intra_freq_resel_e_red_cap_r18)>(),
     decltype(sib1_v1800_ies_s::intra_freq_resel_e_red_cap_r18)::has_ext,
     false,
     decltype(sib1_v1800_ies_s::intra_freq_resel_e_red_cap_r18)::nof_types,
     decltype(sib1_v1800_ies_s::intra_freq_resel_e_red_cap_r18)::nof_exts,
     nullptr,
     &sib1_v1800_ies_s::intra_freq_resel_e_red_cap_r18_opts::desc},
    {0,
     (int32_t)offsetof(sib1_v1800_ies_s, non_serving_cell_mii_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib1_v1800_ies_s, sdt_beam_fail_recovery_prohibit_timer_r18),
     (int32_t)offsetof(sib1_v1800_ies_s, sdt_beam_fail_recovery_prohibit_timer_r18_present),
     asn1::table::enum_op<decltype(sib1_v1800_ies_s::sdt_beam_fail_recovery_prohibit_timer_r18)>(),
     decltype(sib1_v1800_ies_s::sdt_beam_fail_recovery_prohibit_timer_r18)::has_ext,
     false,
     decltype(sib1_v1800_ies_s::sdt_beam_fail_recovery_prohibit_timer_r18)::nof_types,
     decltype(sib1_v1800_ies_s::sdt_beam_fail_recovery_prohibit_timer_r18)::nof_exts,
     nullptr,
     &sib1_v1800_ies_s::sdt_beam_fail_recovery_prohibit_timer_r18_opts::desc},
    {offsetof(sib1_v1800_ies_s, ered_cap_cfg_common_r18),
     (int32_t)offsetof(sib1_v1800_ies_s, ered_cap_cfg_common_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e_red_cap_cfg_common_sib_r18_s::desc},
    {offsetof(sib1_v1800_ies_s, cell_barred_fixed_vsat_r18),
     (int32_t)offsetof(sib1_v1800_ies_s, cell_barred_fixed_vsat_r18_present),
     asn1::table::enum_op<decltype(sib1_v1800_ies_s::cell_barred_fixed_vsat_r18)>(),
     decltype(sib1_v1800_ies_s::cell_barred_fixed_vsat_r18)::has_ext,
     false,
     decltype(sib1_v1800_ies_s::cell_barred_fixed_vsat_r18)::nof_types,
     decltype(sib1_v1800_ies_s::cell_barred_fixed_vsat_r18)::nof_exts,
     nullptr,
     &sib1_v1800_ies_s::cell_barred_fixed_vsat_r18_opts::desc},
    {offsetof(sib1_v1800_ies_s, cell_barred_mobile_vsat_r18),
     (int32_t)offsetof(sib1_v1800_ies_s, cell_barred_mobile_vsat_r18_present),
     asn1::table::enum_op<decltype(sib1_v1800_ies_s::cell_barred_mobile_vsat_r18)>(),
     decltype(sib1_v1800_ies_s::cell_barred_mobile_vsat_r18)::has_ext,
     false,
     decltype(sib1_v1800_ies_s::cell_barred_mobile_vsat_r18)::nof_types,
     decltype(sib1_v1800_ies_s::cell_barred_mobile_vsat_r18)::nof_exts,
     nullptr,
     &sib1_v1800_ies_s::cell_barred_mobile_vsat_r18_opts::desc},
    {0,
     (int32_t)offsetof(sib1_v1800_ies_s, resel_meass_nr_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sib1_v1800_ies_s, cell_barred2_rx_xr_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib1_v1800_ies_s, intra_freq_resel2_rx_xr_r18),
     (int32_t)offsetof(sib1_v1800_ies_s, intra_freq_resel2_rx_xr_r18_present),
     asn1::table::enum_op<decltype(sib1_v1800_ies_s::intra_freq_resel2_rx_xr_r18)>(),
     decltype(sib1_v1800_ies_s::intra_freq_resel2_rx_xr_r18)::has_ext,
     false,
     decltype(sib1_v1800_ies_s::intra_freq_resel2_rx_xr_r18)::nof_types,
     decltype(sib1_v1800_ies_s::intra_freq_resel2_rx_xr_r18)::nof_exts,
     nullptr,
     &sib1_v1800_ies_s::intra_freq_resel2_rx_xr_r18_opts::desc},
    {0,
     (int32_t)offsetof(sib1_v1800_ies_s, barr_exempt_emergency_call_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sib1_v1800_ies_s, n3c_support_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sib1_v1800_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_sib1_v1800_ies_s_names[] = {
    {"ncr-Support-r18", "true"},
    {"mt-SDT-ConfigCommonSIB-r18", nullptr},
    {"musim-CapRestrictionAllowed-r18", "true"},
    {"featurePriorities-v1800", nullptr},
    {"si-SchedulingInfo-v1800", nullptr},
    {"cellBarredATG-r18", nullptr},
    {"cellBarredNES-r18", "notBarred"},
    {"mobileIAB-Cell-r18", "true"},
    {"eDRX-AllowedInactive-r18", "true"},
    {"intraFreqReselection-eRedCap-r18", nullptr},
    {"nonServingCellMII-r18", "true"},
    {"sdt-BeamFailureRecoveryProhibitTimer-r18", nullptr},
    {"eRedCap-ConfigCommon-r18", nullptr},
    {"cellBarredFixedVSAT-r18", nullptr},
    {"cellBarredMobileVSAT-r18", nullptr},
    {"reselectionMeasurementsNR-r18", "true"},
    {"cellBarred2RxXR-r18", "barred"},
    {"intraFreqReselection2RxXR-r18", nullptr},
    {"barringExemptEmergencyCall-r18", "true"},
    {"n3c-Support-r18", "true"},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_sib1_v1800_ies_s_opts[] = {
    {0, (int32_t)offsetof(sib1_v1800_ies_s, ncr_support_r18_present)},
    {1, (int32_t)offsetof(sib1_v1800_ies_s, mt_sdt_cfg_common_sib_r18_present)},
    {2, (int32_t)offsetof(sib1_v1800_ies_s, musim_cap_restrict_allowed_r18_present)},
    {3, (int32_t)offsetof(sib1_v1800_ies_s, feature_priorities_v1800_present)},
    {4, (int32_t)offsetof(sib1_v1800_ies_s, si_sched_info_v1800_present)},
    {5, (int32_t)offsetof(sib1_v1800_ies_s, cell_barred_atg_r18_present)},
    {6, (int32_t)offsetof(sib1_v1800_ies_s, cell_barred_nes_r18_present)},
    {7, (int32_t)offsetof(sib1_v1800_ies_s, mobile_iab_cell_r18_present)},
    {8, (int32_t)offsetof(sib1_v1800_ies_s, edrx_allowed_inactive_r18_present)},
    {9, (int32_t)offsetof(sib1_v1800_ies_s, intra_freq_resel_e_red_cap_r18_present)},
    {10, (int32_t)offsetof(sib1_v1800_ies_s, non_serving_cell_mii_r18_present)},
    {11, (int32_t)offsetof(sib1_v1800_ies_s, sdt_beam_fail_recovery_prohibit_timer_r18_present)},
    {12, (int32_t)offsetof(sib1_v1800_ies_s, ered_cap_cfg_common_r18_present)},
    {13, (int32_t)offsetof(sib1_v1800_ies_s, cell_barred_fixed_vsat_r18_present)},
    {14, (int32_t)offsetof(sib1_v1800_ies_s, cell_barred_mobile_vsat_r18_present)},
    {15, (int32_t)offsetof(sib1_v1800_ies_s, resel_meass_nr_r18_present)},
    {16, (int32_t)offsetof(sib1_v1800_ies_s, cell_barred2_rx_xr_r18_present)},
    {17, (int32_t)offsetof(sib1_v1800_ies_s, intra_freq_resel2_rx_xr_r18_present)},
    {18, (int32_t)offsetof(sib1_v1800_ies_s, barr_exempt_emergency_call_r18_present)},
    {19, (int32_t)offsetof(sib1_v1800_ies_s, n3c_support_r18_present)},
    {20, (int32_t)offsetof(sib1_v1800_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc sib1_v1800_ies_s::desc = {
    {seq_table_sib1_v1800_ies_s_fields, 21, UINT64_C(0x0), seq_table_sib1_v1800_ies_s_opts, 21},
    false,
    false,
    nullptr,
    0,
    seq_table_sib1_v1800_ies_s_names};

static const char*           enum_names_sib1_v1800_ies_s_cell_barred_atg_r18_e__values[] = {"barred", "notBarred"};
const asn1::table::enum_desc sib1_v1800_ies_s::cell_barred_atg_r18_opts::desc            = {
    enum_names_sib1_v1800_ies_s_cell_barred_atg_r18_e__values,
    2,
    "sib1_v1800_ies_s::cell_barred_atg_r18_e_"};

static const char* enum_names_sib1_v1800_ies_s_intra_freq_resel_e_red_cap_r18_e__values[] = {"allowed", "notAllowed"};
const asn1::table::enum_desc sib1_v1800_ies_s::intra_freq_resel_e_red_cap_r18_opts::desc  = {
    enum_names_sib1_v1800_ies_s_intra_freq_resel_e_red_cap_r18_e__values,
    2,
    "sib1_v1800_ies_s::intra_freq_resel_e_red_cap_r18_e_"};

static const char* enum_names_sib1_v1800_ies_s_sdt_beam_fail_recovery_prohibit_timer_r18_e__values[] =
    {"ms50", "ms100", "ms200", "ms500", "ms1000", "ms1500", "ms2000", "ms3000"};
const asn1::table::enum_desc sib1_v1800_ies_s::sdt_beam_fail_recovery_prohibit_timer_r18_opts::desc = {
    enum_names_sib1_v1800_ies_s_sdt_beam_fail_recovery_prohibit_timer_r18_e__values,
    8,
    "sib1_v1800_ies_s::sdt_beam_fail_recovery_prohibit_timer_r18_e_"};
uint16_t sib1_v1800_ies_s::sdt_beam_fail_recovery_prohibit_timer_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {50, 100, 200, 500, 1000, 1500, 2000, 3000};
  return map_enum_number(numbers, 8, value, "sib1_v1800_ies_s::sdt_beam_fail_recovery_prohibit_timer_r18_e_");
}

static const char* enum_names_sib1_v1800_ies_s_cell_barred_fixed_vsat_r18_e__values[] = {"barred", "notBarred"};
const asn1::table::enum_desc sib1_v1800_ies_s::cell_barred_fixed_vsat_r18_opts::desc  = {
    enum_names_sib1_v1800_ies_s_cell_barred_fixed_vsat_r18_e__values,
    2,
    "sib1_v1800_ies_s::cell_barred_fixed_vsat_r18_e_"};

static const char* enum_names_sib1_v1800_ies_s_cell_barred_mobile_vsat_r18_e__values[] = {"barred", "notBarred"};
const asn1::table::enum_desc sib1_v1800_ies_s::cell_barred_mobile_vsat_r18_opts::desc  = {
    enum_names_sib1_v1800_ies_s_cell_barred_mobile_vsat_r18_e__values,
    2,
    "sib1_v1800_ies_s::cell_barred_mobile_vsat_r18_e_"};

static const char* enum_names_sib1_v1800_ies_s_intra_freq_resel2_rx_xr_r18_e__values[] = {"allowed", "notAllowed"};
const asn1::table::enum_desc sib1_v1800_ies_s::intra_freq_resel2_rx_xr_r18_opts::desc  = {
    enum_names_sib1_v1800_ies_s_intra_freq_resel2_rx_xr_r18_e__values,
    2,
    "sib1_v1800_ies_s::intra_freq_resel2_rx_xr_r18_e_"};

// SIB1-v1740-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib1_v1740_ies_s_fields[] = {
    {offsetof(sib1_v1740_ies_s, si_sched_info_v1740),
     (int32_t)offsetof(sib1_v1740_ies_s, si_sched_info_v1740_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_sched_info_v1740_s::desc},
    {offsetof(sib1_v1740_ies_s, non_crit_ext),
     (int32_t)offsetof(sib1_v1740_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib1_v1800_ies_s::desc}};
static const asn1::table::field_name seq_table_sib1_v1740_ies_s_names[] = {{"si-SchedulingInfo-v1740", nullptr},
                                                                           {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib1_v1740_ies_s_opts[]  = {
    {0, (int32_t)offsetof(sib1_v1740_ies_s, si_sched_info_v1740_present)},
    {1, (int32_t)offsetof(sib1_v1740_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc sib1_v1740_ies_s::desc = {
    {seq_table_sib1_v1740_ies_s_fields, 2, UINT64_C(0x0), seq_table_sib1_v1740_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_sib1_v1740_ies_s_names};

// SIB1-v1700-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib1_v1700_ies_s_uac_barr_info_v1700_s__fields[] = {
    {offsetof(sib1_v1700_ies_s::uac_barr_info_v1700_s_, uac_barr_info_set_list_v1700),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sib1_v1700_ies_s::uac_barr_info_v1700_s_::uac_barr_info_set_list_v1700),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &uac_barr_info_set_v1700_s::desc}};
static const asn1::table::field_name seq_table_sib1_v1700_ies_s_uac_barr_info_v1700_s__names[] = {
    {"uac-BarringInfoSetList-v1700", nullptr}};
static const asn1::table::seq_desc seq_table_sib1_v1700_ies_s_uac_barr_info_v1700_s_ = {
    {seq_table_sib1_v1700_ies_s_uac_barr_info_v1700_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sib1_v1700_ies_s_uac_barr_info_v1700_s__names};
static const asn1::table::seq_field seq_table_sib1_v1700_ies_s_feature_priorities_r17_s__fields[] = {
    {offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, red_cap_prio_r17),
     (int32_t)offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, red_cap_prio_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, slicing_prio_r17),
     (int32_t)offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, slicing_prio_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, msg3_repeats_prio_r17),
     (int32_t)offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, msg3_repeats_prio_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, sdt_prio_r17),
     (int32_t)offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, sdt_prio_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr}};
static const asn1::table::field_name seq_table_sib1_v1700_ies_s_feature_priorities_r17_s__names[] = {
    {"redCapPriority-r17", nullptr},
    {"slicingPriority-r17", nullptr},
    {"msg3-Repetitions-Priority-r17", nullptr},
    {"sdt-Priority-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sib1_v1700_ies_s_feature_priorities_r17_s__opts[] = {
    {0, (int32_t)offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, red_cap_prio_r17_present)},
    {1, (int32_t)offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, slicing_prio_r17_present)},
    {2, (int32_t)offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, msg3_repeats_prio_r17_present)},
    {3, (int32_t)offsetof(sib1_v1700_ies_s::feature_priorities_r17_s_, sdt_prio_r17_present)}};
static const asn1::table::seq_desc seq_table_sib1_v1700_ies_s_feature_priorities_r17_s_ = {
    {seq_table_sib1_v1700_ies_s_feature_priorities_r17_s__fields,
     4,
     UINT64_C(0x0),
     seq_table_sib1_v1700_ies_s_feature_priorities_r17_s__opts,
     4},
    false,
    false,
    nullptr,
    0,
    seq_table_sib1_v1700_ies_s_feature_priorities_r17_s__names};
static const asn1::table::seq_field seq_table_sib1_v1700_ies_s_fields[] = {
    {0,
     (int32_t)offsetof(sib1_v1700_ies_s, hsdn_cell_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib1_v1700_ies_s, uac_barr_info_v1700),
     (int32_t)offsetof(sib1_v1700_ies_s, uac_barr_info_v1700_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib1_v1700_ies_s_uac_barr_info_v1700_s_},
    {offsetof(sib1_v1700_ies_s, sdt_cfg_common_r17),
     (int32_t)offsetof(sib1_v1700_ies_s, sdt_cfg_common_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sdt_cfg_common_sib_r17_s::desc},
    {offsetof(sib1_v1700_ies_s, red_cap_cfg_common_r17),
     (int32_t)offsetof(sib1_v1700_ies_s, red_cap_cfg_common_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &red_cap_cfg_common_sib_r17_s::desc},
    {offsetof(sib1_v1700_ies_s, feature_priorities_r17),
     (int32_t)offsetof(sib1_v1700_ies_s, feature_priorities_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib1_v1700_ies_s_feature_priorities_r17_s_},
    {offsetof(sib1_v1700_ies_s, si_sched_info_v1700),
     (int32_t)offsetof(sib1_v1700_ies_s, si_sched_info_v1700_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_sched_info_v1700_s::desc},
    {offsetof(sib1_v1700_ies_s, hyper_sfn_r17),
     (int32_t)offsetof(sib1_v1700_ies_s, hyper_sfn_r17_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(sib1_v1700_ies_s::hyper_sfn_r17), asn1::table::struct_json>::value},
    {0,
     (int32_t)offsetof(sib1_v1700_ies_s, edrx_allowed_idle_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sib1_v1700_ies_s, edrx_allowed_inactive_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib1_v1700_ies_s, intra_freq_resel_red_cap_r17),
     (int32_t)offsetof(sib1_v1700_ies_s, intra_freq_resel_red_cap_r17_present),
     asn1::table::enum_op<decltype(sib1_v1700_ies_s::intra_freq_resel_red_cap_r17)>(),
     decltype(sib1_v1700_ies_s::intra_freq_resel_red_cap_r17)::has_ext,
     false,
     decltype(sib1_v1700_ies_s::intra_freq_resel_red_cap_r17)::nof_types,
     decltype(sib1_v1700_ies_s::intra_freq_resel_red_cap_r17)::nof_exts,
     nullptr,
     &sib1_v1700_ies_s::intra_freq_resel_red_cap_r17_opts::desc},
    {offsetof(sib1_v1700_ies_s, cell_barred_ntn_r17),
     (int32_t)offsetof(sib1_v1700_ies_s, cell_barred_ntn_r17_present),
     asn1::table::enum_op<decltype(sib1_v1700_ies_s::cell_barred_ntn_r17)>(),
     decltype(sib1_v1700_ies_s::cell_barred_ntn_r17)::has_ext,
     false,
     decltype(sib1_v1700_ies_s::cell_barred_ntn_r17)::nof_types,
     decltype(sib1_v1700_ies_s::cell_barred_ntn_r17)::nof_exts,
     nullptr,
     &sib1_v1700_ies_s::cell_barred_ntn_r17_opts::desc},
    {offsetof(sib1_v1700_ies_s, non_crit_ext),
     (int32_t)offsetof(sib1_v1700_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib1_v1740_ies_s::desc}};
static const asn1::table::field_name seq_table_sib1_v1700_ies_s_names[] = {{"hsdn-Cell-r17", "true"},
                                                                           {"uac-BarringInfo-v1700", nullptr},
                                                                           {"sdt-ConfigCommon-r17", nullptr},
                                                                           {"redCap-ConfigCommon-r17", nullptr},
                                                                           {"featurePriorities-r17", nullptr},
                                                                           {"si-SchedulingInfo-v1700", nullptr},
                                                                           {"hyperSFN-r17", nullptr},
                                                                           {"eDRX-AllowedIdle-r17", "true"},
                                                                           {"eDRX-AllowedInactive-r17", "true"},
                                                                           {"intraFreqReselectionRedCap-r17", nullptr},
                                                                           {"cellBarredNTN-r17", nullptr},
                                                                           {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib1_v1700_ies_s_opts[]  = {
    {0, (int32_t)offsetof(sib1_v1700_ies_s, hsdn_cell_r17_present)},
    {1, (int32_t)offsetof(sib1_v1700_ies_s, uac_barr_info_v1700_present)},
    {2, (int32_t)offsetof(sib1_v1700_ies_s, sdt_cfg_common_r17_present)},
    {3, (int32_t)offsetof(sib1_v1700_ies_s, red_cap_cfg_common_r17_present)},
    {4, (int32_t)offsetof(sib1_v1700_ies_s, feature_priorities_r17_present)},
    {5, (int32_t)offsetof(sib1_v1700_ies_s, si_sched_info_v1700_present)},
    {6, (int32_t)offsetof(sib1_v1700_ies_s, hyper_sfn_r17_present)},
    {7, (int32_t)offsetof(sib1_v1700_ies_s, edrx_allowed_idle_r17_present)},
    {8, (int32_t)offsetof(sib1_v1700_ies_s, edrx_allowed_inactive_r17_present)},
    {9, (int32_t)offsetof(sib1_v1700_ies_s, intra_freq_resel_red_cap_r17_present)},
    {10, (int32_t)offsetof(sib1_v1700_ies_s, cell_barred_ntn_r17_present)},
    {11, (int32_t)offsetof(sib1_v1700_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc sib1_v1700_ies_s::desc = {
    {seq_table_sib1_v1700_ies_s_fields, 12, UINT64_C(0x0), seq_table_sib1_v1700_ies_s_opts, 12},
    false,
    false,
    nullptr,
    0,
    seq_table_sib1_v1700_ies_s_names};

static const char* enum_names_sib1_v1700_ies_s_intra_freq_resel_red_cap_r17_e__values[] = {"allowed", "notAllowed"};
const asn1::table::enum_desc sib1_v1700_ies_s::intra_freq_resel_red_cap_r17_opts::desc  = {
    enum_names_sib1_v1700_ies_s_intra_freq_resel_red_cap_r17_e__values,
    2,
    "sib1_v1700_ies_s::intra_freq_resel_red_cap_r17_e_"};

static const char*           enum_names_sib1_v1700_ies_s_cell_barred_ntn_r17_e__values[] = {"barred", "notBarred"};
const asn1::table::enum_desc sib1_v1700_ies_s::cell_barred_ntn_r17_opts::desc            = {
    enum_names_sib1_v1700_ies_s_cell_barred_ntn_r17_e__values,
    2,
    "sib1_v1700_ies_s::cell_barred_ntn_r17_e_"};

// SIB1-v1630-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib1_v1630_ies_s_uac_barr_info_v1630_s__fields[] = {
    {offsetof(sib1_v1630_ies_s::uac_barr_info_v1630_s_, uac_ac1_select_assist_info_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     2,
     12,
     &asn1::table::list_codec<decltype(sib1_v1630_ies_s::uac_barr_info_v1630_s_::uac_ac1_select_assist_info_r16),
                              asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib1_v1630_ies_s_uac_barr_info_v1630_s__names[] = {
    {"uac-AC1-SelectAssistInfo-r16", nullptr}};
static const asn1::table::seq_desc seq_table_sib1_v1630_ies_s_uac_barr_info_v1630_s_ = {
    {seq_table_sib1_v1630_ies_s_uac_barr_info_v1630_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sib1_v1630_ies_s_uac_barr_info_v1630_s__names};
static const asn1::table::seq_field seq_table_sib1_v1630_ies_s_fields[] = {
    {offsetof(sib1_v1630_ies_s, uac_barr_info_v1630),
     (int32_t)offsetof(sib1_v1630_ies_s, uac_barr_info_v1630_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib1_v1630_ies_s_uac_barr_info_v1630_s_},
    {offsetof(sib1_v1630_ies_s, non_crit_ext),
     (int32_t)offsetof(sib1_v1630_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib1_v1700_ies_s::desc}};
static const asn1::table::field_name seq_table_sib1_v1630_ies_s_names[] = {{"uac-BarringInfo-v1630", nullptr},
                                                                           {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib1_v1630_ies_s_opts[]  = {
    {0, (int32_t)offsetof(sib1_v1630_ies_s, uac_barr_info_v1630_present)},
    {1, (int32_t)offsetof(sib1_v1630_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc sib1_v1630_ies_s::desc = {
    {seq_table_sib1_v1630_ies_s_fields, 2, UINT64_C(0x0), seq_table_sib1_v1630_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_sib1_v1630_ies_s_names};

// SIB1-v1610-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib1_v1610_ies_s_fields[] = {
    {0,
     (int32_t)offsetof(sib1_v1610_ies_s, idle_mode_meass_eutra_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sib1_v1610_ies_s, idle_mode_meass_nr_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib1_v1610_ies_s, pos_si_sched_info_r16),
     (int32_t)offsetof(sib1_v1610_ies_s, pos_si_sched_info_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pos_si_sched_info_r16_s::desc},
    {offsetof(sib1_v1610_ies_s, non_crit_ext),
     (int32_t)offsetof(sib1_v1610_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib1_v1630_ies_s::desc}};
static const asn1::table::field_name seq_table_sib1_v1610_ies_s_names[] = {{"idleModeMeasurementsEUTRA-r16", "true"},
                                                                           {"idleModeMeasurementsNR-r16", "true"},
                                                                           {"posSI-SchedulingInfo-r16", nullptr},
                                                                           {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib1_v1610_ies_s_opts[]  = {
    {0, (int32_t)offsetof(sib1_v1610_ies_s, idle_mode_meass_eutra_r16_present)},
    {1, (int32_t)offsetof(sib1_v1610_ies_s, idle_mode_meass_nr_r16_present)},
    {2, (int32_t)offsetof(sib1_v1610_ies_s, pos_si_sched_info_r16_present)},
    {3, (int32_t)offsetof(sib1_v1610_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc sib1_v1610_ies_s::desc = {
    {seq_table_sib1_v1610_ies_s_fields, 4, UINT64_C(0x0), seq_table_sib1_v1610_ies_s_opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_sib1_v1610_ies_s_names};

// SIB1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib1_s_cell_sel_info_s__fields[] = {
    {offsetof(sib1_s::cell_sel_info_s_, q_rx_lev_min),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-70),
     UINT64_C(-22),
     nullptr},
    {offsetof(sib1_s::cell_sel_info_s_, q_rx_lev_min_offset),
     (int32_t)offsetof(sib1_s::cell_sel_info_s_, q_rx_lev_min_offset_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(sib1_s::cell_sel_info_s_, q_rx_lev_min_sul),
     (int32_t)offsetof(sib1_s::cell_sel_info_s_, q_rx_lev_min_sul_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-70),
     UINT64_C(-22),
     nullptr},
    {offsetof(sib1_s::cell_sel_info_s_, q_qual_min),
     (int32_t)offsetof(sib1_s::cell_sel_info_s_, q_qual_min_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-43),
     UINT64_C(-12),
     nullptr},
    {offsetof(sib1_s::cell_sel_info_s_, q_qual_min_offset),
     (int32_t)offsetof(sib1_s::cell_sel_info_s_, q_qual_min_offset_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr}};
static const asn1::table::field_name seq_table_sib1_s_cell_sel_info_s__names[] = {{"q-RxLevMin", nullptr},
                                                                                  {"q-RxLevMinOffset", nullptr},
                                                                                  {"q-RxLevMinSUL", nullptr},
                                                                                  {"q-QualMin", nullptr},
                                                                                  {"q-QualMinOffset", nullptr}};
static const asn1::table::seq_opt    seq_table_sib1_s_cell_sel_info_s__opts[]  = {
    {1, (int32_t)offsetof(sib1_s::cell_sel_info_s_, q_rx_lev_min_offset_present)},
    {2, (int32_t)offsetof(sib1_s::cell_sel_info_s_, q_rx_lev_min_sul_present)},
    {3, (int32_t)offsetof(sib1_s::cell_sel_info_s_, q_qual_min_present)},
    {4, (int32_t)offsetof(sib1_s::cell_sel_info_s_, q_qual_min_offset_present)}};
static const asn1::table::seq_desc seq_table_sib1_s_cell_sel_info_s_ = {
    {seq_table_sib1_s_cell_sel_info_s__fields, 5, UINT64_C(0x1), seq_table_sib1_s_cell_sel_info_s__opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_sib1_s_cell_sel_info_s__names};
static const asn1::table::seq_field seq_table_sib1_s_uac_barr_info_s__fields[] = {
    {offsetof(sib1_s::uac_barr_info_s_, uac_barr_for_common),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     63,
     &asn1::table::item_list_codec<decltype(sib1_s::uac_barr_info_s_::uac_barr_for_common),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &uac_barr_per_cat_s::desc},
    {offsetof(sib1_s::uac_barr_info_s_, uac_barr_per_plmn_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::item_list_codec<decltype(sib1_s::uac_barr_info_s_::uac_barr_per_plmn_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &uac_barr_per_plmn_s::desc},
    {offsetof(sib1_s::uac_barr_info_s_, uac_barr_info_set_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sib1_s::uac_barr_info_s_::uac_barr_info_set_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &uac_barr_info_set_s::desc},
    {offsetof(sib1_s::uac_barr_info_s_, uac_access_category1_sel_assist_info),
     (int32_t)offsetof(sib1_s::uac_barr_info_s_, uac_access_category1_sel_assist_info_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::desc}};
static const asn1::table::field_name seq_table_sib1_s_uac_barr_info_s__names[] = {
    {"uac-BarringForCommon", nullptr},
    {"uac-BarringPerPLMN-List", nullptr},
    {"uac-BarringInfoSetList", nullptr},
    {"uac-AccessCategory1-SelectionAssistanceInfo", nullptr}};
static const asn1::table::seq_opt seq_table_sib1_s_uac_barr_info_s__opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(sib1_s::uac_barr_info_s_, uac_access_category1_sel_assist_info_present)}};
static const asn1::table::seq_desc seq_table_sib1_s_uac_barr_info_s_ = {
    {seq_table_sib1_s_uac_barr_info_s__fields, 4, UINT64_C(0x4), seq_table_sib1_s_uac_barr_info_s__opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_sib1_s_uac_barr_info_s__names};
static const asn1::table::seq_field seq_table_sib1_s_fields[] = {
    {offsetof(sib1_s, cell_sel_info),
     (int32_t)offsetof(sib1_s, cell_sel_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib1_s_cell_sel_info_s_},
    {offsetof(sib1_s, cell_access_related_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_access_related_info_s::desc},
    {offsetof(sib1_s, conn_est_fail_ctrl),
     (int32_t)offsetof(sib1_s, conn_est_fail_ctrl_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &conn_est_fail_ctrl_s::desc},
    {offsetof(sib1_s, si_sched_info),
     (int32_t)offsetof(sib1_s, si_sched_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &si_sched_info_s::desc},
    {offsetof(sib1_s, serving_cell_cfg_common),
     (int32_t)offsetof(sib1_s, serving_cell_cfg_common_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &serving_cell_cfg_common_sib_s::desc},
    {0,
     (int32_t)offsetof(sib1_s, ims_emergency_support_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sib1_s, ecall_over_ims_support_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib1_s, ue_timers_and_consts),
     (int32_t)offsetof(sib1_s, ue_timers_and_consts_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_timers_and_consts_s::desc},
    {offsetof(sib1_s, uac_barr_info),
     (int32_t)offsetof(sib1_s, uac_barr_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sib1_s_uac_barr_info_s_},
    {0,
     (int32_t)offsetof(sib1_s, use_full_resume_id_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib1_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib1_s::late_non_crit_ext), asn1::table::string_json>::value},
    {offsetof(sib1_s, non_crit_ext),
     (int32_t)offsetof(sib1_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sib1_v1610_ies_s::desc}};
static const asn1::table::field_name seq_table_sib1_s_names[] = {{"cellSelectionInfo", nullptr},
                                                                 {"cellAccessRelatedInfo", nullptr},
                                                                 {"connEstFailureControl", nullptr},
                                                                 {"si-SchedulingInfo", nullptr},
                                                                 {"servingCellConfigCommon", nullptr},
                                                                 {"ims-EmergencySupport", "true"},
                                                                 {"eCallOverIMS-Support", "true"},
                                                                 {"ue-TimersAndConstants", nullptr},
                                                                 {"uac-BarringInfo", nullptr},
                                                                 {"useFullResumeID", "true"},
                                                                 {"lateNonCriticalExtension", nullptr},
                                                                 {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib1_s_opts[]  = {
    {0, (int32_t)offsetof(sib1_s, cell_sel_info_present)},
    {2, (int32_t)offsetof(sib1_s, conn_est_fail_ctrl_present)},
    {3, (int32_t)offsetof(sib1_s, si_sched_info_present)},
    {4, (int32_t)offsetof(sib1_s, serving_cell_cfg_common_present)},
    {5, (int32_t)offsetof(sib1_s, ims_emergency_support_present)},
    {6, (int32_t)offsetof(sib1_s, ecall_over_ims_support_present)},
    {7, (int32_t)offsetof(sib1_s, ue_timers_and_consts_present)},
    {8, (int32_t)offsetof(sib1_s, uac_barr_info_present)},
    {9, (int32_t)offsetof(sib1_s, use_full_resume_id_present)},
    {10, asn1::table::seq_field::list_presence},
    {11, (int32_t)offsetof(sib1_s, non_crit_ext_present)}};
const asn1::table::seq_desc sib1_s::desc = {{seq_table_sib1_s_fields, 12, UINT64_C(0x2), seq_table_sib1_s_opts, 11},
                                            false,
                                            false,
                                            nullptr,
                                            0,
                                            seq_table_sib1_s_names};

static const asn1::table::storage_ops* const
    choice_storage_sib1_s_uac_barr_info_s__uac_access_category1_sel_assist_info_c__alternatives[] = {
        &asn1::table::alternative_ops<uac_access_category1_sel_assist_info_e, true>::value,
        &asn1::table::alternative_ops<
            sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::individual_plmn_list_l_,
            false>::value};
const asn1::table::choice_storage_desc sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::storage_desc =
    {choice_storage_sib1_s_uac_barr_info_s__uac_access_category1_sel_assist_info_c__alternatives,
     2,
     "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_"};

uac_access_category1_sel_assist_info_e&
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::set_plmn_common()
{
  set(types::plmn_common);
  return c.get<uac_access_category1_sel_assist_info_e>();
}
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::individual_plmn_list_l_&
sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::set_individual_plmn_list()
{
  set(types::individual_plmn_list);
  return c.get<individual_plmn_list_l_>();
}
static const asn1::table::seq_field
    choice_table_sib1_s_uac_barr_info_s__uac_access_category1_sel_assist_info_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<uac_access_category1_sel_assist_info_e>(),
         uac_access_category1_sel_assist_info_e::has_ext,
         false,
         uac_access_category1_sel_assist_info_e::nof_types,
         uac_access_category1_sel_assist_info_e::nof_exts,
         nullptr,
         &uac_access_category1_sel_assist_info_opts::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         2,
         12,
         &asn1::table::list_codec<
             sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::individual_plmn_list_l_,
             asn1::table::string_json>::value}};
static const asn1::table::field_name
    choice_table_sib1_s_uac_barr_info_s__uac_access_category1_sel_assist_info_c__names[] = {
        {"plmnCommon", nullptr},
        {"individualPLMNList", nullptr}};
const asn1::table::choice_desc sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::desc = {
    asn1::table::enum_int_type<sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::types>(),
    sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::types::nof_types,
    sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::types::nof_exts,
    sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::types::has_ext,
    false,
    choice_table_sib1_s_uac_barr_info_s__uac_access_category1_sel_assist_info_c__alternatives,
    nullptr,
    choice_table_sib1_s_uac_barr_info_s__uac_access_category1_sel_assist_info_c__names,
    "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_",
    offsetof(sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_, c.type),
    offsetof(sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_, c.value),
    false,
    &sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::storage_desc};

static const char* enum_names_sib1_s_uac_barr_info_s__uac_access_category1_sel_assist_info_c__types_values[] = {
    "plmnCommon",
    "individualPLMNList"};
const asn1::table::enum_desc sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::types_opts::desc = {
    enum_names_sib1_s_uac_barr_info_s__uac_access_category1_sel_assist_info_c__types_values,
    2,
    "sib1_s::uac_barr_info_s_::uac_access_category1_sel_assist_info_c_::types"};
