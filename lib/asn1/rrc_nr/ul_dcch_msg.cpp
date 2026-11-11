// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/rrc_nr/ul_dcch_msg.h"
#include "ocudu/asn1/asn1_codec.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"

using namespace asn1;
using namespace asn1::rrc_nr;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// UL-DCCH-MessageType ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ul_dcch_msg_type_c_alternatives[] = {
    &asn1::table::alternative_ops<ul_dcch_msg_type_c::c1_c_, false>::value,
    &asn1::table::alternative_ops<ul_dcch_msg_type_c::msg_class_ext_c_, false>::value};
const asn1::table::choice_storage_desc ul_dcch_msg_type_c::storage_desc = {
    choice_storage_ul_dcch_msg_type_c_alternatives,
    2,
    "ul_dcch_msg_type_c"};

ul_dcch_msg_type_c::c1_c_& ul_dcch_msg_type_c::set_c1()
{
  set(types::c1);
  return c.get<c1_c_>();
}
ul_dcch_msg_type_c::msg_class_ext_c_& ul_dcch_msg_type_c::set_msg_class_ext()
{
  set(types::msg_class_ext);
  return c.get<msg_class_ext_c_>();
}
static const asn1::table::seq_field choice_table_ul_dcch_msg_type_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<ul_dcch_msg_type_c::c1_c_, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::choice,
     false,
     false,
     0,
     0,
     nullptr,
     &ul_dcch_msg_type_c::msg_class_ext_c_::desc}};
static const asn1::table::field_name choice_table_ul_dcch_msg_type_c_names[] = {{"c1", nullptr},
                                                                                {"messageClassExtension", nullptr}};
const asn1::table::choice_desc ul_dcch_msg_type_c::desc = {asn1::table::enum_int_type<ul_dcch_msg_type_c::types>(),
                                                           ul_dcch_msg_type_c::types::nof_types,
                                                           ul_dcch_msg_type_c::types::nof_exts,
                                                           ul_dcch_msg_type_c::types::has_ext,
                                                           false,
                                                           choice_table_ul_dcch_msg_type_c_alternatives,
                                                           nullptr,
                                                           choice_table_ul_dcch_msg_type_c_names,
                                                           "ul_dcch_msg_type_c",
                                                           offsetof(ul_dcch_msg_type_c, c.type),
                                                           offsetof(ul_dcch_msg_type_c, c.value),
                                                           false,
                                                           &ul_dcch_msg_type_c::storage_desc};

static const asn1::table::seq_field choice_table_ul_dcch_msg_type_c_c1_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<meas_report_s>::value,
     &meas_report_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_recfg_complete_s>::value,
     &rrc_recfg_complete_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_setup_complete_s>::value,
     &rrc_setup_complete_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_reest_complete_s>::value,
     &rrc_reest_complete_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<rrc_resume_complete_s>::value,
     &rrc_resume_complete_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<security_mode_complete_s>::value,
     &security_mode_complete_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<security_mode_fail_s>::value,
     &security_mode_fail_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ul_info_transfer_s>::value,
     &ul_info_transfer_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<location_meas_ind_s>::value,
     &location_meas_ind_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ue_cap_info_s>::value,
     &ue_cap_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<counter_check_resp_s>::value,
     &counter_check_resp_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ue_assist_info_s>::value,
     &ue_assist_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<fail_info_s>::value,
     &fail_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ul_info_transfer_mrdc_s>::value,
     &ul_info_transfer_mrdc_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<scg_fail_info_s>::value,
     &scg_fail_info_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<scg_fail_info_eutra_s>::value,
     &scg_fail_info_eutra_s::desc}};
