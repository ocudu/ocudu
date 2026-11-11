// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/ul_ccch1_msg.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// RRCResumeRequest1-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_resume_request1_ies_s_fields[] = {
    {offsetof(rrc_resume_request1_ies_s, resume_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_resume_request1_ies_s::resume_id), asn1::table::struct_json>::value},
    {offsetof(rrc_resume_request1_ies_s, resume_mac_i),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_resume_request1_ies_s::resume_mac_i), asn1::table::struct_json>::value},
    {offsetof(rrc_resume_request1_ies_s, resume_cause),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rrc_resume_request1_ies_s::resume_cause)>(),
     decltype(rrc_resume_request1_ies_s::resume_cause)::has_ext,
     false,
     decltype(rrc_resume_request1_ies_s::resume_cause)::nof_types,
     decltype(rrc_resume_request1_ies_s::resume_cause)::nof_exts,
     nullptr,
     &resume_cause_opts::desc},
    {offsetof(rrc_resume_request1_ies_s, spare),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_resume_request1_ies_s::spare), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_rrc_resume_request1_ies_s_names[] = {{"resumeIdentity", nullptr},
                                                                                    {"resumeMAC-I", nullptr},
                                                                                    {"resumeCause", nullptr},
                                                                                    {"spare", nullptr}};
const asn1::table::seq_desc          rrc_resume_request1_ies_s::desc             = {
    {seq_table_rrc_resume_request1_ies_s_fields, 4, UINT64_C(0xf), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_resume_request1_ies_s_names};

// RRCResumeRequest1 ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_resume_request1_s_fields[] = {
    {offsetof(rrc_resume_request1_s, rrc_resume_request1),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_resume_request1_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_resume_request1_s_names[] = {{"rrcResumeRequest1", nullptr}};
const asn1::table::seq_desc          rrc_resume_request1_s::desc             = {
    {seq_table_rrc_resume_request1_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_resume_request1_s_names};

// UL-CCCH1-MessageType ::= CHOICE
void ul_ccch1_msg_type_c::set(types::options e)
{
  type_ = e;
}
ul_ccch1_msg_type_c::c1_c_& ul_ccch1_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void ul_ccch1_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
static const asn1::table::seq_field choice_table_ul_ccch1_msg_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_ccch1_msg_type_c::c1_c_::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ul_ccch1_msg_type_c_names[] = {{"c1", nullptr},
                                                                                 {"messageClassExtension", nullptr}};
const asn1::table::choice_desc ul_ccch1_msg_type_c::desc = {asn1::table::enum_int_type<ul_ccch1_msg_type_c::types>(),
                                                            ul_ccch1_msg_type_c::types::nof_types,
                                                            ul_ccch1_msg_type_c::types::nof_exts,
                                                            ul_ccch1_msg_type_c::types::has_ext,
                                                            false,
                                                            choice_table_ul_ccch1_msg_type_c_alternatives,
                                                            &asn1::table::choice_set<ul_ccch1_msg_type_c>,
                                                            choice_table_ul_ccch1_msg_type_c_names,
                                                            "ul_ccch1_msg_type_c",
                                                            offsetof(ul_ccch1_msg_type_c, type_),
                                                            offsetof(ul_ccch1_msg_type_c, c),
                                                            false};

void ul_ccch1_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
}
rrc_resume_request1_s& ul_ccch1_msg_type_c::c1_c_::set_rrc_resume_request1()
{
  set(types::rrc_resume_request1);
  return c;
}
void ul_ccch1_msg_type_c::c1_c_::set_spare3()
{
  set(types::spare3);
}
void ul_ccch1_msg_type_c::c1_c_::set_spare2()
{
  set(types::spare2);
}
void ul_ccch1_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_ul_ccch1_msg_type_c_c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_resume_request1_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_ul_ccch1_msg_type_c_c1_c__names[] = {{"rrcResumeRequest1", nullptr},
                                                                                       {"spare3", nullptr},
                                                                                       {"spare2", nullptr},
                                                                                       {"spare1", nullptr}};
const asn1::table::choice_desc       ul_ccch1_msg_type_c::c1_c_::desc               = {
    asn1::table::enum_int_type<ul_ccch1_msg_type_c::c1_c_::types>(),
    ul_ccch1_msg_type_c::c1_c_::types::nof_types,
    ul_ccch1_msg_type_c::c1_c_::types::nof_exts,
    ul_ccch1_msg_type_c::c1_c_::types::has_ext,
    false,
    choice_table_ul_ccch1_msg_type_c_c1_c__alternatives,
    &asn1::table::choice_set<ul_ccch1_msg_type_c::c1_c_>,
    choice_table_ul_ccch1_msg_type_c_c1_c__names,
    "ul_ccch1_msg_type_c::c1_c_",
    offsetof(ul_ccch1_msg_type_c::c1_c_, type_),
    offsetof(ul_ccch1_msg_type_c::c1_c_, c),
    false};

static const char*           enum_names_ul_ccch1_msg_type_c_c1_c__types_values[] = {"rrcResumeRequest1",
                                                                                    "spare3",
                                                                                    "spare2",
                                                                                    "spare1"};
const asn1::table::enum_desc ul_ccch1_msg_type_c::c1_c_::types_opts::desc        = {
    enum_names_ul_ccch1_msg_type_c_c1_c__types_values,
    4,
    "ul_ccch1_msg_type_c::c1_c_::types"};
uint8_t ul_ccch1_msg_type_c::c1_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_ccch1_msg_type_c::c1_c_::types");
}

static const char*           enum_names_ul_ccch1_msg_type_c_types_values[] = {"c1", "messageClassExtension"};
const asn1::table::enum_desc ul_ccch1_msg_type_c::types_opts::desc = {enum_names_ul_ccch1_msg_type_c_types_values,
                                                                      2,
                                                                      "ul_ccch1_msg_type_c::types"};
uint8_t                      ul_ccch1_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_ccch1_msg_type_c::types");
}

// UL-CCCH1-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_ul_ccch1_msg_s_fields[] = {{offsetof(ul_ccch1_msg_s, msg),
                                                                           asn1::table::seq_field::mandatory,
                                                                           asn1::table::field_op::choice,
                                                                           false,
                                                                           false,
                                                                           0,
                                                                           0,
                                                                           nullptr,
                                                                           &ul_ccch1_msg_type_c::desc}};
static const asn1::table::field_name seq_table_ul_ccch1_msg_s_names[]  = {{"message", nullptr}};
const asn1::table::seq_desc ul_ccch1_msg_s::desc = {{seq_table_ul_ccch1_msg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                                    false,
                                                    false,
                                                    nullptr,
                                                    0,
                                                    seq_table_ul_ccch1_msg_s_names,
                                                    "UL-CCCH1-Message"};
