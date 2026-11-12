// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/asn1/asn1_ap_codec.h"
#include "ocudu/asn1/nrppa/nrppa_pdu_contents.h"

using namespace asn1;
using namespace asn1::nrppa;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// NRPPA-ELEMENTARY-PROCEDURES ::= OBJECT SET OF NRPPA-ELEMENTARY-PROCEDURE
static const uint32_t objset_proc_code_nr_ppa_elem_procs_o[] = {2, 6, 9, 11, 16, 17, 19, 20, 3,  4,  5, 0,
                                                                1, 7, 8, 10, 12, 13, 14, 15, 18, 21, 22};
static const uint8_t  objset_init_msg_nr_ppa_elem_procs_o[]  = {
    nr_ppa_elem_procs_o::init_msg_c::types::e_c_id_meas_initiation_request,
    nr_ppa_elem_procs_o::init_msg_c::types::otdoa_info_request,
    nr_ppa_elem_procs_o::init_msg_c::types::positioning_info_request,
    nr_ppa_elem_procs_o::init_msg_c::types::meas_request,
    nr_ppa_elem_procs_o::init_msg_c::types::trp_info_request,
    nr_ppa_elem_procs_o::init_msg_c::types::positioning_activation_request,
    nr_ppa_elem_procs_o::init_msg_c::types::prs_cfg_request,
    nr_ppa_elem_procs_o::init_msg_c::types::meas_precfg_required,
    nr_ppa_elem_procs_o::init_msg_c::types::e_c_id_meas_fail_ind,
    nr_ppa_elem_procs_o::init_msg_c::types::e_c_id_meas_report,
    nr_ppa_elem_procs_o::init_msg_c::types::e_c_id_meas_termination_cmd,
    nr_ppa_elem_procs_o::init_msg_c::types::error_ind,
    nr_ppa_elem_procs_o::init_msg_c::types::private_msg,
    nr_ppa_elem_procs_o::init_msg_c::types::assist_info_ctrl,
    nr_ppa_elem_procs_o::init_msg_c::types::assist_info_feedback,
    nr_ppa_elem_procs_o::init_msg_c::types::positioning_info_upd,
    nr_ppa_elem_procs_o::init_msg_c::types::meas_report,
    nr_ppa_elem_procs_o::init_msg_c::types::meas_upd,
    nr_ppa_elem_procs_o::init_msg_c::types::meas_abort,
    nr_ppa_elem_procs_o::init_msg_c::types::meas_fail_ind,
    nr_ppa_elem_procs_o::init_msg_c::types::positioning_deactivation,
    nr_ppa_elem_procs_o::init_msg_c::types::meas_activation,
    nr_ppa_elem_procs_o::init_msg_c::types::srs_info_reserv_notif};
static const uint8_t objset_successful_outcome_nr_ppa_elem_procs_o[] = {
    nr_ppa_elem_procs_o::successful_outcome_c::types::e_c_id_meas_initiation_resp,
    nr_ppa_elem_procs_o::successful_outcome_c::types::otdoa_info_resp,
    nr_ppa_elem_procs_o::successful_outcome_c::types::positioning_info_resp,
    nr_ppa_elem_procs_o::successful_outcome_c::types::meas_resp,
    nr_ppa_elem_procs_o::successful_outcome_c::types::trp_info_resp,
    nr_ppa_elem_procs_o::successful_outcome_c::types::positioning_activation_resp,
    nr_ppa_elem_procs_o::successful_outcome_c::types::prs_cfg_resp,
    nr_ppa_elem_procs_o::successful_outcome_c::types::meas_precfg_confirm,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype};
static const uint8_t objset_unsuccessful_outcome_nr_ppa_elem_procs_o[] = {
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::e_c_id_meas_initiation_fail,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::otdoa_info_fail,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::positioning_info_fail,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::meas_fail,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::trp_info_fail,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::positioning_activation_fail,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::prs_cfg_fail,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::meas_precfg_refuse,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype};
static const uint8_t objset_crit_nr_ppa_elem_procs_o[] = {
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::ignore,
    crit_e::ignore, crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::reject};
static const asn1::table::objset_column objset_columns_nr_ppa_elem_procs_o[] = {
    {objset_init_msg_nr_ppa_elem_procs_o, nr_ppa_elem_procs_o::init_msg_c::types::nulltype},
    {objset_successful_outcome_nr_ppa_elem_procs_o, nr_ppa_elem_procs_o::successful_outcome_c::types::nulltype},
    {objset_unsuccessful_outcome_nr_ppa_elem_procs_o, nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nulltype},
    {objset_crit_nr_ppa_elem_procs_o, crit_e::nulltype}};
const asn1::table::objset_desc nr_ppa_elem_procs_o::desc = {objset_proc_code_nr_ppa_elem_procs_o,
                                                            23,
                                                            "proc_code",
                                                            objset_columns_nr_ppa_elem_procs_o};

