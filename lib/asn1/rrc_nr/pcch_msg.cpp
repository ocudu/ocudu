// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/pcch_msg.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// PCCH-MessageType ::= CHOICE
void pcch_msg_type_c::set(types::options e)
{
  type_ = e;
}
pcch_msg_type_c::c1_c_& pcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void pcch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
static const asn1::table::seq_field  choice_table_pcch_msg_type_c_alternatives[] = {{0,
                                                                                     asn1::table::seq_field::mandatory,
                                                                                     asn1::table::field_op::choice,
                                                                                     false,
                                                                                     false,
                                                                                     0,
                                                                                     0,
                                                                                     nullptr,
                                                                                     &pcch_msg_type_c::c1_c_::desc},
                                                                                    {0,
                                                                                     asn1::table::seq_field::mandatory,
                                                                                     asn1::table::field_op::sequence,
                                                                                     false,
                                                                                     false,
                                                                                     0,
                                                                                     0,
                                                                                     nullptr,
                                                                                     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_pcch_msg_type_c_names[]        = {{"c1", nullptr},
                                                                                    {"messageClassExtension", nullptr}};
const asn1::table::choice_desc       pcch_msg_type_c::desc = {asn1::table::enum_int_type<pcch_msg_type_c::types>(),
                                                              pcch_msg_type_c::types::nof_types,
                                                              pcch_msg_type_c::types::nof_exts,
                                                              pcch_msg_type_c::types::has_ext,
                                                              false,
                                                              choice_table_pcch_msg_type_c_alternatives,
                                                              &asn1::table::choice_set<pcch_msg_type_c>,
                                                              choice_table_pcch_msg_type_c_names,
                                                              "pcch_msg_type_c",
                                                              offsetof(pcch_msg_type_c, type_),
                                                              offsetof(pcch_msg_type_c, c),
                                                              false};

void pcch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
}
paging_s& pcch_msg_type_c::c1_c_::set_paging()
{
  set(types::paging);
  return c;
}
void pcch_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}
static const asn1::table::seq_field choice_table_pcch_msg_type_c_c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &paging_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_pcch_msg_type_c_c1_c__names[] = {{"paging", nullptr},
                                                                                   {"spare1", nullptr}};
const asn1::table::choice_desc       pcch_msg_type_c::c1_c_::desc               = {
    asn1::table::enum_int_type<pcch_msg_type_c::c1_c_::types>(),
    pcch_msg_type_c::c1_c_::types::nof_types,
    pcch_msg_type_c::c1_c_::types::nof_exts,
    pcch_msg_type_c::c1_c_::types::has_ext,
    false,
    choice_table_pcch_msg_type_c_c1_c__alternatives,
    &asn1::table::choice_set<pcch_msg_type_c::c1_c_>,
    choice_table_pcch_msg_type_c_c1_c__names,
    "pcch_msg_type_c::c1_c_",
    offsetof(pcch_msg_type_c::c1_c_, type_),
    offsetof(pcch_msg_type_c::c1_c_, c),
    false};

static const char*           enum_names_pcch_msg_type_c_c1_c__types_values[] = {"paging", "spare1"};
const asn1::table::enum_desc pcch_msg_type_c::c1_c_::types_opts::desc = {enum_names_pcch_msg_type_c_c1_c__types_values,
                                                                         2,
                                                                         "pcch_msg_type_c::c1_c_::types"};

static const char*           enum_names_pcch_msg_type_c_types_values[] = {"c1", "messageClassExtension"};
const asn1::table::enum_desc pcch_msg_type_c::types_opts::desc         = {enum_names_pcch_msg_type_c_types_values,
                                                                          2,
                                                                          "pcch_msg_type_c::types"};
uint8_t                      pcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "pcch_msg_type_c::types");
}

// PCCH-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_pcch_msg_s_fields[] = {{offsetof(pcch_msg_s, msg),
                                                                       asn1::table::seq_field::mandatory,
                                                                       asn1::table::field_op::choice,
                                                                       false,
                                                                       false,
                                                                       0,
                                                                       0,
                                                                       nullptr,
                                                                       &pcch_msg_type_c::desc}};
static const asn1::table::field_name seq_table_pcch_msg_s_names[]  = {{"message", nullptr}};
const asn1::table::seq_desc          pcch_msg_s::desc              = {
    {seq_table_pcch_msg_s_fields, uint64_t{0x1}, nullptr, seq_table_pcch_msg_s_names, 1, 0},
    false,
    false,
    nullptr,
    0,
    "PCCH-Message"};
