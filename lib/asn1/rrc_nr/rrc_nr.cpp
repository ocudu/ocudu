// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/rrc_nr.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// AreaValidityTA-Config-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_area_validity_ta_cfg_r18_s_fields[] = {
    {offsetof(area_validity_ta_cfg_r18_s, inactive_pos_srs_validity_area_tat_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_tat_r18)>(),
     decltype(area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_tat_r18)::has_ext,
     false,
     decltype(area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_tat_r18)::nof_types,
     decltype(area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_tat_r18)::nof_exts,
     nullptr,
     &area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_tat_r18_opts::desc},
    {offsetof(area_validity_ta_cfg_r18_s, inactive_pos_srs_validity_area_rsrp_r18),
     (int32_t)offsetof(area_validity_ta_cfg_r18_s, inactive_pos_srs_validity_area_rsrp_r18_present),
     asn1::table::enum_op<decltype(area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_rsrp_r18)>(),
     decltype(area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_rsrp_r18)::has_ext,
     false,
     decltype(area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_rsrp_r18)::nof_types,
     decltype(area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_rsrp_r18)::nof_exts,
     nullptr,
     &rsrp_change_thres_r17_opts::desc},
    {0,
     (int32_t)offsetof(area_validity_ta_cfg_r18_s, autonomous_ta_adjustment_enabled_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_area_validity_ta_cfg_r18_s_names[] = {
    {"inactivePosSRS-ValidityAreaTAT-r18", nullptr},
    {"inactivePosSRS-ValidityAreaRSRP-r18", nullptr},
    {"autonomousTA-AdjustmentEnabled-r18", "true"}};
static const asn1::table::seq_opt seq_table_area_validity_ta_cfg_r18_s_opts[] = {
    {1, (int32_t)offsetof(area_validity_ta_cfg_r18_s, inactive_pos_srs_validity_area_rsrp_r18_present)},
    {2, (int32_t)offsetof(area_validity_ta_cfg_r18_s, autonomous_ta_adjustment_enabled_r18_present)}};
const asn1::table::seq_desc area_validity_ta_cfg_r18_s::desc = {
    {seq_table_area_validity_ta_cfg_r18_s_fields, 3, UINT64_C(0x1), seq_table_area_validity_ta_cfg_r18_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_area_validity_ta_cfg_r18_s_names};

static const char* enum_names_area_validity_ta_cfg_r18_s_inactive_pos_srs_validity_area_tat_r18_e__values[] =
    {"ms1280", "ms1920", "ms2560", "ms5120", "ms10240", "ms20480", "ms40960", "infinity"};
const asn1::table::enum_desc area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_tat_r18_opts::desc = {
    enum_names_area_validity_ta_cfg_r18_s_inactive_pos_srs_validity_area_tat_r18_e__values,
    8,
    "area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_tat_r18_e_"};
int32_t area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_tat_r18_opts::to_number() const
{
  static const int32_t numbers[] = {1280, 1920, 2560, 5120, 10240, 20480, 40960, -1};
  return map_enum_number(numbers, 8, value, "area_validity_ta_cfg_r18_s::inactive_pos_srs_validity_area_tat_r18_e_");
}

// SRS-PosConfigPerULCarrier-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_pos_cfg_per_ul_carrier_r18_s_fields[] = {
    {offsetof(srs_pos_cfg_per_ul_carrier_r18_s, freq_info_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(srs_pos_cfg_per_ul_carrier_r18_s, srs_pos_cfg_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_pos_cfg_r17_s::desc},
    {offsetof(srs_pos_cfg_per_ul_carrier_r18_s, scs_specific_carrier_r18),
     (int32_t)offsetof(srs_pos_cfg_per_ul_carrier_r18_s, scs_specific_carrier_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &scs_specific_carrier_s::desc},
    {offsetof(srs_pos_cfg_per_ul_carrier_r18_s, bwp_r18),
     (int32_t)offsetof(srs_pos_cfg_per_ul_carrier_r18_s, bwp_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_s::desc}};
static const asn1::table::field_name seq_table_srs_pos_cfg_per_ul_carrier_r18_s_names[] = {
    {"freqInfo-r18", nullptr},
    {"srs-PosConfig-r18", nullptr},
    {"scs-SpecificCarrier-r18", nullptr},
    {"bwp-r18", nullptr}};
static const asn1::table::seq_opt seq_table_srs_pos_cfg_per_ul_carrier_r18_s_opts[] = {
    {2, (int32_t)offsetof(srs_pos_cfg_per_ul_carrier_r18_s, scs_specific_carrier_r18_present)},
    {3, (int32_t)offsetof(srs_pos_cfg_per_ul_carrier_r18_s, bwp_r18_present)}};
const asn1::table::seq_desc srs_pos_cfg_per_ul_carrier_r18_s::desc = {
    {seq_table_srs_pos_cfg_per_ul_carrier_r18_s_fields,
     4,
     UINT64_C(0x3),
     seq_table_srs_pos_cfg_per_ul_carrier_r18_s_opts,
     2},
    true,
    false,
    nullptr,
    0,
    seq_table_srs_pos_cfg_per_ul_carrier_r18_s_names};

// SRS-PosRRC-InactiveAggBW-AdditionalCarriers-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s_fields[] = {
    {offsetof(srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s, aggr_pos_srs_carrier_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2,
     &asn1::table::item_list_codec<
         decltype(srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s::aggr_pos_srs_carrier_list_r18),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &srs_pos_cfg_per_ul_carrier_r18_s::desc}};
static const asn1::table::field_name seq_table_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s_names[] = {
    {"aggregatedPosSRS-CarrierList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s::desc = {
    {seq_table_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s_opts,
     1},
    true,
    false,
    nullptr,
    0,
    seq_table_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s_names};

// SRS-PosRRC-InactiveValidityAreaConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_per_va_r18_setup_release_alternatives
        [] = {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::sequence,
               false,
               false,
               0,
               0,
               nullptr,
               &srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s::desc}};
static const asn1::table::field_name
    seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_per_va_r18_setup_release_names
        [] = {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_per_va_r18_setup_release =
        {asn1::table::enum_int_type<asn1::setup_release_e>(),
         2,
         0,
         false,
         false,
         seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_per_va_r18_setup_release_alternatives,
         nullptr,
         seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_per_va_r18_setup_release_names,
         "setup_release_c",
         setup_release_c<srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s>::type_offset(),
         setup_release_c<srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_per_va_r18_setup_release_alternatives
        [] = {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               1,
               16,
               &asn1::table::list_codec<
                   setup_release_c<
                       dyn_seq_of<dyn_seq_of<srs_pos_res_set_linked_for_agg_bw_r18_s, 2, 3>, 1, 16>>::value_type,
                   asn1::table::list_json<asn1::table::struct_json>,
                   asn1::table::list_items<false, 2, 3, false, asn1::table::object_items<false>>>::value}};
static const asn1::table::field_name
    seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_per_va_r18_setup_release_names
        [] = {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_per_va_r18_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_per_va_r18_setup_release_alternatives,
        nullptr,
        seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_per_va_r18_setup_release_names,
        "setup_release_c",
        setup_release_c<dyn_seq_of<dyn_seq_of<srs_pos_res_set_linked_for_agg_bw_r18_s, 2, 3>, 1, 16>>::type_offset(),
        setup_release_c<dyn_seq_of<dyn_seq_of<srs_pos_res_set_linked_for_agg_bw_r18_s, 2, 3>, 1, 16>>::value_offset()};
static const asn1::table::seq_field seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_fields[] = {
    {offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, srs_pos_cfg_validity_area_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<decltype(srs_pos_rrc_inactive_validity_area_cfg_r18_s::srs_pos_cfg_validity_area_r18),
                              asn1::table::struct_json>::value},
    {offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, srs_pos_cfg_n_ul_r18),
     (int32_t)offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, srs_pos_cfg_n_ul_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_pos_cfg_r17_s::desc},
    {offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, srs_pos_cfg_sul_r18),
     (int32_t)offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, srs_pos_cfg_sul_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_pos_cfg_r17_s::desc},
    {offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, bwp_n_ul_r18),
     (int32_t)offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, bwp_n_ul_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_s::desc},
    {offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, bwp_sul_r18),
     (int32_t)offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, bwp_sul_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_s::desc},
    {offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, area_validity_ta_cfg_r18),
     (int32_t)offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, area_validity_ta_cfg_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &area_validity_ta_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_names[] = {
    {"srs-PosConfigValidityArea-r18", nullptr},
    {"srs-PosConfigNUL-r18", nullptr},
    {"srs-PosConfigSUL-r18", nullptr},
    {"bwp-NUL-r18", nullptr},
    {"bwp-SUL-r18", nullptr},
    {"areaValidityTA-Config-r18", nullptr}};
static const asn1::table::seq_opt seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_opts[] = {
    {1, (int32_t)offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, srs_pos_cfg_n_ul_r18_present)},
    {2, (int32_t)offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, srs_pos_cfg_sul_r18_present)},
    {3, (int32_t)offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, bwp_n_ul_r18_present)},
    {4, (int32_t)offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, bwp_sul_r18_present)},
    {5, (int32_t)offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, area_validity_ta_cfg_r18_present)}};
static const asn1::table::seq_field seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g0_fields[] = {
    {offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, srs_pos_cfg_validity_area_ext_v1830),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::ptr_list_codec<
         decltype(srs_pos_rrc_inactive_validity_area_cfg_r18_s::srs_pos_cfg_validity_area_ext_v1830),
         asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g0_names[] = {
    {"srs-PosConfigValidityAreaExt-v1830", nullptr}};
static const asn1::table::seq_opt seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g1_fields[] = {
    {offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, srs_pos_rrc_inactive_agg_bw_add_carriers_per_va_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(srs_pos_rrc_inactive_validity_area_cfg_r18_s::
                                                 srs_pos_rrc_inactive_agg_bw_add_carriers_per_va_r18)>::value,
     &seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_per_va_r18_setup_release},
    {offsetof(srs_pos_rrc_inactive_validity_area_cfg_r18_s, srs_pos_rrc_inactive_agg_bw_cfg_list_per_va_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(srs_pos_rrc_inactive_validity_area_cfg_r18_s::
                                                 srs_pos_rrc_inactive_agg_bw_cfg_list_per_va_r18)>::value,
     &seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_per_va_r18_setup_release}};
static const asn1::table::field_name seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g1_names[] = {
    {"srs-PosRRC-InactiveAggBW-AdditionalCarriersPerVA-r18", nullptr},
    {"srs-PosRRC-InactiveAggBW-ConfigListPerVA-r18", nullptr}};
static const asn1::table::seq_opt seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_groups[] = {
    {{seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g0_fields,
      1,
      UINT64_C(0x0),
      seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g0_opts,
      1},
     false,
     seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g0_names},
    {{seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g1_fields,
      2,
      UINT64_C(0x0),
      seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g1_opts,
      2},
     false,
     seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_g1_names}};
const asn1::table::seq_desc srs_pos_rrc_inactive_validity_area_cfg_r18_s::desc = {
    {seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_fields,
     6,
     UINT64_C(0x1),
     seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_opts,
     5},
    true,
    false,
    seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_groups,
    2,
    seq_table_srs_pos_rrc_inactive_validity_area_cfg_r18_s_names};

// AS-Config ::= SEQUENCE
static const asn1::table::seq_field seq_table_as_cfg_s_fields[] = {
    {offsetof(as_cfg_s, rrc_recfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(as_cfg_s::rrc_recfg), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_as_cfg_s_names[]     = {{"rrcReconfiguration", nullptr}};
static const asn1::table::seq_field  seq_table_as_cfg_s_g0_fields[] = {
    {offsetof(as_cfg_s, source_rb_sn_cfg),
      asn1::table::seq_field::list_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::sized_object_codec<decltype(as_cfg_s::source_rb_sn_cfg), asn1::table::string_json>::value},
    {offsetof(as_cfg_s, source_scg_nr_cfg),
      asn1::table::seq_field::list_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::sized_object_codec<decltype(as_cfg_s::source_scg_nr_cfg), asn1::table::string_json>::value},
    {offsetof(as_cfg_s, source_scg_eutra_cfg),
      asn1::table::seq_field::list_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::sized_object_codec<decltype(as_cfg_s::source_scg_eutra_cfg), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_as_cfg_s_g0_names[]  = {{"sourceRB-SN-Config", nullptr},
                                                                       {"sourceSCG-NR-Config", nullptr},
                                                                       {"sourceSCG-EUTRA-Config", nullptr}};
static const asn1::table::seq_opt    seq_table_as_cfg_s_g0_opts[]   = {{0, asn1::table::seq_field::list_presence},
                                                                       {1, asn1::table::seq_field::list_presence},
                                                                       {2, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field  seq_table_as_cfg_s_g1_fields[] = {
    {0, (int32_t)offsetof(as_cfg_s, source_scg_cfg_present), asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name seq_table_as_cfg_s_g1_names[] = {{"sourceSCG-Configured", "true"}};
static const asn1::table::seq_opt    seq_table_as_cfg_s_g1_opts[]  = {
    {0, (int32_t)offsetof(as_cfg_s, source_scg_cfg_present)}};
static const asn1::table::seq_field seq_table_as_cfg_s_g2_fields[] = {
    {offsetof(as_cfg_s, sdt_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(as_cfg_s::sdt_cfg_r17)>::value,
     &sdt_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_as_cfg_s_g2_names[]  = {{"sdt-Config-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_as_cfg_s_g2_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_as_cfg_s_g3_fields[] = {
    {offsetof(as_cfg_s, srs_pos_rrc_inactive_validity_area_pre_cfg_list_r18),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      16,
      &asn1::table::ptr_item_list_codec<decltype(as_cfg_s::srs_pos_rrc_inactive_validity_area_pre_cfg_list_r18),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &srs_pos_rrc_inactive_validity_area_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_as_cfg_s_g3_names[] = {
    {"srs-PosRRC-InactiveValidityAreaPreConfigList-r18", nullptr}};
static const asn1::table::seq_opt   seq_table_as_cfg_s_g3_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_as_cfg_s_groups[]  = {
    {{seq_table_as_cfg_s_g0_fields, 3, UINT64_C(0x0), seq_table_as_cfg_s_g0_opts, 3},
      false,
      seq_table_as_cfg_s_g0_names},
    {{seq_table_as_cfg_s_g1_fields, 1, UINT64_C(0x0), seq_table_as_cfg_s_g1_opts, 1},
      false,
      seq_table_as_cfg_s_g1_names},
    {{seq_table_as_cfg_s_g2_fields, 1, UINT64_C(0x0), seq_table_as_cfg_s_g2_opts, 1},
      false,
      seq_table_as_cfg_s_g2_names},
    {{seq_table_as_cfg_s_g3_fields, 1, UINT64_C(0x0), seq_table_as_cfg_s_g3_opts, 1},
      false,
      seq_table_as_cfg_s_g3_names}};
const asn1::table::seq_desc as_cfg_s::desc = {{seq_table_as_cfg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                              true,
                                              false,
                                              seq_table_as_cfg_s_groups,
                                              4,
                                              seq_table_as_cfg_s_names};

// ReestabNCellInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_reestab_ncell_info_s_fields[] = {
    {offsetof(reestab_ncell_info_s, cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(reestab_ncell_info_s::cell_id), asn1::table::struct_json>::value},
    {offsetof(reestab_ncell_info_s, key_g_node_b_star),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(reestab_ncell_info_s::key_g_node_b_star), asn1::table::struct_json>::value},
    {offsetof(reestab_ncell_info_s, short_mac_i),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(reestab_ncell_info_s::short_mac_i), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_reestab_ncell_info_s_names[] = {{"cellIdentity", nullptr},
                                                                               {"key-gNodeB-Star", nullptr},
                                                                               {"shortMAC-I", nullptr}};
const asn1::table::seq_desc          reestab_ncell_info_s::desc             = {
    {seq_table_reestab_ncell_info_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_reestab_ncell_info_s_names};

// ReestablishmentInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_reest_info_s_fields[] = {
    {offsetof(reest_info_s, source_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(reest_info_s, target_cell_short_mac_i),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(reest_info_s::target_cell_short_mac_i), asn1::table::struct_json>::value},
    {offsetof(reest_info_s, add_reestab_info_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(reest_info_s::add_reestab_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &reestab_ncell_info_s::desc}};
static const asn1::table::field_name seq_table_reest_info_s_names[] = {{"sourcePhysCellId", nullptr},
                                                                       {"targetCellShortMAC-I", nullptr},
                                                                       {"additionalReestabInfoList", nullptr}};
static const asn1::table::seq_opt    seq_table_reest_info_s_opts[]  = {{2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          reest_info_s::desc             = {
    {seq_table_reest_info_s_fields, 3, UINT64_C(0x3), seq_table_reest_info_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_reest_info_s_names};

// BandCombinationInfoSN ::= SEQUENCE
static const asn1::table::seq_field seq_table_band_combination_info_sn_s_fields[] = {
    {offsetof(band_combination_info_sn_s, band_combination_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(1),
     UINT64_C(65536),
     nullptr},
    {offsetof(band_combination_info_sn_s, requested_feature_sets),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(128),
     nullptr}};
static const asn1::table::field_name seq_table_band_combination_info_sn_s_names[] = {{"bandCombinationIndex", nullptr},
                                                                                     {"requestedFeatureSets", nullptr}};
const asn1::table::seq_desc          band_combination_info_sn_s::desc             = {
    {seq_table_band_combination_info_sn_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_band_combination_info_sn_s_names};

// ConfigRestrictInfoDAPS-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfg_restrict_info_daps_r16_s_pwr_coordination_r16_s__fields[] = {
    {offsetof(cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_, p_daps_source_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr},
    {offsetof(cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_, p_daps_target_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr},
    {offsetof(cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_, ul_pwr_sharing_daps_mode_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<
         decltype(cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16)>(),
     decltype(cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16)::has_ext,
     false,
     decltype(cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16)::nof_types,
     decltype(cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16)::nof_exts,
     nullptr,
     &cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_opts::desc}};
static const asn1::table::field_name seq_table_cfg_restrict_info_daps_r16_s_pwr_coordination_r16_s__names[] = {
    {"p-DAPS-Source-r16", nullptr},
    {"p-DAPS-Target-r16", nullptr},
    {"uplinkPowerSharingDAPS-Mode-r16", nullptr}};
static const asn1::table::seq_desc seq_table_cfg_restrict_info_daps_r16_s_pwr_coordination_r16_s_ = {
    {seq_table_cfg_restrict_info_daps_r16_s_pwr_coordination_r16_s__fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_cfg_restrict_info_daps_r16_s_pwr_coordination_r16_s__names};
static const asn1::table::seq_field seq_table_cfg_restrict_info_daps_r16_s_fields[] = {
    {offsetof(cfg_restrict_info_daps_r16_s, pwr_coordination_r16),
     (int32_t)offsetof(cfg_restrict_info_daps_r16_s, pwr_coordination_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_cfg_restrict_info_daps_r16_s_pwr_coordination_r16_s_}};
static const asn1::table::field_name seq_table_cfg_restrict_info_daps_r16_s_names[] = {
    {"powerCoordination-r16", nullptr}};
static const asn1::table::seq_opt seq_table_cfg_restrict_info_daps_r16_s_opts[] = {
    {0, (int32_t)offsetof(cfg_restrict_info_daps_r16_s, pwr_coordination_r16_present)}};
const asn1::table::seq_desc cfg_restrict_info_daps_r16_s::desc = {
    {seq_table_cfg_restrict_info_daps_r16_s_fields, 1, UINT64_C(0x0), seq_table_cfg_restrict_info_daps_r16_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_cfg_restrict_info_daps_r16_s_names};

static const char*
    enum_names_cfg_restrict_info_daps_r16_s_pwr_coordination_r16_s__ul_pwr_sharing_daps_mode_r16_e__values[] = {
        "semi-static-mode1",
        "semi-static-mode2",
        "dynamic"};
const asn1::table::enum_desc
    cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_opts::desc = {
        enum_names_cfg_restrict_info_daps_r16_s_pwr_coordination_r16_s__ul_pwr_sharing_daps_mode_r16_e__values,
        3,
        "cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_e_"};
uint8_t cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(
      numbers, 2, value, "cfg_restrict_info_daps_r16_s::pwr_coordination_r16_s_::ul_pwr_sharing_daps_mode_r16_e_");
}

// ConfigRestrictInfoDAPS-v1640 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfg_restrict_info_daps_v1640_s_fields[] = {
    {offsetof(cfg_restrict_info_daps_v1640_s, source_feature_set_per_dl_cc_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(1024),
     nullptr},
    {offsetof(cfg_restrict_info_daps_v1640_s, source_feature_set_per_ul_cc_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(1024),
     nullptr}};
static const asn1::table::field_name seq_table_cfg_restrict_info_daps_v1640_s_names[] = {
    {"sourceFeatureSetPerDownlinkCC-r16", nullptr},
    {"sourceFeatureSetPerUplinkCC-r16", nullptr}};
const asn1::table::seq_desc cfg_restrict_info_daps_v1640_s::desc = {
    {seq_table_cfg_restrict_info_daps_v1640_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_cfg_restrict_info_daps_v1640_s_names};

// AS-Context ::= SEQUENCE
static const asn1::table::seq_field seq_table_as_context_s_fields[] = {
    {offsetof(as_context_s, reest_info),
     (int32_t)offsetof(as_context_s, reest_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &reest_info_s::desc},
    {offsetof(as_context_s, cfg_restrict_info),
     (int32_t)offsetof(as_context_s, cfg_restrict_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cfg_restrict_info_scg_s::desc}};
static const asn1::table::field_name seq_table_as_context_s_names[] = {{"reestablishmentInfo", nullptr},
                                                                       {"configRestrictInfo", nullptr}};
static const asn1::table::seq_opt    seq_table_as_context_s_opts[]  = {
    {0, (int32_t)offsetof(as_context_s, reest_info_present)},
    {1, (int32_t)offsetof(as_context_s, cfg_restrict_info_present)}};
static const asn1::table::seq_field seq_table_as_context_s_g0_fields[] = {
    {offsetof(as_context_s, ran_notif_area_info),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(as_context_s::ran_notif_area_info)>::value,
     &ran_notif_area_info_c::desc}};
static const asn1::table::field_name seq_table_as_context_s_g0_names[]  = {{"ran-NotificationAreaInfo", nullptr}};
static const asn1::table::seq_opt    seq_table_as_context_s_g0_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_as_context_s_g1_fields[] = {
    {offsetof(as_context_s, ue_assist_info),
      asn1::table::seq_field::list_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::sized_object_codec<decltype(as_context_s::ue_assist_info), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_as_context_s_g1_names[]  = {{"ueAssistanceInformation", nullptr}};
static const asn1::table::seq_opt    seq_table_as_context_s_g1_opts[]   = {{0, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field  seq_table_as_context_s_g2_fields[] = {
    {offsetof(as_context_s, sel_band_combination_sn),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(as_context_s::sel_band_combination_sn)>::value,
      &band_combination_info_sn_s::desc}};
static const asn1::table::field_name seq_table_as_context_s_g2_names[]  = {{"selectedBandCombinationSN", nullptr}};
static const asn1::table::seq_opt    seq_table_as_context_s_g2_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_as_context_s_g3_fields[] = {
    {offsetof(as_context_s, cfg_restrict_info_daps_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(as_context_s::cfg_restrict_info_daps_r16)>::value,
      &cfg_restrict_info_daps_r16_s::desc},
    {offsetof(as_context_s, sidelink_ue_info_nr_r16),
      asn1::table::seq_field::list_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::sized_object_codec<decltype(as_context_s::sidelink_ue_info_nr_r16),
                                       asn1::table::string_json>::value},
    {offsetof(as_context_s, sidelink_ue_info_eutra_r16),
      asn1::table::seq_field::list_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::sized_object_codec<decltype(as_context_s::sidelink_ue_info_eutra_r16),
                                       asn1::table::string_json>::value},
    {offsetof(as_context_s, ue_assist_info_eutra_r16),
      asn1::table::seq_field::list_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::sized_object_codec<decltype(as_context_s::ue_assist_info_eutra_r16),
                                       asn1::table::string_json>::value},
    {offsetof(as_context_s, ue_assist_info_scg_r16),
      asn1::table::seq_field::list_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::sized_object_codec<decltype(as_context_s::ue_assist_info_scg_r16), asn1::table::string_json>::value},
    {offsetof(as_context_s, need_for_gaps_info_nr_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(as_context_s::need_for_gaps_info_nr_r16)>::value,
      &need_for_gaps_info_nr_r16_s::desc}};
static const asn1::table::field_name seq_table_as_context_s_g3_names[]  = {{"configRestrictInfoDAPS-r16", nullptr},
                                                                           {"sidelinkUEInformationNR-r16", nullptr},
                                                                           {"sidelinkUEInformationEUTRA-r16", nullptr},
                                                                           {"ueAssistanceInformationEUTRA-r16", nullptr},
                                                                           {"ueAssistanceInformationSCG-r16", nullptr},
                                                                           {"needForGapsInfoNR-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_as_context_s_g3_opts[]   = {{0, asn1::table::seq_field::ptr_presence},
                                                                           {1, asn1::table::seq_field::list_presence},
                                                                           {2, asn1::table::seq_field::list_presence},
                                                                           {3, asn1::table::seq_field::list_presence},
                                                                           {4, asn1::table::seq_field::list_presence},
                                                                           {5, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_as_context_s_g4_fields[] = {
    {offsetof(as_context_s, cfg_restrict_info_daps_v1640),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(as_context_s::cfg_restrict_info_daps_v1640)>::value,
      &cfg_restrict_info_daps_v1640_s::desc}};
static const asn1::table::field_name seq_table_as_context_s_g4_names[]  = {{"configRestrictInfoDAPS-v1640", nullptr}};
static const asn1::table::seq_opt    seq_table_as_context_s_g4_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_as_context_s_g5_fields[] = {
    {offsetof(as_context_s, need_for_gap_ncsg_info_nr_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(as_context_s::need_for_gap_ncsg_info_nr_r17)>::value,
      &need_for_gap_ncsg_info_nr_r17_s::desc},
    {offsetof(as_context_s, need_for_gap_ncsg_info_eutra_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(as_context_s::need_for_gap_ncsg_info_eutra_r17)>::value,
      &need_for_gap_ncsg_info_eutra_r17_s::desc},
    {offsetof(as_context_s, mbs_interest_ind_r17),
      asn1::table::seq_field::list_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::sized_object_codec<decltype(as_context_s::mbs_interest_ind_r17), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_as_context_s_g5_names[]  = {{"needForGapNCSG-InfoNR-r17", nullptr},
                                                                           {"needForGapNCSG-InfoEUTRA-r17", nullptr},
                                                                           {"mbsInterestIndication-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_as_context_s_g5_opts[]   = {{0, asn1::table::seq_field::ptr_presence},
                                                                           {1, asn1::table::seq_field::ptr_presence},
                                                                           {2, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field  seq_table_as_context_s_g6_fields[] = {
    {offsetof(as_context_s, need_for_interruption_info_nr_r18),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(as_context_s::need_for_interruption_info_nr_r18)>::value,
      &need_for_interruption_info_nr_r18_s::desc},
    {offsetof(as_context_s, flight_path_info_report_r18),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      20,
      &asn1::table::ptr_item_list_codec<decltype(as_context_s::flight_path_info_report_r18),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &way_point_r18_s::desc}};
static const asn1::table::field_name seq_table_as_context_s_g6_names[] = {{"needForInterruptionInfoNR-r18", nullptr},
                                                                          {"flightPathInfoReport-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_as_context_s_g6_opts[]  = {{0, asn1::table::seq_field::ptr_presence},
                                                                          {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_as_context_s_groups[]   = {
    {{seq_table_as_context_s_g0_fields, 1, UINT64_C(0x0), seq_table_as_context_s_g0_opts, 1},
        false,
        seq_table_as_context_s_g0_names},
    {{seq_table_as_context_s_g1_fields, 1, UINT64_C(0x0), seq_table_as_context_s_g1_opts, 1},
        false,
        seq_table_as_context_s_g1_names},
    {{seq_table_as_context_s_g2_fields, 1, UINT64_C(0x0), seq_table_as_context_s_g2_opts, 1},
        false,
        seq_table_as_context_s_g2_names},
    {{seq_table_as_context_s_g3_fields, 6, UINT64_C(0x0), seq_table_as_context_s_g3_opts, 6},
        false,
        seq_table_as_context_s_g3_names},
    {{seq_table_as_context_s_g4_fields, 1, UINT64_C(0x0), seq_table_as_context_s_g4_opts, 1},
        false,
        seq_table_as_context_s_g4_names},
    {{seq_table_as_context_s_g5_fields, 3, UINT64_C(0x0), seq_table_as_context_s_g5_opts, 3},
        false,
        seq_table_as_context_s_g5_names},
    {{seq_table_as_context_s_g6_fields, 2, UINT64_C(0x0), seq_table_as_context_s_g6_opts, 2},
        false,
        seq_table_as_context_s_g6_names}};
const asn1::table::seq_desc as_context_s::desc = {
    {seq_table_as_context_s_fields, 2, UINT64_C(0x0), seq_table_as_context_s_opts, 2},
    true,
    false,
    seq_table_as_context_s_groups,
    7,
    seq_table_as_context_s_names};

// AggregatedBandwidthSN-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_aggr_bw_sn_r17_s_fields[] = {
    {offsetof(aggr_bw_sn_r17_s, agg_bw_fdd_dl_r17),
     (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_fdd_dl_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &supported_agg_bw_r17_c::desc},
    {offsetof(aggr_bw_sn_r17_s, agg_bw_fdd_ul_r17),
     (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_fdd_ul_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &supported_agg_bw_r17_c::desc},
    {offsetof(aggr_bw_sn_r17_s, agg_bw_tdd_dl_r17),
     (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_tdd_dl_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &supported_agg_bw_r17_c::desc},
    {offsetof(aggr_bw_sn_r17_s, agg_bw_tdd_ul_r17),
     (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_tdd_ul_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &supported_agg_bw_r17_c::desc},
    {offsetof(aggr_bw_sn_r17_s, agg_bw_total_dl_r17),
     (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_total_dl_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &supported_agg_bw_r17_c::desc},
    {offsetof(aggr_bw_sn_r17_s, agg_bw_total_ul_r17),
     (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_total_ul_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &supported_agg_bw_r17_c::desc}};
static const asn1::table::field_name seq_table_aggr_bw_sn_r17_s_names[] = {{"aggBW-FDD-DL-r17", nullptr},
                                                                           {"aggBW-FDD-UL-r17", nullptr},
                                                                           {"aggBW-TDD-DL-r17", nullptr},
                                                                           {"aggBW-TDD-UL-r17", nullptr},
                                                                           {"aggBW-TotalDL-r17", nullptr},
                                                                           {"aggBW-TotalUL-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_aggr_bw_sn_r17_s_opts[]  = {
    {0, (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_fdd_dl_r17_present)},
    {1, (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_fdd_ul_r17_present)},
    {2, (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_tdd_dl_r17_present)},
    {3, (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_tdd_ul_r17_present)},
    {4, (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_total_dl_r17_present)},
    {5, (int32_t)offsetof(aggr_bw_sn_r17_s, agg_bw_total_ul_r17_present)}};
const asn1::table::seq_desc aggr_bw_sn_r17_s::desc = {
    {seq_table_aggr_bw_sn_r17_s_fields, 6, UINT64_C(0x0), seq_table_aggr_bw_sn_r17_s_opts, 6},
    false,
    false,
    nullptr,
    0,
    seq_table_aggr_bw_sn_r17_s_names};

// CG-CandidateInfoId-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_candidate_info_id_r17_s_fields[] = {
    {offsetof(cg_candidate_info_id_r17_s, ssb_freq_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(cg_candidate_info_id_r17_s, pci_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr}};
static const asn1::table::field_name seq_table_cg_candidate_info_id_r17_s_names[] = {{"ssbFrequency-r17", nullptr},
                                                                                     {"physCellId-r17", nullptr}};
const asn1::table::seq_desc          cg_candidate_info_id_r17_s::desc             = {
    {seq_table_cg_candidate_info_id_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_candidate_info_id_r17_s_names};

// CG-CandidateInfo-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_candidate_info_r17_s_fields[] = {
    {offsetof(cg_candidate_info_r17_s, cg_candidate_info_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_candidate_info_id_r17_s::desc},
    {offsetof(cg_candidate_info_r17_s, candidate_cg_cfg_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cg_candidate_info_r17_s::candidate_cg_cfg_r17),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_cg_candidate_info_r17_s_names[] = {{"cg-CandidateInfoId-r17", nullptr},
                                                                                  {"candidateCG-Config-r17", nullptr}};
const asn1::table::seq_desc          cg_candidate_info_r17_s::desc             = {
    {seq_table_cg_candidate_info_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_candidate_info_r17_s_names};

// CG-CandidateList-r17-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_candidate_list_r17_ies_s_fields[] = {
    {offsetof(cg_candidate_list_r17_ies_s, cg_candidate_to_add_mod_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(cg_candidate_list_r17_ies_s::cg_candidate_to_add_mod_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cg_candidate_info_r17_s::desc},
    {offsetof(cg_candidate_list_r17_ies_s, cg_candidate_to_release_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(cg_candidate_list_r17_ies_s::cg_candidate_to_release_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cg_candidate_info_id_r17_s::desc},
    {0,
     (int32_t)offsetof(cg_candidate_list_r17_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_cg_candidate_list_r17_ies_s_names[] = {
    {"cg-CandidateToAddModList-r17", nullptr},
    {"cg-CandidateToReleaseList-r17", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_cg_candidate_list_r17_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(cg_candidate_list_r17_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_candidate_list_r17_ies_s::desc = {
    {seq_table_cg_candidate_list_r17_ies_s_fields, 3, UINT64_C(0x0), seq_table_cg_candidate_list_r17_ies_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_candidate_list_r17_ies_s_names};

// CG-CandidateList ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_candidate_list_s_fields[] = {
    {offsetof(cg_candidate_list_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_candidate_list_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_cg_candidate_list_s_names[] = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          cg_candidate_list_s::desc             = {
    {seq_table_cg_candidate_list_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_candidate_list_s_names};

void cg_candidate_list_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
cg_candidate_list_s::crit_exts_c_::c1_c_& cg_candidate_list_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void cg_candidate_list_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_cg_candidate_list_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_candidate_list_s::crit_exts_c_::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_cg_candidate_list_s_crit_exts_c__names[] = {
    {"c1", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc cg_candidate_list_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<cg_candidate_list_s::crit_exts_c_::types>(),
    cg_candidate_list_s::crit_exts_c_::types::nof_types,
    cg_candidate_list_s::crit_exts_c_::types::nof_exts,
    cg_candidate_list_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_cg_candidate_list_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<cg_candidate_list_s::crit_exts_c_>,
    choice_table_cg_candidate_list_s_crit_exts_c__names,
    "cg_candidate_list_s::crit_exts_c_",
    offsetof(cg_candidate_list_s::crit_exts_c_, type_),
    offsetof(cg_candidate_list_s::crit_exts_c_, c),
    false};

void cg_candidate_list_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
cg_candidate_list_r17_ies_s& cg_candidate_list_s::crit_exts_c_::c1_c_::set_cg_candidate_list_r17()
{
  set(types::cg_candidate_list_r17);
  return c;
}
void cg_candidate_list_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void cg_candidate_list_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void cg_candidate_list_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_cg_candidate_list_s_crit_exts_c__c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_candidate_list_r17_ies_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_cg_candidate_list_s_crit_exts_c__c1_c__names[] =
    {{"cg-CandidateList-r17", nullptr}, {"spare3", nullptr}, {"spare2", nullptr}, {"spare1", nullptr}};
const asn1::table::choice_desc cg_candidate_list_s::crit_exts_c_::c1_c_::desc = {
    asn1::table::enum_int_type<cg_candidate_list_s::crit_exts_c_::c1_c_::types>(),
    cg_candidate_list_s::crit_exts_c_::c1_c_::types::nof_types,
    cg_candidate_list_s::crit_exts_c_::c1_c_::types::nof_exts,
    cg_candidate_list_s::crit_exts_c_::c1_c_::types::has_ext,
    false,
    choice_table_cg_candidate_list_s_crit_exts_c__c1_c__alternatives,
    &asn1::table::choice_set<cg_candidate_list_s::crit_exts_c_::c1_c_>,
    choice_table_cg_candidate_list_s_crit_exts_c__c1_c__names,
    "cg_candidate_list_s::crit_exts_c_::c1_c_",
    offsetof(cg_candidate_list_s::crit_exts_c_::c1_c_, type_),
    offsetof(cg_candidate_list_s::crit_exts_c_::c1_c_, c),
    false};

static const char*           enum_names_cg_candidate_list_s_crit_exts_c__c1_c__types_values[] = {"cg-CandidateList-r17",
                                                                                                 "spare3",
                                                                                                 "spare2",
                                                                                                 "spare1"};
const asn1::table::enum_desc cg_candidate_list_s::crit_exts_c_::c1_c_::types_opts::desc       = {
    enum_names_cg_candidate_list_s_crit_exts_c__c1_c__types_values,
    4,
    "cg_candidate_list_s::crit_exts_c_::c1_c_::types"};

static const char* enum_names_cg_candidate_list_s_crit_exts_c__types_values[]    = {"c1", "criticalExtensionsFuture"};
const asn1::table::enum_desc cg_candidate_list_s::crit_exts_c_::types_opts::desc = {
    enum_names_cg_candidate_list_s_crit_exts_c__types_values,
    2,
    "cg_candidate_list_s::crit_exts_c_::types"};
uint8_t cg_candidate_list_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cg_candidate_list_s::crit_exts_c_::types");
}

// ConfigRestrictModReqSCG ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfg_restrict_mod_req_scg_s_fields[] = {
    {offsetof(cfg_restrict_mod_req_scg_s, requested_bc_mrdc),
     (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_bc_mrdc_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &band_combination_info_sn_s::desc},
    {offsetof(cfg_restrict_mod_req_scg_s, requested_p_max_fr1),
     (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_p_max_fr1_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr}};
static const asn1::table::field_name seq_table_cfg_restrict_mod_req_scg_s_names[] = {{"requestedBC-MRDC", nullptr},
                                                                                     {"requestedP-MaxFR1", nullptr}};
static const asn1::table::seq_opt    seq_table_cfg_restrict_mod_req_scg_s_opts[]  = {
    {0, (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_bc_mrdc_present)},
    {1, (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_p_max_fr1_present)}};
static const asn1::table::seq_field seq_table_cfg_restrict_mod_req_scg_s_g0_fields[] = {
    {offsetof(cfg_restrict_mod_req_scg_s, requested_pdcch_blind_detection_scg),
     (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_pdcch_blind_detection_scg_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(15),
     nullptr},
    {offsetof(cfg_restrict_mod_req_scg_s, requested_p_max_eutra),
     (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_p_max_eutra_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr}};
static const asn1::table::field_name seq_table_cfg_restrict_mod_req_scg_s_g0_names[] = {
    {"requestedPDCCH-BlindDetectionSCG", nullptr},
    {"requestedP-MaxEUTRA", nullptr}};
static const asn1::table::seq_opt seq_table_cfg_restrict_mod_req_scg_s_g0_opts[] = {
    {0, (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_pdcch_blind_detection_scg_present)},
    {1, (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_p_max_eutra_present)}};
static const asn1::table::seq_field seq_table_cfg_restrict_mod_req_scg_s_g1_fields[] = {
    {offsetof(cfg_restrict_mod_req_scg_s, requested_p_max_fr2_r16),
     (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_p_max_fr2_r16_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr},
    {offsetof(cfg_restrict_mod_req_scg_s, requested_max_inter_freq_meas_id_scg_r16),
     (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_max_inter_freq_meas_id_scg_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(62),
     nullptr},
    {offsetof(cfg_restrict_mod_req_scg_s, requested_max_intra_freq_meas_id_scg_r16),
     (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_max_intra_freq_meas_id_scg_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(62),
     nullptr},
    {offsetof(cfg_restrict_mod_req_scg_s, requested_toffset_r16),
     (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_toffset_r16_present),
     asn1::table::enum_op<decltype(cfg_restrict_mod_req_scg_s::requested_toffset_r16)>(),
     decltype(cfg_restrict_mod_req_scg_s::requested_toffset_r16)::has_ext,
     false,
     decltype(cfg_restrict_mod_req_scg_s::requested_toffset_r16)::nof_types,
     decltype(cfg_restrict_mod_req_scg_s::requested_toffset_r16)::nof_exts,
     nullptr,
     &t_offset_r16_opts::desc}};
static const asn1::table::field_name seq_table_cfg_restrict_mod_req_scg_s_g1_names[] = {
    {"requestedP-MaxFR2-r16", nullptr},
    {"requestedMaxInterFreqMeasIdSCG-r16", nullptr},
    {"requestedMaxIntraFreqMeasIdSCG-r16", nullptr},
    {"requestedToffset-r16", nullptr}};
static const asn1::table::seq_opt seq_table_cfg_restrict_mod_req_scg_s_g1_opts[] = {
    {0, (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_p_max_fr2_r16_present)},
    {1, (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_max_inter_freq_meas_id_scg_r16_present)},
    {2, (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_max_intra_freq_meas_id_scg_r16_present)},
    {3, (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_toffset_r16_present)}};
static const asn1::table::seq_field seq_table_cfg_restrict_mod_req_scg_s_g2_fields[] = {
    {offsetof(cfg_restrict_mod_req_scg_s, reserved_res_cfg_nrdc_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(cfg_restrict_mod_req_scg_s::reserved_res_cfg_nrdc_r17)>::value,
     &res_cfg_nrdc_r17_s::desc}};
static const asn1::table::field_name seq_table_cfg_restrict_mod_req_scg_s_g2_names[] = {
    {"reservedResourceConfigNRDC-r17", nullptr}};
static const asn1::table::seq_opt seq_table_cfg_restrict_mod_req_scg_s_g2_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_cfg_restrict_mod_req_scg_s_g3_fields[] = {
    {offsetof(cfg_restrict_mod_req_scg_s, aggr_bw_sn_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(cfg_restrict_mod_req_scg_s::aggr_bw_sn_r17)>::value,
     &aggr_bw_sn_r17_s::desc}};
static const asn1::table::field_name seq_table_cfg_restrict_mod_req_scg_s_g3_names[] = {
    {"aggregatedBandwidthSN-r17", nullptr}};
static const asn1::table::seq_opt seq_table_cfg_restrict_mod_req_scg_s_g3_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_cfg_restrict_mod_req_scg_s_g4_fields[] = {
    {offsetof(cfg_restrict_mod_req_scg_s, requested_max_ltm_candidate_id_scg_r18),
     (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_max_ltm_candidate_id_scg_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(8),
     nullptr}};
static const asn1::table::field_name seq_table_cfg_restrict_mod_req_scg_s_g4_names[] = {
    {"requestedMaxLTM-CandidateIdSCG-r18", nullptr}};
static const asn1::table::seq_opt seq_table_cfg_restrict_mod_req_scg_s_g4_opts[] = {
    {0, (int32_t)offsetof(cfg_restrict_mod_req_scg_s, requested_max_ltm_candidate_id_scg_r18_present)}};
static const asn1::table::seq_field seq_table_cfg_restrict_mod_req_scg_s_g5_fields[] = {
    {offsetof(cfg_restrict_mod_req_scg_s, requested_l1_meas_cfg_nrdc_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(cfg_restrict_mod_req_scg_s::requested_l1_meas_cfg_nrdc_r18)>::value,
     &l1_meas_cfg_nrdc_r18_s::desc}};
static const asn1::table::field_name seq_table_cfg_restrict_mod_req_scg_s_g5_names[] = {
    {"requestedL1-MeasConfigNRDC-r18", nullptr}};
static const asn1::table::seq_opt seq_table_cfg_restrict_mod_req_scg_s_g5_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_cfg_restrict_mod_req_scg_s_g6_fields[] = {
    {offsetof(cfg_restrict_mod_req_scg_s, requested_ltm_res_cfg_nrdc_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(cfg_restrict_mod_req_scg_s::requested_ltm_res_cfg_nrdc_r18)>::value,
     &ltm_res_cfg_nrdc_r18_s::desc}};
static const asn1::table::field_name seq_table_cfg_restrict_mod_req_scg_s_g6_names[] = {
    {"requestedLTM-ResourceConfigNRDC-r18", nullptr}};
static const asn1::table::seq_opt seq_table_cfg_restrict_mod_req_scg_s_g6_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_cfg_restrict_mod_req_scg_s_groups[] = {
    {{seq_table_cfg_restrict_mod_req_scg_s_g0_fields,
      2,
      UINT64_C(0x0),
      seq_table_cfg_restrict_mod_req_scg_s_g0_opts,
      2},
     false,
     seq_table_cfg_restrict_mod_req_scg_s_g0_names},
    {{seq_table_cfg_restrict_mod_req_scg_s_g1_fields,
      4,
      UINT64_C(0x0),
      seq_table_cfg_restrict_mod_req_scg_s_g1_opts,
      4},
     false,
     seq_table_cfg_restrict_mod_req_scg_s_g1_names},
    {{seq_table_cfg_restrict_mod_req_scg_s_g2_fields,
      1,
      UINT64_C(0x0),
      seq_table_cfg_restrict_mod_req_scg_s_g2_opts,
      1},
     false,
     seq_table_cfg_restrict_mod_req_scg_s_g2_names},
    {{seq_table_cfg_restrict_mod_req_scg_s_g3_fields,
      1,
      UINT64_C(0x0),
      seq_table_cfg_restrict_mod_req_scg_s_g3_opts,
      1},
     false,
     seq_table_cfg_restrict_mod_req_scg_s_g3_names},
    {{seq_table_cfg_restrict_mod_req_scg_s_g4_fields,
      1,
      UINT64_C(0x0),
      seq_table_cfg_restrict_mod_req_scg_s_g4_opts,
      1},
     false,
     seq_table_cfg_restrict_mod_req_scg_s_g4_names},
    {{seq_table_cfg_restrict_mod_req_scg_s_g5_fields,
      1,
      UINT64_C(0x0),
      seq_table_cfg_restrict_mod_req_scg_s_g5_opts,
      1},
     false,
     seq_table_cfg_restrict_mod_req_scg_s_g5_names},
    {{seq_table_cfg_restrict_mod_req_scg_s_g6_fields,
      1,
      UINT64_C(0x0),
      seq_table_cfg_restrict_mod_req_scg_s_g6_opts,
      1},
     false,
     seq_table_cfg_restrict_mod_req_scg_s_g6_names}};
const asn1::table::seq_desc cfg_restrict_mod_req_scg_s::desc = {
    {seq_table_cfg_restrict_mod_req_scg_s_fields, 2, UINT64_C(0x0), seq_table_cfg_restrict_mod_req_scg_s_opts, 2},
    true,
    false,
    seq_table_cfg_restrict_mod_req_scg_s_groups,
    7,
    seq_table_cfg_restrict_mod_req_scg_s_names};

// MeasConfigSN ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_cfg_sn_s_fields[] = {
    {offsetof(meas_cfg_sn_s, measured_frequencies_sn),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(meas_cfg_sn_s::measured_frequencies_sn),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_freq_info_s::desc}};
static const asn1::table::field_name seq_table_meas_cfg_sn_s_names[] = {{"measuredFrequenciesSN", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_cfg_sn_s_opts[]  = {{0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          meas_cfg_sn_s::desc             = {
    {seq_table_meas_cfg_sn_s_fields, 1, UINT64_C(0x0), seq_table_meas_cfg_sn_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_meas_cfg_sn_s_names};

// PH-UplinkCarrierSCG ::= SEQUENCE
static const asn1::table::seq_field seq_table_ph_ul_carrier_scg_s_fields[] = {
    {offsetof(ph_ul_carrier_scg_s, ph_type1or3),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ph_ul_carrier_scg_s::ph_type1or3)>(),
     decltype(ph_ul_carrier_scg_s::ph_type1or3)::has_ext,
     false,
     decltype(ph_ul_carrier_scg_s::ph_type1or3)::nof_types,
     decltype(ph_ul_carrier_scg_s::ph_type1or3)::nof_exts,
     nullptr,
     &ph_ul_carrier_scg_s::ph_type1or3_opts::desc}};
static const asn1::table::field_name seq_table_ph_ul_carrier_scg_s_names[] = {{"ph-Type1or3", nullptr}};
const asn1::table::seq_desc          ph_ul_carrier_scg_s::desc             = {
    {seq_table_ph_ul_carrier_scg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_ph_ul_carrier_scg_s_names};

static const char*           enum_names_ph_ul_carrier_scg_s_ph_type1or3_e__values[] = {"type1", "type3"};
const asn1::table::enum_desc ph_ul_carrier_scg_s::ph_type1or3_opts::desc            = {
    enum_names_ph_ul_carrier_scg_s_ph_type1or3_e__values,
    2,
    "ph_ul_carrier_scg_s::ph_type1or3_e_"};
uint8_t ph_ul_carrier_scg_s::ph_type1or3_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 3};
  return map_enum_number(numbers, 2, value, "ph_ul_carrier_scg_s::ph_type1or3_e_");
}

// PH-InfoSCG ::= SEQUENCE
static const asn1::table::seq_field seq_table_ph_info_scg_s_fields[] = {
    {offsetof(ph_info_scg_s, serv_cell_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(ph_info_scg_s, ph_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ph_ul_carrier_scg_s::desc},
    {offsetof(ph_info_scg_s, ph_supplementary_ul),
     (int32_t)offsetof(ph_info_scg_s, ph_supplementary_ul_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ph_ul_carrier_scg_s::desc}};
static const asn1::table::field_name seq_table_ph_info_scg_s_names[] = {{"servCellIndex", nullptr},
                                                                        {"ph-Uplink", nullptr},
                                                                        {"ph-SupplementaryUplink", nullptr}};
static const asn1::table::seq_opt    seq_table_ph_info_scg_s_opts[]  = {
    {2, (int32_t)offsetof(ph_info_scg_s, ph_supplementary_ul_present)}};
static const asn1::table::seq_field seq_table_ph_info_scg_s_g0_fields[] = {
    {0,
     (int32_t)offsetof(ph_info_scg_s, two_srs_pusch_repeat_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ph_info_scg_s_g0_names[] = {{"twoSRS-PUSCH-Repetition-r17", "enabled"}};
static const asn1::table::seq_opt    seq_table_ph_info_scg_s_g0_opts[]  = {
    {0, (int32_t)offsetof(ph_info_scg_s, two_srs_pusch_repeat_r17_present)}};
static const asn1::table::seq_field seq_table_ph_info_scg_s_g1_fields[] = {
    {0,
     (int32_t)offsetof(ph_info_scg_s, two_srs_multipanel_scheme_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ph_info_scg_s_g1_names[] = {{"twoSRS-MultipanelScheme-r18", "enabled"}};
static const asn1::table::seq_opt    seq_table_ph_info_scg_s_g1_opts[]  = {
    {0, (int32_t)offsetof(ph_info_scg_s, two_srs_multipanel_scheme_r18_present)}};
static const asn1::table::seq_group seq_table_ph_info_scg_s_groups[] = {
    {{seq_table_ph_info_scg_s_g0_fields, 1, UINT64_C(0x0), seq_table_ph_info_scg_s_g0_opts, 1},
     false,
     seq_table_ph_info_scg_s_g0_names},
    {{seq_table_ph_info_scg_s_g1_fields, 1, UINT64_C(0x0), seq_table_ph_info_scg_s_g1_opts, 1},
     false,
     seq_table_ph_info_scg_s_g1_names}};
const asn1::table::seq_desc ph_info_scg_s::desc = {
    {seq_table_ph_info_scg_s_fields, 3, UINT64_C(0x3), seq_table_ph_info_scg_s_opts, 1},
    true,
    false,
    seq_table_ph_info_scg_s_groups,
    2,
    seq_table_ph_info_scg_s_names};

// CandidateCell-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_candidate_cell_r17_s_fields[] = {
    {offsetof(candidate_cell_r17_s, pci_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(candidate_cell_r17_s, cond_execution_cond_scg_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(candidate_cell_r17_s::cond_execution_cond_scg_r17),
                                      asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_candidate_cell_r17_s_names[] = {{"physCellId-r17", nullptr},
                                                                               {"condExecutionCondSCG-r17", nullptr}};
static const asn1::table::seq_opt seq_table_candidate_cell_r17_s_opts[] = {{1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       candidate_cell_r17_s::desc            = {
    {seq_table_candidate_cell_r17_s_fields, 2, UINT64_C(0x1), seq_table_candidate_cell_r17_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_candidate_cell_r17_s_names};

// CandidateCellInfo-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_candidate_cell_info_r17_s_fields[] = {
    {offsetof(candidate_cell_info_r17_s, ssb_freq_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(candidate_cell_info_r17_s, candidate_list_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(candidate_cell_info_r17_s::candidate_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &candidate_cell_r17_s::desc}};
static const asn1::table::field_name seq_table_candidate_cell_info_r17_s_names[] = {{"ssbFrequency-r17", nullptr},
                                                                                    {"candidateList-r17", nullptr}};
const asn1::table::seq_desc          candidate_cell_info_r17_s::desc             = {
    {seq_table_candidate_cell_info_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_candidate_cell_info_r17_s_names};

// CG-Config-v1800-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_cfg_v1800_ies_s_fields[] = {
    {offsetof(cg_cfg_v1800_ies_s, candidate_serving_freq_range_list_nr_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     128,
     &asn1::table::item_list_codec<decltype(cg_cfg_v1800_ies_s::candidate_serving_freq_range_list_nr_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &candidate_serving_freq_range_nr_r18_s::desc},
    {offsetof(cg_cfg_v1800_ies_s, candidate_serving_freq_list_nr_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     128,
     &asn1::table::list_codec<decltype(cg_cfg_v1800_ies_s::candidate_serving_freq_list_nr_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(3279165), false, false>>::value},
    {0,
     (int32_t)offsetof(cg_cfg_v1800_ies_s, idc_tdm_assist_cfg_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(cg_cfg_v1800_ies_s, candidate_cell_info_list_subsequent_cp_c_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(cg_cfg_v1800_ies_s::candidate_cell_info_list_subsequent_cp_c_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &candidate_cell_info_r17_s::desc},
    {offsetof(cg_cfg_v1800_ies_s, scpac_ref_cfg_scg_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(cg_cfg_v1800_ies_s::scpac_ref_cfg_scg_r18),
                                      asn1::table::string_json>::value},
    {offsetof(cg_cfg_v1800_ies_s, subsequent_cp_ac_info_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(cg_cfg_v1800_ies_s::subsequent_cp_ac_info_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &candidate_cell_info_r17_s::desc},
    {offsetof(cg_cfg_v1800_ies_s, success_pscell_cfg_r18),
     (int32_t)offsetof(cg_cfg_v1800_ies_s, success_pscell_cfg_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &success_pscell_cfg_r18_s::desc},
    {0,
     (int32_t)offsetof(cg_cfg_v1800_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_cg_cfg_v1800_ies_s_names[] = {
    {"candidateServingFreqRangeListNR-r18", nullptr},
    {"candidateServingFreqListNR-r18", nullptr},
    {"idc-TDM-AssistanceConfig-r18", "enabled"},
    {"candidateCellInfoListSubsequentCPC-r18", nullptr},
    {"scpac-ReferenceConfigurationSCG-r18", nullptr},
    {"subsequentCPAC-Information-r18", nullptr},
    {"successPSCell-Config-r18", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_cg_cfg_v1800_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(cg_cfg_v1800_ies_s, idc_tdm_assist_cfg_r18_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(cg_cfg_v1800_ies_s, success_pscell_cfg_r18_present)},
    {7, (int32_t)offsetof(cg_cfg_v1800_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_cfg_v1800_ies_s::desc = {
    {seq_table_cg_cfg_v1800_ies_s_fields, 8, UINT64_C(0x0), seq_table_cg_cfg_v1800_ies_s_opts, 8},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1800_ies_s_names};

// CG-Config-v1730-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_cfg_v1730_ies_s_fields[] = {
    {offsetof(cg_cfg_v1730_ies_s, fr1_carriers_scg_r17),
     (int32_t)offsetof(cg_cfg_v1730_ies_s, fr1_carriers_scg_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(cg_cfg_v1730_ies_s, fr2_carriers_scg_r17),
     (int32_t)offsetof(cg_cfg_v1730_ies_s, fr2_carriers_scg_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {offsetof(cg_cfg_v1730_ies_s, non_crit_ext),
     (int32_t)offsetof(cg_cfg_v1730_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_v1800_ies_s::desc}};
static const asn1::table::field_name seq_table_cg_cfg_v1730_ies_s_names[] = {{"fr1-Carriers-SCG-r17", nullptr},
                                                                             {"fr2-Carriers-SCG-r17", nullptr},
                                                                             {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_cg_cfg_v1730_ies_s_opts[]  = {
    {0, (int32_t)offsetof(cg_cfg_v1730_ies_s, fr1_carriers_scg_r17_present)},
    {1, (int32_t)offsetof(cg_cfg_v1730_ies_s, fr2_carriers_scg_r17_present)},
    {2, (int32_t)offsetof(cg_cfg_v1730_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_cfg_v1730_ies_s::desc = {
    {seq_table_cg_cfg_v1730_ies_s_fields, 3, UINT64_C(0x0), seq_table_cg_cfg_v1730_ies_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1730_ies_s_names};

// CG-Config-v1700-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_cfg_v1700_ies_s_fields[] = {
    {offsetof(cg_cfg_v1700_ies_s, candidate_cell_info_list_cp_c_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(cg_cfg_v1700_ies_s::candidate_cell_info_list_cp_c_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &candidate_cell_info_r17_s::desc},
    {0,
     (int32_t)offsetof(cg_cfg_v1700_ies_s, two_phr_mode_scg_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(cg_cfg_v1700_ies_s, non_crit_ext),
     (int32_t)offsetof(cg_cfg_v1700_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_v1730_ies_s::desc}};
static const asn1::table::field_name seq_table_cg_cfg_v1700_ies_s_names[] = {{"candidateCellInfoListCPC-r17", nullptr},
                                                                             {"twoPHRModeSCG-r17", "enabled"},
                                                                             {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_cg_cfg_v1700_ies_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(cg_cfg_v1700_ies_s, two_phr_mode_scg_r17_present)},
    {2, (int32_t)offsetof(cg_cfg_v1700_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_cfg_v1700_ies_s::desc = {
    {seq_table_cg_cfg_v1700_ies_s_fields, 3, UINT64_C(0x0), seq_table_cg_cfg_v1700_ies_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1700_ies_s_names};

// CG-Config-v1640-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_cfg_v1640_ies_s_fields[] = {
    {offsetof(cg_cfg_v1640_ies_s, serv_cell_info_list_scg_nr_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(cg_cfg_v1640_ies_s::serv_cell_info_list_scg_nr_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &serv_cell_info_xcg_nr_r16_s::desc},
    {offsetof(cg_cfg_v1640_ies_s, serv_cell_info_list_scg_eutra_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(cg_cfg_v1640_ies_s::serv_cell_info_list_scg_eutra_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &serv_cell_info_xcg_eutra_r16_s::desc},
    {offsetof(cg_cfg_v1640_ies_s, non_crit_ext),
     (int32_t)offsetof(cg_cfg_v1640_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_v1700_ies_s::desc}};
static const asn1::table::field_name seq_table_cg_cfg_v1640_ies_s_names[] = {{"servCellInfoListSCG-NR-r16", nullptr},
                                                                             {"servCellInfoListSCG-EUTRA-r16", nullptr},
                                                                             {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_cg_cfg_v1640_ies_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(cg_cfg_v1640_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_cfg_v1640_ies_s::desc = {
    {seq_table_cg_cfg_v1640_ies_s_fields, 3, UINT64_C(0x0), seq_table_cg_cfg_v1640_ies_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1640_ies_s_names};

// CG-Config-v1630-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_cfg_v1630_ies_s_fields[] = {
    {offsetof(cg_cfg_v1630_ies_s, sel_toffset_r16),
     (int32_t)offsetof(cg_cfg_v1630_ies_s, sel_toffset_r16_present),
     asn1::table::enum_op<decltype(cg_cfg_v1630_ies_s::sel_toffset_r16)>(),
     decltype(cg_cfg_v1630_ies_s::sel_toffset_r16)::has_ext,
     false,
     decltype(cg_cfg_v1630_ies_s::sel_toffset_r16)::nof_types,
     decltype(cg_cfg_v1630_ies_s::sel_toffset_r16)::nof_exts,
     nullptr,
     &t_offset_r16_opts::desc},
    {offsetof(cg_cfg_v1630_ies_s, non_crit_ext),
     (int32_t)offsetof(cg_cfg_v1630_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_v1640_ies_s::desc}};
static const asn1::table::field_name seq_table_cg_cfg_v1630_ies_s_names[] = {{"selectedToffset-r16", nullptr},
                                                                             {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_cg_cfg_v1630_ies_s_opts[]  = {
    {0, (int32_t)offsetof(cg_cfg_v1630_ies_s, sel_toffset_r16_present)},
    {1, (int32_t)offsetof(cg_cfg_v1630_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_cfg_v1630_ies_s::desc = {
    {seq_table_cg_cfg_v1630_ies_s_fields, 2, UINT64_C(0x0), seq_table_cg_cfg_v1630_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1630_ies_s_names};

// CG-Config-v1620-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_cfg_v1620_ies_s_fields[] = {
    {offsetof(cg_cfg_v1620_ies_s, ue_assist_info_scg_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(cg_cfg_v1620_ies_s::ue_assist_info_scg_r16),
                                      asn1::table::string_json>::value},
    {offsetof(cg_cfg_v1620_ies_s, non_crit_ext),
     (int32_t)offsetof(cg_cfg_v1620_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_v1630_ies_s::desc}};
static const asn1::table::field_name seq_table_cg_cfg_v1620_ies_s_names[] = {
    {"ueAssistanceInformationSCG-r16", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_cg_cfg_v1620_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(cg_cfg_v1620_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_cfg_v1620_ies_s::desc = {
    {seq_table_cg_cfg_v1620_ies_s_fields, 2, UINT64_C(0x0), seq_table_cg_cfg_v1620_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1620_ies_s_names};

// CG-Config-v1610-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_cfg_v1610_ies_s_fields[] = {
    {offsetof(cg_cfg_v1610_ies_s, drx_info_scg2),
     (int32_t)offsetof(cg_cfg_v1610_ies_s, drx_info_scg2_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drx_info2_s::desc},
    {offsetof(cg_cfg_v1610_ies_s, non_crit_ext),
     (int32_t)offsetof(cg_cfg_v1610_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_v1620_ies_s::desc}};
static const asn1::table::field_name seq_table_cg_cfg_v1610_ies_s_names[] = {{"drx-InfoSCG2", nullptr},
                                                                             {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_cg_cfg_v1610_ies_s_opts[]  = {
    {0, (int32_t)offsetof(cg_cfg_v1610_ies_s, drx_info_scg2_present)},
    {1, (int32_t)offsetof(cg_cfg_v1610_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_cfg_v1610_ies_s::desc = {
    {seq_table_cg_cfg_v1610_ies_s_fields, 2, UINT64_C(0x0), seq_table_cg_cfg_v1610_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1610_ies_s_names};

// CG-Config-v1590-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_cfg_v1590_ies_s_fields[] = {
    {offsetof(cg_cfg_v1590_ies_s, scell_frequencies_sn_nr),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(cg_cfg_v1590_ies_s::scell_frequencies_sn_nr),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(3279165), false, false>>::value},
    {offsetof(cg_cfg_v1590_ies_s, scell_frequencies_sn_eutra),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::list_codec<decltype(cg_cfg_v1590_ies_s::scell_frequencies_sn_eutra),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(262143), false, false>>::value},
    {offsetof(cg_cfg_v1590_ies_s, non_crit_ext),
     (int32_t)offsetof(cg_cfg_v1590_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_v1610_ies_s::desc}};
static const asn1::table::field_name seq_table_cg_cfg_v1590_ies_s_names[] = {{"scellFrequenciesSN-NR", nullptr},
                                                                             {"scellFrequenciesSN-EUTRA", nullptr},
                                                                             {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_cg_cfg_v1590_ies_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(cg_cfg_v1590_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_cfg_v1590_ies_s::desc = {
    {seq_table_cg_cfg_v1590_ies_s_fields, 3, UINT64_C(0x0), seq_table_cg_cfg_v1590_ies_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1590_ies_s_names};

// CG-Config-v1560-IEs ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__requested_cell_info_eutra_s__fields[] = {
        {offsetof(cg_cfg_v1560_ies_s::report_cgi_request_eutra_s_::requested_cell_info_eutra_s_, eutra_freq),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u32,
         false,
         false,
         INT64_C(0),
         UINT64_C(262143),
         nullptr},
        {offsetof(cg_cfg_v1560_ies_s::report_cgi_request_eutra_s_::requested_cell_info_eutra_s_,
                  cell_for_which_to_report_cgi_eutra),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(503),
         nullptr}};
static const asn1::table::field_name
    seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__requested_cell_info_eutra_s__names[] = {
        {"eutraFrequency", nullptr},
        {"cellForWhichToReportCGI-EUTRA", nullptr}};
static const asn1::table::seq_desc
    seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__requested_cell_info_eutra_s_ = {
        {seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__requested_cell_info_eutra_s__fields,
         2,
         UINT64_C(0x3),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__requested_cell_info_eutra_s__names};
static const asn1::table::seq_field seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__fields[] = {
    {offsetof(cg_cfg_v1560_ies_s::report_cgi_request_eutra_s_, requested_cell_info_eutra),
     (int32_t)offsetof(cg_cfg_v1560_ies_s::report_cgi_request_eutra_s_, requested_cell_info_eutra_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__requested_cell_info_eutra_s_}};
static const asn1::table::field_name seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__names[] = {
    {"requestedCellInfoEUTRA", nullptr}};
static const asn1::table::seq_opt seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__opts[] = {
    {0, (int32_t)offsetof(cg_cfg_v1560_ies_s::report_cgi_request_eutra_s_, requested_cell_info_eutra_present)}};
static const asn1::table::seq_desc seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s_ = {
    {seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__fields,
     1,
     UINT64_C(0x0),
     seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s__names};
static const asn1::table::seq_field seq_table_cg_cfg_v1560_ies_s_fields[] = {
    {offsetof(cg_cfg_v1560_ies_s, pscell_freq_eutra),
     (int32_t)offsetof(cg_cfg_v1560_ies_s, pscell_freq_eutra_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(262143),
     nullptr},
    {offsetof(cg_cfg_v1560_ies_s, scg_cell_group_cfg_eutra),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(cg_cfg_v1560_ies_s::scg_cell_group_cfg_eutra),
                                      asn1::table::string_json>::value},
    {offsetof(cg_cfg_v1560_ies_s, candidate_cell_info_list_sn_eutra),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(cg_cfg_v1560_ies_s::candidate_cell_info_list_sn_eutra),
                                      asn1::table::string_json>::value},
    {offsetof(cg_cfg_v1560_ies_s, candidate_serving_freq_list_eutra),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::list_codec<decltype(cg_cfg_v1560_ies_s::candidate_serving_freq_list_eutra),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(262143), false, false>>::value},
    {0,
     (int32_t)offsetof(cg_cfg_v1560_ies_s, need_for_gaps_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(cg_cfg_v1560_ies_s, drx_cfg_scg),
     (int32_t)offsetof(cg_cfg_v1560_ies_s, drx_cfg_scg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drx_cfg_s::desc},
    {offsetof(cg_cfg_v1560_ies_s, report_cgi_request_eutra),
     (int32_t)offsetof(cg_cfg_v1560_ies_s, report_cgi_request_eutra_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_cg_cfg_v1560_ies_s_report_cgi_request_eutra_s_},
    {offsetof(cg_cfg_v1560_ies_s, non_crit_ext),
     (int32_t)offsetof(cg_cfg_v1560_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_v1590_ies_s::desc}};
static const asn1::table::field_name seq_table_cg_cfg_v1560_ies_s_names[] = {{"pSCellFrequencyEUTRA", nullptr},
                                                                             {"scg-CellGroupConfigEUTRA", nullptr},
                                                                             {"candidateCellInfoListSN-EUTRA", nullptr},
                                                                             {"candidateServingFreqListEUTRA", nullptr},
                                                                             {"needForGaps", "true"},
                                                                             {"drx-ConfigSCG", nullptr},
                                                                             {"reportCGI-RequestEUTRA", nullptr},
                                                                             {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_cg_cfg_v1560_ies_s_opts[]  = {
    {0, (int32_t)offsetof(cg_cfg_v1560_ies_s, pscell_freq_eutra_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(cg_cfg_v1560_ies_s, need_for_gaps_present)},
    {5, (int32_t)offsetof(cg_cfg_v1560_ies_s, drx_cfg_scg_present)},
    {6, (int32_t)offsetof(cg_cfg_v1560_ies_s, report_cgi_request_eutra_present)},
    {7, (int32_t)offsetof(cg_cfg_v1560_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_cfg_v1560_ies_s::desc = {
    {seq_table_cg_cfg_v1560_ies_s_fields, 8, UINT64_C(0x0), seq_table_cg_cfg_v1560_ies_s_opts, 8},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1560_ies_s_names};

// CG-Config-v1540-IEs ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__requested_cell_info_s__fields[] = {
        {offsetof(cg_cfg_v1540_ies_s::report_cgi_request_nr_s_::requested_cell_info_s_, ssb_freq),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u32,
         false,
         false,
         INT64_C(0),
         UINT64_C(3279165),
         nullptr},
        {offsetof(cg_cfg_v1540_ies_s::report_cgi_request_nr_s_::requested_cell_info_s_, cell_for_which_to_report_cgi),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(1007),
         nullptr}};
static const asn1::table::field_name
    seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__requested_cell_info_s__names[] = {
        {"ssbFrequency", nullptr},
        {"cellForWhichToReportCGI", nullptr}};
static const asn1::table::seq_desc seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__requested_cell_info_s_ = {
    {seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__requested_cell_info_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__requested_cell_info_s__names};
static const asn1::table::seq_field seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__fields[] = {
    {offsetof(cg_cfg_v1540_ies_s::report_cgi_request_nr_s_, requested_cell_info),
     (int32_t)offsetof(cg_cfg_v1540_ies_s::report_cgi_request_nr_s_, requested_cell_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__requested_cell_info_s_}};
static const asn1::table::field_name seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__names[] = {
    {"requestedCellInfo", nullptr}};
static const asn1::table::seq_opt seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__opts[] = {
    {0, (int32_t)offsetof(cg_cfg_v1540_ies_s::report_cgi_request_nr_s_, requested_cell_info_present)}};
static const asn1::table::seq_desc seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s_ = {
    {seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__fields,
     1,
     UINT64_C(0x0),
     seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s__names};
static const asn1::table::seq_field seq_table_cg_cfg_v1540_ies_s_fields[] = {
    {offsetof(cg_cfg_v1540_ies_s, pscell_freq),
     (int32_t)offsetof(cg_cfg_v1540_ies_s, pscell_freq_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(cg_cfg_v1540_ies_s, report_cgi_request_nr),
     (int32_t)offsetof(cg_cfg_v1540_ies_s, report_cgi_request_nr_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_cg_cfg_v1540_ies_s_report_cgi_request_nr_s_},
    {offsetof(cg_cfg_v1540_ies_s, ph_info_scg),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::
         item_list_codec<decltype(cg_cfg_v1540_ies_s::ph_info_scg), asn1::table::sequence_item_ops, false, true>::value,
     &ph_info_scg_s::desc},
    {offsetof(cg_cfg_v1540_ies_s, non_crit_ext),
     (int32_t)offsetof(cg_cfg_v1540_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_v1560_ies_s::desc}};
static const asn1::table::field_name seq_table_cg_cfg_v1540_ies_s_names[] = {{"pSCellFrequency", nullptr},
                                                                             {"reportCGI-RequestNR", nullptr},
                                                                             {"ph-InfoSCG", nullptr},
                                                                             {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_cg_cfg_v1540_ies_s_opts[]  = {
    {0, (int32_t)offsetof(cg_cfg_v1540_ies_s, pscell_freq_present)},
    {1, (int32_t)offsetof(cg_cfg_v1540_ies_s, report_cgi_request_nr_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(cg_cfg_v1540_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_cfg_v1540_ies_s::desc = {
    {seq_table_cg_cfg_v1540_ies_s_fields, 4, UINT64_C(0x0), seq_table_cg_cfg_v1540_ies_s_opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_v1540_ies_s_names};

// CG-Config-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_cfg_ies_s_fields[] = {
    {offsetof(cg_cfg_ies_s, scg_cell_group_cfg),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(cg_cfg_ies_s::scg_cell_group_cfg), asn1::table::string_json>::value},
    {offsetof(cg_cfg_ies_s, scg_rb_cfg),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(cg_cfg_ies_s::scg_rb_cfg), asn1::table::string_json>::value},
    {offsetof(cg_cfg_ies_s, cfg_restrict_mod_req),
     (int32_t)offsetof(cg_cfg_ies_s, cfg_restrict_mod_req_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cfg_restrict_mod_req_scg_s::desc},
    {offsetof(cg_cfg_ies_s, drx_info_scg),
     (int32_t)offsetof(cg_cfg_ies_s, drx_info_scg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &drx_info_s::desc},
    {offsetof(cg_cfg_ies_s, candidate_cell_info_list_sn),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(cg_cfg_ies_s::candidate_cell_info_list_sn),
                                      asn1::table::string_json>::value},
    {offsetof(cg_cfg_ies_s, meas_cfg_sn),
     (int32_t)offsetof(cg_cfg_ies_s, meas_cfg_sn_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_cfg_sn_s::desc},
    {offsetof(cg_cfg_ies_s, sel_band_combination),
     (int32_t)offsetof(cg_cfg_ies_s, sel_band_combination_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &band_combination_info_sn_s::desc},
    {offsetof(cg_cfg_ies_s, fr_info_list_scg),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     31,
     &asn1::table::
         item_list_codec<decltype(cg_cfg_ies_s::fr_info_list_scg), asn1::table::sequence_item_ops, false, true>::value,
     &fr_info_s::desc},
    {offsetof(cg_cfg_ies_s, candidate_serving_freq_list_nr),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::list_codec<decltype(cg_cfg_ies_s::candidate_serving_freq_list_nr),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(3279165), false, false>>::value},
    {offsetof(cg_cfg_ies_s, non_crit_ext),
     (int32_t)offsetof(cg_cfg_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_v1540_ies_s::desc}};
static const asn1::table::field_name seq_table_cg_cfg_ies_s_names[] = {{"scg-CellGroupConfig", nullptr},
                                                                       {"scg-RB-Config", nullptr},
                                                                       {"configRestrictModReq", nullptr},
                                                                       {"drx-InfoSCG", nullptr},
                                                                       {"candidateCellInfoListSN", nullptr},
                                                                       {"measConfigSN", nullptr},
                                                                       {"selectedBandCombination", nullptr},
                                                                       {"fr-InfoListSCG", nullptr},
                                                                       {"candidateServingFreqListNR", nullptr},
                                                                       {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_cg_cfg_ies_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(cg_cfg_ies_s, cfg_restrict_mod_req_present)},
    {3, (int32_t)offsetof(cg_cfg_ies_s, drx_info_scg_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(cg_cfg_ies_s, meas_cfg_sn_present)},
    {6, (int32_t)offsetof(cg_cfg_ies_s, sel_band_combination_present)},
    {7, asn1::table::seq_field::list_presence},
    {8, asn1::table::seq_field::list_presence},
    {9, (int32_t)offsetof(cg_cfg_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc cg_cfg_ies_s::desc = {
    {seq_table_cg_cfg_ies_s_fields, 10, UINT64_C(0x0), seq_table_cg_cfg_ies_s_opts, 10},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_cfg_ies_s_names};

// CG-Config ::= SEQUENCE
static const asn1::table::seq_field  seq_table_cg_cfg_s_fields[] = {{offsetof(cg_cfg_s, crit_exts),
                                                                     asn1::table::seq_field::mandatory,
                                                                     asn1::table::field_op::choice,
                                                                     false,
                                                                     false,
                                                                     0,
                                                                     0,
                                                                     nullptr,
                                                                     &cg_cfg_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_cg_cfg_s_names[]  = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          cg_cfg_s::desc =
    {{seq_table_cg_cfg_s_fields, 1, UINT64_C(0x1), nullptr, 0}, false, false, nullptr, 0, seq_table_cg_cfg_s_names};

void cg_cfg_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_s::crit_exts_c_::c1_c_& cg_cfg_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void cg_cfg_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_cg_cfg_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_s::crit_exts_c_::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_cg_cfg_s_crit_exts_c__names[] = {
    {"c1", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc cg_cfg_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<cg_cfg_s::crit_exts_c_::types>(),
    cg_cfg_s::crit_exts_c_::types::nof_types,
    cg_cfg_s::crit_exts_c_::types::nof_exts,
    cg_cfg_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_cg_cfg_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<cg_cfg_s::crit_exts_c_>,
    choice_table_cg_cfg_s_crit_exts_c__names,
    "cg_cfg_s::crit_exts_c_",
    offsetof(cg_cfg_s::crit_exts_c_, type_),
    offsetof(cg_cfg_s::crit_exts_c_, c),
    false};

void cg_cfg_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
cg_cfg_ies_s& cg_cfg_s::crit_exts_c_::c1_c_::set_cg_cfg()
{
  set(types::cg_cfg);
  return c;
}
void cg_cfg_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void cg_cfg_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void cg_cfg_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_cg_cfg_s_crit_exts_c__c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cg_cfg_ies_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_cg_cfg_s_crit_exts_c__c1_c__names[] = {{"cg-Config", nullptr},
                                                                                         {"spare3", nullptr},
                                                                                         {"spare2", nullptr},
                                                                                         {"spare1", nullptr}};
const asn1::table::choice_desc       cg_cfg_s::crit_exts_c_::c1_c_::desc              = {
    asn1::table::enum_int_type<cg_cfg_s::crit_exts_c_::c1_c_::types>(),
    cg_cfg_s::crit_exts_c_::c1_c_::types::nof_types,
    cg_cfg_s::crit_exts_c_::c1_c_::types::nof_exts,
    cg_cfg_s::crit_exts_c_::c1_c_::types::has_ext,
    false,
    choice_table_cg_cfg_s_crit_exts_c__c1_c__alternatives,
    &asn1::table::choice_set<cg_cfg_s::crit_exts_c_::c1_c_>,
    choice_table_cg_cfg_s_crit_exts_c__c1_c__names,
    "cg_cfg_s::crit_exts_c_::c1_c_",
    offsetof(cg_cfg_s::crit_exts_c_::c1_c_, type_),
    offsetof(cg_cfg_s::crit_exts_c_::c1_c_, c),
    false};

static const char* enum_names_cg_cfg_s_crit_exts_c__c1_c__types_values[] = {"cg-Config", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc cg_cfg_s::crit_exts_c_::c1_c_::types_opts::desc = {
    enum_names_cg_cfg_s_crit_exts_c__c1_c__types_values,
    4,
    "cg_cfg_s::crit_exts_c_::c1_c_::types"};

static const char*           enum_names_cg_cfg_s_crit_exts_c__types_values[] = {"c1", "criticalExtensionsFuture"};
const asn1::table::enum_desc cg_cfg_s::crit_exts_c_::types_opts::desc = {enum_names_cg_cfg_s_crit_exts_c__types_values,
                                                                         2,
                                                                         "cg_cfg_s::crit_exts_c_::types"};
uint8_t                      cg_cfg_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "cg_cfg_s::crit_exts_c_::types");
}

// CandPSCellExecutionConditionInfo-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cand_pscell_execution_condition_info_r18_s_fields[] = {
    {offsetof(cand_pscell_execution_condition_info_r18_s, cand_pscell_exec_cond_report_cfg_nr_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     2,
     &asn1::table::item_list_codec<
         decltype(cand_pscell_execution_condition_info_r18_s::cand_pscell_exec_cond_report_cfg_nr_r18),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &report_cfg_nr_s::desc}};
static const asn1::table::field_name seq_table_cand_pscell_execution_condition_info_r18_s_names[] = {
    {"candPSCellExecCondReportConfigNR-r18", nullptr}};
const asn1::table::seq_desc cand_pscell_execution_condition_info_r18_s::desc = {
    {seq_table_cand_pscell_execution_condition_info_r18_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_cand_pscell_execution_condition_info_r18_s_names};

// HandoverCommand-v1800-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ho_cmd_v1800_ies_s_fields[] = {
    {offsetof(ho_cmd_v1800_ies_s, cand_pscell_execution_condition_info_r18),
     (int32_t)offsetof(ho_cmd_v1800_ies_s, cand_pscell_execution_condition_info_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cand_pscell_execution_condition_info_r18_s::desc},
    {0,
     (int32_t)offsetof(ho_cmd_v1800_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ho_cmd_v1800_ies_s_names[] = {
    {"candPSCellExecutionConditionInfo-r18", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ho_cmd_v1800_ies_s_opts[] = {
    {0, (int32_t)offsetof(ho_cmd_v1800_ies_s, cand_pscell_execution_condition_info_r18_present)},
    {1, (int32_t)offsetof(ho_cmd_v1800_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ho_cmd_v1800_ies_s::desc = {
    {seq_table_ho_cmd_v1800_ies_s_fields, 2, UINT64_C(0x0), seq_table_ho_cmd_v1800_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ho_cmd_v1800_ies_s_names};

// HandoverCommand-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ho_cmd_ies_s_fields[] = {
    {offsetof(ho_cmd_ies_s, ho_cmd_msg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ho_cmd_ies_s::ho_cmd_msg), asn1::table::string_json>::value},
    {offsetof(ho_cmd_ies_s, non_crit_ext),
     (int32_t)offsetof(ho_cmd_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ho_cmd_v1800_ies_s::desc}};
static const asn1::table::field_name seq_table_ho_cmd_ies_s_names[] = {{"handoverCommandMessage", nullptr},
                                                                       {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ho_cmd_ies_s_opts[]  = {
    {1, (int32_t)offsetof(ho_cmd_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ho_cmd_ies_s::desc = {
    {seq_table_ho_cmd_ies_s_fields, 2, UINT64_C(0x1), seq_table_ho_cmd_ies_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_ho_cmd_ies_s_names};

// HandoverCommand ::= SEQUENCE
static const asn1::table::seq_field  seq_table_ho_cmd_s_fields[] = {{offsetof(ho_cmd_s, crit_exts),
                                                                     asn1::table::seq_field::mandatory,
                                                                     asn1::table::field_op::choice,
                                                                     false,
                                                                     false,
                                                                     0,
                                                                     0,
                                                                     nullptr,
                                                                     &ho_cmd_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_ho_cmd_s_names[]  = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          ho_cmd_s::desc =
    {{seq_table_ho_cmd_s_fields, 1, UINT64_C(0x1), nullptr, 0}, false, false, nullptr, 0, seq_table_ho_cmd_s_names};

void ho_cmd_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ho_cmd_s::crit_exts_c_::c1_c_& ho_cmd_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ho_cmd_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_ho_cmd_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ho_cmd_s::crit_exts_c_::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ho_cmd_s_crit_exts_c__names[] = {
    {"c1", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc ho_cmd_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<ho_cmd_s::crit_exts_c_::types>(),
    ho_cmd_s::crit_exts_c_::types::nof_types,
    ho_cmd_s::crit_exts_c_::types::nof_exts,
    ho_cmd_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_ho_cmd_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<ho_cmd_s::crit_exts_c_>,
    choice_table_ho_cmd_s_crit_exts_c__names,
    "ho_cmd_s::crit_exts_c_",
    offsetof(ho_cmd_s::crit_exts_c_, type_),
    offsetof(ho_cmd_s::crit_exts_c_, c),
    false};

void ho_cmd_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ho_cmd_ies_s& ho_cmd_s::crit_exts_c_::c1_c_::set_ho_cmd()
{
  set(types::ho_cmd);
  return c;
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ho_cmd_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_ho_cmd_s_crit_exts_c__c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ho_cmd_ies_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_ho_cmd_s_crit_exts_c__c1_c__names[] = {{"handoverCommand", nullptr},
                                                                                         {"spare3", nullptr},
                                                                                         {"spare2", nullptr},
                                                                                         {"spare1", nullptr}};
const asn1::table::choice_desc       ho_cmd_s::crit_exts_c_::c1_c_::desc              = {
    asn1::table::enum_int_type<ho_cmd_s::crit_exts_c_::c1_c_::types>(),
    ho_cmd_s::crit_exts_c_::c1_c_::types::nof_types,
    ho_cmd_s::crit_exts_c_::c1_c_::types::nof_exts,
    ho_cmd_s::crit_exts_c_::c1_c_::types::has_ext,
    false,
    choice_table_ho_cmd_s_crit_exts_c__c1_c__alternatives,
    &asn1::table::choice_set<ho_cmd_s::crit_exts_c_::c1_c_>,
    choice_table_ho_cmd_s_crit_exts_c__c1_c__names,
    "ho_cmd_s::crit_exts_c_::c1_c_",
    offsetof(ho_cmd_s::crit_exts_c_::c1_c_, type_),
    offsetof(ho_cmd_s::crit_exts_c_::c1_c_, c),
    false};

static const char*           enum_names_ho_cmd_s_crit_exts_c__c1_c__types_values[] = {"handoverCommand",
                                                                                      "spare3",
                                                                                      "spare2",
                                                                                      "spare1"};
const asn1::table::enum_desc ho_cmd_s::crit_exts_c_::c1_c_::types_opts::desc       = {
    enum_names_ho_cmd_s_crit_exts_c__c1_c__types_values,
    4,
    "ho_cmd_s::crit_exts_c_::c1_c_::types"};

static const char*           enum_names_ho_cmd_s_crit_exts_c__types_values[] = {"c1", "criticalExtensionsFuture"};
const asn1::table::enum_desc ho_cmd_s::crit_exts_c_::types_opts::desc = {enum_names_ho_cmd_s_crit_exts_c__types_values,
                                                                         2,
                                                                         "ho_cmd_s::crit_exts_c_::types"};
uint8_t                      ho_cmd_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ho_cmd_s::crit_exts_c_::types");
}

// RRM-Config ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrm_cfg_s_fields[] = {
    {offsetof(rrm_cfg_s, ue_inactive_time),
     (int32_t)offsetof(rrm_cfg_s, ue_inactive_time_present),
     asn1::table::enum_op<decltype(rrm_cfg_s::ue_inactive_time)>(),
     decltype(rrm_cfg_s::ue_inactive_time)::has_ext,
     false,
     decltype(rrm_cfg_s::ue_inactive_time)::nof_types,
     decltype(rrm_cfg_s::ue_inactive_time)::nof_exts,
     nullptr,
     &rrm_cfg_s::ue_inactive_time_opts::desc},
    {offsetof(rrm_cfg_s, candidate_cell_info_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(rrm_cfg_s::candidate_cell_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_result2_nr_s::desc}};
static const asn1::table::field_name seq_table_rrm_cfg_s_names[] = {{"ue-InactiveTime", nullptr},
                                                                    {"candidateCellInfoList", nullptr}};
static const asn1::table::seq_opt    seq_table_rrm_cfg_s_opts[]  = {
    {0, (int32_t)offsetof(rrm_cfg_s, ue_inactive_time_present)},
    {1, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_rrm_cfg_s_g0_fields[] = {
    {offsetof(rrm_cfg_s, candidate_cell_info_list_sn_eutra),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::ptr_item_list_codec<decltype(rrm_cfg_s::candidate_cell_info_list_sn_eutra),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &meas_result2_eutra_s::desc}};
static const asn1::table::field_name seq_table_rrm_cfg_s_g0_names[] = {{"candidateCellInfoListSN-EUTRA", nullptr}};
static const asn1::table::seq_opt    seq_table_rrm_cfg_s_g0_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_rrm_cfg_s_groups[]   = {
    {{seq_table_rrm_cfg_s_g0_fields, 1, UINT64_C(0x0), seq_table_rrm_cfg_s_g0_opts, 1},
        false,
        seq_table_rrm_cfg_s_g0_names}};
const asn1::table::seq_desc rrm_cfg_s::desc = {
    {seq_table_rrm_cfg_s_fields, 2, UINT64_C(0x0), seq_table_rrm_cfg_s_opts, 2},
    true,
    false,
    seq_table_rrm_cfg_s_groups,
    1,
    seq_table_rrm_cfg_s_names};

static const char* enum_names_rrm_cfg_s_ue_inactive_time_e__values[] = {
    "s1",    "s2",       "s3",    "s5",           "s7",      "s10",   "s15",     "s20",      "s25",     "s30",
    "s40",   "s50",      "min1",  "min1s20",      "min1s40", "min2",  "min2s30", "min3",     "min3s30", "min4",
    "min5",  "min6",     "min7",  "min8",         "min9",    "min10", "min12",   "min14",    "min17",   "min20",
    "min24", "min28",    "min33", "min38",        "min44",   "min50", "hr1",     "hr1min30", "hr2",     "hr2min30",
    "hr3",   "hr3min30", "hr4",   "hr5",          "hr6",     "hr8",   "hr10",    "hr13",     "hr16",    "hr20",
    "day1",  "day1hr12", "day2",  "day2hr12",     "day3",    "day4",  "day5",    "day7",     "day10",   "day14",
    "day19", "day24",    "day30", "dayMoreThan30"};
const asn1::table::enum_desc rrm_cfg_s::ue_inactive_time_opts::desc = {enum_names_rrm_cfg_s_ue_inactive_time_e__values,
                                                                       64,
                                                                       "rrm_cfg_s::ue_inactive_time_e_"};

// HandoverPreparationInformation-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ho_prep_info_ies_s_fields[] = {
    {offsetof(ho_prep_info_ies_s, ue_cap_rat_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     8,
     &asn1::table::item_list_codec<decltype(ho_prep_info_ies_s::ue_cap_rat_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ue_cap_rat_container_s::desc},
    {offsetof(ho_prep_info_ies_s, source_cfg),
     (int32_t)offsetof(ho_prep_info_ies_s, source_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &as_cfg_s::desc},
    {offsetof(ho_prep_info_ies_s, rrm_cfg),
     (int32_t)offsetof(ho_prep_info_ies_s, rrm_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrm_cfg_s::desc},
    {offsetof(ho_prep_info_ies_s, as_context),
     (int32_t)offsetof(ho_prep_info_ies_s, as_context_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &as_context_s::desc},
    {0,
     (int32_t)offsetof(ho_prep_info_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ho_prep_info_ies_s_names[] = {{"ue-CapabilityRAT-List", nullptr},
                                                                             {"sourceConfig", nullptr},
                                                                             {"rrm-Config", nullptr},
                                                                             {"as-Context", nullptr},
                                                                             {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ho_prep_info_ies_s_opts[]  = {
    {1, (int32_t)offsetof(ho_prep_info_ies_s, source_cfg_present)},
    {2, (int32_t)offsetof(ho_prep_info_ies_s, rrm_cfg_present)},
    {3, (int32_t)offsetof(ho_prep_info_ies_s, as_context_present)},
    {4, (int32_t)offsetof(ho_prep_info_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ho_prep_info_ies_s::desc = {
    {seq_table_ho_prep_info_ies_s_fields, 5, UINT64_C(0x1), seq_table_ho_prep_info_ies_s_opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_ho_prep_info_ies_s_names};

// HandoverPreparationInformation ::= SEQUENCE
static const asn1::table::seq_field  seq_table_ho_prep_info_s_fields[] = {{offsetof(ho_prep_info_s, crit_exts),
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::field_op::choice,
                                                                           false,
                                                                           false,
                                                                           0,
                                                                           0,
                                                                           nullptr,
                                                                           &ho_prep_info_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_ho_prep_info_s_names[]  = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc ho_prep_info_s::desc = {{seq_table_ho_prep_info_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0,
                                                    seq_table_ho_prep_info_s_names};

void ho_prep_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ho_prep_info_s::crit_exts_c_::c1_c_& ho_prep_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ho_prep_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_ho_prep_info_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ho_prep_info_s::crit_exts_c_::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ho_prep_info_s_crit_exts_c__names[] = {
    {"c1", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc ho_prep_info_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<ho_prep_info_s::crit_exts_c_::types>(),
    ho_prep_info_s::crit_exts_c_::types::nof_types,
    ho_prep_info_s::crit_exts_c_::types::nof_exts,
    ho_prep_info_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_ho_prep_info_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<ho_prep_info_s::crit_exts_c_>,
    choice_table_ho_prep_info_s_crit_exts_c__names,
    "ho_prep_info_s::crit_exts_c_",
    offsetof(ho_prep_info_s::crit_exts_c_, type_),
    offsetof(ho_prep_info_s::crit_exts_c_, c),
    false};

void ho_prep_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ho_prep_info_ies_s& ho_prep_info_s::crit_exts_c_::c1_c_::set_ho_prep_info()
{
  set(types::ho_prep_info);
  return c;
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ho_prep_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_ho_prep_info_s_crit_exts_c__c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ho_prep_info_ies_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_ho_prep_info_s_crit_exts_c__c1_c__names[] =
    {{"handoverPreparationInformation", nullptr}, {"spare3", nullptr}, {"spare2", nullptr}, {"spare1", nullptr}};
const asn1::table::choice_desc ho_prep_info_s::crit_exts_c_::c1_c_::desc = {
    asn1::table::enum_int_type<ho_prep_info_s::crit_exts_c_::c1_c_::types>(),
    ho_prep_info_s::crit_exts_c_::c1_c_::types::nof_types,
    ho_prep_info_s::crit_exts_c_::c1_c_::types::nof_exts,
    ho_prep_info_s::crit_exts_c_::c1_c_::types::has_ext,
    false,
    choice_table_ho_prep_info_s_crit_exts_c__c1_c__alternatives,
    &asn1::table::choice_set<ho_prep_info_s::crit_exts_c_::c1_c_>,
    choice_table_ho_prep_info_s_crit_exts_c__c1_c__names,
    "ho_prep_info_s::crit_exts_c_::c1_c_",
    offsetof(ho_prep_info_s::crit_exts_c_::c1_c_, type_),
    offsetof(ho_prep_info_s::crit_exts_c_::c1_c_, c),
    false};

static const char* enum_names_ho_prep_info_s_crit_exts_c__c1_c__types_values[]     = {"handoverPreparationInformation",
                                                                                      "spare3",
                                                                                      "spare2",
                                                                                      "spare1"};
const asn1::table::enum_desc ho_prep_info_s::crit_exts_c_::c1_c_::types_opts::desc = {
    enum_names_ho_prep_info_s_crit_exts_c__c1_c__types_values,
    4,
    "ho_prep_info_s::crit_exts_c_::c1_c_::types"};

static const char*           enum_names_ho_prep_info_s_crit_exts_c__types_values[] = {"c1", "criticalExtensionsFuture"};
const asn1::table::enum_desc ho_prep_info_s::crit_exts_c_::types_opts::desc        = {
    enum_names_ho_prep_info_s_crit_exts_c__types_values,
    2,
    "ho_prep_info_s::crit_exts_c_::types"};
uint8_t ho_prep_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ho_prep_info_s::crit_exts_c_::types");
}

// MeasTiming ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_timing_s_freq_and_timing_s__fields[] = {
    {offsetof(meas_timing_s::freq_and_timing_s_, carrier_freq),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(meas_timing_s::freq_and_timing_s_, ssb_subcarrier_spacing),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(meas_timing_s::freq_and_timing_s_::ssb_subcarrier_spacing)>(),
     decltype(meas_timing_s::freq_and_timing_s_::ssb_subcarrier_spacing)::has_ext,
     false,
     decltype(meas_timing_s::freq_and_timing_s_::ssb_subcarrier_spacing)::nof_types,
     decltype(meas_timing_s::freq_and_timing_s_::ssb_subcarrier_spacing)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(meas_timing_s::freq_and_timing_s_, ssb_meas_timing_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_mtc_s::desc},
    {offsetof(meas_timing_s::freq_and_timing_s_, ss_rssi_meas),
     (int32_t)offsetof(meas_timing_s::freq_and_timing_s_, ss_rssi_meas_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ss_rssi_meas_s::desc}};
static const asn1::table::field_name seq_table_meas_timing_s_freq_and_timing_s__names[] = {
    {"carrierFreq", nullptr},
    {"ssbSubcarrierSpacing", nullptr},
    {"ssb-MeasurementTimingConfiguration", nullptr},
    {"ss-RSSI-Measurement", nullptr}};
static const asn1::table::seq_opt seq_table_meas_timing_s_freq_and_timing_s__opts[] = {
    {3, (int32_t)offsetof(meas_timing_s::freq_and_timing_s_, ss_rssi_meas_present)}};
static const asn1::table::seq_desc seq_table_meas_timing_s_freq_and_timing_s_ = {
    {seq_table_meas_timing_s_freq_and_timing_s__fields,
     4,
     UINT64_C(0x7),
     seq_table_meas_timing_s_freq_and_timing_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_timing_s_freq_and_timing_s__names};
static const asn1::table::seq_field seq_table_meas_timing_s_fields[] = {
    {offsetof(meas_timing_s, freq_and_timing),
     (int32_t)offsetof(meas_timing_s, freq_and_timing_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_meas_timing_s_freq_and_timing_s_}};
static const asn1::table::field_name seq_table_meas_timing_s_names[] = {{"frequencyAndTiming", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_timing_s_opts[]  = {
    {0, (int32_t)offsetof(meas_timing_s, freq_and_timing_present)}};
static const asn1::table::seq_field seq_table_meas_timing_s_g0_fields[] = {
    {offsetof(meas_timing_s, ssb_to_measure),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(meas_timing_s::ssb_to_measure)>::value,
     &ssb_to_measure_c::desc},
    {offsetof(meas_timing_s, pci),
     (int32_t)offsetof(meas_timing_s, pci_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr}};
static const asn1::table::field_name seq_table_meas_timing_s_g0_names[] = {{"ssb-ToMeasure", nullptr},
                                                                           {"physCellId", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_timing_s_g0_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(meas_timing_s, pci_present)}};
static const asn1::table::seq_group seq_table_meas_timing_s_groups[] = {
    {{seq_table_meas_timing_s_g0_fields, 2, UINT64_C(0x0), seq_table_meas_timing_s_g0_opts, 2},
     false,
     seq_table_meas_timing_s_g0_names}};
const asn1::table::seq_desc meas_timing_s::desc = {
    {seq_table_meas_timing_s_fields, 1, UINT64_C(0x0), seq_table_meas_timing_s_opts, 1},
    true,
    false,
    seq_table_meas_timing_s_groups,
    1,
    seq_table_meas_timing_s_names};

// MeasurementTimingConfiguration-v1610-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_timing_cfg_v1610_ies_s_csi_rs_cfg_r16_s__fields[] = {
    {offsetof(meas_timing_cfg_v1610_ies_s::csi_rs_cfg_r16_s_, csi_rs_subcarrier_spacing_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(meas_timing_cfg_v1610_ies_s::csi_rs_cfg_r16_s_::csi_rs_subcarrier_spacing_r16)>(),
     decltype(meas_timing_cfg_v1610_ies_s::csi_rs_cfg_r16_s_::csi_rs_subcarrier_spacing_r16)::has_ext,
     false,
     decltype(meas_timing_cfg_v1610_ies_s::csi_rs_cfg_r16_s_::csi_rs_subcarrier_spacing_r16)::nof_types,
     decltype(meas_timing_cfg_v1610_ies_s::csi_rs_cfg_r16_s_::csi_rs_subcarrier_spacing_r16)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(meas_timing_cfg_v1610_ies_s::csi_rs_cfg_r16_s_, csi_rs_cell_mob_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_rs_cell_mob_s::desc},
    {offsetof(meas_timing_cfg_v1610_ies_s::csi_rs_cfg_r16_s_, ref_ssb_freq_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr}};
static const asn1::table::field_name seq_table_meas_timing_cfg_v1610_ies_s_csi_rs_cfg_r16_s__names[] = {
    {"csi-RS-SubcarrierSpacing-r16", nullptr},
    {"csi-RS-CellMobility-r16", nullptr},
    {"refSSBFreq-r16", nullptr}};
static const asn1::table::seq_desc seq_table_meas_timing_cfg_v1610_ies_s_csi_rs_cfg_r16_s_ = {
    {seq_table_meas_timing_cfg_v1610_ies_s_csi_rs_cfg_r16_s__fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_timing_cfg_v1610_ies_s_csi_rs_cfg_r16_s__names};
static const asn1::table::seq_field seq_table_meas_timing_cfg_v1610_ies_s_fields[] = {
    {offsetof(meas_timing_cfg_v1610_ies_s, csi_rs_cfg_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_meas_timing_cfg_v1610_ies_s_csi_rs_cfg_r16_s_},
    {0,
     (int32_t)offsetof(meas_timing_cfg_v1610_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_meas_timing_cfg_v1610_ies_s_names[] = {
    {"csi-RS-Config-r16", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_meas_timing_cfg_v1610_ies_s_opts[] = {
    {1, (int32_t)offsetof(meas_timing_cfg_v1610_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc meas_timing_cfg_v1610_ies_s::desc = {
    {seq_table_meas_timing_cfg_v1610_ies_s_fields, 2, UINT64_C(0x1), seq_table_meas_timing_cfg_v1610_ies_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_timing_cfg_v1610_ies_s_names};

// MeasurementTimingConfiguration-v1550-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_timing_cfg_v1550_ies_s_fields[] = {
    {offsetof(meas_timing_cfg_v1550_ies_s, camp_on_first_ssb),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(meas_timing_cfg_v1550_ies_s, ps_cell_only_on_first_ssb),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(meas_timing_cfg_v1550_ies_s, non_crit_ext),
     (int32_t)offsetof(meas_timing_cfg_v1550_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_timing_cfg_v1610_ies_s::desc}};
static const asn1::table::field_name seq_table_meas_timing_cfg_v1550_ies_s_names[] = {
    {"campOnFirstSSB", nullptr},
    {"psCellOnlyOnFirstSSB", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_meas_timing_cfg_v1550_ies_s_opts[] = {
    {2, (int32_t)offsetof(meas_timing_cfg_v1550_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc meas_timing_cfg_v1550_ies_s::desc = {
    {seq_table_meas_timing_cfg_v1550_ies_s_fields, 3, UINT64_C(0x3), seq_table_meas_timing_cfg_v1550_ies_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_timing_cfg_v1550_ies_s_names};

// MeasurementTimingConfiguration-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_timing_cfg_ies_s_fields[] = {
    {offsetof(meas_timing_cfg_ies_s, meas_timing),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(meas_timing_cfg_ies_s::meas_timing),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_timing_s::desc},
    {offsetof(meas_timing_cfg_ies_s, non_crit_ext),
     (int32_t)offsetof(meas_timing_cfg_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_timing_cfg_v1550_ies_s::desc}};
static const asn1::table::field_name seq_table_meas_timing_cfg_ies_s_names[] = {{"measTiming", nullptr},
                                                                                {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_timing_cfg_ies_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(meas_timing_cfg_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc meas_timing_cfg_ies_s::desc = {
    {seq_table_meas_timing_cfg_ies_s_fields, 2, UINT64_C(0x0), seq_table_meas_timing_cfg_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_timing_cfg_ies_s_names};

// MeasurementTimingConfiguration ::= SEQUENCE
static const asn1::table::seq_field  seq_table_meas_timing_cfg_s_fields[] = {{offsetof(meas_timing_cfg_s, crit_exts),
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::field_op::choice,
                                                                              false,
                                                                              false,
                                                                              0,
                                                                              0,
                                                                              nullptr,
                                                                              &meas_timing_cfg_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_meas_timing_cfg_s_names[]  = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          meas_timing_cfg_s::desc              = {
    {seq_table_meas_timing_cfg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_timing_cfg_s_names};

void meas_timing_cfg_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
meas_timing_cfg_s::crit_exts_c_::c1_c_& meas_timing_cfg_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void meas_timing_cfg_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_meas_timing_cfg_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_timing_cfg_s::crit_exts_c_::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_meas_timing_cfg_s_crit_exts_c__names[] = {
    {"c1", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc meas_timing_cfg_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<meas_timing_cfg_s::crit_exts_c_::types>(),
    meas_timing_cfg_s::crit_exts_c_::types::nof_types,
    meas_timing_cfg_s::crit_exts_c_::types::nof_exts,
    meas_timing_cfg_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_meas_timing_cfg_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<meas_timing_cfg_s::crit_exts_c_>,
    choice_table_meas_timing_cfg_s_crit_exts_c__names,
    "meas_timing_cfg_s::crit_exts_c_",
    offsetof(meas_timing_cfg_s::crit_exts_c_, type_),
    offsetof(meas_timing_cfg_s::crit_exts_c_, c),
    false};

void meas_timing_cfg_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
meas_timing_cfg_ies_s& meas_timing_cfg_s::crit_exts_c_::c1_c_::set_meas_timing_conf()
{
  set(types::meas_timing_conf);
  return c;
}
void meas_timing_cfg_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void meas_timing_cfg_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void meas_timing_cfg_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_meas_timing_cfg_s_crit_exts_c__c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_timing_cfg_ies_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_meas_timing_cfg_s_crit_exts_c__c1_c__names[] =
    {{"measTimingConf", nullptr}, {"spare3", nullptr}, {"spare2", nullptr}, {"spare1", nullptr}};
const asn1::table::choice_desc meas_timing_cfg_s::crit_exts_c_::c1_c_::desc = {
    asn1::table::enum_int_type<meas_timing_cfg_s::crit_exts_c_::c1_c_::types>(),
    meas_timing_cfg_s::crit_exts_c_::c1_c_::types::nof_types,
    meas_timing_cfg_s::crit_exts_c_::c1_c_::types::nof_exts,
    meas_timing_cfg_s::crit_exts_c_::c1_c_::types::has_ext,
    false,
    choice_table_meas_timing_cfg_s_crit_exts_c__c1_c__alternatives,
    &asn1::table::choice_set<meas_timing_cfg_s::crit_exts_c_::c1_c_>,
    choice_table_meas_timing_cfg_s_crit_exts_c__c1_c__names,
    "meas_timing_cfg_s::crit_exts_c_::c1_c_",
    offsetof(meas_timing_cfg_s::crit_exts_c_::c1_c_, type_),
    offsetof(meas_timing_cfg_s::crit_exts_c_::c1_c_, c),
    false};

static const char*           enum_names_meas_timing_cfg_s_crit_exts_c__c1_c__types_values[] = {"measTimingConf",
                                                                                               "spare3",
                                                                                               "spare2",
                                                                                               "spare1"};
const asn1::table::enum_desc meas_timing_cfg_s::crit_exts_c_::c1_c_::types_opts::desc       = {
    enum_names_meas_timing_cfg_s_crit_exts_c__c1_c__types_values,
    4,
    "meas_timing_cfg_s::crit_exts_c_::c1_c_::types"};

static const char* enum_names_meas_timing_cfg_s_crit_exts_c__types_values[]    = {"c1", "criticalExtensionsFuture"};
const asn1::table::enum_desc meas_timing_cfg_s::crit_exts_c_::types_opts::desc = {
    enum_names_meas_timing_cfg_s_crit_exts_c__types_values,
    2,
    "meas_timing_cfg_s::crit_exts_c_::types"};
uint8_t meas_timing_cfg_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "meas_timing_cfg_s::crit_exts_c_::types");
}

// UERadioAccessCapabilityInformation-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_radio_access_cap_info_ies_s_fields[] = {
    {offsetof(ue_radio_access_cap_info_ies_s, ue_radio_access_cap_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ue_radio_access_cap_info_ies_s::ue_radio_access_cap_info),
                                asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(ue_radio_access_cap_info_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_radio_access_cap_info_ies_s_names[] = {
    {"ue-RadioAccessCapabilityInfo", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_radio_access_cap_info_ies_s_opts[] = {
    {1, (int32_t)offsetof(ue_radio_access_cap_info_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_radio_access_cap_info_ies_s::desc = {{seq_table_ue_radio_access_cap_info_ies_s_fields,
                                                                     2,
                                                                     UINT64_C(0x1),
                                                                     seq_table_ue_radio_access_cap_info_ies_s_opts,
                                                                     1},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_ue_radio_access_cap_info_ies_s_names};

// UERadioAccessCapabilityInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_radio_access_cap_info_s_fields[] = {
    {offsetof(ue_radio_access_cap_info_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_radio_access_cap_info_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_ue_radio_access_cap_info_s_names[] = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          ue_radio_access_cap_info_s::desc             = {
    {seq_table_ue_radio_access_cap_info_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_radio_access_cap_info_s_names};

void ue_radio_access_cap_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_access_cap_info_s::crit_exts_c_::c1_c_& ue_radio_access_cap_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_radio_access_cap_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_ue_radio_access_cap_info_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ue_radio_access_cap_info_s_crit_exts_c__names[] = {
    {"c1", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc ue_radio_access_cap_info_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<ue_radio_access_cap_info_s::crit_exts_c_::types>(),
    ue_radio_access_cap_info_s::crit_exts_c_::types::nof_types,
    ue_radio_access_cap_info_s::crit_exts_c_::types::nof_exts,
    ue_radio_access_cap_info_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_ue_radio_access_cap_info_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<ue_radio_access_cap_info_s::crit_exts_c_>,
    choice_table_ue_radio_access_cap_info_s_crit_exts_c__names,
    "ue_radio_access_cap_info_s::crit_exts_c_",
    offsetof(ue_radio_access_cap_info_s::crit_exts_c_, type_),
    offsetof(ue_radio_access_cap_info_s::crit_exts_c_, c),
    false};

void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_access_cap_info_ies_s& ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_ue_radio_access_cap_info()
{
  set(types::ue_radio_access_cap_info);
  return c;
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_ue_radio_access_cap_info_s_crit_exts_c__c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_radio_access_cap_info_ies_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_ue_radio_access_cap_info_s_crit_exts_c__c1_c__names[] = {
    {"ueRadioAccessCapabilityInformation", nullptr},
    {"spare7", nullptr},
    {"spare6", nullptr},
    {"spare5", nullptr},
    {"spare4", nullptr},
    {"spare3", nullptr},
    {"spare2", nullptr},
    {"spare1", nullptr}};
const asn1::table::choice_desc ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::desc = {
    asn1::table::enum_int_type<ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types>(),
    ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types::nof_types,
    ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types::nof_exts,
    ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types::has_ext,
    false,
    choice_table_ue_radio_access_cap_info_s_crit_exts_c__c1_c__alternatives,
    &asn1::table::choice_set<ue_radio_access_cap_info_s::crit_exts_c_::c1_c_>,
    choice_table_ue_radio_access_cap_info_s_crit_exts_c__c1_c__names,
    "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_",
    offsetof(ue_radio_access_cap_info_s::crit_exts_c_::c1_c_, type_),
    offsetof(ue_radio_access_cap_info_s::crit_exts_c_::c1_c_, c),
    false};

static const char* enum_names_ue_radio_access_cap_info_s_crit_exts_c__c1_c__types_values[] =
    {"ueRadioAccessCapabilityInformation", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types_opts::desc = {
    enum_names_ue_radio_access_cap_info_s_crit_exts_c__c1_c__types_values,
    8,
    "ue_radio_access_cap_info_s::crit_exts_c_::c1_c_::types"};

static const char*           enum_names_ue_radio_access_cap_info_s_crit_exts_c__types_values[] = {"c1",
                                                                                                  "criticalExtensionsFuture"};
const asn1::table::enum_desc ue_radio_access_cap_info_s::crit_exts_c_::types_opts::desc        = {
    enum_names_ue_radio_access_cap_info_s_crit_exts_c__types_values,
    2,
    "ue_radio_access_cap_info_s::crit_exts_c_::types"};
uint8_t ue_radio_access_cap_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_radio_access_cap_info_s::crit_exts_c_::types");
}

// UERadioPagingInformation-v1840-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_radio_paging_info_v1840_ies_s_fields[] = {
    {0,
     (int32_t)offsetof(ue_radio_paging_info_v1840_ies_s, dl_sched_offset_pdsch_type_a_fdd_fr2_ntn_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_radio_paging_info_v1840_ies_s, dl_sched_offset_pdsch_type_b_fdd_fr2_ntn_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_radio_paging_info_v1840_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_radio_paging_info_v1840_ies_s_names[] = {
    {"dl-SchedulingOffset-PDSCH-TypeA-FDD-FR2-NTN-r18", "supported"},
    {"dl-SchedulingOffset-PDSCH-TypeB-FDD-FR2-NTN-r18", "supported"},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_radio_paging_info_v1840_ies_s_opts[] = {
    {0, (int32_t)offsetof(ue_radio_paging_info_v1840_ies_s, dl_sched_offset_pdsch_type_a_fdd_fr2_ntn_r18_present)},
    {1, (int32_t)offsetof(ue_radio_paging_info_v1840_ies_s, dl_sched_offset_pdsch_type_b_fdd_fr2_ntn_r18_present)},
    {2, (int32_t)offsetof(ue_radio_paging_info_v1840_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_radio_paging_info_v1840_ies_s::desc = {
    {seq_table_ue_radio_paging_info_v1840_ies_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_ue_radio_paging_info_v1840_ies_s_opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_radio_paging_info_v1840_ies_s_names};

// UERadioPagingInformation-v1800-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_radio_paging_info_v1800_ies_s_fields[] = {
    {offsetof(ue_radio_paging_info_v1800_ies_s, nof_rx_e_red_cap_r18),
     (int32_t)offsetof(ue_radio_paging_info_v1800_ies_s, nof_rx_e_red_cap_r18_present),
     asn1::table::enum_op<decltype(ue_radio_paging_info_v1800_ies_s::nof_rx_e_red_cap_r18)>(),
     decltype(ue_radio_paging_info_v1800_ies_s::nof_rx_e_red_cap_r18)::has_ext,
     false,
     decltype(ue_radio_paging_info_v1800_ies_s::nof_rx_e_red_cap_r18)::nof_types,
     decltype(ue_radio_paging_info_v1800_ies_s::nof_rx_e_red_cap_r18)::nof_exts,
     nullptr,
     &ue_radio_paging_info_v1800_ies_s::nof_rx_e_red_cap_r18_opts::desc},
    {0,
     (int32_t)offsetof(ue_radio_paging_info_v1800_ies_s, support_of2_rx_xr_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ue_radio_paging_info_v1800_ies_s, non_crit_ext),
     (int32_t)offsetof(ue_radio_paging_info_v1800_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_radio_paging_info_v1840_ies_s::desc}};
static const asn1::table::field_name seq_table_ue_radio_paging_info_v1800_ies_s_names[] = {
    {"numberOfRxERedCap-r18", nullptr},
    {"supportOf2RxXR-r18", "supported"},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_radio_paging_info_v1800_ies_s_opts[] = {
    {0, (int32_t)offsetof(ue_radio_paging_info_v1800_ies_s, nof_rx_e_red_cap_r18_present)},
    {1, (int32_t)offsetof(ue_radio_paging_info_v1800_ies_s, support_of2_rx_xr_r18_present)},
    {2, (int32_t)offsetof(ue_radio_paging_info_v1800_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_radio_paging_info_v1800_ies_s::desc = {
    {seq_table_ue_radio_paging_info_v1800_ies_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_ue_radio_paging_info_v1800_ies_s_opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_radio_paging_info_v1800_ies_s_names};

static const char* enum_names_ue_radio_paging_info_v1800_ies_s_nof_rx_e_red_cap_r18_e__values[] = {"one", "two"};
const asn1::table::enum_desc ue_radio_paging_info_v1800_ies_s::nof_rx_e_red_cap_r18_opts::desc  = {
    enum_names_ue_radio_paging_info_v1800_ies_s_nof_rx_e_red_cap_r18_e__values,
    2,
    "ue_radio_paging_info_v1800_ies_s::nof_rx_e_red_cap_r18_e_"};
uint8_t ue_radio_paging_info_v1800_ies_s::nof_rx_e_red_cap_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ue_radio_paging_info_v1800_ies_s::nof_rx_e_red_cap_r18_e_");
}

// UERadioPagingInformation-v1700-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_radio_paging_info_v1700_ies_s_fields[] = {
    {offsetof(ue_radio_paging_info_v1700_ies_s, ue_radio_paging_info_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(ue_radio_paging_info_v1700_ies_s::ue_radio_paging_info_r17),
                                      asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(ue_radio_paging_info_v1700_ies_s, inactive_state_po_determination_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ue_radio_paging_info_v1700_ies_s, nof_rx_red_cap_r17),
     (int32_t)offsetof(ue_radio_paging_info_v1700_ies_s, nof_rx_red_cap_r17_present),
     asn1::table::enum_op<decltype(ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17)>(),
     decltype(ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17)::has_ext,
     false,
     decltype(ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17)::nof_types,
     decltype(ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17)::nof_exts,
     nullptr,
     &ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_opts::desc},
    {offsetof(ue_radio_paging_info_v1700_ies_s, half_duplex_fdd_type_a_red_cap_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::list_codec<decltype(ue_radio_paging_info_v1700_ies_s::half_duplex_fdd_type_a_red_cap_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(1024), false, false>>::value},
    {offsetof(ue_radio_paging_info_v1700_ies_s, non_crit_ext),
     (int32_t)offsetof(ue_radio_paging_info_v1700_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_radio_paging_info_v1800_ies_s::desc}};
static const asn1::table::field_name seq_table_ue_radio_paging_info_v1700_ies_s_names[] = {
    {"ue-RadioPagingInfo-r17", nullptr},
    {"inactiveStatePO-Determination-r17", "supported"},
    {"numberOfRxRedCap-r17", nullptr},
    {"halfDuplexFDD-TypeA-RedCap-r17", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_radio_paging_info_v1700_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(ue_radio_paging_info_v1700_ies_s, inactive_state_po_determination_r17_present)},
    {2, (int32_t)offsetof(ue_radio_paging_info_v1700_ies_s, nof_rx_red_cap_r17_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(ue_radio_paging_info_v1700_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_radio_paging_info_v1700_ies_s::desc = {
    {seq_table_ue_radio_paging_info_v1700_ies_s_fields,
     5,
     UINT64_C(0x0),
     seq_table_ue_radio_paging_info_v1700_ies_s_opts,
     5},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_radio_paging_info_v1700_ies_s_names};

static const char* enum_names_ue_radio_paging_info_v1700_ies_s_nof_rx_red_cap_r17_e__values[] = {"one", "two"};
const asn1::table::enum_desc ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_opts::desc  = {
    enum_names_ue_radio_paging_info_v1700_ies_s_nof_rx_red_cap_r17_e__values,
    2,
    "ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_e_"};
uint8_t ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ue_radio_paging_info_v1700_ies_s::nof_rx_red_cap_r17_e_");
}

// UERadioPagingInformation-v15e0-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_radio_paging_info_v15e0_ies_s_fields[] = {
    {0,
     (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_a_fdd_fr1_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_a_tdd_fr1_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_a_tdd_fr2_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_b_fdd_fr1_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_b_tdd_fr1_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_b_tdd_fr2_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ue_radio_paging_info_v15e0_ies_s, non_crit_ext),
     (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_radio_paging_info_v1700_ies_s::desc}};
static const asn1::table::field_name seq_table_ue_radio_paging_info_v15e0_ies_s_names[] = {
    {"dl-SchedulingOffset-PDSCH-TypeA-FDD-FR1", "supported"},
    {"dl-SchedulingOffset-PDSCH-TypeA-TDD-FR1", "supported"},
    {"dl-SchedulingOffset-PDSCH-TypeA-TDD-FR2", "supported"},
    {"dl-SchedulingOffset-PDSCH-TypeB-FDD-FR1", "supported"},
    {"dl-SchedulingOffset-PDSCH-TypeB-TDD-FR1", "supported"},
    {"dl-SchedulingOffset-PDSCH-TypeB-TDD-FR2", "supported"},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_radio_paging_info_v15e0_ies_s_opts[] = {
    {0, (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_a_fdd_fr1_present)},
    {1, (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_a_tdd_fr1_present)},
    {2, (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_a_tdd_fr2_present)},
    {3, (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_b_fdd_fr1_present)},
    {4, (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_b_tdd_fr1_present)},
    {5, (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, dl_sched_offset_pdsch_type_b_tdd_fr2_present)},
    {6, (int32_t)offsetof(ue_radio_paging_info_v15e0_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_radio_paging_info_v15e0_ies_s::desc = {
    {seq_table_ue_radio_paging_info_v15e0_ies_s_fields,
     7,
     UINT64_C(0x0),
     seq_table_ue_radio_paging_info_v15e0_ies_s_opts,
     7},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_radio_paging_info_v15e0_ies_s_names};

// UERadioPagingInformation-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_radio_paging_info_ies_s_fields[] = {
    {offsetof(ue_radio_paging_info_ies_s, supported_band_list_nr_for_paging),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::list_codec<decltype(ue_radio_paging_info_ies_s::supported_band_list_nr_for_paging),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(1024), false, false>>::value},
    {offsetof(ue_radio_paging_info_ies_s, non_crit_ext),
     (int32_t)offsetof(ue_radio_paging_info_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_radio_paging_info_v15e0_ies_s::desc}};
static const asn1::table::field_name seq_table_ue_radio_paging_info_ies_s_names[] = {
    {"supportedBandListNRForPaging", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_radio_paging_info_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(ue_radio_paging_info_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_radio_paging_info_ies_s::desc = {
    {seq_table_ue_radio_paging_info_ies_s_fields, 2, UINT64_C(0x0), seq_table_ue_radio_paging_info_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_radio_paging_info_ies_s_names};

// UERadioPagingInformation ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_radio_paging_info_s_fields[] = {
    {offsetof(ue_radio_paging_info_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_radio_paging_info_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_ue_radio_paging_info_s_names[] = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          ue_radio_paging_info_s::desc             = {
    {seq_table_ue_radio_paging_info_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_radio_paging_info_s_names};

void ue_radio_paging_info_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_paging_info_s::crit_exts_c_::c1_c_& ue_radio_paging_info_s::crit_exts_c_::set_c1()
{
  set(types::c1);
  return c;
}
void ue_radio_paging_info_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_ue_radio_paging_info_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_radio_paging_info_s::crit_exts_c_::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ue_radio_paging_info_s_crit_exts_c__names[] = {
    {"c1", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc ue_radio_paging_info_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<ue_radio_paging_info_s::crit_exts_c_::types>(),
    ue_radio_paging_info_s::crit_exts_c_::types::nof_types,
    ue_radio_paging_info_s::crit_exts_c_::types::nof_exts,
    ue_radio_paging_info_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_ue_radio_paging_info_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<ue_radio_paging_info_s::crit_exts_c_>,
    choice_table_ue_radio_paging_info_s_crit_exts_c__names,
    "ue_radio_paging_info_s::crit_exts_c_",
    offsetof(ue_radio_paging_info_s::crit_exts_c_, type_),
    offsetof(ue_radio_paging_info_s::crit_exts_c_, c),
    false};

void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set(types::options e)
{
  type_ = e;
}
ue_radio_paging_info_ies_s& ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_ue_radio_paging_info()
{
  set(types::ue_radio_paging_info);
  return c;
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare7()
{
  set(types::spare7);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare6()
{
  set(types::spare6);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare5()
{
  set(types::spare5);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare4()
{
  set(types::spare4);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ue_radio_paging_info_s::crit_exts_c_::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_ue_radio_paging_info_s_crit_exts_c__c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_radio_paging_info_ies_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_ue_radio_paging_info_s_crit_exts_c__c1_c__names[] = {
    {"ueRadioPagingInformation", nullptr},
    {"spare7", nullptr},
    {"spare6", nullptr},
    {"spare5", nullptr},
    {"spare4", nullptr},
    {"spare3", nullptr},
    {"spare2", nullptr},
    {"spare1", nullptr}};
const asn1::table::choice_desc ue_radio_paging_info_s::crit_exts_c_::c1_c_::desc = {
    asn1::table::enum_int_type<ue_radio_paging_info_s::crit_exts_c_::c1_c_::types>(),
    ue_radio_paging_info_s::crit_exts_c_::c1_c_::types::nof_types,
    ue_radio_paging_info_s::crit_exts_c_::c1_c_::types::nof_exts,
    ue_radio_paging_info_s::crit_exts_c_::c1_c_::types::has_ext,
    false,
    choice_table_ue_radio_paging_info_s_crit_exts_c__c1_c__alternatives,
    &asn1::table::choice_set<ue_radio_paging_info_s::crit_exts_c_::c1_c_>,
    choice_table_ue_radio_paging_info_s_crit_exts_c__c1_c__names,
    "ue_radio_paging_info_s::crit_exts_c_::c1_c_",
    offsetof(ue_radio_paging_info_s::crit_exts_c_::c1_c_, type_),
    offsetof(ue_radio_paging_info_s::crit_exts_c_::c1_c_, c),
    false};

static const char* enum_names_ue_radio_paging_info_s_crit_exts_c__c1_c__types_values[] =
    {"ueRadioPagingInformation", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc ue_radio_paging_info_s::crit_exts_c_::c1_c_::types_opts::desc = {
    enum_names_ue_radio_paging_info_s_crit_exts_c__c1_c__types_values,
    8,
    "ue_radio_paging_info_s::crit_exts_c_::c1_c_::types"};

static const char* enum_names_ue_radio_paging_info_s_crit_exts_c__types_values[] = {"c1", "criticalExtensionsFuture"};
const asn1::table::enum_desc ue_radio_paging_info_s::crit_exts_c_::types_opts::desc = {
    enum_names_ue_radio_paging_info_s_crit_exts_c__types_values,
    2,
    "ue_radio_paging_info_s::crit_exts_c_::types"};
uint8_t ue_radio_paging_info_s::crit_exts_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ue_radio_paging_info_s::crit_exts_c_::types");
}

// PDCCH-BlindDetectionMixedList-r16 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_cg_ue_mixed_ext_r16_s__fields[] = {
        {offsetof(pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_cg_ue_mixed_ext_r16_s_,
                  pdcch_blind_detection_mcg_ue_mixed_v16a0),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &pdcch_blind_detection_cg_ue_mixed_ext_r16_s::desc},
        {offsetof(pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_cg_ue_mixed_ext_r16_s_,
                  pdcch_blind_detection_scg_ue_mixed_v16a0),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &pdcch_blind_detection_cg_ue_mixed_ext_r16_s::desc}};
static const asn1::table::field_name
    seq_table_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_cg_ue_mixed_ext_r16_s__names[] = {
        {"pdcch-BlindDetectionMCG-UE-Mixed-v16a0", nullptr},
        {"pdcch-BlindDetectionSCG-UE-Mixed-v16a0", nullptr}};
static const asn1::table::seq_desc
    seq_table_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_cg_ue_mixed_ext_r16_s_ = {
        {seq_table_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_cg_ue_mixed_ext_r16_s__fields,
         2,
         UINT64_C(0x3),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_cg_ue_mixed_ext_r16_s__names};
static const asn1::table::seq_field seq_table_pdcch_blind_detection_mixed_list_r16_s_fields[] = {
    {offsetof(pdcch_blind_detection_mixed_list_r16_s, pdcch_blind_detection_ca_mixed_ext_r16),
     (int32_t)offsetof(pdcch_blind_detection_mixed_list_r16_s, pdcch_blind_detection_ca_mixed_ext_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::desc},
    {offsetof(pdcch_blind_detection_mixed_list_r16_s, pdcch_blind_detection_cg_ue_mixed_ext_r16),
     (int32_t)offsetof(pdcch_blind_detection_mixed_list_r16_s, pdcch_blind_detection_cg_ue_mixed_ext_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_cg_ue_mixed_ext_r16_s_}};
static const asn1::table::field_name seq_table_pdcch_blind_detection_mixed_list_r16_s_names[] = {
    {"pdcch-BlindDetectionCA-MixedExt-r16", nullptr},
    {"pdcch-BlindDetectionCG-UE-MixedExt-r16", nullptr}};
static const asn1::table::seq_opt seq_table_pdcch_blind_detection_mixed_list_r16_s_opts[] = {
    {0, (int32_t)offsetof(pdcch_blind_detection_mixed_list_r16_s, pdcch_blind_detection_ca_mixed_ext_r16_present)},
    {1, (int32_t)offsetof(pdcch_blind_detection_mixed_list_r16_s, pdcch_blind_detection_cg_ue_mixed_ext_r16_present)}};
const asn1::table::seq_desc pdcch_blind_detection_mixed_list_r16_s::desc = {
    {seq_table_pdcch_blind_detection_mixed_list_r16_s_fields,
     2,
     UINT64_C(0x0),
     seq_table_pdcch_blind_detection_mixed_list_r16_s_opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_pdcch_blind_detection_mixed_list_r16_s_names};

static const asn1::table::storage_ops* const
    choice_storage_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_ca_mixed_ext_r16_c__alternatives[] = {
        &asn1::table::alternative_ops<pdcch_blind_detection_ca_mixed_ext_r16_s, false>::value,
        &asn1::table::alternative_ops<pdcch_blind_detection_ca_mixed_ext_r16_s, false>::value};
const asn1::table::choice_storage_desc
    pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::storage_desc = {
        choice_storage_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_ca_mixed_ext_r16_c__alternatives,
        2,
        "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_"};

pdcch_blind_detection_ca_mixed_ext_r16_s& pdcch_blind_detection_mixed_list_r16_s::
    pdcch_blind_detection_ca_mixed_ext_r16_c_::set_pdcch_blind_detection_ca_mixed_v16a0()
{
  set(types::pdcch_blind_detection_ca_mixed_v16a0);
  return c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>();
}
pdcch_blind_detection_ca_mixed_ext_r16_s& pdcch_blind_detection_mixed_list_r16_s::
    pdcch_blind_detection_ca_mixed_ext_r16_c_::set_pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0()
{
  set(types::pdcch_blind_detection_ca_mixed_non_aligned_span_v16a0);
  return c.get<pdcch_blind_detection_ca_mixed_ext_r16_s>();
}
static const asn1::table::seq_field
    choice_table_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_ca_mixed_ext_r16_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &pdcch_blind_detection_ca_mixed_ext_r16_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &pdcch_blind_detection_ca_mixed_ext_r16_s::desc}};
static const asn1::table::field_name
    choice_table_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_ca_mixed_ext_r16_c__names[] = {
        {"pdcch-BlindDetectionCA-Mixed-v16a0", nullptr},
        {"pdcch-BlindDetectionCA-Mixed-NonAlignedSpan-v16a0", nullptr}};
const asn1::table::choice_desc pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::desc =
    {asn1::table::enum_int_type<
         pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::types>(),
     pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::types::nof_types,
     pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::types::nof_exts,
     pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::types::has_ext,
     false,
     choice_table_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_ca_mixed_ext_r16_c__alternatives,
     nullptr,
     choice_table_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_ca_mixed_ext_r16_c__names,
     "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_",
     offsetof(pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_, c.type),
     offsetof(pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_, c.value),
     false,
     &pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::storage_desc};

static const char*
    enum_names_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_ca_mixed_ext_r16_c__types_values[] = {
        "pdcch-BlindDetectionCA-Mixed-v16a0",
        "pdcch-BlindDetectionCA-Mixed-NonAlignedSpan-v16a0"};
const asn1::table::enum_desc
    pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::types_opts::desc = {
        enum_names_pdcch_blind_detection_mixed_list_r16_s_pdcch_blind_detection_ca_mixed_ext_r16_c__types_values,
        2,
        "pdcch_blind_detection_mixed_list_r16_s::pdcch_blind_detection_ca_mixed_ext_r16_c_::types"};

// CA-ParametersNR-v16a0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ca_params_nr_v16a0_s_fields[] = {
    {offsetof(ca_params_nr_v16a0_s, pdcch_blind_detection_mixed_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(ca_params_nr_v16a0_s::pdcch_blind_detection_mixed_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pdcch_blind_detection_mixed_list_r16_s::desc}};
static const asn1::table::field_name seq_table_ca_params_nr_v16a0_s_names[] = {
    {"pdcch-BlindDetectionMixedList-r16", nullptr}};
const asn1::table::seq_desc ca_params_nr_v16a0_s::desc = {
    {seq_table_ca_params_nr_v16a0_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ca_params_nr_v16a0_s_names};

// CA-ParametersNRDC-v16a0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ca_params_nrdc_v16a0_s_fields[] = {
    {offsetof(ca_params_nrdc_v16a0_s, ca_params_nr_for_dc_v16a0),
     (int32_t)offsetof(ca_params_nrdc_v16a0_s, ca_params_nr_for_dc_v16a0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ca_params_nr_v16a0_s::desc}};
static const asn1::table::field_name seq_table_ca_params_nrdc_v16a0_s_names[] = {
    {"ca-ParametersNR-ForDC-v16a0", nullptr}};
static const asn1::table::seq_opt seq_table_ca_params_nrdc_v16a0_s_opts[] = {
    {0, (int32_t)offsetof(ca_params_nrdc_v16a0_s, ca_params_nr_for_dc_v16a0_present)}};
const asn1::table::seq_desc ca_params_nrdc_v16a0_s::desc = {
    {seq_table_ca_params_nrdc_v16a0_s_fields, 1, UINT64_C(0x0), seq_table_ca_params_nrdc_v16a0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_ca_params_nrdc_v16a0_s_names};

// BandCombination-v16a0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_band_combination_v16a0_s_fields[] = {
    {offsetof(band_combination_v16a0_s, ca_params_nr_v16a0),
     (int32_t)offsetof(band_combination_v16a0_s, ca_params_nr_v16a0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ca_params_nr_v16a0_s::desc},
    {offsetof(band_combination_v16a0_s, ca_params_nrdc_v16a0),
     (int32_t)offsetof(band_combination_v16a0_s, ca_params_nrdc_v16a0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ca_params_nrdc_v16a0_s::desc}};
static const asn1::table::field_name seq_table_band_combination_v16a0_s_names[] = {
    {"ca-ParametersNR-v16a0", nullptr},
    {"ca-ParametersNRDC-v16a0", nullptr}};
static const asn1::table::seq_opt seq_table_band_combination_v16a0_s_opts[] = {
    {0, (int32_t)offsetof(band_combination_v16a0_s, ca_params_nr_v16a0_present)},
    {1, (int32_t)offsetof(band_combination_v16a0_s, ca_params_nrdc_v16a0_present)}};
const asn1::table::seq_desc band_combination_v16a0_s::desc = {
    {seq_table_band_combination_v16a0_s_fields, 2, UINT64_C(0x0), seq_table_band_combination_v16a0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_band_combination_v16a0_s_names};

// BandCombination-UplinkTxSwitch-v16a0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_band_combination_ul_tx_switch_v16a0_s_fields[] = {
    {offsetof(band_combination_ul_tx_switch_v16a0_s, band_combination_v16a0),
     (int32_t)offsetof(band_combination_ul_tx_switch_v16a0_s, band_combination_v16a0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &band_combination_v16a0_s::desc}};
static const asn1::table::field_name seq_table_band_combination_ul_tx_switch_v16a0_s_names[] = {
    {"bandCombination-v16a0", nullptr}};
static const asn1::table::seq_opt seq_table_band_combination_ul_tx_switch_v16a0_s_opts[] = {
    {0, (int32_t)offsetof(band_combination_ul_tx_switch_v16a0_s, band_combination_v16a0_present)}};
const asn1::table::seq_desc band_combination_ul_tx_switch_v16a0_s::desc = {
    {seq_table_band_combination_ul_tx_switch_v16a0_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_band_combination_ul_tx_switch_v16a0_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_band_combination_ul_tx_switch_v16a0_s_names};

// CA-ParametersNRDC-v16j0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ca_params_nrdc_v16j0_s_fields[] = {
    {offsetof(ca_params_nrdc_v16j0_s, ca_params_nr_for_dc_v16j0),
     (int32_t)offsetof(ca_params_nrdc_v16j0_s, ca_params_nr_for_dc_v16j0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ca_params_nr_v1690_s::desc}};
static const asn1::table::field_name seq_table_ca_params_nrdc_v16j0_s_names[] = {
    {"ca-ParametersNR-ForDC-v16j0", nullptr}};
static const asn1::table::seq_opt seq_table_ca_params_nrdc_v16j0_s_opts[] = {
    {0, (int32_t)offsetof(ca_params_nrdc_v16j0_s, ca_params_nr_for_dc_v16j0_present)}};
const asn1::table::seq_desc ca_params_nrdc_v16j0_s::desc = {
    {seq_table_ca_params_nrdc_v16j0_s_fields, 1, UINT64_C(0x0), seq_table_ca_params_nrdc_v16j0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_ca_params_nrdc_v16j0_s_names};

// BandCombination-v16j0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_band_combination_v16j0_s_fields[] = {
    {offsetof(band_combination_v16j0_s, ca_params_nr_v16j0),
     (int32_t)offsetof(band_combination_v16j0_s, ca_params_nr_v16j0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ca_params_nr_v1690_s::desc},
    {offsetof(band_combination_v16j0_s, ca_params_nrdc_v16j0),
     (int32_t)offsetof(band_combination_v16j0_s, ca_params_nrdc_v16j0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ca_params_nrdc_v16j0_s::desc}};
static const asn1::table::field_name seq_table_band_combination_v16j0_s_names[] = {
    {"ca-ParametersNR-v16j0", nullptr},
    {"ca-ParametersNRDC-v16j0", nullptr}};
static const asn1::table::seq_opt seq_table_band_combination_v16j0_s_opts[] = {
    {0, (int32_t)offsetof(band_combination_v16j0_s, ca_params_nr_v16j0_present)},
    {1, (int32_t)offsetof(band_combination_v16j0_s, ca_params_nrdc_v16j0_present)}};
const asn1::table::seq_desc band_combination_v16j0_s::desc = {
    {seq_table_band_combination_v16j0_s_fields, 2, UINT64_C(0x0), seq_table_band_combination_v16j0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_band_combination_v16j0_s_names};

// BandCombination-UplinkTxSwitch-v16j0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_band_combination_ul_tx_switch_v16j0_s_fields[] = {
    {offsetof(band_combination_ul_tx_switch_v16j0_s, band_combination_v16j0),
     (int32_t)offsetof(band_combination_ul_tx_switch_v16j0_s, band_combination_v16j0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &band_combination_v16j0_s::desc}};
static const asn1::table::field_name seq_table_band_combination_ul_tx_switch_v16j0_s_names[] = {
    {"bandCombination-v16j0", nullptr}};
static const asn1::table::seq_opt seq_table_band_combination_ul_tx_switch_v16j0_s_opts[] = {
    {0, (int32_t)offsetof(band_combination_ul_tx_switch_v16j0_s, band_combination_v16j0_present)}};
const asn1::table::seq_desc band_combination_ul_tx_switch_v16j0_s::desc = {
    {seq_table_band_combination_ul_tx_switch_v16j0_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_band_combination_ul_tx_switch_v16j0_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_band_combination_ul_tx_switch_v16j0_s_names};

// CA-ParametersNRDC-v17b0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ca_params_nrdc_v17b0_s_fields[] = {
    {offsetof(ca_params_nrdc_v17b0_s, ca_params_nr_for_dc_v17b0),
     (int32_t)offsetof(ca_params_nrdc_v17b0_s, ca_params_nr_for_dc_v17b0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ca_params_nr_v1740_s::desc}};
static const asn1::table::field_name seq_table_ca_params_nrdc_v17b0_s_names[] = {
    {"ca-ParametersNR-ForDC-v17b0", nullptr}};
static const asn1::table::seq_opt seq_table_ca_params_nrdc_v17b0_s_opts[] = {
    {0, (int32_t)offsetof(ca_params_nrdc_v17b0_s, ca_params_nr_for_dc_v17b0_present)}};
const asn1::table::seq_desc ca_params_nrdc_v17b0_s::desc = {
    {seq_table_ca_params_nrdc_v17b0_s_fields, 1, UINT64_C(0x0), seq_table_ca_params_nrdc_v17b0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_ca_params_nrdc_v17b0_s_names};

// BandCombination-v17b0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_band_combination_v17b0_s_fields[] = {
    {offsetof(band_combination_v17b0_s, ca_params_nr_v17b0),
     (int32_t)offsetof(band_combination_v17b0_s, ca_params_nr_v17b0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ca_params_nr_v1740_s::desc},
    {offsetof(band_combination_v17b0_s, ca_params_nrdc_v17b0),
     (int32_t)offsetof(band_combination_v17b0_s, ca_params_nrdc_v17b0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ca_params_nrdc_v17b0_s::desc}};
static const asn1::table::field_name seq_table_band_combination_v17b0_s_names[] = {
    {"ca-ParametersNR-v17b0", nullptr},
    {"ca-ParametersNRDC-v17b0", nullptr}};
static const asn1::table::seq_opt seq_table_band_combination_v17b0_s_opts[] = {
    {0, (int32_t)offsetof(band_combination_v17b0_s, ca_params_nr_v17b0_present)},
    {1, (int32_t)offsetof(band_combination_v17b0_s, ca_params_nrdc_v17b0_present)}};
const asn1::table::seq_desc band_combination_v17b0_s::desc = {
    {seq_table_band_combination_v17b0_s_fields, 2, UINT64_C(0x0), seq_table_band_combination_v17b0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_band_combination_v17b0_s_names};

// BandCombination-UplinkTxSwitch-v17b0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_band_combination_ul_tx_switch_v17b0_s_fields[] = {
    {offsetof(band_combination_ul_tx_switch_v17b0_s, band_combination_v17b0),
     (int32_t)offsetof(band_combination_ul_tx_switch_v17b0_s, band_combination_v17b0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &band_combination_v17b0_s::desc}};
static const asn1::table::field_name seq_table_band_combination_ul_tx_switch_v17b0_s_names[] = {
    {"bandCombination-v17b0", nullptr}};
static const asn1::table::seq_opt seq_table_band_combination_ul_tx_switch_v17b0_s_opts[] = {
    {0, (int32_t)offsetof(band_combination_ul_tx_switch_v17b0_s, band_combination_v17b0_present)}};
const asn1::table::seq_desc band_combination_ul_tx_switch_v17b0_s::desc = {
    {seq_table_band_combination_ul_tx_switch_v17b0_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_band_combination_ul_tx_switch_v17b0_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_band_combination_ul_tx_switch_v17b0_s_names};

// BandNR-v16c0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_band_nr_v16c0_s_fields[] = {
    {0,
     (int32_t)offsetof(band_nr_v16c0_s, pusch_repeat_type_a_v16c0_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_band_nr_v16c0_s_names[] = {{"pusch-RepetitionTypeA-v16c0", "supported"}};
static const asn1::table::seq_opt    seq_table_band_nr_v16c0_s_opts[]  = {
    {0, (int32_t)offsetof(band_nr_v16c0_s, pusch_repeat_type_a_v16c0_present)}};
const asn1::table::seq_desc band_nr_v16c0_s::desc = {
    {seq_table_band_nr_v16c0_s_fields, 1, UINT64_C(0x0), seq_table_band_nr_v16c0_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_band_nr_v16c0_s_names};

// MIMO-ParametersPerBand-v17b0 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__fields[] = {
        {offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_,
                  min_beam_application_time_joint_tci_v17b0),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
             min_beam_application_time_joint_tci_v17b0_c_::desc},
        {offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_, max_num_mac_ce_per_cc_v17b0),
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                           max_num_mac_ce_per_cc_v17b0)>(),
         decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::max_num_mac_ce_per_cc_v17b0)::
             has_ext,
         false,
         decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::max_num_mac_ce_per_cc_v17b0)::
             nof_types,
         decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::max_num_mac_ce_per_cc_v17b0)::
             nof_exts,
         nullptr,
         &mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::max_num_mac_ce_per_cc_v17b0_opts::
             desc}};
static const asn1::table::field_name
    seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__names[] = {
        {"minBeamApplicationTimeJointTCI-v17b0", nullptr},
        {"maxNumMAC-CE-PerCC-v17b0", nullptr}};
static const asn1::table::seq_desc seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s_ = {
    {seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__fields,
     2,
     UINT64_C(0x3),
     nullptr,
     0},
    false,
    false,
    nullptr,
    0,
    seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__names};
static const asn1::table::seq_field
    seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__fields[] = {
        {offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_,
                  min_beam_application_time_separate_tci_v17b0),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
             min_beam_application_time_separate_tci_v17b0_c_::desc},
        {offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_,
                  max_activ_dl_tci_per_cc_v17b0),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(2),
         UINT64_C(8),
         nullptr},
        {offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_,
                  max_activ_ul_tci_per_cc_v17b0),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(2),
         UINT64_C(8),
         nullptr}};
static const asn1::table::field_name
    seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__names[] = {
        {"minBeamApplicationTimeSeparateTCI-v17b0", nullptr},
        {"maxActivatedDL-TCIPerCC-v17b0", nullptr},
        {"maxActivatedUL-TCIPerCC-v17b0", nullptr}};
static const asn1::table::seq_desc seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s_ = {
    {seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__fields,
     3,
     UINT64_C(0x7),
     nullptr,
     0},
    false,
    false,
    nullptr,
    0,
    seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__names};
static const asn1::table::seq_field seq_table_mimo_params_per_band_v17b0_s_fields[] = {
    {offsetof(mimo_params_per_band_v17b0_s, unified_joint_tci_multi_mac_ce_v17b0),
     (int32_t)offsetof(mimo_params_per_band_v17b0_s, unified_joint_tci_multi_mac_ce_v17b0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s_},
    {offsetof(mimo_params_per_band_v17b0_s, unified_separate_tci_multi_mac_ce_v17b0),
     (int32_t)offsetof(mimo_params_per_band_v17b0_s, unified_separate_tci_multi_mac_ce_v17b0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s_}};
static const asn1::table::field_name seq_table_mimo_params_per_band_v17b0_s_names[] = {
    {"unifiedJointTCI-multiMAC-CE-v17b0", nullptr},
    {"unifiedSeparateTCI-multiMAC-CE-v17b0", nullptr}};
static const asn1::table::seq_opt seq_table_mimo_params_per_band_v17b0_s_opts[] = {
    {0, (int32_t)offsetof(mimo_params_per_band_v17b0_s, unified_joint_tci_multi_mac_ce_v17b0_present)},
    {1, (int32_t)offsetof(mimo_params_per_band_v17b0_s, unified_separate_tci_multi_mac_ce_v17b0_present)}};
const asn1::table::seq_desc mimo_params_per_band_v17b0_s::desc = {
    {seq_table_mimo_params_per_band_v17b0_s_fields, 2, UINT64_C(0x0), seq_table_mimo_params_per_band_v17b0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_mimo_params_per_band_v17b0_s_names};

static const asn1::table::storage_ops* const
    choice_storage_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__alternatives
        [] = {&asn1::table::alternative_ops<mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                                min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_,
                                            false>::value,
              &asn1::table::alternative_ops<mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                                min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_,
                                            false>::value};
const asn1::table::choice_storage_desc mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::storage_desc = {
        choice_storage_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__alternatives,
        2,
        "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_joint_tci_"
        "v17b0_c_"};

mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_joint_tci_v17b0_c_::
    fr1_v17b0_s_&
    mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
        min_beam_application_time_joint_tci_v17b0_c_::set_fr1_v17b0()
{
  set(types::fr1_v17b0);
  return c.get<fr1_v17b0_s_>();
}
mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_joint_tci_v17b0_c_::
    fr2_v17b0_s_&
    mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
        min_beam_application_time_joint_tci_v17b0_c_::set_fr2_v17b0()
{
  set(types::fr2_v17b0);
  return c.get<fr2_v17b0_s_>();
}
static const asn1::table::seq_field
    seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__fields
        [] = {{offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_,
                        scs_15k_hz_v17b0),
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_15k_hz_v17b0_present),
               asn1::table::enum_op<
                   decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0)>(),
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0)::has_ext,
               false,
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0)::nof_types,
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0)::nof_exts,
               nullptr,
               &mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                   min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0_opts::desc},
              {offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_,
                        scs_30k_hz_v17b0),
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_30k_hz_v17b0_present),
               asn1::table::enum_op<
                   decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0)>(),
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0)::has_ext,
               false,
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0)::nof_types,
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0)::nof_exts,
               nullptr,
               &mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                   min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0_opts::desc},
              {offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_,
                        scs_60k_hz_v17b0),
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_60k_hz_v17b0_present),
               asn1::table::enum_op<
                   decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0)>(),
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0)::has_ext,
               false,
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0)::nof_types,
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0)::nof_exts,
               nullptr,
               &mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                   min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0_opts::desc}};
static const asn1::table::field_name
    seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__names
        [] = {{"scs-15kHz-v17b0", nullptr}, {"scs-30kHz-v17b0", nullptr}, {"scs-60kHz-v17b0", nullptr}};
static const asn1::table::seq_opt
    seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__opts
        [] = {{0,
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_15k_hz_v17b0_present)},
              {1,
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_30k_hz_v17b0_present)},
              {2,
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_60k_hz_v17b0_present)}};
static const asn1::table::seq_desc
    seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s_ =
        {{seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__fields,
          3,
          UINT64_C(0x0),
          seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__opts,
          3},
         false,
         false,
         nullptr,
         0,
         seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__names};
static const asn1::table::seq_field
    seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s__fields
        [] = {{offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_,
                        scs_60k_hz_v17b0),
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_,
                                 scs_60k_hz_v17b0_present),
               asn1::table::enum_op<
                   decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0)>(),
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0)::has_ext,
               false,
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0)::nof_types,
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0)::nof_exts,
               nullptr,
               &mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                   min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0_opts::desc},
              {offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_,
                        scs_120k_hz_v17b0),
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_,
                                 scs_120k_hz_v17b0_present),
               asn1::table::enum_op<
                   decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0)>(),
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0)::has_ext,
               false,
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0)::nof_types,
               decltype(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0)::nof_exts,
               nullptr,
               &mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                   min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0_opts::desc}};
static const asn1::table::field_name
    seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s__names
        [] = {{"scs-60kHz-v17b0", nullptr}, {"scs-120kHz-v17b0", nullptr}};
static const asn1::table::seq_opt
    seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s__opts
        [] = {{0,
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_,
                                 scs_60k_hz_v17b0_present)},
              {1,
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_,
                                 scs_120k_hz_v17b0_present)}};
static const asn1::table::seq_desc
    seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s_ =
        {{seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s__fields,
          2,
          UINT64_C(0x0),
          seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s__opts,
          2},
         false,
         false,
         nullptr,
         0,
         seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s__names};
static const asn1::table::seq_field
    choice_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__alternatives
        [] = {
            {0,
             asn1::table::seq_field::mandatory,
             asn1::table::field_op::sequence,
             false,
             false,
             0,
             0,
             nullptr,
             &seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s_},
            {0,
             asn1::table::seq_field::mandatory,
             asn1::table::field_op::sequence,
             false,
             false,
             0,
             0,
             nullptr,
             &seq_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s_}};
static const asn1::table::field_name
    choice_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__names
        [] = {{"fr1-v17b0", nullptr}, {"fr2-v17b0", nullptr}};
const asn1::table::choice_desc mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::desc = {
        asn1::table::enum_int_type<mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                                       min_beam_application_time_joint_tci_v17b0_c_::types>(),
        mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
            min_beam_application_time_joint_tci_v17b0_c_::types::nof_types,
        mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
            min_beam_application_time_joint_tci_v17b0_c_::types::nof_exts,
        mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
            min_beam_application_time_joint_tci_v17b0_c_::types::has_ext,
        false,
        choice_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__alternatives,
        nullptr,
        choice_table_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__names,
        "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_joint_tci_"
        "v17b0_c_",
        offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                     min_beam_application_time_joint_tci_v17b0_c_,
                 c.type),
        offsetof(mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
                     min_beam_application_time_joint_tci_v17b0_c_,
                 c.value),
        false,
        &mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
            min_beam_application_time_joint_tci_v17b0_c_::storage_desc};

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__scs_15k_hz_v17b0_e__values
        [] = {"sym1", "sym2", "sym4", "sym7", "sym14", "sym28", "sym42", "sym56", "sym70"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__scs_15k_hz_v17b0_e__values,
        9,
        "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_joint_tci_"
        "v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0_e_"};
uint8_t mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 7, 14, 28, 42, 56, 70};
  return map_enum_number(numbers,
                         9,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_"
                         "time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0_e_");
}

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__scs_30k_hz_v17b0_e__values
        [] = {"sym1", "sym2", "sym4", "sym7", "sym14", "sym28", "sym42", "sym56", "sym70"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__scs_30k_hz_v17b0_e__values,
        9,
        "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_joint_tci_"
        "v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0_e_"};
uint8_t mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 7, 14, 28, 42, 56, 70};
  return map_enum_number(numbers,
                         9,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_"
                         "time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0_e_");
}

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__scs_60k_hz_v17b0_e__values
        [] = {"sym1", "sym2", "sym4", "sym7", "sym14", "sym28", "sym42", "sym56", "sym70"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr1_v17b0_s__scs_60k_hz_v17b0_e__values,
        9,
        "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_joint_tci_"
        "v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0_e_"};
uint8_t mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 7, 14, 28, 42, 56, 70};
  return map_enum_number(numbers,
                         9,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_"
                         "time_joint_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0_e_");
}

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s__scs_60k_hz_v17b0_e__values
        [] = {"sym1",
              "sym2",
              "sym4",
              "sym7",
              "sym14",
              "sym28",
              "sym42",
              "sym56",
              "sym70",
              "sym84",
              "sym98",
              "sym112",
              "sym224",
              "sym336"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s__scs_60k_hz_v17b0_e__values,
        14,
        "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_joint_tci_"
        "v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0_e_"};
uint16_t mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 7, 14, 28, 42, 56, 70, 84, 98, 112, 224, 336};
  return map_enum_number(numbers,
                         14,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_"
                         "time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0_e_");
}

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s__scs_120k_hz_v17b0_e__values
        [] = {"sym1",
              "sym2",
              "sym4",
              "sym7",
              "sym14",
              "sym28",
              "sym42",
              "sym56",
              "sym70",
              "sym84",
              "sym98",
              "sym112",
              "sym224",
              "sym336"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__fr2_v17b0_s__scs_120k_hz_v17b0_e__values,
        14,
        "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_joint_tci_"
        "v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0_e_"};
uint16_t mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 7, 14, 28, 42, 56, 70, 84, 98, 112, 224, 336};
  return map_enum_number(numbers,
                         14,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_"
                         "time_joint_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0_e_");
}

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__types_values
        [] = {"fr1-v17b0", "fr2-v17b0"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::types_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__min_beam_application_time_joint_tci_v17b0_c__types_values,
        2,
        "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_joint_tci_"
        "v17b0_c_::types"};
uint8_t mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_joint_tci_v17b0_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers,
                         2,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::min_beam_application_"
                         "time_joint_tci_v17b0_c_::types");
}

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__max_num_mac_ce_per_cc_v17b0_e__values
        [] = {"n2", "n3", "n4", "n5", "n6", "n7", "n8"};
const asn1::table::enum_desc
    mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::max_num_mac_ce_per_cc_v17b0_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_joint_tci_multi_mac_ce_v17b0_s__max_num_mac_ce_per_cc_v17b0_e__values,
        7,
        "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::max_num_mac_ce_per_cc_v17b0_e_"};
uint8_t
mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::max_num_mac_ce_per_cc_v17b0_opts::to_number()
    const
{
  static const uint8_t numbers[] = {2, 3, 4, 5, 6, 7, 8};
  return map_enum_number(
      numbers,
      7,
      value,
      "mimo_params_per_band_v17b0_s::unified_joint_tci_multi_mac_ce_v17b0_s_::max_num_mac_ce_per_cc_v17b0_e_");
}

static const asn1::table::storage_ops* const
    choice_storage_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__alternatives
        [] = {&asn1::table::alternative_ops<mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                                min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_,
                                            false>::value,
              &asn1::table::alternative_ops<mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                                min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_,
                                            false>::value};
const asn1::table::choice_storage_desc mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::storage_desc = {
        choice_storage_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__alternatives,
        2,
        "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_separate_"
        "tci_v17b0_c_"};

mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_&
    mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
        min_beam_application_time_separate_tci_v17b0_c_::set_fr1_v17b0()
{
  set(types::fr1_v17b0);
  return c.get<fr1_v17b0_s_>();
}
mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_&
    mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
        min_beam_application_time_separate_tci_v17b0_c_::set_fr2_v17b0()
{
  set(types::fr2_v17b0);
  return c.get<fr2_v17b0_s_>();
}
static const asn1::table::seq_field
    seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__fields
        [] = {{offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_,
                        scs_15k_hz_v17b0),
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_15k_hz_v17b0_present),
               asn1::table::enum_op<
                   decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0)>(),
               decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0)::has_ext,
               false,
               decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0)::nof_types,
               decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0)::nof_exts,
               nullptr,
               &mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                   min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0_opts::desc},
              {offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_,
                        scs_30k_hz_v17b0),
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_30k_hz_v17b0_present),
               asn1::table::enum_op<
                   decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0)>(),
               decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0)::has_ext,
               false,
               decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0)::nof_types,
               decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0)::nof_exts,
               nullptr,
               &mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                   min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0_opts::desc},
              {offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_,
                        scs_60k_hz_v17b0),
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_60k_hz_v17b0_present),
               asn1::table::enum_op<
                   decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0)>(),
               decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0)::has_ext,
               false,
               decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0)::nof_types,
               decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                            min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0)::nof_exts,
               nullptr,
               &mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                   min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0_opts::desc}};
static const asn1::table::field_name
    seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__names
        [] = {{"scs-15kHz-v17b0", nullptr}, {"scs-30kHz-v17b0", nullptr}, {"scs-60kHz-v17b0", nullptr}};
static const asn1::table::seq_opt
    seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__opts
        [] = {{0,
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_15k_hz_v17b0_present)},
              {1,
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_30k_hz_v17b0_present)},
              {2,
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_,
                                 scs_60k_hz_v17b0_present)}};
static const asn1::table::seq_desc
    seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s_ =
        {{seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__fields,
          3,
          UINT64_C(0x0),
          seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__opts,
          3},
         false,
         false,
         nullptr,
         0,
         seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__names};
static const asn1::table::seq_field
    seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s__fields
        [] = {
            {offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                          min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_,
                      scs_60k_hz_v17b0),
             (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                   min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_,
                               scs_60k_hz_v17b0_present),
             asn1::table::enum_op<
                 decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                              min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0)>(),
             decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                          min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0)::has_ext,
             false,
             decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                          min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0)::nof_types,
             decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                          min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0)::nof_exts,
             nullptr,
             &mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                 min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0_opts::desc},
            {offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                          min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_,
                      scs_120k_hz_v17b0),
             (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                   min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_,
                               scs_120k_hz_v17b0_present),
             asn1::table::enum_op<
                 decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                              min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0)>(),
             decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                          min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0)::has_ext,
             false,
             decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                          min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0)::nof_types,
             decltype(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                          min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0)::nof_exts,
             nullptr,
             &mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                 min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0_opts::desc}};
static const asn1::table::field_name
    seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s__names
        [] = {{"scs-60kHz-v17b0", nullptr}, {"scs-120kHz-v17b0", nullptr}};
static const asn1::table::seq_opt
    seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s__opts
        [] = {{0,
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_,
                                 scs_60k_hz_v17b0_present)},
              {1,
               (int32_t)offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                     min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_,
                                 scs_120k_hz_v17b0_present)}};
static const asn1::table::seq_desc
    seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s_ =
        {{seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s__fields,
          2,
          UINT64_C(0x0),
          seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s__opts,
          2},
         false,
         false,
         nullptr,
         0,
         seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s__names};
static const asn1::table::seq_field
    choice_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__alternatives
        [] = {
            {0,
             asn1::table::seq_field::mandatory,
             asn1::table::field_op::sequence,
             false,
             false,
             0,
             0,
             nullptr,
             &seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s_},
            {0,
             asn1::table::seq_field::mandatory,
             asn1::table::field_op::sequence,
             false,
             false,
             0,
             0,
             nullptr,
             &seq_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s_}};
static const asn1::table::field_name
    choice_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__names
        [] = {{"fr1-v17b0", nullptr}, {"fr2-v17b0", nullptr}};
const asn1::table::choice_desc mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::desc = {
        asn1::table::enum_int_type<mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                                       min_beam_application_time_separate_tci_v17b0_c_::types>(),
        mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
            min_beam_application_time_separate_tci_v17b0_c_::types::nof_types,
        mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
            min_beam_application_time_separate_tci_v17b0_c_::types::nof_exts,
        mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
            min_beam_application_time_separate_tci_v17b0_c_::types::has_ext,
        false,
        choice_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__alternatives,
        nullptr,
        choice_table_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__names,
        "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_separate_"
        "tci_v17b0_c_",
        offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                     min_beam_application_time_separate_tci_v17b0_c_,
                 c.type),
        offsetof(mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
                     min_beam_application_time_separate_tci_v17b0_c_,
                 c.value),
        false,
        &mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
            min_beam_application_time_separate_tci_v17b0_c_::storage_desc};

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__scs_15k_hz_v17b0_e__values
        [] = {"sym1", "sym2", "sym4", "sym7", "sym14", "sym28", "sym42", "sym56", "sym70"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__scs_15k_hz_v17b0_e__values,
        9,
        "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_separate_"
        "tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0_e_"};
uint8_t mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 7, 14, 28, 42, 56, 70};
  return map_enum_number(numbers,
                         9,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_"
                         "application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_15k_hz_v17b0_e_");
}

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__scs_30k_hz_v17b0_e__values
        [] = {"sym1", "sym2", "sym4", "sym7", "sym14", "sym28", "sym42", "sym56", "sym70"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__scs_30k_hz_v17b0_e__values,
        9,
        "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_separate_"
        "tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0_e_"};
uint8_t mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 7, 14, 28, 42, 56, 70};
  return map_enum_number(numbers,
                         9,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_"
                         "application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_30k_hz_v17b0_e_");
}

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__scs_60k_hz_v17b0_e__values
        [] = {"sym1", "sym2", "sym4", "sym7", "sym14", "sym28", "sym42", "sym56", "sym70"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr1_v17b0_s__scs_60k_hz_v17b0_e__values,
        9,
        "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_separate_"
        "tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0_e_"};
uint8_t mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 7, 14, 28, 42, 56, 70};
  return map_enum_number(numbers,
                         9,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_"
                         "application_time_separate_tci_v17b0_c_::fr1_v17b0_s_::scs_60k_hz_v17b0_e_");
}

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s__scs_60k_hz_v17b0_e__values
        [] = {"sym1",
              "sym2",
              "sym4",
              "sym7",
              "sym14",
              "sym28",
              "sym42",
              "sym56",
              "sym70",
              "sym84",
              "sym98",
              "sym112",
              "sym224",
              "sym336"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s__scs_60k_hz_v17b0_e__values,
        14,
        "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_separate_"
        "tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0_e_"};
uint16_t mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 7, 14, 28, 42, 56, 70, 84, 98, 112, 224, 336};
  return map_enum_number(numbers,
                         14,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_"
                         "application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_60k_hz_v17b0_e_");
}

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s__scs_120k_hz_v17b0_e__values
        [] = {"sym1",
              "sym2",
              "sym4",
              "sym7",
              "sym14",
              "sym28",
              "sym42",
              "sym56",
              "sym70",
              "sym84",
              "sym98",
              "sym112",
              "sym224",
              "sym336"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__fr2_v17b0_s__scs_120k_hz_v17b0_e__values,
        14,
        "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_separate_"
        "tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0_e_"};
uint16_t mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 7, 14, 28, 42, 56, 70, 84, 98, 112, 224, 336};
  return map_enum_number(numbers,
                         14,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_"
                         "application_time_separate_tci_v17b0_c_::fr2_v17b0_s_::scs_120k_hz_v17b0_e_");
}

static const char*
    enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__types_values
        [] = {"fr1-v17b0", "fr2-v17b0"};
const asn1::table::enum_desc mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::types_opts::desc = {
        enum_names_mimo_params_per_band_v17b0_s_unified_separate_tci_multi_mac_ce_v17b0_s__min_beam_application_time_separate_tci_v17b0_c__types_values,
        2,
        "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_application_time_separate_"
        "tci_v17b0_c_::types"};
uint8_t mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::
    min_beam_application_time_separate_tci_v17b0_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers,
                         2,
                         value,
                         "mimo_params_per_band_v17b0_s::unified_separate_tci_multi_mac_ce_v17b0_s_::min_beam_"
                         "application_time_separate_tci_v17b0_c_::types");
}

// BandNR-v17b0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_band_nr_v17b0_s_fields[] = {
    {offsetof(band_nr_v17b0_s, mimo_params_per_band_v17b0),
     (int32_t)offsetof(band_nr_v17b0_s, mimo_params_per_band_v17b0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mimo_params_per_band_v17b0_s::desc}};
static const asn1::table::field_name seq_table_band_nr_v17b0_s_names[] = {{"mimo-ParametersPerBand-v17b0", nullptr}};
static const asn1::table::seq_opt    seq_table_band_nr_v17b0_s_opts[]  = {
    {0, (int32_t)offsetof(band_nr_v17b0_s, mimo_params_per_band_v17b0_present)}};
const asn1::table::seq_desc band_nr_v17b0_s::desc = {
    {seq_table_band_nr_v17b0_s_fields, 1, UINT64_C(0x0), seq_table_band_nr_v17b0_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_band_nr_v17b0_s_names};

// CG-SDT-ConfigLCH-Restriction-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_sdt_cfg_lch_restrict_r17_s_fields[] = {
    {offsetof(cg_sdt_cfg_lch_restrict_r17_s, lc_ch_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {0,
     (int32_t)offsetof(cg_sdt_cfg_lch_restrict_r17_s, cfg_grant_type1_allowed_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(cg_sdt_cfg_lch_restrict_r17_s, allowed_cg_list_r17),
     (int32_t)offsetof(cg_sdt_cfg_lch_restrict_r17_s, allowed_cg_list_r17_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     31,
     &asn1::table::list_codec<decltype(cg_sdt_cfg_lch_restrict_r17_s::allowed_cg_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(31), false, false>>::value}};
static const asn1::table::field_name seq_table_cg_sdt_cfg_lch_restrict_r17_s_names[] = {
    {"logicalChannelIdentity-r17", nullptr},
    {"configuredGrantType1Allowed-r17", "true"},
    {"allowedCG-List-r17", nullptr}};
static const asn1::table::seq_opt seq_table_cg_sdt_cfg_lch_restrict_r17_s_opts[] = {
    {1, (int32_t)offsetof(cg_sdt_cfg_lch_restrict_r17_s, cfg_grant_type1_allowed_r17_present)},
    {2, (int32_t)offsetof(cg_sdt_cfg_lch_restrict_r17_s, allowed_cg_list_r17_present)}};
const asn1::table::seq_desc cg_sdt_cfg_lch_restrict_r17_s::desc = {
    {seq_table_cg_sdt_cfg_lch_restrict_r17_s_fields, 3, UINT64_C(0x1), seq_table_cg_sdt_cfg_lch_restrict_r17_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_sdt_cfg_lch_restrict_r17_s_names};

// CG-SDT-ConfigLCH-RestrictionExt-v1800 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_sdt_cfg_lch_restrict_ext_v1800_s_fields[] = {
    {offsetof(cg_sdt_cfg_lch_restrict_ext_v1800_s, cg_sdt_max_dur_to_next_cg_occasion_r18),
     (int32_t)offsetof(cg_sdt_cfg_lch_restrict_ext_v1800_s, cg_sdt_max_dur_to_next_cg_occasion_r18_present),
     asn1::table::enum_op<decltype(cg_sdt_cfg_lch_restrict_ext_v1800_s::cg_sdt_max_dur_to_next_cg_occasion_r18)>(),
     decltype(cg_sdt_cfg_lch_restrict_ext_v1800_s::cg_sdt_max_dur_to_next_cg_occasion_r18)::has_ext,
     false,
     decltype(cg_sdt_cfg_lch_restrict_ext_v1800_s::cg_sdt_max_dur_to_next_cg_occasion_r18)::nof_types,
     decltype(cg_sdt_cfg_lch_restrict_ext_v1800_s::cg_sdt_max_dur_to_next_cg_occasion_r18)::nof_exts,
     nullptr,
     &cg_sdt_cfg_lch_restrict_ext_v1800_s::cg_sdt_max_dur_to_next_cg_occasion_r18_opts::desc}};
static const asn1::table::field_name seq_table_cg_sdt_cfg_lch_restrict_ext_v1800_s_names[] = {
    {"cg-SDT-MaxDurationToNextCG-Occasion-r18", nullptr}};
static const asn1::table::seq_opt seq_table_cg_sdt_cfg_lch_restrict_ext_v1800_s_opts[] = {
    {0, (int32_t)offsetof(cg_sdt_cfg_lch_restrict_ext_v1800_s, cg_sdt_max_dur_to_next_cg_occasion_r18_present)}};
const asn1::table::seq_desc cg_sdt_cfg_lch_restrict_ext_v1800_s::desc = {
    {seq_table_cg_sdt_cfg_lch_restrict_ext_v1800_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_cg_sdt_cfg_lch_restrict_ext_v1800_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_sdt_cfg_lch_restrict_ext_v1800_s_names};

static const char* enum_names_cg_sdt_cfg_lch_restrict_ext_v1800_s_cg_sdt_max_dur_to_next_cg_occasion_r18_e__values[] = {
    "ms10",
    "ms100",
    "sec1",
    "sec10",
    "sec60",
    "sec100",
    "sec300",
    "sec600",
    "sec1200",
    "sec1800",
    "sec3600",
    "spare5",
    "spare4",
    "spare3",
    "spare2",
    "spare1"};
const asn1::table::enum_desc cg_sdt_cfg_lch_restrict_ext_v1800_s::cg_sdt_max_dur_to_next_cg_occasion_r18_opts::desc = {
    enum_names_cg_sdt_cfg_lch_restrict_ext_v1800_s_cg_sdt_max_dur_to_next_cg_occasion_r18_e__values,
    16,
    "cg_sdt_cfg_lch_restrict_ext_v1800_s::cg_sdt_max_dur_to_next_cg_occasion_r18_e_"};

// CG-SDT-TA-ValidationConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cg_sdt_ta_validation_cfg_r17_s_fields[] = {
    {offsetof(cg_sdt_ta_validation_cfg_r17_s, cg_sdt_rsrp_change_thres_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17)>(),
     decltype(cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17)::has_ext,
     false,
     decltype(cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17)::nof_types,
     decltype(cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17)::nof_exts,
     nullptr,
     &cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_opts::desc}};
static const asn1::table::field_name seq_table_cg_sdt_ta_validation_cfg_r17_s_names[] = {
    {"cg-SDT-RSRP-ChangeThreshold-r17", nullptr}};
const asn1::table::seq_desc cg_sdt_ta_validation_cfg_r17_s::desc = {
    {seq_table_cg_sdt_ta_validation_cfg_r17_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_cg_sdt_ta_validation_cfg_r17_s_names};

static const char* enum_names_cg_sdt_ta_validation_cfg_r17_s_cg_sdt_rsrp_change_thres_r17_e__values[] = {"dB2",
                                                                                                         "dB4",
                                                                                                         "dB6",
                                                                                                         "dB8",
                                                                                                         "dB10",
                                                                                                         "dB14",
                                                                                                         "dB18",
                                                                                                         "dB22",
                                                                                                         "dB26",
                                                                                                         "dB30",
                                                                                                         "dB34",
                                                                                                         "spare5",
                                                                                                         "spare4",
                                                                                                         "spare3",
                                                                                                         "spare2",
                                                                                                         "spare1"};
const asn1::table::enum_desc cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_opts::desc  = {
    enum_names_cg_sdt_ta_validation_cfg_r17_s_cg_sdt_rsrp_change_thres_r17_e__values,
    16,
    "cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_e_"};
uint8_t cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8, 10, 14, 18, 22, 26, 30, 34};
  return map_enum_number(numbers, 11, value, "cg_sdt_ta_validation_cfg_r17_s::cg_sdt_rsrp_change_thres_r17_e_");
}

// CellGrouping-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_grouping_r16_s_fields[] = {
    {offsetof(cell_grouping_r16_s, mcg_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::list_codec<decltype(cell_grouping_r16_s::mcg_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(1024), false, false>>::value},
    {offsetof(cell_grouping_r16_s, scg_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::list_codec<decltype(cell_grouping_r16_s::scg_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(1024), false, false>>::value},
    {offsetof(cell_grouping_r16_s, mode_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(cell_grouping_r16_s::mode_r16)>(),
     decltype(cell_grouping_r16_s::mode_r16)::has_ext,
     false,
     decltype(cell_grouping_r16_s::mode_r16)::nof_types,
     decltype(cell_grouping_r16_s::mode_r16)::nof_exts,
     nullptr,
     &cell_grouping_r16_s::mode_r16_opts::desc}};
static const asn1::table::field_name seq_table_cell_grouping_r16_s_names[] = {{"mcg-r16", nullptr},
                                                                              {"scg-r16", nullptr},
                                                                              {"mode-r16", nullptr}};
const asn1::table::seq_desc          cell_grouping_r16_s::desc             = {
    {seq_table_cell_grouping_r16_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_cell_grouping_r16_s_names};

static const char*           enum_names_cell_grouping_r16_s_mode_r16_e__values[] = {"sync", "async"};
const asn1::table::enum_desc cell_grouping_r16_s::mode_r16_opts::desc            = {
    enum_names_cell_grouping_r16_s_mode_r16_e__values,
    2,
    "cell_grouping_r16_s::mode_r16_e_"};

// EarlyUL-SyncConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_early_ul_sync_cfg_r18_s_fields[] = {
    {offsetof(early_ul_sync_cfg_r18_s, freq_info_ul_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &freq_info_ul_s::desc},
    {offsetof(early_ul_sync_cfg_r18_s, rach_cfg_generic_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rach_cfg_generic_s::desc},
    {offsetof(early_ul_sync_cfg_r18_s, bwp_generic_params_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_s::desc},
    {offsetof(early_ul_sync_cfg_r18_s, ssb_per_rach_occasion_r18),
     (int32_t)offsetof(early_ul_sync_cfg_r18_s, ssb_per_rach_occasion_r18_present),
     asn1::table::enum_op<decltype(early_ul_sync_cfg_r18_s::ssb_per_rach_occasion_r18)>(),
     decltype(early_ul_sync_cfg_r18_s::ssb_per_rach_occasion_r18)::has_ext,
     false,
     decltype(early_ul_sync_cfg_r18_s::ssb_per_rach_occasion_r18)::nof_types,
     decltype(early_ul_sync_cfg_r18_s::ssb_per_rach_occasion_r18)::nof_exts,
     nullptr,
     &early_ul_sync_cfg_r18_s::ssb_per_rach_occasion_r18_opts::desc},
    {offsetof(early_ul_sync_cfg_r18_s, prach_root_seq_idx_r18),
     (int32_t)offsetof(early_ul_sync_cfg_r18_s, prach_root_seq_idx_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::desc},
    {offsetof(early_ul_sync_cfg_r18_s, ltm_prach_subcarrier_spacing_r18),
     (int32_t)offsetof(early_ul_sync_cfg_r18_s, ltm_prach_subcarrier_spacing_r18_present),
     asn1::table::enum_op<decltype(early_ul_sync_cfg_r18_s::ltm_prach_subcarrier_spacing_r18)>(),
     decltype(early_ul_sync_cfg_r18_s::ltm_prach_subcarrier_spacing_r18)::has_ext,
     false,
     decltype(early_ul_sync_cfg_r18_s::ltm_prach_subcarrier_spacing_r18)::nof_types,
     decltype(early_ul_sync_cfg_r18_s::ltm_prach_subcarrier_spacing_r18)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(early_ul_sync_cfg_r18_s, n_timing_advance_offset_r18),
     (int32_t)offsetof(early_ul_sync_cfg_r18_s, n_timing_advance_offset_r18_present),
     asn1::table::enum_op<decltype(early_ul_sync_cfg_r18_s::n_timing_advance_offset_r18)>(),
     decltype(early_ul_sync_cfg_r18_s::n_timing_advance_offset_r18)::has_ext,
     false,
     decltype(early_ul_sync_cfg_r18_s::n_timing_advance_offset_r18)::nof_types,
     decltype(early_ul_sync_cfg_r18_s::n_timing_advance_offset_r18)::nof_exts,
     nullptr,
     &early_ul_sync_cfg_r18_s::n_timing_advance_offset_r18_opts::desc}};
static const asn1::table::field_name seq_table_early_ul_sync_cfg_r18_s_names[] = {
    {"frequencyInfoUL-r18", nullptr},
    {"rach-ConfigGeneric-r18", nullptr},
    {"bwp-GenericParameters-r18", nullptr},
    {"ssb-PerRACH-Occasion-r18", nullptr},
    {"prach-RootSequenceIndex-r18", nullptr},
    {"ltm-PRACH-SubcarrierSpacing-r18", nullptr},
    {"n-TimingAdvanceOffset-r18", nullptr}};
static const asn1::table::seq_opt seq_table_early_ul_sync_cfg_r18_s_opts[] = {
    {3, (int32_t)offsetof(early_ul_sync_cfg_r18_s, ssb_per_rach_occasion_r18_present)},
    {4, (int32_t)offsetof(early_ul_sync_cfg_r18_s, prach_root_seq_idx_r18_present)},
    {5, (int32_t)offsetof(early_ul_sync_cfg_r18_s, ltm_prach_subcarrier_spacing_r18_present)},
    {6, (int32_t)offsetof(early_ul_sync_cfg_r18_s, n_timing_advance_offset_r18_present)}};
static const asn1::table::seq_field seq_table_early_ul_sync_cfg_r18_s_g0_fields[] = {
    {offsetof(early_ul_sync_cfg_r18_s, ltm_tdd_ul_dl_cfg_common_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(early_ul_sync_cfg_r18_s::ltm_tdd_ul_dl_cfg_common_r18)>::value,
     &tdd_ul_dl_cfg_common_s::desc},
    {offsetof(early_ul_sync_cfg_r18_s, ltm_restricted_set_cfg_r18),
     (int32_t)offsetof(early_ul_sync_cfg_r18_s, ltm_restricted_set_cfg_r18_present),
     asn1::table::enum_op<decltype(early_ul_sync_cfg_r18_s::ltm_restricted_set_cfg_r18)>(),
     decltype(early_ul_sync_cfg_r18_s::ltm_restricted_set_cfg_r18)::has_ext,
     false,
     decltype(early_ul_sync_cfg_r18_s::ltm_restricted_set_cfg_r18)::nof_types,
     decltype(early_ul_sync_cfg_r18_s::ltm_restricted_set_cfg_r18)::nof_exts,
     nullptr,
     &early_ul_sync_cfg_r18_s::ltm_restricted_set_cfg_r18_opts::desc}};
static const asn1::table::field_name seq_table_early_ul_sync_cfg_r18_s_g0_names[] = {
    {"ltm-tdd-UL-DL-ConfigurationCommon-r18", nullptr},
    {"ltm-restrictedSetConfig-r18", nullptr}};
static const asn1::table::seq_opt seq_table_early_ul_sync_cfg_r18_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(early_ul_sync_cfg_r18_s, ltm_restricted_set_cfg_r18_present)}};
static const asn1::table::seq_group seq_table_early_ul_sync_cfg_r18_s_groups[] = {
    {{seq_table_early_ul_sync_cfg_r18_s_g0_fields, 2, UINT64_C(0x0), seq_table_early_ul_sync_cfg_r18_s_g0_opts, 2},
     false,
     seq_table_early_ul_sync_cfg_r18_s_g0_names}};
const asn1::table::seq_desc early_ul_sync_cfg_r18_s::desc = {
    {seq_table_early_ul_sync_cfg_r18_s_fields, 7, UINT64_C(0x7), seq_table_early_ul_sync_cfg_r18_s_opts, 4},
    true,
    false,
    seq_table_early_ul_sync_cfg_r18_s_groups,
    1,
    seq_table_early_ul_sync_cfg_r18_s_names};

static const char* enum_names_early_ul_sync_cfg_r18_s_ssb_per_rach_occasion_r18_e__values[] =
    {"oneEighth", "oneFourth", "oneHalf", "one", "two", "four", "eight", "sixteen"};
const asn1::table::enum_desc early_ul_sync_cfg_r18_s::ssb_per_rach_occasion_r18_opts::desc = {
    enum_names_early_ul_sync_cfg_r18_s_ssb_per_rach_occasion_r18_e__values,
    8,
    "early_ul_sync_cfg_r18_s::ssb_per_rach_occasion_r18_e_"};
float early_ul_sync_cfg_r18_s::ssb_per_rach_occasion_r18_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0, 6.0};
  return map_enum_number(numbers, 8, value, "early_ul_sync_cfg_r18_s::ssb_per_rach_occasion_r18_e_");
}
const char* early_ul_sync_cfg_r18_s::ssb_per_rach_occasion_r18_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/8", "1/4", "1/2", "1", "2", "4", "8", "6"};
  return convert_enum_idx(number_strs, 8, value, "early_ul_sync_cfg_r18_s::ssb_per_rach_occasion_r18_e_");
}

static const asn1::table::storage_ops* const
    choice_storage_early_ul_sync_cfg_r18_s_prach_root_seq_idx_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::storage_desc = {
    choice_storage_early_ul_sync_cfg_r18_s_prach_root_seq_idx_r18_c__alternatives,
    2,
    "early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_"};

uint16_t& early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::set_l839()
{
  set(types::l839);
  return c.get<uint16_t>();
}
uint8_t& early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::set_l139()
{
  set(types::l139);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_early_ul_sync_cfg_r18_s_prach_root_seq_idx_r18_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(837),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(137),
     nullptr}};
static const asn1::table::field_name choice_table_early_ul_sync_cfg_r18_s_prach_root_seq_idx_r18_c__names[] = {
    {"l839", nullptr},
    {"l139", nullptr}};
const asn1::table::choice_desc early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::desc = {
    asn1::table::enum_int_type<early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::types>(),
    early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::types::nof_types,
    early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::types::nof_exts,
    early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::types::has_ext,
    false,
    choice_table_early_ul_sync_cfg_r18_s_prach_root_seq_idx_r18_c__alternatives,
    nullptr,
    choice_table_early_ul_sync_cfg_r18_s_prach_root_seq_idx_r18_c__names,
    "early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_",
    offsetof(early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_, c.type),
    offsetof(early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_, c.value),
    false,
    &early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::storage_desc};

static const char* enum_names_early_ul_sync_cfg_r18_s_prach_root_seq_idx_r18_c__types_values[]    = {"l839", "l139"};
const asn1::table::enum_desc early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::types_opts::desc = {
    enum_names_early_ul_sync_cfg_r18_s_prach_root_seq_idx_r18_c__types_values,
    2,
    "early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::types"};
uint16_t early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {839, 139};
  return map_enum_number(numbers, 2, value, "early_ul_sync_cfg_r18_s::prach_root_seq_idx_r18_c_::types");
}

static const char*           enum_names_early_ul_sync_cfg_r18_s_n_timing_advance_offset_r18_e__values[] = {"n0",
                                                                                                           "n25600",
                                                                                                           "n39936",
                                                                                                           "spare1"};
const asn1::table::enum_desc early_ul_sync_cfg_r18_s::n_timing_advance_offset_r18_opts::desc            = {
    enum_names_early_ul_sync_cfg_r18_s_n_timing_advance_offset_r18_e__values,
    4,
    "early_ul_sync_cfg_r18_s::n_timing_advance_offset_r18_e_"};
uint16_t early_ul_sync_cfg_r18_s::n_timing_advance_offset_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 25600, 39936};
  return map_enum_number(numbers, 3, value, "early_ul_sync_cfg_r18_s::n_timing_advance_offset_r18_e_");
}

static const char* enum_names_early_ul_sync_cfg_r18_s_ltm_restricted_set_cfg_r18_e__values[] = {"unrestrictedSet",
                                                                                                "restrictedSetTypeA",
                                                                                                "restrictedSetTypeB"};
const asn1::table::enum_desc early_ul_sync_cfg_r18_s::ltm_restricted_set_cfg_r18_opts::desc  = {
    enum_names_early_ul_sync_cfg_r18_s_ltm_restricted_set_cfg_r18_e__values,
    3,
    "early_ul_sync_cfg_r18_s::ltm_restricted_set_cfg_r18_e_"};

// FeatureSetDownlink-v15t0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_feature_set_dl_v15t0_s_fields[] = {
    {0,
     (int32_t)offsetof(feature_set_dl_v15t0_s, zero_slot_offset_aperiodic_srs_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_feature_set_dl_v15t0_s_names[] = {
    {"zeroSlotOffsetAperiodicSRS", "supported"}};
static const asn1::table::seq_opt seq_table_feature_set_dl_v15t0_s_opts[] = {
    {0, (int32_t)offsetof(feature_set_dl_v15t0_s, zero_slot_offset_aperiodic_srs_present)}};
const asn1::table::seq_desc feature_set_dl_v15t0_s::desc = {
    {seq_table_feature_set_dl_v15t0_s_fields, 1, UINT64_C(0x0), seq_table_feature_set_dl_v15t0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_feature_set_dl_v15t0_s_names};

// FeatureSetDownlink-v16k0 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__fields[] = {
        {offsetof(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_,
                  scs_15k_hz_r16),
         (int32_t)offsetof(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_,
                           scs_15k_hz_r16_present),
         asn1::table::enum_op<
             decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                          scs_15k_hz_r16)>(),
         decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                      scs_15k_hz_r16)::has_ext,
         false,
         decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                      scs_15k_hz_r16)::nof_types,
         decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                      scs_15k_hz_r16)::nof_exts,
         nullptr,
         &feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
             scs_15k_hz_r16_opts::desc},
        {offsetof(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_,
                  scs_30k_hz_r16),
         (int32_t)offsetof(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_,
                           scs_30k_hz_r16_present),
         asn1::table::enum_op<
             decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                          scs_30k_hz_r16)>(),
         decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                      scs_30k_hz_r16)::has_ext,
         false,
         decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                      scs_30k_hz_r16)::nof_types,
         decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                      scs_30k_hz_r16)::nof_exts,
         nullptr,
         &feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
             scs_30k_hz_r16_opts::desc},
        {offsetof(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_,
                  scs_60k_hz_r16),
         (int32_t)offsetof(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_,
                           scs_60k_hz_r16_present),
         asn1::table::enum_op<
             decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                          scs_60k_hz_r16)>(),
         decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                      scs_60k_hz_r16)::has_ext,
         false,
         decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                      scs_60k_hz_r16)::nof_types,
         decltype(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
                      scs_60k_hz_r16)::nof_exts,
         nullptr,
         &feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
             scs_60k_hz_r16_opts::desc}};
static const asn1::table::field_name
    seq_table_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__names[] = {
        {"scs-15kHz-r16", nullptr},
        {"scs-30kHz-r16", nullptr},
        {"scs-60kHz-r16", nullptr}};
static const asn1::table::seq_opt
    seq_table_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__opts[] = {
        {0,
         (int32_t)offsetof(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_,
                           scs_15k_hz_r16_present)},
        {1,
         (int32_t)offsetof(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_,
                           scs_30k_hz_r16_present)},
        {2,
         (int32_t)offsetof(feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_,
                           scs_60k_hz_r16_present)}};
static const asn1::table::seq_desc
    seq_table_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_ = {
        {seq_table_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__fields,
         3,
         UINT64_C(0x0),
         seq_table_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__opts,
         3},
        false,
        false,
        nullptr,
        0,
        seq_table_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__names};
static const asn1::table::seq_field seq_table_feature_set_dl_v16k0_s_fields[] = {
    {0,
     (int32_t)offsetof(feature_set_dl_v16k0_s, offset_srs_cb_pusch_ant_switch_fr1_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(feature_set_dl_v16k0_s, offset_srs_cb_pusch_pdcch_monitor_single_occ_fr1_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(feature_set_dl_v16k0_s, offset_srs_cb_pusch_pdcch_monitor_any_occ_without_gap_fr1_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(feature_set_dl_v16k0_s, offset_srs_cb_pusch_pdcch_monitor_any_occ_with_gap_fr1_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(feature_set_dl_v16k0_s, offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16),
     (int32_t)offsetof(feature_set_dl_v16k0_s, offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_}};
static const asn1::table::field_name seq_table_feature_set_dl_v16k0_s_names[] = {
    {"offsetSRS-CB-PUSCH-Ant-Switch-fr1-r16", "supported"},
    {"offsetSRS-CB-PUSCH-PDCCH-MonitorSingleOcc-fr1-r16", "supported"},
    {"offsetSRS-CB-PUSCH-PDCCH-MonitorAnyOccWithoutGap-fr1-r16", "supported"},
    {"offsetSRS-CB-PUSCH-PDCCH-MonitorAnyOccWithGap-fr1-r16", "supported"},
    {"offsetSRS-CB-PUSCH-PDCCH-MonitorAnyOccWithSpanGap-fr1-r16", nullptr}};
static const asn1::table::seq_opt seq_table_feature_set_dl_v16k0_s_opts[] = {
    {0, (int32_t)offsetof(feature_set_dl_v16k0_s, offset_srs_cb_pusch_ant_switch_fr1_r16_present)},
    {1, (int32_t)offsetof(feature_set_dl_v16k0_s, offset_srs_cb_pusch_pdcch_monitor_single_occ_fr1_r16_present)},
    {2,
     (int32_t)offsetof(feature_set_dl_v16k0_s, offset_srs_cb_pusch_pdcch_monitor_any_occ_without_gap_fr1_r16_present)},
    {3, (int32_t)offsetof(feature_set_dl_v16k0_s, offset_srs_cb_pusch_pdcch_monitor_any_occ_with_gap_fr1_r16_present)},
    {4,
     (int32_t)offsetof(feature_set_dl_v16k0_s,
                       offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_present)}};
const asn1::table::seq_desc feature_set_dl_v16k0_s::desc = {
    {seq_table_feature_set_dl_v16k0_s_fields, 5, UINT64_C(0x0), seq_table_feature_set_dl_v16k0_s_opts, 5},
    false,
    false,
    nullptr,
    0,
    seq_table_feature_set_dl_v16k0_s_names};

static const char*
    enum_names_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__scs_15k_hz_r16_e__values
        [] = {"set1", "set2", "set3"};
const asn1::table::enum_desc feature_set_dl_v16k0_s::
    offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::scs_15k_hz_r16_opts::desc = {
        enum_names_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__scs_15k_hz_r16_e__values,
        3,
        "feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::scs_15k_hz_r16_"
        "e_"};
uint8_t feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
    scs_15k_hz_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(
      numbers,
      3,
      value,
      "feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::scs_15k_hz_r16_e_");
}

static const char*
    enum_names_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__scs_30k_hz_r16_e__values
        [] = {"set1", "set2", "set3"};
const asn1::table::enum_desc feature_set_dl_v16k0_s::
    offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::scs_30k_hz_r16_opts::desc = {
        enum_names_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__scs_30k_hz_r16_e__values,
        3,
        "feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::scs_30k_hz_r16_"
        "e_"};
uint8_t feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
    scs_30k_hz_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(
      numbers,
      3,
      value,
      "feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::scs_30k_hz_r16_e_");
}

static const char*
    enum_names_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__scs_60k_hz_r16_e__values
        [] = {"set1", "set2", "set3"};
const asn1::table::enum_desc feature_set_dl_v16k0_s::
    offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::scs_60k_hz_r16_opts::desc = {
        enum_names_feature_set_dl_v16k0_s_offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s__scs_60k_hz_r16_e__values,
        3,
        "feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::scs_60k_hz_r16_"
        "e_"};
uint8_t feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::
    scs_60k_hz_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(
      numbers,
      3,
      value,
      "feature_set_dl_v16k0_s::offset_srs_cb_pusch_pdcch_monitor_any_occ_with_span_gap_fr1_r16_s_::scs_60k_hz_r16_e_");
}

// FeatureSetDownlink-v17d0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_feature_set_dl_v17d0_s_fields[] = {
    {0,
     (int32_t)offsetof(feature_set_dl_v17d0_s, srs_ant_switching2_sp_1_periodic_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(feature_set_dl_v17d0_s, srs_ext_aperiodic_srs_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(feature_set_dl_v17d0_s, srs_one_ap_srs_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_feature_set_dl_v17d0_s_names[] = {
    {"srs-AntennaSwitching2SP-1Periodic-r17", "supported"},
    {"srs-ExtensionAperiodicSRS-r17", "supported"},
    {"srs-OneAP-SRS-r17", "supported"}};
static const asn1::table::seq_opt seq_table_feature_set_dl_v17d0_s_opts[] = {
    {0, (int32_t)offsetof(feature_set_dl_v17d0_s, srs_ant_switching2_sp_1_periodic_r17_present)},
    {1, (int32_t)offsetof(feature_set_dl_v17d0_s, srs_ext_aperiodic_srs_r17_present)},
    {2, (int32_t)offsetof(feature_set_dl_v17d0_s, srs_one_ap_srs_r17_present)}};
const asn1::table::seq_desc feature_set_dl_v17d0_s::desc = {
    {seq_table_feature_set_dl_v17d0_s_fields, 3, UINT64_C(0x0), seq_table_feature_set_dl_v17d0_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_feature_set_dl_v17d0_s_names};

// FeatureSetUplink-v16d0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_feature_set_ul_v16d0_s_pusch_repeat_type_b_v16d0_s__fields[] = {
    {offsetof(feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_, max_num_pusch_tx_cap1_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap1_r16)>(),
     decltype(feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap1_r16)::has_ext,
     false,
     decltype(feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap1_r16)::nof_types,
     decltype(feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap1_r16)::nof_exts,
     nullptr,
     &feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap1_r16_opts::desc},
    {offsetof(feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_, max_num_pusch_tx_cap2_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap2_r16)>(),
     decltype(feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap2_r16)::has_ext,
     false,
     decltype(feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap2_r16)::nof_types,
     decltype(feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap2_r16)::nof_exts,
     nullptr,
     &feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap2_r16_opts::desc}};
static const asn1::table::field_name seq_table_feature_set_ul_v16d0_s_pusch_repeat_type_b_v16d0_s__names[] = {
    {"maxNumberPUSCH-Tx-Cap1-r16", nullptr},
    {"maxNumberPUSCH-Tx-Cap2-r16", nullptr}};
static const asn1::table::seq_desc seq_table_feature_set_ul_v16d0_s_pusch_repeat_type_b_v16d0_s_ = {
    {seq_table_feature_set_ul_v16d0_s_pusch_repeat_type_b_v16d0_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_feature_set_ul_v16d0_s_pusch_repeat_type_b_v16d0_s__names};
static const asn1::table::seq_field seq_table_feature_set_ul_v16d0_s_fields[] = {
    {offsetof(feature_set_ul_v16d0_s, pusch_repeat_type_b_v16d0),
     (int32_t)offsetof(feature_set_ul_v16d0_s, pusch_repeat_type_b_v16d0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_feature_set_ul_v16d0_s_pusch_repeat_type_b_v16d0_s_}};
static const asn1::table::field_name seq_table_feature_set_ul_v16d0_s_names[] = {
    {"pusch-RepetitionTypeB-v16d0", nullptr}};
static const asn1::table::seq_opt seq_table_feature_set_ul_v16d0_s_opts[] = {
    {0, (int32_t)offsetof(feature_set_ul_v16d0_s, pusch_repeat_type_b_v16d0_present)}};
const asn1::table::seq_desc feature_set_ul_v16d0_s::desc = {
    {seq_table_feature_set_ul_v16d0_s_fields, 1, UINT64_C(0x0), seq_table_feature_set_ul_v16d0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_feature_set_ul_v16d0_s_names};

static const char*
    enum_names_feature_set_ul_v16d0_s_pusch_repeat_type_b_v16d0_s__max_num_pusch_tx_cap1_r16_e__values[] =
        {"n2", "n3", "n4", "n7", "n8", "n12"};
const asn1::table::enum_desc
    feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap1_r16_opts::desc = {
        enum_names_feature_set_ul_v16d0_s_pusch_repeat_type_b_v16d0_s__max_num_pusch_tx_cap1_r16_e__values,
        6,
        "feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap1_r16_e_"};
uint8_t feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap1_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4, 7, 8, 12};
  return map_enum_number(
      numbers, 6, value, "feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap1_r16_e_");
}

static const char*
    enum_names_feature_set_ul_v16d0_s_pusch_repeat_type_b_v16d0_s__max_num_pusch_tx_cap2_r16_e__values[] =
        {"n2", "n3", "n4", "n7", "n8", "n12"};
const asn1::table::enum_desc
    feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap2_r16_opts::desc = {
        enum_names_feature_set_ul_v16d0_s_pusch_repeat_type_b_v16d0_s__max_num_pusch_tx_cap2_r16_e__values,
        6,
        "feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap2_r16_e_"};
uint8_t feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap2_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4, 7, 8, 12};
  return map_enum_number(
      numbers, 6, value, "feature_set_ul_v16d0_s::pusch_repeat_type_b_v16d0_s_::max_num_pusch_tx_cap2_r16_e_");
}

// FeatureSets-v15t0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_feature_sets_v15t0_s_fields[] = {
    {offsetof(feature_sets_v15t0_s, feature_sets_dl_v15t0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::item_list_codec<decltype(feature_sets_v15t0_s::feature_sets_dl_v15t0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &feature_set_dl_v15t0_s::desc}};
static const asn1::table::field_name seq_table_feature_sets_v15t0_s_names[] = {{"featureSetsDownlink-v15t0", nullptr}};
static const asn1::table::seq_opt seq_table_feature_sets_v15t0_s_opts[] = {{0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       feature_sets_v15t0_s::desc            = {
    {seq_table_feature_sets_v15t0_s_fields, 1, UINT64_C(0x0), seq_table_feature_sets_v15t0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_feature_sets_v15t0_s_names};

// FeatureSets-v16d0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_feature_sets_v16d0_s_fields[] = {
    {offsetof(feature_sets_v16d0_s, feature_sets_ul_v16d0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::item_list_codec<decltype(feature_sets_v16d0_s::feature_sets_ul_v16d0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &feature_set_ul_v16d0_s::desc}};
static const asn1::table::field_name seq_table_feature_sets_v16d0_s_names[] = {{"featureSetsUplink-v16d0", nullptr}};
static const asn1::table::seq_opt seq_table_feature_sets_v16d0_s_opts[] = {{0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       feature_sets_v16d0_s::desc            = {
    {seq_table_feature_sets_v16d0_s_fields, 1, UINT64_C(0x0), seq_table_feature_sets_v16d0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_feature_sets_v16d0_s_names};

// FeatureSets-v16k0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_feature_sets_v16k0_s_fields[] = {
    {offsetof(feature_sets_v16k0_s, feature_sets_dl_v16k0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::item_list_codec<decltype(feature_sets_v16k0_s::feature_sets_dl_v16k0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &feature_set_dl_v16k0_s::desc}};
static const asn1::table::field_name seq_table_feature_sets_v16k0_s_names[] = {{"featureSetsDownlink-v16k0", nullptr}};
static const asn1::table::seq_opt seq_table_feature_sets_v16k0_s_opts[] = {{0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       feature_sets_v16k0_s::desc            = {
    {seq_table_feature_sets_v16k0_s_fields, 1, UINT64_C(0x0), seq_table_feature_sets_v16k0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_feature_sets_v16k0_s_names};

// FeatureSets-v17d0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_feature_sets_v17d0_s_fields[] = {
    {offsetof(feature_sets_v17d0_s, feature_sets_dl_v17d0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::item_list_codec<decltype(feature_sets_v17d0_s::feature_sets_dl_v17d0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &feature_set_dl_v17d0_s::desc}};
static const asn1::table::field_name seq_table_feature_sets_v17d0_s_names[] = {{"featureSetsDownlink-v17d0", nullptr}};
static const asn1::table::seq_opt seq_table_feature_sets_v17d0_s_opts[] = {{0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       feature_sets_v17d0_s::desc            = {
    {seq_table_feature_sets_v17d0_s_fields, 1, UINT64_C(0x0), seq_table_feature_sets_v17d0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_feature_sets_v17d0_s_names};

// MAC-Parameters-v17b0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mac_params_v17b0_s_fields[] = {
    {0,
     (int32_t)offsetof(mac_params_v17b0_s, mtrp_pusch_phr_type1_report_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_mac_params_v17b0_s_names[] = {
    {"mTRP-PUSCH-PHR-Type1-Reporting-r17", "supported"}};
static const asn1::table::seq_opt seq_table_mac_params_v17b0_s_opts[] = {
    {0, (int32_t)offsetof(mac_params_v17b0_s, mtrp_pusch_phr_type1_report_r17_present)}};
const asn1::table::seq_desc mac_params_v17b0_s::desc = {
    {seq_table_mac_params_v17b0_s_fields, 1, UINT64_C(0x0), seq_table_mac_params_v17b0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_mac_params_v17b0_s_names};

// MAC-Parameters-v17c0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mac_params_v17c0_s_fields[] = {
    {0,
     (int32_t)offsetof(mac_params_v17c0_s, direct_scell_activation_with_tci_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_mac_params_v17c0_s_names[] = {
    {"directSCellActivationWithTCI-r17", "supported"}};
static const asn1::table::seq_opt seq_table_mac_params_v17c0_s_opts[] = {
    {0, (int32_t)offsetof(mac_params_v17c0_s, direct_scell_activation_with_tci_r17_present)}};
const asn1::table::seq_desc mac_params_v17c0_s::desc = {
    {seq_table_mac_params_v17c0_s_fields, 1, UINT64_C(0x0), seq_table_mac_params_v17c0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_mac_params_v17c0_s_names};

// MBS-NeighbourCell-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_neighbour_cell_r17_s_fields[] = {
    {offsetof(mbs_neighbour_cell_r17_s, pci_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(mbs_neighbour_cell_r17_s, carrier_freq_r17),
     (int32_t)offsetof(mbs_neighbour_cell_r17_s, carrier_freq_r17_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr}};
static const asn1::table::field_name seq_table_mbs_neighbour_cell_r17_s_names[] = {{"physCellId-r17", nullptr},
                                                                                   {"carrierFreq-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_mbs_neighbour_cell_r17_s_opts[]  = {
    {1, (int32_t)offsetof(mbs_neighbour_cell_r17_s, carrier_freq_r17_present)}};
const asn1::table::seq_desc mbs_neighbour_cell_r17_s::desc = {
    {seq_table_mbs_neighbour_cell_r17_s_fields, 2, UINT64_C(0x1), seq_table_mbs_neighbour_cell_r17_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_mbs_neighbour_cell_r17_s_names};

// MRB-PDCP-ConfigBroadcast-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrb_pdcp_cfg_broadcast_r17_s_fields[] = {
    {0,
     (int32_t)offsetof(mrb_pdcp_cfg_broadcast_r17_s, pdcp_sn_size_dl_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(mrb_pdcp_cfg_broadcast_r17_s, hdr_compress_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::desc},
    {offsetof(mrb_pdcp_cfg_broadcast_r17_s, t_reordering_r17),
     (int32_t)offsetof(mrb_pdcp_cfg_broadcast_r17_s, t_reordering_r17_present),
     asn1::table::enum_op<decltype(mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17)>(),
     decltype(mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17)::has_ext,
     false,
     decltype(mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17)::nof_types,
     decltype(mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17)::nof_exts,
     nullptr,
     &mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_opts::desc}};
static const asn1::table::field_name seq_table_mrb_pdcp_cfg_broadcast_r17_s_names[] = {
    {"pdcp-SN-SizeDL-r17", "len12bits"},
    {"headerCompression-r17", nullptr},
    {"t-Reordering-r17", nullptr}};
static const asn1::table::seq_opt seq_table_mrb_pdcp_cfg_broadcast_r17_s_opts[] = {
    {0, (int32_t)offsetof(mrb_pdcp_cfg_broadcast_r17_s, pdcp_sn_size_dl_r17_present)},
    {2, (int32_t)offsetof(mrb_pdcp_cfg_broadcast_r17_s, t_reordering_r17_present)}};
const asn1::table::seq_desc mrb_pdcp_cfg_broadcast_r17_s::desc = {
    {seq_table_mrb_pdcp_cfg_broadcast_r17_s_fields, 3, UINT64_C(0x2), seq_table_mrb_pdcp_cfg_broadcast_r17_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_mrb_pdcp_cfg_broadcast_r17_s_names};

void mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::set(types::options e)
{
  type_ = e;
}
void mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::set_not_used()
{
  set(types::not_used);
}
mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::rohc_s_&
mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::set_rohc()
{
  set(types::rohc);
  return c;
}
static const asn1::table::seq_field
    seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__profiles_r17_s__fields[] = {
        {offsetof(mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::rohc_s_::profiles_r17_s_, profile0x0000_r17),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::boolean,
         false,
         false,
         0,
         0,
         nullptr},
        {offsetof(mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::rohc_s_::profiles_r17_s_, profile0x0001_r17),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::boolean,
         false,
         false,
         0,
         0,
         nullptr},
        {offsetof(mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::rohc_s_::profiles_r17_s_, profile0x0002_r17),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::boolean,
         false,
         false,
         0,
         0,
         nullptr}};
static const asn1::table::field_name
    seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__profiles_r17_s__names[] = {
        {"profile0x0000-r17", nullptr},
        {"profile0x0001-r17", nullptr},
        {"profile0x0002-r17", nullptr}};
static const asn1::table::seq_desc seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__profiles_r17_s_ =
    {{seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__profiles_r17_s__fields,
      3,
      UINT64_C(0x7),
      nullptr,
      0},
     false,
     false,
     nullptr,
     0,
     seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__profiles_r17_s__names};
static const asn1::table::seq_field seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__fields[] = {
    {offsetof(mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::rohc_s_, max_c_id_r17),
     (int32_t)offsetof(mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::rohc_s_, max_c_id_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(16),
     nullptr},
    {offsetof(mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::rohc_s_, profiles_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__profiles_r17_s_}};
static const asn1::table::field_name seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__names[] = {
    {"maxCID-r17", nullptr},
    {"profiles-r17", nullptr}};
static const asn1::table::seq_opt seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__opts[] = {
    {0, (int32_t)offsetof(mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::rohc_s_, max_c_id_r17_present)}};
static const asn1::table::seq_desc seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s_ = {
    {seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__fields,
     2,
     UINT64_C(0x2),
     seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s__names};
static const asn1::table::seq_field choice_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__rohc_s_}};
static const asn1::table::field_name choice_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__names[] = {
    {"notUsed", nullptr},
    {"rohc", nullptr}};
const asn1::table::choice_desc mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::desc = {
    asn1::table::enum_int_type<mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::types>(),
    mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::types::nof_types,
    mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::types::nof_exts,
    mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::types::has_ext,
    false,
    choice_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__alternatives,
    &asn1::table::choice_set<mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_>,
    choice_table_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__names,
    "mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_",
    offsetof(mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_, type_),
    offsetof(mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_, c),
    false};

static const char* enum_names_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__types_values[]    = {"notUsed", "rohc"};
const asn1::table::enum_desc mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::types_opts::desc = {
    enum_names_mrb_pdcp_cfg_broadcast_r17_s_hdr_compress_r17_c__types_values,
    2,
    "mrb_pdcp_cfg_broadcast_r17_s::hdr_compress_r17_c_::types"};

static const char* enum_names_mrb_pdcp_cfg_broadcast_r17_s_t_reordering_r17_e__values[] =
    {"ms1", "ms10", "ms40", "ms160", "ms500", "ms1000", "ms1250", "ms2750"};
const asn1::table::enum_desc mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_opts::desc = {
    enum_names_mrb_pdcp_cfg_broadcast_r17_s_t_reordering_r17_e__values,
    8,
    "mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_e_"};
uint16_t mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 10, 40, 160, 500, 1000, 1250, 2750};
  return map_enum_number(numbers, 8, value, "mrb_pdcp_cfg_broadcast_r17_s::t_reordering_r17_e_");
}

// MRB-RLC-ConfigBroadcast-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrb_rlc_cfg_broadcast_r17_s_fields[] = {
    {offsetof(mrb_rlc_cfg_broadcast_r17_s, lc_ch_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {0,
     (int32_t)offsetof(mrb_rlc_cfg_broadcast_r17_s, sn_field_len_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(mrb_rlc_cfg_broadcast_r17_s, t_reassembly_r17),
     (int32_t)offsetof(mrb_rlc_cfg_broadcast_r17_s, t_reassembly_r17_present),
     asn1::table::enum_op<decltype(mrb_rlc_cfg_broadcast_r17_s::t_reassembly_r17)>(),
     decltype(mrb_rlc_cfg_broadcast_r17_s::t_reassembly_r17)::has_ext,
     false,
     decltype(mrb_rlc_cfg_broadcast_r17_s::t_reassembly_r17)::nof_types,
     decltype(mrb_rlc_cfg_broadcast_r17_s::t_reassembly_r17)::nof_exts,
     nullptr,
     &t_reassembly_opts::desc}};
static const asn1::table::field_name seq_table_mrb_rlc_cfg_broadcast_r17_s_names[] = {
    {"logicalChannelIdentity-r17", nullptr},
    {"sn-FieldLength-r17", "size6"},
    {"t-Reassembly-r17", nullptr}};
static const asn1::table::seq_opt seq_table_mrb_rlc_cfg_broadcast_r17_s_opts[] = {
    {1, (int32_t)offsetof(mrb_rlc_cfg_broadcast_r17_s, sn_field_len_r17_present)},
    {2, (int32_t)offsetof(mrb_rlc_cfg_broadcast_r17_s, t_reassembly_r17_present)}};
const asn1::table::seq_desc mrb_rlc_cfg_broadcast_r17_s::desc = {
    {seq_table_mrb_rlc_cfg_broadcast_r17_s_fields, 3, UINT64_C(0x1), seq_table_mrb_rlc_cfg_broadcast_r17_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_mrb_rlc_cfg_broadcast_r17_s_names};

// MRB-InfoBroadcast-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrb_info_broadcast_r17_s_fields[] = {
    {offsetof(mrb_info_broadcast_r17_s, pdcp_cfg_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mrb_pdcp_cfg_broadcast_r17_s::desc},
    {offsetof(mrb_info_broadcast_r17_s, rlc_cfg_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mrb_rlc_cfg_broadcast_r17_s::desc}};
static const asn1::table::field_name seq_table_mrb_info_broadcast_r17_s_names[] = {{"pdcp-Config-r17", nullptr},
                                                                                   {"rlc-Config-r17", nullptr}};
const asn1::table::seq_desc          mrb_info_broadcast_r17_s::desc             = {
    {seq_table_mrb_info_broadcast_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_mrb_info_broadcast_r17_s_names};

// MBS-SessionInfo-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_session_info_r17_s_fields[] = {
    {offsetof(mbs_session_info_r17_s, mbs_session_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tmgi_r17_s::desc},
    {offsetof(mbs_session_info_r17_s, g_rnti_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(mbs_session_info_r17_s, mrb_list_broadcast_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(mbs_session_info_r17_s::mrb_list_broadcast_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mrb_info_broadcast_r17_s::desc},
    {offsetof(mbs_session_info_r17_s, mtch_sched_info_r17),
     (int32_t)offsetof(mbs_session_info_r17_s, mtch_sched_info_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(mbs_session_info_r17_s, mtch_neighbour_cell_r17),
     (int32_t)offsetof(mbs_session_info_r17_s, mtch_neighbour_cell_r17_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_session_info_r17_s::mtch_neighbour_cell_r17),
                                asn1::table::struct_json>::value},
    {offsetof(mbs_session_info_r17_s, pdsch_cfg_idx_r17),
     (int32_t)offsetof(mbs_session_info_r17_s, pdsch_cfg_idx_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {offsetof(mbs_session_info_r17_s, mtch_ssb_map_win_idx_r17),
     (int32_t)offsetof(mbs_session_info_r17_s, mtch_ssb_map_win_idx_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(15),
     nullptr}};
static const asn1::table::field_name seq_table_mbs_session_info_r17_s_names[] = {
    {"mbs-SessionId-r17", nullptr},
    {"g-RNTI-r17", nullptr},
    {"mrb-ListBroadcast-r17", nullptr},
    {"mtch-SchedulingInfo-r17", nullptr},
    {"mtch-NeighbourCell-r17", nullptr},
    {"pdsch-ConfigIndex-r17", nullptr},
    {"mtch-SSB-MappingWindowIndex-r17", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_session_info_r17_s_opts[] = {
    {3, (int32_t)offsetof(mbs_session_info_r17_s, mtch_sched_info_r17_present)},
    {4, (int32_t)offsetof(mbs_session_info_r17_s, mtch_neighbour_cell_r17_present)},
    {5, (int32_t)offsetof(mbs_session_info_r17_s, pdsch_cfg_idx_r17_present)},
    {6, (int32_t)offsetof(mbs_session_info_r17_s, mtch_ssb_map_win_idx_r17_present)}};
const asn1::table::seq_desc mbs_session_info_r17_s::desc = {
    {seq_table_mbs_session_info_r17_s_fields, 7, UINT64_C(0x7), seq_table_mbs_session_info_r17_s_opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_mbs_session_info_r17_s_names};

// MRB-PDCP-ConfigMulticast-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrb_pdcp_cfg_multicast_r18_s_fields[] = {
    {offsetof(mrb_pdcp_cfg_multicast_r18_s, pdcp_sn_size_dl_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mrb_pdcp_cfg_multicast_r18_s::pdcp_sn_size_dl_r18)>(),
     decltype(mrb_pdcp_cfg_multicast_r18_s::pdcp_sn_size_dl_r18)::has_ext,
     false,
     decltype(mrb_pdcp_cfg_multicast_r18_s::pdcp_sn_size_dl_r18)::nof_types,
     decltype(mrb_pdcp_cfg_multicast_r18_s::pdcp_sn_size_dl_r18)::nof_exts,
     nullptr,
     &mrb_pdcp_cfg_multicast_r18_s::pdcp_sn_size_dl_r18_opts::desc},
    {offsetof(mrb_pdcp_cfg_multicast_r18_s, hdr_compress_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::desc},
    {offsetof(mrb_pdcp_cfg_multicast_r18_s, t_reordering_r17),
     (int32_t)offsetof(mrb_pdcp_cfg_multicast_r18_s, t_reordering_r17_present),
     asn1::table::enum_op<decltype(mrb_pdcp_cfg_multicast_r18_s::t_reordering_r17)>(),
     decltype(mrb_pdcp_cfg_multicast_r18_s::t_reordering_r17)::has_ext,
     false,
     decltype(mrb_pdcp_cfg_multicast_r18_s::t_reordering_r17)::nof_types,
     decltype(mrb_pdcp_cfg_multicast_r18_s::t_reordering_r17)::nof_exts,
     nullptr,
     &mrb_pdcp_cfg_multicast_r18_s::t_reordering_r17_opts::desc}};
static const asn1::table::field_name seq_table_mrb_pdcp_cfg_multicast_r18_s_names[] = {
    {"pdcp-SN-SizeDL-r18", nullptr},
    {"headerCompression-r18", nullptr},
    {"t-Reordering-r17", nullptr}};
static const asn1::table::seq_opt seq_table_mrb_pdcp_cfg_multicast_r18_s_opts[] = {
    {2, (int32_t)offsetof(mrb_pdcp_cfg_multicast_r18_s, t_reordering_r17_present)}};
const asn1::table::seq_desc mrb_pdcp_cfg_multicast_r18_s::desc = {
    {seq_table_mrb_pdcp_cfg_multicast_r18_s_fields, 3, UINT64_C(0x3), seq_table_mrb_pdcp_cfg_multicast_r18_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_mrb_pdcp_cfg_multicast_r18_s_names};

static const char* enum_names_mrb_pdcp_cfg_multicast_r18_s_pdcp_sn_size_dl_r18_e__values[] = {"len12bits", "len18bits"};
const asn1::table::enum_desc mrb_pdcp_cfg_multicast_r18_s::pdcp_sn_size_dl_r18_opts::desc  = {
    enum_names_mrb_pdcp_cfg_multicast_r18_s_pdcp_sn_size_dl_r18_e__values,
    2,
    "mrb_pdcp_cfg_multicast_r18_s::pdcp_sn_size_dl_r18_e_"};
uint8_t mrb_pdcp_cfg_multicast_r18_s::pdcp_sn_size_dl_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {12, 18};
  return map_enum_number(numbers, 2, value, "mrb_pdcp_cfg_multicast_r18_s::pdcp_sn_size_dl_r18_e_");
}

void mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::set(types::options e)
{
  type_ = e;
}
void mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::set_not_used()
{
  set(types::not_used);
}
mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::rohc_s_&
mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::set_rohc()
{
  set(types::rohc);
  return c;
}
static const asn1::table::seq_field
    seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__profiles_r18_s__fields[] = {
        {offsetof(mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::rohc_s_::profiles_r18_s_, profile0x0000_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::boolean,
         false,
         false,
         0,
         0,
         nullptr},
        {offsetof(mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::rohc_s_::profiles_r18_s_, profile0x0001_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::boolean,
         false,
         false,
         0,
         0,
         nullptr},
        {offsetof(mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::rohc_s_::profiles_r18_s_, profile0x0002_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::boolean,
         false,
         false,
         0,
         0,
         nullptr}};
static const asn1::table::field_name
    seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__profiles_r18_s__names[] = {
        {"profile0x0000-r18", nullptr},
        {"profile0x0001-r18", nullptr},
        {"profile0x0002-r18", nullptr}};
static const asn1::table::seq_desc seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__profiles_r18_s_ =
    {{seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__profiles_r18_s__fields,
      3,
      UINT64_C(0x7),
      nullptr,
      0},
     false,
     false,
     nullptr,
     0,
     seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__profiles_r18_s__names};
static const asn1::table::seq_field seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__fields[] = {
    {offsetof(mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::rohc_s_, max_c_id_r18),
     (int32_t)offsetof(mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::rohc_s_, max_c_id_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(16),
     nullptr},
    {offsetof(mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::rohc_s_, profiles_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__profiles_r18_s_}};
static const asn1::table::field_name seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__names[] = {
    {"maxCID-r18", nullptr},
    {"profiles-r18", nullptr}};
static const asn1::table::seq_opt seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__opts[] = {
    {0, (int32_t)offsetof(mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::rohc_s_, max_c_id_r18_present)}};
static const asn1::table::seq_desc seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s_ = {
    {seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__fields,
     2,
     UINT64_C(0x2),
     seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s__names};
static const asn1::table::seq_field choice_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__rohc_s_}};
static const asn1::table::field_name choice_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__names[] = {
    {"notUsed", nullptr},
    {"rohc", nullptr}};
const asn1::table::choice_desc mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::desc = {
    asn1::table::enum_int_type<mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::types>(),
    mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::types::nof_types,
    mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::types::nof_exts,
    mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::types::has_ext,
    false,
    choice_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__alternatives,
    &asn1::table::choice_set<mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_>,
    choice_table_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__names,
    "mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_",
    offsetof(mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_, type_),
    offsetof(mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_, c),
    false};

static const char* enum_names_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__types_values[]    = {"notUsed", "rohc"};
const asn1::table::enum_desc mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::types_opts::desc = {
    enum_names_mrb_pdcp_cfg_multicast_r18_s_hdr_compress_r18_c__types_values,
    2,
    "mrb_pdcp_cfg_multicast_r18_s::hdr_compress_r18_c_::types"};

static const char* enum_names_mrb_pdcp_cfg_multicast_r18_s_t_reordering_r17_e__values[] =
    {"ms1", "ms10", "ms40", "ms160", "ms500", "ms1000", "ms1250", "ms2750"};
const asn1::table::enum_desc mrb_pdcp_cfg_multicast_r18_s::t_reordering_r17_opts::desc = {
    enum_names_mrb_pdcp_cfg_multicast_r18_s_t_reordering_r17_e__values,
    8,
    "mrb_pdcp_cfg_multicast_r18_s::t_reordering_r17_e_"};
uint16_t mrb_pdcp_cfg_multicast_r18_s::t_reordering_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 10, 40, 160, 500, 1000, 1250, 2750};
  return map_enum_number(numbers, 8, value, "mrb_pdcp_cfg_multicast_r18_s::t_reordering_r17_e_");
}

// MRB-RLC-ConfigMulticast-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrb_rlc_cfg_multicast_r18_s_fields[] = {
    {offsetof(mrb_rlc_cfg_multicast_r18_s, lc_ch_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::desc},
    {offsetof(mrb_rlc_cfg_multicast_r18_s, sn_field_len_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(mrb_rlc_cfg_multicast_r18_s::sn_field_len_r18)>(),
     decltype(mrb_rlc_cfg_multicast_r18_s::sn_field_len_r18)::has_ext,
     false,
     decltype(mrb_rlc_cfg_multicast_r18_s::sn_field_len_r18)::nof_types,
     decltype(mrb_rlc_cfg_multicast_r18_s::sn_field_len_r18)::nof_exts,
     nullptr,
     &mrb_rlc_cfg_multicast_r18_s::sn_field_len_r18_opts::desc},
    {offsetof(mrb_rlc_cfg_multicast_r18_s, t_reassembly_r18),
     (int32_t)offsetof(mrb_rlc_cfg_multicast_r18_s, t_reassembly_r18_present),
     asn1::table::enum_op<decltype(mrb_rlc_cfg_multicast_r18_s::t_reassembly_r18)>(),
     decltype(mrb_rlc_cfg_multicast_r18_s::t_reassembly_r18)::has_ext,
     false,
     decltype(mrb_rlc_cfg_multicast_r18_s::t_reassembly_r18)::nof_types,
     decltype(mrb_rlc_cfg_multicast_r18_s::t_reassembly_r18)::nof_exts,
     nullptr,
     &t_reassembly_opts::desc}};
static const asn1::table::field_name seq_table_mrb_rlc_cfg_multicast_r18_s_names[] = {
    {"logicalChannelIdentity-r18", nullptr},
    {"sn-FieldLength-r18", nullptr},
    {"t-Reassembly-r18", nullptr}};
static const asn1::table::seq_opt seq_table_mrb_rlc_cfg_multicast_r18_s_opts[] = {
    {2, (int32_t)offsetof(mrb_rlc_cfg_multicast_r18_s, t_reassembly_r18_present)}};
const asn1::table::seq_desc mrb_rlc_cfg_multicast_r18_s::desc = {
    {seq_table_mrb_rlc_cfg_multicast_r18_s_fields, 3, UINT64_C(0x3), seq_table_mrb_rlc_cfg_multicast_r18_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_mrb_rlc_cfg_multicast_r18_s_names};

static const asn1::table::storage_ops* const choice_storage_mrb_rlc_cfg_multicast_r18_s_lc_ch_id_r18_c__alternatives[] =
    {&asn1::table::alternative_ops<uint8_t, true>::value, &asn1::table::alternative_ops<uint32_t, true>::value};
const asn1::table::choice_storage_desc mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::storage_desc = {
    choice_storage_mrb_rlc_cfg_multicast_r18_s_lc_ch_id_r18_c__alternatives,
    2,
    "mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_"};

uint8_t& mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::set_lc_ch_idmulticast_r18()
{
  set(types::lc_ch_idmulticast_r18);
  return c.get<uint8_t>();
}
uint32_t& mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::set_lc_ch_id_ext_r18()
{
  set(types::lc_ch_id_ext_r18);
  return c.get<uint32_t>();
}
static const asn1::table::seq_field choice_table_mrb_rlc_cfg_multicast_r18_s_lc_ch_id_r18_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(32),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(320),
     UINT64_C(65855),
     nullptr}};
static const asn1::table::field_name choice_table_mrb_rlc_cfg_multicast_r18_s_lc_ch_id_r18_c__names[] = {
    {"logicalChannelIdentitymulticast-r18", nullptr},
    {"logicalChannelIdentityExt-r18", nullptr}};
const asn1::table::choice_desc mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::desc = {
    asn1::table::enum_int_type<mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::types>(),
    mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::types::nof_types,
    mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::types::nof_exts,
    mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::types::has_ext,
    false,
    choice_table_mrb_rlc_cfg_multicast_r18_s_lc_ch_id_r18_c__alternatives,
    nullptr,
    choice_table_mrb_rlc_cfg_multicast_r18_s_lc_ch_id_r18_c__names,
    "mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_",
    offsetof(mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_, c.type),
    offsetof(mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_, c.value),
    false,
    &mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::storage_desc};

static const char* enum_names_mrb_rlc_cfg_multicast_r18_s_lc_ch_id_r18_c__types_values[] = {
    "logicalChannelIdentitymulticast-r18",
    "logicalChannelIdentityExt-r18"};
const asn1::table::enum_desc mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::types_opts::desc = {
    enum_names_mrb_rlc_cfg_multicast_r18_s_lc_ch_id_r18_c__types_values,
    2,
    "mrb_rlc_cfg_multicast_r18_s::lc_ch_id_r18_c_::types"};

static const char*           enum_names_mrb_rlc_cfg_multicast_r18_s_sn_field_len_r18_e__values[] = {"size6", "size12"};
const asn1::table::enum_desc mrb_rlc_cfg_multicast_r18_s::sn_field_len_r18_opts::desc            = {
    enum_names_mrb_rlc_cfg_multicast_r18_s_sn_field_len_r18_e__values,
    2,
    "mrb_rlc_cfg_multicast_r18_s::sn_field_len_r18_e_"};
uint8_t mrb_rlc_cfg_multicast_r18_s::sn_field_len_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 12};
  return map_enum_number(numbers, 2, value, "mrb_rlc_cfg_multicast_r18_s::sn_field_len_r18_e_");
}

// MRB-InfoMulticast-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mrb_info_multicast_r18_s_fields[] = {
    {offsetof(mrb_info_multicast_r18_s, pdcp_cfg_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mrb_pdcp_cfg_multicast_r18_s::desc},
    {offsetof(mrb_info_multicast_r18_s, rlc_cfg_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mrb_rlc_cfg_multicast_r18_s::desc}};
static const asn1::table::field_name seq_table_mrb_info_multicast_r18_s_names[] = {{"pdcp-Config-r18", nullptr},
                                                                                   {"rlc-Config-r18", nullptr}};
const asn1::table::seq_desc          mrb_info_multicast_r18_s::desc             = {
    {seq_table_mrb_info_multicast_r18_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_mrb_info_multicast_r18_s_names};

// MBS-SessionInfoMulticast-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_session_info_multicast_r18_s_fields[] = {
    {offsetof(mbs_session_info_multicast_r18_s, mbs_session_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tmgi_r17_s::desc},
    {offsetof(mbs_session_info_multicast_r18_s, g_rnti_r18),
     (int32_t)offsetof(mbs_session_info_multicast_r18_s, g_rnti_r18_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(mbs_session_info_multicast_r18_s, mrb_list_multicast_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(mbs_session_info_multicast_r18_s::mrb_list_multicast_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mrb_info_multicast_r18_s::desc},
    {offsetof(mbs_session_info_multicast_r18_s, mtch_sched_info_r18),
     (int32_t)offsetof(mbs_session_info_multicast_r18_s, mtch_sched_info_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(mbs_session_info_multicast_r18_s, mtch_neighbour_cell_r18),
     (int32_t)offsetof(mbs_session_info_multicast_r18_s, mtch_neighbour_cell_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(mbs_session_info_multicast_r18_s::mtch_neighbour_cell_r18),
                                asn1::table::struct_json>::value},
    {offsetof(mbs_session_info_multicast_r18_s, pdsch_cfg_idx_r18),
     (int32_t)offsetof(mbs_session_info_multicast_r18_s, pdsch_cfg_idx_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {offsetof(mbs_session_info_multicast_r18_s, mtch_ssb_map_win_idx_r18),
     (int32_t)offsetof(mbs_session_info_multicast_r18_s, mtch_ssb_map_win_idx_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(15),
     nullptr},
    {offsetof(mbs_session_info_multicast_r18_s, thres_idx_r18),
     (int32_t)offsetof(mbs_session_info_multicast_r18_s, thres_idx_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {0,
     (int32_t)offsetof(mbs_session_info_multicast_r18_s, pdcp_sync_ind_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mbs_session_info_multicast_r18_s, stop_monitoring_rnti_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_mbs_session_info_multicast_r18_s_names[] = {
    {"mbs-SessionId-r18", nullptr},
    {"g-RNTI-r18", nullptr},
    {"mrb-ListMulticast-r18", nullptr},
    {"mtch-SchedulingInfo-r18", nullptr},
    {"mtch-NeighbourCell-r18", nullptr},
    {"pdsch-ConfigIndex-r18", nullptr},
    {"mtch-SSB-MappingWindowIndex-r18", nullptr},
    {"thresholdIndex-r18", nullptr},
    {"pdcp-SyncIndicator-r18", "true"},
    {"stopMonitoringRNTI-r18", "true"}};
static const asn1::table::seq_opt seq_table_mbs_session_info_multicast_r18_s_opts[] = {
    {1, (int32_t)offsetof(mbs_session_info_multicast_r18_s, g_rnti_r18_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(mbs_session_info_multicast_r18_s, mtch_sched_info_r18_present)},
    {4, (int32_t)offsetof(mbs_session_info_multicast_r18_s, mtch_neighbour_cell_r18_present)},
    {5, (int32_t)offsetof(mbs_session_info_multicast_r18_s, pdsch_cfg_idx_r18_present)},
    {6, (int32_t)offsetof(mbs_session_info_multicast_r18_s, mtch_ssb_map_win_idx_r18_present)},
    {7, (int32_t)offsetof(mbs_session_info_multicast_r18_s, thres_idx_r18_present)},
    {8, (int32_t)offsetof(mbs_session_info_multicast_r18_s, pdcp_sync_ind_r18_present)},
    {9, (int32_t)offsetof(mbs_session_info_multicast_r18_s, stop_monitoring_rnti_r18_present)}};
const asn1::table::seq_desc mbs_session_info_multicast_r18_s::desc = {
    {seq_table_mbs_session_info_multicast_r18_s_fields,
     10,
     UINT64_C(0x1),
     seq_table_mbs_session_info_multicast_r18_s_opts,
     9},
    true,
    false,
    nullptr,
    0,
    seq_table_mbs_session_info_multicast_r18_s_names};

// MTCH-SSB-MappingWindowCycleOffset-r17 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_mtch_ssb_map_win_cycle_offset_r17_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc mtch_ssb_map_win_cycle_offset_r17_c::storage_desc = {
    choice_storage_mtch_ssb_map_win_cycle_offset_r17_c_alternatives,
    6,
    "mtch_ssb_map_win_cycle_offset_r17_c"};

uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint16_t& mtch_ssb_map_win_cycle_offset_r17_c::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field choice_table_mtch_ssb_map_win_cycle_offset_r17_c_alternatives[] = {
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
static const asn1::table::field_name choice_table_mtch_ssb_map_win_cycle_offset_r17_c_names[] = {{"ms10", nullptr},
                                                                                                 {"ms20", nullptr},
                                                                                                 {"ms32", nullptr},
                                                                                                 {"ms64", nullptr},
                                                                                                 {"ms128", nullptr},
                                                                                                 {"ms256", nullptr}};
const asn1::table::choice_desc       mtch_ssb_map_win_cycle_offset_r17_c::desc                = {
    asn1::table::enum_int_type<mtch_ssb_map_win_cycle_offset_r17_c::types>(),
    mtch_ssb_map_win_cycle_offset_r17_c::types::nof_types,
    mtch_ssb_map_win_cycle_offset_r17_c::types::nof_exts,
    mtch_ssb_map_win_cycle_offset_r17_c::types::has_ext,
    false,
    choice_table_mtch_ssb_map_win_cycle_offset_r17_c_alternatives,
    nullptr,
    choice_table_mtch_ssb_map_win_cycle_offset_r17_c_names,
    "mtch_ssb_map_win_cycle_offset_r17_c",
    offsetof(mtch_ssb_map_win_cycle_offset_r17_c, c.type),
    offsetof(mtch_ssb_map_win_cycle_offset_r17_c, c.value),
    false,
    &mtch_ssb_map_win_cycle_offset_r17_c::storage_desc};

static const char* enum_names_mtch_ssb_map_win_cycle_offset_r17_c_types_values[] =
    {"ms10", "ms20", "ms32", "ms64", "ms128", "ms256"};
const asn1::table::enum_desc mtch_ssb_map_win_cycle_offset_r17_c::types_opts::desc = {
    enum_names_mtch_ssb_map_win_cycle_offset_r17_c_types_values,
    6,
    "mtch_ssb_map_win_cycle_offset_r17_c::types"};
uint16_t mtch_ssb_map_win_cycle_offset_r17_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 20, 32, 64, 128, 256};
  return map_enum_number(numbers, 6, value, "mtch_ssb_map_win_cycle_offset_r17_c::types");
}

// MBSBroadcastConfiguration-r17-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_broadcast_cfg_r17_ies_s_fields[] = {
    {offsetof(mbs_broadcast_cfg_r17_ies_s, mbs_session_info_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::item_list_codec<decltype(mbs_broadcast_cfg_r17_ies_s::mbs_session_info_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mbs_session_info_r17_s::desc},
    {offsetof(mbs_broadcast_cfg_r17_ies_s, mbs_neighbour_cell_list_r17),
     (int32_t)offsetof(mbs_broadcast_cfg_r17_ies_s, mbs_neighbour_cell_list_r17_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     8,
     &asn1::table::item_list_codec<decltype(mbs_broadcast_cfg_r17_ies_s::mbs_neighbour_cell_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mbs_neighbour_cell_r17_s::desc},
    {offsetof(mbs_broadcast_cfg_r17_ies_s, drx_cfg_ptm_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mbs_broadcast_cfg_r17_ies_s::drx_cfg_ptm_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drx_cfg_ptm_r17_s::desc},
    {offsetof(mbs_broadcast_cfg_r17_ies_s, pdsch_cfg_mtch_r17),
     (int32_t)offsetof(mbs_broadcast_cfg_r17_ies_s, pdsch_cfg_mtch_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdsch_cfg_broadcast_r17_s::desc},
    {offsetof(mbs_broadcast_cfg_r17_ies_s, mtch_ssb_map_win_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(mbs_broadcast_cfg_r17_ies_s::mtch_ssb_map_win_list_r17),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &mtch_ssb_map_win_cycle_offset_r17_c::desc},
    {offsetof(mbs_broadcast_cfg_r17_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(mbs_broadcast_cfg_r17_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(mbs_broadcast_cfg_r17_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_mbs_broadcast_cfg_r17_ies_s_names[] = {
    {"mbs-SessionInfoList-r17", nullptr},
    {"mbs-NeighbourCellList-r17", nullptr},
    {"drx-ConfigPTM-List-r17", nullptr},
    {"pdsch-ConfigMTCH-r17", nullptr},
    {"mtch-SSB-MappingWindowList-r17", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_broadcast_cfg_r17_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(mbs_broadcast_cfg_r17_ies_s, mbs_neighbour_cell_list_r17_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(mbs_broadcast_cfg_r17_ies_s, pdsch_cfg_mtch_r17_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(mbs_broadcast_cfg_r17_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc mbs_broadcast_cfg_r17_ies_s::desc = {
    {seq_table_mbs_broadcast_cfg_r17_ies_s_fields, 7, UINT64_C(0x0), seq_table_mbs_broadcast_cfg_r17_ies_s_opts, 7},
    false,
    false,
    nullptr,
    0,
    seq_table_mbs_broadcast_cfg_r17_ies_s_names};

// MBSBroadcastConfiguration-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_broadcast_cfg_r17_s_fields[] = {
    {offsetof(mbs_broadcast_cfg_r17_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_broadcast_cfg_r17_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_mbs_broadcast_cfg_r17_s_names[] = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          mbs_broadcast_cfg_r17_s::desc             = {
    {seq_table_mbs_broadcast_cfg_r17_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_mbs_broadcast_cfg_r17_s_names};

void mbs_broadcast_cfg_r17_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
mbs_broadcast_cfg_r17_ies_s& mbs_broadcast_cfg_r17_s::crit_exts_c_::set_mbs_broadcast_cfg_r17()
{
  set(types::mbs_broadcast_cfg_r17);
  return c;
}
void mbs_broadcast_cfg_r17_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_mbs_broadcast_cfg_r17_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_broadcast_cfg_r17_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_mbs_broadcast_cfg_r17_s_crit_exts_c__names[] = {
    {"mbsBroadcastConfiguration-r17", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc mbs_broadcast_cfg_r17_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<mbs_broadcast_cfg_r17_s::crit_exts_c_::types>(),
    mbs_broadcast_cfg_r17_s::crit_exts_c_::types::nof_types,
    mbs_broadcast_cfg_r17_s::crit_exts_c_::types::nof_exts,
    mbs_broadcast_cfg_r17_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_mbs_broadcast_cfg_r17_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<mbs_broadcast_cfg_r17_s::crit_exts_c_>,
    choice_table_mbs_broadcast_cfg_r17_s_crit_exts_c__names,
    "mbs_broadcast_cfg_r17_s::crit_exts_c_",
    offsetof(mbs_broadcast_cfg_r17_s::crit_exts_c_, type_),
    offsetof(mbs_broadcast_cfg_r17_s::crit_exts_c_, c),
    false};

static const char* enum_names_mbs_broadcast_cfg_r17_s_crit_exts_c__types_values[]    = {"mbsBroadcastConfiguration-r17",
                                                                                        "criticalExtensionsFuture"};
const asn1::table::enum_desc mbs_broadcast_cfg_r17_s::crit_exts_c_::types_opts::desc = {
    enum_names_mbs_broadcast_cfg_r17_s_crit_exts_c__types_values,
    2,
    "mbs_broadcast_cfg_r17_s::crit_exts_c_::types"};

// ThresholdMBS-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_thres_mbs_r18_s_fields[] = {
    {offsetof(thres_mbs_r18_s, rsrp_r18),
     (int32_t)offsetof(thres_mbs_r18_s, rsrp_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(thres_mbs_r18_s, rsrq_r18),
     (int32_t)offsetof(thres_mbs_r18_s, rsrq_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr}};
static const asn1::table::field_name seq_table_thres_mbs_r18_s_names[] = {{"rsrp-r18", nullptr}, {"rsrq-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_thres_mbs_r18_s_opts[]  = {
    {0, (int32_t)offsetof(thres_mbs_r18_s, rsrp_r18_present)},
    {1, (int32_t)offsetof(thres_mbs_r18_s, rsrq_r18_present)}};
const asn1::table::seq_desc thres_mbs_r18_s::desc = {
    {seq_table_thres_mbs_r18_s_fields, 2, UINT64_C(0x0), seq_table_thres_mbs_r18_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_thres_mbs_r18_s_names};

// MBSMulticastConfiguration-r18-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_multicast_cfg_r18_ies_s_fields[] = {
    {offsetof(mbs_multicast_cfg_r18_ies_s, mbs_session_info_list_multicast_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::item_list_codec<decltype(mbs_multicast_cfg_r18_ies_s::mbs_session_info_list_multicast_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mbs_session_info_multicast_r18_s::desc},
    {offsetof(mbs_multicast_cfg_r18_ies_s, mbs_neighbour_cell_list_r18),
     (int32_t)offsetof(mbs_multicast_cfg_r18_ies_s, mbs_neighbour_cell_list_r18_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     8,
     &asn1::table::item_list_codec<decltype(mbs_multicast_cfg_r18_ies_s::mbs_neighbour_cell_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &mbs_neighbour_cell_r17_s::desc},
    {offsetof(mbs_multicast_cfg_r18_ies_s, drx_cfg_ptm_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mbs_multicast_cfg_r18_ies_s::drx_cfg_ptm_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &drx_cfg_ptm_r17_s::desc},
    {offsetof(mbs_multicast_cfg_r18_ies_s, pdsch_cfg_mtch_r18),
     (int32_t)offsetof(mbs_multicast_cfg_r18_ies_s, pdsch_cfg_mtch_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdsch_cfg_broadcast_r17_s::desc},
    {offsetof(mbs_multicast_cfg_r18_ies_s, mtch_ssb_map_win_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(mbs_multicast_cfg_r18_ies_s::mtch_ssb_map_win_list_r18),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &mtch_ssb_map_win_cycle_offset_r17_c::desc},
    {offsetof(mbs_multicast_cfg_r18_ies_s, thres_mbs_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(mbs_multicast_cfg_r18_ies_s::thres_mbs_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &thres_mbs_r18_s::desc},
    {offsetof(mbs_multicast_cfg_r18_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(mbs_multicast_cfg_r18_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(mbs_multicast_cfg_r18_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_mbs_multicast_cfg_r18_ies_s_names[] = {
    {"mbs-SessionInfoListMulticast-r18", nullptr},
    {"mbs-NeighbourCellList-r18", nullptr},
    {"drx-ConfigPTM-List-r18", nullptr},
    {"pdsch-ConfigMTCH-r18", nullptr},
    {"mtch-SSB-MappingWindowList-r18", nullptr},
    {"thresholdMBS-List-r18", nullptr},
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_mbs_multicast_cfg_r18_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(mbs_multicast_cfg_r18_ies_s, mbs_neighbour_cell_list_r18_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(mbs_multicast_cfg_r18_ies_s, pdsch_cfg_mtch_r18_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, asn1::table::seq_field::list_presence},
    {7, (int32_t)offsetof(mbs_multicast_cfg_r18_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc mbs_multicast_cfg_r18_ies_s::desc = {
    {seq_table_mbs_multicast_cfg_r18_ies_s_fields, 8, UINT64_C(0x0), seq_table_mbs_multicast_cfg_r18_ies_s_opts, 8},
    false,
    false,
    nullptr,
    0,
    seq_table_mbs_multicast_cfg_r18_ies_s_names};

// MBSMulticastConfiguration-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mbs_multicast_cfg_r18_s_fields[] = {
    {offsetof(mbs_multicast_cfg_r18_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_multicast_cfg_r18_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_mbs_multicast_cfg_r18_s_names[] = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          mbs_multicast_cfg_r18_s::desc             = {
    {seq_table_mbs_multicast_cfg_r18_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_mbs_multicast_cfg_r18_s_names};

void mbs_multicast_cfg_r18_s::crit_exts_c_::set(types::options e)
{
  type_ = e;
}
mbs_multicast_cfg_r18_ies_s& mbs_multicast_cfg_r18_s::crit_exts_c_::set_mbs_multicast_cfg_r18()
{
  set(types::mbs_multicast_cfg_r18);
  return c;
}
void mbs_multicast_cfg_r18_s::crit_exts_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field choice_table_mbs_multicast_cfg_r18_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_multicast_cfg_r18_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_mbs_multicast_cfg_r18_s_crit_exts_c__names[] = {
    {"mbsMulticastConfiguration-r18", nullptr},
    {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc mbs_multicast_cfg_r18_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<mbs_multicast_cfg_r18_s::crit_exts_c_::types>(),
    mbs_multicast_cfg_r18_s::crit_exts_c_::types::nof_types,
    mbs_multicast_cfg_r18_s::crit_exts_c_::types::nof_exts,
    mbs_multicast_cfg_r18_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_mbs_multicast_cfg_r18_s_crit_exts_c__alternatives,
    &asn1::table::choice_set<mbs_multicast_cfg_r18_s::crit_exts_c_>,
    choice_table_mbs_multicast_cfg_r18_s_crit_exts_c__names,
    "mbs_multicast_cfg_r18_s::crit_exts_c_",
    offsetof(mbs_multicast_cfg_r18_s::crit_exts_c_, type_),
    offsetof(mbs_multicast_cfg_r18_s::crit_exts_c_, c),
    false};

static const char* enum_names_mbs_multicast_cfg_r18_s_crit_exts_c__types_values[]    = {"mbsMulticastConfiguration-r18",
                                                                                        "criticalExtensionsFuture"};
const asn1::table::enum_desc mbs_multicast_cfg_r18_s::crit_exts_c_::types_opts::desc = {
    enum_names_mbs_multicast_cfg_r18_s_crit_exts_c__types_values,
    2,
    "mbs_multicast_cfg_r18_s::crit_exts_c_::types"};

// MCCH-MessageType-r17 ::= CHOICE
void mcch_msg_type_r17_c::set(types::options e)
{
  type_ = e;
}
mcch_msg_type_r17_c::c1_c_& mcch_msg_type_r17_c::set_c1()
{
  set(types::c1);
  return c;
}
void mcch_msg_type_r17_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
static const asn1::table::seq_field choice_table_mcch_msg_type_r17_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &mcch_msg_type_r17_c::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_mcch_msg_type_r17_c_names[] = {{"c1", nullptr},
                                                                                 {"messageClassExtension", nullptr}};
const asn1::table::choice_desc mcch_msg_type_r17_c::desc = {asn1::table::enum_int_type<mcch_msg_type_r17_c::types>(),
                                                            mcch_msg_type_r17_c::types::nof_types,
                                                            mcch_msg_type_r17_c::types::nof_exts,
                                                            mcch_msg_type_r17_c::types::has_ext,
                                                            false,
                                                            choice_table_mcch_msg_type_r17_c_alternatives,
                                                            &asn1::table::choice_set<mcch_msg_type_r17_c>,
                                                            choice_table_mcch_msg_type_r17_c_names,
                                                            "mcch_msg_type_r17_c",
                                                            offsetof(mcch_msg_type_r17_c, type_),
                                                            offsetof(mcch_msg_type_r17_c, c),
                                                            false};

void mcch_msg_type_r17_c::c1_c_::set(types::options e)
{
  type_ = e;
}
mbs_broadcast_cfg_r17_s& mcch_msg_type_r17_c::c1_c_::set_mbs_broadcast_cfg_r17()
{
  set(types::mbs_broadcast_cfg_r17);
  return c;
}
void mcch_msg_type_r17_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_mcch_msg_type_r17_c_c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_broadcast_cfg_r17_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_mcch_msg_type_r17_c_c1_c__names[] = {
    {"mbsBroadcastConfiguration-r17", nullptr},
    {"spare1", nullptr}};
const asn1::table::choice_desc mcch_msg_type_r17_c::c1_c_::desc = {
    asn1::table::enum_int_type<mcch_msg_type_r17_c::c1_c_::types>(),
    mcch_msg_type_r17_c::c1_c_::types::nof_types,
    mcch_msg_type_r17_c::c1_c_::types::nof_exts,
    mcch_msg_type_r17_c::c1_c_::types::has_ext,
    false,
    choice_table_mcch_msg_type_r17_c_c1_c__alternatives,
    &asn1::table::choice_set<mcch_msg_type_r17_c::c1_c_>,
    choice_table_mcch_msg_type_r17_c_c1_c__names,
    "mcch_msg_type_r17_c::c1_c_",
    offsetof(mcch_msg_type_r17_c::c1_c_, type_),
    offsetof(mcch_msg_type_r17_c::c1_c_, c),
    false};

static const char* enum_names_mcch_msg_type_r17_c_c1_c__types_values[]    = {"mbsBroadcastConfiguration-r17", "spare1"};
const asn1::table::enum_desc mcch_msg_type_r17_c::c1_c_::types_opts::desc = {
    enum_names_mcch_msg_type_r17_c_c1_c__types_values,
    2,
    "mcch_msg_type_r17_c::c1_c_::types"};

static const char*           enum_names_mcch_msg_type_r17_c_types_values[] = {"c1", "messageClassExtension"};
const asn1::table::enum_desc mcch_msg_type_r17_c::types_opts::desc = {enum_names_mcch_msg_type_r17_c_types_values,
                                                                      2,
                                                                      "mcch_msg_type_r17_c::types"};
uint8_t                      mcch_msg_type_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "mcch_msg_type_r17_c::types");
}

// MCCH-Message-r17 ::= SEQUENCE
static const asn1::table::seq_field  seq_table_mcch_msg_r17_s_fields[] = {{offsetof(mcch_msg_r17_s, msg),
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::field_op::choice,
                                                                           false,
                                                                           false,
                                                                           0,
                                                                           0,
                                                                           nullptr,
                                                                           &mcch_msg_type_r17_c::desc}};
static const asn1::table::field_name seq_table_mcch_msg_r17_s_names[]  = {{"message", nullptr}};
const asn1::table::seq_desc mcch_msg_r17_s::desc = {{seq_table_mcch_msg_r17_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0,
                                                    seq_table_mcch_msg_r17_s_names,
                                                    "MCCH-Message-r17"};

// MeasAndMobParametersCommon-v15t0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_and_mob_params_common_v15t0_s_fields[] = {
    {0,
     (int32_t)offsetof(meas_and_mob_params_common_v15t0_s, intra_f_neigh_meas_for_scell_without_ssb_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_meas_and_mob_params_common_v15t0_s_names[] = {
    {"intraF-NeighMeasForSCellWithoutSSB", "supported"}};
static const asn1::table::seq_opt seq_table_meas_and_mob_params_common_v15t0_s_opts[] = {
    {0, (int32_t)offsetof(meas_and_mob_params_common_v15t0_s, intra_f_neigh_meas_for_scell_without_ssb_present)}};
const asn1::table::seq_desc meas_and_mob_params_common_v15t0_s::desc = {
    {seq_table_meas_and_mob_params_common_v15t0_s_fields,
     1,
     UINT64_C(0x0),
     seq_table_meas_and_mob_params_common_v15t0_s_opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_and_mob_params_common_v15t0_s_names};

// MeasAndMobParameters-v15t0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_and_mob_params_v15t0_s_fields[] = {
    {offsetof(meas_and_mob_params_v15t0_s, meas_and_mob_params_common_v15t0),
     (int32_t)offsetof(meas_and_mob_params_v15t0_s, meas_and_mob_params_common_v15t0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_and_mob_params_common_v15t0_s::desc}};
static const asn1::table::field_name seq_table_meas_and_mob_params_v15t0_s_names[] = {
    {"measAndMobParametersCommon-v15t0", nullptr}};
static const asn1::table::seq_opt seq_table_meas_and_mob_params_v15t0_s_opts[] = {
    {0, (int32_t)offsetof(meas_and_mob_params_v15t0_s, meas_and_mob_params_common_v15t0_present)}};
const asn1::table::seq_desc meas_and_mob_params_v15t0_s::desc = {
    {seq_table_meas_and_mob_params_v15t0_s_fields, 1, UINT64_C(0x0), seq_table_meas_and_mob_params_v15t0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_meas_and_mob_params_v15t0_s_names};

// MeasResultSCG-Failure ::= SEQUENCE
static const asn1::table::seq_field seq_table_meas_result_scg_fail_s_fields[] = {
    {offsetof(meas_result_scg_fail_s, meas_result_per_mo_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(meas_result_scg_fail_s::meas_result_per_mo_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &meas_result2_nr_s::desc}};
static const asn1::table::field_name seq_table_meas_result_scg_fail_s_names[]     = {{"measResultPerMOList", nullptr}};
static const asn1::table::seq_field  seq_table_meas_result_scg_fail_s_g0_fields[] = {
    {offsetof(meas_result_scg_fail_s, location_info_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(meas_result_scg_fail_s::location_info_r16)>::value,
      &location_info_r16_s::desc}};
static const asn1::table::field_name seq_table_meas_result_scg_fail_s_g0_names[] = {{"locationInfo-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_meas_result_scg_fail_s_g0_opts[]  = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_meas_result_scg_fail_s_groups[] = {
    {{seq_table_meas_result_scg_fail_s_g0_fields, 1, UINT64_C(0x0), seq_table_meas_result_scg_fail_s_g0_opts, 1},
     false,
     seq_table_meas_result_scg_fail_s_g0_names}};
const asn1::table::seq_desc meas_result_scg_fail_s::desc = {
    {seq_table_meas_result_scg_fail_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    seq_table_meas_result_scg_fail_s_groups,
    1,
    seq_table_meas_result_scg_fail_s_names};

// MulticastMCCH-MessageType-r18 ::= CHOICE
void multicast_mcch_msg_type_r18_c::set(types::options e)
{
  type_ = e;
}
multicast_mcch_msg_type_r18_c::c1_c_& multicast_mcch_msg_type_r18_c::set_c1()
{
  set(types::c1);
  return c;
}
void multicast_mcch_msg_type_r18_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
static const asn1::table::seq_field choice_table_multicast_mcch_msg_type_r18_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &multicast_mcch_msg_type_r18_c::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_multicast_mcch_msg_type_r18_c_names[] = {
    {"c1", nullptr},
    {"messageClassExtension", nullptr}};
const asn1::table::choice_desc multicast_mcch_msg_type_r18_c::desc = {
    asn1::table::enum_int_type<multicast_mcch_msg_type_r18_c::types>(),
    multicast_mcch_msg_type_r18_c::types::nof_types,
    multicast_mcch_msg_type_r18_c::types::nof_exts,
    multicast_mcch_msg_type_r18_c::types::has_ext,
    false,
    choice_table_multicast_mcch_msg_type_r18_c_alternatives,
    &asn1::table::choice_set<multicast_mcch_msg_type_r18_c>,
    choice_table_multicast_mcch_msg_type_r18_c_names,
    "multicast_mcch_msg_type_r18_c",
    offsetof(multicast_mcch_msg_type_r18_c, type_),
    offsetof(multicast_mcch_msg_type_r18_c, c),
    false};

void multicast_mcch_msg_type_r18_c::c1_c_::set(types::options e)
{
  type_ = e;
}
mbs_multicast_cfg_r18_s& multicast_mcch_msg_type_r18_c::c1_c_::set_mbs_multicast_cfg_r18()
{
  set(types::mbs_multicast_cfg_r18);
  return c;
}
void multicast_mcch_msg_type_r18_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_multicast_mcch_msg_type_r18_c_c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mbs_multicast_cfg_r18_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_multicast_mcch_msg_type_r18_c_c1_c__names[] = {
    {"mbsMulticastConfiguration-r18", nullptr},
    {"spare1", nullptr}};
const asn1::table::choice_desc multicast_mcch_msg_type_r18_c::c1_c_::desc = {
    asn1::table::enum_int_type<multicast_mcch_msg_type_r18_c::c1_c_::types>(),
    multicast_mcch_msg_type_r18_c::c1_c_::types::nof_types,
    multicast_mcch_msg_type_r18_c::c1_c_::types::nof_exts,
    multicast_mcch_msg_type_r18_c::c1_c_::types::has_ext,
    false,
    choice_table_multicast_mcch_msg_type_r18_c_c1_c__alternatives,
    &asn1::table::choice_set<multicast_mcch_msg_type_r18_c::c1_c_>,
    choice_table_multicast_mcch_msg_type_r18_c_c1_c__names,
    "multicast_mcch_msg_type_r18_c::c1_c_",
    offsetof(multicast_mcch_msg_type_r18_c::c1_c_, type_),
    offsetof(multicast_mcch_msg_type_r18_c::c1_c_, c),
    false};

static const char* enum_names_multicast_mcch_msg_type_r18_c_c1_c__types_values[]    = {"mbsMulticastConfiguration-r18",
                                                                                       "spare1"};
const asn1::table::enum_desc multicast_mcch_msg_type_r18_c::c1_c_::types_opts::desc = {
    enum_names_multicast_mcch_msg_type_r18_c_c1_c__types_values,
    2,
    "multicast_mcch_msg_type_r18_c::c1_c_::types"};

static const char*           enum_names_multicast_mcch_msg_type_r18_c_types_values[] = {"c1", "messageClassExtension"};
const asn1::table::enum_desc multicast_mcch_msg_type_r18_c::types_opts::desc         = {
    enum_names_multicast_mcch_msg_type_r18_c_types_values,
    2,
    "multicast_mcch_msg_type_r18_c::types"};
uint8_t multicast_mcch_msg_type_r18_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "multicast_mcch_msg_type_r18_c::types");
}

// MulticastMCCH-Message-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_multicast_mcch_msg_r18_s_fields[] = {
    {offsetof(multicast_mcch_msg_r18_s, msg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &multicast_mcch_msg_type_r18_c::desc}};
static const asn1::table::field_name seq_table_multicast_mcch_msg_r18_s_names[] = {{"message", nullptr}};
const asn1::table::seq_desc          multicast_mcch_msg_r18_s::desc             = {
    {seq_table_multicast_mcch_msg_r18_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_multicast_mcch_msg_r18_s_names,
    "MulticastMCCH-Message-r18"};

// NRDC-Parameters-v15c0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_nrdc_params_v15c0_s_fields[] = {
    {0,
     (int32_t)offsetof(nrdc_params_v15c0_s, pdcp_dupl_split_srb_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(nrdc_params_v15c0_s, pdcp_dupl_split_drb_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_nrdc_params_v15c0_s_names[] = {
    {"pdcp-DuplicationSplitSRB", "supported"},
    {"pdcp-DuplicationSplitDRB", "supported"}};
static const asn1::table::seq_opt seq_table_nrdc_params_v15c0_s_opts[] = {
    {0, (int32_t)offsetof(nrdc_params_v15c0_s, pdcp_dupl_split_srb_present)},
    {1, (int32_t)offsetof(nrdc_params_v15c0_s, pdcp_dupl_split_drb_present)}};
const asn1::table::seq_desc nrdc_params_v15c0_s::desc = {
    {seq_table_nrdc_params_v15c0_s_fields, 2, UINT64_C(0x0), seq_table_nrdc_params_v15c0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_nrdc_params_v15c0_s_names};

// Phy-ParametersCommon-v16a0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_phy_params_common_v16a0_s_fields[] = {
    {0,
     (int32_t)offsetof(phy_params_common_v16a0_s, srs_periodicity_and_offset_ext_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_phy_params_common_v16a0_s_names[] = {
    {"srs-PeriodicityAndOffsetExt-r16", "supported"}};
static const asn1::table::seq_opt seq_table_phy_params_common_v16a0_s_opts[] = {
    {0, (int32_t)offsetof(phy_params_common_v16a0_s, srs_periodicity_and_offset_ext_r16_present)}};
const asn1::table::seq_desc phy_params_common_v16a0_s::desc = {
    {seq_table_phy_params_common_v16a0_s_fields, 1, UINT64_C(0x0), seq_table_phy_params_common_v16a0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_phy_params_common_v16a0_s_names};

// Phy-Parameters-v16a0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_phy_params_v16a0_s_fields[] = {
    {offsetof(phy_params_v16a0_s, phy_params_common_v16a0),
     (int32_t)offsetof(phy_params_v16a0_s, phy_params_common_v16a0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &phy_params_common_v16a0_s::desc}};
static const asn1::table::field_name seq_table_phy_params_v16a0_s_names[] = {{"phy-ParametersCommon-v16a0", nullptr}};
static const asn1::table::seq_opt    seq_table_phy_params_v16a0_s_opts[]  = {
    {0, (int32_t)offsetof(phy_params_v16a0_s, phy_params_common_v16a0_present)}};
const asn1::table::seq_desc phy_params_v16a0_s::desc = {
    {seq_table_phy_params_v16a0_s_fields, 1, UINT64_C(0x0), seq_table_phy_params_v16a0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_phy_params_v16a0_s_names};

// RF-Parameters-v15g0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_rf_params_v15g0_s_fields[] = {
    {offsetof(rf_params_v15g0_s, supported_band_combination_list_v15g0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65536,
     &asn1::table::item_list_codec<decltype(rf_params_v15g0_s::supported_band_combination_list_v15g0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &band_combination_v15g0_s::desc}};
static const asn1::table::field_name seq_table_rf_params_v15g0_s_names[] = {
    {"supportedBandCombinationList-v15g0", nullptr}};
static const asn1::table::seq_opt seq_table_rf_params_v15g0_s_opts[] = {{0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       rf_params_v15g0_s::desc            = {
    {seq_table_rf_params_v15g0_s_fields, 1, UINT64_C(0x0), seq_table_rf_params_v15g0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_rf_params_v15g0_s_names};

// RF-Parameters-v16a0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_rf_params_v16a0_s_fields[] = {
    {offsetof(rf_params_v16a0_s, supported_band_combination_list_v16a0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65536,
     &asn1::table::item_list_codec<decltype(rf_params_v16a0_s::supported_band_combination_list_v16a0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &band_combination_v16a0_s::desc},
    {offsetof(rf_params_v16a0_s, supported_band_combination_list_ul_tx_switch_v16a0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65536,
     &asn1::table::item_list_codec<decltype(rf_params_v16a0_s::supported_band_combination_list_ul_tx_switch_v16a0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &band_combination_ul_tx_switch_v16a0_s::desc}};
static const asn1::table::field_name seq_table_rf_params_v16a0_s_names[] = {
    {"supportedBandCombinationList-v16a0", nullptr},
    {"supportedBandCombinationList-UplinkTxSwitch-v16a0", nullptr}};
static const asn1::table::seq_opt seq_table_rf_params_v16a0_s_opts[] = {{0, asn1::table::seq_field::list_presence},
                                                                        {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       rf_params_v16a0_s::desc            = {
    {seq_table_rf_params_v16a0_s_fields, 2, UINT64_C(0x0), seq_table_rf_params_v16a0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_rf_params_v16a0_s_names};

// RF-Parameters-v16c0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_rf_params_v16c0_s_fields[] = {
    {offsetof(rf_params_v16c0_s, supported_band_list_nr_v16c0),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::item_list_codec<decltype(rf_params_v16c0_s::supported_band_list_nr_v16c0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &band_nr_v16c0_s::desc}};
static const asn1::table::field_name seq_table_rf_params_v16c0_s_names[] = {{"supportedBandListNR-v16c0", nullptr}};
const asn1::table::seq_desc          rf_params_v16c0_s::desc             = {
    {seq_table_rf_params_v16c0_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rf_params_v16c0_s_names};

// RF-Parameters-v16j0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_rf_params_v16j0_s_fields[] = {
    {offsetof(rf_params_v16j0_s, supported_band_combination_list_v16j0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65536,
     &asn1::table::item_list_codec<decltype(rf_params_v16j0_s::supported_band_combination_list_v16j0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &band_combination_v16j0_s::desc},
    {offsetof(rf_params_v16j0_s, supported_band_combination_list_ul_tx_switch_v16j0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65536,
     &asn1::table::item_list_codec<decltype(rf_params_v16j0_s::supported_band_combination_list_ul_tx_switch_v16j0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &band_combination_ul_tx_switch_v16j0_s::desc}};
static const asn1::table::field_name seq_table_rf_params_v16j0_s_names[] = {
    {"supportedBandCombinationList-v16j0", nullptr},
    {"supportedBandCombinationList-UplinkTxSwitch-v16j0", nullptr}};
static const asn1::table::seq_opt seq_table_rf_params_v16j0_s_opts[] = {{0, asn1::table::seq_field::list_presence},
                                                                        {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       rf_params_v16j0_s::desc            = {
    {seq_table_rf_params_v16j0_s_fields, 2, UINT64_C(0x0), seq_table_rf_params_v16j0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_rf_params_v16j0_s_names};

// RF-Parameters-v17b0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_rf_params_v17b0_s_fields[] = {
    {offsetof(rf_params_v17b0_s, supported_band_list_nr_v17b0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1024,
     &asn1::table::item_list_codec<decltype(rf_params_v17b0_s::supported_band_list_nr_v17b0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &band_nr_v17b0_s::desc},
    {offsetof(rf_params_v17b0_s, supported_band_combination_list_v17b0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65536,
     &asn1::table::item_list_codec<decltype(rf_params_v17b0_s::supported_band_combination_list_v17b0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &band_combination_v17b0_s::desc},
    {offsetof(rf_params_v17b0_s, supported_band_combination_list_ul_tx_switch_v17b0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     65536,
     &asn1::table::item_list_codec<decltype(rf_params_v17b0_s::supported_band_combination_list_ul_tx_switch_v17b0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &band_combination_ul_tx_switch_v17b0_s::desc}};
static const asn1::table::field_name seq_table_rf_params_v17b0_s_names[] = {
    {"supportedBandListNR-v17b0", nullptr},
    {"supportedBandCombinationList-v17b0", nullptr},
    {"supportedBandCombinationList-UplinkTxSwitch-v17b0", nullptr}};
static const asn1::table::seq_opt seq_table_rf_params_v17b0_s_opts[] = {{0, asn1::table::seq_field::list_presence},
                                                                        {1, asn1::table::seq_field::list_presence},
                                                                        {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       rf_params_v17b0_s::desc            = {
    {seq_table_rf_params_v17b0_s_fields, 3, UINT64_C(0x0), seq_table_rf_params_v17b0_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_rf_params_v17b0_s_names};

// SL-FreqConfigExt-v16k0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_freq_cfg_ext_v16k0_s_fields[] = {
    {offsetof(sl_freq_cfg_ext_v16k0_s, add_spec_emission_r16),
     (int32_t)offsetof(sl_freq_cfg_ext_v16k0_s, add_spec_emission_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr}};
static const asn1::table::field_name seq_table_sl_freq_cfg_ext_v16k0_s_names[] = {
    {"additionalSpectrumEmission-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_freq_cfg_ext_v16k0_s_opts[] = {
    {0, (int32_t)offsetof(sl_freq_cfg_ext_v16k0_s, add_spec_emission_r16_present)}};
const asn1::table::seq_desc sl_freq_cfg_ext_v16k0_s::desc = {
    {seq_table_sl_freq_cfg_ext_v16k0_s_fields, 1, UINT64_C(0x0), seq_table_sl_freq_cfg_ext_v16k0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_freq_cfg_ext_v16k0_s_names};

// SL-PHY-MAC-RLC-Config-v16k0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_phy_mac_rlc_cfg_v16k0_s_fields[] = {
    {offsetof(sl_phy_mac_rlc_cfg_v16k0_s, sl_freq_info_to_add_mod_list_ext_v16k0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sl_phy_mac_rlc_cfg_v16k0_s::sl_freq_info_to_add_mod_list_ext_v16k0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_freq_cfg_ext_v16k0_s::desc}};
static const asn1::table::field_name seq_table_sl_phy_mac_rlc_cfg_v16k0_s_names[] = {
    {"sl-FreqInfoToAddModListExt-v16k0", nullptr}};
static const asn1::table::seq_opt seq_table_sl_phy_mac_rlc_cfg_v16k0_s_opts[] = {
    {0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_phy_mac_rlc_cfg_v16k0_s::desc = {
    {seq_table_sl_phy_mac_rlc_cfg_v16k0_s_fields, 1, UINT64_C(0x0), seq_table_sl_phy_mac_rlc_cfg_v16k0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_phy_mac_rlc_cfg_v16k0_s_names};

// SL-ConfigDedicatedNR-v16k0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cfg_ded_nr_v16k0_s_fields[] = {
    {offsetof(sl_cfg_ded_nr_v16k0_s, sl_phy_mac_rlc_cfg_v16k0),
     (int32_t)offsetof(sl_cfg_ded_nr_v16k0_s, sl_phy_mac_rlc_cfg_v16k0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_phy_mac_rlc_cfg_v16k0_s::desc}};
static const asn1::table::field_name seq_table_sl_cfg_ded_nr_v16k0_s_names[] = {
    {"sl-PHY-MAC-RLC-Config-v16k0", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_ded_nr_v16k0_s_opts[] = {
    {0, (int32_t)offsetof(sl_cfg_ded_nr_v16k0_s, sl_phy_mac_rlc_cfg_v16k0_present)}};
const asn1::table::seq_desc sl_cfg_ded_nr_v16k0_s::desc = {
    {seq_table_sl_cfg_ded_nr_v16k0_s_fields, 1, UINT64_C(0x0), seq_table_sl_cfg_ded_nr_v16k0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_cfg_ded_nr_v16k0_s_names};

// RRCReconfiguration-v16k0-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_recfg_v16k0_ies_s_sl_cfg_ded_nr_v16k0_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_cfg_ded_nr_v16k0_s::desc}};
static const asn1::table::field_name seq_table_rrc_recfg_v16k0_ies_s_sl_cfg_ded_nr_v16k0_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_rrc_recfg_v16k0_ies_s_sl_cfg_ded_nr_v16k0_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_rrc_recfg_v16k0_ies_s_sl_cfg_ded_nr_v16k0_setup_release_alternatives,
    nullptr,
    seq_table_rrc_recfg_v16k0_ies_s_sl_cfg_ded_nr_v16k0_setup_release_names,
    "setup_release_c",
    setup_release_c<sl_cfg_ded_nr_v16k0_s>::type_offset(),
    setup_release_c<sl_cfg_ded_nr_v16k0_s>::value_offset()};
static const asn1::table::seq_field seq_table_rrc_recfg_v16k0_ies_s_fields[] = {
    {offsetof(rrc_recfg_v16k0_ies_s, sl_cfg_ded_nr_v16k0),
     (int32_t)offsetof(rrc_recfg_v16k0_ies_s, sl_cfg_ded_nr_v16k0_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_rrc_recfg_v16k0_ies_s_sl_cfg_ded_nr_v16k0_setup_release},
    {0,
     (int32_t)offsetof(rrc_recfg_v16k0_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_rrc_recfg_v16k0_ies_s_names[] = {{"sl-ConfigDedicatedNR-v16k0", nullptr},
                                                                                {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_rrc_recfg_v16k0_ies_s_opts[]  = {
    {0, (int32_t)offsetof(rrc_recfg_v16k0_ies_s, sl_cfg_ded_nr_v16k0_present)},
    {1, (int32_t)offsetof(rrc_recfg_v16k0_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_recfg_v16k0_ies_s::desc = {
    {seq_table_rrc_recfg_v16k0_ies_s_fields, 2, UINT64_C(0x0), seq_table_rrc_recfg_v16k0_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_recfg_v16k0_ies_s_names};

// RRCReconfiguration-v15t0-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_recfg_v15t0_ies_s_fields[] = {
    {offsetof(rrc_recfg_v15t0_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(rrc_recfg_v15t0_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {offsetof(rrc_recfg_v15t0_ies_s, non_crit_ext),
     (int32_t)offsetof(rrc_recfg_v15t0_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_recfg_v16k0_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_recfg_v15t0_ies_s_names[] = {{"lateNonCriticalExtension", nullptr},
                                                                                {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_rrc_recfg_v15t0_ies_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(rrc_recfg_v15t0_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_recfg_v15t0_ies_s::desc = {
    {seq_table_rrc_recfg_v15t0_ies_s_fields, 2, UINT64_C(0x0), seq_table_rrc_recfg_v15t0_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_recfg_v15t0_ies_s_names};

// RRCSetupComplete-v17b0-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_setup_complete_v17b0_ies_s_fields[] = {
    {0,
     (int32_t)offsetof(rrc_setup_complete_v17b0_ies_s, ul_rrc_max_capa_segments_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(rrc_setup_complete_v17b0_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_rrc_setup_complete_v17b0_ies_s_names[] = {
    {"ul-RRC-MaxCapaSegments-r17", "true"},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_rrc_setup_complete_v17b0_ies_s_opts[] = {
    {0, (int32_t)offsetof(rrc_setup_complete_v17b0_ies_s, ul_rrc_max_capa_segments_r17_present)},
    {1, (int32_t)offsetof(rrc_setup_complete_v17b0_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_setup_complete_v17b0_ies_s::desc = {{seq_table_rrc_setup_complete_v17b0_ies_s_fields,
                                                                     2,
                                                                     UINT64_C(0x0),
                                                                     seq_table_rrc_setup_complete_v17b0_ies_s_opts,
                                                                     2},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_rrc_setup_complete_v17b0_ies_s_names};

// RRCSetupComplete-v16j0-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_setup_complete_v16j0_ies_s_fields[] = {
    {offsetof(rrc_setup_complete_v16j0_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(rrc_setup_complete_v16j0_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {offsetof(rrc_setup_complete_v16j0_ies_s, non_crit_ext),
     (int32_t)offsetof(rrc_setup_complete_v16j0_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_setup_complete_v17b0_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_setup_complete_v16j0_ies_s_names[] = {
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_rrc_setup_complete_v16j0_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(rrc_setup_complete_v16j0_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_setup_complete_v16j0_ies_s::desc = {{seq_table_rrc_setup_complete_v16j0_ies_s_fields,
                                                                     2,
                                                                     UINT64_C(0x0),
                                                                     seq_table_rrc_setup_complete_v16j0_ies_s_opts,
                                                                     2},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_rrc_setup_complete_v16j0_ies_s_names};

// RRCSetupComplete-v15s0-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_setup_complete_v15s0_ies_s_fields[] = {
    {offsetof(rrc_setup_complete_v15s0_ies_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(rrc_setup_complete_v15s0_ies_s::late_non_crit_ext),
                                      asn1::table::string_json>::value},
    {offsetof(rrc_setup_complete_v15s0_ies_s, non_crit_ext),
     (int32_t)offsetof(rrc_setup_complete_v15s0_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_setup_complete_v16j0_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_setup_complete_v15s0_ies_s_names[] = {
    {"lateNonCriticalExtension", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_rrc_setup_complete_v15s0_ies_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(rrc_setup_complete_v15s0_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc rrc_setup_complete_v15s0_ies_s::desc = {{seq_table_rrc_setup_complete_v15s0_ies_s_fields,
                                                                     2,
                                                                     UINT64_C(0x0),
                                                                     seq_table_rrc_setup_complete_v15s0_ies_s_opts,
                                                                     2},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_rrc_setup_complete_v15s0_ies_s_names};

// SDT-MAC-PHY-CG-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_n_ul_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &bwp_ul_ded_sdt_r17_s::desc}};
static const asn1::table::field_name
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_n_ul_r17_setup_release_names[] = {{"release", nullptr},
                                                                                             {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_n_ul_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_n_ul_r17_setup_release_alternatives,
    nullptr,
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_n_ul_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<bwp_ul_ded_sdt_r17_s>::type_offset(),
    setup_release_c<bwp_ul_ded_sdt_r17_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_sul_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &bwp_ul_ded_sdt_r17_s::desc}};
static const asn1::table::field_name
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_sul_r17_setup_release_names[] = {{"release", nullptr},
                                                                                            {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_sul_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_sul_r17_setup_release_alternatives,
    nullptr,
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_sul_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<bwp_ul_ded_sdt_r17_s>::type_offset(),
    setup_release_c<bwp_ul_ded_sdt_r17_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_ta_validation_cfg_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &cg_sdt_ta_validation_cfg_r17_s::desc}};
static const asn1::table::field_name
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_ta_validation_cfg_r17_setup_release_names[] = {{"release", nullptr},
                                                                                             {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_ta_validation_cfg_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_ta_validation_cfg_r17_setup_release_alternatives,
    nullptr,
    seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_ta_validation_cfg_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<cg_sdt_ta_validation_cfg_r17_s>::type_offset(),
    setup_release_c<cg_sdt_ta_validation_cfg_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_sdt_mac_phy_cg_cfg_r17_s_fields[] = {
    {offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_lch_restrict_to_add_mod_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(sdt_mac_phy_cg_cfg_r17_s::cg_sdt_cfg_lch_restrict_to_add_mod_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cg_sdt_cfg_lch_restrict_r17_s::desc},
    {offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_lch_restrict_to_release_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::list_codec<decltype(sdt_mac_phy_cg_cfg_r17_s::cg_sdt_cfg_lch_restrict_to_release_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(32), false, false>>::value},
    {offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_init_bwp_n_ul_r17),
     (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_init_bwp_n_ul_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_n_ul_r17_setup_release},
    {offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_init_bwp_sul_r17),
     (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_init_bwp_sul_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_cfg_init_bwp_sul_r17_setup_release},
    {offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_init_bwp_dl_r17),
     (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_init_bwp_dl_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_dl_ded_sdt_r17_s::desc},
    {offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_time_align_timer_r17),
     (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_time_align_timer_r17_present),
     asn1::table::enum_op<decltype(sdt_mac_phy_cg_cfg_r17_s::cg_sdt_time_align_timer_r17)>(),
     decltype(sdt_mac_phy_cg_cfg_r17_s::cg_sdt_time_align_timer_r17)::has_ext,
     false,
     decltype(sdt_mac_phy_cg_cfg_r17_s::cg_sdt_time_align_timer_r17)::nof_types,
     decltype(sdt_mac_phy_cg_cfg_r17_s::cg_sdt_time_align_timer_r17)::nof_exts,
     nullptr,
     &time_align_timer_opts::desc},
    {offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_rsrp_thres_ssb_r17),
     (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_rsrp_thres_ssb_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_ta_validation_cfg_r17),
     (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_ta_validation_cfg_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_sdt_mac_phy_cg_cfg_r17_s_cg_sdt_ta_validation_cfg_r17_setup_release},
    {offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cs_rnti_r17),
     (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cs_rnti_r17_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(65535),
     nullptr}};
static const asn1::table::field_name seq_table_sdt_mac_phy_cg_cfg_r17_s_names[] = {
    {"cg-SDT-ConfigLCH-RestrictionToAddModList-r17", nullptr},
    {"cg-SDT-ConfigLCH-RestrictionToReleaseList-r17", nullptr},
    {"cg-SDT-ConfigInitialBWP-NUL-r17", nullptr},
    {"cg-SDT-ConfigInitialBWP-SUL-r17", nullptr},
    {"cg-SDT-ConfigInitialBWP-DL-r17", nullptr},
    {"cg-SDT-TimeAlignmentTimer-r17", nullptr},
    {"cg-SDT-RSRP-ThresholdSSB-r17", nullptr},
    {"cg-SDT-TA-ValidationConfig-r17", nullptr},
    {"cg-SDT-CS-RNTI-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sdt_mac_phy_cg_cfg_r17_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_init_bwp_n_ul_r17_present)},
    {3, (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_init_bwp_sul_r17_present)},
    {4, (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_init_bwp_dl_r17_present)},
    {5, (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_time_align_timer_r17_present)},
    {6, (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_rsrp_thres_ssb_r17_present)},
    {7, (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_ta_validation_cfg_r17_present)},
    {8, (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cs_rnti_r17_present)}};
static const asn1::table::seq_field seq_table_sdt_mac_phy_cg_cfg_r17_s_g0_fields[] = {
    {offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_sdt_cfg_lch_restrict_to_add_mod_list_ext_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::ptr_item_list_codec<
         decltype(sdt_mac_phy_cg_cfg_r17_s::cg_sdt_cfg_lch_restrict_to_add_mod_list_ext_v1800),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &cg_sdt_cfg_lch_restrict_ext_v1800_s::desc},
    {offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_mt_sdt_max_dur_to_next_cg_occasion_r18),
     (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_mt_sdt_max_dur_to_next_cg_occasion_r18_present),
     asn1::table::enum_op<decltype(sdt_mac_phy_cg_cfg_r17_s::cg_mt_sdt_max_dur_to_next_cg_occasion_r18)>(),
     decltype(sdt_mac_phy_cg_cfg_r17_s::cg_mt_sdt_max_dur_to_next_cg_occasion_r18)::has_ext,
     false,
     decltype(sdt_mac_phy_cg_cfg_r17_s::cg_mt_sdt_max_dur_to_next_cg_occasion_r18)::nof_types,
     decltype(sdt_mac_phy_cg_cfg_r17_s::cg_mt_sdt_max_dur_to_next_cg_occasion_r18)::nof_exts,
     nullptr,
     &sdt_mac_phy_cg_cfg_r17_s::cg_mt_sdt_max_dur_to_next_cg_occasion_r18_opts::desc}};
static const asn1::table::field_name seq_table_sdt_mac_phy_cg_cfg_r17_s_g0_names[] = {
    {"cg-SDT-ConfigLCH-RestrictionToAddModListExt-v1800", nullptr},
    {"cg-MT-SDT-MaxDurationToNextCG-Occasion-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sdt_mac_phy_cg_cfg_r17_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(sdt_mac_phy_cg_cfg_r17_s, cg_mt_sdt_max_dur_to_next_cg_occasion_r18_present)}};
static const asn1::table::seq_group seq_table_sdt_mac_phy_cg_cfg_r17_s_groups[] = {
    {{seq_table_sdt_mac_phy_cg_cfg_r17_s_g0_fields, 2, UINT64_C(0x0), seq_table_sdt_mac_phy_cg_cfg_r17_s_g0_opts, 2},
     false,
     seq_table_sdt_mac_phy_cg_cfg_r17_s_g0_names}};
const asn1::table::seq_desc sdt_mac_phy_cg_cfg_r17_s::desc = {
    {seq_table_sdt_mac_phy_cg_cfg_r17_s_fields, 9, UINT64_C(0x0), seq_table_sdt_mac_phy_cg_cfg_r17_s_opts, 9},
    true,
    false,
    seq_table_sdt_mac_phy_cg_cfg_r17_s_groups,
    1,
    seq_table_sdt_mac_phy_cg_cfg_r17_s_names};

static const char* enum_names_sdt_mac_phy_cg_cfg_r17_s_cg_mt_sdt_max_dur_to_next_cg_occasion_r18_e__values[] = {
    "ms10",
    "ms100",
    "sec1",
    "sec10",
    "sec60",
    "sec100",
    "sec300",
    "sec600",
    "sec1200",
    "sec1800",
    "sec3600",
    "spare5",
    "spare4",
    "spare3",
    "spare2",
    "spare1"};
const asn1::table::enum_desc sdt_mac_phy_cg_cfg_r17_s::cg_mt_sdt_max_dur_to_next_cg_occasion_r18_opts::desc = {
    enum_names_sdt_mac_phy_cg_cfg_r17_s_cg_mt_sdt_max_dur_to_next_cg_occasion_r18_e__values,
    16,
    "sdt_mac_phy_cg_cfg_r17_s::cg_mt_sdt_max_dur_to_next_cg_occasion_r18_e_"};

// SL-ConfigCommonNR-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cfg_common_nr_r16_s_fields[] = {
    {offsetof(sl_cfg_common_nr_r16_s, sl_freq_info_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sl_cfg_common_nr_r16_s::sl_freq_info_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_freq_cfg_common_r16_s::desc},
    {offsetof(sl_cfg_common_nr_r16_s, sl_ue_sel_cfg_r16),
     (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_ue_sel_cfg_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_ue_sel_cfg_r16_s::desc},
    {offsetof(sl_cfg_common_nr_r16_s, sl_nr_anchor_carrier_freq_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<decltype(sl_cfg_common_nr_r16_s::sl_nr_anchor_carrier_freq_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(3279165), false, false>>::value},
    {offsetof(sl_cfg_common_nr_r16_s, sl_eutra_anchor_carrier_freq_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<decltype(sl_cfg_common_nr_r16_s::sl_eutra_anchor_carrier_freq_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(262143), false, false>>::value},
    {offsetof(sl_cfg_common_nr_r16_s, sl_radio_bearer_cfg_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::item_list_codec<decltype(sl_cfg_common_nr_r16_s::sl_radio_bearer_cfg_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_radio_bearer_cfg_r16_s::desc},
    {offsetof(sl_cfg_common_nr_r16_s, sl_rlc_bearer_cfg_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::item_list_codec<decltype(sl_cfg_common_nr_r16_s::sl_rlc_bearer_cfg_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_rlc_bearer_cfg_r16_s::desc},
    {offsetof(sl_cfg_common_nr_r16_s, sl_meas_cfg_common_r16),
     (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_meas_cfg_common_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_cfg_common_r16_s::desc},
    {0,
     (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_csi_acquisition_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sl_cfg_common_nr_r16_s, sl_offset_dfn_r16),
     (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_offset_dfn_r16_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(1000),
     nullptr},
    {offsetof(sl_cfg_common_nr_r16_s, t400_r16),
     (int32_t)offsetof(sl_cfg_common_nr_r16_s, t400_r16_present),
     asn1::table::enum_op<decltype(sl_cfg_common_nr_r16_s::t400_r16)>(),
     decltype(sl_cfg_common_nr_r16_s::t400_r16)::has_ext,
     false,
     decltype(sl_cfg_common_nr_r16_s::t400_r16)::nof_types,
     decltype(sl_cfg_common_nr_r16_s::t400_r16)::nof_exts,
     nullptr,
     &sl_cfg_common_nr_r16_s::t400_r16_opts::desc},
    {offsetof(sl_cfg_common_nr_r16_s, sl_max_num_consecutive_dtx_r16),
     (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_max_num_consecutive_dtx_r16_present),
     asn1::table::enum_op<decltype(sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16)>(),
     decltype(sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16)::has_ext,
     false,
     decltype(sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16)::nof_types,
     decltype(sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16)::nof_exts,
     nullptr,
     &sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::desc},
    {offsetof(sl_cfg_common_nr_r16_s, sl_ssb_prio_nr_r16),
     (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_ssb_prio_nr_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr}};
static const asn1::table::field_name seq_table_sl_cfg_common_nr_r16_s_names[] = {
    {"sl-FreqInfoList-r16", nullptr},
    {"sl-UE-SelectedConfig-r16", nullptr},
    {"sl-NR-AnchorCarrierFreqList-r16", nullptr},
    {"sl-EUTRA-AnchorCarrierFreqList-r16", nullptr},
    {"sl-RadioBearerConfigList-r16", nullptr},
    {"sl-RLC-BearerConfigList-r16", nullptr},
    {"sl-MeasConfigCommon-r16", nullptr},
    {"sl-CSI-Acquisition-r16", "enabled"},
    {"sl-OffsetDFN-r16", nullptr},
    {"t400-r16", nullptr},
    {"sl-MaxNumConsecutiveDTX-r16", nullptr},
    {"sl-SSB-PriorityNR-r16", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_common_nr_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_ue_sel_cfg_r16_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_meas_cfg_common_r16_present)},
    {7, (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_csi_acquisition_r16_present)},
    {8, (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_offset_dfn_r16_present)},
    {9, (int32_t)offsetof(sl_cfg_common_nr_r16_s, t400_r16_present)},
    {10, (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_max_num_consecutive_dtx_r16_present)},
    {11, (int32_t)offsetof(sl_cfg_common_nr_r16_s, sl_ssb_prio_nr_r16_present)}};
const asn1::table::seq_desc sl_cfg_common_nr_r16_s::desc = {
    {seq_table_sl_cfg_common_nr_r16_s_fields, 12, UINT64_C(0x0), seq_table_sl_cfg_common_nr_r16_s_opts, 12},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_cfg_common_nr_r16_s_names};

static const char* enum_names_sl_cfg_common_nr_r16_s_t400_r16_e__values[] =
    {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
const asn1::table::enum_desc sl_cfg_common_nr_r16_s::t400_r16_opts::desc = {
    enum_names_sl_cfg_common_nr_r16_s_t400_r16_e__values,
    8,
    "sl_cfg_common_nr_r16_s::t400_r16_e_"};
uint16_t sl_cfg_common_nr_r16_s::t400_r16_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "sl_cfg_common_nr_r16_s::t400_r16_e_");
}

static const char* enum_names_sl_cfg_common_nr_r16_s_sl_max_num_consecutive_dtx_r16_e__values[] =
    {"n1", "n2", "n3", "n4", "n6", "n8", "n16", "n32"};
const asn1::table::enum_desc sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::desc = {
    enum_names_sl_cfg_common_nr_r16_s_sl_max_num_consecutive_dtx_r16_e__values,
    8,
    "sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_e_"};
uint8_t sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 8, value, "sl_cfg_common_nr_r16_s::sl_max_num_consecutive_dtx_r16_e_");
}

// SL-DiscConfigCommon-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_disc_cfg_common_r17_s_fields[] = {
    {offsetof(sl_disc_cfg_common_r17_s, sl_relay_ue_cfg_common_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_relay_ue_cfg_r17_s::desc},
    {offsetof(sl_disc_cfg_common_r17_s, sl_remote_ue_cfg_common_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_remote_ue_cfg_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_disc_cfg_common_r17_s_names[] = {
    {"sl-RelayUE-ConfigCommon-r17", nullptr},
    {"sl-RemoteUE-ConfigCommon-r17", nullptr}};
const asn1::table::seq_desc sl_disc_cfg_common_r17_s::desc = {
    {seq_table_sl_disc_cfg_common_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_disc_cfg_common_r17_s_names};

// UE-TimersAndConstantsRemoteUE-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_timers_and_consts_remote_ue_r17_s_fields[] = {
    {offsetof(ue_timers_and_consts_remote_ue_r17_s, t300_remote_ue_r17),
     (int32_t)offsetof(ue_timers_and_consts_remote_ue_r17_s, t300_remote_ue_r17_present),
     asn1::table::enum_op<decltype(ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17)>(),
     decltype(ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17)::has_ext,
     false,
     decltype(ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17)::nof_types,
     decltype(ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17)::nof_exts,
     nullptr,
     &ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_opts::desc},
    {offsetof(ue_timers_and_consts_remote_ue_r17_s, t301_remote_ue_r17),
     (int32_t)offsetof(ue_timers_and_consts_remote_ue_r17_s, t301_remote_ue_r17_present),
     asn1::table::enum_op<decltype(ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17)>(),
     decltype(ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17)::has_ext,
     false,
     decltype(ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17)::nof_types,
     decltype(ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17)::nof_exts,
     nullptr,
     &ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_opts::desc},
    {offsetof(ue_timers_and_consts_remote_ue_r17_s, t319_remote_ue_r17),
     (int32_t)offsetof(ue_timers_and_consts_remote_ue_r17_s, t319_remote_ue_r17_present),
     asn1::table::enum_op<decltype(ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17)>(),
     decltype(ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17)::has_ext,
     false,
     decltype(ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17)::nof_types,
     decltype(ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17)::nof_exts,
     nullptr,
     &ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_opts::desc}};
static const asn1::table::field_name seq_table_ue_timers_and_consts_remote_ue_r17_s_names[] = {
    {"t300-RemoteUE-r17", nullptr},
    {"t301-RemoteUE-r17", nullptr},
    {"t319-RemoteUE-r17", nullptr}};
static const asn1::table::seq_opt seq_table_ue_timers_and_consts_remote_ue_r17_s_opts[] = {
    {0, (int32_t)offsetof(ue_timers_and_consts_remote_ue_r17_s, t300_remote_ue_r17_present)},
    {1, (int32_t)offsetof(ue_timers_and_consts_remote_ue_r17_s, t301_remote_ue_r17_present)},
    {2, (int32_t)offsetof(ue_timers_and_consts_remote_ue_r17_s, t319_remote_ue_r17_present)}};
const asn1::table::seq_desc ue_timers_and_consts_remote_ue_r17_s::desc = {
    {seq_table_ue_timers_and_consts_remote_ue_r17_s_fields,
     3,
     UINT64_C(0x0),
     seq_table_ue_timers_and_consts_remote_ue_r17_s_opts,
     3},
    true,
    false,
    nullptr,
    0,
    seq_table_ue_timers_and_consts_remote_ue_r17_s_names};

static const char* enum_names_ue_timers_and_consts_remote_ue_r17_s_t300_remote_ue_r17_e__values[] =
    {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
const asn1::table::enum_desc ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_opts::desc = {
    enum_names_ue_timers_and_consts_remote_ue_r17_s_t300_remote_ue_r17_e__values,
    8,
    "ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_e_"};
uint16_t ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t300_remote_ue_r17_e_");
}

static const char* enum_names_ue_timers_and_consts_remote_ue_r17_s_t301_remote_ue_r17_e__values[] =
    {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
const asn1::table::enum_desc ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_opts::desc = {
    enum_names_ue_timers_and_consts_remote_ue_r17_s_t301_remote_ue_r17_e__values,
    8,
    "ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_e_"};
uint16_t ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t301_remote_ue_r17_e_");
}

static const char* enum_names_ue_timers_and_consts_remote_ue_r17_s_t319_remote_ue_r17_e__values[] =
    {"ms100", "ms200", "ms300", "ms400", "ms600", "ms1000", "ms1500", "ms2000"};
const asn1::table::enum_desc ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_opts::desc = {
    enum_names_ue_timers_and_consts_remote_ue_r17_s_t319_remote_ue_r17_e__values,
    8,
    "ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_e_"};
uint16_t ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_opts::to_number() const
{
  static const uint16_t numbers[] = {100, 200, 300, 400, 600, 1000, 1500, 2000};
  return map_enum_number(numbers, 8, value, "ue_timers_and_consts_remote_ue_r17_s::t319_remote_ue_r17_e_");
}

// SL-DiscConfigCommon-v1800 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_disc_cfg_common_v1800_s_fields[] = {
    {offsetof(sl_disc_cfg_common_v1800_s, sl_relay_ue_cfg_common_u2_u_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_relay_ue_cfg_u2_u_r18_s::desc},
    {offsetof(sl_disc_cfg_common_v1800_s, sl_remote_ue_cfg_common_u2_u_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_remote_ue_cfg_u2_u_r18_s::desc}};
static const asn1::table::field_name seq_table_sl_disc_cfg_common_v1800_s_names[] = {
    {"sl-RelayUE-ConfigCommonU2U-r18", nullptr},
    {"sl-RemoteUE-ConfigCommonU2U-r18", nullptr}};
const asn1::table::seq_desc sl_disc_cfg_common_v1800_s::desc = {
    {seq_table_sl_disc_cfg_common_v1800_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_disc_cfg_common_v1800_s_names};

// SL-DiscConfigCommon-v1840 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_disc_cfg_common_v1840_s_fields[] = {
    {offsetof(sl_disc_cfg_common_v1840_s, sl_relay_ue_cfg_common_u2_u_v1840),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_relay_ue_cfg_u2_u_v1840_s::desc},
    {offsetof(sl_disc_cfg_common_v1840_s, sl_remote_ue_cfg_common_u2_u_v1840),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_remote_ue_cfg_u2_u_v1830_s::desc}};
static const asn1::table::field_name seq_table_sl_disc_cfg_common_v1840_s_names[] = {
    {"sl-RelayUE-ConfigCommonU2U-v1840", nullptr},
    {"sl-RemoteUE-ConfigCommonU2U-v1840", nullptr}};
const asn1::table::seq_desc sl_disc_cfg_common_v1840_s::desc = {
    {seq_table_sl_disc_cfg_common_v1840_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_disc_cfg_common_v1840_s_names};

// SIB12-IEs-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib12_ies_r16_s_fields[] = {
    {offsetof(sib12_ies_r16_s, sl_cfg_common_nr_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_cfg_common_nr_r16_s::desc},
    {offsetof(sib12_ies_r16_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib12_ies_r16_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib12_ies_r16_s_names[] = {{"sl-ConfigCommonNR-r16", nullptr},
                                                                          {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib12_ies_r16_s_opts[]  = {{1, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field  seq_table_sib12_ies_r16_s_g0_fields[] = {
    {offsetof(sib12_ies_r16_s, sl_drx_cfg_common_gc_bc_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(sib12_ies_r16_s::sl_drx_cfg_common_gc_bc_r17)>::value,
      &sl_drx_cfg_gc_bc_r17_s::desc},
    {offsetof(sib12_ies_r16_s, sl_disc_cfg_common_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(sib12_ies_r16_s::sl_disc_cfg_common_r17)>::value,
      &sl_disc_cfg_common_r17_s::desc},
    {0,
      (int32_t)offsetof(sib12_ies_r16_s, sl_l2_u2_n_relay_r17_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr},
    {0,
      (int32_t)offsetof(sib12_ies_r16_s, sl_non_relay_discovery_r17_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr},
    {0,
      (int32_t)offsetof(sib12_ies_r16_s, sl_l3_u2_n_relay_discovery_r17_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr},
    {offsetof(sib12_ies_r16_s, sl_timers_and_consts_remote_ue_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(sib12_ies_r16_s::sl_timers_and_consts_remote_ue_r17)>::value,
      &ue_timers_and_consts_remote_ue_r17_s::desc}};
static const asn1::table::field_name seq_table_sib12_ies_r16_s_g0_names[] = {
    {"sl-DRX-ConfigCommonGC-BC-r17", nullptr},
    {"sl-DiscConfigCommon-r17", nullptr},
    {"sl-L2U2N-Relay-r17", "enabled"},
    {"sl-NonRelayDiscovery-r17", "enabled"},
    {"sl-L3U2N-RelayDiscovery-r17", "enabled"},
    {"sl-TimersAndConstantsRemoteUE-r17", nullptr}};
static const asn1::table::seq_opt seq_table_sib12_ies_r16_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, (int32_t)offsetof(sib12_ies_r16_s, sl_l2_u2_n_relay_r17_present)},
    {3, (int32_t)offsetof(sib12_ies_r16_s, sl_non_relay_discovery_r17_present)},
    {4, (int32_t)offsetof(sib12_ies_r16_s, sl_l3_u2_n_relay_discovery_r17_present)},
    {5, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_sib12_ies_r16_s_g1_fields[] = {
    {offsetof(sib12_ies_r16_s, sl_freq_info_list_size_ext_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::ptr_item_list_codec<decltype(sib12_ies_r16_s::sl_freq_info_list_size_ext_v1800),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_freq_cfg_common_r16_s::desc},
    {offsetof(sib12_ies_r16_s, sl_rlc_bearer_cfg_list_size_ext_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::ptr_item_list_codec<decltype(sib12_ies_r16_s::sl_rlc_bearer_cfg_list_size_ext_v1800),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_rlc_bearer_cfg_r16_s::desc},
    {offsetof(sib12_ies_r16_s, sl_sync_freq_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_list_codec<decltype(sib12_ies_r16_s::sl_sync_freq_list_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(1), UINT64_C(8), false, false>>::value},
    {0,
     (int32_t)offsetof(sib12_ies_r16_s, sl_sync_tx_multi_freq_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib12_ies_r16_s, sl_max_trans_pwr_ca_r18),
     (int32_t)offsetof(sib12_ies_r16_s, sl_max_trans_pwr_ca_r18_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr},
    {offsetof(sib12_ies_r16_s, sl_disc_cfg_common_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sib12_ies_r16_s::sl_disc_cfg_common_v1800)>::value,
     &sl_disc_cfg_common_v1800_s::desc},
    {0,
     (int32_t)offsetof(sib12_ies_r16_s, sl_l2_u2_u_relay_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(sib12_ies_r16_s, sl_l3_u2_u_relay_discovery_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(sib12_ies_r16_s, t400_u2_u_r18),
     (int32_t)offsetof(sib12_ies_r16_s, t400_u2_u_r18_present),
     asn1::table::enum_op<decltype(sib12_ies_r16_s::t400_u2_u_r18)>(),
     decltype(sib12_ies_r16_s::t400_u2_u_r18)::has_ext,
     false,
     decltype(sib12_ies_r16_s::t400_u2_u_r18)::nof_types,
     decltype(sib12_ies_r16_s::t400_u2_u_r18)::nof_exts,
     nullptr,
     &sib12_ies_r16_s::t400_u2_u_r18_opts::desc}};
static const asn1::table::field_name seq_table_sib12_ies_r16_s_g1_names[] = {
    {"sl-FreqInfoListSizeExt-v1800", nullptr},
    {"sl-RLC-BearerConfigListSizeExt-v1800", nullptr},
    {"sl-SyncFreqList-r18", nullptr},
    {"sl-SyncTxMultiFreq-r18", "true"},
    {"sl-MaxTransPowerCA-r18", nullptr},
    {"sl-DiscConfigCommon-v1800", nullptr},
    {"sl-L2-U2U-Relay-r18", "enabled"},
    {"sl-L3-U2U-RelayDiscovery-r18", "enabled"},
    {"t400-U2U-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sib12_ies_r16_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, (int32_t)offsetof(sib12_ies_r16_s, sl_sync_tx_multi_freq_r18_present)},
    {4, (int32_t)offsetof(sib12_ies_r16_s, sl_max_trans_pwr_ca_r18_present)},
    {5, asn1::table::seq_field::ptr_presence},
    {6, (int32_t)offsetof(sib12_ies_r16_s, sl_l2_u2_u_relay_r18_present)},
    {7, (int32_t)offsetof(sib12_ies_r16_s, sl_l3_u2_u_relay_discovery_r18_present)},
    {8, (int32_t)offsetof(sib12_ies_r16_s, t400_u2_u_r18_present)}};
static const asn1::table::seq_field seq_table_sib12_ies_r16_s_g2_fields[] = {
    {offsetof(sib12_ies_r16_s, sl_disc_cfg_common_v1840),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(sib12_ies_r16_s::sl_disc_cfg_common_v1840)>::value,
     &sl_disc_cfg_common_v1840_s::desc}};
static const asn1::table::field_name seq_table_sib12_ies_r16_s_g2_names[] = {{"sl-DiscConfigCommon-v1840", nullptr}};
static const asn1::table::seq_opt    seq_table_sib12_ies_r16_s_g2_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group  seq_table_sib12_ies_r16_s_groups[]   = {
    {{seq_table_sib12_ies_r16_s_g0_fields, 6, UINT64_C(0x0), seq_table_sib12_ies_r16_s_g0_opts, 6},
        false,
        seq_table_sib12_ies_r16_s_g0_names},
    {{seq_table_sib12_ies_r16_s_g1_fields, 9, UINT64_C(0x0), seq_table_sib12_ies_r16_s_g1_opts, 9},
        false,
        seq_table_sib12_ies_r16_s_g1_names},
    {{seq_table_sib12_ies_r16_s_g2_fields, 1, UINT64_C(0x0), seq_table_sib12_ies_r16_s_g2_opts, 1},
        false,
        seq_table_sib12_ies_r16_s_g2_names}};
const asn1::table::seq_desc sib12_ies_r16_s::desc = {
    {seq_table_sib12_ies_r16_s_fields, 2, UINT64_C(0x1), seq_table_sib12_ies_r16_s_opts, 1},
    true,
    false,
    seq_table_sib12_ies_r16_s_groups,
    3,
    seq_table_sib12_ies_r16_s_names};

static const char* enum_names_sib12_ies_r16_s_t400_u2_u_r18_e__values[] =
    {"ms200", "ms400", "ms600", "ms800", "ms1200", "ms2000", "ms3000", "ms4000"};
const asn1::table::enum_desc sib12_ies_r16_s::t400_u2_u_r18_opts::desc = {
    enum_names_sib12_ies_r16_s_t400_u2_u_r18_e__values,
    8,
    "sib12_ies_r16_s::t400_u2_u_r18_e_"};
uint16_t sib12_ies_r16_s::t400_u2_u_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {200, 400, 600, 800, 1200, 2000, 3000, 4000};
  return map_enum_number(numbers, 8, value, "sib12_ies_r16_s::t400_u2_u_r18_e_");
}

// SL-ConfigCommonNR-v16k0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_cfg_common_nr_v16k0_s_fields[] = {
    {offsetof(sl_cfg_common_nr_v16k0_s, sl_freq_info_list_ext_v16k0),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sl_cfg_common_nr_v16k0_s::sl_freq_info_list_ext_v16k0),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_freq_cfg_common_ext_v16k0_s::desc}};
static const asn1::table::field_name seq_table_sl_cfg_common_nr_v16k0_s_names[] = {
    {"sl-FreqInfoListExt-v16k0", nullptr}};
static const asn1::table::seq_opt seq_table_sl_cfg_common_nr_v16k0_s_opts[] = {
    {0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sl_cfg_common_nr_v16k0_s::desc = {
    {seq_table_sl_cfg_common_nr_v16k0_s_fields, 1, UINT64_C(0x0), seq_table_sl_cfg_common_nr_v16k0_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_cfg_common_nr_v16k0_s_names};

// SIB12-IEs-v16k0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib12_ies_v16k0_s_fields[] = {
    {offsetof(sib12_ies_v16k0_s, sl_cfg_common_nr_v16k0),
     (int32_t)offsetof(sib12_ies_v16k0_s, sl_cfg_common_nr_v16k0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_cfg_common_nr_v16k0_s::desc},
    {0,
     (int32_t)offsetof(sib12_ies_v16k0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_sib12_ies_v16k0_s_names[] = {{"sl-ConfigCommonNR-v16k0", nullptr},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib12_ies_v16k0_s_opts[]  = {
    {0, (int32_t)offsetof(sib12_ies_v16k0_s, sl_cfg_common_nr_v16k0_present)},
    {1, (int32_t)offsetof(sib12_ies_v16k0_s, non_crit_ext_present)}};
const asn1::table::seq_desc sib12_ies_v16k0_s::desc = {
    {seq_table_sib12_ies_v16k0_s_fields, 2, UINT64_C(0x0), seq_table_sib12_ies_v16k0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_sib12_ies_v16k0_s_names};

// TRS-ResourceSet-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_trs_res_set_r17_s_fields[] = {
    {offsetof(trs_res_set_r17_s, pwr_ctrl_offset_ss_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trs_res_set_r17_s::pwr_ctrl_offset_ss_r17)>(),
     decltype(trs_res_set_r17_s::pwr_ctrl_offset_ss_r17)::has_ext,
     false,
     decltype(trs_res_set_r17_s::pwr_ctrl_offset_ss_r17)::nof_types,
     decltype(trs_res_set_r17_s::pwr_ctrl_offset_ss_r17)::nof_exts,
     nullptr,
     &trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_opts::desc},
    {offsetof(trs_res_set_r17_s, scrambling_id_info_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trs_res_set_r17_s::scrambling_id_info_r17_c_::desc},
    {offsetof(trs_res_set_r17_s, first_ofdm_symbol_in_time_domain_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(trs_res_set_r17_s, start_rb_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(274),
     nullptr},
    {offsetof(trs_res_set_r17_s, nrof_rbs_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(24),
     UINT64_C(276),
     nullptr},
    {offsetof(trs_res_set_r17_s, ssb_idx_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(trs_res_set_r17_s, periodicity_and_offset_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trs_res_set_r17_s::periodicity_and_offset_r17_c_::desc},
    {offsetof(trs_res_set_r17_s, freq_domain_alloc_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trs_res_set_r17_s::freq_domain_alloc_r17), asn1::table::struct_json>::value},
    {offsetof(trs_res_set_r17_s, ind_bit_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(5),
     nullptr},
    {offsetof(trs_res_set_r17_s, nrof_res_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trs_res_set_r17_s::nrof_res_r17)>(),
     decltype(trs_res_set_r17_s::nrof_res_r17)::has_ext,
     false,
     decltype(trs_res_set_r17_s::nrof_res_r17)::nof_types,
     decltype(trs_res_set_r17_s::nrof_res_r17)::nof_exts,
     nullptr,
     &trs_res_set_r17_s::nrof_res_r17_opts::desc}};
static const asn1::table::field_name seq_table_trs_res_set_r17_s_names[] = {
    {"powerControlOffsetSS-r17", nullptr},
    {"scramblingID-Info-r17", nullptr},
    {"firstOFDMSymbolInTimeDomain-r17", nullptr},
    {"startingRB-r17", nullptr},
    {"nrofRBs-r17", nullptr},
    {"ssb-Index-r17", nullptr},
    {"periodicityAndOffset-r17", nullptr},
    {"frequencyDomainAllocation-r17", nullptr},
    {"indBitID-r17", nullptr},
    {"nrofResources-r17", nullptr}};
const asn1::table::seq_desc trs_res_set_r17_s::desc = {
    {seq_table_trs_res_set_r17_s_fields, 10, UINT64_C(0x3ff), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_trs_res_set_r17_s_names};

static const char* enum_names_trs_res_set_r17_s_pwr_ctrl_offset_ss_r17_e__values[] = {"db-3", "db0", "db3", "db6"};
const asn1::table::enum_desc trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_opts::desc  = {
    enum_names_trs_res_set_r17_s_pwr_ctrl_offset_ss_r17_e__values,
    4,
    "trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_e_"};
int8_t trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_opts::to_number() const
{
  static const int8_t numbers[] = {-3, 0, 3, 6};
  return map_enum_number(numbers, 4, value, "trs_res_set_r17_s::pwr_ctrl_offset_ss_r17_e_");
}

static const asn1::table::storage_ops* const choice_storage_trs_res_set_r17_s_scrambling_id_info_r17_c__alternatives[] =
    {&asn1::table::alternative_ops<uint16_t, true>::value,
     &asn1::table::alternative_ops<
         trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_per_res_list_with2_r17_l_,
         false>::value,
     &asn1::table::alternative_ops<
         trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_per_res_list_with4_r17_l_,
         false>::value};
const asn1::table::choice_storage_desc trs_res_set_r17_s::scrambling_id_info_r17_c_::storage_desc = {
    choice_storage_trs_res_set_r17_s_scrambling_id_info_r17_c__alternatives,
    3,
    "trs_res_set_r17_s::scrambling_id_info_r17_c_"};

uint16_t& trs_res_set_r17_s::scrambling_id_info_r17_c_::set_scrambling_id_for_common_r17()
{
  set(types::scrambling_id_for_common_r17);
  return c.get<uint16_t>();
}
trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_per_res_list_with2_r17_l_&
trs_res_set_r17_s::scrambling_id_info_r17_c_::set_scrambling_id_per_res_list_with2_r17()
{
  set(types::scrambling_id_per_res_list_with2_r17);
  return c.get<scrambling_id_per_res_list_with2_r17_l_>();
}
trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_per_res_list_with4_r17_l_&
trs_res_set_r17_s::scrambling_id_info_r17_c_::set_scrambling_id_per_res_list_with4_r17()
{
  set(types::scrambling_id_per_res_list_with4_r17);
  return c.get<scrambling_id_per_res_list_with4_r17_l_>();
}
static const asn1::table::seq_field choice_table_trs_res_set_r17_s_scrambling_id_info_r17_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     2,
     2,
     &asn1::table::list_codec<trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_per_res_list_with2_r17_l_,
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(1023), false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     4,
     4,
     &asn1::table::list_codec<trs_res_set_r17_s::scrambling_id_info_r17_c_::scrambling_id_per_res_list_with4_r17_l_,
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(1023), false, false>>::value}};
static const asn1::table::field_name choice_table_trs_res_set_r17_s_scrambling_id_info_r17_c__names[] = {
    {"scramblingIDforCommon-r17", nullptr},
    {"scramblingIDperResourceListWith2-r17", nullptr},
    {"scramblingIDperResourceListWith4-r17", nullptr}};
const asn1::table::choice_desc trs_res_set_r17_s::scrambling_id_info_r17_c_::desc = {
    asn1::table::enum_int_type<trs_res_set_r17_s::scrambling_id_info_r17_c_::types>(),
    trs_res_set_r17_s::scrambling_id_info_r17_c_::types::nof_types,
    trs_res_set_r17_s::scrambling_id_info_r17_c_::types::nof_exts,
    trs_res_set_r17_s::scrambling_id_info_r17_c_::types::has_ext,
    false,
    choice_table_trs_res_set_r17_s_scrambling_id_info_r17_c__alternatives,
    nullptr,
    choice_table_trs_res_set_r17_s_scrambling_id_info_r17_c__names,
    "trs_res_set_r17_s::scrambling_id_info_r17_c_",
    offsetof(trs_res_set_r17_s::scrambling_id_info_r17_c_, c.type),
    offsetof(trs_res_set_r17_s::scrambling_id_info_r17_c_, c.value),
    false,
    &trs_res_set_r17_s::scrambling_id_info_r17_c_::storage_desc};

static const char* enum_names_trs_res_set_r17_s_scrambling_id_info_r17_c__types_values[] = {
    "scramblingIDforCommon-r17",
    "scramblingIDperResourceListWith2-r17",
    "scramblingIDperResourceListWith4-r17"};
const asn1::table::enum_desc trs_res_set_r17_s::scrambling_id_info_r17_c_::types_opts::desc = {
    enum_names_trs_res_set_r17_s_scrambling_id_info_r17_c__types_values,
    3,
    "trs_res_set_r17_s::scrambling_id_info_r17_c_::types"};
uint8_t trs_res_set_r17_s::scrambling_id_info_r17_c_::types_opts::to_number() const
{
  switch (value) {
    case scrambling_id_per_res_list_with2_r17:
      return 2;
    case scrambling_id_per_res_list_with4_r17:
      return 4;
    default:
      invalid_enum_number(value, "trs_res_set_r17_s::scrambling_id_info_r17_c_::types");
  }
  return 0;
}

static const asn1::table::storage_ops* const
    choice_storage_trs_res_set_r17_s_periodicity_and_offset_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc trs_res_set_r17_s::periodicity_and_offset_r17_c_::storage_desc = {
    choice_storage_trs_res_set_r17_s_periodicity_and_offset_r17_c__alternatives,
    4,
    "trs_res_set_r17_s::periodicity_and_offset_r17_c_"};

uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots10()
{
  set(types::slots10);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots20()
{
  set(types::slots20);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots40()
{
  set(types::slots40);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r17_s::periodicity_and_offset_r17_c_::set_slots80()
{
  set(types::slots80);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_trs_res_set_r17_s_periodicity_and_offset_r17_c__alternatives[] = {
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
     nullptr}};
static const asn1::table::field_name choice_table_trs_res_set_r17_s_periodicity_and_offset_r17_c__names[] =
    {{"slots10", nullptr}, {"slots20", nullptr}, {"slots40", nullptr}, {"slots80", nullptr}};
const asn1::table::choice_desc trs_res_set_r17_s::periodicity_and_offset_r17_c_::desc = {
    asn1::table::enum_int_type<trs_res_set_r17_s::periodicity_and_offset_r17_c_::types>(),
    trs_res_set_r17_s::periodicity_and_offset_r17_c_::types::nof_types,
    trs_res_set_r17_s::periodicity_and_offset_r17_c_::types::nof_exts,
    trs_res_set_r17_s::periodicity_and_offset_r17_c_::types::has_ext,
    false,
    choice_table_trs_res_set_r17_s_periodicity_and_offset_r17_c__alternatives,
    nullptr,
    choice_table_trs_res_set_r17_s_periodicity_and_offset_r17_c__names,
    "trs_res_set_r17_s::periodicity_and_offset_r17_c_",
    offsetof(trs_res_set_r17_s::periodicity_and_offset_r17_c_, c.type),
    offsetof(trs_res_set_r17_s::periodicity_and_offset_r17_c_, c.value),
    false,
    &trs_res_set_r17_s::periodicity_and_offset_r17_c_::storage_desc};

static const char*           enum_names_trs_res_set_r17_s_periodicity_and_offset_r17_c__types_values[] = {"slots10",
                                                                                                          "slots20",
                                                                                                          "slots40",
                                                                                                          "slots80"};
const asn1::table::enum_desc trs_res_set_r17_s::periodicity_and_offset_r17_c_::types_opts::desc        = {
    enum_names_trs_res_set_r17_s_periodicity_and_offset_r17_c__types_values,
    4,
    "trs_res_set_r17_s::periodicity_and_offset_r17_c_::types"};
uint8_t trs_res_set_r17_s::periodicity_and_offset_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {10, 20, 40, 80};
  return map_enum_number(numbers, 4, value, "trs_res_set_r17_s::periodicity_and_offset_r17_c_::types");
}

static const char*           enum_names_trs_res_set_r17_s_nrof_res_r17_e__values[] = {"n2", "n4"};
const asn1::table::enum_desc trs_res_set_r17_s::nrof_res_r17_opts::desc            = {
    enum_names_trs_res_set_r17_s_nrof_res_r17_e__values,
    2,
    "trs_res_set_r17_s::nrof_res_r17_e_"};
uint8_t trs_res_set_r17_s::nrof_res_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "trs_res_set_r17_s::nrof_res_r17_e_");
}

// SIB17-IEs-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib17_ies_r17_s_fields[] = {
    {offsetof(sib17_ies_r17_s, trs_res_set_cfg_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(sib17_ies_r17_s::trs_res_set_cfg_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trs_res_set_r17_s::desc},
    {offsetof(sib17_ies_r17_s, validity_dur_r17),
     (int32_t)offsetof(sib17_ies_r17_s, validity_dur_r17_present),
     asn1::table::enum_op<decltype(sib17_ies_r17_s::validity_dur_r17)>(),
     decltype(sib17_ies_r17_s::validity_dur_r17)::has_ext,
     false,
     decltype(sib17_ies_r17_s::validity_dur_r17)::nof_types,
     decltype(sib17_ies_r17_s::validity_dur_r17)::nof_exts,
     nullptr,
     &sib17_ies_r17_s::validity_dur_r17_opts::desc},
    {offsetof(sib17_ies_r17_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib17_ies_r17_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib17_ies_r17_s_names[] = {{"trs-ResourceSetConfig-r17", nullptr},
                                                                          {"validityDuration-r17", nullptr},
                                                                          {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib17_ies_r17_s_opts[]  = {
    {1, (int32_t)offsetof(sib17_ies_r17_s, validity_dur_r17_present)},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sib17_ies_r17_s::desc = {
    {seq_table_sib17_ies_r17_s_fields, 3, UINT64_C(0x1), seq_table_sib17_ies_r17_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_sib17_ies_r17_s_names};

static const char*           enum_names_sib17_ies_r17_s_validity_dur_r17_e__values[] = {"t1",
                                                                                        "t2",
                                                                                        "t4",
                                                                                        "t8",
                                                                                        "t16",
                                                                                        "t32",
                                                                                        "t64",
                                                                                        "t128",
                                                                                        "t256",
                                                                                        "t512",
                                                                                        "infinity",
                                                                                        "spare5",
                                                                                        "spare4",
                                                                                        "spare3",
                                                                                        "spare2",
                                                                                        "spare1"};
const asn1::table::enum_desc sib17_ies_r17_s::validity_dur_r17_opts::desc            = {
    enum_names_sib17_ies_r17_s_validity_dur_r17_e__values,
    16,
    "sib17_ies_r17_s::validity_dur_r17_e_"};
int16_t sib17_ies_r17_s::validity_dur_r17_opts::to_number() const
{
  static const int16_t numbers[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, -1};
  return map_enum_number(numbers, 11, value, "sib17_ies_r17_s::validity_dur_r17_e_");
}

// TRS-ResourceSet-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_trs_res_set_r18_s_fields[] = {
    {offsetof(trs_res_set_r18_s, pwr_ctrl_offset_ss_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trs_res_set_r18_s::pwr_ctrl_offset_ss_r18)>(),
     decltype(trs_res_set_r18_s::pwr_ctrl_offset_ss_r18)::has_ext,
     false,
     decltype(trs_res_set_r18_s::pwr_ctrl_offset_ss_r18)::nof_types,
     decltype(trs_res_set_r18_s::pwr_ctrl_offset_ss_r18)::nof_exts,
     nullptr,
     &trs_res_set_r18_s::pwr_ctrl_offset_ss_r18_opts::desc},
    {offsetof(trs_res_set_r18_s, scrambling_id_info_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trs_res_set_r18_s::scrambling_id_info_r18_c_::desc},
    {offsetof(trs_res_set_r18_s, first_ofdm_symbol_in_time_domain_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(9),
     nullptr},
    {offsetof(trs_res_set_r18_s, start_rb_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(274),
     nullptr},
    {offsetof(trs_res_set_r18_s, nrof_rbs_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(24),
     UINT64_C(276),
     nullptr},
    {offsetof(trs_res_set_r18_s, ssb_idx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(trs_res_set_r18_s, periodicity_and_offset_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trs_res_set_r18_s::periodicity_and_offset_r18_c_::desc},
    {offsetof(trs_res_set_r18_s, freq_domain_alloc_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(trs_res_set_r18_s::freq_domain_alloc_r18), asn1::table::struct_json>::value},
    {offsetof(trs_res_set_r18_s, ind_bit_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(5),
     nullptr},
    {offsetof(trs_res_set_r18_s, nrof_res_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trs_res_set_r18_s::nrof_res_r18)>(),
     decltype(trs_res_set_r18_s::nrof_res_r18)::has_ext,
     false,
     decltype(trs_res_set_r18_s::nrof_res_r18)::nof_types,
     decltype(trs_res_set_r18_s::nrof_res_r18)::nof_exts,
     nullptr,
     &trs_res_set_r18_s::nrof_res_r18_opts::desc}};
static const asn1::table::field_name seq_table_trs_res_set_r18_s_names[] = {
    {"powerControlOffsetSS-r18", nullptr},
    {"scramblingID-Info-r18", nullptr},
    {"firstOFDMSymbolInTimeDomain-r18", nullptr},
    {"startingRB-r18", nullptr},
    {"nrofRBs-r18", nullptr},
    {"ssb-Index-r18", nullptr},
    {"periodicityAndOffset-r18", nullptr},
    {"frequencyDomainAllocation-r18", nullptr},
    {"indBitID-r18", nullptr},
    {"nrofResources-r18", nullptr}};
const asn1::table::seq_desc trs_res_set_r18_s::desc = {
    {seq_table_trs_res_set_r18_s_fields, 10, UINT64_C(0x3ff), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_trs_res_set_r18_s_names};

static const char* enum_names_trs_res_set_r18_s_pwr_ctrl_offset_ss_r18_e__values[] = {"db-3", "db0", "db3", "db6"};
const asn1::table::enum_desc trs_res_set_r18_s::pwr_ctrl_offset_ss_r18_opts::desc  = {
    enum_names_trs_res_set_r18_s_pwr_ctrl_offset_ss_r18_e__values,
    4,
    "trs_res_set_r18_s::pwr_ctrl_offset_ss_r18_e_"};
int8_t trs_res_set_r18_s::pwr_ctrl_offset_ss_r18_opts::to_number() const
{
  static const int8_t numbers[] = {-3, 0, 3, 6};
  return map_enum_number(numbers, 4, value, "trs_res_set_r18_s::pwr_ctrl_offset_ss_r18_e_");
}

static const asn1::table::storage_ops* const choice_storage_trs_res_set_r18_s_scrambling_id_info_r18_c__alternatives[] =
    {&asn1::table::alternative_ops<uint16_t, true>::value,
     &asn1::table::alternative_ops<
         trs_res_set_r18_s::scrambling_id_info_r18_c_::scrambling_id_per_res_list_with2_r18_l_,
         false>::value,
     &asn1::table::alternative_ops<
         trs_res_set_r18_s::scrambling_id_info_r18_c_::scrambling_id_per_res_list_with4_r18_l_,
         false>::value};
const asn1::table::choice_storage_desc trs_res_set_r18_s::scrambling_id_info_r18_c_::storage_desc = {
    choice_storage_trs_res_set_r18_s_scrambling_id_info_r18_c__alternatives,
    3,
    "trs_res_set_r18_s::scrambling_id_info_r18_c_"};

uint16_t& trs_res_set_r18_s::scrambling_id_info_r18_c_::set_scrambling_id_for_common_r18()
{
  set(types::scrambling_id_for_common_r18);
  return c.get<uint16_t>();
}
trs_res_set_r18_s::scrambling_id_info_r18_c_::scrambling_id_per_res_list_with2_r18_l_&
trs_res_set_r18_s::scrambling_id_info_r18_c_::set_scrambling_id_per_res_list_with2_r18()
{
  set(types::scrambling_id_per_res_list_with2_r18);
  return c.get<scrambling_id_per_res_list_with2_r18_l_>();
}
trs_res_set_r18_s::scrambling_id_info_r18_c_::scrambling_id_per_res_list_with4_r18_l_&
trs_res_set_r18_s::scrambling_id_info_r18_c_::set_scrambling_id_per_res_list_with4_r18()
{
  set(types::scrambling_id_per_res_list_with4_r18);
  return c.get<scrambling_id_per_res_list_with4_r18_l_>();
}
static const asn1::table::seq_field choice_table_trs_res_set_r18_s_scrambling_id_info_r18_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     2,
     2,
     &asn1::table::list_codec<trs_res_set_r18_s::scrambling_id_info_r18_c_::scrambling_id_per_res_list_with2_r18_l_,
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(1023), false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     4,
     4,
     &asn1::table::list_codec<trs_res_set_r18_s::scrambling_id_info_r18_c_::scrambling_id_per_res_list_with4_r18_l_,
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(1023), false, false>>::value}};
static const asn1::table::field_name choice_table_trs_res_set_r18_s_scrambling_id_info_r18_c__names[] = {
    {"scramblingIDforCommon-r18", nullptr},
    {"scramblingIDperResourceListWith2-r18", nullptr},
    {"scramblingIDperResourceListWith4-r18", nullptr}};
const asn1::table::choice_desc trs_res_set_r18_s::scrambling_id_info_r18_c_::desc = {
    asn1::table::enum_int_type<trs_res_set_r18_s::scrambling_id_info_r18_c_::types>(),
    trs_res_set_r18_s::scrambling_id_info_r18_c_::types::nof_types,
    trs_res_set_r18_s::scrambling_id_info_r18_c_::types::nof_exts,
    trs_res_set_r18_s::scrambling_id_info_r18_c_::types::has_ext,
    false,
    choice_table_trs_res_set_r18_s_scrambling_id_info_r18_c__alternatives,
    nullptr,
    choice_table_trs_res_set_r18_s_scrambling_id_info_r18_c__names,
    "trs_res_set_r18_s::scrambling_id_info_r18_c_",
    offsetof(trs_res_set_r18_s::scrambling_id_info_r18_c_, c.type),
    offsetof(trs_res_set_r18_s::scrambling_id_info_r18_c_, c.value),
    false,
    &trs_res_set_r18_s::scrambling_id_info_r18_c_::storage_desc};

static const char* enum_names_trs_res_set_r18_s_scrambling_id_info_r18_c__types_values[] = {
    "scramblingIDforCommon-r18",
    "scramblingIDperResourceListWith2-r18",
    "scramblingIDperResourceListWith4-r18"};
const asn1::table::enum_desc trs_res_set_r18_s::scrambling_id_info_r18_c_::types_opts::desc = {
    enum_names_trs_res_set_r18_s_scrambling_id_info_r18_c__types_values,
    3,
    "trs_res_set_r18_s::scrambling_id_info_r18_c_::types"};
uint8_t trs_res_set_r18_s::scrambling_id_info_r18_c_::types_opts::to_number() const
{
  switch (value) {
    case scrambling_id_per_res_list_with2_r18:
      return 2;
    case scrambling_id_per_res_list_with4_r18:
      return 4;
    default:
      invalid_enum_number(value, "trs_res_set_r18_s::scrambling_id_info_r18_c_::types");
  }
  return 0;
}

static const asn1::table::storage_ops* const
    choice_storage_trs_res_set_r18_s_periodicity_and_offset_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc trs_res_set_r18_s::periodicity_and_offset_r18_c_::storage_desc = {
    choice_storage_trs_res_set_r18_s_periodicity_and_offset_r18_c__alternatives,
    7,
    "trs_res_set_r18_s::periodicity_and_offset_r18_c_"};

uint8_t& trs_res_set_r18_s::periodicity_and_offset_r18_c_::set_slots10()
{
  set(types::slots10);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r18_s::periodicity_and_offset_r18_c_::set_slots20()
{
  set(types::slots20);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r18_s::periodicity_and_offset_r18_c_::set_slots40()
{
  set(types::slots40);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r18_s::periodicity_and_offset_r18_c_::set_slots80()
{
  set(types::slots80);
  return c.get<uint8_t>();
}
uint8_t& trs_res_set_r18_s::periodicity_and_offset_r18_c_::set_slots160()
{
  set(types::slots160);
  return c.get<uint8_t>();
}
uint16_t& trs_res_set_r18_s::periodicity_and_offset_r18_c_::set_slots320()
{
  set(types::slots320);
  return c.get<uint16_t>();
}
uint16_t& trs_res_set_r18_s::periodicity_and_offset_r18_c_::set_slots640()
{
  set(types::slots640);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field choice_table_trs_res_set_r18_s_periodicity_and_offset_r18_c__alternatives[] = {
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
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(319),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(639),
     nullptr}};
static const asn1::table::field_name choice_table_trs_res_set_r18_s_periodicity_and_offset_r18_c__names[] = {
    {"slots10", nullptr},
    {"slots20", nullptr},
    {"slots40", nullptr},
    {"slots80", nullptr},
    {"slots160", nullptr},
    {"slots320", nullptr},
    {"slots640", nullptr}};
const asn1::table::choice_desc trs_res_set_r18_s::periodicity_and_offset_r18_c_::desc = {
    asn1::table::enum_int_type<trs_res_set_r18_s::periodicity_and_offset_r18_c_::types>(),
    trs_res_set_r18_s::periodicity_and_offset_r18_c_::types::nof_types,
    trs_res_set_r18_s::periodicity_and_offset_r18_c_::types::nof_exts,
    trs_res_set_r18_s::periodicity_and_offset_r18_c_::types::has_ext,
    false,
    choice_table_trs_res_set_r18_s_periodicity_and_offset_r18_c__alternatives,
    nullptr,
    choice_table_trs_res_set_r18_s_periodicity_and_offset_r18_c__names,
    "trs_res_set_r18_s::periodicity_and_offset_r18_c_",
    offsetof(trs_res_set_r18_s::periodicity_and_offset_r18_c_, c.type),
    offsetof(trs_res_set_r18_s::periodicity_and_offset_r18_c_, c.value),
    false,
    &trs_res_set_r18_s::periodicity_and_offset_r18_c_::storage_desc};

static const char* enum_names_trs_res_set_r18_s_periodicity_and_offset_r18_c__types_values[] =
    {"slots10", "slots20", "slots40", "slots80", "slots160", "slots320", "slots640"};
const asn1::table::enum_desc trs_res_set_r18_s::periodicity_and_offset_r18_c_::types_opts::desc = {
    enum_names_trs_res_set_r18_s_periodicity_and_offset_r18_c__types_values,
    7,
    "trs_res_set_r18_s::periodicity_and_offset_r18_c_::types"};
uint16_t trs_res_set_r18_s::periodicity_and_offset_r18_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10, 20, 40, 80, 160, 320, 640};
  return map_enum_number(numbers, 7, value, "trs_res_set_r18_s::periodicity_and_offset_r18_c_::types");
}

static const char*           enum_names_trs_res_set_r18_s_nrof_res_r18_e__values[] = {"n2", "n4"};
const asn1::table::enum_desc trs_res_set_r18_s::nrof_res_r18_opts::desc            = {
    enum_names_trs_res_set_r18_s_nrof_res_r18_e__values,
    2,
    "trs_res_set_r18_s::nrof_res_r18_e_"};
uint8_t trs_res_set_r18_s::nrof_res_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "trs_res_set_r18_s::nrof_res_r18_e_");
}

// SIB17bis-IEs-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib17bis_ies_r18_s_fields[] = {
    {offsetof(sib17bis_ies_r18_s, trs_res_set_cfg_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(sib17bis_ies_r18_s::trs_res_set_cfg_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trs_res_set_r18_s::desc},
    {offsetof(sib17bis_ies_r18_s, validity_dur_r18),
     (int32_t)offsetof(sib17bis_ies_r18_s, validity_dur_r18_present),
     asn1::table::enum_op<decltype(sib17bis_ies_r18_s::validity_dur_r18)>(),
     decltype(sib17bis_ies_r18_s::validity_dur_r18)::has_ext,
     false,
     decltype(sib17bis_ies_r18_s::validity_dur_r18)::nof_types,
     decltype(sib17bis_ies_r18_s::validity_dur_r18)::nof_exts,
     nullptr,
     &sib17bis_ies_r18_s::validity_dur_r18_opts::desc},
    {offsetof(sib17bis_ies_r18_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib17bis_ies_r18_s::late_non_crit_ext),
                                      asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib17bis_ies_r18_s_names[] = {{"trs-ResourceSetConfig-r18", nullptr},
                                                                             {"validityDuration-r18", nullptr},
                                                                             {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib17bis_ies_r18_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(sib17bis_ies_r18_s, validity_dur_r18_present)},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc sib17bis_ies_r18_s::desc = {
    {seq_table_sib17bis_ies_r18_s_fields, 3, UINT64_C(0x0), seq_table_sib17bis_ies_r18_s_opts, 3},
    true,
    false,
    nullptr,
    0,
    seq_table_sib17bis_ies_r18_s_names};

static const char*           enum_names_sib17bis_ies_r18_s_validity_dur_r18_e__values[] = {"t1",
                                                                                           "t2",
                                                                                           "t4",
                                                                                           "t8",
                                                                                           "t16",
                                                                                           "t32",
                                                                                           "t64",
                                                                                           "t128",
                                                                                           "t256",
                                                                                           "t512",
                                                                                           "infinity",
                                                                                           "spare5",
                                                                                           "spare4",
                                                                                           "spare3",
                                                                                           "spare2",
                                                                                           "spare1"};
const asn1::table::enum_desc sib17bis_ies_r18_s::validity_dur_r18_opts::desc            = {
    enum_names_sib17bis_ies_r18_s_validity_dur_r18_e__values,
    16,
    "sib17bis_ies_r18_s::validity_dur_r18_e_"};
int16_t sib17bis_ies_r18_s::validity_dur_r18_opts::to_number() const
{
  static const int16_t numbers[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, -1};
  return map_enum_number(numbers, 11, value, "sib17bis_ies_r18_s::validity_dur_r18_e_");
}

// SL-PosConfigCommonNR-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_pos_cfg_common_nr_r18_s_fields[] = {
    {offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_freq_info_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(sl_pos_cfg_common_nr_r18_s::sl_pos_freq_info_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sl_freq_cfg_common_r16_s::desc},
    {offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_ue_sel_cfg_r18),
     (int32_t)offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_ue_sel_cfg_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_ue_sel_cfg_r16_s::desc},
    {offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_nr_anchor_carrier_freq_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<decltype(sl_pos_cfg_common_nr_r18_s::sl_pos_nr_anchor_carrier_freq_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(3279165), false, false>>::value},
    {offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_meas_cfg_common_r18),
     (int32_t)offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_meas_cfg_common_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_meas_cfg_common_r16_s::desc},
    {offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_offset_dfn_r18),
     (int32_t)offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_offset_dfn_r18_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(1000),
     nullptr},
    {offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_ssb_prio_nr_r18),
     (int32_t)offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_ssb_prio_nr_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr}};
static const asn1::table::field_name seq_table_sl_pos_cfg_common_nr_r18_s_names[] = {
    {"sl-PosFreqInfoList-r18", nullptr},
    {"sl-PosUE-SelectedConfig-r18", nullptr},
    {"sl-PosNR-AnchorCarrierFreqList-r18", nullptr},
    {"sl-PosMeasConfigCommon-r18", nullptr},
    {"sl-PosOffsetDFN-r18", nullptr},
    {"sl-PosSSB-PriorityNR-r18", nullptr}};
static const asn1::table::seq_opt seq_table_sl_pos_cfg_common_nr_r18_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_ue_sel_cfg_r18_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_meas_cfg_common_r18_present)},
    {4, (int32_t)offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_offset_dfn_r18_present)},
    {5, (int32_t)offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_ssb_prio_nr_r18_present)}};
static const asn1::table::seq_field seq_table_sl_pos_cfg_common_nr_r18_s_g0_fields[] = {
    {offsetof(sl_pos_cfg_common_nr_r18_s, sl_pos_freq_info_list_ext_v1880),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_item_list_codec<decltype(sl_pos_cfg_common_nr_r18_s::sl_pos_freq_info_list_ext_v1880),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &sl_freq_cfg_common_ext_v16k0_s::desc}};
static const asn1::table::field_name seq_table_sl_pos_cfg_common_nr_r18_s_g0_names[] = {
    {"sl-PosFreqInfoListExt-v1880", nullptr}};
static const asn1::table::seq_opt seq_table_sl_pos_cfg_common_nr_r18_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_sl_pos_cfg_common_nr_r18_s_groups[] = {
    {{seq_table_sl_pos_cfg_common_nr_r18_s_g0_fields,
      1,
      UINT64_C(0x0),
      seq_table_sl_pos_cfg_common_nr_r18_s_g0_opts,
      1},
     false,
     seq_table_sl_pos_cfg_common_nr_r18_s_g0_names}};
const asn1::table::seq_desc sl_pos_cfg_common_nr_r18_s::desc = {
    {seq_table_sl_pos_cfg_common_nr_r18_s_fields, 6, UINT64_C(0x0), seq_table_sl_pos_cfg_common_nr_r18_s_opts, 6},
    true,
    false,
    seq_table_sl_pos_cfg_common_nr_r18_s_groups,
    1,
    seq_table_sl_pos_cfg_common_nr_r18_s_names};

// SIB23-IEs-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sib23_ies_r18_s_fields[] = {
    {offsetof(sib23_ies_r18_s, sl_pos_cfg_common_nr_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_pos_cfg_common_nr_r18_s::desc},
    {offsetof(sib23_ies_r18_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(sib23_ies_r18_s::late_non_crit_ext), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_sib23_ies_r18_s_names[] = {{"sl-PosConfigCommonNR-r18", nullptr},
                                                                          {"lateNonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_sib23_ies_r18_s_opts[]  = {{1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          sib23_ies_r18_s::desc             = {
    {seq_table_sib23_ies_r18_s_fields, 2, UINT64_C(0x1), seq_table_sib23_ies_r18_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_sib23_ies_r18_s_names};

// SL-ServingCellInfo-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_serving_cell_info_r17_s_fields[] = {
    {offsetof(sl_serving_cell_info_r17_s, sl_pci_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(sl_serving_cell_info_r17_s, sl_carrier_freq_nr_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr}};
static const asn1::table::field_name seq_table_sl_serving_cell_info_r17_s_names[] = {{"sl-PhysCellId-r17", nullptr},
                                                                                     {"sl-CarrierFreqNR-r17", nullptr}};
const asn1::table::seq_desc          sl_serving_cell_info_r17_s::desc             = {
    {seq_table_sl_serving_cell_info_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_sl_serving_cell_info_r17_s_names};

// SRS-PosRRC-InactiveConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_pos_rrc_inactive_cfg_r17_s_fields[] = {
    {offsetof(srs_pos_rrc_inactive_cfg_r17_s, srs_pos_cfg_n_ul_r17),
     (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, srs_pos_cfg_n_ul_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_pos_cfg_r17_s::desc},
    {offsetof(srs_pos_rrc_inactive_cfg_r17_s, srs_pos_cfg_sul_r17),
     (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, srs_pos_cfg_sul_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_pos_cfg_r17_s::desc},
    {offsetof(srs_pos_rrc_inactive_cfg_r17_s, bwp_n_ul_r17),
     (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, bwp_n_ul_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_s::desc},
    {offsetof(srs_pos_rrc_inactive_cfg_r17_s, bwp_sul_r17),
     (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, bwp_sul_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_s::desc},
    {offsetof(srs_pos_rrc_inactive_cfg_r17_s, inactive_pos_srs_time_align_timer_r17),
     (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, inactive_pos_srs_time_align_timer_r17_present),
     asn1::table::enum_op<decltype(srs_pos_rrc_inactive_cfg_r17_s::inactive_pos_srs_time_align_timer_r17)>(),
     decltype(srs_pos_rrc_inactive_cfg_r17_s::inactive_pos_srs_time_align_timer_r17)::has_ext,
     false,
     decltype(srs_pos_rrc_inactive_cfg_r17_s::inactive_pos_srs_time_align_timer_r17)::nof_types,
     decltype(srs_pos_rrc_inactive_cfg_r17_s::inactive_pos_srs_time_align_timer_r17)::nof_exts,
     nullptr,
     &time_align_timer_opts::desc},
    {offsetof(srs_pos_rrc_inactive_cfg_r17_s, inactive_pos_srs_rsrp_change_thres_r17),
     (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, inactive_pos_srs_rsrp_change_thres_r17_present),
     asn1::table::enum_op<decltype(srs_pos_rrc_inactive_cfg_r17_s::inactive_pos_srs_rsrp_change_thres_r17)>(),
     decltype(srs_pos_rrc_inactive_cfg_r17_s::inactive_pos_srs_rsrp_change_thres_r17)::has_ext,
     false,
     decltype(srs_pos_rrc_inactive_cfg_r17_s::inactive_pos_srs_rsrp_change_thres_r17)::nof_types,
     decltype(srs_pos_rrc_inactive_cfg_r17_s::inactive_pos_srs_rsrp_change_thres_r17)::nof_exts,
     nullptr,
     &rsrp_change_thres_r17_opts::desc}};
static const asn1::table::field_name seq_table_srs_pos_rrc_inactive_cfg_r17_s_names[] = {
    {"srs-PosConfigNUL-r17", nullptr},
    {"srs-PosConfigSUL-r17", nullptr},
    {"bwp-NUL-r17", nullptr},
    {"bwp-SUL-r17", nullptr},
    {"inactivePosSRS-TimeAlignmentTimer-r17", nullptr},
    {"inactivePosSRS-RSRP-ChangeThreshold-r17", nullptr}};
static const asn1::table::seq_opt seq_table_srs_pos_rrc_inactive_cfg_r17_s_opts[] = {
    {0, (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, srs_pos_cfg_n_ul_r17_present)},
    {1, (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, srs_pos_cfg_sul_r17_present)},
    {2, (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, bwp_n_ul_r17_present)},
    {3, (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, bwp_sul_r17_present)},
    {4, (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, inactive_pos_srs_time_align_timer_r17_present)},
    {5, (int32_t)offsetof(srs_pos_rrc_inactive_cfg_r17_s, inactive_pos_srs_rsrp_change_thres_r17_present)}};
const asn1::table::seq_desc srs_pos_rrc_inactive_cfg_r17_s::desc = {{seq_table_srs_pos_rrc_inactive_cfg_r17_s_fields,
                                                                     6,
                                                                     UINT64_C(0x0),
                                                                     seq_table_srs_pos_rrc_inactive_cfg_r17_s_opts,
                                                                     6},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_srs_pos_rrc_inactive_cfg_r17_s_names};

// SRS-PosRRC-InactiveEnhancedConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_r18_setup_release_alternatives
        [] = {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               1,
               16,
               &asn1::table::list_codec<
                   setup_release_c<
                       dyn_seq_of<dyn_seq_of<srs_pos_res_set_linked_for_agg_bw_r18_s, 2, 3>, 1, 16>>::value_type,
                   asn1::table::list_json<asn1::table::struct_json>,
                   asn1::table::list_items<false, 2, 3, false, asn1::table::object_items<false>>>::value}};
static const asn1::table::field_name
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_r18_setup_release_names[] = {
        {"release", nullptr},
        {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_r18_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_r18_setup_release_alternatives,
        nullptr,
        seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_r18_setup_release_names,
        "setup_release_c",
        setup_release_c<dyn_seq_of<dyn_seq_of<srs_pos_res_set_linked_for_agg_bw_r18_s, 2, 3>, 1, 16>>::type_offset(),
        setup_release_c<dyn_seq_of<dyn_seq_of<srs_pos_res_set_linked_for_agg_bw_r18_s, 2, 3>, 1, 16>>::value_offset()};
static const asn1::table::seq_field
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_tx_hop_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &srs_pos_tx_hop_r18_s::desc}};
static const asn1::table::field_name
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_tx_hop_r18_setup_release_names[] = {{"release", nullptr},
                                                                                                  {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_tx_hop_r18_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_tx_hop_r18_setup_release_alternatives,
        nullptr,
        seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_tx_hop_r18_setup_release_names,
        "setup_release_c",
        setup_release_c<srs_pos_tx_hop_r18_s>::type_offset(),
        setup_release_c<srs_pos_tx_hop_r18_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_pre_cfg_list_r18_setup_release_alternatives
        [] = {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               1,
               16,
               &asn1::table::item_list_codec<
                   setup_release_c<dyn_seq_of<srs_pos_rrc_inactive_validity_area_cfg_r18_s, 1, 16>>::value_type,
                   asn1::table::sequence_item_ops,
                   false,
                   true>::value,
               &srs_pos_rrc_inactive_validity_area_cfg_r18_s::desc}};
static const asn1::table::field_name
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_pre_cfg_list_r18_setup_release_names
        [] = {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_pre_cfg_list_r18_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_pre_cfg_list_r18_setup_release_alternatives,
        nullptr,
        seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_pre_cfg_list_r18_setup_release_names,
        "setup_release_c",
        setup_release_c<dyn_seq_of<srs_pos_rrc_inactive_validity_area_cfg_r18_s, 1, 16>>::type_offset(),
        setup_release_c<dyn_seq_of<srs_pos_rrc_inactive_validity_area_cfg_r18_s, 1, 16>>::value_offset()};
static const asn1::table::seq_field
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_non_pre_cfg_r18_setup_release_alternatives
        [] = {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::sequence,
               false,
               false,
               0,
               0,
               nullptr,
               &srs_pos_rrc_inactive_validity_area_cfg_r18_s::desc}};
static const asn1::table::field_name
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_non_pre_cfg_r18_setup_release_names
        [] = {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_non_pre_cfg_r18_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_non_pre_cfg_r18_setup_release_alternatives,
        nullptr,
        seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_non_pre_cfg_r18_setup_release_names,
        "setup_release_c",
        setup_release_c<srs_pos_rrc_inactive_validity_area_cfg_r18_s>::type_offset(),
        setup_release_c<srs_pos_rrc_inactive_validity_area_cfg_r18_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_setup_release_alternatives
        [] = {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::sequence,
               false,
               false,
               0,
               0,
               nullptr,
               &srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s::desc}};
static const asn1::table::field_name
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_setup_release_names
        [] = {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_setup_release_alternatives,
        nullptr,
        seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_setup_release_names,
        "setup_release_c",
        setup_release_c<srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s>::type_offset(),
        setup_release_c<srs_pos_rrc_inactive_agg_bw_add_carriers_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_fields[] = {
    {offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s, srs_pos_rrc_inactive_agg_bw_cfg_list_r18),
     (int32_t)offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s, srs_pos_rrc_inactive_agg_bw_cfg_list_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_cfg_list_r18_setup_release},
    {offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s, srs_pos_tx_hop_r18),
     (int32_t)offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s, srs_pos_tx_hop_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_tx_hop_r18_setup_release},
    {offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s, srs_pos_rrc_inactive_validity_area_pre_cfg_list_r18),
     (int32_t)offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s,
                       srs_pos_rrc_inactive_validity_area_pre_cfg_list_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_pre_cfg_list_r18_setup_release},
    {offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s, srs_pos_rrc_inactive_validity_area_non_pre_cfg_r18),
     (int32_t)offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s,
                       srs_pos_rrc_inactive_validity_area_non_pre_cfg_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_validity_area_non_pre_cfg_r18_setup_release}};
static const asn1::table::field_name seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_names[] = {
    {"srs-PosRRC-InactiveAggBW-ConfigList-r18", nullptr},
    {"srs-PosTx-Hopping-r18", nullptr},
    {"srs-PosRRC-InactiveValidityAreaPreConfigList-r18", nullptr},
    {"srs-PosRRC-InactiveValidityAreaNonPreConfig-r18", nullptr}};
static const asn1::table::seq_opt seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_opts[] = {
    {0, (int32_t)offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s, srs_pos_rrc_inactive_agg_bw_cfg_list_r18_present)},
    {1, (int32_t)offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s, srs_pos_tx_hop_r18_present)},
    {2,
     (int32_t)offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s,
                       srs_pos_rrc_inactive_validity_area_pre_cfg_list_r18_present)},
    {3,
     (int32_t)offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s,
                       srs_pos_rrc_inactive_validity_area_non_pre_cfg_r18_present)}};
static const asn1::table::seq_field seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_g0_fields[] = {
    {offsetof(srs_pos_rrc_inactive_enhanced_cfg_r18_s, srs_pos_rrc_inactive_agg_bw_add_carriers_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<
         decltype(srs_pos_rrc_inactive_enhanced_cfg_r18_s::srs_pos_rrc_inactive_agg_bw_add_carriers_r18)>::value,
     &seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_srs_pos_rrc_inactive_agg_bw_add_carriers_r18_setup_release}};
static const asn1::table::field_name seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_g0_names[] = {
    {"srs-PosRRC-InactiveAggBW-AdditionalCarriers-r18", nullptr}};
static const asn1::table::seq_opt seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_groups[] = {
    {{seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_g0_fields,
      1,
      UINT64_C(0x0),
      seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_g0_opts,
      1},
     false,
     seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_g0_names}};
const asn1::table::seq_desc srs_pos_rrc_inactive_enhanced_cfg_r18_s::desc = {
    {seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_fields,
     4,
     UINT64_C(0x0),
     seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_opts,
     4},
    true,
    false,
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_groups,
    1,
    seq_table_srs_pos_rrc_inactive_enhanced_cfg_r18_s_names};

// UE-CapabilityRequestFilterCommon ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_request_filt_common_s_mrdc_request_s__fields[] = {
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s::mrdc_request_s_, omit_en_dc_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s::mrdc_request_s_, include_nr_dc_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s::mrdc_request_s_, include_ne_dc_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ue_cap_request_filt_common_s_mrdc_request_s__names[] = {
    {"omitEN-DC", "true"},
    {"includeNR-DC", "true"},
    {"includeNE-DC", "true"}};
static const asn1::table::seq_opt seq_table_ue_cap_request_filt_common_s_mrdc_request_s__opts[] = {
    {0, (int32_t)offsetof(ue_cap_request_filt_common_s::mrdc_request_s_, omit_en_dc_present)},
    {1, (int32_t)offsetof(ue_cap_request_filt_common_s::mrdc_request_s_, include_nr_dc_present)},
    {2, (int32_t)offsetof(ue_cap_request_filt_common_s::mrdc_request_s_, include_ne_dc_present)}};
static const asn1::table::seq_desc seq_table_ue_cap_request_filt_common_s_mrdc_request_s_ = {
    {seq_table_ue_cap_request_filt_common_s_mrdc_request_s__fields,
     3,
     UINT64_C(0x0),
     seq_table_ue_cap_request_filt_common_s_mrdc_request_s__opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_request_filt_common_s_mrdc_request_s__names};
static const asn1::table::seq_field seq_table_ue_cap_request_filt_common_s_codebook_type_request_r16_s__fields[] = {
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s::codebook_type_request_r16_s_, type1_single_panel_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s::codebook_type_request_r16_s_, type1_multi_panel_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s::codebook_type_request_r16_s_, type2_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s::codebook_type_request_r16_s_, type2_port_sel_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ue_cap_request_filt_common_s_codebook_type_request_r16_s__names[] = {
    {"type1-SinglePanel-r16", "true"},
    {"type1-MultiPanel-r16", "true"},
    {"type2-r16", "true"},
    {"type2-PortSelection-r16", "true"}};
static const asn1::table::seq_opt seq_table_ue_cap_request_filt_common_s_codebook_type_request_r16_s__opts[] = {
    {0, (int32_t)offsetof(ue_cap_request_filt_common_s::codebook_type_request_r16_s_, type1_single_panel_r16_present)},
    {1, (int32_t)offsetof(ue_cap_request_filt_common_s::codebook_type_request_r16_s_, type1_multi_panel_r16_present)},
    {2, (int32_t)offsetof(ue_cap_request_filt_common_s::codebook_type_request_r16_s_, type2_r16_present)},
    {3, (int32_t)offsetof(ue_cap_request_filt_common_s::codebook_type_request_r16_s_, type2_port_sel_r16_present)}};
static const asn1::table::seq_desc seq_table_ue_cap_request_filt_common_s_codebook_type_request_r16_s_ = {
    {seq_table_ue_cap_request_filt_common_s_codebook_type_request_r16_s__fields,
     4,
     UINT64_C(0x0),
     seq_table_ue_cap_request_filt_common_s_codebook_type_request_r16_s__opts,
     4},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_request_filt_common_s_codebook_type_request_r16_s__names};
static const asn1::table::seq_field seq_table_ue_cap_request_filt_common_s_lower_msd_request_r18_s__fields[] = {
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s::lower_msd_request_r18_s_, pc1dot5_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s::lower_msd_request_r18_s_, pc2_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s::lower_msd_request_r18_s_, pc3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ue_cap_request_filt_common_s_lower_msd_request_r18_s__names[] = {
    {"pc1dot5-r18", "true"},
    {"pc2-r18", "true"},
    {"pc3-r18", "true"}};
static const asn1::table::seq_opt seq_table_ue_cap_request_filt_common_s_lower_msd_request_r18_s__opts[] = {
    {0, (int32_t)offsetof(ue_cap_request_filt_common_s::lower_msd_request_r18_s_, pc1dot5_r18_present)},
    {1, (int32_t)offsetof(ue_cap_request_filt_common_s::lower_msd_request_r18_s_, pc2_r18_present)},
    {2, (int32_t)offsetof(ue_cap_request_filt_common_s::lower_msd_request_r18_s_, pc3_r18_present)}};
static const asn1::table::seq_desc seq_table_ue_cap_request_filt_common_s_lower_msd_request_r18_s_ = {
    {seq_table_ue_cap_request_filt_common_s_lower_msd_request_r18_s__fields,
     3,
     UINT64_C(0x0),
     seq_table_ue_cap_request_filt_common_s_lower_msd_request_r18_s__opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_request_filt_common_s_lower_msd_request_r18_s__names};
static const asn1::table::seq_field seq_table_ue_cap_request_filt_common_s_fields[] = {
    {offsetof(ue_cap_request_filt_common_s, mrdc_request),
     (int32_t)offsetof(ue_cap_request_filt_common_s, mrdc_request_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ue_cap_request_filt_common_s_mrdc_request_s_}};
static const asn1::table::field_name seq_table_ue_cap_request_filt_common_s_names[] = {{"mrdc-Request", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_cap_request_filt_common_s_opts[]  = {
    {0, (int32_t)offsetof(ue_cap_request_filt_common_s, mrdc_request_present)}};
static const asn1::table::seq_field seq_table_ue_cap_request_filt_common_s_g0_fields[] = {
    {offsetof(ue_cap_request_filt_common_s, codebook_type_request_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(ue_cap_request_filt_common_s::codebook_type_request_r16)>::value,
     &seq_table_ue_cap_request_filt_common_s_codebook_type_request_r16_s_},
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s, ul_tx_switch_request_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ue_cap_request_filt_common_s_g0_names[] = {
    {"codebookTypeRequest-r16", nullptr},
    {"uplinkTxSwitchRequest-r16", "true"}};
static const asn1::table::seq_opt seq_table_ue_cap_request_filt_common_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(ue_cap_request_filt_common_s, ul_tx_switch_request_r16_present)}};
static const asn1::table::seq_field seq_table_ue_cap_request_filt_common_s_g1_fields[] = {
    {offsetof(ue_cap_request_filt_common_s, requested_cell_grouping_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::ptr_item_list_codec<decltype(ue_cap_request_filt_common_s::requested_cell_grouping_r16),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &cell_grouping_r16_s::desc}};
static const asn1::table::field_name seq_table_ue_cap_request_filt_common_s_g1_names[] = {
    {"requestedCellGrouping-r16", nullptr}};
static const asn1::table::seq_opt seq_table_ue_cap_request_filt_common_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_ue_cap_request_filt_common_s_g2_fields[] = {
    {0,
     (int32_t)offsetof(ue_cap_request_filt_common_s, fallback_group_five_request_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ue_cap_request_filt_common_s_g2_names[] = {
    {"fallbackGroupFiveRequest-r17", "true"}};
static const asn1::table::seq_opt seq_table_ue_cap_request_filt_common_s_g2_opts[] = {
    {0, (int32_t)offsetof(ue_cap_request_filt_common_s, fallback_group_five_request_r17_present)}};
static const asn1::table::seq_field seq_table_ue_cap_request_filt_common_s_g3_fields[] = {
    {offsetof(ue_cap_request_filt_common_s, lower_msd_request_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(ue_cap_request_filt_common_s::lower_msd_request_r18)>::value,
     &seq_table_ue_cap_request_filt_common_s_lower_msd_request_r18_s_}};
static const asn1::table::field_name seq_table_ue_cap_request_filt_common_s_g3_names[] = {
    {"lowerMSDRequest-r18", nullptr}};
static const asn1::table::seq_opt seq_table_ue_cap_request_filt_common_s_g3_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_ue_cap_request_filt_common_s_groups[] = {
    {{seq_table_ue_cap_request_filt_common_s_g0_fields,
      2,
      UINT64_C(0x0),
      seq_table_ue_cap_request_filt_common_s_g0_opts,
      2},
     false,
     seq_table_ue_cap_request_filt_common_s_g0_names},
    {{seq_table_ue_cap_request_filt_common_s_g1_fields,
      1,
      UINT64_C(0x0),
      seq_table_ue_cap_request_filt_common_s_g1_opts,
      1},
     false,
     seq_table_ue_cap_request_filt_common_s_g1_names},
    {{seq_table_ue_cap_request_filt_common_s_g2_fields,
      1,
      UINT64_C(0x0),
      seq_table_ue_cap_request_filt_common_s_g2_opts,
      1},
     false,
     seq_table_ue_cap_request_filt_common_s_g2_names},
    {{seq_table_ue_cap_request_filt_common_s_g3_fields,
      1,
      UINT64_C(0x0),
      seq_table_ue_cap_request_filt_common_s_g3_opts,
      1},
     false,
     seq_table_ue_cap_request_filt_common_s_g3_names}};
const asn1::table::seq_desc ue_cap_request_filt_common_s::desc = {
    {seq_table_ue_cap_request_filt_common_s_fields, 1, UINT64_C(0x0), seq_table_ue_cap_request_filt_common_s_opts, 1},
    true,
    false,
    seq_table_ue_cap_request_filt_common_s_groups,
    4,
    seq_table_ue_cap_request_filt_common_s_names};

// UE-CapabilityRequestFilterNR-v1710 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_request_filt_nr_v1710_s_fields[] = {
    {0,
     (int32_t)offsetof(ue_cap_request_filt_nr_v1710_s, sidelink_request_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ue_cap_request_filt_nr_v1710_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_cap_request_filt_nr_v1710_s_names[] = {
    {"sidelinkRequest-r17", "true"},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_cap_request_filt_nr_v1710_s_opts[] = {
    {0, (int32_t)offsetof(ue_cap_request_filt_nr_v1710_s, sidelink_request_r17_present)},
    {1, (int32_t)offsetof(ue_cap_request_filt_nr_v1710_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_cap_request_filt_nr_v1710_s::desc = {{seq_table_ue_cap_request_filt_nr_v1710_s_fields,
                                                                     2,
                                                                     UINT64_C(0x0),
                                                                     seq_table_ue_cap_request_filt_nr_v1710_s_opts,
                                                                     2},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_ue_cap_request_filt_nr_v1710_s_names};

// UE-CapabilityRequestFilterNR-v1540 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_request_filt_nr_v1540_s_fields[] = {
    {0,
     (int32_t)offsetof(ue_cap_request_filt_nr_v1540_s, srs_switching_time_request_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ue_cap_request_filt_nr_v1540_s, non_crit_ext),
     (int32_t)offsetof(ue_cap_request_filt_nr_v1540_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_request_filt_nr_v1710_s::desc}};
static const asn1::table::field_name seq_table_ue_cap_request_filt_nr_v1540_s_names[] = {
    {"srs-SwitchingTimeRequest", "true"},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_cap_request_filt_nr_v1540_s_opts[] = {
    {0, (int32_t)offsetof(ue_cap_request_filt_nr_v1540_s, srs_switching_time_request_present)},
    {1, (int32_t)offsetof(ue_cap_request_filt_nr_v1540_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_cap_request_filt_nr_v1540_s::desc = {{seq_table_ue_cap_request_filt_nr_v1540_s_fields,
                                                                     2,
                                                                     UINT64_C(0x0),
                                                                     seq_table_ue_cap_request_filt_nr_v1540_s_opts,
                                                                     2},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_ue_cap_request_filt_nr_v1540_s_names};

// UE-CapabilityRequestFilterNR ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_request_filt_nr_s_fields[] = {
    {offsetof(ue_cap_request_filt_nr_s, freq_band_list_filt),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1280,
     &asn1::table::item_list_codec<decltype(ue_cap_request_filt_nr_s::freq_band_list_filt),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &freq_band_info_c::desc},
    {offsetof(ue_cap_request_filt_nr_s, non_crit_ext),
     (int32_t)offsetof(ue_cap_request_filt_nr_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_request_filt_nr_v1540_s::desc}};
static const asn1::table::field_name seq_table_ue_cap_request_filt_nr_s_names[] = {{"frequencyBandListFilter", nullptr},
                                                                                   {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_cap_request_filt_nr_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(ue_cap_request_filt_nr_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_cap_request_filt_nr_s::desc = {
    {seq_table_ue_cap_request_filt_nr_s_fields, 2, UINT64_C(0x0), seq_table_ue_cap_request_filt_nr_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_request_filt_nr_s_names};

// UE-NR-Capability-v17d0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v17d0_s_fields[] = {
    {offsetof(ue_nr_cap_v17d0_s, feature_sets_v17d0),
     (int32_t)offsetof(ue_nr_cap_v17d0_s, feature_sets_v17d0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &feature_sets_v17d0_s::desc},
    {0,
     (int32_t)offsetof(ue_nr_cap_v17d0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_nr_cap_v17d0_s_names[] = {{"featureSets-v17d0", nullptr},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v17d0_s_opts[]  = {
    {0, (int32_t)offsetof(ue_nr_cap_v17d0_s, feature_sets_v17d0_present)},
    {1, (int32_t)offsetof(ue_nr_cap_v17d0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v17d0_s::desc = {
    {seq_table_ue_nr_cap_v17d0_s_fields, 2, UINT64_C(0x0), seq_table_ue_nr_cap_v17d0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v17d0_s_names};

// UE-NR-Capability-v17c0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v17c0_s_fields[] = {
    {offsetof(ue_nr_cap_v17c0_s, mac_params_v17c0),
     (int32_t)offsetof(ue_nr_cap_v17c0_s, mac_params_v17c0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mac_params_v17c0_s::desc},
    {offsetof(ue_nr_cap_v17c0_s, non_crit_ext),
     (int32_t)offsetof(ue_nr_cap_v17c0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_nr_cap_v17d0_s::desc}};
static const asn1::table::field_name seq_table_ue_nr_cap_v17c0_s_names[] = {{"mac-Parameters-v17c0", nullptr},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v17c0_s_opts[]  = {
    {0, (int32_t)offsetof(ue_nr_cap_v17c0_s, mac_params_v17c0_present)},
    {1, (int32_t)offsetof(ue_nr_cap_v17c0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v17c0_s::desc = {
    {seq_table_ue_nr_cap_v17c0_s_fields, 2, UINT64_C(0x0), seq_table_ue_nr_cap_v17c0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v17c0_s_names};

// UE-NR-Capability-v17b0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v17b0_s_fields[] = {
    {offsetof(ue_nr_cap_v17b0_s, mac_params_v17b0),
     (int32_t)offsetof(ue_nr_cap_v17b0_s, mac_params_v17b0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mac_params_v17b0_s::desc},
    {offsetof(ue_nr_cap_v17b0_s, rf_params_v17b0),
     (int32_t)offsetof(ue_nr_cap_v17b0_s, rf_params_v17b0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rf_params_v17b0_s::desc},
    {0,
     (int32_t)offsetof(ue_nr_cap_v17b0_s, ul_rrc_max_capa_segments_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ue_nr_cap_v17b0_s, non_crit_ext),
     (int32_t)offsetof(ue_nr_cap_v17b0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_nr_cap_v17c0_s::desc}};
static const asn1::table::field_name seq_table_ue_nr_cap_v17b0_s_names[] = {{"mac-Parameters-v17b0", nullptr},
                                                                            {"rf-Parameters-v17b0", nullptr},
                                                                            {"ul-RRC-MaxCapaSegments-r17", "supported"},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v17b0_s_opts[]  = {
    {0, (int32_t)offsetof(ue_nr_cap_v17b0_s, mac_params_v17b0_present)},
    {1, (int32_t)offsetof(ue_nr_cap_v17b0_s, rf_params_v17b0_present)},
    {2, (int32_t)offsetof(ue_nr_cap_v17b0_s, ul_rrc_max_capa_segments_r17_present)},
    {3, (int32_t)offsetof(ue_nr_cap_v17b0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v17b0_s::desc = {
    {seq_table_ue_nr_cap_v17b0_s_fields, 4, UINT64_C(0x0), seq_table_ue_nr_cap_v17b0_s_opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v17b0_s_names};

// UE-NR-Capability-v16j0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v16j0_s_fields[] = {
    {offsetof(ue_nr_cap_v16j0_s, rf_params_v16j0),
     (int32_t)offsetof(ue_nr_cap_v16j0_s, rf_params_v16j0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rf_params_v16j0_s::desc},
    {offsetof(ue_nr_cap_v16j0_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(ue_nr_cap_v16j0_s::late_non_crit_ext), asn1::table::string_json>::value},
    {offsetof(ue_nr_cap_v16j0_s, non_crit_ext),
     (int32_t)offsetof(ue_nr_cap_v16j0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_nr_cap_v17b0_s::desc}};
static const asn1::table::field_name seq_table_ue_nr_cap_v16j0_s_names[] = {{"rf-Parameters-v16j0", nullptr},
                                                                            {"lateNonCriticalExtension", nullptr},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v16j0_s_opts[]  = {
    {0, (int32_t)offsetof(ue_nr_cap_v16j0_s, rf_params_v16j0_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(ue_nr_cap_v16j0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v16j0_s::desc = {
    {seq_table_ue_nr_cap_v16j0_s_fields, 3, UINT64_C(0x0), seq_table_ue_nr_cap_v16j0_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v16j0_s_names};

// UE-NR-Capability-v16d0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v16d0_s_fields[] = {
    {offsetof(ue_nr_cap_v16d0_s, feature_sets_v16d0),
     (int32_t)offsetof(ue_nr_cap_v16d0_s, feature_sets_v16d0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &feature_sets_v16d0_s::desc},
    {offsetof(ue_nr_cap_v16d0_s, non_crit_ext),
     (int32_t)offsetof(ue_nr_cap_v16d0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_nr_cap_v16j0_s::desc}};
static const asn1::table::field_name seq_table_ue_nr_cap_v16d0_s_names[] = {{"featureSets-v16d0", nullptr},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v16d0_s_opts[]  = {
    {0, (int32_t)offsetof(ue_nr_cap_v16d0_s, feature_sets_v16d0_present)},
    {1, (int32_t)offsetof(ue_nr_cap_v16d0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v16d0_s::desc = {
    {seq_table_ue_nr_cap_v16d0_s_fields, 2, UINT64_C(0x0), seq_table_ue_nr_cap_v16d0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v16d0_s_names};

// UE-NR-Capability-v16c0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v16c0_s_fields[] = {
    {offsetof(ue_nr_cap_v16c0_s, rf_params_v16c0),
     (int32_t)offsetof(ue_nr_cap_v16c0_s, rf_params_v16c0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rf_params_v16c0_s::desc},
    {offsetof(ue_nr_cap_v16c0_s, non_crit_ext),
     (int32_t)offsetof(ue_nr_cap_v16c0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_nr_cap_v16d0_s::desc}};
static const asn1::table::field_name seq_table_ue_nr_cap_v16c0_s_names[] = {{"rf-Parameters-v16c0", nullptr},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v16c0_s_opts[]  = {
    {0, (int32_t)offsetof(ue_nr_cap_v16c0_s, rf_params_v16c0_present)},
    {1, (int32_t)offsetof(ue_nr_cap_v16c0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v16c0_s::desc = {
    {seq_table_ue_nr_cap_v16c0_s_fields, 2, UINT64_C(0x0), seq_table_ue_nr_cap_v16c0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v16c0_s_names};

// UE-NR-Capability-v16a0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v16a0_s_fields[] = {
    {offsetof(ue_nr_cap_v16a0_s, phy_params_v16a0),
     (int32_t)offsetof(ue_nr_cap_v16a0_s, phy_params_v16a0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &phy_params_v16a0_s::desc},
    {offsetof(ue_nr_cap_v16a0_s, rf_params_v16a0),
     (int32_t)offsetof(ue_nr_cap_v16a0_s, rf_params_v16a0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rf_params_v16a0_s::desc},
    {offsetof(ue_nr_cap_v16a0_s, non_crit_ext),
     (int32_t)offsetof(ue_nr_cap_v16a0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_nr_cap_v16c0_s::desc}};
static const asn1::table::field_name seq_table_ue_nr_cap_v16a0_s_names[] = {{"phy-Parameters-v16a0", nullptr},
                                                                            {"rf-Parameters-v16a0", nullptr},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v16a0_s_opts[]  = {
    {0, (int32_t)offsetof(ue_nr_cap_v16a0_s, phy_params_v16a0_present)},
    {1, (int32_t)offsetof(ue_nr_cap_v16a0_s, rf_params_v16a0_present)},
    {2, (int32_t)offsetof(ue_nr_cap_v16a0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v16a0_s::desc = {
    {seq_table_ue_nr_cap_v16a0_s_fields, 3, UINT64_C(0x0), seq_table_ue_nr_cap_v16a0_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v16a0_s_names};

// UE-NR-Capability-v15j0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v15j0_s_fields[] = {
    {offsetof(ue_nr_cap_v15j0_s, late_non_crit_ext),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(ue_nr_cap_v15j0_s::late_non_crit_ext), asn1::table::string_json>::value},
    {offsetof(ue_nr_cap_v15j0_s, non_crit_ext),
     (int32_t)offsetof(ue_nr_cap_v15j0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_nr_cap_v16a0_s::desc}};
static const asn1::table::field_name seq_table_ue_nr_cap_v15j0_s_names[] = {{"lateNonCriticalExtension", nullptr},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v15j0_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(ue_nr_cap_v15j0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v15j0_s::desc = {
    {seq_table_ue_nr_cap_v15j0_s_fields, 2, UINT64_C(0x0), seq_table_ue_nr_cap_v15j0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v15j0_s_names};

// UE-NR-Capability-v15g0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v15g0_s_fields[] = {
    {offsetof(ue_nr_cap_v15g0_s, rf_params_v15g0),
     (int32_t)offsetof(ue_nr_cap_v15g0_s, rf_params_v15g0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rf_params_v15g0_s::desc},
    {offsetof(ue_nr_cap_v15g0_s, non_crit_ext),
     (int32_t)offsetof(ue_nr_cap_v15g0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_nr_cap_v15j0_s::desc}};
static const asn1::table::field_name seq_table_ue_nr_cap_v15g0_s_names[] = {{"rf-Parameters-v15g0", nullptr},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v15g0_s_opts[]  = {
    {0, (int32_t)offsetof(ue_nr_cap_v15g0_s, rf_params_v15g0_present)},
    {1, (int32_t)offsetof(ue_nr_cap_v15g0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v15g0_s::desc = {
    {seq_table_ue_nr_cap_v15g0_s_fields, 2, UINT64_C(0x0), seq_table_ue_nr_cap_v15g0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v15g0_s_names};

// UE-NR-Capability-v15c0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v15c0_s_fields[] = {
    {offsetof(ue_nr_cap_v15c0_s, nrdc_params_v15c0),
     (int32_t)offsetof(ue_nr_cap_v15c0_s, nrdc_params_v15c0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nrdc_params_v15c0_s::desc},
    {0,
     (int32_t)offsetof(ue_nr_cap_v15c0_s, partial_fr2_fallback_rx_req_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ue_nr_cap_v15c0_s, non_crit_ext),
     (int32_t)offsetof(ue_nr_cap_v15c0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_nr_cap_v15g0_s::desc}};
static const asn1::table::field_name seq_table_ue_nr_cap_v15c0_s_names[] = {{"nrdc-Parameters-v15c0", nullptr},
                                                                            {"partialFR2-FallbackRX-Req", "true"},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v15c0_s_opts[]  = {
    {0, (int32_t)offsetof(ue_nr_cap_v15c0_s, nrdc_params_v15c0_present)},
    {1, (int32_t)offsetof(ue_nr_cap_v15c0_s, partial_fr2_fallback_rx_req_present)},
    {2, (int32_t)offsetof(ue_nr_cap_v15c0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v15c0_s::desc = {
    {seq_table_ue_nr_cap_v15c0_s_fields, 3, UINT64_C(0x0), seq_table_ue_nr_cap_v15c0_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v15c0_s_names};

// UE-NR-Capability-v15t0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v15t0_s_fields[] = {
    {offsetof(ue_nr_cap_v15t0_s, feature_sets_v15t0),
     (int32_t)offsetof(ue_nr_cap_v15t0_s, feature_sets_v15t0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &feature_sets_v15t0_s::desc},
    {offsetof(ue_nr_cap_v15t0_s, meas_and_mob_params_v15t0),
     (int32_t)offsetof(ue_nr_cap_v15t0_s, meas_and_mob_params_v15t0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &meas_and_mob_params_v15t0_s::desc},
    {0,
     (int32_t)offsetof(ue_nr_cap_v15t0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_nr_cap_v15t0_s_names[] = {{"featureSets-v15t0", nullptr},
                                                                            {"measAndMobParameters-v15t0", nullptr},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v15t0_s_opts[]  = {
    {0, (int32_t)offsetof(ue_nr_cap_v15t0_s, feature_sets_v15t0_present)},
    {1, (int32_t)offsetof(ue_nr_cap_v15t0_s, meas_and_mob_params_v15t0_present)},
    {2, (int32_t)offsetof(ue_nr_cap_v15t0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v15t0_s::desc = {
    {seq_table_ue_nr_cap_v15t0_s_fields, 3, UINT64_C(0x0), seq_table_ue_nr_cap_v15t0_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v15t0_s_names};

// UE-NR-Capability-v16k0 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_nr_cap_v16k0_s_fields[] = {
    {offsetof(ue_nr_cap_v16k0_s, feature_sets_v16k0),
     (int32_t)offsetof(ue_nr_cap_v16k0_s, feature_sets_v16k0_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &feature_sets_v16k0_s::desc},
    {0,
     (int32_t)offsetof(ue_nr_cap_v16k0_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_nr_cap_v16k0_s_names[] = {{"featureSets-v16k0", nullptr},
                                                                            {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_nr_cap_v16k0_s_opts[]  = {
    {0, (int32_t)offsetof(ue_nr_cap_v16k0_s, feature_sets_v16k0_present)},
    {1, (int32_t)offsetof(ue_nr_cap_v16k0_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_nr_cap_v16k0_s::desc = {
    {seq_table_ue_nr_cap_v16k0_s_fields, 2, UINT64_C(0x0), seq_table_ue_nr_cap_v16k0_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_nr_cap_v16k0_s_names};

// UECapabilityEnquiry-v17b0-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_enquiry_v17b0_ies_s_fields[] = {
    {offsetof(ue_cap_enquiry_v17b0_ies_s, rrc_max_capa_seg_allowed_r17),
     (int32_t)offsetof(ue_cap_enquiry_v17b0_ies_s, rrc_max_capa_seg_allowed_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(2),
     UINT64_C(16),
     nullptr},
    {0,
     (int32_t)offsetof(ue_cap_enquiry_v17b0_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name seq_table_ue_cap_enquiry_v17b0_ies_s_names[] = {
    {"rrc-MaxCapaSegAllowed-r17", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_cap_enquiry_v17b0_ies_s_opts[] = {
    {0, (int32_t)offsetof(ue_cap_enquiry_v17b0_ies_s, rrc_max_capa_seg_allowed_r17_present)},
    {1, (int32_t)offsetof(ue_cap_enquiry_v17b0_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_cap_enquiry_v17b0_ies_s::desc = {
    {seq_table_ue_cap_enquiry_v17b0_ies_s_fields, 2, UINT64_C(0x0), seq_table_ue_cap_enquiry_v17b0_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_enquiry_v17b0_ies_s_names};

// UECapabilityEnquiry-v1610-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_enquiry_v1610_ies_s_fields[] = {
    {0,
     (int32_t)offsetof(ue_cap_enquiry_v1610_ies_s, rrc_seg_allowed_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ue_cap_enquiry_v1610_ies_s, non_crit_ext),
     (int32_t)offsetof(ue_cap_enquiry_v1610_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_enquiry_v17b0_ies_s::desc}};
static const asn1::table::field_name seq_table_ue_cap_enquiry_v1610_ies_s_names[] = {{"rrc-SegAllowed-r16", "enabled"},
                                                                                     {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_cap_enquiry_v1610_ies_s_opts[]  = {
    {0, (int32_t)offsetof(ue_cap_enquiry_v1610_ies_s, rrc_seg_allowed_r16_present)},
    {1, (int32_t)offsetof(ue_cap_enquiry_v1610_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_cap_enquiry_v1610_ies_s::desc = {
    {seq_table_ue_cap_enquiry_v1610_ies_s_fields, 2, UINT64_C(0x0), seq_table_ue_cap_enquiry_v1610_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_enquiry_v1610_ies_s_names};

// UECapabilityEnquiry-v1560-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_cap_enquiry_v1560_ies_s_fields[] = {
    {offsetof(ue_cap_enquiry_v1560_ies_s, cap_request_filt_common),
     (int32_t)offsetof(ue_cap_enquiry_v1560_ies_s, cap_request_filt_common_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_request_filt_common_s::desc},
    {offsetof(ue_cap_enquiry_v1560_ies_s, non_crit_ext),
     (int32_t)offsetof(ue_cap_enquiry_v1560_ies_s, non_crit_ext_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_cap_enquiry_v1610_ies_s::desc}};
static const asn1::table::field_name seq_table_ue_cap_enquiry_v1560_ies_s_names[] = {
    {"capabilityRequestFilterCommon", nullptr},
    {"nonCriticalExtension", nullptr}};
static const asn1::table::seq_opt seq_table_ue_cap_enquiry_v1560_ies_s_opts[] = {
    {0, (int32_t)offsetof(ue_cap_enquiry_v1560_ies_s, cap_request_filt_common_present)},
    {1, (int32_t)offsetof(ue_cap_enquiry_v1560_ies_s, non_crit_ext_present)}};
const asn1::table::seq_desc ue_cap_enquiry_v1560_ies_s::desc = {
    {seq_table_ue_cap_enquiry_v1560_ies_s_fields, 2, UINT64_C(0x0), seq_table_ue_cap_enquiry_v1560_ies_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_ue_cap_enquiry_v1560_ies_s_names};

// SL-AccessInfo-L2U2N-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_sl_access_info_l2_u2_n_r17_s_fields[] = {
    {offsetof(sl_access_info_l2_u2_n_r17_s, cell_access_related_info_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_access_related_info_s::desc},
    {offsetof(sl_access_info_l2_u2_n_r17_s, sl_serving_cell_info_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &sl_serving_cell_info_r17_s::desc}};
static const asn1::table::field_name seq_table_sl_access_info_l2_u2_n_r17_s_names[] = {
    {"cellAccessRelatedInfo-r17", nullptr},
    {"sl-ServingCellInfo-r17", nullptr}};
static const asn1::table::seq_field seq_table_sl_access_info_l2_u2_n_r17_s_g0_fields[] = {
    {0,
     (int32_t)offsetof(sl_access_info_l2_u2_n_r17_s, sl_relay_ind_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sl_access_info_l2_u2_n_r17_s_g0_names[] = {
    {"sl-RelayIndication-r18", "support"}};
static const asn1::table::seq_opt seq_table_sl_access_info_l2_u2_n_r17_s_g0_opts[] = {
    {0, (int32_t)offsetof(sl_access_info_l2_u2_n_r17_s, sl_relay_ind_r18_present)}};
static const asn1::table::seq_group seq_table_sl_access_info_l2_u2_n_r17_s_groups[] = {
    {{seq_table_sl_access_info_l2_u2_n_r17_s_g0_fields,
      1,
      UINT64_C(0x0),
      seq_table_sl_access_info_l2_u2_n_r17_s_g0_opts,
      1},
     false,
     seq_table_sl_access_info_l2_u2_n_r17_s_g0_names}};
const asn1::table::seq_desc sl_access_info_l2_u2_n_r17_s::desc = {
    {seq_table_sl_access_info_l2_u2_n_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    true,
    false,
    seq_table_sl_access_info_l2_u2_n_r17_s_groups,
    1,
    seq_table_sl_access_info_l2_u2_n_r17_s_names};
