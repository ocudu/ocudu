// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/dl_ccch_msg.h"
#include "ocudu/asn1/asn1_codec.h"
#include "ocudu/asn1/rrc_nr/dl_ccch_msg_ies.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// DL-CCCH-MessageType ::= CHOICE
void dl_ccch_msg_type_c::set(types::options e)
{
  type_ = e;
}
dl_ccch_msg_type_c::c1_c_& dl_ccch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void dl_ccch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
static const asn1::table::seq_field choice_table_dl_ccch_msg_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<dl_ccch_msg_type_c::c1_c_, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_dl_ccch_msg_type_c_names[] = {{"c1", nullptr},
                                                                                {"messageClassExtension", nullptr}};
const asn1::table::choice_desc dl_ccch_msg_type_c::desc = {asn1::table::enum_int_type<dl_ccch_msg_type_c::types>(),
                                                           dl_ccch_msg_type_c::types::nof_types,
                                                           dl_ccch_msg_type_c::types::nof_exts,
                                                           dl_ccch_msg_type_c::types::has_ext,
                                                           false,
                                                           choice_table_dl_ccch_msg_type_c_alternatives,
                                                           &asn1::table::choice_set<dl_ccch_msg_type_c>,
                                                           choice_table_dl_ccch_msg_type_c_names,
                                                           "dl_ccch_msg_type_c",
                                                           offsetof(dl_ccch_msg_type_c, type_),
                                                           offsetof(dl_ccch_msg_type_c, c),
                                                           false};

static const asn1::table::seq_field choice_table_dl_ccch_msg_type_c_c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_reject_s>::value,
     &rrc_reject_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_setup_s>::value,
     &rrc_setup_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_dl_ccch_msg_type_c_c1_c__names[] = {{"rrcReject", nullptr},
                                                                                      {"rrcSetup", nullptr},
                                                                                      {"spare2", nullptr},
                                                                                      {"spare1", nullptr}};
const asn1::table::choice_desc       dl_ccch_msg_type_c::c1_c_::desc               = {
    asn1::table::enum_int_type<dl_ccch_msg_type_c::c1_c_::types>(),
    dl_ccch_msg_type_c::c1_c_::types::nof_types,
    dl_ccch_msg_type_c::c1_c_::types::nof_exts,
    dl_ccch_msg_type_c::c1_c_::types::has_ext,
    false,
    choice_table_dl_ccch_msg_type_c_c1_c__alternatives,
    &asn1::table::choice_set<dl_ccch_msg_type_c::c1_c_>,
    choice_table_dl_ccch_msg_type_c_c1_c__names,
    "dl_ccch_msg_type_c::c1_c_",
    offsetof(dl_ccch_msg_type_c::c1_c_, type_),
    offsetof(dl_ccch_msg_type_c::c1_c_, c),
    false};

void dl_ccch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, dl_ccch_msg_type_c::c1_c_::desc);
}
rrc_reject_s& dl_ccch_msg_type_c::c1_c_::set_rrc_reject()
{
  set(types::rrc_reject);
  return c.get<rrc_reject_s>();
}
rrc_setup_s& dl_ccch_msg_type_c::c1_c_::set_rrc_setup()
{
  set(types::rrc_setup);
  return c.get<rrc_setup_s>();
}
void dl_ccch_msg_type_c::c1_c_::set_spare2()
{
  set(types::spare2);
}
void dl_ccch_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}

static const char* enum_names_dl_ccch_msg_type_c_c1_c__types_values[] = {"rrcReject", "rrcSetup", "spare2", "spare1"};
const asn1::table::enum_desc dl_ccch_msg_type_c::c1_c_::types_opts::desc = {
    enum_names_dl_ccch_msg_type_c_c1_c__types_values,
    4,
    "dl_ccch_msg_type_c::c1_c_::types"};

static const char*           enum_names_dl_ccch_msg_type_c_types_values[] = {"c1", "messageClassExtension"};
const asn1::table::enum_desc dl_ccch_msg_type_c::types_opts::desc         = {enum_names_dl_ccch_msg_type_c_types_values,
                                                                             2,
                                                                             "dl_ccch_msg_type_c::types"};
uint8_t                      dl_ccch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "dl_ccch_msg_type_c::types");
}

// DL-CCCH-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_dl_ccch_msg_s_fields[] = {{offsetof(dl_ccch_msg_s, msg),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::choice,
                                                                          false,
                                                                          false,
                                                                          0,
                                                                          0,
                                                                          nullptr,
                                                                          &dl_ccch_msg_type_c::desc}};
static const asn1::table::field_name seq_table_dl_ccch_msg_s_names[]  = {{"message", nullptr}};
const asn1::table::seq_desc dl_ccch_msg_s::desc = {{seq_table_dl_ccch_msg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                                   false,
                                                   false,
                                                   nullptr,
                                                   0,
                                                   seq_table_dl_ccch_msg_s_names,
                                                   "DL-CCCH-Message"};
