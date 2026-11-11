// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/dl_dcch_msg.h"
#include "ocudu/asn1/asn1_codec.h"
#include "ocudu/asn1/rrc_nr/dl_dcch_msg_ies.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// DL-DCCH-MessageType ::= CHOICE
void dl_dcch_msg_type_c::set(types::options e)
{
  type_ = e;
}
dl_dcch_msg_type_c::c1_c_& dl_dcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c;
}
void dl_dcch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
}
static const asn1::table::seq_field choice_table_dl_dcch_msg_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<dl_dcch_msg_type_c::c1_c_, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_dl_dcch_msg_type_c_names[] = {{"c1", nullptr},
                                                                                {"messageClassExtension", nullptr}};
const asn1::table::choice_desc dl_dcch_msg_type_c::desc = {asn1::table::enum_int_type<dl_dcch_msg_type_c::types>(),
                                                           dl_dcch_msg_type_c::types::nof_types,
                                                           dl_dcch_msg_type_c::types::nof_exts,
                                                           dl_dcch_msg_type_c::types::has_ext,
                                                           false,
                                                           choice_table_dl_dcch_msg_type_c_alternatives,
                                                           &asn1::table::choice_set<dl_dcch_msg_type_c>,
                                                           choice_table_dl_dcch_msg_type_c_names,
                                                           "dl_dcch_msg_type_c",
                                                           offsetof(dl_dcch_msg_type_c, type_),
                                                           offsetof(dl_dcch_msg_type_c, c),
                                                           false};

static const asn1::table::seq_field choice_table_dl_dcch_msg_type_c_c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_recfg_s>::value,
     &rrc_recfg_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_resume_s>::value,
     &rrc_resume_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_release_s>::value,
     &rrc_release_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_reest_s>::value,
     &rrc_reest_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<security_mode_cmd_s>::value,
     &security_mode_cmd_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<dl_info_transfer_s>::value,
     &dl_info_transfer_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ue_cap_enquiry_s>::value,
     &ue_cap_enquiry_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<counter_check_s>::value,
     &counter_check_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<mob_from_nr_cmd_s>::value,
     &mob_from_nr_cmd_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<dl_ded_msg_segment_r16_s>::value,
     &dl_ded_msg_segment_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ue_info_request_r16_s>::value,
     &ue_info_request_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<dl_info_transfer_mrdc_r16_s>::value,
     &dl_info_transfer_mrdc_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<logged_meas_cfg_r16_s>::value,
     &logged_meas_cfg_r16_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_dl_dcch_msg_type_c_c1_c__names[] = {
    {"rrcReconfiguration", nullptr},
    {"rrcResume", nullptr},
    {"rrcRelease", nullptr},
    {"rrcReestablishment", nullptr},
    {"securityModeCommand", nullptr},
    {"dlInformationTransfer", nullptr},
    {"ueCapabilityEnquiry", nullptr},
    {"counterCheck", nullptr},
    {"mobilityFromNRCommand", nullptr},
    {"dlDedicatedMessageSegment-r16", nullptr},
    {"ueInformationRequest-r16", nullptr},
    {"dlInformationTransferMRDC-r16", nullptr},
    {"loggedMeasurementConfiguration-r16", nullptr},
    {"spare3", nullptr},
    {"spare2", nullptr},
    {"spare1", nullptr}};
const asn1::table::choice_desc dl_dcch_msg_type_c::c1_c_::desc = {
    asn1::table::enum_int_type<dl_dcch_msg_type_c::c1_c_::types>(),
    dl_dcch_msg_type_c::c1_c_::types::nof_types,
    dl_dcch_msg_type_c::c1_c_::types::nof_exts,
    dl_dcch_msg_type_c::c1_c_::types::has_ext,
    false,
    choice_table_dl_dcch_msg_type_c_c1_c__alternatives,
    &asn1::table::choice_set<dl_dcch_msg_type_c::c1_c_>,
    choice_table_dl_dcch_msg_type_c_c1_c__names,
    "dl_dcch_msg_type_c::c1_c_",
    offsetof(dl_dcch_msg_type_c::c1_c_, type_),
    offsetof(dl_dcch_msg_type_c::c1_c_, c),
    false};

