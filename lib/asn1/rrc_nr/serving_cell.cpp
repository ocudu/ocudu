// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/serving_cell.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// SRS-PosConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_pos_cfg_r17_s_fields[] = {
    {offsetof(srs_pos_cfg_r17_s, srs_pos_res_set_to_release_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<decltype(srs_pos_cfg_r17_s::srs_pos_res_set_to_release_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(15), false, false>>::value},
    {offsetof(srs_pos_cfg_r17_s, srs_pos_res_set_to_add_mod_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(srs_pos_cfg_r17_s::srs_pos_res_set_to_add_mod_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &srs_pos_res_set_r16_s::desc},
    {offsetof(srs_pos_cfg_r17_s, srs_pos_res_to_release_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(srs_pos_cfg_r17_s::srs_pos_res_to_release_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, false>>::value},
    {offsetof(srs_pos_cfg_r17_s, srs_pos_res_to_add_mod_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(srs_pos_cfg_r17_s::srs_pos_res_to_add_mod_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &srs_pos_res_r16_s::desc}};
static const asn1::table::field_name seq_table_srs_pos_cfg_r17_s_names[] = {
    {"srs-PosResourceSetToReleaseList-r17", nullptr},
    {"srs-PosResourceSetToAddModList-r17", nullptr},
    {"srs-PosResourceToReleaseList-r17", nullptr},
    {"srs-PosResourceToAddModList-r17", nullptr}};
static const asn1::table::seq_opt seq_table_srs_pos_cfg_r17_s_opts[] = {{0, asn1::table::seq_field::list_presence},
                                                                        {1, asn1::table::seq_field::list_presence},
                                                                        {2, asn1::table::seq_field::list_presence},
                                                                        {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       srs_pos_cfg_r17_s::desc            = {
    {seq_table_srs_pos_cfg_r17_s_fields, 4, UINT64_C(0x0), seq_table_srs_pos_cfg_r17_s_opts, 4},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_pos_cfg_r17_s_names};

// RSRP-ChangeThreshold-r17 ::= ENUMERATED
static const char*           enum_names_rsrp_change_thres_r17_e_values[] = {"dB4",
                                                                            "dB6",
                                                                            "dB8",
                                                                            "dB10",
                                                                            "dB14",
                                                                            "dB18",
                                                                            "dB22",
                                                                            "dB26",
                                                                            "dB30",
                                                                            "dB34",
                                                                            "spare6",
                                                                            "spare5",
                                                                            "spare4",
                                                                            "spare3",
                                                                            "spare2",
                                                                            "spare1"};
const asn1::table::enum_desc rsrp_change_thres_r17_opts::desc            = {enum_names_rsrp_change_thres_r17_e_values,
                                                                            16,
                                                                            "rsrp_change_thres_r17_e"};
uint8_t                      rsrp_change_thres_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 6, 8, 10, 14, 18, 22, 26, 30, 34};
  return map_enum_number(numbers, 10, value, "rsrp_change_thres_r17_e");
}

// SCS-SpecificCarrier ::= SEQUENCE
static const asn1::table::seq_field seq_table_scs_specific_carrier_s_fields[] = {
    {offsetof(scs_specific_carrier_s, offset_to_carrier),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(2199),
     nullptr},
    {offsetof(scs_specific_carrier_s, subcarrier_spacing),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(scs_specific_carrier_s::subcarrier_spacing)>(),
     decltype(scs_specific_carrier_s::subcarrier_spacing)::has_ext,
     false,
     decltype(scs_specific_carrier_s::subcarrier_spacing)::nof_types,
     decltype(scs_specific_carrier_s::subcarrier_spacing)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(scs_specific_carrier_s, carrier_bw),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(275),
     nullptr}};
static const asn1::table::field_name seq_table_scs_specific_carrier_s_names[]     = {{"offsetToCarrier", nullptr},
                                                                                     {"subcarrierSpacing", nullptr},
                                                                                     {"carrierBandwidth", nullptr}};
static const asn1::table::seq_field  seq_table_scs_specific_carrier_s_g0_fields[] = {
    {offsetof(scs_specific_carrier_s, tx_direct_current_location),
      (int32_t)offsetof(scs_specific_carrier_s, tx_direct_current_location_present),
      asn1::table::field_op::int_u16,
      false,
      false,
      INT64_C(0),
      UINT64_C(4095),
      nullptr}};
static const asn1::table::field_name seq_table_scs_specific_carrier_s_g0_names[] = {
    {"txDirectCurrentLocation", nullptr}};
static const asn1::table::seq_opt seq_table_scs_specific_carrier_s_g0_opts[] = {
    {0, (int32_t)offsetof(scs_specific_carrier_s, tx_direct_current_location_present)}};
static const asn1::table::seq_group seq_table_scs_specific_carrier_s_groups[] = {
    {{seq_table_scs_specific_carrier_s_g0_fields, 1, UINT64_C(0x0), seq_table_scs_specific_carrier_s_g0_opts, 1},
     false,
     seq_table_scs_specific_carrier_s_g0_names}};
const asn1::table::seq_desc scs_specific_carrier_s::desc = {
    {seq_table_scs_specific_carrier_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    true,
    false,
    seq_table_scs_specific_carrier_s_groups,
    1,
    seq_table_scs_specific_carrier_s_names};

// TDD-UL-DL-Pattern ::= SEQUENCE
static const asn1::table::seq_field seq_table_tdd_ul_dl_pattern_s_fields[] = {
    {offsetof(tdd_ul_dl_pattern_s, dl_ul_tx_periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(tdd_ul_dl_pattern_s::dl_ul_tx_periodicity)>(),
     decltype(tdd_ul_dl_pattern_s::dl_ul_tx_periodicity)::has_ext,
     false,
     decltype(tdd_ul_dl_pattern_s::dl_ul_tx_periodicity)::nof_types,
     decltype(tdd_ul_dl_pattern_s::dl_ul_tx_periodicity)::nof_exts,
     nullptr,
     &tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::desc},
    {offsetof(tdd_ul_dl_pattern_s, nrof_dl_slots),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(320),
     nullptr},
    {offsetof(tdd_ul_dl_pattern_s, nrof_dl_symbols),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(13),
     nullptr},
    {offsetof(tdd_ul_dl_pattern_s, nrof_ul_slots),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(320),
     nullptr},
    {offsetof(tdd_ul_dl_pattern_s, nrof_ul_symbols),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(13),
     nullptr}};
static const asn1::table::field_name seq_table_tdd_ul_dl_pattern_s_names[] = {
    {"dl-UL-TransmissionPeriodicity", nullptr},
    {"nrofDownlinkSlots", nullptr},
    {"nrofDownlinkSymbols", nullptr},
    {"nrofUplinkSlots", nullptr},
    {"nrofUplinkSymbols", nullptr}};
static const asn1::table::seq_field seq_table_tdd_ul_dl_pattern_s_g0_fields[] = {
    {offsetof(tdd_ul_dl_pattern_s, dl_ul_tx_periodicity_v1530),
     (int32_t)offsetof(tdd_ul_dl_pattern_s, dl_ul_tx_periodicity_v1530_present),
     asn1::table::enum_op<decltype(tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530)>(),
     decltype(tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530)::has_ext,
     false,
     decltype(tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530)::nof_types,
     decltype(tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530)::nof_exts,
     nullptr,
     &tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530_opts::desc}};
static const asn1::table::field_name seq_table_tdd_ul_dl_pattern_s_g0_names[] = {
    {"dl-UL-TransmissionPeriodicity-v1530", nullptr}};
static const asn1::table::seq_opt seq_table_tdd_ul_dl_pattern_s_g0_opts[] = {
    {0, (int32_t)offsetof(tdd_ul_dl_pattern_s, dl_ul_tx_periodicity_v1530_present)}};
static const asn1::table::seq_group seq_table_tdd_ul_dl_pattern_s_groups[] = {
    {{seq_table_tdd_ul_dl_pattern_s_g0_fields, 1, UINT64_C(0x0), seq_table_tdd_ul_dl_pattern_s_g0_opts, 1},
     false,
     seq_table_tdd_ul_dl_pattern_s_g0_names}};
const asn1::table::seq_desc tdd_ul_dl_pattern_s::desc = {
    {seq_table_tdd_ul_dl_pattern_s_fields, 5, UINT64_C(0x1f), nullptr, 0},
    true,
    false,
    seq_table_tdd_ul_dl_pattern_s_groups,
    1,
    seq_table_tdd_ul_dl_pattern_s_names};

static const char* enum_names_tdd_ul_dl_pattern_s_dl_ul_tx_periodicity_e__values[] =
    {"ms0p5", "ms0p625", "ms1", "ms1p25", "ms2", "ms2p5", "ms5", "ms10"};
const asn1::table::enum_desc tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_opts::desc = {
    enum_names_tdd_ul_dl_pattern_s_dl_ul_tx_periodicity_e__values,
    8,
    "tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_e_"};

static const char*           enum_names_tdd_ul_dl_pattern_s_dl_ul_tx_periodicity_v1530_e__values[] = {"ms3", "ms4"};
const asn1::table::enum_desc tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530_opts::desc            = {
    enum_names_tdd_ul_dl_pattern_s_dl_ul_tx_periodicity_v1530_e__values,
    2,
    "tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530_e_"};
uint8_t tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 4};
  return map_enum_number(numbers, 2, value, "tdd_ul_dl_pattern_s::dl_ul_tx_periodicity_v1530_e_");
}

// TDD-UL-DL-ConfigCommon ::= SEQUENCE
static const asn1::table::seq_field seq_table_tdd_ul_dl_cfg_common_s_fields[] = {
    {offsetof(tdd_ul_dl_cfg_common_s, ref_subcarrier_spacing),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(tdd_ul_dl_cfg_common_s::ref_subcarrier_spacing)>(),
     decltype(tdd_ul_dl_cfg_common_s::ref_subcarrier_spacing)::has_ext,
     false,
     decltype(tdd_ul_dl_cfg_common_s::ref_subcarrier_spacing)::nof_types,
     decltype(tdd_ul_dl_cfg_common_s::ref_subcarrier_spacing)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(tdd_ul_dl_cfg_common_s, pattern1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tdd_ul_dl_pattern_s::desc},
    {offsetof(tdd_ul_dl_cfg_common_s, pattern2),
     (int32_t)offsetof(tdd_ul_dl_cfg_common_s, pattern2_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tdd_ul_dl_pattern_s::desc}};
static const asn1::table::field_name seq_table_tdd_ul_dl_cfg_common_s_names[] = {
    {"referenceSubcarrierSpacing", nullptr},
    {"pattern1", nullptr},
    {"pattern2", nullptr}};
static const asn1::table::seq_opt seq_table_tdd_ul_dl_cfg_common_s_opts[] = {
    {2, (int32_t)offsetof(tdd_ul_dl_cfg_common_s, pattern2_present)}};
const asn1::table::seq_desc tdd_ul_dl_cfg_common_s::desc = {
    {seq_table_tdd_ul_dl_cfg_common_s_fields, 3, UINT64_C(0x3), seq_table_tdd_ul_dl_cfg_common_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_tdd_ul_dl_cfg_common_s_names};

// AdditionalTDDConfig-perPCI-ToAddMod-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_add_tdd_cfg_per_pci_to_add_mod_r18_s_fields[] = {
    {offsetof(add_tdd_cfg_per_pci_to_add_mod_r18_s, add_tdd_cfg_idx_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(7),
     nullptr},
    {offsetof(add_tdd_cfg_per_pci_to_add_mod_r18_s, tdd_ul_dl_cfg_common_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tdd_ul_dl_cfg_common_s::desc}};
static const asn1::table::field_name seq_table_add_tdd_cfg_per_pci_to_add_mod_r18_s_names[] = {
    {"additionalTDDConfig-Index-r18", nullptr},
    {"tdd-UL-DL-ConfigurationCommon-r18", nullptr}};
const asn1::table::seq_desc add_tdd_cfg_per_pci_to_add_mod_r18_s::desc = {
    {seq_table_add_tdd_cfg_per_pci_to_add_mod_r18_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_add_tdd_cfg_per_pci_to_add_mod_r18_s_names};

// AvailabilityCombination-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_availability_combination_r16_s_fields[] = {
    {offsetof(availability_combination_r16_s, availability_combination_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(511),
     nullptr},
    {offsetof(availability_combination_r16_s, res_availability_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     256,
     &asn1::table::list_codec<decltype(availability_combination_r16_s::res_availability_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(7), false, false>>::value}};
static const asn1::table::field_name seq_table_availability_combination_r16_s_names[] = {
    {"availabilityCombinationId-r16", nullptr},
    {"resourceAvailability-r16", nullptr}};
const asn1::table::seq_desc availability_combination_r16_s::desc = {
    {seq_table_availability_combination_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_availability_combination_r16_s_names};

// RB-SetGroup-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_rb_set_group_r17_s_fields[] = {
    {offsetof(rb_set_group_r17_s, res_availability_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     256,
     &asn1::table::list_codec<decltype(rb_set_group_r17_s::res_availability_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(7), false, false>>::value},
    {offsetof(rb_set_group_r17_s, rb_sets_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<decltype(rb_set_group_r17_s::rb_sets_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(7), false, false>>::value}};
static const asn1::table::field_name seq_table_rb_set_group_r17_s_names[] = {{"resourceAvailability-r17", nullptr},
                                                                             {"rb-Sets-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_rb_set_group_r17_s_opts[]  = {{0, asn1::table::seq_field::list_presence},
                                                                             {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          rb_set_group_r17_s::desc             = {
    {seq_table_rb_set_group_r17_s_fields, 2, UINT64_C(0x0), seq_table_rb_set_group_r17_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_rb_set_group_r17_s_names};

// AvailabilityCombinationRB-Groups-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_availability_combination_rb_groups_r17_s_fields[] = {
    {offsetof(availability_combination_rb_groups_r17_s, availability_combination_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(511),
     nullptr},
    {offsetof(availability_combination_rb_groups_r17_s, rb_set_groups_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(availability_combination_rb_groups_r17_s::rb_set_groups_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &rb_set_group_r17_s::desc},
    {offsetof(availability_combination_rb_groups_r17_s, res_availability_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     256,
     &asn1::table::list_codec<decltype(availability_combination_rb_groups_r17_s::res_availability_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(7), false, false>>::value}};
static const asn1::table::field_name seq_table_availability_combination_rb_groups_r17_s_names[] = {
    {"availabilityCombinationId-r17", nullptr},
    {"rb-SetGroups-r17", nullptr},
    {"resourceAvailability-r17", nullptr}};
static const asn1::table::seq_opt seq_table_availability_combination_rb_groups_r17_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc availability_combination_rb_groups_r17_s::desc = {
    {seq_table_availability_combination_rb_groups_r17_s_fields,
     3,
     UINT64_C(0x1),
     seq_table_availability_combination_rb_groups_r17_s_opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_availability_combination_rb_groups_r17_s_names};

// AvailabilityCombinationsPerCell-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_availability_combinations_per_cell_r16_s_fields[] = {
    {offsetof(availability_combinations_per_cell_r16_s, availability_combinations_per_cell_idx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(512),
     nullptr},
    {offsetof(availability_combinations_per_cell_r16_s, iab_du_cell_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(availability_combinations_per_cell_r16_s::iab_du_cell_id_r16),
                                asn1::table::struct_json>::value},
    {offsetof(availability_combinations_per_cell_r16_s, position_in_dci_ai_r16),
     (int32_t)offsetof(availability_combinations_per_cell_r16_s, position_in_dci_ai_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(availability_combinations_per_cell_r16_s, availability_combinations_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::item_list_codec<decltype(availability_combinations_per_cell_r16_s::availability_combinations_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &availability_combination_r16_s::desc}};
static const asn1::table::field_name seq_table_availability_combinations_per_cell_r16_s_names[] = {
    {"availabilityCombinationsPerCellIndex-r16", nullptr},
    {"iab-DU-CellIdentity-r16", nullptr},
    {"positionInDCI-AI-r16", nullptr},
    {"availabilityCombinations-r16", nullptr}};
static const asn1::table::seq_opt seq_table_availability_combinations_per_cell_r16_s_opts[] = {
    {2, (int32_t)offsetof(availability_combinations_per_cell_r16_s, position_in_dci_ai_r16_present)}};
static const asn1::table::seq_field seq_table_availability_combinations_per_cell_r16_s_g0_fields[] = {
    {offsetof(availability_combinations_per_cell_r16_s, availability_combinations_rb_groups_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::ptr_item_list_codec<
         decltype(availability_combinations_per_cell_r16_s::availability_combinations_rb_groups_r17),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &availability_combination_rb_groups_r17_s::desc}};
static const asn1::table::field_name seq_table_availability_combinations_per_cell_r16_s_g0_names[] = {
    {"availabilityCombinationsRB-Groups-r17", nullptr}};
static const asn1::table::seq_opt seq_table_availability_combinations_per_cell_r16_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_availability_combinations_per_cell_r16_s_g1_fields[] = {
    {offsetof(availability_combinations_per_cell_r16_s, position_in_dci_ai_rb_groups_v1720),
     (int32_t)offsetof(availability_combinations_per_cell_r16_s, position_in_dci_ai_rb_groups_v1720_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr}};
static const asn1::table::field_name seq_table_availability_combinations_per_cell_r16_s_g1_names[] = {
    {"positionInDCI-AI-RBGroups-v1720", nullptr}};
static const asn1::table::seq_opt seq_table_availability_combinations_per_cell_r16_s_g1_opts[] = {
    {0, (int32_t)offsetof(availability_combinations_per_cell_r16_s, position_in_dci_ai_rb_groups_v1720_present)}};
static const asn1::table::seq_group seq_table_availability_combinations_per_cell_r16_s_groups[] = {
    {{seq_table_availability_combinations_per_cell_r16_s_g0_fields,
      1,
      UINT64_C(0x0),
      seq_table_availability_combinations_per_cell_r16_s_g0_opts,
      1},
     false,
     seq_table_availability_combinations_per_cell_r16_s_g0_names},
    {{seq_table_availability_combinations_per_cell_r16_s_g1_fields,
      1,
      UINT64_C(0x0),
      seq_table_availability_combinations_per_cell_r16_s_g1_opts,
      1},
     false,
     seq_table_availability_combinations_per_cell_r16_s_g1_names}};
const asn1::table::seq_desc availability_combinations_per_cell_r16_s::desc = {
    {seq_table_availability_combinations_per_cell_r16_s_fields,
     4,
     UINT64_C(0xb),
     seq_table_availability_combinations_per_cell_r16_s_opts,
     1},
    true,
    false,
    seq_table_availability_combinations_per_cell_r16_s_groups,
    2,
    seq_table_availability_combinations_per_cell_r16_s_names};

// AvailabilityIndicator-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_availability_ind_r16_s_fields[] = {
    {offsetof(availability_ind_r16_s, ai_rnti_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(availability_ind_r16_s, dci_payload_size_ai_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(128),
     nullptr},
    {offsetof(availability_ind_r16_s, available_comb_to_add_mod_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::item_list_codec<decltype(availability_ind_r16_s::available_comb_to_add_mod_list_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &availability_combinations_per_cell_r16_s::desc},
    {offsetof(availability_ind_r16_s, available_comb_to_release_list_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::list_codec<decltype(availability_ind_r16_s::available_comb_to_release_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(512), false, false>>::value}};
static const asn1::table::field_name seq_table_availability_ind_r16_s_names[] = {
    {"ai-RNTI-r16", nullptr},
    {"dci-PayloadSizeAI-r16", nullptr},
    {"availableCombToAddModList-r16", nullptr},
    {"availableCombToReleaseList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_availability_ind_r16_s_opts[] = {
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc availability_ind_r16_s::desc = {
    {seq_table_availability_ind_r16_s_fields, 4, UINT64_C(0x3), seq_table_availability_ind_r16_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_availability_ind_r16_s_names};

// AvailableRB-SetsPerCell-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_available_rb_sets_per_cell_r16_s_fields[] = {
    {offsetof(available_rb_sets_per_cell_r16_s, serving_cell_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(available_rb_sets_per_cell_r16_s, position_in_dci_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr}};
static const asn1::table::field_name seq_table_available_rb_sets_per_cell_r16_s_names[] = {
    {"servingCellId-r16", nullptr},
    {"positionInDCI-r16", nullptr}};
const asn1::table::seq_desc available_rb_sets_per_cell_r16_s::desc = {
    {seq_table_available_rb_sets_per_cell_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_available_rb_sets_per_cell_r16_s_names};

// BCCH-Config ::= SEQUENCE
static const asn1::table::seq_field seq_table_bcch_cfg_s_fields[] = {
    {offsetof(bcch_cfg_s, mod_period_coeff),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(bcch_cfg_s::mod_period_coeff)>(),
     decltype(bcch_cfg_s::mod_period_coeff)::has_ext,
     false,
     decltype(bcch_cfg_s::mod_period_coeff)::nof_types,
     decltype(bcch_cfg_s::mod_period_coeff)::nof_exts,
     nullptr,
     &bcch_cfg_s::mod_period_coeff_opts::desc}};
static const asn1::table::field_name seq_table_bcch_cfg_s_names[] = {{"modificationPeriodCoeff", nullptr}};
const asn1::table::seq_desc          bcch_cfg_s::desc =
    {{seq_table_bcch_cfg_s_fields, 1, UINT64_C(0x1), nullptr, 0}, true, false, nullptr, 0, seq_table_bcch_cfg_s_names};

static const char*           enum_names_bcch_cfg_s_mod_period_coeff_e__values[] = {"n2", "n4", "n8", "n16"};
const asn1::table::enum_desc bcch_cfg_s::mod_period_coeff_opts::desc            = {
    enum_names_bcch_cfg_s_mod_period_coeff_e__values,
    4,
    "bcch_cfg_s::mod_period_coeff_e_"};
uint8_t bcch_cfg_s::mod_period_coeff_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 8, 16};
  return map_enum_number(numbers, 4, value, "bcch_cfg_s::mod_period_coeff_e_");
}

// NR-NS-PmaxValue ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_ns_pmax_value_s_fields[] = {
    {offsetof(nr_ns_pmax_value_s, add_pmax),
     (int32_t)offsetof(nr_ns_pmax_value_s, add_pmax_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr},
    {offsetof(nr_ns_pmax_value_s, add_spec_emission),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr}};
static const asn1::table::field_name seq_table_nr_ns_pmax_value_s_names[] = {{"additionalPmax", nullptr},
                                                                             {"additionalSpectrumEmission", nullptr}};
static const asn1::table::seq_opt    seq_table_nr_ns_pmax_value_s_opts[]  = {
    {0, (int32_t)offsetof(nr_ns_pmax_value_s, add_pmax_present)}};
const asn1::table::seq_desc nr_ns_pmax_value_s::desc = {
    {seq_table_nr_ns_pmax_value_s_fields, 2, UINT64_C(0x2), seq_table_nr_ns_pmax_value_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_nr_ns_pmax_value_s_names};

// NR-MultiBandInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_multi_band_info_s_fields[] = {
    {offsetof(nr_multi_band_info_s, freq_band_ind_nr),
     (int32_t)offsetof(nr_multi_band_info_s, freq_band_ind_nr_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(1024),
     nullptr},
    {offsetof(nr_multi_band_info_s, nr_ns_pmax_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(nr_multi_band_info_s::nr_ns_pmax_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_ns_pmax_value_s::desc}};
static const asn1::table::field_name seq_table_nr_multi_band_info_s_names[] = {{"freqBandIndicatorNR", nullptr},
                                                                               {"nr-NS-PmaxList", nullptr}};
static const asn1::table::seq_opt    seq_table_nr_multi_band_info_s_opts[]  = {
    {0, (int32_t)offsetof(nr_multi_band_info_s, freq_band_ind_nr_present)},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc nr_multi_band_info_s::desc = {
    {seq_table_nr_multi_band_info_s_fields, 2, UINT64_C(0x0), seq_table_nr_multi_band_info_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_nr_multi_band_info_s_names};

// NR-NS-PmaxValue-v1760 ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_ns_pmax_value_v1760_s_fields[] = {
    {offsetof(nr_ns_pmax_value_v1760_s, add_spec_emission_v1760),
     (int32_t)offsetof(nr_ns_pmax_value_v1760_s, add_spec_emission_v1760_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(8),
     UINT64_C(39),
     nullptr}};
static const asn1::table::field_name seq_table_nr_ns_pmax_value_v1760_s_names[] = {
    {"additionalSpectrumEmission-v1760", nullptr}};
static const asn1::table::seq_opt seq_table_nr_ns_pmax_value_v1760_s_opts[] = {
    {0, (int32_t)offsetof(nr_ns_pmax_value_v1760_s, add_spec_emission_v1760_present)}};
const asn1::table::seq_desc nr_ns_pmax_value_v1760_s::desc = {
    {seq_table_nr_ns_pmax_value_v1760_s_fields, 1, UINT64_C(0x0), seq_table_nr_ns_pmax_value_v1760_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_nr_ns_pmax_value_v1760_s_names};

// NR-MultiBandInfo-v1760 ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_multi_band_info_v1760_s_fields[] = {
    {offsetof(nr_multi_band_info_v1760_s, nr_ns_pmax_list_v1760),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(nr_multi_band_info_v1760_s::nr_ns_pmax_list_v1760),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_ns_pmax_value_v1760_s::desc}};
static const asn1::table::field_name seq_table_nr_multi_band_info_v1760_s_names[] = {{"nr-NS-PmaxList-v1760", nullptr}};
static const asn1::table::seq_opt    seq_table_nr_multi_band_info_v1760_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc nr_multi_band_info_v1760_s::desc = {
    {seq_table_nr_multi_band_info_v1760_s_fields, 1, UINT64_C(0x0), seq_table_nr_multi_band_info_v1760_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_nr_multi_band_info_v1760_s_names};

// NR-NS-PmaxValueAerial-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_ns_pmax_value_aerial_r18_s_fields[] = {
    {offsetof(nr_ns_pmax_value_aerial_r18_s, add_pmax_r18),
     (int32_t)offsetof(nr_ns_pmax_value_aerial_r18_s, add_pmax_r18_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr},
    {offsetof(nr_ns_pmax_value_aerial_r18_s, add_spec_emission_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(39),
     nullptr}};
static const asn1::table::field_name seq_table_nr_ns_pmax_value_aerial_r18_s_names[] = {
    {"additionalPmax-r18", nullptr},
    {"additionalSpectrumEmission-r18", nullptr}};
static const asn1::table::seq_opt seq_table_nr_ns_pmax_value_aerial_r18_s_opts[] = {
    {0, (int32_t)offsetof(nr_ns_pmax_value_aerial_r18_s, add_pmax_r18_present)}};
const asn1::table::seq_desc nr_ns_pmax_value_aerial_r18_s::desc = {
    {seq_table_nr_ns_pmax_value_aerial_r18_s_fields, 2, UINT64_C(0x2), seq_table_nr_ns_pmax_value_aerial_r18_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_nr_ns_pmax_value_aerial_r18_s_names};

// NR-MultiBandInfoAerial-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_multi_band_info_aerial_r18_s_fields[] = {
    {offsetof(nr_multi_band_info_aerial_r18_s, freq_band_ind_nr_aerial_r18),
     (int32_t)offsetof(nr_multi_band_info_aerial_r18_s, freq_band_ind_nr_aerial_r18_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(1024),
     nullptr},
    {offsetof(nr_multi_band_info_aerial_r18_s, nr_ns_pmax_list_aerial_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(nr_multi_band_info_aerial_r18_s::nr_ns_pmax_list_aerial_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_ns_pmax_value_aerial_r18_s::desc}};
static const asn1::table::field_name seq_table_nr_multi_band_info_aerial_r18_s_names[] = {
    {"freqBandIndicatorNR-Aerial-r18", nullptr},
    {"nr-NS-PmaxListAerial-r18", nullptr}};
static const asn1::table::seq_opt seq_table_nr_multi_band_info_aerial_r18_s_opts[] = {
    {0, (int32_t)offsetof(nr_multi_band_info_aerial_r18_s, freq_band_ind_nr_aerial_r18_present)},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc nr_multi_band_info_aerial_r18_s::desc = {{seq_table_nr_multi_band_info_aerial_r18_s_fields,
                                                                      2,
                                                                      UINT64_C(0x0),
                                                                      seq_table_nr_multi_band_info_aerial_r18_s_opts,
                                                                      2},
                                                                     false,
                                                                     false,
                                                                     nullptr,
                                                                     0,
                                                                     seq_table_nr_multi_band_info_aerial_r18_s_names};

// EUTRA-MBSFN-SubframeConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_eutra_mbsfn_sf_cfg_s_fields[] = {
    {offsetof(eutra_mbsfn_sf_cfg_s, radioframe_alloc_period),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(eutra_mbsfn_sf_cfg_s::radioframe_alloc_period)>(),
     decltype(eutra_mbsfn_sf_cfg_s::radioframe_alloc_period)::has_ext,
     false,
     decltype(eutra_mbsfn_sf_cfg_s::radioframe_alloc_period)::nof_types,
     decltype(eutra_mbsfn_sf_cfg_s::radioframe_alloc_period)::nof_exts,
     nullptr,
     &eutra_mbsfn_sf_cfg_s::radioframe_alloc_period_opts::desc},
    {offsetof(eutra_mbsfn_sf_cfg_s, radioframe_alloc_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(eutra_mbsfn_sf_cfg_s, sf_alloc1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::desc},
    {offsetof(eutra_mbsfn_sf_cfg_s, sf_alloc2),
     (int32_t)offsetof(eutra_mbsfn_sf_cfg_s, sf_alloc2_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::desc}};
static const asn1::table::field_name seq_table_eutra_mbsfn_sf_cfg_s_names[] = {{"radioframeAllocationPeriod", nullptr},
                                                                               {"radioframeAllocationOffset", nullptr},
                                                                               {"subframeAllocation1", nullptr},
                                                                               {"subframeAllocation2", nullptr}};
static const asn1::table::seq_opt    seq_table_eutra_mbsfn_sf_cfg_s_opts[]  = {
    {3, (int32_t)offsetof(eutra_mbsfn_sf_cfg_s, sf_alloc2_present)}};
const asn1::table::seq_desc eutra_mbsfn_sf_cfg_s::desc = {
    {seq_table_eutra_mbsfn_sf_cfg_s_fields, 4, UINT64_C(0x7), seq_table_eutra_mbsfn_sf_cfg_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_eutra_mbsfn_sf_cfg_s_names};

static const char* enum_names_eutra_mbsfn_sf_cfg_s_radioframe_alloc_period_e__values[] =
    {"n1", "n2", "n4", "n8", "n16", "n32"};
const asn1::table::enum_desc eutra_mbsfn_sf_cfg_s::radioframe_alloc_period_opts::desc = {
    enum_names_eutra_mbsfn_sf_cfg_s_radioframe_alloc_period_e__values,
    6,
    "eutra_mbsfn_sf_cfg_s::radioframe_alloc_period_e_"};
uint8_t eutra_mbsfn_sf_cfg_s::radioframe_alloc_period_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32};
  return map_enum_number(numbers, 6, value, "eutra_mbsfn_sf_cfg_s::radioframe_alloc_period_e_");
}

static const asn1::table::storage_ops* const choice_storage_eutra_mbsfn_sf_cfg_s_sf_alloc1_c__alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<6>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<24>, false>::value};
const asn1::table::choice_storage_desc eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::storage_desc = {
    choice_storage_eutra_mbsfn_sf_cfg_s_sf_alloc1_c__alternatives,
    2,
    "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_"};

fixed_bitstring<6>& eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::set_one_frame()
{
  set(types::one_frame);
  return c.get<fixed_bitstring<6>>();
}
fixed_bitstring<24>& eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::set_four_frames()
{
  set(types::four_frames);
  return c.get<fixed_bitstring<24>>();
}
static const asn1::table::seq_field choice_table_eutra_mbsfn_sf_cfg_s_sf_alloc1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<6>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<24>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_eutra_mbsfn_sf_cfg_s_sf_alloc1_c__names[] = {{"oneFrame", nullptr},
                                                                                               {"fourFrames", nullptr}};
const asn1::table::choice_desc       eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::desc               = {
    asn1::table::enum_int_type<eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types>(),
    eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types::nof_types,
    eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types::nof_exts,
    eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types::has_ext,
    false,
    choice_table_eutra_mbsfn_sf_cfg_s_sf_alloc1_c__alternatives,
    nullptr,
    choice_table_eutra_mbsfn_sf_cfg_s_sf_alloc1_c__names,
    "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_",
    offsetof(eutra_mbsfn_sf_cfg_s::sf_alloc1_c_, c.type),
    offsetof(eutra_mbsfn_sf_cfg_s::sf_alloc1_c_, c.value),
    false,
    &eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::storage_desc};

static const char*           enum_names_eutra_mbsfn_sf_cfg_s_sf_alloc1_c__types_values[] = {"oneFrame", "fourFrames"};
const asn1::table::enum_desc eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types_opts::desc        = {
    enum_names_eutra_mbsfn_sf_cfg_s_sf_alloc1_c__types_values,
    2,
    "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types"};
uint8_t eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 4};
  return map_enum_number(numbers, 2, value, "eutra_mbsfn_sf_cfg_s::sf_alloc1_c_::types");
}

static const asn1::table::storage_ops* const choice_storage_eutra_mbsfn_sf_cfg_s_sf_alloc2_c__alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<2>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<8>, false>::value};
const asn1::table::choice_storage_desc eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::storage_desc = {
    choice_storage_eutra_mbsfn_sf_cfg_s_sf_alloc2_c__alternatives,
    2,
    "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_"};

fixed_bitstring<2>& eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::set_one_frame()
{
  set(types::one_frame);
  return c.get<fixed_bitstring<2>>();
}
fixed_bitstring<8>& eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::set_four_frames()
{
  set(types::four_frames);
  return c.get<fixed_bitstring<8>>();
}
static const asn1::table::seq_field choice_table_eutra_mbsfn_sf_cfg_s_sf_alloc2_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<2>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<8>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_eutra_mbsfn_sf_cfg_s_sf_alloc2_c__names[] = {{"oneFrame", nullptr},
                                                                                               {"fourFrames", nullptr}};
const asn1::table::choice_desc       eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::desc               = {
    asn1::table::enum_int_type<eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types>(),
    eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types::nof_types,
    eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types::nof_exts,
    eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types::has_ext,
    false,
    choice_table_eutra_mbsfn_sf_cfg_s_sf_alloc2_c__alternatives,
    nullptr,
    choice_table_eutra_mbsfn_sf_cfg_s_sf_alloc2_c__names,
    "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_",
    offsetof(eutra_mbsfn_sf_cfg_s::sf_alloc2_c_, c.type),
    offsetof(eutra_mbsfn_sf_cfg_s::sf_alloc2_c_, c.value),
    false,
    &eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::storage_desc};

static const char*           enum_names_eutra_mbsfn_sf_cfg_s_sf_alloc2_c__types_values[] = {"oneFrame", "fourFrames"};
const asn1::table::enum_desc eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types_opts::desc        = {
    enum_names_eutra_mbsfn_sf_cfg_s_sf_alloc2_c__types_values,
    2,
    "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types"};
uint8_t eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 4};
  return map_enum_number(numbers, 2, value, "eutra_mbsfn_sf_cfg_s::sf_alloc2_c_::types");
}

// RateMatchPatternLTE-CRS ::= SEQUENCE
static const asn1::table::seq_field seq_table_rate_match_pattern_lte_crs_s_fields[] = {
    {offsetof(rate_match_pattern_lte_crs_s, carrier_freq_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(16383),
     nullptr},
    {offsetof(rate_match_pattern_lte_crs_s, carrier_bw_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rate_match_pattern_lte_crs_s::carrier_bw_dl)>(),
     decltype(rate_match_pattern_lte_crs_s::carrier_bw_dl)::has_ext,
     false,
     decltype(rate_match_pattern_lte_crs_s::carrier_bw_dl)::nof_types,
     decltype(rate_match_pattern_lte_crs_s::carrier_bw_dl)::nof_exts,
     nullptr,
     &rate_match_pattern_lte_crs_s::carrier_bw_dl_opts::desc},
    {offsetof(rate_match_pattern_lte_crs_s, mbsfn_sf_cfg_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(rate_match_pattern_lte_crs_s::mbsfn_sf_cfg_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &eutra_mbsfn_sf_cfg_s::desc},
    {offsetof(rate_match_pattern_lte_crs_s, nrof_crs_ports),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rate_match_pattern_lte_crs_s::nrof_crs_ports)>(),
     decltype(rate_match_pattern_lte_crs_s::nrof_crs_ports)::has_ext,
     false,
     decltype(rate_match_pattern_lte_crs_s::nrof_crs_ports)::nof_types,
     decltype(rate_match_pattern_lte_crs_s::nrof_crs_ports)::nof_exts,
     nullptr,
     &rate_match_pattern_lte_crs_s::nrof_crs_ports_opts::desc},
    {offsetof(rate_match_pattern_lte_crs_s, v_shift),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rate_match_pattern_lte_crs_s::v_shift)>(),
     decltype(rate_match_pattern_lte_crs_s::v_shift)::has_ext,
     false,
     decltype(rate_match_pattern_lte_crs_s::v_shift)::nof_types,
     decltype(rate_match_pattern_lte_crs_s::v_shift)::nof_exts,
     nullptr,
     &rate_match_pattern_lte_crs_s::v_shift_opts::desc}};
static const asn1::table::field_name seq_table_rate_match_pattern_lte_crs_s_names[] = {
    {"carrierFreqDL", nullptr},
    {"carrierBandwidthDL", nullptr},
    {"mbsfn-SubframeConfigList", nullptr},
    {"nrofCRS-Ports", nullptr},
    {"v-Shift", nullptr}};
static const asn1::table::seq_opt seq_table_rate_match_pattern_lte_crs_s_opts[] = {
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc rate_match_pattern_lte_crs_s::desc = {
    {seq_table_rate_match_pattern_lte_crs_s_fields, 5, UINT64_C(0x1b), seq_table_rate_match_pattern_lte_crs_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_rate_match_pattern_lte_crs_s_names};

static const char* enum_names_rate_match_pattern_lte_crs_s_carrier_bw_dl_e__values[] =
    {"n6", "n15", "n25", "n50", "n75", "n100", "spare2", "spare1"};
const asn1::table::enum_desc rate_match_pattern_lte_crs_s::carrier_bw_dl_opts::desc = {
    enum_names_rate_match_pattern_lte_crs_s_carrier_bw_dl_e__values,
    8,
    "rate_match_pattern_lte_crs_s::carrier_bw_dl_e_"};
uint8_t rate_match_pattern_lte_crs_s::carrier_bw_dl_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(numbers, 6, value, "rate_match_pattern_lte_crs_s::carrier_bw_dl_e_");
}

static const char*           enum_names_rate_match_pattern_lte_crs_s_nrof_crs_ports_e__values[] = {"n1", "n2", "n4"};
const asn1::table::enum_desc rate_match_pattern_lte_crs_s::nrof_crs_ports_opts::desc            = {
    enum_names_rate_match_pattern_lte_crs_s_nrof_crs_ports_e__values,
    3,
    "rate_match_pattern_lte_crs_s::nrof_crs_ports_e_"};
uint8_t rate_match_pattern_lte_crs_s::nrof_crs_ports_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "rate_match_pattern_lte_crs_s::nrof_crs_ports_e_");
}

static const char* enum_names_rate_match_pattern_lte_crs_s_v_shift_e__values[] = {"n0", "n1", "n2", "n3", "n4", "n5"};
const asn1::table::enum_desc rate_match_pattern_lte_crs_s::v_shift_opts::desc  = {
    enum_names_rate_match_pattern_lte_crs_s_v_shift_e__values,
    6,
    "rate_match_pattern_lte_crs_s::v_shift_e_"};
uint8_t rate_match_pattern_lte_crs_s::v_shift_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5};
  return map_enum_number(numbers, 6, value, "rate_match_pattern_lte_crs_s::v_shift_e_");
}

// FrequencyInfoDL-SIB ::= SEQUENCE
static const asn1::table::seq_field seq_table_freq_info_dl_sib_s_fields[] = {
    {offsetof(freq_info_dl_sib_s, freq_band_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(freq_info_dl_sib_s::freq_band_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_s::desc},
    {offsetof(freq_info_dl_sib_s, offset_to_point_a),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(2199),
     nullptr},
    {offsetof(freq_info_dl_sib_s, scs_specific_carrier_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     5,
     &asn1::table::item_list_codec<decltype(freq_info_dl_sib_s::scs_specific_carrier_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &scs_specific_carrier_s::desc}};
static const asn1::table::field_name seq_table_freq_info_dl_sib_s_names[] = {{"frequencyBandList", nullptr},
                                                                             {"offsetToPointA", nullptr},
                                                                             {"scs-SpecificCarrierList", nullptr}};
const asn1::table::seq_desc          freq_info_dl_sib_s::desc             = {
    {seq_table_freq_info_dl_sib_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_freq_info_dl_sib_s_names};

// PagingCycle ::= ENUMERATED
static const char*           enum_names_paging_cycle_e_values[] = {"rf32", "rf64", "rf128", "rf256"};
const asn1::table::enum_desc paging_cycle_opts::desc = {enum_names_paging_cycle_e_values, 4, "paging_cycle_e"};
uint16_t                     paging_cycle_opts::to_number() const
{
  static const uint16_t numbers[] = {32, 64, 128, 256};
  return map_enum_number(numbers, 4, value, "paging_cycle_e");
}

// PCCH-Config ::= SEQUENCE
static const asn1::table::seq_field seq_table_pcch_cfg_s_fields[] = {
    {offsetof(pcch_cfg_s, default_paging_cycle),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pcch_cfg_s::default_paging_cycle)>(),
     decltype(pcch_cfg_s::default_paging_cycle)::has_ext,
     false,
     decltype(pcch_cfg_s::default_paging_cycle)::nof_types,
     decltype(pcch_cfg_s::default_paging_cycle)::nof_exts,
     nullptr,
     &paging_cycle_opts::desc},
    {offsetof(pcch_cfg_s, nand_paging_frame_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &pcch_cfg_s::nand_paging_frame_offset_c_::desc},
    {offsetof(pcch_cfg_s, ns),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pcch_cfg_s::ns)>(),
     decltype(pcch_cfg_s::ns)::has_ext,
     false,
     decltype(pcch_cfg_s::ns)::nof_types,
     decltype(pcch_cfg_s::ns)::nof_exts,
     nullptr,
     &pcch_cfg_s::ns_opts::desc},
    {offsetof(pcch_cfg_s, first_pdcch_monitoring_occasion_of_po),
     (int32_t)offsetof(pcch_cfg_s, first_pdcch_monitoring_occasion_of_po_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::desc}};
static const asn1::table::field_name seq_table_pcch_cfg_s_names[] = {{"defaultPagingCycle", nullptr},
                                                                     {"nAndPagingFrameOffset", nullptr},
                                                                     {"ns", nullptr},
                                                                     {"firstPDCCH-MonitoringOccasionOfPO", nullptr}};
static const asn1::table::seq_opt    seq_table_pcch_cfg_s_opts[]  = {
    {3, (int32_t)offsetof(pcch_cfg_s, first_pdcch_monitoring_occasion_of_po_present)}};
static const asn1::table::seq_field seq_table_pcch_cfg_s_g0_fields[] = {
    {offsetof(pcch_cfg_s, nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16),
     (int32_t)offsetof(pcch_cfg_s, nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(2),
     UINT64_C(4),
     nullptr}};
static const asn1::table::field_name seq_table_pcch_cfg_s_g0_names[] = {
    {"nrofPDCCH-MonitoringOccasionPerSSB-InPO-r16", nullptr}};
static const asn1::table::seq_opt seq_table_pcch_cfg_s_g0_opts[] = {
    {0, (int32_t)offsetof(pcch_cfg_s, nrof_pdcch_monitoring_occasion_per_ssb_in_po_r16_present)}};
static const asn1::table::seq_field seq_table_pcch_cfg_s_g1_fields[] = {
    {0,
     (int32_t)offsetof(pcch_cfg_s, ran_paging_in_idle_po_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pcch_cfg_s, first_pdcch_monitoring_occasion_of_po_v1710),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710)>::value,
     &pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::desc}};
static const asn1::table::field_name seq_table_pcch_cfg_s_g1_names[] = {
    {"ranPagingInIdlePO-r17", "true"},
    {"firstPDCCH-MonitoringOccasionOfPO-v1710", nullptr}};
static const asn1::table::seq_opt seq_table_pcch_cfg_s_g1_opts[] = {
    {0, (int32_t)offsetof(pcch_cfg_s, ran_paging_in_idle_po_r17_present)},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_pcch_cfg_s_groups[] = {
    {{seq_table_pcch_cfg_s_g0_fields, 1, UINT64_C(0x0), seq_table_pcch_cfg_s_g0_opts, 1},
     false,
     seq_table_pcch_cfg_s_g0_names},
    {{seq_table_pcch_cfg_s_g1_fields, 2, UINT64_C(0x0), seq_table_pcch_cfg_s_g1_opts, 2},
     false,
     seq_table_pcch_cfg_s_g1_names}};
const asn1::table::seq_desc pcch_cfg_s::desc = {
    {seq_table_pcch_cfg_s_fields, 4, UINT64_C(0x7), seq_table_pcch_cfg_s_opts, 1},
    true,
    false,
    seq_table_pcch_cfg_s_groups,
    2,
    seq_table_pcch_cfg_s_names};

static const asn1::table::storage_ops* const choice_storage_pcch_cfg_s_nand_paging_frame_offset_c__alternatives[] = {
    nullptr,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc pcch_cfg_s::nand_paging_frame_offset_c_::storage_desc = {
    choice_storage_pcch_cfg_s_nand_paging_frame_offset_c__alternatives,
    5,
    "pcch_cfg_s::nand_paging_frame_offset_c_"};

void pcch_cfg_s::nand_paging_frame_offset_c_::set_one_t()
{
  set(types::one_t);
}
uint8_t& pcch_cfg_s::nand_paging_frame_offset_c_::set_half_t()
{
  set(types::half_t);
  return c.get<uint8_t>();
}
uint8_t& pcch_cfg_s::nand_paging_frame_offset_c_::set_quarter_t()
{
  set(types::quarter_t);
  return c.get<uint8_t>();
}
uint8_t& pcch_cfg_s::nand_paging_frame_offset_c_::set_one_eighth_t()
{
  set(types::one_eighth_t);
  return c.get<uint8_t>();
}
uint8_t& pcch_cfg_s::nand_paging_frame_offset_c_::set_one_sixteenth_t()
{
  set(types::one_sixteenth_t);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field choice_table_pcch_cfg_s_nand_paging_frame_offset_c__alternatives[] = {
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
     UINT64_C(7),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(15),
     nullptr}};
static const asn1::table::field_name choice_table_pcch_cfg_s_nand_paging_frame_offset_c__names[] =
    {{"oneT", nullptr}, {"halfT", nullptr}, {"quarterT", nullptr}, {"oneEighthT", nullptr}, {"oneSixteenthT", nullptr}};
const asn1::table::choice_desc pcch_cfg_s::nand_paging_frame_offset_c_::desc = {
    asn1::table::enum_int_type<pcch_cfg_s::nand_paging_frame_offset_c_::types>(),
    pcch_cfg_s::nand_paging_frame_offset_c_::types::nof_types,
    pcch_cfg_s::nand_paging_frame_offset_c_::types::nof_exts,
    pcch_cfg_s::nand_paging_frame_offset_c_::types::has_ext,
    false,
    choice_table_pcch_cfg_s_nand_paging_frame_offset_c__alternatives,
    nullptr,
    choice_table_pcch_cfg_s_nand_paging_frame_offset_c__names,
    "pcch_cfg_s::nand_paging_frame_offset_c_",
    offsetof(pcch_cfg_s::nand_paging_frame_offset_c_, c.type),
    offsetof(pcch_cfg_s::nand_paging_frame_offset_c_, c.value),
    false,
    &pcch_cfg_s::nand_paging_frame_offset_c_::storage_desc};

static const char*           enum_names_pcch_cfg_s_nand_paging_frame_offset_c__types_values[] = {"oneT",
                                                                                                 "halfT",
                                                                                                 "quarterT",
                                                                                                 "oneEighthT",
                                                                                                 "oneSixteenthT"};
const asn1::table::enum_desc pcch_cfg_s::nand_paging_frame_offset_c_::types_opts::desc        = {
    enum_names_pcch_cfg_s_nand_paging_frame_offset_c__types_values,
    5,
    "pcch_cfg_s::nand_paging_frame_offset_c_::types"};
float pcch_cfg_s::nand_paging_frame_offset_c_::types_opts::to_number() const
{
  static const float numbers[] = {1.0, 0.5, 0.25, 0.125, 0.0625};
  return map_enum_number(numbers, 5, value, "pcch_cfg_s::nand_paging_frame_offset_c_::types");
}
const char* pcch_cfg_s::nand_paging_frame_offset_c_::types_opts::to_number_string() const
{
  static const char* number_strs[] = {"1", "0.5", "0.25", "1/8", "1/16"};
  return convert_enum_idx(number_strs, 5, value, "pcch_cfg_s::nand_paging_frame_offset_c_::types");
}

static const char*           enum_names_pcch_cfg_s_ns_e__values[] = {"four", "two", "one"};
const asn1::table::enum_desc pcch_cfg_s::ns_opts::desc = {enum_names_pcch_cfg_s_ns_e__values, 3, "pcch_cfg_s::ns_e_"};
uint8_t                      pcch_cfg_s::ns_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 2, 1};
  return map_enum_number(numbers, 3, value, "pcch_cfg_s::ns_e_");
}

static const asn1::table::storage_ops* const
    choice_storage_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_c__alternatives[] = {
        &asn1::table::alternative_ops<pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs15_kh_zone_t_l_,
                                      false>::value,
        &asn1::table::alternative_ops<
            pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs30_kh_zone_t_scs15_kh_zhalf_t_l_,
            false>::value,
        &asn1::table::alternative_ops<pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
                                          scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_,
                                      false>::value,
        &asn1::table::alternative_ops<
            pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
                scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_,
            false>::value,
        &asn1::table::alternative_ops<
            pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
                scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_,
            false>::value,
        &asn1::table::alternative_ops<
            pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
                scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_,
            false>::value,
        &asn1::table::alternative_ops<pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
                                          scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_,
                                      false>::value,
        &asn1::table::alternative_ops<
            pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_,
            false>::value};
const asn1::table::choice_storage_desc pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::storage_desc = {
    choice_storage_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_c__alternatives,
    8,
    "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_"};

pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs15_kh_zone_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs15_kh_zone_t()
{
  set(types::scs15_kh_zone_t);
  return c.get<scs15_kh_zone_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs30_kh_zone_t_scs15_kh_zhalf_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs30_kh_zone_t_scs15_kh_zhalf_t()
{
  set(types::scs30_kh_zone_t_scs15_kh_zhalf_t);
  return c.get<scs30_kh_zone_t_scs15_kh_zhalf_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t()
{
  set(types::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t);
  return c.get<scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
    scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_&
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t()
{
  set(types::scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t);
  return c.get<scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
    scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_&
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t()
{
  set(types::scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t);
  return c.get<scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
    scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_&
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t()
{
  set(types::scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t);
  return c.get<scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
    scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_&
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
        set_scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t()
{
  set(types::scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t);
  return c.get<scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::set_scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t()
{
  set(types::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t);
  return c.get<scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_>();
}
static const asn1::table::seq_field choice_table_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs15_kh_zone_t_l_,
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(139), false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs30_kh_zone_t_scs15_kh_zhalf_t_l_,
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(279), false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<
         pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs60_kh_zone_t_scs30_kh_zhalf_t_scs15_kh_zquarter_t_l_,
         asn1::table::int_json,
         asn1::table::int_items<false, INT64_C(0), UINT64_C(559), false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
                                  scs120_kh_zone_t_scs60_kh_zhalf_t_scs30_kh_zquarter_t_scs15_kh_zone_eighth_t_l_,
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(1119), false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<
         pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
             scs120_kh_zhalf_t_scs60_kh_zquarter_t_scs30_kh_zone_eighth_t_scs15_kh_zone_sixteenth_t_l_,
         asn1::table::int_json,
         asn1::table::int_items<false, INT64_C(0), UINT64_C(2239), false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<
         pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
             scs480_kh_zone_t_scs120_kh_zquarter_t_scs60_kh_zone_eighth_t_scs30_kh_zone_sixteenth_t_l_,
         asn1::table::int_json,
         asn1::table::int_items<false, INT64_C(0), UINT64_C(4479), false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::
                                  scs480_kh_zhalf_t_scs120_kh_zone_eighth_t_scs60_kh_zone_sixteenth_t_l_,
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(8959), false, false>>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<
         pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::scs480_kh_zquarter_t_scs120_kh_zone_sixteenth_t_l_,
         asn1::table::int_json,
         asn1::table::int_items<false, INT64_C(0), UINT64_C(17919), false, false>>::value}};
static const asn1::table::field_name choice_table_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_c__names[] = {
    {"sCS15KHZoneT", nullptr},
    {"sCS30KHZoneT-SCS15KHZhalfT", nullptr},
    {"sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT", nullptr},
    {"sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT", nullptr},
    {"sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT", nullptr},
    {"sCS480KHZoneT-SCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT", nullptr},
    {"sCS480KHZhalfT-SCS120KHZoneEighthT-SCS60KHZoneSixteenthT", nullptr},
    {"sCS480KHZquarterT-SCS120KHZoneSixteenthT", nullptr}};
const asn1::table::choice_desc pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::desc = {
    asn1::table::enum_int_type<pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::types>(),
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::types::nof_types,
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::types::nof_exts,
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::types::has_ext,
    false,
    choice_table_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_c__alternatives,
    nullptr,
    choice_table_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_c__names,
    "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_",
    offsetof(pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_, c.type),
    offsetof(pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_, c.value),
    false,
    &pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::storage_desc};

static const char* enum_names_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_c__types_values[] = {
    "sCS15KHZoneT",
    "sCS30KHZoneT-SCS15KHZhalfT",
    "sCS60KHZoneT-SCS30KHZhalfT-SCS15KHZquarterT",
    "sCS120KHZoneT-SCS60KHZhalfT-SCS30KHZquarterT-SCS15KHZoneEighthT",
    "sCS120KHZhalfT-SCS60KHZquarterT-SCS30KHZoneEighthT-SCS15KHZoneSixteenthT",
    "sCS480KHZoneT-SCS120KHZquarterT-SCS60KHZoneEighthT-SCS30KHZoneSixteenthT",
    "sCS480KHZhalfT-SCS120KHZoneEighthT-SCS60KHZoneSixteenthT",
    "sCS480KHZquarterT-SCS120KHZoneSixteenthT"};
const asn1::table::enum_desc pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::types_opts::desc = {
    enum_names_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_c__types_values,
    8,
    "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_c_::types"};

static const asn1::table::storage_ops* const
    choice_storage_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_v1710_c__alternatives[] = {
        &asn1::table::alternative_ops<
            pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::scs480_kh_zone_eighth_t_l_,
            false>::value,
        &asn1::table::alternative_ops<
            pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::scs480_kh_zone_sixteenth_t_l_,
            false>::value};
const asn1::table::choice_storage_desc pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::storage_desc = {
    choice_storage_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_v1710_c__alternatives,
    2,
    "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_"};

pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::scs480_kh_zone_eighth_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::set_scs480_kh_zone_eighth_t()
{
  set(types::scs480_kh_zone_eighth_t);
  return c.get<scs480_kh_zone_eighth_t_l_>();
}
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::scs480_kh_zone_sixteenth_t_l_&
pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::set_scs480_kh_zone_sixteenth_t()
{
  set(types::scs480_kh_zone_sixteenth_t);
  return c.get<scs480_kh_zone_sixteenth_t_l_>();
}
static const asn1::table::seq_field
    choice_table_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_v1710_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         4,
         &asn1::table::list_codec<
             pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::scs480_kh_zone_eighth_t_l_,
             asn1::table::int_json,
             asn1::table::int_items<false, INT64_C(0), UINT64_C(35839), false, false>>::value},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         4,
         &asn1::table::list_codec<
             pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::scs480_kh_zone_sixteenth_t_l_,
             asn1::table::int_json,
             asn1::table::int_items<false, INT64_C(0), UINT64_C(71679), false, false>>::value}};
static const asn1::table::field_name choice_table_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_v1710_c__names[] = {
    {"sCS480KHZoneEighthT", nullptr},
    {"sCS480KHZoneSixteenthT", nullptr}};
const asn1::table::choice_desc pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::desc = {
    asn1::table::enum_int_type<pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types>(),
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types::nof_types,
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types::nof_exts,
    pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types::has_ext,
    false,
    choice_table_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_v1710_c__alternatives,
    nullptr,
    choice_table_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_v1710_c__names,
    "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_",
    offsetof(pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_, c.type),
    offsetof(pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_, c.value),
    false,
    &pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::storage_desc};

static const char* enum_names_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_v1710_c__types_values[] = {
    "sCS480KHZoneEighthT",
    "sCS480KHZoneSixteenthT"};
const asn1::table::enum_desc pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types_opts::desc = {
    enum_names_pcch_cfg_s_first_pdcch_monitoring_occasion_of_po_v1710_c__types_values,
    2,
    "pcch_cfg_s::first_pdcch_monitoring_occasion_of_po_v1710_c_::types"};

// SubgroupConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_subgroup_cfg_r17_s_fields[] = {
    {offsetof(subgroup_cfg_r17_s, subgroups_num_per_po_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(subgroup_cfg_r17_s, subgroups_num_for_ue_id_r17),
     (int32_t)offsetof(subgroup_cfg_r17_s, subgroups_num_for_ue_id_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr}};
static const asn1::table::field_name seq_table_subgroup_cfg_r17_s_names[] = {{"subgroupsNumPerPO-r17", nullptr},
                                                                             {"subgroupsNumForUEID-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_subgroup_cfg_r17_s_opts[]  = {
    {1, (int32_t)offsetof(subgroup_cfg_r17_s, subgroups_num_for_ue_id_r17_present)}};
const asn1::table::seq_desc subgroup_cfg_r17_s::desc = {
    {seq_table_subgroup_cfg_r17_s_fields, 2, UINT64_C(0x1), seq_table_subgroup_cfg_r17_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_subgroup_cfg_r17_s_names};

// PEI-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_pei_cfg_r17_s_fields[] = {
    {offsetof(pei_cfg_r17_s, po_num_per_pei_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pei_cfg_r17_s::po_num_per_pei_r17)>(),
     decltype(pei_cfg_r17_s::po_num_per_pei_r17)::has_ext,
     false,
     decltype(pei_cfg_r17_s::po_num_per_pei_r17)::nof_types,
     decltype(pei_cfg_r17_s::po_num_per_pei_r17)::nof_exts,
     nullptr,
     &pei_cfg_r17_s::po_num_per_pei_r17_opts::desc},
    {offsetof(pei_cfg_r17_s, payload_size_dci_2_7_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(43),
     nullptr},
    {offsetof(pei_cfg_r17_s, pei_frame_offset_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(16),
     nullptr},
    {offsetof(pei_cfg_r17_s, subgroup_cfg_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &subgroup_cfg_r17_s::desc},
    {0,
     (int32_t)offsetof(pei_cfg_r17_s, last_used_cell_only_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pei_cfg_r17_s_names[] = {{"po-NumPerPEI-r17", nullptr},
                                                                        {"payloadSizeDCI-2-7-r17", nullptr},
                                                                        {"pei-FrameOffset-r17", nullptr},
                                                                        {"subgroupConfig-r17", nullptr},
                                                                        {"lastUsedCellOnly-r17", "true"}};
static const asn1::table::seq_opt    seq_table_pei_cfg_r17_s_opts[]  = {
    {4, (int32_t)offsetof(pei_cfg_r17_s, last_used_cell_only_r17_present)}};
const asn1::table::seq_desc pei_cfg_r17_s::desc = {
    {seq_table_pei_cfg_r17_s_fields, 5, UINT64_C(0xf), seq_table_pei_cfg_r17_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_pei_cfg_r17_s_names};

static const char*           enum_names_pei_cfg_r17_s_po_num_per_pei_r17_e__values[] = {"po1", "po2", "po4", "po8"};
const asn1::table::enum_desc pei_cfg_r17_s::po_num_per_pei_r17_opts::desc            = {
    enum_names_pei_cfg_r17_s_po_num_per_pei_r17_e__values,
    4,
    "pei_cfg_r17_s::po_num_per_pei_r17_e_"};
uint8_t pei_cfg_r17_s::po_num_per_pei_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "pei_cfg_r17_s::po_num_per_pei_r17_e_");
}

// FrequencyInfoDL-SIB-v1800 ::= SEQUENCE
static const asn1::table::seq_field seq_table_freq_info_dl_sib_v1800_s_fields[] = {
    {offsetof(freq_info_dl_sib_v1800_s, freq_band_list_aerial_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(freq_info_dl_sib_v1800_s::freq_band_list_aerial_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_aerial_r18_s::desc}};
static const asn1::table::field_name seq_table_freq_info_dl_sib_v1800_s_names[] = {
    {"frequencyBandListAerial-r18", nullptr}};
const asn1::table::seq_desc freq_info_dl_sib_v1800_s::desc = {
    {seq_table_freq_info_dl_sib_v1800_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_freq_info_dl_sib_v1800_s_names};

// DownlinkConfigCommonSIB ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_cfg_common_sib_s_fields[] = {
    {offsetof(dl_cfg_common_sib_s, freq_info_dl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &freq_info_dl_sib_s::desc},
    {offsetof(dl_cfg_common_sib_s, init_dl_bwp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_dl_common_s::desc},
    {offsetof(dl_cfg_common_sib_s, bcch_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bcch_cfg_s::desc},
    {offsetof(dl_cfg_common_sib_s, pcch_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pcch_cfg_s::desc}};
static const asn1::table::field_name seq_table_dl_cfg_common_sib_s_names[]     = {{"frequencyInfoDL", nullptr},
                                                                                  {"initialDownlinkBWP", nullptr},
                                                                                  {"bcch-Config", nullptr},
                                                                                  {"pcch-Config", nullptr}};
static const asn1::table::seq_field  seq_table_dl_cfg_common_sib_s_g0_fields[] = {
    {offsetof(dl_cfg_common_sib_s, pei_cfg_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(dl_cfg_common_sib_s::pei_cfg_r17)>::value,
      &pei_cfg_r17_s::desc},
    {offsetof(dl_cfg_common_sib_s, init_dl_bwp_red_cap_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(dl_cfg_common_sib_s::init_dl_bwp_red_cap_r17)>::value,
      &bwp_dl_common_s::desc}};
static const asn1::table::field_name seq_table_dl_cfg_common_sib_s_g0_names[] = {
    {"pei-Config-r17", nullptr},
    {"initialDownlinkBWP-RedCap-r17", nullptr}};
static const asn1::table::seq_opt seq_table_dl_cfg_common_sib_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence},
                                                                             {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_dl_cfg_common_sib_s_g1_fields[] = {
    {offsetof(dl_cfg_common_sib_s, freq_info_dl_v1800),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(dl_cfg_common_sib_s::freq_info_dl_v1800)>::value,
     &freq_info_dl_sib_v1800_s::desc}};
static const asn1::table::field_name seq_table_dl_cfg_common_sib_s_g1_names[] = {{"frequencyInfoDL-v1800", nullptr}};
static const asn1::table::seq_opt seq_table_dl_cfg_common_sib_s_g1_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_dl_cfg_common_sib_s_groups[] = {
    {{seq_table_dl_cfg_common_sib_s_g0_fields, 2, UINT64_C(0x0), seq_table_dl_cfg_common_sib_s_g0_opts, 2},
     false,
     seq_table_dl_cfg_common_sib_s_g0_names},
    {{seq_table_dl_cfg_common_sib_s_g1_fields, 1, UINT64_C(0x0), seq_table_dl_cfg_common_sib_s_g1_opts, 1},
     false,
     seq_table_dl_cfg_common_sib_s_g1_names}};
const asn1::table::seq_desc dl_cfg_common_sib_s::desc = {
    {seq_table_dl_cfg_common_sib_s_fields, 4, UINT64_C(0xf), nullptr, 0},
    true,
    false,
    seq_table_dl_cfg_common_sib_s_groups,
    2,
    seq_table_dl_cfg_common_sib_s_names};

// FrequencyInfoUL-SIB ::= SEQUENCE
static const asn1::table::seq_field seq_table_freq_info_ul_sib_s_fields[] = {
    {offsetof(freq_info_ul_sib_s, freq_band_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(freq_info_ul_sib_s::freq_band_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_s::desc},
    {offsetof(freq_info_ul_sib_s, absolute_freq_point_a),
     (int32_t)offsetof(freq_info_ul_sib_s, absolute_freq_point_a_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {offsetof(freq_info_ul_sib_s, scs_specific_carrier_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     5,
     &asn1::table::item_list_codec<decltype(freq_info_ul_sib_s::scs_specific_carrier_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &scs_specific_carrier_s::desc},
    {offsetof(freq_info_ul_sib_s, p_max),
     (int32_t)offsetof(freq_info_ul_sib_s, p_max_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-30),
     UINT64_C(33),
     nullptr},
    {0,
     (int32_t)offsetof(freq_info_ul_sib_s, freq_shift7p5khz_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_freq_info_ul_sib_s_names[] = {{"frequencyBandList", nullptr},
                                                                             {"absoluteFrequencyPointA", nullptr},
                                                                             {"scs-SpecificCarrierList", nullptr},
                                                                             {"p-Max", nullptr},
                                                                             {"frequencyShift7p5khz", "true"}};
static const asn1::table::seq_opt    seq_table_freq_info_ul_sib_s_opts[]  = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(freq_info_ul_sib_s, absolute_freq_point_a_present)},
    {3, (int32_t)offsetof(freq_info_ul_sib_s, p_max_present)},
    {4, (int32_t)offsetof(freq_info_ul_sib_s, freq_shift7p5khz_present)}};
static const asn1::table::seq_field seq_table_freq_info_ul_sib_s_g0_fields[] = {
    {offsetof(freq_info_ul_sib_s, freq_band_list_aerial_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_item_list_codec<decltype(freq_info_ul_sib_s::freq_band_list_aerial_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &nr_multi_band_info_aerial_r18_s::desc}};
static const asn1::table::field_name seq_table_freq_info_ul_sib_s_g0_names[] = {
    {"frequencyBandListAerial-r18", nullptr}};
static const asn1::table::seq_opt seq_table_freq_info_ul_sib_s_g0_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_freq_info_ul_sib_s_groups[] = {
    {{seq_table_freq_info_ul_sib_s_g0_fields, 1, UINT64_C(0x0), seq_table_freq_info_ul_sib_s_g0_opts, 1},
     false,
     seq_table_freq_info_ul_sib_s_g0_names}};
const asn1::table::seq_desc freq_info_ul_sib_s::desc = {
    {seq_table_freq_info_ul_sib_s_fields, 5, UINT64_C(0x4), seq_table_freq_info_ul_sib_s_opts, 4},
    true,
    false,
    seq_table_freq_info_ul_sib_s_groups,
    1,
    seq_table_freq_info_ul_sib_s_names};

// TimeAlignmentTimer ::= ENUMERATED
static const char* enum_names_time_align_timer_e_values[] =
    {"ms500", "ms750", "ms1280", "ms1920", "ms2560", "ms5120", "ms10240", "infinity"};
const asn1::table::enum_desc time_align_timer_opts::desc = {enum_names_time_align_timer_e_values,
                                                            8,
                                                            "time_align_timer_e"};
int16_t                      time_align_timer_opts::to_number() const
{
  static const int16_t numbers[] = {500, 750, 1280, 1920, 2560, 5120, 10240, -1};
  return map_enum_number(numbers, 8, value, "time_align_timer_e");
}

// UplinkConfigCommonSIB ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_cfg_common_sib_s_fields[] = {
    {offsetof(ul_cfg_common_sib_s, freq_info_ul),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &freq_info_ul_sib_s::desc},
    {offsetof(ul_cfg_common_sib_s, init_ul_bwp),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_ul_common_s::desc},
    {offsetof(ul_cfg_common_sib_s, time_align_timer_common),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ul_cfg_common_sib_s::time_align_timer_common)>(),
     decltype(ul_cfg_common_sib_s::time_align_timer_common)::has_ext,
     false,
     decltype(ul_cfg_common_sib_s::time_align_timer_common)::nof_types,
     decltype(ul_cfg_common_sib_s::time_align_timer_common)::nof_exts,
     nullptr,
     &time_align_timer_opts::desc}};
static const asn1::table::field_name seq_table_ul_cfg_common_sib_s_names[] = {{"frequencyInfoUL", nullptr},
                                                                              {"initialUplinkBWP", nullptr},
                                                                              {"timeAlignmentTimerCommon", nullptr}};
const asn1::table::seq_desc          ul_cfg_common_sib_s::desc             = {
    {seq_table_ul_cfg_common_sib_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ul_cfg_common_sib_s_names};

// SemiStaticChannelAccessConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_semi_static_ch_access_cfg_r16_s_fields[] = {
    {offsetof(semi_static_ch_access_cfg_r16_s, period_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(semi_static_ch_access_cfg_r16_s::period_r16)>(),
     decltype(semi_static_ch_access_cfg_r16_s::period_r16)::has_ext,
     false,
     decltype(semi_static_ch_access_cfg_r16_s::period_r16)::nof_types,
     decltype(semi_static_ch_access_cfg_r16_s::period_r16)::nof_exts,
     nullptr,
     &semi_static_ch_access_cfg_r16_s::period_r16_opts::desc}};
static const asn1::table::field_name seq_table_semi_static_ch_access_cfg_r16_s_names[] = {{"period-r16", nullptr}};
const asn1::table::seq_desc          semi_static_ch_access_cfg_r16_s::desc             = {
    {seq_table_semi_static_ch_access_cfg_r16_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_semi_static_ch_access_cfg_r16_s_names};

static const char* enum_names_semi_static_ch_access_cfg_r16_s_period_r16_e__values[] =
    {"ms1", "ms2", "ms2dot5", "ms4", "ms5", "ms10"};
const asn1::table::enum_desc semi_static_ch_access_cfg_r16_s::period_r16_opts::desc = {
    enum_names_semi_static_ch_access_cfg_r16_s_period_r16_e__values,
    6,
    "semi_static_ch_access_cfg_r16_s::period_r16_e_"};
float semi_static_ch_access_cfg_r16_s::period_r16_opts::to_number() const
{
  static const float numbers[] = {1.0, 2.0, 2.5, 4.0, 5.0, 10.0};
  return map_enum_number(numbers, 6, value, "semi_static_ch_access_cfg_r16_s::period_r16_e_");
}
const char* semi_static_ch_access_cfg_r16_s::period_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"1", "2", "2.5", "4", "5", "10"};
  return convert_enum_idx(number_strs, 6, value, "semi_static_ch_access_cfg_r16_s::period_r16_e_");
}

// HighSpeedConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_high_speed_cfg_r16_s_fields[] = {
    {0,
     (int32_t)offsetof(high_speed_cfg_r16_s, high_speed_meas_flag_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(high_speed_cfg_r16_s, high_speed_demod_flag_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_high_speed_cfg_r16_s_names[] = {{"highSpeedMeasFlag-r16", "true"},
                                                                               {"highSpeedDemodFlag-r16", "true"}};
static const asn1::table::seq_opt    seq_table_high_speed_cfg_r16_s_opts[]  = {
    {0, (int32_t)offsetof(high_speed_cfg_r16_s, high_speed_meas_flag_r16_present)},
    {1, (int32_t)offsetof(high_speed_cfg_r16_s, high_speed_demod_flag_r16_present)}};
const asn1::table::seq_desc high_speed_cfg_r16_s::desc = {
    {seq_table_high_speed_cfg_r16_s_fields, 2, UINT64_C(0x0), seq_table_high_speed_cfg_r16_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_high_speed_cfg_r16_s_names};

// HighSpeedConfigFR2-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_high_speed_cfg_fr2_r17_s_fields[] = {
    {offsetof(high_speed_cfg_fr2_r17_s, high_speed_meas_flag_fr2_r17),
     (int32_t)offsetof(high_speed_cfg_fr2_r17_s, high_speed_meas_flag_fr2_r17_present),
     asn1::table::enum_op<decltype(high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17)>(),
     decltype(high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17)::has_ext,
     false,
     decltype(high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17)::nof_types,
     decltype(high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17)::nof_exts,
     nullptr,
     &high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17_opts::desc},
    {offsetof(high_speed_cfg_fr2_r17_s, high_speed_deployment_type_fr2_r17),
     (int32_t)offsetof(high_speed_cfg_fr2_r17_s, high_speed_deployment_type_fr2_r17_present),
     asn1::table::enum_op<decltype(high_speed_cfg_fr2_r17_s::high_speed_deployment_type_fr2_r17)>(),
     decltype(high_speed_cfg_fr2_r17_s::high_speed_deployment_type_fr2_r17)::has_ext,
     false,
     decltype(high_speed_cfg_fr2_r17_s::high_speed_deployment_type_fr2_r17)::nof_types,
     decltype(high_speed_cfg_fr2_r17_s::high_speed_deployment_type_fr2_r17)::nof_exts,
     nullptr,
     &high_speed_cfg_fr2_r17_s::high_speed_deployment_type_fr2_r17_opts::desc},
    {0,
     (int32_t)offsetof(high_speed_cfg_fr2_r17_s, high_speed_large_one_step_ul_timing_fr2_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_high_speed_cfg_fr2_r17_s_names[] = {
    {"highSpeedMeasFlagFR2-r17", nullptr},
    {"highSpeedDeploymentTypeFR2-r17", nullptr},
    {"highSpeedLargeOneStepUL-TimingFR2-r17", "true"}};
static const asn1::table::seq_opt seq_table_high_speed_cfg_fr2_r17_s_opts[] = {
    {0, (int32_t)offsetof(high_speed_cfg_fr2_r17_s, high_speed_meas_flag_fr2_r17_present)},
    {1, (int32_t)offsetof(high_speed_cfg_fr2_r17_s, high_speed_deployment_type_fr2_r17_present)},
    {2, (int32_t)offsetof(high_speed_cfg_fr2_r17_s, high_speed_large_one_step_ul_timing_fr2_r17_present)}};
const asn1::table::seq_desc high_speed_cfg_fr2_r17_s::desc = {
    {seq_table_high_speed_cfg_fr2_r17_s_fields, 3, UINT64_C(0x0), seq_table_high_speed_cfg_fr2_r17_s_opts, 3},
    true,
    false,
    nullptr,
    0,
    seq_table_high_speed_cfg_fr2_r17_s_names};

static const char* enum_names_high_speed_cfg_fr2_r17_s_high_speed_meas_flag_fr2_r17_e__values[] = {"set1", "set2"};
const asn1::table::enum_desc high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17_opts::desc  = {
    enum_names_high_speed_cfg_fr2_r17_s_high_speed_meas_flag_fr2_r17_e__values,
    2,
    "high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17_e_"};
uint8_t high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "high_speed_cfg_fr2_r17_s::high_speed_meas_flag_fr2_r17_e_");
}

static const char* enum_names_high_speed_cfg_fr2_r17_s_high_speed_deployment_type_fr2_r17_e__values[] = {
    "unidirectional",
    "bidirectional"};
const asn1::table::enum_desc high_speed_cfg_fr2_r17_s::high_speed_deployment_type_fr2_r17_opts::desc = {
    enum_names_high_speed_cfg_fr2_r17_s_high_speed_deployment_type_fr2_r17_e__values,
    2,
    "high_speed_cfg_fr2_r17_s::high_speed_deployment_type_fr2_r17_e_"};

// UplinkConfigCommonSIB-v1700 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_cfg_common_sib_v1700_s_fields[] = {
    {offsetof(ul_cfg_common_sib_v1700_s, init_ul_bwp_red_cap_r17),
     (int32_t)offsetof(ul_cfg_common_sib_v1700_s, init_ul_bwp_red_cap_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_ul_common_s::desc}};
static const asn1::table::field_name seq_table_ul_cfg_common_sib_v1700_s_names[] = {
    {"initialUplinkBWP-RedCap-r17", nullptr}};
static const asn1::table::seq_opt seq_table_ul_cfg_common_sib_v1700_s_opts[] = {
    {0, (int32_t)offsetof(ul_cfg_common_sib_v1700_s, init_ul_bwp_red_cap_r17_present)}};
const asn1::table::seq_desc ul_cfg_common_sib_v1700_s::desc = {
    {seq_table_ul_cfg_common_sib_v1700_s_fields, 1, UINT64_C(0x0), seq_table_ul_cfg_common_sib_v1700_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_ul_cfg_common_sib_v1700_s_names};

// FrequencyInfoDL-SIB-v1760 ::= SEQUENCE
static const asn1::table::seq_field seq_table_freq_info_dl_sib_v1760_s_fields[] = {
    {offsetof(freq_info_dl_sib_v1760_s, freq_band_list_v1760),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(freq_info_dl_sib_v1760_s::freq_band_list_v1760),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_v1760_s::desc}};
static const asn1::table::field_name seq_table_freq_info_dl_sib_v1760_s_names[] = {
    {"frequencyBandList-v1760", nullptr}};
const asn1::table::seq_desc freq_info_dl_sib_v1760_s::desc = {
    {seq_table_freq_info_dl_sib_v1760_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_freq_info_dl_sib_v1760_s_names};

// DownlinkConfigCommonSIB-v1760 ::= SEQUENCE
static const asn1::table::seq_field seq_table_dl_cfg_common_sib_v1760_s_fields[] = {
    {offsetof(dl_cfg_common_sib_v1760_s, freq_info_dl_v1760),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &freq_info_dl_sib_v1760_s::desc}};
static const asn1::table::field_name seq_table_dl_cfg_common_sib_v1760_s_names[] = {{"frequencyInfoDL-v1760", nullptr}};
const asn1::table::seq_desc          dl_cfg_common_sib_v1760_s::desc             = {
    {seq_table_dl_cfg_common_sib_v1760_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_dl_cfg_common_sib_v1760_s_names};

// FrequencyInfoUL-SIB-v1760 ::= SEQUENCE
static const asn1::table::seq_field seq_table_freq_info_ul_sib_v1760_s_fields[] = {
    {offsetof(freq_info_ul_sib_v1760_s, freq_band_list_v1760),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(freq_info_ul_sib_v1760_s::freq_band_list_v1760),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_multi_band_info_v1760_s::desc}};
static const asn1::table::field_name seq_table_freq_info_ul_sib_v1760_s_names[] = {
    {"frequencyBandList-v1760", nullptr}};
const asn1::table::seq_desc freq_info_ul_sib_v1760_s::desc = {
    {seq_table_freq_info_ul_sib_v1760_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_freq_info_ul_sib_v1760_s_names};

// UplinkConfigCommonSIB-v1760 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_cfg_common_sib_v1760_s_fields[] = {
    {offsetof(ul_cfg_common_sib_v1760_s, freq_info_ul_v1760),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &freq_info_ul_sib_v1760_s::desc}};
static const asn1::table::field_name seq_table_ul_cfg_common_sib_v1760_s_names[] = {{"frequencyInfoUL-v1760", nullptr}};
const asn1::table::seq_desc          ul_cfg_common_sib_v1760_s::desc             = {
    {seq_table_ul_cfg_common_sib_v1760_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ul_cfg_common_sib_v1760_s_names};

// ServingCellConfigCommonSIB ::= SEQUENCE
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_sib_s_ssb_positions_in_burst_s__fields[] = {
    {offsetof(serving_cell_cfg_common_sib_s::ssb_positions_in_burst_s_, in_one_group),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(serving_cell_cfg_common_sib_s::ssb_positions_in_burst_s_::in_one_group),
                                asn1::table::struct_json>::value},
    {offsetof(serving_cell_cfg_common_sib_s::ssb_positions_in_burst_s_, group_presence),
     (int32_t)offsetof(serving_cell_cfg_common_sib_s::ssb_positions_in_burst_s_, group_presence_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(serving_cell_cfg_common_sib_s::ssb_positions_in_burst_s_::group_presence),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_sib_s_ssb_positions_in_burst_s__names[] = {
    {"inOneGroup", nullptr},
    {"groupPresence", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_sib_s_ssb_positions_in_burst_s__opts[] = {
    {1, (int32_t)offsetof(serving_cell_cfg_common_sib_s::ssb_positions_in_burst_s_, group_presence_present)}};
static const asn1::table::seq_desc seq_table_serving_cell_cfg_common_sib_s_ssb_positions_in_burst_s_ = {
    {seq_table_serving_cell_cfg_common_sib_s_ssb_positions_in_burst_s__fields,
     2,
     UINT64_C(0x1),
     seq_table_serving_cell_cfg_common_sib_s_ssb_positions_in_burst_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_serving_cell_cfg_common_sib_s_ssb_positions_in_burst_s__names};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_sib_s_fields[] = {
    {offsetof(serving_cell_cfg_common_sib_s, dl_cfg_common),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_cfg_common_sib_s::desc},
    {offsetof(serving_cell_cfg_common_sib_s, ul_cfg_common),
     (int32_t)offsetof(serving_cell_cfg_common_sib_s, ul_cfg_common_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_cfg_common_sib_s::desc},
    {offsetof(serving_cell_cfg_common_sib_s, supplementary_ul),
     (int32_t)offsetof(serving_cell_cfg_common_sib_s, supplementary_ul_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_cfg_common_sib_s::desc},
    {offsetof(serving_cell_cfg_common_sib_s, n_timing_advance_offset),
     (int32_t)offsetof(serving_cell_cfg_common_sib_s, n_timing_advance_offset_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_common_sib_s::n_timing_advance_offset)>(),
     decltype(serving_cell_cfg_common_sib_s::n_timing_advance_offset)::has_ext,
     false,
     decltype(serving_cell_cfg_common_sib_s::n_timing_advance_offset)::nof_types,
     decltype(serving_cell_cfg_common_sib_s::n_timing_advance_offset)::nof_exts,
     nullptr,
     &serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::desc},
    {offsetof(serving_cell_cfg_common_sib_s, ssb_positions_in_burst),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_serving_cell_cfg_common_sib_s_ssb_positions_in_burst_s_},
    {offsetof(serving_cell_cfg_common_sib_s, ssb_periodicity_serving_cell),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell)>(),
     decltype(serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell)::has_ext,
     false,
     decltype(serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell)::nof_types,
     decltype(serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell)::nof_exts,
     nullptr,
     &serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_opts::desc},
    {offsetof(serving_cell_cfg_common_sib_s, tdd_ul_dl_cfg_common),
     (int32_t)offsetof(serving_cell_cfg_common_sib_s, tdd_ul_dl_cfg_common_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tdd_ul_dl_cfg_common_s::desc},
    {offsetof(serving_cell_cfg_common_sib_s, ss_pbch_block_pwr),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-60),
     UINT64_C(50),
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_sib_s_names[] = {
    {"downlinkConfigCommon", nullptr},
    {"uplinkConfigCommon", nullptr},
    {"supplementaryUplink", nullptr},
    {"n-TimingAdvanceOffset", nullptr},
    {"ssb-PositionsInBurst", nullptr},
    {"ssb-PeriodicityServingCell", nullptr},
    {"tdd-UL-DL-ConfigurationCommon", nullptr},
    {"ss-PBCH-BlockPower", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_sib_s_opts[] = {
    {1, (int32_t)offsetof(serving_cell_cfg_common_sib_s, ul_cfg_common_present)},
    {2, (int32_t)offsetof(serving_cell_cfg_common_sib_s, supplementary_ul_present)},
    {3, (int32_t)offsetof(serving_cell_cfg_common_sib_s, n_timing_advance_offset_present)},
    {6, (int32_t)offsetof(serving_cell_cfg_common_sib_s, tdd_ul_dl_cfg_common_present)}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_sib_s_g0_fields[] = {
    {offsetof(serving_cell_cfg_common_sib_s, ch_access_mode_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_sib_s::ch_access_mode_r16)>::value,
     &serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::desc},
    {offsetof(serving_cell_cfg_common_sib_s, discovery_burst_win_len_r16),
     (int32_t)offsetof(serving_cell_cfg_common_sib_s, discovery_burst_win_len_r16_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16)>(),
     decltype(serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16)::has_ext,
     false,
     decltype(serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16)::nof_types,
     decltype(serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16)::nof_exts,
     nullptr,
     &serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_opts::desc},
    {offsetof(serving_cell_cfg_common_sib_s, high_speed_cfg_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_sib_s::high_speed_cfg_r16)>::value,
     &high_speed_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_sib_s_g0_names[] = {
    {"channelAccessMode-r16", nullptr},
    {"discoveryBurstWindowLength-r16", nullptr},
    {"highSpeedConfig-r16", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_sib_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(serving_cell_cfg_common_sib_s, discovery_burst_win_len_r16_present)},
    {2, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_sib_s_g1_fields[] = {
    {0,
     (int32_t)offsetof(serving_cell_cfg_common_sib_s, ch_access_mode2_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(serving_cell_cfg_common_sib_s, discovery_burst_win_len_v1700),
     (int32_t)offsetof(serving_cell_cfg_common_sib_s, discovery_burst_win_len_v1700_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700)>(),
     decltype(serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700)::has_ext,
     false,
     decltype(serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700)::nof_types,
     decltype(serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700)::nof_exts,
     nullptr,
     &serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_opts::desc},
    {offsetof(serving_cell_cfg_common_sib_s, high_speed_cfg_fr2_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_sib_s::high_speed_cfg_fr2_r17)>::value,
     &high_speed_cfg_fr2_r17_s::desc},
    {offsetof(serving_cell_cfg_common_sib_s, ul_cfg_common_v1700),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_sib_s::ul_cfg_common_v1700)>::value,
     &ul_cfg_common_sib_v1700_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_sib_s_g1_names[] = {
    {"channelAccessMode2-r17", "enabled"},
    {"discoveryBurstWindowLength-v1700", nullptr},
    {"highSpeedConfigFR2-r17", nullptr},
    {"uplinkConfigCommon-v1700", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_sib_s_g1_opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_common_sib_s, ch_access_mode2_r17_present)},
    {1, (int32_t)offsetof(serving_cell_cfg_common_sib_s, discovery_burst_win_len_v1700_present)},
    {2, asn1::table::seq_field::ptr_presence},
    {3, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_sib_s_g2_fields[] = {
    {0,
     (int32_t)offsetof(serving_cell_cfg_common_sib_s, enhanced_meas_ngso_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_sib_s_g2_names[] = {
    {"enhancedMeasurementNGSO-r17", "true"}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_sib_s_g2_opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_common_sib_s, enhanced_meas_ngso_r17_present)}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_sib_s_g3_fields[] = {
    {0,
     (int32_t)offsetof(serving_cell_cfg_common_sib_s, ra_ch_access_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_sib_s_g3_names[] = {
    {"ra-ChannelAccess-r17", "enabled"}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_sib_s_g3_opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_common_sib_s, ra_ch_access_r17_present)}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_sib_s_g4_fields[] = {
    {offsetof(serving_cell_cfg_common_sib_s, dl_cfg_common_v1760),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_sib_s::dl_cfg_common_v1760)>::value,
     &dl_cfg_common_sib_v1760_s::desc},
    {offsetof(serving_cell_cfg_common_sib_s, ul_cfg_common_v1760),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_common_sib_s::ul_cfg_common_v1760)>::value,
     &ul_cfg_common_sib_v1760_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_sib_s_g4_names[] = {
    {"downlinkConfigCommon-v1760", nullptr},
    {"uplinkConfigCommon-v1760", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_common_sib_s_g4_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_serving_cell_cfg_common_sib_s_groups[] = {
    {{seq_table_serving_cell_cfg_common_sib_s_g0_fields,
      3,
      UINT64_C(0x0),
      seq_table_serving_cell_cfg_common_sib_s_g0_opts,
      3},
     false,
     seq_table_serving_cell_cfg_common_sib_s_g0_names},
    {{seq_table_serving_cell_cfg_common_sib_s_g1_fields,
      4,
      UINT64_C(0x0),
      seq_table_serving_cell_cfg_common_sib_s_g1_opts,
      4},
     false,
     seq_table_serving_cell_cfg_common_sib_s_g1_names},
    {{seq_table_serving_cell_cfg_common_sib_s_g2_fields,
      1,
      UINT64_C(0x0),
      seq_table_serving_cell_cfg_common_sib_s_g2_opts,
      1},
     false,
     seq_table_serving_cell_cfg_common_sib_s_g2_names},
    {{seq_table_serving_cell_cfg_common_sib_s_g3_fields,
      1,
      UINT64_C(0x0),
      seq_table_serving_cell_cfg_common_sib_s_g3_opts,
      1},
     false,
     seq_table_serving_cell_cfg_common_sib_s_g3_names},
    {{seq_table_serving_cell_cfg_common_sib_s_g4_fields,
      2,
      UINT64_C(0x0),
      seq_table_serving_cell_cfg_common_sib_s_g4_opts,
      2},
     false,
     seq_table_serving_cell_cfg_common_sib_s_g4_names}};
const asn1::table::seq_desc serving_cell_cfg_common_sib_s::desc = {{seq_table_serving_cell_cfg_common_sib_s_fields,
                                                                    8,
                                                                    UINT64_C(0xb1),
                                                                    seq_table_serving_cell_cfg_common_sib_s_opts,
                                                                    4},
                                                                   true,
                                                                   false,
                                                                   seq_table_serving_cell_cfg_common_sib_s_groups,
                                                                   5,
                                                                   seq_table_serving_cell_cfg_common_sib_s_names};

static const char*           enum_names_serving_cell_cfg_common_sib_s_n_timing_advance_offset_e__values[] = {"n0",
                                                                                                             "n25600",
                                                                                                             "n39936"};
const asn1::table::enum_desc serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::desc            = {
    enum_names_serving_cell_cfg_common_sib_s_n_timing_advance_offset_e__values,
    3,
    "serving_cell_cfg_common_sib_s::n_timing_advance_offset_e_"};
uint16_t serving_cell_cfg_common_sib_s::n_timing_advance_offset_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 25600, 39936};
  return map_enum_number(numbers, 3, value, "serving_cell_cfg_common_sib_s::n_timing_advance_offset_e_");
}

static const char* enum_names_serving_cell_cfg_common_sib_s_ssb_periodicity_serving_cell_e__values[] =
    {"ms5", "ms10", "ms20", "ms40", "ms80", "ms160"};
const asn1::table::enum_desc serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_opts::desc = {
    enum_names_serving_cell_cfg_common_sib_s_ssb_periodicity_serving_cell_e__values,
    6,
    "serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_e_"};
uint8_t serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_sib_s::ssb_periodicity_serving_cell_e_");
}

void serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::set(types::options e)
{
  type_ = e;
}
void serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::set_dyn()
{
  set(types::dyn);
}
semi_static_ch_access_cfg_r16_s& serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::set_semi_static()
{
  set(types::semi_static);
  return c;
}
static const asn1::table::seq_field choice_table_serving_cell_cfg_common_sib_s_ch_access_mode_r16_c__alternatives[] = {
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
static const asn1::table::field_name choice_table_serving_cell_cfg_common_sib_s_ch_access_mode_r16_c__names[] = {
    {"dynamic", nullptr},
    {"semiStatic", nullptr}};
const asn1::table::choice_desc serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::desc = {
    asn1::table::enum_int_type<serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::types>(),
    serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::types::nof_types,
    serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::types::nof_exts,
    serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::types::has_ext,
    false,
    choice_table_serving_cell_cfg_common_sib_s_ch_access_mode_r16_c__alternatives,
    &asn1::table::choice_set<serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_>,
    choice_table_serving_cell_cfg_common_sib_s_ch_access_mode_r16_c__names,
    "serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_",
    offsetof(serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_, type_),
    offsetof(serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_, c),
    false};

static const char*           enum_names_serving_cell_cfg_common_sib_s_ch_access_mode_r16_c__types_values[] = {"dynamic",
                                                                                                              "semiStatic"};
const asn1::table::enum_desc serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::types_opts::desc        = {
    enum_names_serving_cell_cfg_common_sib_s_ch_access_mode_r16_c__types_values,
    2,
    "serving_cell_cfg_common_sib_s::ch_access_mode_r16_c_::types"};

static const char* enum_names_serving_cell_cfg_common_sib_s_discovery_burst_win_len_r16_e__values[] =
    {"ms0dot5", "ms1", "ms2", "ms3", "ms4", "ms5"};
const asn1::table::enum_desc serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_opts::desc = {
    enum_names_serving_cell_cfg_common_sib_s_discovery_burst_win_len_r16_e__values,
    6,
    "serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_e_"};
float serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_opts::to_number() const
{
  static const float numbers[] = {0.5, 1.0, 2.0, 3.0, 4.0, 5.0};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_e_");
}
const char* serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.5", "1", "2", "3", "4", "5"};
  return convert_enum_idx(number_strs, 6, value, "serving_cell_cfg_common_sib_s::discovery_burst_win_len_r16_e_");
}

static const char* enum_names_serving_cell_cfg_common_sib_s_discovery_burst_win_len_v1700_e__values[] =
    {"ms0dot125", "ms0dot25", "ms0dot5", "ms0dot75", "ms1", "ms1dot25"};
const asn1::table::enum_desc serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_opts::desc = {
    enum_names_serving_cell_cfg_common_sib_s_discovery_burst_win_len_v1700_e__values,
    6,
    "serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_e_"};
float serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_opts::to_number() const
{
  static const float numbers[] = {0.125, 0.25, 0.5, 0.75, 1.0, 1.25};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_e_");
}
const char* serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_opts::to_number_string() const
{
  static const char* number_strs[] = {"0.125", "0.25", "0.5", "0.75", "1", "1.25"};
  return convert_enum_idx(number_strs, 6, value, "serving_cell_cfg_common_sib_s::discovery_burst_win_len_v1700_e_");
}

// NZP-CSI-RS-Pairing-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_nzp_csi_rs_pairing_r17_s_fields[] = {
    {offsetof(nzp_csi_rs_pairing_r17_s, nzp_csi_rs_res_id1_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(7),
     nullptr},
    {offsetof(nzp_csi_rs_pairing_r17_s, nzp_csi_rs_res_id2_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(7),
     nullptr}};
static const asn1::table::field_name seq_table_nzp_csi_rs_pairing_r17_s_names[] = {
    {"nzp-CSI-RS-ResourceId1-r17", nullptr},
    {"nzp-CSI-RS-ResourceId2-r17", nullptr}};
const asn1::table::seq_desc nzp_csi_rs_pairing_r17_s::desc = {
    {seq_table_nzp_csi_rs_pairing_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_nzp_csi_rs_pairing_r17_s_names};

// CMRGroupingAndPairing-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cmr_grouping_and_pairing_r17_s_fields[] = {
    {offsetof(cmr_grouping_and_pairing_r17_s, nrof_res_group1_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(7),
     nullptr},
    {offsetof(cmr_grouping_and_pairing_r17_s, pair1_of_nzp_csi_rs_r17),
     (int32_t)offsetof(cmr_grouping_and_pairing_r17_s, pair1_of_nzp_csi_rs_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nzp_csi_rs_pairing_r17_s::desc},
    {offsetof(cmr_grouping_and_pairing_r17_s, pair2_of_nzp_csi_rs_r17),
     (int32_t)offsetof(cmr_grouping_and_pairing_r17_s, pair2_of_nzp_csi_rs_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nzp_csi_rs_pairing_r17_s::desc}};
static const asn1::table::field_name seq_table_cmr_grouping_and_pairing_r17_s_names[] = {
    {"nrofResourcesGroup1-r17", nullptr},
    {"pair1OfNZP-CSI-RS-r17", nullptr},
    {"pair2OfNZP-CSI-RS-r17", nullptr}};
static const asn1::table::seq_opt seq_table_cmr_grouping_and_pairing_r17_s_opts[] = {
    {1, (int32_t)offsetof(cmr_grouping_and_pairing_r17_s, pair1_of_nzp_csi_rs_r17_present)},
    {2, (int32_t)offsetof(cmr_grouping_and_pairing_r17_s, pair2_of_nzp_csi_rs_r17_present)}};
const asn1::table::seq_desc cmr_grouping_and_pairing_r17_s::desc = {{seq_table_cmr_grouping_and_pairing_r17_s_fields,
                                                                     3,
                                                                     UINT64_C(0x1),
                                                                     seq_table_cmr_grouping_and_pairing_r17_s_opts,
                                                                     2},
                                                                    false,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_cmr_grouping_and_pairing_r17_s_names};

// CO-DurationsPerCell-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_co_durs_per_cell_r16_s_fields[] = {
    {offsetof(co_durs_per_cell_r16_s, serving_cell_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(co_durs_per_cell_r16_s, position_in_dci_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(co_durs_per_cell_r16_s, subcarrier_spacing_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(co_durs_per_cell_r16_s::subcarrier_spacing_r16)>(),
     decltype(co_durs_per_cell_r16_s::subcarrier_spacing_r16)::has_ext,
     false,
     decltype(co_durs_per_cell_r16_s::subcarrier_spacing_r16)::nof_types,
     decltype(co_durs_per_cell_r16_s::subcarrier_spacing_r16)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(co_durs_per_cell_r16_s, co_dur_list_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(co_durs_per_cell_r16_s::co_dur_list_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(1120), false, false>>::value}};
static const asn1::table::field_name seq_table_co_durs_per_cell_r16_s_names[] = {{"servingCellId-r16", nullptr},
                                                                                 {"positionInDCI-r16", nullptr},
                                                                                 {"subcarrierSpacing-r16", nullptr},
                                                                                 {"co-DurationList-r16", nullptr}};
const asn1::table::seq_desc          co_durs_per_cell_r16_s::desc             = {
    {seq_table_co_durs_per_cell_r16_s_fields, 4, UINT64_C(0xf), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_co_durs_per_cell_r16_s_names};

// CO-DurationsPerCell-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_co_durs_per_cell_r17_s_fields[] = {
    {offsetof(co_durs_per_cell_r17_s, serving_cell_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(co_durs_per_cell_r17_s, position_in_dci_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr},
    {offsetof(co_durs_per_cell_r17_s, subcarrier_spacing_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(co_durs_per_cell_r17_s::subcarrier_spacing_r17)>(),
     decltype(co_durs_per_cell_r17_s::subcarrier_spacing_r17)::has_ext,
     false,
     decltype(co_durs_per_cell_r17_s::subcarrier_spacing_r17)::nof_types,
     decltype(co_durs_per_cell_r17_s::subcarrier_spacing_r17)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(co_durs_per_cell_r17_s, co_dur_list_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(co_durs_per_cell_r17_s::co_dur_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(4480), false, false>>::value}};
static const asn1::table::field_name seq_table_co_durs_per_cell_r17_s_names[] = {{"servingCellId-r17", nullptr},
                                                                                 {"positionInDCI-r17", nullptr},
                                                                                 {"subcarrierSpacing-r17", nullptr},
                                                                                 {"co-DurationList-r17", nullptr}};
const asn1::table::seq_desc          co_durs_per_cell_r17_s::desc             = {
    {seq_table_co_durs_per_cell_r17_s_fields, 4, UINT64_C(0xf), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_co_durs_per_cell_r17_s_names};

// CSI-AssociatedReportConfigInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_csi_associated_report_cfg_info_s_res_for_ch_td_cp_r18_s__fields[] = {
    {offsetof(csi_associated_report_cfg_info_s::res_for_ch_td_cp_r18_s_, res_set2_td_cp_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(16),
     nullptr},
    {offsetof(csi_associated_report_cfg_info_s::res_for_ch_td_cp_r18_s_, res_set3_td_cp_r18),
     (int32_t)offsetof(csi_associated_report_cfg_info_s::res_for_ch_td_cp_r18_s_, res_set3_td_cp_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(16),
     nullptr}};
static const asn1::table::field_name seq_table_csi_associated_report_cfg_info_s_res_for_ch_td_cp_r18_s__names[] = {
    {"resourceSet2TDCP-r18", nullptr},
    {"resourceSet3TDCP-r18", nullptr}};
static const asn1::table::seq_opt seq_table_csi_associated_report_cfg_info_s_res_for_ch_td_cp_r18_s__opts[] = {
    {1, (int32_t)offsetof(csi_associated_report_cfg_info_s::res_for_ch_td_cp_r18_s_, res_set3_td_cp_r18_present)}};
static const asn1::table::seq_desc seq_table_csi_associated_report_cfg_info_s_res_for_ch_td_cp_r18_s_ = {
    {seq_table_csi_associated_report_cfg_info_s_res_for_ch_td_cp_r18_s__fields,
     2,
     UINT64_C(0x1),
     seq_table_csi_associated_report_cfg_info_s_res_for_ch_td_cp_r18_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_associated_report_cfg_info_s_res_for_ch_td_cp_r18_s__names};
static const asn1::table::seq_field seq_table_csi_associated_report_cfg_info_s_fields[] = {
    {offsetof(csi_associated_report_cfg_info_s, report_cfg_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(47),
     nullptr},
    {offsetof(csi_associated_report_cfg_info_s, res_for_ch),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_associated_report_cfg_info_s::res_for_ch_c_::desc},
    {offsetof(csi_associated_report_cfg_info_s, csi_im_res_for_interference),
     (int32_t)offsetof(csi_associated_report_cfg_info_s, csi_im_res_for_interference_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(16),
     nullptr},
    {offsetof(csi_associated_report_cfg_info_s, nzp_csi_rs_res_for_interference),
     (int32_t)offsetof(csi_associated_report_cfg_info_s, nzp_csi_rs_res_for_interference_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(16),
     nullptr}};
static const asn1::table::field_name seq_table_csi_associated_report_cfg_info_s_names[] = {
    {"reportConfigId", nullptr},
    {"resourcesForChannel", nullptr},
    {"csi-IM-ResourcesForInterference", nullptr},
    {"nzp-CSI-RS-ResourcesForInterference", nullptr}};
static const asn1::table::seq_opt seq_table_csi_associated_report_cfg_info_s_opts[] = {
    {2, (int32_t)offsetof(csi_associated_report_cfg_info_s, csi_im_res_for_interference_present)},
    {3, (int32_t)offsetof(csi_associated_report_cfg_info_s, nzp_csi_rs_res_for_interference_present)}};
static const asn1::table::seq_field seq_table_csi_associated_report_cfg_info_s_g0_fields[] = {
    {offsetof(csi_associated_report_cfg_info_s, res_for_ch2_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(csi_associated_report_cfg_info_s::res_for_ch2_r17)>::value,
     &csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::desc},
    {offsetof(csi_associated_report_cfg_info_s, csi_ssb_res_set_ext),
     (int32_t)offsetof(csi_associated_report_cfg_info_s, csi_ssb_res_set_ext_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(2),
     nullptr}};
static const asn1::table::field_name seq_table_csi_associated_report_cfg_info_s_g0_names[] = {
    {"resourcesForChannel2-r17", nullptr},
    {"csi-SSB-ResourceSetExt", nullptr}};
static const asn1::table::seq_opt seq_table_csi_associated_report_cfg_info_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(csi_associated_report_cfg_info_s, csi_ssb_res_set_ext_present)}};
static const asn1::table::seq_field seq_table_csi_associated_report_cfg_info_s_g1_fields[] = {
    {offsetof(csi_associated_report_cfg_info_s, res_for_ch_td_cp_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(csi_associated_report_cfg_info_s::res_for_ch_td_cp_r18)>::value,
     &seq_table_csi_associated_report_cfg_info_s_res_for_ch_td_cp_r18_s_},
    {offsetof(csi_associated_report_cfg_info_s, apply_indicated_tci_state_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18)>::value,
     &csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::desc},
    {offsetof(csi_associated_report_cfg_info_s, apply_indicated_tci_state2_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18)>::value,
     &csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::desc},
    {offsetof(csi_associated_report_cfg_info_s, csi_report_sub_cfg_trigger_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_list_codec<decltype(csi_associated_report_cfg_info_s::csi_report_sub_cfg_trigger_list_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(0), UINT64_C(7), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_associated_report_cfg_info_s_g1_names[] = {
    {"resourcesForChannelTDCP-r18", nullptr},
    {"applyIndicatedTCI-State-r18", nullptr},
    {"applyIndicatedTCI-State2-r18", nullptr},
    {"csi-ReportSubConfigTriggerList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_csi_associated_report_cfg_info_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_csi_associated_report_cfg_info_s_groups[] = {
    {{seq_table_csi_associated_report_cfg_info_s_g0_fields,
      2,
      UINT64_C(0x0),
      seq_table_csi_associated_report_cfg_info_s_g0_opts,
      2},
     false,
     seq_table_csi_associated_report_cfg_info_s_g0_names},
    {{seq_table_csi_associated_report_cfg_info_s_g1_fields,
      4,
      UINT64_C(0x0),
      seq_table_csi_associated_report_cfg_info_s_g1_opts,
      4},
     false,
     seq_table_csi_associated_report_cfg_info_s_g1_names}};
const asn1::table::seq_desc csi_associated_report_cfg_info_s::desc = {
    {seq_table_csi_associated_report_cfg_info_s_fields,
     4,
     UINT64_C(0x3),
     seq_table_csi_associated_report_cfg_info_s_opts,
     2},
    true,
    false,
    seq_table_csi_associated_report_cfg_info_s_groups,
    2,
    seq_table_csi_associated_report_cfg_info_s_names};

static const asn1::table::storage_ops* const
    choice_storage_csi_associated_report_cfg_info_s_res_for_ch_c__alternatives[] = {
        &asn1::table::alternative_ops<csi_associated_report_cfg_info_s::res_for_ch_c_::nzp_csi_rs_s_, false>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc csi_associated_report_cfg_info_s::res_for_ch_c_::storage_desc = {
    choice_storage_csi_associated_report_cfg_info_s_res_for_ch_c__alternatives,
    2,
    "csi_associated_report_cfg_info_s::res_for_ch_c_"};

csi_associated_report_cfg_info_s::res_for_ch_c_::nzp_csi_rs_s_&
csi_associated_report_cfg_info_s::res_for_ch_c_::set_nzp_csi_rs()
{
  set(types::nzp_csi_rs);
  return c.get<nzp_csi_rs_s_>();
}
uint8_t& csi_associated_report_cfg_info_s::res_for_ch_c_::set_csi_ssb_res_set()
{
  set(types::csi_ssb_res_set);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field seq_table_csi_associated_report_cfg_info_s_res_for_ch_c__nzp_csi_rs_s__fields[] = {
    {offsetof(csi_associated_report_cfg_info_s::res_for_ch_c_::nzp_csi_rs_s_, res_set),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(16),
     nullptr},
    {offsetof(csi_associated_report_cfg_info_s::res_for_ch_c_::nzp_csi_rs_s_, qcl_info),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<decltype(csi_associated_report_cfg_info_s::res_for_ch_c_::nzp_csi_rs_s_::qcl_info),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(127), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_associated_report_cfg_info_s_res_for_ch_c__nzp_csi_rs_s__names[] = {
    {"resourceSet", nullptr},
    {"qcl-info", nullptr}};
static const asn1::table::seq_opt seq_table_csi_associated_report_cfg_info_s_res_for_ch_c__nzp_csi_rs_s__opts[] = {
    {1, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_desc seq_table_csi_associated_report_cfg_info_s_res_for_ch_c__nzp_csi_rs_s_ = {
    {seq_table_csi_associated_report_cfg_info_s_res_for_ch_c__nzp_csi_rs_s__fields,
     2,
     UINT64_C(0x1),
     seq_table_csi_associated_report_cfg_info_s_res_for_ch_c__nzp_csi_rs_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_associated_report_cfg_info_s_res_for_ch_c__nzp_csi_rs_s__names};
static const asn1::table::seq_field choice_table_csi_associated_report_cfg_info_s_res_for_ch_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_associated_report_cfg_info_s_res_for_ch_c__nzp_csi_rs_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(1),
     nullptr}};
static const asn1::table::field_name choice_table_csi_associated_report_cfg_info_s_res_for_ch_c__names[] = {
    {"nzp-CSI-RS", nullptr},
    {"csi-SSB-ResourceSet", nullptr}};
const asn1::table::choice_desc csi_associated_report_cfg_info_s::res_for_ch_c_::desc = {
    asn1::table::enum_int_type<csi_associated_report_cfg_info_s::res_for_ch_c_::types>(),
    csi_associated_report_cfg_info_s::res_for_ch_c_::types::nof_types,
    csi_associated_report_cfg_info_s::res_for_ch_c_::types::nof_exts,
    csi_associated_report_cfg_info_s::res_for_ch_c_::types::has_ext,
    false,
    choice_table_csi_associated_report_cfg_info_s_res_for_ch_c__alternatives,
    nullptr,
    choice_table_csi_associated_report_cfg_info_s_res_for_ch_c__names,
    "csi_associated_report_cfg_info_s::res_for_ch_c_",
    offsetof(csi_associated_report_cfg_info_s::res_for_ch_c_, c.type),
    offsetof(csi_associated_report_cfg_info_s::res_for_ch_c_, c.value),
    false,
    &csi_associated_report_cfg_info_s::res_for_ch_c_::storage_desc};

static const char*           enum_names_csi_associated_report_cfg_info_s_res_for_ch_c__types_values[] = {"nzp-CSI-RS",
                                                                                                         "csi-SSB-ResourceSet"};
const asn1::table::enum_desc csi_associated_report_cfg_info_s::res_for_ch_c_::types_opts::desc        = {
    enum_names_csi_associated_report_cfg_info_s_res_for_ch_c__types_values,
    2,
    "csi_associated_report_cfg_info_s::res_for_ch_c_::types"};

static const asn1::table::storage_ops* const
    choice_storage_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::nzp_csi_rs2_r17_s_,
                                      false>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::storage_desc = {
    choice_storage_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__alternatives,
    2,
    "csi_associated_report_cfg_info_s::res_for_ch2_r17_c_"};

csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::nzp_csi_rs2_r17_s_&
csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::set_nzp_csi_rs2_r17()
{
  set(types::nzp_csi_rs2_r17);
  return c.get<nzp_csi_rs2_r17_s_>();
}
uint8_t& csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::set_csi_ssb_res_set2_r17()
{
  set(types::csi_ssb_res_set2_r17);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field
    seq_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__nzp_csi_rs2_r17_s__fields[] = {
        {offsetof(csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::nzp_csi_rs2_r17_s_, res_set2_r17),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(16),
         nullptr},
        {offsetof(csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::nzp_csi_rs2_r17_s_, qcl_info2_r17),
         asn1::table::seq_field::list_presence,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         16,
         &asn1::table::list_codec<
             decltype(csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::nzp_csi_rs2_r17_s_::qcl_info2_r17),
             asn1::table::int_json,
             asn1::table::int_items<false, INT64_C(0), UINT64_C(127), false, false>>::value}};
static const asn1::table::field_name
    seq_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__nzp_csi_rs2_r17_s__names[] = {
        {"resourceSet2-r17", nullptr},
        {"qcl-info2-r17", nullptr}};
static const asn1::table::seq_opt
    seq_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__nzp_csi_rs2_r17_s__opts[] = {
        {1, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_desc seq_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__nzp_csi_rs2_r17_s_ = {
    {seq_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__nzp_csi_rs2_r17_s__fields,
     2,
     UINT64_C(0x1),
     seq_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__nzp_csi_rs2_r17_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__nzp_csi_rs2_r17_s__names};
static const asn1::table::seq_field choice_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__nzp_csi_rs2_r17_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(2),
     nullptr}};
static const asn1::table::field_name choice_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__names[] = {
    {"nzp-CSI-RS2-r17", nullptr},
    {"csi-SSB-ResourceSet2-r17", nullptr}};
const asn1::table::choice_desc csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::desc = {
    asn1::table::enum_int_type<csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::types>(),
    csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::types::nof_types,
    csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::types::nof_exts,
    csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::types::has_ext,
    false,
    choice_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__alternatives,
    nullptr,
    choice_table_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__names,
    "csi_associated_report_cfg_info_s::res_for_ch2_r17_c_",
    offsetof(csi_associated_report_cfg_info_s::res_for_ch2_r17_c_, c.type),
    offsetof(csi_associated_report_cfg_info_s::res_for_ch2_r17_c_, c.value),
    false,
    &csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::storage_desc};

static const char* enum_names_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__types_values[] = {
    "nzp-CSI-RS2-r17",
    "csi-SSB-ResourceSet2-r17"};
const asn1::table::enum_desc csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::types_opts::desc = {
    enum_names_csi_associated_report_cfg_info_s_res_for_ch2_r17_c__types_values,
    2,
    "csi_associated_report_cfg_info_s::res_for_ch2_r17_c_::types"};

static const asn1::table::storage_ops* const
    choice_storage_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<
            csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_set_r18_e_,
            true>::value,
        &asn1::table::alternative_ops<
            csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_res_r18_l_,
            false>::value};
const asn1::table::choice_storage_desc
    csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::storage_desc = {
        choice_storage_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__alternatives,
        2,
        "csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_"};

csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_set_r18_e_&
csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::set_per_set_r18()
{
  set(types::per_set_r18);
  return c.get<per_set_r18_e_>();
}
csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_res_r18_l_&
csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::set_per_res_r18()
{
  set(types::per_res_r18);
  return c.get<per_res_r18_l_>();
}
static const asn1::table::seq_field
    choice_table_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_set_r18_e_>(),
         csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_set_r18_e_::has_ext,
         false,
         csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_set_r18_e_::nof_types,
         csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_set_r18_e_::nof_exts,
         nullptr,
         &csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_set_r18_opts::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         16,
         &asn1::table::list_codec<csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_res_r18_l_,
                                  asn1::table::string_json>::value}};
static const asn1::table::field_name
    choice_table_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__names[] = {
        {"perSet-r18", nullptr},
        {"perResource-r18", nullptr}};
const asn1::table::choice_desc csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::desc = {
    asn1::table::enum_int_type<csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::types>(),
    csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::types::nof_types,
    csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::types::nof_exts,
    csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::types::has_ext,
    false,
    choice_table_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__alternatives,
    nullptr,
    choice_table_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__names,
    "csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_",
    offsetof(csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_, c.type),
    offsetof(csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_, c.value),
    false,
    &csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::storage_desc};

static const char*
    enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__per_set_r18_e__values[] = {"first",
                                                                                                            "second"};
const asn1::table::enum_desc
    csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_set_r18_opts::desc = {
        enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__per_set_r18_e__values,
        2,
        "csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_set_r18_e_"};

static const char*
    enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__per_res_r18_l_item_e__values[] = {
        "first",
        "second"};
const asn1::table::enum_desc
    csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_res_r18_l_item_opts::desc = {
        enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__per_res_r18_l_item_e__values,
        2,
        "csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::per_res_r18_l_item_e_"};

static const char* enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__types_values[] = {
    "perSet-r18",
    "perResource-r18"};
const asn1::table::enum_desc csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::types_opts::desc = {
    enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state_r18_c__types_values,
    2,
    "csi_associated_report_cfg_info_s::apply_indicated_tci_state_r18_c_::types"};

static const asn1::table::storage_ops* const
    choice_storage_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<
            csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_set_r18_e_,
            true>::value,
        &asn1::table::alternative_ops<
            csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_res_r18_l_,
            false>::value};
const asn1::table::choice_storage_desc
    csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::storage_desc = {
        choice_storage_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__alternatives,
        2,
        "csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_"};

csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_set_r18_e_&
csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::set_per_set_r18()
{
  set(types::per_set_r18);
  return c.get<per_set_r18_e_>();
}
csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_res_r18_l_&
csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::set_per_res_r18()
{
  set(types::per_res_r18);
  return c.get<per_res_r18_l_>();
}
static const asn1::table::seq_field
    choice_table_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_set_r18_e_>(),
         csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_set_r18_e_::has_ext,
         false,
         csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_set_r18_e_::nof_types,
         csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_set_r18_e_::nof_exts,
         nullptr,
         &csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_set_r18_opts::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         16,
         &asn1::table::list_codec<csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_res_r18_l_,
                                  asn1::table::string_json>::value}};
static const asn1::table::field_name
    choice_table_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__names[] = {
        {"perSet-r18", nullptr},
        {"perResource-r18", nullptr}};
const asn1::table::choice_desc csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::desc = {
    asn1::table::enum_int_type<csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::types>(),
    csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::types::nof_types,
    csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::types::nof_exts,
    csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::types::has_ext,
    false,
    choice_table_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__alternatives,
    nullptr,
    choice_table_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__names,
    "csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_",
    offsetof(csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_, c.type),
    offsetof(csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_, c.value),
    false,
    &csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::storage_desc};

static const char*
    enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__per_set_r18_e__values[] = {"first",
                                                                                                             "second"};
const asn1::table::enum_desc
    csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_set_r18_opts::desc = {
        enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__per_set_r18_e__values,
        2,
        "csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_set_r18_e_"};

static const char*
    enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__per_res_r18_l_item_e__values[] = {
        "first",
        "second"};
const asn1::table::enum_desc
    csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_res_r18_l_item_opts::desc = {
        enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__per_res_r18_l_item_e__values,
        2,
        "csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::per_res_r18_l_item_e_"};

static const char* enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__types_values[] = {
    "perSet-r18",
    "perResource-r18"};
const asn1::table::enum_desc csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::types_opts::desc = {
    enum_names_csi_associated_report_cfg_info_s_apply_indicated_tci_state2_r18_c__types_values,
    2,
    "csi_associated_report_cfg_info_s::apply_indicated_tci_state2_r18_c_::types"};

// CSI-AperiodicTriggerState ::= SEQUENCE
static const asn1::table::seq_field seq_table_csi_aperiodic_trigger_state_s_fields[] = {
    {offsetof(csi_aperiodic_trigger_state_s, associated_report_cfg_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(csi_aperiodic_trigger_state_s::associated_report_cfg_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &csi_associated_report_cfg_info_s::desc}};
static const asn1::table::field_name seq_table_csi_aperiodic_trigger_state_s_names[] = {
    {"associatedReportConfigInfoList", nullptr}};
static const asn1::table::seq_field seq_table_csi_aperiodic_trigger_state_s_g0_fields[] = {
    {0,
     (int32_t)offsetof(csi_aperiodic_trigger_state_s, ap_csi_mux_mode_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_csi_aperiodic_trigger_state_s_g0_names[] = {
    {"ap-CSI-MultiplexingMode-r17", "enabled"}};
static const asn1::table::seq_opt seq_table_csi_aperiodic_trigger_state_s_g0_opts[] = {
    {0, (int32_t)offsetof(csi_aperiodic_trigger_state_s, ap_csi_mux_mode_r17_present)}};
static const asn1::table::seq_field seq_table_csi_aperiodic_trigger_state_s_g1_fields[] = {
    {offsetof(csi_aperiodic_trigger_state_s, ltm_associated_report_cfg_info_r18),
     (int32_t)offsetof(csi_aperiodic_trigger_state_s, ltm_associated_report_cfg_info_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(47),
     nullptr}};
static const asn1::table::field_name seq_table_csi_aperiodic_trigger_state_s_g1_names[] = {
    {"ltm-AssociatedReportConfigInfo-r18", nullptr}};
static const asn1::table::seq_opt seq_table_csi_aperiodic_trigger_state_s_g1_opts[] = {
    {0, (int32_t)offsetof(csi_aperiodic_trigger_state_s, ltm_associated_report_cfg_info_r18_present)}};
static const asn1::table::seq_group seq_table_csi_aperiodic_trigger_state_s_groups[] = {
    {{seq_table_csi_aperiodic_trigger_state_s_g0_fields,
      1,
      UINT64_C(0x0),
      seq_table_csi_aperiodic_trigger_state_s_g0_opts,
      1},
     false,
     seq_table_csi_aperiodic_trigger_state_s_g0_names},
    {{seq_table_csi_aperiodic_trigger_state_s_g1_fields,
      1,
      UINT64_C(0x0),
      seq_table_csi_aperiodic_trigger_state_s_g1_opts,
      1},
     false,
     seq_table_csi_aperiodic_trigger_state_s_g1_names}};
const asn1::table::seq_desc csi_aperiodic_trigger_state_s::desc = {
    {seq_table_csi_aperiodic_trigger_state_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    seq_table_csi_aperiodic_trigger_state_s_groups,
    2,
    seq_table_csi_aperiodic_trigger_state_s_names};

// CSI-IM-Resource ::= SEQUENCE
static const asn1::table::seq_field seq_table_csi_im_res_s_fields[] = {
    {offsetof(csi_im_res_s, csi_im_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(csi_im_res_s, csi_im_res_elem_pattern),
     (int32_t)offsetof(csi_im_res_s, csi_im_res_elem_pattern_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_im_res_s::csi_im_res_elem_pattern_c_::desc},
    {offsetof(csi_im_res_s, freq_band),
     (int32_t)offsetof(csi_im_res_s, freq_band_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_freq_occupation_s::desc},
    {offsetof(csi_im_res_s, periodicity_and_offset),
     (int32_t)offsetof(csi_im_res_s, periodicity_and_offset_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_res_periodicity_and_offset_c::desc}};
static const asn1::table::field_name seq_table_csi_im_res_s_names[] = {{"csi-IM-ResourceId", nullptr},
                                                                       {"csi-IM-ResourceElementPattern", nullptr},
                                                                       {"freqBand", nullptr},
                                                                       {"periodicityAndOffset", nullptr}};
static const asn1::table::seq_opt    seq_table_csi_im_res_s_opts[]  = {
    {1, (int32_t)offsetof(csi_im_res_s, csi_im_res_elem_pattern_present)},
    {2, (int32_t)offsetof(csi_im_res_s, freq_band_present)},
    {3, (int32_t)offsetof(csi_im_res_s, periodicity_and_offset_present)}};
const asn1::table::seq_desc csi_im_res_s::desc = {
    {seq_table_csi_im_res_s_fields, 4, UINT64_C(0x1), seq_table_csi_im_res_s_opts, 3},
    true,
    false,
    nullptr,
    0,
    seq_table_csi_im_res_s_names};

static const asn1::table::storage_ops* const choice_storage_csi_im_res_s_csi_im_res_elem_pattern_c__alternatives[] = {
    &asn1::table::alternative_ops<csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_, false>::value,
    &asn1::table::alternative_ops<csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_, false>::value};
const asn1::table::choice_storage_desc csi_im_res_s::csi_im_res_elem_pattern_c_::storage_desc = {
    choice_storage_csi_im_res_s_csi_im_res_elem_pattern_c__alternatives,
    2,
    "csi_im_res_s::csi_im_res_elem_pattern_c_"};

csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_& csi_im_res_s::csi_im_res_elem_pattern_c_::set_pattern0()
{
  set(types::pattern0);
  return c.get<pattern0_s_>();
}
csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_& csi_im_res_s::csi_im_res_elem_pattern_c_::set_pattern1()
{
  set(types::pattern1);
  return c.get<pattern1_s_>();
}
static const asn1::table::seq_field seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern0_s__fields[] = {
    {offsetof(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_, subcarrier_location_p0),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0)>(),
     decltype(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0)::has_ext,
     false,
     decltype(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0)::nof_types,
     decltype(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0)::nof_exts,
     nullptr,
     &csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_opts::desc},
    {offsetof(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_, symbol_location_p0),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(12),
     nullptr}};
static const asn1::table::field_name seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern0_s__names[] = {
    {"subcarrierLocation-p0", nullptr},
    {"symbolLocation-p0", nullptr}};
static const asn1::table::seq_desc seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern0_s_ = {
    {seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern0_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern0_s__names};
static const asn1::table::seq_field seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern1_s__fields[] = {
    {offsetof(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_, subcarrier_location_p1),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1)>(),
     decltype(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1)::has_ext,
     false,
     decltype(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1)::nof_types,
     decltype(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1)::nof_exts,
     nullptr,
     &csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_opts::desc},
    {offsetof(csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_, symbol_location_p1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(13),
     nullptr}};
static const asn1::table::field_name seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern1_s__names[] = {
    {"subcarrierLocation-p1", nullptr},
    {"symbolLocation-p1", nullptr}};
static const asn1::table::seq_desc seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern1_s_ = {
    {seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern1_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern1_s__names};
static const asn1::table::seq_field choice_table_csi_im_res_s_csi_im_res_elem_pattern_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern0_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_im_res_s_csi_im_res_elem_pattern_c__pattern1_s_}};
static const asn1::table::field_name choice_table_csi_im_res_s_csi_im_res_elem_pattern_c__names[] = {
    {"pattern0", nullptr},
    {"pattern1", nullptr}};
const asn1::table::choice_desc csi_im_res_s::csi_im_res_elem_pattern_c_::desc = {
    asn1::table::enum_int_type<csi_im_res_s::csi_im_res_elem_pattern_c_::types>(),
    csi_im_res_s::csi_im_res_elem_pattern_c_::types::nof_types,
    csi_im_res_s::csi_im_res_elem_pattern_c_::types::nof_exts,
    csi_im_res_s::csi_im_res_elem_pattern_c_::types::has_ext,
    false,
    choice_table_csi_im_res_s_csi_im_res_elem_pattern_c__alternatives,
    nullptr,
    choice_table_csi_im_res_s_csi_im_res_elem_pattern_c__names,
    "csi_im_res_s::csi_im_res_elem_pattern_c_",
    offsetof(csi_im_res_s::csi_im_res_elem_pattern_c_, c.type),
    offsetof(csi_im_res_s::csi_im_res_elem_pattern_c_, c.value),
    false,
    &csi_im_res_s::csi_im_res_elem_pattern_c_::storage_desc};

static const char* enum_names_csi_im_res_s_csi_im_res_elem_pattern_c__pattern0_s__subcarrier_location_p0_e__values[] =
    {"s0", "s2", "s4", "s6", "s8", "s10"};
const asn1::table::enum_desc csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_opts::desc =
    {enum_names_csi_im_res_s_csi_im_res_elem_pattern_c__pattern0_s__subcarrier_location_p0_e__values,
     6,
     "csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_e_"};
uint8_t csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 2, 4, 6, 8, 10};
  return map_enum_number(
      numbers, 6, value, "csi_im_res_s::csi_im_res_elem_pattern_c_::pattern0_s_::subcarrier_location_p0_e_");
}

static const char* enum_names_csi_im_res_s_csi_im_res_elem_pattern_c__pattern1_s__subcarrier_location_p1_e__values[] = {
    "s0",
    "s4",
    "s8"};
const asn1::table::enum_desc csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_opts::desc =
    {enum_names_csi_im_res_s_csi_im_res_elem_pattern_c__pattern1_s__subcarrier_location_p1_e__values,
     3,
     "csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_e_"};
uint8_t csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 4, 8};
  return map_enum_number(
      numbers, 3, value, "csi_im_res_s::csi_im_res_elem_pattern_c_::pattern1_s_::subcarrier_location_p1_e_");
}

static const char* enum_names_csi_im_res_s_csi_im_res_elem_pattern_c__types_values[]    = {"pattern0", "pattern1"};
const asn1::table::enum_desc csi_im_res_s::csi_im_res_elem_pattern_c_::types_opts::desc = {
    enum_names_csi_im_res_s_csi_im_res_elem_pattern_c__types_values,
    2,
    "csi_im_res_s::csi_im_res_elem_pattern_c_::types"};
uint8_t csi_im_res_s::csi_im_res_elem_pattern_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "csi_im_res_s::csi_im_res_elem_pattern_c_::types");
}

// CSI-IM-ResourceSet ::= SEQUENCE
static const asn1::table::seq_field seq_table_csi_im_res_set_s_fields[] = {
    {offsetof(csi_im_res_set_s, csi_im_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(csi_im_res_set_s, csi_im_res),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<decltype(csi_im_res_set_s::csi_im_res),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(31), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_im_res_set_s_names[] = {{"csi-IM-ResourceSetId", nullptr},
                                                                           {"csi-IM-Resources", nullptr}};
const asn1::table::seq_desc csi_im_res_set_s::desc = {{seq_table_csi_im_res_set_s_fields, 2, UINT64_C(0x3), nullptr, 0},
                                                      true,
                                                      false,
                                                      nullptr,
                                                      0,
                                                      seq_table_csi_im_res_set_s_names};

// NZP-CSI-RS-Resource ::= SEQUENCE
static const asn1::table::seq_field seq_table_nzp_csi_rs_res_s_fields[] = {
    {offsetof(nzp_csi_rs_res_s, nzp_csi_rs_res_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(191),
     nullptr},
    {offsetof(nzp_csi_rs_res_s, res_map),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_rs_res_map_s::desc},
    {offsetof(nzp_csi_rs_res_s, pwr_ctrl_offset),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-8),
     UINT64_C(15),
     nullptr},
    {offsetof(nzp_csi_rs_res_s, pwr_ctrl_offset_ss),
     (int32_t)offsetof(nzp_csi_rs_res_s, pwr_ctrl_offset_ss_present),
     asn1::table::enum_op<decltype(nzp_csi_rs_res_s::pwr_ctrl_offset_ss)>(),
     decltype(nzp_csi_rs_res_s::pwr_ctrl_offset_ss)::has_ext,
     false,
     decltype(nzp_csi_rs_res_s::pwr_ctrl_offset_ss)::nof_types,
     decltype(nzp_csi_rs_res_s::pwr_ctrl_offset_ss)::nof_exts,
     nullptr,
     &nzp_csi_rs_res_s::pwr_ctrl_offset_ss_opts::desc},
    {offsetof(nzp_csi_rs_res_s, scrambling_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(nzp_csi_rs_res_s, periodicity_and_offset),
     (int32_t)offsetof(nzp_csi_rs_res_s, periodicity_and_offset_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_res_periodicity_and_offset_c::desc},
    {offsetof(nzp_csi_rs_res_s, qcl_info_periodic_csi_rs),
     (int32_t)offsetof(nzp_csi_rs_res_s, qcl_info_periodic_csi_rs_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr}};
static const asn1::table::field_name seq_table_nzp_csi_rs_res_s_names[] = {{"nzp-CSI-RS-ResourceId", nullptr},
                                                                           {"resourceMapping", nullptr},
                                                                           {"powerControlOffset", nullptr},
                                                                           {"powerControlOffsetSS", nullptr},
                                                                           {"scramblingID", nullptr},
                                                                           {"periodicityAndOffset", nullptr},
                                                                           {"qcl-InfoPeriodicCSI-RS", nullptr}};
static const asn1::table::seq_opt    seq_table_nzp_csi_rs_res_s_opts[]  = {
    {3, (int32_t)offsetof(nzp_csi_rs_res_s, pwr_ctrl_offset_ss_present)},
    {5, (int32_t)offsetof(nzp_csi_rs_res_s, periodicity_and_offset_present)},
    {6, (int32_t)offsetof(nzp_csi_rs_res_s, qcl_info_periodic_csi_rs_present)}};
static const asn1::table::seq_field seq_table_nzp_csi_rs_res_s_g0_fields[] = {
    {offsetof(nzp_csi_rs_res_s, subcarrier_spacing_r18),
     (int32_t)offsetof(nzp_csi_rs_res_s, subcarrier_spacing_r18_present),
     asn1::table::enum_op<decltype(nzp_csi_rs_res_s::subcarrier_spacing_r18)>(),
     decltype(nzp_csi_rs_res_s::subcarrier_spacing_r18)::has_ext,
     false,
     decltype(nzp_csi_rs_res_s::subcarrier_spacing_r18)::nof_types,
     decltype(nzp_csi_rs_res_s::subcarrier_spacing_r18)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(nzp_csi_rs_res_s, absolute_freq_point_a_r18),
     (int32_t)offsetof(nzp_csi_rs_res_s, absolute_freq_point_a_r18_present),
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(3279165),
     nullptr},
    {0, (int32_t)offsetof(nzp_csi_rs_res_s, cp_r18_present), asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name seq_table_nzp_csi_rs_res_s_g0_names[] = {{"subcarrierSpacing-r18", nullptr},
                                                                              {"absoluteFrequencyPointA-r18", nullptr},
                                                                              {"cyclicPrefix-r18", "extended"}};
static const asn1::table::seq_opt    seq_table_nzp_csi_rs_res_s_g0_opts[]  = {
    {0, (int32_t)offsetof(nzp_csi_rs_res_s, subcarrier_spacing_r18_present)},
    {1, (int32_t)offsetof(nzp_csi_rs_res_s, absolute_freq_point_a_r18_present)},
    {2, (int32_t)offsetof(nzp_csi_rs_res_s, cp_r18_present)}};
static const asn1::table::seq_group seq_table_nzp_csi_rs_res_s_groups[] = {
    {{seq_table_nzp_csi_rs_res_s_g0_fields, 3, UINT64_C(0x0), seq_table_nzp_csi_rs_res_s_g0_opts, 3},
     false,
     seq_table_nzp_csi_rs_res_s_g0_names}};
const asn1::table::seq_desc nzp_csi_rs_res_s::desc = {
    {seq_table_nzp_csi_rs_res_s_fields, 7, UINT64_C(0x17), seq_table_nzp_csi_rs_res_s_opts, 3},
    true,
    false,
    seq_table_nzp_csi_rs_res_s_groups,
    1,
    seq_table_nzp_csi_rs_res_s_names};

static const char*           enum_names_nzp_csi_rs_res_s_pwr_ctrl_offset_ss_e__values[] = {"db-3", "db0", "db3", "db6"};
const asn1::table::enum_desc nzp_csi_rs_res_s::pwr_ctrl_offset_ss_opts::desc            = {
    enum_names_nzp_csi_rs_res_s_pwr_ctrl_offset_ss_e__values,
    4,
    "nzp_csi_rs_res_s::pwr_ctrl_offset_ss_e_"};
int8_t nzp_csi_rs_res_s::pwr_ctrl_offset_ss_opts::to_number() const
{
  static const int8_t numbers[] = {-3, 0, 3, 6};
  return map_enum_number(numbers, 4, value, "nzp_csi_rs_res_s::pwr_ctrl_offset_ss_e_");
}

// NZP-CSI-RS-ResourceSet ::= SEQUENCE
static const asn1::table::seq_field seq_table_nzp_csi_rs_res_set_s_fields[] = {
    {offsetof(nzp_csi_rs_res_set_s, nzp_csi_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(nzp_csi_rs_res_set_s, nzp_csi_rs_res),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(nzp_csi_rs_res_set_s::nzp_csi_rs_res),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(191), false, false>>::value},
    {offsetof(nzp_csi_rs_res_set_s, repeat),
     (int32_t)offsetof(nzp_csi_rs_res_set_s, repeat_present),
     asn1::table::enum_op<decltype(nzp_csi_rs_res_set_s::repeat)>(),
     decltype(nzp_csi_rs_res_set_s::repeat)::has_ext,
     false,
     decltype(nzp_csi_rs_res_set_s::repeat)::nof_types,
     decltype(nzp_csi_rs_res_set_s::repeat)::nof_exts,
     nullptr,
     &nzp_csi_rs_res_set_s::repeat_opts::desc},
    {offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset),
     (int32_t)offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(6),
     nullptr},
    {0,
     (int32_t)offsetof(nzp_csi_rs_res_set_s, trs_info_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_nzp_csi_rs_res_set_s_names[] = {{"nzp-CSI-ResourceSetId", nullptr},
                                                                               {"nzp-CSI-RS-Resources", nullptr},
                                                                               {"repetition", nullptr},
                                                                               {"aperiodicTriggeringOffset", nullptr},
                                                                               {"trs-Info", "true"}};
static const asn1::table::seq_opt    seq_table_nzp_csi_rs_res_set_s_opts[]  = {
    {2, (int32_t)offsetof(nzp_csi_rs_res_set_s, repeat_present)},
    {3, (int32_t)offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset_present)},
    {4, (int32_t)offsetof(nzp_csi_rs_res_set_s, trs_info_present)}};
static const asn1::table::seq_field seq_table_nzp_csi_rs_res_set_s_g0_fields[] = {
    {offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset_r16),
     (int32_t)offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr}};
static const asn1::table::field_name seq_table_nzp_csi_rs_res_set_s_g0_names[] = {
    {"aperiodicTriggeringOffset-r16", nullptr}};
static const asn1::table::seq_opt seq_table_nzp_csi_rs_res_set_s_g0_opts[] = {
    {0, (int32_t)offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset_r16_present)}};
static const asn1::table::seq_field seq_table_nzp_csi_rs_res_set_s_g1_fields[] = {
    {0,
     (int32_t)offsetof(nzp_csi_rs_res_set_s, pdc_info_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(nzp_csi_rs_res_set_s, cmr_grouping_and_pairing_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(nzp_csi_rs_res_set_s::cmr_grouping_and_pairing_r17)>::value,
     &cmr_grouping_and_pairing_r17_s::desc},
    {offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset_r17),
     (int32_t)offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(124),
     nullptr},
    {offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset_l2_r17),
     (int32_t)offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset_l2_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr}};
static const asn1::table::field_name seq_table_nzp_csi_rs_res_set_s_g1_names[] = {
    {"pdc-Info-r17", "true"},
    {"cmrGroupingAndPairing-r17", nullptr},
    {"aperiodicTriggeringOffset-r17", nullptr},
    {"aperiodicTriggeringOffsetL2-r17", nullptr}};
static const asn1::table::seq_opt seq_table_nzp_csi_rs_res_set_s_g1_opts[] = {
    {0, (int32_t)offsetof(nzp_csi_rs_res_set_s, pdc_info_r17_present)},
    {1, asn1::table::seq_field::ptr_presence},
    {2, (int32_t)offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset_r17_present)},
    {3, (int32_t)offsetof(nzp_csi_rs_res_set_s, aperiodic_trigger_offset_l2_r17_present)}};
static const asn1::table::seq_field seq_table_nzp_csi_rs_res_set_s_g2_fields[] = {
    {0,
     (int32_t)offsetof(nzp_csi_rs_res_set_s, res_type_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_nzp_csi_rs_res_set_s_g2_names[] = {{"resourceType-r18", "periodic"}};
static const asn1::table::seq_opt    seq_table_nzp_csi_rs_res_set_s_g2_opts[]  = {
    {0, (int32_t)offsetof(nzp_csi_rs_res_set_s, res_type_r18_present)}};
static const asn1::table::seq_group seq_table_nzp_csi_rs_res_set_s_groups[] = {
    {{seq_table_nzp_csi_rs_res_set_s_g0_fields, 1, UINT64_C(0x0), seq_table_nzp_csi_rs_res_set_s_g0_opts, 1},
     false,
     seq_table_nzp_csi_rs_res_set_s_g0_names},
    {{seq_table_nzp_csi_rs_res_set_s_g1_fields, 4, UINT64_C(0x0), seq_table_nzp_csi_rs_res_set_s_g1_opts, 4},
     false,
     seq_table_nzp_csi_rs_res_set_s_g1_names},
    {{seq_table_nzp_csi_rs_res_set_s_g2_fields, 1, UINT64_C(0x0), seq_table_nzp_csi_rs_res_set_s_g2_opts, 1},
     false,
     seq_table_nzp_csi_rs_res_set_s_g2_names}};
const asn1::table::seq_desc nzp_csi_rs_res_set_s::desc = {
    {seq_table_nzp_csi_rs_res_set_s_fields, 5, UINT64_C(0x3), seq_table_nzp_csi_rs_res_set_s_opts, 3},
    true,
    false,
    seq_table_nzp_csi_rs_res_set_s_groups,
    3,
    seq_table_nzp_csi_rs_res_set_s_names};

static const char*           enum_names_nzp_csi_rs_res_set_s_repeat_e__values[] = {"on", "off"};
const asn1::table::enum_desc nzp_csi_rs_res_set_s::repeat_opts::desc            = {
    enum_names_nzp_csi_rs_res_set_s_repeat_e__values,
    2,
    "nzp_csi_rs_res_set_s::repeat_e_"};

// CSI-SSB-ResourceSet ::= SEQUENCE
static const asn1::table::seq_field seq_table_csi_ssb_res_set_s_fields[] = {
    {offsetof(csi_ssb_res_set_s, csi_ssb_res_set_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(csi_ssb_res_set_s, csi_ssb_res_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(csi_ssb_res_set_s::csi_ssb_res_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_ssb_res_set_s_names[]     = {{"csi-SSB-ResourceSetId", nullptr},
                                                                                {"csi-SSB-ResourceList", nullptr}};
static const asn1::table::seq_field  seq_table_csi_ssb_res_set_s_g0_fields[] = {
    {offsetof(csi_ssb_res_set_s, serving_add_pci_list_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      64,
      &asn1::table::ptr_list_codec<decltype(csi_ssb_res_set_s::serving_add_pci_list_r17),
                                   asn1::table::int_json,
                                   asn1::table::int_items<false, INT64_C(0), UINT64_C(7), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_ssb_res_set_s_g0_names[] = {
    {"servingAdditionalPCIList-r17", nullptr}};
static const asn1::table::seq_opt   seq_table_csi_ssb_res_set_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_csi_ssb_res_set_s_groups[]  = {
    {{seq_table_csi_ssb_res_set_s_g0_fields, 1, UINT64_C(0x0), seq_table_csi_ssb_res_set_s_g0_opts, 1},
      false,
      seq_table_csi_ssb_res_set_s_g0_names}};
const asn1::table::seq_desc csi_ssb_res_set_s::desc = {
    {seq_table_csi_ssb_res_set_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    true,
    false,
    seq_table_csi_ssb_res_set_s_groups,
    1,
    seq_table_csi_ssb_res_set_s_names};

// CSI-ResourceConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_csi_res_cfg_s_fields[] = {
    {offsetof(csi_res_cfg_s, csi_res_cfg_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(111),
     nullptr},
    {offsetof(csi_res_cfg_s, csi_rs_res_set_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_res_cfg_s::csi_rs_res_set_list_c_::desc},
    {offsetof(csi_res_cfg_s, bwp_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr},
    {offsetof(csi_res_cfg_s, res_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(csi_res_cfg_s::res_type)>(),
     decltype(csi_res_cfg_s::res_type)::has_ext,
     false,
     decltype(csi_res_cfg_s::res_type)::nof_types,
     decltype(csi_res_cfg_s::res_type)::nof_exts,
     nullptr,
     &csi_res_cfg_s::res_type_opts::desc}};
static const asn1::table::field_name seq_table_csi_res_cfg_s_names[]     = {{"csi-ResourceConfigId", nullptr},
                                                                            {"csi-RS-ResourceSetList", nullptr},
                                                                            {"bwp-Id", nullptr},
                                                                            {"resourceType", nullptr}};
static const asn1::table::seq_field  seq_table_csi_res_cfg_s_g0_fields[] = {
    {offsetof(csi_res_cfg_s, csi_ssb_res_set_list_ext_r17),
      (int32_t)offsetof(csi_res_cfg_s, csi_ssb_res_set_list_ext_r17_present),
      asn1::table::field_op::int_u8,
      false,
      false,
      INT64_C(0),
      UINT64_C(63),
      nullptr}};
static const asn1::table::field_name seq_table_csi_res_cfg_s_g0_names[] = {{"csi-SSB-ResourceSetListExt-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_csi_res_cfg_s_g0_opts[]  = {
    {0, (int32_t)offsetof(csi_res_cfg_s, csi_ssb_res_set_list_ext_r17_present)}};
static const asn1::table::seq_group seq_table_csi_res_cfg_s_groups[] = {
    {{seq_table_csi_res_cfg_s_g0_fields, 1, UINT64_C(0x0), seq_table_csi_res_cfg_s_g0_opts, 1},
     false,
     seq_table_csi_res_cfg_s_g0_names}};
const asn1::table::seq_desc csi_res_cfg_s::desc = {{seq_table_csi_res_cfg_s_fields, 4, UINT64_C(0xf), nullptr, 0},
                                                   true,
                                                   false,
                                                   seq_table_csi_res_cfg_s_groups,
                                                   1,
                                                   seq_table_csi_res_cfg_s_names};

static const asn1::table::storage_ops* const choice_storage_csi_res_cfg_s_csi_rs_res_set_list_c__alternatives[] = {
    &asn1::table::alternative_ops<csi_res_cfg_s::csi_rs_res_set_list_c_::nzp_csi_rs_ssb_s_, false>::value,
    &asn1::table::alternative_ops<csi_res_cfg_s::csi_rs_res_set_list_c_::csi_im_res_set_list_l_, false>::value};
const asn1::table::choice_storage_desc csi_res_cfg_s::csi_rs_res_set_list_c_::storage_desc = {
    choice_storage_csi_res_cfg_s_csi_rs_res_set_list_c__alternatives,
    2,
    "csi_res_cfg_s::csi_rs_res_set_list_c_"};

csi_res_cfg_s::csi_rs_res_set_list_c_::nzp_csi_rs_ssb_s_& csi_res_cfg_s::csi_rs_res_set_list_c_::set_nzp_csi_rs_ssb()
{
  set(types::nzp_csi_rs_ssb);
  return c.get<nzp_csi_rs_ssb_s_>();
}
csi_res_cfg_s::csi_rs_res_set_list_c_::csi_im_res_set_list_l_&
csi_res_cfg_s::csi_rs_res_set_list_c_::set_csi_im_res_set_list()
{
  set(types::csi_im_res_set_list);
  return c.get<csi_im_res_set_list_l_>();
}
static const asn1::table::seq_field seq_table_csi_res_cfg_s_csi_rs_res_set_list_c__nzp_csi_rs_ssb_s__fields[] = {
    {offsetof(csi_res_cfg_s::csi_rs_res_set_list_c_::nzp_csi_rs_ssb_s_, nzp_csi_rs_res_set_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<
         decltype(csi_res_cfg_s::csi_rs_res_set_list_c_::nzp_csi_rs_ssb_s_::nzp_csi_rs_res_set_list),
         asn1::table::int_json,
         asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, false>>::value},
    {offsetof(csi_res_cfg_s::csi_rs_res_set_list_c_::nzp_csi_rs_ssb_s_, csi_ssb_res_set_list),
     (int32_t)offsetof(csi_res_cfg_s::csi_rs_res_set_list_c_::nzp_csi_rs_ssb_s_, csi_ssb_res_set_list_present),
     asn1::table::field_op::codec,
     false,
     false,
     1,
     1,
     &asn1::table::list_codec<decltype(csi_res_cfg_s::csi_rs_res_set_list_c_::nzp_csi_rs_ssb_s_::csi_ssb_res_set_list),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(63), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_res_cfg_s_csi_rs_res_set_list_c__nzp_csi_rs_ssb_s__names[] = {
    {"nzp-CSI-RS-ResourceSetList", nullptr},
    {"csi-SSB-ResourceSetList", nullptr}};
static const asn1::table::seq_opt seq_table_csi_res_cfg_s_csi_rs_res_set_list_c__nzp_csi_rs_ssb_s__opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, (int32_t)offsetof(csi_res_cfg_s::csi_rs_res_set_list_c_::nzp_csi_rs_ssb_s_, csi_ssb_res_set_list_present)}};
static const asn1::table::seq_desc seq_table_csi_res_cfg_s_csi_rs_res_set_list_c__nzp_csi_rs_ssb_s_ = {
    {seq_table_csi_res_cfg_s_csi_rs_res_set_list_c__nzp_csi_rs_ssb_s__fields,
     2,
     UINT64_C(0x0),
     seq_table_csi_res_cfg_s_csi_rs_res_set_list_c__nzp_csi_rs_ssb_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_res_cfg_s_csi_rs_res_set_list_c__nzp_csi_rs_ssb_s__names};
static const asn1::table::seq_field choice_table_csi_res_cfg_s_csi_rs_res_set_list_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_res_cfg_s_csi_rs_res_set_list_c__nzp_csi_rs_ssb_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<csi_res_cfg_s::csi_rs_res_set_list_c_::csi_im_res_set_list_l_,
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, false>>::value}};
static const asn1::table::field_name choice_table_csi_res_cfg_s_csi_rs_res_set_list_c__names[] = {
    {"nzp-CSI-RS-SSB", nullptr},
    {"csi-IM-ResourceSetList", nullptr}};
const asn1::table::choice_desc csi_res_cfg_s::csi_rs_res_set_list_c_::desc = {
    asn1::table::enum_int_type<csi_res_cfg_s::csi_rs_res_set_list_c_::types>(),
    csi_res_cfg_s::csi_rs_res_set_list_c_::types::nof_types,
    csi_res_cfg_s::csi_rs_res_set_list_c_::types::nof_exts,
    csi_res_cfg_s::csi_rs_res_set_list_c_::types::has_ext,
    false,
    choice_table_csi_res_cfg_s_csi_rs_res_set_list_c__alternatives,
    nullptr,
    choice_table_csi_res_cfg_s_csi_rs_res_set_list_c__names,
    "csi_res_cfg_s::csi_rs_res_set_list_c_",
    offsetof(csi_res_cfg_s::csi_rs_res_set_list_c_, c.type),
    offsetof(csi_res_cfg_s::csi_rs_res_set_list_c_, c.value),
    false,
    &csi_res_cfg_s::csi_rs_res_set_list_c_::storage_desc};

static const char*           enum_names_csi_res_cfg_s_csi_rs_res_set_list_c__types_values[] = {"nzp-CSI-RS-SSB",
                                                                                               "csi-IM-ResourceSetList"};
const asn1::table::enum_desc csi_res_cfg_s::csi_rs_res_set_list_c_::types_opts::desc        = {
    enum_names_csi_res_cfg_s_csi_rs_res_set_list_c__types_values,
    2,
    "csi_res_cfg_s::csi_rs_res_set_list_c_::types"};

static const char* enum_names_csi_res_cfg_s_res_type_e__values[] = {"aperiodic", "semiPersistent", "periodic"};
const asn1::table::enum_desc csi_res_cfg_s::res_type_opts::desc  = {enum_names_csi_res_cfg_s_res_type_e__values,
                                                                    3,
                                                                    "csi_res_cfg_s::res_type_e_"};

// CSI-ReportPeriodicityAndOffset ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_csi_report_periodicity_and_offset_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc csi_report_periodicity_and_offset_c::storage_desc = {
    choice_storage_csi_report_periodicity_and_offset_c_alternatives,
    10,
    "csi_report_periodicity_and_offset_c"};

uint8_t& csi_report_periodicity_and_offset_c::set_slots4()
{
  set(types::slots4);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots5()
{
  set(types::slots5);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots8()
{
  set(types::slots8);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots10()
{
  set(types::slots10);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots16()
{
  set(types::slots16);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots20()
{
  set(types::slots20);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots40()
{
  set(types::slots40);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots80()
{
  set(types::slots80);
  return c.get<uint8_t>();
}
uint8_t& csi_report_periodicity_and_offset_c::set_slots160()
{
  set(types::slots160);
  return c.get<uint8_t>();
}
uint16_t& csi_report_periodicity_and_offset_c::set_slots320()
{
  set(types::slots320);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field choice_table_csi_report_periodicity_and_offset_c_alternatives[] = {
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
     nullptr}};
static const asn1::table::field_name choice_table_csi_report_periodicity_and_offset_c_names[] = {{"slots4", nullptr},
                                                                                                 {"slots5", nullptr},
                                                                                                 {"slots8", nullptr},
                                                                                                 {"slots10", nullptr},
                                                                                                 {"slots16", nullptr},
                                                                                                 {"slots20", nullptr},
                                                                                                 {"slots40", nullptr},
                                                                                                 {"slots80", nullptr},
                                                                                                 {"slots160", nullptr},
                                                                                                 {"slots320", nullptr}};
const asn1::table::choice_desc       csi_report_periodicity_and_offset_c::desc                = {
    asn1::table::enum_int_type<csi_report_periodicity_and_offset_c::types>(),
    csi_report_periodicity_and_offset_c::types::nof_types,
    csi_report_periodicity_and_offset_c::types::nof_exts,
    csi_report_periodicity_and_offset_c::types::has_ext,
    false,
    choice_table_csi_report_periodicity_and_offset_c_alternatives,
    nullptr,
    choice_table_csi_report_periodicity_and_offset_c_names,
    "csi_report_periodicity_and_offset_c",
    offsetof(csi_report_periodicity_and_offset_c, c.type),
    offsetof(csi_report_periodicity_and_offset_c, c.value),
    false,
    &csi_report_periodicity_and_offset_c::storage_desc};

static const char* enum_names_csi_report_periodicity_and_offset_c_types_values[] =
    {"slots4", "slots5", "slots8", "slots10", "slots16", "slots20", "slots40", "slots80", "slots160", "slots320"};
const asn1::table::enum_desc csi_report_periodicity_and_offset_c::types_opts::desc = {
    enum_names_csi_report_periodicity_and_offset_c_types_values,
    10,
    "csi_report_periodicity_and_offset_c::types"};
uint16_t csi_report_periodicity_and_offset_c::types_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 5, 8, 10, 16, 20, 40, 80, 160, 320};
  return map_enum_number(numbers, 10, value, "csi_report_periodicity_and_offset_c::types");
}

// PUCCH-CSI-Resource ::= SEQUENCE
static const asn1::table::seq_field  seq_table_pucch_csi_res_s_fields[] = {{offsetof(pucch_csi_res_s, ul_bw_part_id),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::int_u8,
                                                                            false,
                                                                            false,
                                                                            INT64_C(0),
                                                                            UINT64_C(4),
                                                                            nullptr},
                                                                           {offsetof(pucch_csi_res_s, pucch_res),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::int_u8,
                                                                            false,
                                                                            false,
                                                                            INT64_C(0),
                                                                            UINT64_C(127),
                                                                            nullptr}};
static const asn1::table::field_name seq_table_pucch_csi_res_s_names[]  = {{"uplinkBandwidthPartId", nullptr},
                                                                           {"pucch-Resource", nullptr}};
const asn1::table::seq_desc pucch_csi_res_s::desc = {{seq_table_pucch_csi_res_s_fields, 2, UINT64_C(0x3), nullptr, 0},
                                                     false,
                                                     false,
                                                     nullptr,
                                                     0,
                                                     seq_table_pucch_csi_res_s_names};

// CodebookConfig ::= SEQUENCE
static const asn1::table::seq_field  seq_table_codebook_cfg_s_fields[] = {{offsetof(codebook_cfg_s, codebook_type),
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::field_op::choice,
                                                                           false,
                                                                           false,
                                                                           0,
                                                                           0,
                                                                           nullptr,
                                                                           &codebook_cfg_s::codebook_type_c_::desc}};
static const asn1::table::field_name seq_table_codebook_cfg_s_names[]  = {{"codebookType", nullptr}};
const asn1::table::seq_desc codebook_cfg_s::desc = {{seq_table_codebook_cfg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0,
                                                    seq_table_codebook_cfg_s_names};

static const asn1::table::storage_ops* const choice_storage_codebook_cfg_s_codebook_type_c__alternatives[] = {
    &asn1::table::alternative_ops<codebook_cfg_s::codebook_type_c_::type1_s_, false>::value,
    &asn1::table::alternative_ops<codebook_cfg_s::codebook_type_c_::type2_s_, false>::value};
const asn1::table::choice_storage_desc codebook_cfg_s::codebook_type_c_::storage_desc = {
    choice_storage_codebook_cfg_s_codebook_type_c__alternatives,
    2,
    "codebook_cfg_s::codebook_type_c_"};

codebook_cfg_s::codebook_type_c_::type1_s_& codebook_cfg_s::codebook_type_c_::set_type1()
{
  set(types::type1);
  return c.get<type1_s_>();
}
codebook_cfg_s::codebook_type_c_::type2_s_& codebook_cfg_s::codebook_type_c_::set_type2()
{
  set(types::type2);
  return c.get<type2_s_>();
}
static const asn1::table::seq_field seq_table_codebook_cfg_s_codebook_type_c__type1_s__fields[] = {
    {offsetof(codebook_cfg_s::codebook_type_c_::type1_s_, sub_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::desc},
    {offsetof(codebook_cfg_s::codebook_type_c_::type1_s_, codebook_mode),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(2),
     nullptr}};
static const asn1::table::field_name seq_table_codebook_cfg_s_codebook_type_c__type1_s__names[] = {
    {"subType", nullptr},
    {"codebookMode", nullptr}};
static const asn1::table::seq_desc seq_table_codebook_cfg_s_codebook_type_c__type1_s_ = {
    {seq_table_codebook_cfg_s_codebook_type_c__type1_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__names};
static const asn1::table::seq_field seq_table_codebook_cfg_s_codebook_type_c__type2_s__fields[] = {
    {offsetof(codebook_cfg_s::codebook_type_c_::type2_s_, sub_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::desc},
    {offsetof(codebook_cfg_s::codebook_type_c_::type2_s_, phase_alphabet_size),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size)>(),
     decltype(codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size)::has_ext,
     false,
     decltype(codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size)::nof_types,
     decltype(codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size)::nof_exts,
     nullptr,
     &codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size_opts::desc},
    {offsetof(codebook_cfg_s::codebook_type_c_::type2_s_, subband_amplitude),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(codebook_cfg_s::codebook_type_c_::type2_s_, nof_beams),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams)>(),
     decltype(codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams)::has_ext,
     false,
     decltype(codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams)::nof_types,
     decltype(codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams)::nof_exts,
     nullptr,
     &codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_opts::desc}};
static const asn1::table::field_name seq_table_codebook_cfg_s_codebook_type_c__type2_s__names[] =
    {{"subType", nullptr}, {"phaseAlphabetSize", nullptr}, {"subbandAmplitude", nullptr}, {"numberOfBeams", nullptr}};
static const asn1::table::seq_desc seq_table_codebook_cfg_s_codebook_type_c__type2_s_ = {
    {seq_table_codebook_cfg_s_codebook_type_c__type2_s__fields, 4, UINT64_C(0xf), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_s_codebook_type_c__type2_s__names};
static const asn1::table::seq_field choice_table_codebook_cfg_s_codebook_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_s_codebook_type_c__type1_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_s_codebook_type_c__type2_s_}};
static const asn1::table::field_name choice_table_codebook_cfg_s_codebook_type_c__names[] = {{"type1", nullptr},
                                                                                             {"type2", nullptr}};
const asn1::table::choice_desc       codebook_cfg_s::codebook_type_c_::desc               = {
    asn1::table::enum_int_type<codebook_cfg_s::codebook_type_c_::types>(),
    codebook_cfg_s::codebook_type_c_::types::nof_types,
    codebook_cfg_s::codebook_type_c_::types::nof_exts,
    codebook_cfg_s::codebook_type_c_::types::has_ext,
    false,
    choice_table_codebook_cfg_s_codebook_type_c__alternatives,
    nullptr,
    choice_table_codebook_cfg_s_codebook_type_c__names,
    "codebook_cfg_s::codebook_type_c_",
    offsetof(codebook_cfg_s::codebook_type_c_, c.type),
    offsetof(codebook_cfg_s::codebook_type_c_, c.value),
    false,
    &codebook_cfg_s::codebook_type_c_::storage_desc};

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__alternatives[] = {
        &asn1::table::alternative_ops<codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_,
                                      false>::value,
        &asn1::table::alternative_ops<codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_,
                                      false>::value};
const asn1::table::choice_storage_desc codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::storage_desc = {
    choice_storage_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__alternatives,
    2,
    "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_"};

codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_&
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::set_type_i_single_panel()
{
  set(types::type_i_single_panel);
  return c.get<type_i_single_panel_s_>();
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_&
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::set_type_i_multi_panel()
{
  set(types::type_i_multi_panel);
  return c.get<type_i_multi_panel_s_>();
}
static const asn1::table::seq_field
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__fields[] = {
        {offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_, nr_of_ant_ports),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::desc},
        {offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_,
                  type_i_single_panel_ri_restrict),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::
                                                 type_i_single_panel_s_::type_i_single_panel_ri_restrict),
                                    asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__names[] = {
        {"nrOfAntennaPorts", nullptr},
        {"typeI-SinglePanel-ri-Restriction", nullptr}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s_ = {
        {seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__fields,
         2,
         UINT64_C(0x3),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__names};
static const asn1::table::seq_field
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__fields[] = {
        {offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_, ng_n1_n2),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::desc},
        {offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_, ri_restrict),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<
             decltype(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ri_restrict),
             asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__names[] = {
        {"ng-n1-n2", nullptr},
        {"ri-Restriction", nullptr}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s_ = {
        {seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__fields,
         2,
         UINT64_C(0x3),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__names};
static const asn1::table::seq_field choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s_}};
static const asn1::table::field_name choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__names[] = {
    {"typeI-SinglePanel", nullptr},
    {"typeI-MultiPanel", nullptr}};
const asn1::table::choice_desc codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::desc = {
    asn1::table::enum_int_type<codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::types>(),
    codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::types::nof_types,
    codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::types::nof_exts,
    codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::types::has_ext,
    false,
    choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__alternatives,
    nullptr,
    choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__names,
    "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_",
    offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_, c.type),
    offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_, c.value),
    false,
    &codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::storage_desc};

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__alternatives
        [] = {&asn1::table::alternative_ops<codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::
                                                type_i_single_panel_s_::nr_of_ant_ports_c_::two_s_,
                                            false>::value,
              &asn1::table::alternative_ops<codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::
                                                type_i_single_panel_s_::nr_of_ant_ports_c_::more_than_two_s_,
                                            false>::value};
const asn1::table::choice_storage_desc codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::storage_desc = {
        choice_storage_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__alternatives,
        2,
        "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_"};

codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::two_s_&
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::set_two()
{
  set(types::two);
  return c.get<two_s_>();
}
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::more_than_two_s_&
codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::set_more_than_two()
{
  set(types::more_than_two);
  return c.get<more_than_two_s_>();
}
static const asn1::table::seq_field
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__two_s__fields
        [] = {{offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
                            nr_of_ant_ports_c_::two_s_,
                        two_tx_codebook_subset_restrict),
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<
                   decltype(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
                                nr_of_ant_ports_c_::two_s_::two_tx_codebook_subset_restrict),
                   asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__two_s__names
        [] = {{"twoTX-CodebookSubsetRestriction", nullptr}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__two_s_ = {
        {seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__two_s__fields,
         1,
         UINT64_C(0x1),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__two_s__names};
static const asn1::table::seq_field
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__fields
        [] = {{offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
                            nr_of_ant_ports_c_::more_than_two_s_,
                        n1_n2),
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::choice,
               false,
               false,
               0,
               0,
               nullptr,
               &codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
                   more_than_two_s_::n1_n2_c_::desc},
              {offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
                            nr_of_ant_ports_c_::more_than_two_s_,
                        type_i_single_panel_codebook_subset_restrict_i2),
               (int32_t)offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
                                     nr_of_ant_ports_c_::more_than_two_s_,
                                 type_i_single_panel_codebook_subset_restrict_i2_present),
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<
                   decltype(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
                                nr_of_ant_ports_c_::more_than_two_s_::type_i_single_panel_codebook_subset_restrict_i2),
                   asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__names
        [] = {{"n1-n2", nullptr}, {"typeI-SinglePanel-codebookSubsetRestriction-i2", nullptr}};
static const asn1::table::seq_opt
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__opts
        [] = {{1,
               (int32_t)offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
                                     nr_of_ant_ports_c_::more_than_two_s_,
                                 type_i_single_panel_codebook_subset_restrict_i2_present)}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s_ =
        {{seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__fields,
          2,
          UINT64_C(0x1),
          seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__opts,
          1},
         false,
         false,
         nullptr,
         0,
         seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__names};
static const asn1::table::seq_field
    choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__alternatives[] =
        {{0,
          asn1::table::seq_field::mandatory,
          asn1::table::field_op::sequence,
          false,
          false,
          0,
          0,
          nullptr,
          &seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__two_s_},
         {0,
          asn1::table::seq_field::mandatory,
          asn1::table::field_op::sequence,
          false,
          false,
          0,
          0,
          nullptr,
          &seq_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s_}};
static const asn1::table::field_name
    choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__names
        [] = {{"two", nullptr}, {"moreThanTwo", nullptr}};
const asn1::table::choice_desc
    codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::desc = {
        asn1::table::enum_int_type<codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
                                       nr_of_ant_ports_c_::types>(),
        codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::types::
            nof_types,
        codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::types::
            nof_exts,
        codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::types::
            has_ext,
        false,
        choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__alternatives,
        nullptr,
        choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__names,
        "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_",
        offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_,
                 c.type),
        offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_,
                 c.value),
        false,
        &codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
            storage_desc};

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives
        [] = {&asn1::table::alternative_ops<fixed_bitstring<8>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<16>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<96>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<24>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<128>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<32>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<192>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<192>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<48>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<256>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<256>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value};
const asn1::table::choice_storage_desc codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::storage_desc = {
        choice_storage_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives,
        13,
        "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::more_"
        "than_two_s_::n1_n2_c_"};

fixed_bitstring<8>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_one_type_i_single_panel_restrict()
{
  set(types::two_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_two_type_i_single_panel_restrict()
{
  set(types::two_two_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<16>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_one_type_i_single_panel_restrict()
{
  set(types::four_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<96>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_three_two_type_i_single_panel_restrict()
{
  set(types::three_two_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<96>>();
}
fixed_bitstring<24>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_one_type_i_single_panel_restrict()
{
  set(types::six_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<24>>();
}
fixed_bitstring<128>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_two_type_i_single_panel_restrict()
{
  set(types::four_two_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<128>>();
}
fixed_bitstring<32>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_one_type_i_single_panel_restrict()
{
  set(types::eight_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<192>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_three_type_i_single_panel_restrict()
{
  set(types::four_three_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<192>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_two_type_i_single_panel_restrict()
{
  set(types::six_two_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<48>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_twelve_one_type_i_single_panel_restrict()
{
  set(types::twelve_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<256>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_four_type_i_single_panel_restrict()
{
  set(types::four_four_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<256>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_two_type_i_single_panel_restrict()
{
  set(types::eight_two_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<64>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_sixteen_one_type_i_single_panel_restrict()
{
  set(types::sixteen_one_type_i_single_panel_restrict);
  return c.get<fixed_bitstring<64>>();
}
static const asn1::table::seq_field
    choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives
        [] = {{0,
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
               &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<16>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<96>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<24>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<128>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<32>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<192>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<192>, asn1::table::struct_json>::value},
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
               &asn1::table::object_codec<fixed_bitstring<256>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<256>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value}};
static const asn1::table::field_name
    choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__names
        [] = {{"two-one-TypeI-SinglePanel-Restriction", nullptr},
              {"two-two-TypeI-SinglePanel-Restriction", nullptr},
              {"four-one-TypeI-SinglePanel-Restriction", nullptr},
              {"three-two-TypeI-SinglePanel-Restriction", nullptr},
              {"six-one-TypeI-SinglePanel-Restriction", nullptr},
              {"four-two-TypeI-SinglePanel-Restriction", nullptr},
              {"eight-one-TypeI-SinglePanel-Restriction", nullptr},
              {"four-three-TypeI-SinglePanel-Restriction", nullptr},
              {"six-two-TypeI-SinglePanel-Restriction", nullptr},
              {"twelve-one-TypeI-SinglePanel-Restriction", nullptr},
              {"four-four-TypeI-SinglePanel-Restriction", nullptr},
              {"eight-two-TypeI-SinglePanel-Restriction", nullptr},
              {"sixteen-one-TypeI-SinglePanel-Restriction", nullptr}};
const asn1::table::choice_desc codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::desc = {
        asn1::table::enum_int_type<codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
                                       nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::types>(),
        codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::types::nof_types,
        codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::types::nof_exts,
        codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::types::has_ext,
        false,
        choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives,
        nullptr,
        choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__names,
        "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::more_"
        "than_two_s_::n1_n2_c_",
        offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
                     more_than_two_s_::n1_n2_c_,
                 c.type),
        offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
                     more_than_two_s_::n1_n2_c_,
                 c.value),
        false,
        &codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::storage_desc};

static const char*
    enum_names_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__types_values
        [] = {"two-one-TypeI-SinglePanel-Restriction",
              "two-two-TypeI-SinglePanel-Restriction",
              "four-one-TypeI-SinglePanel-Restriction",
              "three-two-TypeI-SinglePanel-Restriction",
              "six-one-TypeI-SinglePanel-Restriction",
              "four-two-TypeI-SinglePanel-Restriction",
              "eight-one-TypeI-SinglePanel-Restriction",
              "four-three-TypeI-SinglePanel-Restriction",
              "six-two-TypeI-SinglePanel-Restriction",
              "twelve-one-TypeI-SinglePanel-Restriction",
              "four-four-TypeI-SinglePanel-Restriction",
              "eight-two-TypeI-SinglePanel-Restriction",
              "sixteen-one-TypeI-SinglePanel-Restriction"};
const asn1::table::enum_desc codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::types_opts::desc = {
        enum_names_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__types_values,
        13,
        "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::more_"
        "than_two_s_::n1_n2_c_::types"};

static const char*
    enum_names_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__types_values
        [] = {"two", "moreThanTwo"};
const asn1::table::enum_desc codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::
    nr_of_ant_ports_c_::types_opts::desc = {
        enum_names_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_single_panel_s__nr_of_ant_ports_c__types_values,
        2,
        "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::types"};
uint8_t codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::
    types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(
      numbers,
      1,
      value,
      "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_single_panel_s_::nr_of_ant_ports_c_::types");
}

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__ng_n1_n2_c__alternatives
        [] = {&asn1::table::alternative_ops<fixed_bitstring<8>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<16>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<8>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<32>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<16>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<128>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value};
const asn1::table::choice_storage_desc
    codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::storage_desc = {
        choice_storage_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__ng_n1_n2_c__alternatives,
        8,
        "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_"};

fixed_bitstring<8>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_two_two_one_type_i_multi_panel_restrict()
{
  set(types::two_two_one_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<16>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_two_four_one_type_i_multi_panel_restrict()
{
  set(types::two_four_one_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<8>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_four_two_one_type_i_multi_panel_restrict()
{
  set(types::four_two_one_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_two_two_two_type_i_multi_panel_restrict()
{
  set(types::two_two_two_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<32>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_two_eight_one_type_i_multi_panel_restrict()
{
  set(types::two_eight_one_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<16>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_four_four_one_type_i_multi_panel_restrict()
{
  set(types::four_four_one_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<128>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_two_four_two_type_i_multi_panel_restrict()
{
  set(types::two_four_two_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<128>>();
}
fixed_bitstring<64>& codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::
    set_four_two_two_type_i_multi_panel_restrict()
{
  set(types::four_two_two_type_i_multi_panel_restrict);
  return c.get<fixed_bitstring<64>>();
}
static const asn1::table::seq_field
    choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__ng_n1_n2_c__alternatives[] =
        {{0,
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
          &asn1::table::object_codec<fixed_bitstring<16>, asn1::table::struct_json>::value},
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
          &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value},
         {0,
          asn1::table::seq_field::mandatory,
          asn1::table::field_op::codec,
          false,
          false,
          0,
          0,
          &asn1::table::object_codec<fixed_bitstring<32>, asn1::table::struct_json>::value},
         {0,
          asn1::table::seq_field::mandatory,
          asn1::table::field_op::codec,
          false,
          false,
          0,
          0,
          &asn1::table::object_codec<fixed_bitstring<16>, asn1::table::struct_json>::value},
         {0,
          asn1::table::seq_field::mandatory,
          asn1::table::field_op::codec,
          false,
          false,
          0,
          0,
          &asn1::table::object_codec<fixed_bitstring<128>, asn1::table::struct_json>::value},
         {0,
          asn1::table::seq_field::mandatory,
          asn1::table::field_op::codec,
          false,
          false,
          0,
          0,
          &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value}};
static const asn1::table::field_name
    choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__ng_n1_n2_c__names[] = {
        {"two-two-one-TypeI-MultiPanel-Restriction", nullptr},
        {"two-four-one-TypeI-MultiPanel-Restriction", nullptr},
        {"four-two-one-TypeI-MultiPanel-Restriction", nullptr},
        {"two-two-two-TypeI-MultiPanel-Restriction", nullptr},
        {"two-eight-one-TypeI-MultiPanel-Restriction", nullptr},
        {"four-four-one-TypeI-MultiPanel-Restriction", nullptr},
        {"two-four-two-TypeI-MultiPanel-Restriction", nullptr},
        {"four-two-two-TypeI-MultiPanel-Restriction", nullptr}};
const asn1::table::choice_desc
    codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::desc = {
        asn1::table::enum_int_type<
            codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::types>(),
        codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::types::nof_types,
        codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::types::nof_exts,
        codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::types::has_ext,
        false,
        choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__ng_n1_n2_c__alternatives,
        nullptr,
        choice_table_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__ng_n1_n2_c__names,
        "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_",
        offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_, c.type),
        offsetof(codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_, c.value),
        false,
        &codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::storage_desc};

static const char*
    enum_names_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__ng_n1_n2_c__types_values[] = {
        "two-two-one-TypeI-MultiPanel-Restriction",
        "two-four-one-TypeI-MultiPanel-Restriction",
        "four-two-one-TypeI-MultiPanel-Restriction",
        "two-two-two-TypeI-MultiPanel-Restriction",
        "two-eight-one-TypeI-MultiPanel-Restriction",
        "four-four-one-TypeI-MultiPanel-Restriction",
        "two-four-two-TypeI-MultiPanel-Restriction",
        "four-two-two-TypeI-MultiPanel-Restriction"};
const asn1::table::enum_desc
    codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::types_opts::desc = {
        enum_names_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__type_i_multi_panel_s__ng_n1_n2_c__types_values,
        8,
        "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::type_i_multi_panel_s_::ng_n1_n2_c_::types"};

static const char* enum_names_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__types_values[] = {
    "typeI-SinglePanel",
    "typeI-MultiPanel"};
const asn1::table::enum_desc codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::types_opts::desc = {
    enum_names_codebook_cfg_s_codebook_type_c__type1_s__sub_type_c__types_values,
    2,
    "codebook_cfg_s::codebook_type_c_::type1_s_::sub_type_c_::types"};

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__alternatives[] = {
        &asn1::table::alternative_ops<codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_,
                                      false>::value,
        &asn1::table::alternative_ops<codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_,
                                      false>::value};
const asn1::table::choice_storage_desc codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::storage_desc = {
    choice_storage_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__alternatives,
    2,
    "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_"};

codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::set_type_ii()
{
  set(types::type_ii);
  return c.get<type_ii_s_>();
}
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::set_type_ii_port_sel()
{
  set(types::type_ii_port_sel);
  return c.get<type_ii_port_sel_s_>();
}
static const asn1::table::seq_field seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__fields[] =
    {{offsetof(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_, n1_n2_codebook_subset_restrict),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      nullptr,
      &codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::desc},
     {offsetof(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_, type_ii_ri_restrict),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<
          decltype(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::type_ii_ri_restrict),
          asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__names[] =
    {{"n1-n2-codebookSubsetRestriction", nullptr}, {"typeII-RI-Restriction", nullptr}};
static const asn1::table::seq_desc seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s_ = {
    {seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__names};
static const asn1::table::seq_field
    seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_s__fields[] = {
        {offsetof(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_, port_sel_sampling_size),
         (int32_t)offsetof(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_,
                           port_sel_sampling_size_present),
         asn1::table::enum_op<decltype(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::
                                           port_sel_sampling_size)>(),
         decltype(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::
                      port_sel_sampling_size)::has_ext,
         false,
         decltype(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::
                      port_sel_sampling_size)::nof_types,
         decltype(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::
                      port_sel_sampling_size)::nof_exts,
         nullptr,
         &codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::port_sel_sampling_size_opts::
             desc},
        {offsetof(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_,
                  type_ii_port_sel_ri_restrict),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::
                                                 type_ii_port_sel_s_::type_ii_port_sel_ri_restrict),
                                    asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_s__names[] = {
        {"portSelectionSamplingSize", nullptr},
        {"typeII-PortSelectionRI-Restriction", nullptr}};
static const asn1::table::seq_opt
    seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_s__opts[] = {
        {0,
         (int32_t)offsetof(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_,
                           port_sel_sampling_size_present)}};
static const asn1::table::seq_desc seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_s_ =
    {{seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_s__fields,
      2,
      UINT64_C(0x2),
      seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_s__opts,
      1},
     false,
     false,
     nullptr,
     0,
     seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_s__names};
static const asn1::table::seq_field choice_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_s_}};
static const asn1::table::field_name choice_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__names[] = {
    {"typeII", nullptr},
    {"typeII-PortSelection", nullptr}};
const asn1::table::choice_desc codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::desc = {
    asn1::table::enum_int_type<codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types>(),
    codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types::nof_types,
    codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types::nof_exts,
    codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types::has_ext,
    false,
    choice_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__alternatives,
    nullptr,
    choice_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__names,
    "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_",
    offsetof(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_, c.type),
    offsetof(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_, c.value),
    false,
    &codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::storage_desc};

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__n1_n2_codebook_subset_restrict_c__alternatives
        [] = {&asn1::table::alternative_ops<fixed_bitstring<16>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<43>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<32>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<59>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<48>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<75>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<107>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<107>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<96>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<139>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<139>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<128>, false>::value};
const asn1::table::choice_storage_desc codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::
    n1_n2_codebook_subset_restrict_c_::storage_desc = {
        choice_storage_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__n1_n2_codebook_subset_restrict_c__alternatives,
        13,
        "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_"};

fixed_bitstring<16>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_two_one()
{
  set(types::two_one);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<43>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_two_two()
{
  set(types::two_two);
  return c.get<fixed_bitstring<43>>();
}
fixed_bitstring<32>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_four_one()
{
  set(types::four_one);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<59>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_three_two()
{
  set(types::three_two);
  return c.get<fixed_bitstring<59>>();
}
fixed_bitstring<48>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_six_one()
{
  set(types::six_one);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<75>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_four_two()
{
  set(types::four_two);
  return c.get<fixed_bitstring<75>>();
}
fixed_bitstring<64>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_eight_one()
{
  set(types::eight_one);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<107>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_four_three()
{
  set(types::four_three);
  return c.get<fixed_bitstring<107>>();
}
fixed_bitstring<107>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_six_two()
{
  set(types::six_two);
  return c.get<fixed_bitstring<107>>();
}
fixed_bitstring<96>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_twelve_one()
{
  set(types::twelve_one);
  return c.get<fixed_bitstring<96>>();
}
fixed_bitstring<139>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_four_four()
{
  set(types::four_four);
  return c.get<fixed_bitstring<139>>();
}
fixed_bitstring<139>&
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::set_eight_two()
{
  set(types::eight_two);
  return c.get<fixed_bitstring<139>>();
}
fixed_bitstring<128>& codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::
    n1_n2_codebook_subset_restrict_c_::set_sixteen_one()
{
  set(types::sixteen_one);
  return c.get<fixed_bitstring<128>>();
}
static const asn1::table::seq_field
    choice_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__n1_n2_codebook_subset_restrict_c__alternatives
        [] = {{0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<16>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<43>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<32>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<59>, asn1::table::struct_json>::value},
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
               &asn1::table::object_codec<fixed_bitstring<75>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<107>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<107>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<96>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<139>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<139>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<128>, asn1::table::struct_json>::value}};
static const asn1::table::field_name
    choice_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__n1_n2_codebook_subset_restrict_c__names
        [] = {{"two-one", nullptr},
              {"two-two", nullptr},
              {"four-one", nullptr},
              {"three-two", nullptr},
              {"six-one", nullptr},
              {"four-two", nullptr},
              {"eight-one", nullptr},
              {"four-three", nullptr},
              {"six-two", nullptr},
              {"twelve-one", nullptr},
              {"four-four", nullptr},
              {"eight-two", nullptr},
              {"sixteen-one", nullptr}};
const asn1::table::choice_desc
    codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::desc = {
        asn1::table::enum_int_type<codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::
                                       n1_n2_codebook_subset_restrict_c_::types>(),
        codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::types::
            nof_types,
        codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::types::
            nof_exts,
        codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::types::
            has_ext,
        false,
        choice_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__n1_n2_codebook_subset_restrict_c__alternatives,
        nullptr,
        choice_table_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__n1_n2_codebook_subset_restrict_c__names,
        "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_",
        offsetof(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_,
                 c.type),
        offsetof(codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_,
                 c.value),
        false,
        &codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::
            storage_desc};

static const char*
    enum_names_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__n1_n2_codebook_subset_restrict_c__types_values
        [] = {"two-one",
              "two-two",
              "four-one",
              "three-two",
              "six-one",
              "four-two",
              "eight-one",
              "four-three",
              "six-two",
              "twelve-one",
              "four-four",
              "eight-two",
              "sixteen-one"};
const asn1::table::enum_desc codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::
    n1_n2_codebook_subset_restrict_c_::types_opts::desc = {
        enum_names_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_s__n1_n2_codebook_subset_restrict_c__types_values,
        13,
        "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_s_::n1_n2_codebook_subset_restrict_c_::"
        "types"};

static const char*
    enum_names_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_s__port_sel_sampling_size_e__values
        [] = {"n1", "n2", "n3", "n4"};
const asn1::table::enum_desc
    codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::port_sel_sampling_size_opts::desc = {
        enum_names_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_s__port_sel_sampling_size_e__values,
        4,
        "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::port_sel_sampling_size_e_"};
uint8_t
codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::port_sel_sampling_size_opts::to_number()
    const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(
      numbers,
      4,
      value,
      "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_s_::port_sel_sampling_size_e_");
}

static const char* enum_names_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__types_values[] = {
    "typeII",
    "typeII-PortSelection"};
const asn1::table::enum_desc codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types_opts::desc = {
    enum_names_codebook_cfg_s_codebook_type_c__type2_s__sub_type_c__types_values,
    2,
    "codebook_cfg_s::codebook_type_c_::type2_s_::sub_type_c_::types"};

static const char* enum_names_codebook_cfg_s_codebook_type_c__type2_s__phase_alphabet_size_e__values[]  = {"n4", "n8"};
const asn1::table::enum_desc codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size_opts::desc = {
    enum_names_codebook_cfg_s_codebook_type_c__type2_s__phase_alphabet_size_e__values,
    2,
    "codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size_e_"};
uint8_t codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8};
  return map_enum_number(numbers, 2, value, "codebook_cfg_s::codebook_type_c_::type2_s_::phase_alphabet_size_e_");
}

static const char* enum_names_codebook_cfg_s_codebook_type_c__type2_s__nof_beams_e__values[] = {"two", "three", "four"};
const asn1::table::enum_desc codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_opts::desc = {
    enum_names_codebook_cfg_s_codebook_type_c__type2_s__nof_beams_e__values,
    3,
    "codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_e_"};
uint8_t codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 3, 4};
  return map_enum_number(numbers, 3, value, "codebook_cfg_s::codebook_type_c_::type2_s_::nof_beams_e_");
}

static const char*           enum_names_codebook_cfg_s_codebook_type_c__types_values[] = {"type1", "type2"};
const asn1::table::enum_desc codebook_cfg_s::codebook_type_c_::types_opts::desc        = {
    enum_names_codebook_cfg_s_codebook_type_c__types_values,
    2,
    "codebook_cfg_s::codebook_type_c_::types"};
uint8_t codebook_cfg_s::codebook_type_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "codebook_cfg_s::codebook_type_c_::types");
}

// PortIndexFor8Ranks ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_port_idx_for8_ranks_c_alternatives[] = {
    &asn1::table::alternative_ops<port_idx_for8_ranks_c::port_idx8_s_, false>::value,
    &asn1::table::alternative_ops<port_idx_for8_ranks_c::port_idx4_s_, false>::value,
    &asn1::table::alternative_ops<port_idx_for8_ranks_c::port_idx2_s_, false>::value,
    nullptr};
const asn1::table::choice_storage_desc port_idx_for8_ranks_c::storage_desc = {
    choice_storage_port_idx_for8_ranks_c_alternatives,
    4,
    "port_idx_for8_ranks_c"};

port_idx_for8_ranks_c::port_idx8_s_& port_idx_for8_ranks_c::set_port_idx8()
{
  set(types::port_idx8);
  return c.get<port_idx8_s_>();
}
port_idx_for8_ranks_c::port_idx4_s_& port_idx_for8_ranks_c::set_port_idx4()
{
  set(types::port_idx4);
  return c.get<port_idx4_s_>();
}
port_idx_for8_ranks_c::port_idx2_s_& port_idx_for8_ranks_c::set_port_idx2()
{
  set(types::port_idx2);
  return c.get<port_idx2_s_>();
}
void port_idx_for8_ranks_c::set_port_idx1()
{
  set(types::port_idx1);
}
static const asn1::table::seq_field seq_table_port_idx_for8_ranks_c_port_idx8_s__fields[] = {
    {offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank1_8),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank1_8_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank2_8),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank2_8_present),
     asn1::table::field_op::codec,
     false,
     false,
     2,
     2,
     &asn1::table::list_codec<decltype(port_idx_for8_ranks_c::port_idx8_s_::rank2_8),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(7), false, false>>::value},
    {offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank3_8),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank3_8_present),
     asn1::table::field_op::codec,
     false,
     false,
     3,
     3,
     &asn1::table::list_codec<decltype(port_idx_for8_ranks_c::port_idx8_s_::rank3_8),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(7), false, false>>::value},
    {offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank4_8),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank4_8_present),
     asn1::table::field_op::codec,
     false,
     false,
     4,
     4,
     &asn1::table::list_codec<decltype(port_idx_for8_ranks_c::port_idx8_s_::rank4_8),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(7), false, false>>::value},
    {offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank5_8),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank5_8_present),
     asn1::table::field_op::codec,
     false,
     false,
     5,
     5,
     &asn1::table::list_codec<decltype(port_idx_for8_ranks_c::port_idx8_s_::rank5_8),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(7), false, false>>::value},
    {offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank6_8),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank6_8_present),
     asn1::table::field_op::codec,
     false,
     false,
     6,
     6,
     &asn1::table::list_codec<decltype(port_idx_for8_ranks_c::port_idx8_s_::rank6_8),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(7), false, false>>::value},
    {offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank7_8),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank7_8_present),
     asn1::table::field_op::codec,
     false,
     false,
     7,
     7,
     &asn1::table::list_codec<decltype(port_idx_for8_ranks_c::port_idx8_s_::rank7_8),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(7), false, false>>::value},
    {offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank8_8),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank8_8_present),
     asn1::table::field_op::codec,
     false,
     false,
     8,
     8,
     &asn1::table::list_codec<decltype(port_idx_for8_ranks_c::port_idx8_s_::rank8_8),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(7), false, false>>::value}};
static const asn1::table::field_name seq_table_port_idx_for8_ranks_c_port_idx8_s__names[] = {{"rank1-8", nullptr},
                                                                                             {"rank2-8", nullptr},
                                                                                             {"rank3-8", nullptr},
                                                                                             {"rank4-8", nullptr},
                                                                                             {"rank5-8", nullptr},
                                                                                             {"rank6-8", nullptr},
                                                                                             {"rank7-8", nullptr},
                                                                                             {"rank8-8", nullptr}};
static const asn1::table::seq_opt    seq_table_port_idx_for8_ranks_c_port_idx8_s__opts[]  = {
    {0, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank1_8_present)},
    {1, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank2_8_present)},
    {2, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank3_8_present)},
    {3, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank4_8_present)},
    {4, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank5_8_present)},
    {5, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank6_8_present)},
    {6, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank7_8_present)},
    {7, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx8_s_, rank8_8_present)}};
static const asn1::table::seq_desc seq_table_port_idx_for8_ranks_c_port_idx8_s_ = {
    {seq_table_port_idx_for8_ranks_c_port_idx8_s__fields,
     8,
     UINT64_C(0x0),
     seq_table_port_idx_for8_ranks_c_port_idx8_s__opts,
     8},
    false,
    false,
    nullptr,
    0,
    seq_table_port_idx_for8_ranks_c_port_idx8_s__names};
static const asn1::table::seq_field seq_table_port_idx_for8_ranks_c_port_idx4_s__fields[] = {
    {offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank1_4),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank1_4_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank2_4),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank2_4_present),
     asn1::table::field_op::codec,
     false,
     false,
     2,
     2,
     &asn1::table::list_codec<decltype(port_idx_for8_ranks_c::port_idx4_s_::rank2_4),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(3), false, false>>::value},
    {offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank3_4),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank3_4_present),
     asn1::table::field_op::codec,
     false,
     false,
     3,
     3,
     &asn1::table::list_codec<decltype(port_idx_for8_ranks_c::port_idx4_s_::rank3_4),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(3), false, false>>::value},
    {offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank4_4),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank4_4_present),
     asn1::table::field_op::codec,
     false,
     false,
     4,
     4,
     &asn1::table::list_codec<decltype(port_idx_for8_ranks_c::port_idx4_s_::rank4_4),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(3), false, false>>::value}};
static const asn1::table::field_name seq_table_port_idx_for8_ranks_c_port_idx4_s__names[] = {{"rank1-4", nullptr},
                                                                                             {"rank2-4", nullptr},
                                                                                             {"rank3-4", nullptr},
                                                                                             {"rank4-4", nullptr}};
static const asn1::table::seq_opt    seq_table_port_idx_for8_ranks_c_port_idx4_s__opts[]  = {
    {0, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank1_4_present)},
    {1, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank2_4_present)},
    {2, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank3_4_present)},
    {3, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx4_s_, rank4_4_present)}};
static const asn1::table::seq_desc seq_table_port_idx_for8_ranks_c_port_idx4_s_ = {
    {seq_table_port_idx_for8_ranks_c_port_idx4_s__fields,
     4,
     UINT64_C(0x0),
     seq_table_port_idx_for8_ranks_c_port_idx4_s__opts,
     4},
    false,
    false,
    nullptr,
    0,
    seq_table_port_idx_for8_ranks_c_port_idx4_s__names};
static const asn1::table::seq_field seq_table_port_idx_for8_ranks_c_port_idx2_s__fields[] = {
    {offsetof(port_idx_for8_ranks_c::port_idx2_s_, rank1_2),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx2_s_, rank1_2_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(1),
     nullptr},
    {offsetof(port_idx_for8_ranks_c::port_idx2_s_, rank2_2),
     (int32_t)offsetof(port_idx_for8_ranks_c::port_idx2_s_, rank2_2_present),
     asn1::table::field_op::codec,
     false,
     false,
     2,
     2,
     &asn1::table::list_codec<decltype(port_idx_for8_ranks_c::port_idx2_s_::rank2_2),
                              asn1::table::int_json,
                              asn1::table::int_items<true, INT64_C(0), UINT64_C(1), false, false>>::value}};
static const asn1::table::field_name seq_table_port_idx_for8_ranks_c_port_idx2_s__names[] = {{"rank1-2", nullptr},
                                                                                             {"rank2-2", nullptr}};
static const asn1::table::seq_opt    seq_table_port_idx_for8_ranks_c_port_idx2_s__opts[]  = {
    {0, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx2_s_, rank1_2_present)},
    {1, (int32_t)offsetof(port_idx_for8_ranks_c::port_idx2_s_, rank2_2_present)}};
static const asn1::table::seq_desc seq_table_port_idx_for8_ranks_c_port_idx2_s_ = {
    {seq_table_port_idx_for8_ranks_c_port_idx2_s__fields,
     2,
     UINT64_C(0x0),
     seq_table_port_idx_for8_ranks_c_port_idx2_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_port_idx_for8_ranks_c_port_idx2_s__names};
static const asn1::table::seq_field choice_table_port_idx_for8_ranks_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_port_idx_for8_ranks_c_port_idx8_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_port_idx_for8_ranks_c_port_idx4_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_port_idx_for8_ranks_c_port_idx2_s_},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_port_idx_for8_ranks_c_names[] = {{"portIndex8", nullptr},
                                                                                   {"portIndex4", nullptr},
                                                                                   {"portIndex2", nullptr},
                                                                                   {"portIndex1", nullptr}};
const asn1::table::choice_desc       port_idx_for8_ranks_c::desc                = {
    asn1::table::enum_int_type<port_idx_for8_ranks_c::types>(),
    port_idx_for8_ranks_c::types::nof_types,
    port_idx_for8_ranks_c::types::nof_exts,
    port_idx_for8_ranks_c::types::has_ext,
    false,
    choice_table_port_idx_for8_ranks_c_alternatives,
    nullptr,
    choice_table_port_idx_for8_ranks_c_names,
    "port_idx_for8_ranks_c",
    offsetof(port_idx_for8_ranks_c, c.type),
    offsetof(port_idx_for8_ranks_c, c.value),
    false,
    &port_idx_for8_ranks_c::storage_desc};

static const char*           enum_names_port_idx_for8_ranks_c_types_values[] = {"portIndex8",
                                                                                "portIndex4",
                                                                                "portIndex2",
                                                                                "portIndex1"};
const asn1::table::enum_desc port_idx_for8_ranks_c::types_opts::desc = {enum_names_port_idx_for8_ranks_c_types_values,
                                                                        4,
                                                                        "port_idx_for8_ranks_c::types"};
uint8_t                      port_idx_for8_ranks_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {8, 4, 2, 1};
  return map_enum_number(numbers, 4, value, "port_idx_for8_ranks_c::types");
}

// CodebookConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_codebook_cfg_r16_s_fields[] = {
    {offsetof(codebook_cfg_r16_s, codebook_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &codebook_cfg_r16_s::codebook_type_c_::desc}};
static const asn1::table::field_name seq_table_codebook_cfg_r16_s_names[] = {{"codebookType", nullptr}};
const asn1::table::seq_desc          codebook_cfg_r16_s::desc             = {
    {seq_table_codebook_cfg_r16_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_r16_s_names};

static const asn1::table::seq_field seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__fields[] = {
    {offsetof(codebook_cfg_r16_s::codebook_type_c_::type2_s_, sub_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::desc},
    {offsetof(codebook_cfg_r16_s::codebook_type_c_::type2_s_, nof_pmi_subbands_per_cqi_subband_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(2),
     nullptr},
    {offsetof(codebook_cfg_r16_s::codebook_type_c_::type2_s_, param_combination_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr}};
static const asn1::table::field_name seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__names[] = {
    {"subType", nullptr},
    {"numberOfPMI-SubbandsPerCQI-Subband-r16", nullptr},
    {"paramCombination-r16", nullptr}};
static const asn1::table::seq_desc seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s_ = {
    {seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__names};
static const asn1::table::seq_field choice_table_codebook_cfg_r16_s_codebook_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s_}};
static const asn1::table::field_name choice_table_codebook_cfg_r16_s_codebook_type_c__names[] = {{"type2", nullptr}};
const asn1::table::choice_desc       codebook_cfg_r16_s::codebook_type_c_::desc               = {
    asn1::table::enum_int_type<codebook_cfg_r16_s::codebook_type_c_::types>(),
    codebook_cfg_r16_s::codebook_type_c_::types::nof_types,
    codebook_cfg_r16_s::codebook_type_c_::types::nof_exts,
    codebook_cfg_r16_s::codebook_type_c_::types::has_ext,
    false,
    choice_table_codebook_cfg_r16_s_codebook_type_c__alternatives,
    nullptr,
    choice_table_codebook_cfg_r16_s_codebook_type_c__names,
    "codebook_cfg_r16_s::codebook_type_c_",
    asn1::table::choice_desc::no_member,
    offsetof(codebook_cfg_r16_s::codebook_type_c_, c),
    false};

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__alternatives[] = {
        &asn1::table::alternative_ops<codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_,
                                      false>::value,
        &asn1::table::alternative_ops<
            codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_,
            false>::value};
const asn1::table::choice_storage_desc codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::storage_desc = {
    choice_storage_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__alternatives,
    2,
    "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_"};

codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_&
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::set_type_ii_r16()
{
  set(types::type_ii_r16);
  return c.get<type_ii_r16_s_>();
}
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_&
codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::set_type_ii_port_sel_r16()
{
  set(types::type_ii_port_sel_r16);
  return c.get<type_ii_port_sel_r16_s_>();
}
static const asn1::table::seq_field
    seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__fields[] = {
        {offsetof(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_,
                  n1_n2_codebook_subset_restrict_r16),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
             n1_n2_codebook_subset_restrict_r16_c_::desc},
        {offsetof(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_, type_ii_ri_restrict_r16),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::
                                                 type_ii_r16_s_::type_ii_ri_restrict_r16),
                                    asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__names[] = {
        {"n1-n2-codebookSubsetRestriction-r16", nullptr},
        {"typeII-RI-Restriction-r16", nullptr}};
static const asn1::table::seq_desc seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s_ = {
    {seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__fields,
     2,
     UINT64_C(0x3),
     nullptr,
     0},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__names};
static const asn1::table::seq_field
    seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_r16_s__fields[] = {
        {offsetof(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_,
                  port_sel_sampling_size_r16),
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<decltype(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::
                                           type_ii_port_sel_r16_s_::port_sel_sampling_size_r16)>(),
         decltype(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::
                      port_sel_sampling_size_r16)::has_ext,
         false,
         decltype(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::
                      port_sel_sampling_size_r16)::nof_types,
         decltype(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::
                      port_sel_sampling_size_r16)::nof_exts,
         nullptr,
         &codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::
             port_sel_sampling_size_r16_opts::desc},
        {offsetof(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_,
                  type_ii_port_sel_ri_restrict_r16),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::
                                                 type_ii_port_sel_r16_s_::type_ii_port_sel_ri_restrict_r16),
                                    asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_r16_s__names[] = {
        {"portSelectionSamplingSize-r16", nullptr},
        {"typeII-PortSelectionRI-Restriction-r16", nullptr}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_r16_s_ = {
        {seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_r16_s__fields,
         2,
         UINT64_C(0x3),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_r16_s__names};
static const asn1::table::seq_field
    choice_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s_},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &seq_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_r16_s_}};
static const asn1::table::field_name choice_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__names[] = {
    {"typeII-r16", nullptr},
    {"typeII-PortSelection-r16", nullptr}};
const asn1::table::choice_desc codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::desc = {
    asn1::table::enum_int_type<codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::types>(),
    codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::types::nof_types,
    codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::types::nof_exts,
    codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::types::has_ext,
    false,
    choice_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__alternatives,
    nullptr,
    choice_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__names,
    "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_",
    offsetof(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_, c.type),
    offsetof(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_, c.value),
    false,
    &codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::storage_desc};

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__n1_n2_codebook_subset_restrict_r16_c__alternatives
        [] = {&asn1::table::alternative_ops<fixed_bitstring<16>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<43>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<32>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<59>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<48>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<75>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<107>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<107>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<96>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<139>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<139>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<128>, false>::value};
const asn1::table::choice_storage_desc codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::storage_desc = {
        choice_storage_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__n1_n2_codebook_subset_restrict_r16_c__alternatives,
        13,
        "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_codebook_subset_restrict_"
        "r16_c_"};

fixed_bitstring<16>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_two_one()
{
  set(types::two_one);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<43>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_two_two()
{
  set(types::two_two);
  return c.get<fixed_bitstring<43>>();
}
fixed_bitstring<32>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_four_one()
{
  set(types::four_one);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<59>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_three_two()
{
  set(types::three_two);
  return c.get<fixed_bitstring<59>>();
}
fixed_bitstring<48>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_six_one()
{
  set(types::six_one);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<75>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_four_two()
{
  set(types::four_two);
  return c.get<fixed_bitstring<75>>();
}
fixed_bitstring<64>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_eight_one()
{
  set(types::eight_one);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<107>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_four_three()
{
  set(types::four_three);
  return c.get<fixed_bitstring<107>>();
}
fixed_bitstring<107>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_six_two()
{
  set(types::six_two);
  return c.get<fixed_bitstring<107>>();
}
fixed_bitstring<96>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_twelve_one()
{
  set(types::twelve_one);
  return c.get<fixed_bitstring<96>>();
}
fixed_bitstring<139>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_four_four()
{
  set(types::four_four);
  return c.get<fixed_bitstring<139>>();
}
fixed_bitstring<139>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_eight_two()
{
  set(types::eight_two);
  return c.get<fixed_bitstring<139>>();
}
fixed_bitstring<128>& codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::set_sixteen_one()
{
  set(types::sixteen_one);
  return c.get<fixed_bitstring<128>>();
}
static const asn1::table::seq_field
    choice_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__n1_n2_codebook_subset_restrict_r16_c__alternatives
        [] = {{0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<16>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<43>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<32>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<59>, asn1::table::struct_json>::value},
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
               &asn1::table::object_codec<fixed_bitstring<75>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<107>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<107>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<96>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<139>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<139>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<128>, asn1::table::struct_json>::value}};
static const asn1::table::field_name
    choice_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__n1_n2_codebook_subset_restrict_r16_c__names
        [] = {{"two-one", nullptr},
              {"two-two", nullptr},
              {"four-one", nullptr},
              {"three-two", nullptr},
              {"six-one", nullptr},
              {"four-two", nullptr},
              {"eight-one", nullptr},
              {"four-three", nullptr},
              {"six-two", nullptr},
              {"twelve-one", nullptr},
              {"four-four", nullptr},
              {"eight-two", nullptr},
              {"sixteen-one", nullptr}};
const asn1::table::choice_desc codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::desc = {
        asn1::table::enum_int_type<codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
                                       n1_n2_codebook_subset_restrict_r16_c_::types>(),
        codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
            n1_n2_codebook_subset_restrict_r16_c_::types::nof_types,
        codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
            n1_n2_codebook_subset_restrict_r16_c_::types::nof_exts,
        codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
            n1_n2_codebook_subset_restrict_r16_c_::types::has_ext,
        false,
        choice_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__n1_n2_codebook_subset_restrict_r16_c__alternatives,
        nullptr,
        choice_table_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__n1_n2_codebook_subset_restrict_r16_c__names,
        "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_codebook_subset_restrict_"
        "r16_c_",
        offsetof(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
                     n1_n2_codebook_subset_restrict_r16_c_,
                 c.type),
        offsetof(codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
                     n1_n2_codebook_subset_restrict_r16_c_,
                 c.value),
        false,
        &codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
            n1_n2_codebook_subset_restrict_r16_c_::storage_desc};

static const char*
    enum_names_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__n1_n2_codebook_subset_restrict_r16_c__types_values
        [] = {"two-one",
              "two-two",
              "four-one",
              "three-two",
              "six-one",
              "four-two",
              "eight-one",
              "four-three",
              "six-two",
              "twelve-one",
              "four-four",
              "eight-two",
              "sixteen-one"};
const asn1::table::enum_desc codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::
    n1_n2_codebook_subset_restrict_r16_c_::types_opts::desc = {
        enum_names_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_r16_s__n1_n2_codebook_subset_restrict_r16_c__types_values,
        13,
        "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_r16_s_::n1_n2_codebook_subset_restrict_"
        "r16_c_::types"};

static const char*
    enum_names_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_r16_s__port_sel_sampling_size_r16_e__values
        [] = {"n1", "n2", "n3", "n4"};
const asn1::table::enum_desc codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::
    port_sel_sampling_size_r16_opts::desc = {
        enum_names_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__type_ii_port_sel_r16_s__port_sel_sampling_size_r16_e__values,
        4,
        "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::port_sel_sampling_size_"
        "r16_e_"};
uint8_t codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::
    port_sel_sampling_size_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers,
                         4,
                         value,
                         "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::type_ii_port_sel_r16_s_::port_"
                         "sel_sampling_size_r16_e_");
}

static const char* enum_names_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__types_values[] = {
    "typeII-r16",
    "typeII-PortSelection-r16"};
const asn1::table::enum_desc codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::types_opts::desc = {
    enum_names_codebook_cfg_r16_s_codebook_type_c__type2_s__sub_type_c__types_values,
    2,
    "codebook_cfg_r16_s::codebook_type_c_::type2_s_::sub_type_c_::types"};

static const char*           enum_names_codebook_cfg_r16_s_codebook_type_c__types_values[] = {"type2"};
const asn1::table::enum_desc codebook_cfg_r16_s::codebook_type_c_::types_opts::desc        = {
    enum_names_codebook_cfg_r16_s_codebook_type_c__types_values,
    1,
    "codebook_cfg_r16_s::codebook_type_c_::types"};
uint8_t codebook_cfg_r16_s::codebook_type_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "codebook_cfg_r16_s::codebook_type_c_::types");
}

// CodebookConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_codebook_cfg_r17_s_fields[] = {
    {offsetof(codebook_cfg_r17_s, codebook_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &codebook_cfg_r17_s::codebook_type_c_::desc}};
static const asn1::table::field_name seq_table_codebook_cfg_r17_s_names[] = {{"codebookType", nullptr}};
const asn1::table::seq_desc          codebook_cfg_r17_s::desc             = {
    {seq_table_codebook_cfg_r17_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_r17_s_names};

static const asn1::table::storage_ops* const choice_storage_codebook_cfg_r17_s_codebook_type_c__alternatives[] = {
    &asn1::table::alternative_ops<codebook_cfg_r17_s::codebook_type_c_::type1_s_, false>::value,
    &asn1::table::alternative_ops<codebook_cfg_r17_s::codebook_type_c_::type2_s_, false>::value};
const asn1::table::choice_storage_desc codebook_cfg_r17_s::codebook_type_c_::storage_desc = {
    choice_storage_codebook_cfg_r17_s_codebook_type_c__alternatives,
    2,
    "codebook_cfg_r17_s::codebook_type_c_"};

codebook_cfg_r17_s::codebook_type_c_::type1_s_& codebook_cfg_r17_s::codebook_type_c_::set_type1()
{
  set(types::type1);
  return c.get<type1_s_>();
}
codebook_cfg_r17_s::codebook_type_c_::type2_s_& codebook_cfg_r17_s::codebook_type_c_::set_type2()
{
  set(types::type2);
  return c.get<type2_s_>();
}
static const asn1::table::seq_field
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__fields[] = {
        {offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_, nr_of_ant_ports),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::desc}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__names[] = {
        {"nrOfAntennaPorts", nullptr}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s_ = {
        {seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__fields,
         1,
         UINT64_C(0x1),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__names};
static const asn1::table::seq_field
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__fields[] = {
        {offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_, nr_of_ant_ports),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::desc}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__names[] = {
        {"nrOfAntennaPorts", nullptr}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s_ = {
        {seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__fields,
         1,
         UINT64_C(0x1),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__names};
static const asn1::table::seq_field seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__fields[] = {
    {offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_, type_i_single_panel_group1_r17),
     (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_, type_i_single_panel_group1_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s_},
    {offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_, type_i_single_panel_group2_r17),
     (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_, type_i_single_panel_group2_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s_},
    {offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_, type_i_single_panel_ri_restrict_strp_r17),
     (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_,
                       type_i_single_panel_ri_restrict_strp_r17_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_ri_restrict_strp_r17),
         asn1::table::struct_json>::value},
    {offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_, type_i_single_panel_ri_restrict_sdm_r17),
     (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_, type_i_single_panel_ri_restrict_sdm_r17_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_ri_restrict_sdm_r17),
         asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__names[] = {
    {"typeI-SinglePanel-Group1-r17", nullptr},
    {"typeI-SinglePanel-Group2-r17", nullptr},
    {"typeI-SinglePanel-ri-RestrictionSTRP-r17", nullptr},
    {"typeI-SinglePanel-ri-RestrictionSDM-r17", nullptr}};
static const asn1::table::seq_opt seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__opts[] = {
    {0, (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_, type_i_single_panel_group1_r17_present)},
    {1, (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_, type_i_single_panel_group2_r17_present)},
    {2,
     (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_,
                       type_i_single_panel_ri_restrict_strp_r17_present)},
    {3,
     (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_,
                       type_i_single_panel_ri_restrict_sdm_r17_present)}};
static const asn1::table::seq_desc seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s_ = {
    {seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__fields,
     4,
     UINT64_C(0x0),
     seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__opts,
     4},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__names};
static const asn1::table::seq_field
    seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__type_ii_port_sel_r17_s__fields[] = {
        {offsetof(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_, param_combination_r17),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(8),
         nullptr},
        {offsetof(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_, value_of_n_r17),
         (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_,
                           value_of_n_r17_present),
         asn1::table::enum_op<
             decltype(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17)>(),
         decltype(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17)::has_ext,
         false,
         decltype(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17)::nof_types,
         decltype(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17)::nof_exts,
         nullptr,
         &codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17_opts::desc},
        {offsetof(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_,
                  nof_pmi_subbands_per_cqi_subband_r17),
         (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_,
                           nof_pmi_subbands_per_cqi_subband_r17_present),
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(2),
         nullptr},
        {offsetof(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_,
                  type_ii_port_sel_ri_restrict_r17),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::
                                                 type_ii_port_sel_ri_restrict_r17),
                                    asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__type_ii_port_sel_r17_s__names[] = {
        {"paramCombination-r17", nullptr},
        {"valueOfN-r17", nullptr},
        {"numberOfPMI-SubbandsPerCQI-Subband-r17", nullptr},
        {"typeII-PortSelectionRI-Restriction-r17", nullptr}};
static const asn1::table::seq_opt
    seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__type_ii_port_sel_r17_s__opts[] = {
        {1,
         (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_,
                           value_of_n_r17_present)},
        {2,
         (int32_t)offsetof(codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_,
                           nof_pmi_subbands_per_cqi_subband_r17_present)}};
static const asn1::table::seq_desc seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__type_ii_port_sel_r17_s_ = {
    {seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__type_ii_port_sel_r17_s__fields,
     4,
     UINT64_C(0x9),
     seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__type_ii_port_sel_r17_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__type_ii_port_sel_r17_s__names};
static const asn1::table::seq_field seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__fields[] = {
    {offsetof(codebook_cfg_r17_s::codebook_type_c_::type2_s_, type_ii_port_sel_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__type_ii_port_sel_r17_s_}};
static const asn1::table::field_name seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__names[] = {
    {"typeII-PortSelection-r17", nullptr}};
static const asn1::table::seq_desc seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s_ = {
    {seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s__names};
static const asn1::table::seq_field choice_table_codebook_cfg_r17_s_codebook_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_r17_s_codebook_type_c__type2_s_}};
static const asn1::table::field_name choice_table_codebook_cfg_r17_s_codebook_type_c__names[] = {{"type1", nullptr},
                                                                                                 {"type2", nullptr}};
const asn1::table::choice_desc       codebook_cfg_r17_s::codebook_type_c_::desc               = {
    asn1::table::enum_int_type<codebook_cfg_r17_s::codebook_type_c_::types>(),
    codebook_cfg_r17_s::codebook_type_c_::types::nof_types,
    codebook_cfg_r17_s::codebook_type_c_::types::nof_exts,
    codebook_cfg_r17_s::codebook_type_c_::types::has_ext,
    false,
    choice_table_codebook_cfg_r17_s_codebook_type_c__alternatives,
    nullptr,
    choice_table_codebook_cfg_r17_s_codebook_type_c__names,
    "codebook_cfg_r17_s::codebook_type_c_",
    offsetof(codebook_cfg_r17_s::codebook_type_c_, c.type),
    offsetof(codebook_cfg_r17_s::codebook_type_c_, c.value),
    false,
    &codebook_cfg_r17_s::codebook_type_c_::storage_desc};

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__alternatives
        [] = {&asn1::table::alternative_ops<codebook_cfg_r17_s::codebook_type_c_::type1_s_::
                                                type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::two_s_,
                                            false>::value,
              &asn1::table::alternative_ops<codebook_cfg_r17_s::codebook_type_c_::type1_s_::
                                                type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::more_than_two_s_,
                                            false>::value};
const asn1::table::choice_storage_desc codebook_cfg_r17_s::codebook_type_c_::type1_s_::
    type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::storage_desc = {
        choice_storage_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__alternatives,
        2,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_"};

codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::two_s_&
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::set_two()
{
  set(types::two);
  return c.get<two_s_>();
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::more_than_two_s_&
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    set_more_than_two()
{
  set(types::more_than_two);
  return c.get<more_than_two_s_>();
}
static const asn1::table::seq_field
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__two_s__fields
        [] = {{offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
                            nr_of_ant_ports_c_::two_s_,
                        two_tx_codebook_subset_restrict1_r17),
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<
                   decltype(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
                                nr_of_ant_ports_c_::two_s_::two_tx_codebook_subset_restrict1_r17),
                   asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__two_s__names
        [] = {{"twoTX-CodebookSubsetRestriction1-r17", nullptr}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__two_s_ = {
        {seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__two_s__fields,
         1,
         UINT64_C(0x1),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__two_s__names};
static const asn1::table::seq_field
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__fields
        [] = {{offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
                            nr_of_ant_ports_c_::more_than_two_s_,
                        n1_n2),
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::choice,
               false,
               false,
               0,
               0,
               nullptr,
               &codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
                   more_than_two_s_::n1_n2_c_::desc}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__names
        [] = {{"n1-n2", nullptr}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s_ =
        {{seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__fields,
          1,
          UINT64_C(0x1),
          nullptr,
          0},
         false,
         false,
         nullptr,
         0,
         seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__names};
static const asn1::table::seq_field
    choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__alternatives
        [] = {
            {0,
             asn1::table::seq_field::mandatory,
             asn1::table::field_op::sequence,
             false,
             false,
             0,
             0,
             nullptr,
             &seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__two_s_},
            {0,
             asn1::table::seq_field::mandatory,
             asn1::table::field_op::sequence,
             false,
             false,
             0,
             0,
             nullptr,
             &seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s_}};
static const asn1::table::field_name
    choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__names
        [] = {{"two", nullptr}, {"moreThanTwo", nullptr}};
const asn1::table::choice_desc
    codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::desc = {
        asn1::table::enum_int_type<codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
                                       nr_of_ant_ports_c_::types>(),
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::types::
            nof_types,
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::types::
            nof_exts,
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::types::
            has_ext,
        false,
        choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__alternatives,
        nullptr,
        choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__names,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_",
        offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_,
                 c.type),
        offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_,
                 c.value),
        false,
        &codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
            storage_desc};

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives
        [] = {&asn1::table::alternative_ops<fixed_bitstring<8>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<16>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<96>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<24>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<128>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<32>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<192>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<192>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<48>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<256>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<256>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value};
const asn1::table::choice_storage_desc codebook_cfg_r17_s::codebook_type_c_::type1_s_::
    type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::storage_desc = {
        choice_storage_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives,
        13,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::more_"
        "than_two_s_::n1_n2_c_"};

fixed_bitstring<8>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_one_type_i_single_panel_restrict1_r17()
{
  set(types::two_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_two_type_i_single_panel_restrict1_r17()
{
  set(types::two_two_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<16>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_one_type_i_single_panel_restrict1_r17()
{
  set(types::four_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<96>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_three_two_type_i_single_panel_restrict1_r17()
{
  set(types::three_two_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<96>>();
}
fixed_bitstring<24>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_one_type_i_single_panel_restrict1_r17()
{
  set(types::six_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<24>>();
}
fixed_bitstring<128>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_two_type_i_single_panel_restrict1_r17()
{
  set(types::four_two_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<128>>();
}
fixed_bitstring<32>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_one_type_i_single_panel_restrict1_r17()
{
  set(types::eight_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<192>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_three_type_i_single_panel_restrict1_r17()
{
  set(types::four_three_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<192>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_two_type_i_single_panel_restrict1_r17()
{
  set(types::six_two_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<48>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_twelve_one_type_i_single_panel_restrict1_r17()
{
  set(types::twelve_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<256>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_four_type_i_single_panel_restrict1_r17()
{
  set(types::four_four_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<256>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_two_type_i_single_panel_restrict1_r17()
{
  set(types::eight_two_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<64>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_sixteen_one_type_i_single_panel_restrict1_r17()
{
  set(types::sixteen_one_type_i_single_panel_restrict1_r17);
  return c.get<fixed_bitstring<64>>();
}
static const asn1::table::seq_field
    choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives
        [] = {{0,
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
               &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<16>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<96>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<24>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<128>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<32>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<192>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<192>, asn1::table::struct_json>::value},
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
               &asn1::table::object_codec<fixed_bitstring<256>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<256>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value}};
static const asn1::table::field_name
    choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__names
        [] = {{"two-one-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"two-two-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"four-one-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"three-two-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"six-one-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"four-two-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"eight-one-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"four-three-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"six-two-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"twelve-one-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"four-four-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"eight-two-TypeI-SinglePanel-Restriction1-r17", nullptr},
              {"sixteen-one-TypeI-SinglePanel-Restriction1-r17", nullptr}};
const asn1::table::choice_desc codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::desc = {
        asn1::table::enum_int_type<codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
                                       nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::types>(),
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::types::nof_types,
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::types::nof_exts,
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::types::has_ext,
        false,
        choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives,
        nullptr,
        choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__names,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::more_"
        "than_two_s_::n1_n2_c_",
        offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
                     more_than_two_s_::n1_n2_c_,
                 c.type),
        offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
                     more_than_two_s_::n1_n2_c_,
                 c.value),
        false,
        &codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::storage_desc};

static const char*
    enum_names_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__types_values
        [] = {"two-one-TypeI-SinglePanel-Restriction1-r17",
              "two-two-TypeI-SinglePanel-Restriction1-r17",
              "four-one-TypeI-SinglePanel-Restriction1-r17",
              "three-two-TypeI-SinglePanel-Restriction1-r17",
              "six-one-TypeI-SinglePanel-Restriction1-r17",
              "four-two-TypeI-SinglePanel-Restriction1-r17",
              "eight-one-TypeI-SinglePanel-Restriction1-r17",
              "four-three-TypeI-SinglePanel-Restriction1-r17",
              "six-two-TypeI-SinglePanel-Restriction1-r17",
              "twelve-one-TypeI-SinglePanel-Restriction1-r17",
              "four-four-TypeI-SinglePanel-Restriction1-r17",
              "eight-two-TypeI-SinglePanel-Restriction1-r17",
              "sixteen-one-TypeI-SinglePanel-Restriction1-r17"};
const asn1::table::enum_desc codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::types_opts::desc = {
        enum_names_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__types_values,
        13,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::more_"
        "than_two_s_::n1_n2_c_::types"};

static const char*
    enum_names_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__types_values
        [] = {"two", "moreThanTwo"};
const asn1::table::enum_desc codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::
    nr_of_ant_ports_c_::types_opts::desc = {
        enum_names_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group1_r17_s__nr_of_ant_ports_c__types_values,
        2,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::types"};
uint8_t codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::
    types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(
      numbers,
      1,
      value,
      "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group1_r17_s_::nr_of_ant_ports_c_::types");
}

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__alternatives
        [] = {&asn1::table::alternative_ops<codebook_cfg_r17_s::codebook_type_c_::type1_s_::
                                                type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::two_s_,
                                            false>::value,
              &asn1::table::alternative_ops<codebook_cfg_r17_s::codebook_type_c_::type1_s_::
                                                type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::more_than_two_s_,
                                            false>::value};
const asn1::table::choice_storage_desc codebook_cfg_r17_s::codebook_type_c_::type1_s_::
    type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::storage_desc = {
        choice_storage_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__alternatives,
        2,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_"};

codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::two_s_&
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::set_two()
{
  set(types::two);
  return c.get<two_s_>();
}
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::more_than_two_s_&
codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    set_more_than_two()
{
  set(types::more_than_two);
  return c.get<more_than_two_s_>();
}
static const asn1::table::seq_field
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__two_s__fields
        [] = {{offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
                            nr_of_ant_ports_c_::two_s_,
                        two_tx_codebook_subset_restrict2_r17),
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<
                   decltype(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
                                nr_of_ant_ports_c_::two_s_::two_tx_codebook_subset_restrict2_r17),
                   asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__two_s__names
        [] = {{"twoTX-CodebookSubsetRestriction2-r17", nullptr}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__two_s_ = {
        {seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__two_s__fields,
         1,
         UINT64_C(0x1),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__two_s__names};
static const asn1::table::seq_field
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__fields
        [] = {{offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
                            nr_of_ant_ports_c_::more_than_two_s_,
                        n1_n2),
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::choice,
               false,
               false,
               0,
               0,
               nullptr,
               &codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
                   more_than_two_s_::n1_n2_c_::desc}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__names
        [] = {{"n1-n2", nullptr}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s_ =
        {{seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__fields,
          1,
          UINT64_C(0x1),
          nullptr,
          0},
         false,
         false,
         nullptr,
         0,
         seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__names};
static const asn1::table::seq_field
    choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__alternatives
        [] = {
            {0,
             asn1::table::seq_field::mandatory,
             asn1::table::field_op::sequence,
             false,
             false,
             0,
             0,
             nullptr,
             &seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__two_s_},
            {0,
             asn1::table::seq_field::mandatory,
             asn1::table::field_op::sequence,
             false,
             false,
             0,
             0,
             nullptr,
             &seq_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s_}};
static const asn1::table::field_name
    choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__names
        [] = {{"two", nullptr}, {"moreThanTwo", nullptr}};
const asn1::table::choice_desc
    codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::desc = {
        asn1::table::enum_int_type<codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
                                       nr_of_ant_ports_c_::types>(),
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::types::
            nof_types,
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::types::
            nof_exts,
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::types::
            has_ext,
        false,
        choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__alternatives,
        nullptr,
        choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__names,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_",
        offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_,
                 c.type),
        offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_,
                 c.value),
        false,
        &codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
            storage_desc};

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives
        [] = {&asn1::table::alternative_ops<fixed_bitstring<8>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<16>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<96>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<24>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<128>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<32>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<192>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<192>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<48>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<256>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<256>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value};
const asn1::table::choice_storage_desc codebook_cfg_r17_s::codebook_type_c_::type1_s_::
    type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::storage_desc = {
        choice_storage_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives,
        13,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::more_"
        "than_two_s_::n1_n2_c_"};

fixed_bitstring<8>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_one_type_i_single_panel_restrict2_r17()
{
  set(types::two_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_two_two_type_i_single_panel_restrict2_r17()
{
  set(types::two_two_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<64>>();
}
fixed_bitstring<16>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_one_type_i_single_panel_restrict2_r17()
{
  set(types::four_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<96>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_three_two_type_i_single_panel_restrict2_r17()
{
  set(types::three_two_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<96>>();
}
fixed_bitstring<24>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_one_type_i_single_panel_restrict2_r17()
{
  set(types::six_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<24>>();
}
fixed_bitstring<128>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_two_type_i_single_panel_restrict2_r17()
{
  set(types::four_two_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<128>>();
}
fixed_bitstring<32>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_one_type_i_single_panel_restrict2_r17()
{
  set(types::eight_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<32>>();
}
fixed_bitstring<192>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_three_type_i_single_panel_restrict2_r17()
{
  set(types::four_three_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<192>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_six_two_type_i_single_panel_restrict2_r17()
{
  set(types::six_two_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<192>>();
}
fixed_bitstring<48>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_twelve_one_type_i_single_panel_restrict2_r17()
{
  set(types::twelve_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<48>>();
}
fixed_bitstring<256>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_four_four_type_i_single_panel_restrict2_r17()
{
  set(types::four_four_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<256>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_eight_two_type_i_single_panel_restrict2_r17()
{
  set(types::eight_two_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<256>>();
}
fixed_bitstring<64>& codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::set_sixteen_one_type_i_single_panel_restrict2_r17()
{
  set(types::sixteen_one_type_i_single_panel_restrict2_r17);
  return c.get<fixed_bitstring<64>>();
}
static const asn1::table::seq_field
    choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives
        [] = {{0,
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
               &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<16>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<96>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<24>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<128>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<32>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<192>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<192>, asn1::table::struct_json>::value},
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
               &asn1::table::object_codec<fixed_bitstring<256>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<256>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value}};
static const asn1::table::field_name
    choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__names
        [] = {{"two-one-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"two-two-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"four-one-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"three-two-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"six-one-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"four-two-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"eight-one-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"four-three-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"six-two-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"twelve-one-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"four-four-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"eight-two-TypeI-SinglePanel-Restriction2-r17", nullptr},
              {"sixteen-one-TypeI-SinglePanel-Restriction2-r17", nullptr}};
const asn1::table::choice_desc codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::desc = {
        asn1::table::enum_int_type<codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
                                       nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::types>(),
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::types::nof_types,
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::types::nof_exts,
        codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::types::has_ext,
        false,
        choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__alternatives,
        nullptr,
        choice_table_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__names,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::more_"
        "than_two_s_::n1_n2_c_",
        offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
                     more_than_two_s_::n1_n2_c_,
                 c.type),
        offsetof(codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
                     more_than_two_s_::n1_n2_c_,
                 c.value),
        false,
        &codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
            more_than_two_s_::n1_n2_c_::storage_desc};

static const char*
    enum_names_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__types_values
        [] = {"two-one-TypeI-SinglePanel-Restriction2-r17",
              "two-two-TypeI-SinglePanel-Restriction2-r17",
              "four-one-TypeI-SinglePanel-Restriction2-r17",
              "three-two-TypeI-SinglePanel-Restriction2-r17",
              "six-one-TypeI-SinglePanel-Restriction2-r17",
              "four-two-TypeI-SinglePanel-Restriction2-r17",
              "eight-one-TypeI-SinglePanel-Restriction2-r17",
              "four-three-TypeI-SinglePanel-Restriction2-r17",
              "six-two-TypeI-SinglePanel-Restriction2-r17",
              "twelve-one-TypeI-SinglePanel-Restriction2-r17",
              "four-four-TypeI-SinglePanel-Restriction2-r17",
              "eight-two-TypeI-SinglePanel-Restriction2-r17",
              "sixteen-one-TypeI-SinglePanel-Restriction2-r17"};
const asn1::table::enum_desc codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::more_than_two_s_::n1_n2_c_::types_opts::desc = {
        enum_names_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__more_than_two_s__n1_n2_c__types_values,
        13,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::more_"
        "than_two_s_::n1_n2_c_::types"};

static const char*
    enum_names_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__types_values
        [] = {"two", "moreThanTwo"};
const asn1::table::enum_desc codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::
    nr_of_ant_ports_c_::types_opts::desc = {
        enum_names_codebook_cfg_r17_s_codebook_type_c__type1_s__type_i_single_panel_group2_r17_s__nr_of_ant_ports_c__types_values,
        2,
        "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::types"};
uint8_t codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::
    types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(
      numbers,
      1,
      value,
      "codebook_cfg_r17_s::codebook_type_c_::type1_s_::type_i_single_panel_group2_r17_s_::nr_of_ant_ports_c_::types");
}

static const char*
    enum_names_codebook_cfg_r17_s_codebook_type_c__type2_s__type_ii_port_sel_r17_s__value_of_n_r17_e__values[] = {"n2",
                                                                                                                  "n4"};
const asn1::table::enum_desc
    codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17_opts::desc = {
        enum_names_codebook_cfg_r17_s_codebook_type_c__type2_s__type_ii_port_sel_r17_s__value_of_n_r17_e__values,
        2,
        "codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17_e_"};
uint8_t codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(
      numbers, 2, value, "codebook_cfg_r17_s::codebook_type_c_::type2_s_::type_ii_port_sel_r17_s_::value_of_n_r17_e_");
}

static const char*           enum_names_codebook_cfg_r17_s_codebook_type_c__types_values[] = {"type1", "type2"};
const asn1::table::enum_desc codebook_cfg_r17_s::codebook_type_c_::types_opts::desc        = {
    enum_names_codebook_cfg_r17_s_codebook_type_c__types_values,
    2,
    "codebook_cfg_r17_s::codebook_type_c_::types"};
uint8_t codebook_cfg_r17_s::codebook_type_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "codebook_cfg_r17_s::codebook_type_c_::types");
}

// CodebookConfig-v1730 ::= SEQUENCE
static const asn1::table::seq_field seq_table_codebook_cfg_v1730_s_fields[] = {
    {offsetof(codebook_cfg_v1730_s, codebook_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &codebook_cfg_v1730_s::codebook_type_c_::desc}};
static const asn1::table::field_name seq_table_codebook_cfg_v1730_s_names[] = {{"codebookType", nullptr}};
const asn1::table::seq_desc          codebook_cfg_v1730_s::desc             = {
    {seq_table_codebook_cfg_v1730_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_v1730_s_names};

static const asn1::table::seq_field seq_table_codebook_cfg_v1730_s_codebook_type_c__type1_s__fields[] = {
    {offsetof(codebook_cfg_v1730_s::codebook_type_c_::type1_s_, codebook_mode),
     (int32_t)offsetof(codebook_cfg_v1730_s::codebook_type_c_::type1_s_, codebook_mode_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(2),
     nullptr}};
static const asn1::table::field_name seq_table_codebook_cfg_v1730_s_codebook_type_c__type1_s__names[] = {
    {"codebookMode", nullptr}};
static const asn1::table::seq_opt seq_table_codebook_cfg_v1730_s_codebook_type_c__type1_s__opts[] = {
    {0, (int32_t)offsetof(codebook_cfg_v1730_s::codebook_type_c_::type1_s_, codebook_mode_present)}};
static const asn1::table::seq_desc seq_table_codebook_cfg_v1730_s_codebook_type_c__type1_s_ = {
    {seq_table_codebook_cfg_v1730_s_codebook_type_c__type1_s__fields,
     1,
     UINT64_C(0x0),
     seq_table_codebook_cfg_v1730_s_codebook_type_c__type1_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_v1730_s_codebook_type_c__type1_s__names};
static const asn1::table::seq_field choice_table_codebook_cfg_v1730_s_codebook_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_v1730_s_codebook_type_c__type1_s_}};
static const asn1::table::field_name choice_table_codebook_cfg_v1730_s_codebook_type_c__names[] = {{"type1", nullptr}};
const asn1::table::choice_desc       codebook_cfg_v1730_s::codebook_type_c_::desc               = {
    asn1::table::enum_int_type<codebook_cfg_v1730_s::codebook_type_c_::types>(),
    codebook_cfg_v1730_s::codebook_type_c_::types::nof_types,
    codebook_cfg_v1730_s::codebook_type_c_::types::nof_exts,
    codebook_cfg_v1730_s::codebook_type_c_::types::has_ext,
    false,
    choice_table_codebook_cfg_v1730_s_codebook_type_c__alternatives,
    nullptr,
    choice_table_codebook_cfg_v1730_s_codebook_type_c__names,
    "codebook_cfg_v1730_s::codebook_type_c_",
    asn1::table::choice_desc::no_member,
    offsetof(codebook_cfg_v1730_s::codebook_type_c_, c),
    false};

static const char*           enum_names_codebook_cfg_v1730_s_codebook_type_c__types_values[] = {"type1"};
const asn1::table::enum_desc codebook_cfg_v1730_s::codebook_type_c_::types_opts::desc        = {
    enum_names_codebook_cfg_v1730_s_codebook_type_c__types_values,
    1,
    "codebook_cfg_v1730_s::codebook_type_c_::types"};
uint8_t codebook_cfg_v1730_s::codebook_type_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "codebook_cfg_v1730_s::codebook_type_c_::types");
}

// DelayD ::= ENUMERATED
static const char* enum_names_delay_d_e_values[] =
    {"symb4", "slot1", "slot2", "slot3", "slot4", "slot5", "slot6", "slot10"};
const asn1::table::enum_desc delay_d_opts::desc = {enum_names_delay_d_e_values, 8, "delay_d_e"};

// TDCP-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_td_cp_r18_s_fields[] = {
    {offsetof(td_cp_r18_s, delay_d_setof_len_y_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<decltype(td_cp_r18_s::delay_d_setof_len_y_r18), asn1::table::string_json>::value},
    {0,
     (int32_t)offsetof(td_cp_r18_s, phase_report_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_td_cp_r18_s_names[] = {{"delayDSetofLengthY-r18", nullptr},
                                                                      {"phaseReporting-r18", "enable"}};
static const asn1::table::seq_opt    seq_table_td_cp_r18_s_opts[]  = {
    {1, (int32_t)offsetof(td_cp_r18_s, phase_report_r18_present)}};
const asn1::table::seq_desc td_cp_r18_s::desc = {
    {seq_table_td_cp_r18_s_fields, 2, UINT64_C(0x1), seq_table_td_cp_r18_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_td_cp_r18_s_names};

// N1-N2-CBSR-List-r18 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_n1_n2_cbsr_list_r18_c_alternatives[] = {
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::two_one_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::two_two_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::four_one_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::three_two_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::six_one_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::four_two_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::eight_one_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::four_three_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::twelve_one_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::four_four_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::eight_two_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_, false>::value};
const asn1::table::choice_storage_desc n1_n2_cbsr_list_r18_c::storage_desc = {
    choice_storage_n1_n2_cbsr_list_r18_c_alternatives,
    12,
    "n1_n2_cbsr_list_r18_c"};

n1_n2_cbsr_list_r18_c::two_one_r18_c_& n1_n2_cbsr_list_r18_c::set_two_one_r18()
{
  set(types::two_one_r18);
  return c.get<two_one_r18_c_>();
}
n1_n2_cbsr_list_r18_c::two_two_r18_c_& n1_n2_cbsr_list_r18_c::set_two_two_r18()
{
  set(types::two_two_r18);
  return c.get<two_two_r18_c_>();
}
n1_n2_cbsr_list_r18_c::four_one_r18_c_& n1_n2_cbsr_list_r18_c::set_four_one_r18()
{
  set(types::four_one_r18);
  return c.get<four_one_r18_c_>();
}
n1_n2_cbsr_list_r18_c::three_two_r18_c_& n1_n2_cbsr_list_r18_c::set_three_two_r18()
{
  set(types::three_two_r18);
  return c.get<three_two_r18_c_>();
}
n1_n2_cbsr_list_r18_c::six_one_r18_c_& n1_n2_cbsr_list_r18_c::set_six_one_r18()
{
  set(types::six_one_r18);
  return c.get<six_one_r18_c_>();
}
n1_n2_cbsr_list_r18_c::four_two_r18_c_& n1_n2_cbsr_list_r18_c::set_four_two_r18()
{
  set(types::four_two_r18);
  return c.get<four_two_r18_c_>();
}
n1_n2_cbsr_list_r18_c::eight_one_r18_c_& n1_n2_cbsr_list_r18_c::set_eight_one_r18()
{
  set(types::eight_one_r18);
  return c.get<eight_one_r18_c_>();
}
n1_n2_cbsr_list_r18_c::four_three_r18_c_& n1_n2_cbsr_list_r18_c::set_four_three_r18()
{
  set(types::four_three_r18);
  return c.get<four_three_r18_c_>();
}
n1_n2_cbsr_list_r18_c::twelve_one_r18_c_& n1_n2_cbsr_list_r18_c::set_twelve_one_r18()
{
  set(types::twelve_one_r18);
  return c.get<twelve_one_r18_c_>();
}
n1_n2_cbsr_list_r18_c::four_four_r18_c_& n1_n2_cbsr_list_r18_c::set_four_four_r18()
{
  set(types::four_four_r18);
  return c.get<four_four_r18_c_>();
}
n1_n2_cbsr_list_r18_c::eight_two_r18_c_& n1_n2_cbsr_list_r18_c::set_eight_two_r18()
{
  set(types::eight_two_r18);
  return c.get<eight_two_r18_c_>();
}
n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_& n1_n2_cbsr_list_r18_c::set_sixteen_one_r18()
{
  set(types::sixteen_one_r18);
  return c.get<sixteen_one_r18_c_>();
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::two_one_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::two_two_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::four_one_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::three_two_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::six_one_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::four_two_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::eight_one_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::four_three_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::four_four_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::eight_two_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::desc}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_names[] = {{"two-one-r18", nullptr},
                                                                                   {"two-two-r18", nullptr},
                                                                                   {"four-one-r18", nullptr},
                                                                                   {"three-two-r18", nullptr},
                                                                                   {"six-one-r18", nullptr},
                                                                                   {"four-two-r18", nullptr},
                                                                                   {"eight-one-r18", nullptr},
                                                                                   {"four-three-r18", nullptr},
                                                                                   {"twelve-one-r18", nullptr},
                                                                                   {"four-four-r18", nullptr},
                                                                                   {"eight-two-r18", nullptr},
                                                                                   {"sixteen-one-r18", nullptr}};
const asn1::table::choice_desc       n1_n2_cbsr_list_r18_c::desc                = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::types>(),
    n1_n2_cbsr_list_r18_c::types::nof_types,
    n1_n2_cbsr_list_r18_c::types::nof_exts,
    n1_n2_cbsr_list_r18_c::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_alternatives,
    nullptr,
    choice_table_n1_n2_cbsr_list_r18_c_names,
    "n1_n2_cbsr_list_r18_c",
    offsetof(n1_n2_cbsr_list_r18_c, c.type),
    offsetof(n1_n2_cbsr_list_r18_c, c.value),
    false,
    &n1_n2_cbsr_list_r18_c::storage_desc};

void n1_n2_cbsr_list_r18_c::two_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::two_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::two_one_r18_c_::cbsr_list_r18_l_& n1_n2_cbsr_list_r18_c::two_one_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_two_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::two_one_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_two_one_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::two_one_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::two_one_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::two_one_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::two_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::two_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_two_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::two_one_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_two_one_r18_c__names,
    "n1_n2_cbsr_list_r18_c::two_one_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::two_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::two_one_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_list_r18_c_two_one_r18_c__types_values[]    = {"no-cbsr-r18", "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::two_one_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_list_r18_c_two_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::two_one_r18_c_::types"};

void n1_n2_cbsr_list_r18_c::two_two_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::two_two_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::two_two_r18_c_::cbsr_list_r18_l_& n1_n2_cbsr_list_r18_c::two_two_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_two_two_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::two_two_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_two_two_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::two_two_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::two_two_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::two_two_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::two_two_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::two_two_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_two_two_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::two_two_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_two_two_r18_c__names,
    "n1_n2_cbsr_list_r18_c::two_two_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::two_two_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::two_two_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_list_r18_c_two_two_r18_c__types_values[]    = {"no-cbsr-r18", "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::two_two_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_list_r18_c_two_two_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::two_two_r18_c_::types"};

void n1_n2_cbsr_list_r18_c::four_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::four_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::four_one_r18_c_::cbsr_list_r18_l_& n1_n2_cbsr_list_r18_c::four_one_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_four_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::four_one_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_four_one_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::four_one_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::four_one_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::four_one_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::four_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::four_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_four_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::four_one_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_four_one_r18_c__names,
    "n1_n2_cbsr_list_r18_c::four_one_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::four_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::four_one_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_list_r18_c_four_one_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::four_one_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_list_r18_c_four_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::four_one_r18_c_::types"};

void n1_n2_cbsr_list_r18_c::three_two_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::three_two_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::three_two_r18_c_::cbsr_list_r18_l_& n1_n2_cbsr_list_r18_c::three_two_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_three_two_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::three_two_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_three_two_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::three_two_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::three_two_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::three_two_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::three_two_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::three_two_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_three_two_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::three_two_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_three_two_r18_c__names,
    "n1_n2_cbsr_list_r18_c::three_two_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::three_two_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::three_two_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_list_r18_c_three_two_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::three_two_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_list_r18_c_three_two_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::three_two_r18_c_::types"};

void n1_n2_cbsr_list_r18_c::six_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::six_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::six_one_r18_c_::cbsr_list_r18_l_& n1_n2_cbsr_list_r18_c::six_one_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_six_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::six_one_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_six_one_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::six_one_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::six_one_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::six_one_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::six_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::six_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_six_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::six_one_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_six_one_r18_c__names,
    "n1_n2_cbsr_list_r18_c::six_one_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::six_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::six_one_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_list_r18_c_six_one_r18_c__types_values[]    = {"no-cbsr-r18", "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::six_one_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_list_r18_c_six_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::six_one_r18_c_::types"};

void n1_n2_cbsr_list_r18_c::four_two_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::four_two_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::four_two_r18_c_::cbsr_list_r18_l_& n1_n2_cbsr_list_r18_c::four_two_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_four_two_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::four_two_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_four_two_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::four_two_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::four_two_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::four_two_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::four_two_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::four_two_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_four_two_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::four_two_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_four_two_r18_c__names,
    "n1_n2_cbsr_list_r18_c::four_two_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::four_two_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::four_two_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_list_r18_c_four_two_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::four_two_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_list_r18_c_four_two_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::four_two_r18_c_::types"};

void n1_n2_cbsr_list_r18_c::eight_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::eight_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::eight_one_r18_c_::cbsr_list_r18_l_& n1_n2_cbsr_list_r18_c::eight_one_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_eight_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::eight_one_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_eight_one_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::eight_one_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::eight_one_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::eight_one_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::eight_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::eight_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_eight_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::eight_one_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_eight_one_r18_c__names,
    "n1_n2_cbsr_list_r18_c::eight_one_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::eight_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::eight_one_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_list_r18_c_eight_one_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::eight_one_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_list_r18_c_eight_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::eight_one_r18_c_::types"};

void n1_n2_cbsr_list_r18_c::four_three_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::four_three_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::four_three_r18_c_::cbsr_list_r18_l_&
n1_n2_cbsr_list_r18_c::four_three_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_four_three_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::four_three_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_four_three_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::four_three_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::four_three_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::four_three_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::four_three_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::four_three_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_four_three_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::four_three_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_four_three_r18_c__names,
    "n1_n2_cbsr_list_r18_c::four_three_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::four_three_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::four_three_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_list_r18_c_four_three_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::four_three_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_list_r18_c_four_three_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::four_three_r18_c_::types"};

void n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::cbsr_list_r18_l_&
n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_twelve_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_twelve_one_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_twelve_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::twelve_one_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_twelve_one_r18_c__names,
    "n1_n2_cbsr_list_r18_c::twelve_one_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::twelve_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::twelve_one_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_list_r18_c_twelve_one_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_list_r18_c_twelve_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::twelve_one_r18_c_::types"};

void n1_n2_cbsr_list_r18_c::four_four_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::four_four_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::four_four_r18_c_::cbsr_list_r18_l_& n1_n2_cbsr_list_r18_c::four_four_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_four_four_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::four_four_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_four_four_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::four_four_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::four_four_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::four_four_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::four_four_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::four_four_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_four_four_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::four_four_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_four_four_r18_c__names,
    "n1_n2_cbsr_list_r18_c::four_four_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::four_four_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::four_four_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_list_r18_c_four_four_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::four_four_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_list_r18_c_four_four_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::four_four_r18_c_::types"};

void n1_n2_cbsr_list_r18_c::eight_two_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::eight_two_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::eight_two_r18_c_::cbsr_list_r18_l_& n1_n2_cbsr_list_r18_c::eight_two_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_eight_two_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::eight_two_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_eight_two_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::eight_two_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::eight_two_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::eight_two_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::eight_two_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::eight_two_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_eight_two_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::eight_two_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_eight_two_r18_c__names,
    "n1_n2_cbsr_list_r18_c::eight_two_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::eight_two_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::eight_two_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_list_r18_c_eight_two_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::eight_two_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_list_r18_c_eight_two_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::eight_two_r18_c_::types"};

void n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::cbsr_list_r18_l_&
n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::set_cbsr_list_r18()
{
  set(types::cbsr_list_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_list_r18_c_sixteen_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::cbsr_list_r18_l_,
                              asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_list_r18_c_sixteen_one_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-list-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::types>(),
    n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::types::nof_types,
    n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_list_r18_c_sixteen_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_>,
    choice_table_n1_n2_cbsr_list_r18_c_sixteen_one_r18_c__names,
    "n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_",
    offsetof(n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_list_r18_c_sixteen_one_r18_c__types_values[] = {"no-cbsr-r18",
                                                                                                   "cbsr-list-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_list_r18_c_sixteen_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_list_r18_c::sixteen_one_r18_c_::types"};

static const char*           enum_names_n1_n2_cbsr_list_r18_c_types_values[] = {"two-one-r18",
                                                                                "two-two-r18",
                                                                                "four-one-r18",
                                                                                "three-two-r18",
                                                                                "six-one-r18",
                                                                                "four-two-r18",
                                                                                "eight-one-r18",
                                                                                "four-three-r18",
                                                                                "twelve-one-r18",
                                                                                "four-four-r18",
                                                                                "eight-two-r18",
                                                                                "sixteen-one-r18"};
const asn1::table::enum_desc n1_n2_cbsr_list_r18_c::types_opts::desc = {enum_names_n1_n2_cbsr_list_r18_c_types_values,
                                                                        12,
                                                                        "n1_n2_cbsr_list_r18_c::types"};

// N1-N2-CBSR-r18 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_n1_n2_cbsr_r18_c_alternatives[] = {
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::two_one_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::two_two_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::four_one_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::three_two_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::six_one_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::four_two_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::eight_one_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::four_three_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::twelve_one_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::four_four_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::eight_two_r18_c_, false>::value,
    &asn1::table::alternative_ops<n1_n2_cbsr_r18_c::sixteen_one_r18_c_, false>::value};
const asn1::table::choice_storage_desc n1_n2_cbsr_r18_c::storage_desc = {choice_storage_n1_n2_cbsr_r18_c_alternatives,
                                                                         12,
                                                                         "n1_n2_cbsr_r18_c"};

n1_n2_cbsr_r18_c::two_one_r18_c_& n1_n2_cbsr_r18_c::set_two_one_r18()
{
  set(types::two_one_r18);
  return c.get<two_one_r18_c_>();
}
n1_n2_cbsr_r18_c::two_two_r18_c_& n1_n2_cbsr_r18_c::set_two_two_r18()
{
  set(types::two_two_r18);
  return c.get<two_two_r18_c_>();
}
n1_n2_cbsr_r18_c::four_one_r18_c_& n1_n2_cbsr_r18_c::set_four_one_r18()
{
  set(types::four_one_r18);
  return c.get<four_one_r18_c_>();
}
n1_n2_cbsr_r18_c::three_two_r18_c_& n1_n2_cbsr_r18_c::set_three_two_r18()
{
  set(types::three_two_r18);
  return c.get<three_two_r18_c_>();
}
n1_n2_cbsr_r18_c::six_one_r18_c_& n1_n2_cbsr_r18_c::set_six_one_r18()
{
  set(types::six_one_r18);
  return c.get<six_one_r18_c_>();
}
n1_n2_cbsr_r18_c::four_two_r18_c_& n1_n2_cbsr_r18_c::set_four_two_r18()
{
  set(types::four_two_r18);
  return c.get<four_two_r18_c_>();
}
n1_n2_cbsr_r18_c::eight_one_r18_c_& n1_n2_cbsr_r18_c::set_eight_one_r18()
{
  set(types::eight_one_r18);
  return c.get<eight_one_r18_c_>();
}
n1_n2_cbsr_r18_c::four_three_r18_c_& n1_n2_cbsr_r18_c::set_four_three_r18()
{
  set(types::four_three_r18);
  return c.get<four_three_r18_c_>();
}
n1_n2_cbsr_r18_c::twelve_one_r18_c_& n1_n2_cbsr_r18_c::set_twelve_one_r18()
{
  set(types::twelve_one_r18);
  return c.get<twelve_one_r18_c_>();
}
n1_n2_cbsr_r18_c::four_four_r18_c_& n1_n2_cbsr_r18_c::set_four_four_r18()
{
  set(types::four_four_r18);
  return c.get<four_four_r18_c_>();
}
n1_n2_cbsr_r18_c::eight_two_r18_c_& n1_n2_cbsr_r18_c::set_eight_two_r18()
{
  set(types::eight_two_r18);
  return c.get<eight_two_r18_c_>();
}
n1_n2_cbsr_r18_c::sixteen_one_r18_c_& n1_n2_cbsr_r18_c::set_sixteen_one_r18()
{
  set(types::sixteen_one_r18);
  return c.get<sixteen_one_r18_c_>();
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::two_one_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::two_two_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::four_one_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::three_two_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::six_one_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::four_two_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::eight_one_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::four_three_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::twelve_one_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::four_four_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::eight_two_r18_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &n1_n2_cbsr_r18_c::sixteen_one_r18_c_::desc}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_names[] = {{"two-one-r18", nullptr},
                                                                              {"two-two-r18", nullptr},
                                                                              {"four-one-r18", nullptr},
                                                                              {"three-two-r18", nullptr},
                                                                              {"six-one-r18", nullptr},
                                                                              {"four-two-r18", nullptr},
                                                                              {"eight-one-r18", nullptr},
                                                                              {"four-three-r18", nullptr},
                                                                              {"twelve-one-r18", nullptr},
                                                                              {"four-four-r18", nullptr},
                                                                              {"eight-two-r18", nullptr},
                                                                              {"sixteen-one-r18", nullptr}};
const asn1::table::choice_desc       n1_n2_cbsr_r18_c::desc = {asn1::table::enum_int_type<n1_n2_cbsr_r18_c::types>(),
                                                               n1_n2_cbsr_r18_c::types::nof_types,
                                                               n1_n2_cbsr_r18_c::types::nof_exts,
                                                               n1_n2_cbsr_r18_c::types::has_ext,
                                                               false,
                                                               choice_table_n1_n2_cbsr_r18_c_alternatives,
                                                               nullptr,
                                                               choice_table_n1_n2_cbsr_r18_c_names,
                                                               "n1_n2_cbsr_r18_c",
                                                               offsetof(n1_n2_cbsr_r18_c, c.type),
                                                               offsetof(n1_n2_cbsr_r18_c, c.value),
                                                               false,
                                                               &n1_n2_cbsr_r18_c::storage_desc};

void n1_n2_cbsr_r18_c::two_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::two_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<8>& n1_n2_cbsr_r18_c::two_one_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_two_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<8>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_two_one_r18_c__names[] = {{"no-cbsr-r18", nullptr},
                                                                                             {"cbsr-r18", nullptr}};
const asn1::table::choice_desc       n1_n2_cbsr_r18_c::two_one_r18_c_::desc               = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::two_one_r18_c_::types>(),
    n1_n2_cbsr_r18_c::two_one_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::two_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::two_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_two_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::two_one_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_two_one_r18_c__names,
    "n1_n2_cbsr_r18_c::two_one_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::two_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::two_one_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_r18_c_two_one_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::two_one_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_r18_c_two_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::two_one_r18_c_::types"};

void n1_n2_cbsr_r18_c::two_two_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::two_two_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<27>& n1_n2_cbsr_r18_c::two_two_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_two_two_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<27>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_two_two_r18_c__names[] = {{"no-cbsr-r18", nullptr},
                                                                                             {"cbsr-r18", nullptr}};
const asn1::table::choice_desc       n1_n2_cbsr_r18_c::two_two_r18_c_::desc               = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::two_two_r18_c_::types>(),
    n1_n2_cbsr_r18_c::two_two_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::two_two_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::two_two_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_two_two_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::two_two_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_two_two_r18_c__names,
    "n1_n2_cbsr_r18_c::two_two_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::two_two_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::two_two_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_r18_c_two_two_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::two_two_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_r18_c_two_two_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::two_two_r18_c_::types"};

void n1_n2_cbsr_r18_c::four_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::four_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<16>& n1_n2_cbsr_r18_c::four_one_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_four_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<16>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_four_one_r18_c__names[] = {{"no-cbsr-r18", nullptr},
                                                                                              {"cbsr-r18", nullptr}};
const asn1::table::choice_desc       n1_n2_cbsr_r18_c::four_one_r18_c_::desc               = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::four_one_r18_c_::types>(),
    n1_n2_cbsr_r18_c::four_one_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::four_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::four_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_four_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::four_one_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_four_one_r18_c__names,
    "n1_n2_cbsr_r18_c::four_one_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::four_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::four_one_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_r18_c_four_one_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::four_one_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_r18_c_four_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::four_one_r18_c_::types"};

void n1_n2_cbsr_r18_c::three_two_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::three_two_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<35>& n1_n2_cbsr_r18_c::three_two_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_three_two_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<35>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_three_two_r18_c__names[] = {{"no-cbsr-r18", nullptr},
                                                                                               {"cbsr-r18", nullptr}};
const asn1::table::choice_desc       n1_n2_cbsr_r18_c::three_two_r18_c_::desc               = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::three_two_r18_c_::types>(),
    n1_n2_cbsr_r18_c::three_two_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::three_two_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::three_two_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_three_two_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::three_two_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_three_two_r18_c__names,
    "n1_n2_cbsr_r18_c::three_two_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::three_two_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::three_two_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_r18_c_three_two_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::three_two_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_r18_c_three_two_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::three_two_r18_c_::types"};

void n1_n2_cbsr_r18_c::six_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::six_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<24>& n1_n2_cbsr_r18_c::six_one_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_six_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<24>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_six_one_r18_c__names[] = {{"no-cbsr-r18", nullptr},
                                                                                             {"cbsr-r18", nullptr}};
const asn1::table::choice_desc       n1_n2_cbsr_r18_c::six_one_r18_c_::desc               = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::six_one_r18_c_::types>(),
    n1_n2_cbsr_r18_c::six_one_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::six_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::six_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_six_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::six_one_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_six_one_r18_c__names,
    "n1_n2_cbsr_r18_c::six_one_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::six_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::six_one_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_r18_c_six_one_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::six_one_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_r18_c_six_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::six_one_r18_c_::types"};

void n1_n2_cbsr_r18_c::four_two_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::four_two_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<43>& n1_n2_cbsr_r18_c::four_two_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_four_two_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<43>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_four_two_r18_c__names[] = {{"no-cbsr-r18", nullptr},
                                                                                              {"cbsr-r18", nullptr}};
const asn1::table::choice_desc       n1_n2_cbsr_r18_c::four_two_r18_c_::desc               = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::four_two_r18_c_::types>(),
    n1_n2_cbsr_r18_c::four_two_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::four_two_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::four_two_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_four_two_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::four_two_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_four_two_r18_c__names,
    "n1_n2_cbsr_r18_c::four_two_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::four_two_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::four_two_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_r18_c_four_two_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::four_two_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_r18_c_four_two_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::four_two_r18_c_::types"};

void n1_n2_cbsr_r18_c::eight_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::eight_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<32>& n1_n2_cbsr_r18_c::eight_one_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_eight_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<32>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_eight_one_r18_c__names[] = {{"no-cbsr-r18", nullptr},
                                                                                               {"cbsr-r18", nullptr}};
const asn1::table::choice_desc       n1_n2_cbsr_r18_c::eight_one_r18_c_::desc               = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::eight_one_r18_c_::types>(),
    n1_n2_cbsr_r18_c::eight_one_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::eight_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::eight_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_eight_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::eight_one_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_eight_one_r18_c__names,
    "n1_n2_cbsr_r18_c::eight_one_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::eight_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::eight_one_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_r18_c_eight_one_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::eight_one_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_r18_c_eight_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::eight_one_r18_c_::types"};

void n1_n2_cbsr_r18_c::four_three_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::four_three_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<59>& n1_n2_cbsr_r18_c::four_three_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_four_three_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<59>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_four_three_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_r18_c::four_three_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::four_three_r18_c_::types>(),
    n1_n2_cbsr_r18_c::four_three_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::four_three_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::four_three_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_four_three_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::four_three_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_four_three_r18_c__names,
    "n1_n2_cbsr_r18_c::four_three_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::four_three_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::four_three_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_r18_c_four_three_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::four_three_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_r18_c_four_three_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::four_three_r18_c_::types"};

void n1_n2_cbsr_r18_c::twelve_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::twelve_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<48>& n1_n2_cbsr_r18_c::twelve_one_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_twelve_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<48>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_twelve_one_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_r18_c::twelve_one_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::twelve_one_r18_c_::types>(),
    n1_n2_cbsr_r18_c::twelve_one_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::twelve_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::twelve_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_twelve_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::twelve_one_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_twelve_one_r18_c__names,
    "n1_n2_cbsr_r18_c::twelve_one_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::twelve_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::twelve_one_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_r18_c_twelve_one_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::twelve_one_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_r18_c_twelve_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::twelve_one_r18_c_::types"};

void n1_n2_cbsr_r18_c::four_four_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::four_four_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<75>& n1_n2_cbsr_r18_c::four_four_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_four_four_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<75>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_four_four_r18_c__names[] = {{"no-cbsr-r18", nullptr},
                                                                                               {"cbsr-r18", nullptr}};
const asn1::table::choice_desc       n1_n2_cbsr_r18_c::four_four_r18_c_::desc               = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::four_four_r18_c_::types>(),
    n1_n2_cbsr_r18_c::four_four_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::four_four_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::four_four_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_four_four_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::four_four_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_four_four_r18_c__names,
    "n1_n2_cbsr_r18_c::four_four_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::four_four_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::four_four_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_r18_c_four_four_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::four_four_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_r18_c_four_four_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::four_four_r18_c_::types"};

void n1_n2_cbsr_r18_c::eight_two_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::eight_two_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<75>& n1_n2_cbsr_r18_c::eight_two_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_eight_two_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<75>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_eight_two_r18_c__names[] = {{"no-cbsr-r18", nullptr},
                                                                                               {"cbsr-r18", nullptr}};
const asn1::table::choice_desc       n1_n2_cbsr_r18_c::eight_two_r18_c_::desc               = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::eight_two_r18_c_::types>(),
    n1_n2_cbsr_r18_c::eight_two_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::eight_two_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::eight_two_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_eight_two_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::eight_two_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_eight_two_r18_c__names,
    "n1_n2_cbsr_r18_c::eight_two_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::eight_two_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::eight_two_r18_c_, c),
    false};

static const char*           enum_names_n1_n2_cbsr_r18_c_eight_two_r18_c__types_values[] = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::eight_two_r18_c_::types_opts::desc        = {
    enum_names_n1_n2_cbsr_r18_c_eight_two_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::eight_two_r18_c_::types"};

void n1_n2_cbsr_r18_c::sixteen_one_r18_c_::set(types::options e)
{
  type_ = e;
}
void n1_n2_cbsr_r18_c::sixteen_one_r18_c_::set_no_cbsr_r18()
{
  set(types::no_cbsr_r18);
}
fixed_bitstring<64>& n1_n2_cbsr_r18_c::sixteen_one_r18_c_::set_cbsr_r18()
{
  set(types::cbsr_r18);
  return c;
}
static const asn1::table::seq_field choice_table_n1_n2_cbsr_r18_c_sixteen_one_r18_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<64>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_n1_n2_cbsr_r18_c_sixteen_one_r18_c__names[] = {
    {"no-cbsr-r18", nullptr},
    {"cbsr-r18", nullptr}};
const asn1::table::choice_desc n1_n2_cbsr_r18_c::sixteen_one_r18_c_::desc = {
    asn1::table::enum_int_type<n1_n2_cbsr_r18_c::sixteen_one_r18_c_::types>(),
    n1_n2_cbsr_r18_c::sixteen_one_r18_c_::types::nof_types,
    n1_n2_cbsr_r18_c::sixteen_one_r18_c_::types::nof_exts,
    n1_n2_cbsr_r18_c::sixteen_one_r18_c_::types::has_ext,
    false,
    choice_table_n1_n2_cbsr_r18_c_sixteen_one_r18_c__alternatives,
    &asn1::table::choice_set<n1_n2_cbsr_r18_c::sixteen_one_r18_c_>,
    choice_table_n1_n2_cbsr_r18_c_sixteen_one_r18_c__names,
    "n1_n2_cbsr_r18_c::sixteen_one_r18_c_",
    offsetof(n1_n2_cbsr_r18_c::sixteen_one_r18_c_, type_),
    offsetof(n1_n2_cbsr_r18_c::sixteen_one_r18_c_, c),
    false};

static const char* enum_names_n1_n2_cbsr_r18_c_sixteen_one_r18_c__types_values[]    = {"no-cbsr-r18", "cbsr-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::sixteen_one_r18_c_::types_opts::desc = {
    enum_names_n1_n2_cbsr_r18_c_sixteen_one_r18_c__types_values,
    2,
    "n1_n2_cbsr_r18_c::sixteen_one_r18_c_::types"};

static const char*           enum_names_n1_n2_cbsr_r18_c_types_values[] = {"two-one-r18",
                                                                           "two-two-r18",
                                                                           "four-one-r18",
                                                                           "three-two-r18",
                                                                           "six-one-r18",
                                                                           "four-two-r18",
                                                                           "eight-one-r18",
                                                                           "four-three-r18",
                                                                           "twelve-one-r18",
                                                                           "four-four-r18",
                                                                           "eight-two-r18",
                                                                           "sixteen-one-r18"};
const asn1::table::enum_desc n1_n2_cbsr_r18_c::types_opts::desc         = {enum_names_n1_n2_cbsr_r18_c_types_values,
                                                                           12,
                                                                           "n1_n2_cbsr_r18_c::types"};

// TD-DD-Config-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_td_dd_cfg_r18_s_fields[] = {
    {offsetof(td_dd_cfg_r18_s, vector_len_dd_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(td_dd_cfg_r18_s::vector_len_dd_r18)>(),
     decltype(td_dd_cfg_r18_s::vector_len_dd_r18)::has_ext,
     false,
     decltype(td_dd_cfg_r18_s::vector_len_dd_r18)::nof_types,
     decltype(td_dd_cfg_r18_s::vector_len_dd_r18)::nof_exts,
     nullptr,
     &td_dd_cfg_r18_s::vector_len_dd_r18_opts::desc},
    {offsetof(td_dd_cfg_r18_s, unit_dur_dd_r18),
     (int32_t)offsetof(td_dd_cfg_r18_s, unit_dur_dd_r18_present),
     asn1::table::enum_op<decltype(td_dd_cfg_r18_s::unit_dur_dd_r18)>(),
     decltype(td_dd_cfg_r18_s::unit_dur_dd_r18)::has_ext,
     false,
     decltype(td_dd_cfg_r18_s::unit_dur_dd_r18)::nof_types,
     decltype(td_dd_cfg_r18_s::unit_dur_dd_r18)::nof_exts,
     nullptr,
     &td_dd_cfg_r18_s::unit_dur_dd_r18_opts::desc},
    {offsetof(td_dd_cfg_r18_s, aperiodic_res_offset_r18),
     (int32_t)offsetof(td_dd_cfg_r18_s, aperiodic_res_offset_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(2),
     nullptr},
    {offsetof(td_dd_cfg_r18_s, td_cqi_r18),
     (int32_t)offsetof(td_dd_cfg_r18_s, td_cqi_r18_present),
     asn1::table::enum_op<decltype(td_dd_cfg_r18_s::td_cqi_r18)>(),
     decltype(td_dd_cfg_r18_s::td_cqi_r18)::has_ext,
     false,
     decltype(td_dd_cfg_r18_s::td_cqi_r18)::nof_types,
     decltype(td_dd_cfg_r18_s::td_cqi_r18)::nof_exts,
     nullptr,
     &td_dd_cfg_r18_s::td_cqi_r18_opts::desc}};
static const asn1::table::field_name seq_table_td_dd_cfg_r18_s_names[] = {{"vectorLengthDD-r18", nullptr},
                                                                          {"unitDurationDD-r18", nullptr},
                                                                          {"aperiodicResourceOffset-r18", nullptr},
                                                                          {"tdCQI-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_td_dd_cfg_r18_s_opts[]  = {
    {1, (int32_t)offsetof(td_dd_cfg_r18_s, unit_dur_dd_r18_present)},
    {2, (int32_t)offsetof(td_dd_cfg_r18_s, aperiodic_res_offset_r18_present)},
    {3, (int32_t)offsetof(td_dd_cfg_r18_s, td_cqi_r18_present)}};
const asn1::table::seq_desc td_dd_cfg_r18_s::desc = {
    {seq_table_td_dd_cfg_r18_s_fields, 4, UINT64_C(0x1), seq_table_td_dd_cfg_r18_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_td_dd_cfg_r18_s_names};

static const char*           enum_names_td_dd_cfg_r18_s_vector_len_dd_r18_e__values[] = {"n1", "n2", "n4", "n8"};
const asn1::table::enum_desc td_dd_cfg_r18_s::vector_len_dd_r18_opts::desc            = {
    enum_names_td_dd_cfg_r18_s_vector_len_dd_r18_e__values,
    4,
    "td_dd_cfg_r18_s::vector_len_dd_r18_e_"};
uint8_t td_dd_cfg_r18_s::vector_len_dd_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8};
  return map_enum_number(numbers, 4, value, "td_dd_cfg_r18_s::vector_len_dd_r18_e_");
}

static const char*           enum_names_td_dd_cfg_r18_s_unit_dur_dd_r18_e__values[] = {"m1", "m2"};
const asn1::table::enum_desc td_dd_cfg_r18_s::unit_dur_dd_r18_opts::desc            = {
    enum_names_td_dd_cfg_r18_s_unit_dur_dd_r18_e__values,
    2,
    "td_dd_cfg_r18_s::unit_dur_dd_r18_e_"};
uint8_t td_dd_cfg_r18_s::unit_dur_dd_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "td_dd_cfg_r18_s::unit_dur_dd_r18_e_");
}

static const char*           enum_names_td_dd_cfg_r18_s_td_cqi_r18_e__values[] = {"n11", "n12", "n2", "spare1"};
const asn1::table::enum_desc td_dd_cfg_r18_s::td_cqi_r18_opts::desc = {enum_names_td_dd_cfg_r18_s_td_cqi_r18_e__values,
                                                                       4,
                                                                       "td_dd_cfg_r18_s::td_cqi_r18_e_"};
uint8_t                      td_dd_cfg_r18_s::td_cqi_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {11, 12, 2};
  return map_enum_number(numbers, 3, value, "td_dd_cfg_r18_s::td_cqi_r18_e_");
}

// CodebookConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_codebook_cfg_r18_s_fields[] = {
    {offsetof(codebook_cfg_r18_s, codebook_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &codebook_cfg_r18_s::codebook_type_c_::desc}};
static const asn1::table::field_name seq_table_codebook_cfg_r18_s_names[] = {{"codebookType", nullptr}};
const asn1::table::seq_desc          codebook_cfg_r18_s::desc             = {
    {seq_table_codebook_cfg_r18_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_r18_s_names};

static const asn1::table::seq_field choice_table_codebook_cfg_r18_s_codebook_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &codebook_cfg_r18_s::codebook_type_c_::type2_c_::desc}};
static const asn1::table::field_name choice_table_codebook_cfg_r18_s_codebook_type_c__names[] = {{"type2", nullptr}};
const asn1::table::choice_desc       codebook_cfg_r18_s::codebook_type_c_::desc               = {
    asn1::table::enum_int_type<codebook_cfg_r18_s::codebook_type_c_::types>(),
    codebook_cfg_r18_s::codebook_type_c_::types::nof_types,
    codebook_cfg_r18_s::codebook_type_c_::types::nof_exts,
    codebook_cfg_r18_s::codebook_type_c_::types::has_ext,
    false,
    choice_table_codebook_cfg_r18_s_codebook_type_c__alternatives,
    nullptr,
    choice_table_codebook_cfg_r18_s_codebook_type_c__names,
    "codebook_cfg_r18_s::codebook_type_c_",
    asn1::table::choice_desc::no_member,
    offsetof(codebook_cfg_r18_s::codebook_type_c_, c),
    false};

static const asn1::table::storage_ops* const
    choice_storage_codebook_cfg_r18_s_codebook_type_c__type2_c__alternatives[] = {
        &asn1::table::alternative_ops<codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_, false>::value,
        &asn1::table::alternative_ops<codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_,
                                      false>::value,
        &asn1::table::alternative_ops<codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_,
                                      false>::value,
        &asn1::table::alternative_ops<codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_,
                                      false>::value};
const asn1::table::choice_storage_desc codebook_cfg_r18_s::codebook_type_c_::type2_c_::storage_desc = {
    choice_storage_codebook_cfg_r18_s_codebook_type_c__type2_c__alternatives,
    4,
    "codebook_cfg_r18_s::codebook_type_c_::type2_c_"};

codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_&
codebook_cfg_r18_s::codebook_type_c_::type2_c_::set_type_ii_cjt_r18()
{
  set(types::type_ii_cjt_r18);
  return c.get<type_ii_cjt_r18_s_>();
}
codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_&
codebook_cfg_r18_s::codebook_type_c_::type2_c_::set_type_ii_cjt_port_sel_r18()
{
  set(types::type_ii_cjt_port_sel_r18);
  return c.get<type_ii_cjt_port_sel_r18_s_>();
}
codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_&
codebook_cfg_r18_s::codebook_type_c_::type2_c_::set_type_ii_doppler_r18()
{
  set(types::type_ii_doppler_r18);
  return c.get<type_ii_doppler_r18_s_>();
}
codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_&
codebook_cfg_r18_s::codebook_type_c_::type2_c_::set_type_ii_doppler_port_sel_r18()
{
  set(types::type_ii_doppler_port_sel_r18);
  return c.get<type_ii_doppler_port_sel_r18_s_>();
}
static const asn1::table::seq_field seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_r18_s__fields[] =
    {{offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_,
               n1_n2_codebook_subset_restrict_list_r18),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      nullptr,
      &n1_n2_cbsr_list_r18_c::desc},
     {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_, param_combination_cjt_r18),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::int_u8,
      false,
      false,
      INT64_C(1),
      UINT64_C(7),
      nullptr},
     {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_, param_combination_cjt_l_r18),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      4,
      &asn1::table::list_codec<
          decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_::param_combination_cjt_l_r18),
          asn1::table::int_json,
          asn1::table::int_items<false, INT64_C(1), UINT64_C(5), false, false>>::value},
     {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
     {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_, value_of_o3_r18),
      (int32_t)offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_, value_of_o3_r18_present),
      asn1::table::enum_op<
          decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_::value_of_o3_r18)>(),
      decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_::value_of_o3_r18)::has_ext,
      false,
      decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_::value_of_o3_r18)::nof_types,
      decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_::value_of_o3_r18)::nof_exts,
      nullptr,
      &codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_::value_of_o3_r18_opts::desc},
     {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_,
               nof_pmi_subbands_per_cqi_subband_r18),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::int_u8,
      false,
      false,
      INT64_C(1),
      UINT64_C(2),
      nullptr},
     {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_, type_ii_ri_restrict_r18),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<
          decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_::type_ii_ri_restrict_r18),
          asn1::table::struct_json>::value},
     {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_, codebook_mode_r18),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::int_u8,
      false,
      false,
      INT64_C(1),
      UINT64_C(2),
      nullptr}};
static const asn1::table::field_name seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_r18_s__names[] =
    {{"n1-n2-codebookSubsetRestrictionList-r18", nullptr},
     {"paramCombination-CJT-r18", nullptr},
     {"paramCombination-CJT-L-r18", nullptr},
     {"restrictedCMR-Selection-r18", "enable"},
     {"valueOfO3-r18", nullptr},
     {"numberOfPMI-SubbandsPerCQI-Subband-r18", nullptr},
     {"typeII-RI-Restriction-r18", nullptr},
     {"codebookMode-r18", nullptr}};
static const asn1::table::seq_opt seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_r18_s__opts[] = {
    {4,
     (int32_t)offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_, value_of_o3_r18_present)}};
static const asn1::table::seq_desc seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_r18_s_ = {
    {seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_r18_s__fields,
     8,
     UINT64_C(0xef),
     seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_r18_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_r18_s__names};
static const asn1::table::seq_field
    seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s__fields[] = {
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_,
                  param_combination_cjt_ps_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(5),
         nullptr},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_,
                  param_combination_cjt_ps_alpha_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         4,
         &asn1::table::list_codec<decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::
                                               param_combination_cjt_ps_alpha_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(1), UINT64_C(8), false, false>>::value},
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_, value_of_o3_r18),
         (int32_t)offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_,
                           value_of_o3_r18_present),
         asn1::table::enum_op<
             decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_o3_r18)>(),
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_o3_r18)::
             has_ext,
         false,
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_o3_r18)::
             nof_types,
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_o3_r18)::
             nof_exts,
         nullptr,
         &codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_o3_r18_opts::desc},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_, value_of_n_cjt_r18),
         (int32_t)offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_,
                           value_of_n_cjt_r18_present),
         asn1::table::enum_op<decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::
                                           value_of_n_cjt_r18)>(),
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_n_cjt_r18)::
             has_ext,
         false,
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_n_cjt_r18)::
             nof_types,
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_n_cjt_r18)::
             nof_exts,
         nullptr,
         &codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_n_cjt_r18_opts::desc},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_,
                  nof_pmi_subbands_per_cqi_subband_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(2),
         nullptr},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_,
                  type_ii_port_sel_ri_restrict_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::
                                                 type_ii_cjt_port_sel_r18_s_::type_ii_port_sel_ri_restrict_r18),
                                    asn1::table::struct_json>::value},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_, codebook_mode_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(2),
         nullptr}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s__names[] = {
        {"paramCombination-CJT-PS-r18", nullptr},
        {"paramCombination-CJT-PS-alpha-r18", nullptr},
        {"restrictedCMR-Selection-r18", "enable"},
        {"valueOfO3-r18", nullptr},
        {"valueOfN-CJT-r18", nullptr},
        {"numberOfPMI-SubbandsPerCQI-Subband-r18", nullptr},
        {"typeII-PortSelectionRI-Restriction-r18", nullptr},
        {"codebookMode-r18", nullptr}};
static const asn1::table::seq_opt
    seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s__opts[] = {
        {3,
         (int32_t)offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_,
                           value_of_o3_r18_present)},
        {4,
         (int32_t)offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_,
                           value_of_n_cjt_r18_present)}};
static const asn1::table::seq_desc seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s_ =
    {{seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s__fields,
      8,
      UINT64_C(0xe7),
      seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s__opts,
      2},
     false,
     false,
     nullptr,
     0,
     seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s__names};
static const asn1::table::seq_field
    seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_r18_s__fields[] = {
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_,
                  n1_n2_codebook_subset_restrict_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &n1_n2_cbsr_r18_c::desc},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_,
                  param_combination_doppler_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(9),
         nullptr},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_, td_dd_cfg_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &td_dd_cfg_r18_s::desc},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_,
                  nof_pmi_subbands_per_cqi_subband_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(2),
         nullptr},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_, prediction_delay_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<
             decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_::prediction_delay_r18)>(),
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_::prediction_delay_r18)::
             has_ext,
         false,
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_::prediction_delay_r18)::
             nof_types,
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_::prediction_delay_r18)::
             nof_exts,
         nullptr,
         &codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_::prediction_delay_r18_opts::desc},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_, type_ii_ri_restrict_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<
             decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_::type_ii_ri_restrict_r18),
             asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_r18_s__names[] = {
        {"n1-n2-codebookSubsetRestriction-r18", nullptr},
        {"paramCombination-Doppler-r18", nullptr},
        {"td-dd-config-r18", nullptr},
        {"numberOfPMI-SubbandsPerCQI-Subband-r18", nullptr},
        {"predictionDelay-r18", nullptr},
        {"typeII-RI-Restriction-r18", nullptr}};
static const asn1::table::seq_desc seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_r18_s_ = {
    {seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_r18_s__fields,
     6,
     UINT64_C(0x3f),
     nullptr,
     0},
    false,
    false,
    nullptr,
    0,
    seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_r18_s__names};
static const asn1::table::seq_field
    seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s__fields[] = {
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_,
                  param_combination_doppler_ps_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(8),
         nullptr},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_, td_dd_cfg_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &td_dd_cfg_r18_s::desc},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_,
                  value_of_n_doppler_r18),
         (int32_t)offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_,
                           value_of_n_doppler_r18_present),
         asn1::table::enum_op<decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::
                                           value_of_n_doppler_r18)>(),
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::
                      value_of_n_doppler_r18)::has_ext,
         false,
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::
                      value_of_n_doppler_r18)::nof_types,
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::
                      value_of_n_doppler_r18)::nof_exts,
         nullptr,
         &codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::value_of_n_doppler_r18_opts::
             desc},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_,
                  nof_pmi_subbands_per_cqi_subband_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(2),
         nullptr},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_,
                  prediction_delay_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::
                                           prediction_delay_r18)>(),
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::
                      prediction_delay_r18)::has_ext,
         false,
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::
                      prediction_delay_r18)::nof_types,
         decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::
                      prediction_delay_r18)::nof_exts,
         nullptr,
         &codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::prediction_delay_r18_opts::
             desc},
        {offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_,
                  type_ii_port_sel_ri_restrict_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<decltype(codebook_cfg_r18_s::codebook_type_c_::type2_c_::
                                                 type_ii_doppler_port_sel_r18_s_::type_ii_port_sel_ri_restrict_r18),
                                    asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s__names[] = {
        {"paramCombinationDoppler-PS-r18", nullptr},
        {"td-dd-config-r18", nullptr},
        {"valueOfN-Doppler-r18", nullptr},
        {"numberOfPMI-SubbandsPerCQI-Subband-r18", nullptr},
        {"predictionDelay-r18", nullptr},
        {"typeII-PortSelectionRI-Restriction-r18", nullptr}};
static const asn1::table::seq_opt
    seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s__opts[] = {
        {2,
         (int32_t)offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_,
                           value_of_n_doppler_r18_present)}};
static const asn1::table::seq_desc
    seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s_ = {
        {seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s__fields,
         6,
         UINT64_C(0x3b),
         seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s__opts,
         1},
        false,
        false,
        nullptr,
        0,
        seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s__names};
static const asn1::table::seq_field choice_table_codebook_cfg_r18_s_codebook_type_c__type2_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_r18_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_r18_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s_}};
static const asn1::table::field_name choice_table_codebook_cfg_r18_s_codebook_type_c__type2_c__names[] = {
    {"typeII-CJT-r18", nullptr},
    {"typeII-CJT-PortSelection-r18", nullptr},
    {"typeII-Doppler-r18", nullptr},
    {"typeII-DopplerPortSelection-r18", nullptr}};
const asn1::table::choice_desc codebook_cfg_r18_s::codebook_type_c_::type2_c_::desc = {
    asn1::table::enum_int_type<codebook_cfg_r18_s::codebook_type_c_::type2_c_::types>(),
    codebook_cfg_r18_s::codebook_type_c_::type2_c_::types::nof_types,
    codebook_cfg_r18_s::codebook_type_c_::type2_c_::types::nof_exts,
    codebook_cfg_r18_s::codebook_type_c_::type2_c_::types::has_ext,
    false,
    choice_table_codebook_cfg_r18_s_codebook_type_c__type2_c__alternatives,
    nullptr,
    choice_table_codebook_cfg_r18_s_codebook_type_c__type2_c__names,
    "codebook_cfg_r18_s::codebook_type_c_::type2_c_",
    offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_, c.type),
    offsetof(codebook_cfg_r18_s::codebook_type_c_::type2_c_, c.value),
    false,
    &codebook_cfg_r18_s::codebook_type_c_::type2_c_::storage_desc};

static const char*
    enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_r18_s__value_of_o3_r18_e__values[] = {"n1",
                                                                                                              "n4"};
const asn1::table::enum_desc
    codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_::value_of_o3_r18_opts::desc = {
        enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_r18_s__value_of_o3_r18_e__values,
        2,
        "codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_::value_of_o3_r18_e_"};
uint8_t codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_::value_of_o3_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 4};
  return map_enum_number(
      numbers, 2, value, "codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_r18_s_::value_of_o3_r18_e_");
}

static const char*
    enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s__value_of_o3_r18_e__values[] = {
        "n1",
        "n4"};
const asn1::table::enum_desc
    codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_o3_r18_opts::desc = {
        enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s__value_of_o3_r18_e__values,
        2,
        "codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_o3_r18_e_"};
uint8_t
codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_o3_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 4};
  return map_enum_number(
      numbers,
      2,
      value,
      "codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_o3_r18_e_");
}

static const char*
    enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s__value_of_n_cjt_r18_e__values[] =
        {"n2", "n4"};
const asn1::table::enum_desc
    codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_n_cjt_r18_opts::desc = {
        enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_cjt_port_sel_r18_s__value_of_n_cjt_r18_e__values,
        2,
        "codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_n_cjt_r18_e_"};
uint8_t
codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_n_cjt_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(
      numbers,
      2,
      value,
      "codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_cjt_port_sel_r18_s_::value_of_n_cjt_r18_e_");
}

static const char*
    enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_r18_s__prediction_delay_r18_e__values[] =
        {"m0", "n0", "n1", "n2"};
const asn1::table::enum_desc
    codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_::prediction_delay_r18_opts::desc = {
        enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_r18_s__prediction_delay_r18_e__values,
        4,
        "codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_r18_s_::prediction_delay_r18_e_"};

static const char*
    enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s__value_of_n_doppler_r18_e__values
        [] = {"n2", "n4"};
const asn1::table::enum_desc codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::
    value_of_n_doppler_r18_opts::desc = {
        enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s__value_of_n_doppler_r18_e__values,
        2,
        "codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::value_of_n_doppler_r18_e_"};
uint8_t codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::value_of_n_doppler_r18_opts::
    to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(
      numbers,
      2,
      value,
      "codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::value_of_n_doppler_r18_e_");
}

static const char*
    enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s__prediction_delay_r18_e__values
        [] = {"m0", "n0", "n1", "n2"};
const asn1::table::enum_desc
    codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::prediction_delay_r18_opts::desc = {
        enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__type_ii_doppler_port_sel_r18_s__prediction_delay_r18_e__values,
        4,
        "codebook_cfg_r18_s::codebook_type_c_::type2_c_::type_ii_doppler_port_sel_r18_s_::prediction_delay_r18_e_"};

static const char* enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__types_values[] =
    {"typeII-CJT-r18", "typeII-CJT-PortSelection-r18", "typeII-Doppler-r18", "typeII-DopplerPortSelection-r18"};
const asn1::table::enum_desc codebook_cfg_r18_s::codebook_type_c_::type2_c_::types_opts::desc = {
    enum_names_codebook_cfg_r18_s_codebook_type_c__type2_c__types_values,
    4,
    "codebook_cfg_r18_s::codebook_type_c_::type2_c_::types"};

static const char*           enum_names_codebook_cfg_r18_s_codebook_type_c__types_values[] = {"type2"};
const asn1::table::enum_desc codebook_cfg_r18_s::codebook_type_c_::types_opts::desc        = {
    enum_names_codebook_cfg_r18_s_codebook_type_c__types_values,
    1,
    "codebook_cfg_r18_s::codebook_type_c_::types"};
uint8_t codebook_cfg_r18_s::codebook_type_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "codebook_cfg_r18_s::codebook_type_c_::types");
}

// CSI-ReportSubConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_csi_report_sub_cfg_r18_s_fields[] = {
    {offsetof(csi_report_sub_cfg_r18_s, report_sub_cfg_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(csi_report_sub_cfg_r18_s, report_sub_cfg_params_r18),
     (int32_t)offsetof(csi_report_sub_cfg_r18_s, report_sub_cfg_params_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::desc},
    {offsetof(csi_report_sub_cfg_r18_s, pwr_offset_r18),
     (int32_t)offsetof(csi_report_sub_cfg_r18_s, pwr_offset_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(23),
     nullptr}};
static const asn1::table::field_name seq_table_csi_report_sub_cfg_r18_s_names[] = {
    {"reportSubConfigId-r18", nullptr},
    {"reportSubConfigParams-r18", nullptr},
    {"powerOffset-r18", nullptr}};
static const asn1::table::seq_opt seq_table_csi_report_sub_cfg_r18_s_opts[] = {
    {1, (int32_t)offsetof(csi_report_sub_cfg_r18_s, report_sub_cfg_params_r18_present)},
    {2, (int32_t)offsetof(csi_report_sub_cfg_r18_s, pwr_offset_r18_present)}};
const asn1::table::seq_desc csi_report_sub_cfg_r18_s::desc = {
    {seq_table_csi_report_sub_cfg_r18_s_fields, 3, UINT64_C(0x1), seq_table_csi_report_sub_cfg_r18_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_sub_cfg_r18_s_names};

static const asn1::table::storage_ops* const
    choice_storage_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_,
                                      false>::value,
        &asn1::table::alternative_ops<csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a2_params_s_,
                                      false>::value};
const asn1::table::choice_storage_desc csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::storage_desc = {
    choice_storage_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__alternatives,
    2,
    "csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_"};

csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_&
csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::set_a1_params()
{
  set(types::a1_params);
  return c.get<a1_params_s_>();
}
csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a2_params_s_&
csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::set_a2_params()
{
  set(types::a2_params);
  return c.get<a2_params_s_>();
}
static const asn1::table::seq_field
    seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__fields[] = {
        {offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_, codebook_sub_cfg_r18),
         (int32_t)offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_,
                           codebook_sub_cfg_r18_present),
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &codebook_cfg_s::desc},
        {offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_, port_subset_ind_r18),
         (int32_t)offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_,
                           port_subset_ind_r18_present),
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::desc},
        {offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_, non_pmi_port_ind_r18),
         asn1::table::seq_field::list_presence,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         128,
         &asn1::table::item_list_codec<
             decltype(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::non_pmi_port_ind_r18),
             asn1::table::choice_item_ops,
             false,
             true>::value,
         &port_idx_for8_ranks_c::desc}};
static const asn1::table::field_name
    seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__names[] = {
        {"codebookSubConfig-r18", nullptr},
        {"portSubsetIndicator-r18", nullptr},
        {"non-PMI-PortIndication-r18", nullptr}};
static const asn1::table::seq_opt seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__opts[] =
    {{0,
      (int32_t)offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_,
                        codebook_sub_cfg_r18_present)},
     {1,
      (int32_t)offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_,
                        port_subset_ind_r18_present)},
     {2, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_desc seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s_ = {
    {seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__fields,
     3,
     UINT64_C(0x0),
     seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__names};
static const asn1::table::seq_field
    seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a2_params_s__fields[] = {
        {offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a2_params_s_, nzp_csi_rs_res_list_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         64,
         &asn1::table::list_codec<
             decltype(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a2_params_s_::nzp_csi_rs_res_list_r18),
             asn1::table::int_json,
             asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, false>>::value}};
static const asn1::table::field_name
    seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a2_params_s__names[] = {
        {"nzp-CSI-RS-ResourceList-r18", nullptr}};
static const asn1::table::seq_desc seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a2_params_s_ = {
    {seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a2_params_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a2_params_s__names};
static const asn1::table::seq_field choice_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__alternatives[] =
    {{0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s_},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &seq_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a2_params_s_}};
static const asn1::table::field_name choice_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__names[] = {
    {"a1-parameters", nullptr},
    {"a2-parameters", nullptr}};
const asn1::table::choice_desc csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::desc = {
    asn1::table::enum_int_type<csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::types>(),
    csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::types::nof_types,
    csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::types::nof_exts,
    csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::types::has_ext,
    false,
    choice_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__alternatives,
    nullptr,
    choice_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__names,
    "csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_",
    offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_, c.type),
    offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_, c.value),
    false,
    &csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::storage_desc};

static const asn1::table::storage_ops* const
    choice_storage_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__port_subset_ind_r18_c__alternatives
        [] = {&asn1::table::alternative_ops<fixed_bitstring<2>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<4>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<8>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<12>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<16>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<24>, false>::value,
              &asn1::table::alternative_ops<fixed_bitstring<32>, false>::value};
const asn1::table::choice_storage_desc
    csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::storage_desc = {
        choice_storage_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__port_subset_ind_r18_c__alternatives,
        7,
        "csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_"};

fixed_bitstring<2>&
csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::set_p2()
{
  set(types::p2);
  return c.get<fixed_bitstring<2>>();
}
fixed_bitstring<4>&
csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::set_p4()
{
  set(types::p4);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<8>&
csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::set_p8()
{
  set(types::p8);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<12>&
csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::set_p12()
{
  set(types::p12);
  return c.get<fixed_bitstring<12>>();
}
fixed_bitstring<16>&
csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::set_p16()
{
  set(types::p16);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<24>&
csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::set_p24()
{
  set(types::p24);
  return c.get<fixed_bitstring<24>>();
}
fixed_bitstring<32>&
csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::set_p32()
{
  set(types::p32);
  return c.get<fixed_bitstring<32>>();
}
static const asn1::table::seq_field
    choice_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__port_subset_ind_r18_c__alternatives
        [] = {{0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<2>, asn1::table::struct_json>::value},
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
               &asn1::table::object_codec<fixed_bitstring<12>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<16>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<24>, asn1::table::struct_json>::value},
              {0,
               asn1::table::seq_field::mandatory,
               asn1::table::field_op::codec,
               false,
               false,
               0,
               0,
               &asn1::table::object_codec<fixed_bitstring<32>, asn1::table::struct_json>::value}};
static const asn1::table::field_name
    choice_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__port_subset_ind_r18_c__names[] = {
        {"p2", nullptr},
        {"p4", nullptr},
        {"p8", nullptr},
        {"p12", nullptr},
        {"p16", nullptr},
        {"p24", nullptr},
        {"p32", nullptr}};
const asn1::table::choice_desc
    csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::desc = {
        asn1::table::enum_int_type<
            csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::types>(),
        csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::types::nof_types,
        csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::types::nof_exts,
        csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::types::has_ext,
        false,
        choice_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__port_subset_ind_r18_c__alternatives,
        nullptr,
        choice_table_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__port_subset_ind_r18_c__names,
        "csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_",
        offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_, c.type),
        offsetof(csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_, c.value),
        false,
        &csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::storage_desc};

static const char*
    enum_names_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__port_subset_ind_r18_c__types_values
        [] = {"p2", "p4", "p8", "p12", "p16", "p24", "p32"};
const asn1::table::enum_desc
    csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::types_opts::desc = {
        enum_names_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__a1_params_s__port_subset_ind_r18_c__types_values,
        7,
        "csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::types"};
uint8_t
csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::types_opts::to_number()
    const
{
  static const uint8_t numbers[] = {2, 4, 8, 12, 16, 24, 32};
  return map_enum_number(
      numbers,
      7,
      value,
      "csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::a1_params_s_::port_subset_ind_r18_c_::types");
}

static const char* enum_names_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__types_values[] = {"a1-parameters",
                                                                                                      "a2-parameters"};
const asn1::table::enum_desc csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::types_opts::desc = {
    enum_names_csi_report_sub_cfg_r18_s_report_sub_cfg_params_r18_c__types_values,
    2,
    "csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::types"};
uint8_t csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "csi_report_sub_cfg_r18_s::report_sub_cfg_params_r18_c_::types");
}

// CSI-ReportConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_csi_report_cfg_s_report_freq_cfg_s__fields[] = {
    {offsetof(csi_report_cfg_s::report_freq_cfg_s_, cqi_format_ind),
     (int32_t)offsetof(csi_report_cfg_s::report_freq_cfg_s_, cqi_format_ind_present),
     asn1::table::enum_op<decltype(csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind)>(),
     decltype(csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind)::has_ext,
     false,
     decltype(csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind)::nof_types,
     decltype(csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind)::nof_exts,
     nullptr,
     &csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::desc},
    {offsetof(csi_report_cfg_s::report_freq_cfg_s_, pmi_format_ind),
     (int32_t)offsetof(csi_report_cfg_s::report_freq_cfg_s_, pmi_format_ind_present),
     asn1::table::enum_op<decltype(csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind)>(),
     decltype(csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind)::has_ext,
     false,
     decltype(csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind)::nof_types,
     decltype(csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind)::nof_exts,
     nullptr,
     &csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind_opts::desc},
    {offsetof(csi_report_cfg_s::report_freq_cfg_s_, csi_report_band),
     (int32_t)offsetof(csi_report_cfg_s::report_freq_cfg_s_, csi_report_band_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::desc}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_report_freq_cfg_s__names[] = {
    {"cqi-FormatIndicator", nullptr},
    {"pmi-FormatIndicator", nullptr},
    {"csi-ReportingBand", nullptr}};
static const asn1::table::seq_opt seq_table_csi_report_cfg_s_report_freq_cfg_s__opts[] = {
    {0, (int32_t)offsetof(csi_report_cfg_s::report_freq_cfg_s_, cqi_format_ind_present)},
    {1, (int32_t)offsetof(csi_report_cfg_s::report_freq_cfg_s_, pmi_format_ind_present)},
    {2, (int32_t)offsetof(csi_report_cfg_s::report_freq_cfg_s_, csi_report_band_present)}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_report_freq_cfg_s_ = {
    {seq_table_csi_report_cfg_s_report_freq_cfg_s__fields,
     3,
     UINT64_C(0x0),
     seq_table_csi_report_cfg_s_report_freq_cfg_s__opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_report_freq_cfg_s__names};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1530_s__fields[] = {
    {offsetof(csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_, report_slot_cfg_v1530),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530)>(),
     decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530)::has_ext,
     false,
     decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530)::nof_types,
     decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530)::nof_exts,
     nullptr,
     &csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530_opts::desc}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1530_s__names[] = {
    {"reportSlotConfig-v1530", nullptr}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1530_s_ = {
    {seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1530_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1530_s__names};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1610_s__fields[] = {
    {offsetof(csi_report_cfg_s::semi_persistent_on_pusch_v1610_s_, report_slot_offset_list_dci_0_2_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<
         decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1610_s_::report_slot_offset_list_dci_0_2_r16),
         asn1::table::int_json,
         asn1::table::int_items<false, INT64_C(0), UINT64_C(32), false, false>>::value},
    {offsetof(csi_report_cfg_s::semi_persistent_on_pusch_v1610_s_, report_slot_offset_list_dci_0_1_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<
         decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1610_s_::report_slot_offset_list_dci_0_1_r16),
         asn1::table::int_json,
         asn1::table::int_items<false, INT64_C(0), UINT64_C(32), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1610_s__names[] = {
    {"reportSlotOffsetListDCI-0-2-r16", nullptr},
    {"reportSlotOffsetListDCI-0-1-r16", nullptr}};
static const asn1::table::seq_opt seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1610_s__opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1610_s_ = {
    {seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1610_s__fields,
     2,
     UINT64_C(0x0),
     seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1610_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1610_s__names};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_aperiodic_v1610_s__fields[] = {
    {offsetof(csi_report_cfg_s::aperiodic_v1610_s_, report_slot_offset_list_dci_0_2_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(csi_report_cfg_s::aperiodic_v1610_s_::report_slot_offset_list_dci_0_2_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(32), false, false>>::value},
    {offsetof(csi_report_cfg_s::aperiodic_v1610_s_, report_slot_offset_list_dci_0_1_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(csi_report_cfg_s::aperiodic_v1610_s_::report_slot_offset_list_dci_0_1_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(32), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_aperiodic_v1610_s__names[] = {
    {"reportSlotOffsetListDCI-0-2-r16", nullptr},
    {"reportSlotOffsetListDCI-0-1-r16", nullptr}};
static const asn1::table::seq_opt seq_table_csi_report_cfg_s_aperiodic_v1610_s__opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_aperiodic_v1610_s_ = {
    {seq_table_csi_report_cfg_s_aperiodic_v1610_s__fields,
     2,
     UINT64_C(0x0),
     seq_table_csi_report_cfg_s_aperiodic_v1610_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_aperiodic_v1610_s__names};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_group_based_beam_report_v1710_s__fields[] = {
    {offsetof(csi_report_cfg_s::group_based_beam_report_v1710_s_, nrof_reported_groups_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17)>(),
     decltype(csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17)::has_ext,
     false,
     decltype(csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17)::nof_types,
     decltype(csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17)::nof_exts,
     nullptr,
     &csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17_opts::desc}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_group_based_beam_report_v1710_s__names[] = {
    {"nrofReportedGroups-r17", nullptr}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_group_based_beam_report_v1710_s_ = {
    {seq_table_csi_report_cfg_s_group_based_beam_report_v1710_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_group_based_beam_report_v1710_s__names};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1720_s__fields[] = {
    {offsetof(csi_report_cfg_s::semi_persistent_on_pusch_v1720_s_, report_slot_offset_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<
         decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1720_s_::report_slot_offset_list_r17),
         asn1::table::int_json,
         asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value},
    {offsetof(csi_report_cfg_s::semi_persistent_on_pusch_v1720_s_, report_slot_offset_list_dci_0_2_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<
         decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1720_s_::report_slot_offset_list_dci_0_2_r17),
         asn1::table::int_json,
         asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value},
    {offsetof(csi_report_cfg_s::semi_persistent_on_pusch_v1720_s_, report_slot_offset_list_dci_0_1_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<
         decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1720_s_::report_slot_offset_list_dci_0_1_r17),
         asn1::table::int_json,
         asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1720_s__names[] = {
    {"reportSlotOffsetList-r17", nullptr},
    {"reportSlotOffsetListDCI-0-2-r17", nullptr},
    {"reportSlotOffsetListDCI-0-1-r17", nullptr}};
static const asn1::table::seq_opt seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1720_s__opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1720_s_ = {
    {seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1720_s__fields,
     3,
     UINT64_C(0x0),
     seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1720_s__opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1720_s__names};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_aperiodic_v1720_s__fields[] = {
    {offsetof(csi_report_cfg_s::aperiodic_v1720_s_, report_slot_offset_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(csi_report_cfg_s::aperiodic_v1720_s_::report_slot_offset_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value},
    {offsetof(csi_report_cfg_s::aperiodic_v1720_s_, report_slot_offset_list_dci_0_2_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(csi_report_cfg_s::aperiodic_v1720_s_::report_slot_offset_list_dci_0_2_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value},
    {offsetof(csi_report_cfg_s::aperiodic_v1720_s_, report_slot_offset_list_dci_0_1_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(csi_report_cfg_s::aperiodic_v1720_s_::report_slot_offset_list_dci_0_1_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_aperiodic_v1720_s__names[] = {
    {"reportSlotOffsetList-r17", nullptr},
    {"reportSlotOffsetListDCI-0-2-r17", nullptr},
    {"reportSlotOffsetListDCI-0-1-r17", nullptr}};
static const asn1::table::seq_opt seq_table_csi_report_cfg_s_aperiodic_v1720_s__opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_aperiodic_v1720_s_ = {
    {seq_table_csi_report_cfg_s_aperiodic_v1720_s__fields,
     3,
     UINT64_C(0x0),
     seq_table_csi_report_cfg_s_aperiodic_v1720_s__opts,
     3},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_aperiodic_v1720_s__names};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_group_based_beam_report_v1800_s__fields[] = {
    {offsetof(csi_report_cfg_s::group_based_beam_report_v1800_s_, report_mode_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(csi_report_cfg_s::group_based_beam_report_v1800_s_::report_mode_r18)>(),
     decltype(csi_report_cfg_s::group_based_beam_report_v1800_s_::report_mode_r18)::has_ext,
     false,
     decltype(csi_report_cfg_s::group_based_beam_report_v1800_s_::report_mode_r18)::nof_types,
     decltype(csi_report_cfg_s::group_based_beam_report_v1800_s_::report_mode_r18)::nof_exts,
     nullptr,
     &csi_report_cfg_s::group_based_beam_report_v1800_s_::report_mode_r18_opts::desc}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_group_based_beam_report_v1800_s__names[] = {
    {"reportingMode-r18", nullptr}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_group_based_beam_report_v1800_s_ = {
    {seq_table_csi_report_cfg_s_group_based_beam_report_v1800_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_group_based_beam_report_v1800_s__names};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_fields[] = {
    {offsetof(csi_report_cfg_s, report_cfg_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(47),
     nullptr},
    {offsetof(csi_report_cfg_s, carrier),
     (int32_t)offsetof(csi_report_cfg_s, carrier_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(csi_report_cfg_s, res_for_ch_meas),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(111),
     nullptr},
    {offsetof(csi_report_cfg_s, csi_im_res_for_interference),
     (int32_t)offsetof(csi_report_cfg_s, csi_im_res_for_interference_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(111),
     nullptr},
    {offsetof(csi_report_cfg_s, nzp_csi_rs_res_for_interference),
     (int32_t)offsetof(csi_report_cfg_s, nzp_csi_rs_res_for_interference_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(111),
     nullptr},
    {offsetof(csi_report_cfg_s, report_cfg_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_report_cfg_s::report_cfg_type_c_::desc},
    {offsetof(csi_report_cfg_s, report_quant),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_report_cfg_s::report_quant_c_::desc},
    {offsetof(csi_report_cfg_s, report_freq_cfg),
     (int32_t)offsetof(csi_report_cfg_s, report_freq_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_report_cfg_s_report_freq_cfg_s_},
    {offsetof(csi_report_cfg_s, time_restrict_for_ch_meass),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(csi_report_cfg_s::time_restrict_for_ch_meass)>(),
     decltype(csi_report_cfg_s::time_restrict_for_ch_meass)::has_ext,
     false,
     decltype(csi_report_cfg_s::time_restrict_for_ch_meass)::nof_types,
     decltype(csi_report_cfg_s::time_restrict_for_ch_meass)::nof_exts,
     nullptr,
     &csi_report_cfg_s::time_restrict_for_ch_meass_opts::desc},
    {offsetof(csi_report_cfg_s, time_restrict_for_interference_meass),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(csi_report_cfg_s::time_restrict_for_interference_meass)>(),
     decltype(csi_report_cfg_s::time_restrict_for_interference_meass)::has_ext,
     false,
     decltype(csi_report_cfg_s::time_restrict_for_interference_meass)::nof_types,
     decltype(csi_report_cfg_s::time_restrict_for_interference_meass)::nof_exts,
     nullptr,
     &csi_report_cfg_s::time_restrict_for_interference_meass_opts::desc},
    {offsetof(csi_report_cfg_s, codebook_cfg),
     (int32_t)offsetof(csi_report_cfg_s, codebook_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &codebook_cfg_s::desc},
    {offsetof(csi_report_cfg_s, dummy),
     (int32_t)offsetof(csi_report_cfg_s, dummy_present),
     asn1::table::enum_op<decltype(csi_report_cfg_s::dummy)>(),
     decltype(csi_report_cfg_s::dummy)::has_ext,
     false,
     decltype(csi_report_cfg_s::dummy)::nof_types,
     decltype(csi_report_cfg_s::dummy)::nof_exts,
     nullptr,
     &csi_report_cfg_s::dummy_opts::desc},
    {offsetof(csi_report_cfg_s, group_based_beam_report),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_report_cfg_s::group_based_beam_report_c_::desc},
    {offsetof(csi_report_cfg_s, cqi_table),
     (int32_t)offsetof(csi_report_cfg_s, cqi_table_present),
     asn1::table::enum_op<decltype(csi_report_cfg_s::cqi_table)>(),
     decltype(csi_report_cfg_s::cqi_table)::has_ext,
     false,
     decltype(csi_report_cfg_s::cqi_table)::nof_types,
     decltype(csi_report_cfg_s::cqi_table)::nof_exts,
     nullptr,
     &csi_report_cfg_s::cqi_table_opts::desc},
    {offsetof(csi_report_cfg_s, subband_size),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(csi_report_cfg_s::subband_size)>(),
     decltype(csi_report_cfg_s::subband_size)::has_ext,
     false,
     decltype(csi_report_cfg_s::subband_size)::nof_types,
     decltype(csi_report_cfg_s::subband_size)::nof_exts,
     nullptr,
     &csi_report_cfg_s::subband_size_opts::desc},
    {offsetof(csi_report_cfg_s, non_pmi_port_ind),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     128,
     &asn1::table::item_list_codec<decltype(csi_report_cfg_s::non_pmi_port_ind),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &port_idx_for8_ranks_c::desc}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_names[] = {
    {"reportConfigId", nullptr},
    {"carrier", nullptr},
    {"resourcesForChannelMeasurement", nullptr},
    {"csi-IM-ResourcesForInterference", nullptr},
    {"nzp-CSI-RS-ResourcesForInterference", nullptr},
    {"reportConfigType", nullptr},
    {"reportQuantity", nullptr},
    {"reportFreqConfiguration", nullptr},
    {"timeRestrictionForChannelMeasurements", nullptr},
    {"timeRestrictionForInterferenceMeasurements", nullptr},
    {"codebookConfig", nullptr},
    {"dummy", nullptr},
    {"groupBasedBeamReporting", nullptr},
    {"cqi-Table", nullptr},
    {"subbandSize", nullptr},
    {"non-PMI-PortIndication", nullptr}};
static const asn1::table::seq_opt seq_table_csi_report_cfg_s_opts[] = {
    {1, (int32_t)offsetof(csi_report_cfg_s, carrier_present)},
    {3, (int32_t)offsetof(csi_report_cfg_s, csi_im_res_for_interference_present)},
    {4, (int32_t)offsetof(csi_report_cfg_s, nzp_csi_rs_res_for_interference_present)},
    {7, (int32_t)offsetof(csi_report_cfg_s, report_freq_cfg_present)},
    {10, (int32_t)offsetof(csi_report_cfg_s, codebook_cfg_present)},
    {11, (int32_t)offsetof(csi_report_cfg_s, dummy_present)},
    {13, (int32_t)offsetof(csi_report_cfg_s, cqi_table_present)},
    {15, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_g0_fields[] = {
    {offsetof(csi_report_cfg_s, semi_persistent_on_pusch_v1530),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1530)>::value,
     &seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1530_s_}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_g0_names[] = {{"semiPersistentOnPUSCH-v1530", nullptr}};
static const asn1::table::seq_opt    seq_table_csi_report_cfg_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_csi_report_cfg_s_g1_fields[] = {
    {offsetof(csi_report_cfg_s, semi_persistent_on_pusch_v1610),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1610)>::value,
      &seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1610_s_},
    {offsetof(csi_report_cfg_s, aperiodic_v1610),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::aperiodic_v1610)>::value,
      &seq_table_csi_report_cfg_s_aperiodic_v1610_s_},
    {offsetof(csi_report_cfg_s, report_quant_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_codec<decltype(csi_report_cfg_s::report_quant_r16), asn1::table::struct_json>::value},
    {offsetof(csi_report_cfg_s, codebook_cfg_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::codebook_cfg_r16)>::value,
      &codebook_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_g1_names[] = {{"semiPersistentOnPUSCH-v1610", nullptr},
                                                                              {"aperiodic-v1610", nullptr},
                                                                              {"reportQuantity-r16", nullptr},
                                                                              {"codebookConfig-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_csi_report_cfg_s_g1_opts[]  = {{0, asn1::table::seq_field::ptr_presence},
                                                                              {1, asn1::table::seq_field::ptr_presence},
                                                                              {2, asn1::table::seq_field::ptr_presence},
                                                                              {3, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_csi_report_cfg_s_g2_fields[] = {
    {0,
      (int32_t)offsetof(csi_report_cfg_s, cqi_bits_per_subband_r17_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr},
    {offsetof(csi_report_cfg_s, group_based_beam_report_v1710),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::group_based_beam_report_v1710)>::value,
      &seq_table_csi_report_cfg_s_group_based_beam_report_v1710_s_},
    {offsetof(csi_report_cfg_s, codebook_cfg_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::codebook_cfg_r17)>::value,
      &codebook_cfg_r17_s::desc},
    {0,
      (int32_t)offsetof(csi_report_cfg_s, shared_cmr_r17_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr},
    {offsetof(csi_report_cfg_s, csi_report_mode_r17),
      (int32_t)offsetof(csi_report_cfg_s, csi_report_mode_r17_present),
      asn1::table::enum_op<decltype(csi_report_cfg_s::csi_report_mode_r17)>(),
      decltype(csi_report_cfg_s::csi_report_mode_r17)::has_ext,
      false,
      decltype(csi_report_cfg_s::csi_report_mode_r17)::nof_types,
      decltype(csi_report_cfg_s::csi_report_mode_r17)::nof_exts,
      nullptr,
      &csi_report_cfg_s::csi_report_mode_r17_opts::desc},
    {offsetof(csi_report_cfg_s, nof_single_trp_csi_mode1_r17),
      (int32_t)offsetof(csi_report_cfg_s, nof_single_trp_csi_mode1_r17_present),
      asn1::table::enum_op<decltype(csi_report_cfg_s::nof_single_trp_csi_mode1_r17)>(),
      decltype(csi_report_cfg_s::nof_single_trp_csi_mode1_r17)::has_ext,
      false,
      decltype(csi_report_cfg_s::nof_single_trp_csi_mode1_r17)::nof_types,
      decltype(csi_report_cfg_s::nof_single_trp_csi_mode1_r17)::nof_exts,
      nullptr,
      &csi_report_cfg_s::nof_single_trp_csi_mode1_r17_opts::desc},
    {offsetof(csi_report_cfg_s, report_quant_r17),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_codec<decltype(csi_report_cfg_s::report_quant_r17), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_g2_names[] = {
    {"cqi-BitsPerSubband-r17", "bits4"},
    {"groupBasedBeamReporting-v1710", nullptr},
    {"codebookConfig-r17", nullptr},
    {"sharedCMR-r17", "enable"},
    {"csi-ReportMode-r17", nullptr},
    {"numberOfSingleTRP-CSI-Mode1-r17", nullptr},
    {"reportQuantity-r17", nullptr}};
static const asn1::table::seq_opt seq_table_csi_report_cfg_s_g2_opts[] = {
    {0, (int32_t)offsetof(csi_report_cfg_s, cqi_bits_per_subband_r17_present)},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, (int32_t)offsetof(csi_report_cfg_s, shared_cmr_r17_present)},
    {4, (int32_t)offsetof(csi_report_cfg_s, csi_report_mode_r17_present)},
    {5, (int32_t)offsetof(csi_report_cfg_s, nof_single_trp_csi_mode1_r17_present)},
    {6, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_g3_fields[] = {
    {offsetof(csi_report_cfg_s, semi_persistent_on_pusch_v1720),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::semi_persistent_on_pusch_v1720)>::value,
     &seq_table_csi_report_cfg_s_semi_persistent_on_pusch_v1720_s_},
    {offsetof(csi_report_cfg_s, aperiodic_v1720),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::aperiodic_v1720)>::value,
     &seq_table_csi_report_cfg_s_aperiodic_v1720_s_}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_g3_names[] = {{"semiPersistentOnPUSCH-v1720", nullptr},
                                                                              {"aperiodic-v1720", nullptr}};
static const asn1::table::seq_opt    seq_table_csi_report_cfg_s_g3_opts[]  = {{0, asn1::table::seq_field::ptr_presence},
                                                                              {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_csi_report_cfg_s_g4_fields[] = {
    {offsetof(csi_report_cfg_s, codebook_cfg_v1730),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::codebook_cfg_v1730)>::value,
      &codebook_cfg_v1730_s::desc}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_g4_names[] = {{"codebookConfig-v1730", nullptr}};
static const asn1::table::seq_opt    seq_table_csi_report_cfg_s_g4_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field  seq_table_csi_report_cfg_s_g5_fields[] = {
    {offsetof(csi_report_cfg_s, group_based_beam_report_v1800),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::group_based_beam_report_v1800)>::value,
      &seq_table_csi_report_cfg_s_group_based_beam_report_v1800_s_},
    {offsetof(csi_report_cfg_s, report_quant_r18),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::report_quant_r18)>::value,
      &td_cp_r18_s::desc},
    {offsetof(csi_report_cfg_s, codebook_cfg_r18),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(csi_report_cfg_s::codebook_cfg_r18)>::value,
      &codebook_cfg_r18_s::desc},
    {offsetof(csi_report_cfg_s, csi_report_sub_cfg_to_add_mod_list_r18),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      8,
      &asn1::table::ptr_item_list_codec<decltype(csi_report_cfg_s::csi_report_sub_cfg_to_add_mod_list_r18),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &csi_report_sub_cfg_r18_s::desc},
    {offsetof(csi_report_cfg_s, csi_report_sub_cfg_to_release_list_r18),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      8,
      &asn1::table::ptr_list_codec<decltype(csi_report_cfg_s::csi_report_sub_cfg_to_release_list_r18),
                                   asn1::table::int_json,
                                   asn1::table::int_items<false, INT64_C(0), UINT64_C(7), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_g5_names[] = {
    {"groupBasedBeamReporting-v1800", nullptr},
    {"reportQuantity-r18", nullptr},
    {"codebookConfig-r18", nullptr},
    {"csi-ReportSubConfigToAddModList-r18", nullptr},
    {"csi-ReportSubConfigToReleaseList-r18", nullptr}};
static const asn1::table::seq_opt   seq_table_csi_report_cfg_s_g5_opts[] = {{0, asn1::table::seq_field::ptr_presence},
                                                                            {1, asn1::table::seq_field::ptr_presence},
                                                                            {2, asn1::table::seq_field::ptr_presence},
                                                                            {3, asn1::table::seq_field::ptr_presence},
                                                                            {4, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_csi_report_cfg_s_groups[]  = {
    {{seq_table_csi_report_cfg_s_g0_fields, 1, UINT64_C(0x0), seq_table_csi_report_cfg_s_g0_opts, 1},
      false,
      seq_table_csi_report_cfg_s_g0_names},
    {{seq_table_csi_report_cfg_s_g1_fields, 4, UINT64_C(0x0), seq_table_csi_report_cfg_s_g1_opts, 4},
      false,
      seq_table_csi_report_cfg_s_g1_names},
    {{seq_table_csi_report_cfg_s_g2_fields, 7, UINT64_C(0x0), seq_table_csi_report_cfg_s_g2_opts, 7},
      false,
      seq_table_csi_report_cfg_s_g2_names},
    {{seq_table_csi_report_cfg_s_g3_fields, 2, UINT64_C(0x0), seq_table_csi_report_cfg_s_g3_opts, 2},
      false,
      seq_table_csi_report_cfg_s_g3_names},
    {{seq_table_csi_report_cfg_s_g4_fields, 1, UINT64_C(0x0), seq_table_csi_report_cfg_s_g4_opts, 1},
      false,
      seq_table_csi_report_cfg_s_g4_names},
    {{seq_table_csi_report_cfg_s_g5_fields, 5, UINT64_C(0x0), seq_table_csi_report_cfg_s_g5_opts, 5},
      false,
      seq_table_csi_report_cfg_s_g5_names}};
const asn1::table::seq_desc csi_report_cfg_s::desc = {
    {seq_table_csi_report_cfg_s_fields, 16, UINT64_C(0x5365), seq_table_csi_report_cfg_s_opts, 8},
    true,
    false,
    seq_table_csi_report_cfg_s_groups,
    6,
    seq_table_csi_report_cfg_s_names};

static const asn1::table::storage_ops* const choice_storage_csi_report_cfg_s_report_cfg_type_c__alternatives[] = {
    &asn1::table::alternative_ops<csi_report_cfg_s::report_cfg_type_c_::periodic_s_, false>::value,
    &asn1::table::alternative_ops<csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pucch_s_, false>::value,
    &asn1::table::alternative_ops<csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_, false>::value,
    &asn1::table::alternative_ops<csi_report_cfg_s::report_cfg_type_c_::aperiodic_s_, false>::value};
const asn1::table::choice_storage_desc csi_report_cfg_s::report_cfg_type_c_::storage_desc = {
    choice_storage_csi_report_cfg_s_report_cfg_type_c__alternatives,
    4,
    "csi_report_cfg_s::report_cfg_type_c_"};

csi_report_cfg_s::report_cfg_type_c_::periodic_s_& csi_report_cfg_s::report_cfg_type_c_::set_periodic()
{
  set(types::periodic);
  return c.get<periodic_s_>();
}
csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pucch_s_&
csi_report_cfg_s::report_cfg_type_c_::set_semi_persistent_on_pucch()
{
  set(types::semi_persistent_on_pucch);
  return c.get<semi_persistent_on_pucch_s_>();
}
csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_&
csi_report_cfg_s::report_cfg_type_c_::set_semi_persistent_on_pusch()
{
  set(types::semi_persistent_on_pusch);
  return c.get<semi_persistent_on_pusch_s_>();
}
csi_report_cfg_s::report_cfg_type_c_::aperiodic_s_& csi_report_cfg_s::report_cfg_type_c_::set_aperiodic()
{
  set(types::aperiodic);
  return c.get<aperiodic_s_>();
}
static const asn1::table::seq_field seq_table_csi_report_cfg_s_report_cfg_type_c__periodic_s__fields[] = {
    {offsetof(csi_report_cfg_s::report_cfg_type_c_::periodic_s_, report_slot_cfg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_report_periodicity_and_offset_c::desc},
    {offsetof(csi_report_cfg_s::report_cfg_type_c_::periodic_s_, pucch_csi_res_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(csi_report_cfg_s::report_cfg_type_c_::periodic_s_::pucch_csi_res_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &pucch_csi_res_s::desc}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_report_cfg_type_c__periodic_s__names[] = {
    {"reportSlotConfig", nullptr},
    {"pucch-CSI-ResourceList", nullptr}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_report_cfg_type_c__periodic_s_ = {
    {seq_table_csi_report_cfg_s_report_cfg_type_c__periodic_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_report_cfg_type_c__periodic_s__names};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pucch_s__fields[] =
    {{offsetof(csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pucch_s_, report_slot_cfg),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::choice,
      false,
      false,
      0,
      0,
      nullptr,
      &csi_report_periodicity_and_offset_c::desc},
     {offsetof(csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pucch_s_, pucch_csi_res_list),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      4,
      &asn1::table::item_list_codec<
          decltype(csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pucch_s_::pucch_csi_res_list),
          asn1::table::sequence_item_ops,
          false,
          true>::value,
      &pucch_csi_res_s::desc}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pucch_s__names[] =
    {{"reportSlotConfig", nullptr}, {"pucch-CSI-ResourceList", nullptr}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pucch_s_ = {
    {seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pucch_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pucch_s__names};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pusch_s__fields[] =
    {{offsetof(csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_, report_slot_cfg),
      asn1::table::seq_field::mandatory,
      asn1::table::enum_op<
          decltype(csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg)>(),
      decltype(csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg)::has_ext,
      false,
      decltype(csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg)::nof_types,
      decltype(csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg)::nof_exts,
      nullptr,
      &csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::desc},
     {offsetof(csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_, report_slot_offset_list),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      16,
      &asn1::table::list_codec<
          decltype(csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_offset_list),
          asn1::table::int_json,
          asn1::table::int_items<false, INT64_C(0), UINT64_C(32), false, false>>::value},
     {offsetof(csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_, p0alpha),
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::int_u8,
      false,
      false,
      INT64_C(0),
      UINT64_C(29),
      nullptr}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pusch_s__names[] =
    {{"reportSlotConfig", nullptr}, {"reportSlotOffsetList", nullptr}, {"p0alpha", nullptr}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pusch_s_ = {
    {seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pusch_s__fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pusch_s__names};
static const asn1::table::seq_field seq_table_csi_report_cfg_s_report_cfg_type_c__aperiodic_s__fields[] = {
    {offsetof(csi_report_cfg_s::report_cfg_type_c_::aperiodic_s_, report_slot_offset_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<decltype(csi_report_cfg_s::report_cfg_type_c_::aperiodic_s_::report_slot_offset_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(32), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_report_cfg_type_c__aperiodic_s__names[] = {
    {"reportSlotOffsetList", nullptr}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_report_cfg_type_c__aperiodic_s_ = {
    {seq_table_csi_report_cfg_s_report_cfg_type_c__aperiodic_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_report_cfg_type_c__aperiodic_s__names};
static const asn1::table::seq_field choice_table_csi_report_cfg_s_report_cfg_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_report_cfg_s_report_cfg_type_c__periodic_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pucch_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pusch_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_report_cfg_s_report_cfg_type_c__aperiodic_s_}};
static const asn1::table::field_name choice_table_csi_report_cfg_s_report_cfg_type_c__names[] = {
    {"periodic", nullptr},
    {"semiPersistentOnPUCCH", nullptr},
    {"semiPersistentOnPUSCH", nullptr},
    {"aperiodic", nullptr}};
const asn1::table::choice_desc csi_report_cfg_s::report_cfg_type_c_::desc = {
    asn1::table::enum_int_type<csi_report_cfg_s::report_cfg_type_c_::types>(),
    csi_report_cfg_s::report_cfg_type_c_::types::nof_types,
    csi_report_cfg_s::report_cfg_type_c_::types::nof_exts,
    csi_report_cfg_s::report_cfg_type_c_::types::has_ext,
    false,
    choice_table_csi_report_cfg_s_report_cfg_type_c__alternatives,
    nullptr,
    choice_table_csi_report_cfg_s_report_cfg_type_c__names,
    "csi_report_cfg_s::report_cfg_type_c_",
    offsetof(csi_report_cfg_s::report_cfg_type_c_, c.type),
    offsetof(csi_report_cfg_s::report_cfg_type_c_, c.value),
    false,
    &csi_report_cfg_s::report_cfg_type_c_::storage_desc};

static const char*
    enum_names_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pusch_s__report_slot_cfg_e__values[] =
        {"sl5", "sl10", "sl20", "sl40", "sl80", "sl160", "sl320"};
const asn1::table::enum_desc
    csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::desc = {
        enum_names_csi_report_cfg_s_report_cfg_type_c__semi_persistent_on_pusch_s__report_slot_cfg_e__values,
        7,
        "csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_e_"};
uint16_t csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_opts::to_number() const
{
  static const uint16_t numbers[] = {5, 10, 20, 40, 80, 160, 320};
  return map_enum_number(
      numbers, 7, value, "csi_report_cfg_s::report_cfg_type_c_::semi_persistent_on_pusch_s_::report_slot_cfg_e_");
}

static const char*           enum_names_csi_report_cfg_s_report_cfg_type_c__types_values[] = {"periodic",
                                                                                              "semiPersistentOnPUCCH",
                                                                                              "semiPersistentOnPUSCH",
                                                                                              "aperiodic"};
const asn1::table::enum_desc csi_report_cfg_s::report_cfg_type_c_::types_opts::desc        = {
    enum_names_csi_report_cfg_s_report_cfg_type_c__types_values,
    4,
    "csi_report_cfg_s::report_cfg_type_c_::types"};

void csi_report_cfg_s::report_quant_c_::set(types::options e)
{
  type_ = e;
}
void csi_report_cfg_s::report_quant_c_::set_none()
{
  set(types::none);
}
void csi_report_cfg_s::report_quant_c_::set_cri_ri_pmi_cqi()
{
  set(types::cri_ri_pmi_cqi);
}
void csi_report_cfg_s::report_quant_c_::set_cri_ri_i1()
{
  set(types::cri_ri_i1);
}
csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_& csi_report_cfg_s::report_quant_c_::set_cri_ri_i1_cqi()
{
  set(types::cri_ri_i1_cqi);
  return c;
}
void csi_report_cfg_s::report_quant_c_::set_cri_ri_cqi()
{
  set(types::cri_ri_cqi);
}
void csi_report_cfg_s::report_quant_c_::set_cri_rsrp()
{
  set(types::cri_rsrp);
}
void csi_report_cfg_s::report_quant_c_::set_ssb_idx_rsrp()
{
  set(types::ssb_idx_rsrp);
}
void csi_report_cfg_s::report_quant_c_::set_cri_ri_li_pmi_cqi()
{
  set(types::cri_ri_li_pmi_cqi);
}
static const asn1::table::seq_field seq_table_csi_report_cfg_s_report_quant_c__cri_ri_i1_cqi_s__fields[] = {
    {offsetof(csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_, pdsch_bundle_size_for_csi),
     (int32_t)offsetof(csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_, pdsch_bundle_size_for_csi_present),
     asn1::table::enum_op<decltype(csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi)>(),
     decltype(csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi)::has_ext,
     false,
     decltype(csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi)::nof_types,
     decltype(csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi)::nof_exts,
     nullptr,
     &csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi_opts::desc}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_report_quant_c__cri_ri_i1_cqi_s__names[] = {
    {"pdsch-BundleSizeForCSI", nullptr}};
static const asn1::table::seq_opt seq_table_csi_report_cfg_s_report_quant_c__cri_ri_i1_cqi_s__opts[] = {
    {0, (int32_t)offsetof(csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_, pdsch_bundle_size_for_csi_present)}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_report_quant_c__cri_ri_i1_cqi_s_ = {
    {seq_table_csi_report_cfg_s_report_quant_c__cri_ri_i1_cqi_s__fields,
     1,
     UINT64_C(0x0),
     seq_table_csi_report_cfg_s_report_quant_c__cri_ri_i1_cqi_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_report_quant_c__cri_ri_i1_cqi_s__names};
static const asn1::table::seq_field choice_table_csi_report_cfg_s_report_quant_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_report_cfg_s_report_quant_c__cri_ri_i1_cqi_s_},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_csi_report_cfg_s_report_quant_c__names[] = {
    {"none", nullptr},
    {"cri-RI-PMI-CQI", nullptr},
    {"cri-RI-i1", nullptr},
    {"cri-RI-i1-CQI", nullptr},
    {"cri-RI-CQI", nullptr},
    {"cri-RSRP", nullptr},
    {"ssb-Index-RSRP", nullptr},
    {"cri-RI-LI-PMI-CQI", nullptr}};
const asn1::table::choice_desc csi_report_cfg_s::report_quant_c_::desc = {
    asn1::table::enum_int_type<csi_report_cfg_s::report_quant_c_::types>(),
    csi_report_cfg_s::report_quant_c_::types::nof_types,
    csi_report_cfg_s::report_quant_c_::types::nof_exts,
    csi_report_cfg_s::report_quant_c_::types::has_ext,
    false,
    choice_table_csi_report_cfg_s_report_quant_c__alternatives,
    &asn1::table::choice_set<csi_report_cfg_s::report_quant_c_>,
    choice_table_csi_report_cfg_s_report_quant_c__names,
    "csi_report_cfg_s::report_quant_c_",
    offsetof(csi_report_cfg_s::report_quant_c_, type_),
    offsetof(csi_report_cfg_s::report_quant_c_, c),
    false};

static const char* enum_names_csi_report_cfg_s_report_quant_c__cri_ri_i1_cqi_s__pdsch_bundle_size_for_csi_e__values[] =
    {"n2", "n4"};
const asn1::table::enum_desc csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi_opts::desc =
    {enum_names_csi_report_cfg_s_report_quant_c__cri_ri_i1_cqi_s__pdsch_bundle_size_for_csi_e__values,
     2,
     "csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi_e_"};
uint8_t csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4};
  return map_enum_number(
      numbers, 2, value, "csi_report_cfg_s::report_quant_c_::cri_ri_i1_cqi_s_::pdsch_bundle_size_for_csi_e_");
}

static const char*           enum_names_csi_report_cfg_s_report_quant_c__types_values[] = {"none",
                                                                                           "cri-RI-PMI-CQI",
                                                                                           "cri-RI-i1",
                                                                                           "cri-RI-i1-CQI",
                                                                                           "cri-RI-CQI",
                                                                                           "cri-RSRP",
                                                                                           "ssb-Index-RSRP",
                                                                                           "cri-RI-LI-PMI-CQI"};
const asn1::table::enum_desc csi_report_cfg_s::report_quant_c_::types_opts::desc        = {
    enum_names_csi_report_cfg_s_report_quant_c__types_values,
    8,
    "csi_report_cfg_s::report_quant_c_::types"};

static const char*           enum_names_csi_report_cfg_s_report_freq_cfg_s__cqi_format_ind_e__values[] = {"widebandCQI",
                                                                                                          "subbandCQI"};
const asn1::table::enum_desc csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_opts::desc           = {
    enum_names_csi_report_cfg_s_report_freq_cfg_s__cqi_format_ind_e__values,
    2,
    "csi_report_cfg_s::report_freq_cfg_s_::cqi_format_ind_e_"};

static const char*           enum_names_csi_report_cfg_s_report_freq_cfg_s__pmi_format_ind_e__values[] = {"widebandPMI",
                                                                                                          "subbandPMI"};
const asn1::table::enum_desc csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind_opts::desc           = {
    enum_names_csi_report_cfg_s_report_freq_cfg_s__pmi_format_ind_e__values,
    2,
    "csi_report_cfg_s::report_freq_cfg_s_::pmi_format_ind_e_"};

static const asn1::table::storage_ops* const
    choice_storage_csi_report_cfg_s_report_freq_cfg_s__csi_report_band_c__alternatives[] = {
        &asn1::table::alternative_ops<fixed_bitstring<3>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<4>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<5>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<6>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<7>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<8>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<9>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<10>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<11>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<12>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<13>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<14>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<15>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<16>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<17>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<18>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<19>, false>::value};
const asn1::table::choice_storage_desc csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::storage_desc = {
    choice_storage_csi_report_cfg_s_report_freq_cfg_s__csi_report_band_c__alternatives,
    17,
    "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_"};

fixed_bitstring<3>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands3()
{
  set(types::subbands3);
  return c.get<fixed_bitstring<3>>();
}
fixed_bitstring<4>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands4()
{
  set(types::subbands4);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<5>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands5()
{
  set(types::subbands5);
  return c.get<fixed_bitstring<5>>();
}
fixed_bitstring<6>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands6()
{
  set(types::subbands6);
  return c.get<fixed_bitstring<6>>();
}
fixed_bitstring<7>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands7()
{
  set(types::subbands7);
  return c.get<fixed_bitstring<7>>();
}
fixed_bitstring<8>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands8()
{
  set(types::subbands8);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<9>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands9()
{
  set(types::subbands9);
  return c.get<fixed_bitstring<9>>();
}
fixed_bitstring<10>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands10()
{
  set(types::subbands10);
  return c.get<fixed_bitstring<10>>();
}
fixed_bitstring<11>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands11()
{
  set(types::subbands11);
  return c.get<fixed_bitstring<11>>();
}
fixed_bitstring<12>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands12()
{
  set(types::subbands12);
  return c.get<fixed_bitstring<12>>();
}
fixed_bitstring<13>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands13()
{
  set(types::subbands13);
  return c.get<fixed_bitstring<13>>();
}
fixed_bitstring<14>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands14()
{
  set(types::subbands14);
  return c.get<fixed_bitstring<14>>();
}
fixed_bitstring<15>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands15()
{
  set(types::subbands15);
  return c.get<fixed_bitstring<15>>();
}
fixed_bitstring<16>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands16()
{
  set(types::subbands16);
  return c.get<fixed_bitstring<16>>();
}
fixed_bitstring<17>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands17()
{
  set(types::subbands17);
  return c.get<fixed_bitstring<17>>();
}
fixed_bitstring<18>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands18()
{
  set(types::subbands18);
  return c.get<fixed_bitstring<18>>();
}
fixed_bitstring<19>& csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::set_subbands19_v1530()
{
  set(types::subbands19_v1530);
  return c.get<fixed_bitstring<19>>();
}
static const asn1::table::seq_field choice_table_csi_report_cfg_s_report_freq_cfg_s__csi_report_band_c__alternatives[] =
    {{0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<fixed_bitstring<3>, asn1::table::struct_json>::value},
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
      &asn1::table::object_codec<fixed_bitstring<5>, asn1::table::struct_json>::value},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<fixed_bitstring<6>, asn1::table::struct_json>::value},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<fixed_bitstring<7>, asn1::table::struct_json>::value},
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
      &asn1::table::object_codec<fixed_bitstring<9>, asn1::table::struct_json>::value},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<fixed_bitstring<10>, asn1::table::struct_json>::value},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<fixed_bitstring<11>, asn1::table::struct_json>::value},
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
      &asn1::table::object_codec<fixed_bitstring<13>, asn1::table::struct_json>::value},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<fixed_bitstring<14>, asn1::table::struct_json>::value},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<fixed_bitstring<15>, asn1::table::struct_json>::value},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<fixed_bitstring<16>, asn1::table::struct_json>::value},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<fixed_bitstring<17>, asn1::table::struct_json>::value},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<fixed_bitstring<18>, asn1::table::struct_json>::value},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      0,
      0,
      &asn1::table::object_codec<fixed_bitstring<19>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_csi_report_cfg_s_report_freq_cfg_s__csi_report_band_c__names[] = {
    {"subbands3", nullptr},
    {"subbands4", nullptr},
    {"subbands5", nullptr},
    {"subbands6", nullptr},
    {"subbands7", nullptr},
    {"subbands8", nullptr},
    {"subbands9", nullptr},
    {"subbands10", nullptr},
    {"subbands11", nullptr},
    {"subbands12", nullptr},
    {"subbands13", nullptr},
    {"subbands14", nullptr},
    {"subbands15", nullptr},
    {"subbands16", nullptr},
    {"subbands17", nullptr},
    {"subbands18", nullptr},
    {"subbands19-v1530", nullptr}};
const asn1::table::choice_desc csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::desc = {
    asn1::table::enum_int_type<csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types>(),
    csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types::nof_types,
    csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types::nof_exts,
    csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types::has_ext,
    false,
    choice_table_csi_report_cfg_s_report_freq_cfg_s__csi_report_band_c__alternatives,
    nullptr,
    choice_table_csi_report_cfg_s_report_freq_cfg_s__csi_report_band_c__names,
    "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_",
    offsetof(csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_, c.type),
    offsetof(csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_, c.value),
    false,
    &csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::storage_desc};

static const char* enum_names_csi_report_cfg_s_report_freq_cfg_s__csi_report_band_c__types_values[] = {
    "subbands3",
    "subbands4",
    "subbands5",
    "subbands6",
    "subbands7",
    "subbands8",
    "subbands9",
    "subbands10",
    "subbands11",
    "subbands12",
    "subbands13",
    "subbands14",
    "subbands15",
    "subbands16",
    "subbands17",
    "subbands18",
    "subbands19-v1530"};
const asn1::table::enum_desc csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types_opts::desc = {
    enum_names_csi_report_cfg_s_report_freq_cfg_s__csi_report_band_c__types_values,
    17,
    "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types"};
uint8_t csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
  return map_enum_number(numbers, 17, value, "csi_report_cfg_s::report_freq_cfg_s_::csi_report_band_c_::types");
}

static const char* enum_names_csi_report_cfg_s_time_restrict_for_ch_meass_e__values[] = {"configured", "notConfigured"};
const asn1::table::enum_desc csi_report_cfg_s::time_restrict_for_ch_meass_opts::desc  = {
    enum_names_csi_report_cfg_s_time_restrict_for_ch_meass_e__values,
    2,
    "csi_report_cfg_s::time_restrict_for_ch_meass_e_"};

static const char* enum_names_csi_report_cfg_s_time_restrict_for_interference_meass_e__values[] = {"configured",
                                                                                                   "notConfigured"};
const asn1::table::enum_desc csi_report_cfg_s::time_restrict_for_interference_meass_opts::desc  = {
    enum_names_csi_report_cfg_s_time_restrict_for_interference_meass_e__values,
    2,
    "csi_report_cfg_s::time_restrict_for_interference_meass_e_"};

static const char*           enum_names_csi_report_cfg_s_dummy_e__values[] = {"n1", "n2"};
const asn1::table::enum_desc csi_report_cfg_s::dummy_opts::desc = {enum_names_csi_report_cfg_s_dummy_e__values,
                                                                   2,
                                                                   "csi_report_cfg_s::dummy_e_"};
uint8_t                      csi_report_cfg_s::dummy_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "csi_report_cfg_s::dummy_e_");
}

void csi_report_cfg_s::group_based_beam_report_c_::set(types::options e)
{
  type_ = e;
}
void csi_report_cfg_s::group_based_beam_report_c_::set_enabled()
{
  set(types::enabled);
}
csi_report_cfg_s::group_based_beam_report_c_::disabled_s_& csi_report_cfg_s::group_based_beam_report_c_::set_disabled()
{
  set(types::disabled);
  return c;
}
static const asn1::table::seq_field seq_table_csi_report_cfg_s_group_based_beam_report_c__disabled_s__fields[] = {
    {offsetof(csi_report_cfg_s::group_based_beam_report_c_::disabled_s_, nrof_reported_rs),
     (int32_t)offsetof(csi_report_cfg_s::group_based_beam_report_c_::disabled_s_, nrof_reported_rs_present),
     asn1::table::enum_op<decltype(csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs)>(),
     decltype(csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs)::has_ext,
     false,
     decltype(csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs)::nof_types,
     decltype(csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs)::nof_exts,
     nullptr,
     &csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_opts::desc}};
static const asn1::table::field_name seq_table_csi_report_cfg_s_group_based_beam_report_c__disabled_s__names[] = {
    {"nrofReportedRS", nullptr}};
static const asn1::table::seq_opt seq_table_csi_report_cfg_s_group_based_beam_report_c__disabled_s__opts[] = {
    {0, (int32_t)offsetof(csi_report_cfg_s::group_based_beam_report_c_::disabled_s_, nrof_reported_rs_present)}};
static const asn1::table::seq_desc seq_table_csi_report_cfg_s_group_based_beam_report_c__disabled_s_ = {
    {seq_table_csi_report_cfg_s_group_based_beam_report_c__disabled_s__fields,
     1,
     UINT64_C(0x0),
     seq_table_csi_report_cfg_s_group_based_beam_report_c__disabled_s__opts,
     1},
    false,
    false,
    nullptr,
    0,
    seq_table_csi_report_cfg_s_group_based_beam_report_c__disabled_s__names};
static const asn1::table::seq_field choice_table_csi_report_cfg_s_group_based_beam_report_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_report_cfg_s_group_based_beam_report_c__disabled_s_}};
static const asn1::table::field_name choice_table_csi_report_cfg_s_group_based_beam_report_c__names[] = {
    {"enabled", nullptr},
    {"disabled", nullptr}};
const asn1::table::choice_desc csi_report_cfg_s::group_based_beam_report_c_::desc = {
    asn1::table::enum_int_type<csi_report_cfg_s::group_based_beam_report_c_::types>(),
    csi_report_cfg_s::group_based_beam_report_c_::types::nof_types,
    csi_report_cfg_s::group_based_beam_report_c_::types::nof_exts,
    csi_report_cfg_s::group_based_beam_report_c_::types::has_ext,
    false,
    choice_table_csi_report_cfg_s_group_based_beam_report_c__alternatives,
    &asn1::table::choice_set<csi_report_cfg_s::group_based_beam_report_c_>,
    choice_table_csi_report_cfg_s_group_based_beam_report_c__names,
    "csi_report_cfg_s::group_based_beam_report_c_",
    offsetof(csi_report_cfg_s::group_based_beam_report_c_, type_),
    offsetof(csi_report_cfg_s::group_based_beam_report_c_, c),
    false};

static const char* enum_names_csi_report_cfg_s_group_based_beam_report_c__disabled_s__nrof_reported_rs_e__values[] =
    {"n1", "n2", "n3", "n4"};
const asn1::table::enum_desc csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_opts::desc = {
    enum_names_csi_report_cfg_s_group_based_beam_report_c__disabled_s__nrof_reported_rs_e__values,
    4,
    "csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_e_"};
uint8_t csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(
      numbers, 4, value, "csi_report_cfg_s::group_based_beam_report_c_::disabled_s_::nrof_reported_rs_e_");
}

static const char* enum_names_csi_report_cfg_s_group_based_beam_report_c__types_values[]    = {"enabled", "disabled"};
const asn1::table::enum_desc csi_report_cfg_s::group_based_beam_report_c_::types_opts::desc = {
    enum_names_csi_report_cfg_s_group_based_beam_report_c__types_values,
    2,
    "csi_report_cfg_s::group_based_beam_report_c_::types"};

static const char* enum_names_csi_report_cfg_s_cqi_table_e__values[] = {"table1", "table2", "table3", "table4-r17"};
const asn1::table::enum_desc csi_report_cfg_s::cqi_table_opts::desc  = {enum_names_csi_report_cfg_s_cqi_table_e__values,
                                                                        4,
                                                                        "csi_report_cfg_s::cqi_table_e_"};
uint8_t                      csi_report_cfg_s::cqi_table_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "csi_report_cfg_s::cqi_table_e_");
}

static const char*           enum_names_csi_report_cfg_s_subband_size_e__values[] = {"value1", "value2"};
const asn1::table::enum_desc csi_report_cfg_s::subband_size_opts::desc            = {
    enum_names_csi_report_cfg_s_subband_size_e__values,
    2,
    "csi_report_cfg_s::subband_size_e_"};
uint8_t csi_report_cfg_s::subband_size_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "csi_report_cfg_s::subband_size_e_");
}

static const char* enum_names_csi_report_cfg_s_semi_persistent_on_pusch_v1530_s__report_slot_cfg_v1530_e__values[] = {
    "sl4",
    "sl8",
    "sl16"};
const asn1::table::enum_desc csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530_opts::desc = {
    enum_names_csi_report_cfg_s_semi_persistent_on_pusch_v1530_s__report_slot_cfg_v1530_e__values,
    3,
    "csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530_e_"};
uint8_t csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530_opts::to_number() const
{
  static const uint8_t numbers[] = {4, 8, 16};
  return map_enum_number(
      numbers, 3, value, "csi_report_cfg_s::semi_persistent_on_pusch_v1530_s_::report_slot_cfg_v1530_e_");
}

void csi_report_cfg_s::report_quant_r16_c_::set(types::options e)
{
  type_ = e;
}
void csi_report_cfg_s::report_quant_r16_c_::set_cri_sinr_r16()
{
  set(types::cri_sinr_r16);
}
void csi_report_cfg_s::report_quant_r16_c_::set_ssb_idx_sinr_r16()
{
  set(types::ssb_idx_sinr_r16);
}
static const asn1::table::seq_field choice_table_csi_report_cfg_s_report_quant_r16_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_csi_report_cfg_s_report_quant_r16_c__names[] = {
    {"cri-SINR-r16", nullptr},
    {"ssb-Index-SINR-r16", nullptr}};
const asn1::table::choice_desc csi_report_cfg_s::report_quant_r16_c_::desc = {
    asn1::table::enum_int_type<csi_report_cfg_s::report_quant_r16_c_::types>(),
    csi_report_cfg_s::report_quant_r16_c_::types::nof_types,
    csi_report_cfg_s::report_quant_r16_c_::types::nof_exts,
    csi_report_cfg_s::report_quant_r16_c_::types::has_ext,
    false,
    choice_table_csi_report_cfg_s_report_quant_r16_c__alternatives,
    &asn1::table::choice_set<csi_report_cfg_s::report_quant_r16_c_>,
    choice_table_csi_report_cfg_s_report_quant_r16_c__names,
    "csi_report_cfg_s::report_quant_r16_c_",
    offsetof(csi_report_cfg_s::report_quant_r16_c_, type_),
    asn1::table::choice_desc::no_member,
    false};

static const char*           enum_names_csi_report_cfg_s_report_quant_r16_c__types_values[] = {"cri-SINR-r16",
                                                                                               "ssb-Index-SINR-r16"};
const asn1::table::enum_desc csi_report_cfg_s::report_quant_r16_c_::types_opts::desc        = {
    enum_names_csi_report_cfg_s_report_quant_r16_c__types_values,
    2,
    "csi_report_cfg_s::report_quant_r16_c_::types"};

static const char* enum_names_csi_report_cfg_s_group_based_beam_report_v1710_s__nrof_reported_groups_r17_e__values[] =
    {"n1", "n2", "n3", "n4"};
const asn1::table::enum_desc csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17_opts::desc = {
    enum_names_csi_report_cfg_s_group_based_beam_report_v1710_s__nrof_reported_groups_r17_e__values,
    4,
    "csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17_e_"};
uint8_t csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(
      numbers, 4, value, "csi_report_cfg_s::group_based_beam_report_v1710_s_::nrof_reported_groups_r17_e_");
}

static const char*           enum_names_csi_report_cfg_s_csi_report_mode_r17_e__values[] = {"mode1", "mode2"};
const asn1::table::enum_desc csi_report_cfg_s::csi_report_mode_r17_opts::desc            = {
    enum_names_csi_report_cfg_s_csi_report_mode_r17_e__values,
    2,
    "csi_report_cfg_s::csi_report_mode_r17_e_"};
uint8_t csi_report_cfg_s::csi_report_mode_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "csi_report_cfg_s::csi_report_mode_r17_e_");
}

static const char*           enum_names_csi_report_cfg_s_nof_single_trp_csi_mode1_r17_e__values[] = {"n0", "n1", "n2"};
const asn1::table::enum_desc csi_report_cfg_s::nof_single_trp_csi_mode1_r17_opts::desc            = {
    enum_names_csi_report_cfg_s_nof_single_trp_csi_mode1_r17_e__values,
    3,
    "csi_report_cfg_s::nof_single_trp_csi_mode1_r17_e_"};
uint8_t csi_report_cfg_s::nof_single_trp_csi_mode1_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2};
  return map_enum_number(numbers, 3, value, "csi_report_cfg_s::nof_single_trp_csi_mode1_r17_e_");
}

void csi_report_cfg_s::report_quant_r17_c_::set(types::options e)
{
  type_ = e;
}
void csi_report_cfg_s::report_quant_r17_c_::set_cri_rsrp_idx_r17()
{
  set(types::cri_rsrp_idx_r17);
}
void csi_report_cfg_s::report_quant_r17_c_::set_ssb_idx_rsrp_idx_r17()
{
  set(types::ssb_idx_rsrp_idx_r17);
}
void csi_report_cfg_s::report_quant_r17_c_::set_cri_sinr_idx_r17()
{
  set(types::cri_sinr_idx_r17);
}
void csi_report_cfg_s::report_quant_r17_c_::set_ssb_idx_sinr_idx_r17()
{
  set(types::ssb_idx_sinr_idx_r17);
}
static const asn1::table::seq_field choice_table_csi_report_cfg_s_report_quant_r17_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_csi_report_cfg_s_report_quant_r17_c__names[] = {
    {"cri-RSRP-Index-r17", nullptr},
    {"ssb-Index-RSRP-Index-r17", nullptr},
    {"cri-SINR-Index-r17", nullptr},
    {"ssb-Index-SINR-Index-r17", nullptr}};
const asn1::table::choice_desc csi_report_cfg_s::report_quant_r17_c_::desc = {
    asn1::table::enum_int_type<csi_report_cfg_s::report_quant_r17_c_::types>(),
    csi_report_cfg_s::report_quant_r17_c_::types::nof_types,
    csi_report_cfg_s::report_quant_r17_c_::types::nof_exts,
    csi_report_cfg_s::report_quant_r17_c_::types::has_ext,
    false,
    choice_table_csi_report_cfg_s_report_quant_r17_c__alternatives,
    &asn1::table::choice_set<csi_report_cfg_s::report_quant_r17_c_>,
    choice_table_csi_report_cfg_s_report_quant_r17_c__names,
    "csi_report_cfg_s::report_quant_r17_c_",
    offsetof(csi_report_cfg_s::report_quant_r17_c_, type_),
    asn1::table::choice_desc::no_member,
    false};

static const char*           enum_names_csi_report_cfg_s_report_quant_r17_c__types_values[] = {"cri-RSRP-Index-r17",
                                                                                               "ssb-Index-RSRP-Index-r17",
                                                                                               "cri-SINR-Index-r17",
                                                                                               "ssb-Index-SINR-Index-r17"};
const asn1::table::enum_desc csi_report_cfg_s::report_quant_r17_c_::types_opts::desc        = {
    enum_names_csi_report_cfg_s_report_quant_r17_c__types_values,
    4,
    "csi_report_cfg_s::report_quant_r17_c_::types"};

static const char* enum_names_csi_report_cfg_s_group_based_beam_report_v1800_s__report_mode_r18_e__values[] = {
    "jointULDL",
    "onlyUL"};
const asn1::table::enum_desc csi_report_cfg_s::group_based_beam_report_v1800_s_::report_mode_r18_opts::desc = {
    enum_names_csi_report_cfg_s_group_based_beam_report_v1800_s__report_mode_r18_e__values,
    2,
    "csi_report_cfg_s::group_based_beam_report_v1800_s_::report_mode_r18_e_"};

// CSI-SemiPersistentOnPUSCH-TriggerState ::= SEQUENCE
static const asn1::table::seq_field seq_table_csi_semi_persistent_on_pusch_trigger_state_s_fields[] = {
    {offsetof(csi_semi_persistent_on_pusch_trigger_state_s, associated_report_cfg_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(47),
     nullptr}};
static const asn1::table::field_name seq_table_csi_semi_persistent_on_pusch_trigger_state_s_names[] = {
    {"associatedReportConfigInfo", nullptr}};
static const asn1::table::seq_field seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g0_fields[] = {
    {0,
     (int32_t)offsetof(csi_semi_persistent_on_pusch_trigger_state_s, sp_csi_mux_mode_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g0_names[] = {
    {"sp-CSI-MultiplexingMode-r17", "enabled"}};
static const asn1::table::seq_opt seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g0_opts[] = {
    {0, (int32_t)offsetof(csi_semi_persistent_on_pusch_trigger_state_s, sp_csi_mux_mode_r17_present)}};
static const asn1::table::seq_field seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g1_fields[] = {
    {offsetof(csi_semi_persistent_on_pusch_trigger_state_s, csi_report_sub_cfg_trigger_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::ptr_list_codec<
         decltype(csi_semi_persistent_on_pusch_trigger_state_s::csi_report_sub_cfg_trigger_list_r18),
         asn1::table::int_json,
         asn1::table::int_items<false, INT64_C(0), UINT64_C(7), false, false>>::value},
    {offsetof(csi_semi_persistent_on_pusch_trigger_state_s, ltm_associated_report_cfg_info_r18),
     (int32_t)offsetof(csi_semi_persistent_on_pusch_trigger_state_s, ltm_associated_report_cfg_info_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(47),
     nullptr}};
static const asn1::table::field_name seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g1_names[] = {
    {"csi-ReportSubConfigTriggerList-r18", nullptr},
    {"ltm-AssociatedReportConfigInfo-r18", nullptr}};
static const asn1::table::seq_opt seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(csi_semi_persistent_on_pusch_trigger_state_s, ltm_associated_report_cfg_info_r18_present)}};
static const asn1::table::seq_group seq_table_csi_semi_persistent_on_pusch_trigger_state_s_groups[] = {
    {{seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g0_fields,
      1,
      UINT64_C(0x0),
      seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g0_opts,
      1},
     false,
     seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g0_names},
    {{seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g1_fields,
      2,
      UINT64_C(0x0),
      seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g1_opts,
      2},
     false,
     seq_table_csi_semi_persistent_on_pusch_trigger_state_s_g1_names}};
const asn1::table::seq_desc csi_semi_persistent_on_pusch_trigger_state_s::desc = {
    {seq_table_csi_semi_persistent_on_pusch_trigger_state_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    seq_table_csi_semi_persistent_on_pusch_trigger_state_s_groups,
    2,
    seq_table_csi_semi_persistent_on_pusch_trigger_state_s_names};

// SCellActivationRS-Config-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_scell_activation_rs_cfg_r17_s_fields[] = {
    {offsetof(scell_activation_rs_cfg_r17_s, scell_activation_rs_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(1),
     UINT64_C(255),
     nullptr},
    {offsetof(scell_activation_rs_cfg_r17_s, res_set_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(scell_activation_rs_cfg_r17_s, gap_between_bursts_r17),
     (int32_t)offsetof(scell_activation_rs_cfg_r17_s, gap_between_bursts_r17_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(2),
     UINT64_C(31),
     nullptr},
    {offsetof(scell_activation_rs_cfg_r17_s, qcl_info_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr}};
static const asn1::table::field_name seq_table_scell_activation_rs_cfg_r17_s_names[] = {
    {"scellActivationRS-Id-r17", nullptr},
    {"resourceSet-r17", nullptr},
    {"gapBetweenBursts-r17", nullptr},
    {"qcl-Info-r17", nullptr}};
static const asn1::table::seq_opt seq_table_scell_activation_rs_cfg_r17_s_opts[] = {
    {2, (int32_t)offsetof(scell_activation_rs_cfg_r17_s, gap_between_bursts_r17_present)}};
const asn1::table::seq_desc scell_activation_rs_cfg_r17_s::desc = {
    {seq_table_scell_activation_rs_cfg_r17_s_fields, 4, UINT64_C(0xb), seq_table_scell_activation_rs_cfg_r17_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_scell_activation_rs_cfg_r17_s_names};

// LTM-ReportContent-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ltm_report_content_r18_s_fields[] = {
    {offsetof(ltm_report_content_r18_s, nr_of_reported_cells_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ltm_report_content_r18_s::nr_of_reported_cells_r18)>(),
     decltype(ltm_report_content_r18_s::nr_of_reported_cells_r18)::has_ext,
     false,
     decltype(ltm_report_content_r18_s::nr_of_reported_cells_r18)::nof_types,
     decltype(ltm_report_content_r18_s::nr_of_reported_cells_r18)::nof_exts,
     nullptr,
     &ltm_report_content_r18_s::nr_of_reported_cells_r18_opts::desc},
    {offsetof(ltm_report_content_r18_s, nr_of_reported_rs_per_cell_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ltm_report_content_r18_s::nr_of_reported_rs_per_cell_r18)>(),
     decltype(ltm_report_content_r18_s::nr_of_reported_rs_per_cell_r18)::has_ext,
     false,
     decltype(ltm_report_content_r18_s::nr_of_reported_rs_per_cell_r18)::nof_types,
     decltype(ltm_report_content_r18_s::nr_of_reported_rs_per_cell_r18)::nof_exts,
     nullptr,
     &ltm_report_content_r18_s::nr_of_reported_rs_per_cell_r18_opts::desc},
    {0,
     (int32_t)offsetof(ltm_report_content_r18_s, sp_cell_inclusion_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ltm_report_content_r18_s_names[] = {
    {"nrOfReportedCells-r18", nullptr},
    {"nrOfReportedRS-PerCell-r18", nullptr},
    {"spCellInclusion-r18", "true"}};
static const asn1::table::seq_opt seq_table_ltm_report_content_r18_s_opts[] = {
    {2, (int32_t)offsetof(ltm_report_content_r18_s, sp_cell_inclusion_r18_present)}};
const asn1::table::seq_desc ltm_report_content_r18_s::desc = {
    {seq_table_ltm_report_content_r18_s_fields, 3, UINT64_C(0x3), seq_table_ltm_report_content_r18_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_ltm_report_content_r18_s_names};

static const char* enum_names_ltm_report_content_r18_s_nr_of_reported_cells_r18_e__values[] = {"n1", "n2", "n3", "n4"};
const asn1::table::enum_desc ltm_report_content_r18_s::nr_of_reported_cells_r18_opts::desc  = {
    enum_names_ltm_report_content_r18_s_nr_of_reported_cells_r18_e__values,
    4,
    "ltm_report_content_r18_s::nr_of_reported_cells_r18_e_"};
uint8_t ltm_report_content_r18_s::nr_of_reported_cells_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "ltm_report_content_r18_s::nr_of_reported_cells_r18_e_");
}

static const char*           enum_names_ltm_report_content_r18_s_nr_of_reported_rs_per_cell_r18_e__values[] = {"n1",
                                                                                                               "n2",
                                                                                                               "n3",
                                                                                                               "n4"};
const asn1::table::enum_desc ltm_report_content_r18_s::nr_of_reported_rs_per_cell_r18_opts::desc            = {
    enum_names_ltm_report_content_r18_s_nr_of_reported_rs_per_cell_r18_e__values,
    4,
    "ltm_report_content_r18_s::nr_of_reported_rs_per_cell_r18_e_"};
uint8_t ltm_report_content_r18_s::nr_of_reported_rs_per_cell_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "ltm_report_content_r18_s::nr_of_reported_rs_per_cell_r18_e_");
}

// LTM-CSI-ReportConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ltm_csi_report_cfg_r18_s_fields[] = {
    {offsetof(ltm_csi_report_cfg_r18_s, ltm_csi_report_cfg_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(47),
     nullptr},
    {offsetof(ltm_csi_report_cfg_r18_s, ltm_res_for_ch_meas_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(111),
     nullptr},
    {offsetof(ltm_csi_report_cfg_r18_s, ltm_report_cfg_type_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::desc},
    {offsetof(ltm_csi_report_cfg_r18_s, ltm_report_content_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ltm_report_content_r18_s::desc}};
static const asn1::table::field_name seq_table_ltm_csi_report_cfg_r18_s_names[] = {
    {"ltm-CSI-ReportConfigId-r18", nullptr},
    {"ltm-ResourcesForChannelMeasurement-r18", nullptr},
    {"ltm-ReportConfigType-r18", nullptr},
    {"ltm-ReportContent-r18", nullptr}};
const asn1::table::seq_desc ltm_csi_report_cfg_r18_s::desc = {
    {seq_table_ltm_csi_report_cfg_r18_s_fields, 4, UINT64_C(0xf), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_ltm_csi_report_cfg_r18_s_names};

static const asn1::table::storage_ops* const
    choice_storage_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::periodic_r18_s_,
                                      false>::value,
        &asn1::table::alternative_ops<
            ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::semi_persistent_on_pucch_r18_s_,
            false>::value,
        &asn1::table::alternative_ops<
            ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::semi_persistent_on_pusch_r18_s_,
            false>::value,
        &asn1::table::alternative_ops<ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::aperiodic_r18_s_,
                                      false>::value};
const asn1::table::choice_storage_desc ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::storage_desc = {
    choice_storage_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__alternatives,
    4,
    "ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_"};

ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::periodic_r18_s_&
ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::set_periodic_r18()
{
  set(types::periodic_r18);
  return c.get<periodic_r18_s_>();
}
ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::semi_persistent_on_pucch_r18_s_&
ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::set_semi_persistent_on_pucch_r18()
{
  set(types::semi_persistent_on_pucch_r18);
  return c.get<semi_persistent_on_pucch_r18_s_>();
}
ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::semi_persistent_on_pusch_r18_s_&
ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::set_semi_persistent_on_pusch_r18()
{
  set(types::semi_persistent_on_pusch_r18);
  return c.get<semi_persistent_on_pusch_r18_s_>();
}
ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::aperiodic_r18_s_&
ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::set_aperiodic_r18()
{
  set(types::aperiodic_r18);
  return c.get<aperiodic_r18_s_>();
}
static const asn1::table::seq_field
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__periodic_r18_s__fields[] = {
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::periodic_r18_s_, report_slot_cfg_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &csi_report_periodicity_and_offset_c::desc},
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::periodic_r18_s_, pucch_csi_res_list_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         4,
         &asn1::table::item_list_codec<
             decltype(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::periodic_r18_s_::pucch_csi_res_list_r18),
             asn1::table::sequence_item_ops,
             false,
             true>::value,
         &pucch_csi_res_s::desc}};
static const asn1::table::field_name
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__periodic_r18_s__names[] = {
        {"reportSlotConfig-r18", nullptr},
        {"pucch-CSI-ResourceList-r18", nullptr}};
static const asn1::table::seq_desc seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__periodic_r18_s_ = {
    {seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__periodic_r18_s__fields,
     2,
     UINT64_C(0x3),
     nullptr,
     0},
    false,
    false,
    nullptr,
    0,
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__periodic_r18_s__names};
static const asn1::table::seq_field
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pucch_r18_s__fields[] = {
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::semi_persistent_on_pucch_r18_s_,
                  report_slot_cfg_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &csi_report_periodicity_and_offset_c::desc},
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::semi_persistent_on_pucch_r18_s_,
                  pucch_csi_res_list_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         4,
         &asn1::table::item_list_codec<decltype(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::
                                                    semi_persistent_on_pucch_r18_s_::pucch_csi_res_list_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
         &pucch_csi_res_s::desc}};
static const asn1::table::field_name
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pucch_r18_s__names[] = {
        {"reportSlotConfig-r18", nullptr},
        {"pucch-CSI-ResourceList-r18", nullptr}};
static const asn1::table::seq_desc
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pucch_r18_s_ = {
        {seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pucch_r18_s__fields,
         2,
         UINT64_C(0x3),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pucch_r18_s__names};
static const asn1::table::seq_field
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pusch_r18_s__fields[] = {
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::semi_persistent_on_pusch_r18_s_,
                  report_slot_cfg_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &csi_report_periodicity_and_offset_c::desc},
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::semi_persistent_on_pusch_r18_s_,
                  report_slot_offset_list_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         64,
         &asn1::table::list_codec<decltype(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::
                                               semi_persistent_on_pusch_r18_s_::report_slot_offset_list_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value},
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::semi_persistent_on_pusch_r18_s_,
                  report_slot_offset_list_dci_0_2_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         64,
         &asn1::table::list_codec<decltype(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::
                                               semi_persistent_on_pusch_r18_s_::report_slot_offset_list_dci_0_2_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value},
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::semi_persistent_on_pusch_r18_s_,
                  report_slot_offset_list_dci_0_1_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         64,
         &asn1::table::list_codec<decltype(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::
                                               semi_persistent_on_pusch_r18_s_::report_slot_offset_list_dci_0_1_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value},
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::semi_persistent_on_pusch_r18_s_, p0alpha_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(0),
         UINT64_C(29),
         nullptr}};
static const asn1::table::field_name
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pusch_r18_s__names[] = {
        {"reportSlotConfig-r18", nullptr},
        {"reportSlotOffsetList-r18", nullptr},
        {"reportSlotOffsetListDCI-0-2-r18", nullptr},
        {"reportSlotOffsetListDCI-0-1-r18", nullptr},
        {"p0alpha-r18", nullptr}};
static const asn1::table::seq_desc
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pusch_r18_s_ = {
        {seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pusch_r18_s__fields,
         5,
         UINT64_C(0x1f),
         nullptr,
         0},
        false,
        false,
        nullptr,
        0,
        seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pusch_r18_s__names};
static const asn1::table::seq_field
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__aperiodic_r18_s__fields[] = {
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::aperiodic_r18_s_, report_slot_offset_list_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         64,
         &asn1::table::list_codec<decltype(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::aperiodic_r18_s_::
                                               report_slot_offset_list_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value},
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::aperiodic_r18_s_,
                  report_slot_offset_list_dci_0_2_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         64,
         &asn1::table::list_codec<decltype(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::aperiodic_r18_s_::
                                               report_slot_offset_list_dci_0_2_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value},
        {offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::aperiodic_r18_s_,
                  report_slot_offset_list_dci_0_1_r18),
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         64,
         &asn1::table::list_codec<decltype(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::aperiodic_r18_s_::
                                               report_slot_offset_list_dci_0_1_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(0), UINT64_C(128), false, false>>::value}};
static const asn1::table::field_name
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__aperiodic_r18_s__names[] = {
        {"reportSlotOffsetList-r18", nullptr},
        {"reportSlotOffsetListDCI-0-2-r18", nullptr},
        {"reportSlotOffsetListDCI-0-1-r18", nullptr}};
static const asn1::table::seq_desc seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__aperiodic_r18_s_ = {
    {seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__aperiodic_r18_s__fields,
     3,
     UINT64_C(0x7),
     nullptr,
     0},
    false,
    false,
    nullptr,
    0,
    seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__aperiodic_r18_s__names};
static const asn1::table::seq_field choice_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__periodic_r18_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pucch_r18_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__semi_persistent_on_pusch_r18_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__aperiodic_r18_s_}};
static const asn1::table::field_name choice_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__names[] = {
    {"periodic-r18", nullptr},
    {"semiPersistentOnPUCCH-r18", nullptr},
    {"semiPersistentOnPUSCH-r18", nullptr},
    {"aperiodic-r18", nullptr}};
const asn1::table::choice_desc ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::desc = {
    asn1::table::enum_int_type<ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::types>(),
    ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::types::nof_types,
    ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::types::nof_exts,
    ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::types::has_ext,
    false,
    choice_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__alternatives,
    nullptr,
    choice_table_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__names,
    "ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_",
    offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_, c.type),
    offsetof(ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_, c.value),
    false,
    &ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::storage_desc};

static const char* enum_names_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__types_values[] =
    {"periodic-r18", "semiPersistentOnPUCCH-r18", "semiPersistentOnPUSCH-r18", "aperiodic-r18"};
const asn1::table::enum_desc ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::types_opts::desc = {
    enum_names_ltm_csi_report_cfg_r18_s_ltm_report_cfg_type_r18_c__types_values,
    4,
    "ltm_csi_report_cfg_r18_s::ltm_report_cfg_type_r18_c_::types"};

// CSI-MeasConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_csi_meas_cfg_s_aperiodic_trigger_state_list_setup_release_alternatives[] =
    {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      128,
      &asn1::table::item_list_codec<setup_release_c<dyn_seq_of<csi_aperiodic_trigger_state_s, 1, 128>>::value_type,
                                    asn1::table::sequence_item_ops,
                                    false,
                                    true>::value,
      &csi_aperiodic_trigger_state_s::desc}};
static const asn1::table::field_name seq_table_csi_meas_cfg_s_aperiodic_trigger_state_list_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_csi_meas_cfg_s_aperiodic_trigger_state_list_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_csi_meas_cfg_s_aperiodic_trigger_state_list_setup_release_alternatives,
    nullptr,
    seq_table_csi_meas_cfg_s_aperiodic_trigger_state_list_setup_release_names,
    "setup_release_c",
    setup_release_c<dyn_seq_of<csi_aperiodic_trigger_state_s, 1, 128>>::type_offset(),
    setup_release_c<dyn_seq_of<csi_aperiodic_trigger_state_s, 1, 128>>::value_offset()};
static const asn1::table::seq_field
    seq_table_csi_meas_cfg_s_semi_persistent_on_pusch_trigger_state_list_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         64,
         &asn1::table::item_list_codec<
             setup_release_c<dyn_seq_of<csi_semi_persistent_on_pusch_trigger_state_s, 1, 64>>::value_type,
             asn1::table::sequence_item_ops,
             false,
             true>::value,
         &csi_semi_persistent_on_pusch_trigger_state_s::desc}};
static const asn1::table::field_name
    seq_table_csi_meas_cfg_s_semi_persistent_on_pusch_trigger_state_list_setup_release_names[] = {{"release", nullptr},
                                                                                                  {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_csi_meas_cfg_s_semi_persistent_on_pusch_trigger_state_list_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_csi_meas_cfg_s_semi_persistent_on_pusch_trigger_state_list_setup_release_alternatives,
        nullptr,
        seq_table_csi_meas_cfg_s_semi_persistent_on_pusch_trigger_state_list_setup_release_names,
        "setup_release_c",
        setup_release_c<dyn_seq_of<csi_semi_persistent_on_pusch_trigger_state_s, 1, 64>>::type_offset(),
        setup_release_c<dyn_seq_of<csi_semi_persistent_on_pusch_trigger_state_s, 1, 64>>::value_offset()};
static const asn1::table::seq_field seq_table_csi_meas_cfg_s_fields[] = {
    {offsetof(csi_meas_cfg_s, nzp_csi_rs_res_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     192,
     &asn1::table::item_list_codec<decltype(csi_meas_cfg_s::nzp_csi_rs_res_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nzp_csi_rs_res_s::desc},
    {offsetof(csi_meas_cfg_s, nzp_csi_rs_res_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     192,
     &asn1::table::list_codec<decltype(csi_meas_cfg_s::nzp_csi_rs_res_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(191), false, false>>::value},
    {offsetof(csi_meas_cfg_s, nzp_csi_rs_res_set_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(csi_meas_cfg_s::nzp_csi_rs_res_set_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nzp_csi_rs_res_set_s::desc},
    {offsetof(csi_meas_cfg_s, nzp_csi_rs_res_set_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(csi_meas_cfg_s::nzp_csi_rs_res_set_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, false>>::value},
    {offsetof(csi_meas_cfg_s, csi_im_res_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::item_list_codec<decltype(csi_meas_cfg_s::csi_im_res_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &csi_im_res_s::desc},
    {offsetof(csi_meas_cfg_s, csi_im_res_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::list_codec<decltype(csi_meas_cfg_s::csi_im_res_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(31), false, false>>::value},
    {offsetof(csi_meas_cfg_s, csi_im_res_set_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(csi_meas_cfg_s::csi_im_res_set_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &csi_im_res_set_s::desc},
    {offsetof(csi_meas_cfg_s, csi_im_res_set_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(csi_meas_cfg_s::csi_im_res_set_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, false>>::value},
    {offsetof(csi_meas_cfg_s, csi_ssb_res_set_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(csi_meas_cfg_s::csi_ssb_res_set_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &csi_ssb_res_set_s::desc},
    {offsetof(csi_meas_cfg_s, csi_ssb_res_set_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(csi_meas_cfg_s::csi_ssb_res_set_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, false>>::value},
    {offsetof(csi_meas_cfg_s, csi_res_cfg_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     112,
     &asn1::table::item_list_codec<decltype(csi_meas_cfg_s::csi_res_cfg_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &csi_res_cfg_s::desc},
    {offsetof(csi_meas_cfg_s, csi_res_cfg_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     112,
     &asn1::table::list_codec<decltype(csi_meas_cfg_s::csi_res_cfg_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(111), false, false>>::value},
    {offsetof(csi_meas_cfg_s, csi_report_cfg_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     48,
     &asn1::table::item_list_codec<decltype(csi_meas_cfg_s::csi_report_cfg_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &csi_report_cfg_s::desc},
    {offsetof(csi_meas_cfg_s, csi_report_cfg_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     48,
     &asn1::table::list_codec<decltype(csi_meas_cfg_s::csi_report_cfg_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(47), false, false>>::value},
    {offsetof(csi_meas_cfg_s, report_trigger_size),
     (int32_t)offsetof(csi_meas_cfg_s, report_trigger_size_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(6),
     nullptr},
    {offsetof(csi_meas_cfg_s, aperiodic_trigger_state_list),
     (int32_t)offsetof(csi_meas_cfg_s, aperiodic_trigger_state_list_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_meas_cfg_s_aperiodic_trigger_state_list_setup_release},
    {offsetof(csi_meas_cfg_s, semi_persistent_on_pusch_trigger_state_list),
     (int32_t)offsetof(csi_meas_cfg_s, semi_persistent_on_pusch_trigger_state_list_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_csi_meas_cfg_s_semi_persistent_on_pusch_trigger_state_list_setup_release}};
static const asn1::table::field_name seq_table_csi_meas_cfg_s_names[] = {
    {"nzp-CSI-RS-ResourceToAddModList", nullptr},
    {"nzp-CSI-RS-ResourceToReleaseList", nullptr},
    {"nzp-CSI-RS-ResourceSetToAddModList", nullptr},
    {"nzp-CSI-RS-ResourceSetToReleaseList", nullptr},
    {"csi-IM-ResourceToAddModList", nullptr},
    {"csi-IM-ResourceToReleaseList", nullptr},
    {"csi-IM-ResourceSetToAddModList", nullptr},
    {"csi-IM-ResourceSetToReleaseList", nullptr},
    {"csi-SSB-ResourceSetToAddModList", nullptr},
    {"csi-SSB-ResourceSetToReleaseList", nullptr},
    {"csi-ResourceConfigToAddModList", nullptr},
    {"csi-ResourceConfigToReleaseList", nullptr},
    {"csi-ReportConfigToAddModList", nullptr},
    {"csi-ReportConfigToReleaseList", nullptr},
    {"reportTriggerSize", nullptr},
    {"aperiodicTriggerStateList", nullptr},
    {"semiPersistentOnPUSCH-TriggerStateList", nullptr}};
static const asn1::table::seq_opt seq_table_csi_meas_cfg_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, asn1::table::seq_field::list_presence},
    {7, asn1::table::seq_field::list_presence},
    {8, asn1::table::seq_field::list_presence},
    {9, asn1::table::seq_field::list_presence},
    {10, asn1::table::seq_field::list_presence},
    {11, asn1::table::seq_field::list_presence},
    {12, asn1::table::seq_field::list_presence},
    {13, asn1::table::seq_field::list_presence},
    {14, (int32_t)offsetof(csi_meas_cfg_s, report_trigger_size_present)},
    {15, (int32_t)offsetof(csi_meas_cfg_s, aperiodic_trigger_state_list_present)},
    {16, (int32_t)offsetof(csi_meas_cfg_s, semi_persistent_on_pusch_trigger_state_list_present)}};
static const asn1::table::seq_field seq_table_csi_meas_cfg_s_g0_fields[] = {
    {offsetof(csi_meas_cfg_s, report_trigger_size_dci_0_2_r16),
     (int32_t)offsetof(csi_meas_cfg_s, report_trigger_size_dci_0_2_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(6),
     nullptr}};
static const asn1::table::field_name seq_table_csi_meas_cfg_s_g0_names[] = {{"reportTriggerSizeDCI-0-2-r16", nullptr}};
static const asn1::table::seq_opt    seq_table_csi_meas_cfg_s_g0_opts[]  = {
    {0, (int32_t)offsetof(csi_meas_cfg_s, report_trigger_size_dci_0_2_r16_present)}};
static const asn1::table::seq_field seq_table_csi_meas_cfg_s_g1_fields[] = {
    {offsetof(csi_meas_cfg_s, scell_activation_rs_cfg_to_add_mod_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     255,
     &asn1::table::ptr_item_list_codec<decltype(csi_meas_cfg_s::scell_activation_rs_cfg_to_add_mod_list_r17),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &scell_activation_rs_cfg_r17_s::desc},
    {offsetof(csi_meas_cfg_s, scell_activation_rs_cfg_to_release_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     255,
     &asn1::table::ptr_list_codec<decltype(csi_meas_cfg_s::scell_activation_rs_cfg_to_release_list_r17),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(1), UINT64_C(255), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_meas_cfg_s_g1_names[] = {
    {"sCellActivationRS-ConfigToAddModList-r17", nullptr},
    {"sCellActivationRS-ConfigToReleaseList-r17", nullptr}};
static const asn1::table::seq_opt   seq_table_csi_meas_cfg_s_g1_opts[]   = {{0, asn1::table::seq_field::ptr_presence},
                                                                            {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_csi_meas_cfg_s_g2_fields[] = {
    {offsetof(csi_meas_cfg_s, ltm_csi_report_cfg_to_add_mod_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     48,
     &asn1::table::ptr_item_list_codec<decltype(csi_meas_cfg_s::ltm_csi_report_cfg_to_add_mod_list_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &ltm_csi_report_cfg_r18_s::desc},
    {offsetof(csi_meas_cfg_s, ltm_csi_report_cfg_to_release_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     48,
     &asn1::table::ptr_list_codec<decltype(csi_meas_cfg_s::ltm_csi_report_cfg_to_release_list_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(0), UINT64_C(47), false, false>>::value}};
static const asn1::table::field_name seq_table_csi_meas_cfg_s_g2_names[] = {
    {"ltm-CSI-ReportConfigToAddModList-r18", nullptr},
    {"ltm-CSI-ReportConfigToReleaseList-r18", nullptr}};
static const asn1::table::seq_opt   seq_table_csi_meas_cfg_s_g2_opts[] = {{0, asn1::table::seq_field::ptr_presence},
                                                                          {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_csi_meas_cfg_s_groups[]  = {
    {{seq_table_csi_meas_cfg_s_g0_fields, 1, UINT64_C(0x0), seq_table_csi_meas_cfg_s_g0_opts, 1},
      false,
      seq_table_csi_meas_cfg_s_g0_names},
    {{seq_table_csi_meas_cfg_s_g1_fields, 2, UINT64_C(0x0), seq_table_csi_meas_cfg_s_g1_opts, 2},
      false,
      seq_table_csi_meas_cfg_s_g1_names},
    {{seq_table_csi_meas_cfg_s_g2_fields, 2, UINT64_C(0x0), seq_table_csi_meas_cfg_s_g2_opts, 2},
      false,
      seq_table_csi_meas_cfg_s_g2_names}};
const asn1::table::seq_desc csi_meas_cfg_s::desc = {
    {seq_table_csi_meas_cfg_s_fields, 17, UINT64_C(0x0), seq_table_csi_meas_cfg_s_opts, 17},
    true,
    false,
    seq_table_csi_meas_cfg_s_groups,
    3,
    seq_table_csi_meas_cfg_s_names};

// CellDTX-DRX-Config-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_dtx_drx_cfg_r18_s_fields[] = {
    {offsetof(cell_dtx_drx_cfg_r18_s, cell_dtx_drx_on_dur_timer_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::desc},
    {offsetof(cell_dtx_drx_cfg_r18_s, cell_dtx_drx_cycle_start_offset_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::desc},
    {offsetof(cell_dtx_drx_cfg_r18_s, cell_dtx_drx_slot_offset_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(cell_dtx_drx_cfg_r18_s, cell_dtx_drx_cfg_type_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cfg_type_r18)>(),
     decltype(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cfg_type_r18)::has_ext,
     false,
     decltype(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cfg_type_r18)::nof_types,
     decltype(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cfg_type_r18)::nof_exts,
     nullptr,
     &cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cfg_type_r18_opts::desc},
    {offsetof(cell_dtx_drx_cfg_r18_s, cell_dtx_drx_activation_status_r18),
     (int32_t)offsetof(cell_dtx_drx_cfg_r18_s, cell_dtx_drx_activation_status_r18_present),
     asn1::table::enum_op<decltype(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_activation_status_r18)>(),
     decltype(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_activation_status_r18)::has_ext,
     false,
     decltype(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_activation_status_r18)::nof_types,
     decltype(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_activation_status_r18)::nof_exts,
     nullptr,
     &cell_dtx_drx_cfg_r18_s::cell_dtx_drx_activation_status_r18_opts::desc}};
static const asn1::table::field_name seq_table_cell_dtx_drx_cfg_r18_s_names[] = {
    {"cellDTX-DRX-onDurationTimer-r18", nullptr},
    {"cellDTX-DRX-CycleStartOffset-r18", nullptr},
    {"cellDTX-DRX-SlotOffset-r18", nullptr},
    {"cellDTX-DRX-ConfigType-r18", nullptr},
    {"cellDTX-DRX-ActivationStatus-r18", nullptr}};
static const asn1::table::seq_opt seq_table_cell_dtx_drx_cfg_r18_s_opts[] = {
    {4, (int32_t)offsetof(cell_dtx_drx_cfg_r18_s, cell_dtx_drx_activation_status_r18_present)}};
const asn1::table::seq_desc cell_dtx_drx_cfg_r18_s::desc = {
    {seq_table_cell_dtx_drx_cfg_r18_s_fields, 5, UINT64_C(0xf), seq_table_cell_dtx_drx_cfg_r18_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_cell_dtx_drx_cfg_r18_s_names};

static const asn1::table::storage_ops* const
    choice_storage_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_on_dur_timer_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::milli_seconds_e_,
                                      true>::value};
const asn1::table::choice_storage_desc cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::storage_desc = {
    choice_storage_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_on_dur_timer_r18_c__alternatives,
    2,
    "cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_"};

uint8_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::set_sub_milli_seconds()
{
  set(types::sub_milli_seconds);
  return c.get<uint8_t>();
}
cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::milli_seconds_e_&
cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::set_milli_seconds()
{
  set(types::milli_seconds);
  return c.get<milli_seconds_e_>();
}
static const asn1::table::seq_field
    choice_table_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_on_dur_timer_r18_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(31),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::enum_op<cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::milli_seconds_e_>(),
         cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::milli_seconds_e_::has_ext,
         false,
         cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::milli_seconds_e_::nof_types,
         cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::milli_seconds_e_::nof_exts,
         nullptr,
         &cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::milli_seconds_opts::desc}};
static const asn1::table::field_name choice_table_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_on_dur_timer_r18_c__names[] = {
    {"subMilliSeconds", nullptr},
    {"milliSeconds", nullptr}};
const asn1::table::choice_desc cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::desc = {
    asn1::table::enum_int_type<cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::types>(),
    cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::types::nof_types,
    cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::types::nof_exts,
    cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::types::has_ext,
    false,
    choice_table_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_on_dur_timer_r18_c__alternatives,
    nullptr,
    choice_table_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_on_dur_timer_r18_c__names,
    "cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_",
    offsetof(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_, c.type),
    offsetof(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_, c.value),
    false,
    &cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::storage_desc};

static const char* enum_names_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_on_dur_timer_r18_c__milli_seconds_e__values[] = {
    "ms1",    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",   "ms20",   "ms30",  "ms40",
    "ms50",   "ms60",   "ms80",   "ms100",  "ms200",  "ms300",  "ms400",  "ms500",  "ms600",  "ms800", "ms1000",
    "ms1200", "ms1600", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::milli_seconds_opts::desc = {
    enum_names_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_on_dur_timer_r18_c__milli_seconds_e__values,
    32,
    "cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::milli_seconds_e_"};
uint16_t cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::milli_seconds_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,  3,   4,   5,   6,   8,   10,  20,  30,   40,   50,
                                     60, 80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(
      numbers, 24, value, "cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::milli_seconds_e_");
}

static const char* enum_names_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_on_dur_timer_r18_c__types_values[] = {
    "subMilliSeconds",
    "milliSeconds"};
const asn1::table::enum_desc cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::types_opts::desc = {
    enum_names_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_on_dur_timer_r18_c__types_values,
    2,
    "cell_dtx_drx_cfg_r18_s::cell_dtx_drx_on_dur_timer_r18_c_::types"};

static const asn1::table::storage_ops* const
    choice_storage_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_cycle_start_offset_r18_c__alternatives[] = {
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
const asn1::table::choice_storage_desc cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::storage_desc = {
    choice_storage_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_cycle_start_offset_r18_c__alternatives,
    20,
    "cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_"};

uint8_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms10()
{
  set(types::ms10);
  return c.get<uint8_t>();
}
uint8_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms20()
{
  set(types::ms20);
  return c.get<uint8_t>();
}
uint8_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms32()
{
  set(types::ms32);
  return c.get<uint8_t>();
}
uint8_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms40()
{
  set(types::ms40);
  return c.get<uint8_t>();
}
uint8_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms60()
{
  set(types::ms60);
  return c.get<uint8_t>();
}
uint8_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms64()
{
  set(types::ms64);
  return c.get<uint8_t>();
}
uint8_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms70()
{
  set(types::ms70);
  return c.get<uint8_t>();
}
uint8_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms80()
{
  set(types::ms80);
  return c.get<uint8_t>();
}
uint8_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms128()
{
  set(types::ms128);
  return c.get<uint8_t>();
}
uint8_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms160()
{
  set(types::ms160);
  return c.get<uint8_t>();
}
uint16_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms256()
{
  set(types::ms256);
  return c.get<uint16_t>();
}
uint16_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms320()
{
  set(types::ms320);
  return c.get<uint16_t>();
}
uint16_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms512()
{
  set(types::ms512);
  return c.get<uint16_t>();
}
uint16_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms640()
{
  set(types::ms640);
  return c.get<uint16_t>();
}
uint16_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms1024()
{
  set(types::ms1024);
  return c.get<uint16_t>();
}
uint16_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms1280()
{
  set(types::ms1280);
  return c.get<uint16_t>();
}
uint16_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms2048()
{
  set(types::ms2048);
  return c.get<uint16_t>();
}
uint16_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms2560()
{
  set(types::ms2560);
  return c.get<uint16_t>();
}
uint16_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms5120()
{
  set(types::ms5120);
  return c.get<uint16_t>();
}
uint16_t& cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::set_ms10240()
{
  set(types::ms10240);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field
    choice_table_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_cycle_start_offset_r18_c__alternatives[] = {
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
         UINT64_C(39),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(0),
         UINT64_C(59),
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
         UINT64_C(69),
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
         UINT64_C(127),
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
         UINT64_C(255),
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
         UINT64_C(511),
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
         UINT64_C(1023),
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
         UINT64_C(2047),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(2559),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(5119),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(10239),
         nullptr}};
static const asn1::table::field_name
    choice_table_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_cycle_start_offset_r18_c__names[] = {
        {"ms10", nullptr},   {"ms20", nullptr},   {"ms32", nullptr},   {"ms40", nullptr},   {"ms60", nullptr},
        {"ms64", nullptr},   {"ms70", nullptr},   {"ms80", nullptr},   {"ms128", nullptr},  {"ms160", nullptr},
        {"ms256", nullptr},  {"ms320", nullptr},  {"ms512", nullptr},  {"ms640", nullptr},  {"ms1024", nullptr},
        {"ms1280", nullptr}, {"ms2048", nullptr}, {"ms2560", nullptr}, {"ms5120", nullptr}, {"ms10240", nullptr}};
const asn1::table::choice_desc cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::desc = {
    asn1::table::enum_int_type<cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::types>(),
    cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::types::nof_types,
    cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::types::nof_exts,
    cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::types::has_ext,
    false,
    choice_table_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_cycle_start_offset_r18_c__alternatives,
    nullptr,
    choice_table_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_cycle_start_offset_r18_c__names,
    "cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_",
    offsetof(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_, c.type),
    offsetof(cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_, c.value),
    false,
    &cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::storage_desc};

static const char* enum_names_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_cycle_start_offset_r18_c__types_values[] = {
    "ms10",  "ms20",  "ms32",  "ms40",  "ms60",   "ms64",   "ms70",   "ms80",   "ms128",  "ms160",
    "ms256", "ms320", "ms512", "ms640", "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
const asn1::table::enum_desc cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::types_opts::desc = {
    enum_names_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_cycle_start_offset_r18_c__types_values,
    20,
    "cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::types"};
uint16_t cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cycle_start_offset_r18_c_::types");
}

static const char* enum_names_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_cfg_type_r18_e__values[] = {"dtx", "drx", "dtxdrx"};
const asn1::table::enum_desc cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cfg_type_r18_opts::desc  = {
    enum_names_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_cfg_type_r18_e__values,
    3,
    "cell_dtx_drx_cfg_r18_s::cell_dtx_drx_cfg_type_r18_e_"};

static const char* enum_names_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_activation_status_r18_e__values[] = {"activated",
                                                                                                       "deactivated"};
const asn1::table::enum_desc cell_dtx_drx_cfg_r18_s::cell_dtx_drx_activation_status_r18_opts::desc  = {
    enum_names_cell_dtx_drx_cfg_r18_s_cell_dtx_drx_activation_status_r18_e__values,
    2,
    "cell_dtx_drx_cfg_r18_s::cell_dtx_drx_activation_status_r18_e_"};

// TDD-UL-DL-SlotConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_tdd_ul_dl_slot_cfg_s_fields[] = {
    {offsetof(tdd_ul_dl_slot_cfg_s, slot_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(319),
     nullptr},
    {offsetof(tdd_ul_dl_slot_cfg_s, symbols),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &tdd_ul_dl_slot_cfg_s::symbols_c_::desc}};
static const asn1::table::field_name seq_table_tdd_ul_dl_slot_cfg_s_names[] = {{"slotIndex", nullptr},
                                                                               {"symbols", nullptr}};
const asn1::table::seq_desc          tdd_ul_dl_slot_cfg_s::desc             = {
    {seq_table_tdd_ul_dl_slot_cfg_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_tdd_ul_dl_slot_cfg_s_names};

void tdd_ul_dl_slot_cfg_s::symbols_c_::set(types::options e)
{
  type_ = e;
}
void tdd_ul_dl_slot_cfg_s::symbols_c_::set_all_dl()
{
  set(types::all_dl);
}
void tdd_ul_dl_slot_cfg_s::symbols_c_::set_all_ul()
{
  set(types::all_ul);
}
tdd_ul_dl_slot_cfg_s::symbols_c_::explicit_s_& tdd_ul_dl_slot_cfg_s::symbols_c_::set_explicit_value()
{
  set(types::explicit_value);
  return c;
}
static const asn1::table::seq_field seq_table_tdd_ul_dl_slot_cfg_s_symbols_c__explicit_s__fields[] = {
    {offsetof(tdd_ul_dl_slot_cfg_s::symbols_c_::explicit_s_, nrof_dl_symbols),
     (int32_t)offsetof(tdd_ul_dl_slot_cfg_s::symbols_c_::explicit_s_, nrof_dl_symbols_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(13),
     nullptr},
    {offsetof(tdd_ul_dl_slot_cfg_s::symbols_c_::explicit_s_, nrof_ul_symbols),
     (int32_t)offsetof(tdd_ul_dl_slot_cfg_s::symbols_c_::explicit_s_, nrof_ul_symbols_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(13),
     nullptr}};
static const asn1::table::field_name seq_table_tdd_ul_dl_slot_cfg_s_symbols_c__explicit_s__names[] = {
    {"nrofDownlinkSymbols", nullptr},
    {"nrofUplinkSymbols", nullptr}};
static const asn1::table::seq_opt seq_table_tdd_ul_dl_slot_cfg_s_symbols_c__explicit_s__opts[] = {
    {0, (int32_t)offsetof(tdd_ul_dl_slot_cfg_s::symbols_c_::explicit_s_, nrof_dl_symbols_present)},
    {1, (int32_t)offsetof(tdd_ul_dl_slot_cfg_s::symbols_c_::explicit_s_, nrof_ul_symbols_present)}};
static const asn1::table::seq_desc seq_table_tdd_ul_dl_slot_cfg_s_symbols_c__explicit_s_ = {
    {seq_table_tdd_ul_dl_slot_cfg_s_symbols_c__explicit_s__fields,
     2,
     UINT64_C(0x0),
     seq_table_tdd_ul_dl_slot_cfg_s_symbols_c__explicit_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_tdd_ul_dl_slot_cfg_s_symbols_c__explicit_s__names};
static const asn1::table::seq_field choice_table_tdd_ul_dl_slot_cfg_s_symbols_c__alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_tdd_ul_dl_slot_cfg_s_symbols_c__explicit_s_}};
static const asn1::table::field_name choice_table_tdd_ul_dl_slot_cfg_s_symbols_c__names[] = {{"allDownlink", nullptr},
                                                                                             {"allUplink", nullptr},
                                                                                             {"explicit", nullptr}};
const asn1::table::choice_desc       tdd_ul_dl_slot_cfg_s::symbols_c_::desc               = {
    asn1::table::enum_int_type<tdd_ul_dl_slot_cfg_s::symbols_c_::types>(),
    tdd_ul_dl_slot_cfg_s::symbols_c_::types::nof_types,
    tdd_ul_dl_slot_cfg_s::symbols_c_::types::nof_exts,
    tdd_ul_dl_slot_cfg_s::symbols_c_::types::has_ext,
    false,
    choice_table_tdd_ul_dl_slot_cfg_s_symbols_c__alternatives,
    &asn1::table::choice_set<tdd_ul_dl_slot_cfg_s::symbols_c_>,
    choice_table_tdd_ul_dl_slot_cfg_s_symbols_c__names,
    "tdd_ul_dl_slot_cfg_s::symbols_c_",
    offsetof(tdd_ul_dl_slot_cfg_s::symbols_c_, type_),
    offsetof(tdd_ul_dl_slot_cfg_s::symbols_c_, c),
    false};

static const char* enum_names_tdd_ul_dl_slot_cfg_s_symbols_c__types_values[] = {"allDownlink", "allUplink", "explicit"};
const asn1::table::enum_desc tdd_ul_dl_slot_cfg_s::symbols_c_::types_opts::desc = {
    enum_names_tdd_ul_dl_slot_cfg_s_symbols_c__types_values,
    3,
    "tdd_ul_dl_slot_cfg_s::symbols_c_::types"};

// TDD-UL-DL-ConfigDedicated ::= SEQUENCE
static const asn1::table::seq_field seq_table_tdd_ul_dl_cfg_ded_s_fields[] = {
    {offsetof(tdd_ul_dl_cfg_ded_s, slot_specific_cfgs_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     320,
     &asn1::table::item_list_codec<decltype(tdd_ul_dl_cfg_ded_s::slot_specific_cfgs_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &tdd_ul_dl_slot_cfg_s::desc},
    {offsetof(tdd_ul_dl_cfg_ded_s, slot_specific_cfgs_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     320,
     &asn1::table::list_codec<decltype(tdd_ul_dl_cfg_ded_s::slot_specific_cfgs_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(319), false, false>>::value}};
static const asn1::table::field_name seq_table_tdd_ul_dl_cfg_ded_s_names[] = {
    {"slotSpecificConfigurationsToAddModList", nullptr},
    {"slotSpecificConfigurationsToReleaseList", nullptr}};
static const asn1::table::seq_opt seq_table_tdd_ul_dl_cfg_ded_s_opts[] = {{0, asn1::table::seq_field::list_presence},
                                                                          {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       tdd_ul_dl_cfg_ded_s::desc            = {
    {seq_table_tdd_ul_dl_cfg_ded_s_fields, 2, UINT64_C(0x0), seq_table_tdd_ul_dl_cfg_ded_s_opts, 2},
    true,
    false,
    nullptr,
    0,
    seq_table_tdd_ul_dl_cfg_ded_s_names};

// PUSCH-CodeBlockGroupTransmission ::= SEQUENCE
static const asn1::table::seq_field seq_table_pusch_code_block_group_tx_s_fields[] = {
    {offsetof(pusch_code_block_group_tx_s, max_code_block_groups_per_transport_block),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block)>(),
     decltype(pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block)::has_ext,
     false,
     decltype(pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block)::nof_types,
     decltype(pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block)::nof_exts,
     nullptr,
     &pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::desc}};
static const asn1::table::field_name seq_table_pusch_code_block_group_tx_s_names[] = {
    {"maxCodeBlockGroupsPerTransportBlock", nullptr}};
const asn1::table::seq_desc pusch_code_block_group_tx_s::desc = {
    {seq_table_pusch_code_block_group_tx_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_pusch_code_block_group_tx_s_names};

static const char* enum_names_pusch_code_block_group_tx_s_max_code_block_groups_per_transport_block_e__values[] =
    {"n2", "n4", "n6", "n8"};
const asn1::table::enum_desc pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::desc = {
    enum_names_pusch_code_block_group_tx_s_max_code_block_groups_per_transport_block_e__values,
    4,
    "pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block_e_"};
uint8_t pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8};
  return map_enum_number(
      numbers, 4, value, "pusch_code_block_group_tx_s::max_code_block_groups_per_transport_block_e_");
}

// PUSCH-ServingCellConfig ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_pusch_serving_cell_cfg_s_code_block_group_tx_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &pusch_code_block_group_tx_s::desc}};
static const asn1::table::field_name seq_table_pusch_serving_cell_cfg_s_code_block_group_tx_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pusch_serving_cell_cfg_s_code_block_group_tx_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pusch_serving_cell_cfg_s_code_block_group_tx_setup_release_alternatives,
    nullptr,
    seq_table_pusch_serving_cell_cfg_s_code_block_group_tx_setup_release_names,
    "setup_release_c",
    setup_release_c<pusch_code_block_group_tx_s>::type_offset(),
    setup_release_c<pusch_code_block_group_tx_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_pusch_serving_cell_cfg_s_max_mimo_layers_dci_0_2_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(4),
         nullptr}};
static const asn1::table::field_name
    seq_table_pusch_serving_cell_cfg_s_max_mimo_layers_dci_0_2_r16_setup_release_names[] = {{"release", nullptr},
                                                                                            {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pusch_serving_cell_cfg_s_max_mimo_layers_dci_0_2_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pusch_serving_cell_cfg_s_max_mimo_layers_dci_0_2_r16_setup_release_alternatives,
    nullptr,
    seq_table_pusch_serving_cell_cfg_s_max_mimo_layers_dci_0_2_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<integer<uint8_t, 1, 4>>::type_offset(),
    setup_release_c<integer<uint8_t, 1, 4>>::value_offset()};
static const asn1::table::seq_field seq_table_pusch_serving_cell_cfg_s_ul_harq_mode_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<setup_release_c<fixed_bitstring<32>>::value_type, asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_pusch_serving_cell_cfg_s_ul_harq_mode_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pusch_serving_cell_cfg_s_ul_harq_mode_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pusch_serving_cell_cfg_s_ul_harq_mode_r17_setup_release_alternatives,
    nullptr,
    seq_table_pusch_serving_cell_cfg_s_ul_harq_mode_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<fixed_bitstring<32>>::type_offset(),
    setup_release_c<fixed_bitstring<32>>::value_offset()};
static const asn1::table::seq_field seq_table_pusch_serving_cell_cfg_s_fields[] = {
    {offsetof(pusch_serving_cell_cfg_s, code_block_group_tx),
     (int32_t)offsetof(pusch_serving_cell_cfg_s, code_block_group_tx_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_pusch_serving_cell_cfg_s_code_block_group_tx_setup_release},
    {0,
     (int32_t)offsetof(pusch_serving_cell_cfg_s, rate_matching_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pusch_serving_cell_cfg_s, xoverhead),
     (int32_t)offsetof(pusch_serving_cell_cfg_s, xoverhead_present),
     asn1::table::enum_op<decltype(pusch_serving_cell_cfg_s::xoverhead)>(),
     decltype(pusch_serving_cell_cfg_s::xoverhead)::has_ext,
     false,
     decltype(pusch_serving_cell_cfg_s::xoverhead)::nof_types,
     decltype(pusch_serving_cell_cfg_s::xoverhead)::nof_exts,
     nullptr,
     &pusch_serving_cell_cfg_s::xoverhead_opts::desc}};
static const asn1::table::field_name seq_table_pusch_serving_cell_cfg_s_names[] = {
    {"codeBlockGroupTransmission", nullptr},
    {"rateMatching", "limitedBufferRM"},
    {"xOverhead", nullptr}};
static const asn1::table::seq_opt seq_table_pusch_serving_cell_cfg_s_opts[] = {
    {0, (int32_t)offsetof(pusch_serving_cell_cfg_s, code_block_group_tx_present)},
    {1, (int32_t)offsetof(pusch_serving_cell_cfg_s, rate_matching_present)},
    {2, (int32_t)offsetof(pusch_serving_cell_cfg_s, xoverhead_present)}};
static const asn1::table::seq_field seq_table_pusch_serving_cell_cfg_s_g0_fields[] = {
    {offsetof(pusch_serving_cell_cfg_s, max_mimo_layers),
     (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layers_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(4),
     nullptr},
    {offsetof(pusch_serving_cell_cfg_s, processing_type2_enabled),
     (int32_t)offsetof(pusch_serving_cell_cfg_s, processing_type2_enabled_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pusch_serving_cell_cfg_s_g0_names[] = {
    {"maxMIMO-Layers", nullptr},
    {"processingType2Enabled", nullptr}};
static const asn1::table::seq_opt seq_table_pusch_serving_cell_cfg_s_g0_opts[] = {
    {0, (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layers_present)},
    {1, (int32_t)offsetof(pusch_serving_cell_cfg_s, processing_type2_enabled_present)}};
static const asn1::table::seq_field seq_table_pusch_serving_cell_cfg_s_g1_fields[] = {
    {offsetof(pusch_serving_cell_cfg_s, max_mimo_layers_dci_0_2_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pusch_serving_cell_cfg_s::max_mimo_layers_dci_0_2_r16)>::value,
     &seq_table_pusch_serving_cell_cfg_s_max_mimo_layers_dci_0_2_r16_setup_release}};
static const asn1::table::field_name seq_table_pusch_serving_cell_cfg_s_g1_names[] = {
    {"maxMIMO-LayersDCI-0-2-r16", nullptr}};
static const asn1::table::seq_opt seq_table_pusch_serving_cell_cfg_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_pusch_serving_cell_cfg_s_g2_fields[] = {
    {0,
     (int32_t)offsetof(pusch_serving_cell_cfg_s, nrof_harq_processes_for_pusch_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(pusch_serving_cell_cfg_s, ul_harq_mode_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pusch_serving_cell_cfg_s::ul_harq_mode_r17)>::value,
     &seq_table_pusch_serving_cell_cfg_s_ul_harq_mode_r17_setup_release}};
static const asn1::table::field_name seq_table_pusch_serving_cell_cfg_s_g2_names[] = {
    {"nrofHARQ-ProcessesForPUSCH-r17", "n32"},
    {"uplinkHARQ-mode-r17", nullptr}};
static const asn1::table::seq_opt seq_table_pusch_serving_cell_cfg_s_g2_opts[] = {
    {0, (int32_t)offsetof(pusch_serving_cell_cfg_s, nrof_harq_processes_for_pusch_r17_present)},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_pusch_serving_cell_cfg_s_g3_fields[] = {
    {offsetof(pusch_serving_cell_cfg_s, max_mimo_layers_v1810),
     (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layers_v1810_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(5),
     UINT64_C(8),
     nullptr},
    {offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sdm_r18),
     (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sdm_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(2),
     nullptr},
    {offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sdm_dci_0_2_r18),
     (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sdm_dci_0_2_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(2),
     nullptr},
    {offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sfn_r18),
     (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sfn_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(2),
     nullptr},
    {offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sfn_dci_0_2_r18),
     (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sfn_dci_0_2_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(2),
     nullptr}};
static const asn1::table::field_name seq_table_pusch_serving_cell_cfg_s_g3_names[] = {
    {"maxMIMO-Layers-v1810", nullptr},
    {"maxMIMO-LayersforSDM-r18", nullptr},
    {"maxMIMO-LayersforSDM-DCI-0-2-r18", nullptr},
    {"maxMIMO-LayersforSFN-r18", nullptr},
    {"maxMIMO-LayersforSFN-DCI-0-2-r18", nullptr}};
static const asn1::table::seq_opt seq_table_pusch_serving_cell_cfg_s_g3_opts[] = {
    {0, (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layers_v1810_present)},
    {1, (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sdm_r18_present)},
    {2, (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sdm_dci_0_2_r18_present)},
    {3, (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sfn_r18_present)},
    {4, (int32_t)offsetof(pusch_serving_cell_cfg_s, max_mimo_layersfor_sfn_dci_0_2_r18_present)}};
static const asn1::table::seq_group seq_table_pusch_serving_cell_cfg_s_groups[] = {
    {{seq_table_pusch_serving_cell_cfg_s_g0_fields, 2, UINT64_C(0x0), seq_table_pusch_serving_cell_cfg_s_g0_opts, 2},
     false,
     seq_table_pusch_serving_cell_cfg_s_g0_names},
    {{seq_table_pusch_serving_cell_cfg_s_g1_fields, 1, UINT64_C(0x0), seq_table_pusch_serving_cell_cfg_s_g1_opts, 1},
     false,
     seq_table_pusch_serving_cell_cfg_s_g1_names},
    {{seq_table_pusch_serving_cell_cfg_s_g2_fields, 2, UINT64_C(0x0), seq_table_pusch_serving_cell_cfg_s_g2_opts, 2},
     false,
     seq_table_pusch_serving_cell_cfg_s_g2_names},
    {{seq_table_pusch_serving_cell_cfg_s_g3_fields, 5, UINT64_C(0x0), seq_table_pusch_serving_cell_cfg_s_g3_opts, 5},
     false,
     seq_table_pusch_serving_cell_cfg_s_g3_names}};
const asn1::table::seq_desc pusch_serving_cell_cfg_s::desc = {
    {seq_table_pusch_serving_cell_cfg_s_fields, 3, UINT64_C(0x0), seq_table_pusch_serving_cell_cfg_s_opts, 3},
    true,
    false,
    seq_table_pusch_serving_cell_cfg_s_groups,
    4,
    seq_table_pusch_serving_cell_cfg_s_names};

static const char*           enum_names_pusch_serving_cell_cfg_s_xoverhead_e__values[] = {"xoh6", "xoh12", "xoh18"};
const asn1::table::enum_desc pusch_serving_cell_cfg_s::xoverhead_opts::desc            = {
    enum_names_pusch_serving_cell_cfg_s_xoverhead_e__values,
    3,
    "pusch_serving_cell_cfg_s::xoverhead_e_"};
uint8_t pusch_serving_cell_cfg_s::xoverhead_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 12, 18};
  return map_enum_number(numbers, 3, value, "pusch_serving_cell_cfg_s::xoverhead_e_");
}

// SRS-CC-SetIndex ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_cc_set_idx_s_fields[] = {
    {offsetof(srs_cc_set_idx_s, cc_set_idx),
     (int32_t)offsetof(srs_cc_set_idx_s, cc_set_idx_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(srs_cc_set_idx_s, cc_idx_in_one_cc_set),
     (int32_t)offsetof(srs_cc_set_idx_s, cc_idx_in_one_cc_set_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr}};
static const asn1::table::field_name seq_table_srs_cc_set_idx_s_names[] = {{"cc-SetIndex", nullptr},
                                                                           {"cc-IndexInOneCC-Set", nullptr}};
static const asn1::table::seq_opt    seq_table_srs_cc_set_idx_s_opts[]  = {
    {0, (int32_t)offsetof(srs_cc_set_idx_s, cc_set_idx_present)},
    {1, (int32_t)offsetof(srs_cc_set_idx_s, cc_idx_in_one_cc_set_present)}};
const asn1::table::seq_desc srs_cc_set_idx_s::desc = {
    {seq_table_srs_cc_set_idx_s_fields, 2, UINT64_C(0x0), seq_table_srs_cc_set_idx_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_cc_set_idx_s_names};

// SRS-TPC-PDCCH-Config ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_tpc_pdcch_cfg_s_fields[] = {
    {offsetof(srs_tpc_pdcch_cfg_s, srs_cc_set_idxlist),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(srs_tpc_pdcch_cfg_s::srs_cc_set_idxlist),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &srs_cc_set_idx_s::desc}};
static const asn1::table::field_name seq_table_srs_tpc_pdcch_cfg_s_names[] = {{"srs-CC-SetIndexlist", nullptr}};
static const asn1::table::seq_opt seq_table_srs_tpc_pdcch_cfg_s_opts[] = {{0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       srs_tpc_pdcch_cfg_s::desc            = {
    {seq_table_srs_tpc_pdcch_cfg_s_fields, 1, UINT64_C(0x0), seq_table_srs_tpc_pdcch_cfg_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_srs_tpc_pdcch_cfg_s_names};

// SRS-CarrierSwitching ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_carrier_switching_s_fields[] = {
    {offsetof(srs_carrier_switching_s, srs_switch_from_serv_cell_idx),
     (int32_t)offsetof(srs_carrier_switching_s, srs_switch_from_serv_cell_idx_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(srs_carrier_switching_s, srs_switch_from_carrier),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_carrier_switching_s::srs_switch_from_carrier)>(),
     decltype(srs_carrier_switching_s::srs_switch_from_carrier)::has_ext,
     false,
     decltype(srs_carrier_switching_s::srs_switch_from_carrier)::nof_types,
     decltype(srs_carrier_switching_s::srs_switch_from_carrier)::nof_exts,
     nullptr,
     &srs_carrier_switching_s::srs_switch_from_carrier_opts::desc},
    {offsetof(srs_carrier_switching_s, srs_tpc_pdcch_group),
     (int32_t)offsetof(srs_carrier_switching_s, srs_tpc_pdcch_group_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_carrier_switching_s::srs_tpc_pdcch_group_c_::desc},
    {offsetof(srs_carrier_switching_s, monitoring_cells),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::list_codec<decltype(srs_carrier_switching_s::monitoring_cells),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(31), false, false>>::value}};
static const asn1::table::field_name seq_table_srs_carrier_switching_s_names[] = {
    {"srs-SwitchFromServCellIndex", nullptr},
    {"srs-SwitchFromCarrier", nullptr},
    {"srs-TPC-PDCCH-Group", nullptr},
    {"monitoringCells", nullptr}};
static const asn1::table::seq_opt seq_table_srs_carrier_switching_s_opts[] = {
    {0, (int32_t)offsetof(srs_carrier_switching_s, srs_switch_from_serv_cell_idx_present)},
    {2, (int32_t)offsetof(srs_carrier_switching_s, srs_tpc_pdcch_group_present)},
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc srs_carrier_switching_s::desc = {
    {seq_table_srs_carrier_switching_s_fields, 4, UINT64_C(0x2), seq_table_srs_carrier_switching_s_opts, 3},
    true,
    false,
    nullptr,
    0,
    seq_table_srs_carrier_switching_s_names};

static const char*           enum_names_srs_carrier_switching_s_srs_switch_from_carrier_e__values[] = {"sUL", "nUL"};
const asn1::table::enum_desc srs_carrier_switching_s::srs_switch_from_carrier_opts::desc            = {
    enum_names_srs_carrier_switching_s_srs_switch_from_carrier_e__values,
    2,
    "srs_carrier_switching_s::srs_switch_from_carrier_e_"};

static const asn1::table::storage_ops* const
    choice_storage_srs_carrier_switching_s_srs_tpc_pdcch_group_c__alternatives[] = {
        &asn1::table::alternative_ops<srs_carrier_switching_s::srs_tpc_pdcch_group_c_::type_a_l_, false>::value,
        &asn1::table::alternative_ops<srs_tpc_pdcch_cfg_s, false>::value};
const asn1::table::choice_storage_desc srs_carrier_switching_s::srs_tpc_pdcch_group_c_::storage_desc = {
    choice_storage_srs_carrier_switching_s_srs_tpc_pdcch_group_c__alternatives,
    2,
    "srs_carrier_switching_s::srs_tpc_pdcch_group_c_"};

srs_carrier_switching_s::srs_tpc_pdcch_group_c_::type_a_l_&
srs_carrier_switching_s::srs_tpc_pdcch_group_c_::set_type_a()
{
  set(types::type_a);
  return c.get<type_a_l_>();
}
srs_tpc_pdcch_cfg_s& srs_carrier_switching_s::srs_tpc_pdcch_group_c_::set_type_b()
{
  set(types::type_b);
  return c.get<srs_tpc_pdcch_cfg_s>();
}
static const asn1::table::seq_field choice_table_srs_carrier_switching_s_srs_tpc_pdcch_group_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::list_codec<srs_carrier_switching_s::srs_tpc_pdcch_group_c_::type_a_l_,
                              asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_tpc_pdcch_cfg_s::desc}};
static const asn1::table::field_name choice_table_srs_carrier_switching_s_srs_tpc_pdcch_group_c__names[] = {
    {"typeA", nullptr},
    {"typeB", nullptr}};
const asn1::table::choice_desc srs_carrier_switching_s::srs_tpc_pdcch_group_c_::desc = {
    asn1::table::enum_int_type<srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types>(),
    srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types::nof_types,
    srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types::nof_exts,
    srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types::has_ext,
    false,
    choice_table_srs_carrier_switching_s_srs_tpc_pdcch_group_c__alternatives,
    nullptr,
    choice_table_srs_carrier_switching_s_srs_tpc_pdcch_group_c__names,
    "srs_carrier_switching_s::srs_tpc_pdcch_group_c_",
    offsetof(srs_carrier_switching_s::srs_tpc_pdcch_group_c_, c.type),
    offsetof(srs_carrier_switching_s::srs_tpc_pdcch_group_c_, c.value),
    false,
    &srs_carrier_switching_s::srs_tpc_pdcch_group_c_::storage_desc};

static const char* enum_names_srs_carrier_switching_s_srs_tpc_pdcch_group_c__types_values[]    = {"typeA", "typeB"};
const asn1::table::enum_desc srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types_opts::desc = {
    enum_names_srs_carrier_switching_s_srs_tpc_pdcch_group_c__types_values,
    2,
    "srs_carrier_switching_s::srs_tpc_pdcch_group_c_::types"};

// UplinkTxSwitching-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_tx_switching_r16_s_fields[] = {
    {offsetof(ul_tx_switching_r16_s, ul_tx_switching_period_location_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ul_tx_switching_r16_s, ul_tx_switching_carrier_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ul_tx_switching_r16_s::ul_tx_switching_carrier_r16)>(),
     decltype(ul_tx_switching_r16_s::ul_tx_switching_carrier_r16)::has_ext,
     false,
     decltype(ul_tx_switching_r16_s::ul_tx_switching_carrier_r16)::nof_types,
     decltype(ul_tx_switching_r16_s::ul_tx_switching_carrier_r16)::nof_exts,
     nullptr,
     &ul_tx_switching_r16_s::ul_tx_switching_carrier_r16_opts::desc}};
static const asn1::table::field_name seq_table_ul_tx_switching_r16_s_names[] = {
    {"uplinkTxSwitchingPeriodLocation-r16", nullptr},
    {"uplinkTxSwitchingCarrier-r16", nullptr}};
const asn1::table::seq_desc ul_tx_switching_r16_s::desc = {
    {seq_table_ul_tx_switching_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ul_tx_switching_r16_s_names};

static const char* enum_names_ul_tx_switching_r16_s_ul_tx_switching_carrier_r16_e__values[] = {"carrier1", "carrier2"};
const asn1::table::enum_desc ul_tx_switching_r16_s::ul_tx_switching_carrier_r16_opts::desc  = {
    enum_names_ul_tx_switching_r16_s_ul_tx_switching_carrier_r16_e__values,
    2,
    "ul_tx_switching_r16_s::ul_tx_switching_carrier_r16_e_"};
uint8_t ul_tx_switching_r16_s::ul_tx_switching_carrier_r16_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ul_tx_switching_r16_s::ul_tx_switching_carrier_r16_e_");
}

// SRS-PosUplinkTransmissionWindowConfig-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_srs_pos_ul_tx_win_cfg_r18_s_fields[] = {
    {offsetof(srs_pos_ul_tx_win_cfg_r18_s, dummy),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1023),
     nullptr},
    {offsetof(srs_pos_ul_tx_win_cfg_r18_s, win_periodicity_and_offset_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::desc},
    {offsetof(srs_pos_ul_tx_win_cfg_r18_s, dur_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(srs_pos_ul_tx_win_cfg_r18_s::dur_r18)>(),
     decltype(srs_pos_ul_tx_win_cfg_r18_s::dur_r18)::has_ext,
     false,
     decltype(srs_pos_ul_tx_win_cfg_r18_s::dur_r18)::nof_types,
     decltype(srs_pos_ul_tx_win_cfg_r18_s::dur_r18)::nof_exts,
     nullptr,
     &srs_pos_ul_tx_win_cfg_r18_s::dur_r18_opts::desc}};
static const asn1::table::field_name seq_table_srs_pos_ul_tx_win_cfg_r18_s_names[] = {
    {"dummy", nullptr},
    {"windowPeriodicityAndOffset-r18", nullptr},
    {"duration-r18", nullptr}};
const asn1::table::seq_desc srs_pos_ul_tx_win_cfg_r18_s::desc = {
    {seq_table_srs_pos_ul_tx_win_cfg_r18_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_srs_pos_ul_tx_win_cfg_r18_s_names};

static const asn1::table::storage_ops* const
    choice_storage_srs_pos_ul_tx_win_cfg_r18_s_win_periodicity_and_offset_r18_c__alternatives[] = {
        &asn1::table::alternative_ops<srs_periodicity_and_offset_r16_c, false>::value,
        &asn1::table::alternative_ops<srs_periodicity_and_offset_ext_r16_c, false>::value};
const asn1::table::choice_storage_desc srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::storage_desc = {
    choice_storage_srs_pos_ul_tx_win_cfg_r18_s_win_periodicity_and_offset_r18_c__alternatives,
    2,
    "srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_"};

srs_periodicity_and_offset_r16_c&
srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::set_periodicity_and_offset_r18()
{
  set(types::periodicity_and_offset_r18);
  return c.get<srs_periodicity_and_offset_r16_c>();
}
srs_periodicity_and_offset_ext_r16_c&
srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::set_periodicity_and_offset_ext_r18()
{
  set(types::periodicity_and_offset_ext_r18);
  return c.get<srs_periodicity_and_offset_ext_r16_c>();
}
static const asn1::table::seq_field
    choice_table_srs_pos_ul_tx_win_cfg_r18_s_win_periodicity_and_offset_r18_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &srs_periodicity_and_offset_r16_c::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::choice,
         false,
         false,
         0,
         0,
         nullptr,
         &srs_periodicity_and_offset_ext_r16_c::desc}};
static const asn1::table::field_name
    choice_table_srs_pos_ul_tx_win_cfg_r18_s_win_periodicity_and_offset_r18_c__names[] = {
        {"periodicityAndOffset-r18", nullptr},
        {"periodicityAndOffsetExt-r18", nullptr}};
const asn1::table::choice_desc srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::desc = {
    asn1::table::enum_int_type<srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::types>(),
    srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::types::nof_types,
    srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::types::nof_exts,
    srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::types::has_ext,
    false,
    choice_table_srs_pos_ul_tx_win_cfg_r18_s_win_periodicity_and_offset_r18_c__alternatives,
    nullptr,
    choice_table_srs_pos_ul_tx_win_cfg_r18_s_win_periodicity_and_offset_r18_c__names,
    "srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_",
    offsetof(srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_, c.type),
    offsetof(srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_, c.value),
    false,
    &srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::storage_desc};

static const char* enum_names_srs_pos_ul_tx_win_cfg_r18_s_win_periodicity_and_offset_r18_c__types_values[] = {
    "periodicityAndOffset-r18",
    "periodicityAndOffsetExt-r18"};
const asn1::table::enum_desc srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::types_opts::desc = {
    enum_names_srs_pos_ul_tx_win_cfg_r18_s_win_periodicity_and_offset_r18_c__types_values,
    2,
    "srs_pos_ul_tx_win_cfg_r18_s::win_periodicity_and_offset_r18_c_::types"};

static const char*           enum_names_srs_pos_ul_tx_win_cfg_r18_s_dur_r18_e__values[] = {"sl1", "sl2", "sl4", "sl6"};
const asn1::table::enum_desc srs_pos_ul_tx_win_cfg_r18_s::dur_r18_opts::desc            = {
    enum_names_srs_pos_ul_tx_win_cfg_r18_s_dur_r18_e__values,
    4,
    "srs_pos_ul_tx_win_cfg_r18_s::dur_r18_e_"};
uint8_t srs_pos_ul_tx_win_cfg_r18_s::dur_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 6};
  return map_enum_number(numbers, 4, value, "srs_pos_ul_tx_win_cfg_r18_s::dur_r18_e_");
}

// SRS-PosTx-Hopping-r18 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_srs_pos_tx_hop_r18_s_srs_pos_ul_tx_win_cfg_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &srs_pos_ul_tx_win_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_srs_pos_tx_hop_r18_s_srs_pos_ul_tx_win_cfg_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_srs_pos_tx_hop_r18_s_srs_pos_ul_tx_win_cfg_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_srs_pos_tx_hop_r18_s_srs_pos_ul_tx_win_cfg_r18_setup_release_alternatives,
    nullptr,
    seq_table_srs_pos_tx_hop_r18_s_srs_pos_ul_tx_win_cfg_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<srs_pos_ul_tx_win_cfg_r18_s>::type_offset(),
    setup_release_c<srs_pos_ul_tx_win_cfg_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_srs_pos_tx_hop_r18_s_fields[] = {
    {offsetof(srs_pos_tx_hop_r18_s, srs_pos_cfg_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_pos_cfg_r17_s::desc},
    {offsetof(srs_pos_tx_hop_r18_s, bwp_r18),
     (int32_t)offsetof(srs_pos_tx_hop_r18_s, bwp_r18_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_s::desc},
    {offsetof(srs_pos_tx_hop_r18_s, inactive_pos_srs_time_align_timer_r18),
     (int32_t)offsetof(srs_pos_tx_hop_r18_s, inactive_pos_srs_time_align_timer_r18_present),
     asn1::table::enum_op<decltype(srs_pos_tx_hop_r18_s::inactive_pos_srs_time_align_timer_r18)>(),
     decltype(srs_pos_tx_hop_r18_s::inactive_pos_srs_time_align_timer_r18)::has_ext,
     false,
     decltype(srs_pos_tx_hop_r18_s::inactive_pos_srs_time_align_timer_r18)::nof_types,
     decltype(srs_pos_tx_hop_r18_s::inactive_pos_srs_time_align_timer_r18)::nof_exts,
     nullptr,
     &time_align_timer_opts::desc},
    {offsetof(srs_pos_tx_hop_r18_s, inactive_pos_srs_rsrp_change_thres_r18),
     (int32_t)offsetof(srs_pos_tx_hop_r18_s, inactive_pos_srs_rsrp_change_thres_r18_present),
     asn1::table::enum_op<decltype(srs_pos_tx_hop_r18_s::inactive_pos_srs_rsrp_change_thres_r18)>(),
     decltype(srs_pos_tx_hop_r18_s::inactive_pos_srs_rsrp_change_thres_r18)::has_ext,
     false,
     decltype(srs_pos_tx_hop_r18_s::inactive_pos_srs_rsrp_change_thres_r18)::nof_types,
     decltype(srs_pos_tx_hop_r18_s::inactive_pos_srs_rsrp_change_thres_r18)::nof_exts,
     nullptr,
     &rsrp_change_thres_r17_opts::desc},
    {offsetof(srs_pos_tx_hop_r18_s, srs_pos_ul_tx_win_cfg_r18),
     (int32_t)offsetof(srs_pos_tx_hop_r18_s, srs_pos_ul_tx_win_cfg_r18_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_srs_pos_tx_hop_r18_s_srs_pos_ul_tx_win_cfg_r18_setup_release}};
static const asn1::table::field_name seq_table_srs_pos_tx_hop_r18_s_names[] = {
    {"srs-PosConfig-r18", nullptr},
    {"bwp-r18", nullptr},
    {"inactivePosSRS-TimeAlignmentTimer-r18", nullptr},
    {"inactivePosSRS-RSRP-ChangeThreshold-r18", nullptr},
    {"srs-PosUplinkTransmissionWindowConfig-r18", nullptr}};
static const asn1::table::seq_opt seq_table_srs_pos_tx_hop_r18_s_opts[] = {
    {1, (int32_t)offsetof(srs_pos_tx_hop_r18_s, bwp_r18_present)},
    {2, (int32_t)offsetof(srs_pos_tx_hop_r18_s, inactive_pos_srs_time_align_timer_r18_present)},
    {3, (int32_t)offsetof(srs_pos_tx_hop_r18_s, inactive_pos_srs_rsrp_change_thres_r18_present)},
    {4, (int32_t)offsetof(srs_pos_tx_hop_r18_s, srs_pos_ul_tx_win_cfg_r18_present)}};
const asn1::table::seq_desc srs_pos_tx_hop_r18_s::desc = {
    {seq_table_srs_pos_tx_hop_r18_s_fields, 5, UINT64_C(0x1), seq_table_srs_pos_tx_hop_r18_s_opts, 4},
    true,
    false,
    nullptr,
    0,
    seq_table_srs_pos_tx_hop_r18_s_names};

// UplinkConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_cfg_s_pusch_serving_cell_cfg_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pusch_serving_cell_cfg_s::desc}};
static const asn1::table::field_name seq_table_ul_cfg_s_pusch_serving_cell_cfg_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_ul_cfg_s_pusch_serving_cell_cfg_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_ul_cfg_s_pusch_serving_cell_cfg_setup_release_alternatives,
    nullptr,
    seq_table_ul_cfg_s_pusch_serving_cell_cfg_setup_release_names,
    "setup_release_c",
    setup_release_c<pusch_serving_cell_cfg_s>::type_offset(),
    setup_release_c<pusch_serving_cell_cfg_s>::value_offset()};
static const asn1::table::seq_field seq_table_ul_cfg_s_carrier_switching_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &srs_carrier_switching_s::desc}};
static const asn1::table::field_name seq_table_ul_cfg_s_carrier_switching_setup_release_names[] = {{"release", nullptr},
                                                                                                   {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_ul_cfg_s_carrier_switching_setup_release        = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_ul_cfg_s_carrier_switching_setup_release_alternatives,
    nullptr,
    seq_table_ul_cfg_s_carrier_switching_setup_release_names,
    "setup_release_c",
    setup_release_c<srs_carrier_switching_s>::type_offset(),
    setup_release_c<srs_carrier_switching_s>::value_offset()};
static const asn1::table::seq_field seq_table_ul_cfg_s_ul_tx_switching_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_tx_switching_r16_s::desc}};
static const asn1::table::field_name seq_table_ul_cfg_s_ul_tx_switching_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_ul_cfg_s_ul_tx_switching_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_ul_cfg_s_ul_tx_switching_r16_setup_release_alternatives,
    nullptr,
    seq_table_ul_cfg_s_ul_tx_switching_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<ul_tx_switching_r16_s>::type_offset(),
    setup_release_c<ul_tx_switching_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_ul_cfg_s_srs_pos_tx_hop_r18_setup_release_alternatives[] = {
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
static const asn1::table::field_name seq_table_ul_cfg_s_srs_pos_tx_hop_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_ul_cfg_s_srs_pos_tx_hop_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_ul_cfg_s_srs_pos_tx_hop_r18_setup_release_alternatives,
    nullptr,
    seq_table_ul_cfg_s_srs_pos_tx_hop_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<srs_pos_tx_hop_r18_s>::type_offset(),
    setup_release_c<srs_pos_tx_hop_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_ul_cfg_s_fields[] = {
    {offsetof(ul_cfg_s, init_ul_bwp),
     (int32_t)offsetof(ul_cfg_s, init_ul_bwp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_ul_ded_s::desc},
    {offsetof(ul_cfg_s, ul_bwp_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<decltype(ul_cfg_s::ul_bwp_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(4), false, false>>::value},
    {offsetof(ul_cfg_s, ul_bwp_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(ul_cfg_s::ul_bwp_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bwp_ul_s::desc},
    {offsetof(ul_cfg_s, first_active_ul_bwp_id),
     (int32_t)offsetof(ul_cfg_s, first_active_ul_bwp_id_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr},
    {offsetof(ul_cfg_s, pusch_serving_cell_cfg),
     (int32_t)offsetof(ul_cfg_s, pusch_serving_cell_cfg_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ul_cfg_s_pusch_serving_cell_cfg_setup_release},
    {offsetof(ul_cfg_s, carrier_switching),
     (int32_t)offsetof(ul_cfg_s, carrier_switching_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_ul_cfg_s_carrier_switching_setup_release}};
static const asn1::table::field_name seq_table_ul_cfg_s_names[] = {{"initialUplinkBWP", nullptr},
                                                                   {"uplinkBWP-ToReleaseList", nullptr},
                                                                   {"uplinkBWP-ToAddModList", nullptr},
                                                                   {"firstActiveUplinkBWP-Id", nullptr},
                                                                   {"pusch-ServingCellConfig", nullptr},
                                                                   {"carrierSwitching", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_cfg_s_opts[]  = {
    {0, (int32_t)offsetof(ul_cfg_s, init_ul_bwp_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(ul_cfg_s, first_active_ul_bwp_id_present)},
    {4, (int32_t)offsetof(ul_cfg_s, pusch_serving_cell_cfg_present)},
    {5, (int32_t)offsetof(ul_cfg_s, carrier_switching_present)}};
static const asn1::table::seq_field seq_table_ul_cfg_s_g0_fields[] = {
    {offsetof(ul_cfg_s, pwr_boost_pi2_bpsk),
     (int32_t)offsetof(ul_cfg_s, pwr_boost_pi2_bpsk_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ul_cfg_s, ul_ch_bw_per_scs_list),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     5,
     &asn1::table::ptr_item_list_codec<decltype(ul_cfg_s::ul_ch_bw_per_scs_list),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &scs_specific_carrier_s::desc}};
static const asn1::table::field_name seq_table_ul_cfg_s_g0_names[] = {{"powerBoostPi2BPSK", nullptr},
                                                                      {"uplinkChannelBW-PerSCS-List", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_cfg_s_g0_opts[]  = {
    {0, (int32_t)offsetof(ul_cfg_s, pwr_boost_pi2_bpsk_present)},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_ul_cfg_s_g1_fields[] = {
    {0,
     (int32_t)offsetof(ul_cfg_s, enable_pl_rs_upd_for_pusch_srs_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ul_cfg_s, enable_default_beam_pl_for_pusch0_0_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ul_cfg_s, enable_default_beam_pl_for_pucch_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(ul_cfg_s, enable_default_beam_pl_for_srs_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ul_cfg_s, ul_tx_switching_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(ul_cfg_s::ul_tx_switching_r16)>::value,
     &seq_table_ul_cfg_s_ul_tx_switching_r16_setup_release},
    {0,
     (int32_t)offsetof(ul_cfg_s, mpr_pwr_boost_fr2_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ul_cfg_s_g1_names[] = {
    {"enablePL-RS-UpdateForPUSCH-SRS-r16", "enabled"},
    {"enableDefaultBeamPL-ForPUSCH0-0-r16", "enabled"},
    {"enableDefaultBeamPL-ForPUCCH-r16", "enabled"},
    {"enableDefaultBeamPL-ForSRS-r16", "enabled"},
    {"uplinkTxSwitching-r16", nullptr},
    {"mpr-PowerBoost-FR2-r16", "true"}};
static const asn1::table::seq_opt seq_table_ul_cfg_s_g1_opts[] = {
    {0, (int32_t)offsetof(ul_cfg_s, enable_pl_rs_upd_for_pusch_srs_r16_present)},
    {1, (int32_t)offsetof(ul_cfg_s, enable_default_beam_pl_for_pusch0_0_r16_present)},
    {2, (int32_t)offsetof(ul_cfg_s, enable_default_beam_pl_for_pucch_r16_present)},
    {3, (int32_t)offsetof(ul_cfg_s, enable_default_beam_pl_for_srs_r16_present)},
    {4, asn1::table::seq_field::ptr_presence},
    {5, (int32_t)offsetof(ul_cfg_s, mpr_pwr_boost_fr2_r16_present)}};
static const asn1::table::seq_field seq_table_ul_cfg_s_g2_fields[] = {
    {offsetof(ul_cfg_s, srs_pos_tx_hop_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(ul_cfg_s::srs_pos_tx_hop_r18)>::value,
     &seq_table_ul_cfg_s_srs_pos_tx_hop_r18_setup_release},
    {0,
     (int32_t)offsetof(ul_cfg_s, enable_pl_rs_upd_for_type1_cg_pusch_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ul_cfg_s, pwr_boost_pi2_bpsk_r18),
     (int32_t)offsetof(ul_cfg_s, pwr_boost_pi2_bpsk_r18_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(ul_cfg_s, pwr_boost_qpsk_r18),
     (int32_t)offsetof(ul_cfg_s, pwr_boost_qpsk_r18_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ul_cfg_s_g2_names[] = {
    {"srs-PosTx-Hopping-r18", nullptr},
    {"enablePL-RS-UpdateForType1CG-PUSCH-r18", "enabled"},
    {"powerBoostPi2BPSK-r18", nullptr},
    {"powerBoostQPSK-r18", nullptr}};
static const asn1::table::seq_opt seq_table_ul_cfg_s_g2_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(ul_cfg_s, enable_pl_rs_upd_for_type1_cg_pusch_r18_present)},
    {2, (int32_t)offsetof(ul_cfg_s, pwr_boost_pi2_bpsk_r18_present)},
    {3, (int32_t)offsetof(ul_cfg_s, pwr_boost_qpsk_r18_present)}};
static const asn1::table::seq_group seq_table_ul_cfg_s_groups[] = {
    {{seq_table_ul_cfg_s_g0_fields, 2, UINT64_C(0x0), seq_table_ul_cfg_s_g0_opts, 2},
     false,
     seq_table_ul_cfg_s_g0_names},
    {{seq_table_ul_cfg_s_g1_fields, 6, UINT64_C(0x0), seq_table_ul_cfg_s_g1_opts, 6},
     false,
     seq_table_ul_cfg_s_g1_names},
    {{seq_table_ul_cfg_s_g2_fields, 4, UINT64_C(0x0), seq_table_ul_cfg_s_g2_opts, 4},
     false,
     seq_table_ul_cfg_s_g2_names}};
const asn1::table::seq_desc ul_cfg_s::desc = {{seq_table_ul_cfg_s_fields, 6, UINT64_C(0x0), seq_table_ul_cfg_s_opts, 6},
                                              true,
                                              false,
                                              seq_table_ul_cfg_s_groups,
                                              3,
                                              seq_table_ul_cfg_s_names};

// SlotFormatCombination ::= SEQUENCE
static const asn1::table::seq_field seq_table_slot_format_combination_s_fields[] = {
    {offsetof(slot_format_combination_s, slot_format_combination_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(511),
     nullptr},
    {offsetof(slot_format_combination_s, slot_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     256,
     &asn1::table::list_codec<decltype(slot_format_combination_s::slot_formats),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(255), false, false>>::value}};
static const asn1::table::field_name seq_table_slot_format_combination_s_names[] = {
    {"slotFormatCombinationId", nullptr},
    {"slotFormats", nullptr}};
const asn1::table::seq_desc slot_format_combination_s::desc = {
    {seq_table_slot_format_combination_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_slot_format_combination_s_names};

// SlotFormatCombinationsPerCell ::= SEQUENCE
static const asn1::table::seq_field seq_table_slot_format_combinations_per_cell_s_fields[] = {
    {offsetof(slot_format_combinations_per_cell_s, serving_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(slot_format_combinations_per_cell_s, subcarrier_spacing),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(slot_format_combinations_per_cell_s::subcarrier_spacing)>(),
     decltype(slot_format_combinations_per_cell_s::subcarrier_spacing)::has_ext,
     false,
     decltype(slot_format_combinations_per_cell_s::subcarrier_spacing)::nof_types,
     decltype(slot_format_combinations_per_cell_s::subcarrier_spacing)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(slot_format_combinations_per_cell_s, subcarrier_spacing2),
     (int32_t)offsetof(slot_format_combinations_per_cell_s, subcarrier_spacing2_present),
     asn1::table::enum_op<decltype(slot_format_combinations_per_cell_s::subcarrier_spacing2)>(),
     decltype(slot_format_combinations_per_cell_s::subcarrier_spacing2)::has_ext,
     false,
     decltype(slot_format_combinations_per_cell_s::subcarrier_spacing2)::nof_types,
     decltype(slot_format_combinations_per_cell_s::subcarrier_spacing2)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(slot_format_combinations_per_cell_s, slot_format_combinations),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     512,
     &asn1::table::item_list_codec<decltype(slot_format_combinations_per_cell_s::slot_format_combinations),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &slot_format_combination_s::desc},
    {offsetof(slot_format_combinations_per_cell_s, position_in_dci),
     (int32_t)offsetof(slot_format_combinations_per_cell_s, position_in_dci_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr}};
static const asn1::table::field_name seq_table_slot_format_combinations_per_cell_s_names[] = {
    {"servingCellId", nullptr},
    {"subcarrierSpacing", nullptr},
    {"subcarrierSpacing2", nullptr},
    {"slotFormatCombinations", nullptr},
    {"positionInDCI", nullptr}};
static const asn1::table::seq_opt seq_table_slot_format_combinations_per_cell_s_opts[] = {
    {2, (int32_t)offsetof(slot_format_combinations_per_cell_s, subcarrier_spacing2_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(slot_format_combinations_per_cell_s, position_in_dci_present)}};
static const asn1::table::seq_field seq_table_slot_format_combinations_per_cell_s_g0_fields[] = {
    {0,
     (int32_t)offsetof(slot_format_combinations_per_cell_s, enable_cfg_ul_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_slot_format_combinations_per_cell_s_g0_names[] = {
    {"enableConfiguredUL-r16", "enabled"}};
static const asn1::table::seq_opt seq_table_slot_format_combinations_per_cell_s_g0_opts[] = {
    {0, (int32_t)offsetof(slot_format_combinations_per_cell_s, enable_cfg_ul_r16_present)}};
static const asn1::table::seq_group seq_table_slot_format_combinations_per_cell_s_groups[] = {
    {{seq_table_slot_format_combinations_per_cell_s_g0_fields,
      1,
      UINT64_C(0x0),
      seq_table_slot_format_combinations_per_cell_s_g0_opts,
      1},
     false,
     seq_table_slot_format_combinations_per_cell_s_g0_names}};
const asn1::table::seq_desc slot_format_combinations_per_cell_s::desc = {
    {seq_table_slot_format_combinations_per_cell_s_fields,
     5,
     UINT64_C(0x3),
     seq_table_slot_format_combinations_per_cell_s_opts,
     3},
    true,
    false,
    seq_table_slot_format_combinations_per_cell_s_groups,
    1,
    seq_table_slot_format_combinations_per_cell_s_names};

// SearchSpaceSwitchTrigger-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_search_space_switch_trigger_r16_s_fields[] = {
    {offsetof(search_space_switch_trigger_r16_s, serving_cell_id_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(search_space_switch_trigger_r16_s, position_in_dci_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(127),
     nullptr}};
static const asn1::table::field_name seq_table_search_space_switch_trigger_r16_s_names[] = {
    {"servingCellId-r16", nullptr},
    {"positionInDCI-r16", nullptr}};
const asn1::table::seq_desc search_space_switch_trigger_r16_s::desc = {
    {seq_table_search_space_switch_trigger_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_search_space_switch_trigger_r16_s_names};

// SlotFormatIndicator ::= SEQUENCE
static const asn1::table::seq_field seq_table_slot_format_ind_s_fields[] = {
    {offsetof(slot_format_ind_s, sfi_rnti),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(slot_format_ind_s, dci_payload_size),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(128),
     nullptr},
    {offsetof(slot_format_ind_s, slot_format_comb_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::item_list_codec<decltype(slot_format_ind_s::slot_format_comb_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &slot_format_combinations_per_cell_s::desc},
    {offsetof(slot_format_ind_s, slot_format_comb_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<decltype(slot_format_ind_s::slot_format_comb_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(31), false, false>>::value}};
static const asn1::table::field_name seq_table_slot_format_ind_s_names[] = {{"sfi-RNTI", nullptr},
                                                                            {"dci-PayloadSize", nullptr},
                                                                            {"slotFormatCombToAddModList", nullptr},
                                                                            {"slotFormatCombToReleaseList", nullptr}};
static const asn1::table::seq_opt    seq_table_slot_format_ind_s_opts[]  = {{2, asn1::table::seq_field::list_presence},
                                                                            {3, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field  seq_table_slot_format_ind_s_g0_fields[] = {
    {offsetof(slot_format_ind_s, available_rb_sets_to_add_mod_list_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      16,
      &asn1::table::ptr_item_list_codec<decltype(slot_format_ind_s::available_rb_sets_to_add_mod_list_r16),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &available_rb_sets_per_cell_r16_s::desc},
    {offsetof(slot_format_ind_s, available_rb_sets_to_release_list_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      16,
      &asn1::table::ptr_list_codec<decltype(slot_format_ind_s::available_rb_sets_to_release_list_r16),
                                   asn1::table::int_json,
                                   asn1::table::int_items<false, INT64_C(0), UINT64_C(31), false, false>>::value},
    {offsetof(slot_format_ind_s, switch_trigger_to_add_mod_list_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      4,
      &asn1::table::ptr_item_list_codec<decltype(slot_format_ind_s::switch_trigger_to_add_mod_list_r16),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &search_space_switch_trigger_r16_s::desc},
    {offsetof(slot_format_ind_s, switch_trigger_to_release_list_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      4,
      &asn1::table::ptr_list_codec<decltype(slot_format_ind_s::switch_trigger_to_release_list_r16),
                                   asn1::table::int_json,
                                   asn1::table::int_items<false, INT64_C(0), UINT64_C(31), false, false>>::value},
    {offsetof(slot_format_ind_s, co_durs_per_cell_to_add_mod_list_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      16,
      &asn1::table::ptr_item_list_codec<decltype(slot_format_ind_s::co_durs_per_cell_to_add_mod_list_r16),
                                        asn1::table::sequence_item_ops,
                                        false,
                                        true>::value,
      &co_durs_per_cell_r16_s::desc},
    {offsetof(slot_format_ind_s, co_durs_per_cell_to_release_list_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::codec,
      false,
      false,
      1,
      16,
      &asn1::table::ptr_list_codec<decltype(slot_format_ind_s::co_durs_per_cell_to_release_list_r16),
                                   asn1::table::int_json,
                                   asn1::table::int_items<false, INT64_C(0), UINT64_C(31), false, false>>::value}};
static const asn1::table::field_name seq_table_slot_format_ind_s_g0_names[] = {
    {"availableRB-SetsToAddModList-r16", nullptr},
    {"availableRB-SetsToReleaseList-r16", nullptr},
    {"switchTriggerToAddModList-r16", nullptr},
    {"switchTriggerToReleaseList-r16", nullptr},
    {"co-DurationsPerCellToAddModList-r16", nullptr},
    {"co-DurationsPerCellToReleaseList-r16", nullptr}};
static const asn1::table::seq_opt   seq_table_slot_format_ind_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence},
                                                                             {1, asn1::table::seq_field::ptr_presence},
                                                                             {2, asn1::table::seq_field::ptr_presence},
                                                                             {3, asn1::table::seq_field::ptr_presence},
                                                                             {4, asn1::table::seq_field::ptr_presence},
                                                                             {5, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_slot_format_ind_s_g1_fields[] = {
    {offsetof(slot_format_ind_s, switch_trigger_to_add_mod_list_size_ext_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::ptr_item_list_codec<decltype(slot_format_ind_s::switch_trigger_to_add_mod_list_size_ext_r16),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &search_space_switch_trigger_r16_s::desc},
    {offsetof(slot_format_ind_s, switch_trigger_to_release_list_size_ext_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     12,
     &asn1::table::ptr_list_codec<decltype(slot_format_ind_s::switch_trigger_to_release_list_size_ext_r16),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(0), UINT64_C(31), false, false>>::value}};
static const asn1::table::field_name seq_table_slot_format_ind_s_g1_names[] = {
    {"switchTriggerToAddModListSizeExt-r16", nullptr},
    {"switchTriggerToReleaseListSizeExt-r16", nullptr}};
static const asn1::table::seq_opt   seq_table_slot_format_ind_s_g1_opts[] = {{0, asn1::table::seq_field::ptr_presence},
                                                                             {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_slot_format_ind_s_g2_fields[] = {
    {offsetof(slot_format_ind_s, co_durs_per_cell_to_add_mod_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::ptr_item_list_codec<decltype(slot_format_ind_s::co_durs_per_cell_to_add_mod_list_r17),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &co_durs_per_cell_r17_s::desc}};
static const asn1::table::field_name seq_table_slot_format_ind_s_g2_names[] = {
    {"co-DurationsPerCellToAddModList-r17", nullptr}};
static const asn1::table::seq_opt   seq_table_slot_format_ind_s_g2_opts[] = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_slot_format_ind_s_groups[]  = {
    {{seq_table_slot_format_ind_s_g0_fields, 6, UINT64_C(0x0), seq_table_slot_format_ind_s_g0_opts, 6},
      false,
      seq_table_slot_format_ind_s_g0_names},
    {{seq_table_slot_format_ind_s_g1_fields, 2, UINT64_C(0x0), seq_table_slot_format_ind_s_g1_opts, 2},
      false,
      seq_table_slot_format_ind_s_g1_names},
    {{seq_table_slot_format_ind_s_g2_fields, 1, UINT64_C(0x0), seq_table_slot_format_ind_s_g2_opts, 1},
      false,
      seq_table_slot_format_ind_s_g2_names}};
const asn1::table::seq_desc slot_format_ind_s::desc = {
    {seq_table_slot_format_ind_s_fields, 4, UINT64_C(0x3), seq_table_slot_format_ind_s_opts, 2},
    true,
    false,
    seq_table_slot_format_ind_s_groups,
    3,
    seq_table_slot_format_ind_s_names};

// PDCCH-ServingCellConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdcch_serving_cell_cfg_s_slot_format_ind_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &slot_format_ind_s::desc}};
static const asn1::table::field_name seq_table_pdcch_serving_cell_cfg_s_slot_format_ind_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pdcch_serving_cell_cfg_s_slot_format_ind_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pdcch_serving_cell_cfg_s_slot_format_ind_setup_release_alternatives,
    nullptr,
    seq_table_pdcch_serving_cell_cfg_s_slot_format_ind_setup_release_names,
    "setup_release_c",
    setup_release_c<slot_format_ind_s>::type_offset(),
    setup_release_c<slot_format_ind_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_pdcch_serving_cell_cfg_s_availability_ind_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &availability_ind_r16_s::desc}};
static const asn1::table::field_name seq_table_pdcch_serving_cell_cfg_s_availability_ind_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pdcch_serving_cell_cfg_s_availability_ind_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pdcch_serving_cell_cfg_s_availability_ind_r16_setup_release_alternatives,
    nullptr,
    seq_table_pdcch_serving_cell_cfg_s_availability_ind_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<availability_ind_r16_s>::type_offset(),
    setup_release_c<availability_ind_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_pdcch_serving_cell_cfg_s_fields[] = {
    {offsetof(pdcch_serving_cell_cfg_s, slot_format_ind),
     (int32_t)offsetof(pdcch_serving_cell_cfg_s, slot_format_ind_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_pdcch_serving_cell_cfg_s_slot_format_ind_setup_release}};
static const asn1::table::field_name seq_table_pdcch_serving_cell_cfg_s_names[] = {{"slotFormatIndicator", nullptr}};
static const asn1::table::seq_opt    seq_table_pdcch_serving_cell_cfg_s_opts[]  = {
    {0, (int32_t)offsetof(pdcch_serving_cell_cfg_s, slot_format_ind_present)}};
static const asn1::table::seq_field seq_table_pdcch_serving_cell_cfg_s_g0_fields[] = {
    {offsetof(pdcch_serving_cell_cfg_s, availability_ind_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pdcch_serving_cell_cfg_s::availability_ind_r16)>::value,
     &seq_table_pdcch_serving_cell_cfg_s_availability_ind_r16_setup_release},
    {offsetof(pdcch_serving_cell_cfg_s, search_space_switch_timer_r16),
     (int32_t)offsetof(pdcch_serving_cell_cfg_s, search_space_switch_timer_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(80),
     nullptr}};
static const asn1::table::field_name seq_table_pdcch_serving_cell_cfg_s_g0_names[] = {
    {"availabilityIndicator-r16", nullptr},
    {"searchSpaceSwitchTimer-r16", nullptr}};
static const asn1::table::seq_opt seq_table_pdcch_serving_cell_cfg_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(pdcch_serving_cell_cfg_s, search_space_switch_timer_r16_present)}};
static const asn1::table::seq_field seq_table_pdcch_serving_cell_cfg_s_g1_fields[] = {
    {offsetof(pdcch_serving_cell_cfg_s, search_space_switch_timer_v1710),
     (int32_t)offsetof(pdcch_serving_cell_cfg_s, search_space_switch_timer_v1710_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(81),
     UINT64_C(1280),
     nullptr}};
static const asn1::table::field_name seq_table_pdcch_serving_cell_cfg_s_g1_names[] = {
    {"searchSpaceSwitchTimer-v1710", nullptr}};
static const asn1::table::seq_opt seq_table_pdcch_serving_cell_cfg_s_g1_opts[] = {
    {0, (int32_t)offsetof(pdcch_serving_cell_cfg_s, search_space_switch_timer_v1710_present)}};
static const asn1::table::seq_group seq_table_pdcch_serving_cell_cfg_s_groups[] = {
    {{seq_table_pdcch_serving_cell_cfg_s_g0_fields, 2, UINT64_C(0x0), seq_table_pdcch_serving_cell_cfg_s_g0_opts, 2},
     false,
     seq_table_pdcch_serving_cell_cfg_s_g0_names},
    {{seq_table_pdcch_serving_cell_cfg_s_g1_fields, 1, UINT64_C(0x0), seq_table_pdcch_serving_cell_cfg_s_g1_opts, 1},
     false,
     seq_table_pdcch_serving_cell_cfg_s_g1_names}};
const asn1::table::seq_desc pdcch_serving_cell_cfg_s::desc = {
    {seq_table_pdcch_serving_cell_cfg_s_fields, 1, UINT64_C(0x0), seq_table_pdcch_serving_cell_cfg_s_opts, 1},
    true,
    false,
    seq_table_pdcch_serving_cell_cfg_s_groups,
    2,
    seq_table_pdcch_serving_cell_cfg_s_names};

// PDSCH-CodeBlockGroupTransmission ::= SEQUENCE
static const asn1::table::seq_field seq_table_pdsch_code_block_group_tx_s_fields[] = {
    {offsetof(pdsch_code_block_group_tx_s, max_code_block_groups_per_transport_block),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block)>(),
     decltype(pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block)::has_ext,
     false,
     decltype(pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block)::nof_types,
     decltype(pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block)::nof_exts,
     nullptr,
     &pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::desc},
    {offsetof(pdsch_code_block_group_tx_s, code_block_group_flush_ind),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdsch_code_block_group_tx_s_names[] = {
    {"maxCodeBlockGroupsPerTransportBlock", nullptr},
    {"codeBlockGroupFlushIndicator", nullptr}};
const asn1::table::seq_desc pdsch_code_block_group_tx_s::desc = {
    {seq_table_pdsch_code_block_group_tx_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_pdsch_code_block_group_tx_s_names};

static const char* enum_names_pdsch_code_block_group_tx_s_max_code_block_groups_per_transport_block_e__values[] =
    {"n2", "n4", "n6", "n8"};
const asn1::table::enum_desc pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::desc = {
    enum_names_pdsch_code_block_group_tx_s_max_code_block_groups_per_transport_block_e__values,
    4,
    "pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_e_"};
uint8_t pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8};
  return map_enum_number(
      numbers, 4, value, "pdsch_code_block_group_tx_s::max_code_block_groups_per_transport_block_e_");
}

// PDSCH-ServingCellConfig ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_pdsch_serving_cell_cfg_s_code_block_group_tx_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &pdsch_code_block_group_tx_s::desc}};
static const asn1::table::field_name seq_table_pdsch_serving_cell_cfg_s_code_block_group_tx_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pdsch_serving_cell_cfg_s_code_block_group_tx_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pdsch_serving_cell_cfg_s_code_block_group_tx_setup_release_alternatives,
    nullptr,
    seq_table_pdsch_serving_cell_cfg_s_code_block_group_tx_setup_release_names,
    "setup_release_c",
    setup_release_c<pdsch_code_block_group_tx_s>::type_offset(),
    setup_release_c<pdsch_code_block_group_tx_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_pdsch_serving_cell_cfg_s_pdsch_code_block_group_tx_list_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         2,
         &asn1::table::item_list_codec<setup_release_c<dyn_seq_of<pdsch_code_block_group_tx_s, 1, 2>>::value_type,
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
         &pdsch_code_block_group_tx_s::desc}};
static const asn1::table::field_name
    seq_table_pdsch_serving_cell_cfg_s_pdsch_code_block_group_tx_list_r16_setup_release_names[] = {{"release", nullptr},
                                                                                                   {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_pdsch_serving_cell_cfg_s_pdsch_code_block_group_tx_list_r16_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_pdsch_serving_cell_cfg_s_pdsch_code_block_group_tx_list_r16_setup_release_alternatives,
        nullptr,
        seq_table_pdsch_serving_cell_cfg_s_pdsch_code_block_group_tx_list_r16_setup_release_names,
        "setup_release_c",
        setup_release_c<dyn_seq_of<pdsch_code_block_group_tx_s, 1, 2>>::type_offset(),
        setup_release_c<dyn_seq_of<pdsch_code_block_group_tx_s, 1, 2>>::value_offset()};
static const asn1::table::seq_field
    seq_table_pdsch_serving_cell_cfg_s_dl_harq_feedback_disabled_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         0,
         0,
         &asn1::table::object_codec<setup_release_c<fixed_bitstring<32>>::value_type,
                                    asn1::table::struct_json>::value}};
static const asn1::table::field_name
    seq_table_pdsch_serving_cell_cfg_s_dl_harq_feedback_disabled_r17_setup_release_names[] = {{"release", nullptr},
                                                                                              {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_pdsch_serving_cell_cfg_s_dl_harq_feedback_disabled_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_pdsch_serving_cell_cfg_s_dl_harq_feedback_disabled_r17_setup_release_alternatives,
    nullptr,
    seq_table_pdsch_serving_cell_cfg_s_dl_harq_feedback_disabled_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<fixed_bitstring<32>>::type_offset(),
    setup_release_c<fixed_bitstring<32>>::value_offset()};
static const asn1::table::seq_field seq_table_pdsch_serving_cell_cfg_s_fields[] = {
    {offsetof(pdsch_serving_cell_cfg_s, code_block_group_tx),
     (int32_t)offsetof(pdsch_serving_cell_cfg_s, code_block_group_tx_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_pdsch_serving_cell_cfg_s_code_block_group_tx_setup_release},
    {offsetof(pdsch_serving_cell_cfg_s, xoverhead),
     (int32_t)offsetof(pdsch_serving_cell_cfg_s, xoverhead_present),
     asn1::table::enum_op<decltype(pdsch_serving_cell_cfg_s::xoverhead)>(),
     decltype(pdsch_serving_cell_cfg_s::xoverhead)::has_ext,
     false,
     decltype(pdsch_serving_cell_cfg_s::xoverhead)::nof_types,
     decltype(pdsch_serving_cell_cfg_s::xoverhead)::nof_exts,
     nullptr,
     &pdsch_serving_cell_cfg_s::xoverhead_opts::desc},
    {offsetof(pdsch_serving_cell_cfg_s, nrof_harq_processes_for_pdsch),
     (int32_t)offsetof(pdsch_serving_cell_cfg_s, nrof_harq_processes_for_pdsch_present),
     asn1::table::enum_op<decltype(pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch)>(),
     decltype(pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch)::has_ext,
     false,
     decltype(pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch)::nof_types,
     decltype(pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch)::nof_exts,
     nullptr,
     &pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::desc},
    {offsetof(pdsch_serving_cell_cfg_s, pucch_cell),
     (int32_t)offsetof(pdsch_serving_cell_cfg_s, pucch_cell_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr}};
static const asn1::table::field_name seq_table_pdsch_serving_cell_cfg_s_names[] = {
    {"codeBlockGroupTransmission", nullptr},
    {"xOverhead", nullptr},
    {"nrofHARQ-ProcessesForPDSCH", nullptr},
    {"pucch-Cell", nullptr}};
static const asn1::table::seq_opt seq_table_pdsch_serving_cell_cfg_s_opts[] = {
    {0, (int32_t)offsetof(pdsch_serving_cell_cfg_s, code_block_group_tx_present)},
    {1, (int32_t)offsetof(pdsch_serving_cell_cfg_s, xoverhead_present)},
    {2, (int32_t)offsetof(pdsch_serving_cell_cfg_s, nrof_harq_processes_for_pdsch_present)},
    {3, (int32_t)offsetof(pdsch_serving_cell_cfg_s, pucch_cell_present)}};
static const asn1::table::seq_field seq_table_pdsch_serving_cell_cfg_s_g0_fields[] = {
    {offsetof(pdsch_serving_cell_cfg_s, max_mimo_layers),
     (int32_t)offsetof(pdsch_serving_cell_cfg_s, max_mimo_layers_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(8),
     nullptr},
    {offsetof(pdsch_serving_cell_cfg_s, processing_type2_enabled),
     (int32_t)offsetof(pdsch_serving_cell_cfg_s, processing_type2_enabled_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdsch_serving_cell_cfg_s_g0_names[] = {
    {"maxMIMO-Layers", nullptr},
    {"processingType2Enabled", nullptr}};
static const asn1::table::seq_opt seq_table_pdsch_serving_cell_cfg_s_g0_opts[] = {
    {0, (int32_t)offsetof(pdsch_serving_cell_cfg_s, max_mimo_layers_present)},
    {1, (int32_t)offsetof(pdsch_serving_cell_cfg_s, processing_type2_enabled_present)}};
static const asn1::table::seq_field seq_table_pdsch_serving_cell_cfg_s_g1_fields[] = {
    {offsetof(pdsch_serving_cell_cfg_s, pdsch_code_block_group_tx_list_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pdsch_serving_cell_cfg_s::pdsch_code_block_group_tx_list_r16)>::value,
     &seq_table_pdsch_serving_cell_cfg_s_pdsch_code_block_group_tx_list_r16_setup_release}};
static const asn1::table::field_name seq_table_pdsch_serving_cell_cfg_s_g1_names[] = {
    {"pdsch-CodeBlockGroupTransmissionList-r16", nullptr}};
static const asn1::table::seq_opt seq_table_pdsch_serving_cell_cfg_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_pdsch_serving_cell_cfg_s_g2_fields[] = {
    {offsetof(pdsch_serving_cell_cfg_s, dl_harq_feedback_disabled_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(pdsch_serving_cell_cfg_s::dl_harq_feedback_disabled_r17)>::value,
     &seq_table_pdsch_serving_cell_cfg_s_dl_harq_feedback_disabled_r17_setup_release},
    {0,
     (int32_t)offsetof(pdsch_serving_cell_cfg_s, nrof_harq_processes_for_pdsch_v1700_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_pdsch_serving_cell_cfg_s_g2_names[] = {
    {"downlinkHARQ-FeedbackDisabled-r17", nullptr},
    {"nrofHARQ-ProcessesForPDSCH-v1700", "n32"}};
static const asn1::table::seq_opt seq_table_pdsch_serving_cell_cfg_s_g2_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(pdsch_serving_cell_cfg_s, nrof_harq_processes_for_pdsch_v1700_present)}};
static const asn1::table::seq_group seq_table_pdsch_serving_cell_cfg_s_groups[] = {
    {{seq_table_pdsch_serving_cell_cfg_s_g0_fields, 2, UINT64_C(0x0), seq_table_pdsch_serving_cell_cfg_s_g0_opts, 2},
     false,
     seq_table_pdsch_serving_cell_cfg_s_g0_names},
    {{seq_table_pdsch_serving_cell_cfg_s_g1_fields, 1, UINT64_C(0x0), seq_table_pdsch_serving_cell_cfg_s_g1_opts, 1},
     false,
     seq_table_pdsch_serving_cell_cfg_s_g1_names},
    {{seq_table_pdsch_serving_cell_cfg_s_g2_fields, 2, UINT64_C(0x0), seq_table_pdsch_serving_cell_cfg_s_g2_opts, 2},
     false,
     seq_table_pdsch_serving_cell_cfg_s_g2_names}};
const asn1::table::seq_desc pdsch_serving_cell_cfg_s::desc = {
    {seq_table_pdsch_serving_cell_cfg_s_fields, 4, UINT64_C(0x0), seq_table_pdsch_serving_cell_cfg_s_opts, 4},
    true,
    false,
    seq_table_pdsch_serving_cell_cfg_s_groups,
    3,
    seq_table_pdsch_serving_cell_cfg_s_names};

static const char*           enum_names_pdsch_serving_cell_cfg_s_xoverhead_e__values[] = {"xOh6", "xOh12", "xOh18"};
const asn1::table::enum_desc pdsch_serving_cell_cfg_s::xoverhead_opts::desc            = {
    enum_names_pdsch_serving_cell_cfg_s_xoverhead_e__values,
    3,
    "pdsch_serving_cell_cfg_s::xoverhead_e_"};
uint8_t pdsch_serving_cell_cfg_s::xoverhead_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 12, 18};
  return map_enum_number(numbers, 3, value, "pdsch_serving_cell_cfg_s::xoverhead_e_");
}

static const char* enum_names_pdsch_serving_cell_cfg_s_nrof_harq_processes_for_pdsch_e__values[] =
    {"n2", "n4", "n6", "n10", "n12", "n16"};
const asn1::table::enum_desc pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::desc = {
    enum_names_pdsch_serving_cell_cfg_s_nrof_harq_processes_for_pdsch_e__values,
    6,
    "pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_e_"};
uint8_t pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 10, 12, 16};
  return map_enum_number(numbers, 6, value, "pdsch_serving_cell_cfg_s::nrof_harq_processes_for_pdsch_e_");
}

// CrossCarrierSchedulingConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_cross_carrier_sched_cfg_s_carrier_ind_size_r16_s__fields[] = {
    {offsetof(cross_carrier_sched_cfg_s::carrier_ind_size_r16_s_, carrier_ind_size_dci_1_2_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(cross_carrier_sched_cfg_s::carrier_ind_size_r16_s_, carrier_ind_size_dci_0_2_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr}};
static const asn1::table::field_name seq_table_cross_carrier_sched_cfg_s_carrier_ind_size_r16_s__names[] = {
    {"carrierIndicatorSizeDCI-1-2-r16", nullptr},
    {"carrierIndicatorSizeDCI-0-2-r16", nullptr}};
static const asn1::table::seq_desc seq_table_cross_carrier_sched_cfg_s_carrier_ind_size_r16_s_ = {
    {seq_table_cross_carrier_sched_cfg_s_carrier_ind_size_r16_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_cross_carrier_sched_cfg_s_carrier_ind_size_r16_s__names};
static const asn1::table::seq_field seq_table_cross_carrier_sched_cfg_s_fields[] = {
    {offsetof(cross_carrier_sched_cfg_s, sched_cell_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cross_carrier_sched_cfg_s::sched_cell_info_c_::desc}};
static const asn1::table::field_name seq_table_cross_carrier_sched_cfg_s_names[] = {{"schedulingCellInfo", nullptr}};
static const asn1::table::seq_field  seq_table_cross_carrier_sched_cfg_s_g0_fields[] = {
    {offsetof(cross_carrier_sched_cfg_s, carrier_ind_size_r16),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(cross_carrier_sched_cfg_s::carrier_ind_size_r16)>::value,
      &seq_table_cross_carrier_sched_cfg_s_carrier_ind_size_r16_s_},
    {0,
      (int32_t)offsetof(cross_carrier_sched_cfg_s, enable_default_beam_for_cc_s_r16_present),
      asn1::table::field_op::none,
      false,
      false,
      0,
      0,
      nullptr}};
static const asn1::table::field_name seq_table_cross_carrier_sched_cfg_s_g0_names[] = {
    {"carrierIndicatorSize-r16", nullptr},
    {"enableDefaultBeamForCCS-r16", "enabled"}};
static const asn1::table::seq_opt seq_table_cross_carrier_sched_cfg_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, (int32_t)offsetof(cross_carrier_sched_cfg_s, enable_default_beam_for_cc_s_r16_present)}};
static const asn1::table::seq_field seq_table_cross_carrier_sched_cfg_s_g1_fields[] = {
    {offsetof(cross_carrier_sched_cfg_s, ccs_blind_detection_split_r17),
     (int32_t)offsetof(cross_carrier_sched_cfg_s, ccs_blind_detection_split_r17_present),
     asn1::table::enum_op<decltype(cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17)>(),
     decltype(cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17)::has_ext,
     false,
     decltype(cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17)::nof_types,
     decltype(cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17)::nof_exts,
     nullptr,
     &cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_opts::desc}};
static const asn1::table::field_name seq_table_cross_carrier_sched_cfg_s_g1_names[] = {
    {"ccs-BlindDetectionSplit-r17", nullptr}};
static const asn1::table::seq_opt seq_table_cross_carrier_sched_cfg_s_g1_opts[] = {
    {0, (int32_t)offsetof(cross_carrier_sched_cfg_s, ccs_blind_detection_split_r17_present)}};
static const asn1::table::seq_group seq_table_cross_carrier_sched_cfg_s_groups[] = {
    {{seq_table_cross_carrier_sched_cfg_s_g0_fields, 2, UINT64_C(0x0), seq_table_cross_carrier_sched_cfg_s_g0_opts, 2},
     false,
     seq_table_cross_carrier_sched_cfg_s_g0_names},
    {{seq_table_cross_carrier_sched_cfg_s_g1_fields, 1, UINT64_C(0x0), seq_table_cross_carrier_sched_cfg_s_g1_opts, 1},
     false,
     seq_table_cross_carrier_sched_cfg_s_g1_names}};
const asn1::table::seq_desc cross_carrier_sched_cfg_s::desc = {
    {seq_table_cross_carrier_sched_cfg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    seq_table_cross_carrier_sched_cfg_s_groups,
    2,
    seq_table_cross_carrier_sched_cfg_s_names};

static const asn1::table::storage_ops* const
    choice_storage_cross_carrier_sched_cfg_s_sched_cell_info_c__alternatives[] = {
        &asn1::table::alternative_ops<cross_carrier_sched_cfg_s::sched_cell_info_c_::own_s_, false>::value,
        &asn1::table::alternative_ops<cross_carrier_sched_cfg_s::sched_cell_info_c_::other_s_, false>::value};
const asn1::table::choice_storage_desc cross_carrier_sched_cfg_s::sched_cell_info_c_::storage_desc = {
    choice_storage_cross_carrier_sched_cfg_s_sched_cell_info_c__alternatives,
    2,
    "cross_carrier_sched_cfg_s::sched_cell_info_c_"};

cross_carrier_sched_cfg_s::sched_cell_info_c_::own_s_& cross_carrier_sched_cfg_s::sched_cell_info_c_::set_own()
{
  set(types::own);
  return c.get<own_s_>();
}
cross_carrier_sched_cfg_s::sched_cell_info_c_::other_s_& cross_carrier_sched_cfg_s::sched_cell_info_c_::set_other()
{
  set(types::other);
  return c.get<other_s_>();
}
static const asn1::table::seq_field seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__own_s__fields[] = {
    {offsetof(cross_carrier_sched_cfg_s::sched_cell_info_c_::own_s_, cif_presence),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__own_s__names[] = {
    {"cif-Presence", nullptr}};
static const asn1::table::seq_desc seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__own_s_ = {
    {seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__own_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__own_s__names};
static const asn1::table::seq_field seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__other_s__fields[] = {
    {offsetof(cross_carrier_sched_cfg_s::sched_cell_info_c_::other_s_, sched_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(31),
     nullptr},
    {offsetof(cross_carrier_sched_cfg_s::sched_cell_info_c_::other_s_, cif_in_sched_cell),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(7),
     nullptr}};
static const asn1::table::field_name seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__other_s__names[] = {
    {"schedulingCellId", nullptr},
    {"cif-InSchedulingCell", nullptr}};
static const asn1::table::seq_desc seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__other_s_ = {
    {seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__other_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__other_s__names};
static const asn1::table::seq_field choice_table_cross_carrier_sched_cfg_s_sched_cell_info_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__own_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_cross_carrier_sched_cfg_s_sched_cell_info_c__other_s_}};
static const asn1::table::field_name choice_table_cross_carrier_sched_cfg_s_sched_cell_info_c__names[] = {
    {"own", nullptr},
    {"other", nullptr}};
const asn1::table::choice_desc cross_carrier_sched_cfg_s::sched_cell_info_c_::desc = {
    asn1::table::enum_int_type<cross_carrier_sched_cfg_s::sched_cell_info_c_::types>(),
    cross_carrier_sched_cfg_s::sched_cell_info_c_::types::nof_types,
    cross_carrier_sched_cfg_s::sched_cell_info_c_::types::nof_exts,
    cross_carrier_sched_cfg_s::sched_cell_info_c_::types::has_ext,
    false,
    choice_table_cross_carrier_sched_cfg_s_sched_cell_info_c__alternatives,
    nullptr,
    choice_table_cross_carrier_sched_cfg_s_sched_cell_info_c__names,
    "cross_carrier_sched_cfg_s::sched_cell_info_c_",
    offsetof(cross_carrier_sched_cfg_s::sched_cell_info_c_, c.type),
    offsetof(cross_carrier_sched_cfg_s::sched_cell_info_c_, c.value),
    false,
    &cross_carrier_sched_cfg_s::sched_cell_info_c_::storage_desc};

static const char*           enum_names_cross_carrier_sched_cfg_s_sched_cell_info_c__types_values[] = {"own", "other"};
const asn1::table::enum_desc cross_carrier_sched_cfg_s::sched_cell_info_c_::types_opts::desc        = {
    enum_names_cross_carrier_sched_cfg_s_sched_cell_info_c__types_values,
    2,
    "cross_carrier_sched_cfg_s::sched_cell_info_c_::types"};

static const char* enum_names_cross_carrier_sched_cfg_s_ccs_blind_detection_split_r17_e__values[] =
    {"oneSeventh", "threeFourteenth", "twoSeventh", "threeSeventh", "oneHalf", "fourSeventh", "fiveSeventh", "spare1"};
const asn1::table::enum_desc cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_opts::desc = {
    enum_names_cross_carrier_sched_cfg_s_ccs_blind_detection_split_r17_e__values,
    8,
    "cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_e_"};
float cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_opts::to_number() const
{
  static const float numbers[] = {0.14285714285714285,
                                  0.21428571428571427,
                                  0.2857142857142857,
                                  0.42857142857142855,
                                  0.5,
                                  0.5714285714285714,
                                  0.7142857142857143};
  return map_enum_number(numbers, 7, value, "cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_e_");
}
const char* cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"1/7", "3/14", "2/7", "3/7", "1/2", "4/7", "5/7"};
  return convert_enum_idx(number_strs, 8, value, "cross_carrier_sched_cfg_s::ccs_blind_detection_split_r17_e_");
}

// TDD-UL-DL-SlotConfig-IAB-MT-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_fields[] = {
    {offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s, slot_idx_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(319),
     nullptr},
    {offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s, symbols_iab_mt_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::desc}};
static const asn1::table::field_name seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_names[] = {
    {"slotIndex-r16", nullptr},
    {"symbols-IAB-MT-r16", nullptr}};
const asn1::table::seq_desc tdd_ul_dl_slot_cfg_iab_mt_r16_s::desc = {
    {seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_names};

static const asn1::table::storage_ops* const
    choice_storage_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__alternatives[] = {
        nullptr,
        nullptr,
        &asn1::table::alternative_ops<tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_r16_s_,
                                      false>::value,
        &asn1::table::alternative_ops<tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_iab_mt_r16_s_,
                                      false>::value};
const asn1::table::choice_storage_desc tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::storage_desc = {
    choice_storage_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__alternatives,
    4,
    "tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_"};

void tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::set_all_dl_r16()
{
  set(types::all_dl_r16);
}
void tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::set_all_ul_r16()
{
  set(types::all_ul_r16);
}
tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_r16_s_&
tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::set_explicit_r16()
{
  set(types::explicit_r16);
  return c.get<explicit_r16_s_>();
}
tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_iab_mt_r16_s_&
tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::set_explicit_iab_mt_r16()
{
  set(types::explicit_iab_mt_r16);
  return c.get<explicit_iab_mt_r16_s_>();
}
static const asn1::table::seq_field
    seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_r16_s__fields[] = {
        {offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_r16_s_, nrof_dl_symbols_r16),
         (int32_t)offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_r16_s_,
                           nrof_dl_symbols_r16_present),
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(13),
         nullptr},
        {offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_r16_s_, nrof_ul_symbols_r16),
         (int32_t)offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_r16_s_,
                           nrof_ul_symbols_r16_present),
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(13),
         nullptr}};
static const asn1::table::field_name
    seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_r16_s__names[] = {
        {"nrofDownlinkSymbols-r16", nullptr},
        {"nrofUplinkSymbols-r16", nullptr}};
static const asn1::table::seq_opt
    seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_r16_s__opts[] = {
        {0,
         (int32_t)offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_r16_s_,
                           nrof_dl_symbols_r16_present)},
        {1,
         (int32_t)offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_r16_s_,
                           nrof_ul_symbols_r16_present)}};
static const asn1::table::seq_desc seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_r16_s_ = {
    {seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_r16_s__fields,
     2,
     UINT64_C(0x0),
     seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_r16_s__opts,
     2},
    false,
    false,
    nullptr,
    0,
    seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_r16_s__names};
static const asn1::table::seq_field
    seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_iab_mt_r16_s__fields[] = {
        {offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_iab_mt_r16_s_, nrof_dl_symbols_r16),
         (int32_t)offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_iab_mt_r16_s_,
                           nrof_dl_symbols_r16_present),
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(13),
         nullptr},
        {offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_iab_mt_r16_s_, nrof_ul_symbols_r16),
         (int32_t)offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_iab_mt_r16_s_,
                           nrof_ul_symbols_r16_present),
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(1),
         UINT64_C(13),
         nullptr}};
static const asn1::table::field_name
    seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_iab_mt_r16_s__names[] = {
        {"nrofDownlinkSymbols-r16", nullptr},
        {"nrofUplinkSymbols-r16", nullptr}};
static const asn1::table::seq_opt
    seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_iab_mt_r16_s__opts[] = {
        {0,
         (int32_t)offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_iab_mt_r16_s_,
                           nrof_dl_symbols_r16_present)},
        {1,
         (int32_t)offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::explicit_iab_mt_r16_s_,
                           nrof_ul_symbols_r16_present)}};
static const asn1::table::seq_desc
    seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_iab_mt_r16_s_ = {
        {seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_iab_mt_r16_s__fields,
         2,
         UINT64_C(0x0),
         seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_iab_mt_r16_s__opts,
         2},
        false,
        false,
        nullptr,
        0,
        seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_iab_mt_r16_s__names};
static const asn1::table::seq_field choice_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__alternatives[] =
    {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
     {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_r16_s_},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &seq_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__explicit_iab_mt_r16_s_}};
static const asn1::table::field_name choice_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__names[] = {
    {"allDownlink-r16", nullptr},
    {"allUplink-r16", nullptr},
    {"explicit-r16", nullptr},
    {"explicit-IAB-MT-r16", nullptr}};
const asn1::table::choice_desc tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::desc = {
    asn1::table::enum_int_type<tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::types>(),
    tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::types::nof_types,
    tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::types::nof_exts,
    tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::types::has_ext,
    false,
    choice_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__alternatives,
    nullptr,
    choice_table_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__names,
    "tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_",
    offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_, c.type),
    offsetof(tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_, c.value),
    false,
    &tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::storage_desc};

static const char* enum_names_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__types_values[] =
    {"allDownlink-r16", "allUplink-r16", "explicit-r16", "explicit-IAB-MT-r16"};
const asn1::table::enum_desc tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::types_opts::desc = {
    enum_names_tdd_ul_dl_slot_cfg_iab_mt_r16_s_symbols_iab_mt_r16_c__types_values,
    4,
    "tdd_ul_dl_slot_cfg_iab_mt_r16_s::symbols_iab_mt_r16_c_::types"};

// TDD-UL-DL-ConfigDedicated-IAB-MT-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_tdd_ul_dl_cfg_ded_iab_mt_r16_s_fields[] = {
    {offsetof(tdd_ul_dl_cfg_ded_iab_mt_r16_s, slot_specific_cfgs_to_add_mod_list_iab_mt_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     320,
     &asn1::table::item_list_codec<
         decltype(tdd_ul_dl_cfg_ded_iab_mt_r16_s::slot_specific_cfgs_to_add_mod_list_iab_mt_r16),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &tdd_ul_dl_slot_cfg_iab_mt_r16_s::desc},
    {offsetof(tdd_ul_dl_cfg_ded_iab_mt_r16_s, slot_specific_cfgs_to_release_list_iab_mt_r16),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     320,
     &asn1::table::list_codec<decltype(tdd_ul_dl_cfg_ded_iab_mt_r16_s::slot_specific_cfgs_to_release_list_iab_mt_r16),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(319), false, false>>::value}};
static const asn1::table::field_name seq_table_tdd_ul_dl_cfg_ded_iab_mt_r16_s_names[] = {
    {"slotSpecificConfigurationsToAddModList-IAB-MT-r16", nullptr},
    {"slotSpecificConfigurationsToReleaseList-IAB-MT-r16", nullptr}};
static const asn1::table::seq_opt seq_table_tdd_ul_dl_cfg_ded_iab_mt_r16_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc tdd_ul_dl_cfg_ded_iab_mt_r16_s::desc = {{seq_table_tdd_ul_dl_cfg_ded_iab_mt_r16_s_fields,
                                                                     2,
                                                                     UINT64_C(0x0),
                                                                     seq_table_tdd_ul_dl_cfg_ded_iab_mt_r16_s_opts,
                                                                     2},
                                                                    true,
                                                                    false,
                                                                    nullptr,
                                                                    0,
                                                                    seq_table_tdd_ul_dl_cfg_ded_iab_mt_r16_s_names};

// WithinActiveTimeConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_within_active_time_cfg_r16_s_fields[] = {
    {offsetof(within_active_time_cfg_r16_s, first_within_active_time_bwp_id_r16),
     (int32_t)offsetof(within_active_time_cfg_r16_s, first_within_active_time_bwp_id_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr},
    {offsetof(within_active_time_cfg_r16_s, dormancy_group_within_active_time_r16),
     (int32_t)offsetof(within_active_time_cfg_r16_s, dormancy_group_within_active_time_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr}};
static const asn1::table::field_name seq_table_within_active_time_cfg_r16_s_names[] = {
    {"firstWithinActiveTimeBWP-Id-r16", nullptr},
    {"dormancyGroupWithinActiveTime-r16", nullptr}};
static const asn1::table::seq_opt seq_table_within_active_time_cfg_r16_s_opts[] = {
    {0, (int32_t)offsetof(within_active_time_cfg_r16_s, first_within_active_time_bwp_id_r16_present)},
    {1, (int32_t)offsetof(within_active_time_cfg_r16_s, dormancy_group_within_active_time_r16_present)}};
const asn1::table::seq_desc within_active_time_cfg_r16_s::desc = {
    {seq_table_within_active_time_cfg_r16_s_fields, 2, UINT64_C(0x0), seq_table_within_active_time_cfg_r16_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_within_active_time_cfg_r16_s_names};

// OutsideActiveTimeConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_outside_active_time_cfg_r16_s_fields[] = {
    {offsetof(outside_active_time_cfg_r16_s, first_outside_active_time_bwp_id_r16),
     (int32_t)offsetof(outside_active_time_cfg_r16_s, first_outside_active_time_bwp_id_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr},
    {offsetof(outside_active_time_cfg_r16_s, dormancy_group_outside_active_time_r16),
     (int32_t)offsetof(outside_active_time_cfg_r16_s, dormancy_group_outside_active_time_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr}};
static const asn1::table::field_name seq_table_outside_active_time_cfg_r16_s_names[] = {
    {"firstOutsideActiveTimeBWP-Id-r16", nullptr},
    {"dormancyGroupOutsideActiveTime-r16", nullptr}};
static const asn1::table::seq_opt seq_table_outside_active_time_cfg_r16_s_opts[] = {
    {0, (int32_t)offsetof(outside_active_time_cfg_r16_s, first_outside_active_time_bwp_id_r16_present)},
    {1, (int32_t)offsetof(outside_active_time_cfg_r16_s, dormancy_group_outside_active_time_r16_present)}};
const asn1::table::seq_desc outside_active_time_cfg_r16_s::desc = {
    {seq_table_outside_active_time_cfg_r16_s_fields, 2, UINT64_C(0x0), seq_table_outside_active_time_cfg_r16_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_outside_active_time_cfg_r16_s_names};

// DormantBWP-Config-r16 ::= SEQUENCE
static const asn1::table::seq_field
    seq_table_dormant_bwp_cfg_r16_s_within_active_time_cfg_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &within_active_time_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_dormant_bwp_cfg_r16_s_within_active_time_cfg_r16_setup_release_names[] =
    {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_dormant_bwp_cfg_r16_s_within_active_time_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_dormant_bwp_cfg_r16_s_within_active_time_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_dormant_bwp_cfg_r16_s_within_active_time_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<within_active_time_cfg_r16_s>::type_offset(),
    setup_release_c<within_active_time_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_dormant_bwp_cfg_r16_s_outside_active_time_cfg_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &outside_active_time_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_dormant_bwp_cfg_r16_s_outside_active_time_cfg_r16_setup_release_names[] =
    {{"release", nullptr}, {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_dormant_bwp_cfg_r16_s_outside_active_time_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_dormant_bwp_cfg_r16_s_outside_active_time_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_dormant_bwp_cfg_r16_s_outside_active_time_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<outside_active_time_cfg_r16_s>::type_offset(),
    setup_release_c<outside_active_time_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_dormant_bwp_cfg_r16_s_fields[] = {
    {offsetof(dormant_bwp_cfg_r16_s, dormant_bwp_id_r16),
     (int32_t)offsetof(dormant_bwp_cfg_r16_s, dormant_bwp_id_r16_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr},
    {offsetof(dormant_bwp_cfg_r16_s, within_active_time_cfg_r16),
     (int32_t)offsetof(dormant_bwp_cfg_r16_s, within_active_time_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_dormant_bwp_cfg_r16_s_within_active_time_cfg_r16_setup_release},
    {offsetof(dormant_bwp_cfg_r16_s, outside_active_time_cfg_r16),
     (int32_t)offsetof(dormant_bwp_cfg_r16_s, outside_active_time_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_dormant_bwp_cfg_r16_s_outside_active_time_cfg_r16_setup_release}};
static const asn1::table::field_name seq_table_dormant_bwp_cfg_r16_s_names[] = {
    {"dormantBWP-Id-r16", nullptr},
    {"withinActiveTimeConfig-r16", nullptr},
    {"outsideActiveTimeConfig-r16", nullptr}};
static const asn1::table::seq_opt seq_table_dormant_bwp_cfg_r16_s_opts[] = {
    {0, (int32_t)offsetof(dormant_bwp_cfg_r16_s, dormant_bwp_id_r16_present)},
    {1, (int32_t)offsetof(dormant_bwp_cfg_r16_s, within_active_time_cfg_r16_present)},
    {2, (int32_t)offsetof(dormant_bwp_cfg_r16_s, outside_active_time_cfg_r16_present)}};
const asn1::table::seq_desc dormant_bwp_cfg_r16_s::desc = {
    {seq_table_dormant_bwp_cfg_r16_s_fields, 3, UINT64_C(0x0), seq_table_dormant_bwp_cfg_r16_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_dormant_bwp_cfg_r16_s_names};

// DummyJ ::= SEQUENCE
static const asn1::table::seq_field seq_table_dummy_j_s_fields[] = {
    {offsetof(dummy_j_s, max_energy_detection_thres_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-85),
     UINT64_C(-52),
     nullptr},
    {offsetof(dummy_j_s, energy_detection_thres_offset_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-20),
     UINT64_C(-13),
     nullptr},
    {offsetof(dummy_j_s, ul_to_dl_cot_sharing_ed_thres_r16),
     (int32_t)offsetof(dummy_j_s, ul_to_dl_cot_sharing_ed_thres_r16_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-85),
     UINT64_C(-52),
     nullptr},
    {0,
     (int32_t)offsetof(dummy_j_s, absence_of_any_other_technology_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_dummy_j_s_names[] = {{"maxEnergyDetectionThreshold-r16", nullptr},
                                                                    {"energyDetectionThresholdOffset-r16", nullptr},
                                                                    {"ul-toDL-COT-SharingED-Threshold-r16", nullptr},
                                                                    {"absenceOfAnyOtherTechnology-r16", "true"}};
static const asn1::table::seq_opt    seq_table_dummy_j_s_opts[]  = {
    {2, (int32_t)offsetof(dummy_j_s, ul_to_dl_cot_sharing_ed_thres_r16_present)},
    {3, (int32_t)offsetof(dummy_j_s, absence_of_any_other_technology_r16_present)}};
const asn1::table::seq_desc dummy_j_s::desc = {
    {seq_table_dummy_j_s_fields, 4, UINT64_C(0x3), seq_table_dummy_j_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_dummy_j_s_names};

// GuardBand-r16 ::= SEQUENCE
static const asn1::table::seq_field  seq_table_guard_band_r16_s_fields[] = {{offsetof(guard_band_r16_s, start_crb_r16),
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::int_u16,
                                                                             false,
                                                                             false,
                                                                             INT64_C(0),
                                                                             UINT64_C(274),
                                                                             nullptr},
                                                                            {offsetof(guard_band_r16_s, nrof_c_rbs_r16),
                                                                             asn1::table::seq_field::mandatory,
                                                                             asn1::table::field_op::int_u8,
                                                                             false,
                                                                             false,
                                                                             INT64_C(0),
                                                                             UINT64_C(15),
                                                                             nullptr}};
static const asn1::table::field_name seq_table_guard_band_r16_s_names[]  = {{"startCRB-r16", nullptr},
                                                                            {"nrofCRBs-r16", nullptr}};
const asn1::table::seq_desc guard_band_r16_s::desc = {{seq_table_guard_band_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
                                                      false,
                                                      false,
                                                      nullptr,
                                                      0,
                                                      seq_table_guard_band_r16_s_names};

// IntraCellGuardBandsPerSCS-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_intra_cell_guard_bands_per_scs_r16_s_fields[] = {
    {offsetof(intra_cell_guard_bands_per_scs_r16_s, guard_band_scs_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(intra_cell_guard_bands_per_scs_r16_s::guard_band_scs_r16)>(),
     decltype(intra_cell_guard_bands_per_scs_r16_s::guard_band_scs_r16)::has_ext,
     false,
     decltype(intra_cell_guard_bands_per_scs_r16_s::guard_band_scs_r16)::nof_types,
     decltype(intra_cell_guard_bands_per_scs_r16_s::guard_band_scs_r16)::nof_exts,
     nullptr,
     &subcarrier_spacing_opts::desc},
    {offsetof(intra_cell_guard_bands_per_scs_r16_s, intra_cell_guard_bands_r16),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(intra_cell_guard_bands_per_scs_r16_s::intra_cell_guard_bands_r16),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &guard_band_r16_s::desc}};
static const asn1::table::field_name seq_table_intra_cell_guard_bands_per_scs_r16_s_names[] = {
    {"guardBandSCS-r16", nullptr},
    {"intraCellGuardBands-r16", nullptr}};
const asn1::table::seq_desc intra_cell_guard_bands_per_scs_r16_s::desc = {
    {seq_table_intra_cell_guard_bands_per_scs_r16_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_intra_cell_guard_bands_per_scs_r16_s_names};

// ChannelAccessConfig-r16 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ch_access_cfg_r16_s_fields[] = {
    {offsetof(ch_access_cfg_r16_s, energy_detection_cfg_r16),
     (int32_t)offsetof(ch_access_cfg_r16_s, energy_detection_cfg_r16_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::desc},
    {offsetof(ch_access_cfg_r16_s, ul_to_dl_cot_sharing_ed_thres_r16),
     (int32_t)offsetof(ch_access_cfg_r16_s, ul_to_dl_cot_sharing_ed_thres_r16_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-85),
     UINT64_C(-52),
     nullptr},
    {0,
     (int32_t)offsetof(ch_access_cfg_r16_s, absence_of_any_other_technology_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_ch_access_cfg_r16_s_names[] = {
    {"energyDetectionConfig-r16", nullptr},
    {"ul-toDL-COT-SharingED-Threshold-r16", nullptr},
    {"absenceOfAnyOtherTechnology-r16", "true"}};
static const asn1::table::seq_opt seq_table_ch_access_cfg_r16_s_opts[] = {
    {0, (int32_t)offsetof(ch_access_cfg_r16_s, energy_detection_cfg_r16_present)},
    {1, (int32_t)offsetof(ch_access_cfg_r16_s, ul_to_dl_cot_sharing_ed_thres_r16_present)},
    {2, (int32_t)offsetof(ch_access_cfg_r16_s, absence_of_any_other_technology_r16_present)}};
const asn1::table::seq_desc ch_access_cfg_r16_s::desc = {
    {seq_table_ch_access_cfg_r16_s_fields, 3, UINT64_C(0x0), seq_table_ch_access_cfg_r16_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_ch_access_cfg_r16_s_names};

static const asn1::table::storage_ops* const
    choice_storage_ch_access_cfg_r16_s_energy_detection_cfg_r16_c__alternatives[] = {
        &asn1::table::alternative_ops<int8_t, true>::value,
        &asn1::table::alternative_ops<int8_t, true>::value};
const asn1::table::choice_storage_desc ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::storage_desc = {
    choice_storage_ch_access_cfg_r16_s_energy_detection_cfg_r16_c__alternatives,
    2,
    "ch_access_cfg_r16_s::energy_detection_cfg_r16_c_"};

int8_t& ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::set_max_energy_detection_thres_r16()
{
  set(types::max_energy_detection_thres_r16);
  return c.get<int8_t>();
}
int8_t& ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::set_energy_detection_thres_offset_r16()
{
  set(types::energy_detection_thres_offset_r16);
  return c.get<int8_t>();
}
static const asn1::table::seq_field choice_table_ch_access_cfg_r16_s_energy_detection_cfg_r16_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-85),
     UINT64_C(-52),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-13),
     UINT64_C(20),
     nullptr}};
static const asn1::table::field_name choice_table_ch_access_cfg_r16_s_energy_detection_cfg_r16_c__names[] = {
    {"maxEnergyDetectionThreshold-r16", nullptr},
    {"energyDetectionThresholdOffset-r16", nullptr}};
const asn1::table::choice_desc ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::desc = {
    asn1::table::enum_int_type<ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::types>(),
    ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::types::nof_types,
    ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::types::nof_exts,
    ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::types::has_ext,
    false,
    choice_table_ch_access_cfg_r16_s_energy_detection_cfg_r16_c__alternatives,
    nullptr,
    choice_table_ch_access_cfg_r16_s_energy_detection_cfg_r16_c__names,
    "ch_access_cfg_r16_s::energy_detection_cfg_r16_c_",
    offsetof(ch_access_cfg_r16_s::energy_detection_cfg_r16_c_, c.type),
    offsetof(ch_access_cfg_r16_s::energy_detection_cfg_r16_c_, c.value),
    false,
    &ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::storage_desc};

static const char* enum_names_ch_access_cfg_r16_s_energy_detection_cfg_r16_c__types_values[] = {
    "maxEnergyDetectionThreshold-r16",
    "energyDetectionThresholdOffset-r16"};
const asn1::table::enum_desc ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::types_opts::desc = {
    enum_names_ch_access_cfg_r16_s_energy_detection_cfg_r16_c__types_values,
    2,
    "ch_access_cfg_r16_s::energy_detection_cfg_r16_c_::types"};

// NR-DL-PRS-Periodicity-and-ResourceSetSlotOffset-r17 ::= CHOICE
static const asn1::table::storage_ops* const
    choice_storage_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_alternatives[] = {
        &asn1::table::alternative_ops<nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_, false>::value,
        &asn1::table::alternative_ops<nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_, false>::value,
        &asn1::table::alternative_ops<nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_, false>::value,
        &asn1::table::alternative_ops<nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_,
                                      false>::value};
const asn1::table::choice_storage_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::storage_desc = {
    choice_storage_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_alternatives,
    4,
    "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c"};

nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::set_scs15_r17()
{
  set(types::scs15_r17);
  return c.get<scs15_r17_c_>();
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::set_scs30_r17()
{
  set(types::scs30_r17);
  return c.get<scs30_r17_c_>();
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::set_scs60_r17()
{
  set(types::scs60_r17);
  return c.get<scs60_r17_c_>();
}
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_&
nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::set_scs120_r17()
{
  set(types::scs120_r17);
  return c.get<scs120_r17_c_>();
}
static const asn1::table::seq_field choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::desc}};
static const asn1::table::field_name choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_names[] =
    {{"scs15-r17", nullptr}, {"scs30-r17", nullptr}, {"scs60-r17", nullptr}, {"scs120-r17", nullptr}};
const asn1::table::choice_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::desc = {
    asn1::table::enum_int_type<nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types>(),
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types::nof_types,
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types::nof_exts,
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types::has_ext,
    false,
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_alternatives,
    nullptr,
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_names,
    "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c",
    offsetof(nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c, c.type),
    offsetof(nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c, c.value),
    false,
    &nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::storage_desc};

static const asn1::table::storage_ops* const
    choice_storage_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs15_r17_c__alternatives[] = {
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
        &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::storage_desc =
    {choice_storage_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs15_r17_c__alternatives,
     17,
     "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_"};

uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n4_r17()
{
  set(types::n4_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n5_r17()
{
  set(types::n5_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n8_r17()
{
  set(types::n8_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n10_r17()
{
  set(types::n10_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n16_r17()
{
  set(types::n16_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n20_r17()
{
  set(types::n20_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n32_r17()
{
  set(types::n32_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n40_r17()
{
  set(types::n40_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n64_r17()
{
  set(types::n64_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n80_r17()
{
  set(types::n80_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n160_r17()
{
  set(types::n160_r17);
  return c.get<uint8_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n320_r17()
{
  set(types::n320_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n640_r17()
{
  set(types::n640_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n1280_r17()
{
  set(types::n1280_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n2560_r17()
{
  set(types::n2560_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n5120_r17()
{
  set(types::n5120_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::set_n10240_r17()
{
  set(types::n10240_r17);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs15_r17_c__alternatives[] = {
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
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(5119),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(10239),
         nullptr}};
static const asn1::table::field_name
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs15_r17_c__names[] = {{"n4-r17", nullptr},
                                                                                             {"n5-r17", nullptr},
                                                                                             {"n8-r17", nullptr},
                                                                                             {"n10-r17", nullptr},
                                                                                             {"n16-r17", nullptr},
                                                                                             {"n20-r17", nullptr},
                                                                                             {"n32-r17", nullptr},
                                                                                             {"n40-r17", nullptr},
                                                                                             {"n64-r17", nullptr},
                                                                                             {"n80-r17", nullptr},
                                                                                             {"n160-r17", nullptr},
                                                                                             {"n320-r17", nullptr},
                                                                                             {"n640-r17", nullptr},
                                                                                             {"n1280-r17", nullptr},
                                                                                             {"n2560-r17", nullptr},
                                                                                             {"n5120-r17", nullptr},
                                                                                             {"n10240-r17", nullptr}};
const asn1::table::choice_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::desc = {
    asn1::table::enum_int_type<nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types>(),
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types::nof_types,
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types::nof_exts,
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types::has_ext,
    false,
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs15_r17_c__alternatives,
    nullptr,
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs15_r17_c__names,
    "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_",
    offsetof(nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_, c.type),
    offsetof(nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_, c.value),
    false,
    &nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::storage_desc};

static const char* enum_names_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs15_r17_c__types_values[] = {
    "n4-r17",
    "n5-r17",
    "n8-r17",
    "n10-r17",
    "n16-r17",
    "n20-r17",
    "n32-r17",
    "n40-r17",
    "n64-r17",
    "n80-r17",
    "n160-r17",
    "n320-r17",
    "n640-r17",
    "n1280-r17",
    "n2560-r17",
    "n5120-r17",
    "n10240-r17"};
const asn1::table::enum_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types_opts::desc = {
    enum_names_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs15_r17_c__types_values,
    17,
    "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types"};
uint16_t nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 160, 320, 640, 1280, 2560, 5120, 10240};
  return map_enum_number(
      numbers, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs15_r17_c_::types");
}

static const asn1::table::storage_ops* const
    choice_storage_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs30_r17_c__alternatives[] = {
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
        &asn1::table::alternative_ops<uint16_t, true>::value};
const asn1::table::choice_storage_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::storage_desc =
    {choice_storage_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs30_r17_c__alternatives,
     17,
     "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_"};

uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n8_r17()
{
  set(types::n8_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n10_r17()
{
  set(types::n10_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n16_r17()
{
  set(types::n16_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n20_r17()
{
  set(types::n20_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n32_r17()
{
  set(types::n32_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n40_r17()
{
  set(types::n40_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n64_r17()
{
  set(types::n64_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n80_r17()
{
  set(types::n80_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n128_r17()
{
  set(types::n128_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n160_r17()
{
  set(types::n160_r17);
  return c.get<uint8_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n320_r17()
{
  set(types::n320_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n640_r17()
{
  set(types::n640_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n1280_r17()
{
  set(types::n1280_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n2560_r17()
{
  set(types::n2560_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n5120_r17()
{
  set(types::n5120_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n10240_r17()
{
  set(types::n10240_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::set_n20480_r17()
{
  set(types::n20480_r17);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs30_r17_c__alternatives[] = {
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
         UINT64_C(127),
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
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(5119),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(10239),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(20479),
         nullptr}};
static const asn1::table::field_name
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs30_r17_c__names[] = {{"n8-r17", nullptr},
                                                                                             {"n10-r17", nullptr},
                                                                                             {"n16-r17", nullptr},
                                                                                             {"n20-r17", nullptr},
                                                                                             {"n32-r17", nullptr},
                                                                                             {"n40-r17", nullptr},
                                                                                             {"n64-r17", nullptr},
                                                                                             {"n80-r17", nullptr},
                                                                                             {"n128-r17", nullptr},
                                                                                             {"n160-r17", nullptr},
                                                                                             {"n320-r17", nullptr},
                                                                                             {"n640-r17", nullptr},
                                                                                             {"n1280-r17", nullptr},
                                                                                             {"n2560-r17", nullptr},
                                                                                             {"n5120-r17", nullptr},
                                                                                             {"n10240-r17", nullptr},
                                                                                             {"n20480-r17", nullptr}};
const asn1::table::choice_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::desc = {
    asn1::table::enum_int_type<nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types>(),
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types::nof_types,
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types::nof_exts,
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types::has_ext,
    false,
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs30_r17_c__alternatives,
    nullptr,
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs30_r17_c__names,
    "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_",
    offsetof(nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_, c.type),
    offsetof(nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_, c.value),
    false,
    &nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::storage_desc};

static const char* enum_names_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs30_r17_c__types_values[] = {
    "n8-r17",
    "n10-r17",
    "n16-r17",
    "n20-r17",
    "n32-r17",
    "n40-r17",
    "n64-r17",
    "n80-r17",
    "n128-r17",
    "n160-r17",
    "n320-r17",
    "n640-r17",
    "n1280-r17",
    "n2560-r17",
    "n5120-r17",
    "n10240-r17",
    "n20480-r17"};
const asn1::table::enum_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types_opts::desc = {
    enum_names_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs30_r17_c__types_values,
    17,
    "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types"};
uint16_t nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {8, 10, 16, 20, 32, 40, 64, 80, 128, 160, 320, 640, 1280, 2560, 5120, 10240, 20480};
  return map_enum_number(
      numbers, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs30_r17_c_::types");
}

static const asn1::table::storage_ops* const
    choice_storage_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs60_r17_c__alternatives[] = {
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
const asn1::table::choice_storage_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::storage_desc =
    {choice_storage_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs60_r17_c__alternatives,
     17,
     "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_"};

uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n16_r17()
{
  set(types::n16_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n20_r17()
{
  set(types::n20_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n32_r17()
{
  set(types::n32_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n40_r17()
{
  set(types::n40_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n64_r17()
{
  set(types::n64_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n80_r17()
{
  set(types::n80_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n128_r17()
{
  set(types::n128_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n160_r17()
{
  set(types::n160_r17);
  return c.get<uint8_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n256_r17()
{
  set(types::n256_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n320_r17()
{
  set(types::n320_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n640_r17()
{
  set(types::n640_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n1280_r17()
{
  set(types::n1280_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n2560_r17()
{
  set(types::n2560_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n5120_r17()
{
  set(types::n5120_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n10240_r17()
{
  set(types::n10240_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n20480_r17()
{
  set(types::n20480_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::set_n40960_r17()
{
  set(types::n40960_r17);
  return c.get<uint16_t>();
}
static const asn1::table::seq_field
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs60_r17_c__alternatives[] = {
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
         UINT64_C(127),
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
         UINT64_C(255),
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
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(5119),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(10239),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(20479),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(40959),
         nullptr}};
static const asn1::table::field_name
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs60_r17_c__names[] = {{"n16-r17", nullptr},
                                                                                             {"n20-r17", nullptr},
                                                                                             {"n32-r17", nullptr},
                                                                                             {"n40-r17", nullptr},
                                                                                             {"n64-r17", nullptr},
                                                                                             {"n80-r17", nullptr},
                                                                                             {"n128-r17", nullptr},
                                                                                             {"n160-r17", nullptr},
                                                                                             {"n256-r17", nullptr},
                                                                                             {"n320-r17", nullptr},
                                                                                             {"n640-r17", nullptr},
                                                                                             {"n1280-r17", nullptr},
                                                                                             {"n2560-r17", nullptr},
                                                                                             {"n5120-r17", nullptr},
                                                                                             {"n10240-r17", nullptr},
                                                                                             {"n20480-r17", nullptr},
                                                                                             {"n40960-r17", nullptr}};
const asn1::table::choice_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::desc = {
    asn1::table::enum_int_type<nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types>(),
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types::nof_types,
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types::nof_exts,
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types::has_ext,
    false,
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs60_r17_c__alternatives,
    nullptr,
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs60_r17_c__names,
    "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_",
    offsetof(nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_, c.type),
    offsetof(nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_, c.value),
    false,
    &nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::storage_desc};

static const char* enum_names_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs60_r17_c__types_values[] = {
    "n16-r17",
    "n20-r17",
    "n32-r17",
    "n40-r17",
    "n64-r17",
    "n80-r17",
    "n128-r17",
    "n160-r17",
    "n256-r17",
    "n320-r17",
    "n640-r17",
    "n1280-r17",
    "n2560-r17",
    "n5120-r17",
    "n10240-r17",
    "n20480-r17",
    "n40960-r17"};
const asn1::table::enum_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types_opts::desc = {
    enum_names_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs60_r17_c__types_values,
    17,
    "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types"};
uint16_t nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types_opts::to_number() const
{
  static const uint16_t numbers[] = {
      16, 20, 32, 40, 64, 80, 128, 160, 256, 320, 640, 1280, 2560, 5120, 10240, 20480, 40960};
  return map_enum_number(
      numbers, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs60_r17_c_::types");
}

static const asn1::table::storage_ops* const
    choice_storage_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs120_r17_c__alternatives[] = {
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
        &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint16_t, true>::value,
        &asn1::table::alternative_ops<uint32_t, true>::value};
const asn1::table::choice_storage_desc
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::storage_desc = {
        choice_storage_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs120_r17_c__alternatives,
        17,
        "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_"};

uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n32_r17()
{
  set(types::n32_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n40_r17()
{
  set(types::n40_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n64_r17()
{
  set(types::n64_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n80_r17()
{
  set(types::n80_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n128_r17()
{
  set(types::n128_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n160_r17()
{
  set(types::n160_r17);
  return c.get<uint8_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n256_r17()
{
  set(types::n256_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n320_r17()
{
  set(types::n320_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n512_r17()
{
  set(types::n512_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n640_r17()
{
  set(types::n640_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n1280_r17()
{
  set(types::n1280_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n2560_r17()
{
  set(types::n2560_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n5120_r17()
{
  set(types::n5120_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n10240_r17()
{
  set(types::n10240_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n20480_r17()
{
  set(types::n20480_r17);
  return c.get<uint16_t>();
}
uint16_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n40960_r17()
{
  set(types::n40960_r17);
  return c.get<uint16_t>();
}
uint32_t& nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::set_n81920_r17()
{
  set(types::n81920_r17);
  return c.get<uint32_t>();
}
static const asn1::table::seq_field
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs120_r17_c__alternatives[] = {
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
         UINT64_C(127),
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
         UINT64_C(255),
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
         UINT64_C(511),
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
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(5119),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(10239),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(20479),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u16,
         false,
         false,
         INT64_C(0),
         UINT64_C(40959),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u32,
         false,
         false,
         INT64_C(0),
         UINT64_C(81919),
         nullptr}};
static const asn1::table::field_name
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs120_r17_c__names[] = {{"n32-r17", nullptr},
                                                                                              {"n40-r17", nullptr},
                                                                                              {"n64-r17", nullptr},
                                                                                              {"n80-r17", nullptr},
                                                                                              {"n128-r17", nullptr},
                                                                                              {"n160-r17", nullptr},
                                                                                              {"n256-r17", nullptr},
                                                                                              {"n320-r17", nullptr},
                                                                                              {"n512-r17", nullptr},
                                                                                              {"n640-r17", nullptr},
                                                                                              {"n1280-r17", nullptr},
                                                                                              {"n2560-r17", nullptr},
                                                                                              {"n5120-r17", nullptr},
                                                                                              {"n10240-r17", nullptr},
                                                                                              {"n20480-r17", nullptr},
                                                                                              {"n40960-r17", nullptr},
                                                                                              {"n81920-r17", nullptr}};
const asn1::table::choice_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::desc = {
    asn1::table::enum_int_type<nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types>(),
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types::nof_types,
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types::nof_exts,
    nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types::has_ext,
    false,
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs120_r17_c__alternatives,
    nullptr,
    choice_table_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs120_r17_c__names,
    "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_",
    offsetof(nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_, c.type),
    offsetof(nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_, c.value),
    false,
    &nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::storage_desc};

static const char* enum_names_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs120_r17_c__types_values[] = {
    "n32-r17",
    "n40-r17",
    "n64-r17",
    "n80-r17",
    "n128-r17",
    "n160-r17",
    "n256-r17",
    "n320-r17",
    "n512-r17",
    "n640-r17",
    "n1280-r17",
    "n2560-r17",
    "n5120-r17",
    "n10240-r17",
    "n20480-r17",
    "n40960-r17",
    "n81920-r17"};
const asn1::table::enum_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types_opts::desc = {
    enum_names_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_scs120_r17_c__types_values,
    17,
    "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types"};
uint32_t nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types_opts::to_number() const
{
  static const uint32_t numbers[] = {
      32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1280, 2560, 5120, 10240, 20480, 40960, 81920};
  return map_enum_number(
      numbers, 17, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::scs120_r17_c_::types");
}

static const char* enum_names_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_types_values[]   = {"scs15-r17",
                                                                                                      "scs30-r17",
                                                                                                      "scs60-r17",
                                                                                                      "scs120-r17"};
const asn1::table::enum_desc nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types_opts::desc = {
    enum_names_nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c_types_values,
    4,
    "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types"};
uint8_t nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30, 60, 120};
  return map_enum_number(numbers, 4, value, "nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::types");
}

// DL-PRS-QCL-Info-r17 ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_dl_prs_qcl_info_r17_c_alternatives[] = {
    &asn1::table::alternative_ops<dl_prs_qcl_info_r17_c::ssb_r17_s_, false>::value,
    &asn1::table::alternative_ops<dl_prs_qcl_info_r17_c::dl_prs_r17_s_, false>::value};
const asn1::table::choice_storage_desc dl_prs_qcl_info_r17_c::storage_desc = {
    choice_storage_dl_prs_qcl_info_r17_c_alternatives,
    2,
    "dl_prs_qcl_info_r17_c"};

dl_prs_qcl_info_r17_c::ssb_r17_s_& dl_prs_qcl_info_r17_c::set_ssb_r17()
{
  set(types::ssb_r17);
  return c.get<ssb_r17_s_>();
}
dl_prs_qcl_info_r17_c::dl_prs_r17_s_& dl_prs_qcl_info_r17_c::set_dl_prs_r17()
{
  set(types::dl_prs_r17);
  return c.get<dl_prs_r17_s_>();
}
static const asn1::table::seq_field seq_table_dl_prs_qcl_info_r17_c_ssb_r17_s__fields[] = {
    {offsetof(dl_prs_qcl_info_r17_c::ssb_r17_s_, ssb_idx_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(dl_prs_qcl_info_r17_c::ssb_r17_s_, rs_type_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(dl_prs_qcl_info_r17_c::ssb_r17_s_::rs_type_r17)>(),
     decltype(dl_prs_qcl_info_r17_c::ssb_r17_s_::rs_type_r17)::has_ext,
     false,
     decltype(dl_prs_qcl_info_r17_c::ssb_r17_s_::rs_type_r17)::nof_types,
     decltype(dl_prs_qcl_info_r17_c::ssb_r17_s_::rs_type_r17)::nof_exts,
     nullptr,
     &dl_prs_qcl_info_r17_c::ssb_r17_s_::rs_type_r17_opts::desc}};
static const asn1::table::field_name seq_table_dl_prs_qcl_info_r17_c_ssb_r17_s__names[] = {{"ssb-Index-r17", nullptr},
                                                                                           {"rs-Type-r17", nullptr}};
static const asn1::table::seq_desc   seq_table_dl_prs_qcl_info_r17_c_ssb_r17_s_         = {
    {seq_table_dl_prs_qcl_info_r17_c_ssb_r17_s__fields, 2, UINT64_C(0x3), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_dl_prs_qcl_info_r17_c_ssb_r17_s__names};
static const asn1::table::seq_field seq_table_dl_prs_qcl_info_r17_c_dl_prs_r17_s__fields[] = {
    {offsetof(dl_prs_qcl_info_r17_c::dl_prs_r17_s_, qcl_dl_prs_res_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr}};
static const asn1::table::field_name seq_table_dl_prs_qcl_info_r17_c_dl_prs_r17_s__names[] = {
    {"qcl-DL-PRS-ResourceID-r17", nullptr}};
static const asn1::table::seq_desc seq_table_dl_prs_qcl_info_r17_c_dl_prs_r17_s_ = {
    {seq_table_dl_prs_qcl_info_r17_c_dl_prs_r17_s__fields, 1, UINT64_C(0x1), nullptr, 0},
    true,
    false,
    nullptr,
    0,
    seq_table_dl_prs_qcl_info_r17_c_dl_prs_r17_s__names};
static const asn1::table::seq_field choice_table_dl_prs_qcl_info_r17_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_dl_prs_qcl_info_r17_c_ssb_r17_s_},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_dl_prs_qcl_info_r17_c_dl_prs_r17_s_}};
static const asn1::table::field_name choice_table_dl_prs_qcl_info_r17_c_names[] = {{"ssb-r17", nullptr},
                                                                                   {"dl-PRS-r17", nullptr}};
const asn1::table::choice_desc       dl_prs_qcl_info_r17_c::desc                = {
    asn1::table::enum_int_type<dl_prs_qcl_info_r17_c::types>(),
    dl_prs_qcl_info_r17_c::types::nof_types,
    dl_prs_qcl_info_r17_c::types::nof_exts,
    dl_prs_qcl_info_r17_c::types::has_ext,
    false,
    choice_table_dl_prs_qcl_info_r17_c_alternatives,
    nullptr,
    choice_table_dl_prs_qcl_info_r17_c_names,
    "dl_prs_qcl_info_r17_c",
    offsetof(dl_prs_qcl_info_r17_c, c.type),
    offsetof(dl_prs_qcl_info_r17_c, c.value),
    false,
    &dl_prs_qcl_info_r17_c::storage_desc};

static const char*           enum_names_dl_prs_qcl_info_r17_c_ssb_r17_s__rs_type_r17_e__values[] = {"typeC",
                                                                                                    "typeD",
                                                                                                    "typeC-plus-typeD"};
const asn1::table::enum_desc dl_prs_qcl_info_r17_c::ssb_r17_s_::rs_type_r17_opts::desc           = {
    enum_names_dl_prs_qcl_info_r17_c_ssb_r17_s__rs_type_r17_e__values,
    3,
    "dl_prs_qcl_info_r17_c::ssb_r17_s_::rs_type_r17_e_"};

static const char*           enum_names_dl_prs_qcl_info_r17_c_types_values[] = {"ssb-r17", "dl-PRS-r17"};
const asn1::table::enum_desc dl_prs_qcl_info_r17_c::types_opts::desc = {enum_names_dl_prs_qcl_info_r17_c_types_values,
                                                                        2,
                                                                        "dl_prs_qcl_info_r17_c::types"};

// NR-DL-PRS-Resource-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_dl_prs_res_r17_s_fields[] = {
    {offsetof(nr_dl_prs_res_r17_s, nr_dl_prs_res_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(63),
     nullptr},
    {offsetof(nr_dl_prs_res_r17_s, dl_prs_seq_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(4095),
     nullptr},
    {offsetof(nr_dl_prs_res_r17_s, dl_prs_comb_size_n_and_re_offset_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::desc},
    {offsetof(nr_dl_prs_res_r17_s, dl_prs_res_slot_offset_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(511),
     nullptr},
    {offsetof(nr_dl_prs_res_r17_s, dl_prs_res_symbol_offset_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(12),
     nullptr},
    {offsetof(nr_dl_prs_res_r17_s, dl_prs_qcl_info_r17),
     (int32_t)offsetof(nr_dl_prs_res_r17_s, dl_prs_qcl_info_r17_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &dl_prs_qcl_info_r17_c::desc}};
static const asn1::table::field_name seq_table_nr_dl_prs_res_r17_s_names[] = {
    {"nr-DL-PRS-ResourceID-r17", nullptr},
    {"dl-PRS-SequenceID-r17", nullptr},
    {"dl-PRS-CombSizeN-AndReOffset-r17", nullptr},
    {"dl-PRS-ResourceSlotOffset-r17", nullptr},
    {"dl-PRS-ResourceSymbolOffset-r17", nullptr},
    {"dl-PRS-QCL-Info-r17", nullptr}};
static const asn1::table::seq_opt seq_table_nr_dl_prs_res_r17_s_opts[] = {
    {5, (int32_t)offsetof(nr_dl_prs_res_r17_s, dl_prs_qcl_info_r17_present)}};
static const asn1::table::seq_field seq_table_nr_dl_prs_res_r17_s_g0_fields[] = {
    {offsetof(nr_dl_prs_res_r17_s, dl_prs_res_symbol_offset_v1800),
     (int32_t)offsetof(nr_dl_prs_res_r17_s, dl_prs_res_symbol_offset_v1800_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(13),
     UINT64_C(13),
     nullptr}};
static const asn1::table::field_name seq_table_nr_dl_prs_res_r17_s_g0_names[] = {
    {"dl-PRS-ResourceSymbolOffset-v1800", nullptr}};
static const asn1::table::seq_opt seq_table_nr_dl_prs_res_r17_s_g0_opts[] = {
    {0, (int32_t)offsetof(nr_dl_prs_res_r17_s, dl_prs_res_symbol_offset_v1800_present)}};
static const asn1::table::seq_group seq_table_nr_dl_prs_res_r17_s_groups[] = {
    {{seq_table_nr_dl_prs_res_r17_s_g0_fields, 1, UINT64_C(0x0), seq_table_nr_dl_prs_res_r17_s_g0_opts, 1},
     false,
     seq_table_nr_dl_prs_res_r17_s_g0_names}};
const asn1::table::seq_desc nr_dl_prs_res_r17_s::desc = {
    {seq_table_nr_dl_prs_res_r17_s_fields, 6, UINT64_C(0x1f), seq_table_nr_dl_prs_res_r17_s_opts, 1},
    true,
    false,
    seq_table_nr_dl_prs_res_r17_s_groups,
    1,
    seq_table_nr_dl_prs_res_r17_s_names};

static const asn1::table::storage_ops* const
    choice_storage_nr_dl_prs_res_r17_s_dl_prs_comb_size_n_and_re_offset_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value,
        &asn1::table::alternative_ops<uint8_t, true>::value};
const asn1::table::choice_storage_desc nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::storage_desc = {
    choice_storage_nr_dl_prs_res_r17_s_dl_prs_comb_size_n_and_re_offset_r17_c__alternatives,
    4,
    "nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_"};

uint8_t& nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::set_n2_r17()
{
  set(types::n2_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::set_n4_r17()
{
  set(types::n4_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::set_n6_r17()
{
  set(types::n6_r17);
  return c.get<uint8_t>();
}
uint8_t& nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::set_n12_r17()
{
  set(types::n12_r17);
  return c.get<uint8_t>();
}
static const asn1::table::seq_field
    choice_table_nr_dl_prs_res_r17_s_dl_prs_comb_size_n_and_re_offset_r17_c__alternatives[] = {
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
         UINT64_C(5),
         nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::int_u8,
         false,
         false,
         INT64_C(0),
         UINT64_C(11),
         nullptr}};
static const asn1::table::field_name choice_table_nr_dl_prs_res_r17_s_dl_prs_comb_size_n_and_re_offset_r17_c__names[] =
    {{"n2-r17", nullptr}, {"n4-r17", nullptr}, {"n6-r17", nullptr}, {"n12-r17", nullptr}};
const asn1::table::choice_desc nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::desc = {
    asn1::table::enum_int_type<nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types>(),
    nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types::nof_types,
    nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types::nof_exts,
    nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types::has_ext,
    false,
    choice_table_nr_dl_prs_res_r17_s_dl_prs_comb_size_n_and_re_offset_r17_c__alternatives,
    nullptr,
    choice_table_nr_dl_prs_res_r17_s_dl_prs_comb_size_n_and_re_offset_r17_c__names,
    "nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_",
    offsetof(nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_, c.type),
    offsetof(nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_, c.value),
    false,
    &nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::storage_desc};

static const char* enum_names_nr_dl_prs_res_r17_s_dl_prs_comb_size_n_and_re_offset_r17_c__types_values[]    = {"n2-r17",
                                                                                                               "n4-r17",
                                                                                                               "n6-r17",
                                                                                                               "n12-r17"};
const asn1::table::enum_desc nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types_opts::desc = {
    enum_names_nr_dl_prs_res_r17_s_dl_prs_comb_size_n_and_re_offset_r17_c__types_values,
    4,
    "nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types"};
uint8_t nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 12};
  return map_enum_number(numbers, 4, value, "nr_dl_prs_res_r17_s::dl_prs_comb_size_n_and_re_offset_r17_c_::types");
}

// RepFactorAndTimeGap-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_rep_factor_and_time_gap_r17_s_fields[] = {
    {offsetof(rep_factor_and_time_gap_r17_s, repeat_factor_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rep_factor_and_time_gap_r17_s::repeat_factor_r17)>(),
     decltype(rep_factor_and_time_gap_r17_s::repeat_factor_r17)::has_ext,
     false,
     decltype(rep_factor_and_time_gap_r17_s::repeat_factor_r17)::nof_types,
     decltype(rep_factor_and_time_gap_r17_s::repeat_factor_r17)::nof_exts,
     nullptr,
     &rep_factor_and_time_gap_r17_s::repeat_factor_r17_opts::desc},
    {offsetof(rep_factor_and_time_gap_r17_s, time_gap_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rep_factor_and_time_gap_r17_s::time_gap_r17)>(),
     decltype(rep_factor_and_time_gap_r17_s::time_gap_r17)::has_ext,
     false,
     decltype(rep_factor_and_time_gap_r17_s::time_gap_r17)::nof_types,
     decltype(rep_factor_and_time_gap_r17_s::time_gap_r17)::nof_exts,
     nullptr,
     &rep_factor_and_time_gap_r17_s::time_gap_r17_opts::desc}};
static const asn1::table::field_name seq_table_rep_factor_and_time_gap_r17_s_names[] = {
    {"repetitionFactor-r17", nullptr},
    {"timeGap-r17", nullptr}};
const asn1::table::seq_desc rep_factor_and_time_gap_r17_s::desc = {
    {seq_table_rep_factor_and_time_gap_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rep_factor_and_time_gap_r17_s_names};

static const char* enum_names_rep_factor_and_time_gap_r17_s_repeat_factor_r17_e__values[] =
    {"n2", "n4", "n6", "n8", "n16", "n32", "spare2", "spare1"};
const asn1::table::enum_desc rep_factor_and_time_gap_r17_s::repeat_factor_r17_opts::desc = {
    enum_names_rep_factor_and_time_gap_r17_s_repeat_factor_r17_e__values,
    8,
    "rep_factor_and_time_gap_r17_s::repeat_factor_r17_e_"};
uint8_t rep_factor_and_time_gap_r17_s::repeat_factor_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 8, 16, 32};
  return map_enum_number(numbers, 6, value, "rep_factor_and_time_gap_r17_s::repeat_factor_r17_e_");
}

static const char* enum_names_rep_factor_and_time_gap_r17_s_time_gap_r17_e__values[] =
    {"s1", "s2", "s4", "s8", "s16", "s32", "spare2", "spare1"};
const asn1::table::enum_desc rep_factor_and_time_gap_r17_s::time_gap_r17_opts::desc = {
    enum_names_rep_factor_and_time_gap_r17_s_time_gap_r17_e__values,
    8,
    "rep_factor_and_time_gap_r17_s::time_gap_r17_e_"};
uint8_t rep_factor_and_time_gap_r17_s::time_gap_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4, 8, 16, 32};
  return map_enum_number(numbers, 6, value, "rep_factor_and_time_gap_r17_s::time_gap_r17_e_");
}

// NR-DL-PRS-PDC-ResourceSet-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_dl_prs_pdc_res_set_r17_s_fields[] = {
    {offsetof(nr_dl_prs_pdc_res_set_r17_s, periodicity_and_offset_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_dl_prs_periodicity_and_res_set_slot_offset_r17_c::desc},
    {offsetof(nr_dl_prs_pdc_res_set_r17_s, num_symbols_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17)>(),
     decltype(nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17)::has_ext,
     false,
     decltype(nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17)::nof_types,
     decltype(nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17)::nof_exts,
     nullptr,
     &nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17_opts::desc},
    {offsetof(nr_dl_prs_pdc_res_set_r17_s, dl_prs_res_bw_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(63),
     nullptr},
    {offsetof(nr_dl_prs_pdc_res_set_r17_s, dl_prs_start_prb_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(2176),
     nullptr},
    {offsetof(nr_dl_prs_pdc_res_set_r17_s, res_list_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(nr_dl_prs_pdc_res_set_r17_s::res_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &nr_dl_prs_res_r17_s::desc},
    {offsetof(nr_dl_prs_pdc_res_set_r17_s, rep_factor_and_time_gap_r17),
     (int32_t)offsetof(nr_dl_prs_pdc_res_set_r17_s, rep_factor_and_time_gap_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rep_factor_and_time_gap_r17_s::desc}};
static const asn1::table::field_name seq_table_nr_dl_prs_pdc_res_set_r17_s_names[] = {
    {"periodicityAndOffset-r17", nullptr},
    {"numSymbols-r17", nullptr},
    {"dl-PRS-ResourceBandwidth-r17", nullptr},
    {"dl-PRS-StartPRB-r17", nullptr},
    {"resourceList-r17", nullptr},
    {"repFactorAndTimeGap-r17", nullptr}};
static const asn1::table::seq_opt seq_table_nr_dl_prs_pdc_res_set_r17_s_opts[] = {
    {5, (int32_t)offsetof(nr_dl_prs_pdc_res_set_r17_s, rep_factor_and_time_gap_r17_present)}};
const asn1::table::seq_desc nr_dl_prs_pdc_res_set_r17_s::desc = {
    {seq_table_nr_dl_prs_pdc_res_set_r17_s_fields, 6, UINT64_C(0x1f), seq_table_nr_dl_prs_pdc_res_set_r17_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_nr_dl_prs_pdc_res_set_r17_s_names};

static const char* enum_names_nr_dl_prs_pdc_res_set_r17_s_num_symbols_r17_e__values[] =
    {"n2", "n4", "n6", "n12", "n1-v1800", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17_opts::desc = {
    enum_names_nr_dl_prs_pdc_res_set_r17_s_num_symbols_r17_e__values,
    8,
    "nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17_e_"};
uint8_t nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {2, 4, 6, 12, 1};
  return map_enum_number(numbers, 5, value, "nr_dl_prs_pdc_res_set_r17_s::num_symbols_r17_e_");
}

// NR-DL-PRS-PDC-Info-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_dl_prs_pdc_info_r17_s_fields[] = {
    {offsetof(nr_dl_prs_pdc_info_r17_s, nr_dl_prs_pdc_res_set_r17),
     (int32_t)offsetof(nr_dl_prs_pdc_info_r17_s, nr_dl_prs_pdc_res_set_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_dl_prs_pdc_res_set_r17_s::desc}};
static const asn1::table::field_name seq_table_nr_dl_prs_pdc_info_r17_s_names[] = {
    {"nr-DL-PRS-PDC-ResourceSet-r17", nullptr}};
static const asn1::table::seq_opt seq_table_nr_dl_prs_pdc_info_r17_s_opts[] = {
    {0, (int32_t)offsetof(nr_dl_prs_pdc_info_r17_s, nr_dl_prs_pdc_res_set_r17_present)}};
const asn1::table::seq_desc nr_dl_prs_pdc_info_r17_s::desc = {
    {seq_table_nr_dl_prs_pdc_info_r17_s_fields, 1, UINT64_C(0x0), seq_table_nr_dl_prs_pdc_info_r17_s_opts, 1},
    true,
    false,
    nullptr,
    0,
    seq_table_nr_dl_prs_pdc_info_r17_s_names};

// SemiStaticChannelAccessConfigUE-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_semi_static_ch_access_cfg_ue_r17_s_fields[] = {
    {offsetof(semi_static_ch_access_cfg_ue_r17_s, period_ue_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(semi_static_ch_access_cfg_ue_r17_s::period_ue_r17)>(),
     decltype(semi_static_ch_access_cfg_ue_r17_s::period_ue_r17)::has_ext,
     false,
     decltype(semi_static_ch_access_cfg_ue_r17_s::period_ue_r17)::nof_types,
     decltype(semi_static_ch_access_cfg_ue_r17_s::period_ue_r17)::nof_exts,
     nullptr,
     &semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_opts::desc},
    {offsetof(semi_static_ch_access_cfg_ue_r17_s, offset_ue_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(559),
     nullptr}};
static const asn1::table::field_name seq_table_semi_static_ch_access_cfg_ue_r17_s_names[] = {{"periodUE-r17", nullptr},
                                                                                             {"offsetUE-r17", nullptr}};
const asn1::table::seq_desc          semi_static_ch_access_cfg_ue_r17_s::desc             = {
    {seq_table_semi_static_ch_access_cfg_ue_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_semi_static_ch_access_cfg_ue_r17_s_names};

static const char* enum_names_semi_static_ch_access_cfg_ue_r17_s_period_ue_r17_e__values[] =
    {"ms1", "ms2", "ms2dot5", "ms4", "ms5", "ms10", "spare2", "spare1"};
const asn1::table::enum_desc semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_opts::desc = {
    enum_names_semi_static_ch_access_cfg_ue_r17_s_period_ue_r17_e__values,
    8,
    "semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_e_"};
float semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_opts::to_number() const
{
  static const float numbers[] = {1.0, 2.0, 2.5, 4.0, 5.0, 10.0};
  return map_enum_number(numbers, 6, value, "semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_e_");
}
const char* semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_opts::to_number_string() const
{
  static const char* number_strs[] = {"1", "2", "2.5", "4", "5", "10"};
  return convert_enum_idx(number_strs, 8, value, "semi_static_ch_access_cfg_ue_r17_s::period_ue_r17_e_");
}

// SSB-MTC-AdditionalPCI-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ssb_mtc_add_pci_r17_s_fields[] = {
    {offsetof(ssb_mtc_add_pci_r17_s, add_pci_idx_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(7),
     nullptr},
    {offsetof(ssb_mtc_add_pci_r17_s, add_pci_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(ssb_mtc_add_pci_r17_s, periodicity_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ssb_mtc_add_pci_r17_s::periodicity_r17)>(),
     decltype(ssb_mtc_add_pci_r17_s::periodicity_r17)::has_ext,
     false,
     decltype(ssb_mtc_add_pci_r17_s::periodicity_r17)::nof_types,
     decltype(ssb_mtc_add_pci_r17_s::periodicity_r17)::nof_exts,
     nullptr,
     &ssb_mtc_add_pci_r17_s::periodicity_r17_opts::desc},
    {offsetof(ssb_mtc_add_pci_r17_s, ssb_positions_in_burst_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::desc},
    {offsetof(ssb_mtc_add_pci_r17_s, ss_pbch_block_pwr_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-60),
     UINT64_C(50),
     nullptr}};
static const asn1::table::field_name seq_table_ssb_mtc_add_pci_r17_s_names[] = {{"additionalPCIIndex-r17", nullptr},
                                                                                {"additionalPCI-r17", nullptr},
                                                                                {"periodicity-r17", nullptr},
                                                                                {"ssb-PositionsInBurst-r17", nullptr},
                                                                                {"ss-PBCH-BlockPower-r17", nullptr}};
const asn1::table::seq_desc          ssb_mtc_add_pci_r17_s::desc             = {
    {seq_table_ssb_mtc_add_pci_r17_s_fields, 5, UINT64_C(0x1f), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_ssb_mtc_add_pci_r17_s_names};

static const char* enum_names_ssb_mtc_add_pci_r17_s_periodicity_r17_e__values[] =
    {"ms5", "ms10", "ms20", "ms40", "ms80", "ms160", "spare2", "spare1"};
const asn1::table::enum_desc ssb_mtc_add_pci_r17_s::periodicity_r17_opts::desc = {
    enum_names_ssb_mtc_add_pci_r17_s_periodicity_r17_e__values,
    8,
    "ssb_mtc_add_pci_r17_s::periodicity_r17_e_"};
uint8_t ssb_mtc_add_pci_r17_s::periodicity_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "ssb_mtc_add_pci_r17_s::periodicity_r17_e_");
}

static const asn1::table::storage_ops* const
    choice_storage_ssb_mtc_add_pci_r17_s_ssb_positions_in_burst_r17_c__alternatives[] = {
        &asn1::table::alternative_ops<fixed_bitstring<4>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<8>, false>::value,
        &asn1::table::alternative_ops<fixed_bitstring<64>, false>::value};
const asn1::table::choice_storage_desc ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::storage_desc = {
    choice_storage_ssb_mtc_add_pci_r17_s_ssb_positions_in_burst_r17_c__alternatives,
    3,
    "ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_"};

fixed_bitstring<4>& ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::set_short_bitmap()
{
  set(types::short_bitmap);
  return c.get<fixed_bitstring<4>>();
}
fixed_bitstring<8>& ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::set_medium_bitmap()
{
  set(types::medium_bitmap);
  return c.get<fixed_bitstring<8>>();
}
fixed_bitstring<64>& ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::set_long_bitmap()
{
  set(types::long_bitmap);
  return c.get<fixed_bitstring<64>>();
}
static const asn1::table::seq_field choice_table_ssb_mtc_add_pci_r17_s_ssb_positions_in_burst_r17_c__alternatives[] = {
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
static const asn1::table::field_name choice_table_ssb_mtc_add_pci_r17_s_ssb_positions_in_burst_r17_c__names[] = {
    {"shortBitmap", nullptr},
    {"mediumBitmap", nullptr},
    {"longBitmap", nullptr}};
const asn1::table::choice_desc ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::desc = {
    asn1::table::enum_int_type<ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::types>(),
    ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::types::nof_types,
    ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::types::nof_exts,
    ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::types::has_ext,
    false,
    choice_table_ssb_mtc_add_pci_r17_s_ssb_positions_in_burst_r17_c__alternatives,
    nullptr,
    choice_table_ssb_mtc_add_pci_r17_s_ssb_positions_in_burst_r17_c__names,
    "ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_",
    offsetof(ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_, c.type),
    offsetof(ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_, c.value),
    false,
    &ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::storage_desc};

static const char* enum_names_ssb_mtc_add_pci_r17_s_ssb_positions_in_burst_r17_c__types_values[]    = {"shortBitmap",
                                                                                                       "mediumBitmap",
                                                                                                       "longBitmap"};
const asn1::table::enum_desc ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::types_opts::desc = {
    enum_names_ssb_mtc_add_pci_r17_s_ssb_positions_in_burst_r17_c__types_values,
    3,
    "ssb_mtc_add_pci_r17_s::ssb_positions_in_burst_r17_c_::types"};

// P0AlphaSet-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_p0_alpha_set_r17_s_fields[] = {
    {offsetof(p0_alpha_set_r17_s, p0_r17),
     (int32_t)offsetof(p0_alpha_set_r17_s, p0_r17_present),
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-16),
     UINT64_C(15),
     nullptr},
    {offsetof(p0_alpha_set_r17_s, alpha_r17),
     (int32_t)offsetof(p0_alpha_set_r17_s, alpha_r17_present),
     asn1::table::enum_op<decltype(p0_alpha_set_r17_s::alpha_r17)>(),
     decltype(p0_alpha_set_r17_s::alpha_r17)::has_ext,
     false,
     decltype(p0_alpha_set_r17_s::alpha_r17)::nof_types,
     decltype(p0_alpha_set_r17_s::alpha_r17)::nof_exts,
     nullptr,
     &alpha_opts::desc},
    {offsetof(p0_alpha_set_r17_s, closed_loop_idx_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(p0_alpha_set_r17_s::closed_loop_idx_r17)>(),
     decltype(p0_alpha_set_r17_s::closed_loop_idx_r17)::has_ext,
     false,
     decltype(p0_alpha_set_r17_s::closed_loop_idx_r17)::nof_types,
     decltype(p0_alpha_set_r17_s::closed_loop_idx_r17)::nof_exts,
     nullptr,
     &p0_alpha_set_r17_s::closed_loop_idx_r17_opts::desc}};
static const asn1::table::field_name seq_table_p0_alpha_set_r17_s_names[] = {{"p0-r17", nullptr},
                                                                             {"alpha-r17", nullptr},
                                                                             {"closedLoopIndex-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_p0_alpha_set_r17_s_opts[]  = {
    {0, (int32_t)offsetof(p0_alpha_set_r17_s, p0_r17_present)},
    {1, (int32_t)offsetof(p0_alpha_set_r17_s, alpha_r17_present)}};
const asn1::table::seq_desc p0_alpha_set_r17_s::desc = {
    {seq_table_p0_alpha_set_r17_s_fields, 3, UINT64_C(0x4), seq_table_p0_alpha_set_r17_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_p0_alpha_set_r17_s_names};

static const char*           enum_names_p0_alpha_set_r17_s_closed_loop_idx_r17_e__values[] = {"i0", "i1"};
const asn1::table::enum_desc p0_alpha_set_r17_s::closed_loop_idx_r17_opts::desc            = {
    enum_names_p0_alpha_set_r17_s_closed_loop_idx_r17_e__values,
    2,
    "p0_alpha_set_r17_s::closed_loop_idx_r17_e_"};
uint8_t p0_alpha_set_r17_s::closed_loop_idx_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1};
  return map_enum_number(numbers, 2, value, "p0_alpha_set_r17_s::closed_loop_idx_r17_e_");
}

// Uplink-powerControl-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_ul_pwr_ctrl_r17_s_fields[] = {
    {offsetof(ul_pwr_ctrl_r17_s, ul_pwrctrl_id_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(64),
     nullptr},
    {offsetof(ul_pwr_ctrl_r17_s, p0_alpha_setfor_pusch_r17),
     (int32_t)offsetof(ul_pwr_ctrl_r17_s, p0_alpha_setfor_pusch_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &p0_alpha_set_r17_s::desc},
    {offsetof(ul_pwr_ctrl_r17_s, p0_alpha_setfor_pucch_r17),
     (int32_t)offsetof(ul_pwr_ctrl_r17_s, p0_alpha_setfor_pucch_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &p0_alpha_set_r17_s::desc},
    {offsetof(ul_pwr_ctrl_r17_s, p0_alpha_setfor_srs_r17),
     (int32_t)offsetof(ul_pwr_ctrl_r17_s, p0_alpha_setfor_srs_r17_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &p0_alpha_set_r17_s::desc}};
static const asn1::table::field_name seq_table_ul_pwr_ctrl_r17_s_names[] = {{"ul-powercontrolId-r17", nullptr},
                                                                            {"p0AlphaSetforPUSCH-r17", nullptr},
                                                                            {"p0AlphaSetforPUCCH-r17", nullptr},
                                                                            {"p0AlphaSetforSRS-r17", nullptr}};
static const asn1::table::seq_opt    seq_table_ul_pwr_ctrl_r17_s_opts[]  = {
    {1, (int32_t)offsetof(ul_pwr_ctrl_r17_s, p0_alpha_setfor_pusch_r17_present)},
    {2, (int32_t)offsetof(ul_pwr_ctrl_r17_s, p0_alpha_setfor_pucch_r17_present)},
    {3, (int32_t)offsetof(ul_pwr_ctrl_r17_s, p0_alpha_setfor_srs_r17_present)}};
const asn1::table::seq_desc ul_pwr_ctrl_r17_s::desc = {
    {seq_table_ul_pwr_ctrl_r17_s_fields, 4, UINT64_C(0x1), seq_table_ul_pwr_ctrl_r17_s_opts, 3},
    false,
    false,
    nullptr,
    0,
    seq_table_ul_pwr_ctrl_r17_s_names};

// MIMOParam-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mimo_param_r17_s_fields[] = {
    {offsetof(mimo_param_r17_s, add_pci_to_add_mod_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(mimo_param_r17_s::add_pci_to_add_mod_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ssb_mtc_add_pci_r17_s::desc},
    {offsetof(mimo_param_r17_s, add_pci_to_release_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(mimo_param_r17_s::add_pci_to_release_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(7), false, false>>::value},
    {offsetof(mimo_param_r17_s, unified_tci_state_type_r17),
     (int32_t)offsetof(mimo_param_r17_s, unified_tci_state_type_r17_present),
     asn1::table::enum_op<decltype(mimo_param_r17_s::unified_tci_state_type_r17)>(),
     decltype(mimo_param_r17_s::unified_tci_state_type_r17)::has_ext,
     false,
     decltype(mimo_param_r17_s::unified_tci_state_type_r17)::nof_types,
     decltype(mimo_param_r17_s::unified_tci_state_type_r17)::nof_exts,
     nullptr,
     &mimo_param_r17_s::unified_tci_state_type_r17_opts::desc},
    {offsetof(mimo_param_r17_s, ul_pwr_ctrl_to_add_mod_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::item_list_codec<decltype(mimo_param_r17_s::ul_pwr_ctrl_to_add_mod_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ul_pwr_ctrl_r17_s::desc},
    {offsetof(mimo_param_r17_s, ul_pwr_ctrl_to_release_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<decltype(mimo_param_r17_s::ul_pwr_ctrl_to_release_list_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(64), false, false>>::value},
    {offsetof(mimo_param_r17_s, sfn_schem_epdcch_r17),
     (int32_t)offsetof(mimo_param_r17_s, sfn_schem_epdcch_r17_present),
     asn1::table::enum_op<decltype(mimo_param_r17_s::sfn_schem_epdcch_r17)>(),
     decltype(mimo_param_r17_s::sfn_schem_epdcch_r17)::has_ext,
     false,
     decltype(mimo_param_r17_s::sfn_schem_epdcch_r17)::nof_types,
     decltype(mimo_param_r17_s::sfn_schem_epdcch_r17)::nof_exts,
     nullptr,
     &mimo_param_r17_s::sfn_schem_epdcch_r17_opts::desc},
    {offsetof(mimo_param_r17_s, sfn_scheme_pdsch_r17),
     (int32_t)offsetof(mimo_param_r17_s, sfn_scheme_pdsch_r17_present),
     asn1::table::enum_op<decltype(mimo_param_r17_s::sfn_scheme_pdsch_r17)>(),
     decltype(mimo_param_r17_s::sfn_scheme_pdsch_r17)::has_ext,
     false,
     decltype(mimo_param_r17_s::sfn_scheme_pdsch_r17)::nof_types,
     decltype(mimo_param_r17_s::sfn_scheme_pdsch_r17)::nof_exts,
     nullptr,
     &mimo_param_r17_s::sfn_scheme_pdsch_r17_opts::desc}};
static const asn1::table::field_name seq_table_mimo_param_r17_s_names[] = {
    {"additionalPCI-ToAddModList-r17", nullptr},
    {"additionalPCI-ToReleaseList-r17", nullptr},
    {"unifiedTCI-StateType-r17", nullptr},
    {"uplink-PowerControlToAddModList-r17", nullptr},
    {"uplink-PowerControlToReleaseList-r17", nullptr},
    {"sfnSchemePDCCH-r17", nullptr},
    {"sfnSchemePDSCH-r17", nullptr}};
static const asn1::table::seq_opt seq_table_mimo_param_r17_s_opts[] = {
    {0, asn1::table::seq_field::list_presence},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(mimo_param_r17_s, unified_tci_state_type_r17_present)},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(mimo_param_r17_s, sfn_schem_epdcch_r17_present)},
    {6, (int32_t)offsetof(mimo_param_r17_s, sfn_scheme_pdsch_r17_present)}};
const asn1::table::seq_desc mimo_param_r17_s::desc = {
    {seq_table_mimo_param_r17_s_fields, 7, UINT64_C(0x0), seq_table_mimo_param_r17_s_opts, 7},
    false,
    false,
    nullptr,
    0,
    seq_table_mimo_param_r17_s_names};

static const char*           enum_names_mimo_param_r17_s_unified_tci_state_type_r17_e__values[] = {"separate", "joint"};
const asn1::table::enum_desc mimo_param_r17_s::unified_tci_state_type_r17_opts::desc            = {
    enum_names_mimo_param_r17_s_unified_tci_state_type_r17_e__values,
    2,
    "mimo_param_r17_s::unified_tci_state_type_r17_e_"};

static const char* enum_names_mimo_param_r17_s_sfn_schem_epdcch_r17_e__values[] = {"sfnSchemeA", "sfnSchemeB"};
const asn1::table::enum_desc mimo_param_r17_s::sfn_schem_epdcch_r17_opts::desc  = {
    enum_names_mimo_param_r17_s_sfn_schem_epdcch_r17_e__values,
    2,
    "mimo_param_r17_s::sfn_schem_epdcch_r17_e_"};

static const char* enum_names_mimo_param_r17_s_sfn_scheme_pdsch_r17_e__values[] = {"sfnSchemeA", "sfnSchemeB"};
const asn1::table::enum_desc mimo_param_r17_s::sfn_scheme_pdsch_r17_opts::desc  = {
    enum_names_mimo_param_r17_s_sfn_scheme_pdsch_r17_e__values,
    2,
    "mimo_param_r17_s::sfn_scheme_pdsch_r17_e_"};

// TCI-ActivatedConfig-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_tci_activ_cfg_r17_s_fields[] = {
    {offsetof(tci_activ_cfg_r17_s, pdcch_tci_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     5,
     &asn1::table::list_codec<decltype(tci_activ_cfg_r17_s::pdcch_tci_r17),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(127), false, false>>::value},
    {offsetof(tci_activ_cfg_r17_s, pdsch_tci_r17),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tci_activ_cfg_r17_s::pdsch_tci_r17), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_tci_activ_cfg_r17_s_names[] = {{"pdcch-TCI-r17", nullptr},
                                                                              {"pdsch-TCI-r17", nullptr}};
const asn1::table::seq_desc          tci_activ_cfg_r17_s::desc             = {
    {seq_table_tci_activ_cfg_r17_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_tci_activ_cfg_r17_s_names};

// LTE-NeighCellsCRS-AssistInfo-r17 ::= SEQUENCE
static const asn1::table::seq_field seq_table_lte_neigh_cells_crs_assist_info_r17_s_fields[] = {
    {offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_carrier_bw_dl_r17),
     (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_carrier_bw_dl_r17_present),
     asn1::table::enum_op<decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17)>(),
     decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17)::has_ext,
     false,
     decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17)::nof_types,
     decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17)::nof_exts,
     nullptr,
     &lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17_opts::desc},
    {offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_carrier_freq_dl_r17),
     (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_carrier_freq_dl_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(16383),
     nullptr},
    {offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_cell_id_r17),
     (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_cell_id_r17_present),
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(503),
     nullptr},
    {0,
     (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_crs_muting_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_mbsfn_sf_cfg_list_r17),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::item_list_codec<decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_mbsfn_sf_cfg_list_r17),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &eutra_mbsfn_sf_cfg_s::desc},
    {offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_nrof_crs_ports_r17),
     (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_nrof_crs_ports_r17_present),
     asn1::table::enum_op<decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17)>(),
     decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17)::has_ext,
     false,
     decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17)::nof_types,
     decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17)::nof_exts,
     nullptr,
     &lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17_opts::desc},
    {offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_v_shift_r17),
     (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_v_shift_r17_present),
     asn1::table::enum_op<decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17)>(),
     decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17)::has_ext,
     false,
     decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17)::nof_types,
     decltype(lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17)::nof_exts,
     nullptr,
     &lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17_opts::desc}};
static const asn1::table::field_name seq_table_lte_neigh_cells_crs_assist_info_r17_s_names[] = {
    {"neighCarrierBandwidthDL-r17", nullptr},
    {"neighCarrierFreqDL-r17", nullptr},
    {"neighCellId-r17", nullptr},
    {"neighCRS-muting-r17", "enabled"},
    {"neighMBSFN-SubframeConfigList-r17", nullptr},
    {"neighNrofCRS-Ports-r17", nullptr},
    {"neighV-Shift-r17", nullptr}};
static const asn1::table::seq_opt seq_table_lte_neigh_cells_crs_assist_info_r17_s_opts[] = {
    {0, (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_carrier_bw_dl_r17_present)},
    {1, (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_carrier_freq_dl_r17_present)},
    {2, (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_cell_id_r17_present)},
    {3, (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_crs_muting_r17_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_nrof_crs_ports_r17_present)},
    {6, (int32_t)offsetof(lte_neigh_cells_crs_assist_info_r17_s, neigh_v_shift_r17_present)}};
const asn1::table::seq_desc lte_neigh_cells_crs_assist_info_r17_s::desc = {
    {seq_table_lte_neigh_cells_crs_assist_info_r17_s_fields,
     7,
     UINT64_C(0x0),
     seq_table_lte_neigh_cells_crs_assist_info_r17_s_opts,
     7},
    false,
    false,
    nullptr,
    0,
    seq_table_lte_neigh_cells_crs_assist_info_r17_s_names};

static const char* enum_names_lte_neigh_cells_crs_assist_info_r17_s_neigh_carrier_bw_dl_r17_e__values[] =
    {"n6", "n15", "n25", "n50", "n75", "n100", "spare2", "spare1"};
const asn1::table::enum_desc lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17_opts::desc = {
    enum_names_lte_neigh_cells_crs_assist_info_r17_s_neigh_carrier_bw_dl_r17_e__values,
    8,
    "lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17_e_"};
uint8_t lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {6, 15, 25, 50, 75, 100};
  return map_enum_number(numbers, 6, value, "lte_neigh_cells_crs_assist_info_r17_s::neigh_carrier_bw_dl_r17_e_");
}

static const char* enum_names_lte_neigh_cells_crs_assist_info_r17_s_neigh_nrof_crs_ports_r17_e__values[] = {"n1",
                                                                                                            "n2",
                                                                                                            "n4"};
const asn1::table::enum_desc lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17_opts::desc  = {
    enum_names_lte_neigh_cells_crs_assist_info_r17_s_neigh_nrof_crs_ports_r17_e__values,
    3,
    "lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17_e_"};
uint8_t lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "lte_neigh_cells_crs_assist_info_r17_s::neigh_nrof_crs_ports_r17_e_");
}

static const char* enum_names_lte_neigh_cells_crs_assist_info_r17_s_neigh_v_shift_r17_e__values[] =
    {"n0", "n1", "n2", "n3", "n4", "n5"};
const asn1::table::enum_desc lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17_opts::desc = {
    enum_names_lte_neigh_cells_crs_assist_info_r17_s_neigh_v_shift_r17_e__values,
    6,
    "lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17_e_"};
uint8_t lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3, 4, 5};
  return map_enum_number(numbers, 6, value, "lte_neigh_cells_crs_assist_info_r17_s::neigh_v_shift_r17_e_");
}

// Tag2-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_tag2_r18_s_fields[] = {
    {offsetof(tag2_r18_s, tag2_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(tag2_r18_s, tag2_flag_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(tag2_r18_s, n_timing_advance_offset2_r18),
     (int32_t)offsetof(tag2_r18_s, n_timing_advance_offset2_r18_present),
     asn1::table::enum_op<decltype(tag2_r18_s::n_timing_advance_offset2_r18)>(),
     decltype(tag2_r18_s::n_timing_advance_offset2_r18)::has_ext,
     false,
     decltype(tag2_r18_s::n_timing_advance_offset2_r18)::nof_types,
     decltype(tag2_r18_s::n_timing_advance_offset2_r18)::nof_exts,
     nullptr,
     &tag2_r18_s::n_timing_advance_offset2_r18_opts::desc}};
static const asn1::table::field_name seq_table_tag2_r18_s_names[] = {{"tag2-Id-r18", nullptr},
                                                                     {"tag2-flag-r18", nullptr},
                                                                     {"n-TimingAdvanceOffset2-r18", nullptr}};
static const asn1::table::seq_opt    seq_table_tag2_r18_s_opts[]  = {
    {2, (int32_t)offsetof(tag2_r18_s, n_timing_advance_offset2_r18_present)}};
const asn1::table::seq_desc tag2_r18_s::desc = {
    {seq_table_tag2_r18_s_fields, 3, UINT64_C(0x3), seq_table_tag2_r18_s_opts, 1},
    false,
    false,
    nullptr,
    0,
    seq_table_tag2_r18_s_names};

static const char*           enum_names_tag2_r18_s_n_timing_advance_offset2_r18_e__values[] = {"n0",
                                                                                               "n25600",
                                                                                               "n39936",
                                                                                               "spare1"};
const asn1::table::enum_desc tag2_r18_s::n_timing_advance_offset2_r18_opts::desc            = {
    enum_names_tag2_r18_s_n_timing_advance_offset2_r18_e__values,
    4,
    "tag2_r18_s::n_timing_advance_offset2_r18_e_"};
uint16_t tag2_r18_s::n_timing_advance_offset2_r18_opts::to_number() const
{
  static const uint16_t numbers[] = {0, 25600, 39936};
  return map_enum_number(numbers, 3, value, "tag2_r18_s::n_timing_advance_offset2_r18_e_");
}

// MC-DCI-SetOfCells-r18 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mc_dci_set_of_cells_r18_s_fields[] = {
    {offsetof(mc_dci_set_of_cells_r18_s, set_of_cells_id_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {offsetof(mc_dci_set_of_cells_r18_s, nci_value_r18),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(7),
     nullptr},
    {offsetof(mc_dci_set_of_cells_r18_s, sched_cell_list_dci_1_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     2,
     4,
     &asn1::table::list_codec<decltype(mc_dci_set_of_cells_r18_s::sched_cell_list_dci_1_3_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(31), false, false>>::value},
    {offsetof(mc_dci_set_of_cells_r18_s, sched_cell_list_dci_0_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     2,
     4,
     &asn1::table::list_codec<decltype(mc_dci_set_of_cells_r18_s::sched_cell_list_dci_0_3_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(31), false, false>>::value},
    {offsetof(mc_dci_set_of_cells_r18_s, sched_cell_combo_list_dci_1_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<
         decltype(mc_dci_set_of_cells_r18_s::sched_cell_combo_list_dci_1_3_r18),
         asn1::table::list_json<asn1::table::int_json>,
         asn1::table::
             list_items<false, 1, 4, false, asn1::table::int_items<false, INT64_C(0), UINT64_C(3), false, false>>>::
         value},
    {offsetof(mc_dci_set_of_cells_r18_s, sched_cell_combo_list_dci_0_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<
         decltype(mc_dci_set_of_cells_r18_s::sched_cell_combo_list_dci_0_3_r18),
         asn1::table::list_json<asn1::table::int_json>,
         asn1::table::
             list_items<false, 1, 4, false, asn1::table::int_items<false, INT64_C(0), UINT64_C(3), false, false>>>::
         value},
    {offsetof(mc_dci_set_of_cells_r18_s, ant_ports_dci1_3_r18),
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, ant_ports_dci1_3_r18_present),
     asn1::table::enum_op<decltype(mc_dci_set_of_cells_r18_s::ant_ports_dci1_3_r18)>(),
     decltype(mc_dci_set_of_cells_r18_s::ant_ports_dci1_3_r18)::has_ext,
     false,
     decltype(mc_dci_set_of_cells_r18_s::ant_ports_dci1_3_r18)::nof_types,
     decltype(mc_dci_set_of_cells_r18_s::ant_ports_dci1_3_r18)::nof_exts,
     nullptr,
     &mc_dci_set_of_cells_r18_s::ant_ports_dci1_3_r18_opts::desc},
    {offsetof(mc_dci_set_of_cells_r18_s, ant_ports_dci0_3_r18),
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, ant_ports_dci0_3_r18_present),
     asn1::table::enum_op<decltype(mc_dci_set_of_cells_r18_s::ant_ports_dci0_3_r18)>(),
     decltype(mc_dci_set_of_cells_r18_s::ant_ports_dci0_3_r18)::has_ext,
     false,
     decltype(mc_dci_set_of_cells_r18_s::ant_ports_dci0_3_r18)::nof_types,
     decltype(mc_dci_set_of_cells_r18_s::ant_ports_dci0_3_r18)::nof_exts,
     nullptr,
     &mc_dci_set_of_cells_r18_s::ant_ports_dci0_3_r18_opts::desc},
    {offsetof(mc_dci_set_of_cells_r18_s, tpmi_dci0_3_r18),
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, tpmi_dci0_3_r18_present),
     asn1::table::enum_op<decltype(mc_dci_set_of_cells_r18_s::tpmi_dci0_3_r18)>(),
     decltype(mc_dci_set_of_cells_r18_s::tpmi_dci0_3_r18)::has_ext,
     false,
     decltype(mc_dci_set_of_cells_r18_s::tpmi_dci0_3_r18)::nof_types,
     decltype(mc_dci_set_of_cells_r18_s::tpmi_dci0_3_r18)::nof_exts,
     nullptr,
     &mc_dci_set_of_cells_r18_s::tpmi_dci0_3_r18_opts::desc},
    {offsetof(mc_dci_set_of_cells_r18_s, sri_dci0_3_r18),
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, sri_dci0_3_r18_present),
     asn1::table::enum_op<decltype(mc_dci_set_of_cells_r18_s::sri_dci0_3_r18)>(),
     decltype(mc_dci_set_of_cells_r18_s::sri_dci0_3_r18)::has_ext,
     false,
     decltype(mc_dci_set_of_cells_r18_s::sri_dci0_3_r18)::nof_types,
     decltype(mc_dci_set_of_cells_r18_s::sri_dci0_3_r18)::nof_exts,
     nullptr,
     &mc_dci_set_of_cells_r18_s::sri_dci0_3_r18_opts::desc},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, prio_ind_dci_1_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, prio_ind_dci_0_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, dormancy_dci_1_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, dormancy_dci_0_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdcch_mon_adapt_dci_1_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdcch_mon_adapt_dci_0_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, minimum_sched_offset_k0_dci_1_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, minimum_sched_offset_k0_dci_0_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdsch_harq_ack_one_shot_feedback_dci_1_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdsch_harq_ack_enh_type3_dci_1_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdsch_harq_ack_enh_type3_dc_ifield_dci_1_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdsch_harq_ack_retx_dci_1_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pucch_s_scell_dyn_dci_1_3_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(mc_dci_set_of_cells_r18_s, tdra_field_idx_list_dci_1_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     32,
     &asn1::table::list_codec<
         decltype(mc_dci_set_of_cells_r18_s::tdra_field_idx_list_dci_1_3_r18),
         asn1::table::list_json<asn1::table::int_json>,
         asn1::table::
             list_items<false, 2, 16, false, asn1::table::int_items<false, INT64_C(0), UINT64_C(15), false, false>>>::
         value},
    {offsetof(mc_dci_set_of_cells_r18_s, tdra_field_idx_list_dci_0_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     64,
     &asn1::table::list_codec<
         decltype(mc_dci_set_of_cells_r18_s::tdra_field_idx_list_dci_0_3_r18),
         asn1::table::list_json<asn1::table::int_json>,
         asn1::table::
             list_items<false, 2, 16, false, asn1::table::int_items<false, INT64_C(0), UINT64_C(63), false, false>>>::
         value},
    {offsetof(mc_dci_set_of_cells_r18_s, rate_match_list_dci_1_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<decltype(mc_dci_set_of_cells_r18_s::rate_match_list_dci_1_3_r18),
                              asn1::table::list_json<asn1::table::struct_json>,
                              asn1::table::list_items<false, 1, 4, false, asn1::table::object_items<false>>>::value},
    {offsetof(mc_dci_set_of_cells_r18_s, zp_csi_rs_list_dci_1_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<decltype(mc_dci_set_of_cells_r18_s::zp_csi_rs_list_dci_1_3_r18),
                              asn1::table::list_json<asn1::table::struct_json>,
                              asn1::table::list_items<false, 1, 4, false, asn1::table::object_items<false>>>::value},
    {offsetof(mc_dci_set_of_cells_r18_s, tci_list_dci_1_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<decltype(mc_dci_set_of_cells_r18_s::tci_list_dci_1_3_r18),
                              asn1::table::list_json<asn1::table::struct_json>,
                              asn1::table::list_items<false, 2, 4, false, asn1::table::object_items<false>>>::value},
    {offsetof(mc_dci_set_of_cells_r18_s, srs_request_list_dci_1_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<decltype(mc_dci_set_of_cells_r18_s::srs_request_list_dci_1_3_r18),
                              asn1::table::list_json<asn1::table::struct_json>,
                              asn1::table::list_items<false, 1, 4, false, asn1::table::object_items<false>>>::value},
    {offsetof(mc_dci_set_of_cells_r18_s, srs_offset_list_dci_1_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<
         decltype(mc_dci_set_of_cells_r18_s::srs_offset_list_dci_1_3_r18),
         asn1::table::list_json<asn1::table::int_json>,
         asn1::table::
             list_items<false, 1, 4, false, asn1::table::int_items<false, INT64_C(0), UINT64_C(3), false, false>>>::
         value},
    {offsetof(mc_dci_set_of_cells_r18_s, srs_request_list_dci_0_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     16,
     &asn1::table::list_codec<decltype(mc_dci_set_of_cells_r18_s::srs_request_list_dci_0_3_r18),
                              asn1::table::list_json<asn1::table::struct_json>,
                              asn1::table::list_items<false, 1, 4, false, asn1::table::object_items<false>>>::value},
    {offsetof(mc_dci_set_of_cells_r18_s, srs_offset_list_dci_0_3_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     8,
     &asn1::table::list_codec<
         decltype(mc_dci_set_of_cells_r18_s::srs_offset_list_dci_0_3_r18),
         asn1::table::list_json<asn1::table::int_json>,
         asn1::table::
             list_items<false, 1, 4, false, asn1::table::int_items<false, INT64_C(0), UINT64_C(3), false, false>>>::
         value}};
static const asn1::table::field_name seq_table_mc_dci_set_of_cells_r18_s_names[] = {
    {"setOfCellsId-r18", nullptr},
    {"nCI-Value-r18", nullptr},
    {"scheduledCellListDCI-1-3-r18", nullptr},
    {"scheduledCellListDCI-0-3-r18", nullptr},
    {"scheduledCellComboListDCI-1-3-r18", nullptr},
    {"scheduledCellComboListDCI-0-3-r18", nullptr},
    {"antennaPortsDCI1-3-r18", nullptr},
    {"antennaPortsDCI0-3-r18", nullptr},
    {"tpmi-DCI0-3-r18", nullptr},
    {"sri-DCI0-3-r18", nullptr},
    {"priorityIndicatorDCI-1-3-r18", "enabled"},
    {"priorityIndicatorDCI-0-3-r18", "enabled"},
    {"dormancyDCI-1-3-r18", "enabled"},
    {"dormancyDCI-0-3-r18", "enabled"},
    {"pdcchMonAdaptDCI-1-3-r18", "enabled"},
    {"pdcchMonAdaptDCI-0-3-r18", "enabled"},
    {"minimumSchedulingOffsetK0DCI-1-3-r18", "enabled"},
    {"minimumSchedulingOffsetK0DCI-0-3-r18", "enabled"},
    {"pdsch-HARQ-ACK-OneShotFeedbackDCI-1-3-r18", "enabled"},
    {"pdsch-HARQ-ACK-enhType3DCI-1-3-r18", "enabled"},
    {"pdsch-HARQ-ACK-enhType3DCIfieldDCI-1-3-r18", "enabled"},
    {"pdsch-HARQ-ACK-retxDCI-1-3-r18", "enabled"},
    {"pucch-sSCellDynDCI-1-3-r18", "enabled"},
    {"tdra-FieldIndexListDCI-1-3-r18", nullptr},
    {"tdra-FieldIndexListDCI-0-3-r18", nullptr},
    {"rateMatchListDCI-1-3-r18", nullptr},
    {"zp-CSI-RSListDCI-1-3-r18", nullptr},
    {"tci-ListDCI-1-3-r18", nullptr},
    {"srs-RequestListDCI-1-3-r18", nullptr},
    {"srs-OffsetListDCI-1-3-r18", nullptr},
    {"srs-RequestListDCI-0-3-r18", nullptr},
    {"srs-OffsetListDCI-0-3-r18", nullptr}};
static const asn1::table::seq_opt seq_table_mc_dci_set_of_cells_r18_s_opts[] = {
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, ant_ports_dci1_3_r18_present)},
    {7, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, ant_ports_dci0_3_r18_present)},
    {8, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, tpmi_dci0_3_r18_present)},
    {9, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, sri_dci0_3_r18_present)},
    {10, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, prio_ind_dci_1_3_r18_present)},
    {11, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, prio_ind_dci_0_3_r18_present)},
    {12, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, dormancy_dci_1_3_r18_present)},
    {13, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, dormancy_dci_0_3_r18_present)},
    {14, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdcch_mon_adapt_dci_1_3_r18_present)},
    {15, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdcch_mon_adapt_dci_0_3_r18_present)},
    {16, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, minimum_sched_offset_k0_dci_1_3_r18_present)},
    {17, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, minimum_sched_offset_k0_dci_0_3_r18_present)},
    {18, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdsch_harq_ack_one_shot_feedback_dci_1_3_r18_present)},
    {19, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdsch_harq_ack_enh_type3_dci_1_3_r18_present)},
    {20, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdsch_harq_ack_enh_type3_dc_ifield_dci_1_3_r18_present)},
    {21, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pdsch_harq_ack_retx_dci_1_3_r18_present)},
    {22, (int32_t)offsetof(mc_dci_set_of_cells_r18_s, pucch_s_scell_dyn_dci_1_3_r18_present)},
    {23, asn1::table::seq_field::list_presence},
    {24, asn1::table::seq_field::list_presence},
    {25, asn1::table::seq_field::list_presence},
    {26, asn1::table::seq_field::list_presence},
    {27, asn1::table::seq_field::list_presence},
    {28, asn1::table::seq_field::list_presence},
    {29, asn1::table::seq_field::list_presence},
    {30, asn1::table::seq_field::list_presence},
    {31, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc mc_dci_set_of_cells_r18_s::desc = {
    {seq_table_mc_dci_set_of_cells_r18_s_fields, 32, UINT64_C(0x3), seq_table_mc_dci_set_of_cells_r18_s_opts, 30},
    false,
    false,
    nullptr,
    0,
    seq_table_mc_dci_set_of_cells_r18_s_names};

static const char* enum_names_mc_dci_set_of_cells_r18_s_ant_ports_dci1_3_r18_e__values[] = {"type1a", "type2"};
const asn1::table::enum_desc mc_dci_set_of_cells_r18_s::ant_ports_dci1_3_r18_opts::desc  = {
    enum_names_mc_dci_set_of_cells_r18_s_ant_ports_dci1_3_r18_e__values,
    2,
    "mc_dci_set_of_cells_r18_s::ant_ports_dci1_3_r18_e_"};
uint8_t mc_dci_set_of_cells_r18_s::ant_ports_dci1_3_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "mc_dci_set_of_cells_r18_s::ant_ports_dci1_3_r18_e_");
}

static const char* enum_names_mc_dci_set_of_cells_r18_s_ant_ports_dci0_3_r18_e__values[] = {"type1a", "type2"};
const asn1::table::enum_desc mc_dci_set_of_cells_r18_s::ant_ports_dci0_3_r18_opts::desc  = {
    enum_names_mc_dci_set_of_cells_r18_s_ant_ports_dci0_3_r18_e__values,
    2,
    "mc_dci_set_of_cells_r18_s::ant_ports_dci0_3_r18_e_"};
uint8_t mc_dci_set_of_cells_r18_s::ant_ports_dci0_3_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "mc_dci_set_of_cells_r18_s::ant_ports_dci0_3_r18_e_");
}

static const char*           enum_names_mc_dci_set_of_cells_r18_s_tpmi_dci0_3_r18_e__values[] = {"type1a", "type2"};
const asn1::table::enum_desc mc_dci_set_of_cells_r18_s::tpmi_dci0_3_r18_opts::desc            = {
    enum_names_mc_dci_set_of_cells_r18_s_tpmi_dci0_3_r18_e__values,
    2,
    "mc_dci_set_of_cells_r18_s::tpmi_dci0_3_r18_e_"};
uint8_t mc_dci_set_of_cells_r18_s::tpmi_dci0_3_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "mc_dci_set_of_cells_r18_s::tpmi_dci0_3_r18_e_");
}

static const char*           enum_names_mc_dci_set_of_cells_r18_s_sri_dci0_3_r18_e__values[] = {"type1a", "type2"};
const asn1::table::enum_desc mc_dci_set_of_cells_r18_s::sri_dci0_3_r18_opts::desc            = {
    enum_names_mc_dci_set_of_cells_r18_s_sri_dci0_3_r18_e__values,
    2,
    "mc_dci_set_of_cells_r18_s::sri_dci0_3_r18_e_"};
uint8_t mc_dci_set_of_cells_r18_s::sri_dci0_3_r18_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "mc_dci_set_of_cells_r18_s::sri_dci0_3_r18_e_");
}

// MIMOParam-v1850 ::= SEQUENCE
static const asn1::table::seq_field seq_table_mimo_param_v1850_s_fields[] = {
    {offsetof(mimo_param_v1850_s, add_tdd_cfg_per_pci_to_add_mod_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::item_list_codec<decltype(mimo_param_v1850_s::add_tdd_cfg_per_pci_to_add_mod_list_r18),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &add_tdd_cfg_per_pci_to_add_mod_r18_s::desc},
    {offsetof(mimo_param_v1850_s, add_tdd_cfg_per_pci_to_release_list_r18),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     7,
     &asn1::table::list_codec<decltype(mimo_param_v1850_s::add_tdd_cfg_per_pci_to_release_list_r18),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(1), UINT64_C(7), false, false>>::value}};
static const asn1::table::field_name seq_table_mimo_param_v1850_s_names[] = {
    {"additionalTDDConfig-perPCI-ToAddModList-r18", nullptr},
    {"additionalTDDConfig-perPCI-ToReleaseList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_mimo_param_v1850_s_opts[] = {{0, asn1::table::seq_field::list_presence},
                                                                         {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       mimo_param_v1850_s::desc            = {
    {seq_table_mimo_param_v1850_s_fields, 2, UINT64_C(0x0), seq_table_mimo_param_v1850_s_opts, 2},
    false,
    false,
    nullptr,
    0,
    seq_table_mimo_param_v1850_s_names};

// ServingCellConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_pdcch_serving_cell_cfg_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdcch_serving_cell_cfg_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_pdcch_serving_cell_cfg_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_pdcch_serving_cell_cfg_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_pdcch_serving_cell_cfg_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_pdcch_serving_cell_cfg_setup_release_names,
    "setup_release_c",
    setup_release_c<pdcch_serving_cell_cfg_s>::type_offset(),
    setup_release_c<pdcch_serving_cell_cfg_s>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_pdsch_serving_cell_cfg_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &pdsch_serving_cell_cfg_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_pdsch_serving_cell_cfg_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_pdsch_serving_cell_cfg_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_pdsch_serving_cell_cfg_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_pdsch_serving_cell_cfg_setup_release_names,
    "setup_release_c",
    setup_release_c<pdsch_serving_cell_cfg_s>::type_offset(),
    setup_release_c<pdsch_serving_cell_cfg_s>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_csi_meas_cfg_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &csi_meas_cfg_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_csi_meas_cfg_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_csi_meas_cfg_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_csi_meas_cfg_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_csi_meas_cfg_setup_release_names,
    "setup_release_c",
    setup_release_c<csi_meas_cfg_s>::type_offset(),
    setup_release_c<csi_meas_cfg_s>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_lte_crs_to_match_around_setup_release_alternatives[] =
    {{0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
     {0,
      asn1::table::seq_field::mandatory,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      nullptr,
      &rate_match_pattern_lte_crs_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_lte_crs_to_match_around_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_lte_crs_to_match_around_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_lte_crs_to_match_around_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_lte_crs_to_match_around_setup_release_names,
    "setup_release_c",
    setup_release_c<rate_match_pattern_lte_crs_s>::type_offset(),
    setup_release_c<rate_match_pattern_lte_crs_s>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_dormant_bwp_cfg_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dormant_bwp_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_dormant_bwp_cfg_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_dormant_bwp_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_dormant_bwp_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_dormant_bwp_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<dormant_bwp_cfg_r16_s>::type_offset(),
    setup_release_c<dormant_bwp_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_dummy2_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &dummy_j_s::desc}};
static const asn1::table::field_name  seq_table_serving_cell_cfg_s_dummy2_setup_release_names[] = {{"release", nullptr},
                                                                                                   {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_dummy2_setup_release         = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_dummy2_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_dummy2_setup_release_names,
    "setup_release_c",
    setup_release_c<dummy_j_s>::type_offset(),
    setup_release_c<dummy_j_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list1_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         3,
         &asn1::table::item_list_codec<setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::value_type,
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
         &rate_match_pattern_lte_crs_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_lte_crs_pattern_list1_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_lte_crs_pattern_list1_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list1_r16_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list1_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::type_offset(),
    setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::value_offset()};
static const asn1::table::seq_field
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list2_r16_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         3,
         &asn1::table::item_list_codec<setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::value_type,
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
         &rate_match_pattern_lte_crs_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_lte_crs_pattern_list2_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_lte_crs_pattern_list2_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list2_r16_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list2_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::type_offset(),
    setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_ch_access_cfg_r16_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ch_access_cfg_r16_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_ch_access_cfg_r16_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_ch_access_cfg_r16_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_ch_access_cfg_r16_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_ch_access_cfg_r16_setup_release_names,
    "setup_release_c",
    setup_release_c<ch_access_cfg_r16_s>::type_offset(),
    setup_release_c<ch_access_cfg_r16_s>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_nr_dl_prs_pdc_info_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_dl_prs_pdc_info_r17_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_nr_dl_prs_pdc_info_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_nr_dl_prs_pdc_info_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_nr_dl_prs_pdc_info_r17_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_nr_dl_prs_pdc_info_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<nr_dl_prs_pdc_info_r17_s>::type_offset(),
    setup_release_c<nr_dl_prs_pdc_info_r17_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_serving_cell_cfg_s_semi_static_ch_access_cfg_ue_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &semi_static_ch_access_cfg_ue_r17_s::desc}};
static const asn1::table::field_name
    seq_table_serving_cell_cfg_s_semi_static_ch_access_cfg_ue_r17_setup_release_names[] = {{"release", nullptr},
                                                                                           {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_semi_static_ch_access_cfg_ue_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_semi_static_ch_access_cfg_ue_r17_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_semi_static_ch_access_cfg_ue_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<semi_static_ch_access_cfg_ue_r17_s>::type_offset(),
    setup_release_c<semi_static_ch_access_cfg_ue_r17_s>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_mimo_param_r17_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mimo_param_r17_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_mimo_param_r17_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_mimo_param_r17_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_mimo_param_r17_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_mimo_param_r17_setup_release_names,
    "setup_release_c",
    setup_release_c<mimo_param_r17_s>::type_offset(),
    setup_release_c<mimo_param_r17_s>::value_offset()};
static const asn1::table::seq_field
    seq_table_serving_cell_cfg_s_lte_neigh_cells_crs_assist_info_list_r17_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         8,
         &asn1::table::item_list_codec<
             setup_release_c<dyn_seq_of<lte_neigh_cells_crs_assist_info_r17_s, 1, 8>>::value_type,
             asn1::table::sequence_item_ops,
             false,
             true>::value,
         &lte_neigh_cells_crs_assist_info_r17_s::desc}};
static const asn1::table::field_name
    seq_table_serving_cell_cfg_s_lte_neigh_cells_crs_assist_info_list_r17_setup_release_names[] = {{"release", nullptr},
                                                                                                   {"setup", nullptr}};
static const asn1::table::choice_desc
    seq_table_serving_cell_cfg_s_lte_neigh_cells_crs_assist_info_list_r17_setup_release = {
        asn1::table::enum_int_type<asn1::setup_release_e>(),
        2,
        0,
        false,
        false,
        seq_table_serving_cell_cfg_s_lte_neigh_cells_crs_assist_info_list_r17_setup_release_alternatives,
        nullptr,
        seq_table_serving_cell_cfg_s_lte_neigh_cells_crs_assist_info_list_r17_setup_release_names,
        "setup_release_c",
        setup_release_c<dyn_seq_of<lte_neigh_cells_crs_assist_info_r17_s, 1, 8>>::type_offset(),
        setup_release_c<dyn_seq_of<lte_neigh_cells_crs_assist_info_r17_s, 1, 8>>::value_offset()};
static const asn1::table::seq_field
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list3_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         3,
         &asn1::table::item_list_codec<setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::value_type,
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
         &rate_match_pattern_lte_crs_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_lte_crs_pattern_list3_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_lte_crs_pattern_list3_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list3_r18_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list3_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::type_offset(),
    setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::value_offset()};
static const asn1::table::seq_field
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list4_r18_setup_release_alternatives[] = {
        {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::codec,
         false,
         false,
         1,
         3,
         &asn1::table::item_list_codec<setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::value_type,
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
         &rate_match_pattern_lte_crs_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_lte_crs_pattern_list4_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_lte_crs_pattern_list4_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list4_r18_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_lte_crs_pattern_list4_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::type_offset(),
    setup_release_c<dyn_seq_of<rate_match_pattern_lte_crs_s, 1, 3>>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_cell_dtx_drx_cfg_r18_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_dtx_drx_cfg_r18_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_cell_dtx_drx_cfg_r18_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_cell_dtx_drx_cfg_r18_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_cell_dtx_drx_cfg_r18_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_cell_dtx_drx_cfg_r18_setup_release_names,
    "setup_release_c",
    setup_release_c<cell_dtx_drx_cfg_r18_s>::type_offset(),
    setup_release_c<cell_dtx_drx_cfg_r18_s>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_mimo_param_v1850_setup_release_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &mimo_param_v1850_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_mimo_param_v1850_setup_release_names[] = {
    {"release", nullptr},
    {"setup", nullptr}};
static const asn1::table::choice_desc seq_table_serving_cell_cfg_s_mimo_param_v1850_setup_release = {
    asn1::table::enum_int_type<asn1::setup_release_e>(),
    2,
    0,
    false,
    false,
    seq_table_serving_cell_cfg_s_mimo_param_v1850_setup_release_alternatives,
    nullptr,
    seq_table_serving_cell_cfg_s_mimo_param_v1850_setup_release_names,
    "setup_release_c",
    setup_release_c<mimo_param_v1850_s>::type_offset(),
    setup_release_c<mimo_param_v1850_s>::value_offset()};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_fields[] = {
    {offsetof(serving_cell_cfg_s, tdd_ul_dl_cfg_ded),
     (int32_t)offsetof(serving_cell_cfg_s, tdd_ul_dl_cfg_ded_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tdd_ul_dl_cfg_ded_s::desc},
    {offsetof(serving_cell_cfg_s, init_dl_bwp),
     (int32_t)offsetof(serving_cell_cfg_s, init_dl_bwp_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &bwp_dl_ded_s::desc},
    {offsetof(serving_cell_cfg_s, dl_bwp_to_release_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::list_codec<decltype(serving_cell_cfg_s::dl_bwp_to_release_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, INT64_C(0), UINT64_C(4), false, false>>::value},
    {offsetof(serving_cell_cfg_s, dl_bwp_to_add_mod_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::item_list_codec<decltype(serving_cell_cfg_s::dl_bwp_to_add_mod_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &bwp_dl_s::desc},
    {offsetof(serving_cell_cfg_s, first_active_dl_bwp_id),
     (int32_t)offsetof(serving_cell_cfg_s, first_active_dl_bwp_id_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr},
    {offsetof(serving_cell_cfg_s, bwp_inactivity_timer),
     (int32_t)offsetof(serving_cell_cfg_s, bwp_inactivity_timer_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_s::bwp_inactivity_timer)>(),
     decltype(serving_cell_cfg_s::bwp_inactivity_timer)::has_ext,
     false,
     decltype(serving_cell_cfg_s::bwp_inactivity_timer)::nof_types,
     decltype(serving_cell_cfg_s::bwp_inactivity_timer)::nof_exts,
     nullptr,
     &serving_cell_cfg_s::bwp_inactivity_timer_opts::desc},
    {offsetof(serving_cell_cfg_s, default_dl_bwp_id),
     (int32_t)offsetof(serving_cell_cfg_s, default_dl_bwp_id_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(4),
     nullptr},
    {offsetof(serving_cell_cfg_s, ul_cfg),
     (int32_t)offsetof(serving_cell_cfg_s, ul_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_cfg_s::desc},
    {offsetof(serving_cell_cfg_s, supplementary_ul),
     (int32_t)offsetof(serving_cell_cfg_s, supplementary_ul_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_cfg_s::desc},
    {offsetof(serving_cell_cfg_s, pdcch_serving_cell_cfg),
     (int32_t)offsetof(serving_cell_cfg_s, pdcch_serving_cell_cfg_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_serving_cell_cfg_s_pdcch_serving_cell_cfg_setup_release},
    {offsetof(serving_cell_cfg_s, pdsch_serving_cell_cfg),
     (int32_t)offsetof(serving_cell_cfg_s, pdsch_serving_cell_cfg_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_serving_cell_cfg_s_pdsch_serving_cell_cfg_setup_release},
    {offsetof(serving_cell_cfg_s, csi_meas_cfg),
     (int32_t)offsetof(serving_cell_cfg_s, csi_meas_cfg_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &seq_table_serving_cell_cfg_s_csi_meas_cfg_setup_release},
    {offsetof(serving_cell_cfg_s, scell_deactivation_timer),
     (int32_t)offsetof(serving_cell_cfg_s, scell_deactivation_timer_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_s::scell_deactivation_timer)>(),
     decltype(serving_cell_cfg_s::scell_deactivation_timer)::has_ext,
     false,
     decltype(serving_cell_cfg_s::scell_deactivation_timer)::nof_types,
     decltype(serving_cell_cfg_s::scell_deactivation_timer)::nof_exts,
     nullptr,
     &serving_cell_cfg_s::scell_deactivation_timer_opts::desc},
    {offsetof(serving_cell_cfg_s, cross_carrier_sched_cfg),
     (int32_t)offsetof(serving_cell_cfg_s, cross_carrier_sched_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cross_carrier_sched_cfg_s::desc},
    {offsetof(serving_cell_cfg_s, tag_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(3),
     nullptr},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, dummy1_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(serving_cell_cfg_s, pathloss_ref_linking),
     (int32_t)offsetof(serving_cell_cfg_s, pathloss_ref_linking_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_s::pathloss_ref_linking)>(),
     decltype(serving_cell_cfg_s::pathloss_ref_linking)::has_ext,
     false,
     decltype(serving_cell_cfg_s::pathloss_ref_linking)::nof_types,
     decltype(serving_cell_cfg_s::pathloss_ref_linking)::nof_exts,
     nullptr,
     &serving_cell_cfg_s::pathloss_ref_linking_opts::desc},
    {offsetof(serving_cell_cfg_s, serving_cell_mo),
     (int32_t)offsetof(serving_cell_cfg_s, serving_cell_mo_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(1),
     UINT64_C(64),
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_names[] = {
    {"tdd-UL-DL-ConfigurationDedicated", nullptr},
    {"initialDownlinkBWP", nullptr},
    {"downlinkBWP-ToReleaseList", nullptr},
    {"downlinkBWP-ToAddModList", nullptr},
    {"firstActiveDownlinkBWP-Id", nullptr},
    {"bwp-InactivityTimer", nullptr},
    {"defaultDownlinkBWP-Id", nullptr},
    {"uplinkConfig", nullptr},
    {"supplementaryUplink", nullptr},
    {"pdcch-ServingCellConfig", nullptr},
    {"pdsch-ServingCellConfig", nullptr},
    {"csi-MeasConfig", nullptr},
    {"sCellDeactivationTimer", nullptr},
    {"crossCarrierSchedulingConfig", nullptr},
    {"tag-Id", nullptr},
    {"dummy1", "enabled"},
    {"pathlossReferenceLinking", nullptr},
    {"servingCellMO", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_s_opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_s, tdd_ul_dl_cfg_ded_present)},
    {1, (int32_t)offsetof(serving_cell_cfg_s, init_dl_bwp_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, (int32_t)offsetof(serving_cell_cfg_s, first_active_dl_bwp_id_present)},
    {5, (int32_t)offsetof(serving_cell_cfg_s, bwp_inactivity_timer_present)},
    {6, (int32_t)offsetof(serving_cell_cfg_s, default_dl_bwp_id_present)},
    {7, (int32_t)offsetof(serving_cell_cfg_s, ul_cfg_present)},
    {8, (int32_t)offsetof(serving_cell_cfg_s, supplementary_ul_present)},
    {9, (int32_t)offsetof(serving_cell_cfg_s, pdcch_serving_cell_cfg_present)},
    {10, (int32_t)offsetof(serving_cell_cfg_s, pdsch_serving_cell_cfg_present)},
    {11, (int32_t)offsetof(serving_cell_cfg_s, csi_meas_cfg_present)},
    {12, (int32_t)offsetof(serving_cell_cfg_s, scell_deactivation_timer_present)},
    {13, (int32_t)offsetof(serving_cell_cfg_s, cross_carrier_sched_cfg_present)},
    {15, (int32_t)offsetof(serving_cell_cfg_s, dummy1_present)},
    {16, (int32_t)offsetof(serving_cell_cfg_s, pathloss_ref_linking_present)},
    {17, (int32_t)offsetof(serving_cell_cfg_s, serving_cell_mo_present)}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_g0_fields[] = {
    {offsetof(serving_cell_cfg_s, lte_crs_to_match_around),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::lte_crs_to_match_around)>::value,
     &seq_table_serving_cell_cfg_s_lte_crs_to_match_around_setup_release},
    {offsetof(serving_cell_cfg_s, rate_match_pattern_to_add_mod_list),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::ptr_item_list_codec<decltype(serving_cell_cfg_s::rate_match_pattern_to_add_mod_list),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &rate_match_pattern_s::desc},
    {offsetof(serving_cell_cfg_s, rate_match_pattern_to_release_list),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::ptr_list_codec<decltype(serving_cell_cfg_s::rate_match_pattern_to_release_list),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(0), UINT64_C(3), false, false>>::value},
    {offsetof(serving_cell_cfg_s, dl_ch_bw_per_scs_list),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     5,
     &asn1::table::ptr_item_list_codec<decltype(serving_cell_cfg_s::dl_ch_bw_per_scs_list),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &scs_specific_carrier_s::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_g0_names[] = {
    {"lte-CRS-ToMatchAround", nullptr},
    {"rateMatchPatternToAddModList", nullptr},
    {"rateMatchPatternToReleaseList", nullptr},
    {"downlinkChannelBW-PerSCS-List", nullptr}};
static const asn1::table::seq_opt   seq_table_serving_cell_cfg_s_g0_opts[] = {{0, asn1::table::seq_field::ptr_presence},
                                                                              {1, asn1::table::seq_field::ptr_presence},
                                                                              {2, asn1::table::seq_field::ptr_presence},
                                                                              {3, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_g1_fields[] = {
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, supplementary_ul_release_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(serving_cell_cfg_s, tdd_ul_dl_cfg_ded_iab_mt_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::tdd_ul_dl_cfg_ded_iab_mt_r16)>::value,
     &tdd_ul_dl_cfg_ded_iab_mt_r16_s::desc},
    {offsetof(serving_cell_cfg_s, dormant_bwp_cfg_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::dormant_bwp_cfg_r16)>::value,
     &seq_table_serving_cell_cfg_s_dormant_bwp_cfg_r16_setup_release},
    {offsetof(serving_cell_cfg_s, ca_slot_offset_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::ca_slot_offset_r16)>::value,
     &serving_cell_cfg_s::ca_slot_offset_r16_c_::desc},
    {offsetof(serving_cell_cfg_s, dummy2),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::dummy2)>::value,
     &seq_table_serving_cell_cfg_s_dummy2_setup_release},
    {offsetof(serving_cell_cfg_s, intra_cell_guard_bands_dl_list_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     5,
     &asn1::table::ptr_item_list_codec<decltype(serving_cell_cfg_s::intra_cell_guard_bands_dl_list_r16),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &intra_cell_guard_bands_per_scs_r16_s::desc},
    {offsetof(serving_cell_cfg_s, intra_cell_guard_bands_ul_list_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     5,
     &asn1::table::ptr_item_list_codec<decltype(serving_cell_cfg_s::intra_cell_guard_bands_ul_list_r16),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &intra_cell_guard_bands_per_scs_r16_s::desc},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, csi_rs_validation_with_dci_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(serving_cell_cfg_s, lte_crs_pattern_list1_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::lte_crs_pattern_list1_r16)>::value,
     &seq_table_serving_cell_cfg_s_lte_crs_pattern_list1_r16_setup_release},
    {offsetof(serving_cell_cfg_s, lte_crs_pattern_list2_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::lte_crs_pattern_list2_r16)>::value,
     &seq_table_serving_cell_cfg_s_lte_crs_pattern_list2_r16_setup_release},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, crs_rate_match_per_coreset_pool_idx_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, enable_two_default_tci_states_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, enable_default_tci_state_per_coreset_pool_idx_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, enable_beam_switch_timing_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, cbg_tx_diff_t_bs_processing_type1_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, cbg_tx_diff_t_bs_processing_type2_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_g1_names[] = {
    {"supplementaryUplinkRelease-r16", "true"},
    {"tdd-UL-DL-ConfigurationDedicated-IAB-MT-r16", nullptr},
    {"dormantBWP-Config-r16", nullptr},
    {"ca-SlotOffset-r16", nullptr},
    {"dummy2", nullptr},
    {"intraCellGuardBandsDL-List-r16", nullptr},
    {"intraCellGuardBandsUL-List-r16", nullptr},
    {"csi-RS-ValidationWithDCI-r16", "enabled"},
    {"lte-CRS-PatternList1-r16", nullptr},
    {"lte-CRS-PatternList2-r16", nullptr},
    {"crs-RateMatch-PerCORESETPoolIndex-r16", "enabled"},
    {"enableTwoDefaultTCI-States-r16", "enabled"},
    {"enableDefaultTCI-StatePerCoresetPoolIndex-r16", "enabled"},
    {"enableBeamSwitchTiming-r16", "true"},
    {"cbg-TxDiffTBsProcessingType1-r16", "enabled"},
    {"cbg-TxDiffTBsProcessingType2-r16", "enabled"}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_s_g1_opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_s, supplementary_ul_release_r16_present)},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, asn1::table::seq_field::ptr_presence},
    {4, asn1::table::seq_field::ptr_presence},
    {5, asn1::table::seq_field::ptr_presence},
    {6, asn1::table::seq_field::ptr_presence},
    {7, (int32_t)offsetof(serving_cell_cfg_s, csi_rs_validation_with_dci_r16_present)},
    {8, asn1::table::seq_field::ptr_presence},
    {9, asn1::table::seq_field::ptr_presence},
    {10, (int32_t)offsetof(serving_cell_cfg_s, crs_rate_match_per_coreset_pool_idx_r16_present)},
    {11, (int32_t)offsetof(serving_cell_cfg_s, enable_two_default_tci_states_r16_present)},
    {12, (int32_t)offsetof(serving_cell_cfg_s, enable_default_tci_state_per_coreset_pool_idx_r16_present)},
    {13, (int32_t)offsetof(serving_cell_cfg_s, enable_beam_switch_timing_r16_present)},
    {14, (int32_t)offsetof(serving_cell_cfg_s, cbg_tx_diff_t_bs_processing_type1_r16_present)},
    {15, (int32_t)offsetof(serving_cell_cfg_s, cbg_tx_diff_t_bs_processing_type2_r16_present)}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_g2_fields[] = {
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, directional_collision_handling_r16_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(serving_cell_cfg_s, ch_access_cfg_r16),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::ch_access_cfg_r16)>::value,
     &seq_table_serving_cell_cfg_s_ch_access_cfg_r16_setup_release}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_g2_names[] = {
    {"directionalCollisionHandling-r16", "enabled"},
    {"channelAccessConfig-r16", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_s_g2_opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_s, directional_collision_handling_r16_present)},
    {1, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_g3_fields[] = {
    {offsetof(serving_cell_cfg_s, nr_dl_prs_pdc_info_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::nr_dl_prs_pdc_info_r17)>::value,
     &seq_table_serving_cell_cfg_s_nr_dl_prs_pdc_info_r17_setup_release},
    {offsetof(serving_cell_cfg_s, semi_static_ch_access_cfg_ue_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::semi_static_ch_access_cfg_ue_r17)>::value,
     &seq_table_serving_cell_cfg_s_semi_static_ch_access_cfg_ue_r17_setup_release},
    {offsetof(serving_cell_cfg_s, mimo_param_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::mimo_param_r17)>::value,
     &seq_table_serving_cell_cfg_s_mimo_param_r17_setup_release},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, ch_access_mode2_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, time_domain_harq_bundling_type1_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(serving_cell_cfg_s, nrof_harq_bundling_groups_r17),
     (int32_t)offsetof(serving_cell_cfg_s, nrof_harq_bundling_groups_r17_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_s::nrof_harq_bundling_groups_r17)>(),
     decltype(serving_cell_cfg_s::nrof_harq_bundling_groups_r17)::has_ext,
     false,
     decltype(serving_cell_cfg_s::nrof_harq_bundling_groups_r17)::nof_types,
     decltype(serving_cell_cfg_s::nrof_harq_bundling_groups_r17)::nof_exts,
     nullptr,
     &serving_cell_cfg_s::nrof_harq_bundling_groups_r17_opts::desc},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, fdmed_reception_multicast_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, more_than_one_nack_only_mode_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(serving_cell_cfg_s, tci_activ_cfg_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::tci_activ_cfg_r17)>::value,
     &tci_activ_cfg_r17_s::desc},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, directional_collision_handling_dc_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(serving_cell_cfg_s, lte_neigh_cells_crs_assist_info_list_r17),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::lte_neigh_cells_crs_assist_info_list_r17)>::value,
     &seq_table_serving_cell_cfg_s_lte_neigh_cells_crs_assist_info_list_r17_setup_release}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_g3_names[] = {
    {"nr-dl-PRS-PDC-Info-r17", nullptr},
    {"semiStaticChannelAccessConfigUE-r17", nullptr},
    {"mimoParam-r17", nullptr},
    {"channelAccessMode2-r17", "enabled"},
    {"timeDomainHARQ-BundlingType1-r17", "enabled"},
    {"nrofHARQ-BundlingGroups-r17", nullptr},
    {"fdmed-ReceptionMulticast-r17", "true"},
    {"moreThanOneNackOnlyMode-r17", "mode2"},
    {"tci-ActivatedConfig-r17", nullptr},
    {"directionalCollisionHandling-DC-r17", "enabled"},
    {"lte-NeighCellsCRS-AssistInfoList-r17", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_s_g3_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, asn1::table::seq_field::ptr_presence},
    {3, (int32_t)offsetof(serving_cell_cfg_s, ch_access_mode2_r17_present)},
    {4, (int32_t)offsetof(serving_cell_cfg_s, time_domain_harq_bundling_type1_r17_present)},
    {5, (int32_t)offsetof(serving_cell_cfg_s, nrof_harq_bundling_groups_r17_present)},
    {6, (int32_t)offsetof(serving_cell_cfg_s, fdmed_reception_multicast_r17_present)},
    {7, (int32_t)offsetof(serving_cell_cfg_s, more_than_one_nack_only_mode_r17_present)},
    {8, asn1::table::seq_field::ptr_presence},
    {9, (int32_t)offsetof(serving_cell_cfg_s, directional_collision_handling_dc_r17_present)},
    {10, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_g4_fields[] = {
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, lte_neigh_cells_crs_assumptions_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_g4_names[] = {
    {"lte-NeighCellsCRS-Assumptions-r17", "false"}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_s_g4_opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_s, lte_neigh_cells_crs_assumptions_r17_present)}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_g5_fields[] = {
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, cross_carrier_sched_cfg_release_r17_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_g5_names[] = {
    {"crossCarrierSchedulingConfigRelease-r17", "true"}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_s_g5_opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_s, cross_carrier_sched_cfg_release_r17_present)}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_g6_fields[] = {
    {offsetof(serving_cell_cfg_s, multi_pdsch_per_slot_type1_cb_r17),
     (int32_t)offsetof(serving_cell_cfg_s, multi_pdsch_per_slot_type1_cb_r17_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_s::multi_pdsch_per_slot_type1_cb_r17)>(),
     decltype(serving_cell_cfg_s::multi_pdsch_per_slot_type1_cb_r17)::has_ext,
     false,
     decltype(serving_cell_cfg_s::multi_pdsch_per_slot_type1_cb_r17)::nof_types,
     decltype(serving_cell_cfg_s::multi_pdsch_per_slot_type1_cb_r17)::nof_exts,
     nullptr,
     &serving_cell_cfg_s::multi_pdsch_per_slot_type1_cb_r17_opts::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_g6_names[] = {
    {"multiPDSCH-PerSlotType1-CB-r17", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_s_g6_opts[] = {
    {0, (int32_t)offsetof(serving_cell_cfg_s, multi_pdsch_per_slot_type1_cb_r17_present)}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_g7_fields[] = {
    {offsetof(serving_cell_cfg_s, lte_crs_pattern_list3_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::lte_crs_pattern_list3_r18)>::value,
     &seq_table_serving_cell_cfg_s_lte_crs_pattern_list3_r18_setup_release},
    {offsetof(serving_cell_cfg_s, lte_crs_pattern_list4_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::lte_crs_pattern_list4_r18)>::value,
     &seq_table_serving_cell_cfg_s_lte_crs_pattern_list4_r18_setup_release},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, pdcch_candidate_reception_with_crs_overlap_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(serving_cell_cfg_s, cjt_scheme_pdsch_r18),
     (int32_t)offsetof(serving_cell_cfg_s, cjt_scheme_pdsch_r18_present),
     asn1::table::enum_op<decltype(serving_cell_cfg_s::cjt_scheme_pdsch_r18)>(),
     decltype(serving_cell_cfg_s::cjt_scheme_pdsch_r18)::has_ext,
     false,
     decltype(serving_cell_cfg_s::cjt_scheme_pdsch_r18)::nof_types,
     decltype(serving_cell_cfg_s::cjt_scheme_pdsch_r18)::nof_exts,
     nullptr,
     &serving_cell_cfg_s::cjt_scheme_pdsch_r18_opts::desc},
    {offsetof(serving_cell_cfg_s, tag2_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::tag2_r18)>::value,
     &tag2_r18_s::desc},
    {offsetof(serving_cell_cfg_s, cell_dtx_drx_cfg_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::cell_dtx_drx_cfg_r18)>::value,
     &seq_table_serving_cell_cfg_s_cell_dtx_drx_cfg_r18_setup_release},
    {offsetof(serving_cell_cfg_s, position_in_dci_cell_dtrx_r18),
     (int32_t)offsetof(serving_cell_cfg_s, position_in_dci_cell_dtrx_r18_present),
     asn1::table::field_op::int_u8,
     false,
     false,
     INT64_C(0),
     UINT64_C(139),
     nullptr},
    {0,
     (int32_t)offsetof(serving_cell_cfg_s, cell_dtx_drx_l1activation_r18_present),
     asn1::table::field_op::none,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(serving_cell_cfg_s, mc_dci_set_of_cells_to_add_mod_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::ptr_item_list_codec<decltype(serving_cell_cfg_s::mc_dci_set_of_cells_to_add_mod_list_r18),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &mc_dci_set_of_cells_r18_s::desc},
    {offsetof(serving_cell_cfg_s, mc_dci_set_of_cells_to_release_list_r18),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     false,
     1,
     4,
     &asn1::table::ptr_list_codec<decltype(serving_cell_cfg_s::mc_dci_set_of_cells_to_release_list_r18),
                                  asn1::table::int_json,
                                  asn1::table::int_items<false, INT64_C(0), UINT64_C(3), false, false>>::value}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_g7_names[] = {
    {"lte-CRS-PatternList3-r18", nullptr},
    {"lte-CRS-PatternList4-r18", nullptr},
    {"pdcch-CandidateReceptionWithCRS-Overlap-r18", "enabled"},
    {"cjt-Scheme-PDSCH-r18", nullptr},
    {"tag2-r18", nullptr},
    {"cellDTX-DRX-Config-r18", nullptr},
    {"positionInDCI-cellDTRX-r18", nullptr},
    {"cellDTX-DRX-L1activation-r18", "enabled"},
    {"mc-DCI-SetOfCellsToAddModList-r18", nullptr},
    {"mc-DCI-SetOfCellsToReleaseList-r18", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_s_g7_opts[] = {
    {0, asn1::table::seq_field::ptr_presence},
    {1, asn1::table::seq_field::ptr_presence},
    {2, (int32_t)offsetof(serving_cell_cfg_s, pdcch_candidate_reception_with_crs_overlap_r18_present)},
    {3, (int32_t)offsetof(serving_cell_cfg_s, cjt_scheme_pdsch_r18_present)},
    {4, asn1::table::seq_field::ptr_presence},
    {5, asn1::table::seq_field::ptr_presence},
    {6, (int32_t)offsetof(serving_cell_cfg_s, position_in_dci_cell_dtrx_r18_present)},
    {7, (int32_t)offsetof(serving_cell_cfg_s, cell_dtx_drx_l1activation_r18_present)},
    {8, asn1::table::seq_field::ptr_presence},
    {9, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_field seq_table_serving_cell_cfg_s_g8_fields[] = {
    {offsetof(serving_cell_cfg_s, mimo_param_v1850),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(serving_cell_cfg_s::mimo_param_v1850)>::value,
     &seq_table_serving_cell_cfg_s_mimo_param_v1850_setup_release}};
static const asn1::table::field_name seq_table_serving_cell_cfg_s_g8_names[] = {{"mimoParam-v1850", nullptr}};
static const asn1::table::seq_opt seq_table_serving_cell_cfg_s_g8_opts[]  = {{0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_serving_cell_cfg_s_groups[] = {
    {{seq_table_serving_cell_cfg_s_g0_fields, 4, UINT64_C(0x0), seq_table_serving_cell_cfg_s_g0_opts, 4},
     false,
     seq_table_serving_cell_cfg_s_g0_names},
    {{seq_table_serving_cell_cfg_s_g1_fields, 16, UINT64_C(0x0), seq_table_serving_cell_cfg_s_g1_opts, 16},
     false,
     seq_table_serving_cell_cfg_s_g1_names},
    {{seq_table_serving_cell_cfg_s_g2_fields, 2, UINT64_C(0x0), seq_table_serving_cell_cfg_s_g2_opts, 2},
     false,
     seq_table_serving_cell_cfg_s_g2_names},
    {{seq_table_serving_cell_cfg_s_g3_fields, 11, UINT64_C(0x0), seq_table_serving_cell_cfg_s_g3_opts, 11},
     false,
     seq_table_serving_cell_cfg_s_g3_names},
    {{seq_table_serving_cell_cfg_s_g4_fields, 1, UINT64_C(0x0), seq_table_serving_cell_cfg_s_g4_opts, 1},
     false,
     seq_table_serving_cell_cfg_s_g4_names},
    {{seq_table_serving_cell_cfg_s_g5_fields, 1, UINT64_C(0x0), seq_table_serving_cell_cfg_s_g5_opts, 1},
     false,
     seq_table_serving_cell_cfg_s_g5_names},
    {{seq_table_serving_cell_cfg_s_g6_fields, 1, UINT64_C(0x0), seq_table_serving_cell_cfg_s_g6_opts, 1},
     false,
     seq_table_serving_cell_cfg_s_g6_names},
    {{seq_table_serving_cell_cfg_s_g7_fields, 10, UINT64_C(0x0), seq_table_serving_cell_cfg_s_g7_opts, 10},
     false,
     seq_table_serving_cell_cfg_s_g7_names},
    {{seq_table_serving_cell_cfg_s_g8_fields, 1, UINT64_C(0x0), seq_table_serving_cell_cfg_s_g8_opts, 1},
     false,
     seq_table_serving_cell_cfg_s_g8_names}};
const asn1::table::seq_desc serving_cell_cfg_s::desc = {
    {seq_table_serving_cell_cfg_s_fields, 18, UINT64_C(0x4000), seq_table_serving_cell_cfg_s_opts, 17},
    true,
    false,
    seq_table_serving_cell_cfg_s_groups,
    9,
    seq_table_serving_cell_cfg_s_names};

static const char* enum_names_serving_cell_cfg_s_bwp_inactivity_timer_e__values[] = {
    "ms2",    "ms3",    "ms4",    "ms5",    "ms6",    "ms8",    "ms10",   "ms20",   "ms30",   "ms40",   "ms50",
    "ms60",   "ms80",   "ms100",  "ms200",  "ms300",  "ms500",  "ms750",  "ms1280", "ms1920", "ms2560", "spare10",
    "spare9", "spare8", "spare7", "spare6", "spare5", "spare4", "spare3", "spare2", "spare1"};
const asn1::table::enum_desc serving_cell_cfg_s::bwp_inactivity_timer_opts::desc = {
    enum_names_serving_cell_cfg_s_bwp_inactivity_timer_e__values,
    31,
    "serving_cell_cfg_s::bwp_inactivity_timer_e_"};
uint16_t serving_cell_cfg_s::bwp_inactivity_timer_opts::to_number() const
{
  static const uint16_t numbers[] = {2,  3,  4,   5,   6,   8,   10,  20,   30,   40,  50,
                                     60, 80, 100, 200, 300, 500, 750, 1280, 1920, 2560};
  return map_enum_number(numbers, 21, value, "serving_cell_cfg_s::bwp_inactivity_timer_e_");
}

static const char*           enum_names_serving_cell_cfg_s_scell_deactivation_timer_e__values[] = {"ms20",
                                                                                                   "ms40",
                                                                                                   "ms80",
                                                                                                   "ms160",
                                                                                                   "ms200",
                                                                                                   "ms240",
                                                                                                   "ms320",
                                                                                                   "ms400",
                                                                                                   "ms480",
                                                                                                   "ms520",
                                                                                                   "ms640",
                                                                                                   "ms720",
                                                                                                   "ms840",
                                                                                                   "ms1280",
                                                                                                   "spare2",
                                                                                                   "spare1"};
const asn1::table::enum_desc serving_cell_cfg_s::scell_deactivation_timer_opts::desc            = {
    enum_names_serving_cell_cfg_s_scell_deactivation_timer_e__values,
    16,
    "serving_cell_cfg_s::scell_deactivation_timer_e_"};
uint16_t serving_cell_cfg_s::scell_deactivation_timer_opts::to_number() const
{
  static const uint16_t numbers[] = {20, 40, 80, 160, 200, 240, 320, 400, 480, 520, 640, 720, 840, 1280};
  return map_enum_number(numbers, 14, value, "serving_cell_cfg_s::scell_deactivation_timer_e_");
}

static const char*           enum_names_serving_cell_cfg_s_pathloss_ref_linking_e__values[] = {"spCell", "sCell"};
const asn1::table::enum_desc serving_cell_cfg_s::pathloss_ref_linking_opts::desc            = {
    enum_names_serving_cell_cfg_s_pathloss_ref_linking_e__values,
    2,
    "serving_cell_cfg_s::pathloss_ref_linking_e_"};

static const asn1::table::storage_ops* const choice_storage_serving_cell_cfg_s_ca_slot_offset_r16_c__alternatives[] = {
    &asn1::table::alternative_ops<int8_t, true>::value,
    &asn1::table::alternative_ops<int8_t, true>::value,
    &asn1::table::alternative_ops<int8_t, true>::value,
    &asn1::table::alternative_ops<int8_t, true>::value};
const asn1::table::choice_storage_desc serving_cell_cfg_s::ca_slot_offset_r16_c_::storage_desc = {
    choice_storage_serving_cell_cfg_s_ca_slot_offset_r16_c__alternatives,
    4,
    "serving_cell_cfg_s::ca_slot_offset_r16_c_"};

int8_t& serving_cell_cfg_s::ca_slot_offset_r16_c_::set_ref_scs15k_hz()
{
  set(types::ref_scs15k_hz);
  return c.get<int8_t>();
}
int8_t& serving_cell_cfg_s::ca_slot_offset_r16_c_::set_ref_scs30_k_hz()
{
  set(types::ref_scs30_k_hz);
  return c.get<int8_t>();
}
int8_t& serving_cell_cfg_s::ca_slot_offset_r16_c_::set_ref_scs60_k_hz()
{
  set(types::ref_scs60_k_hz);
  return c.get<int8_t>();
}
int8_t& serving_cell_cfg_s::ca_slot_offset_r16_c_::set_ref_scs120_k_hz()
{
  set(types::ref_scs120_k_hz);
  return c.get<int8_t>();
}
static const asn1::table::seq_field choice_table_serving_cell_cfg_s_ca_slot_offset_r16_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-2),
     UINT64_C(2),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-5),
     UINT64_C(5),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-10),
     UINT64_C(10),
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i8,
     false,
     false,
     INT64_C(-20),
     UINT64_C(20),
     nullptr}};
static const asn1::table::field_name choice_table_serving_cell_cfg_s_ca_slot_offset_r16_c__names[] =
    {{"refSCS15kHz", nullptr}, {"refSCS30KHz", nullptr}, {"refSCS60KHz", nullptr}, {"refSCS120KHz", nullptr}};
const asn1::table::choice_desc serving_cell_cfg_s::ca_slot_offset_r16_c_::desc = {
    asn1::table::enum_int_type<serving_cell_cfg_s::ca_slot_offset_r16_c_::types>(),
    serving_cell_cfg_s::ca_slot_offset_r16_c_::types::nof_types,
    serving_cell_cfg_s::ca_slot_offset_r16_c_::types::nof_exts,
    serving_cell_cfg_s::ca_slot_offset_r16_c_::types::has_ext,
    false,
    choice_table_serving_cell_cfg_s_ca_slot_offset_r16_c__alternatives,
    nullptr,
    choice_table_serving_cell_cfg_s_ca_slot_offset_r16_c__names,
    "serving_cell_cfg_s::ca_slot_offset_r16_c_",
    offsetof(serving_cell_cfg_s::ca_slot_offset_r16_c_, c.type),
    offsetof(serving_cell_cfg_s::ca_slot_offset_r16_c_, c.value),
    false,
    &serving_cell_cfg_s::ca_slot_offset_r16_c_::storage_desc};

static const char*           enum_names_serving_cell_cfg_s_ca_slot_offset_r16_c__types_values[] = {"refSCS15kHz",
                                                                                                   "refSCS30KHz",
                                                                                                   "refSCS60KHz",
                                                                                                   "refSCS120KHz"};
const asn1::table::enum_desc serving_cell_cfg_s::ca_slot_offset_r16_c_::types_opts::desc        = {
    enum_names_serving_cell_cfg_s_ca_slot_offset_r16_c__types_values,
    4,
    "serving_cell_cfg_s::ca_slot_offset_r16_c_::types"};
uint8_t serving_cell_cfg_s::ca_slot_offset_r16_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30, 60, 120};
  return map_enum_number(numbers, 4, value, "serving_cell_cfg_s::ca_slot_offset_r16_c_::types");
}

static const char* enum_names_serving_cell_cfg_s_nrof_harq_bundling_groups_r17_e__values[] = {"n1", "n2", "n4"};
const asn1::table::enum_desc serving_cell_cfg_s::nrof_harq_bundling_groups_r17_opts::desc  = {
    enum_names_serving_cell_cfg_s_nrof_harq_bundling_groups_r17_e__values,
    3,
    "serving_cell_cfg_s::nrof_harq_bundling_groups_r17_e_"};
uint8_t serving_cell_cfg_s::nrof_harq_bundling_groups_r17_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 4};
  return map_enum_number(numbers, 3, value, "serving_cell_cfg_s::nrof_harq_bundling_groups_r17_e_");
}

static const char*           enum_names_serving_cell_cfg_s_multi_pdsch_per_slot_type1_cb_r17_e__values[] = {"enabled",
                                                                                                            "disabled"};
const asn1::table::enum_desc serving_cell_cfg_s::multi_pdsch_per_slot_type1_cb_r17_opts::desc            = {
    enum_names_serving_cell_cfg_s_multi_pdsch_per_slot_type1_cb_r17_e__values,
    2,
    "serving_cell_cfg_s::multi_pdsch_per_slot_type1_cb_r17_e_"};

static const char* enum_names_serving_cell_cfg_s_cjt_scheme_pdsch_r18_e__values[] = {"cjtSchemeA", "cjtSchemeB"};
const asn1::table::enum_desc serving_cell_cfg_s::cjt_scheme_pdsch_r18_opts::desc  = {
    enum_names_serving_cell_cfg_s_cjt_scheme_pdsch_r18_e__values,
    2,
    "serving_cell_cfg_s::cjt_scheme_pdsch_r18_e_"};
