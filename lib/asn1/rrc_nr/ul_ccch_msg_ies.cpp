// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/ul_ccch_msg_ies.h"
#include "ocudu/asn1/asn1_codec.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// EstablishmentCause ::= ENUMERATED
static const char*           enum_names_establishment_cause_e_values[] = {"emergency",
                                                                          "highPriorityAccess",
                                                                          "mt-Access",
                                                                          "mo-Signalling",
                                                                          "mo-Data",
                                                                          "mo-VoiceCall",
                                                                          "mo-VideoCall",
                                                                          "mo-SMS",
                                                                          "mps-PriorityAccess",
                                                                          "mcs-PriorityAccess",
                                                                          "spare6",
                                                                          "spare5",
                                                                          "spare4",
                                                                          "spare3",
                                                                          "spare2",
                                                                          "spare1"};
const asn1::table::enum_desc establishment_cause_opts::desc            = {enum_names_establishment_cause_e_values,
                                                                          16,
                                                                          "establishment_cause_e"};

// InitialUE-Identity ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_init_ue_id_c_alternatives[] = {
    &asn1::table::alternative_ops<fixed_bitstring<39>, false>::value,
    &asn1::table::alternative_ops<fixed_bitstring<39>, false>::value};
const asn1::table::choice_storage_desc init_ue_id_c::storage_desc = {choice_storage_init_ue_id_c_alternatives,
                                                                     2,
                                                                     "init_ue_id_c"};

fixed_bitstring<39>& init_ue_id_c::set_ng_5_g_s_tmsi_part1()
{
  set(types::ng_5_g_s_tmsi_part1);
  return c.get<fixed_bitstring<39>>();
}
fixed_bitstring<39>& init_ue_id_c::set_random_value()
{
  set(types::random_value);
  return c.get<fixed_bitstring<39>>();
}
static const asn1::table::seq_field choice_table_init_ue_id_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<39>, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<fixed_bitstring<39>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_init_ue_id_c_names[] = {{"ng-5G-S-TMSI-Part1", nullptr},
                                                                          {"randomValue", nullptr}};
const asn1::table::choice_desc       init_ue_id_c::desc = {asn1::table::enum_int_type<init_ue_id_c::types>(),
                                                           init_ue_id_c::types::nof_types,
                                                           init_ue_id_c::types::nof_exts,
                                                           init_ue_id_c::types::has_ext,
                                                           false,
                                                           choice_table_init_ue_id_c_alternatives,
                                                           nullptr,
                                                           choice_table_init_ue_id_c_names,
                                                           "init_ue_id_c",
                                                           offsetof(init_ue_id_c, c.type),
                                                           offsetof(init_ue_id_c, c.value),
                                                           false,
                                                           &init_ue_id_c::storage_desc};

static const char*           enum_names_init_ue_id_c_types_values[] = {"ng-5G-S-TMSI-Part1", "randomValue"};
const asn1::table::enum_desc init_ue_id_c::types_opts::desc         = {enum_names_init_ue_id_c_types_values,
                                                                       2,
                                                                       "init_ue_id_c::types"};
int8_t                       init_ue_id_c::types_opts::to_number() const
{
  static const int8_t numbers[] = {-5};
  return map_enum_number(numbers, 1, value, "init_ue_id_c::types");
}

// RRC-PosSystemInfoRequest-r16-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_pos_sys_info_request_r16_ies_s_fields[] = {
    {offsetof(rrc_pos_sys_info_request_r16_ies_s, requested_pos_si_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_pos_sys_info_request_r16_ies_s::requested_pos_si_list),
                                asn1::table::struct_json>::value},
    {offsetof(rrc_pos_sys_info_request_r16_ies_s, spare),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_pos_sys_info_request_r16_ies_s::spare), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_rrc_pos_sys_info_request_r16_ies_s_names[] = {
    {"requestedPosSI-List", nullptr},
    {"spare", nullptr}};