// InitiatingMessage ::= OPEN TYPE
static const asn1::table::seq_field choice_table_nr_ppa_elem_procs_o_init_msg_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e_c_id_meas_initiation_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<otdoa_info_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_info_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<meas_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<trp_info_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_activation_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<prs_cfg_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<meas_precfg_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e_c_id_meas_fail_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e_c_id_meas_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e_c_id_meas_termination_cmd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<error_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<private_msg_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<assist_info_ctrl_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<assist_info_feedback_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_info_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<meas_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<meas_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<meas_abort_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<meas_fail_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_deactivation_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<meas_activation_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<srs_info_reserv_notif_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_nr_ppa_elem_procs_o_init_msg_c_names[] = {
    {"E-CIDMeasurementInitiationRequest", nullptr},
    {"OTDOAInformationRequest", nullptr},
    {"PositioningInformationRequest", nullptr},
    {"MeasurementRequest", nullptr},
    {"TRPInformationRequest", nullptr},
    {"PositioningActivationRequest", nullptr},
    {"PRSConfigurationRequest", nullptr},
    {"MeasurementPreconfigurationRequired", nullptr},
    {"E-CIDMeasurementFailureIndication", nullptr},
    {"E-CIDMeasurementReport", nullptr},
    {"E-CIDMeasurementTerminationCommand", nullptr},
    {"ErrorIndication", nullptr},
    {"PrivateMessage", nullptr},
    {"AssistanceInformationControl", nullptr},
    {"AssistanceInformationFeedback", nullptr},
    {"PositioningInformationUpdate", nullptr},
    {"MeasurementReport", nullptr},
    {"MeasurementUpdate", nullptr},
    {"MeasurementAbort", nullptr},
    {"MeasurementFailureIndication", nullptr},
    {"PositioningDeactivation", nullptr},
    {"MeasurementActivation", nullptr},
    {"SRSInformationReservationNotification", nullptr}};
const asn1::table::choice_desc nr_ppa_elem_procs_o::init_msg_c::desc = {
    asn1::table::enum_int_type<nr_ppa_elem_procs_o::init_msg_c::types>(),
    nr_ppa_elem_procs_o::init_msg_c::types::nof_types,
    nr_ppa_elem_procs_o::init_msg_c::types::nof_exts,
    nr_ppa_elem_procs_o::init_msg_c::types::has_ext,
    true,
    choice_table_nr_ppa_elem_procs_o_init_msg_c_alternatives,
    &asn1::table::choice_set<nr_ppa_elem_procs_o::init_msg_c>,
    choice_table_nr_ppa_elem_procs_o_init_msg_c_names,
    "nr_ppa_elem_procs_o::init_msg_c",
    offsetof(nr_ppa_elem_procs_o::init_msg_c, type_),
    offsetof(nr_ppa_elem_procs_o::init_msg_c, c),
    false,
    nullptr,
    true};

