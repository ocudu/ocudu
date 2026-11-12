// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/ul_ccch_msg.h"
#include "ocudu/asn1/asn1_codec.h"
#include "ocudu/asn1/rrc_nr/ul_ccch_msg_ies.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// UL-CCCH-MessageType ::= CHOICE
void ul_ccch_msg_type_c::set(types::options e)
{
  type_ = e;
}
ul_ccch_msg_type_c::c1_c_& ul_ccch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void ul_ccch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
static const asn1::table::seq_field choice_table_ul_ccch_msg_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<ul_ccch_msg_type_c::c1_c_, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ul_ccch_msg_type_c_names[] = {{"c1", nullptr},
                                                                                {"messageClassExtension", nullptr}};
const asn1::table::choice_desc ul_ccch_msg_type_c::desc = {asn1::table::enum_int_type<ul_ccch_msg_type_c::types>(),
                                                           ul_ccch_msg_type_c::types::nof_types,
                                                           ul_ccch_msg_type_c::types::nof_exts,
                                                           ul_ccch_msg_type_c::types::has_ext,
                                                           false,
                                                           choice_table_ul_ccch_msg_type_c_alternatives,
                                                           &asn1::table::choice_set<ul_ccch_msg_type_c>,
                                                           choice_table_ul_ccch_msg_type_c_names,
                                                           "ul_ccch_msg_type_c",
                                                           offsetof(ul_ccch_msg_type_c, type_),
                                                           offsetof(ul_ccch_msg_type_c, c),
                                                           false};

static const asn1::table::seq_field choice_table_ul_ccch_msg_type_c_c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_setup_request_s>::value,
     &rrc_setup_request_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_resume_request_s>::value,
     &rrc_resume_request_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_reest_request_s>::value,
     &rrc_reest_request_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_sys_info_request_s>::value,
     &rrc_sys_info_request_s::desc}};
static const asn1::table::field_name choice_table_ul_ccch_msg_type_c_c1_c__names[] = {
    {"rrcSetupRequest", nullptr},
    {"rrcResumeRequest", nullptr},
    {"rrcReestablishmentRequest", nullptr},
    {"rrcSystemInfoRequest", nullptr}};
const asn1::table::choice_desc ul_ccch_msg_type_c::c1_c_::desc = {
    asn1::table::enum_int_type<ul_ccch_msg_type_c::c1_c_::types>(),
    ul_ccch_msg_type_c::c1_c_::types::nof_types,
    ul_ccch_msg_type_c::c1_c_::types::nof_exts,
    ul_ccch_msg_type_c::c1_c_::types::has_ext,
    false,
    choice_table_ul_ccch_msg_type_c_c1_c__alternatives,
    &asn1::table::choice_set<ul_ccch_msg_type_c::c1_c_>,
    choice_table_ul_ccch_msg_type_c_c1_c__names,
    "ul_ccch_msg_type_c::c1_c_",
    offsetof(ul_ccch_msg_type_c::c1_c_, type_),
    offsetof(ul_ccch_msg_type_c::c1_c_, c),
    false};

void ul_ccch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, ul_ccch_msg_type_c::c1_c_::desc);
}
rrc_setup_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_setup_request()
{
  set(types::rrc_setup_request);
  return c.get<rrc_setup_request_s>();
}
rrc_resume_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_resume_request()
{
  set(types::rrc_resume_request);
  return c.get<rrc_resume_request_s>();
}
rrc_reest_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_reest_request()
{
  set(types::rrc_reest_request);
  return c.get<rrc_reest_request_s>();
}
rrc_sys_info_request_s& ul_ccch_msg_type_c::c1_c_::set_rrc_sys_info_request()
{
  set(types::rrc_sys_info_request);
  return c.get<rrc_sys_info_request_s>();
}

static const char*           enum_names_ul_ccch_msg_type_c_c1_c__types_values[] = {"rrcSetupRequest",
                                                                                   "rrcResumeRequest",
                                                                                   "rrcReestablishmentRequest",
                                                                                   "rrcSystemInfoRequest"};
const asn1::table::enum_desc ul_ccch_msg_type_c::c1_c_::types_opts::desc        = {
    enum_names_ul_ccch_msg_type_c_c1_c__types_values,
    4,
    "ul_ccch_msg_type_c::c1_c_::types"};

static const char*           enum_names_ul_ccch_msg_type_c_types_values[] = {"c1", "messageClassExtension"};
const asn1::table::enum_desc ul_ccch_msg_type_c::types_opts::desc         = {enum_names_ul_ccch_msg_type_c_types_values,
                                                                             2,
                                                                             "ul_ccch_msg_type_c::types"};
uint8_t                      ul_ccch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_ccch_msg_type_c::types");
}

// UL-CCCH-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_ul_ccch_msg_s_fields[] = {{offsetof(ul_ccch_msg_s, msg),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::choice,
                                                                          false,
                                                                          false,
                                                                          0,
                                                                          0,
                                                                          nullptr,
                                                                          &ul_ccch_msg_type_c::desc}};
static const asn1::table::field_name seq_table_ul_ccch_msg_s_names[]  = {{"message", nullptr}};
const asn1::table::seq_desc          ul_ccch_msg_s::desc              = {
    {seq_table_ul_ccch_msg_s_fields, uint64_t{0x1}, nullptr, seq_table_ul_ccch_msg_s_names, 1, 0},
    false,
    false,
    nullptr,
    0,
    "UL-CCCH-Message"};
