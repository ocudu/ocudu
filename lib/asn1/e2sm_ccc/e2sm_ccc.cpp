// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e2sm_ccc/e2sm_ccc.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::e2sm_ccc;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// AttributeName ::= SEQUENCE
static const asn1::table::seq_field seq_table_attribute_name_s_fields[] = {
    {offsetof(attribute_name_s, attribute_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(attribute_name_s::attribute_name), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_attribute_name_s_names[] = {{"attributeName", nullptr}};
const asn1::table::seq_desc          attribute_name_s::desc             = {
    {seq_table_attribute_name_s_fields, uint64_t{0x1}, nullptr, seq_table_attribute_name_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// RANConfigurationStructureForADF ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_cfg_structure_for_adf_s_fields[] = {
    {offsetof(ran_cfg_structure_for_adf_s, report_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(ran_cfg_structure_for_adf_s::report_type)>(),
     decltype(ran_cfg_structure_for_adf_s::report_type)::has_ext,
     false,
     decltype(ran_cfg_structure_for_adf_s::report_type)::nof_types,
     decltype(ran_cfg_structure_for_adf_s::report_type)::nof_exts,
     nullptr,
     &ran_cfg_structure_for_adf_s::report_type_opts::desc},
    {offsetof(ran_cfg_structure_for_adf_s, ran_cfg_structure_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_cfg_structure_for_adf_s::ran_cfg_structure_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_cfg_structure_for_adf_s, list_of_attributes),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(ran_cfg_structure_for_adf_s::list_of_attributes),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &attribute_name_s::desc}};
static const asn1::table::field_name seq_table_ran_cfg_structure_for_adf_s_names[] = {
    {"reportType", nullptr},
    {"ranConfigurationStructureName", nullptr},
    {"listOfAttributes", nullptr}};
static const asn1::table::seq_opt seq_table_ran_cfg_structure_for_adf_s_opts[] = {
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_cfg_structure_for_adf_s::desc = {{seq_table_ran_cfg_structure_for_adf_s_fields,
                                                                  uint64_t{0x3},
                                                                  seq_table_ran_cfg_structure_for_adf_s_opts,
                                                                  seq_table_ran_cfg_structure_for_adf_s_names,
                                                                  3,
                                                                  1},
                                                                 false,
                                                                 true,
                                                                 nullptr,
                                                                 0};

static const char*           enum_names_ran_cfg_structure_for_adf_s_report_type_e__values[] = {"all", "change"};
const asn1::table::enum_desc ran_cfg_structure_for_adf_s::report_type_opts::desc            = {
    enum_names_ran_cfg_structure_for_adf_s_report_type_e__values,
    2,
    "ran_cfg_structure_for_adf_s::report_type_e_"};

// E2SM-CCC-ActionDefinitionFormat1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_action_definition_format1_s_fields[] = {
    {offsetof(e2sm_ccc_action_definition_format1_s, list_of_node_level_ran_cfg_structures_for_adf),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<
         decltype(e2sm_ccc_action_definition_format1_s::list_of_node_level_ran_cfg_structures_for_adf),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &ran_cfg_structure_for_adf_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_action_definition_format1_s_names[] = {
    {"listOfNodeLevelRANConfigurationStructuresForADF", nullptr}};
const asn1::table::seq_desc e2sm_ccc_action_definition_format1_s::desc = {
    {seq_table_e2sm_ccc_action_definition_format1_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_ccc_action_definition_format1_s_names,
     1,
     0},
    false,
    true,
    nullptr,
    0};

// PlmnId ::= SEQUENCE
static const asn1::table::seq_field seq_table_plmn_id_s_fields[] = {
    {offsetof(plmn_id_s, mcc),
     (int32_t)offsetof(plmn_id_s, mcc_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(plmn_id_s::mcc), asn1::table::string_json>::value},
    {offsetof(plmn_id_s, mnc),
     (int32_t)offsetof(plmn_id_s, mnc_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(plmn_id_s::mnc), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_plmn_id_s_names[] = {{"mcc", nullptr}, {"mnc", nullptr}};
static const asn1::table::seq_opt    seq_table_plmn_id_s_opts[]  = {{0, (int32_t)offsetof(plmn_id_s, mcc_present)},
                                                                    {1, (int32_t)offsetof(plmn_id_s, mnc_present)}};
const asn1::table::seq_desc          plmn_id_s::desc             = {
    {seq_table_plmn_id_s_fields, uint64_t{0x0}, seq_table_plmn_id_s_opts, seq_table_plmn_id_s_names, 2, 2},
    false,
    true,
    nullptr,
    0};

// NR-CGI ::= SEQUENCE
static const asn1::table::seq_field seq_table_nr_cgi_s_fields[] = {
    {offsetof(nr_cgi_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc},
    {offsetof(nr_cgi_s, nr_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nr_cgi_s::nr_cell_id), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_nr_cgi_s_names[] = {{"plmnIdentity", nullptr},
                                                                   {"nRCellIdentity", nullptr}};
const asn1::table::seq_desc          nr_cgi_s::desc =
    {{seq_table_nr_cgi_s_fields, uint64_t{0x3}, nullptr, seq_table_nr_cgi_s_names, 2, 0}, false, true, nullptr, 0};

// EUTRA-CGI ::= SEQUENCE
static const asn1::table::seq_field seq_table_eutra_cgi_s_fields[] = {
    {offsetof(eutra_cgi_s, plmn_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc},
    {offsetof(eutra_cgi_s, eutra_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(eutra_cgi_s::eutra_cell_id), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_eutra_cgi_s_names[] = {{"plmnIdentity", nullptr},
                                                                      {"eUTRACellIdentity", nullptr}};
const asn1::table::seq_desc          eutra_cgi_s::desc             = {
    {seq_table_eutra_cgi_s_fields, uint64_t{0x3}, nullptr, seq_table_eutra_cgi_s_names, 2, 0},
    false,
    true,
    nullptr,
    0};

// CellGlobalId ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_cell_global_id_c_alternatives[] = {
    &asn1::table::alternative_ops<nr_cgi_s, false, false>::value,
    &asn1::table::alternative_ops<eutra_cgi_s, false, false>::value};
const asn1::table::choice_storage_desc cell_global_id_c::storage_desc = {choice_storage_cell_global_id_c_alternatives,
                                                                         2,
                                                                         "cell_global_id_c"};

nr_cgi_s& cell_global_id_c::set_nr_cgi()
{
  set(types::nr_cgi);
  return c.get<nr_cgi_s>();
}
eutra_cgi_s& cell_global_id_c::set_eutra_cgi()
{
  set(types::eutra_cgi);
  return c.get<eutra_cgi_s>();
}
static const asn1::table::seq_field  choice_table_cell_global_id_c_alternatives[] = {{0,
                                                                                      asn1::table::seq_field::mandatory,
                                                                                      asn1::table::field_op::sequence,
                                                                                      false,
                                                                                      false,
                                                                                      0,
                                                                                      0,
                                                                                      nullptr,
                                                                                      &nr_cgi_s::desc},
                                                                                     {0,
                                                                                      asn1::table::seq_field::mandatory,
                                                                                      asn1::table::field_op::sequence,
                                                                                      false,
                                                                                      false,
                                                                                      0,
                                                                                      0,
                                                                                      nullptr,
                                                                                      &eutra_cgi_s::desc}};
static const asn1::table::field_name choice_table_cell_global_id_c_names[]        = {{"nR-CGI", nullptr},
                                                                                     {"eUTRA-CGI", nullptr}};
const asn1::table::choice_desc       cell_global_id_c::desc = {asn1::table::enum_int_type<cell_global_id_c::types>(),
                                                               cell_global_id_c::types::nof_types,
                                                               cell_global_id_c::types::nof_exts,
                                                               cell_global_id_c::types::has_ext,
                                                               true,
                                                               choice_table_cell_global_id_c_alternatives,
                                                               nullptr,
                                                               choice_table_cell_global_id_c_names,
                                                               "cell_global_id_c",
                                                               offsetof(cell_global_id_c, c.type),
                                                               offsetof(cell_global_id_c, c.value),
                                                               false,
                                                               &cell_global_id_c::storage_desc};

static const char*           enum_names_cell_global_id_c_types_values[] = {"nR-CGI", "eUTRA-CGI"};
const asn1::table::enum_desc cell_global_id_c::types_opts::desc         = {enum_names_cell_global_id_c_types_values,
                                                                           2,
                                                                           "cell_global_id_c::types"};

// CellConfigurationToBeReportedForADF ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_cfg_to_be_reported_for_adf_s_fields[] = {
    {offsetof(cell_cfg_to_be_reported_for_adf_s, cell_global_id),
     (int32_t)offsetof(cell_cfg_to_be_reported_for_adf_s, cell_global_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_global_id_c::desc},
    {offsetof(cell_cfg_to_be_reported_for_adf_s, list_of_cell_level_ran_cfg_structures_for_adf),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<
         decltype(cell_cfg_to_be_reported_for_adf_s::list_of_cell_level_ran_cfg_structures_for_adf),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &ran_cfg_structure_for_adf_s::desc}};
static const asn1::table::field_name seq_table_cell_cfg_to_be_reported_for_adf_s_names[] = {
    {"cellGlobalId", nullptr},
    {"listOfCellLevelRANConfigurationStructuresForADF", nullptr}};
static const asn1::table::seq_opt seq_table_cell_cfg_to_be_reported_for_adf_s_opts[] = {
    {0, (int32_t)offsetof(cell_cfg_to_be_reported_for_adf_s, cell_global_id_present)},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc cell_cfg_to_be_reported_for_adf_s::desc = {
    {seq_table_cell_cfg_to_be_reported_for_adf_s_fields,
     uint64_t{0x0},
     seq_table_cell_cfg_to_be_reported_for_adf_s_opts,
     seq_table_cell_cfg_to_be_reported_for_adf_s_names,
     2,
     2},
    false,
    true,
    nullptr,
    0};

// E2SM-CCC-ActionDefinitionFormat2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_action_definition_format2_s_fields[] = {
    {offsetof(e2sm_ccc_action_definition_format2_s, list_of_cell_cfgs_to_be_reported_for_adf),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<
         decltype(e2sm_ccc_action_definition_format2_s::list_of_cell_cfgs_to_be_reported_for_adf),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &cell_cfg_to_be_reported_for_adf_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_action_definition_format2_s_names[] = {
    {"listOfCellConfigurationsToBeReportedForADF", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_ccc_action_definition_format2_s_opts[] = {
    {0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc e2sm_ccc_action_definition_format2_s::desc = {
    {seq_table_e2sm_ccc_action_definition_format2_s_fields,
     uint64_t{0x0},
     seq_table_e2sm_ccc_action_definition_format2_s_opts,
     seq_table_e2sm_ccc_action_definition_format2_s_names,
     1,
     1},
    false,
    true,
    nullptr,
    0};

// ActionDefinitionFormat ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_action_definition_format_c_alternatives[] = {
    &asn1::table::alternative_ops<e2sm_ccc_action_definition_format1_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_ccc_action_definition_format2_s, false, false>::value};
const asn1::table::choice_storage_desc action_definition_format_c::storage_desc = {
    choice_storage_action_definition_format_c_alternatives,
    2,
    "action_definition_format_c"};

e2sm_ccc_action_definition_format1_s& action_definition_format_c::set_action_definition_format1()
{
  set(types::action_definition_format1);
  return c.get<e2sm_ccc_action_definition_format1_s>();
}
e2sm_ccc_action_definition_format2_s& action_definition_format_c::set_action_definition_format2()
{
  set(types::action_definition_format2);
  return c.get<e2sm_ccc_action_definition_format2_s>();
}
static const asn1::table::seq_field choice_table_action_definition_format_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_action_definition_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_action_definition_format2_s::desc}};
static const asn1::table::field_name choice_table_action_definition_format_c_names[] = {
    {"actionDefinition-Format1", nullptr},
    {"actionDefinition-Format2", nullptr}};
const asn1::table::choice_desc action_definition_format_c::desc = {
    asn1::table::enum_int_type<action_definition_format_c::types>(),
    action_definition_format_c::types::nof_types,
    action_definition_format_c::types::nof_exts,
    action_definition_format_c::types::has_ext,
    true,
    choice_table_action_definition_format_c_alternatives,
    nullptr,
    choice_table_action_definition_format_c_names,
    "action_definition_format_c",
    offsetof(action_definition_format_c, c.type),
    offsetof(action_definition_format_c, c.value),
    false,
    &action_definition_format_c::storage_desc};

static const char*           enum_names_action_definition_format_c_types_values[] = {"actionDefinition-Format1",
                                                                                     "actionDefinition-Format2"};
const asn1::table::enum_desc action_definition_format_c::types_opts::desc         = {
    enum_names_action_definition_format_c_types_values,
    2,
    "action_definition_format_c::types"};
uint8_t action_definition_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "action_definition_format_c::types");
}

// AdministrativeState ::= ENUMERATED
static const char*           enum_names_administrative_state_e_values[] = {"LOCKED", "SHUTTINGDOWN", "UNLOCKED"};
const asn1::table::enum_desc administrative_state_opts::desc            = {enum_names_administrative_state_e_values,
                                                                           3,
                                                                           "administrative_state_e"};

// SleepMode ::= SEQUENCE
static const asn1::table::seq_field seq_table_sleep_mode_s_fields[] = {
    {offsetof(sleep_mode_s, sleep_mode_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(sleep_mode_s::sleep_mode_type)>(),
     decltype(sleep_mode_s::sleep_mode_type)::has_ext,
     false,
     decltype(sleep_mode_s::sleep_mode_type)::nof_types,
     decltype(sleep_mode_s::sleep_mode_type)::nof_exts,
     nullptr,
     &sleep_mode_s::sleep_mode_type_opts::desc},
    {offsetof(sleep_mode_s, wakeup_dur),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(sleep_mode_s, wakeup_dur_guaranteed),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_sleep_mode_s_names[] = {{"sleepModeType", nullptr},
                                                                       {"wakeupDuration", nullptr},
                                                                       {"wakeupDurationGuaranteed", nullptr}};
const asn1::table::seq_desc          sleep_mode_s::desc             = {
    {seq_table_sleep_mode_s_fields, uint64_t{0x7}, nullptr, seq_table_sleep_mode_s_names, 3, 0},
    false,
    true,
    nullptr,
    0};

static const char*           enum_names_sleep_mode_s_sleep_mode_type_e__values[] = {"SLEEP_MODE0",
                                                                                    "SLEEP_MODE1",
                                                                                    "SLEEP_MODE2",
                                                                                    "SLEEP_MODE3"};
const asn1::table::enum_desc sleep_mode_s::sleep_mode_type_opts::desc            = {
    enum_names_sleep_mode_s_sleep_mode_type_e__values,
    4,
    "sleep_mode_s::sleep_mode_type_e_"};
uint8_t sleep_mode_s::sleep_mode_type_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3};
  return map_enum_number(numbers, 4, value, "sleep_mode_s::sleep_mode_type_e_");
}

// AsmCapabilityInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_asm_cap_info_s_fields[] = {
    {offsetof(asm_cap_info_s, sleep_modes),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(asm_cap_info_s::sleep_modes), asn1::table::sequence_item_ops, false, true>::
         value,
     &sleep_mode_s::desc},
    {offsetof(asm_cap_info_s, defined_dur_sleep_supported),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(asm_cap_info_s, undefined_dur_sleep_supported),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_asm_cap_info_s_names[] = {{"sleepModes", nullptr},
                                                                         {"definedDurationSleepSupported", nullptr},
                                                                         {"undefinedDurationSleepSupported", nullptr}};
const asn1::table::seq_desc          asm_cap_info_s::desc             = {
    {seq_table_asm_cap_info_s_fields, uint64_t{0x7}, nullptr, seq_table_asm_cap_info_s_names, 3, 0},
    false,
    true,
    nullptr,
    0};

// EventTriggerStyle ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_style_s_fields[] = {
    {offsetof(event_trigger_style_s, event_trigger_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(event_trigger_style_s, event_trigger_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(event_trigger_style_s::event_trigger_style_name),
                                asn1::table::string_json>::value},
    {offsetof(event_trigger_style_s, event_trigger_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_event_trigger_style_s_names[] = {{"eventTriggerStyleType", nullptr},
                                                                                {"eventTriggerStyleName", nullptr},
                                                                                {"eventTriggerFormatType", nullptr}};
const asn1::table::seq_desc          event_trigger_style_s::desc             = {
    {seq_table_event_trigger_style_s_fields, uint64_t{0x7}, nullptr, seq_table_event_trigger_style_s_names, 3, 0},
    false,
    true,
    nullptr,
    0};

// EventTrigger ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_s_fields[] = {
    {offsetof(event_trigger_s, list_of_supported_event_trigger_styles),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(event_trigger_s::list_of_supported_event_trigger_styles),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &event_trigger_style_s::desc}};
static const asn1::table::field_name seq_table_event_trigger_s_names[] = {
    {"listOfSupportedEventTriggerStyles", nullptr}};
const asn1::table::seq_desc event_trigger_s::desc = {
    {seq_table_event_trigger_s_fields, uint64_t{0x1}, nullptr, seq_table_event_trigger_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// EventTriggerStyleType ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_style_type_s_fields[] = {
    {offsetof(event_trigger_style_type_s, event_trigger_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_event_trigger_style_type_s_names[] = {
    {"eventTriggerStyleType", nullptr}};
const asn1::table::seq_desc event_trigger_style_type_s::desc = {{seq_table_event_trigger_style_type_s_fields,
                                                                 uint64_t{0x1},
                                                                 nullptr,
                                                                 seq_table_event_trigger_style_type_s_names,
                                                                 1,
                                                                 0},
                                                                false,
                                                                true,
                                                                nullptr,
                                                                0};

// ReportStyle ::= SEQUENCE
static const asn1::table::seq_field seq_table_report_style_s_fields[] = {
    {offsetof(report_style_s, report_service_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(report_style_s, report_service_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(report_style_s::report_service_style_name), asn1::table::string_json>::value},
    {offsetof(report_style_s, list_of_supported_event_trigger_styles_for_report_style),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(report_style_s::list_of_supported_event_trigger_styles_for_report_style),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &event_trigger_style_type_s::desc},
    {offsetof(report_style_s, report_service_action_definition_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(report_style_s, report_service_ind_hdr_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(report_style_s, report_service_ind_msg_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_report_style_s_names[] = {
    {"reportServiceStyleType", nullptr},
    {"reportServiceStyleName", nullptr},
    {"listOfSupportedEventTriggerStylesForReportStyle", nullptr},
    {"reportServiceActionDefinitionFormatType", nullptr},
    {"reportServiceIndicationHeaderFormatType", nullptr},
    {"reportServiceIndicationMessageFormatType", nullptr}};
const asn1::table::seq_desc report_style_s::desc = {
    {seq_table_report_style_s_fields, uint64_t{0x3f}, nullptr, seq_table_report_style_s_names, 6, 0},
    false,
    true,
    nullptr,
    0};

// ReportService ::= SEQUENCE
static const asn1::table::seq_field seq_table_report_service_s_fields[] = {
    {offsetof(report_service_s, list_of_supported_report_styles),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(report_service_s::list_of_supported_report_styles),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &report_style_s::desc}};
static const asn1::table::field_name seq_table_report_service_s_names[] = {{"listOfSupportedReportStyles", nullptr}};
const asn1::table::seq_desc          report_service_s::desc             = {
    {seq_table_report_service_s_fields, uint64_t{0x1}, nullptr, seq_table_report_service_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// ControlStyle ::= SEQUENCE
static const asn1::table::seq_field seq_table_ctrl_style_s_fields[] = {
    {offsetof(ctrl_style_s, ctrl_service_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ctrl_style_s, ctrl_service_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ctrl_style_s::ctrl_service_style_name), asn1::table::string_json>::value},
    {offsetof(ctrl_style_s, ctrl_service_hdr_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ctrl_style_s, ctrl_service_msg_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ctrl_style_s, ric_call_process_id_format_type),
     (int32_t)offsetof(ctrl_style_s, ric_call_process_id_format_type_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ctrl_style_s, ctrl_service_ctrl_outcome_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_ctrl_style_s_names[] = {
    {"controlServiceStyleType", nullptr},
    {"controlServiceStyleName", nullptr},
    {"controlServiceHeaderFormatType", nullptr},
    {"controlServiceMessageFormatType", nullptr},
    {"ricCallProcessIDFormatType", nullptr},
    {"controlServiceControlOutcomeFormatType", nullptr}};
static const asn1::table::seq_opt seq_table_ctrl_style_s_opts[] = {
    {4, (int32_t)offsetof(ctrl_style_s, ric_call_process_id_format_type_present)}};
const asn1::table::seq_desc ctrl_style_s::desc = {
    {seq_table_ctrl_style_s_fields, uint64_t{0x2f}, seq_table_ctrl_style_s_opts, seq_table_ctrl_style_s_names, 6, 1},
    false,
    true,
    nullptr,
    0};

// ControlService ::= SEQUENCE
static const asn1::table::seq_field seq_table_ctrl_service_s_fields[] = {
    {offsetof(ctrl_service_s, list_of_supported_ctrl_styles),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(ctrl_service_s::list_of_supported_ctrl_styles),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ctrl_style_s::desc}};
static const asn1::table::field_name seq_table_ctrl_service_s_names[] = {{"listOfSupportedControlStyles", nullptr}};
const asn1::table::seq_desc          ctrl_service_s::desc             = {
    {seq_table_ctrl_service_s_fields, uint64_t{0x1}, nullptr, seq_table_ctrl_service_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// QueryStyle ::= SEQUENCE
static const asn1::table::seq_field seq_table_query_style_s_fields[] = {
    {offsetof(query_style_s, query_service_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(query_style_s, query_service_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(query_style_s::query_service_style_name), asn1::table::string_json>::value},
    {offsetof(query_style_s, query_service_hdr_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(query_style_s, query_service_definition_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(query_style_s, query_service_outcome_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_query_style_s_names[] = {{"queryServiceStyleType", nullptr},
                                                                        {"queryServiceStyleName", nullptr},
                                                                        {"queryServiceHeaderFormatType", nullptr},
                                                                        {"queryServiceDefinitionFormatType", nullptr},
                                                                        {"queryServiceOutcomeFormatType", nullptr}};
const asn1::table::seq_desc          query_style_s::desc             = {
    {seq_table_query_style_s_fields, uint64_t{0x1f}, nullptr, seq_table_query_style_s_names, 5, 0},
    false,
    true,
    nullptr,
    0};

// QueryService ::= SEQUENCE
static const asn1::table::seq_field seq_table_query_service_s_fields[] = {
    {offsetof(query_service_s, list_of_supported_query_styles),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(query_service_s::list_of_supported_query_styles),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &query_style_s::desc}};
static const asn1::table::field_name seq_table_query_service_s_names[] = {{"listOfSupportedQueryStyles", nullptr}};
const asn1::table::seq_desc          query_service_s::desc             = {
    {seq_table_query_service_s_fields, uint64_t{0x1}, nullptr, seq_table_query_service_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// RICServices ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_services_s_fields[] = {
    {offsetof(ric_services_s, event_trigger),
     (int32_t)offsetof(ric_services_s, event_trigger_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_s::desc},
    {offsetof(ric_services_s, report_service),
     (int32_t)offsetof(ric_services_s, report_service_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &report_service_s::desc},
    {offsetof(ric_services_s, insert_service),
     (int32_t)offsetof(ric_services_s, insert_service_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ric_services_s, ctrl_service),
     (int32_t)offsetof(ric_services_s, ctrl_service_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ctrl_service_s::desc},
    {offsetof(ric_services_s, policy_service),
     (int32_t)offsetof(ric_services_s, policy_service_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ric_services_s, query_service),
     (int32_t)offsetof(ric_services_s, query_service_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &query_service_s::desc}};
static const asn1::table::field_name seq_table_ric_services_s_names[] = {{"eventTrigger", nullptr},
                                                                         {"reportService", nullptr},
                                                                         {"insertService", nullptr},
                                                                         {"controlService", nullptr},
                                                                         {"policyService", nullptr},
                                                                         {"queryService", nullptr}};
static const asn1::table::seq_opt    seq_table_ric_services_s_opts[]  = {
    {0, (int32_t)offsetof(ric_services_s, event_trigger_present)},
    {1, (int32_t)offsetof(ric_services_s, report_service_present)},
    {2, (int32_t)offsetof(ric_services_s, insert_service_present)},
    {3, (int32_t)offsetof(ric_services_s, ctrl_service_present)},
    {4, (int32_t)offsetof(ric_services_s, policy_service_present)},
    {5, (int32_t)offsetof(ric_services_s, query_service_present)}};
const asn1::table::seq_desc ric_services_s::desc = {{seq_table_ric_services_s_fields,
                                                     uint64_t{0x0},
                                                     seq_table_ric_services_s_opts,
                                                     seq_table_ric_services_s_names,
                                                     6,
                                                     6},
                                                    false,
                                                    true,
                                                    nullptr,
                                                    0};

// Attribute ::= SEQUENCE
static const asn1::table::seq_field seq_table_attribute_s_fields[] = {
    {offsetof(attribute_s, attribute_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(attribute_s::attribute_name), asn1::table::string_json>::value},
    {offsetof(attribute_s, supported_services),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_services_s::desc}};
static const asn1::table::field_name seq_table_attribute_s_names[] = {{"attributeName", nullptr},
                                                                      {"supportedServices", nullptr}};
const asn1::table::seq_desc          attribute_s::desc             = {
    {seq_table_attribute_s_fields, uint64_t{0x3}, nullptr, seq_table_attribute_s_names, 2, 0},
    false,
    true,
    nullptr,
    0};

// BwpContext ::= ENUMERATED
static const char*           enum_names_bwp_context_e_values[] = {"DL", "UL", "SUL"};
const asn1::table::enum_desc bwp_context_opts::desc            = {enum_names_bwp_context_e_values, 3, "bwp_context_e"};

// Cause ::= ENUMERATED
static const char* enum_names_cause_e_values[] =
    {"NotSupported", "NotAvailable", "IncompatibleState", "JsonError", "SemanticError", "Unspecified"};
const asn1::table::enum_desc cause_opts::desc = {enum_names_cause_e_values, 6, "cause_e"};

// O-GnbCuCpFunction ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_gnb_cu_cp_function_s_fields[] = {
    {offsetof(o_gnb_cu_cp_function_s, gnb_id),
     (int32_t)offsetof(o_gnb_cu_cp_function_s, gnb_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(o_gnb_cu_cp_function_s, gnb_id_len),
     (int32_t)offsetof(o_gnb_cu_cp_function_s, gnb_id_len_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{22},
     uint64_t{10},
     nullptr},
    {offsetof(o_gnb_cu_cp_function_s, gnb_cu_name),
     (int32_t)offsetof(o_gnb_cu_cp_function_s, gnb_cu_name_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(o_gnb_cu_cp_function_s::gnb_cu_name), asn1::table::string_json>::value},
    {offsetof(o_gnb_cu_cp_function_s, plmn_id),
     (int32_t)offsetof(o_gnb_cu_cp_function_s, plmn_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc},
    {offsetof(o_gnb_cu_cp_function_s, x2_block_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::list_codec<decltype(o_gnb_cu_cp_function_s::x2_block_list), asn1::table::string_json>::value},
    {offsetof(o_gnb_cu_cp_function_s, xn_block_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::list_codec<decltype(o_gnb_cu_cp_function_s::xn_block_list), asn1::table::string_json>::value},
    {offsetof(o_gnb_cu_cp_function_s, x2_allow_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::list_codec<decltype(o_gnb_cu_cp_function_s::x2_allow_list), asn1::table::string_json>::value},
    {offsetof(o_gnb_cu_cp_function_s, xn_allow_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::list_codec<decltype(o_gnb_cu_cp_function_s::xn_allow_list), asn1::table::string_json>::value},
    {offsetof(o_gnb_cu_cp_function_s, x2_ho_block_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::list_codec<decltype(o_gnb_cu_cp_function_s::x2_ho_block_list), asn1::table::string_json>::value},
    {offsetof(o_gnb_cu_cp_function_s, xn_ho_block_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::list_codec<decltype(o_gnb_cu_cp_function_s::xn_ho_block_list), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_o_gnb_cu_cp_function_s_names[] = {{"gnbId", nullptr},
                                                                                 {"gnbIdLength", nullptr},
                                                                                 {"gnbCuName", nullptr},
                                                                                 {"plmnId", nullptr},
                                                                                 {"x2BlockList", nullptr},
                                                                                 {"xnBlockList", nullptr},
                                                                                 {"x2AllowList", nullptr},
                                                                                 {"xnAllowList", nullptr},
                                                                                 {"x2HOBlockList", nullptr},
                                                                                 {"xnHOBlockList", nullptr}};
static const asn1::table::seq_opt    seq_table_o_gnb_cu_cp_function_s_opts[]  = {
    {0, (int32_t)offsetof(o_gnb_cu_cp_function_s, gnb_id_present)},
    {1, (int32_t)offsetof(o_gnb_cu_cp_function_s, gnb_id_len_present)},
    {2, (int32_t)offsetof(o_gnb_cu_cp_function_s, gnb_cu_name_present)},
    {3, (int32_t)offsetof(o_gnb_cu_cp_function_s, plmn_id_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, asn1::table::seq_field::list_presence},
    {6, asn1::table::seq_field::list_presence},
    {7, asn1::table::seq_field::list_presence},
    {8, asn1::table::seq_field::list_presence},
    {9, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc o_gnb_cu_cp_function_s::desc = {{seq_table_o_gnb_cu_cp_function_s_fields,
                                                             uint64_t{0x0},
                                                             seq_table_o_gnb_cu_cp_function_s_opts,
                                                             seq_table_o_gnb_cu_cp_function_s_names,
                                                             10,
                                                             10},
                                                            false,
                                                            true,
                                                            nullptr,
                                                            0};

// Snssai ::= SEQUENCE
static const asn1::table::seq_field seq_table_snssai_s_fields[] = {
    {offsetof(snssai_s, sst),
     (int32_t)offsetof(snssai_s, sst_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(snssai_s, sd),
     (int32_t)offsetof(snssai_s, sd_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(snssai_s::sd), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_snssai_s_names[] = {{"sst", nullptr}, {"sd", nullptr}};
static const asn1::table::seq_opt    seq_table_snssai_s_opts[]  = {{0, (int32_t)offsetof(snssai_s, sst_present)},
                                                                   {1, (int32_t)offsetof(snssai_s, sd_present)}};
const asn1::table::seq_desc          snssai_s::desc             = {
    {seq_table_snssai_s_fields, uint64_t{0x0}, seq_table_snssai_s_opts, seq_table_snssai_s_names, 2, 2},
    false,
    true,
    nullptr,
    0};

// PlmnInfo ::= SEQUENCE
static const asn1::table::seq_field  seq_table_plmn_info_s_fields[] = {{offsetof(plmn_info_s, plmn_id),
                                                                        (int32_t)offsetof(plmn_info_s, plmn_id_present),
                                                                        asn1::table::field_op::sequence,
                                                                        false,
                                                                        false,
                                                                        0,
                                                                        0,
                                                                        nullptr,
                                                                        &plmn_id_s::desc},
                                                                       {offsetof(plmn_info_s, snssai),
                                                                        (int32_t)offsetof(plmn_info_s, snssai_present),
                                                                        asn1::table::field_op::sequence,
                                                                        false,
                                                                        false,
                                                                        0,
                                                                        0,
                                                                        nullptr,
                                                                        &snssai_s::desc}};
static const asn1::table::field_name seq_table_plmn_info_s_names[]  = {{"plmnId", nullptr}, {"snssai", nullptr}};
static const asn1::table::seq_opt seq_table_plmn_info_s_opts[] = {{0, (int32_t)offsetof(plmn_info_s, plmn_id_present)},
                                                                  {1, (int32_t)offsetof(plmn_info_s, snssai_present)}};
const asn1::table::seq_desc       plmn_info_s::desc            = {
    {seq_table_plmn_info_s_fields, uint64_t{0x0}, seq_table_plmn_info_s_opts, seq_table_plmn_info_s_names, 2, 2},
    false,
    true,
    nullptr,
    0};

// O-GnbCuUpFunction ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_gnb_cu_up_function_s_fields[] = {
    {offsetof(o_gnb_cu_up_function_s, gnb_id),
     (int32_t)offsetof(o_gnb_cu_up_function_s, gnb_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(o_gnb_cu_up_function_s, gnb_id_len),
     (int32_t)offsetof(o_gnb_cu_up_function_s, gnb_id_len_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{22},
     uint64_t{10},
     nullptr},
    {offsetof(o_gnb_cu_up_function_s, gnb_cu_up_id),
     (int32_t)offsetof(o_gnb_cu_up_function_s, gnb_cu_up_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr},
    {offsetof(o_gnb_cu_up_function_s, plmn_info_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(o_gnb_cu_up_function_s::plmn_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &plmn_info_s::desc}};
static const asn1::table::field_name seq_table_o_gnb_cu_up_function_s_names[] = {{"gnbId", nullptr},
                                                                                 {"gnbIdLength", nullptr},
                                                                                 {"gnbCuUpId", nullptr},
                                                                                 {"plmnInfoList", nullptr}};
static const asn1::table::seq_opt    seq_table_o_gnb_cu_up_function_s_opts[]  = {
    {0, (int32_t)offsetof(o_gnb_cu_up_function_s, gnb_id_present)},
    {1, (int32_t)offsetof(o_gnb_cu_up_function_s, gnb_id_len_present)},
    {2, (int32_t)offsetof(o_gnb_cu_up_function_s, gnb_cu_up_id_present)},
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc o_gnb_cu_up_function_s::desc = {{seq_table_o_gnb_cu_up_function_s_fields,
                                                             uint64_t{0x0},
                                                             seq_table_o_gnb_cu_up_function_s_opts,
                                                             seq_table_o_gnb_cu_up_function_s_names,
                                                             4,
                                                             4},
                                                            false,
                                                            true,
                                                            nullptr,
                                                            0};

// O-GnbDuFunction ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_gnb_du_function_s_fields[] = {
    {offsetof(o_gnb_du_function_s, gnb_du_id),
     (int32_t)offsetof(o_gnb_du_function_s, gnb_du_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{68719476735},
     nullptr},
    {offsetof(o_gnb_du_function_s, gnb_du_name),
     (int32_t)offsetof(o_gnb_du_function_s, gnb_du_name_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(o_gnb_du_function_s::gnb_du_name), asn1::table::string_json>::value},
    {offsetof(o_gnb_du_function_s, gnb_id),
     (int32_t)offsetof(o_gnb_du_function_s, gnb_id_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(o_gnb_du_function_s, gnb_id_len),
     (int32_t)offsetof(o_gnb_du_function_s, gnb_id_len_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{22},
     uint64_t{10},
     nullptr}};
static const asn1::table::field_name seq_table_o_gnb_du_function_s_names[] = {{"gnbDuId", nullptr},
                                                                              {"gnbDuName", nullptr},
                                                                              {"gnbId", nullptr},
                                                                              {"gnbIdLength", nullptr}};
static const asn1::table::seq_opt    seq_table_o_gnb_du_function_s_opts[]  = {
    {0, (int32_t)offsetof(o_gnb_du_function_s, gnb_du_id_present)},
    {1, (int32_t)offsetof(o_gnb_du_function_s, gnb_du_name_present)},
    {2, (int32_t)offsetof(o_gnb_du_function_s, gnb_id_present)},
    {3, (int32_t)offsetof(o_gnb_du_function_s, gnb_id_len_present)}};
const asn1::table::seq_desc o_gnb_du_function_s::desc = {{seq_table_o_gnb_du_function_s_fields,
                                                          uint64_t{0x0},
                                                          seq_table_o_gnb_du_function_s_opts,
                                                          seq_table_o_gnb_du_function_s_names,
                                                          4,
                                                          4},
                                                         false,
                                                         true,
                                                         nullptr,
                                                         0};

// O-NrCellCu ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_nr_cell_cu_s_fields[] = {
    {offsetof(o_nr_cell_cu_s, cell_local_id),
     (int32_t)offsetof(o_nr_cell_cu_s, cell_local_id_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_nr_cell_cu_s, plmn_info_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::
         item_list_codec<decltype(o_nr_cell_cu_s::plmn_info_list), asn1::table::sequence_item_ops, false, true>::value,
     &plmn_info_s::desc}};
static const asn1::table::field_name seq_table_o_nr_cell_cu_s_names[] = {{"cellLocalId", nullptr},
                                                                         {"plmnInfoList", nullptr}};
static const asn1::table::seq_opt    seq_table_o_nr_cell_cu_s_opts[]  = {
    {0, (int32_t)offsetof(o_nr_cell_cu_s, cell_local_id_present)},
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc o_nr_cell_cu_s::desc = {{seq_table_o_nr_cell_cu_s_fields,
                                                     uint64_t{0x0},
                                                     seq_table_o_nr_cell_cu_s_opts,
                                                     seq_table_o_nr_cell_cu_s_names,
                                                     2,
                                                     2},
                                                    false,
                                                    true,
                                                    nullptr,
                                                    0};

// OperationalState ::= ENUMERATED
static const char*           enum_names_operational_state_e_values[] = {"ENABLED", "DISABLED"};
const asn1::table::enum_desc operational_state_opts::desc            = {enum_names_operational_state_e_values,
                                                                        2,
                                                                        "operational_state_e"};

// CellState ::= ENUMERATED
static const char*           enum_names_cell_state_e_values[] = {"IDLE", "INACTIVE", "ACTIVE"};
const asn1::table::enum_desc cell_state_opts::desc            = {enum_names_cell_state_e_values, 3, "cell_state_e"};

// IsInitialBwp ::= ENUMERATED
static const char*           enum_names_is_init_bwp_e_values[] = {"INITIAL", "OTHER", "SUL"};
const asn1::table::enum_desc is_init_bwp_opts::desc            = {enum_names_is_init_bwp_e_values, 3, "is_init_bwp_e"};

// CyclicPrefix ::= ENUMERATED
static const char*           enum_names_cp_e_values[] = {"15", "30", "60", "120"};
const asn1::table::enum_desc cp_opts::desc            = {enum_names_cp_e_values, 4, "cp_e"};
uint8_t                      cp_opts::to_number() const
{
  static const uint8_t numbers[] = {15, 30, 60, 120};
  return map_enum_number(numbers, 4, value, "cp_e");
}

// O-Bwp ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_bwp_s_fields[] = {
    {offsetof(o_bwp_s, bwp_context),
     (int32_t)offsetof(o_bwp_s, bwp_context_present),
     asn1::table::enum_op<decltype(o_bwp_s::bwp_context)>(),
     decltype(o_bwp_s::bwp_context)::has_ext,
     false,
     decltype(o_bwp_s::bwp_context)::nof_types,
     decltype(o_bwp_s::bwp_context)::nof_exts,
     nullptr,
     &bwp_context_opts::desc},
    {offsetof(o_bwp_s, is_init_bwp),
     (int32_t)offsetof(o_bwp_s, is_init_bwp_present),
     asn1::table::enum_op<decltype(o_bwp_s::is_init_bwp)>(),
     decltype(o_bwp_s::is_init_bwp)::has_ext,
     false,
     decltype(o_bwp_s::is_init_bwp)::nof_types,
     decltype(o_bwp_s::is_init_bwp)::nof_exts,
     nullptr,
     &is_init_bwp_opts::desc},
    {offsetof(o_bwp_s, sub_carrier_spacing),
     (int32_t)offsetof(o_bwp_s, sub_carrier_spacing_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_bwp_s, cp),
     (int32_t)offsetof(o_bwp_s, cp_present),
     asn1::table::enum_op<decltype(o_bwp_s::cp)>(),
     decltype(o_bwp_s::cp)::has_ext,
     false,
     decltype(o_bwp_s::cp)::nof_types,
     decltype(o_bwp_s::cp)::nof_exts,
     nullptr,
     &cp_opts::desc},
    {offsetof(o_bwp_s, start_rb),
     (int32_t)offsetof(o_bwp_s, start_rb_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_bwp_s, nof_rbs),
     (int32_t)offsetof(o_bwp_s, nof_rbs_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_o_bwp_s_names[] = {{"bwpContext", nullptr},
                                                                  {"isInitialBwp", nullptr},
                                                                  {"subCarrierSpacing", nullptr},
                                                                  {"cyclicPrefix", nullptr},
                                                                  {"startRB", nullptr},
                                                                  {"numberOfRBs", nullptr}};
static const asn1::table::seq_opt    seq_table_o_bwp_s_opts[]  = {
    {0, (int32_t)offsetof(o_bwp_s, bwp_context_present)},
    {1, (int32_t)offsetof(o_bwp_s, is_init_bwp_present)},
    {2, (int32_t)offsetof(o_bwp_s, sub_carrier_spacing_present)},
    {3, (int32_t)offsetof(o_bwp_s, cp_present)},
    {4, (int32_t)offsetof(o_bwp_s, start_rb_present)},
    {5, (int32_t)offsetof(o_bwp_s, nof_rbs_present)}};
const asn1::table::seq_desc o_bwp_s::desc = {
    {seq_table_o_bwp_s_fields, uint64_t{0x0}, seq_table_o_bwp_s_opts, seq_table_o_bwp_s_names, 6, 6},
    false,
    true,
    nullptr,
    0};

static const asn1::table::seq_field seq_table_partition_flow_list_item_s__fields[] = {
    {offsetof(partition_flow_list_item_s_, snssai),
     (int32_t)offsetof(partition_flow_list_item_s_, snssai_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &snssai_s::desc},
    {offsetof(partition_flow_list_item_s_, plmn_id),
     (int32_t)offsetof(partition_flow_list_item_s_, plmn_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc},
    {offsetof(partition_flow_list_item_s_, n5qi_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::list_codec<decltype(partition_flow_list_item_s_::n5qi_list),
                              asn1::table::int_json,
                              asn1::table::int_items<false, int64_t{0}, uint64_t{255}, false, true>>::value}};
static const asn1::table::field_name seq_table_partition_flow_list_item_s__names[] = {{"snssai", nullptr},
                                                                                      {"plmnId", nullptr},
                                                                                      {"5qiList", nullptr}};
static const asn1::table::seq_opt    seq_table_partition_flow_list_item_s__opts[]  = {
    {0, (int32_t)offsetof(partition_flow_list_item_s_, snssai_present)},
    {1, (int32_t)offsetof(partition_flow_list_item_s_, plmn_id_present)},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc partition_flow_list_item_s_::desc = {{seq_table_partition_flow_list_item_s__fields,
                                                                  uint64_t{0x0},
                                                                  seq_table_partition_flow_list_item_s__opts,
                                                                  seq_table_partition_flow_list_item_s__names,
                                                                  3,
                                                                  3},
                                                                 false,
                                                                 true,
                                                                 nullptr,
                                                                 0};

static const asn1::table::seq_field seq_table_partition_list_item_s__fields[] = {
    {offsetof(partition_list_item_s_, poffset_to_point_a),
     (int32_t)offsetof(partition_list_item_s_, poffset_to_point_a_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(partition_list_item_s_, pnof_rbs),
     (int32_t)offsetof(partition_list_item_s_, pnof_rbs_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(partition_list_item_s_, partition_flow_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(partition_list_item_s_::partition_flow_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &partition_flow_list_item_s_::desc}};
static const asn1::table::field_name seq_table_partition_list_item_s__names[] = {{"pOffsetToPointA", nullptr},
                                                                                 {"pNumberOfRBs", nullptr},
                                                                                 {"partitionFlowList", nullptr}};
static const asn1::table::seq_opt    seq_table_partition_list_item_s__opts[]  = {
    {0, (int32_t)offsetof(partition_list_item_s_, poffset_to_point_a_present)},
    {1, (int32_t)offsetof(partition_list_item_s_, pnof_rbs_present)},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc partition_list_item_s_::desc = {{seq_table_partition_list_item_s__fields,
                                                             uint64_t{0x0},
                                                             seq_table_partition_list_item_s__opts,
                                                             seq_table_partition_list_item_s__names,
                                                             3,
                                                             3},
                                                            false,
                                                            true,
                                                            nullptr,
                                                            0};

// O-NrCellDu ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_nr_cell_du_s_fields[] = {
    {offsetof(o_nr_cell_du_s, cell_local_id),
     (int32_t)offsetof(o_nr_cell_du_s, cell_local_id_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_nr_cell_du_s, operational_state),
     (int32_t)offsetof(o_nr_cell_du_s, operational_state_present),
     asn1::table::enum_op<decltype(o_nr_cell_du_s::operational_state)>(),
     decltype(o_nr_cell_du_s::operational_state)::has_ext,
     false,
     decltype(o_nr_cell_du_s::operational_state)::nof_types,
     decltype(o_nr_cell_du_s::operational_state)::nof_exts,
     nullptr,
     &operational_state_opts::desc},
    {offsetof(o_nr_cell_du_s, administrative_state),
     (int32_t)offsetof(o_nr_cell_du_s, administrative_state_present),
     asn1::table::enum_op<decltype(o_nr_cell_du_s::administrative_state)>(),
     decltype(o_nr_cell_du_s::administrative_state)::has_ext,
     false,
     decltype(o_nr_cell_du_s::administrative_state)::nof_types,
     decltype(o_nr_cell_du_s::administrative_state)::nof_exts,
     nullptr,
     &administrative_state_opts::desc},
    {offsetof(o_nr_cell_du_s, cell_state),
     (int32_t)offsetof(o_nr_cell_du_s, cell_state_present),
     asn1::table::enum_op<decltype(o_nr_cell_du_s::cell_state)>(),
     decltype(o_nr_cell_du_s::cell_state)::has_ext,
     false,
     decltype(o_nr_cell_du_s::cell_state)::nof_types,
     decltype(o_nr_cell_du_s::cell_state)::nof_exts,
     nullptr,
     &cell_state_opts::desc},
    {offsetof(o_nr_cell_du_s, plmn_info_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::
         item_list_codec<decltype(o_nr_cell_du_s::plmn_info_list), asn1::table::sequence_item_ops, false, true>::value,
     &plmn_info_s::desc},
    {offsetof(o_nr_cell_du_s, nr_pci),
     (int32_t)offsetof(o_nr_cell_du_s, nr_pci_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{503},
     nullptr},
    {offsetof(o_nr_cell_du_s, nr_tac),
     (int32_t)offsetof(o_nr_cell_du_s, nr_tac_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{16777215},
     nullptr},
    {offsetof(o_nr_cell_du_s, arfcn_dl),
     (int32_t)offsetof(o_nr_cell_du_s, arfcn_dl_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_nr_cell_du_s, arfcn_ul),
     (int32_t)offsetof(o_nr_cell_du_s, arfcn_ul_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_nr_cell_du_s, arfcn_sul),
     (int32_t)offsetof(o_nr_cell_du_s, arfcn_sul_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_nr_cell_du_s, bs_ch_bw_dl),
     (int32_t)offsetof(o_nr_cell_du_s, bs_ch_bw_dl_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_nr_cell_du_s, ssb_freq),
     (int32_t)offsetof(o_nr_cell_du_s, ssb_freq_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{3279165},
     nullptr},
    {offsetof(o_nr_cell_du_s, ssb_periodicity),
     (int32_t)offsetof(o_nr_cell_du_s, ssb_periodicity_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_nr_cell_du_s, ssb_sub_carrier_spacing),
     (int32_t)offsetof(o_nr_cell_du_s, ssb_sub_carrier_spacing_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_nr_cell_du_s, ssb_offset),
     (int32_t)offsetof(o_nr_cell_du_s, ssb_offset_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{0},
     uint64_t{159},
     nullptr},
    {offsetof(o_nr_cell_du_s, ssb_dur),
     (int32_t)offsetof(o_nr_cell_du_s, ssb_dur_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_nr_cell_du_s, bs_ch_bw_ul),
     (int32_t)offsetof(o_nr_cell_du_s, bs_ch_bw_ul_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_nr_cell_du_s, bs_ch_bw_sul),
     (int32_t)offsetof(o_nr_cell_du_s, bs_ch_bw_sul_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_nr_cell_du_s, bwp_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(o_nr_cell_du_s::bwp_list), asn1::table::sequence_item_ops, false, true>::
         value,
     &o_bwp_s::desc},
    {offsetof(o_nr_cell_du_s, partition_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::
         item_list_codec<decltype(o_nr_cell_du_s::partition_list), asn1::table::sequence_item_ops, false, true>::value,
     &partition_list_item_s_::desc}};
static const asn1::table::field_name seq_table_o_nr_cell_du_s_names[] = {{"cellLocalId", nullptr},
                                                                         {"operationalState", nullptr},
                                                                         {"administrativeState", nullptr},
                                                                         {"cellState", nullptr},
                                                                         {"plmnInfoList", nullptr},
                                                                         {"nrPci", nullptr},
                                                                         {"nrTac", nullptr},
                                                                         {"arfcnDL", nullptr},
                                                                         {"arfcnUL", nullptr},
                                                                         {"arfcnSUL", nullptr},
                                                                         {"bSChannelBwDL", nullptr},
                                                                         {"ssbFrequency", nullptr},
                                                                         {"ssbPeriodicity", nullptr},
                                                                         {"ssbSubCarrierSpacing", nullptr},
                                                                         {"ssbOffset", nullptr},
                                                                         {"ssbDuration", nullptr},
                                                                         {"bSChannelBwUL", nullptr},
                                                                         {"bSChannelBwSUL", nullptr},
                                                                         {"bwpList", nullptr},
                                                                         {"partitionList", nullptr}};
static const asn1::table::seq_opt    seq_table_o_nr_cell_du_s_opts[]  = {
    {0, (int32_t)offsetof(o_nr_cell_du_s, cell_local_id_present)},
    {1, (int32_t)offsetof(o_nr_cell_du_s, operational_state_present)},
    {2, (int32_t)offsetof(o_nr_cell_du_s, administrative_state_present)},
    {3, (int32_t)offsetof(o_nr_cell_du_s, cell_state_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(o_nr_cell_du_s, nr_pci_present)},
    {6, (int32_t)offsetof(o_nr_cell_du_s, nr_tac_present)},
    {7, (int32_t)offsetof(o_nr_cell_du_s, arfcn_dl_present)},
    {8, (int32_t)offsetof(o_nr_cell_du_s, arfcn_ul_present)},
    {9, (int32_t)offsetof(o_nr_cell_du_s, arfcn_sul_present)},
    {10, (int32_t)offsetof(o_nr_cell_du_s, bs_ch_bw_dl_present)},
    {11, (int32_t)offsetof(o_nr_cell_du_s, ssb_freq_present)},
    {12, (int32_t)offsetof(o_nr_cell_du_s, ssb_periodicity_present)},
    {13, (int32_t)offsetof(o_nr_cell_du_s, ssb_sub_carrier_spacing_present)},
    {14, (int32_t)offsetof(o_nr_cell_du_s, ssb_offset_present)},
    {15, (int32_t)offsetof(o_nr_cell_du_s, ssb_dur_present)},
    {16, (int32_t)offsetof(o_nr_cell_du_s, bs_ch_bw_ul_present)},
    {17, (int32_t)offsetof(o_nr_cell_du_s, bs_ch_bw_sul_present)},
    {18, asn1::table::seq_field::list_presence},
    {19, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc o_nr_cell_du_s::desc = {{seq_table_o_nr_cell_du_s_fields,
                                                     uint64_t{0x0},
                                                     seq_table_o_nr_cell_du_s_opts,
                                                     seq_table_o_nr_cell_du_s_names,
                                                     20,
                                                     20},
                                                    false,
                                                    true,
                                                    nullptr,
                                                    0};

// ResourceType ::= ENUMERATED
static const char*           enum_names_res_type_e_values[] = {"PRB_DL", "PRB_UL", "DRB", "RRC"};
const asn1::table::enum_desc res_type_opts::desc            = {enum_names_res_type_e_values, 4, "res_type_e"};

// RrmPolicyMember ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrm_policy_member_s_fields[] = {
    {offsetof(rrm_policy_member_s, plmn_id),
     (int32_t)offsetof(rrm_policy_member_s, plmn_id_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &plmn_id_s::desc},
    {offsetof(rrm_policy_member_s, snssai),
     (int32_t)offsetof(rrm_policy_member_s, snssai_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &snssai_s::desc}};
static const asn1::table::field_name seq_table_rrm_policy_member_s_names[] = {{"plmnId", nullptr}, {"snssai", nullptr}};
static const asn1::table::seq_opt    seq_table_rrm_policy_member_s_opts[]  = {
    {0, (int32_t)offsetof(rrm_policy_member_s, plmn_id_present)},
    {1, (int32_t)offsetof(rrm_policy_member_s, snssai_present)}};
const asn1::table::seq_desc rrm_policy_member_s::desc = {{seq_table_rrm_policy_member_s_fields,
                                                          uint64_t{0x0},
                                                          seq_table_rrm_policy_member_s_opts,
                                                          seq_table_rrm_policy_member_s_names,
                                                          2,
                                                          2},
                                                         false,
                                                         true,
                                                         nullptr,
                                                         0};

// O-RRMPolicyRatio ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_rrm_policy_ratio_s_fields[] = {
    {offsetof(o_rrm_policy_ratio_s, res_type),
     (int32_t)offsetof(o_rrm_policy_ratio_s, res_type_present),
     asn1::table::enum_op<decltype(o_rrm_policy_ratio_s::res_type)>(),
     decltype(o_rrm_policy_ratio_s::res_type)::has_ext,
     false,
     decltype(o_rrm_policy_ratio_s::res_type)::nof_types,
     decltype(o_rrm_policy_ratio_s::res_type)::nof_exts,
     nullptr,
     &res_type_opts::desc},
    {offsetof(o_rrm_policy_ratio_s, rrm_policy_member_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(o_rrm_policy_ratio_s::rrm_policy_member_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &rrm_policy_member_s::desc},
    {offsetof(o_rrm_policy_ratio_s, rrm_policy_max_ratio),
     (int32_t)offsetof(o_rrm_policy_ratio_s, rrm_policy_max_ratio_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_rrm_policy_ratio_s, rrm_policy_min_ratio),
     (int32_t)offsetof(o_rrm_policy_ratio_s, rrm_policy_min_ratio_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(o_rrm_policy_ratio_s, rrm_policy_ded_ratio),
     (int32_t)offsetof(o_rrm_policy_ratio_s, rrm_policy_ded_ratio_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_o_rrm_policy_ratio_s_names[] = {{"resourceType", nullptr},
                                                                               {"rRMPolicyMemberList", nullptr},
                                                                               {"rRMPolicyMaxRatio", nullptr},
                                                                               {"rRMPolicyMinRatio", nullptr},
                                                                               {"rRMPolicyDedicatedRatio", nullptr}};
static const asn1::table::seq_opt    seq_table_o_rrm_policy_ratio_s_opts[]  = {
    {0, (int32_t)offsetof(o_rrm_policy_ratio_s, res_type_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(o_rrm_policy_ratio_s, rrm_policy_max_ratio_present)},
    {3, (int32_t)offsetof(o_rrm_policy_ratio_s, rrm_policy_min_ratio_present)},
    {4, (int32_t)offsetof(o_rrm_policy_ratio_s, rrm_policy_ded_ratio_present)}};
const asn1::table::seq_desc o_rrm_policy_ratio_s::desc = {{seq_table_o_rrm_policy_ratio_s_fields,
                                                           uint64_t{0x0},
                                                           seq_table_o_rrm_policy_ratio_s_opts,
                                                           seq_table_o_rrm_policy_ratio_s_names,
                                                           5,
                                                           5},
                                                          false,
                                                          true,
                                                          nullptr,
                                                          0};

// O-CESManagementFunction ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_ces_management_function_s_fields[] = {
    {offsetof(o_ces_management_function_s, ces_switch),
     (int32_t)offsetof(o_ces_management_function_s, ces_switch_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(o_ces_management_function_s, energy_saving_state),
     (int32_t)offsetof(o_ces_management_function_s, energy_saving_state_present),
     asn1::table::enum_op<decltype(o_ces_management_function_s::energy_saving_state)>(),
     decltype(o_ces_management_function_s::energy_saving_state)::has_ext,
     false,
     decltype(o_ces_management_function_s::energy_saving_state)::nof_types,
     decltype(o_ces_management_function_s::energy_saving_state)::nof_exts,
     nullptr,
     &o_ces_management_function_s::energy_saving_state_opts::desc},
    {offsetof(o_ces_management_function_s, energy_saving_ctrl),
     (int32_t)offsetof(o_ces_management_function_s, energy_saving_ctrl_present),
     asn1::table::enum_op<decltype(o_ces_management_function_s::energy_saving_ctrl)>(),
     decltype(o_ces_management_function_s::energy_saving_ctrl)::has_ext,
     false,
     decltype(o_ces_management_function_s::energy_saving_ctrl)::nof_types,
     decltype(o_ces_management_function_s::energy_saving_ctrl)::nof_exts,
     nullptr,
     &o_ces_management_function_s::energy_saving_ctrl_opts::desc}};
static const asn1::table::field_name seq_table_o_ces_management_function_s_names[] = {{"cesSwitch", nullptr},
                                                                                      {"energySavingState", nullptr},
                                                                                      {"energySavingControl", nullptr}};
static const asn1::table::seq_opt    seq_table_o_ces_management_function_s_opts[]  = {
    {0, (int32_t)offsetof(o_ces_management_function_s, ces_switch_present)},
    {1, (int32_t)offsetof(o_ces_management_function_s, energy_saving_state_present)},
    {2, (int32_t)offsetof(o_ces_management_function_s, energy_saving_ctrl_present)}};
const asn1::table::seq_desc o_ces_management_function_s::desc = {{seq_table_o_ces_management_function_s_fields,
                                                                  uint64_t{0x0},
                                                                  seq_table_o_ces_management_function_s_opts,
                                                                  seq_table_o_ces_management_function_s_names,
                                                                  3,
                                                                  3},
                                                                 false,
                                                                 true,
                                                                 nullptr,
                                                                 0};

static const char* enum_names_o_ces_management_function_s_energy_saving_state_e__values[] = {"isNotEnergySaving",
                                                                                             "isEnergySaving"};
const asn1::table::enum_desc o_ces_management_function_s::energy_saving_state_opts::desc  = {
    enum_names_o_ces_management_function_s_energy_saving_state_e__values,
    2,
    "o_ces_management_function_s::energy_saving_state_e_"};

static const char* enum_names_o_ces_management_function_s_energy_saving_ctrl_e__values[] = {"toBeEnergySaving",
                                                                                            "toBeNotEnergySaving"};
const asn1::table::enum_desc o_ces_management_function_s::energy_saving_ctrl_opts::desc  = {
    enum_names_o_ces_management_function_s_energy_saving_ctrl_e__values,
    2,
    "o_ces_management_function_s::energy_saving_ctrl_e_"};

// EsObjective ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_es_objive_c_alternatives[] = {
    &asn1::table::alternative_ops<es_objive_c::target_ec_s_, false>::value,
    &asn1::table::alternative_ops<es_objive_c::es_percentage_s_, false>::value};
const asn1::table::choice_storage_desc es_objive_c::storage_desc = {choice_storage_es_objive_c_alternatives,
                                                                    2,
                                                                    "es_objive_c"};

es_objive_c::target_ec_s_& es_objive_c::set_target_ec()
{
  set(types::target_ec);
  return c.get<target_ec_s_>();
}
es_objive_c::es_percentage_s_& es_objive_c::set_es_percentage()
{
  set(types::es_percentage);
  return c.get<es_percentage_s_>();
}
static const asn1::table::seq_field seq_table_es_objive_c_target_ec_s__fields[] = {
    {offsetof(es_objive_c::target_ec_s_, target_ec),
     (int32_t)offsetof(es_objive_c::target_ec_s_, target_ec_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(es_objive_c::target_ec_s_::target_ec), asn1::table::float_json>::value}};
static const asn1::table::field_name seq_table_es_objive_c_target_ec_s__names[] = {{"targetEc", nullptr}};
static const asn1::table::seq_opt    seq_table_es_objive_c_target_ec_s__opts[]  = {
    {0, (int32_t)offsetof(es_objive_c::target_ec_s_, target_ec_present)}};
const asn1::table::seq_desc         es_objive_c::target_ec_s_::desc = {{seq_table_es_objive_c_target_ec_s__fields,
                                                                        uint64_t{0x0},
                                                                        seq_table_es_objive_c_target_ec_s__opts,
                                                                        seq_table_es_objive_c_target_ec_s__names,
                                                                        1,
                                                                        1},
                                                                       false,
                                                                       true,
                                                                       nullptr,
                                                                       0};
static const asn1::table::seq_field seq_table_es_objive_c_es_percentage_s__fields[] = {
    {offsetof(es_objive_c::es_percentage_s_, es_percentage),
     (int32_t)offsetof(es_objive_c::es_percentage_s_, es_percentage_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{0},
     uint64_t{100},
     nullptr}};
static const asn1::table::field_name seq_table_es_objive_c_es_percentage_s__names[] = {{"esPercentage", nullptr}};
static const asn1::table::seq_opt    seq_table_es_objive_c_es_percentage_s__opts[]  = {
    {0, (int32_t)offsetof(es_objive_c::es_percentage_s_, es_percentage_present)}};
const asn1::table::seq_desc es_objive_c::es_percentage_s_::desc = {{seq_table_es_objive_c_es_percentage_s__fields,
                                                                    uint64_t{0x0},
                                                                    seq_table_es_objive_c_es_percentage_s__opts,
                                                                    seq_table_es_objive_c_es_percentage_s__names,
                                                                    1,
                                                                    1},
                                                                   false,
                                                                   true,
                                                                   nullptr,
                                                                   0};
static const asn1::table::seq_field  choice_table_es_objive_c_alternatives[] = {{0,
                                                                                 asn1::table::seq_field::mandatory,
                                                                                 asn1::table::field_op::sequence,
                                                                                 false,
                                                                                 false,
                                                                                 0,
                                                                                 0,
                                                                                 nullptr,
                                                                                 &es_objive_c::target_ec_s_::desc},
                                                                                {0,
                                                                                 asn1::table::seq_field::mandatory,
                                                                                 asn1::table::field_op::sequence,
                                                                                 false,
                                                                                 false,
                                                                                 0,
                                                                                 0,
                                                                                 nullptr,
                                                                                 &es_objive_c::es_percentage_s_::desc}};
static const asn1::table::field_name choice_table_es_objive_c_names[]        = {{"target-Ec", nullptr},
                                                                                {"es-Percentage", nullptr}};
const asn1::table::choice_desc       es_objive_c::desc = {asn1::table::enum_int_type<es_objive_c::types>(),
                                                          es_objive_c::types::nof_types,
                                                          es_objive_c::types::nof_exts,
                                                          es_objive_c::types::has_ext,
                                                          true,
                                                          choice_table_es_objive_c_alternatives,
                                                          nullptr,
                                                          choice_table_es_objive_c_names,
                                                          "es_objive_c",
                                                          offsetof(es_objive_c, c.type),
                                                          offsetof(es_objive_c, c.value),
                                                          false,
                                                          &es_objive_c::storage_desc};

static const char*           enum_names_es_objive_c_types_values[] = {"target-Ec", "es-Percentage"};
const asn1::table::enum_desc es_objive_c::types_opts::desc         = {enum_names_es_objive_c_types_values,
                                                                      2,
                                                                      "es_objive_c::types"};

// PerfObjective ::= SEQUENCE
static const asn1::table::seq_field seq_table_perf_objive_s_fields[] = {
    {offsetof(perf_objive_s, plmn_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::
         item_list_codec<decltype(perf_objive_s::plmn_info_list), asn1::table::sequence_item_ops, false, true>::value,
     &plmn_info_s::desc},
    {offsetof(perf_objive_s, five_qi_value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(perf_objive_s, max_ngbr_flow_br),
     (int32_t)offsetof(perf_objive_s, max_ngbr_flow_br_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(perf_objive_s::max_ngbr_flow_br), asn1::table::string_json>::value},
    {offsetof(perf_objive_s, flow_br_avg_win),
     (int32_t)offsetof(perf_objive_s, flow_br_avg_win_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{4095},
     nullptr},
    {offsetof(perf_objive_s, max_pd),
     (int32_t)offsetof(perf_objive_s, max_pd_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{1023},
     nullptr},
    {offsetof(perf_objive_s, target_pd),
     (int32_t)offsetof(perf_objive_s, target_pd_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{1023},
     nullptr}};
static const asn1::table::field_name seq_table_perf_objive_s_names[] = {{"plmnInfoList", nullptr},
                                                                        {"fiveQIValue", nullptr},
                                                                        {"maxNgbrFlowBr", nullptr},
                                                                        {"flowBrAvgWindow", nullptr},
                                                                        {"maxPd", nullptr},
                                                                        {"targetPd", nullptr}};
static const asn1::table::seq_opt    seq_table_perf_objive_s_opts[]  = {
    {2, (int32_t)offsetof(perf_objive_s, max_ngbr_flow_br_present)},
    {3, (int32_t)offsetof(perf_objive_s, flow_br_avg_win_present)},
    {4, (int32_t)offsetof(perf_objive_s, max_pd_present)},
    {5, (int32_t)offsetof(perf_objive_s, target_pd_present)}};
const asn1::table::seq_desc perf_objive_s::desc = {
    {seq_table_perf_objive_s_fields, uint64_t{0x3}, seq_table_perf_objive_s_opts, seq_table_perf_objive_s_names, 6, 4},
    false,
    true,
    nullptr,
    0};

// O-NESPolicy ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_nes_policy_s_fields[] = {
    {offsetof(o_nes_policy_s, policy_type),
     (int32_t)offsetof(o_nes_policy_s, policy_type_present),
     asn1::table::enum_op<decltype(o_nes_policy_s::policy_type)>(),
     decltype(o_nes_policy_s::policy_type)::has_ext,
     false,
     decltype(o_nes_policy_s::policy_type)::nof_types,
     decltype(o_nes_policy_s::policy_type)::nof_exts,
     nullptr,
     &o_nes_policy_s::policy_type_opts::desc},
    {offsetof(o_nes_policy_s, ant_mask_name),
     (int32_t)offsetof(o_nes_policy_s, ant_mask_name_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(o_nes_policy_s::ant_mask_name), asn1::table::string_json>::value},
    {offsetof(o_nes_policy_s, ant_mask),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(o_nes_policy_s::ant_mask), asn1::table::string_json>::value},
    {offsetof(o_nes_policy_s, sleep_mode),
     (int32_t)offsetof(o_nes_policy_s, sleep_mode_present),
     asn1::table::enum_op<decltype(o_nes_policy_s::sleep_mode)>(),
     decltype(o_nes_policy_s::sleep_mode)::has_ext,
     false,
     decltype(o_nes_policy_s::sleep_mode)::nof_types,
     decltype(o_nes_policy_s::sleep_mode)::nof_exts,
     nullptr,
     &o_nes_policy_s::sleep_mode_opts::desc},
    {offsetof(o_nes_policy_s, data_dir),
     (int32_t)offsetof(o_nes_policy_s, data_dir_present),
     asn1::table::enum_op<decltype(o_nes_policy_s::data_dir)>(),
     decltype(o_nes_policy_s::data_dir)::has_ext,
     false,
     decltype(o_nes_policy_s::data_dir)::nof_types,
     decltype(o_nes_policy_s::data_dir)::nof_exts,
     nullptr,
     &o_nes_policy_s::data_dir_opts::desc},
    {offsetof(o_nes_policy_s, symbol_mask),
     (int32_t)offsetof(o_nes_policy_s, symbol_mask_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{16383},
     nullptr},
    {offsetof(o_nes_policy_s, slot_mask),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(o_nes_policy_s::slot_mask), asn1::table::string_json>::value},
    {offsetof(o_nes_policy_s, hibernate_dur),
     (int32_t)offsetof(o_nes_policy_s, hibernate_dur_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(o_nes_policy_s, es_objive),
     (int32_t)offsetof(o_nes_policy_s, es_objive_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &es_objive_c::desc},
    {offsetof(o_nes_policy_s, perf_objive_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(o_nes_policy_s::perf_objive_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &perf_objive_s::desc}};
static const asn1::table::field_name seq_table_o_nes_policy_s_names[] = {{"policyType", nullptr},
                                                                         {"antennaMaskName", nullptr},
                                                                         {"antennaMask", nullptr},
                                                                         {"sleepMode", nullptr},
                                                                         {"dataDir", nullptr},
                                                                         {"symbolMask", nullptr},
                                                                         {"slotMask", nullptr},
                                                                         {"hibernateDuration", nullptr},
                                                                         {"esObjective", nullptr},
                                                                         {"perfObjectiveList", nullptr}};
static const asn1::table::seq_opt    seq_table_o_nes_policy_s_opts[]  = {
    {0, (int32_t)offsetof(o_nes_policy_s, policy_type_present)},
    {1, (int32_t)offsetof(o_nes_policy_s, ant_mask_name_present)},
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(o_nes_policy_s, sleep_mode_present)},
    {4, (int32_t)offsetof(o_nes_policy_s, data_dir_present)},
    {5, (int32_t)offsetof(o_nes_policy_s, symbol_mask_present)},
    {6, asn1::table::seq_field::list_presence},
    {7, (int32_t)offsetof(o_nes_policy_s, hibernate_dur_present)},
    {8, (int32_t)offsetof(o_nes_policy_s, es_objive_present)},
    {9, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc o_nes_policy_s::desc = {{seq_table_o_nes_policy_s_fields,
                                                     uint64_t{0x0},
                                                     seq_table_o_nes_policy_s_opts,
                                                     seq_table_o_nes_policy_s_names,
                                                     10,
                                                     10},
                                                    false,
                                                    true,
                                                    nullptr,
                                                    0};

static const char* enum_names_o_nes_policy_s_policy_type_e__values[] = {"ASM", "TRX_CONTROL", "DEEP_HIBERNATE"};
const asn1::table::enum_desc o_nes_policy_s::policy_type_opts::desc  = {enum_names_o_nes_policy_s_policy_type_e__values,
                                                                        3,
                                                                        "o_nes_policy_s::policy_type_e_"};

static const char*           enum_names_o_nes_policy_s_sleep_mode_e__values[] = {"SLEEP_MODE0",
                                                                                 "SLEEP_MODE1",
                                                                                 "SLEEP_MODE2",
                                                                                 "SLEEP_MODE3"};
const asn1::table::enum_desc o_nes_policy_s::sleep_mode_opts::desc = {enum_names_o_nes_policy_s_sleep_mode_e__values,
                                                                      4,
                                                                      "o_nes_policy_s::sleep_mode_e_"};
uint8_t                      o_nes_policy_s::sleep_mode_opts::to_number() const
{
  static const uint8_t numbers[] = {0, 1, 2, 3};
  return map_enum_number(numbers, 4, value, "o_nes_policy_s::sleep_mode_e_");
}

static const char*           enum_names_o_nes_policy_s_data_dir_e__values[] = {"DL", "UL"};
const asn1::table::enum_desc o_nes_policy_s::data_dir_opts::desc = {enum_names_o_nes_policy_s_data_dir_e__values,
                                                                    2,
                                                                    "o_nes_policy_s::data_dir_e_"};

// OnDurationTimer ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_on_dur_timer_c_alternatives[] = {
    &asn1::table::alternative_ops<uint8_t, true>::value,
    &asn1::table::alternative_ops<on_dur_timer_c::timeout_e_, true>::value};
const asn1::table::choice_storage_desc on_dur_timer_c::storage_desc = {choice_storage_on_dur_timer_c_alternatives,
                                                                       2,
                                                                       "on_dur_timer_c"};

uint8_t& on_dur_timer_c::set_integer()
{
  set(types::integer);
  return c.get<uint8_t>();
}
on_dur_timer_c::timeout_e_& on_dur_timer_c::set_timeout()
{
  set(types::timeout);
  return c.get<timeout_e_>();
}
static const asn1::table::seq_field choice_table_on_dur_timer_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{1},
     uint64_t{30},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<on_dur_timer_c::timeout_e_>(),
     on_dur_timer_c::timeout_e_::has_ext,
     false,
     on_dur_timer_c::timeout_e_::nof_types,
     on_dur_timer_c::timeout_e_::nof_exts,
     nullptr,
     &on_dur_timer_c::timeout_opts::desc}};
static const asn1::table::field_name choice_table_on_dur_timer_c_names[] = {{"integer", nullptr}, {"timeout", nullptr}};
const asn1::table::choice_desc       on_dur_timer_c::desc = {asn1::table::enum_int_type<on_dur_timer_c::types>(),
                                                             on_dur_timer_c::types::nof_types,
                                                             on_dur_timer_c::types::nof_exts,
                                                             on_dur_timer_c::types::has_ext,
                                                             true,
                                                             choice_table_on_dur_timer_c_alternatives,
                                                             nullptr,
                                                             choice_table_on_dur_timer_c_names,
                                                             "on_dur_timer_c",
                                                             offsetof(on_dur_timer_c, c.type),
                                                             offsetof(on_dur_timer_c, c.value),
                                                             false,
                                                             &on_dur_timer_c::storage_desc};

static const char* enum_names_on_dur_timer_c_timeout_e__values[] = {
    "ms1",  "ms2",   "ms3",   "ms4",   "ms5",   "ms6",   "ms8",   "ms10",  "ms20",   "ms40",   "ms50",  "ms60",
    "ms80", "ms100", "ms200", "ms300", "ms400", "ms500", "ms600", "ms800", "ms1000", "ms1200", "ms1600"};
const asn1::table::enum_desc on_dur_timer_c::timeout_opts::desc = {enum_names_on_dur_timer_c_timeout_e__values,
                                                                   23,
                                                                   "on_dur_timer_c::timeout_e_"};
uint16_t                     on_dur_timer_c::timeout_opts::to_number() const
{
  static const uint16_t numbers[] = {1,  2,   3,   4,   5,   6,   8,   10,  20,   40,   50,  60,
                                     80, 100, 200, 300, 400, 500, 600, 800, 1000, 1200, 1600};
  return map_enum_number(numbers, 23, value, "on_dur_timer_c::timeout_e_");
}

static const char*           enum_names_on_dur_timer_c_types_values[] = {"integer", "timeout"};
const asn1::table::enum_desc on_dur_timer_c::types_opts::desc         = {enum_names_on_dur_timer_c_types_values,
                                                                         2,
                                                                         "on_dur_timer_c::types"};

// CycleStartOffset ::= SEQUENCE
static const asn1::table::seq_field seq_table_cycle_start_offset_s_fields[] = {
    {offsetof(cycle_start_offset_s, periodicity),
     (int32_t)offsetof(cycle_start_offset_s, periodicity_present),
     asn1::table::enum_op<decltype(cycle_start_offset_s::periodicity)>(),
     decltype(cycle_start_offset_s::periodicity)::has_ext,
     false,
     decltype(cycle_start_offset_s::periodicity)::nof_types,
     decltype(cycle_start_offset_s::periodicity)::nof_exts,
     nullptr,
     &cycle_start_offset_s::periodicity_opts::desc},
    {offsetof(cycle_start_offset_s, offset),
     (int32_t)offsetof(cycle_start_offset_s, offset_present),
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{10239},
     nullptr}};
static const asn1::table::field_name seq_table_cycle_start_offset_s_names[] = {{"periodicity", nullptr},
                                                                               {"offset", nullptr}};
static const asn1::table::seq_opt    seq_table_cycle_start_offset_s_opts[]  = {
    {0, (int32_t)offsetof(cycle_start_offset_s, periodicity_present)},
    {1, (int32_t)offsetof(cycle_start_offset_s, offset_present)}};
const asn1::table::seq_desc cycle_start_offset_s::desc = {{seq_table_cycle_start_offset_s_fields,
                                                           uint64_t{0x0},
                                                           seq_table_cycle_start_offset_s_opts,
                                                           seq_table_cycle_start_offset_s_names,
                                                           2,
                                                           2},
                                                          false,
                                                          true,
                                                          nullptr,
                                                          0};

static const char* enum_names_cycle_start_offset_s_periodicity_e__values[] = {
    "ms10",  "ms20",  "ms32",  "ms40",  "ms60",   "ms64",   "ms70",   "ms80",   "ms128",  "ms160",
    "ms256", "ms320", "ms512", "ms640", "ms1024", "ms1280", "ms2048", "ms2560", "ms5120", "ms10240"};
const asn1::table::enum_desc cycle_start_offset_s::periodicity_opts::desc = {
    enum_names_cycle_start_offset_s_periodicity_e__values,
    20,
    "cycle_start_offset_s::periodicity_e_"};
uint16_t cycle_start_offset_s::periodicity_opts::to_number() const
{
  static const uint16_t numbers[] = {10,  20,  32,  40,  60,   64,   70,   80,   128,  160,
                                     256, 320, 512, 640, 1024, 1280, 2048, 2560, 5120, 10240};
  return map_enum_number(numbers, 20, value, "cycle_start_offset_s::periodicity_e_");
}

// O-CellDTXDRXConfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_cell_dtxdrx_cfg_s_fields[] = {
    {offsetof(o_cell_dtxdrx_cfg_s, on_dur_timer),
     (int32_t)offsetof(o_cell_dtxdrx_cfg_s, on_dur_timer_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &on_dur_timer_c::desc},
    {offsetof(o_cell_dtxdrx_cfg_s, cycle_start_offset),
     (int32_t)offsetof(o_cell_dtxdrx_cfg_s, cycle_start_offset_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &cycle_start_offset_s::desc},
    {offsetof(o_cell_dtxdrx_cfg_s, slot_offset),
     (int32_t)offsetof(o_cell_dtxdrx_cfg_s, slot_offset_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{0},
     uint64_t{31},
     nullptr},
    {offsetof(o_cell_dtxdrx_cfg_s, cfg_type),
     (int32_t)offsetof(o_cell_dtxdrx_cfg_s, cfg_type_present),
     asn1::table::enum_op<decltype(o_cell_dtxdrx_cfg_s::cfg_type)>(),
     decltype(o_cell_dtxdrx_cfg_s::cfg_type)::has_ext,
     false,
     decltype(o_cell_dtxdrx_cfg_s::cfg_type)::nof_types,
     decltype(o_cell_dtxdrx_cfg_s::cfg_type)::nof_exts,
     nullptr,
     &o_cell_dtxdrx_cfg_s::cfg_type_opts::desc},
    {offsetof(o_cell_dtxdrx_cfg_s, activation_status),
     (int32_t)offsetof(o_cell_dtxdrx_cfg_s, activation_status_present),
     asn1::table::enum_op<decltype(o_cell_dtxdrx_cfg_s::activation_status)>(),
     decltype(o_cell_dtxdrx_cfg_s::activation_status)::has_ext,
     false,
     decltype(o_cell_dtxdrx_cfg_s::activation_status)::nof_types,
     decltype(o_cell_dtxdrx_cfg_s::activation_status)::nof_exts,
     nullptr,
     &o_cell_dtxdrx_cfg_s::activation_status_opts::desc},
    {offsetof(o_cell_dtxdrx_cfg_s, l1_activation),
     (int32_t)offsetof(o_cell_dtxdrx_cfg_s, l1_activation_present),
     asn1::table::enum_op<decltype(o_cell_dtxdrx_cfg_s::l1_activation)>(),
     decltype(o_cell_dtxdrx_cfg_s::l1_activation)::has_ext,
     false,
     decltype(o_cell_dtxdrx_cfg_s::l1_activation)::nof_types,
     decltype(o_cell_dtxdrx_cfg_s::l1_activation)::nof_exts,
     nullptr,
     &o_cell_dtxdrx_cfg_s::l1_activation_opts::desc}};
static const asn1::table::field_name seq_table_o_cell_dtxdrx_cfg_s_names[] = {{"onDurationTimer", nullptr},
                                                                              {"cycleStartOffset", nullptr},
                                                                              {"slotOffset", nullptr},
                                                                              {"configType", nullptr},
                                                                              {"activationStatus", nullptr},
                                                                              {"l1Activation", nullptr}};
static const asn1::table::seq_opt    seq_table_o_cell_dtxdrx_cfg_s_opts[]  = {
    {0, (int32_t)offsetof(o_cell_dtxdrx_cfg_s, on_dur_timer_present)},
    {1, (int32_t)offsetof(o_cell_dtxdrx_cfg_s, cycle_start_offset_present)},
    {2, (int32_t)offsetof(o_cell_dtxdrx_cfg_s, slot_offset_present)},
    {3, (int32_t)offsetof(o_cell_dtxdrx_cfg_s, cfg_type_present)},
    {4, (int32_t)offsetof(o_cell_dtxdrx_cfg_s, activation_status_present)},
    {5, (int32_t)offsetof(o_cell_dtxdrx_cfg_s, l1_activation_present)}};
const asn1::table::seq_desc o_cell_dtxdrx_cfg_s::desc = {{seq_table_o_cell_dtxdrx_cfg_s_fields,
                                                          uint64_t{0x0},
                                                          seq_table_o_cell_dtxdrx_cfg_s_opts,
                                                          seq_table_o_cell_dtxdrx_cfg_s_names,
                                                          6,
                                                          6},
                                                         false,
                                                         true,
                                                         nullptr,
                                                         0};

static const char*           enum_names_o_cell_dtxdrx_cfg_s_cfg_type_e__values[] = {"DTX", "DRX", "DTXDRX"};
const asn1::table::enum_desc o_cell_dtxdrx_cfg_s::cfg_type_opts::desc            = {
    enum_names_o_cell_dtxdrx_cfg_s_cfg_type_e__values,
    3,
    "o_cell_dtxdrx_cfg_s::cfg_type_e_"};

static const char* enum_names_o_cell_dtxdrx_cfg_s_activation_status_e__values[] = {"ACTIVATED", "DEACTIVATED"};
const asn1::table::enum_desc o_cell_dtxdrx_cfg_s::activation_status_opts::desc  = {
    enum_names_o_cell_dtxdrx_cfg_s_activation_status_e__values,
    2,
    "o_cell_dtxdrx_cfg_s::activation_status_e_"};

static const char*           enum_names_o_cell_dtxdrx_cfg_s_l1_activation_e__values[] = {"ENABLED", "DISABLED"};
const asn1::table::enum_desc o_cell_dtxdrx_cfg_s::l1_activation_opts::desc            = {
    enum_names_o_cell_dtxdrx_cfg_s_l1_activation_e__values,
    2,
    "o_cell_dtxdrx_cfg_s::l1_activation_e_"};

// TPolarizationItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_t_polarization_item_s_fields[] = {
    {offsetof(t_polarization_item_s, tpolarization_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(t_polarization_item_s, tpolarization_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(t_polarization_item_s::tpolarization_type)>(),
     decltype(t_polarization_item_s::tpolarization_type)::has_ext,
     false,
     decltype(t_polarization_item_s::tpolarization_type)::nof_types,
     decltype(t_polarization_item_s::tpolarization_type)::nof_exts,
     nullptr,
     &t_polarization_item_s::tpolarization_type_opts::desc}};
static const asn1::table::field_name seq_table_t_polarization_item_s_names[] = {{"tPolarizationIndex", nullptr},
                                                                                {"tPolarizationType", nullptr}};
const asn1::table::seq_desc          t_polarization_item_s::desc             = {
    {seq_table_t_polarization_item_s_fields, uint64_t{0x3}, nullptr, seq_table_t_polarization_item_s_names, 2, 0},
    false,
    true,
    nullptr,
    0};

static const char*           enum_names_t_polarization_item_s_tpolarization_type_e__values[] = {"MINUS_45",
                                                                                                "ZERO",
                                                                                                "PLUS_45",
                                                                                                "PLUS_90"};
const asn1::table::enum_desc t_polarization_item_s::tpolarization_type_opts::desc            = {
    enum_names_t_polarization_item_s_tpolarization_type_e__values,
    4,
    "t_polarization_item_s::tpolarization_type_e_"};

// TxArrayItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_tx_array_item_s_tnormal_vector_direction_s__fields[] = {
    {offsetof(tx_array_item_s::tnormal_vector_direction_s_, tazimuth_angle),
     (int32_t)offsetof(tx_array_item_s::tnormal_vector_direction_s_, tazimuth_angle_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_array_item_s::tnormal_vector_direction_s_::tazimuth_angle),
                                asn1::table::float_json>::value},
    {offsetof(tx_array_item_s::tnormal_vector_direction_s_, tzenith_angle),
     (int32_t)offsetof(tx_array_item_s::tnormal_vector_direction_s_, tzenith_angle_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_array_item_s::tnormal_vector_direction_s_::tzenith_angle),
                                asn1::table::float_json>::value}};
static const asn1::table::field_name seq_table_tx_array_item_s_tnormal_vector_direction_s__names[] = {
    {"tAzimuthAngle", nullptr},
    {"tZenithAngle", nullptr}};
static const asn1::table::seq_opt seq_table_tx_array_item_s_tnormal_vector_direction_s__opts[] = {
    {0, (int32_t)offsetof(tx_array_item_s::tnormal_vector_direction_s_, tazimuth_angle_present)},
    {1, (int32_t)offsetof(tx_array_item_s::tnormal_vector_direction_s_, tzenith_angle_present)}};
const asn1::table::seq_desc tx_array_item_s::tnormal_vector_direction_s_::desc = {
    {seq_table_tx_array_item_s_tnormal_vector_direction_s__fields,
     uint64_t{0x0},
     seq_table_tx_array_item_s_tnormal_vector_direction_s__opts,
     seq_table_tx_array_item_s_tnormal_vector_direction_s__names,
     2,
     2},
    false,
    true,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_tx_array_item_s_tleftmost_bottom_array_elem_position_s__fields[] = {
    {offsetof(tx_array_item_s::tleftmost_bottom_array_elem_position_s_, tx_axis),
     (int32_t)offsetof(tx_array_item_s::tleftmost_bottom_array_elem_position_s_, tx_axis_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_array_item_s::tleftmost_bottom_array_elem_position_s_::tx_axis),
                                asn1::table::float_json>::value},
    {offsetof(tx_array_item_s::tleftmost_bottom_array_elem_position_s_, ty_axis),
     (int32_t)offsetof(tx_array_item_s::tleftmost_bottom_array_elem_position_s_, ty_axis_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_array_item_s::tleftmost_bottom_array_elem_position_s_::ty_axis),
                                asn1::table::float_json>::value},
    {offsetof(tx_array_item_s::tleftmost_bottom_array_elem_position_s_, tz_axis),
     (int32_t)offsetof(tx_array_item_s::tleftmost_bottom_array_elem_position_s_, tz_axis_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_array_item_s::tleftmost_bottom_array_elem_position_s_::tz_axis),
                                asn1::table::float_json>::value}};
static const asn1::table::field_name seq_table_tx_array_item_s_tleftmost_bottom_array_elem_position_s__names[] = {
    {"tXAxis", nullptr},
    {"tYAxis", nullptr},
    {"tZAxis", nullptr}};
static const asn1::table::seq_opt seq_table_tx_array_item_s_tleftmost_bottom_array_elem_position_s__opts[] = {
    {0, (int32_t)offsetof(tx_array_item_s::tleftmost_bottom_array_elem_position_s_, tx_axis_present)},
    {1, (int32_t)offsetof(tx_array_item_s::tleftmost_bottom_array_elem_position_s_, ty_axis_present)},
    {2, (int32_t)offsetof(tx_array_item_s::tleftmost_bottom_array_elem_position_s_, tz_axis_present)}};
const asn1::table::seq_desc tx_array_item_s::tleftmost_bottom_array_elem_position_s_::desc = {
    {seq_table_tx_array_item_s_tleftmost_bottom_array_elem_position_s__fields,
     uint64_t{0x0},
     seq_table_tx_array_item_s_tleftmost_bottom_array_elem_position_s__opts,
     seq_table_tx_array_item_s_tleftmost_bottom_array_elem_position_s__names,
     3,
     3},
    false,
    true,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_tx_array_item_s_fields[] = {
    {offsetof(tx_array_item_s, tname),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_array_item_s::tname), asn1::table::string_json>::value},
    {offsetof(tx_array_item_s, tnof_rows),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(tx_array_item_s, tnof_columns),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(tx_array_item_s, tnof_array_layers),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{1},
     uint64_t{254},
     nullptr},
    {offsetof(tx_array_item_s, thorizontal_spacing),
     (int32_t)offsetof(tx_array_item_s, thorizontal_spacing_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_array_item_s::thorizontal_spacing), asn1::table::float_json>::value},
    {offsetof(tx_array_item_s, tvertical_spacing),
     (int32_t)offsetof(tx_array_item_s, tvertical_spacing_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_array_item_s::tvertical_spacing), asn1::table::float_json>::value},
    {offsetof(tx_array_item_s, tnormal_vector_direction),
     (int32_t)offsetof(tx_array_item_s, tnormal_vector_direction_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tx_array_item_s::tnormal_vector_direction_s_::desc},
    {offsetof(tx_array_item_s, tleftmost_bottom_array_elem_position),
     (int32_t)offsetof(tx_array_item_s, tleftmost_bottom_array_elem_position_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &tx_array_item_s::tleftmost_bottom_array_elem_position_s_::desc},
    {offsetof(tx_array_item_s, tpolarization_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(tx_array_item_s::tpolarization_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &t_polarization_item_s::desc},
    {offsetof(tx_array_item_s, tband_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{1},
     uint64_t{1023},
     nullptr},
    {offsetof(tx_array_item_s, tmax_gain),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_array_item_s::tmax_gain), asn1::table::float_json>::value},
    {offsetof(tx_array_item_s, tmin_gain),
     (int32_t)offsetof(tx_array_item_s, tmin_gain_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(tx_array_item_s::tmin_gain), asn1::table::float_json>::value},
    {offsetof(tx_array_item_s, tindependent_pwr_budget),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_tx_array_item_s_names[] = {
    {"tName", nullptr},
    {"tNumberOfRows", nullptr},
    {"tNumberOfColumns", nullptr},
    {"tNumberOfArrayLayers", nullptr},
    {"tHorizontalSpacing", nullptr},
    {"tVerticalSpacing", nullptr},
    {"tNormalVectorDirection", nullptr},
    {"tLeftmostBottomArrayElementPosition", nullptr},
    {"tPolarizationList", nullptr},
    {"tBandNumber", nullptr},
    {"tMaxGain", nullptr},
    {"tMinGain", nullptr},
    {"tIndependentPowerBudget", nullptr}};
static const asn1::table::seq_opt seq_table_tx_array_item_s_opts[] = {
    {4, (int32_t)offsetof(tx_array_item_s, thorizontal_spacing_present)},
    {5, (int32_t)offsetof(tx_array_item_s, tvertical_spacing_present)},
    {6, (int32_t)offsetof(tx_array_item_s, tnormal_vector_direction_present)},
    {7, (int32_t)offsetof(tx_array_item_s, tleftmost_bottom_array_elem_position_present)},
    {11, (int32_t)offsetof(tx_array_item_s, tmin_gain_present)}};
const asn1::table::seq_desc tx_array_item_s::desc = {{seq_table_tx_array_item_s_fields,
                                                      uint64_t{0x170f},
                                                      seq_table_tx_array_item_s_opts,
                                                      seq_table_tx_array_item_s_names,
                                                      13,
                                                      5},
                                                     false,
                                                     true,
                                                     nullptr,
                                                     0};

// RPolarizationItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_r_polarization_item_s_fields[] = {
    {offsetof(r_polarization_item_s, rpolarization_idx),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(r_polarization_item_s, rpolarization_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(r_polarization_item_s::rpolarization_type)>(),
     decltype(r_polarization_item_s::rpolarization_type)::has_ext,
     false,
     decltype(r_polarization_item_s::rpolarization_type)::nof_types,
     decltype(r_polarization_item_s::rpolarization_type)::nof_exts,
     nullptr,
     &r_polarization_item_s::rpolarization_type_opts::desc}};
static const asn1::table::field_name seq_table_r_polarization_item_s_names[] = {{"rPolarizationIndex", nullptr},
                                                                                {"rPolarizationType", nullptr}};
const asn1::table::seq_desc          r_polarization_item_s::desc             = {
    {seq_table_r_polarization_item_s_fields, uint64_t{0x3}, nullptr, seq_table_r_polarization_item_s_names, 2, 0},
    false,
    true,
    nullptr,
    0};

static const char*           enum_names_r_polarization_item_s_rpolarization_type_e__values[] = {"MINUS_45",
                                                                                                "ZERO",
                                                                                                "PLUS_45",
                                                                                                "PLUS_90"};
const asn1::table::enum_desc r_polarization_item_s::rpolarization_type_opts::desc            = {
    enum_names_r_polarization_item_s_rpolarization_type_e__values,
    4,
    "r_polarization_item_s::rpolarization_type_e_"};

// RxArrayItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_rx_array_item_s_rnormal_vector_direction_s__fields[] = {
    {offsetof(rx_array_item_s::rnormal_vector_direction_s_, razimuth_angle),
     (int32_t)offsetof(rx_array_item_s::rnormal_vector_direction_s_, razimuth_angle_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_array_item_s::rnormal_vector_direction_s_::razimuth_angle),
                                asn1::table::float_json>::value},
    {offsetof(rx_array_item_s::rnormal_vector_direction_s_, rzenith_angle),
     (int32_t)offsetof(rx_array_item_s::rnormal_vector_direction_s_, rzenith_angle_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_array_item_s::rnormal_vector_direction_s_::rzenith_angle),
                                asn1::table::float_json>::value}};
static const asn1::table::field_name seq_table_rx_array_item_s_rnormal_vector_direction_s__names[] = {
    {"rAzimuthAngle", nullptr},
    {"rZenithAngle", nullptr}};
static const asn1::table::seq_opt seq_table_rx_array_item_s_rnormal_vector_direction_s__opts[] = {
    {0, (int32_t)offsetof(rx_array_item_s::rnormal_vector_direction_s_, razimuth_angle_present)},
    {1, (int32_t)offsetof(rx_array_item_s::rnormal_vector_direction_s_, rzenith_angle_present)}};
const asn1::table::seq_desc rx_array_item_s::rnormal_vector_direction_s_::desc = {
    {seq_table_rx_array_item_s_rnormal_vector_direction_s__fields,
     uint64_t{0x0},
     seq_table_rx_array_item_s_rnormal_vector_direction_s__opts,
     seq_table_rx_array_item_s_rnormal_vector_direction_s__names,
     2,
     2},
    false,
    true,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_rx_array_item_s_rleftmost_bottom_array_elem_position_s__fields[] = {
    {offsetof(rx_array_item_s::rleftmost_bottom_array_elem_position_s_, rx_axis),
     (int32_t)offsetof(rx_array_item_s::rleftmost_bottom_array_elem_position_s_, rx_axis_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_array_item_s::rleftmost_bottom_array_elem_position_s_::rx_axis),
                                asn1::table::float_json>::value},
    {offsetof(rx_array_item_s::rleftmost_bottom_array_elem_position_s_, ry_axis),
     (int32_t)offsetof(rx_array_item_s::rleftmost_bottom_array_elem_position_s_, ry_axis_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_array_item_s::rleftmost_bottom_array_elem_position_s_::ry_axis),
                                asn1::table::float_json>::value},
    {offsetof(rx_array_item_s::rleftmost_bottom_array_elem_position_s_, rz_axis),
     (int32_t)offsetof(rx_array_item_s::rleftmost_bottom_array_elem_position_s_, rz_axis_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_array_item_s::rleftmost_bottom_array_elem_position_s_::rz_axis),
                                asn1::table::float_json>::value}};
static const asn1::table::field_name seq_table_rx_array_item_s_rleftmost_bottom_array_elem_position_s__names[] = {
    {"rXAxis", nullptr},
    {"rYAxis", nullptr},
    {"rZAxis", nullptr}};
static const asn1::table::seq_opt seq_table_rx_array_item_s_rleftmost_bottom_array_elem_position_s__opts[] = {
    {0, (int32_t)offsetof(rx_array_item_s::rleftmost_bottom_array_elem_position_s_, rx_axis_present)},
    {1, (int32_t)offsetof(rx_array_item_s::rleftmost_bottom_array_elem_position_s_, ry_axis_present)},
    {2, (int32_t)offsetof(rx_array_item_s::rleftmost_bottom_array_elem_position_s_, rz_axis_present)}};
const asn1::table::seq_desc rx_array_item_s::rleftmost_bottom_array_elem_position_s_::desc = {
    {seq_table_rx_array_item_s_rleftmost_bottom_array_elem_position_s__fields,
     uint64_t{0x0},
     seq_table_rx_array_item_s_rleftmost_bottom_array_elem_position_s__opts,
     seq_table_rx_array_item_s_rleftmost_bottom_array_elem_position_s__names,
     3,
     3},
    false,
    true,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_rx_array_item_s_rgain_correction_range_s__fields[] = {
    {offsetof(rx_array_item_s::rgain_correction_range_s_, rmax),
     (int32_t)offsetof(rx_array_item_s::rgain_correction_range_s_, rmax_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_array_item_s::rgain_correction_range_s_::rmax),
                                asn1::table::float_json>::value},
    {offsetof(rx_array_item_s::rgain_correction_range_s_, rmin),
     (int32_t)offsetof(rx_array_item_s::rgain_correction_range_s_, rmin_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_array_item_s::rgain_correction_range_s_::rmin),
                                asn1::table::float_json>::value}};
static const asn1::table::field_name seq_table_rx_array_item_s_rgain_correction_range_s__names[] = {{"rMax", nullptr},
                                                                                                    {"rMin", nullptr}};
static const asn1::table::seq_opt    seq_table_rx_array_item_s_rgain_correction_range_s__opts[]  = {
    {0, (int32_t)offsetof(rx_array_item_s::rgain_correction_range_s_, rmax_present)},
    {1, (int32_t)offsetof(rx_array_item_s::rgain_correction_range_s_, rmin_present)}};
const asn1::table::seq_desc rx_array_item_s::rgain_correction_range_s_::desc = {
    {seq_table_rx_array_item_s_rgain_correction_range_s__fields,
     uint64_t{0x0},
     seq_table_rx_array_item_s_rgain_correction_range_s__opts,
     seq_table_rx_array_item_s_rgain_correction_range_s__names,
     2,
     2},
    false,
    true,
    nullptr,
    0};
static const asn1::table::seq_field seq_table_rx_array_item_s_fields[] = {
    {offsetof(rx_array_item_s, rname),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_array_item_s::rname), asn1::table::string_json>::value},
    {offsetof(rx_array_item_s, rnof_rows),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(rx_array_item_s, rnof_columns),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(rx_array_item_s, rnof_array_layers),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{1},
     uint64_t{254},
     nullptr},
    {offsetof(rx_array_item_s, rhorizontal_spacing),
     (int32_t)offsetof(rx_array_item_s, rhorizontal_spacing_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_array_item_s::rhorizontal_spacing), asn1::table::float_json>::value},
    {offsetof(rx_array_item_s, rvertical_spacing),
     (int32_t)offsetof(rx_array_item_s, rvertical_spacing_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rx_array_item_s::rvertical_spacing), asn1::table::float_json>::value},
    {offsetof(rx_array_item_s, rnormal_vector_direction),
     (int32_t)offsetof(rx_array_item_s, rnormal_vector_direction_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rx_array_item_s::rnormal_vector_direction_s_::desc},
    {offsetof(rx_array_item_s, rleftmost_bottom_array_elem_position),
     (int32_t)offsetof(rx_array_item_s, rleftmost_bottom_array_elem_position_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rx_array_item_s::rleftmost_bottom_array_elem_position_s_::desc},
    {offsetof(rx_array_item_s, rpolarization_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(rx_array_item_s::rpolarization_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &r_polarization_item_s::desc},
    {offsetof(rx_array_item_s, rband_num),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{1},
     uint64_t{1023},
     nullptr},
    {offsetof(rx_array_item_s, rgain_correction_range),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rx_array_item_s::rgain_correction_range_s_::desc}};
static const asn1::table::field_name seq_table_rx_array_item_s_names[] = {
    {"rName", nullptr},
    {"rNumberOfRows", nullptr},
    {"rNumberOfColumns", nullptr},
    {"rNumberOfArrayLayers", nullptr},
    {"rHorizontalSpacing", nullptr},
    {"rVerticalSpacing", nullptr},
    {"rNormalVectorDirection", nullptr},
    {"rLeftmostBottomArrayElementPosition", nullptr},
    {"rPolarizationList", nullptr},
    {"rBandNumber", nullptr},
    {"rGainCorrectionRange", nullptr}};
static const asn1::table::seq_opt seq_table_rx_array_item_s_opts[] = {
    {4, (int32_t)offsetof(rx_array_item_s, rhorizontal_spacing_present)},
    {5, (int32_t)offsetof(rx_array_item_s, rvertical_spacing_present)},
    {6, (int32_t)offsetof(rx_array_item_s, rnormal_vector_direction_present)},
    {7, (int32_t)offsetof(rx_array_item_s, rleftmost_bottom_array_elem_position_present)}};
const asn1::table::seq_desc rx_array_item_s::desc = {{seq_table_rx_array_item_s_fields,
                                                      uint64_t{0x70f},
                                                      seq_table_rx_array_item_s_opts,
                                                      seq_table_rx_array_item_s_names,
                                                      11,
                                                      4},
                                                     false,
                                                     true,
                                                     nullptr,
                                                     0};

// OruUserPlaneConfiguration ::= SEQUENCE
static const asn1::table::seq_field seq_table_oru_user_plane_cfg_s_fields[] = {
    {offsetof(oru_user_plane_cfg_s, tx_array_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(oru_user_plane_cfg_s::tx_array_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &tx_array_item_s::desc},
    {offsetof(oru_user_plane_cfg_s, rx_array_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(oru_user_plane_cfg_s::rx_array_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &rx_array_item_s::desc}};
static const asn1::table::field_name seq_table_oru_user_plane_cfg_s_names[] = {{"txArrayList", nullptr},
                                                                               {"rxArrayList", nullptr}};
static const asn1::table::seq_opt seq_table_oru_user_plane_cfg_s_opts[] = {{0, asn1::table::seq_field::list_presence},
                                                                           {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       oru_user_plane_cfg_s::desc            = {{seq_table_oru_user_plane_cfg_s_fields,
                                                                            uint64_t{0x0},
                                                                            seq_table_oru_user_plane_cfg_s_opts,
                                                                            seq_table_oru_user_plane_cfg_s_names,
                                                                            2,
                                                                            2},
                                                                           false,
                                                                           true,
                                                                           nullptr,
                                                                           0};

// EnergySavingCapabilityCommonInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_energy_saving_cap_common_info_s_fields[] = {
    {offsetof(energy_saving_cap_common_info_s, st8_ready_msg_supported),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(energy_saving_cap_common_info_s, sleep_dur_ext_supported),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(energy_saving_cap_common_info_s, emergency_wake_up_cmd_supported),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_energy_saving_cap_common_info_s_names[] = {
    {"ST8-ready-message-supported", nullptr},
    {"sleep-duration-extension-supported", nullptr},
    {"emergency-wake-up-command-supported", nullptr}};
const asn1::table::seq_desc energy_saving_cap_common_info_s::desc = {{seq_table_energy_saving_cap_common_info_s_fields,
                                                                      uint64_t{0x7},
                                                                      nullptr,
                                                                      seq_table_energy_saving_cap_common_info_s_names,
                                                                      3,
                                                                      0},
                                                                     false,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// SupportedTrxControlMask ::= SEQUENCE
static const asn1::table::seq_field seq_table_supported_trx_ctrl_mask_s_fields[] = {
    {offsetof(supported_trx_ctrl_mask_s, mask_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(supported_trx_ctrl_mask_s::mask_name), asn1::table::string_json>::value},
    {offsetof(supported_trx_ctrl_mask_s, ant_mask),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_supported_trx_ctrl_mask_s_names[] = {{"maskName", nullptr},
                                                                                    {"antennaMask", nullptr}};
const asn1::table::seq_desc          supported_trx_ctrl_mask_s::desc = {{seq_table_supported_trx_ctrl_mask_s_fields,
                                                                         uint64_t{0x3},
                                                                         nullptr,
                                                                         seq_table_supported_trx_ctrl_mask_s_names,
                                                                         2,
                                                                         0},
                                                                        false,
                                                                        true,
                                                                        nullptr,
                                                                        0};

// TrxControlCapabilityInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_trx_ctrl_cap_info_s_fields[] = {
    {offsetof(trx_ctrl_cap_info_s, supported_trx_ctrl_masks),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(trx_ctrl_cap_info_s::supported_trx_ctrl_masks),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &supported_trx_ctrl_mask_s::desc},
    {offsetof(trx_ctrl_cap_info_s, sleep_modes),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(trx_ctrl_cap_info_s::sleep_modes),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &sleep_mode_s::desc},
    {offsetof(trx_ctrl_cap_info_s, defined_dur_sleep_supported),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(trx_ctrl_cap_info_s, undefined_dur_sleep_supported),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr}};
static const asn1::table::field_name seq_table_trx_ctrl_cap_info_s_names[] = {
    {"supportedTrxControlMasks", nullptr},
    {"sleepModes", nullptr},
    {"definedDurationSleepSupported", nullptr},
    {"undefinedDurationSleepSupported", nullptr}};
const asn1::table::seq_desc trx_ctrl_cap_info_s::desc = {
    {seq_table_trx_ctrl_cap_info_s_fields, uint64_t{0xf}, nullptr, seq_table_trx_ctrl_cap_info_s_names, 4, 0},
    false,
    true,
    nullptr,
    0};

// DeepHibernateCapabilityInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_deep_hibernate_cap_info_s_fields[] = {
    {offsetof(deep_hibernate_cap_info_s, max_hibernate_dur),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr},
    {offsetof(deep_hibernate_cap_info_s, min_hibernate_dur),
     (int32_t)offsetof(deep_hibernate_cap_info_s, min_hibernate_dur_present),
     asn1::table::field_op::int_u64,
     false,
     true,
     int64_t{0},
     uint64_t{4294967295},
     nullptr}};
static const asn1::table::field_name seq_table_deep_hibernate_cap_info_s_names[] = {{"maxHibernateDuration", nullptr},
                                                                                    {"minHibernateDuration", nullptr}};
static const asn1::table::seq_opt    seq_table_deep_hibernate_cap_info_s_opts[]  = {
    {1, (int32_t)offsetof(deep_hibernate_cap_info_s, min_hibernate_dur_present)}};
const asn1::table::seq_desc deep_hibernate_cap_info_s::desc = {{seq_table_deep_hibernate_cap_info_s_fields,
                                                                uint64_t{0x1},
                                                                seq_table_deep_hibernate_cap_info_s_opts,
                                                                seq_table_deep_hibernate_cap_info_s_names,
                                                                2,
                                                                1},
                                                               false,
                                                               true,
                                                               nullptr,
                                                               0};

// OruCapabilities ::= SEQUENCE
static const asn1::table::seq_field seq_table_oru_cap_s_fields[] = {
    {offsetof(oru_cap_s, energy_saving_cap_common_info),
     (int32_t)offsetof(oru_cap_s, energy_saving_cap_common_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &energy_saving_cap_common_info_s::desc},
    {offsetof(oru_cap_s, asm_cap_info),
     (int32_t)offsetof(oru_cap_s, asm_cap_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asm_cap_info_s::desc},
    {offsetof(oru_cap_s, trx_ctrl_cap_info),
     (int32_t)offsetof(oru_cap_s, trx_ctrl_cap_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trx_ctrl_cap_info_s::desc},
    {offsetof(oru_cap_s, deep_hibernate_cap_info),
     (int32_t)offsetof(oru_cap_s, deep_hibernate_cap_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &deep_hibernate_cap_info_s::desc}};
static const asn1::table::field_name seq_table_oru_cap_s_names[] = {{"energySavingCapabilityCommonInfo", nullptr},
                                                                    {"asmCapabilityInfo", nullptr},
                                                                    {"trxControlCapabilityInfo", nullptr},
                                                                    {"deepHibernateCapabilityInfo", nullptr}};
static const asn1::table::seq_opt    seq_table_oru_cap_s_opts[]  = {
    {0, (int32_t)offsetof(oru_cap_s, energy_saving_cap_common_info_present)},
    {1, (int32_t)offsetof(oru_cap_s, asm_cap_info_present)},
    {2, (int32_t)offsetof(oru_cap_s, trx_ctrl_cap_info_present)},
    {3, (int32_t)offsetof(oru_cap_s, deep_hibernate_cap_info_present)}};
const asn1::table::seq_desc oru_cap_s::desc = {
    {seq_table_oru_cap_s_fields, uint64_t{0x0}, seq_table_oru_cap_s_opts, seq_table_oru_cap_s_names, 4, 4},
    false,
    true,
    nullptr,
    0};

// O-RUInfo ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_ru_info_s_fields[] = {
    {offsetof(o_ru_info_s, oru_user_plane_cfg),
     (int32_t)offsetof(o_ru_info_s, oru_user_plane_cfg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &oru_user_plane_cfg_s::desc},
    {offsetof(o_ru_info_s, oru_cap),
     (int32_t)offsetof(o_ru_info_s, oru_cap_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &oru_cap_s::desc}};
static const asn1::table::field_name seq_table_o_ru_info_s_names[] = {{"oruUserPlaneConfiguration", nullptr},
                                                                      {"oruCapabilities", nullptr}};
static const asn1::table::seq_opt    seq_table_o_ru_info_s_opts[]  = {
    {0, (int32_t)offsetof(o_ru_info_s, oru_user_plane_cfg_present)},
    {1, (int32_t)offsetof(o_ru_info_s, oru_cap_present)}};
const asn1::table::seq_desc o_ru_info_s::desc = {
    {seq_table_o_ru_info_s_fields, uint64_t{0x0}, seq_table_o_ru_info_s_opts, seq_table_o_ru_info_s_names, 2, 2},
    false,
    true,
    nullptr,
    0};

// PrbBlankingConfigItem ::= SEQUENCE
static const asn1::table::seq_field seq_table_prb_blanking_cfg_item_s_fields[] = {
    {offsetof(prb_blanking_cfg_item_s, bwp_idx),
     (int32_t)offsetof(prb_blanking_cfg_item_s, bwp_idx_present),
     asn1::table::field_op::int_u8,
     false,
     true,
     int64_t{0},
     uint64_t{3},
     nullptr},
    {offsetof(prb_blanking_cfg_item_s, frame_num),
     (int32_t)offsetof(prb_blanking_cfg_item_s, frame_num_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prb_blanking_cfg_item_s::frame_num), asn1::table::string_json>::value},
    {offsetof(prb_blanking_cfg_item_s, slot_num),
     (int32_t)offsetof(prb_blanking_cfg_item_s, slot_num_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prb_blanking_cfg_item_s::slot_num), asn1::table::string_json>::value},
    {offsetof(prb_blanking_cfg_item_s, prb_idx),
     (int32_t)offsetof(prb_blanking_cfg_item_s, prb_idx_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(prb_blanking_cfg_item_s::prb_idx), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_prb_blanking_cfg_item_s_names[] = {{"bwpIndex", nullptr},
                                                                                  {"frameNumber", nullptr},
                                                                                  {"slotNumber", nullptr},
                                                                                  {"prbIndex", nullptr}};
static const asn1::table::seq_opt    seq_table_prb_blanking_cfg_item_s_opts[]  = {
    {0, (int32_t)offsetof(prb_blanking_cfg_item_s, bwp_idx_present)},
    {1, (int32_t)offsetof(prb_blanking_cfg_item_s, frame_num_present)},
    {2, (int32_t)offsetof(prb_blanking_cfg_item_s, slot_num_present)},
    {3, (int32_t)offsetof(prb_blanking_cfg_item_s, prb_idx_present)}};
const asn1::table::seq_desc prb_blanking_cfg_item_s::desc = {{seq_table_prb_blanking_cfg_item_s_fields,
                                                              uint64_t{0x0},
                                                              seq_table_prb_blanking_cfg_item_s_opts,
                                                              seq_table_prb_blanking_cfg_item_s_names,
                                                              4,
                                                              4},
                                                             false,
                                                             true,
                                                             nullptr,
                                                             0};

// O-PRBBlankingPolicy ::= SEQUENCE
static const asn1::table::seq_field seq_table_o_prb_blanking_policy_s_fields[] = {
    {offsetof(o_prb_blanking_policy_s, policy_category),
     (int32_t)offsetof(o_prb_blanking_policy_s, policy_category_present),
     asn1::table::enum_op<decltype(o_prb_blanking_policy_s::policy_category)>(),
     decltype(o_prb_blanking_policy_s::policy_category)::has_ext,
     false,
     decltype(o_prb_blanking_policy_s::policy_category)::nof_types,
     decltype(o_prb_blanking_policy_s::policy_category)::nof_exts,
     nullptr,
     &o_prb_blanking_policy_s::policy_category_opts::desc},
    {offsetof(o_prb_blanking_policy_s, link_dir),
     (int32_t)offsetof(o_prb_blanking_policy_s, link_dir_present),
     asn1::table::enum_op<decltype(o_prb_blanking_policy_s::link_dir)>(),
     decltype(o_prb_blanking_policy_s::link_dir)::has_ext,
     false,
     decltype(o_prb_blanking_policy_s::link_dir)::nof_types,
     decltype(o_prb_blanking_policy_s::link_dir)::nof_exts,
     nullptr,
     &o_prb_blanking_policy_s::link_dir_opts::desc},
    {offsetof(o_prb_blanking_policy_s, start_time),
     (int32_t)offsetof(o_prb_blanking_policy_s, start_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(o_prb_blanking_policy_s::start_time), asn1::table::string_json>::value},
    {offsetof(o_prb_blanking_policy_s, end_time),
     (int32_t)offsetof(o_prb_blanking_policy_s, end_time_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(o_prb_blanking_policy_s::end_time), asn1::table::string_json>::value},
    {offsetof(o_prb_blanking_policy_s, prb_blanking_cfg_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(o_prb_blanking_policy_s::prb_blanking_cfg_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &prb_blanking_cfg_item_s::desc},
    {offsetof(o_prb_blanking_policy_s, prb_blanking_obj),
     (int32_t)offsetof(o_prb_blanking_policy_s, prb_blanking_obj_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(o_prb_blanking_policy_s::prb_blanking_obj), asn1::table::float_json>::value}};
static const asn1::table::field_name seq_table_o_prb_blanking_policy_s_names[] = {
    {"policyCategory", nullptr},
    {"linkDir", nullptr},
    {"startTime", nullptr},
    {"endTime", nullptr},
    {"prbBlankingConfigurationList", nullptr},
    {"prbBlankingObj", nullptr}};
static const asn1::table::seq_opt seq_table_o_prb_blanking_policy_s_opts[] = {
    {0, (int32_t)offsetof(o_prb_blanking_policy_s, policy_category_present)},
    {1, (int32_t)offsetof(o_prb_blanking_policy_s, link_dir_present)},
    {2, (int32_t)offsetof(o_prb_blanking_policy_s, start_time_present)},
    {3, (int32_t)offsetof(o_prb_blanking_policy_s, end_time_present)},
    {4, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(o_prb_blanking_policy_s, prb_blanking_obj_present)}};
const asn1::table::seq_desc o_prb_blanking_policy_s::desc = {{seq_table_o_prb_blanking_policy_s_fields,
                                                              uint64_t{0x0},
                                                              seq_table_o_prb_blanking_policy_s_opts,
                                                              seq_table_o_prb_blanking_policy_s_names,
                                                              6,
                                                              6},
                                                             false,
                                                             true,
                                                             nullptr,
                                                             0};

static const char* enum_names_o_prb_blanking_policy_s_policy_category_e__values[] = {"prbBlankingConfiguration",
                                                                                     "prbBlankingObjective"};
const asn1::table::enum_desc o_prb_blanking_policy_s::policy_category_opts::desc  = {
    enum_names_o_prb_blanking_policy_s_policy_category_e__values,
    2,
    "o_prb_blanking_policy_s::policy_category_e_"};

static const char*           enum_names_o_prb_blanking_policy_s_link_dir_e__values[] = {"DL", "UL"};
const asn1::table::enum_desc o_prb_blanking_policy_s::link_dir_opts::desc            = {
    enum_names_o_prb_blanking_policy_s_link_dir_e__values,
    2,
    "o_prb_blanking_policy_s::link_dir_e_"};

// E2SM-CCC-RAN-Configuration-Structure ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_e2sm_ccc_ran_cfg_structure_c_alternatives[] = {
    &asn1::table::alternative_ops<o_gnb_cu_cp_function_s, false, false>::value,
    &asn1::table::alternative_ops<o_gnb_cu_up_function_s, false, false>::value,
    &asn1::table::alternative_ops<o_gnb_du_function_s, false, false>::value,
    &asn1::table::alternative_ops<o_nr_cell_cu_s, false, false>::value,
    &asn1::table::alternative_ops<o_nr_cell_du_s, false, false>::value,
    &asn1::table::alternative_ops<o_rrm_policy_ratio_s, false, false>::value,
    &asn1::table::alternative_ops<o_bwp_s, false>::value,
    &asn1::table::alternative_ops<o_ces_management_function_s, false>::value,
    &asn1::table::alternative_ops<o_nes_policy_s, false, false>::value,
    &asn1::table::alternative_ops<o_cell_dtxdrx_cfg_s, false>::value,
    &asn1::table::alternative_ops<o_ru_info_s, false, false>::value,
    &asn1::table::alternative_ops<o_prb_blanking_policy_s, false, false>::value};
const asn1::table::choice_storage_desc e2sm_ccc_ran_cfg_structure_c::storage_desc = {
    choice_storage_e2sm_ccc_ran_cfg_structure_c_alternatives,
    12,
    "e2sm_ccc_ran_cfg_structure_c"};

o_gnb_cu_cp_function_s& e2sm_ccc_ran_cfg_structure_c::set_o_gnb_cu_cp_function()
{
  set(types::o_gnb_cu_cp_function);
  return c.get<o_gnb_cu_cp_function_s>();
}
o_gnb_cu_up_function_s& e2sm_ccc_ran_cfg_structure_c::set_o_gnb_cu_up_function()
{
  set(types::o_gnb_cu_up_function);
  return c.get<o_gnb_cu_up_function_s>();
}
o_gnb_du_function_s& e2sm_ccc_ran_cfg_structure_c::set_o_gnb_du_function()
{
  set(types::o_gnb_du_function);
  return c.get<o_gnb_du_function_s>();
}
o_nr_cell_cu_s& e2sm_ccc_ran_cfg_structure_c::set_o_nr_cell_cu()
{
  set(types::o_nr_cell_cu);
  return c.get<o_nr_cell_cu_s>();
}
o_nr_cell_du_s& e2sm_ccc_ran_cfg_structure_c::set_o_nr_cell_du()
{
  set(types::o_nr_cell_du);
  return c.get<o_nr_cell_du_s>();
}
o_rrm_policy_ratio_s& e2sm_ccc_ran_cfg_structure_c::set_o_rrm_policy_ratio()
{
  set(types::o_rrm_policy_ratio);
  return c.get<o_rrm_policy_ratio_s>();
}
o_bwp_s& e2sm_ccc_ran_cfg_structure_c::set_o_bwp()
{
  set(types::o_bwp);
  return c.get<o_bwp_s>();
}
o_ces_management_function_s& e2sm_ccc_ran_cfg_structure_c::set_o_ces_management_function()
{
  set(types::o_ces_management_function);
  return c.get<o_ces_management_function_s>();
}
o_nes_policy_s& e2sm_ccc_ran_cfg_structure_c::set_o_nes_policy()
{
  set(types::o_nes_policy);
  return c.get<o_nes_policy_s>();
}
o_cell_dtxdrx_cfg_s& e2sm_ccc_ran_cfg_structure_c::set_o_cell_dtxdrx_cfg()
{
  set(types::o_cell_dtxdrx_cfg);
  return c.get<o_cell_dtxdrx_cfg_s>();
}
o_ru_info_s& e2sm_ccc_ran_cfg_structure_c::set_o_ru_info()
{
  set(types::o_ru_info);
  return c.get<o_ru_info_s>();
}
o_prb_blanking_policy_s& e2sm_ccc_ran_cfg_structure_c::set_o_prb_blanking_policy()
{
  set(types::o_prb_blanking_policy);
  return c.get<o_prb_blanking_policy_s>();
}
static const asn1::table::seq_field choice_table_e2sm_ccc_ran_cfg_structure_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_gnb_cu_cp_function_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_gnb_cu_up_function_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_gnb_du_function_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_nr_cell_cu_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_nr_cell_du_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_rrm_policy_ratio_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_bwp_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_ces_management_function_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_nes_policy_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_cell_dtxdrx_cfg_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_ru_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &o_prb_blanking_policy_s::desc}};
static const asn1::table::field_name choice_table_e2sm_ccc_ran_cfg_structure_c_names[] = {
    {"o-GnbCuCpFunction", nullptr},
    {"o-GnbCuUpFunction", nullptr},
    {"o-GnbDuFunction", nullptr},
    {"o-NrCellCu", nullptr},
    {"o-NrCellDu", nullptr},
    {"o-RRMPolicyRatio", nullptr},
    {"o-Bwp", nullptr},
    {"o-CESManagementFunction", nullptr},
    {"o-NESPolicy", nullptr},
    {"o-CellDTXDRXConfig", nullptr},
    {"o-RUInfo", nullptr},
    {"o-PRBBlankingPolicy", nullptr}};
const asn1::table::choice_desc e2sm_ccc_ran_cfg_structure_c::desc = {
    asn1::table::enum_int_type<e2sm_ccc_ran_cfg_structure_c::types>(),
    e2sm_ccc_ran_cfg_structure_c::types::nof_types,
    e2sm_ccc_ran_cfg_structure_c::types::nof_exts,
    e2sm_ccc_ran_cfg_structure_c::types::has_ext,
    true,
    choice_table_e2sm_ccc_ran_cfg_structure_c_alternatives,
    nullptr,
    choice_table_e2sm_ccc_ran_cfg_structure_c_names,
    "e2sm_ccc_ran_cfg_structure_c",
    offsetof(e2sm_ccc_ran_cfg_structure_c, c.type),
    offsetof(e2sm_ccc_ran_cfg_structure_c, c.value),
    false,
    &e2sm_ccc_ran_cfg_structure_c::storage_desc};

static const char*           enum_names_e2sm_ccc_ran_cfg_structure_c_types_values[] = {"o-GnbCuCpFunction",
                                                                                       "o-GnbCuUpFunction",
                                                                                       "o-GnbDuFunction",
                                                                                       "o-NrCellCu",
                                                                                       "o-NrCellDu",
                                                                                       "o-RRMPolicyRatio",
                                                                                       "o-Bwp",
                                                                                       "o-CESManagementFunction",
                                                                                       "o-NESPolicy",
                                                                                       "o-CellDTXDRXConfig",
                                                                                       "o-RUInfo",
                                                                                       "o-PRBBlankingPolicy"};
const asn1::table::enum_desc e2sm_ccc_ran_cfg_structure_c::types_opts::desc         = {
    enum_names_e2sm_ccc_ran_cfg_structure_c_types_values,
    12,
    "e2sm_ccc_ran_cfg_structure_c::types"};

// ValuesOfAttributes ::= SEQUENCE
static const asn1::table::seq_field seq_table_values_of_attributes_s_fields[] = {
    {offsetof(values_of_attributes_s, ran_cfg_structure),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_ran_cfg_structure_c::desc}};
static const asn1::table::field_name seq_table_values_of_attributes_s_names[] = {
    {"ranConfigurationStructure", nullptr}};
const asn1::table::seq_desc values_of_attributes_s::desc = {
    {seq_table_values_of_attributes_s_fields, uint64_t{0x1}, nullptr, seq_table_values_of_attributes_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// ConfigurationStructureAccepted ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfg_structure_accepted_s_fields[] = {
    {offsetof(cfg_structure_accepted_s, ran_cfg_structure_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cfg_structure_accepted_s::ran_cfg_structure_name),
                                asn1::table::string_json>::value},
    {offsetof(cfg_structure_accepted_s, old_values_of_attributes),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &values_of_attributes_s::desc},
    {offsetof(cfg_structure_accepted_s, current_values_of_attributes),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &values_of_attributes_s::desc},
    {offsetof(cfg_structure_accepted_s, applied_timestamp),
     (int32_t)offsetof(cfg_structure_accepted_s, applied_timestamp_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cfg_structure_accepted_s::applied_timestamp),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_cfg_structure_accepted_s_names[] = {
    {"ranConfigurationStructureName", nullptr},
    {"oldValuesOfAttributes", nullptr},
    {"currentValuesOfAttributes", nullptr},
    {"appliedTimestamp", nullptr}};
static const asn1::table::seq_opt seq_table_cfg_structure_accepted_s_opts[] = {
    {3, (int32_t)offsetof(cfg_structure_accepted_s, applied_timestamp_present)}};
const asn1::table::seq_desc cfg_structure_accepted_s::desc = {{seq_table_cfg_structure_accepted_s_fields,
                                                               uint64_t{0x7},
                                                               seq_table_cfg_structure_accepted_s_opts,
                                                               seq_table_cfg_structure_accepted_s_names,
                                                               4,
                                                               1},
                                                              false,
                                                              true,
                                                              nullptr,
                                                              0};

// ConfigurationStructureFailed ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfg_structure_failed_s_fields[] = {
    {offsetof(cfg_structure_failed_s, ran_cfg_structure_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cfg_structure_failed_s::ran_cfg_structure_name),
                                asn1::table::string_json>::value},
    {offsetof(cfg_structure_failed_s, old_values_of_attributes),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &values_of_attributes_s::desc},
    {offsetof(cfg_structure_failed_s, requested_values_of_attributes),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &values_of_attributes_s::desc},
    {offsetof(cfg_structure_failed_s, cause),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(cfg_structure_failed_s::cause)>(),
     decltype(cfg_structure_failed_s::cause)::has_ext,
     false,
     decltype(cfg_structure_failed_s::cause)::nof_types,
     decltype(cfg_structure_failed_s::cause)::nof_exts,
     nullptr,
     &cause_opts::desc}};
static const asn1::table::field_name seq_table_cfg_structure_failed_s_names[] = {
    {"ranConfigurationStructureName", nullptr},
    {"oldValuesOfAttributes", nullptr},
    {"requestedValuesOfAttributes", nullptr},
    {"cause", nullptr}};
const asn1::table::seq_desc cfg_structure_failed_s::desc = {
    {seq_table_cfg_structure_failed_s_fields, uint64_t{0xf}, nullptr, seq_table_cfg_structure_failed_s_names, 4, 0},
    false,
    true,
    nullptr,
    0};

// CellControlOutcome ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_ctrl_outcome_s_fields[] = {
    {offsetof(cell_ctrl_outcome_s, cell_global_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_global_id_c::desc},
    {offsetof(cell_ctrl_outcome_s, ran_cfg_structures_accepted_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(cell_ctrl_outcome_s::ran_cfg_structures_accepted_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cfg_structure_accepted_s::desc},
    {offsetof(cell_ctrl_outcome_s, ran_cfg_structures_failed_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(cell_ctrl_outcome_s::ran_cfg_structures_failed_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cfg_structure_failed_s::desc}};
static const asn1::table::field_name seq_table_cell_ctrl_outcome_s_names[] = {
    {"cellGlobalId", nullptr},
    {"ranConfigurationStructuresAcceptedList", nullptr},
    {"ranConfigurationStructuresFailedList", nullptr}};
static const asn1::table::seq_opt seq_table_cell_ctrl_outcome_s_opts[] = {{1, asn1::table::seq_field::list_presence},
                                                                          {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       cell_ctrl_outcome_s::desc            = {{seq_table_cell_ctrl_outcome_s_fields,
                                                                           uint64_t{0x1},
                                                                           seq_table_cell_ctrl_outcome_s_opts,
                                                                           seq_table_cell_ctrl_outcome_s_names,
                                                                           3,
                                                                           2},
                                                                          false,
                                                                          true,
                                                                          nullptr,
                                                                          0};

// ConfigurationStructureWrite ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfg_structure_write_s_fields[] = {
    {offsetof(cfg_structure_write_s, ran_cfg_structure_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cfg_structure_write_s::ran_cfg_structure_name),
                                asn1::table::string_json>::value},
    {offsetof(cfg_structure_write_s, old_values_of_attributes),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &values_of_attributes_s::desc},
    {offsetof(cfg_structure_write_s, new_values_of_attributes),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &values_of_attributes_s::desc}};
static const asn1::table::field_name seq_table_cfg_structure_write_s_names[] = {
    {"ranConfigurationStructureName", nullptr},
    {"oldValuesOfAttributes", nullptr},
    {"newValuesOfAttributes", nullptr}};
const asn1::table::seq_desc cfg_structure_write_s::desc = {
    {seq_table_cfg_structure_write_s_fields, uint64_t{0x7}, nullptr, seq_table_cfg_structure_write_s_names, 3, 0},
    false,
    true,
    nullptr,
    0};

// CellControlled ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_ctrl_s_fields[] = {
    {offsetof(cell_ctrl_s, cell_global_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_global_id_c::desc},
    {offsetof(cell_ctrl_s, list_of_cfg_structures),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(cell_ctrl_s::list_of_cfg_structures),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cfg_structure_write_s::desc}};
static const asn1::table::field_name seq_table_cell_ctrl_s_names[] = {{"cellGlobalId", nullptr},
                                                                      {"listOfConfigurationStructures", nullptr}};
const asn1::table::seq_desc          cell_ctrl_s::desc             = {
    {seq_table_cell_ctrl_s_fields, uint64_t{0x3}, nullptr, seq_table_cell_ctrl_s_names, 2, 0},
    false,
    true,
    nullptr,
    0};

// RANConfigurationStructure ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_cfg_structure_s_fields[] = {
    {offsetof(ran_cfg_structure_s, ran_cfg_structure_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_cfg_structure_s::ran_cfg_structure_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_cfg_structure_s, list_of_supported_attributes),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(ran_cfg_structure_s::list_of_supported_attributes),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &attribute_s::desc}};
static const asn1::table::field_name seq_table_ran_cfg_structure_s_names[] = {
    {"ranConfigurationStructureName", nullptr},
    {"listOfSupportedAttributes", nullptr}};
static const asn1::table::seq_opt seq_table_ran_cfg_structure_s_opts[] = {{1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       ran_cfg_structure_s::desc            = {{seq_table_ran_cfg_structure_s_fields,
                                                                           uint64_t{0x1},
                                                                           seq_table_ran_cfg_structure_s_opts,
                                                                           seq_table_ran_cfg_structure_s_names,
                                                                           2,
                                                                           1},
                                                                          false,
                                                                          true,
                                                                          nullptr,
                                                                          0};

// CellForRANFunctionDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_for_ran_function_definition_s_fields[] = {
    {offsetof(cell_for_ran_function_definition_s, cell_global_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_global_id_c::desc},
    {offsetof(cell_for_ran_function_definition_s, list_of_supported_cell_level_ran_cfg_structures),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<
         decltype(cell_for_ran_function_definition_s::list_of_supported_cell_level_ran_cfg_structures),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &ran_cfg_structure_s::desc}};
static const asn1::table::field_name seq_table_cell_for_ran_function_definition_s_names[] = {
    {"cellGlobalID", nullptr},
    {"listOfSupportedCellLevelRANConfigurationStructures", nullptr}};
static const asn1::table::seq_opt seq_table_cell_for_ran_function_definition_s_opts[] = {
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc cell_for_ran_function_definition_s::desc = {
    {seq_table_cell_for_ran_function_definition_s_fields,
     uint64_t{0x1},
     seq_table_cell_for_ran_function_definition_s_opts,
     seq_table_cell_for_ran_function_definition_s_names,
     2,
     1},
    false,
    true,
    nullptr,
    0};

// RANConfigurationStructureForEventTrigger ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_cfg_structure_for_event_trigger_s_fields[] = {
    {offsetof(ran_cfg_structure_for_event_trigger_s, ran_cfg_structure_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_cfg_structure_for_event_trigger_s::ran_cfg_structure_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_cfg_structure_for_event_trigger_s, list_of_attributes),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::list_codec<decltype(ran_cfg_structure_for_event_trigger_s::list_of_attributes),
                              asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_ran_cfg_structure_for_event_trigger_s_names[] = {
    {"ranConfigurationStructureName", nullptr},
    {"listOfAttributes", nullptr}};
static const asn1::table::seq_opt seq_table_ran_cfg_structure_for_event_trigger_s_opts[] = {
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_cfg_structure_for_event_trigger_s::desc = {
    {seq_table_ran_cfg_structure_for_event_trigger_s_fields,
     uint64_t{0x1},
     seq_table_ran_cfg_structure_for_event_trigger_s_opts,
     seq_table_ran_cfg_structure_for_event_trigger_s_names,
     2,
     1},
    false,
    true,
    nullptr,
    0};

// CellLevelConfigurationStructureForEventTrigger ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_level_cfg_structure_for_event_trigger_s_fields[] = {
    {offsetof(cell_level_cfg_structure_for_event_trigger_s, cell_global_id),
     (int32_t)offsetof(cell_level_cfg_structure_for_event_trigger_s, cell_global_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_global_id_c::desc},
    {offsetof(cell_level_cfg_structure_for_event_trigger_s, list_of_ran_cfg_structures_for_event_trigger),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<
         decltype(cell_level_cfg_structure_for_event_trigger_s::list_of_ran_cfg_structures_for_event_trigger),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &ran_cfg_structure_for_event_trigger_s::desc}};
static const asn1::table::field_name seq_table_cell_level_cfg_structure_for_event_trigger_s_names[] = {
    {"cellGlobalId", nullptr},
    {"listOfRANConfigurationStructuresForEventTrigger", nullptr}};
static const asn1::table::seq_opt seq_table_cell_level_cfg_structure_for_event_trigger_s_opts[] = {
    {0, (int32_t)offsetof(cell_level_cfg_structure_for_event_trigger_s, cell_global_id_present)}};
const asn1::table::seq_desc cell_level_cfg_structure_for_event_trigger_s::desc = {
    {seq_table_cell_level_cfg_structure_for_event_trigger_s_fields,
     uint64_t{0x2},
     seq_table_cell_level_cfg_structure_for_event_trigger_s_opts,
     seq_table_cell_level_cfg_structure_for_event_trigger_s_names,
     2,
     1},
    false,
    true,
    nullptr,
    0};

// ConfigurationStructure ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfg_structure_s_fields[] = {
    {offsetof(cfg_structure_s, change_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(cfg_structure_s::change_type)>(),
     decltype(cfg_structure_s::change_type)::has_ext,
     false,
     decltype(cfg_structure_s::change_type)::nof_types,
     decltype(cfg_structure_s::change_type)::nof_exts,
     nullptr,
     &cfg_structure_s::change_type_opts::desc},
    {offsetof(cfg_structure_s, ran_cfg_structure_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cfg_structure_s::ran_cfg_structure_name), asn1::table::string_json>::value},
    {offsetof(cfg_structure_s, values_of_attributes),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &values_of_attributes_s::desc},
    {offsetof(cfg_structure_s, old_values_of_attributes),
     (int32_t)offsetof(cfg_structure_s, old_values_of_attributes_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &values_of_attributes_s::desc}};
static const asn1::table::field_name seq_table_cfg_structure_s_names[] = {{"changeType", nullptr},
                                                                          {"ranConfigurationStructureName", nullptr},
                                                                          {"valuesOfAttributes", nullptr},
                                                                          {"oldValuesOfAttributes", nullptr}};
static const asn1::table::seq_opt    seq_table_cfg_structure_s_opts[]  = {
    {3, (int32_t)offsetof(cfg_structure_s, old_values_of_attributes_present)}};
const asn1::table::seq_desc cfg_structure_s::desc = {{seq_table_cfg_structure_s_fields,
                                                      uint64_t{0x7},
                                                      seq_table_cfg_structure_s_opts,
                                                      seq_table_cfg_structure_s_names,
                                                      4,
                                                      1},
                                                     false,
                                                     true,
                                                     nullptr,
                                                     0};

static const char*           enum_names_cfg_structure_s_change_type_e__values[] = {"none",
                                                                                   "modification",
                                                                                   "addition",
                                                                                   "deletion"};
const asn1::table::enum_desc cfg_structure_s::change_type_opts::desc            = {
    enum_names_cfg_structure_s_change_type_e__values,
    4,
    "cfg_structure_s::change_type_e_"};

// CellReported ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_reported_s_fields[] = {
    {offsetof(cell_reported_s, cell_global_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_global_id_c::desc},
    {offsetof(cell_reported_s, list_of_cfg_structures_reported),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(cell_reported_s::list_of_cfg_structures_reported),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cfg_structure_s::desc}};
static const asn1::table::field_name seq_table_cell_reported_s_names[] = {
    {"cellGlobalId", nullptr},
    {"listOfConfigurationStructuresReported", nullptr}};
const asn1::table::seq_desc cell_reported_s::desc = {
    {seq_table_cell_reported_s_fields, uint64_t{0x3}, nullptr, seq_table_cell_reported_s_names, 2, 0},
    false,
    true,
    nullptr,
    0};

// CelllevelConfigurationStructuresForQuery ::= SEQUENCE
static const asn1::table::seq_field seq_table_celllevel_cfg_structures_for_query_s_fields[] = {
    {offsetof(celllevel_cfg_structures_for_query_s, ran_cfg_structure_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(celllevel_cfg_structures_for_query_s::ran_cfg_structure_name),
                                asn1::table::string_json>::value},
    {offsetof(celllevel_cfg_structures_for_query_s, list_of_attributes),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(celllevel_cfg_structures_for_query_s::list_of_attributes),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &attribute_name_s::desc}};
static const asn1::table::field_name seq_table_celllevel_cfg_structures_for_query_s_names[] = {
    {"ranConfigurationStructureName", nullptr},
    {"listOfAttributes", nullptr}};
static const asn1::table::seq_opt seq_table_celllevel_cfg_structures_for_query_s_opts[] = {
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc celllevel_cfg_structures_for_query_s::desc = {
    {seq_table_celllevel_cfg_structures_for_query_s_fields,
     uint64_t{0x1},
     seq_table_celllevel_cfg_structures_for_query_s_opts,
     seq_table_celllevel_cfg_structures_for_query_s_names,
     2,
     1},
    false,
    true,
    nullptr,
    0};

// CellsToBeQueried ::= SEQUENCE
static const asn1::table::seq_field seq_table_cells_to_be_queried_s_fields[] = {
    {offsetof(cells_to_be_queried_s, cell_global_id),
     (int32_t)offsetof(cells_to_be_queried_s, cell_global_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_global_id_c::desc},
    {offsetof(cells_to_be_queried_s, list_of_celllevel_cfg_structures_for_query),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(cells_to_be_queried_s::list_of_celllevel_cfg_structures_for_query),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &celllevel_cfg_structures_for_query_s::desc}};
static const asn1::table::field_name seq_table_cells_to_be_queried_s_names[] = {
    {"cellGlobalId", nullptr},
    {"listOfCelllevelConfigurationStructuresForQuery", nullptr}};
static const asn1::table::seq_opt seq_table_cells_to_be_queried_s_opts[] = {
    {0, (int32_t)offsetof(cells_to_be_queried_s, cell_global_id_present)}};
const asn1::table::seq_desc cells_to_be_queried_s::desc = {{seq_table_cells_to_be_queried_s_fields,
                                                            uint64_t{0x2},
                                                            seq_table_cells_to_be_queried_s_opts,
                                                            seq_table_cells_to_be_queried_s_names,
                                                            2,
                                                            1},
                                                           false,
                                                           true,
                                                           nullptr,
                                                           0};

// ConfigurationStructuresToBeReportedForQuery ::= SEQUENCE
static const asn1::table::seq_field seq_table_cfg_structures_to_be_reported_for_query_s_fields[] = {
    {offsetof(cfg_structures_to_be_reported_for_query_s, ran_cfg_structure_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cfg_structures_to_be_reported_for_query_s::ran_cfg_structure_name),
                                asn1::table::string_json>::value},
    {offsetof(cfg_structures_to_be_reported_for_query_s, values_of_attributes),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &values_of_attributes_s::desc}};
static const asn1::table::field_name seq_table_cfg_structures_to_be_reported_for_query_s_names[] = {
    {"ranConfigurationStructureName", nullptr},
    {"valuesOfAttributes", nullptr}};
const asn1::table::seq_desc cfg_structures_to_be_reported_for_query_s::desc = {
    {seq_table_cfg_structures_to_be_reported_for_query_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_cfg_structures_to_be_reported_for_query_s_names,
     2,
     0},
    false,
    true,
    nullptr,
    0};

// CellsToBeReportedForQuery ::= SEQUENCE
static const asn1::table::seq_field seq_table_cells_to_be_reported_for_query_s_fields[] = {
    {offsetof(cells_to_be_reported_for_query_s, cell_global_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cell_global_id_c::desc},
    {offsetof(cells_to_be_reported_for_query_s, list_of_cfg_structures_to_be_reported_for_query),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<
         decltype(cells_to_be_reported_for_query_s::list_of_cfg_structures_to_be_reported_for_query),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &cfg_structures_to_be_reported_for_query_s::desc}};
static const asn1::table::field_name seq_table_cells_to_be_reported_for_query_s_names[] = {
    {"cellGlobalId", nullptr},
    {"listOfConfigurationStructuresToBeReportedForQuery", nullptr}};
const asn1::table::seq_desc cells_to_be_reported_for_query_s::desc = {
    {seq_table_cells_to_be_reported_for_query_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_cells_to_be_reported_for_query_s_names,
     2,
     0},
    false,
    true,
    nullptr,
    0};

// E2SM-CCC-ControlHeaderFormat1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_ctrl_hdr_format1_s_fields[] = {
    {offsetof(e2sm_ccc_ctrl_hdr_format1_s, ric_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_ccc_ctrl_hdr_format1_s_names[] = {{"ricStyleType", nullptr}};
const asn1::table::seq_desc          e2sm_ccc_ctrl_hdr_format1_s::desc = {{seq_table_e2sm_ccc_ctrl_hdr_format1_s_fields,
                                                                           uint64_t{0x1},
                                                                           nullptr,
                                                                           seq_table_e2sm_ccc_ctrl_hdr_format1_s_names,
                                                                           1,
                                                                           0},
                                                                          false,
                                                                          true,
                                                                          nullptr,
                                                                          0};

// ControlHeaderFormat ::= CHOICE
static const asn1::table::seq_field choice_table_ctrl_hdr_format_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_ctrl_hdr_format1_s::desc}};
static const asn1::table::field_name choice_table_ctrl_hdr_format_c_names[] = {{"controlHeader-Format1", nullptr}};
const asn1::table::choice_desc       ctrl_hdr_format_c::desc = {asn1::table::enum_int_type<ctrl_hdr_format_c::types>(),
                                                                ctrl_hdr_format_c::types::nof_types,
                                                                ctrl_hdr_format_c::types::nof_exts,
                                                                ctrl_hdr_format_c::types::has_ext,
                                                                true,
                                                                choice_table_ctrl_hdr_format_c_alternatives,
                                                                nullptr,
                                                                choice_table_ctrl_hdr_format_c_names,
                                                                "ctrl_hdr_format_c",
                                                                asn1::table::choice_desc::no_member,
                                                                offsetof(ctrl_hdr_format_c, c),
                                                                false};

static const char*           enum_names_ctrl_hdr_format_c_types_values[] = {"controlHeader-Format1"};
const asn1::table::enum_desc ctrl_hdr_format_c::types_opts::desc         = {enum_names_ctrl_hdr_format_c_types_values,
                                                                            1,
                                                                            "ctrl_hdr_format_c::types"};
uint8_t                      ctrl_hdr_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ctrl_hdr_format_c::types");
}

// E2SM-CCC-ControlMessageFormat1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_ctrl_msg_format1_s_fields[] = {
    {offsetof(e2sm_ccc_ctrl_msg_format1_s, list_of_cfg_structures),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(e2sm_ccc_ctrl_msg_format1_s::list_of_cfg_structures),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cfg_structure_write_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_ctrl_msg_format1_s_names[] = {
    {"listOfConfigurationStructures", nullptr}};
const asn1::table::seq_desc e2sm_ccc_ctrl_msg_format1_s::desc = {{seq_table_e2sm_ccc_ctrl_msg_format1_s_fields,
                                                                  uint64_t{0x1},
                                                                  nullptr,
                                                                  seq_table_e2sm_ccc_ctrl_msg_format1_s_names,
                                                                  1,
                                                                  0},
                                                                 false,
                                                                 true,
                                                                 nullptr,
                                                                 0};

// E2SM-CCC-ControlMessageFormat2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_ctrl_msg_format2_s_fields[] = {
    {offsetof(e2sm_ccc_ctrl_msg_format2_s, list_of_cells_ctrl),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(e2sm_ccc_ctrl_msg_format2_s::list_of_cells_ctrl),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_ctrl_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_ctrl_msg_format2_s_names[] = {
    {"listOfCellsControlled", nullptr}};
const asn1::table::seq_desc e2sm_ccc_ctrl_msg_format2_s::desc = {{seq_table_e2sm_ccc_ctrl_msg_format2_s_fields,
                                                                  uint64_t{0x1},
                                                                  nullptr,
                                                                  seq_table_e2sm_ccc_ctrl_msg_format2_s_names,
                                                                  1,
                                                                  0},
                                                                 false,
                                                                 true,
                                                                 nullptr,
                                                                 0};

// ControlMessageFormat ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ctrl_msg_format_c_alternatives[] = {
    &asn1::table::alternative_ops<e2sm_ccc_ctrl_msg_format1_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_ccc_ctrl_msg_format2_s, false, false>::value};
const asn1::table::choice_storage_desc ctrl_msg_format_c::storage_desc = {choice_storage_ctrl_msg_format_c_alternatives,
                                                                          2,
                                                                          "ctrl_msg_format_c"};

e2sm_ccc_ctrl_msg_format1_s& ctrl_msg_format_c::set_ctrl_msg_format1()
{
  set(types::ctrl_msg_format1);
  return c.get<e2sm_ccc_ctrl_msg_format1_s>();
}
e2sm_ccc_ctrl_msg_format2_s& ctrl_msg_format_c::set_ctrl_msg_format2()
{
  set(types::ctrl_msg_format2);
  return c.get<e2sm_ccc_ctrl_msg_format2_s>();
}
static const asn1::table::seq_field choice_table_ctrl_msg_format_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_ctrl_msg_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_ctrl_msg_format2_s::desc}};
static const asn1::table::field_name choice_table_ctrl_msg_format_c_names[] = {{"controlMessage-Format1", nullptr},
                                                                               {"controlMessage-Format2", nullptr}};
const asn1::table::choice_desc       ctrl_msg_format_c::desc = {asn1::table::enum_int_type<ctrl_msg_format_c::types>(),
                                                                ctrl_msg_format_c::types::nof_types,
                                                                ctrl_msg_format_c::types::nof_exts,
                                                                ctrl_msg_format_c::types::has_ext,
                                                                true,
                                                                choice_table_ctrl_msg_format_c_alternatives,
                                                                nullptr,
                                                                choice_table_ctrl_msg_format_c_names,
                                                                "ctrl_msg_format_c",
                                                                offsetof(ctrl_msg_format_c, c.type),
                                                                offsetof(ctrl_msg_format_c, c.value),
                                                                false,
                                                                &ctrl_msg_format_c::storage_desc};

static const char* enum_names_ctrl_msg_format_c_types_values[]   = {"controlMessage-Format1", "controlMessage-Format2"};
const asn1::table::enum_desc ctrl_msg_format_c::types_opts::desc = {enum_names_ctrl_msg_format_c_types_values,
                                                                    2,
                                                                    "ctrl_msg_format_c::types"};
uint8_t                      ctrl_msg_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ctrl_msg_format_c::types");
}

// E2SM-CCC-ControlOutcomeFormat1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_ctrl_outcome_format1_s_fields[] = {
    {offsetof(e2sm_ccc_ctrl_outcome_format1_s, rx_timestamp),
     (int32_t)offsetof(e2sm_ccc_ctrl_outcome_format1_s, rx_timestamp_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_ccc_ctrl_outcome_format1_s::rx_timestamp),
                                asn1::table::string_json>::value},
    {offsetof(e2sm_ccc_ctrl_outcome_format1_s, ran_cfg_structures_accepted_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(e2sm_ccc_ctrl_outcome_format1_s::ran_cfg_structures_accepted_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cfg_structure_accepted_s::desc},
    {offsetof(e2sm_ccc_ctrl_outcome_format1_s, ran_cfg_structures_failed_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(e2sm_ccc_ctrl_outcome_format1_s::ran_cfg_structures_failed_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cfg_structure_failed_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_ctrl_outcome_format1_s_names[] = {
    {"receivedTimestamp", nullptr},
    {"ranConfigurationStructuresAcceptedList", nullptr},
    {"ranConfigurationStructuresFailedList", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_ccc_ctrl_outcome_format1_s_opts[] = {
    {0, (int32_t)offsetof(e2sm_ccc_ctrl_outcome_format1_s, rx_timestamp_present)},
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc e2sm_ccc_ctrl_outcome_format1_s::desc = {{seq_table_e2sm_ccc_ctrl_outcome_format1_s_fields,
                                                                      uint64_t{0x0},
                                                                      seq_table_e2sm_ccc_ctrl_outcome_format1_s_opts,
                                                                      seq_table_e2sm_ccc_ctrl_outcome_format1_s_names,
                                                                      3,
                                                                      3},
                                                                     false,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2SM-CCC-ControlOutcomeFormat2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_ctrl_outcome_format2_s_fields[] = {
    {offsetof(e2sm_ccc_ctrl_outcome_format2_s, rx_timestamp),
     (int32_t)offsetof(e2sm_ccc_ctrl_outcome_format2_s, rx_timestamp_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_ccc_ctrl_outcome_format2_s::rx_timestamp),
                                asn1::table::string_json>::value},
    {offsetof(e2sm_ccc_ctrl_outcome_format2_s, list_of_cells_for_ctrl_outcome),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(e2sm_ccc_ctrl_outcome_format2_s::list_of_cells_for_ctrl_outcome),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_ctrl_outcome_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_ctrl_outcome_format2_s_names[] = {
    {"receivedTimestamp", nullptr},
    {"listOfCellsForControlOutcome", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_ccc_ctrl_outcome_format2_s_opts[] = {
    {0, (int32_t)offsetof(e2sm_ccc_ctrl_outcome_format2_s, rx_timestamp_present)}};
const asn1::table::seq_desc e2sm_ccc_ctrl_outcome_format2_s::desc = {{seq_table_e2sm_ccc_ctrl_outcome_format2_s_fields,
                                                                      uint64_t{0x2},
                                                                      seq_table_e2sm_ccc_ctrl_outcome_format2_s_opts,
                                                                      seq_table_e2sm_ccc_ctrl_outcome_format2_s_names,
                                                                      2,
                                                                      1},
                                                                     false,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// ControlOutcomeFormat ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ctrl_outcome_format_c_alternatives[] = {
    &asn1::table::alternative_ops<e2sm_ccc_ctrl_outcome_format1_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_ccc_ctrl_outcome_format2_s, false, false>::value};
const asn1::table::choice_storage_desc ctrl_outcome_format_c::storage_desc = {
    choice_storage_ctrl_outcome_format_c_alternatives,
    2,
    "ctrl_outcome_format_c"};

e2sm_ccc_ctrl_outcome_format1_s& ctrl_outcome_format_c::set_ctrl_outcome_format1()
{
  set(types::ctrl_outcome_format1);
  return c.get<e2sm_ccc_ctrl_outcome_format1_s>();
}
e2sm_ccc_ctrl_outcome_format2_s& ctrl_outcome_format_c::set_ctrl_outcome_format2()
{
  set(types::ctrl_outcome_format2);
  return c.get<e2sm_ccc_ctrl_outcome_format2_s>();
}
static const asn1::table::seq_field choice_table_ctrl_outcome_format_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_ctrl_outcome_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_ctrl_outcome_format2_s::desc}};
static const asn1::table::field_name choice_table_ctrl_outcome_format_c_names[] = {{"controlOutcome-Format1", nullptr},
                                                                                   {"controlOutcome-Format2", nullptr}};
const asn1::table::choice_desc       ctrl_outcome_format_c::desc                = {
    asn1::table::enum_int_type<ctrl_outcome_format_c::types>(),
    ctrl_outcome_format_c::types::nof_types,
    ctrl_outcome_format_c::types::nof_exts,
    ctrl_outcome_format_c::types::has_ext,
    true,
    choice_table_ctrl_outcome_format_c_alternatives,
    nullptr,
    choice_table_ctrl_outcome_format_c_names,
    "ctrl_outcome_format_c",
    offsetof(ctrl_outcome_format_c, c.type),
    offsetof(ctrl_outcome_format_c, c.value),
    false,
    &ctrl_outcome_format_c::storage_desc};

static const char*           enum_names_ctrl_outcome_format_c_types_values[] = {"controlOutcome-Format1",
                                                                                "controlOutcome-Format2"};
const asn1::table::enum_desc ctrl_outcome_format_c::types_opts::desc = {enum_names_ctrl_outcome_format_c_types_values,
                                                                        2,
                                                                        "ctrl_outcome_format_c::types"};
uint8_t                      ctrl_outcome_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ctrl_outcome_format_c::types");
}

// E2SM-CCC-EventTriggerDefinition-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_event_trigger_definition_format1_s_fields[] = {
    {offsetof(e2sm_ccc_event_trigger_definition_format1_s, list_of_node_level_cfg_structures_for_event_trigger),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<
         decltype(e2sm_ccc_event_trigger_definition_format1_s::list_of_node_level_cfg_structures_for_event_trigger),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &ran_cfg_structure_for_event_trigger_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_event_trigger_definition_format1_s_names[] = {
    {"listOfNodeLevelConfigurationStructuresForEventTrigger", nullptr}};
const asn1::table::seq_desc e2sm_ccc_event_trigger_definition_format1_s::desc = {
    {seq_table_e2sm_ccc_event_trigger_definition_format1_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_ccc_event_trigger_definition_format1_s_names,
     1,
     0},
    false,
    true,
    nullptr,
    0};

// E2SM-CCC-EventTriggerDefinition-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_event_trigger_definition_format2_s_fields[] = {
    {offsetof(e2sm_ccc_event_trigger_definition_format2_s, list_of_cell_level_cfg_structures_for_event_trigger),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<
         decltype(e2sm_ccc_event_trigger_definition_format2_s::list_of_cell_level_cfg_structures_for_event_trigger),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &cell_level_cfg_structure_for_event_trigger_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_event_trigger_definition_format2_s_names[] = {
    {"listOfCellLevelConfigurationStructuresForEventTrigger", nullptr}};
const asn1::table::seq_desc e2sm_ccc_event_trigger_definition_format2_s::desc = {
    {seq_table_e2sm_ccc_event_trigger_definition_format2_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_ccc_event_trigger_definition_format2_s_names,
     1,
     0},
    false,
    true,
    nullptr,
    0};

// E2SM-CCC-EventTriggerDefinition-Format3 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_event_trigger_definition_format3_s_fields[] = {
    {offsetof(e2sm_ccc_event_trigger_definition_format3_s, period),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_ccc_event_trigger_definition_format3_s_names[] = {
    {"period", nullptr}};
const asn1::table::seq_desc e2sm_ccc_event_trigger_definition_format3_s::desc = {
    {seq_table_e2sm_ccc_event_trigger_definition_format3_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_ccc_event_trigger_definition_format3_s_names,
     1,
     0},
    false,
    true,
    nullptr,
    0};

// E2SM-CCC-IndicationHeaderFormat1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_ind_hdr_format1_s_fields[] = {
    {offsetof(e2sm_ccc_ind_hdr_format1_s, ind_reason),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(e2sm_ccc_ind_hdr_format1_s::ind_reason)>(),
     decltype(e2sm_ccc_ind_hdr_format1_s::ind_reason)::has_ext,
     false,
     decltype(e2sm_ccc_ind_hdr_format1_s::ind_reason)::nof_types,
     decltype(e2sm_ccc_ind_hdr_format1_s::ind_reason)::nof_exts,
     nullptr,
     &e2sm_ccc_ind_hdr_format1_s::ind_reason_opts::desc},
    {offsetof(e2sm_ccc_ind_hdr_format1_s, event_time),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_ccc_ind_hdr_format1_s::event_time), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_e2sm_ccc_ind_hdr_format1_s_names[] = {{"indicationReason", nullptr},
                                                                                     {"eventTime", nullptr}};
const asn1::table::seq_desc          e2sm_ccc_ind_hdr_format1_s::desc = {{seq_table_e2sm_ccc_ind_hdr_format1_s_fields,
                                                                          uint64_t{0x3},
                                                                          nullptr,
                                                                          seq_table_e2sm_ccc_ind_hdr_format1_s_names,
                                                                          2,
                                                                          0},
                                                                         false,
                                                                         true,
                                                                         nullptr,
                                                                         0};

static const char*           enum_names_e2sm_ccc_ind_hdr_format1_s_ind_reason_e__values[] = {"uponSubscription",
                                                                                             "uponChange",
                                                                                             "periodic"};
const asn1::table::enum_desc e2sm_ccc_ind_hdr_format1_s::ind_reason_opts::desc            = {
    enum_names_e2sm_ccc_ind_hdr_format1_s_ind_reason_e__values,
    3,
    "e2sm_ccc_ind_hdr_format1_s::ind_reason_e_"};

// E2SM-CCC-IndicationMessageFormat1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_ind_msg_format1_s_fields[] = {
    {offsetof(e2sm_ccc_ind_msg_format1_s, list_of_cfg_structures_reported),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(e2sm_ccc_ind_msg_format1_s::list_of_cfg_structures_reported),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cfg_structure_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_ind_msg_format1_s_names[] = {
    {"listOfConfigurationStructuresReported", nullptr}};
const asn1::table::seq_desc e2sm_ccc_ind_msg_format1_s::desc = {{seq_table_e2sm_ccc_ind_msg_format1_s_fields,
                                                                 uint64_t{0x1},
                                                                 nullptr,
                                                                 seq_table_e2sm_ccc_ind_msg_format1_s_names,
                                                                 1,
                                                                 0},
                                                                false,
                                                                true,
                                                                nullptr,
                                                                0};

// E2SM-CCC-IndicationMessageFormat2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_ind_msg_format2_s_fields[] = {
    {offsetof(e2sm_ccc_ind_msg_format2_s, list_of_cells_reported),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(e2sm_ccc_ind_msg_format2_s::list_of_cells_reported),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_reported_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_ind_msg_format2_s_names[] = {{"listOfCellsReported", nullptr}};
const asn1::table::seq_desc          e2sm_ccc_ind_msg_format2_s::desc = {{seq_table_e2sm_ccc_ind_msg_format2_s_fields,
                                                                          uint64_t{0x1},
                                                                          nullptr,
                                                                          seq_table_e2sm_ccc_ind_msg_format2_s_names,
                                                                          1,
                                                                          0},
                                                                         false,
                                                                         true,
                                                                         nullptr,
                                                                         0};

// NodelevelRanConfigurationStructuresForQuery ::= SEQUENCE
static const asn1::table::seq_field seq_table_nodelevel_ran_cfg_structures_for_query_s_fields[] = {
    {offsetof(nodelevel_ran_cfg_structures_for_query_s, ran_cfg_structure_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(nodelevel_ran_cfg_structures_for_query_s::ran_cfg_structure_name),
                                asn1::table::string_json>::value},
    {offsetof(nodelevel_ran_cfg_structures_for_query_s, list_of_attributes),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(nodelevel_ran_cfg_structures_for_query_s::list_of_attributes),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &attribute_name_s::desc}};
static const asn1::table::field_name seq_table_nodelevel_ran_cfg_structures_for_query_s_names[] = {
    {"ranConfigurationStructureName", nullptr},
    {"listOfAttributes", nullptr}};
static const asn1::table::seq_opt seq_table_nodelevel_ran_cfg_structures_for_query_s_opts[] = {
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc nodelevel_ran_cfg_structures_for_query_s::desc = {
    {seq_table_nodelevel_ran_cfg_structures_for_query_s_fields,
     uint64_t{0x1},
     seq_table_nodelevel_ran_cfg_structures_for_query_s_opts,
     seq_table_nodelevel_ran_cfg_structures_for_query_s_names,
     2,
     1},
    false,
    true,
    nullptr,
    0};

// E2SM-CCC-QueryDefinitionFormat1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_query_definition_format1_s_fields[] = {
    {offsetof(e2sm_ccc_query_definition_format1_s, list_of_nodelevel_ran_cfg_structures_for_query),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<
         decltype(e2sm_ccc_query_definition_format1_s::list_of_nodelevel_ran_cfg_structures_for_query),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &nodelevel_ran_cfg_structures_for_query_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_query_definition_format1_s_names[] = {
    {"listOfNodelevelRanConfigurationStructuresForQuery", nullptr}};
const asn1::table::seq_desc e2sm_ccc_query_definition_format1_s::desc = {
    {seq_table_e2sm_ccc_query_definition_format1_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_ccc_query_definition_format1_s_names,
     1,
     0},
    false,
    true,
    nullptr,
    0};

// E2SM-CCC-QueryDefinitionFormat2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_query_definition_format2_s_fields[] = {
    {offsetof(e2sm_ccc_query_definition_format2_s, list_of_cells_to_be_queried_for_query),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(e2sm_ccc_query_definition_format2_s::list_of_cells_to_be_queried_for_query),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cells_to_be_queried_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_query_definition_format2_s_names[] = {
    {"listOfCellsToBeQueriedForQuery", nullptr}};
const asn1::table::seq_desc e2sm_ccc_query_definition_format2_s::desc = {
    {seq_table_e2sm_ccc_query_definition_format2_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_ccc_query_definition_format2_s_names,
     1,
     0},
    false,
    true,
    nullptr,
    0};

// E2SM-CCC-QueryHeaderFormat1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_query_hdr_format1_s_fields[] = {
    {offsetof(e2sm_ccc_query_hdr_format1_s, ric_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_ccc_query_hdr_format1_s_names[] = {{"ricStyleType", nullptr}};
const asn1::table::seq_desc e2sm_ccc_query_hdr_format1_s::desc = {{seq_table_e2sm_ccc_query_hdr_format1_s_fields,
                                                                   uint64_t{0x1},
                                                                   nullptr,
                                                                   seq_table_e2sm_ccc_query_hdr_format1_s_names,
                                                                   1,
                                                                   0},
                                                                  false,
                                                                  true,
                                                                  nullptr,
                                                                  0};

// E2SM-CCC-QueryOutcomeFormat1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_query_outcome_format1_s_fields[] = {
    {offsetof(e2sm_ccc_query_outcome_format1_s, list_of_cfg_structures_to_be_reported_for_query),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<
         decltype(e2sm_ccc_query_outcome_format1_s::list_of_cfg_structures_to_be_reported_for_query),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &cfg_structures_to_be_reported_for_query_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_query_outcome_format1_s_names[] = {
    {"listOfConfigurationStructuresToBeReportedForQuery", nullptr}};
const asn1::table::seq_desc e2sm_ccc_query_outcome_format1_s::desc = {
    {seq_table_e2sm_ccc_query_outcome_format1_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_ccc_query_outcome_format1_s_names,
     1,
     0},
    false,
    true,
    nullptr,
    0};

// E2SM-CCC-QueryOutcomeFormat2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_ccc_query_outcome_format2_s_fields[] = {
    {offsetof(e2sm_ccc_query_outcome_format2_s, list_of_cells_to_be_reported_for_query),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(e2sm_ccc_query_outcome_format2_s::list_of_cells_to_be_reported_for_query),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cells_to_be_reported_for_query_s::desc}};
static const asn1::table::field_name seq_table_e2sm_ccc_query_outcome_format2_s_names[] = {
    {"listOfCellsToBeReportedForQuery", nullptr}};
const asn1::table::seq_desc e2sm_ccc_query_outcome_format2_s::desc = {
    {seq_table_e2sm_ccc_query_outcome_format2_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_ccc_query_outcome_format2_s_names,
     1,
     0},
    false,
    true,
    nullptr,
    0};

// EventTriggerDefinitionFormat ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_event_trigger_definition_format_c_alternatives[] = {
    &asn1::table::alternative_ops<e2sm_ccc_event_trigger_definition_format1_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_ccc_event_trigger_definition_format2_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_ccc_event_trigger_definition_format3_s, false>::value};
const asn1::table::choice_storage_desc event_trigger_definition_format_c::storage_desc = {
    choice_storage_event_trigger_definition_format_c_alternatives,
    3,
    "event_trigger_definition_format_c"};

e2sm_ccc_event_trigger_definition_format1_s& event_trigger_definition_format_c::set_event_trigger_definition_format1()
{
  set(types::event_trigger_definition_format1);
  return c.get<e2sm_ccc_event_trigger_definition_format1_s>();
}
e2sm_ccc_event_trigger_definition_format2_s& event_trigger_definition_format_c::set_event_trigger_definition_format2()
{
  set(types::event_trigger_definition_format2);
  return c.get<e2sm_ccc_event_trigger_definition_format2_s>();
}
e2sm_ccc_event_trigger_definition_format3_s& event_trigger_definition_format_c::set_event_trigger_definition_format3()
{
  set(types::event_trigger_definition_format3);
  return c.get<e2sm_ccc_event_trigger_definition_format3_s>();
}
static const asn1::table::seq_field choice_table_event_trigger_definition_format_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_event_trigger_definition_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_event_trigger_definition_format2_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_event_trigger_definition_format3_s::desc}};
static const asn1::table::field_name choice_table_event_trigger_definition_format_c_names[] = {
    {"eventTriggerDefinition-Format1", nullptr},
    {"eventTriggerDefinition-Format2", nullptr},
    {"eventTriggerDefinition-Format3", nullptr}};
const asn1::table::choice_desc event_trigger_definition_format_c::desc = {
    asn1::table::enum_int_type<event_trigger_definition_format_c::types>(),
    event_trigger_definition_format_c::types::nof_types,
    event_trigger_definition_format_c::types::nof_exts,
    event_trigger_definition_format_c::types::has_ext,
    true,
    choice_table_event_trigger_definition_format_c_alternatives,
    nullptr,
    choice_table_event_trigger_definition_format_c_names,
    "event_trigger_definition_format_c",
    offsetof(event_trigger_definition_format_c, c.type),
    offsetof(event_trigger_definition_format_c, c.value),
    false,
    &event_trigger_definition_format_c::storage_desc};

static const char* enum_names_event_trigger_definition_format_c_types_values[]   = {"eventTriggerDefinition-Format1",
                                                                                    "eventTriggerDefinition-Format2",
                                                                                    "eventTriggerDefinition-Format3"};
const asn1::table::enum_desc event_trigger_definition_format_c::types_opts::desc = {
    enum_names_event_trigger_definition_format_c_types_values,
    3,
    "event_trigger_definition_format_c::types"};
uint8_t event_trigger_definition_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(numbers, 3, value, "event_trigger_definition_format_c::types");
}

// IndicationHeaderFormat ::= CHOICE
static const asn1::table::seq_field choice_table_ind_hdr_format_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_ind_hdr_format1_s::desc}};
static const asn1::table::field_name choice_table_ind_hdr_format_c_names[] = {{"indicationHeader-Format1", nullptr}};
const asn1::table::choice_desc       ind_hdr_format_c::desc = {asn1::table::enum_int_type<ind_hdr_format_c::types>(),
                                                               ind_hdr_format_c::types::nof_types,
                                                               ind_hdr_format_c::types::nof_exts,
                                                               ind_hdr_format_c::types::has_ext,
                                                               true,
                                                               choice_table_ind_hdr_format_c_alternatives,
                                                               nullptr,
                                                               choice_table_ind_hdr_format_c_names,
                                                               "ind_hdr_format_c",
                                                               asn1::table::choice_desc::no_member,
                                                               offsetof(ind_hdr_format_c, c),
                                                               false};

static const char*           enum_names_ind_hdr_format_c_types_values[] = {"indicationHeader-Format1"};
const asn1::table::enum_desc ind_hdr_format_c::types_opts::desc         = {enum_names_ind_hdr_format_c_types_values,
                                                                           1,
                                                                           "ind_hdr_format_c::types"};
uint8_t                      ind_hdr_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ind_hdr_format_c::types");
}

// IndicationMessageFormat ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ind_msg_format_c_alternatives[] = {
    &asn1::table::alternative_ops<e2sm_ccc_ind_msg_format1_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_ccc_ind_msg_format2_s, false, false>::value};
const asn1::table::choice_storage_desc ind_msg_format_c::storage_desc = {choice_storage_ind_msg_format_c_alternatives,
                                                                         2,
                                                                         "ind_msg_format_c"};

e2sm_ccc_ind_msg_format1_s& ind_msg_format_c::set_ind_msg_format1()
{
  set(types::ind_msg_format1);
  return c.get<e2sm_ccc_ind_msg_format1_s>();
}
e2sm_ccc_ind_msg_format2_s& ind_msg_format_c::set_ind_msg_format2()
{
  set(types::ind_msg_format2);
  return c.get<e2sm_ccc_ind_msg_format2_s>();
}
static const asn1::table::seq_field choice_table_ind_msg_format_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_ind_msg_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_ind_msg_format2_s::desc}};
static const asn1::table::field_name choice_table_ind_msg_format_c_names[] = {{"indicationMessage-Format1", nullptr},
                                                                              {"indicationMessage-Format2", nullptr}};
const asn1::table::choice_desc       ind_msg_format_c::desc = {asn1::table::enum_int_type<ind_msg_format_c::types>(),
                                                               ind_msg_format_c::types::nof_types,
                                                               ind_msg_format_c::types::nof_exts,
                                                               ind_msg_format_c::types::has_ext,
                                                               true,
                                                               choice_table_ind_msg_format_c_alternatives,
                                                               nullptr,
                                                               choice_table_ind_msg_format_c_names,
                                                               "ind_msg_format_c",
                                                               offsetof(ind_msg_format_c, c.type),
                                                               offsetof(ind_msg_format_c, c.value),
                                                               false,
                                                               &ind_msg_format_c::storage_desc};

static const char*           enum_names_ind_msg_format_c_types_values[] = {"indicationMessage-Format1",
                                                                           "indicationMessage-Format2"};
const asn1::table::enum_desc ind_msg_format_c::types_opts::desc         = {enum_names_ind_msg_format_c_types_values,
                                                                           2,
                                                                           "ind_msg_format_c::types"};
uint8_t                      ind_msg_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "ind_msg_format_c::types");
}

// QueryDefinitionFormat ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_query_definition_format_c_alternatives[] = {
    &asn1::table::alternative_ops<e2sm_ccc_query_definition_format1_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_ccc_query_definition_format2_s, false, false>::value};
const asn1::table::choice_storage_desc query_definition_format_c::storage_desc = {
    choice_storage_query_definition_format_c_alternatives,
    2,
    "query_definition_format_c"};

e2sm_ccc_query_definition_format1_s& query_definition_format_c::set_query_definition_format1()
{
  set(types::query_definition_format1);
  return c.get<e2sm_ccc_query_definition_format1_s>();
}
e2sm_ccc_query_definition_format2_s& query_definition_format_c::set_query_definition_format2()
{
  set(types::query_definition_format2);
  return c.get<e2sm_ccc_query_definition_format2_s>();
}
static const asn1::table::seq_field choice_table_query_definition_format_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_query_definition_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_query_definition_format2_s::desc}};
static const asn1::table::field_name choice_table_query_definition_format_c_names[] = {
    {"queryDefinition-Format1", nullptr},
    {"queryDefinition-Format2", nullptr}};
const asn1::table::choice_desc query_definition_format_c::desc = {
    asn1::table::enum_int_type<query_definition_format_c::types>(),
    query_definition_format_c::types::nof_types,
    query_definition_format_c::types::nof_exts,
    query_definition_format_c::types::has_ext,
    true,
    choice_table_query_definition_format_c_alternatives,
    nullptr,
    choice_table_query_definition_format_c_names,
    "query_definition_format_c",
    offsetof(query_definition_format_c, c.type),
    offsetof(query_definition_format_c, c.value),
    false,
    &query_definition_format_c::storage_desc};

static const char*           enum_names_query_definition_format_c_types_values[] = {"queryDefinition-Format1",
                                                                                    "queryDefinition-Format2"};
const asn1::table::enum_desc query_definition_format_c::types_opts::desc         = {
    enum_names_query_definition_format_c_types_values,
    2,
    "query_definition_format_c::types"};
uint8_t query_definition_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "query_definition_format_c::types");
}

// QueryHeaderFormat ::= CHOICE
static const asn1::table::seq_field choice_table_query_hdr_format_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_query_hdr_format1_s::desc}};
static const asn1::table::field_name choice_table_query_hdr_format_c_names[] = {{"queryHeader-Format1", nullptr}};
const asn1::table::choice_desc query_hdr_format_c::desc = {asn1::table::enum_int_type<query_hdr_format_c::types>(),
                                                           query_hdr_format_c::types::nof_types,
                                                           query_hdr_format_c::types::nof_exts,
                                                           query_hdr_format_c::types::has_ext,
                                                           true,
                                                           choice_table_query_hdr_format_c_alternatives,
                                                           nullptr,
                                                           choice_table_query_hdr_format_c_names,
                                                           "query_hdr_format_c",
                                                           asn1::table::choice_desc::no_member,
                                                           offsetof(query_hdr_format_c, c),
                                                           false};

static const char*           enum_names_query_hdr_format_c_types_values[] = {"queryHeader-Format1"};
const asn1::table::enum_desc query_hdr_format_c::types_opts::desc         = {enum_names_query_hdr_format_c_types_values,
                                                                             1,
                                                                             "query_hdr_format_c::types"};
uint8_t                      query_hdr_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "query_hdr_format_c::types");
}

// QueryOutcomeFormat ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_query_outcome_format_c_alternatives[] = {
    &asn1::table::alternative_ops<e2sm_ccc_query_outcome_format1_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_ccc_query_outcome_format2_s, false, false>::value};
const asn1::table::choice_storage_desc query_outcome_format_c::storage_desc = {
    choice_storage_query_outcome_format_c_alternatives,
    2,
    "query_outcome_format_c"};

e2sm_ccc_query_outcome_format1_s& query_outcome_format_c::set_query_outcome_format1()
{
  set(types::query_outcome_format1);
  return c.get<e2sm_ccc_query_outcome_format1_s>();
}
e2sm_ccc_query_outcome_format2_s& query_outcome_format_c::set_query_outcome_format2()
{
  set(types::query_outcome_format2);
  return c.get<e2sm_ccc_query_outcome_format2_s>();
}
static const asn1::table::seq_field choice_table_query_outcome_format_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_query_outcome_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_ccc_query_outcome_format2_s::desc}};
static const asn1::table::field_name choice_table_query_outcome_format_c_names[] = {{"queryOutcome-Format1", nullptr},
                                                                                    {"queryOutcome-Format2", nullptr}};
const asn1::table::choice_desc       query_outcome_format_c::desc                = {
    asn1::table::enum_int_type<query_outcome_format_c::types>(),
    query_outcome_format_c::types::nof_types,
    query_outcome_format_c::types::nof_exts,
    query_outcome_format_c::types::has_ext,
    true,
    choice_table_query_outcome_format_c_alternatives,
    nullptr,
    choice_table_query_outcome_format_c_names,
    "query_outcome_format_c",
    offsetof(query_outcome_format_c, c.type),
    offsetof(query_outcome_format_c, c.value),
    false,
    &query_outcome_format_c::storage_desc};

static const char* enum_names_query_outcome_format_c_types_values[] = {"queryOutcome-Format1", "queryOutcome-Format2"};
const asn1::table::enum_desc query_outcome_format_c::types_opts::desc = {enum_names_query_outcome_format_c_types_values,
                                                                         2,
                                                                         "query_outcome_format_c::types"};
uint8_t                      query_outcome_format_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "query_outcome_format_c::types");
}

// RANFunctionName ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_name_s_fields[] = {
    {offsetof(ran_function_name_s, ran_function_short_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_name_s::ran_function_short_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_name_s, ran_function_service_model_o_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_name_s::ran_function_service_model_o_id),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_name_s, ran_function_description),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_name_s::ran_function_description),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_name_s, ran_function_instance),
     (int32_t)offsetof(ran_function_name_s, ran_function_instance_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_ran_function_name_s_names[] = {{"ranFunctionShortName", nullptr},
                                                                              {"ranFunctionServiceModelOID", nullptr},
                                                                              {"ranFunctionDescription", nullptr},
                                                                              {"ranFunctionInstance", nullptr}};
static const asn1::table::seq_opt    seq_table_ran_function_name_s_opts[]  = {
    {3, (int32_t)offsetof(ran_function_name_s, ran_function_instance_present)}};
const asn1::table::seq_desc ran_function_name_s::desc = {{seq_table_ran_function_name_s_fields,
                                                          uint64_t{0x7},
                                                          seq_table_ran_function_name_s_opts,
                                                          seq_table_ran_function_name_s_names,
                                                          4,
                                                          1},
                                                         false,
                                                         true,
                                                         nullptr,
                                                         0};

// RANFunctionDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_s_fields[] = {
    {offsetof(ran_function_definition_s, ran_function_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_name_s::desc},
    {offsetof(ran_function_definition_s, list_of_supported_node_level_cfg_structures),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(ran_function_definition_s::list_of_supported_node_level_cfg_structures),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_cfg_structure_s::desc},
    {offsetof(ran_function_definition_s, list_of_cells_for_ran_function_definition),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     63,
     &asn1::table::item_list_codec<decltype(ran_function_definition_s::list_of_cells_for_ran_function_definition),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_for_ran_function_definition_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_s_names[] = {
    {"ranFunctionName", nullptr},
    {"listOfSupportedNodeLevelConfigurationStructures", nullptr},
    {"listOfCellsForRANFunctionDefinition", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_definition_s::desc = {{seq_table_ran_function_definition_s_fields,
                                                                uint64_t{0x1},
                                                                seq_table_ran_function_definition_s_opts,
                                                                seq_table_ran_function_definition_s_names,
                                                                3,
                                                                2},
                                                               false,
                                                               true,
                                                               nullptr,
                                                               0};

// RIC-Control-Header ::= SEQUENCE
static const asn1::table::seq_field  seq_table_ric_ctrl_hdr_s_fields[] = {{offsetof(ric_ctrl_hdr_s, ctrl_hdr_format),
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::field_op::choice,
                                                                           false,
                                                                           false,
                                                                           0,
                                                                           0,
                                                                           nullptr,
                                                                           &ctrl_hdr_format_c::desc}};
static const asn1::table::field_name seq_table_ric_ctrl_hdr_s_names[]  = {{"controlHeaderFormat", nullptr}};
const asn1::table::seq_desc          ric_ctrl_hdr_s::desc              = {
    {seq_table_ric_ctrl_hdr_s_fields, uint64_t{0x1}, nullptr, seq_table_ric_ctrl_hdr_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// RIC-Control-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_ric_ctrl_msg_s_fields[] = {{offsetof(ric_ctrl_msg_s, ctrl_msg_format),
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::field_op::choice,
                                                                           false,
                                                                           false,
                                                                           0,
                                                                           0,
                                                                           nullptr,
                                                                           &ctrl_msg_format_c::desc}};
static const asn1::table::field_name seq_table_ric_ctrl_msg_s_names[]  = {{"controlMessageFormat", nullptr}};
const asn1::table::seq_desc          ric_ctrl_msg_s::desc              = {
    {seq_table_ric_ctrl_msg_s_fields, uint64_t{0x1}, nullptr, seq_table_ric_ctrl_msg_s_names, 1, 0},
    false,
    true,
    nullptr,
    0,
    "RIC-Control-Message"};

// RIC-Control-Outcome ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_ctrl_outcome_s_fields[] = {
    {offsetof(ric_ctrl_outcome_s, ctrl_outcome_format),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ctrl_outcome_format_c::desc}};
static const asn1::table::field_name seq_table_ric_ctrl_outcome_s_names[] = {{"controlOutcomeFormat", nullptr}};
const asn1::table::seq_desc          ric_ctrl_outcome_s::desc             = {
    {seq_table_ric_ctrl_outcome_s_fields, uint64_t{0x1}, nullptr, seq_table_ric_ctrl_outcome_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// RIC-Indication-Header ::= SEQUENCE
static const asn1::table::seq_field  seq_table_ric_ind_hdr_s_fields[] = {{offsetof(ric_ind_hdr_s, ind_hdr_format),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::choice,
                                                                          false,
                                                                          false,
                                                                          0,
                                                                          0,
                                                                          nullptr,
                                                                          &ind_hdr_format_c::desc}};
static const asn1::table::field_name seq_table_ric_ind_hdr_s_names[]  = {{"indicationHeaderFormat", nullptr}};
const asn1::table::seq_desc          ric_ind_hdr_s::desc              = {
    {seq_table_ric_ind_hdr_s_fields, uint64_t{0x1}, nullptr, seq_table_ric_ind_hdr_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// RIC-Indication-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_ric_ind_msg_s_fields[] = {{offsetof(ric_ind_msg_s, ind_msg_format),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::choice,
                                                                          false,
                                                                          false,
                                                                          0,
                                                                          0,
                                                                          nullptr,
                                                                          &ind_msg_format_c::desc}};
static const asn1::table::field_name seq_table_ric_ind_msg_s_names[]  = {{"indicationMessageFormat", nullptr}};
const asn1::table::seq_desc          ric_ind_msg_s::desc              = {
    {seq_table_ric_ind_msg_s_fields, uint64_t{0x1}, nullptr, seq_table_ric_ind_msg_s_names, 1, 0},
    false,
    true,
    nullptr,
    0,
    "RIC-Indication-Message"};

// RIC-Query-Definition ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_query_definition_s_fields[] = {
    {offsetof(ric_query_definition_s, query_definition_format),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &query_definition_format_c::desc}};
static const asn1::table::field_name seq_table_ric_query_definition_s_names[] = {{"queryDefinitionFormat", nullptr}};
const asn1::table::seq_desc          ric_query_definition_s::desc             = {
    {seq_table_ric_query_definition_s_fields, uint64_t{0x1}, nullptr, seq_table_ric_query_definition_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// RIC-Query-Header ::= SEQUENCE
static const asn1::table::seq_field  seq_table_ric_query_hdr_s_fields[] = {{offsetof(ric_query_hdr_s, query_hdr_format),
                                                                            asn1::table::seq_field::mandatory,
                                                                            asn1::table::field_op::choice,
                                                                            false,
                                                                            false,
                                                                            0,
                                                                            0,
                                                                            nullptr,
                                                                            &query_hdr_format_c::desc}};
static const asn1::table::field_name seq_table_ric_query_hdr_s_names[]  = {{"queryHeaderFormat", nullptr}};
const asn1::table::seq_desc          ric_query_hdr_s::desc              = {
    {seq_table_ric_query_hdr_s_fields, uint64_t{0x1}, nullptr, seq_table_ric_query_hdr_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// RIC-Query-Outcome ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_query_outcome_s_fields[] = {
    {offsetof(ric_query_outcome_s, query_outcome_format),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &query_outcome_format_c::desc}};
static const asn1::table::field_name seq_table_ric_query_outcome_s_names[] = {{"queryOutcomeFormat", nullptr}};
const asn1::table::seq_desc          ric_query_outcome_s::desc             = {
    {seq_table_ric_query_outcome_s_fields, uint64_t{0x1}, nullptr, seq_table_ric_query_outcome_s_names, 1, 0},
    false,
    true,
    nullptr,
    0};

// RICActionDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_action_definition_s_fields[] = {
    {offsetof(ric_action_definition_s, ric_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ric_action_definition_s, action_definition_format),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &action_definition_format_c::desc}};
static const asn1::table::field_name seq_table_ric_action_definition_s_names[] = {{"ricStyleType", nullptr},
                                                                                  {"actionDefinitionFormat", nullptr}};
const asn1::table::seq_desc          ric_action_definition_s::desc             = {
    {seq_table_ric_action_definition_s_fields, uint64_t{0x3}, nullptr, seq_table_ric_action_definition_s_names, 2, 0},
    false,
    true,
    nullptr,
    0};

// RICEventTriggerDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_event_trigger_definition_s_fields[] = {
    {offsetof(ric_event_trigger_definition_s, event_trigger_definition_format),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_definition_format_c::desc}};
static const asn1::table::field_name seq_table_ric_event_trigger_definition_s_names[] = {
    {"eventTriggerDefinitionFormat", nullptr}};
const asn1::table::seq_desc ric_event_trigger_definition_s::desc = {{seq_table_ric_event_trigger_definition_s_fields,
                                                                     uint64_t{0x1},
                                                                     nullptr,
                                                                     seq_table_ric_event_trigger_definition_s_names,
                                                                     1,
                                                                     0},
                                                                    false,
                                                                    true,
                                                                    nullptr,
                                                                    0};