static const asn1::table::field_name choice_table_ul_dcch_msg_type_c_c1_c__names[] = {
    {"measurementReport", nullptr},
    {"rrcReconfigurationComplete", nullptr},
    {"rrcSetupComplete", nullptr},
    {"rrcReestablishmentComplete", nullptr},
    {"rrcResumeComplete", nullptr},
    {"securityModeComplete", nullptr},
    {"securityModeFailure", nullptr},
    {"ulInformationTransfer", nullptr},
    {"locationMeasurementIndication", nullptr},
    {"ueCapabilityInformation", nullptr},
    {"counterCheckResponse", nullptr},
    {"ueAssistanceInformation", nullptr},
    {"failureInformation", nullptr},
    {"ulInformationTransferMRDC", nullptr},
    {"scgFailureInformation", nullptr},
    {"scgFailureInformationEUTRA", nullptr}};
const asn1::table::choice_desc ul_dcch_msg_type_c::c1_c_::desc = {
    asn1::table::enum_int_type<ul_dcch_msg_type_c::c1_c_::types>(),
    ul_dcch_msg_type_c::c1_c_::types::nof_types,
    ul_dcch_msg_type_c::c1_c_::types::nof_exts,
    ul_dcch_msg_type_c::c1_c_::types::has_ext,
    false,
    choice_table_ul_dcch_msg_type_c_c1_c__alternatives,
    &asn1::table::choice_set<ul_dcch_msg_type_c::c1_c_>,
    choice_table_ul_dcch_msg_type_c_c1_c__names,
    "ul_dcch_msg_type_c::c1_c_",
    offsetof(ul_dcch_msg_type_c::c1_c_, type_),
    offsetof(ul_dcch_msg_type_c::c1_c_, c),
    false};

void ul_dcch_msg_type_c::c1_c_::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, ul_dcch_msg_type_c::c1_c_::desc);
}
meas_report_s& ul_dcch_msg_type_c::c1_c_::set_meas_report()
{
  set(types::meas_report);
  return c.get<meas_report_s>();
}
rrc_recfg_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_recfg_complete()
{
  set(types::rrc_recfg_complete);
  return c.get<rrc_recfg_complete_s>();
}
rrc_setup_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_setup_complete()
{
  set(types::rrc_setup_complete);
  return c.get<rrc_setup_complete_s>();
}
rrc_reest_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_reest_complete()
{
  set(types::rrc_reest_complete);
  return c.get<rrc_reest_complete_s>();
}
rrc_resume_complete_s& ul_dcch_msg_type_c::c1_c_::set_rrc_resume_complete()
{
  set(types::rrc_resume_complete);
  return c.get<rrc_resume_complete_s>();
}
security_mode_complete_s& ul_dcch_msg_type_c::c1_c_::set_security_mode_complete()
{
  set(types::security_mode_complete);
  return c.get<security_mode_complete_s>();
}
security_mode_fail_s& ul_dcch_msg_type_c::c1_c_::set_security_mode_fail()
{
  set(types::security_mode_fail);
  return c.get<security_mode_fail_s>();
}
ul_info_transfer_s& ul_dcch_msg_type_c::c1_c_::set_ul_info_transfer()
{
  set(types::ul_info_transfer);
  return c.get<ul_info_transfer_s>();
}
location_meas_ind_s& ul_dcch_msg_type_c::c1_c_::set_location_meas_ind()
{
  set(types::location_meas_ind);
  return c.get<location_meas_ind_s>();
}
ue_cap_info_s& ul_dcch_msg_type_c::c1_c_::set_ue_cap_info()
{
  set(types::ue_cap_info);
  return c.get<ue_cap_info_s>();
}
counter_check_resp_s& ul_dcch_msg_type_c::c1_c_::set_counter_check_resp()
{
  set(types::counter_check_resp);
  return c.get<counter_check_resp_s>();
}
ue_assist_info_s& ul_dcch_msg_type_c::c1_c_::set_ue_assist_info()
{
  set(types::ue_assist_info);
  return c.get<ue_assist_info_s>();
}
fail_info_s& ul_dcch_msg_type_c::c1_c_::set_fail_info()
{
  set(types::fail_info);
  return c.get<fail_info_s>();
}
ul_info_transfer_mrdc_s& ul_dcch_msg_type_c::c1_c_::set_ul_info_transfer_mrdc()
{
  set(types::ul_info_transfer_mrdc);
  return c.get<ul_info_transfer_mrdc_s>();
}
scg_fail_info_s& ul_dcch_msg_type_c::c1_c_::set_scg_fail_info()
{
  set(types::scg_fail_info);
  return c.get<scg_fail_info_s>();
}
scg_fail_info_eutra_s& ul_dcch_msg_type_c::c1_c_::set_scg_fail_info_eutra()
{
  set(types::scg_fail_info_eutra);
  return c.get<scg_fail_info_eutra_s>();
}