void nr_ppa_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, nr_ppa_elem_procs_o::init_msg_c::desc);
}
e_c_id_meas_initiation_request_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_initiation_request()
{
  assert_choice_type(types::e_c_id_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_initiation_request_s>();
}
otdoa_info_request_s& nr_ppa_elem_procs_o::init_msg_c::otdoa_info_request()
{
  assert_choice_type(types::otdoa_info_request, type_, "InitiatingMessage");
  return c.get<otdoa_info_request_s>();
}
positioning_info_request_s& nr_ppa_elem_procs_o::init_msg_c::positioning_info_request()
{
  assert_choice_type(types::positioning_info_request, type_, "InitiatingMessage");
  return c.get<positioning_info_request_s>();
}
meas_request_s& nr_ppa_elem_procs_o::init_msg_c::meas_request()
{
  assert_choice_type(types::meas_request, type_, "InitiatingMessage");
  return c.get<meas_request_s>();
}
trp_info_request_s& nr_ppa_elem_procs_o::init_msg_c::trp_info_request()
{
  assert_choice_type(types::trp_info_request, type_, "InitiatingMessage");
  return c.get<trp_info_request_s>();
}
positioning_activation_request_s& nr_ppa_elem_procs_o::init_msg_c::positioning_activation_request()
{
  assert_choice_type(types::positioning_activation_request, type_, "InitiatingMessage");
  return c.get<positioning_activation_request_s>();
}
prs_cfg_request_s& nr_ppa_elem_procs_o::init_msg_c::prs_cfg_request()
{
  assert_choice_type(types::prs_cfg_request, type_, "InitiatingMessage");
  return c.get<prs_cfg_request_s>();
}
meas_precfg_required_s& nr_ppa_elem_procs_o::init_msg_c::meas_precfg_required()
{
  assert_choice_type(types::meas_precfg_required, type_, "InitiatingMessage");
  return c.get<meas_precfg_required_s>();
}
e_c_id_meas_fail_ind_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_fail_ind()
{
  assert_choice_type(types::e_c_id_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_fail_ind_s>();
}
e_c_id_meas_report_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_report()
{
  assert_choice_type(types::e_c_id_meas_report, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_report_s>();
}
e_c_id_meas_termination_cmd_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_termination_cmd()
{
  assert_choice_type(types::e_c_id_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_termination_cmd_s>();
}
error_ind_s& nr_ppa_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
private_msg_s& nr_ppa_elem_procs_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
assist_info_ctrl_s& nr_ppa_elem_procs_o::init_msg_c::assist_info_ctrl()
{
  assert_choice_type(types::assist_info_ctrl, type_, "InitiatingMessage");
  return c.get<assist_info_ctrl_s>();
}
assist_info_feedback_s& nr_ppa_elem_procs_o::init_msg_c::assist_info_feedback()
{
  assert_choice_type(types::assist_info_feedback, type_, "InitiatingMessage");
  return c.get<assist_info_feedback_s>();
}
positioning_info_upd_s& nr_ppa_elem_procs_o::init_msg_c::positioning_info_upd()
{
  assert_choice_type(types::positioning_info_upd, type_, "InitiatingMessage");
  return c.get<positioning_info_upd_s>();
}
meas_report_s& nr_ppa_elem_procs_o::init_msg_c::meas_report()
{
  assert_choice_type(types::meas_report, type_, "InitiatingMessage");
  return c.get<meas_report_s>();
}
meas_upd_s& nr_ppa_elem_procs_o::init_msg_c::meas_upd()
{
  assert_choice_type(types::meas_upd, type_, "InitiatingMessage");
  return c.get<meas_upd_s>();
}
meas_abort_s& nr_ppa_elem_procs_o::init_msg_c::meas_abort()
{
  assert_choice_type(types::meas_abort, type_, "InitiatingMessage");
  return c.get<meas_abort_s>();
}
meas_fail_ind_s& nr_ppa_elem_procs_o::init_msg_c::meas_fail_ind()
{
  assert_choice_type(types::meas_fail_ind, type_, "InitiatingMessage");
  return c.get<meas_fail_ind_s>();
}
positioning_deactivation_s& nr_ppa_elem_procs_o::init_msg_c::positioning_deactivation()
{
  assert_choice_type(types::positioning_deactivation, type_, "InitiatingMessage");
  return c.get<positioning_deactivation_s>();
}
meas_activation_s& nr_ppa_elem_procs_o::init_msg_c::meas_activation()
{
  assert_choice_type(types::meas_activation, type_, "InitiatingMessage");
  return c.get<meas_activation_s>();
}
srs_info_reserv_notif_s& nr_ppa_elem_procs_o::init_msg_c::srs_info_reserv_notif()
{
  assert_choice_type(types::srs_info_reserv_notif, type_, "InitiatingMessage");
  return c.get<srs_info_reserv_notif_s>();
}
const e_c_id_meas_initiation_request_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_initiation_request() const
{
  assert_choice_type(types::e_c_id_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_initiation_request_s>();
}
const otdoa_info_request_s& nr_ppa_elem_procs_o::init_msg_c::otdoa_info_request() const
{
  assert_choice_type(types::otdoa_info_request, type_, "InitiatingMessage");
  return c.get<otdoa_info_request_s>();
}
const positioning_info_request_s& nr_ppa_elem_procs_o::init_msg_c::positioning_info_request() const
{
  assert_choice_type(types::positioning_info_request, type_, "InitiatingMessage");
  return c.get<positioning_info_request_s>();
}
const meas_request_s& nr_ppa_elem_procs_o::init_msg_c::meas_request() const
{
  assert_choice_type(types::meas_request, type_, "InitiatingMessage");
  return c.get<meas_request_s>();
}
const trp_info_request_s& nr_ppa_elem_procs_o::init_msg_c::trp_info_request() const
{
  assert_choice_type(types::trp_info_request, type_, "InitiatingMessage");
  return c.get<trp_info_request_s>();
}
const positioning_activation_request_s& nr_ppa_elem_procs_o::init_msg_c::positioning_activation_request() const
{
  assert_choice_type(types::positioning_activation_request, type_, "InitiatingMessage");
  return c.get<positioning_activation_request_s>();
}
const prs_cfg_request_s& nr_ppa_elem_procs_o::init_msg_c::prs_cfg_request() const
{
  assert_choice_type(types::prs_cfg_request, type_, "InitiatingMessage");
  return c.get<prs_cfg_request_s>();
}
const meas_precfg_required_s& nr_ppa_elem_procs_o::init_msg_c::meas_precfg_required() const
{
  assert_choice_type(types::meas_precfg_required, type_, "InitiatingMessage");
  return c.get<meas_precfg_required_s>();
}
const e_c_id_meas_fail_ind_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_fail_ind() const
{
  assert_choice_type(types::e_c_id_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_fail_ind_s>();
}
const e_c_id_meas_report_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_report() const
{
  assert_choice_type(types::e_c_id_meas_report, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_report_s>();
}
const e_c_id_meas_termination_cmd_s& nr_ppa_elem_procs_o::init_msg_c::e_c_id_meas_termination_cmd() const
{
  assert_choice_type(types::e_c_id_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_termination_cmd_s>();
}
const error_ind_s& nr_ppa_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const private_msg_s& nr_ppa_elem_procs_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const assist_info_ctrl_s& nr_ppa_elem_procs_o::init_msg_c::assist_info_ctrl() const
{
  assert_choice_type(types::assist_info_ctrl, type_, "InitiatingMessage");
  return c.get<assist_info_ctrl_s>();
}
const assist_info_feedback_s& nr_ppa_elem_procs_o::init_msg_c::assist_info_feedback() const
{
  assert_choice_type(types::assist_info_feedback, type_, "InitiatingMessage");
  return c.get<assist_info_feedback_s>();
}
const positioning_info_upd_s& nr_ppa_elem_procs_o::init_msg_c::positioning_info_upd() const
{
  assert_choice_type(types::positioning_info_upd, type_, "InitiatingMessage");
  return c.get<positioning_info_upd_s>();
}
const meas_report_s& nr_ppa_elem_procs_o::init_msg_c::meas_report() const
{
  assert_choice_type(types::meas_report, type_, "InitiatingMessage");
  return c.get<meas_report_s>();
}
const meas_upd_s& nr_ppa_elem_procs_o::init_msg_c::meas_upd() const
{
  assert_choice_type(types::meas_upd, type_, "InitiatingMessage");
  return c.get<meas_upd_s>();
}
const meas_abort_s& nr_ppa_elem_procs_o::init_msg_c::meas_abort() const
{
  assert_choice_type(types::meas_abort, type_, "InitiatingMessage");
  return c.get<meas_abort_s>();
}
const meas_fail_ind_s& nr_ppa_elem_procs_o::init_msg_c::meas_fail_ind() const
{
  assert_choice_type(types::meas_fail_ind, type_, "InitiatingMessage");
  return c.get<meas_fail_ind_s>();
}
const positioning_deactivation_s& nr_ppa_elem_procs_o::init_msg_c::positioning_deactivation() const
{
  assert_choice_type(types::positioning_deactivation, type_, "InitiatingMessage");
  return c.get<positioning_deactivation_s>();
}
const meas_activation_s& nr_ppa_elem_procs_o::init_msg_c::meas_activation() const
{
  assert_choice_type(types::meas_activation, type_, "InitiatingMessage");
  return c.get<meas_activation_s>();
}
const srs_info_reserv_notif_s& nr_ppa_elem_procs_o::init_msg_c::srs_info_reserv_notif() const
{
  assert_choice_type(types::srs_info_reserv_notif, type_, "InitiatingMessage");
  return c.get<srs_info_reserv_notif_s>();
}

static const char* enum_names_nr_ppa_elem_procs_o_init_msg_c_types_values[]    = {"E-CIDMeasurementInitiationRequest",
                                                                                  "OTDOAInformationRequest",
                                                                                  "PositioningInformationRequest",
                                                                                  "MeasurementRequest",
                                                                                  "TRPInformationRequest",
                                                                                  "PositioningActivationRequest",
                                                                                  "PRSConfigurationRequest",
                                                                                  "MeasurementPreconfigurationRequired",
                                                                                  "E-CIDMeasurementFailureIndication",
                                                                                  "E-CIDMeasurementReport",
                                                                                  "E-CIDMeasurementTerminationCommand",
                                                                                  "ErrorIndication",
                                                                                  "PrivateMessage",
                                                                                  "AssistanceInformationControl",
                                                                                  "AssistanceInformationFeedback",
                                                                                  "PositioningInformationUpdate",
                                                                                  "MeasurementReport",
                                                                                  "MeasurementUpdate",
                                                                                  "MeasurementAbort",
                                                                                  "MeasurementFailureIndication",
                                                                                  "PositioningDeactivation",
                                                                                  "MeasurementActivation",
                                                                                  "SRSInformationReservationNotification"};
const asn1::table::enum_desc nr_ppa_elem_procs_o::init_msg_c::types_opts::desc = {
    enum_names_nr_ppa_elem_procs_o_init_msg_c_types_values,
    23,
    "nr_ppa_elem_procs_o::init_msg_c::types"};

// SuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_nr_ppa_elem_procs_o_successful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e_c_id_meas_initiation_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<otdoa_info_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_info_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<meas_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<trp_info_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_activation_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<prs_cfg_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<meas_precfg_confirm_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_nr_ppa_elem_procs_o_successful_outcome_c_names[] = {
    {"E-CIDMeasurementInitiationResponse", nullptr},
    {"OTDOAInformationResponse", nullptr},
    {"PositioningInformationResponse", nullptr},
    {"MeasurementResponse", nullptr},
    {"TRPInformationResponse", nullptr},
    {"PositioningActivationResponse", nullptr},
    {"PRSConfigurationResponse", nullptr},
    {"MeasurementPreconfigurationConfirm", nullptr}};
const asn1::table::choice_desc nr_ppa_elem_procs_o::successful_outcome_c::desc = {
    asn1::table::enum_int_type<nr_ppa_elem_procs_o::successful_outcome_c::types>(),
    nr_ppa_elem_procs_o::successful_outcome_c::types::nof_types,
    nr_ppa_elem_procs_o::successful_outcome_c::types::nof_exts,
    nr_ppa_elem_procs_o::successful_outcome_c::types::has_ext,
    true,
    choice_table_nr_ppa_elem_procs_o_successful_outcome_c_alternatives,
    &asn1::table::choice_set<nr_ppa_elem_procs_o::successful_outcome_c>,
    choice_table_nr_ppa_elem_procs_o_successful_outcome_c_names,
    "nr_ppa_elem_procs_o::successful_outcome_c",
    offsetof(nr_ppa_elem_procs_o::successful_outcome_c, type_),
    offsetof(nr_ppa_elem_procs_o::successful_outcome_c, c),
    false,
    nullptr,
    true};

void nr_ppa_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, nr_ppa_elem_procs_o::successful_outcome_c::desc);
}
e_c_id_meas_initiation_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::e_c_id_meas_initiation_resp()
{
  assert_choice_type(types::e_c_id_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_resp_s>();
}
otdoa_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::otdoa_info_resp()
{
  assert_choice_type(types::otdoa_info_resp, type_, "SuccessfulOutcome");
  return c.get<otdoa_info_resp_s>();
}
positioning_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::positioning_info_resp()
{
  assert_choice_type(types::positioning_info_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_info_resp_s>();
}
meas_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::meas_resp()
{
  assert_choice_type(types::meas_resp, type_, "SuccessfulOutcome");
  return c.get<meas_resp_s>();
}
trp_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::trp_info_resp()
{
  assert_choice_type(types::trp_info_resp, type_, "SuccessfulOutcome");
  return c.get<trp_info_resp_s>();
}
positioning_activation_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::positioning_activation_resp()
{
  assert_choice_type(types::positioning_activation_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_activation_resp_s>();
}
prs_cfg_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::prs_cfg_resp()
{
  assert_choice_type(types::prs_cfg_resp, type_, "SuccessfulOutcome");
  return c.get<prs_cfg_resp_s>();
}
meas_precfg_confirm_s& nr_ppa_elem_procs_o::successful_outcome_c::meas_precfg_confirm()
{
  assert_choice_type(types::meas_precfg_confirm, type_, "SuccessfulOutcome");
  return c.get<meas_precfg_confirm_s>();
}
const e_c_id_meas_initiation_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::e_c_id_meas_initiation_resp() const
{
  assert_choice_type(types::e_c_id_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_resp_s>();
}
const otdoa_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::otdoa_info_resp() const
{
  assert_choice_type(types::otdoa_info_resp, type_, "SuccessfulOutcome");
  return c.get<otdoa_info_resp_s>();
}
const positioning_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::positioning_info_resp() const
{
  assert_choice_type(types::positioning_info_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_info_resp_s>();
}
const meas_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::meas_resp() const
{
  assert_choice_type(types::meas_resp, type_, "SuccessfulOutcome");
  return c.get<meas_resp_s>();
}
const trp_info_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::trp_info_resp() const
{
  assert_choice_type(types::trp_info_resp, type_, "SuccessfulOutcome");
  return c.get<trp_info_resp_s>();
}
const positioning_activation_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::positioning_activation_resp() const
{
  assert_choice_type(types::positioning_activation_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_activation_resp_s>();
}
const prs_cfg_resp_s& nr_ppa_elem_procs_o::successful_outcome_c::prs_cfg_resp() const
{
  assert_choice_type(types::prs_cfg_resp, type_, "SuccessfulOutcome");
  return c.get<prs_cfg_resp_s>();
}
const meas_precfg_confirm_s& nr_ppa_elem_procs_o::successful_outcome_c::meas_precfg_confirm() const
{
  assert_choice_type(types::meas_precfg_confirm, type_, "SuccessfulOutcome");
  return c.get<meas_precfg_confirm_s>();
}

static const char* enum_names_nr_ppa_elem_procs_o_successful_outcome_c_types_values[] = {
    "E-CIDMeasurementInitiationResponse",
    "OTDOAInformationResponse",
    "PositioningInformationResponse",
    "MeasurementResponse",
    "TRPInformationResponse",
    "PositioningActivationResponse",
    "PRSConfigurationResponse",
    "MeasurementPreconfigurationConfirm"};
const asn1::table::enum_desc nr_ppa_elem_procs_o::successful_outcome_c::types_opts::desc = {
    enum_names_nr_ppa_elem_procs_o_successful_outcome_c_types_values,
    8,
    "nr_ppa_elem_procs_o::successful_outcome_c::types"};

// UnsuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_nr_ppa_elem_procs_o_unsuccessful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e_c_id_meas_initiation_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<otdoa_info_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_info_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<meas_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<trp_info_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_activation_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<prs_cfg_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<meas_precfg_refuse_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_nr_ppa_elem_procs_o_unsuccessful_outcome_c_names[] = {
    {"E-CIDMeasurementInitiationFailure", nullptr},
    {"OTDOAInformationFailure", nullptr},
    {"PositioningInformationFailure", nullptr},
    {"MeasurementFailure", nullptr},
    {"TRPInformationFailure", nullptr},
    {"PositioningActivationFailure", nullptr},
    {"PRSConfigurationFailure", nullptr},
    {"MeasurementPreconfigurationRefuse", nullptr}};
const asn1::table::choice_desc nr_ppa_elem_procs_o::unsuccessful_outcome_c::desc = {
    asn1::table::enum_int_type<nr_ppa_elem_procs_o::unsuccessful_outcome_c::types>(),
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nof_types,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::nof_exts,
    nr_ppa_elem_procs_o::unsuccessful_outcome_c::types::has_ext,
    true,
    choice_table_nr_ppa_elem_procs_o_unsuccessful_outcome_c_alternatives,
    &asn1::table::choice_set<nr_ppa_elem_procs_o::unsuccessful_outcome_c>,
    choice_table_nr_ppa_elem_procs_o_unsuccessful_outcome_c_names,
    "nr_ppa_elem_procs_o::unsuccessful_outcome_c",
    offsetof(nr_ppa_elem_procs_o::unsuccessful_outcome_c, type_),
    offsetof(nr_ppa_elem_procs_o::unsuccessful_outcome_c, c),
    false,
    nullptr,
    true};

void nr_ppa_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, nr_ppa_elem_procs_o::unsuccessful_outcome_c::desc);
}
e_c_id_meas_initiation_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::e_c_id_meas_initiation_fail()
{
  assert_choice_type(types::e_c_id_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_fail_s>();
}
otdoa_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::otdoa_info_fail()
{
  assert_choice_type(types::otdoa_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<otdoa_info_fail_s>();
}
positioning_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::positioning_info_fail()
{
  assert_choice_type(types::positioning_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_info_fail_s>();
}
meas_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::meas_fail()
{
  assert_choice_type(types::meas_fail, type_, "UnsuccessfulOutcome");
  return c.get<meas_fail_s>();
}
trp_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::trp_info_fail()
{
  assert_choice_type(types::trp_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<trp_info_fail_s>();
}
positioning_activation_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::positioning_activation_fail()
{
  assert_choice_type(types::positioning_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_activation_fail_s>();
}
prs_cfg_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::prs_cfg_fail()
{
  assert_choice_type(types::prs_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<prs_cfg_fail_s>();
}
meas_precfg_refuse_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::meas_precfg_refuse()
{
  assert_choice_type(types::meas_precfg_refuse, type_, "UnsuccessfulOutcome");
  return c.get<meas_precfg_refuse_s>();
}
const e_c_id_meas_initiation_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::e_c_id_meas_initiation_fail() const
{
  assert_choice_type(types::e_c_id_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_fail_s>();
}
const otdoa_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::otdoa_info_fail() const
{
  assert_choice_type(types::otdoa_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<otdoa_info_fail_s>();
}
const positioning_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::positioning_info_fail() const
{
  assert_choice_type(types::positioning_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_info_fail_s>();
}
const meas_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::meas_fail() const
{
  assert_choice_type(types::meas_fail, type_, "UnsuccessfulOutcome");
  return c.get<meas_fail_s>();
}
const trp_info_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::trp_info_fail() const
{
  assert_choice_type(types::trp_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<trp_info_fail_s>();
}
const positioning_activation_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::positioning_activation_fail() const
{
  assert_choice_type(types::positioning_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_activation_fail_s>();
}
const prs_cfg_fail_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::prs_cfg_fail() const
{
  assert_choice_type(types::prs_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<prs_cfg_fail_s>();
}
const meas_precfg_refuse_s& nr_ppa_elem_procs_o::unsuccessful_outcome_c::meas_precfg_refuse() const
{
  assert_choice_type(types::meas_precfg_refuse, type_, "UnsuccessfulOutcome");
  return c.get<meas_precfg_refuse_s>();
}

static const char* enum_names_nr_ppa_elem_procs_o_unsuccessful_outcome_c_types_values[] = {
    "E-CIDMeasurementInitiationFailure",
    "OTDOAInformationFailure",
    "PositioningInformationFailure",
    "MeasurementFailure",
    "TRPInformationFailure",
    "PositioningActivationFailure",
    "PRSConfigurationFailure",
    "MeasurementPreconfigurationRefuse"};
const asn1::table::enum_desc nr_ppa_elem_procs_o::unsuccessful_outcome_c::types_opts::desc = {
    enum_names_nr_ppa_elem_procs_o_unsuccessful_outcome_c_types_values,
    8,
    "nr_ppa_elem_procs_o::unsuccessful_outcome_c::types"};

// Columns of the object sets of NRPPA-ELEMENTARY-PROCEDURE.
enum class nr_ppa_elem_proc_p_column : uint32_t { init_msg, successful_outcome, unsuccessful_outcome, crit };

// InitiatingMessage ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
static const asn1::table::seq_field seq_table_init_msg_s_fields[] = {
    {offsetof(init_msg_s, proc_code),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(init_msg_s, crit),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     (int64_t)offsetof(init_msg_s, proc_code) - (int64_t)offsetof(init_msg_s, crit),
     0,
     &asn1::table::checked_codec<decltype(init_msg_s::crit),
                                 decltype(init_msg_s::proc_code),
                                 nr_ppa_elem_procs_o::desc,
                                 nr_ppa_elem_proc_p_column::crit,
                                 asn1::table::string_json>::value},
    {offsetof(init_msg_s, nrppatransaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{32767},
     nullptr},
    {offsetof(init_msg_s, value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     (int64_t)offsetof(init_msg_s, proc_code) - (int64_t)offsetof(init_msg_s, value),
     0,
     &asn1::table::selected_codec<decltype(init_msg_s::value),
                                  decltype(init_msg_s::proc_code),
                                  nr_ppa_elem_procs_o::desc,
                                  nr_ppa_elem_proc_p_column::init_msg,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_init_msg_s_names[] = {{"procedureCode", nullptr},
                                                                     {"criticality", nullptr},
                                                                     {"nrppatransactionID", nullptr},
                                                                     {"value", nullptr}};
const asn1::table::seq_desc          init_msg_s::desc =
    {{seq_table_init_msg_s_fields, uint64_t{0xf}, nullptr, seq_table_init_msg_s_names, 4, 0}, false, true, nullptr, 0};
bool init_msg_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(nr_ppa_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(nr_ppa_elem_procs_o::desc, nr_ppa_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, nr_ppa_elem_procs_o::desc, nr_ppa_elem_proc_p_column::init_msg, proc_code);
  return value.type().value != nr_ppa_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
static const asn1::table::seq_field seq_table_successful_outcome_s_fields[] = {
    {offsetof(successful_outcome_s, proc_code),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(successful_outcome_s, crit),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     (int64_t)offsetof(successful_outcome_s, proc_code) - (int64_t)offsetof(successful_outcome_s, crit),
     0,
     &asn1::table::checked_codec<decltype(successful_outcome_s::crit),
                                 decltype(successful_outcome_s::proc_code),
                                 nr_ppa_elem_procs_o::desc,
                                 nr_ppa_elem_proc_p_column::crit,
                                 asn1::table::string_json>::value},
    {offsetof(successful_outcome_s, nrppatransaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{32767},
     nullptr},
    {offsetof(successful_outcome_s, value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     (int64_t)offsetof(successful_outcome_s, proc_code) - (int64_t)offsetof(successful_outcome_s, value),
     0,
     &asn1::table::selected_codec<decltype(successful_outcome_s::value),
                                  decltype(successful_outcome_s::proc_code),
                                  nr_ppa_elem_procs_o::desc,
                                  nr_ppa_elem_proc_p_column::successful_outcome,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_successful_outcome_s_names[] = {{"procedureCode", nullptr},
                                                                               {"criticality", nullptr},
                                                                               {"nrppatransactionID", nullptr},
                                                                               {"value", nullptr}};
const asn1::table::seq_desc          successful_outcome_s::desc             = {
    {seq_table_successful_outcome_s_fields, uint64_t{0xf}, nullptr, seq_table_successful_outcome_s_names, 4, 0},
    false,
    true,
    nullptr,
    0};
bool successful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(nr_ppa_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(nr_ppa_elem_procs_o::desc, nr_ppa_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(
      value, nr_ppa_elem_procs_o::desc, nr_ppa_elem_proc_p_column::successful_outcome, proc_code);
  return value.type().value != nr_ppa_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{NRPPA-ELEMENTARY-PROCEDURE}}
static const asn1::table::seq_field seq_table_unsuccessful_outcome_s_fields[] = {
    {offsetof(unsuccessful_outcome_s, proc_code),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{255},
     nullptr},
    {offsetof(unsuccessful_outcome_s, crit),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     (int64_t)offsetof(unsuccessful_outcome_s, proc_code) - (int64_t)offsetof(unsuccessful_outcome_s, crit),
     0,
     &asn1::table::checked_codec<decltype(unsuccessful_outcome_s::crit),
                                 decltype(unsuccessful_outcome_s::proc_code),
                                 nr_ppa_elem_procs_o::desc,
                                 nr_ppa_elem_proc_p_column::crit,
                                 asn1::table::string_json>::value},
    {offsetof(unsuccessful_outcome_s, nrppatransaction_id),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     int64_t{0},
     uint64_t{32767},
     nullptr},
    {offsetof(unsuccessful_outcome_s, value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     (int64_t)offsetof(unsuccessful_outcome_s, proc_code) - (int64_t)offsetof(unsuccessful_outcome_s, value),
     0,
     &asn1::table::selected_codec<decltype(unsuccessful_outcome_s::value),
                                  decltype(unsuccessful_outcome_s::proc_code),
                                  nr_ppa_elem_procs_o::desc,
                                  nr_ppa_elem_proc_p_column::unsuccessful_outcome,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_unsuccessful_outcome_s_names[] = {{"procedureCode", nullptr},
                                                                                 {"criticality", nullptr},
                                                                                 {"nrppatransactionID", nullptr},
                                                                                 {"value", nullptr}};
const asn1::table::seq_desc          unsuccessful_outcome_s::desc             = {
    {seq_table_unsuccessful_outcome_s_fields, uint64_t{0xf}, nullptr, seq_table_unsuccessful_outcome_s_names, 4, 0},
    false,
    true,
    nullptr,
    0};
bool unsuccessful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(nr_ppa_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(nr_ppa_elem_procs_o::desc, nr_ppa_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(
      value, nr_ppa_elem_procs_o::desc, nr_ppa_elem_proc_p_column::unsuccessful_outcome, proc_code);
  return value.type().value != nr_ppa_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// NRPPA-PDU ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_nr_ppa_pdu_c_alternatives[] = {
    &asn1::table::alternative_ops<init_msg_s, false, false>::value,
    &asn1::table::alternative_ops<successful_outcome_s, false, false>::value,
    &asn1::table::alternative_ops<unsuccessful_outcome_s, false, false>::value};
const asn1::table::choice_storage_desc nr_ppa_pdu_c::storage_desc = {choice_storage_nr_ppa_pdu_c_alternatives,
                                                                     3,
                                                                     "nr_ppa_pdu_c"};

init_msg_s& nr_ppa_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& nr_ppa_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& nr_ppa_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
static const asn1::table::seq_field  choice_table_nr_ppa_pdu_c_alternatives[] = {{0,
                                                                                  asn1::table::seq_field::mandatory,
                                                                                  asn1::table::field_op::sequence,
                                                                                  false,
                                                                                  false,
                                                                                  0,
                                                                                  0,
                                                                                  nullptr,
                                                                                  &init_msg_s::desc},
                                                                                 {0,
                                                                                  asn1::table::seq_field::mandatory,
                                                                                  asn1::table::field_op::sequence,
                                                                                  false,
                                                                                  false,
                                                                                  0,
                                                                                  0,
                                                                                  nullptr,
                                                                                  &successful_outcome_s::desc},
                                                                                 {0,
                                                                                  asn1::table::seq_field::mandatory,
                                                                                  asn1::table::field_op::sequence,
                                                                                  false,
                                                                                  false,
                                                                                  0,
                                                                                  0,
                                                                                  nullptr,
                                                                                  &unsuccessful_outcome_s::desc}};
static const asn1::table::field_name choice_table_nr_ppa_pdu_c_names[]        = {{"initiatingMessage", nullptr},
                                                                                 {"successfulOutcome", nullptr},
                                                                                 {"unsuccessfulOutcome", nullptr}};
const asn1::table::choice_desc       nr_ppa_pdu_c::desc = {asn1::table::enum_int_type<nr_ppa_pdu_c::types>(),
                                                           nr_ppa_pdu_c::types::nof_types,
                                                           nr_ppa_pdu_c::types::nof_exts,
                                                           nr_ppa_pdu_c::types::has_ext,
                                                           true,
                                                           choice_table_nr_ppa_pdu_c_alternatives,
                                                           nullptr,
                                                           choice_table_nr_ppa_pdu_c_names,
                                                           "nr_ppa_pdu_c",
                                                           offsetof(nr_ppa_pdu_c, c.type),
                                                           offsetof(nr_ppa_pdu_c, c.value),
                                                           false,
                                                           &nr_ppa_pdu_c::storage_desc};

static const char*           enum_names_nr_ppa_pdu_c_types_values[] = {"initiatingMessage",
                                                                       "successfulOutcome",
                                                                       "unsuccessfulOutcome"};
const asn1::table::enum_desc nr_ppa_pdu_c::types_opts::desc         = {enum_names_nr_ppa_pdu_c_types_values,
                                                                       3,
                                                                       "nr_ppa_pdu_c::types"};
