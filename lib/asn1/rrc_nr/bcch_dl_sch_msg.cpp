// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "ocudu/asn1/asn1_codec.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// BCCH-DL-SCH-MessageType ::= CHOICE
void bcch_dl_sch_msg_type_c::set(types::options e)
{
  type_ = e;
}
bcch_dl_sch_msg_type_c::c1_c_& bcch_dl_sch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void bcch_dl_sch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
static const asn1::table::seq_field choice_table_bcch_dl_sch_msg_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<bcch_dl_sch_msg_type_c::c1_c_, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_bcch_dl_sch_msg_type_c_names[] = {{"c1", nullptr},
                                                                                    {"messageClassExtension", nullptr}};
const asn1::table::choice_desc       bcch_dl_sch_msg_type_c::desc                = {
    asn1::table::enum_int_type<bcch_dl_sch_msg_type_c::types>(),
    bcch_dl_sch_msg_type_c::types::nof_types,
    bcch_dl_sch_msg_type_c::types::nof_exts,
    bcch_dl_sch_msg_type_c::types::has_ext,
    false,
    choice_table_bcch_dl_sch_msg_type_c_alternatives,
    &asn1::table::choice_set<bcch_dl_sch_msg_type_c>,
    choice_table_bcch_dl_sch_msg_type_c_names,
    "bcch_dl_sch_msg_type_c",
    offsetof(bcch_dl_sch_msg_type_c, type_),
    offsetof(bcch_dl_sch_msg_type_c, c),
    false};

static const asn1::table::seq_field choice_table_bcch_dl_sch_msg_type_c_c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<sys_info_s>::value,
     &sys_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<sib1_s>::value,
     &sib1_s::desc}};
static const asn1::table::field_name choice_table_bcch_dl_sch_msg_type_c_c1_c__names[] = {
    {"systemInformation", nullptr},
    {"systemInformationBlockType1", nullptr}};
const asn1::table::choice_desc bcch_dl_sch_msg_type_c::c1_c_::desc = {
    asn1::table::enum_int_type<bcch_dl_sch_msg_type_c::c1_c_::types>(),
    bcch_dl_sch_msg_type_c::c1_c_::types::nof_types,
    bcch_dl_sch_msg_type_c::c1_c_::types::nof_exts,
    bcch_dl_sch_msg_type_c::c1_c_::types::has_ext,
    false,
    choice_table_bcch_dl_sch_msg_type_c_c1_c__alternatives,
    &asn1::table::choice_set<bcch_dl_sch_msg_type_c::c1_c_>,
    choice_table_bcch_dl_sch_msg_type_c_c1_c__names,
    "bcch_dl_sch_msg_type_c::c1_c_",
    offsetof(bcch_dl_sch_msg_type_c::c1_c_, type_),
    offsetof(bcch_dl_sch_msg_type_c::c1_c_, c),
    false};

void bcch_dl_sch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, bcch_dl_sch_msg_type_c::c1_c_::desc);
}
sys_info_s& bcch_dl_sch_msg_type_c::c1_c_::set_sys_info()
{
  set(types::sys_info);
  return c.get<sys_info_s>();
}
sib1_s& bcch_dl_sch_msg_type_c::c1_c_::set_sib_type1()
{
  set(types::sib_type1);
  return c.get<sib1_s>();
}

static const char*           enum_names_bcch_dl_sch_msg_type_c_c1_c__types_values[] = {"systemInformation",
                                                                                       "systemInformationBlockType1"};
const asn1::table::enum_desc bcch_dl_sch_msg_type_c::c1_c_::types_opts::desc        = {
    enum_names_bcch_dl_sch_msg_type_c_c1_c__types_values,
    2,
    "bcch_dl_sch_msg_type_c::c1_c_::types"};
uint8_t bcch_dl_sch_msg_type_c::c1_c_::types_opts::to_number() const
{
  if (value == sib_type1) {
    return 1;
  }
  invalid_enum_number(value, "bcch_dl_sch_msg_type_c::c1_c_::types");
  return 0;
}

static const char*           enum_names_bcch_dl_sch_msg_type_c_types_values[] = {"c1", "messageClassExtension"};
const asn1::table::enum_desc bcch_dl_sch_msg_type_c::types_opts::desc = {enum_names_bcch_dl_sch_msg_type_c_types_values,
                                                                         2,
                                                                         "bcch_dl_sch_msg_type_c::types"};
uint8_t                      bcch_dl_sch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "bcch_dl_sch_msg_type_c::types");
}

// BCCH-DL-SCH-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_bcch_dl_sch_msg_s_fields[] = {{offsetof(bcch_dl_sch_msg_s, msg),
                                                                              asn1::table::seq_field::mandatory,
                                                                              asn1::table::field_op::choice,
                                                                              false,
                                                                              false,
                                                                              0,
                                                                              0,
                                                                              nullptr,
                                                                              &bcch_dl_sch_msg_type_c::desc}};
static const asn1::table::field_name seq_table_bcch_dl_sch_msg_s_names[]  = {{"message", nullptr}};
const asn1::table::seq_desc          bcch_dl_sch_msg_s::desc              = {
    {seq_table_bcch_dl_sch_msg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_bcch_dl_sch_msg_s_names,
    "BCCH-DL-SCH-Message"};