static const char*           enum_names_ul_dcch_msg_type_c_c1_c__types_values[] = {"measurementReport",
                                                                                   "rrcReconfigurationComplete",
                                                                                   "rrcSetupComplete",
                                                                                   "rrcReestablishmentComplete",
                                                                                   "rrcResumeComplete",
                                                                                   "securityModeComplete",
                                                                                   "securityModeFailure",
                                                                                   "ulInformationTransfer",
                                                                                   "locationMeasurementIndication",
                                                                                   "ueCapabilityInformation",
                                                                                   "counterCheckResponse",
                                                                                   "ueAssistanceInformation",
                                                                                   "failureInformation",
                                                                                   "ulInformationTransferMRDC",
                                                                                   "scgFailureInformation",
                                                                                   "scgFailureInformationEUTRA"};
const asn1::table::enum_desc ul_dcch_msg_type_c::c1_c_::types_opts::desc        = {
    enum_names_ul_dcch_msg_type_c_c1_c__types_values,
    16,
    "ul_dcch_msg_type_c::c1_c_::types"};

void ul_dcch_msg_type_c::msg_class_ext_c_::set(types::options e)
{
  type_ = e;
}
ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_& ul_dcch_msg_type_c::msg_class_ext_c_::set_c2()
{
  set(types::c2);
  return c;
}
void ul_dcch_msg_type_c::msg_class_ext_c_::set_msg_class_ext_future_r16()
{
  set(types::msg_class_ext_future_r16);
}
static const asn1::table::seq_field choice_table_ul_dcch_msg_type_c_msg_class_ext_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     nullptr,
     &asn1::table::empty_sequence}};
static const asn1::table::field_name choice_table_ul_dcch_msg_type_c_msg_class_ext_c__names[] = {
    {"c2", nullptr},
    {"messageClassExtensionFuture-r16", nullptr}};
const asn1::table::choice_desc ul_dcch_msg_type_c::msg_class_ext_c_::desc = {
    asn1::table::enum_int_type<ul_dcch_msg_type_c::msg_class_ext_c_::types>(),
    ul_dcch_msg_type_c::msg_class_ext_c_::types::nof_types,
    ul_dcch_msg_type_c::msg_class_ext_c_::types::nof_exts,
    ul_dcch_msg_type_c::msg_class_ext_c_::types::has_ext,
    false,
    choice_table_ul_dcch_msg_type_c_msg_class_ext_c__alternatives,
    &asn1::table::choice_set<ul_dcch_msg_type_c::msg_class_ext_c_>,
    choice_table_ul_dcch_msg_type_c_msg_class_ext_c__names,
    "ul_dcch_msg_type_c::msg_class_ext_c_",
    offsetof(ul_dcch_msg_type_c::msg_class_ext_c_, type_),
    offsetof(ul_dcch_msg_type_c::msg_class_ext_c_, c),
    false};