void dl_dcch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, dl_dcch_msg_type_c::c1_c_::desc);
}
rrc_recfg_s& dl_dcch_msg_type_c::c1_c_::set_rrc_recfg()
{
  set(types::rrc_recfg);
  return c.get<rrc_recfg_s>();
}
rrc_resume_s& dl_dcch_msg_type_c::c1_c_::set_rrc_resume()
{
  set(types::rrc_resume);
  return c.get<rrc_resume_s>();
}
rrc_release_s& dl_dcch_msg_type_c::c1_c_::set_rrc_release()
{
  set(types::rrc_release);
  return c.get<rrc_release_s>();
}
rrc_reest_s& dl_dcch_msg_type_c::c1_c_::set_rrc_reest()
{
  set(types::rrc_reest);
  return c.get<rrc_reest_s>();
}
security_mode_cmd_s& dl_dcch_msg_type_c::c1_c_::set_security_mode_cmd()
{
  set(types::security_mode_cmd);
  return c.get<security_mode_cmd_s>();
}
dl_info_transfer_s& dl_dcch_msg_type_c::c1_c_::set_dl_info_transfer()
{
  set(types::dl_info_transfer);
  return c.get<dl_info_transfer_s>();
}
ue_cap_enquiry_s& dl_dcch_msg_type_c::c1_c_::set_ue_cap_enquiry()
{
  set(types::ue_cap_enquiry);
  return c.get<ue_cap_enquiry_s>();
}
counter_check_s& dl_dcch_msg_type_c::c1_c_::set_counter_check()
{
  set(types::counter_check);
  return c.get<counter_check_s>();
}
mob_from_nr_cmd_s& dl_dcch_msg_type_c::c1_c_::set_mob_from_nr_cmd()
{
  set(types::mob_from_nr_cmd);
  return c.get<mob_from_nr_cmd_s>();
}
dl_ded_msg_segment_r16_s& dl_dcch_msg_type_c::c1_c_::set_dl_ded_msg_segment_r16()
{
  set(types::dl_ded_msg_segment_r16);
  return c.get<dl_ded_msg_segment_r16_s>();
}
ue_info_request_r16_s& dl_dcch_msg_type_c::c1_c_::set_ue_info_request_r16()
{
  set(types::ue_info_request_r16);
  return c.get<ue_info_request_r16_s>();
}
dl_info_transfer_mrdc_r16_s& dl_dcch_msg_type_c::c1_c_::set_dl_info_transfer_mrdc_r16()
{
  set(types::dl_info_transfer_mrdc_r16);
  return c.get<dl_info_transfer_mrdc_r16_s>();
}
logged_meas_cfg_r16_s& dl_dcch_msg_type_c::c1_c_::set_logged_meas_cfg_r16()
{
  set(types::logged_meas_cfg_r16);
  return c.get<logged_meas_cfg_r16_s>();
}
void dl_dcch_msg_type_c::c1_c_::set_spare3()
{
  set(types::spare3);
}
void dl_dcch_msg_type_c::c1_c_::set_spare2()
{
  set(types::spare2);
}
void dl_dcch_msg_type_c::c1_c_::set_spare1()
{
  set(types::spare1);
}

static const char*           enum_names_dl_dcch_msg_type_c_c1_c__types_values[] = {"rrcReconfiguration",
                                                                                   "rrcResume",
                                                                                   "rrcRelease",
                                                                                   "rrcReestablishment",
                                                                                   "securityModeCommand",
                                                                                   "dlInformationTransfer",
                                                                                   "ueCapabilityEnquiry",
                                                                                   "counterCheck",
                                                                                   "mobilityFromNRCommand",
                                                                                   "dlDedicatedMessageSegment-r16",
                                                                                   "ueInformationRequest-r16",
                                                                                   "dlInformationTransferMRDC-r16",
                                                                                   "loggedMeasurementConfiguration-r16",
                                                                                   "spare3",
                                                                                   "spare2",
                                                                                   "spare1"};
const asn1::table::enum_desc dl_dcch_msg_type_c::c1_c_::types_opts::desc        = {
    enum_names_dl_dcch_msg_type_c_c1_c__types_values,
    16,
    "dl_dcch_msg_type_c::c1_c_::types"};

static const char*           enum_names_dl_dcch_msg_type_c_types_values[] = {"c1", "messageClassExtension"};
const asn1::table::enum_desc dl_dcch_msg_type_c::types_opts::desc         = {enum_names_dl_dcch_msg_type_c_types_values,
                                                                             2,
                                                                             "dl_dcch_msg_type_c::types"};
uint8_t                      dl_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "dl_dcch_msg_type_c::types");
}

// DL-DCCH-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_dl_dcch_msg_s_fields[] = {{offsetof(dl_dcch_msg_s, msg),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::choice,
                                                                          false,
                                                                          false,
                                                                          0,
                                                                          0,
                                                                          nullptr,
                                                                          &dl_dcch_msg_type_c::desc}};
static const asn1::table::field_name seq_table_dl_dcch_msg_s_names[]  = {{"message", nullptr}};
const asn1::table::seq_desc dl_dcch_msg_s::desc = {{seq_table_dl_dcch_msg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                                   false,
                                                   false,
                                                   nullptr,
                                                   0,
                                                   seq_table_dl_dcch_msg_s_names,
                                                   "DL-DCCH-Message"};
