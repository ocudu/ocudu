// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/pcch_msg_ies.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// GroupPaging-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_group_paging_r18_s_fields[] = {
    {0,
     (int32_t)offsetof(group_paging_r18_s, inactive_reception_allowed_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_group_paging_r18_s_names[] = {{"inactiveReceptionAllowed-r18", "true"}};
static const asn1::table::seq_opt    seq_table_group_paging_r18_s_opts[]  = {
    {0, (int32_t)offsetof(group_paging_r18_s, inactive_reception_allowed_r18_present)}};
const asn1::table::seq_desc group_paging_r18_s::desc = {
    {seq_table_group_paging_r18_s_fields, 1, UINT64_C(0x0), seq_table_group_paging_r18_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_group_paging_r18_s_names};

// PagingUE-Identity ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_paging_ue_id_c_alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<48>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<40>, false>::value};
const asn1::table::choice_storage_desc paging_ue_id_c::storage_desc = {choice_storage_paging_ue_id_c_alternatives,
                                                                       2,
                                                                       "paging_ue_id_c"};

fixed_bitstring<48>& paging_ue_id_c::set_ng_5_g_s_tmsi()
{
  set(types::ng_5_g_s_tmsi);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<40>& paging_ue_id_c::set_full_i_rnti()
{
  set(types::full_i_rnti);
  return c.get<fixed_bitstring<40>>();
}
static const asn1::table::seq_field choice_table_paging_ue_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<48>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<40>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_paging_ue_id_c_names[] = {{"ng-5G-S-TMSI", nullptr},
                                                                            {"fullI-RNTI", nullptr}};
const asn1::table::choice_desc       paging_ue_id_c::desc = {asn1::table::enum_int_type<paging_ue_id_c::types>(),
                                                             paging_ue_id_c::types::nof_types,
                                                             paging_ue_id_c::types::nof_exts,
                                                             paging_ue_id_c::types::has_ext,
                                                             false,
                                                             choice_table_paging_ue_id_c_alternatives,
                                                             nullptr,
                                                             choice_table_paging_ue_id_c_names,
                                                             "paging_ue_id_c",
                                                             offsetof(paging_ue_id_c, c.type),
                                                             offsetof(paging_ue_id_c, c.value),
                                                             false,
                                                             &paging_ue_id_c::storage_desc};

static const char*           enum_names_paging_ue_id_c_types_values[] = {"ng-5G-S-TMSI", "fullI-RNTI"};
const asn1::table::enum_desc paging_ue_id_c::types_opts::desc         = {enum_names_paging_ue_id_c_types_values,
                                                                         2,
                                                                         "paging_ue_id_c::types"};
int8_t                       paging_ue_id_c::types_opts::to_number() const
{
  static const int8_t numbers[] = {-5};
  return map_enum_number(numbers, 1, value, "paging_ue_id_c::types");
}

// PagingRecord ::= SEQUENCE
static const asn1::table::seq_field seq_table_paging_record_s_fields[] = {
    {offsetof(paging_record_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &paging_ue_id_c::desc},
    {0,
     (int32_t)offsetof(paging_record_s, access_type_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_paging_record_s_names[] = {{"ue-Identity", nullptr},
                                                                          {"accessType", "non3GPP"}};
static const asn1::table::seq_opt    seq_table_paging_record_s_opts[]  = {
    {1, (int32_t)offsetof(paging_record_s, access_type_present)}};
const asn1::table::seq_desc paging_record_s::desc = {
    {seq_table_paging_record_s_fields, 2, UINT64_C(0x1), seq_table_paging_record_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_paging_record_s_names};

// PagingRecord-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_paging_record_v1700_s_fields[] = {
    {0,
     (int32_t)offsetof(paging_record_v1700_s, paging_cause_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_paging_record_v1700_s_names[] = {{"pagingCause-r17", "voice"}};
static const asn1::table::seq_opt    seq_table_paging_record_v1700_s_opts[]  = {
    {0, (int32_t)offsetof(paging_record_v1700_s, paging_cause_r17_present)}};
const asn1::table::seq_desc paging_record_v1700_s::desc = {
    {seq_table_paging_record_v1700_s_fields, 1, UINT64_C(0x0), seq_table_paging_record_v1700_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_paging_record_v1700_s_names};

// PagingRecord-v1800 ::= SEQUENCE
static const asn1::table::seq_field seq_table_paging_record_v1800_s_fields[] = {
    {0,
     (int32_t)offsetof(paging_record_v1800_s, mt_sdt_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_paging_record_v1800_s_names[] = {{"mt-SDT", "true"}};
static const asn1::table::seq_opt    seq_table_paging_record_v1800_s_opts[]  = {
    {0, (int32_t)offsetof(paging_record_v1800_s, mt_sdt_present)}};
const asn1::table::seq_desc paging_record_v1800_s::desc = {
    {seq_table_paging_record_v1800_s_fields, 1, UINT64_C(0x0), seq_table_paging_record_v1800_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_paging_record_v1800_s_names};

// Paging-v1800-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_paging_v1800_ies_s_fields[] = {
    {offsetof(paging_v1800_ies_s, paging_record_list_v1800),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(paging_v1800_ies_s::paging_record_list_v1800),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &paging_record_v1800_s::desc},
    {offsetof(paging_v1800_ies_s, paging_group_list_v1800),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(paging_v1800_ies_s::paging_group_list_v1800),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &group_paging_r18_s::desc},
    {0,
     (int32_t)offsetof(paging_v1800_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_paging_v1800_ies_s_names[] = {{"pagingRecordList-v1800", nullptr},
                                                                             {"pagingGroupList-v1800", nullptr},
                                                                             {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_paging_v1800_ies_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(paging_v1800_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc paging_v1800_ies_s::desc = {
    {seq_table_paging_v1800_ies_s_fields, 3, UINT64_C(0x0), seq_table_paging_v1800_ies_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_paging_v1800_ies_s_names};

// Paging-v1700-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_paging_v1700_ies_s_fields[] = {
    {offsetof(paging_v1700_ies_s, paging_record_list_v1700),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(paging_v1700_ies_s::paging_record_list_v1700),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &paging_record_v1700_s::desc},
    {offsetof(paging_v1700_ies_s, paging_group_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(paging_v1700_ies_s::paging_group_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &tmgi_r17_s::desc},
    {offsetof(paging_v1700_ies_s, non_crit_ext),
     (int32_t)offsetof(paging_v1700_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &paging_v1800_ies_s::desc}};
static const asn1::table::field_name seq_table_paging_v1700_ies_s_names[] = {{"pagingRecordList-v1700", nullptr},
                                                                             {"pagingGroupList-r17", nullptr},
                                                                             {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_paging_v1700_ies_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(paging_v1700_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc paging_v1700_ies_s::desc = {
    {seq_table_paging_v1700_ies_s_fields, 3, UINT64_C(0x0), seq_table_paging_v1700_ies_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_paging_v1700_ies_s_names};

// Paging ::= SEQUENCE
static const asn1::table::seq_field seq_table_paging_s_fields[] = {
    {offsetof(paging_s, paging_record_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::
         item_list_codec<decltype(paging_s::paging_record_list), asn1::table::sequence_item_ops, false, true>::value,
     &paging_record_s::desc},
    {offsetof(paging_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(paging_s::late_non_crit_ext), asn1::table::string_json>::value},
    {offsetof(paging_s, non_crit_ext),
     (int32_t)offsetof(paging_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &paging_v1700_ies_s::desc}};
static const asn1::table::field_name seq_table_paging_s_names[] = {{"pagingRecordList", nullptr},
                                                                   {"lateNonCriticalExtension", nullptr},
                                                                   {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_paging_s_opts[]  = {{0, asn1::table::seq_field::list_presence},
                                                                   {1, asn1::table::seq_field::list_presence},
                                                                   {2, (int32_t)offsetof(paging_s, non_crit_ext_present)}};
const asn1::table::seq_desc paging_s::desc = {{seq_table_paging_s_fields, 3, UINT64_C(0x0), seq_table_paging_s_opts, 3},
                                              false,
                                              false,
                                              nullptr,
                                              0,
                                              seq_table_paging_s_names};