static const asn1::table::seq_field choice_table_ul_dcch_msg_type_c_msg_class_ext_c__c2_c__alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ul_ded_msg_segment_r16_s>::value,
     &ul_ded_msg_segment_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ded_sib_request_r16_s>::value,
     &ded_sib_request_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<mcg_fail_info_r16_s>::value,
     &mcg_fail_info_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ue_info_resp_r16_s>::value,
     &ue_info_resp_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<sidelink_ue_info_nr_r16_s>::value,
     &sidelink_ue_info_nr_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ul_info_transfer_irat_r16_s>::value,
     &ul_info_transfer_irat_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<iab_other_info_r16_s>::value,
     &iab_other_info_r16_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<mb_si_nterest_ind_r17_s>::value,
     &mb_si_nterest_ind_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<ue_positioning_assist_info_r17_s>::value,
     &ue_positioning_assist_info_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<measurement_report_app_layer_r17_s>::value,
     &measurement_report_app_layer_r17_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::sequence,
     false,
     false,
     0,
     0,
     &asn1::table::any_value_codec<indirect_path_fail_info_r18_s>::value,
     &indirect_path_fail_info_r18_s::desc},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr},
    {0, asn1::table::seq_field::mandatory, asn1::table::field_op::none, false, false, 0, 0, nullptr}};
static const asn1::table::field_name choice_table_ul_dcch_msg_type_c_msg_class_ext_c__c2_c__names[] = {
    {"ulDedicatedMessageSegment-r16", nullptr},
    {"dedicatedSIBRequest-r16", nullptr},
    {"mcgFailureInformation-r16", nullptr},
    {"ueInformationResponse-r16", nullptr},
    {"sidelinkUEInformationNR-r16", nullptr},
    {"ulInformationTransferIRAT-r16", nullptr},
    {"iabOtherInformation-r16", nullptr},
    {"mbsInterestIndication-r17", nullptr},
    {"uePositioningAssistanceInfo-r17", nullptr},
    {"measurementReportAppLayer-r17", nullptr},
    {"indirectPathFailureInformation-r18", nullptr},
    {"spare5", nullptr},
    {"spare4", nullptr},
    {"spare3", nullptr},
    {"spare2", nullptr},
    {"spare1", nullptr}};
const asn1::table::choice_desc ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::desc = {
    asn1::table::enum_int_type<ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types>(),
    ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types::nof_types,
    ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types::nof_exts,
    ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types::has_ext,
    false,
    choice_table_ul_dcch_msg_type_c_msg_class_ext_c__c2_c__alternatives,
    &asn1::table::choice_set<ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_>,
    choice_table_ul_dcch_msg_type_c_msg_class_ext_c__c2_c__names,
    "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_",
    offsetof(ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_, type_),
    offsetof(ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_, c),
    false};

