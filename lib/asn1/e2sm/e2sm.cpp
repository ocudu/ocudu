// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e2sm/e2sm.h"
#include "ocudu/asn1/asn1_ap_codec.h"

using namespace asn1;
using namespace asn1::e2sm;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// AdditionalSupportedFormat ::= SEQUENCE
static const asn1::table::seq_field seq_table_add_supported_format_s_fields[] = {
    {offsetof(add_supported_format_s, ric_ctrl_hdr_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(add_supported_format_s, ric_ctrl_msg_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_add_supported_format_s_names[] = {
    {"ric-ControlHeaderFormat-Type", nullptr},
    {"ric-ControlMessageFormat-Type", nullptr}};
static const asn1::table::seq_field seq_table_add_supported_format_s_g0_fields[] = {
    {offsetof(add_supported_format_s, ric_ctrl_outcome_format_type),
     (int32_t)offsetof(add_supported_format_s, ric_ctrl_outcome_format_type_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_add_supported_format_s_g0_names[] = {
    {"ric-ControlOutcomeFormat-Type", nullptr}};
static const asn1::table::seq_opt seq_table_add_supported_format_s_g0_opts[] = {
    {0, (int32_t)offsetof(add_supported_format_s, ric_ctrl_outcome_format_type_present)}};
static const asn1::table::seq_field seq_table_add_supported_format_s_g1_fields[] = {
    {offsetof(add_supported_format_s, ric_ctrl_action_id),
     (int32_t)offsetof(add_supported_format_s, ric_ctrl_action_id_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr}};
static const asn1::table::field_name seq_table_add_supported_format_s_g1_names[] = {{"ric-ControlAction-ID", nullptr}};
static const asn1::table::seq_opt    seq_table_add_supported_format_s_g1_opts[]  = {
    {0, (int32_t)offsetof(add_supported_format_s, ric_ctrl_action_id_present)}};
static const asn1::table::seq_group seq_table_add_supported_format_s_groups[] = {
    {{seq_table_add_supported_format_s_g0_fields,
      uint64_t{0x0},
      seq_table_add_supported_format_s_g0_opts,
      seq_table_add_supported_format_s_g0_names,
      1,
      1},
     true},
    {{seq_table_add_supported_format_s_g1_fields,
      uint64_t{0x0},
      seq_table_add_supported_format_s_g1_opts,
      seq_table_add_supported_format_s_g1_names,
      1,
      1},
     true}};
const asn1::table::seq_desc add_supported_format_s::desc = {
    {seq_table_add_supported_format_s_fields, uint64_t{0x3}, nullptr, seq_table_add_supported_format_s_names, 2, 0},
    true,
    true,
    seq_table_add_supported_format_s_groups,
    2};

// RANParameter-Testing-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_testing_item_s_fields[] = {
    {offsetof(ran_param_testing_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(ran_param_testing_item_s, ran_param_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_param_testing_item_s::ran_param_type), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ran_param_testing_item_s_names[] = {{"ranParameter-ID", nullptr},
                                                                                   {"ranParameter-Type", nullptr}};
const asn1::table::seq_desc          ran_param_testing_item_s::desc             = {
    {seq_table_ran_param_testing_item_s_fields, uint64_t{0x3}, nullptr, seq_table_ran_param_testing_item_s_names, 2, 0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::seq_field choice_table_ran_param_testing_item_s_ran_param_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ran_param_testing_item_choice_list_s>::value,
     &ran_param_testing_item_choice_list_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ran_param_testing_item_choice_structure_s>::value,
     &ran_param_testing_item_choice_structure_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ran_param_testing_item_choice_elem_true_s>::value,
     &ran_param_testing_item_choice_elem_true_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ran_param_testing_item_choice_elem_false_s>::value,
     &ran_param_testing_item_choice_elem_false_s::desc}};
static const asn1::table::field_name choice_table_ran_param_testing_item_s_ran_param_type_c__names[] = {
    {"ranP-Choice-List", nullptr},
    {"ranP-Choice-Structure", nullptr},
    {"ranP-Choice-ElementTrue", nullptr},
    {"ranP-Choice-ElementFalse", nullptr}};
const asn1::table::choice_desc ran_param_testing_item_s::ran_param_type_c_::desc = {
    asn1::table::enum_int_type<ran_param_testing_item_s::ran_param_type_c_::types>(),
    ran_param_testing_item_s::ran_param_type_c_::types::nof_types,
    ran_param_testing_item_s::ran_param_type_c_::types::nof_exts,
    ran_param_testing_item_s::ran_param_type_c_::types::has_ext,
    true,
    choice_table_ran_param_testing_item_s_ran_param_type_c__alternatives,
    &asn1::table::choice_set<ran_param_testing_item_s::ran_param_type_c_>,
    choice_table_ran_param_testing_item_s_ran_param_type_c__names,
    "ran_param_testing_item_s::ran_param_type_c_",
    offsetof(ran_param_testing_item_s::ran_param_type_c_, type_),
    offsetof(ran_param_testing_item_s::ran_param_type_c_, c),
    false};

void ran_param_testing_item_s::ran_param_type_c_::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, ran_param_testing_item_s::ran_param_type_c_::desc);
}
ran_param_testing_item_choice_list_s& ran_param_testing_item_s::ran_param_type_c_::set_ran_p_choice_list()
{
  set(types::ran_p_choice_list);
  return c.get<ran_param_testing_item_choice_list_s>();
}
ran_param_testing_item_choice_structure_s& ran_param_testing_item_s::ran_param_type_c_::set_ran_p_choice_structure()
{
  set(types::ran_p_choice_structure);
  return c.get<ran_param_testing_item_choice_structure_s>();
}
ran_param_testing_item_choice_elem_true_s& ran_param_testing_item_s::ran_param_type_c_::set_ran_p_choice_elem_true()
{
  set(types::ran_p_choice_elem_true);
  return c.get<ran_param_testing_item_choice_elem_true_s>();
}
ran_param_testing_item_choice_elem_false_s& ran_param_testing_item_s::ran_param_type_c_::set_ran_p_choice_elem_false()
{
  set(types::ran_p_choice_elem_false);
  return c.get<ran_param_testing_item_choice_elem_false_s>();
}

static const char*           enum_names_ran_param_testing_item_s_ran_param_type_c__types_values[] = {"ranP-Choice-List",
                                                                                                     "ranP-Choice-Structure",
                                                                                                     "ranP-Choice-ElementTrue",
                                                                                                     "ranP-Choice-ElementFalse"};
const asn1::table::enum_desc ran_param_testing_item_s::ran_param_type_c_::types_opts::desc        = {
    enum_names_ran_param_testing_item_s_ran_param_type_c__types_values,
    4,
    "ran_param_testing_item_s::ran_param_type_c_::types"};

// RANParameter-Testing-Item-Choice-List ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_testing_item_choice_list_s_fields[] = {
    {offsetof(ran_param_testing_item_choice_list_s, ran_param_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_param_testing_item_choice_list_s::ran_param_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_testing_item_s::desc}};
static const asn1::table::field_name seq_table_ran_param_testing_item_choice_list_s_names[] = {
    {"ranParameter-List", nullptr}};
const asn1::table::seq_desc ran_param_testing_item_choice_list_s::desc = {
    {seq_table_ran_param_testing_item_choice_list_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ran_param_testing_item_choice_list_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RANParameter-Testing-Item-Choice-Structure ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_testing_item_choice_structure_s_fields[] = {
    {offsetof(ran_param_testing_item_choice_structure_s, ran_param_structure),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_param_testing_item_choice_structure_s::ran_param_structure),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_testing_item_s::desc}};
static const asn1::table::field_name seq_table_ran_param_testing_item_choice_structure_s_names[] = {
    {"ranParameter-Structure", nullptr}};
const asn1::table::seq_desc ran_param_testing_item_choice_structure_s::desc = {
    {seq_table_ran_param_testing_item_choice_structure_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ran_param_testing_item_choice_structure_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RANParameter-Value ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ran_param_value_c_alternatives[] = {
    &asn1::table::alternative_ops<bool, false>::value,
    &asn1::table::alternative_ops<int64_t, true>::value,
    &asn1::table::alternative_ops<real_s, false>::value,
    &asn1::table::alternative_ops<unbounded_bitstring<false, true>, false>::value,
    &asn1::table::alternative_ops<unbounded_octstring<true>, false>::value,
    &asn1::table::alternative_ops<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>, false, false>::value};
const asn1::table::choice_storage_desc ran_param_value_c::storage_desc = {choice_storage_ran_param_value_c_alternatives,
                                                                          6,
                                                                          "ran_param_value_c"};

bool& ran_param_value_c::set_value_boolean()
{
  set(types::value_boolean);
  return c.get<bool>();
}
int64_t& ran_param_value_c::set_value_int()
{
  set(types::value_int);
  return c.get<int64_t>();
}
real_s& ran_param_value_c::set_value_real()
{
  set(types::value_real);
  return c.get<real_s>();
}
unbounded_bitstring<false, true>& ran_param_value_c::set_value_bit_s()
{
  set(types::value_bit_s);
  return c.get<unbounded_bitstring<false, true>>();
}
unbounded_octstring<true>& ran_param_value_c::set_value_oct_s()
{
  set(types::value_oct_s);
  return c.get<unbounded_octstring<true>>();
}
printable_string<0, MAX_ASN_STRING_LENGTH, false, true>& ran_param_value_c::set_value_printable_string()
{
  set(types::value_printable_string);
  return c.get<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>>();
}
static const asn1::table::seq_field choice_table_ran_param_value_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::boolean, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<real_s, asn1::table::float_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<unbounded_bitstring<false, true>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<unbounded_octstring<true>, asn1::table::string_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<printable_string<0, MAX_ASN_STRING_LENGTH, false, true>,
                                asn1::table::string_json>::value}};
static const asn1::table::field_name choice_table_ran_param_value_c_names[] = {{"valueBoolean", nullptr},
                                                                               {"valueInt", nullptr},
                                                                               {"valueReal", nullptr},
                                                                               {"valueBitS", nullptr},
                                                                               {"valueOctS", nullptr},
                                                                               {"valuePrintableString", nullptr}};
const asn1::table::choice_desc       ran_param_value_c::desc = {asn1::table::enum_int_type<ran_param_value_c::types>(),
                                                                ran_param_value_c::types::nof_types,
                                                                ran_param_value_c::types::nof_exts,
                                                                ran_param_value_c::types::has_ext,
                                                                true,
                                                                choice_table_ran_param_value_c_alternatives,
                                                                nullptr,
                                                                choice_table_ran_param_value_c_names,
                                                                "ran_param_value_c",
                                                                offsetof(ran_param_value_c, c.type),
                                                                offsetof(ran_param_value_c, c.value),
                                                                false,
                                                                &ran_param_value_c::storage_desc};

static const char* enum_names_ran_param_value_c_types_values[] =
    {"valueBoolean", "valueInt", "valueReal", "valueBitS", "valueOctS", "valuePrintableString"};
const asn1::table::enum_desc ran_param_value_c::types_opts::desc = {enum_names_ran_param_value_c_types_values,
                                                                    6,
                                                                    "ran_param_value_c::types"};

// RANParameter-Testing-Item-Choice-ElementTrue ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_testing_item_choice_elem_true_s_fields[] = {
    {offsetof(ran_param_testing_item_choice_elem_true_s, ran_param_value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_value_c::desc}};
static const asn1::table::field_name seq_table_ran_param_testing_item_choice_elem_true_s_names[] = {
    {"ranParameter-value", nullptr}};
const asn1::table::seq_desc ran_param_testing_item_choice_elem_true_s::desc = {
    {seq_table_ran_param_testing_item_choice_elem_true_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ran_param_testing_item_choice_elem_true_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RANParameter-TestingCondition ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ran_param_testing_condition_c_alternatives[] = {
    &asn1::table::alternative_ops<ran_param_testing_condition_c::ran_p_choice_comparison_e_, true>::value,
    &asn1::table::alternative_ops<ran_param_testing_condition_c::ran_p_choice_presence_e_, true>::value};
const asn1::table::choice_storage_desc ran_param_testing_condition_c::storage_desc = {
    choice_storage_ran_param_testing_condition_c_alternatives,
    2,
    "ran_param_testing_condition_c"};

ran_param_testing_condition_c::ran_p_choice_comparison_e_& ran_param_testing_condition_c::set_ran_p_choice_comparison()
{
  set(types::ran_p_choice_comparison);
  return c.get<ran_p_choice_comparison_e_>();
}
ran_param_testing_condition_c::ran_p_choice_presence_e_& ran_param_testing_condition_c::set_ran_p_choice_presence()
{
  set(types::ran_p_choice_presence);
  return c.get<ran_p_choice_presence_e_>();
}
static const asn1::table::seq_field choice_table_ran_param_testing_condition_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<ran_param_testing_condition_c::ran_p_choice_comparison_e_>(),
     ran_param_testing_condition_c::ran_p_choice_comparison_e_::has_ext,
     false,
     ran_param_testing_condition_c::ran_p_choice_comparison_e_::nof_types,
     ran_param_testing_condition_c::ran_p_choice_comparison_e_::nof_exts,
     nullptr,
     &ran_param_testing_condition_c::ran_p_choice_comparison_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<ran_param_testing_condition_c::ran_p_choice_presence_e_>(),
     ran_param_testing_condition_c::ran_p_choice_presence_e_::has_ext,
     false,
     ran_param_testing_condition_c::ran_p_choice_presence_e_::nof_types,
     ran_param_testing_condition_c::ran_p_choice_presence_e_::nof_exts,
     nullptr,
     &ran_param_testing_condition_c::ran_p_choice_presence_opts::desc}};
static const asn1::table::field_name choice_table_ran_param_testing_condition_c_names[] = {
    {"ranP-Choice-comparison", nullptr},
    {"ranP-Choice-presence", nullptr}};
const asn1::table::choice_desc ran_param_testing_condition_c::desc = {
    asn1::table::enum_int_type<ran_param_testing_condition_c::types>(),
    ran_param_testing_condition_c::types::nof_types,
    ran_param_testing_condition_c::types::nof_exts,
    ran_param_testing_condition_c::types::has_ext,
    true,
    choice_table_ran_param_testing_condition_c_alternatives,
    nullptr,
    choice_table_ran_param_testing_condition_c_names,
    "ran_param_testing_condition_c",
    offsetof(ran_param_testing_condition_c, c.type),
    offsetof(ran_param_testing_condition_c, c.value),
    false,
    &ran_param_testing_condition_c::storage_desc};

static const char* enum_names_ran_param_testing_condition_c_ran_p_choice_comparison_e__values[] =
    {"equal", "difference", "greaterthan", "lessthan", "contains", "starts-with"};
const asn1::table::enum_desc ran_param_testing_condition_c::ran_p_choice_comparison_opts::desc = {
    enum_names_ran_param_testing_condition_c_ran_p_choice_comparison_e__values,
    6,
    "ran_param_testing_condition_c::ran_p_choice_comparison_e_"};

static const char*           enum_names_ran_param_testing_condition_c_ran_p_choice_presence_e__values[] = {"present",
                                                                                                           "configured",
                                                                                                           "rollover",
                                                                                                           "non-zero",
                                                                                                           "value-change"};
const asn1::table::enum_desc ran_param_testing_condition_c::ran_p_choice_presence_opts::desc            = {
    enum_names_ran_param_testing_condition_c_ran_p_choice_presence_e__values,
    5,
    "ran_param_testing_condition_c::ran_p_choice_presence_e_"};
uint8_t ran_param_testing_condition_c::ran_p_choice_presence_opts::to_number() const
{
  if (value == non_zero) {
    return 0;
  }
  invalid_enum_number(value, "ran_param_testing_condition_c::ran_p_choice_presence_e_");
  return 0;
}

static const char*           enum_names_ran_param_testing_condition_c_types_values[] = {"ranP-Choice-comparison",
                                                                                        "ranP-Choice-presence"};
const asn1::table::enum_desc ran_param_testing_condition_c::types_opts::desc         = {
    enum_names_ran_param_testing_condition_c_types_values,
    2,
    "ran_param_testing_condition_c::types"};

// LogicalOR-E2SM-RC-IEs ::= ENUMERATED
static const char*           enum_names_lc_or_e2sm_rc_ies_e_values[] = {"true", "false"};
const asn1::table::enum_desc lc_or_e2sm_rc_ies_opts::desc            = {enum_names_lc_or_e2sm_rc_ies_e_values,
                                                                        2,
                                                                        "lc_or_e2sm_rc_ies_e"};

// RANParameter-Testing-Item-Choice-ElementFalse ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_testing_item_choice_elem_false_s_fields[] = {
    {offsetof(ran_param_testing_item_choice_elem_false_s, ran_param_test_condition),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_testing_condition_c::desc},
    {offsetof(ran_param_testing_item_choice_elem_false_s, ran_param_value),
     (int32_t)offsetof(ran_param_testing_item_choice_elem_false_s, ran_param_value_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_value_c::desc},
    {offsetof(ran_param_testing_item_choice_elem_false_s, lc_or),
     (int32_t)offsetof(ran_param_testing_item_choice_elem_false_s, lc_or_present),
     asn1::table::enum_op<decltype(ran_param_testing_item_choice_elem_false_s::lc_or)>(),
     decltype(ran_param_testing_item_choice_elem_false_s::lc_or)::has_ext,
     false,
     decltype(ran_param_testing_item_choice_elem_false_s::lc_or)::nof_types,
     decltype(ran_param_testing_item_choice_elem_false_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_e2sm_rc_ies_opts::desc}};
static const asn1::table::field_name seq_table_ran_param_testing_item_choice_elem_false_s_names[] = {
    {"ranParameter-TestCondition", nullptr},
    {"ranParameter-Value", nullptr},
    {"logicalOR", nullptr}};
static const asn1::table::seq_opt seq_table_ran_param_testing_item_choice_elem_false_s_opts[] = {
    {1, (int32_t)offsetof(ran_param_testing_item_choice_elem_false_s, ran_param_value_present)},
    {2, (int32_t)offsetof(ran_param_testing_item_choice_elem_false_s, lc_or_present)}};
const asn1::table::seq_desc ran_param_testing_item_choice_elem_false_s::desc = {
    {seq_table_ran_param_testing_item_choice_elem_false_s_fields,
     uint64_t{0x1},
     seq_table_ran_param_testing_item_choice_elem_false_s_opts,
     seq_table_ran_param_testing_item_choice_elem_false_s_names,
     3,
     2},
    true,
    true,
    nullptr,
    0};

// EventTrigger-UE-Info-Item-Choice-Individual ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_ue_info_item_choice_individual_s_fields[] = {
    {offsetof(event_trigger_ue_info_item_choice_individual_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc},
    {offsetof(event_trigger_ue_info_item_choice_individual_s, ran_param_testing),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::item_list_codec<decltype(event_trigger_ue_info_item_choice_individual_s::ran_param_testing),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_testing_item_s::desc}};
static const asn1::table::field_name seq_table_event_trigger_ue_info_item_choice_individual_s_names[] = {
    {"ueID", nullptr},
    {"ranParameterTesting", nullptr}};
static const asn1::table::seq_opt seq_table_event_trigger_ue_info_item_choice_individual_s_opts[] = {
    {1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc event_trigger_ue_info_item_choice_individual_s::desc = {
    {seq_table_event_trigger_ue_info_item_choice_individual_s_fields,
     uint64_t{0x1},
     seq_table_event_trigger_ue_info_item_choice_individual_s_opts,
     seq_table_event_trigger_ue_info_item_choice_individual_s_names,
     2,
     1},
    true,
    true,
    nullptr,
    0};

// EventTrigger-UE-Info-Item-Choice-Group ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_ue_info_item_choice_group_s_fields[] = {
    {offsetof(event_trigger_ue_info_item_choice_group_s, ran_param_testing),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::item_list_codec<decltype(event_trigger_ue_info_item_choice_group_s::ran_param_testing),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_testing_item_s::desc}};
static const asn1::table::field_name seq_table_event_trigger_ue_info_item_choice_group_s_names[] = {
    {"ranParameterTesting", nullptr}};
const asn1::table::seq_desc event_trigger_ue_info_item_choice_group_s::desc = {
    {seq_table_event_trigger_ue_info_item_choice_group_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_event_trigger_ue_info_item_choice_group_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// UEQuery ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_query_s_fields[] = {
    {offsetof(ue_query_s, partial_ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &partial_ue_id_s::desc},
    {offsetof(ue_query_s, ran_param_testing),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::
         item_list_codec<decltype(ue_query_s::ran_param_testing), asn1::table::sequence_item_ops, false, true>::value,
     &ran_param_testing_item_s::desc}};
static const asn1::table::field_name seq_table_ue_query_s_names[] = {{"partialUEID", nullptr},
                                                                     {"ranParameterTesting", nullptr}};
static const asn1::table::seq_opt    seq_table_ue_query_s_opts[]  = {{1, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc          ue_query_s::desc             = {
    {seq_table_ue_query_s_fields, uint64_t{0x1}, seq_table_ue_query_s_opts, seq_table_ue_query_s_names, 2, 1},
    true,
    true,
    nullptr,
    0};

// Associated-UE-Info-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_associated_ue_info_item_s_fields[] = {
    {offsetof(associated_ue_info_item_s, ue_filt_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(associated_ue_info_item_s, ue_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &associated_ue_info_item_s::ue_type_c_::desc},
    {offsetof(associated_ue_info_item_s, lc_or),
     (int32_t)offsetof(associated_ue_info_item_s, lc_or_present),
     asn1::table::enum_op<decltype(associated_ue_info_item_s::lc_or)>(),
     decltype(associated_ue_info_item_s::lc_or)::has_ext,
     false,
     decltype(associated_ue_info_item_s::lc_or)::nof_types,
     decltype(associated_ue_info_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_e2sm_rc_ies_opts::desc}};
static const asn1::table::field_name seq_table_associated_ue_info_item_s_names[] = {{"ueFilterID", nullptr},
                                                                                    {"ueType", nullptr},
                                                                                    {"logicalOR", nullptr}};
static const asn1::table::seq_opt    seq_table_associated_ue_info_item_s_opts[]  = {
    {2, (int32_t)offsetof(associated_ue_info_item_s, lc_or_present)}};
const asn1::table::seq_desc associated_ue_info_item_s::desc = {{seq_table_associated_ue_info_item_s_fields,
                                                                uint64_t{0x3},
                                                                seq_table_associated_ue_info_item_s_opts,
                                                                seq_table_associated_ue_info_item_s_names,
                                                                3,
                                                                1},
                                                               true,
                                                               true,
                                                               nullptr,
                                                               0};

static const asn1::table::storage_ops* const choice_storage_associated_ue_info_item_s_ue_type_c__alternatives[] = {
    &asn1::table::alternative_ops<event_trigger_ue_info_item_choice_individual_s, false, false>::value,
    &asn1::table::alternative_ops<event_trigger_ue_info_item_choice_group_s, false, false>::value,
    &asn1::table::alternative_ops<ue_query_s, false, false>::value};
const asn1::table::choice_storage_desc associated_ue_info_item_s::ue_type_c_::storage_desc = {
    choice_storage_associated_ue_info_item_s_ue_type_c__alternatives,
    3,
    "associated_ue_info_item_s::ue_type_c_"};

event_trigger_ue_info_item_choice_individual_s& associated_ue_info_item_s::ue_type_c_::set_ue_type_choice_individual()
{
  set(types::ue_type_choice_individual);
  return c.get<event_trigger_ue_info_item_choice_individual_s>();
}
event_trigger_ue_info_item_choice_group_s& associated_ue_info_item_s::ue_type_c_::set_ue_type_choice_group()
{
  set(types::ue_type_choice_group);
  return c.get<event_trigger_ue_info_item_choice_group_s>();
}
ue_query_s& associated_ue_info_item_s::ue_type_c_::set_ue_query()
{
  set(types::ue_query);
  return c.get<ue_query_s>();
}
static const asn1::table::seq_field choice_table_associated_ue_info_item_s_ue_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_ue_info_item_choice_individual_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_ue_info_item_choice_group_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_query_s::desc}};
static const asn1::table::field_name choice_table_associated_ue_info_item_s_ue_type_c__names[] = {
    {"ueType-Choice-Individual", nullptr},
    {"ueType-Choice-Group", nullptr},
    {"ueQuery", nullptr}};
const asn1::table::choice_desc associated_ue_info_item_s::ue_type_c_::desc = {
    asn1::table::enum_int_type<associated_ue_info_item_s::ue_type_c_::types>(),
    associated_ue_info_item_s::ue_type_c_::types::nof_types,
    associated_ue_info_item_s::ue_type_c_::types::nof_exts,
    associated_ue_info_item_s::ue_type_c_::types::has_ext,
    true,
    choice_table_associated_ue_info_item_s_ue_type_c__alternatives,
    nullptr,
    choice_table_associated_ue_info_item_s_ue_type_c__names,
    "associated_ue_info_item_s::ue_type_c_",
    offsetof(associated_ue_info_item_s::ue_type_c_, c.type),
    offsetof(associated_ue_info_item_s::ue_type_c_, c.value),
    false,
    &associated_ue_info_item_s::ue_type_c_::storage_desc};

static const char* enum_names_associated_ue_info_item_s_ue_type_c__types_values[]    = {"ueType-Choice-Individual",
                                                                                        "ueType-Choice-Group",
                                                                                        "ueQuery"};
const asn1::table::enum_desc associated_ue_info_item_s::ue_type_c_::types_opts::desc = {
    enum_names_associated_ue_info_item_s_ue_type_c__types_values,
    3,
    "associated_ue_info_item_s::ue_type_c_::types"};

// Associated-UE-Info ::= SEQUENCE
static const asn1::table::seq_field seq_table_associated_ue_info_s_fields[] = {
    {offsetof(associated_ue_info_s, associated_ue_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(associated_ue_info_s::associated_ue_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &associated_ue_info_item_s::desc}};
static const asn1::table::field_name seq_table_associated_ue_info_s_names[] = {{"associatedUEInfo-List", nullptr}};
const asn1::table::seq_desc          associated_ue_info_s::desc             = {
    {seq_table_associated_ue_info_s_fields, uint64_t{0x1}, nullptr, seq_table_associated_ue_info_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// RANParameter-Definition-Choice ::= CHOICE
static const asn1::table::seq_field choice_table_ran_param_definition_choice_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ran_param_definition_choice_list_s>::value,
     &ran_param_definition_choice_list_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ran_param_definition_choice_structure_s>::value,
     &ran_param_definition_choice_structure_s::desc}};
static const asn1::table::field_name choice_table_ran_param_definition_choice_c_names[] = {
    {"choiceLIST", nullptr},
    {"choiceSTRUCTURE", nullptr}};
const asn1::table::choice_desc ran_param_definition_choice_c::desc = {
    asn1::table::enum_int_type<ran_param_definition_choice_c::types>(),
    ran_param_definition_choice_c::types::nof_types,
    ran_param_definition_choice_c::types::nof_exts,
    ran_param_definition_choice_c::types::has_ext,
    true,
    choice_table_ran_param_definition_choice_c_alternatives,
    &asn1::table::choice_set<ran_param_definition_choice_c>,
    choice_table_ran_param_definition_choice_c_names,
    "ran_param_definition_choice_c",
    offsetof(ran_param_definition_choice_c, type_),
    offsetof(ran_param_definition_choice_c, c),
    false};

void ran_param_definition_choice_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, ran_param_definition_choice_c::desc);
}
ran_param_definition_choice_list_s& ran_param_definition_choice_c::set_choice_list()
{
  set(types::choice_list);
  return c.get<ran_param_definition_choice_list_s>();
}
ran_param_definition_choice_structure_s& ran_param_definition_choice_c::set_choice_structure()
{
  set(types::choice_structure);
  return c.get<ran_param_definition_choice_structure_s>();
}

static const char* enum_names_ran_param_definition_choice_c_types_values[]   = {"choiceLIST", "choiceSTRUCTURE"};
const asn1::table::enum_desc ran_param_definition_choice_c::types_opts::desc = {
    enum_names_ran_param_definition_choice_c_types_values,
    2,
    "ran_param_definition_choice_c::types"};

// RANParameter-Definition ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_definition_s_fields[] = {
    {offsetof(ran_param_definition_s, ran_param_definition_choice),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_param_definition_s::ran_param_definition_choice),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ran_param_definition_s_names[] = {
    {"ranParameter-Definition-Choice", nullptr}};
const asn1::table::seq_desc ran_param_definition_s::desc = {
    {seq_table_ran_param_definition_s_fields, uint64_t{0x1}, nullptr, seq_table_ran_param_definition_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// RANParameter-Definition-Choice-LIST-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_definition_choice_list_item_s_fields[] = {
    {offsetof(ran_param_definition_choice_list_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(ran_param_definition_choice_list_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_param_definition_choice_list_item_s::ran_param_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_param_definition_choice_list_item_s, ran_param_definition),
     (int32_t)offsetof(ran_param_definition_choice_list_item_s, ran_param_definition_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_ran_param_definition_choice_list_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-name", nullptr},
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_ran_param_definition_choice_list_item_s_opts[] = {
    {2, (int32_t)offsetof(ran_param_definition_choice_list_item_s, ran_param_definition_present)}};
const asn1::table::seq_desc ran_param_definition_choice_list_item_s::desc = {
    {seq_table_ran_param_definition_choice_list_item_s_fields,
     uint64_t{0x3},
     seq_table_ran_param_definition_choice_list_item_s_opts,
     seq_table_ran_param_definition_choice_list_item_s_names,
     3,
     1},
    true,
    true,
    nullptr,
    0};

// RANParameter-Definition-Choice-LIST ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_definition_choice_list_s_fields[] = {
    {offsetof(ran_param_definition_choice_list_s, ran_param_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_param_definition_choice_list_s::ran_param_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_definition_choice_list_item_s::desc}};
static const asn1::table::field_name seq_table_ran_param_definition_choice_list_s_names[] = {
    {"ranParameter-List", nullptr}};
const asn1::table::seq_desc ran_param_definition_choice_list_s::desc = {
    {seq_table_ran_param_definition_choice_list_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ran_param_definition_choice_list_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RANParameter-Definition-Choice-STRUCTURE-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_definition_choice_structure_item_s_fields[] = {
    {offsetof(ran_param_definition_choice_structure_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(ran_param_definition_choice_structure_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_param_definition_choice_structure_item_s::ran_param_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_param_definition_choice_structure_item_s, ran_param_definition),
     (int32_t)offsetof(ran_param_definition_choice_structure_item_s, ran_param_definition_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_ran_param_definition_choice_structure_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-name", nullptr},
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_ran_param_definition_choice_structure_item_s_opts[] = {
    {2, (int32_t)offsetof(ran_param_definition_choice_structure_item_s, ran_param_definition_present)}};
const asn1::table::seq_desc ran_param_definition_choice_structure_item_s::desc = {
    {seq_table_ran_param_definition_choice_structure_item_s_fields,
     uint64_t{0x3},
     seq_table_ran_param_definition_choice_structure_item_s_opts,
     seq_table_ran_param_definition_choice_structure_item_s_names,
     3,
     1},
    true,
    true,
    nullptr,
    0};

// RANParameter-Definition-Choice-STRUCTURE ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_definition_choice_structure_s_fields[] = {
    {offsetof(ran_param_definition_choice_structure_s, ran_param_structure),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_param_definition_choice_structure_s::ran_param_structure),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_definition_choice_structure_item_s::desc}};
static const asn1::table::field_name seq_table_ran_param_definition_choice_structure_s_names[] = {
    {"ranParameter-STRUCTURE", nullptr}};
const asn1::table::seq_desc ran_param_definition_choice_structure_s::desc = {
    {seq_table_ran_param_definition_choice_structure_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ran_param_definition_choice_structure_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// CallProcessBreakpoint-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_call_process_breakpoint_ran_param_item_s_fields[] = {
    {offsetof(call_process_breakpoint_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(call_process_breakpoint_ran_param_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(call_process_breakpoint_ran_param_item_s::ran_param_name),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_call_process_breakpoint_ran_param_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-name", nullptr}};
static const asn1::table::seq_field seq_table_call_process_breakpoint_ran_param_item_s_g0_fields[] = {
    {offsetof(call_process_breakpoint_ran_param_item_s, ran_param_definition),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(call_process_breakpoint_ran_param_item_s::ran_param_definition)>::value,
     &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_call_process_breakpoint_ran_param_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_call_process_breakpoint_ran_param_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_call_process_breakpoint_ran_param_item_s_groups[] = {
    {{seq_table_call_process_breakpoint_ran_param_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_call_process_breakpoint_ran_param_item_s_g0_opts,
      seq_table_call_process_breakpoint_ran_param_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc call_process_breakpoint_ran_param_item_s::desc = {
    {seq_table_call_process_breakpoint_ran_param_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_call_process_breakpoint_ran_param_item_s_names,
     2,
     0},
    true,
    true,
    seq_table_call_process_breakpoint_ran_param_item_s_groups,
    1};

// CauseProtocol ::= ENUMERATED
static const char*           enum_names_cause_protocol_e_values[] = {"transfer-syntax-error",
                                                                     "abstract-syntax-error-reject",
                                                                     "abstract-syntax-error-ignore-and-notify",
                                                                     "message-not-compatible-with-receiver-state",
                                                                     "semantic-error",
                                                                     "abstract-syntax-error-falsely-constructed-message",
                                                                     "unspecified"};
const asn1::table::enum_desc cause_protocol_opts::desc = {enum_names_cause_protocol_e_values, 7, "cause_protocol_e"};

// CauseRICservice ::= ENUMERATED
static const char*           enum_names_cause_ri_cservice_e_values[] = {"RIC-Event-Trigger-Style-Type-not-supported",
                                                                        "RIC-Event-Trigger-Format-Type-not-supported",
                                                                        "RAN-Parameter-ID-not-supported",
                                                                        "Call-Process-Type-ID-not-supported",
                                                                        "Call-Process-Breakpoint-ID-not-supported",
                                                                        "RIC-Report-Style-Type-not-supported",
                                                                        "Supported-RIC-Event-Trigger-Style-Type-not-supported",
                                                                        "RIC-Action-Format-Type-not-supported",
                                                                        "RIC-Indication-Header-Format-Type-not-supported",
                                                                        "RIC-Indication-Message-Format-Type-not-supported",
                                                                        "RIC-Insert-Style-Type-not-supported",
                                                                        "Insert-Indication-ID-not-supported",
                                                                        "RIC-Call-Process-ID-Format-Type-not-supported",
                                                                        "RIC-Control-Style-Type-not-supported",
                                                                        "Control-Action-ID-not-supported",
                                                                        "RIC-Control-Header-Format-Type-not-supported",
                                                                        "RIC-Control-Message-Format-Type-not-supported",
                                                                        "RIC-Control-Outcome-Format-Type-not-supported",
                                                                        "RIC-Policy-Style-Type-not-supported",
                                                                        "Policy-Action-ID-not-supported",
                                                                        "RIC-Query-Style-Type-not-supported",
                                                                        "RIC-Query-Header-Format-Type-not-supported",
                                                                        "RIC-Query-Definition-Format-Type-not-supported",
                                                                        "RIC-Query-Outcome-Format-Type-not-supported"};
const asn1::table::enum_desc cause_ri_cservice_opts::desc            = {enum_names_cause_ri_cservice_e_values,
                                                                        24,
                                                                        "cause_ri_cservice_e"};

// CellIdentification-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_cell_identif_ran_param_item_s_fields[] = {
    {offsetof(cell_identif_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(cell_identif_ran_param_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(cell_identif_ran_param_item_s::ran_param_name),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_cell_identif_ran_param_item_s_names[] = {{"ranParameter-ID", nullptr},
                                                                                        {"ranParameter-name", nullptr}};
static const asn1::table::seq_field  seq_table_cell_identif_ran_param_item_s_g0_fields[] = {
    {offsetof(cell_identif_ran_param_item_s, ran_param_definition),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(cell_identif_ran_param_item_s::ran_param_definition)>::value,
      &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_cell_identif_ran_param_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_cell_identif_ran_param_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_cell_identif_ran_param_item_s_groups[] = {
    {{seq_table_cell_identif_ran_param_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_cell_identif_ran_param_item_s_g0_opts,
      seq_table_cell_identif_ran_param_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc cell_identif_ran_param_item_s::desc = {{seq_table_cell_identif_ran_param_item_s_fields,
                                                                    uint64_t{0x3},
                                                                    nullptr,
                                                                    seq_table_cell_identif_ran_param_item_s_names,
                                                                    2,
                                                                    0},
                                                                   true,
                                                                   true,
                                                                   seq_table_cell_identif_ran_param_item_s_groups,
                                                                   1};

// ControlAction-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ctrl_action_ran_param_item_s_fields[] = {
    {offsetof(ctrl_action_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(ctrl_action_ran_param_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ctrl_action_ran_param_item_s::ran_param_name),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_ctrl_action_ran_param_item_s_names[]     = {{"ranParameter-ID", nullptr},
                                                                                           {"ranParameter-name", nullptr}};
static const asn1::table::seq_field  seq_table_ctrl_action_ran_param_item_s_g0_fields[] = {
    {offsetof(ctrl_action_ran_param_item_s, ran_param_definition),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(ctrl_action_ran_param_item_s::ran_param_definition)>::value,
      &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_ctrl_action_ran_param_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_ctrl_action_ran_param_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_ctrl_action_ran_param_item_s_groups[] = {
    {{seq_table_ctrl_action_ran_param_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_ctrl_action_ran_param_item_s_g0_opts,
      seq_table_ctrl_action_ran_param_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc ctrl_action_ran_param_item_s::desc = {{seq_table_ctrl_action_ran_param_item_s_fields,
                                                                   uint64_t{0x3},
                                                                   nullptr,
                                                                   seq_table_ctrl_action_ran_param_item_s_names,
                                                                   2,
                                                                   0},
                                                                  true,
                                                                  true,
                                                                  seq_table_ctrl_action_ran_param_item_s_groups,
                                                                  1};

// ControlOutcome-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ctrl_outcome_ran_param_item_s_fields[] = {
    {offsetof(ctrl_outcome_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(ctrl_outcome_ran_param_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ctrl_outcome_ran_param_item_s::ran_param_name),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_ctrl_outcome_ran_param_item_s_names[] = {{"ranParameter-ID", nullptr},
                                                                                        {"ranParameter-name", nullptr}};
static const asn1::table::seq_field  seq_table_ctrl_outcome_ran_param_item_s_g0_fields[] = {
    {offsetof(ctrl_outcome_ran_param_item_s, ran_param_definition),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(ctrl_outcome_ran_param_item_s::ran_param_definition)>::value,
      &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_ctrl_outcome_ran_param_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_ctrl_outcome_ran_param_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_ctrl_outcome_ran_param_item_s_groups[] = {
    {{seq_table_ctrl_outcome_ran_param_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_ctrl_outcome_ran_param_item_s_g0_opts,
      seq_table_ctrl_outcome_ran_param_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc ctrl_outcome_ran_param_item_s::desc = {{seq_table_ctrl_outcome_ran_param_item_s_fields,
                                                                    uint64_t{0x3},
                                                                    nullptr,
                                                                    seq_table_ctrl_outcome_ran_param_item_s_names,
                                                                    2,
                                                                    0},
                                                                   true,
                                                                   true,
                                                                   seq_table_ctrl_outcome_ran_param_item_s_groups,
                                                                   1};

// E2SM-RC-ActionDefinition-Format1-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format1_item_s_fields[] = {
    {offsetof(e2sm_rc_action_definition_format1_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format1_item_s_names[] = {
    {"ranParameter-ID", nullptr}};
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format1_item_s_g0_fields[] = {
    {offsetof(e2sm_rc_action_definition_format1_item_s, ran_param_definition),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(e2sm_rc_action_definition_format1_item_s::ran_param_definition)>::value,
     &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format1_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_action_definition_format1_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_e2sm_rc_action_definition_format1_item_s_groups[] = {
    {{seq_table_e2sm_rc_action_definition_format1_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_e2sm_rc_action_definition_format1_item_s_g0_opts,
      seq_table_e2sm_rc_action_definition_format1_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc e2sm_rc_action_definition_format1_item_s::desc = {
    {seq_table_e2sm_rc_action_definition_format1_item_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_rc_action_definition_format1_item_s_names,
     1,
     0},
    true,
    true,
    seq_table_e2sm_rc_action_definition_format1_item_s_groups,
    1};

// E2SM-RC-ActionDefinition-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format1_s_fields[] = {
    {offsetof(e2sm_rc_action_definition_format1_s, ran_p_to_be_reported_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_action_definition_format1_s::ran_p_to_be_reported_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_action_definition_format1_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format1_s_names[] = {
    {"ranP-ToBeReported-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_action_definition_format1_s::desc = {
    {seq_table_e2sm_rc_action_definition_format1_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_rc_action_definition_format1_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RANParameter-ValueType-Choice-ElementTrue ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_value_type_choice_elem_true_s_fields[] = {
    {offsetof(ran_param_value_type_choice_elem_true_s, ran_param_value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_value_c::desc}};
static const asn1::table::field_name seq_table_ran_param_value_type_choice_elem_true_s_names[] = {
    {"ranParameter-value", nullptr}};
const asn1::table::seq_desc ran_param_value_type_choice_elem_true_s::desc = {
    {seq_table_ran_param_value_type_choice_elem_true_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ran_param_value_type_choice_elem_true_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RANParameter-ValueType-Choice-ElementFalse ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_value_type_choice_elem_false_s_fields[] = {
    {offsetof(ran_param_value_type_choice_elem_false_s, ran_param_value),
     (int32_t)offsetof(ran_param_value_type_choice_elem_false_s, ran_param_value_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_value_c::desc}};
static const asn1::table::field_name seq_table_ran_param_value_type_choice_elem_false_s_names[] = {
    {"ranParameter-value", nullptr}};
static const asn1::table::seq_opt seq_table_ran_param_value_type_choice_elem_false_s_opts[] = {
    {0, (int32_t)offsetof(ran_param_value_type_choice_elem_false_s, ran_param_value_present)}};
const asn1::table::seq_desc ran_param_value_type_choice_elem_false_s::desc = {
    {seq_table_ran_param_value_type_choice_elem_false_s_fields,
     uint64_t{0x0},
     seq_table_ran_param_value_type_choice_elem_false_s_opts,
     seq_table_ran_param_value_type_choice_elem_false_s_names,
     1,
     1},
    true,
    true,
    nullptr,
    0};

// RANParameter-ValueType ::= CHOICE
static const asn1::table::seq_field choice_table_ran_param_value_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ran_param_value_type_choice_elem_true_s>::value,
     &ran_param_value_type_choice_elem_true_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ran_param_value_type_choice_elem_false_s>::value,
     &ran_param_value_type_choice_elem_false_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ran_param_value_type_choice_structure_s>::value,
     &ran_param_value_type_choice_structure_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ran_param_value_type_choice_list_s>::value,
     &ran_param_value_type_choice_list_s::desc}};
static const asn1::table::field_name choice_table_ran_param_value_type_c_names[] = {
    {"ranP-Choice-ElementTrue", nullptr},
    {"ranP-Choice-ElementFalse", nullptr},
    {"ranP-Choice-Structure", nullptr},
    {"ranP-Choice-List", nullptr}};
const asn1::table::choice_desc ran_param_value_type_c::desc = {
    asn1::table::enum_int_type<ran_param_value_type_c::types>(),
    ran_param_value_type_c::types::nof_types,
    ran_param_value_type_c::types::nof_exts,
    ran_param_value_type_c::types::has_ext,
    true,
    choice_table_ran_param_value_type_c_alternatives,
    &asn1::table::choice_set<ran_param_value_type_c>,
    choice_table_ran_param_value_type_c_names,
    "ran_param_value_type_c",
    offsetof(ran_param_value_type_c, type_),
    offsetof(ran_param_value_type_c, c),
    false};

void ran_param_value_type_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, ran_param_value_type_c::desc);
}
ran_param_value_type_choice_elem_true_s& ran_param_value_type_c::set_ran_p_choice_elem_true()
{
  set(types::ran_p_choice_elem_true);
  return c.get<ran_param_value_type_choice_elem_true_s>();
}
ran_param_value_type_choice_elem_false_s& ran_param_value_type_c::set_ran_p_choice_elem_false()
{
  set(types::ran_p_choice_elem_false);
  return c.get<ran_param_value_type_choice_elem_false_s>();
}
ran_param_value_type_choice_structure_s& ran_param_value_type_c::set_ran_p_choice_structure()
{
  set(types::ran_p_choice_structure);
  return c.get<ran_param_value_type_choice_structure_s>();
}
ran_param_value_type_choice_list_s& ran_param_value_type_c::set_ran_p_choice_list()
{
  set(types::ran_p_choice_list);
  return c.get<ran_param_value_type_choice_list_s>();
}

static const char*           enum_names_ran_param_value_type_c_types_values[] = {"ranP-Choice-ElementTrue",
                                                                                 "ranP-Choice-ElementFalse",
                                                                                 "ranP-Choice-Structure",
                                                                                 "ranP-Choice-List"};
const asn1::table::enum_desc ran_param_value_type_c::types_opts::desc = {enum_names_ran_param_value_type_c_types_values,
                                                                         4,
                                                                         "ran_param_value_type_c::types"};

// RANParameter-STRUCTURE-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_structure_item_s_fields[] = {
    {offsetof(ran_param_structure_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(ran_param_structure_item_s, ran_param_value_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_param_structure_item_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ran_param_structure_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
const asn1::table::seq_desc ran_param_structure_item_s::desc = {{seq_table_ran_param_structure_item_s_fields,
                                                                 uint64_t{0x3},
                                                                 nullptr,
                                                                 seq_table_ran_param_structure_item_s_names,
                                                                 2,
                                                                 0},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

// RANParameter-STRUCTURE ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_structure_s_fields[] = {
    {offsetof(ran_param_structure_s, seq_of_ran_params),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_param_structure_s::seq_of_ran_params),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_structure_item_s::desc}};
static const asn1::table::field_name seq_table_ran_param_structure_s_names[] = {{"sequence-of-ranParameters", nullptr}};
static const asn1::table::seq_opt seq_table_ran_param_structure_s_opts[] = {{0, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc       ran_param_structure_s::desc            = {{seq_table_ran_param_structure_s_fields,
                                                                             uint64_t{0x0},
                                                                             seq_table_ran_param_structure_s_opts,
                                                                             seq_table_ran_param_structure_s_names,
                                                                             1,
                                                                             1},
                                                                            true,
                                                                            true,
                                                                            nullptr,
                                                                            0};

// RANParameter-ValueType-Choice-Structure ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_value_type_choice_structure_s_fields[] = {
    {offsetof(ran_param_value_type_choice_structure_s, ran_param_structure),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_structure_s::desc}};
static const asn1::table::field_name seq_table_ran_param_value_type_choice_structure_s_names[] = {
    {"ranParameter-Structure", nullptr}};
const asn1::table::seq_desc ran_param_value_type_choice_structure_s::desc = {
    {seq_table_ran_param_value_type_choice_structure_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ran_param_value_type_choice_structure_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RANParameter-LIST ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_list_s_fields[] = {
    {offsetof(ran_param_list_s, list_of_ran_param),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_param_list_s::list_of_ran_param),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_structure_s::desc}};
static const asn1::table::field_name seq_table_ran_param_list_s_names[] = {{"list-of-ranParameter", nullptr}};
const asn1::table::seq_desc          ran_param_list_s::desc             = {
    {seq_table_ran_param_list_s_fields, uint64_t{0x1}, nullptr, seq_table_ran_param_list_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// RANParameter-ValueType-Choice-List ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_param_value_type_choice_list_s_fields[] = {
    {offsetof(ran_param_value_type_choice_list_s, ran_param_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_list_s::desc}};
static const asn1::table::field_name seq_table_ran_param_value_type_choice_list_s_names[] = {
    {"ranParameter-List", nullptr}};
const asn1::table::seq_desc ran_param_value_type_choice_list_s::desc = {
    {seq_table_ran_param_value_type_choice_list_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ran_param_value_type_choice_list_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RIC-PolicyAction-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_policy_action_ran_param_item_s_fields[] = {
    {offsetof(ric_policy_action_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(ric_policy_action_ran_param_item_s, ran_param_value_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ric_policy_action_ran_param_item_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_ric_policy_action_ran_param_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
const asn1::table::seq_desc ric_policy_action_ran_param_item_s::desc = {
    {seq_table_ric_policy_action_ran_param_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_ric_policy_action_ran_param_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// RIC-PolicyAction ::= SEQUENCE
static const asn1::table::seq_field seq_table_ric_policy_action_s_fields[] = {
    {offsetof(ric_policy_action_s, ric_policy_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(ric_policy_action_s, ran_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ric_policy_action_s::ran_params_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ric_policy_action_ran_param_item_s::desc}};
static const asn1::table::field_name seq_table_ric_policy_action_s_names[] = {{"ric-PolicyAction-ID", nullptr},
                                                                              {"ranParameters-List", nullptr}};
static const asn1::table::seq_opt   seq_table_ric_policy_action_s_opts[] = {{1, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_ric_policy_action_s_g0_fields[] = {
    {offsetof(ric_policy_action_s, ric_policy_decision),
     (int32_t)offsetof(ric_policy_action_s, ric_policy_decision_present),
     asn1::table::enum_op<decltype(ric_policy_action_s::ric_policy_decision)>(),
     decltype(ric_policy_action_s::ric_policy_decision)::has_ext,
     false,
     decltype(ric_policy_action_s::ric_policy_decision)::nof_types,
     decltype(ric_policy_action_s::ric_policy_decision)::nof_exts,
     nullptr,
     &ric_policy_action_s::ric_policy_decision_opts::desc}};
static const asn1::table::field_name seq_table_ric_policy_action_s_g0_names[] = {{"ric-PolicyDecision", nullptr}};
static const asn1::table::seq_opt    seq_table_ric_policy_action_s_g0_opts[]  = {
    {0, (int32_t)offsetof(ric_policy_action_s, ric_policy_decision_present)}};
static const asn1::table::seq_group seq_table_ric_policy_action_s_groups[] = {{{seq_table_ric_policy_action_s_g0_fields,
                                                                                uint64_t{0x0},
                                                                                seq_table_ric_policy_action_s_g0_opts,
                                                                                seq_table_ric_policy_action_s_g0_names,
                                                                                1,
                                                                                1},
                                                                               true}};
const asn1::table::seq_desc         ric_policy_action_s::desc              = {{seq_table_ric_policy_action_s_fields,
                                                                               uint64_t{0x1},
                                                                               seq_table_ric_policy_action_s_opts,
                                                                               seq_table_ric_policy_action_s_names,
                                                                               2,
                                                                               1},
                                                                              true,
                                                                              true,
                                                                              seq_table_ric_policy_action_s_groups,
                                                                              1};

static const char*           enum_names_ric_policy_action_s_ric_policy_decision_e__values[] = {"accept", "reject"};
const asn1::table::enum_desc ric_policy_action_s::ric_policy_decision_opts::desc            = {
    enum_names_ric_policy_action_s_ric_policy_decision_e__values,
    2,
    "ric_policy_action_s::ric_policy_decision_e_"};

// E2SM-RC-ActionDefinition-Format2-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format2_item_s_fields[] = {
    {offsetof(e2sm_rc_action_definition_format2_item_s, ric_policy_action),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ric_policy_action_s::desc},
    {offsetof(e2sm_rc_action_definition_format2_item_s, ric_policy_condition_definition),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::item_list_codec<decltype(e2sm_rc_action_definition_format2_item_s::ric_policy_condition_definition),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_testing_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format2_item_s_names[] = {
    {"ric-PolicyAction", nullptr},
    {"ric-PolicyConditionDefinition", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_action_definition_format2_item_s_opts[] = {
    {1, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format2_item_s_g0_fields[] = {
    {offsetof(e2sm_rc_action_definition_format2_item_s, ric_policy_condition_id),
     (int32_t)offsetof(e2sm_rc_action_definition_format2_item_s, ric_policy_condition_id_present),
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format2_item_s_g0_names[] = {
    {"ric-PolicyConditionID", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_action_definition_format2_item_s_g0_opts[] = {
    {0, (int32_t)offsetof(e2sm_rc_action_definition_format2_item_s, ric_policy_condition_id_present)}};
static const asn1::table::seq_group seq_table_e2sm_rc_action_definition_format2_item_s_groups[] = {
    {{seq_table_e2sm_rc_action_definition_format2_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_e2sm_rc_action_definition_format2_item_s_g0_opts,
      seq_table_e2sm_rc_action_definition_format2_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc e2sm_rc_action_definition_format2_item_s::desc = {
    {seq_table_e2sm_rc_action_definition_format2_item_s_fields,
     uint64_t{0x1},
     seq_table_e2sm_rc_action_definition_format2_item_s_opts,
     seq_table_e2sm_rc_action_definition_format2_item_s_names,
     2,
     1},
    true,
    true,
    seq_table_e2sm_rc_action_definition_format2_item_s_groups,
    1};

// E2SM-RC-ActionDefinition-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format2_s_fields[] = {
    {offsetof(e2sm_rc_action_definition_format2_s, ric_policy_conditions_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_action_definition_format2_s::ric_policy_conditions_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_action_definition_format2_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format2_s_names[] = {
    {"ric-PolicyConditions-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_action_definition_format2_s::desc = {
    {seq_table_e2sm_rc_action_definition_format2_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_rc_action_definition_format2_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ActionDefinition-Format3-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format3_item_s_fields[] = {
    {offsetof(e2sm_rc_action_definition_format3_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format3_item_s_names[] = {
    {"ranParameter-ID", nullptr}};
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format3_item_s_g0_fields[] = {
    {offsetof(e2sm_rc_action_definition_format3_item_s, ran_param_definition),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(e2sm_rc_action_definition_format3_item_s::ran_param_definition)>::value,
     &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format3_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_action_definition_format3_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_e2sm_rc_action_definition_format3_item_s_groups[] = {
    {{seq_table_e2sm_rc_action_definition_format3_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_e2sm_rc_action_definition_format3_item_s_g0_opts,
      seq_table_e2sm_rc_action_definition_format3_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc e2sm_rc_action_definition_format3_item_s::desc = {
    {seq_table_e2sm_rc_action_definition_format3_item_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_rc_action_definition_format3_item_s_names,
     1,
     0},
    true,
    true,
    seq_table_e2sm_rc_action_definition_format3_item_s_groups,
    1};

// E2SM-RC-ActionDefinition-Format3 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format3_s_fields[] = {
    {offsetof(e2sm_rc_action_definition_format3_s, ric_insert_ind_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_action_definition_format3_s, ran_p_insert_ind_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_action_definition_format3_s::ran_p_insert_ind_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_action_definition_format3_item_s::desc},
    {offsetof(e2sm_rc_action_definition_format3_s, ue_id),
     (int32_t)offsetof(e2sm_rc_action_definition_format3_s, ue_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format3_s_names[] = {
    {"ric-InsertIndication-ID", nullptr},
    {"ranP-InsertIndication-List", nullptr},
    {"ueID", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_action_definition_format3_s_opts[] = {
    {2, (int32_t)offsetof(e2sm_rc_action_definition_format3_s, ue_id_present)}};
const asn1::table::seq_desc e2sm_rc_action_definition_format3_s::desc = {
    {seq_table_e2sm_rc_action_definition_format3_s_fields,
     uint64_t{0x3},
     seq_table_e2sm_rc_action_definition_format3_s_opts,
     seq_table_e2sm_rc_action_definition_format3_s_names,
     3,
     1},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ActionDefinition-Format4-RANP-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format4_ranp_item_s_fields[] = {
    {offsetof(e2sm_rc_action_definition_format4_ranp_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format4_ranp_item_s_names[] = {
    {"ranParameter-ID", nullptr}};
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format4_ranp_item_s_g0_fields[] = {
    {offsetof(e2sm_rc_action_definition_format4_ranp_item_s, ran_param_definition),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<
         decltype(e2sm_rc_action_definition_format4_ranp_item_s::ran_param_definition)>::value,
     &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format4_ranp_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_action_definition_format4_ranp_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_e2sm_rc_action_definition_format4_ranp_item_s_groups[] = {
    {{seq_table_e2sm_rc_action_definition_format4_ranp_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_e2sm_rc_action_definition_format4_ranp_item_s_g0_opts,
      seq_table_e2sm_rc_action_definition_format4_ranp_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc e2sm_rc_action_definition_format4_ranp_item_s::desc = {
    {seq_table_e2sm_rc_action_definition_format4_ranp_item_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_rc_action_definition_format4_ranp_item_s_names,
     1,
     0},
    true,
    true,
    seq_table_e2sm_rc_action_definition_format4_ranp_item_s_groups,
    1};

// E2SM-RC-ActionDefinition-Format4-Indication-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format4_ind_item_s_fields[] = {
    {offsetof(e2sm_rc_action_definition_format4_ind_item_s, ric_insert_ind_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_action_definition_format4_ind_item_s, ran_p_insert_ind_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_action_definition_format4_ind_item_s::ran_p_insert_ind_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_action_definition_format4_ranp_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format4_ind_item_s_names[] = {
    {"ric-InsertIndication-ID", nullptr},
    {"ranP-InsertIndication-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_action_definition_format4_ind_item_s::desc = {
    {seq_table_e2sm_rc_action_definition_format4_ind_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_action_definition_format4_ind_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ActionDefinition-Format4-Style-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format4_style_item_s_fields[] = {
    {offsetof(e2sm_rc_action_definition_format4_style_item_s, requested_insert_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(e2sm_rc_action_definition_format4_style_item_s, ric_insert_ind_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(e2sm_rc_action_definition_format4_style_item_s::ric_insert_ind_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_action_definition_format4_ind_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format4_style_item_s_names[] = {
    {"requested-Insert-Style-Type", nullptr},
    {"ric-InsertIndication-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_action_definition_format4_style_item_s::desc = {
    {seq_table_e2sm_rc_action_definition_format4_style_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_action_definition_format4_style_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ActionDefinition-Format4 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_format4_s_fields[] = {
    {offsetof(e2sm_rc_action_definition_format4_s, ric_insert_style_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(e2sm_rc_action_definition_format4_s::ric_insert_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_action_definition_format4_style_item_s::desc},
    {offsetof(e2sm_rc_action_definition_format4_s, ue_id),
     (int32_t)offsetof(e2sm_rc_action_definition_format4_s, ue_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_format4_s_names[] = {
    {"ric-InsertStyle-List", nullptr},
    {"ueID", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_action_definition_format4_s_opts[] = {
    {1, (int32_t)offsetof(e2sm_rc_action_definition_format4_s, ue_id_present)}};
const asn1::table::seq_desc e2sm_rc_action_definition_format4_s::desc = {
    {seq_table_e2sm_rc_action_definition_format4_s_fields,
     uint64_t{0x1},
     seq_table_e2sm_rc_action_definition_format4_s_opts,
     seq_table_e2sm_rc_action_definition_format4_s_names,
     2,
     1},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ActionDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_action_definition_s_fields[] = {
    {offsetof(e2sm_rc_action_definition_s, ric_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(e2sm_rc_action_definition_s, ric_action_definition_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_action_definition_s::ric_action_definition_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_action_definition_s_names[] = {
    {"ric-Style-Type", nullptr},
    {"ric-actionDefinition-formats", nullptr}};
const asn1::table::seq_desc e2sm_rc_action_definition_s::desc = {{seq_table_e2sm_rc_action_definition_s_fields,
                                                                  uint64_t{0x3},
                                                                  nullptr,
                                                                  seq_table_e2sm_rc_action_definition_s_names,
                                                                  2,
                                                                  0},
                                                                 true,
                                                                 true,
                                                                 nullptr,
                                                                 0};

static const asn1::table::storage_ops* const
    choice_storage_e2sm_rc_action_definition_s_ric_action_definition_formats_c__alternatives[] = {
        &asn1::table::alternative_ops<e2sm_rc_action_definition_format1_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_rc_action_definition_format2_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_rc_action_definition_format3_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_rc_action_definition_format4_s, false, false>::value};
const asn1::table::choice_storage_desc e2sm_rc_action_definition_s::ric_action_definition_formats_c_::storage_desc = {
    choice_storage_e2sm_rc_action_definition_s_ric_action_definition_formats_c__alternatives,
    4,
    "e2sm_rc_action_definition_s::ric_action_definition_formats_c_"};

e2sm_rc_action_definition_format1_s&
e2sm_rc_action_definition_s::ric_action_definition_formats_c_::set_action_definition_format1()
{
  set(types::action_definition_format1);
  return c.get<e2sm_rc_action_definition_format1_s>();
}
e2sm_rc_action_definition_format2_s&
e2sm_rc_action_definition_s::ric_action_definition_formats_c_::set_action_definition_format2()
{
  set(types::action_definition_format2);
  return c.get<e2sm_rc_action_definition_format2_s>();
}
e2sm_rc_action_definition_format3_s&
e2sm_rc_action_definition_s::ric_action_definition_formats_c_::set_action_definition_format3()
{
  set(types::action_definition_format3);
  return c.get<e2sm_rc_action_definition_format3_s>();
}
e2sm_rc_action_definition_format4_s&
e2sm_rc_action_definition_s::ric_action_definition_formats_c_::set_action_definition_format4()
{
  set(types::action_definition_format4);
  return c.get<e2sm_rc_action_definition_format4_s>();
}
static const asn1::table::seq_field
    choice_table_e2sm_rc_action_definition_s_ric_action_definition_formats_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_rc_action_definition_format1_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_rc_action_definition_format2_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_rc_action_definition_format3_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_rc_action_definition_format4_s::desc}};
static const asn1::table::field_name choice_table_e2sm_rc_action_definition_s_ric_action_definition_formats_c__names[] =
    {{"actionDefinition-Format1", nullptr},
     {"actionDefinition-Format2", nullptr},
     {"actionDefinition-Format3", nullptr},
     {"actionDefinition-Format4", nullptr}};
const asn1::table::choice_desc e2sm_rc_action_definition_s::ric_action_definition_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_rc_action_definition_s::ric_action_definition_formats_c_::types>(),
    e2sm_rc_action_definition_s::ric_action_definition_formats_c_::types::nof_types,
    e2sm_rc_action_definition_s::ric_action_definition_formats_c_::types::nof_exts,
    e2sm_rc_action_definition_s::ric_action_definition_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_rc_action_definition_s_ric_action_definition_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_rc_action_definition_s_ric_action_definition_formats_c__names,
    "e2sm_rc_action_definition_s::ric_action_definition_formats_c_",
    offsetof(e2sm_rc_action_definition_s::ric_action_definition_formats_c_, c.type),
    offsetof(e2sm_rc_action_definition_s::ric_action_definition_formats_c_, c.value),
    false,
    &e2sm_rc_action_definition_s::ric_action_definition_formats_c_::storage_desc};

static const char* enum_names_e2sm_rc_action_definition_s_ric_action_definition_formats_c__types_values[] =
    {"actionDefinition-Format1", "actionDefinition-Format2", "actionDefinition-Format3", "actionDefinition-Format4"};
const asn1::table::enum_desc e2sm_rc_action_definition_s::ric_action_definition_formats_c_::types_opts::desc = {
    enum_names_e2sm_rc_action_definition_s_ric_action_definition_formats_c__types_values,
    4,
    "e2sm_rc_action_definition_s::ric_action_definition_formats_c_::types"};
uint8_t e2sm_rc_action_definition_s::ric_action_definition_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "e2sm_rc_action_definition_s::ric_action_definition_formats_c_::types");
}

// E2SM-RC-CallProcessID-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_call_process_id_format1_s_fields[] = {
    {offsetof(e2sm_rc_call_process_id_format1_s, ric_call_process_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_rc_call_process_id_format1_s_names[] = {
    {"ric-callProcess-ID", nullptr}};
const asn1::table::seq_desc e2sm_rc_call_process_id_format1_s::desc = {
    {seq_table_e2sm_rc_call_process_id_format1_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_e2sm_rc_call_process_id_format1_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-CallProcessID ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_call_process_id_s_fields[] = {
    {offsetof(e2sm_rc_call_process_id_s, ric_call_process_id_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_call_process_id_s_names[] = {
    {"ric-callProcessID-formats", nullptr}};
const asn1::table::seq_desc e2sm_rc_call_process_id_s::desc = {{seq_table_e2sm_rc_call_process_id_s_fields,
                                                                uint64_t{0x1},
                                                                nullptr,
                                                                seq_table_e2sm_rc_call_process_id_s_names,
                                                                1,
                                                                0},
                                                               true,
                                                               true,
                                                               nullptr,
                                                               0};

static const asn1::table::seq_field
    choice_table_e2sm_rc_call_process_id_s_ric_call_process_id_formats_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_rc_call_process_id_format1_s::desc}};
static const asn1::table::field_name choice_table_e2sm_rc_call_process_id_s_ric_call_process_id_formats_c__names[] = {
    {"callProcessID-Format1", nullptr}};
const asn1::table::choice_desc e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_::types>(),
    e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_::types::nof_types,
    e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_::types::nof_exts,
    e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_rc_call_process_id_s_ric_call_process_id_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_rc_call_process_id_s_ric_call_process_id_formats_c__names,
    "e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_",
    asn1::table::choice_desc::no_member,
    offsetof(e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_, c),
    false};

static const char* enum_names_e2sm_rc_call_process_id_s_ric_call_process_id_formats_c__types_values[] = {
    "callProcessID-Format1"};
const asn1::table::enum_desc e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_::types_opts::desc = {
    enum_names_e2sm_rc_call_process_id_s_ric_call_process_id_formats_c__types_values,
    1,
    "e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_::types"};
uint8_t e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "e2sm_rc_call_process_id_s::ric_call_process_id_formats_c_::types");
}

// E2SM-RC-ControlHeader-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_hdr_format1_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_hdr_format1_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc},
    {offsetof(e2sm_rc_ctrl_hdr_format1_s, ric_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(e2sm_rc_ctrl_hdr_format1_s, ric_ctrl_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_ctrl_hdr_format1_s, ric_ctrl_decision),
     (int32_t)offsetof(e2sm_rc_ctrl_hdr_format1_s, ric_ctrl_decision_present),
     asn1::table::enum_op<decltype(e2sm_rc_ctrl_hdr_format1_s::ric_ctrl_decision)>(),
     decltype(e2sm_rc_ctrl_hdr_format1_s::ric_ctrl_decision)::has_ext,
     false,
     decltype(e2sm_rc_ctrl_hdr_format1_s::ric_ctrl_decision)::nof_types,
     decltype(e2sm_rc_ctrl_hdr_format1_s::ric_ctrl_decision)::nof_exts,
     nullptr,
     &e2sm_rc_ctrl_hdr_format1_s::ric_ctrl_decision_opts::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_hdr_format1_s_names[] = {{"ueID", nullptr},
                                                                                     {"ric-Style-Type", nullptr},
                                                                                     {"ric-ControlAction-ID", nullptr},
                                                                                     {"ric-ControlDecision", nullptr}};
static const asn1::table::seq_opt    seq_table_e2sm_rc_ctrl_hdr_format1_s_opts[]  = {
    {3, (int32_t)offsetof(e2sm_rc_ctrl_hdr_format1_s, ric_ctrl_decision_present)}};
const asn1::table::seq_desc e2sm_rc_ctrl_hdr_format1_s::desc = {{seq_table_e2sm_rc_ctrl_hdr_format1_s_fields,
                                                                 uint64_t{0x7},
                                                                 seq_table_e2sm_rc_ctrl_hdr_format1_s_opts,
                                                                 seq_table_e2sm_rc_ctrl_hdr_format1_s_names,
                                                                 4,
                                                                 1},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

static const char*           enum_names_e2sm_rc_ctrl_hdr_format1_s_ric_ctrl_decision_e__values[] = {"accept", "reject"};
const asn1::table::enum_desc e2sm_rc_ctrl_hdr_format1_s::ric_ctrl_decision_opts::desc            = {
    enum_names_e2sm_rc_ctrl_hdr_format1_s_ric_ctrl_decision_e__values,
    2,
    "e2sm_rc_ctrl_hdr_format1_s::ric_ctrl_decision_e_"};

// E2SM-RC-ControlHeader-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_hdr_format2_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_hdr_format2_s, ue_id),
     (int32_t)offsetof(e2sm_rc_ctrl_hdr_format2_s, ue_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc},
    {offsetof(e2sm_rc_ctrl_hdr_format2_s, ric_ctrl_decision),
     (int32_t)offsetof(e2sm_rc_ctrl_hdr_format2_s, ric_ctrl_decision_present),
     asn1::table::enum_op<decltype(e2sm_rc_ctrl_hdr_format2_s::ric_ctrl_decision)>(),
     decltype(e2sm_rc_ctrl_hdr_format2_s::ric_ctrl_decision)::has_ext,
     false,
     decltype(e2sm_rc_ctrl_hdr_format2_s::ric_ctrl_decision)::nof_types,
     decltype(e2sm_rc_ctrl_hdr_format2_s::ric_ctrl_decision)::nof_exts,
     nullptr,
     &e2sm_rc_ctrl_hdr_format2_s::ric_ctrl_decision_opts::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_hdr_format2_s_names[] = {{"ueID", nullptr},
                                                                                     {"ric-ControlDecision", nullptr}};
static const asn1::table::seq_opt    seq_table_e2sm_rc_ctrl_hdr_format2_s_opts[]  = {
    {0, (int32_t)offsetof(e2sm_rc_ctrl_hdr_format2_s, ue_id_present)},
    {1, (int32_t)offsetof(e2sm_rc_ctrl_hdr_format2_s, ric_ctrl_decision_present)}};
const asn1::table::seq_desc e2sm_rc_ctrl_hdr_format2_s::desc = {{seq_table_e2sm_rc_ctrl_hdr_format2_s_fields,
                                                                 uint64_t{0x0},
                                                                 seq_table_e2sm_rc_ctrl_hdr_format2_s_opts,
                                                                 seq_table_e2sm_rc_ctrl_hdr_format2_s_names,
                                                                 2,
                                                                 2},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

static const char*           enum_names_e2sm_rc_ctrl_hdr_format2_s_ric_ctrl_decision_e__values[] = {"accept", "reject"};
const asn1::table::enum_desc e2sm_rc_ctrl_hdr_format2_s::ric_ctrl_decision_opts::desc            = {
    enum_names_e2sm_rc_ctrl_hdr_format2_s_ric_ctrl_decision_e__values,
    2,
    "e2sm_rc_ctrl_hdr_format2_s::ric_ctrl_decision_e_"};

// UEGroupDefinitionIdentifier-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_group_definition_id_item_s_fields[] = {
    {offsetof(ue_group_definition_id_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(ue_group_definition_id_item_s, ran_param_value_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ue_group_definition_id_item_s::ran_param_value_type),
                                asn1::table::struct_json>::value},
    {offsetof(ue_group_definition_id_item_s, lc_or),
     (int32_t)offsetof(ue_group_definition_id_item_s, lc_or_present),
     asn1::table::enum_op<decltype(ue_group_definition_id_item_s::lc_or)>(),
     decltype(ue_group_definition_id_item_s::lc_or)::has_ext,
     false,
     decltype(ue_group_definition_id_item_s::lc_or)::nof_types,
     decltype(ue_group_definition_id_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_e2sm_rc_ies_opts::desc}};
static const asn1::table::field_name seq_table_ue_group_definition_id_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr},
    {"logicalOR", nullptr}};
static const asn1::table::seq_opt seq_table_ue_group_definition_id_item_s_opts[] = {
    {2, (int32_t)offsetof(ue_group_definition_id_item_s, lc_or_present)}};
const asn1::table::seq_desc ue_group_definition_id_item_s::desc = {{seq_table_ue_group_definition_id_item_s_fields,
                                                                    uint64_t{0x3},
                                                                    seq_table_ue_group_definition_id_item_s_opts,
                                                                    seq_table_ue_group_definition_id_item_s_names,
                                                                    3,
                                                                    1},
                                                                   true,
                                                                   true,
                                                                   nullptr,
                                                                   0};

// UE-Group-Definition ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_group_definition_s_fields[] = {
    {offsetof(ue_group_definition_s, ue_group_definition_id_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::item_list_codec<decltype(ue_group_definition_s::ue_group_definition_id_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ue_group_definition_id_item_s::desc}};
static const asn1::table::field_name seq_table_ue_group_definition_s_names[] = {
    {"ueGroupDefinitionIdentifier-LIST", nullptr}};
const asn1::table::seq_desc ue_group_definition_s::desc = {
    {seq_table_ue_group_definition_s_fields, uint64_t{0x1}, nullptr, seq_table_ue_group_definition_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ControlHeader-Format3 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_hdr_format3_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_hdr_format3_s, ue_group_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_ctrl_hdr_format3_s, ue_group_definition),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_group_definition_s::desc},
    {offsetof(e2sm_rc_ctrl_hdr_format3_s, ric_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(e2sm_rc_ctrl_hdr_format3_s, ric_ctrl_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_hdr_format3_s_names[] = {{"ue-Group-ID", nullptr},
                                                                                     {"ue-Group-Definition", nullptr},
                                                                                     {"ric-Style-Type", nullptr},
                                                                                     {"ric-ControlAction-ID", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ctrl_hdr_format3_s::desc = {{seq_table_e2sm_rc_ctrl_hdr_format3_s_fields,
                                                                          uint64_t{0xf},
                                                                          nullptr,
                                                                          seq_table_e2sm_rc_ctrl_hdr_format3_s_names,
                                                                          4,
                                                                          0},
                                                                         true,
                                                                         true,
                                                                         nullptr,
                                                                         0};

// E2SM-RC-ControlHeader-Format4 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_hdr_format4_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_hdr_format4_s, partial_ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &partial_ue_id_s::desc},
    {offsetof(e2sm_rc_ctrl_hdr_format4_s, ric_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(e2sm_rc_ctrl_hdr_format4_s, ric_ctrl_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_ctrl_hdr_format4_s, ric_ctrl_decision),
     (int32_t)offsetof(e2sm_rc_ctrl_hdr_format4_s, ric_ctrl_decision_present),
     asn1::table::enum_op<decltype(e2sm_rc_ctrl_hdr_format4_s::ric_ctrl_decision)>(),
     decltype(e2sm_rc_ctrl_hdr_format4_s::ric_ctrl_decision)::has_ext,
     false,
     decltype(e2sm_rc_ctrl_hdr_format4_s::ric_ctrl_decision)::nof_types,
     decltype(e2sm_rc_ctrl_hdr_format4_s::ric_ctrl_decision)::nof_exts,
     nullptr,
     &e2sm_rc_ctrl_hdr_format4_s::ric_ctrl_decision_opts::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_hdr_format4_s_names[] = {{"partial-ueID", nullptr},
                                                                                     {"ric-Style-Type", nullptr},
                                                                                     {"ric-ControlAction-ID", nullptr},
                                                                                     {"ric-ControlDecision", nullptr}};
static const asn1::table::seq_opt    seq_table_e2sm_rc_ctrl_hdr_format4_s_opts[]  = {
    {3, (int32_t)offsetof(e2sm_rc_ctrl_hdr_format4_s, ric_ctrl_decision_present)}};
const asn1::table::seq_desc e2sm_rc_ctrl_hdr_format4_s::desc = {{seq_table_e2sm_rc_ctrl_hdr_format4_s_fields,
                                                                 uint64_t{0x7},
                                                                 seq_table_e2sm_rc_ctrl_hdr_format4_s_opts,
                                                                 seq_table_e2sm_rc_ctrl_hdr_format4_s_names,
                                                                 4,
                                                                 1},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

static const char*           enum_names_e2sm_rc_ctrl_hdr_format4_s_ric_ctrl_decision_e__values[] = {"accept", "reject"};
const asn1::table::enum_desc e2sm_rc_ctrl_hdr_format4_s::ric_ctrl_decision_opts::desc            = {
    enum_names_e2sm_rc_ctrl_hdr_format4_s_ric_ctrl_decision_e__values,
    2,
    "e2sm_rc_ctrl_hdr_format4_s::ric_ctrl_decision_e_"};

// E2SM-RC-ControlHeader ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_hdr_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_hdr_s, ric_ctrl_hdr_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_hdr_s_names[] = {{"ric-controlHeader-formats", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ctrl_hdr_s::desc             = {
    {seq_table_e2sm_rc_ctrl_hdr_s_fields, uint64_t{0x1}, nullptr, seq_table_e2sm_rc_ctrl_hdr_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::storage_ops* const choice_storage_e2sm_rc_ctrl_hdr_s_ric_ctrl_hdr_formats_c__alternatives[] =
    {&asn1::table::alternative_ops<e2sm_rc_ctrl_hdr_format1_s, false>::value,
     &asn1::table::alternative_ops<e2sm_rc_ctrl_hdr_format2_s, false>::value,
     &asn1::table::alternative_ops<e2sm_rc_ctrl_hdr_format3_s, false, false>::value,
     &asn1::table::alternative_ops<e2sm_rc_ctrl_hdr_format4_s, false>::value};
const asn1::table::choice_storage_desc e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::storage_desc = {
    choice_storage_e2sm_rc_ctrl_hdr_s_ric_ctrl_hdr_formats_c__alternatives,
    4,
    "e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_"};

e2sm_rc_ctrl_hdr_format1_s& e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::set_ctrl_hdr_format1()
{
  set(types::ctrl_hdr_format1);
  return c.get<e2sm_rc_ctrl_hdr_format1_s>();
}
e2sm_rc_ctrl_hdr_format2_s& e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::set_ctrl_hdr_format2()
{
  set(types::ctrl_hdr_format2);
  return c.get<e2sm_rc_ctrl_hdr_format2_s>();
}
e2sm_rc_ctrl_hdr_format3_s& e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::set_ctrl_hdr_format3()
{
  set(types::ctrl_hdr_format3);
  return c.get<e2sm_rc_ctrl_hdr_format3_s>();
}
e2sm_rc_ctrl_hdr_format4_s& e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::set_ctrl_hdr_format4()
{
  set(types::ctrl_hdr_format4);
  return c.get<e2sm_rc_ctrl_hdr_format4_s>();
}
static const asn1::table::seq_field choice_table_e2sm_rc_ctrl_hdr_s_ric_ctrl_hdr_formats_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_hdr_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_hdr_format2_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_hdr_format3_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_hdr_format4_s::desc}};
static const asn1::table::field_name choice_table_e2sm_rc_ctrl_hdr_s_ric_ctrl_hdr_formats_c__names[] = {
    {"controlHeader-Format1", nullptr},
    {"controlHeader-Format2", nullptr},
    {"controlHeader-Format3", nullptr},
    {"controlHeader-Format4", nullptr}};
const asn1::table::choice_desc e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types>(),
    e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types::nof_types,
    e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types::nof_exts,
    e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_rc_ctrl_hdr_s_ric_ctrl_hdr_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_rc_ctrl_hdr_s_ric_ctrl_hdr_formats_c__names,
    "e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_",
    offsetof(e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_, c.type),
    offsetof(e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_, c.value),
    false,
    &e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::storage_desc};

static const char* enum_names_e2sm_rc_ctrl_hdr_s_ric_ctrl_hdr_formats_c__types_values[]    = {"controlHeader-Format1",
                                                                                              "controlHeader-Format2",
                                                                                              "controlHeader-Format3",
                                                                                              "controlHeader-Format4"};
const asn1::table::enum_desc e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types_opts::desc = {
    enum_names_e2sm_rc_ctrl_hdr_s_ric_ctrl_hdr_formats_c__types_values,
    4,
    "e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types"};
uint8_t e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4};
  return map_enum_number(numbers, 4, value, "e2sm_rc_ctrl_hdr_s::ric_ctrl_hdr_formats_c_::types");
}

// E2SM-RC-ControlMessage-Format1-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_msg_format1_item_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_msg_format1_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_ctrl_msg_format1_item_s, ran_param_value_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_rc_ctrl_msg_format1_item_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_msg_format1_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_msg_format1_item_s::desc = {{seq_table_e2sm_rc_ctrl_msg_format1_item_s_fields,
                                                                      uint64_t{0x3},
                                                                      nullptr,
                                                                      seq_table_e2sm_rc_ctrl_msg_format1_item_s_names,
                                                                      2,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2SM-RC-ControlMessage-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_msg_format1_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_msg_format1_s, ran_p_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     0,
     65535,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ctrl_msg_format1_s::ran_p_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ctrl_msg_format1_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_msg_format1_s_names[] = {{"ranP-List", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ctrl_msg_format1_s::desc = {{seq_table_e2sm_rc_ctrl_msg_format1_s_fields,
                                                                          uint64_t{0x1},
                                                                          nullptr,
                                                                          seq_table_e2sm_rc_ctrl_msg_format1_s_names,
                                                                          1,
                                                                          0},
                                                                         true,
                                                                         true,
                                                                         nullptr,
                                                                         0};

// E2SM-RC-ControlMessage-Format2-ControlAction-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_msg_format2_ctrl_action_item_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_msg_format2_ctrl_action_item_s, ric_ctrl_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_ctrl_msg_format2_ctrl_action_item_s, ran_p_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_msg_format1_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_msg_format2_ctrl_action_item_s_names[] = {
    {"ric-ControlAction-ID", nullptr},
    {"ranP-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_msg_format2_ctrl_action_item_s::desc = {
    {seq_table_e2sm_rc_ctrl_msg_format2_ctrl_action_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_ctrl_msg_format2_ctrl_action_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ControlMessage-Format2-Style-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_msg_format2_style_item_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_msg_format2_style_item_s, indicated_ctrl_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(e2sm_rc_ctrl_msg_format2_style_item_s, ric_ctrl_action_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ctrl_msg_format2_style_item_s::ric_ctrl_action_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ctrl_msg_format2_ctrl_action_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_msg_format2_style_item_s_names[] = {
    {"indicated-Control-Style-Type", nullptr},
    {"ric-ControlAction-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_msg_format2_style_item_s::desc = {
    {seq_table_e2sm_rc_ctrl_msg_format2_style_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_ctrl_msg_format2_style_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ControlMessage-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_msg_format2_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_msg_format2_s, ric_ctrl_style_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ctrl_msg_format2_s::ric_ctrl_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ctrl_msg_format2_style_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_msg_format2_s_names[] = {
    {"ric-ControlStyle-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_msg_format2_s::desc = {{seq_table_e2sm_rc_ctrl_msg_format2_s_fields,
                                                                 uint64_t{0x1},
                                                                 nullptr,
                                                                 seq_table_e2sm_rc_ctrl_msg_format2_s_names,
                                                                 1,
                                                                 0},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

// EntitySpecific-ranP-ControlParameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_entity_specific_ran_p_ctrl_params_s_fields[] = {
    {offsetof(entity_specific_ran_p_ctrl_params_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(entity_specific_ran_p_ctrl_params_s, ran_param_value_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(entity_specific_ran_p_ctrl_params_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_entity_specific_ran_p_ctrl_params_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
const asn1::table::seq_desc entity_specific_ran_p_ctrl_params_s::desc = {
    {seq_table_entity_specific_ran_p_ctrl_params_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_entity_specific_ran_p_ctrl_params_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-EntityFilter ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_entity_filt_s_fields[] = {
    {offsetof(e2sm_rc_entity_filt_s, entity_filt_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{254},
     nullptr},
    {offsetof(e2sm_rc_entity_filt_s, entity_filt_definition),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::item_list_codec<decltype(e2sm_rc_entity_filt_s::entity_filt_definition),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_testing_item_s::desc},
    {offsetof(e2sm_rc_entity_filt_s, entity_specific_ctrl_ran_p_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_entity_filt_s::entity_specific_ctrl_ran_p_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &entity_specific_ran_p_ctrl_params_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_entity_filt_s_names[] = {
    {"entityFilter-ID", nullptr},
    {"entityFilter-Definition", nullptr},
    {"entitySpecificControlRanP-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_entity_filt_s::desc = {
    {seq_table_e2sm_rc_entity_filt_s_fields, uint64_t{0x7}, nullptr, seq_table_e2sm_rc_entity_filt_s_names, 3, 0},
    true,
    true,
    nullptr,
    0};

// EntityAgnostic-ranP-ControlParameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_entity_agnostic_ran_p_ctrl_params_s_fields[] = {
    {offsetof(entity_agnostic_ran_p_ctrl_params_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(entity_agnostic_ran_p_ctrl_params_s, ran_param_value_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(entity_agnostic_ran_p_ctrl_params_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_entity_agnostic_ran_p_ctrl_params_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
const asn1::table::seq_desc entity_agnostic_ran_p_ctrl_params_s::desc = {
    {seq_table_entity_agnostic_ran_p_ctrl_params_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_entity_agnostic_ran_p_ctrl_params_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ControlMessage-Format3 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_msg_format3_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_msg_format3_s, list_of_entity_filts),
     (int32_t)offsetof(e2sm_rc_ctrl_msg_format3_s, list_of_entity_filts_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     255,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ctrl_msg_format3_s::list_of_entity_filts),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_entity_filt_s::desc},
    {offsetof(e2sm_rc_ctrl_msg_format3_s, entity_agnostic_ctrl_ran_p_list),
     (int32_t)offsetof(e2sm_rc_ctrl_msg_format3_s, entity_agnostic_ctrl_ran_p_list_present),
     asn1::table::field_op::codec,
     false,
     true,
     0,
     65535,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ctrl_msg_format3_s::entity_agnostic_ctrl_ran_p_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &entity_agnostic_ran_p_ctrl_params_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_msg_format3_s_names[] = {
    {"listOfEntityFilters", nullptr},
    {"entityAgnosticControlRanP-List", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_ctrl_msg_format3_s_opts[] = {
    {0, (int32_t)offsetof(e2sm_rc_ctrl_msg_format3_s, list_of_entity_filts_present)},
    {1, (int32_t)offsetof(e2sm_rc_ctrl_msg_format3_s, entity_agnostic_ctrl_ran_p_list_present)}};
const asn1::table::seq_desc e2sm_rc_ctrl_msg_format3_s::desc = {{seq_table_e2sm_rc_ctrl_msg_format3_s_fields,
                                                                 uint64_t{0x0},
                                                                 seq_table_e2sm_rc_ctrl_msg_format3_s_opts,
                                                                 seq_table_e2sm_rc_ctrl_msg_format3_s_names,
                                                                 2,
                                                                 2},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

// E2SM-RC-ControlMessage-Format4-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_msg_format4_item_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_msg_format4_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_ctrl_msg_format4_item_s, ran_param_definition),
     (int32_t)offsetof(e2sm_rc_ctrl_msg_format4_item_s, ran_param_definition_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_msg_format4_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_ctrl_msg_format4_item_s_opts[] = {
    {1, (int32_t)offsetof(e2sm_rc_ctrl_msg_format4_item_s, ran_param_definition_present)}};
const asn1::table::seq_desc e2sm_rc_ctrl_msg_format4_item_s::desc = {{seq_table_e2sm_rc_ctrl_msg_format4_item_s_fields,
                                                                      uint64_t{0x1},
                                                                      seq_table_e2sm_rc_ctrl_msg_format4_item_s_opts,
                                                                      seq_table_e2sm_rc_ctrl_msg_format4_item_s_names,
                                                                      2,
                                                                      1},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2SM-RC-ControlMessage-Format4 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_msg_format4_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_msg_format4_s, ran_p_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     0,
     65535,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ctrl_msg_format4_s::ran_p_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ctrl_msg_format4_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_msg_format4_s_names[] = {{"ranP-List", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ctrl_msg_format4_s::desc = {{seq_table_e2sm_rc_ctrl_msg_format4_s_fields,
                                                                          uint64_t{0x1},
                                                                          nullptr,
                                                                          seq_table_e2sm_rc_ctrl_msg_format4_s_names,
                                                                          1,
                                                                          0},
                                                                         true,
                                                                         true,
                                                                         nullptr,
                                                                         0};

// E2SM-RC-ControlMessage ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_msg_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_msg_s, ric_ctrl_msg_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_msg_s_names[] = {{"ric-controlMessage-formats", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ctrl_msg_s::desc             = {
    {seq_table_e2sm_rc_ctrl_msg_s_fields, uint64_t{0x1}, nullptr, seq_table_e2sm_rc_ctrl_msg_s_names, 1, 0},
    true,
    true,
    nullptr,
    0,
    "E2SM-RC-ControlMessage"};

static const asn1::table::storage_ops* const choice_storage_e2sm_rc_ctrl_msg_s_ric_ctrl_msg_formats_c__alternatives[] =
    {&asn1::table::alternative_ops<e2sm_rc_ctrl_msg_format1_s, false, false>::value,
     &asn1::table::alternative_ops<e2sm_rc_ctrl_msg_format2_s, false, false>::value,
     &asn1::table::alternative_ops<e2sm_rc_ctrl_msg_format3_s, false, false>::value,
     &asn1::table::alternative_ops<e2sm_rc_ctrl_msg_format4_s, false, false>::value,
     nullptr};
const asn1::table::choice_storage_desc e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::storage_desc = {
    choice_storage_e2sm_rc_ctrl_msg_s_ric_ctrl_msg_formats_c__alternatives,
    5,
    "e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_"};

e2sm_rc_ctrl_msg_format1_s& e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::set_ctrl_msg_format1()
{
  set(types::ctrl_msg_format1);
  return c.get<e2sm_rc_ctrl_msg_format1_s>();
}
e2sm_rc_ctrl_msg_format2_s& e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::set_ctrl_msg_format2()
{
  set(types::ctrl_msg_format2);
  return c.get<e2sm_rc_ctrl_msg_format2_s>();
}
e2sm_rc_ctrl_msg_format3_s& e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::set_ctrl_msg_format3()
{
  set(types::ctrl_msg_format3);
  return c.get<e2sm_rc_ctrl_msg_format3_s>();
}
e2sm_rc_ctrl_msg_format4_s& e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::set_ctrl_msg_format4()
{
  set(types::ctrl_msg_format4);
  return c.get<e2sm_rc_ctrl_msg_format4_s>();
}
void e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::set_ctrl_msg_format5()
{
  set(types::ctrl_msg_format5);
}
static const asn1::table::seq_field choice_table_e2sm_rc_ctrl_msg_s_ric_ctrl_msg_formats_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_msg_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_msg_format2_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_msg_format3_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_msg_format4_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_e2sm_rc_ctrl_msg_s_ric_ctrl_msg_formats_c__names[] = {
    {"controlMessage-Format1", nullptr},
    {"controlMessage-Format2", nullptr},
    {"controlMessage-Format3", nullptr},
    {"controlMessage-Format4", nullptr},
    {"controlMessage-Format5", nullptr}};
const asn1::table::choice_desc e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types>(),
    e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types::nof_types,
    e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types::nof_exts,
    e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_rc_ctrl_msg_s_ric_ctrl_msg_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_rc_ctrl_msg_s_ric_ctrl_msg_formats_c__names,
    "e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_",
    offsetof(e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_, c.type),
    offsetof(e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_, c.value),
    false,
    &e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::storage_desc};

static const char* enum_names_e2sm_rc_ctrl_msg_s_ric_ctrl_msg_formats_c__types_values[]    = {"controlMessage-Format1",
                                                                                              "controlMessage-Format2",
                                                                                              "controlMessage-Format3",
                                                                                              "controlMessage-Format4",
                                                                                              "controlMessage-Format5"};
const asn1::table::enum_desc e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types_opts::desc = {
    enum_names_e2sm_rc_ctrl_msg_s_ric_ctrl_msg_formats_c__types_values,
    5,
    "e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types"};
uint8_t e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5};
  return map_enum_number(numbers, 5, value, "e2sm_rc_ctrl_msg_s::ric_ctrl_msg_formats_c_::types");
}

// E2SM-RC-ControlOutcome-Format1-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_outcome_format1_item_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_outcome_format1_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_ctrl_outcome_format1_item_s, ran_param_value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_value_c::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_outcome_format1_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-value", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_outcome_format1_item_s::desc = {
    {seq_table_e2sm_rc_ctrl_outcome_format1_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_ctrl_outcome_format1_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ControlOutcome-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_outcome_format1_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_outcome_format1_s, ran_p_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     0,
     255,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ctrl_outcome_format1_s::ran_p_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ctrl_outcome_format1_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_outcome_format1_s_names[] = {{"ranP-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_outcome_format1_s::desc = {{seq_table_e2sm_rc_ctrl_outcome_format1_s_fields,
                                                                     uint64_t{0x1},
                                                                     nullptr,
                                                                     seq_table_e2sm_rc_ctrl_outcome_format1_s_names,
                                                                     1,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// E2SM-RC-ControlOutcome-Format2-RANP-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_outcome_format2_ranp_item_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_outcome_format2_ranp_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_ctrl_outcome_format2_ranp_item_s, ran_param_value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_value_c::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_outcome_format2_ranp_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-value", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_outcome_format2_ranp_item_s::desc = {
    {seq_table_e2sm_rc_ctrl_outcome_format2_ranp_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_ctrl_outcome_format2_ranp_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ControlOutcome-Format2-ControlOutcome-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s, ric_ctrl_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s, ran_p_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s::ran_p_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ctrl_outcome_format2_ranp_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s_names[] = {
    {"ric-ControlAction-ID", nullptr},
    {"ranP-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s::desc = {
    {seq_table_e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ControlOutcome-Format2-Style-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_outcome_format2_style_item_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_outcome_format2_style_item_s, indicated_ctrl_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(e2sm_rc_ctrl_outcome_format2_style_item_s, ric_ctrl_outcome_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ctrl_outcome_format2_style_item_s::ric_ctrl_outcome_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ctrl_outcome_format2_ctrl_outcome_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_outcome_format2_style_item_s_names[] = {
    {"indicated-Control-Style-Type", nullptr},
    {"ric-ControlOutcome-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_outcome_format2_style_item_s::desc = {
    {seq_table_e2sm_rc_ctrl_outcome_format2_style_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_ctrl_outcome_format2_style_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ControlOutcome-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_outcome_format2_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_outcome_format2_s, ric_ctrl_style_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ctrl_outcome_format2_s::ric_ctrl_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ctrl_outcome_format2_style_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_outcome_format2_s_names[] = {
    {"ric-ControlStyle-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_outcome_format2_s::desc = {{seq_table_e2sm_rc_ctrl_outcome_format2_s_fields,
                                                                     uint64_t{0x1},
                                                                     nullptr,
                                                                     seq_table_e2sm_rc_ctrl_outcome_format2_s_names,
                                                                     1,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// E2SM-RC-ControlOutcome-Format3-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_outcome_format3_item_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_outcome_format3_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_ctrl_outcome_format3_item_s, ran_param_value_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_rc_ctrl_outcome_format3_item_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_outcome_format3_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_outcome_format3_item_s::desc = {
    {seq_table_e2sm_rc_ctrl_outcome_format3_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_ctrl_outcome_format3_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-ControlOutcome-Format3 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_outcome_format3_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_outcome_format3_s, ran_p_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     0,
     255,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ctrl_outcome_format3_s::ran_p_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ctrl_outcome_format3_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_outcome_format3_s_names[] = {{"ranP-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_outcome_format3_s::desc = {{seq_table_e2sm_rc_ctrl_outcome_format3_s_fields,
                                                                     uint64_t{0x1},
                                                                     nullptr,
                                                                     seq_table_e2sm_rc_ctrl_outcome_format3_s_names,
                                                                     1,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// E2SM-RC-ControlOutcome ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ctrl_outcome_s_fields[] = {
    {offsetof(e2sm_rc_ctrl_outcome_s, ric_ctrl_outcome_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ctrl_outcome_s_names[] = {
    {"ric-controlOutcome-formats", nullptr}};
const asn1::table::seq_desc e2sm_rc_ctrl_outcome_s::desc = {
    {seq_table_e2sm_rc_ctrl_outcome_s_fields, uint64_t{0x1}, nullptr, seq_table_e2sm_rc_ctrl_outcome_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::storage_ops* const
    choice_storage_e2sm_rc_ctrl_outcome_s_ric_ctrl_outcome_formats_c__alternatives[] = {
        &asn1::table::alternative_ops<e2sm_rc_ctrl_outcome_format1_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_rc_ctrl_outcome_format2_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_rc_ctrl_outcome_format3_s, false, false>::value};
const asn1::table::choice_storage_desc e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::storage_desc = {
    choice_storage_e2sm_rc_ctrl_outcome_s_ric_ctrl_outcome_formats_c__alternatives,
    3,
    "e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_"};

e2sm_rc_ctrl_outcome_format1_s& e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::set_ctrl_outcome_format1()
{
  set(types::ctrl_outcome_format1);
  return c.get<e2sm_rc_ctrl_outcome_format1_s>();
}
e2sm_rc_ctrl_outcome_format2_s& e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::set_ctrl_outcome_format2()
{
  set(types::ctrl_outcome_format2);
  return c.get<e2sm_rc_ctrl_outcome_format2_s>();
}
e2sm_rc_ctrl_outcome_format3_s& e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::set_ctrl_outcome_format3()
{
  set(types::ctrl_outcome_format3);
  return c.get<e2sm_rc_ctrl_outcome_format3_s>();
}
static const asn1::table::seq_field choice_table_e2sm_rc_ctrl_outcome_s_ric_ctrl_outcome_formats_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_outcome_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_outcome_format2_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ctrl_outcome_format3_s::desc}};
static const asn1::table::field_name choice_table_e2sm_rc_ctrl_outcome_s_ric_ctrl_outcome_formats_c__names[] = {
    {"controlOutcome-Format1", nullptr},
    {"controlOutcome-Format2", nullptr},
    {"controlOutcome-Format3", nullptr}};
const asn1::table::choice_desc e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::types>(),
    e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::types::nof_types,
    e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::types::nof_exts,
    e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_rc_ctrl_outcome_s_ric_ctrl_outcome_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_rc_ctrl_outcome_s_ric_ctrl_outcome_formats_c__names,
    "e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_",
    offsetof(e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_, c.type),
    offsetof(e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_, c.value),
    false,
    &e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::storage_desc};

static const char* enum_names_e2sm_rc_ctrl_outcome_s_ric_ctrl_outcome_formats_c__types_values[] = {
    "controlOutcome-Format1",
    "controlOutcome-Format2",
    "controlOutcome-Format3"};
const asn1::table::enum_desc e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::types_opts::desc = {
    enum_names_e2sm_rc_ctrl_outcome_s_ric_ctrl_outcome_formats_c__types_values,
    3,
    "e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::types"};
uint8_t e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(numbers, 3, value, "e2sm_rc_ctrl_outcome_s::ric_ctrl_outcome_formats_c_::types");
}

// MessageType-Choice-NI ::= SEQUENCE
static const asn1::table::seq_field seq_table_msg_type_choice_ni_s_fields[] = {
    {offsetof(msg_type_choice_ni_s, ni_type),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(msg_type_choice_ni_s::ni_type)>(),
     decltype(msg_type_choice_ni_s::ni_type)::has_ext,
     false,
     decltype(msg_type_choice_ni_s::ni_type)::nof_types,
     decltype(msg_type_choice_ni_s::ni_type)::nof_exts,
     nullptr,
     &interface_type_opts::desc},
    {offsetof(msg_type_choice_ni_s, ni_id),
     (int32_t)offsetof(msg_type_choice_ni_s, ni_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &interface_id_c::desc},
    {offsetof(msg_type_choice_ni_s, ni_msg),
     (int32_t)offsetof(msg_type_choice_ni_s, ni_msg_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &interface_msg_id_s::desc}};
static const asn1::table::field_name seq_table_msg_type_choice_ni_s_names[] = {{"nI-Type", nullptr},
                                                                               {"nI-Identifier", nullptr},
                                                                               {"nI-Message", nullptr}};
static const asn1::table::seq_opt    seq_table_msg_type_choice_ni_s_opts[]  = {
    {1, (int32_t)offsetof(msg_type_choice_ni_s, ni_id_present)},
    {2, (int32_t)offsetof(msg_type_choice_ni_s, ni_msg_present)}};
const asn1::table::seq_desc msg_type_choice_ni_s::desc = {{seq_table_msg_type_choice_ni_s_fields,
                                                           uint64_t{0x1},
                                                           seq_table_msg_type_choice_ni_s_opts,
                                                           seq_table_msg_type_choice_ni_s_names,
                                                           3,
                                                           2},
                                                          true,
                                                          true,
                                                          nullptr,
                                                          0};

// MessageType-Choice-RRC ::= SEQUENCE
static const asn1::table::seq_field seq_table_msg_type_choice_rrc_s_fields[] = {
    {offsetof(msg_type_choice_rrc_s, rrc_msg),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_msg_id_s::desc}};
static const asn1::table::field_name seq_table_msg_type_choice_rrc_s_names[] = {{"rRC-Message", nullptr}};
const asn1::table::seq_desc          msg_type_choice_rrc_s::desc             = {
    {seq_table_msg_type_choice_rrc_s_fields, uint64_t{0x1}, nullptr, seq_table_msg_type_choice_rrc_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// MessageType-Choice ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_msg_type_choice_c_alternatives[] = {
    &asn1::table::alternative_ops<msg_type_choice_ni_s, false>::value,
    &asn1::table::alternative_ops<msg_type_choice_rrc_s, false>::value};
const asn1::table::choice_storage_desc msg_type_choice_c::storage_desc = {choice_storage_msg_type_choice_c_alternatives,
                                                                          2,
                                                                          "msg_type_choice_c"};

msg_type_choice_ni_s& msg_type_choice_c::set_msg_type_choice_ni()
{
  set(types::msg_type_choice_ni);
  return c.get<msg_type_choice_ni_s>();
}
msg_type_choice_rrc_s& msg_type_choice_c::set_msg_type_choice_rrc()
{
  set(types::msg_type_choice_rrc);
  return c.get<msg_type_choice_rrc_s>();
}
static const asn1::table::seq_field  choice_table_msg_type_choice_c_alternatives[] = {{0,
                                                                                       asn1::table::seq_field::mandatory,
                                                                                       asn1::table::field_op::sequence,
                                                                                       false,
                                                                                       false,
                                                                                       0,
                                                                                       0,
                                                                                       nullptr,
                                                                                       &msg_type_choice_ni_s::desc},
                                                                                      {0,
                                                                                       asn1::table::seq_field::mandatory,
                                                                                       asn1::table::field_op::sequence,
                                                                                       false,
                                                                                       false,
                                                                                       0,
                                                                                       0,
                                                                                       nullptr,
                                                                                       &msg_type_choice_rrc_s::desc}};
static const asn1::table::field_name choice_table_msg_type_choice_c_names[] = {{"messageType-Choice-NI", nullptr},
                                                                               {"messageType-Choice-RRC", nullptr}};
const asn1::table::choice_desc       msg_type_choice_c::desc = {asn1::table::enum_int_type<msg_type_choice_c::types>(),
                                                                msg_type_choice_c::types::nof_types,
                                                                msg_type_choice_c::types::nof_exts,
                                                                msg_type_choice_c::types::has_ext,
                                                                true,
                                                                choice_table_msg_type_choice_c_alternatives,
                                                                nullptr,
                                                                choice_table_msg_type_choice_c_names,
                                                                "msg_type_choice_c",
                                                                offsetof(msg_type_choice_c, c.type),
                                                                offsetof(msg_type_choice_c, c.value),
                                                                false,
                                                                &msg_type_choice_c::storage_desc};

static const char* enum_names_msg_type_choice_c_types_values[]   = {"messageType-Choice-NI", "messageType-Choice-RRC"};
const asn1::table::enum_desc msg_type_choice_c::types_opts::desc = {enum_names_msg_type_choice_c_types_values,
                                                                    2,
                                                                    "msg_type_choice_c::types"};

// EventTrigger-UE-Info-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_ue_info_item_s_fields[] = {
    {offsetof(event_trigger_ue_info_item_s, event_trigger_ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(event_trigger_ue_info_item_s, ue_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_ue_info_item_s::ue_type_c_::desc},
    {offsetof(event_trigger_ue_info_item_s, lc_or),
     (int32_t)offsetof(event_trigger_ue_info_item_s, lc_or_present),
     asn1::table::enum_op<decltype(event_trigger_ue_info_item_s::lc_or)>(),
     decltype(event_trigger_ue_info_item_s::lc_or)::has_ext,
     false,
     decltype(event_trigger_ue_info_item_s::lc_or)::nof_types,
     decltype(event_trigger_ue_info_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_e2sm_rc_ies_opts::desc}};
static const asn1::table::field_name seq_table_event_trigger_ue_info_item_s_names[] = {{"eventTriggerUEID", nullptr},
                                                                                       {"ueType", nullptr},
                                                                                       {"logicalOR", nullptr}};
static const asn1::table::seq_opt    seq_table_event_trigger_ue_info_item_s_opts[]  = {
    {2, (int32_t)offsetof(event_trigger_ue_info_item_s, lc_or_present)}};
const asn1::table::seq_desc event_trigger_ue_info_item_s::desc = {{seq_table_event_trigger_ue_info_item_s_fields,
                                                                   uint64_t{0x3},
                                                                   seq_table_event_trigger_ue_info_item_s_opts,
                                                                   seq_table_event_trigger_ue_info_item_s_names,
                                                                   3,
                                                                   1},
                                                                  true,
                                                                  true,
                                                                  nullptr,
                                                                  0};

static const asn1::table::storage_ops* const choice_storage_event_trigger_ue_info_item_s_ue_type_c__alternatives[] = {
    &asn1::table::alternative_ops<event_trigger_ue_info_item_choice_individual_s, false, false>::value,
    &asn1::table::alternative_ops<event_trigger_ue_info_item_choice_group_s, false, false>::value};
const asn1::table::choice_storage_desc event_trigger_ue_info_item_s::ue_type_c_::storage_desc = {
    choice_storage_event_trigger_ue_info_item_s_ue_type_c__alternatives,
    2,
    "event_trigger_ue_info_item_s::ue_type_c_"};

event_trigger_ue_info_item_choice_individual_s&
event_trigger_ue_info_item_s::ue_type_c_::set_ue_type_choice_individual()
{
  set(types::ue_type_choice_individual);
  return c.get<event_trigger_ue_info_item_choice_individual_s>();
}
event_trigger_ue_info_item_choice_group_s& event_trigger_ue_info_item_s::ue_type_c_::set_ue_type_choice_group()
{
  set(types::ue_type_choice_group);
  return c.get<event_trigger_ue_info_item_choice_group_s>();
}
static const asn1::table::seq_field choice_table_event_trigger_ue_info_item_s_ue_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_ue_info_item_choice_individual_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_ue_info_item_choice_group_s::desc}};
static const asn1::table::field_name choice_table_event_trigger_ue_info_item_s_ue_type_c__names[] = {
    {"ueType-Choice-Individual", nullptr},
    {"ueType-Choice-Group", nullptr}};
const asn1::table::choice_desc event_trigger_ue_info_item_s::ue_type_c_::desc = {
    asn1::table::enum_int_type<event_trigger_ue_info_item_s::ue_type_c_::types>(),
    event_trigger_ue_info_item_s::ue_type_c_::types::nof_types,
    event_trigger_ue_info_item_s::ue_type_c_::types::nof_exts,
    event_trigger_ue_info_item_s::ue_type_c_::types::has_ext,
    true,
    choice_table_event_trigger_ue_info_item_s_ue_type_c__alternatives,
    nullptr,
    choice_table_event_trigger_ue_info_item_s_ue_type_c__names,
    "event_trigger_ue_info_item_s::ue_type_c_",
    offsetof(event_trigger_ue_info_item_s::ue_type_c_, c.type),
    offsetof(event_trigger_ue_info_item_s::ue_type_c_, c.value),
    false,
    &event_trigger_ue_info_item_s::ue_type_c_::storage_desc};

static const char* enum_names_event_trigger_ue_info_item_s_ue_type_c__types_values[]    = {"ueType-Choice-Individual",
                                                                                           "ueType-Choice-Group"};
const asn1::table::enum_desc event_trigger_ue_info_item_s::ue_type_c_::types_opts::desc = {
    enum_names_event_trigger_ue_info_item_s_ue_type_c__types_values,
    2,
    "event_trigger_ue_info_item_s::ue_type_c_::types"};

// EventTrigger-UE-Info ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_ue_info_s_fields[] = {
    {offsetof(event_trigger_ue_info_s, ue_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(event_trigger_ue_info_s::ue_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &event_trigger_ue_info_item_s::desc}};
static const asn1::table::field_name seq_table_event_trigger_ue_info_s_names[] = {{"ueInfo-List", nullptr}};
const asn1::table::seq_desc          event_trigger_ue_info_s::desc             = {
    {seq_table_event_trigger_ue_info_s_fields, uint64_t{0x1}, nullptr, seq_table_event_trigger_ue_info_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

// EventTrigger-UEevent-Info-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_ue_event_info_item_s_fields[] = {
    {offsetof(event_trigger_ue_event_info_item_s, ue_event_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(event_trigger_ue_event_info_item_s, lc_or),
     (int32_t)offsetof(event_trigger_ue_event_info_item_s, lc_or_present),
     asn1::table::enum_op<decltype(event_trigger_ue_event_info_item_s::lc_or)>(),
     decltype(event_trigger_ue_event_info_item_s::lc_or)::has_ext,
     false,
     decltype(event_trigger_ue_event_info_item_s::lc_or)::nof_types,
     decltype(event_trigger_ue_event_info_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_e2sm_rc_ies_opts::desc}};
static const asn1::table::field_name seq_table_event_trigger_ue_event_info_item_s_names[] = {{"ueEventID", nullptr},
                                                                                             {"logicalOR", nullptr}};
static const asn1::table::seq_opt    seq_table_event_trigger_ue_event_info_item_s_opts[]  = {
    {1, (int32_t)offsetof(event_trigger_ue_event_info_item_s, lc_or_present)}};
const asn1::table::seq_desc event_trigger_ue_event_info_item_s::desc = {
    {seq_table_event_trigger_ue_event_info_item_s_fields,
     uint64_t{0x1},
     seq_table_event_trigger_ue_event_info_item_s_opts,
     seq_table_event_trigger_ue_event_info_item_s_names,
     2,
     1},
    true,
    true,
    nullptr,
    0};

// EventTrigger-UEevent-Info ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_ue_event_info_s_fields[] = {
    {offsetof(event_trigger_ue_event_info_s, ue_event_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(event_trigger_ue_event_info_s::ue_event_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &event_trigger_ue_event_info_item_s::desc}};
static const asn1::table::field_name seq_table_event_trigger_ue_event_info_s_names[] = {{"ueEvent-List", nullptr}};
const asn1::table::seq_desc event_trigger_ue_event_info_s::desc = {{seq_table_event_trigger_ue_event_info_s_fields,
                                                                    uint64_t{0x1},
                                                                    nullptr,
                                                                    seq_table_event_trigger_ue_event_info_s_names,
                                                                    1,
                                                                    0},
                                                                   true,
                                                                   true,
                                                                   nullptr,
                                                                   0};

// E2SM-RC-EventTrigger-Format1-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_event_trigger_format1_item_s_fields[] = {
    {offsetof(e2sm_rc_event_trigger_format1_item_s, ric_event_trigger_condition_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_event_trigger_format1_item_s, msg_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &msg_type_choice_c::desc},
    {offsetof(e2sm_rc_event_trigger_format1_item_s, msg_direction),
     (int32_t)offsetof(e2sm_rc_event_trigger_format1_item_s, msg_direction_present),
     asn1::table::enum_op<decltype(e2sm_rc_event_trigger_format1_item_s::msg_direction)>(),
     decltype(e2sm_rc_event_trigger_format1_item_s::msg_direction)::has_ext,
     false,
     decltype(e2sm_rc_event_trigger_format1_item_s::msg_direction)::nof_types,
     decltype(e2sm_rc_event_trigger_format1_item_s::msg_direction)::nof_exts,
     nullptr,
     &e2sm_rc_event_trigger_format1_item_s::msg_direction_opts::desc},
    {offsetof(e2sm_rc_event_trigger_format1_item_s, associated_ue_info),
     (int32_t)offsetof(e2sm_rc_event_trigger_format1_item_s, associated_ue_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_ue_info_s::desc},
    {offsetof(e2sm_rc_event_trigger_format1_item_s, associated_ue_event),
     (int32_t)offsetof(e2sm_rc_event_trigger_format1_item_s, associated_ue_event_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_ue_event_info_s::desc},
    {offsetof(e2sm_rc_event_trigger_format1_item_s, lc_or),
     (int32_t)offsetof(e2sm_rc_event_trigger_format1_item_s, lc_or_present),
     asn1::table::enum_op<decltype(e2sm_rc_event_trigger_format1_item_s::lc_or)>(),
     decltype(e2sm_rc_event_trigger_format1_item_s::lc_or)::has_ext,
     false,
     decltype(e2sm_rc_event_trigger_format1_item_s::lc_or)::nof_types,
     decltype(e2sm_rc_event_trigger_format1_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_e2sm_rc_ies_opts::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_event_trigger_format1_item_s_names[] = {
    {"ric-eventTriggerCondition-ID", nullptr},
    {"messageType", nullptr},
    {"messageDirection", nullptr},
    {"associatedUEInfo", nullptr},
    {"associatedUEEvent", nullptr},
    {"logicalOR", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_event_trigger_format1_item_s_opts[] = {
    {2, (int32_t)offsetof(e2sm_rc_event_trigger_format1_item_s, msg_direction_present)},
    {3, (int32_t)offsetof(e2sm_rc_event_trigger_format1_item_s, associated_ue_info_present)},
    {4, (int32_t)offsetof(e2sm_rc_event_trigger_format1_item_s, associated_ue_event_present)},
    {5, (int32_t)offsetof(e2sm_rc_event_trigger_format1_item_s, lc_or_present)}};
const asn1::table::seq_desc e2sm_rc_event_trigger_format1_item_s::desc = {
    {seq_table_e2sm_rc_event_trigger_format1_item_s_fields,
     uint64_t{0x3},
     seq_table_e2sm_rc_event_trigger_format1_item_s_opts,
     seq_table_e2sm_rc_event_trigger_format1_item_s_names,
     6,
     4},
    true,
    true,
    nullptr,
    0};

static const char* enum_names_e2sm_rc_event_trigger_format1_item_s_msg_direction_e__values[] = {"incoming", "outgoing"};
const asn1::table::enum_desc e2sm_rc_event_trigger_format1_item_s::msg_direction_opts::desc  = {
    enum_names_e2sm_rc_event_trigger_format1_item_s_msg_direction_e__values,
    2,
    "e2sm_rc_event_trigger_format1_item_s::msg_direction_e_"};

// E2SM-RC-EventTrigger-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_event_trigger_format1_s_fields[] = {
    {offsetof(e2sm_rc_event_trigger_format1_s, msg_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_event_trigger_format1_s::msg_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_event_trigger_format1_item_s::desc},
    {offsetof(e2sm_rc_event_trigger_format1_s, global_associated_ue_info),
     (int32_t)offsetof(e2sm_rc_event_trigger_format1_s, global_associated_ue_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_ue_info_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_event_trigger_format1_s_names[] = {
    {"message-List", nullptr},
    {"globalAssociatedUEInfo", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_event_trigger_format1_s_opts[] = {
    {1, (int32_t)offsetof(e2sm_rc_event_trigger_format1_s, global_associated_ue_info_present)}};
const asn1::table::seq_desc e2sm_rc_event_trigger_format1_s::desc = {{seq_table_e2sm_rc_event_trigger_format1_s_fields,
                                                                      uint64_t{0x1},
                                                                      seq_table_e2sm_rc_event_trigger_format1_s_opts,
                                                                      seq_table_e2sm_rc_event_trigger_format1_s_names,
                                                                      2,
                                                                      1},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2SM-RC-EventTrigger-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_event_trigger_format2_s_fields[] = {
    {offsetof(e2sm_rc_event_trigger_format2_s, ric_call_process_type_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_event_trigger_format2_s, ric_call_process_breakpoint_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_event_trigger_format2_s, associated_e2_node_info),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::item_list_codec<decltype(e2sm_rc_event_trigger_format2_s::associated_e2_node_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_testing_item_s::desc},
    {offsetof(e2sm_rc_event_trigger_format2_s, associated_ue_info),
     (int32_t)offsetof(e2sm_rc_event_trigger_format2_s, associated_ue_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_ue_info_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_event_trigger_format2_s_names[] = {
    {"ric-callProcessType-ID", nullptr},
    {"ric-callProcessBreakpoint-ID", nullptr},
    {"associatedE2NodeInfo", nullptr},
    {"associatedUEInfo", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_event_trigger_format2_s_opts[] = {
    {2, asn1::table::seq_field::list_presence},
    {3, (int32_t)offsetof(e2sm_rc_event_trigger_format2_s, associated_ue_info_present)}};
const asn1::table::seq_desc e2sm_rc_event_trigger_format2_s::desc = {{seq_table_e2sm_rc_event_trigger_format2_s_fields,
                                                                      uint64_t{0x3},
                                                                      seq_table_e2sm_rc_event_trigger_format2_s_opts,
                                                                      seq_table_e2sm_rc_event_trigger_format2_s_names,
                                                                      4,
                                                                      2},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// EventTrigger-Cell-Info-Item-Choice-Individual ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_cell_info_item_choice_individual_s_fields[] = {
    {offsetof(event_trigger_cell_info_item_choice_individual_s, cell_global_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_c::desc}};
static const asn1::table::field_name seq_table_event_trigger_cell_info_item_choice_individual_s_names[] = {
    {"cellGlobalID", nullptr}};
const asn1::table::seq_desc event_trigger_cell_info_item_choice_individual_s::desc = {
    {seq_table_event_trigger_cell_info_item_choice_individual_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_event_trigger_cell_info_item_choice_individual_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// EventTrigger-Cell-Info-Item-Choice-Group ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_cell_info_item_choice_group_s_fields[] = {
    {offsetof(event_trigger_cell_info_item_choice_group_s, ran_param_testing),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::item_list_codec<decltype(event_trigger_cell_info_item_choice_group_s::ran_param_testing),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_testing_item_s::desc}};
static const asn1::table::field_name seq_table_event_trigger_cell_info_item_choice_group_s_names[] = {
    {"ranParameterTesting", nullptr}};
const asn1::table::seq_desc event_trigger_cell_info_item_choice_group_s::desc = {
    {seq_table_event_trigger_cell_info_item_choice_group_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_event_trigger_cell_info_item_choice_group_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// EventTrigger-Cell-Info-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_cell_info_item_s_fields[] = {
    {offsetof(event_trigger_cell_info_item_s, event_trigger_cell_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(event_trigger_cell_info_item_s, cell_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_cell_info_item_s::cell_type_c_::desc},
    {offsetof(event_trigger_cell_info_item_s, lc_or),
     (int32_t)offsetof(event_trigger_cell_info_item_s, lc_or_present),
     asn1::table::enum_op<decltype(event_trigger_cell_info_item_s::lc_or)>(),
     decltype(event_trigger_cell_info_item_s::lc_or)::has_ext,
     false,
     decltype(event_trigger_cell_info_item_s::lc_or)::nof_types,
     decltype(event_trigger_cell_info_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_e2sm_rc_ies_opts::desc}};
static const asn1::table::field_name seq_table_event_trigger_cell_info_item_s_names[] = {
    {"eventTriggerCellID", nullptr},
    {"cellType", nullptr},
    {"logicalOR", nullptr}};
static const asn1::table::seq_opt seq_table_event_trigger_cell_info_item_s_opts[] = {
    {2, (int32_t)offsetof(event_trigger_cell_info_item_s, lc_or_present)}};
const asn1::table::seq_desc event_trigger_cell_info_item_s::desc = {{seq_table_event_trigger_cell_info_item_s_fields,
                                                                     uint64_t{0x3},
                                                                     seq_table_event_trigger_cell_info_item_s_opts,
                                                                     seq_table_event_trigger_cell_info_item_s_names,
                                                                     3,
                                                                     1},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

static const asn1::table::storage_ops* const choice_storage_event_trigger_cell_info_item_s_cell_type_c__alternatives[] =
    {&asn1::table::alternative_ops<event_trigger_cell_info_item_choice_individual_s, false>::value,
     &asn1::table::alternative_ops<event_trigger_cell_info_item_choice_group_s, false, false>::value};
const asn1::table::choice_storage_desc event_trigger_cell_info_item_s::cell_type_c_::storage_desc = {
    choice_storage_event_trigger_cell_info_item_s_cell_type_c__alternatives,
    2,
    "event_trigger_cell_info_item_s::cell_type_c_"};

event_trigger_cell_info_item_choice_individual_s&
event_trigger_cell_info_item_s::cell_type_c_::set_cell_type_choice_individual()
{
  set(types::cell_type_choice_individual);
  return c.get<event_trigger_cell_info_item_choice_individual_s>();
}
event_trigger_cell_info_item_choice_group_s& event_trigger_cell_info_item_s::cell_type_c_::set_cell_type_choice_group()
{
  set(types::cell_type_choice_group);
  return c.get<event_trigger_cell_info_item_choice_group_s>();
}
static const asn1::table::seq_field choice_table_event_trigger_cell_info_item_s_cell_type_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_cell_info_item_choice_individual_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_cell_info_item_choice_group_s::desc}};
static const asn1::table::field_name choice_table_event_trigger_cell_info_item_s_cell_type_c__names[] = {
    {"cellType-Choice-Individual", nullptr},
    {"cellType-Choice-Group", nullptr}};
const asn1::table::choice_desc event_trigger_cell_info_item_s::cell_type_c_::desc = {
    asn1::table::enum_int_type<event_trigger_cell_info_item_s::cell_type_c_::types>(),
    event_trigger_cell_info_item_s::cell_type_c_::types::nof_types,
    event_trigger_cell_info_item_s::cell_type_c_::types::nof_exts,
    event_trigger_cell_info_item_s::cell_type_c_::types::has_ext,
    true,
    choice_table_event_trigger_cell_info_item_s_cell_type_c__alternatives,
    nullptr,
    choice_table_event_trigger_cell_info_item_s_cell_type_c__names,
    "event_trigger_cell_info_item_s::cell_type_c_",
    offsetof(event_trigger_cell_info_item_s::cell_type_c_, c.type),
    offsetof(event_trigger_cell_info_item_s::cell_type_c_, c.value),
    false,
    &event_trigger_cell_info_item_s::cell_type_c_::storage_desc};

static const char* enum_names_event_trigger_cell_info_item_s_cell_type_c__types_values[] = {
    "cellType-Choice-Individual",
    "cellType-Choice-Group"};
const asn1::table::enum_desc event_trigger_cell_info_item_s::cell_type_c_::types_opts::desc = {
    enum_names_event_trigger_cell_info_item_s_cell_type_c__types_values,
    2,
    "event_trigger_cell_info_item_s::cell_type_c_::types"};

// EventTrigger-Cell-Info ::= SEQUENCE
static const asn1::table::seq_field seq_table_event_trigger_cell_info_s_fields[] = {
    {offsetof(event_trigger_cell_info_s, cell_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(event_trigger_cell_info_s::cell_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &event_trigger_cell_info_item_s::desc}};
static const asn1::table::field_name seq_table_event_trigger_cell_info_s_names[] = {{"cellInfo-List", nullptr}};
const asn1::table::seq_desc          event_trigger_cell_info_s::desc = {{seq_table_event_trigger_cell_info_s_fields,
                                                                         uint64_t{0x1},
                                                                         nullptr,
                                                                         seq_table_event_trigger_cell_info_s_names,
                                                                         1,
                                                                         0},
                                                                        true,
                                                                        true,
                                                                        nullptr,
                                                                        0};

// E2SM-RC-EventTrigger-Format3-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_event_trigger_format3_item_s_fields[] = {
    {offsetof(e2sm_rc_event_trigger_format3_item_s, ric_event_trigger_condition_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_event_trigger_format3_item_s, e2_node_info_change_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{511},
     nullptr},
    {offsetof(e2sm_rc_event_trigger_format3_item_s, associated_cell_info),
     (int32_t)offsetof(e2sm_rc_event_trigger_format3_item_s, associated_cell_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_cell_info_s::desc},
    {offsetof(e2sm_rc_event_trigger_format3_item_s, lc_or),
     (int32_t)offsetof(e2sm_rc_event_trigger_format3_item_s, lc_or_present),
     asn1::table::enum_op<decltype(e2sm_rc_event_trigger_format3_item_s::lc_or)>(),
     decltype(e2sm_rc_event_trigger_format3_item_s::lc_or)::has_ext,
     false,
     decltype(e2sm_rc_event_trigger_format3_item_s::lc_or)::nof_types,
     decltype(e2sm_rc_event_trigger_format3_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_e2sm_rc_ies_opts::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_event_trigger_format3_item_s_names[] = {
    {"ric-eventTriggerCondition-ID", nullptr},
    {"e2NodeInfoChange-ID", nullptr},
    {"associatedCellInfo", nullptr},
    {"logicalOR", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_event_trigger_format3_item_s_opts[] = {
    {2, (int32_t)offsetof(e2sm_rc_event_trigger_format3_item_s, associated_cell_info_present)},
    {3, (int32_t)offsetof(e2sm_rc_event_trigger_format3_item_s, lc_or_present)}};
const asn1::table::seq_desc e2sm_rc_event_trigger_format3_item_s::desc = {
    {seq_table_e2sm_rc_event_trigger_format3_item_s_fields,
     uint64_t{0x3},
     seq_table_e2sm_rc_event_trigger_format3_item_s_opts,
     seq_table_e2sm_rc_event_trigger_format3_item_s_names,
     4,
     2},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-EventTrigger-Format3 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_event_trigger_format3_s_fields[] = {
    {offsetof(e2sm_rc_event_trigger_format3_s, e2_node_info_change_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_event_trigger_format3_s::e2_node_info_change_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_event_trigger_format3_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_event_trigger_format3_s_names[] = {
    {"e2NodeInfoChange-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_event_trigger_format3_s::desc = {{seq_table_e2sm_rc_event_trigger_format3_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_e2sm_rc_event_trigger_format3_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// RRC-State ::= ENUMERATED
static const char*           enum_names_rrc_state_e_values[] = {"rrc-connected", "rrc-inactive", "rrc-idle", "any"};
const asn1::table::enum_desc rrc_state_opts::desc            = {enum_names_rrc_state_e_values, 4, "rrc_state_e"};

// TriggerType-Choice-RRCstate-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_trigger_type_choice_rrc_state_item_s_fields[] = {
    {offsetof(trigger_type_choice_rrc_state_item_s, state_changed_to),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trigger_type_choice_rrc_state_item_s::state_changed_to)>(),
     decltype(trigger_type_choice_rrc_state_item_s::state_changed_to)::has_ext,
     false,
     decltype(trigger_type_choice_rrc_state_item_s::state_changed_to)::nof_types,
     decltype(trigger_type_choice_rrc_state_item_s::state_changed_to)::nof_exts,
     nullptr,
     &rrc_state_opts::desc},
    {offsetof(trigger_type_choice_rrc_state_item_s, lc_or),
     (int32_t)offsetof(trigger_type_choice_rrc_state_item_s, lc_or_present),
     asn1::table::enum_op<decltype(trigger_type_choice_rrc_state_item_s::lc_or)>(),
     decltype(trigger_type_choice_rrc_state_item_s::lc_or)::has_ext,
     false,
     decltype(trigger_type_choice_rrc_state_item_s::lc_or)::nof_types,
     decltype(trigger_type_choice_rrc_state_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_e2sm_rc_ies_opts::desc}};
static const asn1::table::field_name seq_table_trigger_type_choice_rrc_state_item_s_names[] = {
    {"stateChangedTo", nullptr},
    {"logicalOR", nullptr}};
static const asn1::table::seq_opt seq_table_trigger_type_choice_rrc_state_item_s_opts[] = {
    {1, (int32_t)offsetof(trigger_type_choice_rrc_state_item_s, lc_or_present)}};
const asn1::table::seq_desc trigger_type_choice_rrc_state_item_s::desc = {
    {seq_table_trigger_type_choice_rrc_state_item_s_fields,
     uint64_t{0x1},
     seq_table_trigger_type_choice_rrc_state_item_s_opts,
     seq_table_trigger_type_choice_rrc_state_item_s_names,
     2,
     1},
    true,
    true,
    nullptr,
    0};

// TriggerType-Choice-RRCstate ::= SEQUENCE
static const asn1::table::seq_field seq_table_trigger_type_choice_rrc_state_s_fields[] = {
    {offsetof(trigger_type_choice_rrc_state_s, rrc_state_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     7,
     &asn1::table::item_list_codec<decltype(trigger_type_choice_rrc_state_s::rrc_state_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &trigger_type_choice_rrc_state_item_s::desc}};
static const asn1::table::field_name seq_table_trigger_type_choice_rrc_state_s_names[] = {{"rrcState-List", nullptr}};
const asn1::table::seq_desc trigger_type_choice_rrc_state_s::desc = {{seq_table_trigger_type_choice_rrc_state_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_trigger_type_choice_rrc_state_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// TriggerType-Choice-UEID ::= SEQUENCE
static const asn1::table::seq_field seq_table_trigger_type_choice_ue_id_s_fields[] = {
    {offsetof(trigger_type_choice_ue_id_s, ue_id_change_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{1},
     uint64_t{511},
     nullptr}};
static const asn1::table::field_name seq_table_trigger_type_choice_ue_id_s_names[] = {{"ueIDchange-ID", nullptr}};
const asn1::table::seq_desc          trigger_type_choice_ue_id_s::desc = {{seq_table_trigger_type_choice_ue_id_s_fields,
                                                                           uint64_t{0x1},
                                                                           nullptr,
                                                                           seq_table_trigger_type_choice_ue_id_s_names,
                                                                           1,
                                                                           0},
                                                                          true,
                                                                          true,
                                                                          nullptr,
                                                                          0};

// TriggerType-Choice-L2state ::= SEQUENCE
static const asn1::table::seq_field seq_table_trigger_type_choice_l2state_s_fields[] = {
    {offsetof(trigger_type_choice_l2state_s, associated_l2variables),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::item_list_codec<decltype(trigger_type_choice_l2state_s::associated_l2variables),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_testing_item_s::desc}};
static const asn1::table::field_name seq_table_trigger_type_choice_l2state_s_names[] = {
    {"associatedL2variables", nullptr}};
const asn1::table::seq_desc trigger_type_choice_l2state_s::desc = {{seq_table_trigger_type_choice_l2state_s_fields,
                                                                    uint64_t{0x1},
                                                                    nullptr,
                                                                    seq_table_trigger_type_choice_l2state_s_names,
                                                                    1,
                                                                    0},
                                                                   true,
                                                                   true,
                                                                   nullptr,
                                                                   0};

// TriggerType-Choice-UEcontext ::= SEQUENCE
static const asn1::table::seq_field seq_table_trigger_type_choice_ue_context_s_fields[] = {
    {offsetof(trigger_type_choice_ue_context_s, associated_ue_ctxt_variables),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::item_list_codec<decltype(trigger_type_choice_ue_context_s::associated_ue_ctxt_variables),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_testing_item_s::desc}};
static const asn1::table::field_name seq_table_trigger_type_choice_ue_context_s_names[] = {
    {"associatedUECtxtVariables", nullptr}};
const asn1::table::seq_desc trigger_type_choice_ue_context_s::desc = {
    {seq_table_trigger_type_choice_ue_context_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_trigger_type_choice_ue_context_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// TriggerType-Choice-MIMOandBFconfig ::= SEQUENCE
static const asn1::table::seq_field seq_table_trigger_type_choice_mim_oand_b_fcfg_s_fields[] = {
    {offsetof(trigger_type_choice_mim_oand_b_fcfg_s, mim_otrans_mode_state),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(trigger_type_choice_mim_oand_b_fcfg_s::mim_otrans_mode_state)>(),
     decltype(trigger_type_choice_mim_oand_b_fcfg_s::mim_otrans_mode_state)::has_ext,
     false,
     decltype(trigger_type_choice_mim_oand_b_fcfg_s::mim_otrans_mode_state)::nof_types,
     decltype(trigger_type_choice_mim_oand_b_fcfg_s::mim_otrans_mode_state)::nof_exts,
     nullptr,
     &trigger_type_choice_mim_oand_b_fcfg_s::mim_otrans_mode_state_opts::desc}};
static const asn1::table::field_name seq_table_trigger_type_choice_mim_oand_b_fcfg_s_names[] = {
    {"mIMOtransModeState", nullptr}};
const asn1::table::seq_desc trigger_type_choice_mim_oand_b_fcfg_s::desc = {
    {seq_table_trigger_type_choice_mim_oand_b_fcfg_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_trigger_type_choice_mim_oand_b_fcfg_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

static const char* enum_names_trigger_type_choice_mim_oand_b_fcfg_s_mim_otrans_mode_state_e__values[] = {"enabled",
                                                                                                         "disabled"};
const asn1::table::enum_desc trigger_type_choice_mim_oand_b_fcfg_s::mim_otrans_mode_state_opts::desc  = {
    enum_names_trigger_type_choice_mim_oand_b_fcfg_s_mim_otrans_mode_state_e__values,
    2,
    "trigger_type_choice_mim_oand_b_fcfg_s::mim_otrans_mode_state_e_"};

// L2MACschChgType-Choice ::= CHOICE
static const asn1::table::seq_field choice_table_l2_ma_csch_chg_type_choice_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trigger_type_choice_mim_oand_b_fcfg_s::desc}};
static const asn1::table::field_name choice_table_l2_ma_csch_chg_type_choice_c_names[] = {
    {"triggerType-Choice-MIMOandBFconfig", nullptr}};
const asn1::table::choice_desc l2_ma_csch_chg_type_choice_c::desc = {
    asn1::table::enum_int_type<l2_ma_csch_chg_type_choice_c::types>(),
    l2_ma_csch_chg_type_choice_c::types::nof_types,
    l2_ma_csch_chg_type_choice_c::types::nof_exts,
    l2_ma_csch_chg_type_choice_c::types::has_ext,
    true,
    choice_table_l2_ma_csch_chg_type_choice_c_alternatives,
    nullptr,
    choice_table_l2_ma_csch_chg_type_choice_c_names,
    "l2_ma_csch_chg_type_choice_c",
    asn1::table::choice_desc::no_member,
    offsetof(l2_ma_csch_chg_type_choice_c, c),
    false};

static const char* enum_names_l2_ma_csch_chg_type_choice_c_types_values[]   = {"triggerType-Choice-MIMOandBFconfig"};
const asn1::table::enum_desc l2_ma_csch_chg_type_choice_c::types_opts::desc = {
    enum_names_l2_ma_csch_chg_type_choice_c_types_values,
    1,
    "l2_ma_csch_chg_type_choice_c::types"};

// TriggerType-Choice-L2MACschChg ::= SEQUENCE
static const asn1::table::seq_field seq_table_trigger_type_choice_l2_ma_csch_chg_s_fields[] = {
    {offsetof(trigger_type_choice_l2_ma_csch_chg_s, l2_ma_csch_chg_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &l2_ma_csch_chg_type_choice_c::desc}};
static const asn1::table::field_name seq_table_trigger_type_choice_l2_ma_csch_chg_s_names[] = {
    {"l2MACschChgType", nullptr}};
const asn1::table::seq_desc trigger_type_choice_l2_ma_csch_chg_s::desc = {
    {seq_table_trigger_type_choice_l2_ma_csch_chg_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_trigger_type_choice_l2_ma_csch_chg_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// TriggerType-Choice ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_trigger_type_choice_c_alternatives[] = {
    &asn1::table::alternative_ops<trigger_type_choice_rrc_state_s, false>::value,
    &asn1::table::alternative_ops<trigger_type_choice_ue_id_s, false>::value,
    &asn1::table::alternative_ops<trigger_type_choice_l2state_s, false, false>::value,
    &asn1::table::alternative_ops<trigger_type_choice_ue_context_s, false, false>::value,
    &asn1::table::alternative_ops<trigger_type_choice_l2_ma_csch_chg_s, false>::value};
const asn1::table::choice_storage_desc trigger_type_choice_c::storage_desc = {
    choice_storage_trigger_type_choice_c_alternatives,
    5,
    "trigger_type_choice_c"};

trigger_type_choice_rrc_state_s& trigger_type_choice_c::set_trigger_type_choice_rrc_state()
{
  set(types::trigger_type_choice_rrc_state);
  return c.get<trigger_type_choice_rrc_state_s>();
}
trigger_type_choice_ue_id_s& trigger_type_choice_c::set_trigger_type_choice_ue_id()
{
  set(types::trigger_type_choice_ue_id);
  return c.get<trigger_type_choice_ue_id_s>();
}
trigger_type_choice_l2state_s& trigger_type_choice_c::set_trigger_type_choice_l2state()
{
  set(types::trigger_type_choice_l2state);
  return c.get<trigger_type_choice_l2state_s>();
}
trigger_type_choice_ue_context_s& trigger_type_choice_c::set_trigger_type_choice_ue_context()
{
  set(types::trigger_type_choice_ue_context);
  return c.get<trigger_type_choice_ue_context_s>();
}
trigger_type_choice_l2_ma_csch_chg_s& trigger_type_choice_c::set_trigger_type_choice_l2_ma_csch_chg()
{
  set(types::trigger_type_choice_l2_ma_csch_chg);
  return c.get<trigger_type_choice_l2_ma_csch_chg_s>();
}
static const asn1::table::seq_field choice_table_trigger_type_choice_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trigger_type_choice_rrc_state_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trigger_type_choice_ue_id_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trigger_type_choice_l2state_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trigger_type_choice_ue_context_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &trigger_type_choice_l2_ma_csch_chg_s::desc}};
static const asn1::table::field_name choice_table_trigger_type_choice_c_names[] = {
    {"triggerType-Choice-RRCstate", nullptr},
    {"triggerType-Choice-UEID", nullptr},
    {"triggerType-Choice-L2state", nullptr},
    {"triggerType-Choice-UEcontext", nullptr},
    {"triggerType-Choice-L2MACschChg", nullptr}};
const asn1::table::choice_desc trigger_type_choice_c::desc = {
    asn1::table::enum_int_type<trigger_type_choice_c::types>(),
    trigger_type_choice_c::types::nof_types,
    trigger_type_choice_c::types::nof_exts,
    trigger_type_choice_c::types::has_ext,
    true,
    choice_table_trigger_type_choice_c_alternatives,
    nullptr,
    choice_table_trigger_type_choice_c_names,
    "trigger_type_choice_c",
    offsetof(trigger_type_choice_c, c.type),
    offsetof(trigger_type_choice_c, c.value),
    false,
    &trigger_type_choice_c::storage_desc};

static const char*           enum_names_trigger_type_choice_c_types_values[] = {"triggerType-Choice-RRCstate",
                                                                                "triggerType-Choice-UEID",
                                                                                "triggerType-Choice-L2state",
                                                                                "triggerType-Choice-UEcontext",
                                                                                "triggerType-Choice-L2MACschChg"};
const asn1::table::enum_desc trigger_type_choice_c::types_opts::desc = {enum_names_trigger_type_choice_c_types_values,
                                                                        5,
                                                                        "trigger_type_choice_c::types"};

// E2SM-RC-EventTrigger-Format4-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_event_trigger_format4_item_s_fields[] = {
    {offsetof(e2sm_rc_event_trigger_format4_item_s, ric_event_trigger_condition_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_event_trigger_format4_item_s, trigger_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &trigger_type_choice_c::desc},
    {offsetof(e2sm_rc_event_trigger_format4_item_s, associated_ue_info),
     (int32_t)offsetof(e2sm_rc_event_trigger_format4_item_s, associated_ue_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &event_trigger_ue_info_s::desc},
    {offsetof(e2sm_rc_event_trigger_format4_item_s, lc_or),
     (int32_t)offsetof(e2sm_rc_event_trigger_format4_item_s, lc_or_present),
     asn1::table::enum_op<decltype(e2sm_rc_event_trigger_format4_item_s::lc_or)>(),
     decltype(e2sm_rc_event_trigger_format4_item_s::lc_or)::has_ext,
     false,
     decltype(e2sm_rc_event_trigger_format4_item_s::lc_or)::nof_types,
     decltype(e2sm_rc_event_trigger_format4_item_s::lc_or)::nof_exts,
     nullptr,
     &lc_or_e2sm_rc_ies_opts::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_event_trigger_format4_item_s_names[] = {
    {"ric-eventTriggerCondition-ID", nullptr},
    {"triggerType", nullptr},
    {"associatedUEInfo", nullptr},
    {"logicalOR", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_event_trigger_format4_item_s_opts[] = {
    {2, (int32_t)offsetof(e2sm_rc_event_trigger_format4_item_s, associated_ue_info_present)},
    {3, (int32_t)offsetof(e2sm_rc_event_trigger_format4_item_s, lc_or_present)}};
const asn1::table::seq_desc e2sm_rc_event_trigger_format4_item_s::desc = {
    {seq_table_e2sm_rc_event_trigger_format4_item_s_fields,
     uint64_t{0x3},
     seq_table_e2sm_rc_event_trigger_format4_item_s_opts,
     seq_table_e2sm_rc_event_trigger_format4_item_s_names,
     4,
     2},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-EventTrigger-Format4 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_event_trigger_format4_s_fields[] = {
    {offsetof(e2sm_rc_event_trigger_format4_s, ue_info_change_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_event_trigger_format4_s::ue_info_change_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_event_trigger_format4_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_event_trigger_format4_s_names[] = {
    {"uEInfoChange-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_event_trigger_format4_s::desc = {{seq_table_e2sm_rc_event_trigger_format4_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_e2sm_rc_event_trigger_format4_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2SM-RC-EventTrigger ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_event_trigger_s_fields[] = {
    {offsetof(e2sm_rc_event_trigger_s, ric_event_trigger_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_event_trigger_s_names[] = {
    {"ric-eventTrigger-formats", nullptr}};
const asn1::table::seq_desc e2sm_rc_event_trigger_s::desc = {
    {seq_table_e2sm_rc_event_trigger_s_fields, uint64_t{0x1}, nullptr, seq_table_e2sm_rc_event_trigger_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::storage_ops* const
    choice_storage_e2sm_rc_event_trigger_s_ric_event_trigger_formats_c__alternatives[] = {
        &asn1::table::alternative_ops<e2sm_rc_event_trigger_format1_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_rc_event_trigger_format2_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_rc_event_trigger_format3_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_rc_event_trigger_format4_s, false, false>::value,
        nullptr};
const asn1::table::choice_storage_desc e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::storage_desc = {
    choice_storage_e2sm_rc_event_trigger_s_ric_event_trigger_formats_c__alternatives,
    5,
    "e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_"};

e2sm_rc_event_trigger_format1_s& e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::set_event_trigger_format1()
{
  set(types::event_trigger_format1);
  return c.get<e2sm_rc_event_trigger_format1_s>();
}
e2sm_rc_event_trigger_format2_s& e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::set_event_trigger_format2()
{
  set(types::event_trigger_format2);
  return c.get<e2sm_rc_event_trigger_format2_s>();
}
e2sm_rc_event_trigger_format3_s& e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::set_event_trigger_format3()
{
  set(types::event_trigger_format3);
  return c.get<e2sm_rc_event_trigger_format3_s>();
}
e2sm_rc_event_trigger_format4_s& e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::set_event_trigger_format4()
{
  set(types::event_trigger_format4);
  return c.get<e2sm_rc_event_trigger_format4_s>();
}
void e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::set_event_trigger_format5()
{
  set(types::event_trigger_format5);
}
static const asn1::table::seq_field choice_table_e2sm_rc_event_trigger_s_ric_event_trigger_formats_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_event_trigger_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_event_trigger_format2_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_event_trigger_format3_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_event_trigger_format4_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_e2sm_rc_event_trigger_s_ric_event_trigger_formats_c__names[] = {
    {"eventTrigger-Format1", nullptr},
    {"eventTrigger-Format2", nullptr},
    {"eventTrigger-Format3", nullptr},
    {"eventTrigger-Format4", nullptr},
    {"eventTrigger-Format5", nullptr}};
const asn1::table::choice_desc e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::types>(),
    e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::types::nof_types,
    e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::types::nof_exts,
    e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_rc_event_trigger_s_ric_event_trigger_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_rc_event_trigger_s_ric_event_trigger_formats_c__names,
    "e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_",
    offsetof(e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_, c.type),
    offsetof(e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_, c.value),
    false,
    &e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::storage_desc};

static const char* enum_names_e2sm_rc_event_trigger_s_ric_event_trigger_formats_c__types_values[] = {
    "eventTrigger-Format1",
    "eventTrigger-Format2",
    "eventTrigger-Format3",
    "eventTrigger-Format4",
    "eventTrigger-Format5"};
const asn1::table::enum_desc e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::types_opts::desc = {
    enum_names_e2sm_rc_event_trigger_s_ric_event_trigger_formats_c__types_values,
    5,
    "e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::types"};
uint8_t e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5};
  return map_enum_number(numbers, 5, value, "e2sm_rc_event_trigger_s::ric_event_trigger_formats_c_::types");
}

// E2SM-RC-IndicationHeader-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_hdr_format1_s_fields[] = {
    {offsetof(e2sm_rc_ind_hdr_format1_s, ric_event_trigger_condition_id),
     (int32_t)offsetof(e2sm_rc_ind_hdr_format1_s, ric_event_trigger_condition_id_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_hdr_format1_s_names[] = {
    {"ric-eventTriggerCondition-ID", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_ind_hdr_format1_s_opts[] = {
    {0, (int32_t)offsetof(e2sm_rc_ind_hdr_format1_s, ric_event_trigger_condition_id_present)}};
const asn1::table::seq_desc e2sm_rc_ind_hdr_format1_s::desc = {{seq_table_e2sm_rc_ind_hdr_format1_s_fields,
                                                                uint64_t{0x0},
                                                                seq_table_e2sm_rc_ind_hdr_format1_s_opts,
                                                                seq_table_e2sm_rc_ind_hdr_format1_s_names,
                                                                1,
                                                                1},
                                                               true,
                                                               true,
                                                               nullptr,
                                                               0};

// E2SM-RC-IndicationHeader-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_hdr_format2_s_fields[] = {
    {offsetof(e2sm_rc_ind_hdr_format2_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc},
    {offsetof(e2sm_rc_ind_hdr_format2_s, ric_insert_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(e2sm_rc_ind_hdr_format2_s, ric_insert_ind_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_hdr_format2_s_names[] = {
    {"ueID", nullptr},
    {"ric-InsertStyle-Type", nullptr},
    {"ric-InsertIndication-ID", nullptr}};
const asn1::table::seq_desc e2sm_rc_ind_hdr_format2_s::desc = {{seq_table_e2sm_rc_ind_hdr_format2_s_fields,
                                                                uint64_t{0x7},
                                                                nullptr,
                                                                seq_table_e2sm_rc_ind_hdr_format2_s_names,
                                                                3,
                                                                0},
                                                               true,
                                                               true,
                                                               nullptr,
                                                               0};

// E2SM-RC-IndicationHeader-Format3 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_hdr_format3_s_fields[] = {
    {offsetof(e2sm_rc_ind_hdr_format3_s, ric_event_trigger_condition_id),
     (int32_t)offsetof(e2sm_rc_ind_hdr_format3_s, ric_event_trigger_condition_id_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_ind_hdr_format3_s, ue_id),
     (int32_t)offsetof(e2sm_rc_ind_hdr_format3_s, ue_id_present),
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_hdr_format3_s_names[] = {
    {"ric-eventTriggerCondition-ID", nullptr},
    {"ueID", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_ind_hdr_format3_s_opts[] = {
    {0, (int32_t)offsetof(e2sm_rc_ind_hdr_format3_s, ric_event_trigger_condition_id_present)},
    {1, (int32_t)offsetof(e2sm_rc_ind_hdr_format3_s, ue_id_present)}};
const asn1::table::seq_desc e2sm_rc_ind_hdr_format3_s::desc = {{seq_table_e2sm_rc_ind_hdr_format3_s_fields,
                                                                uint64_t{0x0},
                                                                seq_table_e2sm_rc_ind_hdr_format3_s_opts,
                                                                seq_table_e2sm_rc_ind_hdr_format3_s_names,
                                                                2,
                                                                2},
                                                               true,
                                                               true,
                                                               nullptr,
                                                               0};

// E2SM-RC-IndicationHeader ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_hdr_s_fields[] = {
    {offsetof(e2sm_rc_ind_hdr_s, ric_ind_hdr_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_hdr_s_names[] = {{"ric-indicationHeader-formats", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ind_hdr_s::desc             = {
    {seq_table_e2sm_rc_ind_hdr_s_fields, uint64_t{0x1}, nullptr, seq_table_e2sm_rc_ind_hdr_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::storage_ops* const choice_storage_e2sm_rc_ind_hdr_s_ric_ind_hdr_formats_c__alternatives[] = {
    &asn1::table::alternative_ops<e2sm_rc_ind_hdr_format1_s, false>::value,
    &asn1::table::alternative_ops<e2sm_rc_ind_hdr_format2_s, false>::value,
    &asn1::table::alternative_ops<e2sm_rc_ind_hdr_format3_s, false>::value};
const asn1::table::choice_storage_desc e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::storage_desc = {
    choice_storage_e2sm_rc_ind_hdr_s_ric_ind_hdr_formats_c__alternatives,
    3,
    "e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_"};

e2sm_rc_ind_hdr_format1_s& e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::set_ind_hdr_format1()
{
  set(types::ind_hdr_format1);
  return c.get<e2sm_rc_ind_hdr_format1_s>();
}
e2sm_rc_ind_hdr_format2_s& e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::set_ind_hdr_format2()
{
  set(types::ind_hdr_format2);
  return c.get<e2sm_rc_ind_hdr_format2_s>();
}
e2sm_rc_ind_hdr_format3_s& e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::set_ind_hdr_format3()
{
  set(types::ind_hdr_format3);
  return c.get<e2sm_rc_ind_hdr_format3_s>();
}
static const asn1::table::seq_field choice_table_e2sm_rc_ind_hdr_s_ric_ind_hdr_formats_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ind_hdr_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ind_hdr_format2_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ind_hdr_format3_s::desc}};
static const asn1::table::field_name choice_table_e2sm_rc_ind_hdr_s_ric_ind_hdr_formats_c__names[] = {
    {"indicationHeader-Format1", nullptr},
    {"indicationHeader-Format2", nullptr},
    {"indicationHeader-Format3", nullptr}};
const asn1::table::choice_desc e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::types>(),
    e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::types::nof_types,
    e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::types::nof_exts,
    e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_rc_ind_hdr_s_ric_ind_hdr_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_rc_ind_hdr_s_ric_ind_hdr_formats_c__names,
    "e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_",
    offsetof(e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_, c.type),
    offsetof(e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_, c.value),
    false,
    &e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::storage_desc};

static const char* enum_names_e2sm_rc_ind_hdr_s_ric_ind_hdr_formats_c__types_values[]    = {"indicationHeader-Format1",
                                                                                            "indicationHeader-Format2",
                                                                                            "indicationHeader-Format3"};
const asn1::table::enum_desc e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::types_opts::desc = {
    enum_names_e2sm_rc_ind_hdr_s_ric_ind_hdr_formats_c__types_values,
    3,
    "e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::types"};
uint8_t e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3};
  return map_enum_number(numbers, 3, value, "e2sm_rc_ind_hdr_s::ric_ind_hdr_formats_c_::types");
}

// E2SM-RC-IndicationMessage-Format1-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format1_item_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format1_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_ind_msg_format1_item_s, ran_param_value_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_rc_ind_msg_format1_item_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format1_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
const asn1::table::seq_desc e2sm_rc_ind_msg_format1_item_s::desc = {{seq_table_e2sm_rc_ind_msg_format1_item_s_fields,
                                                                     uint64_t{0x3},
                                                                     nullptr,
                                                                     seq_table_e2sm_rc_ind_msg_format1_item_s_names,
                                                                     2,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// E2SM-RC-IndicationMessage-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format1_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format1_s, ran_p_reported_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ind_msg_format1_s::ran_p_reported_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ind_msg_format1_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format1_s_names[] = {{"ranP-Reported-List", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ind_msg_format1_s::desc = {{seq_table_e2sm_rc_ind_msg_format1_s_fields,
                                                                         uint64_t{0x1},
                                                                         nullptr,
                                                                         seq_table_e2sm_rc_ind_msg_format1_s_names,
                                                                         1,
                                                                         0},
                                                                        true,
                                                                        true,
                                                                        nullptr,
                                                                        0};

// E2SM-RC-IndicationMessage-Format2-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format2_ran_param_item_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format2_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_ind_msg_format2_ran_param_item_s, ran_param_value_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_rc_ind_msg_format2_ran_param_item_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format2_ran_param_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
const asn1::table::seq_desc e2sm_rc_ind_msg_format2_ran_param_item_s::desc = {
    {seq_table_e2sm_rc_ind_msg_format2_ran_param_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_ind_msg_format2_ran_param_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-IndicationMessage-Format2-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format2_item_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format2_item_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc},
    {offsetof(e2sm_rc_ind_msg_format2_item_s, ran_p_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ind_msg_format2_item_s::ran_p_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ind_msg_format2_ran_param_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format2_item_s_names[] = {{"ueID", nullptr},
                                                                                         {"ranP-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_ind_msg_format2_item_s::desc = {{seq_table_e2sm_rc_ind_msg_format2_item_s_fields,
                                                                     uint64_t{0x3},
                                                                     nullptr,
                                                                     seq_table_e2sm_rc_ind_msg_format2_item_s_names,
                                                                     2,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// E2SM-RC-IndicationMessage-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format2_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format2_s, ue_param_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ind_msg_format2_s::ue_param_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ind_msg_format2_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format2_s_names[] = {{"ueParameter-List", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ind_msg_format2_s::desc = {{seq_table_e2sm_rc_ind_msg_format2_s_fields,
                                                                         uint64_t{0x1},
                                                                         nullptr,
                                                                         seq_table_e2sm_rc_ind_msg_format2_s_names,
                                                                         1,
                                                                         0},
                                                                        true,
                                                                        true,
                                                                        nullptr,
                                                                        0};

// NeighbourCell-Item-Choice-NR ::= SEQUENCE
static const asn1::table::seq_field seq_table_neighbour_cell_item_choice_nr_s_fields[] = {
    {offsetof(neighbour_cell_item_choice_nr_s, nr_cgi),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_cgi_s::desc},
    {offsetof(neighbour_cell_item_choice_nr_s, nr_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{1007},
     nullptr},
    {offsetof(neighbour_cell_item_choice_nr_s, five_gs_tac),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(neighbour_cell_item_choice_nr_s::five_gs_tac),
                                asn1::table::string_json>::value},
    {offsetof(neighbour_cell_item_choice_nr_s, nr_mode_info),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(neighbour_cell_item_choice_nr_s::nr_mode_info)>(),
     decltype(neighbour_cell_item_choice_nr_s::nr_mode_info)::has_ext,
     false,
     decltype(neighbour_cell_item_choice_nr_s::nr_mode_info)::nof_types,
     decltype(neighbour_cell_item_choice_nr_s::nr_mode_info)::nof_exts,
     nullptr,
     &neighbour_cell_item_choice_nr_s::nr_mode_info_opts::desc},
    {offsetof(neighbour_cell_item_choice_nr_s, nr_freq_info),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &nr_freq_info_s::desc},
    {offsetof(neighbour_cell_item_choice_nr_s, x2_xn_established),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(neighbour_cell_item_choice_nr_s::x2_xn_established)>(),
     decltype(neighbour_cell_item_choice_nr_s::x2_xn_established)::has_ext,
     false,
     decltype(neighbour_cell_item_choice_nr_s::x2_xn_established)::nof_types,
     decltype(neighbour_cell_item_choice_nr_s::x2_xn_established)::nof_exts,
     nullptr,
     &neighbour_cell_item_choice_nr_s::x2_xn_established_opts::desc},
    {offsetof(neighbour_cell_item_choice_nr_s, ho_validated),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(neighbour_cell_item_choice_nr_s::ho_validated)>(),
     decltype(neighbour_cell_item_choice_nr_s::ho_validated)::has_ext,
     false,
     decltype(neighbour_cell_item_choice_nr_s::ho_validated)::nof_types,
     decltype(neighbour_cell_item_choice_nr_s::ho_validated)::nof_exts,
     nullptr,
     &neighbour_cell_item_choice_nr_s::ho_validated_opts::desc},
    {offsetof(neighbour_cell_item_choice_nr_s, version),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr}};
static const asn1::table::field_name seq_table_neighbour_cell_item_choice_nr_s_names[] = {
    {"nR-CGI", nullptr},
    {"nR-PCI", nullptr},
    {"fiveGS-TAC", nullptr},
    {"nR-mode-info", nullptr},
    {"nR-FreqInfo", nullptr},
    {"x2-Xn-established", nullptr},
    {"hO-validated", nullptr},
    {"version", nullptr}};
const asn1::table::seq_desc neighbour_cell_item_choice_nr_s::desc = {{seq_table_neighbour_cell_item_choice_nr_s_fields,
                                                                      uint64_t{0xff},
                                                                      nullptr,
                                                                      seq_table_neighbour_cell_item_choice_nr_s_names,
                                                                      8,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

static const char*           enum_names_neighbour_cell_item_choice_nr_s_nr_mode_info_e__values[] = {"fdd", "tdd"};
const asn1::table::enum_desc neighbour_cell_item_choice_nr_s::nr_mode_info_opts::desc            = {
    enum_names_neighbour_cell_item_choice_nr_s_nr_mode_info_e__values,
    2,
    "neighbour_cell_item_choice_nr_s::nr_mode_info_e_"};

static const char* enum_names_neighbour_cell_item_choice_nr_s_x2_xn_established_e__values[] = {"true", "false"};
const asn1::table::enum_desc neighbour_cell_item_choice_nr_s::x2_xn_established_opts::desc  = {
    enum_names_neighbour_cell_item_choice_nr_s_x2_xn_established_e__values,
    2,
    "neighbour_cell_item_choice_nr_s::x2_xn_established_e_"};

static const char*           enum_names_neighbour_cell_item_choice_nr_s_ho_validated_e__values[] = {"true", "false"};
const asn1::table::enum_desc neighbour_cell_item_choice_nr_s::ho_validated_opts::desc            = {
    enum_names_neighbour_cell_item_choice_nr_s_ho_validated_e__values,
    2,
    "neighbour_cell_item_choice_nr_s::ho_validated_e_"};

// NeighbourCell-Item-Choice-E-UTRA ::= SEQUENCE
static const asn1::table::seq_field seq_table_neighbour_cell_item_choice_e_utra_s_fields[] = {
    {offsetof(neighbour_cell_item_choice_e_utra_s, eutra_cgi),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &eutra_cgi_s::desc},
    {offsetof(neighbour_cell_item_choice_e_utra_s, eutra_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     true,
     true,
     int64_t{0},
     uint64_t{503},
     nullptr},
    {offsetof(neighbour_cell_item_choice_e_utra_s, eutra_arfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     true,
     int64_t{0},
     uint64_t{65535},
     nullptr},
    {offsetof(neighbour_cell_item_choice_e_utra_s, eutra_tac),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(neighbour_cell_item_choice_e_utra_s::eutra_tac),
                                asn1::table::string_json>::value},
    {offsetof(neighbour_cell_item_choice_e_utra_s, x2_xn_established),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(neighbour_cell_item_choice_e_utra_s::x2_xn_established)>(),
     decltype(neighbour_cell_item_choice_e_utra_s::x2_xn_established)::has_ext,
     false,
     decltype(neighbour_cell_item_choice_e_utra_s::x2_xn_established)::nof_types,
     decltype(neighbour_cell_item_choice_e_utra_s::x2_xn_established)::nof_exts,
     nullptr,
     &neighbour_cell_item_choice_e_utra_s::x2_xn_established_opts::desc},
    {offsetof(neighbour_cell_item_choice_e_utra_s, ho_validated),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(neighbour_cell_item_choice_e_utra_s::ho_validated)>(),
     decltype(neighbour_cell_item_choice_e_utra_s::ho_validated)::has_ext,
     false,
     decltype(neighbour_cell_item_choice_e_utra_s::ho_validated)::nof_types,
     decltype(neighbour_cell_item_choice_e_utra_s::ho_validated)::nof_exts,
     nullptr,
     &neighbour_cell_item_choice_e_utra_s::ho_validated_opts::desc},
    {offsetof(neighbour_cell_item_choice_e_utra_s, version),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr}};
static const asn1::table::field_name seq_table_neighbour_cell_item_choice_e_utra_s_names[] = {
    {"eUTRA-CGI", nullptr},
    {"eUTRA-PCI", nullptr},
    {"eUTRA-ARFCN", nullptr},
    {"eUTRA-TAC", nullptr},
    {"x2-Xn-established", nullptr},
    {"hO-validated", nullptr},
    {"version", nullptr}};
const asn1::table::seq_desc neighbour_cell_item_choice_e_utra_s::desc = {
    {seq_table_neighbour_cell_item_choice_e_utra_s_fields,
     uint64_t{0x7f},
     nullptr,
     seq_table_neighbour_cell_item_choice_e_utra_s_names,
     7,
     0},
    true,
    true,
    nullptr,
    0};

static const char* enum_names_neighbour_cell_item_choice_e_utra_s_x2_xn_established_e__values[] = {"true", "false"};
const asn1::table::enum_desc neighbour_cell_item_choice_e_utra_s::x2_xn_established_opts::desc  = {
    enum_names_neighbour_cell_item_choice_e_utra_s_x2_xn_established_e__values,
    2,
    "neighbour_cell_item_choice_e_utra_s::x2_xn_established_e_"};

static const char* enum_names_neighbour_cell_item_choice_e_utra_s_ho_validated_e__values[] = {"true", "false"};
const asn1::table::enum_desc neighbour_cell_item_choice_e_utra_s::ho_validated_opts::desc  = {
    enum_names_neighbour_cell_item_choice_e_utra_s_ho_validated_e__values,
    2,
    "neighbour_cell_item_choice_e_utra_s::ho_validated_e_"};

// NeighbourCell-Item ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_neighbour_cell_item_c_alternatives[] = {
    &asn1::table::alternative_ops<neighbour_cell_item_choice_nr_s, false>::value,
    &asn1::table::alternative_ops<neighbour_cell_item_choice_e_utra_s, false>::value};
const asn1::table::choice_storage_desc neighbour_cell_item_c::storage_desc = {
    choice_storage_neighbour_cell_item_c_alternatives,
    2,
    "neighbour_cell_item_c"};

neighbour_cell_item_choice_nr_s& neighbour_cell_item_c::set_ran_type_choice_nr()
{
  set(types::ran_type_choice_nr);
  return c.get<neighbour_cell_item_choice_nr_s>();
}
neighbour_cell_item_choice_e_utra_s& neighbour_cell_item_c::set_ran_type_choice_eutra()
{
  set(types::ran_type_choice_eutra);
  return c.get<neighbour_cell_item_choice_e_utra_s>();
}
static const asn1::table::seq_field choice_table_neighbour_cell_item_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &neighbour_cell_item_choice_nr_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &neighbour_cell_item_choice_e_utra_s::desc}};
static const asn1::table::field_name choice_table_neighbour_cell_item_c_names[] = {{"ranType-Choice-NR", nullptr},
                                                                                   {"ranType-Choice-EUTRA", nullptr}};
const asn1::table::choice_desc       neighbour_cell_item_c::desc                = {
    asn1::table::enum_int_type<neighbour_cell_item_c::types>(),
    neighbour_cell_item_c::types::nof_types,
    neighbour_cell_item_c::types::nof_exts,
    neighbour_cell_item_c::types::has_ext,
    true,
    choice_table_neighbour_cell_item_c_alternatives,
    nullptr,
    choice_table_neighbour_cell_item_c_names,
    "neighbour_cell_item_c",
    offsetof(neighbour_cell_item_c, c.type),
    offsetof(neighbour_cell_item_c, c.value),
    false,
    &neighbour_cell_item_c::storage_desc};

static const char* enum_names_neighbour_cell_item_c_types_values[]   = {"ranType-Choice-NR", "ranType-Choice-EUTRA"};
const asn1::table::enum_desc neighbour_cell_item_c::types_opts::desc = {enum_names_neighbour_cell_item_c_types_values,
                                                                        2,
                                                                        "neighbour_cell_item_c::types"};

// NeighbourRelation-Info ::= SEQUENCE
static const asn1::table::seq_field seq_table_neighbour_relation_info_s_fields[] = {
    {offsetof(neighbour_relation_info_s, serving_cell_pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &serving_cell_pci_c::desc},
    {offsetof(neighbour_relation_info_s, serving_cell_arfcn),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &serving_cell_arfcn_c::desc},
    {offsetof(neighbour_relation_info_s, neighbour_cell_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(neighbour_relation_info_s::neighbour_cell_list),
                                   asn1::table::choice_item_ops,
                                   false,
                                   true>::value,
     &neighbour_cell_item_c::desc}};
static const asn1::table::field_name seq_table_neighbour_relation_info_s_names[] = {{"servingCellPCI", nullptr},
                                                                                    {"servingCellARFCN", nullptr},
                                                                                    {"neighbourCell-List", nullptr}};
const asn1::table::seq_desc          neighbour_relation_info_s::desc = {{seq_table_neighbour_relation_info_s_fields,
                                                                         uint64_t{0x7},
                                                                         nullptr,
                                                                         seq_table_neighbour_relation_info_s_names,
                                                                         3,
                                                                         0},
                                                                        true,
                                                                        true,
                                                                        nullptr,
                                                                        0};

// ServingCellConfigCommon ::= SEQUENCE
static const asn1::table::seq_field seq_table_serving_cell_cfg_common_s_fields[] = {
    {offsetof(serving_cell_cfg_common_s, ssb_positions_in_burst),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(serving_cell_cfg_common_s::ssb_positions_in_burst),
                                asn1::table::string_json>::value},
    {offsetof(serving_cell_cfg_common_s, ssb_periodicity),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(serving_cell_cfg_common_s::ssb_periodicity)>(),
     decltype(serving_cell_cfg_common_s::ssb_periodicity)::has_ext,
     false,
     decltype(serving_cell_cfg_common_s::ssb_periodicity)::nof_types,
     decltype(serving_cell_cfg_common_s::ssb_periodicity)::nof_exts,
     nullptr,
     &serving_cell_cfg_common_s::ssb_periodicity_opts::desc},
    {offsetof(serving_cell_cfg_common_s, ssb_subcarrier_spacing),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(serving_cell_cfg_common_s::ssb_subcarrier_spacing)>(),
     decltype(serving_cell_cfg_common_s::ssb_subcarrier_spacing)::has_ext,
     false,
     decltype(serving_cell_cfg_common_s::ssb_subcarrier_spacing)::nof_types,
     decltype(serving_cell_cfg_common_s::ssb_subcarrier_spacing)::nof_exts,
     nullptr,
     &serving_cell_cfg_common_s::ssb_subcarrier_spacing_opts::desc}};
static const asn1::table::field_name seq_table_serving_cell_cfg_common_s_names[] = {{"ssbPositionsInBurst", nullptr},
                                                                                    {"ssbPeriodicity", nullptr},
                                                                                    {"ssbSubcarrierSpacing", nullptr}};
const asn1::table::seq_desc          serving_cell_cfg_common_s::desc = {{seq_table_serving_cell_cfg_common_s_fields,
                                                                         uint64_t{0x7},
                                                                         nullptr,
                                                                         seq_table_serving_cell_cfg_common_s_names,
                                                                         3,
                                                                         0},
                                                                        true,
                                                                        true,
                                                                        nullptr,
                                                                        0};

static const char* enum_names_serving_cell_cfg_common_s_ssb_periodicity_e__values[] =
    {"ms5", "ms10", "ms20", "ms40", "ms80", "ms160"};
const asn1::table::enum_desc serving_cell_cfg_common_s::ssb_periodicity_opts::desc = {
    enum_names_serving_cell_cfg_common_s_ssb_periodicity_e__values,
    6,
    "serving_cell_cfg_common_s::ssb_periodicity_e_"};
uint8_t serving_cell_cfg_common_s::ssb_periodicity_opts::to_number() const
{
  static const uint8_t numbers[] = {5, 10, 20, 40, 80, 160};
  return map_enum_number(numbers, 6, value, "serving_cell_cfg_common_s::ssb_periodicity_e_");
}

static const char* enum_names_serving_cell_cfg_common_s_ssb_subcarrier_spacing_e__values[] =
    {"kHz15", "kHz30", "kHz60", "kHz120", "kHz240", "kHz480-v1700", "kHz960-v1700"};
const asn1::table::enum_desc serving_cell_cfg_common_s::ssb_subcarrier_spacing_opts::desc = {
    enum_names_serving_cell_cfg_common_s_ssb_subcarrier_spacing_e__values,
    7,
    "serving_cell_cfg_common_s::ssb_subcarrier_spacing_e_"};
uint16_t serving_cell_cfg_common_s::ssb_subcarrier_spacing_opts::to_number() const
{
  static const uint16_t numbers[] = {15, 30, 60, 120, 240, 480, 960};
  return map_enum_number(numbers, 7, value, "serving_cell_cfg_common_s::ssb_subcarrier_spacing_e_");
}

// E2SM-RC-IndicationMessage-Format3-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format3_item_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format3_item_s, cell_global_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_c::desc},
    {offsetof(e2sm_rc_ind_msg_format3_item_s, cell_context_info),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(e2sm_rc_ind_msg_format3_item_s::cell_context_info),
                                      asn1::table::string_json>::value},
    {offsetof(e2sm_rc_ind_msg_format3_item_s, cell_deleted),
     (int32_t)offsetof(e2sm_rc_ind_msg_format3_item_s, cell_deleted_present),
     asn1::table::field_op::boolean,
     false,
     false,
     0,
     0,
     nullptr},
    {offsetof(e2sm_rc_ind_msg_format3_item_s, neighbour_relation_table),
     (int32_t)offsetof(e2sm_rc_ind_msg_format3_item_s, neighbour_relation_table_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &neighbour_relation_info_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format3_item_s_names[] = {
    {"cellGlobal-ID", nullptr},
    {"cellContextInfo", nullptr},
    {"cellDeleted", nullptr},
    {"neighbourRelation-Table", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_ind_msg_format3_item_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(e2sm_rc_ind_msg_format3_item_s, cell_deleted_present)},
    {3, (int32_t)offsetof(e2sm_rc_ind_msg_format3_item_s, neighbour_relation_table_present)}};
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format3_item_s_g0_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format3_item_s, mib),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::sized_object_codec<decltype(e2sm_rc_ind_msg_format3_item_s::mib), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format3_item_s_g0_names[] = {{"mib", nullptr}};
static const asn1::table::seq_opt    seq_table_e2sm_rc_ind_msg_format3_item_s_g0_opts[]  = {
    {0, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format3_item_s_g1_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format3_item_s, serving_cell_cfg_common),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(e2sm_rc_ind_msg_format3_item_s::serving_cell_cfg_common)>::value,
     &serving_cell_cfg_common_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format3_item_s_g1_names[] = {
    {"servingCellConfigCommon", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_ind_msg_format3_item_s_g1_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_e2sm_rc_ind_msg_format3_item_s_groups[] = {
    {{seq_table_e2sm_rc_ind_msg_format3_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_e2sm_rc_ind_msg_format3_item_s_g0_opts,
      seq_table_e2sm_rc_ind_msg_format3_item_s_g0_names,
      1,
      1},
     true},
    {{seq_table_e2sm_rc_ind_msg_format3_item_s_g1_fields,
      uint64_t{0x0},
      seq_table_e2sm_rc_ind_msg_format3_item_s_g1_opts,
      seq_table_e2sm_rc_ind_msg_format3_item_s_g1_names,
      1,
      1},
     true}};
const asn1::table::seq_desc e2sm_rc_ind_msg_format3_item_s::desc = {{seq_table_e2sm_rc_ind_msg_format3_item_s_fields,
                                                                     uint64_t{0x1},
                                                                     seq_table_e2sm_rc_ind_msg_format3_item_s_opts,
                                                                     seq_table_e2sm_rc_ind_msg_format3_item_s_names,
                                                                     4,
                                                                     3},
                                                                    true,
                                                                    true,
                                                                    seq_table_e2sm_rc_ind_msg_format3_item_s_groups,
                                                                    2};

// E2SM-RC-IndicationMessage-Format3 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format3_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format3_s, cell_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ind_msg_format3_s::cell_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ind_msg_format3_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format3_s_names[] = {{"cellInfo-List", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ind_msg_format3_s::desc = {{seq_table_e2sm_rc_ind_msg_format3_s_fields,
                                                                         uint64_t{0x1},
                                                                         nullptr,
                                                                         seq_table_e2sm_rc_ind_msg_format3_s_names,
                                                                         1,
                                                                         0},
                                                                        true,
                                                                        true,
                                                                        nullptr,
                                                                        0};

// E2SM-RC-IndicationMessage-Format5-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format5_item_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format5_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_ind_msg_format5_item_s, ran_param_value_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_rc_ind_msg_format5_item_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format5_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
const asn1::table::seq_desc e2sm_rc_ind_msg_format5_item_s::desc = {{seq_table_e2sm_rc_ind_msg_format5_item_s_fields,
                                                                     uint64_t{0x3},
                                                                     nullptr,
                                                                     seq_table_e2sm_rc_ind_msg_format5_item_s_names,
                                                                     2,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// E2SM-RC-IndicationMessage-Format5 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format5_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format5_s, ran_p_requested_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     0,
     65535,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ind_msg_format5_s::ran_p_requested_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ind_msg_format5_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format5_s_names[] = {{"ranP-Requested-List", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ind_msg_format5_s::desc = {{seq_table_e2sm_rc_ind_msg_format5_s_fields,
                                                                         uint64_t{0x1},
                                                                         nullptr,
                                                                         seq_table_e2sm_rc_ind_msg_format5_s_names,
                                                                         1,
                                                                         0},
                                                                        true,
                                                                        true,
                                                                        nullptr,
                                                                        0};

// E2SM-RC-IndicationMessage-Format6-RANP-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format6_ranp_item_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format6_ranp_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_ind_msg_format6_ranp_item_s, ran_param_value_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_rc_ind_msg_format6_ranp_item_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format6_ranp_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
const asn1::table::seq_desc e2sm_rc_ind_msg_format6_ranp_item_s::desc = {
    {seq_table_e2sm_rc_ind_msg_format6_ranp_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_ind_msg_format6_ranp_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-IndicationMessage-Format6-Indication-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format6_ind_item_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format6_ind_item_s, ric_insert_ind_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(e2sm_rc_ind_msg_format6_ind_item_s, ran_p_insert_ind_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     0,
     65535,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ind_msg_format6_ind_item_s::ran_p_insert_ind_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ind_msg_format6_ranp_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format6_ind_item_s_names[] = {
    {"ric-InsertIndication-ID", nullptr},
    {"ranP-InsertIndication-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_ind_msg_format6_ind_item_s::desc = {
    {seq_table_e2sm_rc_ind_msg_format6_ind_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_ind_msg_format6_ind_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-IndicationMessage-Format6-Style-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format6_style_item_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format6_style_item_s, indicated_insert_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(e2sm_rc_ind_msg_format6_style_item_s, ric_insert_ind_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ind_msg_format6_style_item_s::ric_insert_ind_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ind_msg_format6_ind_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format6_style_item_s_names[] = {
    {"indicated-Insert-Style-Type", nullptr},
    {"ric-InsertIndication-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_ind_msg_format6_style_item_s::desc = {
    {seq_table_e2sm_rc_ind_msg_format6_style_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_e2sm_rc_ind_msg_format6_style_item_s_names,
     2,
     0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-IndicationMessage-Format6 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_format6_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_format6_s, ric_insert_style_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(e2sm_rc_ind_msg_format6_s::ric_insert_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_ind_msg_format6_style_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_format6_s_names[] = {{"ric-InsertStyle-List", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ind_msg_format6_s::desc = {{seq_table_e2sm_rc_ind_msg_format6_s_fields,
                                                                         uint64_t{0x1},
                                                                         nullptr,
                                                                         seq_table_e2sm_rc_ind_msg_format6_s_names,
                                                                         1,
                                                                         0},
                                                                        true,
                                                                        true,
                                                                        nullptr,
                                                                        0};

// E2SM-RC-IndicationMessage ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ind_msg_s_fields[] = {
    {offsetof(e2sm_rc_ind_msg_s, ric_ind_msg_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ind_msg_s_names[] = {{"ric-indicationMessage-formats", nullptr}};
const asn1::table::seq_desc          e2sm_rc_ind_msg_s::desc             = {
    {seq_table_e2sm_rc_ind_msg_s_fields, uint64_t{0x1}, nullptr, seq_table_e2sm_rc_ind_msg_s_names, 1, 0},
    true,
    true,
    nullptr,
    0,
    "E2SM-RC-IndicationMessage"};

static const asn1::table::storage_ops* const choice_storage_e2sm_rc_ind_msg_s_ric_ind_msg_formats_c__alternatives[] = {
    &asn1::table::alternative_ops<e2sm_rc_ind_msg_format1_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_rc_ind_msg_format2_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_rc_ind_msg_format3_s, false>::value,
    nullptr,
    &asn1::table::alternative_ops<e2sm_rc_ind_msg_format5_s, false, false>::value,
    &asn1::table::alternative_ops<e2sm_rc_ind_msg_format6_s, false, false>::value};
const asn1::table::choice_storage_desc e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::storage_desc = {
    choice_storage_e2sm_rc_ind_msg_s_ric_ind_msg_formats_c__alternatives,
    6,
    "e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_"};

e2sm_rc_ind_msg_format1_s& e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::set_ind_msg_format1()
{
  set(types::ind_msg_format1);
  return c.get<e2sm_rc_ind_msg_format1_s>();
}
e2sm_rc_ind_msg_format2_s& e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::set_ind_msg_format2()
{
  set(types::ind_msg_format2);
  return c.get<e2sm_rc_ind_msg_format2_s>();
}
e2sm_rc_ind_msg_format3_s& e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::set_ind_msg_format3()
{
  set(types::ind_msg_format3);
  return c.get<e2sm_rc_ind_msg_format3_s>();
}
void e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::set_ind_msg_format4()
{
  set(types::ind_msg_format4);
}
e2sm_rc_ind_msg_format5_s& e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::set_ind_msg_format5()
{
  set(types::ind_msg_format5);
  return c.get<e2sm_rc_ind_msg_format5_s>();
}
e2sm_rc_ind_msg_format6_s& e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::set_ind_msg_format6()
{
  set(types::ind_msg_format6);
  return c.get<e2sm_rc_ind_msg_format6_s>();
}
static const asn1::table::seq_field choice_table_e2sm_rc_ind_msg_s_ric_ind_msg_formats_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ind_msg_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ind_msg_format2_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ind_msg_format3_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ind_msg_format5_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_ind_msg_format6_s::desc}};
static const asn1::table::field_name choice_table_e2sm_rc_ind_msg_s_ric_ind_msg_formats_c__names[] = {
    {"indicationMessage-Format1", nullptr},
    {"indicationMessage-Format2", nullptr},
    {"indicationMessage-Format3", nullptr},
    {"indicationMessage-Format4", nullptr},
    {"indicationMessage-Format5", nullptr},
    {"indicationMessage-Format6", nullptr}};
const asn1::table::choice_desc e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::types>(),
    e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::types::nof_types,
    e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::types::nof_exts,
    e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_rc_ind_msg_s_ric_ind_msg_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_rc_ind_msg_s_ric_ind_msg_formats_c__names,
    "e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_",
    offsetof(e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_, c.type),
    offsetof(e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_, c.value),
    false,
    &e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::storage_desc};

static const char* enum_names_e2sm_rc_ind_msg_s_ric_ind_msg_formats_c__types_values[]    = {"indicationMessage-Format1",
                                                                                            "indicationMessage-Format2",
                                                                                            "indicationMessage-Format3",
                                                                                            "indicationMessage-Format4",
                                                                                            "indicationMessage-Format5",
                                                                                            "indicationMessage-Format6"};
const asn1::table::enum_desc e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::types_opts::desc = {
    enum_names_e2sm_rc_ind_msg_s_ric_ind_msg_formats_c__types_values,
    6,
    "e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::types"};
uint8_t e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2, 3, 4, 5, 6};
  return map_enum_number(numbers, 6, value, "e2sm_rc_ind_msg_s::ric_ind_msg_formats_c_::types");
}

// E2SM-RC-QueryDefinition-Format1-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_definition_format1_item_s_fields[] = {
    {offsetof(e2sm_rc_query_definition_format1_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_query_definition_format1_item_s, ran_param_definition),
     (int32_t)offsetof(e2sm_rc_query_definition_format1_item_s, ran_param_definition_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_query_definition_format1_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_query_definition_format1_item_s_opts[] = {
    {1, (int32_t)offsetof(e2sm_rc_query_definition_format1_item_s, ran_param_definition_present)}};
const asn1::table::seq_desc e2sm_rc_query_definition_format1_item_s::desc = {
    {seq_table_e2sm_rc_query_definition_format1_item_s_fields,
     uint64_t{0x1},
     seq_table_e2sm_rc_query_definition_format1_item_s_opts,
     seq_table_e2sm_rc_query_definition_format1_item_s_names,
     2,
     1},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-QueryDefinition-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_definition_format1_s_fields[] = {
    {offsetof(e2sm_rc_query_definition_format1_s, ran_p_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_query_definition_format1_s::ran_p_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_query_definition_format1_item_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_query_definition_format1_s_names[] = {{"ranP-List", nullptr}};
const asn1::table::seq_desc          e2sm_rc_query_definition_format1_s::desc             = {
    {seq_table_e2sm_rc_query_definition_format1_s_fields,
                          uint64_t{0x1},
                          nullptr,
                          seq_table_e2sm_rc_query_definition_format1_s_names,
                          1,
                          0},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-QueryDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_definition_s_fields[] = {
    {offsetof(e2sm_rc_query_definition_s, ric_query_definition_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_query_definition_s::ric_query_definition_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_query_definition_s_names[] = {
    {"ric-queryDefinition-formats", nullptr}};
const asn1::table::seq_desc e2sm_rc_query_definition_s::desc = {{seq_table_e2sm_rc_query_definition_s_fields,
                                                                 uint64_t{0x1},
                                                                 nullptr,
                                                                 seq_table_e2sm_rc_query_definition_s_names,
                                                                 1,
                                                                 0},
                                                                true,
                                                                true,
                                                                nullptr,
                                                                0};

static const asn1::table::seq_field
    choice_table_e2sm_rc_query_definition_s_ric_query_definition_formats_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &e2sm_rc_query_definition_format1_s::desc}};
static const asn1::table::field_name choice_table_e2sm_rc_query_definition_s_ric_query_definition_formats_c__names[] = {
    {"queryRequest-Format1", nullptr}};
const asn1::table::choice_desc e2sm_rc_query_definition_s::ric_query_definition_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_rc_query_definition_s::ric_query_definition_formats_c_::types>(),
    e2sm_rc_query_definition_s::ric_query_definition_formats_c_::types::nof_types,
    e2sm_rc_query_definition_s::ric_query_definition_formats_c_::types::nof_exts,
    e2sm_rc_query_definition_s::ric_query_definition_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_rc_query_definition_s_ric_query_definition_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_rc_query_definition_s_ric_query_definition_formats_c__names,
    "e2sm_rc_query_definition_s::ric_query_definition_formats_c_",
    asn1::table::choice_desc::no_member,
    offsetof(e2sm_rc_query_definition_s::ric_query_definition_formats_c_, c),
    false};

static const char* enum_names_e2sm_rc_query_definition_s_ric_query_definition_formats_c__types_values[] = {
    "queryRequest-Format1"};
const asn1::table::enum_desc e2sm_rc_query_definition_s::ric_query_definition_formats_c_::types_opts::desc = {
    enum_names_e2sm_rc_query_definition_s_ric_query_definition_formats_c__types_values,
    1,
    "e2sm_rc_query_definition_s::ric_query_definition_formats_c_::types"};
uint8_t e2sm_rc_query_definition_s::ric_query_definition_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "e2sm_rc_query_definition_s::ric_query_definition_formats_c_::types");
}

// E2SM-RC-QueryHeader-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_hdr_format1_s_fields[] = {
    {offsetof(e2sm_rc_query_hdr_format1_s, ric_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(e2sm_rc_query_hdr_format1_s, associated_e2_node_info),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::item_list_codec<decltype(e2sm_rc_query_hdr_format1_s::associated_e2_node_info),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_param_testing_item_s::desc},
    {offsetof(e2sm_rc_query_hdr_format1_s, associated_ue_info),
     (int32_t)offsetof(e2sm_rc_query_hdr_format1_s, associated_ue_info_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &associated_ue_info_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_query_hdr_format1_s_names[] = {{"ric-Style-Type", nullptr},
                                                                                      {"associatedE2NodeInfo", nullptr},
                                                                                      {"associatedUEInfo", nullptr}};
static const asn1::table::seq_opt    seq_table_e2sm_rc_query_hdr_format1_s_opts[]  = {
    {1, asn1::table::seq_field::list_presence},
    {2, (int32_t)offsetof(e2sm_rc_query_hdr_format1_s, associated_ue_info_present)}};
const asn1::table::seq_desc e2sm_rc_query_hdr_format1_s::desc = {{seq_table_e2sm_rc_query_hdr_format1_s_fields,
                                                                  uint64_t{0x1},
                                                                  seq_table_e2sm_rc_query_hdr_format1_s_opts,
                                                                  seq_table_e2sm_rc_query_hdr_format1_s_names,
                                                                  3,
                                                                  2},
                                                                 true,
                                                                 true,
                                                                 nullptr,
                                                                 0};

// E2SM-RC-QueryHeader ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_hdr_s_fields[] = {
    {offsetof(e2sm_rc_query_hdr_s, ric_query_hdr_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_query_hdr_s_names[] = {{"ric-queryHeader-formats", nullptr}};
const asn1::table::seq_desc          e2sm_rc_query_hdr_s::desc             = {
    {seq_table_e2sm_rc_query_hdr_s_fields, uint64_t{0x1}, nullptr, seq_table_e2sm_rc_query_hdr_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::seq_field choice_table_e2sm_rc_query_hdr_s_ric_query_hdr_formats_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_query_hdr_format1_s::desc}};
static const asn1::table::field_name choice_table_e2sm_rc_query_hdr_s_ric_query_hdr_formats_c__names[] = {
    {"queryHeader-Format1", nullptr}};
const asn1::table::choice_desc e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_::types>(),
    e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_::types::nof_types,
    e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_::types::nof_exts,
    e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_rc_query_hdr_s_ric_query_hdr_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_rc_query_hdr_s_ric_query_hdr_formats_c__names,
    "e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_",
    asn1::table::choice_desc::no_member,
    offsetof(e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_, c),
    false};

static const char* enum_names_e2sm_rc_query_hdr_s_ric_query_hdr_formats_c__types_values[]    = {"queryHeader-Format1"};
const asn1::table::enum_desc e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_::types_opts::desc = {
    enum_names_e2sm_rc_query_hdr_s_ric_query_hdr_formats_c__types_values,
    1,
    "e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_::types"};
uint8_t e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "e2sm_rc_query_hdr_s::ric_query_hdr_formats_c_::types");
}

// E2SM-RC-QueryOutcome-Format1-ItemParameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_outcome_format1_item_params_s_fields[] = {
    {offsetof(e2sm_rc_query_outcome_format1_item_params_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_query_outcome_format1_item_params_s, ran_param_value_type),
     (int32_t)offsetof(e2sm_rc_query_outcome_format1_item_params_s, ran_param_value_type_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_rc_query_outcome_format1_item_params_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_e2sm_rc_query_outcome_format1_item_params_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_query_outcome_format1_item_params_s_opts[] = {
    {1, (int32_t)offsetof(e2sm_rc_query_outcome_format1_item_params_s, ran_param_value_type_present)}};
const asn1::table::seq_desc e2sm_rc_query_outcome_format1_item_params_s::desc = {
    {seq_table_e2sm_rc_query_outcome_format1_item_params_s_fields,
     uint64_t{0x1},
     seq_table_e2sm_rc_query_outcome_format1_item_params_s_opts,
     seq_table_e2sm_rc_query_outcome_format1_item_params_s_names,
     2,
     1},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-QueryOutcome-Format1-ItemCell ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_outcome_format1_item_cell_s_fields[] = {
    {offsetof(e2sm_rc_query_outcome_format1_item_cell_s, cell_global_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &cgi_c::desc},
    {offsetof(e2sm_rc_query_outcome_format1_item_cell_s, ran_p_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     0,
     65535,
     &asn1::table::item_list_codec<decltype(e2sm_rc_query_outcome_format1_item_cell_s::ran_p_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_query_outcome_format1_item_params_s::desc},
    {offsetof(e2sm_rc_query_outcome_format1_item_cell_s, neighbour_relation_table),
     (int32_t)offsetof(e2sm_rc_query_outcome_format1_item_cell_s, neighbour_relation_table_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &neighbour_relation_info_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_query_outcome_format1_item_cell_s_names[] = {
    {"cellGlobal-ID", nullptr},
    {"ranP-List", nullptr},
    {"neighbourRelation-Table", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_query_outcome_format1_item_cell_s_opts[] = {
    {2, (int32_t)offsetof(e2sm_rc_query_outcome_format1_item_cell_s, neighbour_relation_table_present)}};
static const asn1::table::seq_field seq_table_e2sm_rc_query_outcome_format1_item_cell_s_g0_fields[] = {
    {offsetof(e2sm_rc_query_outcome_format1_item_cell_s, serving_cell_cfg_common),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<
         decltype(e2sm_rc_query_outcome_format1_item_cell_s::serving_cell_cfg_common)>::value,
     &serving_cell_cfg_common_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_query_outcome_format1_item_cell_s_g0_names[] = {
    {"servingCellConfigCommon", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_query_outcome_format1_item_cell_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_e2sm_rc_query_outcome_format1_item_cell_s_groups[] = {
    {{seq_table_e2sm_rc_query_outcome_format1_item_cell_s_g0_fields,
      uint64_t{0x0},
      seq_table_e2sm_rc_query_outcome_format1_item_cell_s_g0_opts,
      seq_table_e2sm_rc_query_outcome_format1_item_cell_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc e2sm_rc_query_outcome_format1_item_cell_s::desc = {
    {seq_table_e2sm_rc_query_outcome_format1_item_cell_s_fields,
     uint64_t{0x3},
     seq_table_e2sm_rc_query_outcome_format1_item_cell_s_opts,
     seq_table_e2sm_rc_query_outcome_format1_item_cell_s_names,
     3,
     1},
    true,
    true,
    seq_table_e2sm_rc_query_outcome_format1_item_cell_s_groups,
    1};

// E2SM-RC-QueryOutcome-Format1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_outcome_format1_s_fields[] = {
    {offsetof(e2sm_rc_query_outcome_format1_s, cell_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(e2sm_rc_query_outcome_format1_s::cell_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_query_outcome_format1_item_cell_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_query_outcome_format1_s_names[] = {{"cellInfo-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_query_outcome_format1_s::desc = {{seq_table_e2sm_rc_query_outcome_format1_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_e2sm_rc_query_outcome_format1_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2SM-RC-QueryOutcome-Format2-ItemParameters ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_outcome_format2_item_params_s_fields[] = {
    {offsetof(e2sm_rc_query_outcome_format2_item_params_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(e2sm_rc_query_outcome_format2_item_params_s, ran_param_value_type),
     (int32_t)offsetof(e2sm_rc_query_outcome_format2_item_params_s, ran_param_value_type_present),
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(e2sm_rc_query_outcome_format2_item_params_s::ran_param_value_type),
                                asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_e2sm_rc_query_outcome_format2_item_params_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-valueType", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_query_outcome_format2_item_params_s_opts[] = {
    {1, (int32_t)offsetof(e2sm_rc_query_outcome_format2_item_params_s, ran_param_value_type_present)}};
const asn1::table::seq_desc e2sm_rc_query_outcome_format2_item_params_s::desc = {
    {seq_table_e2sm_rc_query_outcome_format2_item_params_s_fields,
     uint64_t{0x1},
     seq_table_e2sm_rc_query_outcome_format2_item_params_s_opts,
     seq_table_e2sm_rc_query_outcome_format2_item_params_s_names,
     2,
     1},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-QueryOutcome-Format2-ItemUE ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_outcome_format2_item_ue_s_fields[] = {
    {offsetof(e2sm_rc_query_outcome_format2_item_ue_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ue_id_c::desc},
    {offsetof(e2sm_rc_query_outcome_format2_item_ue_s, ran_p_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     0,
     65535,
     &asn1::table::item_list_codec<decltype(e2sm_rc_query_outcome_format2_item_ue_s::ran_p_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_query_outcome_format2_item_params_s::desc},
    {offsetof(e2sm_rc_query_outcome_format2_item_ue_s, ue_filt_id),
     (int32_t)offsetof(e2sm_rc_query_outcome_format2_item_ue_s, ue_filt_id_present),
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr}};
static const asn1::table::field_name seq_table_e2sm_rc_query_outcome_format2_item_ue_s_names[] = {
    {"ueID", nullptr},
    {"ranP-List", nullptr},
    {"ueFilterID", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_query_outcome_format2_item_ue_s_opts[] = {
    {2, (int32_t)offsetof(e2sm_rc_query_outcome_format2_item_ue_s, ue_filt_id_present)}};
const asn1::table::seq_desc e2sm_rc_query_outcome_format2_item_ue_s::desc = {
    {seq_table_e2sm_rc_query_outcome_format2_item_ue_s_fields,
     uint64_t{0x3},
     seq_table_e2sm_rc_query_outcome_format2_item_ue_s_opts,
     seq_table_e2sm_rc_query_outcome_format2_item_ue_s_names,
     3,
     1},
    true,
    true,
    nullptr,
    0};

// E2SM-RC-QueryOutcome-Format2 ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_outcome_format2_s_fields[] = {
    {offsetof(e2sm_rc_query_outcome_format2_s, ue_info_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     0,
     65535,
     &asn1::table::item_list_codec<decltype(e2sm_rc_query_outcome_format2_s::ue_info_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &e2sm_rc_query_outcome_format2_item_ue_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_query_outcome_format2_s_names[] = {{"ueInfo-List", nullptr}};
const asn1::table::seq_desc e2sm_rc_query_outcome_format2_s::desc = {{seq_table_e2sm_rc_query_outcome_format2_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_e2sm_rc_query_outcome_format2_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2SM-RC-QueryOutcome ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_query_outcome_s_fields[] = {
    {offsetof(e2sm_rc_query_outcome_s, ric_query_outcome_formats),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_query_outcome_s_names[] = {
    {"ric-queryOutcome-formats", nullptr}};
const asn1::table::seq_desc e2sm_rc_query_outcome_s::desc = {
    {seq_table_e2sm_rc_query_outcome_s_fields, uint64_t{0x1}, nullptr, seq_table_e2sm_rc_query_outcome_s_names, 1, 0},
    true,
    true,
    nullptr,
    0};

static const asn1::table::storage_ops* const
    choice_storage_e2sm_rc_query_outcome_s_ric_query_outcome_formats_c__alternatives[] = {
        &asn1::table::alternative_ops<e2sm_rc_query_outcome_format1_s, false, false>::value,
        &asn1::table::alternative_ops<e2sm_rc_query_outcome_format2_s, false, false>::value};
const asn1::table::choice_storage_desc e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::storage_desc = {
    choice_storage_e2sm_rc_query_outcome_s_ric_query_outcome_formats_c__alternatives,
    2,
    "e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_"};

e2sm_rc_query_outcome_format1_s& e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::set_query_outcome_format1()
{
  set(types::query_outcome_format1);
  return c.get<e2sm_rc_query_outcome_format1_s>();
}
e2sm_rc_query_outcome_format2_s& e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::set_query_outcome_format2()
{
  set(types::query_outcome_format2);
  return c.get<e2sm_rc_query_outcome_format2_s>();
}
static const asn1::table::seq_field choice_table_e2sm_rc_query_outcome_s_ric_query_outcome_formats_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_query_outcome_format1_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &e2sm_rc_query_outcome_format2_s::desc}};
static const asn1::table::field_name choice_table_e2sm_rc_query_outcome_s_ric_query_outcome_formats_c__names[] = {
    {"queryOutcome-Format1", nullptr},
    {"queryOutcome-Format2", nullptr}};
const asn1::table::choice_desc e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::desc = {
    asn1::table::enum_int_type<e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::types>(),
    e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::types::nof_types,
    e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::types::nof_exts,
    e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::types::has_ext,
    true,
    choice_table_e2sm_rc_query_outcome_s_ric_query_outcome_formats_c__alternatives,
    nullptr,
    choice_table_e2sm_rc_query_outcome_s_ric_query_outcome_formats_c__names,
    "e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_",
    offsetof(e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_, c.type),
    offsetof(e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_, c.value),
    false,
    &e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::storage_desc};

static const char* enum_names_e2sm_rc_query_outcome_s_ric_query_outcome_formats_c__types_values[] = {
    "queryOutcome-Format1",
    "queryOutcome-Format2"};
const asn1::table::enum_desc e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::types_opts::desc = {
    enum_names_e2sm_rc_query_outcome_s_ric_query_outcome_formats_c__types_values,
    2,
    "e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::types"};
uint8_t e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1, 2};
  return map_enum_number(numbers, 2, value, "e2sm_rc_query_outcome_s::ric_query_outcome_formats_c_::types");
}

// RANFunctionDefinition-EventTrigger-Style-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_event_trigger_style_item_s_fields[] = {
    {offsetof(ran_function_definition_event_trigger_style_item_s, ric_event_trigger_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_event_trigger_style_item_s, ric_event_trigger_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(ran_function_definition_event_trigger_style_item_s::ric_event_trigger_style_name),
         asn1::table::string_json>::value},
    {offsetof(ran_function_definition_event_trigger_style_item_s, ric_event_trigger_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_ran_function_definition_event_trigger_style_item_s_names[] = {
    {"ric-EventTriggerStyle-Type", nullptr},
    {"ric-EventTriggerStyle-Name", nullptr},
    {"ric-EventTriggerFormat-Type", nullptr}};
const asn1::table::seq_desc ran_function_definition_event_trigger_style_item_s::desc = {
    {seq_table_ran_function_definition_event_trigger_style_item_s_fields,
     uint64_t{0x7},
     nullptr,
     seq_table_ran_function_definition_event_trigger_style_item_s_names,
     3,
     0},
    true,
    true,
    nullptr,
    0};

// L2Parameters-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_l2_params_ran_param_item_s_fields[] = {
    {offsetof(l2_params_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(l2_params_ran_param_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(l2_params_ran_param_item_s::ran_param_name),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_l2_params_ran_param_item_s_names[]     = {{"ranParameter-ID", nullptr},
                                                                                         {"ranParameter-name", nullptr}};
static const asn1::table::seq_field  seq_table_l2_params_ran_param_item_s_g0_fields[] = {
    {offsetof(l2_params_ran_param_item_s, ran_param_definition),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(l2_params_ran_param_item_s::ran_param_definition)>::value,
      &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_l2_params_ran_param_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_l2_params_ran_param_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_l2_params_ran_param_item_s_groups[] = {
    {{seq_table_l2_params_ran_param_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_l2_params_ran_param_item_s_g0_opts,
      seq_table_l2_params_ran_param_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc l2_params_ran_param_item_s::desc = {{seq_table_l2_params_ran_param_item_s_fields,
                                                                 uint64_t{0x3},
                                                                 nullptr,
                                                                 seq_table_l2_params_ran_param_item_s_names,
                                                                 2,
                                                                 0},
                                                                true,
                                                                true,
                                                                seq_table_l2_params_ran_param_item_s_groups,
                                                                1};

// RANFunctionDefinition-EventTrigger-Breakpoint-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_event_trigger_breakpoint_item_s_fields[] = {
    {offsetof(ran_function_definition_event_trigger_breakpoint_item_s, call_process_breakpoint_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(ran_function_definition_event_trigger_breakpoint_item_s, call_process_breakpoint_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(ran_function_definition_event_trigger_breakpoint_item_s::call_process_breakpoint_name),
         asn1::table::string_json>::value},
    {offsetof(ran_function_definition_event_trigger_breakpoint_item_s, ran_call_process_breakpoint_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<
         decltype(ran_function_definition_event_trigger_breakpoint_item_s::ran_call_process_breakpoint_params_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &call_process_breakpoint_ran_param_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_event_trigger_breakpoint_item_s_names[] = {
    {"callProcessBreakpoint-ID", nullptr},
    {"callProcessBreakpoint-Name", nullptr},
    {"ran-CallProcessBreakpointParameters-List", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_event_trigger_breakpoint_item_s_opts[] = {
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_definition_event_trigger_breakpoint_item_s::desc = {
    {seq_table_ran_function_definition_event_trigger_breakpoint_item_s_fields,
     uint64_t{0x3},
     seq_table_ran_function_definition_event_trigger_breakpoint_item_s_opts,
     seq_table_ran_function_definition_event_trigger_breakpoint_item_s_names,
     3,
     1},
    true,
    true,
    nullptr,
    0};

// RANFunctionDefinition-EventTrigger-CallProcess-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_event_trigger_call_process_item_s_fields[] = {
    {offsetof(ran_function_definition_event_trigger_call_process_item_s, call_process_type_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(ran_function_definition_event_trigger_call_process_item_s, call_process_type_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<
         decltype(ran_function_definition_event_trigger_call_process_item_s::call_process_type_name),
         asn1::table::string_json>::value},
    {offsetof(ran_function_definition_event_trigger_call_process_item_s, call_process_breakpoints_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<
         decltype(ran_function_definition_event_trigger_call_process_item_s::call_process_breakpoints_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &ran_function_definition_event_trigger_breakpoint_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_event_trigger_call_process_item_s_names[] = {
    {"callProcessType-ID", nullptr},
    {"callProcessType-Name", nullptr},
    {"callProcessBreakpoints-List", nullptr}};
const asn1::table::seq_desc ran_function_definition_event_trigger_call_process_item_s::desc = {
    {seq_table_ran_function_definition_event_trigger_call_process_item_s_fields,
     uint64_t{0x7},
     nullptr,
     seq_table_ran_function_definition_event_trigger_call_process_item_s_names,
     3,
     0},
    true,
    true,
    nullptr,
    0};

// UEIdentification-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ue_identif_ran_param_item_s_fields[] = {
    {offsetof(ue_identif_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(ue_identif_ran_param_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ue_identif_ran_param_item_s::ran_param_name),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_ue_identif_ran_param_item_s_names[]     = {{"ranParameter-ID", nullptr},
                                                                                          {"ranParameter-name", nullptr}};
static const asn1::table::seq_field  seq_table_ue_identif_ran_param_item_s_g0_fields[] = {
    {offsetof(ue_identif_ran_param_item_s, ran_param_definition),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(ue_identif_ran_param_item_s::ran_param_definition)>::value,
      &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_ue_identif_ran_param_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_ue_identif_ran_param_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_ue_identif_ran_param_item_s_groups[] = {
    {{seq_table_ue_identif_ran_param_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_ue_identif_ran_param_item_s_g0_opts,
      seq_table_ue_identif_ran_param_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc ue_identif_ran_param_item_s::desc = {{seq_table_ue_identif_ran_param_item_s_fields,
                                                                  uint64_t{0x3},
                                                                  nullptr,
                                                                  seq_table_ue_identif_ran_param_item_s_names,
                                                                  2,
                                                                  0},
                                                                 true,
                                                                 true,
                                                                 seq_table_ue_identif_ran_param_item_s_groups,
                                                                 1};

// RANFunctionDefinition-EventTrigger ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_event_trigger_s_fields[] = {
    {offsetof(ran_function_definition_event_trigger_s, ric_event_trigger_style_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(ran_function_definition_event_trigger_s::ric_event_trigger_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_function_definition_event_trigger_style_item_s::desc},
    {offsetof(ran_function_definition_event_trigger_s, ran_l2_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_function_definition_event_trigger_s::ran_l2_params_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &l2_params_ran_param_item_s::desc},
    {offsetof(ran_function_definition_event_trigger_s, ran_call_process_types_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_function_definition_event_trigger_s::ran_call_process_types_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_function_definition_event_trigger_call_process_item_s::desc},
    {offsetof(ran_function_definition_event_trigger_s, ran_ue_identif_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_function_definition_event_trigger_s::ran_ue_identif_params_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ue_identif_ran_param_item_s::desc},
    {offsetof(ran_function_definition_event_trigger_s, ran_cell_identif_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_function_definition_event_trigger_s::ran_cell_identif_params_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &cell_identif_ran_param_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_event_trigger_s_names[] = {
    {"ric-EventTriggerStyle-List", nullptr},
    {"ran-L2Parameters-List", nullptr},
    {"ran-CallProcessTypes-List", nullptr},
    {"ran-UEIdentificationParameters-List", nullptr},
    {"ran-CellIdentificationParameters-List", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_event_trigger_s_opts[] = {
    {1, asn1::table::seq_field::list_presence},
    {2, asn1::table::seq_field::list_presence},
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_definition_event_trigger_s::desc = {
    {seq_table_ran_function_definition_event_trigger_s_fields,
     uint64_t{0x1},
     seq_table_ran_function_definition_event_trigger_s_opts,
     seq_table_ran_function_definition_event_trigger_s_names,
     5,
     4},
    true,
    true,
    nullptr,
    0};

// Report-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_report_ran_param_item_s_fields[] = {
    {offsetof(report_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(report_ran_param_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(report_ran_param_item_s::ran_param_name), asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_report_ran_param_item_s_names[]     = {{"ranParameter-ID", nullptr},
                                                                                      {"ranParameter-name", nullptr}};
static const asn1::table::seq_field  seq_table_report_ran_param_item_s_g0_fields[] = {
    {offsetof(report_ran_param_item_s, ran_param_definition),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(report_ran_param_item_s::ran_param_definition)>::value,
      &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_report_ran_param_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_report_ran_param_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_report_ran_param_item_s_groups[] = {
    {{seq_table_report_ran_param_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_report_ran_param_item_s_g0_opts,
      seq_table_report_ran_param_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc report_ran_param_item_s::desc = {
    {seq_table_report_ran_param_item_s_fields, uint64_t{0x3}, nullptr, seq_table_report_ran_param_item_s_names, 2, 0},
    true,
    true,
    seq_table_report_ran_param_item_s_groups,
    1};

// RANFunctionDefinition-Report-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_report_item_s_fields[] = {
    {offsetof(ran_function_definition_report_item_s, ric_report_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_report_item_s, ric_report_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_definition_report_item_s::ric_report_style_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_definition_report_item_s, ric_supported_event_trigger_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_report_item_s, ric_report_action_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_report_item_s, ric_ind_hdr_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_report_item_s, ric_ind_msg_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_report_item_s, ran_report_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_function_definition_report_item_s::ran_report_params_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &report_ran_param_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_report_item_s_names[] = {
    {"ric-ReportStyle-Type", nullptr},
    {"ric-ReportStyle-Name", nullptr},
    {"ric-SupportedEventTriggerStyle-Type", nullptr},
    {"ric-ReportActionFormat-Type", nullptr},
    {"ric-IndicationHeaderFormat-Type", nullptr},
    {"ric-IndicationMessageFormat-Type", nullptr},
    {"ran-ReportParameters-List", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_report_item_s_opts[] = {
    {6, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_definition_report_item_s::desc = {
    {seq_table_ran_function_definition_report_item_s_fields,
     uint64_t{0x3f},
     seq_table_ran_function_definition_report_item_s_opts,
     seq_table_ran_function_definition_report_item_s_names,
     7,
     1},
    true,
    true,
    nullptr,
    0};

// RANFunctionDefinition-Report ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_report_s_fields[] = {
    {offsetof(ran_function_definition_report_s, ric_report_style_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(ran_function_definition_report_s::ric_report_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_function_definition_report_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_report_s_names[] = {
    {"ric-ReportStyle-List", nullptr}};
const asn1::table::seq_desc ran_function_definition_report_s::desc = {
    {seq_table_ran_function_definition_report_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ran_function_definition_report_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// InsertIndication-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_insert_ind_ran_param_item_s_fields[] = {
    {offsetof(insert_ind_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(insert_ind_ran_param_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(insert_ind_ran_param_item_s::ran_param_name),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_insert_ind_ran_param_item_s_names[]     = {{"ranParameter-ID", nullptr},
                                                                                          {"ranParameter-name", nullptr}};
static const asn1::table::seq_field  seq_table_insert_ind_ran_param_item_s_g0_fields[] = {
    {offsetof(insert_ind_ran_param_item_s, ran_param_definition),
      asn1::table::seq_field::ptr_presence,
      asn1::table::field_op::sequence,
      false,
      false,
      0,
      0,
      &asn1::table::ptr_nested_codec<decltype(insert_ind_ran_param_item_s::ran_param_definition)>::value,
      &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_insert_ind_ran_param_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_insert_ind_ran_param_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_insert_ind_ran_param_item_s_groups[] = {
    {{seq_table_insert_ind_ran_param_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_insert_ind_ran_param_item_s_g0_opts,
      seq_table_insert_ind_ran_param_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc insert_ind_ran_param_item_s::desc = {{seq_table_insert_ind_ran_param_item_s_fields,
                                                                  uint64_t{0x3},
                                                                  nullptr,
                                                                  seq_table_insert_ind_ran_param_item_s_names,
                                                                  2,
                                                                  0},
                                                                 true,
                                                                 true,
                                                                 seq_table_insert_ind_ran_param_item_s_groups,
                                                                 1};

// RANFunctionDefinition-Insert-Indication-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_insert_ind_item_s_fields[] = {
    {offsetof(ran_function_definition_insert_ind_item_s, ric_insert_ind_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(ran_function_definition_insert_ind_item_s, ric_insert_ind_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_definition_insert_ind_item_s::ric_insert_ind_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_definition_insert_ind_item_s, ran_insert_ind_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_function_definition_insert_ind_item_s::ran_insert_ind_params_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &insert_ind_ran_param_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_insert_ind_item_s_names[] = {
    {"ric-InsertIndication-ID", nullptr},
    {"ric-InsertIndication-Name", nullptr},
    {"ran-InsertIndicationParameters-List", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_insert_ind_item_s_opts[] = {
    {2, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_definition_insert_ind_item_s::desc = {
    {seq_table_ran_function_definition_insert_ind_item_s_fields,
     uint64_t{0x3},
     seq_table_ran_function_definition_insert_ind_item_s_opts,
     seq_table_ran_function_definition_insert_ind_item_s_names,
     3,
     1},
    true,
    true,
    nullptr,
    0};

// RANFunctionDefinition-Insert-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_insert_item_s_fields[] = {
    {offsetof(ran_function_definition_insert_item_s, ric_insert_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_insert_item_s, ric_insert_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_definition_insert_item_s::ric_insert_style_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_definition_insert_item_s, ric_supported_event_trigger_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_insert_item_s, ric_action_definition_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_insert_item_s, ric_insert_ind_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_function_definition_insert_item_s::ric_insert_ind_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_function_definition_insert_ind_item_s::desc},
    {offsetof(ran_function_definition_insert_item_s, ric_ind_hdr_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_insert_item_s, ric_ind_msg_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_insert_item_s, ric_call_process_id_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr}};
static const asn1::table::field_name seq_table_ran_function_definition_insert_item_s_names[] = {
    {"ric-InsertStyle-Type", nullptr},
    {"ric-InsertStyle-Name", nullptr},
    {"ric-SupportedEventTriggerStyle-Type", nullptr},
    {"ric-ActionDefinitionFormat-Type", nullptr},
    {"ric-InsertIndication-List", nullptr},
    {"ric-IndicationHeaderFormat-Type", nullptr},
    {"ric-IndicationMessageFormat-Type", nullptr},
    {"ric-CallProcessIDFormat-Type", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_insert_item_s_opts[] = {
    {4, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_definition_insert_item_s::desc = {
    {seq_table_ran_function_definition_insert_item_s_fields,
     uint64_t{0xef},
     seq_table_ran_function_definition_insert_item_s_opts,
     seq_table_ran_function_definition_insert_item_s_names,
     8,
     1},
    true,
    true,
    nullptr,
    0};

// RANFunctionDefinition-Insert ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_insert_s_fields[] = {
    {offsetof(ran_function_definition_insert_s, ric_insert_style_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(ran_function_definition_insert_s::ric_insert_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_function_definition_insert_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_insert_s_names[] = {
    {"ric-InsertStyle-List", nullptr}};
const asn1::table::seq_desc ran_function_definition_insert_s::desc = {
    {seq_table_ran_function_definition_insert_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ran_function_definition_insert_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// RANFunctionDefinition-Control-Action-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_ctrl_action_item_s_fields[] = {
    {offsetof(ran_function_definition_ctrl_action_item_s, ric_ctrl_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(ran_function_definition_ctrl_action_item_s, ric_ctrl_action_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_definition_ctrl_action_item_s::ric_ctrl_action_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_definition_ctrl_action_item_s, ran_ctrl_action_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_function_definition_ctrl_action_item_s::ran_ctrl_action_params_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ctrl_action_ran_param_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_ctrl_action_item_s_names[] = {
    {"ric-ControlAction-ID", nullptr},
    {"ric-ControlAction-Name", nullptr},
    {"ran-ControlActionParameters-List", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_ctrl_action_item_s_opts[] = {
    {2, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_ran_function_definition_ctrl_action_item_s_g0_fields[] = {
    {offsetof(ran_function_definition_ctrl_action_item_s, ue_group_ctrl_action_supported),
     (int32_t)offsetof(ran_function_definition_ctrl_action_item_s, ue_group_ctrl_action_supported_present),
     asn1::table::enum_op<decltype(ran_function_definition_ctrl_action_item_s::ue_group_ctrl_action_supported)>(),
     decltype(ran_function_definition_ctrl_action_item_s::ue_group_ctrl_action_supported)::has_ext,
     false,
     decltype(ran_function_definition_ctrl_action_item_s::ue_group_ctrl_action_supported)::nof_types,
     decltype(ran_function_definition_ctrl_action_item_s::ue_group_ctrl_action_supported)::nof_exts,
     nullptr,
     &ran_function_definition_ctrl_action_item_s::ue_group_ctrl_action_supported_opts::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_ctrl_action_item_s_g0_names[] = {
    {"ueGroup-ControlAction-Supported", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_ctrl_action_item_s_g0_opts[] = {
    {0, (int32_t)offsetof(ran_function_definition_ctrl_action_item_s, ue_group_ctrl_action_supported_present)}};
static const asn1::table::seq_group seq_table_ran_function_definition_ctrl_action_item_s_groups[] = {
    {{seq_table_ran_function_definition_ctrl_action_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_ran_function_definition_ctrl_action_item_s_g0_opts,
      seq_table_ran_function_definition_ctrl_action_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc ran_function_definition_ctrl_action_item_s::desc = {
    {seq_table_ran_function_definition_ctrl_action_item_s_fields,
     uint64_t{0x3},
     seq_table_ran_function_definition_ctrl_action_item_s_opts,
     seq_table_ran_function_definition_ctrl_action_item_s_names,
     3,
     1},
    true,
    true,
    seq_table_ran_function_definition_ctrl_action_item_s_groups,
    1};

static const char* enum_names_ran_function_definition_ctrl_action_item_s_ue_group_ctrl_action_supported_e__values[] = {
    "true",
    "false"};
const asn1::table::enum_desc ran_function_definition_ctrl_action_item_s::ue_group_ctrl_action_supported_opts::desc = {
    enum_names_ran_function_definition_ctrl_action_item_s_ue_group_ctrl_action_supported_e__values,
    2,
    "ran_function_definition_ctrl_action_item_s::ue_group_ctrl_action_supported_e_"};

// RANFunctionDefinition-Control-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_ctrl_item_s_fields[] = {
    {offsetof(ran_function_definition_ctrl_item_s, ric_ctrl_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_ctrl_item_s, ric_ctrl_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_definition_ctrl_item_s::ric_ctrl_style_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_definition_ctrl_item_s, ric_ctrl_action_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_function_definition_ctrl_item_s::ric_ctrl_action_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_function_definition_ctrl_action_item_s::desc},
    {offsetof(ran_function_definition_ctrl_item_s, ric_ctrl_hdr_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_ctrl_item_s, ric_ctrl_msg_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_ctrl_item_s, ric_call_process_id_format_type),
     (int32_t)offsetof(ran_function_definition_ctrl_item_s, ric_call_process_id_format_type_present),
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_ctrl_item_s, ric_ctrl_outcome_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_ctrl_item_s, ran_ctrl_outcome_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     254,
     &asn1::table::item_list_codec<decltype(ran_function_definition_ctrl_item_s::ran_ctrl_outcome_params_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ctrl_outcome_ran_param_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_ctrl_item_s_names[] = {
    {"ric-ControlStyle-Type", nullptr},
    {"ric-ControlStyle-Name", nullptr},
    {"ric-ControlAction-List", nullptr},
    {"ric-ControlHeaderFormat-Type", nullptr},
    {"ric-ControlMessageFormat-Type", nullptr},
    {"ric-CallProcessIDFormat-Type", nullptr},
    {"ric-ControlOutcomeFormat-Type", nullptr},
    {"ran-ControlOutcomeParameters-List", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_ctrl_item_s_opts[] = {
    {2, asn1::table::seq_field::list_presence},
    {5, (int32_t)offsetof(ran_function_definition_ctrl_item_s, ric_call_process_id_format_type_present)},
    {7, asn1::table::seq_field::list_presence}};
static const asn1::table::seq_field seq_table_ran_function_definition_ctrl_item_s_g0_fields[] = {
    {offsetof(ran_function_definition_ctrl_item_s, list_of_add_supported_formats),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::codec,
     false,
     true,
     0,
     63,
     &asn1::table::ptr_item_list_codec<decltype(ran_function_definition_ctrl_item_s::list_of_add_supported_formats),
                                       asn1::table::sequence_item_ops,
                                       false,
                                       true>::value,
     &add_supported_format_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_ctrl_item_s_g0_names[] = {
    {"listOfAdditionalSupportedFormats", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_ctrl_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_ran_function_definition_ctrl_item_s_groups[] = {
    {{seq_table_ran_function_definition_ctrl_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_ran_function_definition_ctrl_item_s_g0_opts,
      seq_table_ran_function_definition_ctrl_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc ran_function_definition_ctrl_item_s::desc = {
    {seq_table_ran_function_definition_ctrl_item_s_fields,
     uint64_t{0x5b},
     seq_table_ran_function_definition_ctrl_item_s_opts,
     seq_table_ran_function_definition_ctrl_item_s_names,
     8,
     3},
    true,
    true,
    seq_table_ran_function_definition_ctrl_item_s_groups,
    1};

// RANFunctionDefinition-Control ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_ctrl_s_fields[] = {
    {offsetof(ran_function_definition_ctrl_s, ric_ctrl_style_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(ran_function_definition_ctrl_s::ric_ctrl_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_function_definition_ctrl_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_ctrl_s_names[] = {
    {"ric-ControlStyle-List", nullptr}};
const asn1::table::seq_desc ran_function_definition_ctrl_s::desc = {{seq_table_ran_function_definition_ctrl_s_fields,
                                                                     uint64_t{0x1},
                                                                     nullptr,
                                                                     seq_table_ran_function_definition_ctrl_s_names,
                                                                     1,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    nullptr,
                                                                    0};

// PolicyAction-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_policy_action_ran_param_item_s_fields[] = {
    {offsetof(policy_action_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(policy_action_ran_param_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(policy_action_ran_param_item_s::ran_param_name),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_policy_action_ran_param_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-name", nullptr}};
static const asn1::table::seq_field seq_table_policy_action_ran_param_item_s_g0_fields[] = {
    {offsetof(policy_action_ran_param_item_s, ran_param_definition),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(policy_action_ran_param_item_s::ran_param_definition)>::value,
     &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_policy_action_ran_param_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_policy_action_ran_param_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_policy_action_ran_param_item_s_groups[] = {
    {{seq_table_policy_action_ran_param_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_policy_action_ran_param_item_s_g0_opts,
      seq_table_policy_action_ran_param_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc policy_action_ran_param_item_s::desc = {{seq_table_policy_action_ran_param_item_s_fields,
                                                                     uint64_t{0x3},
                                                                     nullptr,
                                                                     seq_table_policy_action_ran_param_item_s_names,
                                                                     2,
                                                                     0},
                                                                    true,
                                                                    true,
                                                                    seq_table_policy_action_ran_param_item_s_groups,
                                                                    1};

// PolicyCondition-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_policy_condition_ran_param_item_s_fields[] = {
    {offsetof(policy_condition_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(policy_condition_ran_param_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(policy_condition_ran_param_item_s::ran_param_name),
                                asn1::table::string_json>::value}};
static const asn1::table::field_name seq_table_policy_condition_ran_param_item_s_names[] = {
    {"ranParameter-ID", nullptr},
    {"ranParameter-name", nullptr}};
static const asn1::table::seq_field seq_table_policy_condition_ran_param_item_s_g0_fields[] = {
    {offsetof(policy_condition_ran_param_item_s, ran_param_definition),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(policy_condition_ran_param_item_s::ran_param_definition)>::value,
     &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_policy_condition_ran_param_item_s_g0_names[] = {
    {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt seq_table_policy_condition_ran_param_item_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_policy_condition_ran_param_item_s_groups[] = {
    {{seq_table_policy_condition_ran_param_item_s_g0_fields,
      uint64_t{0x0},
      seq_table_policy_condition_ran_param_item_s_g0_opts,
      seq_table_policy_condition_ran_param_item_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc policy_condition_ran_param_item_s::desc = {
    {seq_table_policy_condition_ran_param_item_s_fields,
     uint64_t{0x3},
     nullptr,
     seq_table_policy_condition_ran_param_item_s_names,
     2,
     0},
    true,
    true,
    seq_table_policy_condition_ran_param_item_s_groups,
    1};

// RANFunctionDefinition-Policy-Action-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_policy_action_item_s_fields[] = {
    {offsetof(ran_function_definition_policy_action_item_s, ric_policy_action_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     true,
     true,
     int64_t{1},
     uint64_t{65534},
     nullptr},
    {offsetof(ran_function_definition_policy_action_item_s, ric_policy_action_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_definition_policy_action_item_s::ric_policy_action_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_definition_policy_action_item_s, ric_action_definition_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_policy_action_item_s, ran_policy_action_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<
         decltype(ran_function_definition_policy_action_item_s::ran_policy_action_params_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &policy_action_ran_param_item_s::desc},
    {offsetof(ran_function_definition_policy_action_item_s, ran_policy_condition_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<
         decltype(ran_function_definition_policy_action_item_s::ran_policy_condition_params_list),
         asn1::table::sequence_item_ops,
         false,
         true>::value,
     &policy_condition_ran_param_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_policy_action_item_s_names[] = {
    {"ric-PolicyAction-ID", nullptr},
    {"ric-PolicyAction-Name", nullptr},
    {"ric-ActionDefinitionFormat-Type", nullptr},
    {"ran-PolicyActionParameters-List", nullptr},
    {"ran-PolicyConditionParameters-List", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_policy_action_item_s_opts[] = {
    {3, asn1::table::seq_field::list_presence},
    {4, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_definition_policy_action_item_s::desc = {
    {seq_table_ran_function_definition_policy_action_item_s_fields,
     uint64_t{0x7},
     seq_table_ran_function_definition_policy_action_item_s_opts,
     seq_table_ran_function_definition_policy_action_item_s_names,
     5,
     2},
    true,
    true,
    nullptr,
    0};

// RANFunctionDefinition-Policy-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_policy_item_s_fields[] = {
    {offsetof(ran_function_definition_policy_item_s, ric_policy_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_policy_item_s, ric_policy_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_definition_policy_item_s::ric_policy_style_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_definition_policy_item_s, ric_supported_event_trigger_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_policy_item_s, ric_policy_action_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_function_definition_policy_item_s::ric_policy_action_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_function_definition_policy_action_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_policy_item_s_names[] = {
    {"ric-PolicyStyle-Type", nullptr},
    {"ric-PolicyStyle-Name", nullptr},
    {"ric-SupportedEventTriggerStyle-Type", nullptr},
    {"ric-PolicyAction-List", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_policy_item_s_opts[] = {
    {3, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_definition_policy_item_s::desc = {
    {seq_table_ran_function_definition_policy_item_s_fields,
     uint64_t{0x7},
     seq_table_ran_function_definition_policy_item_s_opts,
     seq_table_ran_function_definition_policy_item_s_names,
     4,
     1},
    true,
    true,
    nullptr,
    0};

// RANFunctionDefinition-Policy ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_policy_s_fields[] = {
    {offsetof(ran_function_definition_policy_s, ric_policy_style_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(ran_function_definition_policy_s::ric_policy_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_function_definition_policy_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_policy_s_names[] = {
    {"ric-PolicyStyle-List", nullptr}};
const asn1::table::seq_desc ran_function_definition_policy_s::desc = {
    {seq_table_ran_function_definition_policy_s_fields,
     uint64_t{0x1},
     nullptr,
     seq_table_ran_function_definition_policy_s_names,
     1,
     0},
    true,
    true,
    nullptr,
    0};

// Query-RANParameter-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_query_ran_param_item_s_fields[] = {
    {offsetof(query_ran_param_item_s, ran_param_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u64,
     true,
     true,
     int64_t{1},
     uint64_t{4294967294},
     nullptr},
    {offsetof(query_ran_param_item_s, ran_param_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(query_ran_param_item_s::ran_param_name), asn1::table::string_json>::value},
    {offsetof(query_ran_param_item_s, ran_param_definition),
     (int32_t)offsetof(query_ran_param_item_s, ran_param_definition_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_param_definition_s::desc}};
static const asn1::table::field_name seq_table_query_ran_param_item_s_names[] = {{"ranParameter-ID", nullptr},
                                                                                 {"ranParameter-name", nullptr},
                                                                                 {"ranParameter-Definition", nullptr}};
static const asn1::table::seq_opt    seq_table_query_ran_param_item_s_opts[]  = {
    {2, (int32_t)offsetof(query_ran_param_item_s, ran_param_definition_present)}};
const asn1::table::seq_desc query_ran_param_item_s::desc = {{seq_table_query_ran_param_item_s_fields,
                                                             uint64_t{0x3},
                                                             seq_table_query_ran_param_item_s_opts,
                                                             seq_table_query_ran_param_item_s_names,
                                                             3,
                                                             1},
                                                            true,
                                                            true,
                                                            nullptr,
                                                            0};

// RANFunctionDefinition-Query-Item ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_query_item_s_fields[] = {
    {offsetof(ran_function_definition_query_item_s, ric_query_style_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_query_item_s, ric_query_style_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(ran_function_definition_query_item_s::ric_query_style_name),
                                asn1::table::string_json>::value},
    {offsetof(ran_function_definition_query_item_s, ric_query_hdr_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_query_item_s, ric_query_definition_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_query_item_s, ric_query_outcome_format_type),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_i64,
     false,
     true,
     std::numeric_limits<int64_t>::min(),
     uint64_t{18446744073709551615U},
     nullptr},
    {offsetof(ran_function_definition_query_item_s, ran_query_params_list),
     asn1::table::seq_field::list_presence,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     65534,
     &asn1::table::item_list_codec<decltype(ran_function_definition_query_item_s::ran_query_params_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &query_ran_param_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_query_item_s_names[] = {
    {"ric-QueryStyle-Type", nullptr},
    {"ric-QueryStyle-Name", nullptr},
    {"ric-QueryHeaderFormat-Type", nullptr},
    {"ric-QueryDefinitionFormat-Type", nullptr},
    {"ric-QueryOutcomeFormat-Type", nullptr},
    {"ran-QueryParameters-List", nullptr}};
static const asn1::table::seq_opt seq_table_ran_function_definition_query_item_s_opts[] = {
    {5, asn1::table::seq_field::list_presence}};
const asn1::table::seq_desc ran_function_definition_query_item_s::desc = {
    {seq_table_ran_function_definition_query_item_s_fields,
     uint64_t{0x1f},
     seq_table_ran_function_definition_query_item_s_opts,
     seq_table_ran_function_definition_query_item_s_names,
     6,
     1},
    true,
    true,
    nullptr,
    0};

// RANFunctionDefinition-Query ::= SEQUENCE
static const asn1::table::seq_field seq_table_ran_function_definition_query_s_fields[] = {
    {offsetof(ran_function_definition_query_s, ric_query_style_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     true,
     1,
     62,
     &asn1::table::item_list_codec<decltype(ran_function_definition_query_s::ric_query_style_list),
                                   asn1::table::sequence_item_ops,
                                   false,
                                   true>::value,
     &ran_function_definition_query_item_s::desc}};
static const asn1::table::field_name seq_table_ran_function_definition_query_s_names[] = {
    {"ric-QueryStyle-List", nullptr}};
const asn1::table::seq_desc ran_function_definition_query_s::desc = {{seq_table_ran_function_definition_query_s_fields,
                                                                      uint64_t{0x1},
                                                                      nullptr,
                                                                      seq_table_ran_function_definition_query_s_names,
                                                                      1,
                                                                      0},
                                                                     true,
                                                                     true,
                                                                     nullptr,
                                                                     0};

// E2SM-RC-RANFunctionDefinition ::= SEQUENCE
static const asn1::table::seq_field seq_table_e2sm_rc_ran_function_definition_s_fields[] = {
    {offsetof(e2sm_rc_ran_function_definition_s, ran_function_name),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ra_nfunction_name_s::desc},
    {offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_event_trigger),
     (int32_t)offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_event_trigger_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_definition_event_trigger_s::desc},
    {offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_report),
     (int32_t)offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_report_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_definition_report_s::desc},
    {offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_insert),
     (int32_t)offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_insert_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_definition_insert_s::desc},
    {offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_ctrl),
     (int32_t)offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_ctrl_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_definition_ctrl_s::desc},
    {offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_policy),
     (int32_t)offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_policy_present),
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &ran_function_definition_policy_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ran_function_definition_s_names[] = {
    {"ranFunction-Name", nullptr},
    {"ranFunctionDefinition-EventTrigger", nullptr},
    {"ranFunctionDefinition-Report", nullptr},
    {"ranFunctionDefinition-Insert", nullptr},
    {"ranFunctionDefinition-Control", nullptr},
    {"ranFunctionDefinition-Policy", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_ran_function_definition_s_opts[] = {
    {1, (int32_t)offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_event_trigger_present)},
    {2, (int32_t)offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_report_present)},
    {3, (int32_t)offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_insert_present)},
    {4, (int32_t)offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_ctrl_present)},
    {5, (int32_t)offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_policy_present)}};
static const asn1::table::seq_field seq_table_e2sm_rc_ran_function_definition_s_g0_fields[] = {
    {offsetof(e2sm_rc_ran_function_definition_s, ran_function_definition_query),
     asn1::table::seq_field::ptr_presence,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::ptr_nested_codec<decltype(e2sm_rc_ran_function_definition_s::ran_function_definition_query)>::value,
     &ran_function_definition_query_s::desc}};
static const asn1::table::field_name seq_table_e2sm_rc_ran_function_definition_s_g0_names[] = {
    {"ranFunctionDefinition-Query", nullptr}};
static const asn1::table::seq_opt seq_table_e2sm_rc_ran_function_definition_s_g0_opts[] = {
    {0, asn1::table::seq_field::ptr_presence}};
static const asn1::table::seq_group seq_table_e2sm_rc_ran_function_definition_s_groups[] = {
    {{seq_table_e2sm_rc_ran_function_definition_s_g0_fields,
      uint64_t{0x0},
      seq_table_e2sm_rc_ran_function_definition_s_g0_opts,
      seq_table_e2sm_rc_ran_function_definition_s_g0_names,
      1,
      1},
     true}};
const asn1::table::seq_desc e2sm_rc_ran_function_definition_s::desc = {
    {seq_table_e2sm_rc_ran_function_definition_s_fields,
     uint64_t{0x1},
     seq_table_e2sm_rc_ran_function_definition_s_opts,
     seq_table_e2sm_rc_ran_function_definition_s_names,
     6,
     5},
    true,
    true,
    seq_table_e2sm_rc_ran_function_definition_s_groups,
    1};

// E2SM-RC-ServiceLayerCause ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_e2sm_rc_service_layer_cause_c_alternatives[] = {
    nullptr,
    &asn1::table::alternative_ops<cause_ri_cservice_e, true>::value,
    &asn1::table::alternative_ops<cause_protocol_e, true>::value};
const asn1::table::choice_storage_desc e2sm_rc_service_layer_cause_c::storage_desc = {
    choice_storage_e2sm_rc_service_layer_cause_c_alternatives,
    3,
    "e2sm_rc_service_layer_cause_c"};

void e2sm_rc_service_layer_cause_c::set_ric_request()
{
  set(types::ric_request);
}
cause_ri_cservice_e& e2sm_rc_service_layer_cause_c::set_ric_service()
{
  set(types::ric_service);
  return c.get<cause_ri_cservice_e>();
}
cause_protocol_e& e2sm_rc_service_layer_cause_c::set_protocol()
{
  set(types::protocol);
  return c.get<cause_protocol_e>();
}
static const asn1::table::seq_field choice_table_e2sm_rc_service_layer_cause_c_alternatives[] = {
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<cause_ri_cservice_e>(),
     cause_ri_cservice_e::has_ext,
     false,
     cause_ri_cservice_e::nof_types,
     cause_ri_cservice_e::nof_exts,
     nullptr,
     &cause_ri_cservice_opts::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<cause_protocol_e>(),
     cause_protocol_e::has_ext,
     false,
     cause_protocol_e::nof_types,
     cause_protocol_e::nof_exts,
     nullptr,
     &cause_protocol_opts::desc}};
static const asn1::table::field_name choice_table_e2sm_rc_service_layer_cause_c_names[] = {{"ricRequest", nullptr},
                                                                                           {"ricService", nullptr},
                                                                                           {"protocol", nullptr}};
const asn1::table::choice_desc       e2sm_rc_service_layer_cause_c::desc                = {
    asn1::table::enum_int_type<e2sm_rc_service_layer_cause_c::types>(),
    e2sm_rc_service_layer_cause_c::types::nof_types,
    e2sm_rc_service_layer_cause_c::types::nof_exts,
    e2sm_rc_service_layer_cause_c::types::has_ext,
    true,
    choice_table_e2sm_rc_service_layer_cause_c_alternatives,
    nullptr,
    choice_table_e2sm_rc_service_layer_cause_c_names,
    "e2sm_rc_service_layer_cause_c",
    offsetof(e2sm_rc_service_layer_cause_c, c.type),
    offsetof(e2sm_rc_service_layer_cause_c, c.value),
    false,
    &e2sm_rc_service_layer_cause_c::storage_desc};

static const char* enum_names_e2sm_rc_service_layer_cause_c_types_values[]   = {"ricRequest", "ricService", "protocol"};
const asn1::table::enum_desc e2sm_rc_service_layer_cause_c::types_opts::desc = {
    enum_names_e2sm_rc_service_layer_cause_c_types_values,
    3,
    "e2sm_rc_service_layer_cause_c::types"};
