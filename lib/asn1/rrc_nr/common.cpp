// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/common.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// SubcarrierSpacing ::= ENUMERATED
static const char* enum_names_subcarrier_spacing_e_values[] =
    {"kHz15", "kHz30", "kHz60", "kHz120", "kHz240", "kHz480-v1700", "kHz960-v1700", "spare1"};
const asn1::table::enum_desc subcarrier_spacing_opts::desc = {enum_names_subcarrier_spacing_e_values,
                                                              8,
                                                              "subcarrier_spacing_e"};
uint16_t                     subcarrier_spacing_opts::to_number() const
{
  static const uint16_t numbers[] = {15, 30, 60, 120, 240, 480, 960};
  return map_enum_number(numbers, 7, value, "subcarrier_spacing_e");
}

// ServingCellAndBWP-Id-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_serving_cell_and_bwp_id_r17_s_fields[] = {
    {offsetof(serving_cell_and_bwp_id_r17_s, servingcell_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(serving_cell_and_bwp_id_r17_s, bwp_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_and_bwp_id_r17_s_names[] = {{"servingcell-r17", nullptr},
                                                                                        {"bwp-r17", nullptr}};
const asn1::table::seq_desc          serving_cell_and_bwp_id_r17_s::desc             = {
    {seq_table_serving_cell_and_bwp_id_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_serving_cell_and_bwp_id_r17_s_names};

// PLMN-Identity ::= SEQUENCE
static const asn1::table::seq_field seq_table_plmn_id_s_fields[] = {
    {offsetof(plmn_id_s, mcc),
     (int32_t)offsetof(plmn_id_s, mcc_present),
     asn1::table::field_op::codec,
     false,
     false,
     3,
     3,
     &asn1::table::list_codec<decltype(plmn_id_s::mcc),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(9), false, false>>::value},
    {offsetof(plmn_id_s, mnc),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     2,
     3,
     &asn1::table::list_codec<decltype(plmn_id_s::mnc),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(9), false, false>>::value}};
static const asn1::table::field_name seq_table_plmn_id_s_names[] = {{"mcc", nullptr}, {"mnc", nullptr}};
static const asn1::table::seq_opt    seq_table_plmn_id_s_opts[]  = {{0, (int32_t)offsetof(plmn_id_s, mcc_present)}};
const asn1::table::seq_desc          plmn_id_s::desc             = {
    {seq_table_plmn_id_s_fields, 2, UINT64_C(0x2), seq_table_plmn_id_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_plmn_id_s_names};

// SRS-PeriodicityAndOffset ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_srs_periodicity_and_offset_c_alternatives[] = {
    nullptr,
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
    &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc srs_periodicity_and_offset_c::storage_desc = {
    choice_storage_srs_periodicity_and_offset_c_alternatives,
    17,
    "srs_periodicity_and_offset_c"};

void srs_periodicity_and_offset_c::set_sl1()
{
  set(types::sl1);
}
uint8_t& srs_periodicity_and_offset_c::set_sl2()
{
  set(types::sl2);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl4()
{
  set(types::sl4);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl5()
{
  set(types::sl5);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl8()
{
  set(types::sl8);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl10()
{
  set(types::sl10);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl16()
{
  set(types::sl16);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl20()
{
  set(types::sl20);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl32()
{
  set(types::sl32);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl40()
{
  set(types::sl40);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl64()
{
  set(types::sl64);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl80()
{
  set(types::sl80);
  return c.get<uint8_t>();
}
uint8_t& srs_periodicity_and_offset_c::set_sl160()
{
  set(types::sl160);
  return c.get<uint8_t>();
}
uint16_t& srs_periodicity_and_offset_c::set_sl320()
{
  set(types::sl320);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_c::set_sl640()
{
  set(types::sl640);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_c::set_sl1280()
{
  set(types::sl1280);
  return c.get<uint16_t>();
}
uint16_t& srs_periodicity_and_offset_c::set_sl2560()
{
  set(types::sl2560);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field choice_table_srs_periodicity_and_offset_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
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
     UINT64_C(4),
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
     UINT64_C(9),
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
     UINT64_C(39),
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
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1279),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(2559),
     nullptr}};
static const asn1::table::field_name choice_table_srs_periodicity_and_offset_c_names[] = {{"sl1", nullptr},
                                                                                          {"sl2", nullptr},
                                                                                          {"sl4", nullptr},
                                                                                          {"sl5", nullptr},
                                                                                          {"sl8", nullptr},
                                                                                          {"sl10", nullptr},
                                                                                          {"sl16", nullptr},
                                                                                          {"sl20", nullptr},
                                                                                          {"sl32", nullptr},
                                                                                          {"sl40", nullptr},
                                                                                          {"sl64", nullptr},
                                                                                          {"sl80", nullptr},
                                                                                          {"sl160", nullptr},
                                                                                          {"sl320", nullptr},
                                                                                          {"sl640", nullptr},
                                                                                          {"sl1280", nullptr},
                                                                                          {"sl2560", nullptr}};
const asn1::table::choice_desc       srs_periodicity_and_offset_c::desc                = {
    asn1::table::enum_int_type<srs_periodicity_and_offset_c::types>(),
    srs_periodicity_and_offset_c::types::nof_types,
    srs_periodicity_and_offset_c::types::nof_exts,
    srs_periodicity_and_offset_c::types::has_ext,
    false,
    choice_table_srs_periodicity_and_offset_c_alternatives,
    nullptr,
    choice_table_srs_periodicity_and_offset_c_names,
    "srs_periodicity_and_offset_c",
    offsetof(srs_periodicity_and_offset_c, c.type),
    offsetof(srs_periodicity_and_offset_c, c.value),
    false,
    &srs_periodicity_and_offset_c::storage_desc};

static const char*           enum_names_srs_periodicity_and_offset_c_types_values[] = {"sl1",
                                                                                       "sl2",
                                                                                       "sl4",
                                                                                       "sl5",
                                                                                       "sl8",
                                                                                       "sl10",
                                                                                       "sl16",
                                                                                       "sl20",
                                                                                       "sl32",
                                                                                       "sl40",
                                                                                       "sl64",
                                                                                       "sl80",
                                                                                       "sl160",
                                                                                       "sl320",
                                                                                       "sl640",
                                                                                       "sl1280",
                                                                                       "sl2560"};
const asn1::table::enum_desc srs_periodicity_and_offset_c::types_opts::desc         = {
    enum_names_srs_periodicity_and_offset_c_types_values,
    17,
    "srs_periodicity_and_offset_c::types"};
uint16_t srs_periodicity_and_offset_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {1, 2, 4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560};
  return map_enum_number(numbers, 17, value, "srs_periodicity_and_offset_c::types");
}

// SRS-SpatialRelationInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_spatial_relation_info_s_fields[] = {
    {offsetof(srs_spatial_relation_info_s, serving_cell_id),
     (int32_t)offsetof(srs_spatial_relation_info_s, serving_cell_id_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(srs_spatial_relation_info_s, ref_sig),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_spatial_relation_info_s::ref_sig_c_::desc}};
static const asn1::table::field_name seq_table_srs_spatial_relation_info_s_names[] = {{"servingCellId", nullptr},
                                                                                      {"referenceSignal", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_spatial_relation_info_s_opts[]  = {
    {0, (int32_t)offsetof(srs_spatial_relation_info_s, serving_cell_id_present)}};
const asn1::table::seq_desc srs_spatial_relation_info_s::desc = {
    {seq_table_srs_spatial_relation_info_s_fields, 2, UINT64_C(0x2), seq_table_srs_spatial_relation_info_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_spatial_relation_info_s_names};

static const asn1::table::storage_ops* const choice_storage_srs_spatial_relation_info_s_ref_sig_c__alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<srs_spatial_relation_info_s::ref_sig_c_::srs_s_, false>::value};
const asn1::table::choice_storage_desc srs_spatial_relation_info_s::ref_sig_c_::storage_desc = {
    choice_storage_srs_spatial_relation_info_s_ref_sig_c__alternatives,
    3,
    "srs_spatial_relation_info_s::ref_sig_c_"};

uint8_t& srs_spatial_relation_info_s::ref_sig_c_::set_ssb_idx()
{
  set(types::ssb_idx);
  return c.get<uint8_t>();
}
uint8_t& srs_spatial_relation_info_s::ref_sig_c_::set_csi_rs_idx()
{
  set(types::csi_rs_idx);
  return c.get<uint8_t>();
}
srs_spatial_relation_info_s::ref_sig_c_::srs_s_& srs_spatial_relation_info_s::ref_sig_c_::set_srs()
{
  set(types::srs);
  return c.get<srs_s_>();
}
static const asn1::table::seq_field seq_table_srs_spatial_relation_info_s_ref_sig_c__srs_s__fields[] = {
    {offsetof(srs_spatial_relation_info_s::ref_sig_c_::srs_s_, res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(srs_spatial_relation_info_s::ref_sig_c_::srs_s_, ul_bwp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr}};
static const asn1::table::field_name seq_table_srs_spatial_relation_info_s_ref_sig_c__srs_s__names[] = {
    {"resourceId", nullptr},
    {"uplinkBWP", nullptr}};
static const asn1::table::seq_desc seq_table_srs_spatial_relation_info_s_ref_sig_c__srs_s_ = {
    {seq_table_srs_spatial_relation_info_s_ref_sig_c__srs_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_spatial_relation_info_s_ref_sig_c__srs_s__names};
static const asn1::table::seq_field choice_table_srs_spatial_relation_info_s_ref_sig_c__alternatives[] = {
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
     UINT64_C(191),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_spatial_relation_info_s_ref_sig_c__srs_s_}};
static const asn1::table::field_name choice_table_srs_spatial_relation_info_s_ref_sig_c__names[] = {
    {"ssb-Index", nullptr},
    {"csi-RS-Index", nullptr},
    {"srs", nullptr}};
const asn1::table::choice_desc srs_spatial_relation_info_s::ref_sig_c_::desc = {
    asn1::table::enum_int_type<srs_spatial_relation_info_s::ref_sig_c_::types>(),
    srs_spatial_relation_info_s::ref_sig_c_::types::nof_types,
    srs_spatial_relation_info_s::ref_sig_c_::types::nof_exts,
    srs_spatial_relation_info_s::ref_sig_c_::types::has_ext,
    false,
    choice_table_srs_spatial_relation_info_s_ref_sig_c__alternatives,
    nullptr,
    choice_table_srs_spatial_relation_info_s_ref_sig_c__names,
    "srs_spatial_relation_info_s::ref_sig_c_",
    offsetof(srs_spatial_relation_info_s::ref_sig_c_, c.type),
    offsetof(srs_spatial_relation_info_s::ref_sig_c_, c.value),
    false,
    &srs_spatial_relation_info_s::ref_sig_c_::storage_desc};

static const char*           enum_names_srs_spatial_relation_info_s_ref_sig_c__types_values[] = {"ssb-Index",
                                                                                                 "csi-RS-Index",
                                                                                                 "srs"};
const asn1::table::enum_desc srs_spatial_relation_info_s::ref_sig_c_::types_opts::desc        = {
    enum_names_srs_spatial_relation_info_s_ref_sig_c__types_values,
    3,
    "srs_spatial_relation_info_s::ref_sig_c_::types"};

// SpatialRelationInfo-PDC-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_spatial_relation_info_pdc_r17_s_fields[] = {
    {offsetof(spatial_relation_info_pdc_r17_s, ref_sig),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &spatial_relation_info_pdc_r17_s::ref_sig_c_::desc}};
static const asn1::table::field_name seq_table_spatial_relation_info_pdc_r17_s_names[] = {{"referenceSignal", nullptr}};
const asn1::table::seq_desc          spatial_relation_info_pdc_r17_s::desc             = {
    {seq_table_spatial_relation_info_pdc_r17_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_spatial_relation_info_pdc_r17_s_names};

static const asn1::table::storage_ops* const choice_storage_spatial_relation_info_pdc_r17_s_ref_sig_c__alternatives[] =
    {&asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<uint8_t, true>::value,
     &asn1::table::alternative_ops<spatial_relation_info_pdc_r17_s::ref_sig_c_::srs_s_, false>::value};
const asn1::table::choice_storage_desc spatial_relation_info_pdc_r17_s::ref_sig_c_::storage_desc = {
    choice_storage_spatial_relation_info_pdc_r17_s_ref_sig_c__alternatives,
    4,
    "spatial_relation_info_pdc_r17_s::ref_sig_c_"};

uint8_t& spatial_relation_info_pdc_r17_s::ref_sig_c_::set_ssb_idx()
{
  set(types::ssb_idx);
  return c.get<uint8_t>();
}
uint8_t& spatial_relation_info_pdc_r17_s::ref_sig_c_::set_csi_rs_idx()
{
  set(types::csi_rs_idx);
  return c.get<uint8_t>();
}
uint8_t& spatial_relation_info_pdc_r17_s::ref_sig_c_::set_dl_prs_pdc()
{
  set(types::dl_prs_pdc);
  return c.get<uint8_t>();
}
spatial_relation_info_pdc_r17_s::ref_sig_c_::srs_s_& spatial_relation_info_pdc_r17_s::ref_sig_c_::set_srs()
{
  set(types::srs);
  return c.get<srs_s_>();
}
static const asn1::table::seq_field seq_table_spatial_relation_info_pdc_r17_s_ref_sig_c__srs_s__fields[] = {
    {offsetof(spatial_relation_info_pdc_r17_s::ref_sig_c_::srs_s_, res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(spatial_relation_info_pdc_r17_s::ref_sig_c_::srs_s_, ul_bwp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr}};
static const asn1::table::field_name seq_table_spatial_relation_info_pdc_r17_s_ref_sig_c__srs_s__names[] = {
    {"resourceId", nullptr},
    {"uplinkBWP", nullptr}};
static const asn1::table::seq_desc seq_table_spatial_relation_info_pdc_r17_s_ref_sig_c__srs_s_ = {
    {seq_table_spatial_relation_info_pdc_r17_s_ref_sig_c__srs_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_spatial_relation_info_pdc_r17_s_ref_sig_c__srs_s__names};
static const asn1::table::seq_field choice_table_spatial_relation_info_pdc_r17_s_ref_sig_c__alternatives[] = {
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
     UINT64_C(191),
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
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_spatial_relation_info_pdc_r17_s_ref_sig_c__srs_s_}};
static const asn1::table::field_name choice_table_spatial_relation_info_pdc_r17_s_ref_sig_c__names[] =
    {{"ssb-Index", nullptr}, {"csi-RS-Index", nullptr}, {"dl-PRS-PDC", nullptr}, {"srs", nullptr}};
const asn1::table::choice_desc spatial_relation_info_pdc_r17_s::ref_sig_c_::desc = {
    asn1::table::enum_int_type<spatial_relation_info_pdc_r17_s::ref_sig_c_::types>(),
    spatial_relation_info_pdc_r17_s::ref_sig_c_::types::nof_types,
    spatial_relation_info_pdc_r17_s::ref_sig_c_::types::nof_exts,
    spatial_relation_info_pdc_r17_s::ref_sig_c_::types::has_ext,
    false,
    choice_table_spatial_relation_info_pdc_r17_s_ref_sig_c__alternatives,
    nullptr,
    choice_table_spatial_relation_info_pdc_r17_s_ref_sig_c__names,
    "spatial_relation_info_pdc_r17_s::ref_sig_c_",
    offsetof(spatial_relation_info_pdc_r17_s::ref_sig_c_, c.type),
    offsetof(spatial_relation_info_pdc_r17_s::ref_sig_c_, c.value),
    false,
    &spatial_relation_info_pdc_r17_s::ref_sig_c_::storage_desc};

static const char*           enum_names_spatial_relation_info_pdc_r17_s_ref_sig_c__types_values[] = {"ssb-Index",
                                                                                                     "csi-RS-Index",
                                                                                                     "dl-PRS-PDC",
                                                                                                     "srs"};
const asn1::table::enum_desc spatial_relation_info_pdc_r17_s::ref_sig_c_::types_opts::desc        = {
    enum_names_spatial_relation_info_pdc_r17_s_ref_sig_c__types_values,
    4,
    "spatial_relation_info_pdc_r17_s::ref_sig_c_::types"};

// SRS-Resource ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_res_s_res_map_s__fields[] = {
    {offsetof(srs_res_s::res_map_s_, start_position),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(5),
     nullptr},
    {offsetof(srs_res_s::res_map_s_, nrof_symbols),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::res_map_s_::nrof_symbols)>(),
     decltype(srs_res_s::res_map_s_::nrof_symbols)::has_ext,
     false,
     decltype(srs_res_s::res_map_s_::nrof_symbols)::nof_types,
     decltype(srs_res_s::res_map_s_::nrof_symbols)::nof_exts,
     nullptr,
     &srs_res_s::res_map_s_::nrof_symbols_opts::desc},
    {offsetof(srs_res_s::res_map_s_, repeat_factor),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::res_map_s_::repeat_factor)>(),
     decltype(srs_res_s::res_map_s_::repeat_factor)::has_ext,
     false,
     decltype(srs_res_s::res_map_s_::repeat_factor)::nof_types,
     decltype(srs_res_s::res_map_s_::repeat_factor)::nof_exts,
     nullptr,
     &srs_res_s::res_map_s_::repeat_factor_opts::desc}};
static const asn1::table::field_name seq_table_srs_res_s_res_map_s__names[] = {{"startPosition", nullptr},
                                                                               {"nrofSymbols", nullptr},
                                                                               {"repetitionFactor", nullptr}};
static const asn1::table::seq_desc   seq_table_srs_res_s_res_map_s_         = {
    {seq_table_srs_res_s_res_map_s__fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_res_map_s__names};
static const asn1::table::seq_field seq_table_srs_res_s_freq_hop_s__fields[] = {
    {offsetof(srs_res_s::freq_hop_s_, c_srs),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(srs_res_s::freq_hop_s_, b_srs),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(srs_res_s::freq_hop_s_, b_hop),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr}};
static const asn1::table::field_name seq_table_srs_res_s_freq_hop_s__names[] = {{"c-SRS", nullptr},
                                                                                {"b-SRS", nullptr},
                                                                                {"b-hop", nullptr}};
static const asn1::table::seq_desc   seq_table_srs_res_s_freq_hop_s_         = {
    {seq_table_srs_res_s_freq_hop_s__fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_freq_hop_s__names};
static const asn1::table::seq_field seq_table_srs_res_s_res_map_r16_s__fields[] = {
    {offsetof(srs_res_s::res_map_r16_s_, start_position_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(13),
     nullptr},
    {offsetof(srs_res_s::res_map_r16_s_, nrof_symbols_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::res_map_r16_s_::nrof_symbols_r16)>(),
     decltype(srs_res_s::res_map_r16_s_::nrof_symbols_r16)::has_ext,
     false,
     decltype(srs_res_s::res_map_r16_s_::nrof_symbols_r16)::nof_types,
     decltype(srs_res_s::res_map_r16_s_::nrof_symbols_r16)::nof_exts,
     nullptr,
     &srs_res_s::res_map_r16_s_::nrof_symbols_r16_opts::desc},
    {offsetof(srs_res_s::res_map_r16_s_, repeat_factor_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::res_map_r16_s_::repeat_factor_r16)>(),
     decltype(srs_res_s::res_map_r16_s_::repeat_factor_r16)::has_ext,
     false,
     decltype(srs_res_s::res_map_r16_s_::repeat_factor_r16)::nof_types,
     decltype(srs_res_s::res_map_r16_s_::repeat_factor_r16)::nof_exts,
     nullptr,
     &srs_res_s::res_map_r16_s_::repeat_factor_r16_opts::desc}};
static const asn1::table::field_name seq_table_srs_res_s_res_map_r16_s__names[] = {{"startPosition-r16", nullptr},
                                                                                   {"nrofSymbols-r16", nullptr},
                                                                                   {"repetitionFactor-r16", nullptr}};
static const asn1::table::seq_desc   seq_table_srs_res_s_res_map_r16_s_         = {
    {seq_table_srs_res_s_res_map_r16_s__fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_res_map_r16_s__names};
static const asn1::table::seq_field seq_table_srs_res_s_res_map_r17_s__fields[] = {
    {offsetof(srs_res_s::res_map_r17_s_, start_position_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(13),
     nullptr},
    {offsetof(srs_res_s::res_map_r17_s_, nrof_symbols_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::res_map_r17_s_::nrof_symbols_r17)>(),
     decltype(srs_res_s::res_map_r17_s_::nrof_symbols_r17)::has_ext,
     false,
     decltype(srs_res_s::res_map_r17_s_::nrof_symbols_r17)::nof_types,
     decltype(srs_res_s::res_map_r17_s_::nrof_symbols_r17)::nof_exts,
     nullptr,
     &srs_res_s::res_map_r17_s_::nrof_symbols_r17_opts::desc},
    {offsetof(srs_res_s::res_map_r17_s_, repeat_factor_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::res_map_r17_s_::repeat_factor_r17)>(),
     decltype(srs_res_s::res_map_r17_s_::repeat_factor_r17)::has_ext,
     false,
     decltype(srs_res_s::res_map_r17_s_::repeat_factor_r17)::nof_types,
     decltype(srs_res_s::res_map_r17_s_::repeat_factor_r17)::nof_exts,
     nullptr,
     &srs_res_s::res_map_r17_s_::repeat_factor_r17_opts::desc}};
static const asn1::table::field_name seq_table_srs_res_s_res_map_r17_s__names[] = {{"startPosition-r17", nullptr},
                                                                                   {"nrofSymbols-r17", nullptr},
                                                                                   {"repetitionFactor-r17", nullptr}};
static const asn1::table::seq_desc   seq_table_srs_res_s_res_map_r17_s_         = {
    {seq_table_srs_res_s_res_map_r17_s__fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_res_map_r17_s__names};
static const asn1::table::seq_field seq_table_srs_res_s_partial_freq_sr17_s__fields[] = {
    {offsetof(srs_res_s::partial_freq_sr17_s_, start_rb_idx_f_scaling_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::desc},
    {0,
     (int32_t)offsetof(srs_res_s::partial_freq_sr17_s_, enable_start_rb_hop_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_srs_res_s_partial_freq_sr17_s__names[] = {
    {"startRBIndexFScaling-r17", nullptr},
    {"enableStartRBHopping-r17", "enable"}};
static const asn1::table::seq_opt seq_table_srs_res_s_partial_freq_sr17_s__opts[] = {
    {1, (int32_t)offsetof(srs_res_s::partial_freq_sr17_s_, enable_start_rb_hop_r17_present)}};
static const asn1::table::seq_desc seq_table_srs_res_s_partial_freq_sr17_s_ = {
    {seq_table_srs_res_s_partial_freq_sr17_s__fields,
     2,
     UINT64_C(0x1),
     seq_table_srs_res_s_partial_freq_sr17_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_partial_freq_sr17_s__names};
static const asn1::table::seq_field seq_table_srs_res_s_tx_comb_n8_r17_s__fields[] = {
    {offsetof(srs_res_s::tx_comb_n8_r17_s_, comb_offset_n8_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(srs_res_s::tx_comb_n8_r17_s_, cyclic_shift_n8_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(5),
     nullptr}};
static const asn1::table::field_name seq_table_srs_res_s_tx_comb_n8_r17_s__names[] = {{"combOffset-n8-r17", nullptr},
                                                                                      {"cyclicShift-n8-r17", nullptr}};
static const asn1::table::seq_desc   seq_table_srs_res_s_tx_comb_n8_r17_s_         = {
    {seq_table_srs_res_s_tx_comb_n8_r17_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_tx_comb_n8_r17_s__names};
static const asn1::table::seq_field seq_table_srs_res_s_srs_dl_or_joint_tci_state_v1730_s__fields[] = {
    {offsetof(srs_res_s::srs_dl_or_joint_tci_state_v1730_s_, cell_and_bwp_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &serving_cell_and_bwp_id_r17_s::desc}};
static const asn1::table::field_name seq_table_srs_res_s_srs_dl_or_joint_tci_state_v1730_s__names[] = {
    {"cellAndBWP-r17", nullptr}};
static const asn1::table::seq_desc seq_table_srs_res_s_srs_dl_or_joint_tci_state_v1730_s_ = {
    {seq_table_srs_res_s_srs_dl_or_joint_tci_state_v1730_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_srs_dl_or_joint_tci_state_v1730_s__names};
static const asn1::table::seq_field seq_table_srs_res_s_comb_offset_hop_r18_s__fields[] = {
    {offsetof(srs_res_s::comb_offset_hop_r18_s_, hop_id_r18),
     (int32_t)offsetof(srs_res_s::comb_offset_hop_r18_s_, hop_id_r18_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(srs_res_s::comb_offset_hop_r18_s_, hop_subset_r18),
     (int32_t)offsetof(srs_res_s::comb_offset_hop_r18_s_, hop_subset_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::desc},
    {offsetof(srs_res_s::comb_offset_hop_r18_s_, hop_with_repeat_r18),
     (int32_t)offsetof(srs_res_s::comb_offset_hop_r18_s_, hop_with_repeat_r18_present),
     asn1::table::enum_op<decltype(srs_res_s::comb_offset_hop_r18_s_::hop_with_repeat_r18)>(),
     decltype(srs_res_s::comb_offset_hop_r18_s_::hop_with_repeat_r18)::has_ext,
     false,
     decltype(srs_res_s::comb_offset_hop_r18_s_::hop_with_repeat_r18)::nof_types,
     decltype(srs_res_s::comb_offset_hop_r18_s_::hop_with_repeat_r18)::nof_exts,
     nullptr,
     &srs_res_s::comb_offset_hop_r18_s_::hop_with_repeat_r18_opts::desc}};
static const asn1::table::field_name seq_table_srs_res_s_comb_offset_hop_r18_s__names[] = {
    {"hoppingId-r18", nullptr},
    {"hoppingSubset-r18", nullptr},
    {"hoppingWithRepetition-r18", nullptr}};
static const asn1::table::seq_opt seq_table_srs_res_s_comb_offset_hop_r18_s__opts[] = {
    {0, (int32_t)offsetof(srs_res_s::comb_offset_hop_r18_s_, hop_id_r18_present)},
    {1, (int32_t)offsetof(srs_res_s::comb_offset_hop_r18_s_, hop_subset_r18_present)},
    {2, (int32_t)offsetof(srs_res_s::comb_offset_hop_r18_s_, hop_with_repeat_r18_present)}};
static const asn1::table::seq_desc seq_table_srs_res_s_comb_offset_hop_r18_s_ = {
    {seq_table_srs_res_s_comb_offset_hop_r18_s__fields,
     3,
     UINT64_C(0x0),
     seq_table_srs_res_s_comb_offset_hop_r18_s__opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_comb_offset_hop_r18_s__names};
static const asn1::table::seq_field seq_table_srs_res_s_cyclic_shift_hop_r18_s__fields[] = {
    {offsetof(srs_res_s::cyclic_shift_hop_r18_s_, hop_id_r18),
     (int32_t)offsetof(srs_res_s::cyclic_shift_hop_r18_s_, hop_id_r18_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(srs_res_s::cyclic_shift_hop_r18_s_, hop_subset_r18),
     (int32_t)offsetof(srs_res_s::cyclic_shift_hop_r18_s_, hop_subset_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::desc},
    {0,
     (int32_t)offsetof(srs_res_s::cyclic_shift_hop_r18_s_, hop_finer_granularity_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_srs_res_s_cyclic_shift_hop_r18_s__names[] = {
    {"hoppingId-r18", nullptr},
    {"hoppingSubset-r18", nullptr},
    {"hoppingFinerGranularity-r18", "enable"}};
static const asn1::table::seq_opt seq_table_srs_res_s_cyclic_shift_hop_r18_s__opts[] = {
    {0, (int32_t)offsetof(srs_res_s::cyclic_shift_hop_r18_s_, hop_id_r18_present)},
    {1, (int32_t)offsetof(srs_res_s::cyclic_shift_hop_r18_s_, hop_subset_r18_present)},
    {2, (int32_t)offsetof(srs_res_s::cyclic_shift_hop_r18_s_, hop_finer_granularity_r18_present)}};
static const asn1::table::seq_desc seq_table_srs_res_s_cyclic_shift_hop_r18_s_ = {
    {seq_table_srs_res_s_cyclic_shift_hop_r18_s__fields,
     3,
     UINT64_C(0x0),
     seq_table_srs_res_s_cyclic_shift_hop_r18_s__opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_cyclic_shift_hop_r18_s__names};
static const asn1::table::seq_field seq_table_srs_res_s_spatial_relation_info_pdc_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &spatial_relation_info_pdc_r17_s::desc}};
static const asn1::table::field_name seq_table_srs_res_s_spatial_relation_info_pdc_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_srs_res_s_spatial_relation_info_pdc_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_srs_res_s_spatial_relation_info_pdc_r17_setup_release_alternatives,
    nullptr,
    seq_table_srs_res_s_spatial_relation_info_pdc_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<spatial_relation_info_pdc_r17_s>::type_offset(),
    setup_release_c<spatial_relation_info_pdc_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_srs_res_s_fields[] = {
    {offsetof(srs_res_s, srs_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(srs_res_s, nrof_srs_ports),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::nrof_srs_ports)>(),
     decltype(srs_res_s::nrof_srs_ports)::has_ext,
     false,
     decltype(srs_res_s::nrof_srs_ports)::nof_types,
     decltype(srs_res_s::nrof_srs_ports)::nof_exts,
     nullptr,
     &srs_res_s::nrof_srs_ports_opts::desc},
    {offsetof(srs_res_s, ptrs_port_idx),
     (int32_t)offsetof(srs_res_s, ptrs_port_idx_present),
     asn1::table::enum_op<decltype(srs_res_s::ptrs_port_idx)>(),
     decltype(srs_res_s::ptrs_port_idx)::has_ext,
     false,
     decltype(srs_res_s::ptrs_port_idx)::nof_types,
     decltype(srs_res_s::ptrs_port_idx)::nof_exts,
     nullptr,
     &srs_res_s::ptrs_port_idx_opts::desc},
    {offsetof(srs_res_s, tx_comb),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_res_s::tx_comb_c_::desc},
    {offsetof(srs_res_s, res_map),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_res_s_res_map_s_},
    {offsetof(srs_res_s, freq_domain_position),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(67),
     nullptr},
    {offsetof(srs_res_s, freq_domain_shift),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(268),
     nullptr},
    {offsetof(srs_res_s, freq_hop),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_res_s_freq_hop_s_},
    {offsetof(srs_res_s, group_or_seq_hop),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_res_s::group_or_seq_hop)>(),
     decltype(srs_res_s::group_or_seq_hop)::has_ext,
     false,
     decltype(srs_res_s::group_or_seq_hop)::nof_types,
     decltype(srs_res_s::group_or_seq_hop)::nof_exts,
     nullptr,
     &srs_res_s::group_or_seq_hop_opts::desc},
    {offsetof(srs_res_s, res_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_res_s::res_type_c_::desc},
    {offsetof(srs_res_s, seq_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(srs_res_s, spatial_relation_info),
     (int32_t)offsetof(srs_res_s, spatial_relation_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_spatial_relation_info_s::desc}};
static const asn1::table::field_name seq_table_srs_res_s_names[] = {{"srs-ResourceId", nullptr},
                                                                    {"nrofSRS-Ports", nullptr},
                                                                    {"ptrs-PortIndex", nullptr},
                                                                    {"transmissionComb", nullptr},
                                                                    {"resourceMapping", nullptr},
                                                                    {"freqDomainPosition", nullptr},
                                                                    {"freqDomainShift", nullptr},
                                                                    {"freqHopping", nullptr},
                                                                    {"groupOrSequenceHopping", nullptr},
                                                                    {"resourceType", nullptr},
                                                                    {"sequenceId", nullptr},
                                                                    {"spatialRelationInfo", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_res_s_opts[]  = {
    {2, (int32_t)offsetof(srs_res_s, ptrs_port_idx_present)},
    {11, (int32_t)offsetof(srs_res_s, spatial_relation_info_present)}};
static const asn1::table::seq_field seq_table_srs_res_s_g0_fields[] = {
    {offsetof(srs_res_s, res_map_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(srs_res_s::res_map_r16)>::value,
     &seq_table_srs_res_s_res_map_r16_s_}};
static const asn1::table::field_name seq_table_srs_res_s_g0_names[]  = {{"resourceMapping-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_res_s_g0_opts[]   = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_srs_res_s_g1_fields[] = {
    {offsetof(srs_res_s, spatial_relation_info_pdc_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(srs_res_s::spatial_relation_info_pdc_r17)>::value,
      &seq_table_srs_res_s_spatial_relation_info_pdc_r17_setup_release},
    {offsetof(srs_res_s, res_map_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(srs_res_s::res_map_r17)>::value,
      &seq_table_srs_res_s_res_map_r17_s_},
    {offsetof(srs_res_s, partial_freq_sr17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(srs_res_s::partial_freq_sr17)>::value,
      &seq_table_srs_res_s_partial_freq_sr17_s_},
    {offsetof(srs_res_s, tx_comb_n8_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(srs_res_s::tx_comb_n8_r17)>::value,
      &seq_table_srs_res_s_tx_comb_n8_r17_s_},
    {offsetof(srs_res_s, srs_tci_state_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(srs_res_s::srs_tci_state_r17)>::value,
      &srs_res_s::srs_tci_state_r17_c_::desc}};
static const asn1::table::field_name seq_table_srs_res_s_g1_names[]  = {{"spatialRelationInfo-PDC-r17", nullptr},
                                                                        {"resourceMapping-r17", nullptr},
                                                                        {"partialFreqSounding-r17", nullptr},
                                                                        {"transmissionComb-n8-r17", nullptr},
                                                                        {"srs-TCI-State-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_res_s_g1_opts[]   = {{0, asn1::table::seq_field::ptr_presence},
                                                                        {1, asn1::table::seq_field::ptr_presence},
                                                                        {2, asn1::table::seq_field::ptr_presence},
                                                                        {3, asn1::table::seq_field::ptr_presence},
                                                                        {4, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_srs_res_s_g2_fields[] = {
    {0,
      (int32_t)offsetof(srs_res_s, repeat_factor_v1730_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr},
    {offsetof(srs_res_s, srs_dl_or_joint_tci_state_v1730),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(srs_res_s::srs_dl_or_joint_tci_state_v1730)>::value,
      &seq_table_srs_res_s_srs_dl_or_joint_tci_state_v1730_s_}};
static const asn1::table::field_name seq_table_srs_res_s_g2_names[] = {{"repetitionFactor-v1730", "n3"},
                                                                       {"srs-DLorJointTCI-State-v1730", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_res_s_g2_opts[]  = {
    {0, (int32_t)offsetof(srs_res_s, repeat_factor_v1730_present)},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_srs_res_s_g3_fields[] = {
    {offsetof(srs_res_s, nrof_srs_ports_n8_r18),
     (int32_t)offsetof(srs_res_s, nrof_srs_ports_n8_r18_present),
     asn1::table::enum_op<decltype(srs_res_s::nrof_srs_ports_n8_r18)>(),
     decltype(srs_res_s::nrof_srs_ports_n8_r18)::has_ext,
     false,
     decltype(srs_res_s::nrof_srs_ports_n8_r18)::nof_types,
     decltype(srs_res_s::nrof_srs_ports_n8_r18)::nof_exts,
     nullptr,
     &srs_res_s::nrof_srs_ports_n8_r18_opts::desc},
    {offsetof(srs_res_s, comb_offset_hop_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(srs_res_s::comb_offset_hop_r18)>::value,
     &seq_table_srs_res_s_comb_offset_hop_r18_s_},
    {offsetof(srs_res_s, cyclic_shift_hop_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(srs_res_s::cyclic_shift_hop_r18)>::value,
     &seq_table_srs_res_s_cyclic_shift_hop_r18_s_}};
static const asn1::table::field_name seq_table_srs_res_s_g3_names[] = {{"nrofSRS-Ports-n8-r18", nullptr},
                                                                       {"combOffsetHopping-r18", nullptr},
                                                                       {"cyclicShiftHopping-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_res_s_g3_opts[]  = {
    {0, (int32_t)offsetof(srs_res_s, nrof_srs_ports_n8_r18_present)},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_srs_res_s_groups[] = {
    {{seq_table_srs_res_s_g0_fields, 1, UINT64_C(0x0), seq_table_srs_res_s_g0_opts, 1},
     false,
     seq_table_srs_res_s_g0_names},
    {{seq_table_srs_res_s_g1_fields, 5, UINT64_C(0x0), seq_table_srs_res_s_g1_opts, 5},
     false,
     seq_table_srs_res_s_g1_names},
    {{seq_table_srs_res_s_g2_fields, 2, UINT64_C(0x0), seq_table_srs_res_s_g2_opts, 2},
     false,
     seq_table_srs_res_s_g2_names},
    {{seq_table_srs_res_s_g3_fields, 3, UINT64_C(0x0), seq_table_srs_res_s_g3_opts, 3},
     false,
     seq_table_srs_res_s_g3_names}};
const asn1::table::seq_desc srs_res_s::desc = {
    {seq_table_srs_res_s_fields, 12, UINT64_C(0x7fb), seq_table_srs_res_s_opts, 2},
    true,
    false,
    seq_table_srs_res_s_groups,
    4,
    seq_table_srs_res_s_names};

static const char*           enum_names_srs_res_s_nrof_srs_ports_e__values[] = {"port1", "ports2", "ports4"};
const asn1::table::enum_desc srs_res_s::nrof_srs_ports_opts::desc = {enum_names_srs_res_s_nrof_srs_ports_e__values,
                                                                     3,
                                                                     "srs_res_s::nrof_srs_ports_e_"};
uint8_t                      srs_res_s::nrof_srs_ports_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::nrof_srs_ports_e_");
}

static const char*           enum_names_srs_res_s_ptrs_port_idx_e__values[] = {"n0", "n1"};
const asn1::table::enum_desc srs_res_s::ptrs_port_idx_opts::desc = {enum_names_srs_res_s_ptrs_port_idx_e__values,
                                                                    2,
                                                                    "srs_res_s::ptrs_port_idx_e_"};
uint8_t                      srs_res_s::ptrs_port_idx_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "srs_res_s::ptrs_port_idx_e_");
}

static const asn1::table::storage_ops* const choice_storage_srs_res_s_tx_comb_c__alternatives[] = {
    &asn1::table::alternative_ops<srs_res_s::tx_comb_c_::n2_s_, false>::value,
    &asn1::table::alternative_ops<srs_res_s::tx_comb_c_::n4_s_, false>::value};
const asn1::table::choice_storage_desc srs_res_s::tx_comb_c_::storage_desc = {
    choice_storage_srs_res_s_tx_comb_c__alternatives,
    2,
    "srs_res_s::tx_comb_c_"};

srs_res_s::tx_comb_c_::n2_s_& srs_res_s::tx_comb_c_::set_n2()
{
  set(types::n2);
  return c.get<n2_s_>();
}
srs_res_s::tx_comb_c_::n4_s_& srs_res_s::tx_comb_c_::set_n4()
{
  set(types::n4);
  return c.get<n4_s_>();
}
static const asn1::table::seq_field seq_table_srs_res_s_tx_comb_c__n2_s__fields[] = {
    {offsetof(srs_res_s::tx_comb_c_::n2_s_, comb_offset_n2),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(1),
     nullptr},
    {offsetof(srs_res_s::tx_comb_c_::n2_s_, cyclic_shift_n2),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr}};
static const asn1::table::field_name seq_table_srs_res_s_tx_comb_c__n2_s__names[] = {{"combOffset-n2", nullptr},
                                                                                     {"cyclicShift-n2", nullptr}};
static const asn1::table::seq_desc   seq_table_srs_res_s_tx_comb_c__n2_s_         = {
    {seq_table_srs_res_s_tx_comb_c__n2_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_tx_comb_c__n2_s__names};
static const asn1::table::seq_field seq_table_srs_res_s_tx_comb_c__n4_s__fields[] = {
    {offsetof(srs_res_s::tx_comb_c_::n4_s_, comb_offset_n4),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(srs_res_s::tx_comb_c_::n4_s_, cyclic_shift_n4),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(11),
     nullptr}};
static const asn1::table::field_name seq_table_srs_res_s_tx_comb_c__n4_s__names[] = {{"combOffset-n4", nullptr},
                                                                                     {"cyclicShift-n4", nullptr}};
static const asn1::table::seq_desc   seq_table_srs_res_s_tx_comb_c__n4_s_         = {
    {seq_table_srs_res_s_tx_comb_c__n4_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_tx_comb_c__n4_s__names};
static const asn1::table::seq_field choice_table_srs_res_s_tx_comb_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_res_s_tx_comb_c__n2_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_res_s_tx_comb_c__n4_s_}};
static const asn1::table::field_name choice_table_srs_res_s_tx_comb_c__names[] = {{"n2", nullptr}, {"n4", nullptr}};
const asn1::table::choice_desc       srs_res_s::tx_comb_c_::desc               = {
    asn1::table::enum_int_type<srs_res_s::tx_comb_c_::types>(),
    srs_res_s::tx_comb_c_::types::nof_types,
    srs_res_s::tx_comb_c_::types::nof_exts,
    srs_res_s::tx_comb_c_::types::has_ext,
    false,
    choice_table_srs_res_s_tx_comb_c__alternatives,
    nullptr,
    choice_table_srs_res_s_tx_comb_c__names,
    "srs_res_s::tx_comb_c_",
    offsetof(srs_res_s::tx_comb_c_, c.type),
    offsetof(srs_res_s::tx_comb_c_, c.value),
    false,
    &srs_res_s::tx_comb_c_::storage_desc};

static const char*           enum_names_srs_res_s_tx_comb_c__types_values[] = {"n2", "n4"};
const asn1::table::enum_desc srs_res_s::tx_comb_c_::types_opts::desc = {enum_names_srs_res_s_tx_comb_c__types_values,
                                                                        2,
                                                                        "srs_res_s::tx_comb_c_::types"};
uint8_t                      srs_res_s::tx_comb_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "srs_res_s::tx_comb_c_::types");
}

static const char*           enum_names_srs_res_s_res_map_s__nrof_symbols_e__values[] = {"n1", "n2", "n4"};
const asn1::table::enum_desc srs_res_s::res_map_s_::nrof_symbols_opts::desc           = {
    enum_names_srs_res_s_res_map_s__nrof_symbols_e__values,
    3,
    "srs_res_s::res_map_s_::nrof_symbols_e_"};
uint8_t srs_res_s::res_map_s_::nrof_symbols_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::res_map_s_::nrof_symbols_e_");
}

static const char*           enum_names_srs_res_s_res_map_s__repeat_factor_e__values[] = {"n1", "n2", "n4"};
const asn1::table::enum_desc srs_res_s::res_map_s_::repeat_factor_opts::desc           = {
    enum_names_srs_res_s_res_map_s__repeat_factor_e__values,
    3,
    "srs_res_s::res_map_s_::repeat_factor_e_"};
uint8_t srs_res_s::res_map_s_::repeat_factor_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::res_map_s_::repeat_factor_e_");
}

static const char* enum_names_srs_res_s_group_or_seq_hop_e__values[] = {"neither", "groupHopping", "sequenceHopping"};
const asn1::table::enum_desc srs_res_s::group_or_seq_hop_opts::desc  = {enum_names_srs_res_s_group_or_seq_hop_e__values,
                                                                        3,
                                                                        "srs_res_s::group_or_seq_hop_e_"};

static const asn1::table::storage_ops* const choice_storage_srs_res_s_res_type_c__alternatives[] = {
    &asn1::table::alternative_ops<srs_res_s::res_type_c_::aperiodic_s_, false>::value,
    &asn1::table::alternative_ops<srs_res_s::res_type_c_::semi_persistent_s_, false>::value,
    &asn1::table::alternative_ops<srs_res_s::res_type_c_::periodic_s_, false>::value};
const asn1::table::choice_storage_desc srs_res_s::res_type_c_::storage_desc = {
    choice_storage_srs_res_s_res_type_c__alternatives,
    3,
    "srs_res_s::res_type_c_"};

srs_res_s::res_type_c_::aperiodic_s_& srs_res_s::res_type_c_::set_aperiodic()
{
  set(types::aperiodic);
  return c.get<aperiodic_s_>();
}
srs_res_s::res_type_c_::semi_persistent_s_& srs_res_s::res_type_c_::set_semi_persistent()
{
  set(types::semi_persistent);
  return c.get<semi_persistent_s_>();
}
srs_res_s::res_type_c_::periodic_s_& srs_res_s::res_type_c_::set_periodic()
{
  set(types::periodic);
  return c.get<periodic_s_>();
}
static const asn1::table::seq_desc seq_table_srs_res_s_res_type_c__aperiodic_s_ =
    {{nullptr, 0, UINT64_C(0x0), nullptr, 0}, true, false, nullptr, 0, nullptr};
static const asn1::table::seq_field seq_table_srs_res_s_res_type_c__semi_persistent_s__fields[] = {
    {offsetof(srs_res_s::res_type_c_::semi_persistent_s_, periodicity_and_offset_sp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_periodicity_and_offset_c::desc}};
static const asn1::table::field_name seq_table_srs_res_s_res_type_c__semi_persistent_s__names[] = {
    {"periodicityAndOffset-sp", nullptr}};
static const asn1::table::seq_desc seq_table_srs_res_s_res_type_c__semi_persistent_s_ = {
    {seq_table_srs_res_s_res_type_c__semi_persistent_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_res_type_c__semi_persistent_s__names};
static const asn1::table::seq_field seq_table_srs_res_s_res_type_c__periodic_s__fields[] = {
    {offsetof(srs_res_s::res_type_c_::periodic_s_, periodicity_and_offset_p),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_periodicity_and_offset_c::desc}};
static const asn1::table::field_name seq_table_srs_res_s_res_type_c__periodic_s__names[] = {
    {"periodicityAndOffset-p", nullptr}};
static const asn1::table::seq_desc seq_table_srs_res_s_res_type_c__periodic_s_ = {
    {seq_table_srs_res_s_res_type_c__periodic_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_srs_res_s_res_type_c__periodic_s__names};
static const asn1::table::seq_field choice_table_srs_res_s_res_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_res_s_res_type_c__aperiodic_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_res_s_res_type_c__semi_persistent_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_res_s_res_type_c__periodic_s_}};
static const asn1::table::field_name choice_table_srs_res_s_res_type_c__names[] = {{"aperiodic", nullptr},
                                                                                   {"semi-persistent", nullptr},
                                                                                   {"periodic", nullptr}};
const asn1::table::choice_desc       srs_res_s::res_type_c_::desc               = {
    asn1::table::enum_int_type<srs_res_s::res_type_c_::types>(),
    srs_res_s::res_type_c_::types::nof_types,
    srs_res_s::res_type_c_::types::nof_exts,
    srs_res_s::res_type_c_::types::has_ext,
    false,
    choice_table_srs_res_s_res_type_c__alternatives,
    nullptr,
    choice_table_srs_res_s_res_type_c__names,
    "srs_res_s::res_type_c_",
    offsetof(srs_res_s::res_type_c_, c.type),
    offsetof(srs_res_s::res_type_c_, c.value),
    false,
    &srs_res_s::res_type_c_::storage_desc};

static const char* enum_names_srs_res_s_res_type_c__types_values[]    = {"aperiodic", "semi-persistent", "periodic"};
const asn1::table::enum_desc srs_res_s::res_type_c_::types_opts::desc = {enum_names_srs_res_s_res_type_c__types_values,
                                                                         3,
                                                                         "srs_res_s::res_type_c_::types"};

static const char*           enum_names_srs_res_s_res_map_r16_s__nrof_symbols_r16_e__values[] = {"n1", "n2", "n4"};
const asn1::table::enum_desc srs_res_s::res_map_r16_s_::nrof_symbols_r16_opts::desc           = {
    enum_names_srs_res_s_res_map_r16_s__nrof_symbols_r16_e__values,
    3,
    "srs_res_s::res_map_r16_s_::nrof_symbols_r16_e_"};
uint8_t srs_res_s::res_map_r16_s_::nrof_symbols_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::res_map_r16_s_::nrof_symbols_r16_e_");
}

static const char*           enum_names_srs_res_s_res_map_r16_s__repeat_factor_r16_e__values[] = {"n1", "n2", "n4"};
const asn1::table::enum_desc srs_res_s::res_map_r16_s_::repeat_factor_r16_opts::desc           = {
    enum_names_srs_res_s_res_map_r16_s__repeat_factor_r16_e__values,
    3,
    "srs_res_s::res_map_r16_s_::repeat_factor_r16_e_"};
uint8_t srs_res_s::res_map_r16_s_::repeat_factor_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "srs_res_s::res_map_r16_s_::repeat_factor_r16_e_");
}

static const char* enum_names_srs_res_s_res_map_r17_s__nrof_symbols_r17_e__values[] =
    {"n1", "n2", "n4", "n8", "n10", "n12", "n14"};
const asn1::table::enum_desc srs_res_s::res_map_r17_s_::nrof_symbols_r17_opts::desc = {
    enum_names_srs_res_s_res_map_r17_s__nrof_symbols_r17_e__values,
    7,
    "srs_res_s::res_map_r17_s_::nrof_symbols_r17_e_"};
uint8_t srs_res_s::res_map_r17_s_::nrof_symbols_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 10, 12, 14};
  return map_enum_number(numbers, 7, value, "srs_res_s::res_map_r17_s_::nrof_symbols_r17_e_");
}

static const char* enum_names_srs_res_s_res_map_r17_s__repeat_factor_r17_e__values[] =
    {"n1", "n2", "n4", "n5", "n6", "n7", "n8", "n10", "n12", "n14"};
const asn1::table::enum_desc srs_res_s::res_map_r17_s_::repeat_factor_r17_opts::desc = {
    enum_names_srs_res_s_res_map_r17_s__repeat_factor_r17_e__values,
    10,
    "srs_res_s::res_map_r17_s_::repeat_factor_r17_e_"};
uint8_t srs_res_s::res_map_r17_s_::repeat_factor_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 5, 6, 7, 8, 10, 12, 14};
  return map_enum_number(numbers, 10, value, "srs_res_s::res_map_r17_s_::repeat_factor_r17_e_");
}

static const asn1::table::storage_ops* const
    choice_storage_srs_res_s_partial_freq_sr17_s__start_rb_idx_f_scaling_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::storage_desc = {
    choice_storage_srs_res_s_partial_freq_sr17_s__start_rb_idx_f_scaling_r17_c__alternatives,
    2,
    "srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_"};

uint8_t& srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::set_start_rb_idx_and_freq_scaling_factor2_r17()
{
  set(types::start_rb_idx_and_freq_scaling_factor2_r17);
  return c.get<uint8_t>();
}
uint8_t& srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::set_start_rb_idx_and_freq_scaling_factor4_r17()
{
  set(types::start_rb_idx_and_freq_scaling_factor4_r17);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field
    choice_table_srs_res_s_partial_freq_sr17_s__start_rb_idx_f_scaling_r17_c__alternatives[] = {
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
         nullptr}};
static const asn1::table::field_name choice_table_srs_res_s_partial_freq_sr17_s__start_rb_idx_f_scaling_r17_c__names[] =
    {{"startRBIndexAndFreqScalingFactor2-r17", nullptr}, {"startRBIndexAndFreqScalingFactor4-r17", nullptr}};
const asn1::table::choice_desc srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::desc = {
    asn1::table::enum_int_type<srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types>(),
    srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types::nof_types,
    srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types::nof_exts,
    srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types::has_ext,
    false,
    choice_table_srs_res_s_partial_freq_sr17_s__start_rb_idx_f_scaling_r17_c__alternatives,
    nullptr,
    choice_table_srs_res_s_partial_freq_sr17_s__start_rb_idx_f_scaling_r17_c__names,
    "srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_",
    offsetof(srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_, c.type),
    offsetof(srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_, c.value),
    false,
    &srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::storage_desc};

static const char* enum_names_srs_res_s_partial_freq_sr17_s__start_rb_idx_f_scaling_r17_c__types_values[] = {
    "startRBIndexAndFreqScalingFactor2-r17",
    "startRBIndexAndFreqScalingFactor4-r17"};
const asn1::table::enum_desc srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types_opts::desc = {
    enum_names_srs_res_s_partial_freq_sr17_s__start_rb_idx_f_scaling_r17_c__types_values,
    2,
    "srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types"};
uint8_t srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(numbers, 2, value, "srs_res_s::partial_freq_sr17_s_::start_rb_idx_f_scaling_r17_c_::types");
}

static const asn1::table::storage_ops* const choice_storage_srs_res_s_srs_tci_state_r17_c__alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc srs_res_s::srs_tci_state_r17_c_::storage_desc = {
    choice_storage_srs_res_s_srs_tci_state_r17_c__alternatives,
    2,
    "srs_res_s::srs_tci_state_r17_c_"};

uint8_t& srs_res_s::srs_tci_state_r17_c_::set_srs_ul_tci_state()
{
  set(types::srs_ul_tci_state);
  return c.get<uint8_t>();
}
uint8_t& srs_res_s::srs_tci_state_r17_c_::set_srs_dl_or_joint_tci_state()
{
  set(types::srs_dl_or_joint_tci_state);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_srs_res_s_srs_tci_state_r17_c__alternatives[] = {
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
     nullptr}};
static const asn1::table::field_name choice_table_srs_res_s_srs_tci_state_r17_c__names[] = {
    {"srs-UL-TCI-State", nullptr},
    {"srs-DLorJointTCI-State", nullptr}};
const asn1::table::choice_desc srs_res_s::srs_tci_state_r17_c_::desc = {
    asn1::table::enum_int_type<srs_res_s::srs_tci_state_r17_c_::types>(),
    srs_res_s::srs_tci_state_r17_c_::types::nof_types,
    srs_res_s::srs_tci_state_r17_c_::types::nof_exts,
    srs_res_s::srs_tci_state_r17_c_::types::has_ext,
    false,
    choice_table_srs_res_s_srs_tci_state_r17_c__alternatives,
    nullptr,
    choice_table_srs_res_s_srs_tci_state_r17_c__names,
    "srs_res_s::srs_tci_state_r17_c_",
    offsetof(srs_res_s::srs_tci_state_r17_c_, c.type),
    offsetof(srs_res_s::srs_tci_state_r17_c_, c.value),
    false,
    &srs_res_s::srs_tci_state_r17_c_::storage_desc};

static const char*           enum_names_srs_res_s_srs_tci_state_r17_c__types_values[] = {"srs-UL-TCI-State",
                                                                                         "srs-DLorJointTCI-State"};
const asn1::table::enum_desc srs_res_s::srs_tci_state_r17_c_::types_opts::desc        = {
    enum_names_srs_res_s_srs_tci_state_r17_c__types_values,
    2,
    "srs_res_s::srs_tci_state_r17_c_::types"};

static const char*           enum_names_srs_res_s_nrof_srs_ports_n8_r18_e__values[] = {"ports8", "ports8tdm"};
const asn1::table::enum_desc srs_res_s::nrof_srs_ports_n8_r18_opts::desc            = {
    enum_names_srs_res_s_nrof_srs_ports_n8_r18_e__values,
    2,
    "srs_res_s::nrof_srs_ports_n8_r18_e_"};

static const asn1::table::storage_ops* const
    choice_storage_srs_res_s_comb_offset_hop_r18_s__hop_subset_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<fixed_bitstring<4>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<8>, false>::value};
const asn1::table::choice_storage_desc srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::storage_desc = {
    choice_storage_srs_res_s_comb_offset_hop_r18_s__hop_subset_r18_c__alternatives,
    2,
    "srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_"};

fixed_bitstring<4>& srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::set_tx_comb_n4()
{
  set(types::tx_comb_n4);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<8>& srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::set_tx_comb_n8()
{
  set(types::tx_comb_n8);
  return c.get<fixed_bitstring<8>>();
}
static const asn1::table::seq_field choice_table_srs_res_s_comb_offset_hop_r18_s__hop_subset_r18_c__alternatives[] = {
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
     &asn1::table::object_codec<fixed_bitstring<8>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_srs_res_s_comb_offset_hop_r18_s__hop_subset_r18_c__names[] = {
    {"transmissionComb-n4", nullptr},
    {"transmissionComb-n8", nullptr}};
const asn1::table::choice_desc srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::desc = {
    asn1::table::enum_int_type<srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::types>(),
    srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::types::nof_types,
    srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::types::nof_exts,
    srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::types::has_ext,
    false,
    choice_table_srs_res_s_comb_offset_hop_r18_s__hop_subset_r18_c__alternatives,
    nullptr,
    choice_table_srs_res_s_comb_offset_hop_r18_s__hop_subset_r18_c__names,
    "srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_",
    offsetof(srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_, c.type),
    offsetof(srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_, c.value),
    false,
    &srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::storage_desc};

static const char* enum_names_srs_res_s_comb_offset_hop_r18_s__hop_subset_r18_c__types_values[] = {
    "transmissionComb-n4",
    "transmissionComb-n8"};
const asn1::table::enum_desc srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::types_opts::desc = {
    enum_names_srs_res_s_comb_offset_hop_r18_s__hop_subset_r18_c__types_values,
    2,
    "srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::types"};
uint8_t srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8};
  return map_enum_number(numbers, 2, value, "srs_res_s::comb_offset_hop_r18_s_::hop_subset_r18_c_::types");
}

static const char*           enum_names_srs_res_s_comb_offset_hop_r18_s__hop_with_repeat_r18_e__values[] = {"symbol",
                                                                                                            "repetition"};
const asn1::table::enum_desc srs_res_s::comb_offset_hop_r18_s_::hop_with_repeat_r18_opts::desc           = {
    enum_names_srs_res_s_comb_offset_hop_r18_s__hop_with_repeat_r18_e__values,
    2,
    "srs_res_s::comb_offset_hop_r18_s_::hop_with_repeat_r18_e_"};

static const asn1::table::storage_ops* const
    choice_storage_srs_res_s_cyclic_shift_hop_r18_s__hop_subset_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<fixed_bitstring<8>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<12>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<6>, false>::value};
const asn1::table::choice_storage_desc srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::storage_desc = {
    choice_storage_srs_res_s_cyclic_shift_hop_r18_s__hop_subset_r18_c__alternatives,
    3,
    "srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_"};

fixed_bitstring<8>& srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::set_tx_comb_n2()
{
  set(types::tx_comb_n2);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<12>& srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::set_tx_comb_n4()
{
  set(types::tx_comb_n4);
  return c.get<fixed_bitstring<12>>();
}
fixed_bitstring<6>& srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::set_tx_comb_n8()
{
  set(types::tx_comb_n8);
  return c.get<fixed_bitstring<6>>();
}
static const asn1::table::seq_field choice_table_srs_res_s_cyclic_shift_hop_r18_s__hop_subset_r18_c__alternatives[] = {
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
     &asn1::table::object_codec<fixed_bitstring<12>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<6>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_srs_res_s_cyclic_shift_hop_r18_s__hop_subset_r18_c__names[] = {
    {"transmissionComb-n2", nullptr},
    {"transmissionComb-n4", nullptr},
    {"transmissionComb-n8", nullptr}};
const asn1::table::choice_desc srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::desc = {
    asn1::table::enum_int_type<srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::types>(),
    srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::types::nof_types,
    srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::types::nof_exts,
    srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::types::has_ext,
    false,
    choice_table_srs_res_s_cyclic_shift_hop_r18_s__hop_subset_r18_c__alternatives,
    nullptr,
    choice_table_srs_res_s_cyclic_shift_hop_r18_s__hop_subset_r18_c__names,
    "srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_",
    offsetof(srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_, c.type),
    offsetof(srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_, c.value),
    false,
    &srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::storage_desc};

static const char* enum_names_srs_res_s_cyclic_shift_hop_r18_s__hop_subset_r18_c__types_values[] = {
    "transmissionComb-n2",
    "transmissionComb-n4",
    "transmissionComb-n8"};
const asn1::table::enum_desc srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::types_opts::desc = {
    enum_names_srs_res_s_cyclic_shift_hop_r18_s__hop_subset_r18_c__types_values,
    3,
    "srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::types"};
uint8_t srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8};
  return map_enum_number(numbers, 3, value, "srs_res_s::cyclic_shift_hop_r18_s_::hop_subset_r18_c_::types");
}

// SRS-PosResources-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_pos_resources_r16_s_fields[] = {
    {offsetof(srs_pos_resources_r16_s, max_num_srs_pos_res_set_per_bwp_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_pos_resources_r16_s::max_num_srs_pos_res_set_per_bwp_r16)>(),
     decltype(srs_pos_resources_r16_s::max_num_srs_pos_res_set_per_bwp_r16)::has_ext,
     false,
     decltype(srs_pos_resources_r16_s::max_num_srs_pos_res_set_per_bwp_r16)::nof_types,
     decltype(srs_pos_resources_r16_s::max_num_srs_pos_res_set_per_bwp_r16)::nof_exts,
     nullptr,
     &srs_pos_resources_r16_s::max_num_srs_pos_res_set_per_bwp_r16_opts::desc},
    {offsetof(srs_pos_resources_r16_s, max_num_srs_pos_res_per_bwp_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_pos_resources_r16_s::max_num_srs_pos_res_per_bwp_r16)>(),
     decltype(srs_pos_resources_r16_s::max_num_srs_pos_res_per_bwp_r16)::has_ext,
     false,
     decltype(srs_pos_resources_r16_s::max_num_srs_pos_res_per_bwp_r16)::nof_types,
     decltype(srs_pos_resources_r16_s::max_num_srs_pos_res_per_bwp_r16)::nof_exts,
     nullptr,
     &srs_pos_resources_r16_s::max_num_srs_pos_res_per_bwp_r16_opts::desc},
    {offsetof(srs_pos_resources_r16_s, max_num_srs_res_per_bwp_per_slot_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_pos_resources_r16_s::max_num_srs_res_per_bwp_per_slot_r16)>(),
     decltype(srs_pos_resources_r16_s::max_num_srs_res_per_bwp_per_slot_r16)::has_ext,
     false,
     decltype(srs_pos_resources_r16_s::max_num_srs_res_per_bwp_per_slot_r16)::nof_types,
     decltype(srs_pos_resources_r16_s::max_num_srs_res_per_bwp_per_slot_r16)::nof_exts,
     nullptr,
     &srs_pos_resources_r16_s::max_num_srs_res_per_bwp_per_slot_r16_opts::desc},
    {offsetof(srs_pos_resources_r16_s, max_num_periodic_srs_pos_res_per_bwp_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_r16)>(),
     decltype(srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_r16)::has_ext,
     false,
     decltype(srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_r16)::nof_types,
     decltype(srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_r16)::nof_exts,
     nullptr,
     &srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_r16_opts::desc},
    {offsetof(srs_pos_resources_r16_s, max_num_periodic_srs_pos_res_per_bwp_per_slot_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_per_slot_r16)>(),
     decltype(srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_per_slot_r16)::has_ext,
     false,
     decltype(srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_per_slot_r16)::nof_types,
     decltype(srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_per_slot_r16)::nof_exts,
     nullptr,
     &srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_per_slot_r16_opts::desc}};
static const asn1::table::field_name seq_table_srs_pos_resources_r16_s_names[] = {
    {"maxNumberSRS-PosResourceSetPerBWP-r16", nullptr},
    {"maxNumberSRS-PosResourcesPerBWP-r16", nullptr},
    {"maxNumberSRS-ResourcesPerBWP-PerSlot-r16", nullptr},
    {"maxNumberPeriodicSRS-PosResourcesPerBWP-r16", nullptr},
    {"maxNumberPeriodicSRS-PosResourcesPerBWP-PerSlot-r16", nullptr}};
const asn1::table::seq_desc srs_pos_resources_r16_s::desc = {
    {seq_table_srs_pos_resources_r16_s_fields, 5, UINT64_C(0x1f), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_pos_resources_r16_s_names};

static const char* enum_names_srs_pos_resources_r16_s_max_num_srs_pos_res_set_per_bwp_r16_e__values[] =
    {"n1", "n2", "n4", "n8", "n12", "n16"};
const asn1::table::enum_desc srs_pos_resources_r16_s::max_num_srs_pos_res_set_per_bwp_r16_opts::desc = {
    enum_names_srs_pos_resources_r16_s_max_num_srs_pos_res_set_per_bwp_r16_e__values,
    6,
    "srs_pos_resources_r16_s::max_num_srs_pos_res_set_per_bwp_r16_e_"};
uint8_t srs_pos_resources_r16_s::max_num_srs_pos_res_set_per_bwp_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 12, 16};
  return map_enum_number(numbers, 6, value, "srs_pos_resources_r16_s::max_num_srs_pos_res_set_per_bwp_r16_e_");
}

static const char* enum_names_srs_pos_resources_r16_s_max_num_srs_pos_res_per_bwp_r16_e__values[] =
    {"n1", "n2", "n4", "n8", "n16", "n32", "n64"};
const asn1::table::enum_desc srs_pos_resources_r16_s::max_num_srs_pos_res_per_bwp_r16_opts::desc = {
    enum_names_srs_pos_resources_r16_s_max_num_srs_pos_res_per_bwp_r16_e__values,
    7,
    "srs_pos_resources_r16_s::max_num_srs_pos_res_per_bwp_r16_e_"};
uint8_t srs_pos_resources_r16_s::max_num_srs_pos_res_per_bwp_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 7, value, "srs_pos_resources_r16_s::max_num_srs_pos_res_per_bwp_r16_e_");
}

static const char* enum_names_srs_pos_resources_r16_s_max_num_srs_res_per_bwp_per_slot_r16_e__values[] =
    {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10", "n12", "n14"};
const asn1::table::enum_desc srs_pos_resources_r16_s::max_num_srs_res_per_bwp_per_slot_r16_opts::desc = {
    enum_names_srs_pos_resources_r16_s_max_num_srs_res_per_bwp_per_slot_r16_e__values,
    10,
    "srs_pos_resources_r16_s::max_num_srs_res_per_bwp_per_slot_r16_e_"};
uint8_t srs_pos_resources_r16_s::max_num_srs_res_per_bwp_per_slot_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10, 12, 14};
  return map_enum_number(numbers, 10, value, "srs_pos_resources_r16_s::max_num_srs_res_per_bwp_per_slot_r16_e_");
}

static const char* enum_names_srs_pos_resources_r16_s_max_num_periodic_srs_pos_res_per_bwp_r16_e__values[] =
    {"n1", "n2", "n4", "n8", "n16", "n32", "n64"};
const asn1::table::enum_desc srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_r16_opts::desc = {
    enum_names_srs_pos_resources_r16_s_max_num_periodic_srs_pos_res_per_bwp_r16_e__values,
    7,
    "srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_r16_e_"};
uint8_t srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 7, value, "srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_r16_e_");
}

static const char* enum_names_srs_pos_resources_r16_s_max_num_periodic_srs_pos_res_per_bwp_per_slot_r16_e__values[] =
    {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10", "n12", "n14"};
const asn1::table::enum_desc srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_per_slot_r16_opts::desc = {
    enum_names_srs_pos_resources_r16_s_max_num_periodic_srs_pos_res_per_bwp_per_slot_r16_e__values,
    10,
    "srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_per_slot_r16_e_"};
uint8_t srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_per_slot_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10, 12, 14};
  return map_enum_number(
      numbers, 10, value, "srs_pos_resources_r16_s::max_num_periodic_srs_pos_res_per_bwp_per_slot_r16_e_");
}

// SRS-PosResourceAP-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_pos_res_ap_r16_s_fields[] = {
    {offsetof(srs_pos_res_ap_r16_s, max_num_ap_srs_pos_res_per_bwp_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16)>(),
     decltype(srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16)::has_ext,
     false,
     decltype(srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16)::nof_types,
     decltype(srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16)::nof_exts,
     nullptr,
     &srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16_opts::desc},
    {offsetof(srs_pos_res_ap_r16_s, max_num_ap_srs_pos_res_per_bwp_per_slot_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16)>(),
     decltype(srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16)::has_ext,
     false,
     decltype(srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16)::nof_types,
     decltype(srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16)::nof_exts,
     nullptr,
     &srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16_opts::desc}};
static const asn1::table::field_name seq_table_srs_pos_res_ap_r16_s_names[] = {
    {"maxNumberAP-SRS-PosResourcesPerBWP-r16", nullptr},
    {"maxNumberAP-SRS-PosResourcesPerBWP-PerSlot-r16", nullptr}};
const asn1::table::seq_desc srs_pos_res_ap_r16_s::desc = {
    {seq_table_srs_pos_res_ap_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_pos_res_ap_r16_s_names};

static const char* enum_names_srs_pos_res_ap_r16_s_max_num_ap_srs_pos_res_per_bwp_r16_e__values[] =
    {"n1", "n2", "n4", "n8", "n16", "n32", "n64"};
const asn1::table::enum_desc srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16_opts::desc = {
    enum_names_srs_pos_res_ap_r16_s_max_num_ap_srs_pos_res_per_bwp_r16_e__values,
    7,
    "srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16_e_"};
uint8_t srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 7, value, "srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_r16_e_");
}

static const char* enum_names_srs_pos_res_ap_r16_s_max_num_ap_srs_pos_res_per_bwp_per_slot_r16_e__values[] =
    {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10", "n12", "n14"};
const asn1::table::enum_desc srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16_opts::desc = {
    enum_names_srs_pos_res_ap_r16_s_max_num_ap_srs_pos_res_per_bwp_per_slot_r16_e__values,
    10,
    "srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16_e_"};
uint8_t srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10, 12, 14};
  return map_enum_number(numbers, 10, value, "srs_pos_res_ap_r16_s::max_num_ap_srs_pos_res_per_bwp_per_slot_r16_e_");
}

// SRS-PosResourceSP-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_pos_res_sp_r16_s_fields[] = {
    {offsetof(srs_pos_res_sp_r16_s, max_num_sp_srs_pos_res_per_bwp_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16)>(),
     decltype(srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16)::has_ext,
     false,
     decltype(srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16)::nof_types,
     decltype(srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16)::nof_exts,
     nullptr,
     &srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16_opts::desc},
    {offsetof(srs_pos_res_sp_r16_s, max_num_sp_srs_pos_res_per_bwp_per_slot_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16)>(),
     decltype(srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16)::has_ext,
     false,
     decltype(srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16)::nof_types,
     decltype(srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16)::nof_exts,
     nullptr,
     &srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16_opts::desc}};
static const asn1::table::field_name seq_table_srs_pos_res_sp_r16_s_names[] = {
    {"maxNumberSP-SRS-PosResourcesPerBWP-r16", nullptr},
    {"maxNumberSP-SRS-PosResourcesPerBWP-PerSlot-r16", nullptr}};
const asn1::table::seq_desc srs_pos_res_sp_r16_s::desc = {
    {seq_table_srs_pos_res_sp_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_pos_res_sp_r16_s_names};

static const char* enum_names_srs_pos_res_sp_r16_s_max_num_sp_srs_pos_res_per_bwp_r16_e__values[] =
    {"n1", "n2", "n4", "n8", "n16", "n32", "n64"};
const asn1::table::enum_desc srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16_opts::desc = {
    enum_names_srs_pos_res_sp_r16_s_max_num_sp_srs_pos_res_per_bwp_r16_e__values,
    7,
    "srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16_e_"};
uint8_t srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32, 64};
  return map_enum_number(numbers, 7, value, "srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_r16_e_");
}

static const char* enum_names_srs_pos_res_sp_r16_s_max_num_sp_srs_pos_res_per_bwp_per_slot_r16_e__values[] =
    {"n1", "n2", "n3", "n4", "n5", "n6", "n8", "n10", "n12", "n14"};
const asn1::table::enum_desc srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16_opts::desc = {
    enum_names_srs_pos_res_sp_r16_s_max_num_sp_srs_pos_res_per_bwp_per_slot_r16_e__values,
    10,
    "srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16_e_"};
uint8_t srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6, 8, 10, 12, 14};
  return map_enum_number(numbers, 10, value, "srs_pos_res_sp_r16_s::max_num_sp_srs_pos_res_per_bwp_per_slot_r16_e_");
}

// SRS-AllPosResources-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_all_pos_res_r16_s_fields[] = {
    {offsetof(srs_all_pos_res_r16_s, srs_pos_res_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_pos_resources_r16_s::desc},
    {offsetof(srs_all_pos_res_r16_s, srs_pos_res_ap_r16),
     (int32_t)offsetof(srs_all_pos_res_r16_s, srs_pos_res_ap_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_pos_res_ap_r16_s::desc},
    {offsetof(srs_all_pos_res_r16_s, srs_pos_res_sp_r16),
     (int32_t)offsetof(srs_all_pos_res_r16_s, srs_pos_res_sp_r16_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_pos_res_sp_r16_s::desc}};
static const asn1::table::field_name seq_table_srs_all_pos_res_r16_s_names[] = {{"srs-PosResources-r16", nullptr},
                                                                                {"srs-PosResourceAP-r16", nullptr},
                                                                                {"srs-PosResourceSP-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_all_pos_res_r16_s_opts[]  = {
    {1, (int32_t)offsetof(srs_all_pos_res_r16_s, srs_pos_res_ap_r16_present)},
    {2, (int32_t)offsetof(srs_all_pos_res_r16_s, srs_pos_res_sp_r16_present)}};
const asn1::table::seq_desc srs_all_pos_res_r16_s::desc = {
    {seq_table_srs_all_pos_res_r16_s_fields, 3, UINT64_C(0x1), seq_table_srs_all_pos_res_r16_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_all_pos_res_r16_s_names};