void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::desc);
}
ul_ded_msg_segment_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ul_ded_msg_segment_r16()
{
  set(types::ul_ded_msg_segment_r16);
  return c.get<ul_ded_msg_segment_r16_s>();
}
ded_sib_request_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ded_sib_request_r16()
{
  set(types::ded_sib_request_r16);
  return c.get<ded_sib_request_r16_s>();
}
mcg_fail_info_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_mcg_fail_info_r16()
{
  set(types::mcg_fail_info_r16);
  return c.get<mcg_fail_info_r16_s>();
}
ue_info_resp_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ue_info_resp_r16()
{
  set(types::ue_info_resp_r16);
  return c.get<ue_info_resp_r16_s>();
}
sidelink_ue_info_nr_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_sidelink_ue_info_nr_r16()
{
  set(types::sidelink_ue_info_nr_r16);
  return c.get<sidelink_ue_info_nr_r16_s>();
}
ul_info_transfer_irat_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ul_info_transfer_irat_r16()
{
  set(types::ul_info_transfer_irat_r16);
  return c.get<ul_info_transfer_irat_r16_s>();
}
iab_other_info_r16_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_iab_other_info_r16()
{
  set(types::iab_other_info_r16);
  return c.get<iab_other_info_r16_s>();
}
mb_si_nterest_ind_r17_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_mbs_interest_ind_r17()
{
  set(types::mbs_interest_ind_r17);
  return c.get<mb_si_nterest_ind_r17_s>();
}
ue_positioning_assist_info_r17_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_ue_positioning_assist_info_r17()
{
  set(types::ue_positioning_assist_info_r17);
  return c.get<ue_positioning_assist_info_r17_s>();
}
measurement_report_app_layer_r17_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_meas_report_app_layer_r17()
{
  set(types::meas_report_app_layer_r17);
  return c.get<measurement_report_app_layer_r17_s>();
}
indirect_path_fail_info_r18_s& ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_indirect_path_fail_info_r18()
{
  set(types::indirect_path_fail_info_r18);
  return c.get<indirect_path_fail_info_r18_s>();
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare5()
{
  set(types::spare5);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare4()
{
  set(types::spare4);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare3()
{
  set(types::spare3);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare2()
{
  set(types::spare2);
}
void ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::set_spare1()
{
  set(types::spare1);
}

static const char* enum_names_ul_dcch_msg_type_c_msg_class_ext_c__c2_c__types_values[] = {
    "ulDedicatedMessageSegment-r16",
    "dedicatedSIBRequest-r16",
    "mcgFailureInformation-r16",
    "ueInformationResponse-r16",
    "sidelinkUEInformationNR-r16",
    "ulInformationTransferIRAT-r16",
    "iabOtherInformation-r16",
    "mbsInterestIndication-r17",
    "uePositioningAssistanceInfo-r17",
    "measurementReportAppLayer-r17",
    "indirectPathFailureInformation-r18",
    "spare5",
    "spare4",
    "spare3",
    "spare2",
    "spare1"};
const asn1::table::enum_desc ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types_opts::desc = {
    enum_names_ul_dcch_msg_type_c_msg_class_ext_c__c2_c__types_values,
    16,
    "ul_dcch_msg_type_c::msg_class_ext_c_::c2_c_::types"};

static const char*           enum_names_ul_dcch_msg_type_c_msg_class_ext_c__types_values[] = {"c2",
                                                                                              "messageClassExtensionFuture-r16"};
const asn1::table::enum_desc ul_dcch_msg_type_c::msg_class_ext_c_::types_opts::desc        = {
    enum_names_ul_dcch_msg_type_c_msg_class_ext_c__types_values,
    2,
    "ul_dcch_msg_type_c::msg_class_ext_c_::types"};
uint8_t ul_dcch_msg_type_c::msg_class_ext_c_::types_opts::to_number() const
{
  static const uint8_t numbers[] = {2};
  return map_enum_number(numbers, 1, value, "ul_dcch_msg_type_c::msg_class_ext_c_::types");
}

static const char*           enum_names_ul_dcch_msg_type_c_types_values[] = {"c1", "messageClassExtension"};
const asn1::table::enum_desc ul_dcch_msg_type_c::types_opts::desc         = {enum_names_ul_dcch_msg_type_c_types_values,
                                                                             2,
                                                                             "ul_dcch_msg_type_c::types"};
uint8_t                      ul_dcch_msg_type_c::types_opts::to_number() const
{
  static const uint8_t numbers[] = {1};
  return map_enum_number(numbers, 1, value, "ul_dcch_msg_type_c::types");
}

// UL-DCCH-Message ::= SEQUENCE
static const asn1::table::seq_field  seq_table_ul_dcch_msg_s_fields[] = {{offsetof(ul_dcch_msg_s, msg),
                                                                          asn1::table::seq_field::mandatory,
                                                                          asn1::table::field_op::choice,
                                                                          false,
                                                                          false,
                                                                          0,
                                                                          0,
                                                                          nullptr,
                                                                          &ul_dcch_msg_type_c::desc}};
static const asn1::table::field_name seq_table_ul_dcch_msg_s_names[]  = {{"message", nullptr}};
const asn1::table::seq_desc ul_dcch_msg_s::desc = {{seq_table_ul_dcch_msg_s_fields, 1, UINT64_C(0x1), nullptr, 0},
                                                   false,
                                                   false,
                                                   nullptr,
                                                   0,
                                                   seq_table_ul_dcch_msg_s_names,
                                                   "UL-DCCH-Message"};