const asn1::table::seq_desc rrc_pos_sys_info_request_r16_ies_s::desc = {
    {seq_table_rrc_pos_sys_info_request_r16_ies_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_pos_sys_info_request_r16_ies_s_names};

// ReestabUE-Identity ::= SEQUENCE
static const asn1::table::seq_field seq_table_reestab_ue_id_s_fields[] = {
    {offsetof(reestab_ue_id_s, c_rnti),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u32,
     false,
     false,
     INT64_C(0),
     UINT64_C(65535),
     nullptr},
    {offsetof(reestab_ue_id_s, pci),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     false,
     INT64_C(0),
     UINT64_C(1007),
     nullptr},
    {offsetof(reestab_ue_id_s, short_mac_i),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(reestab_ue_id_s::short_mac_i), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_reestab_ue_id_s_names[] = {{"c-RNTI", nullptr},
                                                                          {"physCellId", nullptr},
                                                                          {"shortMAC-I", nullptr}};
const asn1::table::seq_desc reestab_ue_id_s::desc = {{seq_table_reestab_ue_id_s_fields, 3, UINT64_C(0x7), nullptr, 0},
                                                     false,
                                                     false,
                                                     nullptr,
                                                     0,
                                                     seq_table_reestab_ue_id_s_names};

// ReestablishmentCause ::= ENUMERATED
static const char*           enum_names_reest_cause_e_values[] = {"reconfigurationFailure",
                                                                  "handoverFailure",
                                                                  "otherFailure",
                                                                  "spare1"};
const asn1::table::enum_desc reest_cause_opts::desc            = {enum_names_reest_cause_e_values, 4, "reest_cause_e"};

// RRCReestablishmentRequest-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_reest_request_ies_s_fields[] = {
    {offsetof(rrc_reest_request_ies_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &reestab_ue_id_s::desc},
    {offsetof(rrc_reest_request_ies_s, reest_cause),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rrc_reest_request_ies_s::reest_cause)>(),
     decltype(rrc_reest_request_ies_s::reest_cause)::has_ext,
     false,
     decltype(rrc_reest_request_ies_s::reest_cause)::nof_types,
     decltype(rrc_reest_request_ies_s::reest_cause)::nof_exts,
     nullptr,
     &reest_cause_opts::desc},
    {offsetof(rrc_reest_request_ies_s, spare),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_reest_request_ies_s::spare), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_rrc_reest_request_ies_s_names[] = {{"ue-Identity", nullptr},
                                                                                  {"reestablishmentCause", nullptr},
                                                                                  {"spare", nullptr}};
const asn1::table::seq_desc          rrc_reest_request_ies_s::desc             = {
    {seq_table_rrc_reest_request_ies_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_reest_request_ies_s_names};

// RRCReestablishmentRequest ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_reest_request_s_fields[] = {
    {offsetof(rrc_reest_request_s, rrc_reest_request),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_reest_request_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_reest_request_s_names[] = {{"rrcReestablishmentRequest", nullptr}};
const asn1::table::seq_desc          rrc_reest_request_s::desc             = {
    {seq_table_rrc_reest_request_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_reest_request_s_names};

// ResumeCause ::= ENUMERATED
static const char*           enum_names_resume_cause_e_values[] = {"emergency",
                                                                   "highPriorityAccess",
                                                                   "mt-Access",
                                                                   "mo-Signalling",
                                                                   "mo-Data",
                                                                   "mo-VoiceCall",
                                                                   "mo-VideoCall",
                                                                   "mo-SMS",
                                                                   "rna-Update",
                                                                   "mps-PriorityAccess",
                                                                   "mcs-PriorityAccess",
                                                                   "mt-SDT-v1810",
                                                                   "srs-PosConfigOrActivationReq-v1800",
                                                                   "spare3",
                                                                   "spare2",
                                                                   "spare1"};
const asn1::table::enum_desc resume_cause_opts::desc = {enum_names_resume_cause_e_values, 16, "resume_cause_e"};

// RRCResumeRequest-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_resume_request_ies_s_fields[] = {
    {offsetof(rrc_resume_request_ies_s, resume_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_resume_request_ies_s::resume_id), asn1::table::struct_json>::value},
    {offsetof(rrc_resume_request_ies_s, resume_mac_i),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_resume_request_ies_s::resume_mac_i), asn1::table::struct_json>::value},
    {offsetof(rrc_resume_request_ies_s, resume_cause),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rrc_resume_request_ies_s::resume_cause)>(),
     decltype(rrc_resume_request_ies_s::resume_cause)::has_ext,
     false,
     decltype(rrc_resume_request_ies_s::resume_cause)::nof_types,
     decltype(rrc_resume_request_ies_s::resume_cause)::nof_exts,
     nullptr,
     &resume_cause_opts::desc},
    {offsetof(rrc_resume_request_ies_s, spare),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_resume_request_ies_s::spare), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_rrc_resume_request_ies_s_names[] = {{"resumeIdentity", nullptr},
                                                                                   {"resumeMAC-I", nullptr},
                                                                                   {"resumeCause", nullptr},
                                                                                   {"spare", nullptr}};
const asn1::table::seq_desc          rrc_resume_request_ies_s::desc             = {
    {seq_table_rrc_resume_request_ies_s_fields, 4, UINT64_C(0xf), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_resume_request_ies_s_names};

// RRCResumeRequest ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_resume_request_s_fields[] = {
    {offsetof(rrc_resume_request_s, rrc_resume_request),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_resume_request_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_resume_request_s_names[] = {{"rrcResumeRequest", nullptr}};
const asn1::table::seq_desc          rrc_resume_request_s::desc             = {
    {seq_table_rrc_resume_request_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_resume_request_s_names};

// RRCSetupRequest-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_setup_request_ies_s_fields[] = {
    {offsetof(rrc_setup_request_ies_s, ue_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &init_ue_id_c::desc},
    {offsetof(rrc_setup_request_ies_s, establishment_cause),
     asn1::table::seq_field::mandatory,
     asn1::table::enum_op<decltype(rrc_setup_request_ies_s::establishment_cause)>(),
     decltype(rrc_setup_request_ies_s::establishment_cause)::has_ext,
     false,
     decltype(rrc_setup_request_ies_s::establishment_cause)::nof_types,
     decltype(rrc_setup_request_ies_s::establishment_cause)::nof_exts,
     nullptr,
     &establishment_cause_opts::desc},
    {offsetof(rrc_setup_request_ies_s, spare),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_setup_request_ies_s::spare), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_rrc_setup_request_ies_s_names[] = {{"ue-Identity", nullptr},
                                                                                  {"establishmentCause", nullptr},
                                                                                  {"spare", nullptr}};
const asn1::table::seq_desc          rrc_setup_request_ies_s::desc             = {
    {seq_table_rrc_setup_request_ies_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_setup_request_ies_s_names};

// RRCSetupRequest ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_setup_request_s_fields[] = {
    {offsetof(rrc_setup_request_s, rrc_setup_request),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_setup_request_ies_s::desc}};
static const asn1::table::field_name seq_table_rrc_setup_request_s_names[] = {{"rrcSetupRequest", nullptr}};
const asn1::table::seq_desc          rrc_setup_request_s::desc             = {
    {seq_table_rrc_setup_request_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_setup_request_s_names};

// RRCSystemInfoRequest-IEs ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_sys_info_request_ies_s_fields[] = {
    {offsetof(rrc_sys_info_request_ies_s, requested_si_list),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_sys_info_request_ies_s::requested_si_list),
                                asn1::table::struct_json>::value},
    {offsetof(rrc_sys_info_request_ies_s, spare),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<decltype(rrc_sys_info_request_ies_s::spare), asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_rrc_sys_info_request_ies_s_names[] = {{"requested-SI-List", nullptr},
                                                                                     {"spare", nullptr}};
const asn1::table::seq_desc          rrc_sys_info_request_ies_s::desc             = {
    {seq_table_rrc_sys_info_request_ies_s_fields, 2, UINT64_C(0x3), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_sys_info_request_ies_s_names};

// RRCSystemInfoRequest ::= SEQUENCE
static const asn1::table::seq_field seq_table_rrc_sys_info_request_s_fields[] = {
    {offsetof(rrc_sys_info_request_s, crit_exts),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_sys_info_request_s::crit_exts_c_::desc}};
static const asn1::table::field_name seq_table_rrc_sys_info_request_s_names[] = {{"criticalExtensions", nullptr}};
const asn1::table::seq_desc          rrc_sys_info_request_s::desc             = {
    {seq_table_rrc_sys_info_request_s_fields, 1, UINT64_C(0x1), nullptr, 0},
    false,
    false,
    nullptr,
    0,
    seq_table_rrc_sys_info_request_s_names};

static const asn1::table::storage_ops* const choice_storage_rrc_sys_info_request_s_crit_exts_c__alternatives[] = {
    &asn1::table::alternative_ops<rrc_sys_info_request_ies_s, false>::value,
    &asn1::table::alternative_ops<rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_, false>::value};
const asn1::table::choice_storage_desc rrc_sys_info_request_s::crit_exts_c_::storage_desc = {
    choice_storage_rrc_sys_info_request_s_crit_exts_c__alternatives,
    2,
    "rrc_sys_info_request_s::crit_exts_c_"};

rrc_sys_info_request_ies_s& rrc_sys_info_request_s::crit_exts_c_::set_rrc_sys_info_request()
{
  set(types::rrc_sys_info_request);
  return c.get<rrc_sys_info_request_ies_s>();
}
rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_&
rrc_sys_info_request_s::crit_exts_c_::set_crit_exts_future_r16()
{
  set(types::crit_exts_future_r16);
  return c.get<crit_exts_future_r16_c_>();
}
static const asn1::table::seq_field choice_table_rrc_sys_info_request_s_crit_exts_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_sys_info_request_ies_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::desc}};
static const asn1::table::field_name choice_table_rrc_sys_info_request_s_crit_exts_c__names[] = {
    {"rrcSystemInfoRequest", nullptr},
    {"criticalExtensionsFuture-r16", nullptr}};
const asn1::table::choice_desc rrc_sys_info_request_s::crit_exts_c_::desc = {
    asn1::table::enum_int_type<rrc_sys_info_request_s::crit_exts_c_::types>(),
    rrc_sys_info_request_s::crit_exts_c_::types::nof_types,
    rrc_sys_info_request_s::crit_exts_c_::types::nof_exts,
    rrc_sys_info_request_s::crit_exts_c_::types::has_ext,
    false,
    choice_table_rrc_sys_info_request_s_crit_exts_c__alternatives,
    nullptr,
    choice_table_rrc_sys_info_request_s_crit_exts_c__names,
    "rrc_sys_info_request_s::crit_exts_c_",
    offsetof(rrc_sys_info_request_s::crit_exts_c_, c.type),
    offsetof(rrc_sys_info_request_s::crit_exts_c_, c.value),
    false,
    &rrc_sys_info_request_s::crit_exts_c_::storage_desc};

void rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::set(types::options e)
{
  type_ = e;
}
rrc_pos_sys_info_request_r16_ies_s&
rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::set_rrc_pos_sys_info_request_r16()
{
  set(types::rrc_pos_sys_info_request_r16);
  return c;
}
void rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::set_crit_exts_future()
{
  set(types::crit_exts_future);
}
static const asn1::table::seq_field
    choice_table_rrc_sys_info_request_s_crit_exts_c__crit_exts_future_r16_c__alternatives[] = {
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &rrc_pos_sys_info_request_r16_ies_s::desc},
        {0,
         asn1::table::seq_field::mandatory,
         asn1::table::field_op::sequence,
         false,
         false,
         0,
         0,
         nullptr,
         &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_rrc_sys_info_request_s_crit_exts_c__crit_exts_future_r16_c__names[] =
    {{"rrcPosSystemInfoRequest-r16", nullptr}, {"criticalExtensionsFuture", nullptr}};
const asn1::table::choice_desc rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::desc = {
    asn1::table::enum_int_type<rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::types>(),
    rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::types::nof_types,
    rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::types::nof_exts,
    rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::types::has_ext,
    false,
    choice_table_rrc_sys_info_request_s_crit_exts_c__crit_exts_future_r16_c__alternatives,
    &asn1::table::choice_set<rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_>,
    choice_table_rrc_sys_info_request_s_crit_exts_c__crit_exts_future_r16_c__names,
    "rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_",
    offsetof(rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_, type_),
    offsetof(rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_, c),
    false};

static const char* enum_names_rrc_sys_info_request_s_crit_exts_c__crit_exts_future_r16_c__types_values[] = {
    "rrcPosSystemInfoRequest-r16",
    "criticalExtensionsFuture"};
const asn1::table::enum_desc rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::types_opts::desc = {
    enum_names_rrc_sys_info_request_s_crit_exts_c__crit_exts_future_r16_c__types_values,
    2,
    "rrc_sys_info_request_s::crit_exts_c_::crit_exts_future_r16_c_::types"};

static const char*           enum_names_rrc_sys_info_request_s_crit_exts_c__types_values[] = {"rrcSystemInfoRequest",
                                                                                              "criticalExtensionsFuture-r16"};
const asn1::table::enum_desc rrc_sys_info_request_s::crit_exts_c_::types_opts::desc        = {
    enum_names_rrc_sys_info_request_s_crit_exts_c__types_values,
    2,
    "rrc_sys_info_request_s::crit_exts_c_::types"};
