// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e2ap/e2ap.h"
#include "ocudu/asn1/asn1_ap_codec.h"
#include "ocudu/asn1/e2ap/e2ap_pdu_contents.h"

using namespace asn1;
using namespace asn1::e2ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// E2AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF E2AP-ELEMENTARY-PROCEDURE
static const uint32_t objset_proc_code_e2ap_elem_procs_o[] = {8,  17, 9,  14, 15, 20, 16, 7, 21, 4, 18, 1, 10,
                                                              11, 3,  13, 24, 22, 23, 5,  6, 19, 2, 12, 25};
static const uint8_t  objset_init_msg_e2ap_elem_procs_o[]  = {
    e2ap_elem_procs_o::init_msg_c::types::ric_sub_request,
    e2ap_elem_procs_o::init_msg_c::types::ric_sub_audit_request,
    e2ap_elem_procs_o::init_msg_c::types::ric_sub_delete_request,
    e2ap_elem_procs_o::init_msg_c::types::ric_sub_mod_request,
    e2ap_elem_procs_o::init_msg_c::types::ric_sub_mod_required,
    e2ap_elem_procs_o::init_msg_c::types::ric_sub_state_ctrl_request,
    e2ap_elem_procs_o::init_msg_c::types::ric_query_request,
    e2ap_elem_procs_o::init_msg_c::types::ric_service_upd,
    e2ap_elem_procs_o::init_msg_c::types::ric_assist_request,
    e2ap_elem_procs_o::init_msg_c::types::ric_ctrl_request,
    e2ap_elem_procs_o::init_msg_c::types::ric_service_load_status_request,
    e2ap_elem_procs_o::init_msg_c::types::e2setup_request,
    e2ap_elem_procs_o::init_msg_c::types::e2node_cfg_upd,
    e2ap_elem_procs_o::init_msg_c::types::e2conn_upd,
    e2ap_elem_procs_o::init_msg_c::types::reset_request,
    e2ap_elem_procs_o::init_msg_c::types::e2_removal_request,
    e2ap_elem_procs_o::init_msg_c::types::e2_res_status_request,
    e2ap_elem_procs_o::init_msg_c::types::ric_assist_ind,
    e2ap_elem_procs_o::init_msg_c::types::ric_assist_halt,
    e2ap_elem_procs_o::init_msg_c::types::ric_ind,
    e2ap_elem_procs_o::init_msg_c::types::ric_service_query,
    e2ap_elem_procs_o::init_msg_c::types::ric_service_load_upd,
    e2ap_elem_procs_o::init_msg_c::types::error_ind,
    e2ap_elem_procs_o::init_msg_c::types::ric_sub_delete_required,
    e2ap_elem_procs_o::init_msg_c::types::e2_res_status_upd};
static const uint8_t objset_successful_outcome_e2ap_elem_procs_o[] = {
    e2ap_elem_procs_o::successful_outcome_c::types::ric_sub_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::ric_sub_audit_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::ric_sub_delete_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::ric_sub_mod_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::ric_sub_mod_confirm,
    e2ap_elem_procs_o::successful_outcome_c::types::ric_sub_state_ctrl_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::ric_query_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::ric_service_upd_ack,
    e2ap_elem_procs_o::successful_outcome_c::types::ric_assist_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::ric_ctrl_ack,
    e2ap_elem_procs_o::successful_outcome_c::types::ric_service_load_status_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::e2setup_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::e2node_cfg_upd_ack,
    e2ap_elem_procs_o::successful_outcome_c::types::e2conn_upd_ack,
    e2ap_elem_procs_o::successful_outcome_c::types::reset_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::e2_removal_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::e2_res_status_resp,
    e2ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::successful_outcome_c::types::nulltype};
static const uint8_t objset_unsuccessful_outcome_e2ap_elem_procs_o[] = {
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::ric_sub_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::ric_sub_audit_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::ric_sub_delete_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::ric_sub_mod_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::ric_sub_mod_refuse,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::ric_sub_state_ctrl_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::ric_query_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::ric_service_upd_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::ric_assist_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::ric_ctrl_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::ric_service_load_status_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::e2setup_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::e2node_cfg_upd_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::e2conn_upd_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::e2_removal_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::e2_res_status_fail,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype};
static const uint8_t objset_crit_e2ap_elem_procs_o[] = {
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore};
static const asn1::table::objset_column objset_columns_e2ap_elem_procs_o[] = {
    {objset_init_msg_e2ap_elem_procs_o, e2ap_elem_procs_o::init_msg_c::types::nulltype},
    {objset_successful_outcome_e2ap_elem_procs_o, e2ap_elem_procs_o::successful_outcome_c::types::nulltype},
    {objset_unsuccessful_outcome_e2ap_elem_procs_o, e2ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype},
    {objset_crit_e2ap_elem_procs_o, crit_e::nulltype}};
const asn1::table::objset_desc e2ap_elem_procs_o::desc = {objset_proc_code_e2ap_elem_procs_o,
                                                          25,
                                                          "proc_code",
                                                          objset_columns_e2ap_elem_procs_o};

// InitiatingMessage ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2ap_elem_procs_o_init_msg_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_audit_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_delete_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_mod_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_state_ctrl_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_query_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_service_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_assist_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_ctrl_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_service_load_status_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2node_cfg_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2conn_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<reset_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2_removal_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2_res_status_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_assist_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_assist_halt_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_service_query_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_service_load_upd_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ric_sub_delete_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2_res_status_upd_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_e2ap_elem_procs_o_init_msg_c_names[] = {
    {"RICsubscriptionRequest", nullptr},
    {"RICsubscriptionAuditRequest", nullptr},
    {"RICsubscriptionDeleteRequest", nullptr},
    {"RICsubscriptionModificationRequest", nullptr},
    {"RICsubscriptionModificationRequired", nullptr},
    {"RICsubscriptionStateControlRequest", nullptr},
    {"RICqueryRequest", nullptr},
    {"RICserviceUpdate", nullptr},
    {"RICassistanceRequest", nullptr},
    {"RICcontrolRequest", nullptr},
    {"RICserviceLoadStatusRequest", nullptr},
    {"E2setupRequest", nullptr},
    {"E2nodeConfigurationUpdate", nullptr},
    {"E2connectionUpdate", nullptr},
    {"ResetRequest", nullptr},
    {"E2RemovalRequest", nullptr},
    {"E2ResourceStatusRequest", nullptr},
    {"RICassistanceIndication", nullptr},
    {"RICassistanceHalt", nullptr},
    {"RICindication", nullptr},
    {"RICserviceQuery", nullptr},
    {"RICserviceLoadUpdate", nullptr},
    {"ErrorIndication", nullptr},
    {"RICsubscriptionDeleteRequired", nullptr},
    {"E2ResourceStatusUpdate", nullptr}};
const asn1::table::choice_desc e2ap_elem_procs_o::init_msg_c::desc = {
    asn1::table::enum_int_type<e2ap_elem_procs_o::init_msg_c::types>(),
    e2ap_elem_procs_o::init_msg_c::types::nof_types,
    e2ap_elem_procs_o::init_msg_c::types::nof_exts,
    e2ap_elem_procs_o::init_msg_c::types::has_ext,
    true,
    choice_table_e2ap_elem_procs_o_init_msg_c_alternatives,
    &asn1::table::choice_set<e2ap_elem_procs_o::init_msg_c>,
    choice_table_e2ap_elem_procs_o_init_msg_c_names,
    "e2ap_elem_procs_o::init_msg_c",
    offsetof(e2ap_elem_procs_o::init_msg_c, type_),
    offsetof(e2ap_elem_procs_o::init_msg_c, c),
    false,
    nullptr,
    true};

void e2ap_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, e2ap_elem_procs_o::init_msg_c::desc);
}
ric_sub_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_request()
{
  assert_choice_type(types::ric_sub_request, type_, "InitiatingMessage");
  return c.get<ric_sub_request_s>();
}
ric_sub_audit_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_audit_request()
{
  assert_choice_type(types::ric_sub_audit_request, type_, "InitiatingMessage");
  return c.get<ric_sub_audit_request_s>();
}
ric_sub_delete_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_request()
{
  assert_choice_type(types::ric_sub_delete_request, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_request_s>();
}
ric_sub_mod_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_request()
{
  assert_choice_type(types::ric_sub_mod_request, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_request_s>();
}
ric_sub_mod_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_required()
{
  assert_choice_type(types::ric_sub_mod_required, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_required_s>();
}
ric_sub_state_ctrl_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_state_ctrl_request()
{
  assert_choice_type(types::ric_sub_state_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_sub_state_ctrl_request_s>();
}
ric_query_request_s& e2ap_elem_procs_o::init_msg_c::ric_query_request()
{
  assert_choice_type(types::ric_query_request, type_, "InitiatingMessage");
  return c.get<ric_query_request_s>();
}
ric_service_upd_s& e2ap_elem_procs_o::init_msg_c::ric_service_upd()
{
  assert_choice_type(types::ric_service_upd, type_, "InitiatingMessage");
  return c.get<ric_service_upd_s>();
}
ric_assist_request_s& e2ap_elem_procs_o::init_msg_c::ric_assist_request()
{
  assert_choice_type(types::ric_assist_request, type_, "InitiatingMessage");
  return c.get<ric_assist_request_s>();
}
ric_ctrl_request_s& e2ap_elem_procs_o::init_msg_c::ric_ctrl_request()
{
  assert_choice_type(types::ric_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_ctrl_request_s>();
}
ric_service_load_status_request_s& e2ap_elem_procs_o::init_msg_c::ric_service_load_status_request()
{
  assert_choice_type(types::ric_service_load_status_request, type_, "InitiatingMessage");
  return c.get<ric_service_load_status_request_s>();
}
e2setup_request_s& e2ap_elem_procs_o::init_msg_c::e2setup_request()
{
  assert_choice_type(types::e2setup_request, type_, "InitiatingMessage");
  return c.get<e2setup_request_s>();
}
e2node_cfg_upd_s& e2ap_elem_procs_o::init_msg_c::e2node_cfg_upd()
{
  assert_choice_type(types::e2node_cfg_upd, type_, "InitiatingMessage");
  return c.get<e2node_cfg_upd_s>();
}
e2conn_upd_s& e2ap_elem_procs_o::init_msg_c::e2conn_upd()
{
  assert_choice_type(types::e2conn_upd, type_, "InitiatingMessage");
  return c.get<e2conn_upd_s>();
}
reset_request_s& e2ap_elem_procs_o::init_msg_c::reset_request()
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
e2_removal_request_s& e2ap_elem_procs_o::init_msg_c::e2_removal_request()
{
  assert_choice_type(types::e2_removal_request, type_, "InitiatingMessage");
  return c.get<e2_removal_request_s>();
}
e2_res_status_request_s& e2ap_elem_procs_o::init_msg_c::e2_res_status_request()
{
  assert_choice_type(types::e2_res_status_request, type_, "InitiatingMessage");
  return c.get<e2_res_status_request_s>();
}
ric_assist_ind_s& e2ap_elem_procs_o::init_msg_c::ric_assist_ind()
{
  assert_choice_type(types::ric_assist_ind, type_, "InitiatingMessage");
  return c.get<ric_assist_ind_s>();
}
ric_assist_halt_s& e2ap_elem_procs_o::init_msg_c::ric_assist_halt()
{
  assert_choice_type(types::ric_assist_halt, type_, "InitiatingMessage");
  return c.get<ric_assist_halt_s>();
}
ric_ind_s& e2ap_elem_procs_o::init_msg_c::ric_ind()
{
  assert_choice_type(types::ric_ind, type_, "InitiatingMessage");
  return c.get<ric_ind_s>();
}
ric_service_query_s& e2ap_elem_procs_o::init_msg_c::ric_service_query()
{
  assert_choice_type(types::ric_service_query, type_, "InitiatingMessage");
  return c.get<ric_service_query_s>();
}
ric_service_load_upd_s& e2ap_elem_procs_o::init_msg_c::ric_service_load_upd()
{
  assert_choice_type(types::ric_service_load_upd, type_, "InitiatingMessage");
  return c.get<ric_service_load_upd_s>();
}
error_ind_s& e2ap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
ric_sub_delete_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_required()
{
  assert_choice_type(types::ric_sub_delete_required, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_required_s>();
}
e2_res_status_upd_s& e2ap_elem_procs_o::init_msg_c::e2_res_status_upd()
{
  assert_choice_type(types::e2_res_status_upd, type_, "InitiatingMessage");
  return c.get<e2_res_status_upd_s>();
}
const ric_sub_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_request() const
{
  assert_choice_type(types::ric_sub_request, type_, "InitiatingMessage");
  return c.get<ric_sub_request_s>();
}
const ric_sub_audit_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_audit_request() const
{
  assert_choice_type(types::ric_sub_audit_request, type_, "InitiatingMessage");
  return c.get<ric_sub_audit_request_s>();
}
const ric_sub_delete_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_request() const
{
  assert_choice_type(types::ric_sub_delete_request, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_request_s>();
}
const ric_sub_mod_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_request() const
{
  assert_choice_type(types::ric_sub_mod_request, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_request_s>();
}
const ric_sub_mod_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_mod_required() const
{
  assert_choice_type(types::ric_sub_mod_required, type_, "InitiatingMessage");
  return c.get<ric_sub_mod_required_s>();
}
const ric_sub_state_ctrl_request_s& e2ap_elem_procs_o::init_msg_c::ric_sub_state_ctrl_request() const
{
  assert_choice_type(types::ric_sub_state_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_sub_state_ctrl_request_s>();
}
const ric_query_request_s& e2ap_elem_procs_o::init_msg_c::ric_query_request() const
{
  assert_choice_type(types::ric_query_request, type_, "InitiatingMessage");
  return c.get<ric_query_request_s>();
}
const ric_service_upd_s& e2ap_elem_procs_o::init_msg_c::ric_service_upd() const
{
  assert_choice_type(types::ric_service_upd, type_, "InitiatingMessage");
  return c.get<ric_service_upd_s>();
}
const ric_assist_request_s& e2ap_elem_procs_o::init_msg_c::ric_assist_request() const
{
  assert_choice_type(types::ric_assist_request, type_, "InitiatingMessage");
  return c.get<ric_assist_request_s>();
}
const ric_ctrl_request_s& e2ap_elem_procs_o::init_msg_c::ric_ctrl_request() const
{
  assert_choice_type(types::ric_ctrl_request, type_, "InitiatingMessage");
  return c.get<ric_ctrl_request_s>();
}
const ric_service_load_status_request_s& e2ap_elem_procs_o::init_msg_c::ric_service_load_status_request() const
{
  assert_choice_type(types::ric_service_load_status_request, type_, "InitiatingMessage");
  return c.get<ric_service_load_status_request_s>();
}
const e2setup_request_s& e2ap_elem_procs_o::init_msg_c::e2setup_request() const
{
  assert_choice_type(types::e2setup_request, type_, "InitiatingMessage");
  return c.get<e2setup_request_s>();
}
const e2node_cfg_upd_s& e2ap_elem_procs_o::init_msg_c::e2node_cfg_upd() const
{
  assert_choice_type(types::e2node_cfg_upd, type_, "InitiatingMessage");
  return c.get<e2node_cfg_upd_s>();
}
const e2conn_upd_s& e2ap_elem_procs_o::init_msg_c::e2conn_upd() const
{
  assert_choice_type(types::e2conn_upd, type_, "InitiatingMessage");
  return c.get<e2conn_upd_s>();
}
const reset_request_s& e2ap_elem_procs_o::init_msg_c::reset_request() const
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
const e2_removal_request_s& e2ap_elem_procs_o::init_msg_c::e2_removal_request() const
{
  assert_choice_type(types::e2_removal_request, type_, "InitiatingMessage");
  return c.get<e2_removal_request_s>();
}
const e2_res_status_request_s& e2ap_elem_procs_o::init_msg_c::e2_res_status_request() const
{
  assert_choice_type(types::e2_res_status_request, type_, "InitiatingMessage");
  return c.get<e2_res_status_request_s>();
}
const ric_assist_ind_s& e2ap_elem_procs_o::init_msg_c::ric_assist_ind() const
{
  assert_choice_type(types::ric_assist_ind, type_, "InitiatingMessage");
  return c.get<ric_assist_ind_s>();
}
const ric_assist_halt_s& e2ap_elem_procs_o::init_msg_c::ric_assist_halt() const
{
  assert_choice_type(types::ric_assist_halt, type_, "InitiatingMessage");
  return c.get<ric_assist_halt_s>();
}
const ric_ind_s& e2ap_elem_procs_o::init_msg_c::ric_ind() const
{
  assert_choice_type(types::ric_ind, type_, "InitiatingMessage");
  return c.get<ric_ind_s>();
}
const ric_service_query_s& e2ap_elem_procs_o::init_msg_c::ric_service_query() const
{
  assert_choice_type(types::ric_service_query, type_, "InitiatingMessage");
  return c.get<ric_service_query_s>();
}
const ric_service_load_upd_s& e2ap_elem_procs_o::init_msg_c::ric_service_load_upd() const
{
  assert_choice_type(types::ric_service_load_upd, type_, "InitiatingMessage");
  return c.get<ric_service_load_upd_s>();
}
const error_ind_s& e2ap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const ric_sub_delete_required_s& e2ap_elem_procs_o::init_msg_c::ric_sub_delete_required() const
{
  assert_choice_type(types::ric_sub_delete_required, type_, "InitiatingMessage");
  return c.get<ric_sub_delete_required_s>();
}
const e2_res_status_upd_s& e2ap_elem_procs_o::init_msg_c::e2_res_status_upd() const
{
  assert_choice_type(types::e2_res_status_upd, type_, "InitiatingMessage");
  return c.get<e2_res_status_upd_s>();
}

static const char*           enum_names_e2ap_elem_procs_o_init_msg_c_types_values[] = {"RICsubscriptionRequest",
                                                                                       "RICsubscriptionAuditRequest",
                                                                                       "RICsubscriptionDeleteRequest",
                                                                                       "RICsubscriptionModificationRequest",
                                                                                       "RICsubscriptionModificationRequired",
                                                                                       "RICsubscriptionStateControlRequest",
                                                                                       "RICqueryRequest",
                                                                                       "RICserviceUpdate",
                                                                                       "RICassistanceRequest",
                                                                                       "RICcontrolRequest",
                                                                                       "RICserviceLoadStatusRequest",
                                                                                       "E2setupRequest",
                                                                                       "E2nodeConfigurationUpdate",
                                                                                       "E2connectionUpdate",
                                                                                       "ResetRequest",
                                                                                       "E2RemovalRequest",
                                                                                       "E2ResourceStatusRequest",
                                                                                       "RICassistanceIndication",
                                                                                       "RICassistanceHalt",
                                                                                       "RICindication",
                                                                                       "RICserviceQuery",
                                                                                       "RICserviceLoadUpdate",
                                                                                       "ErrorIndication",
                                                                                       "RICsubscriptionDeleteRequired",
                                                                                       "E2ResourceStatusUpdate"};
const asn1::table::enum_desc e2ap_elem_procs_o::init_msg_c::types_opts::desc        = {
    enum_names_e2ap_elem_procs_o_init_msg_c_types_values,
    25,
    "e2ap_elem_procs_o::init_msg_c::types"};

// SuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2ap_elem_procs_o_successful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_audit_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_delete_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_mod_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_mod_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_state_ctrl_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_query_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_service_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_assist_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_ctrl_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_service_load_status_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2node_cfg_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2conn_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<reset_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2_removal_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2_res_status_resp_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_e2ap_elem_procs_o_successful_outcome_c_names[] = {
    {"RICsubscriptionResponse", nullptr},
    {"RICsubscriptionAuditResponse", nullptr},
    {"RICsubscriptionDeleteResponse", nullptr},
    {"RICsubscriptionModificationResponse", nullptr},
    {"RICsubscriptionModificationConfirm", nullptr},
    {"RICsubscriptionStateControlResponse", nullptr},
    {"RICqueryResponse", nullptr},
    {"RICserviceUpdateAcknowledge", nullptr},
    {"RICassistanceResponse", nullptr},
    {"RICcontrolAcknowledge", nullptr},
    {"RICserviceLoadStatusResponse", nullptr},
    {"E2setupResponse", nullptr},
    {"E2nodeConfigurationUpdateAcknowledge", nullptr},
    {"E2connectionUpdateAcknowledge", nullptr},
    {"ResetResponse", nullptr},
    {"E2RemovalResponse", nullptr},
    {"E2ResourceStatusResponse", nullptr}};
const asn1::table::choice_desc e2ap_elem_procs_o::successful_outcome_c::desc = {
    asn1::table::enum_int_type<e2ap_elem_procs_o::successful_outcome_c::types>(),
    e2ap_elem_procs_o::successful_outcome_c::types::nof_types,
    e2ap_elem_procs_o::successful_outcome_c::types::nof_exts,
    e2ap_elem_procs_o::successful_outcome_c::types::has_ext,
    true,
    choice_table_e2ap_elem_procs_o_successful_outcome_c_alternatives,
    &asn1::table::choice_set<e2ap_elem_procs_o::successful_outcome_c>,
    choice_table_e2ap_elem_procs_o_successful_outcome_c_names,
    "e2ap_elem_procs_o::successful_outcome_c",
    offsetof(e2ap_elem_procs_o::successful_outcome_c, type_),
    offsetof(e2ap_elem_procs_o::successful_outcome_c, c),
    false,
    nullptr,
    true};

void e2ap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, e2ap_elem_procs_o::successful_outcome_c::desc);
}
ric_sub_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_resp()
{
  assert_choice_type(types::ric_sub_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_resp_s>();
}
ric_sub_audit_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_audit_resp()
{
  assert_choice_type(types::ric_sub_audit_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_audit_resp_s>();
}
ric_sub_delete_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_delete_resp()
{
  assert_choice_type(types::ric_sub_delete_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_delete_resp_s>();
}
ric_sub_mod_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_resp()
{
  assert_choice_type(types::ric_sub_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_resp_s>();
}
ric_sub_mod_confirm_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_confirm()
{
  assert_choice_type(types::ric_sub_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_confirm_s>();
}
ric_sub_state_ctrl_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_state_ctrl_resp()
{
  assert_choice_type(types::ric_sub_state_ctrl_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_state_ctrl_resp_s>();
}
ric_query_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_query_resp()
{
  assert_choice_type(types::ric_query_resp, type_, "SuccessfulOutcome");
  return c.get<ric_query_resp_s>();
}
ric_service_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_service_upd_ack()
{
  assert_choice_type(types::ric_service_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ric_service_upd_ack_s>();
}
ric_assist_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_assist_resp()
{
  assert_choice_type(types::ric_assist_resp, type_, "SuccessfulOutcome");
  return c.get<ric_assist_resp_s>();
}
ric_ctrl_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_ctrl_ack()
{
  assert_choice_type(types::ric_ctrl_ack, type_, "SuccessfulOutcome");
  return c.get<ric_ctrl_ack_s>();
}
ric_service_load_status_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_service_load_status_resp()
{
  assert_choice_type(types::ric_service_load_status_resp, type_, "SuccessfulOutcome");
  return c.get<ric_service_load_status_resp_s>();
}
e2setup_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2setup_resp()
{
  assert_choice_type(types::e2setup_resp, type_, "SuccessfulOutcome");
  return c.get<e2setup_resp_s>();
}
e2node_cfg_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2node_cfg_upd_ack()
{
  assert_choice_type(types::e2node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2node_cfg_upd_ack_s>();
}
e2conn_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2conn_upd_ack()
{
  assert_choice_type(types::e2conn_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2conn_upd_ack_s>();
}
reset_resp_s& e2ap_elem_procs_o::successful_outcome_c::reset_resp()
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
e2_removal_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2_removal_resp()
{
  assert_choice_type(types::e2_removal_resp, type_, "SuccessfulOutcome");
  return c.get<e2_removal_resp_s>();
}
e2_res_status_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2_res_status_resp()
{
  assert_choice_type(types::e2_res_status_resp, type_, "SuccessfulOutcome");
  return c.get<e2_res_status_resp_s>();
}
const ric_sub_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_resp() const
{
  assert_choice_type(types::ric_sub_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_resp_s>();
}
const ric_sub_audit_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_audit_resp() const
{
  assert_choice_type(types::ric_sub_audit_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_audit_resp_s>();
}
const ric_sub_delete_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_delete_resp() const
{
  assert_choice_type(types::ric_sub_delete_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_delete_resp_s>();
}
const ric_sub_mod_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_resp() const
{
  assert_choice_type(types::ric_sub_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_resp_s>();
}
const ric_sub_mod_confirm_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_mod_confirm() const
{
  assert_choice_type(types::ric_sub_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ric_sub_mod_confirm_s>();
}
const ric_sub_state_ctrl_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_sub_state_ctrl_resp() const
{
  assert_choice_type(types::ric_sub_state_ctrl_resp, type_, "SuccessfulOutcome");
  return c.get<ric_sub_state_ctrl_resp_s>();
}
const ric_query_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_query_resp() const
{
  assert_choice_type(types::ric_query_resp, type_, "SuccessfulOutcome");
  return c.get<ric_query_resp_s>();
}
const ric_service_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_service_upd_ack() const
{
  assert_choice_type(types::ric_service_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ric_service_upd_ack_s>();
}
const ric_assist_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_assist_resp() const
{
  assert_choice_type(types::ric_assist_resp, type_, "SuccessfulOutcome");
  return c.get<ric_assist_resp_s>();
}
const ric_ctrl_ack_s& e2ap_elem_procs_o::successful_outcome_c::ric_ctrl_ack() const
{
  assert_choice_type(types::ric_ctrl_ack, type_, "SuccessfulOutcome");
  return c.get<ric_ctrl_ack_s>();
}
const ric_service_load_status_resp_s& e2ap_elem_procs_o::successful_outcome_c::ric_service_load_status_resp() const
{
  assert_choice_type(types::ric_service_load_status_resp, type_, "SuccessfulOutcome");
  return c.get<ric_service_load_status_resp_s>();
}
const e2setup_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2setup_resp() const
{
  assert_choice_type(types::e2setup_resp, type_, "SuccessfulOutcome");
  return c.get<e2setup_resp_s>();
}
const e2node_cfg_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2node_cfg_upd_ack() const
{
  assert_choice_type(types::e2node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2node_cfg_upd_ack_s>();
}
const e2conn_upd_ack_s& e2ap_elem_procs_o::successful_outcome_c::e2conn_upd_ack() const
{
  assert_choice_type(types::e2conn_upd_ack, type_, "SuccessfulOutcome");
  return c.get<e2conn_upd_ack_s>();
}
const reset_resp_s& e2ap_elem_procs_o::successful_outcome_c::reset_resp() const
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
const e2_removal_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2_removal_resp() const
{
  assert_choice_type(types::e2_removal_resp, type_, "SuccessfulOutcome");
  return c.get<e2_removal_resp_s>();
}
const e2_res_status_resp_s& e2ap_elem_procs_o::successful_outcome_c::e2_res_status_resp() const
{
  assert_choice_type(types::e2_res_status_resp, type_, "SuccessfulOutcome");
  return c.get<e2_res_status_resp_s>();
}

static const char* enum_names_e2ap_elem_procs_o_successful_outcome_c_types_values[] = {
    "RICsubscriptionResponse",
    "RICsubscriptionAuditResponse",
    "RICsubscriptionDeleteResponse",
    "RICsubscriptionModificationResponse",
    "RICsubscriptionModificationConfirm",
    "RICsubscriptionStateControlResponse",
    "RICqueryResponse",
    "RICserviceUpdateAcknowledge",
    "RICassistanceResponse",
    "RICcontrolAcknowledge",
    "RICserviceLoadStatusResponse",
    "E2setupResponse",
    "E2nodeConfigurationUpdateAcknowledge",
    "E2connectionUpdateAcknowledge",
    "ResetResponse",
    "E2RemovalResponse",
    "E2ResourceStatusResponse"};
const asn1::table::enum_desc e2ap_elem_procs_o::successful_outcome_c::types_opts::desc = {
    enum_names_e2ap_elem_procs_o_successful_outcome_c_types_values,
    17,
    "e2ap_elem_procs_o::successful_outcome_c::types"};

// UnsuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e2ap_elem_procs_o_unsuccessful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_audit_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_delete_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_mod_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_mod_refuse_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_sub_state_ctrl_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_query_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_service_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_assist_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_ctrl_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ric_service_load_status_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2node_cfg_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2conn_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2_removal_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e2_res_status_fail_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_e2ap_elem_procs_o_unsuccessful_outcome_c_names[] = {
    {"RICsubscriptionFailure", nullptr},
    {"RICsubscriptionAuditFailure", nullptr},
    {"RICsubscriptionDeleteFailure", nullptr},
    {"RICsubscriptionModificationFailure", nullptr},
    {"RICsubscriptionModificationRefuse", nullptr},
    {"RICsubscriptionStateControlFailure", nullptr},
    {"RICqueryFailure", nullptr},
    {"RICserviceUpdateFailure", nullptr},
    {"RICassistanceFailure", nullptr},
    {"RICcontrolFailure", nullptr},
    {"RICserviceLoadStatusFailure", nullptr},
    {"E2setupFailure", nullptr},
    {"E2nodeConfigurationUpdateFailure", nullptr},
    {"E2connectionUpdateFailure", nullptr},
    {"E2RemovalFailure", nullptr},
    {"E2ResourceStatusFailure", nullptr}};
const asn1::table::choice_desc e2ap_elem_procs_o::unsuccessful_outcome_c::desc = {
    asn1::table::enum_int_type<e2ap_elem_procs_o::unsuccessful_outcome_c::types>(),
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::nof_types,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::nof_exts,
    e2ap_elem_procs_o::unsuccessful_outcome_c::types::has_ext,
    true,
    choice_table_e2ap_elem_procs_o_unsuccessful_outcome_c_alternatives,
    &asn1::table::choice_set<e2ap_elem_procs_o::unsuccessful_outcome_c>,
    choice_table_e2ap_elem_procs_o_unsuccessful_outcome_c_names,
    "e2ap_elem_procs_o::unsuccessful_outcome_c",
    offsetof(e2ap_elem_procs_o::unsuccessful_outcome_c, type_),
    offsetof(e2ap_elem_procs_o::unsuccessful_outcome_c, c),
    false,
    nullptr,
    true};

void e2ap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, e2ap_elem_procs_o::unsuccessful_outcome_c::desc);
}
ric_sub_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_fail()
{
  assert_choice_type(types::ric_sub_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_fail_s>();
}
ric_sub_audit_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_audit_fail()
{
  assert_choice_type(types::ric_sub_audit_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_audit_fail_s>();
}
ric_sub_delete_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_delete_fail()
{
  assert_choice_type(types::ric_sub_delete_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_delete_fail_s>();
}
ric_sub_mod_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_fail()
{
  assert_choice_type(types::ric_sub_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_fail_s>();
}
ric_sub_mod_refuse_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_refuse()
{
  assert_choice_type(types::ric_sub_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_refuse_s>();
}
ric_sub_state_ctrl_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_state_ctrl_fail()
{
  assert_choice_type(types::ric_sub_state_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_state_ctrl_fail_s>();
}
ric_query_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_query_fail()
{
  assert_choice_type(types::ric_query_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_query_fail_s>();
}
ric_service_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_service_upd_fail()
{
  assert_choice_type(types::ric_service_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_upd_fail_s>();
}
ric_assist_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_assist_fail()
{
  assert_choice_type(types::ric_assist_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_assist_fail_s>();
}
ric_ctrl_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_ctrl_fail()
{
  assert_choice_type(types::ric_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_ctrl_fail_s>();
}
ric_service_load_status_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_service_load_status_fail()
{
  assert_choice_type(types::ric_service_load_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_load_status_fail_s>();
}
e2setup_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2setup_fail()
{
  assert_choice_type(types::e2setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2setup_fail_s>();
}
e2node_cfg_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2node_cfg_upd_fail()
{
  assert_choice_type(types::e2node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2node_cfg_upd_fail_s>();
}
e2conn_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2conn_upd_fail()
{
  assert_choice_type(types::e2conn_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2conn_upd_fail_s>();
}
e2_removal_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2_removal_fail()
{
  assert_choice_type(types::e2_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_removal_fail_s>();
}
e2_res_status_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2_res_status_fail()
{
  assert_choice_type(types::e2_res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_res_status_fail_s>();
}
const ric_sub_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_fail() const
{
  assert_choice_type(types::ric_sub_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_fail_s>();
}
const ric_sub_audit_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_audit_fail() const
{
  assert_choice_type(types::ric_sub_audit_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_audit_fail_s>();
}
const ric_sub_delete_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_delete_fail() const
{
  assert_choice_type(types::ric_sub_delete_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_delete_fail_s>();
}
const ric_sub_mod_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_fail() const
{
  assert_choice_type(types::ric_sub_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_fail_s>();
}
const ric_sub_mod_refuse_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_mod_refuse() const
{
  assert_choice_type(types::ric_sub_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_mod_refuse_s>();
}
const ric_sub_state_ctrl_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_sub_state_ctrl_fail() const
{
  assert_choice_type(types::ric_sub_state_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_sub_state_ctrl_fail_s>();
}
const ric_query_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_query_fail() const
{
  assert_choice_type(types::ric_query_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_query_fail_s>();
}
const ric_service_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_service_upd_fail() const
{
  assert_choice_type(types::ric_service_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_upd_fail_s>();
}
const ric_assist_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_assist_fail() const
{
  assert_choice_type(types::ric_assist_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_assist_fail_s>();
}
const ric_ctrl_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_ctrl_fail() const
{
  assert_choice_type(types::ric_ctrl_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_ctrl_fail_s>();
}
const ric_service_load_status_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::ric_service_load_status_fail() const
{
  assert_choice_type(types::ric_service_load_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<ric_service_load_status_fail_s>();
}
const e2setup_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2setup_fail() const
{
  assert_choice_type(types::e2setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2setup_fail_s>();
}
const e2node_cfg_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2node_cfg_upd_fail() const
{
  assert_choice_type(types::e2node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2node_cfg_upd_fail_s>();
}
const e2conn_upd_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2conn_upd_fail() const
{
  assert_choice_type(types::e2conn_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2conn_upd_fail_s>();
}
const e2_removal_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2_removal_fail() const
{
  assert_choice_type(types::e2_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_removal_fail_s>();
}
const e2_res_status_fail_s& e2ap_elem_procs_o::unsuccessful_outcome_c::e2_res_status_fail() const
{
  assert_choice_type(types::e2_res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<e2_res_status_fail_s>();
}

static const char* enum_names_e2ap_elem_procs_o_unsuccessful_outcome_c_types_values[] = {
    "RICsubscriptionFailure",
    "RICsubscriptionAuditFailure",
    "RICsubscriptionDeleteFailure",
    "RICsubscriptionModificationFailure",
    "RICsubscriptionModificationRefuse",
    "RICsubscriptionStateControlFailure",
    "RICqueryFailure",
    "RICserviceUpdateFailure",
    "RICassistanceFailure",
    "RICcontrolFailure",
    "RICserviceLoadStatusFailure",
    "E2setupFailure",
    "E2nodeConfigurationUpdateFailure",
    "E2connectionUpdateFailure",
    "E2RemovalFailure",
    "E2ResourceStatusFailure"};
const asn1::table::enum_desc e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::desc = {
    enum_names_e2ap_elem_procs_o_unsuccessful_outcome_c_types_values,
    16,
    "e2ap_elem_procs_o::unsuccessful_outcome_c::types"};

// Columns of the object sets of E2AP-ELEMENTARY-PROCEDURE.
enum class e2ap_elem_proc_p_column : uint32_t { init_msg, successful_outcome, unsuccessful_outcome, crit };

// InitiatingMessage ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
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
                                 e2ap_elem_procs_o::desc,
                                 e2ap_elem_proc_p_column::crit,
                                 asn1::table::string_json>::value},
    {offsetof(init_msg_s, value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     (int64_t)offsetof(init_msg_s, proc_code) - (int64_t)offsetof(init_msg_s, value),
     0,
     &asn1::table::selected_codec<decltype(init_msg_s::value),
                                  decltype(init_msg_s::proc_code),
                                  e2ap_elem_procs_o::desc,
                                  e2ap_elem_proc_p_column::init_msg,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_init_msg_s_names[] = {{"procedureCode", nullptr},
                                                                     {"criticality", nullptr},
                                                                     {"value", nullptr}};
const asn1::table::seq_desc          init_msg_s::desc =
    {{seq_table_init_msg_s_fields, uint64_t{0x7}, nullptr, seq_table_init_msg_s_names, 3, 0}, false, true, nullptr, 0};
bool init_msg_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(e2ap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(e2ap_elem_procs_o::desc, e2ap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, e2ap_elem_procs_o::desc, e2ap_elem_proc_p_column::init_msg, proc_code);
  return value.type().value != e2ap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
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
                                 e2ap_elem_procs_o::desc,
                                 e2ap_elem_proc_p_column::crit,
                                 asn1::table::string_json>::value},
    {offsetof(successful_outcome_s, value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     (int64_t)offsetof(successful_outcome_s, proc_code) - (int64_t)offsetof(successful_outcome_s, value),
     0,
     &asn1::table::selected_codec<decltype(successful_outcome_s::value),
                                  decltype(successful_outcome_s::proc_code),
                                  e2ap_elem_procs_o::desc,
                                  e2ap_elem_proc_p_column::successful_outcome,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_successful_outcome_s_names[] = {{"procedureCode", nullptr},
                                                                               {"criticality", nullptr},
                                                                               {"value", nullptr}};
const asn1::table::seq_desc          successful_outcome_s::desc             = {
    {seq_table_successful_outcome_s_fields, uint64_t{0x7}, nullptr, seq_table_successful_outcome_s_names, 3, 0},
    false,
    true,
    nullptr,
    0};
bool successful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(e2ap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(e2ap_elem_procs_o::desc, e2ap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, e2ap_elem_procs_o::desc, e2ap_elem_proc_p_column::successful_outcome, proc_code);
  return value.type().value != e2ap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{E2AP-ELEMENTARY-PROCEDURE}}
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
                                 e2ap_elem_procs_o::desc,
                                 e2ap_elem_proc_p_column::crit,
                                 asn1::table::string_json>::value},
    {offsetof(unsuccessful_outcome_s, value),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     (int64_t)offsetof(unsuccessful_outcome_s, proc_code) - (int64_t)offsetof(unsuccessful_outcome_s, value),
     0,
     &asn1::table::selected_codec<decltype(unsuccessful_outcome_s::value),
                                  decltype(unsuccessful_outcome_s::proc_code),
                                  e2ap_elem_procs_o::desc,
                                  e2ap_elem_proc_p_column::unsuccessful_outcome,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_unsuccessful_outcome_s_names[] = {{"procedureCode", nullptr},
                                                                                 {"criticality", nullptr},
                                                                                 {"value", nullptr}};
const asn1::table::seq_desc          unsuccessful_outcome_s::desc             = {
    {seq_table_unsuccessful_outcome_s_fields, uint64_t{0x7}, nullptr, seq_table_unsuccessful_outcome_s_names, 3, 0},
    false,
    true,
    nullptr,
    0};
bool unsuccessful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(e2ap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(e2ap_elem_procs_o::desc, e2ap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, e2ap_elem_procs_o::desc, e2ap_elem_proc_p_column::unsuccessful_outcome, proc_code);
  return value.type().value != e2ap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// E2AP-PDU ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_e2ap_pdu_c_alternatives[] = {
    &asn1::table::alternative_ops<init_msg_s, false, false>::value,
    &asn1::table::alternative_ops<successful_outcome_s, false, false>::value,
    &asn1::table::alternative_ops<unsuccessful_outcome_s, false, false>::value};
const asn1::table::choice_storage_desc e2ap_pdu_c::storage_desc = {choice_storage_e2ap_pdu_c_alternatives,
                                                                   3,
                                                                   "e2ap_pdu_c"};

init_msg_s& e2ap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& e2ap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& e2ap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
static const asn1::table::seq_field  choice_table_e2ap_pdu_c_alternatives[] = {{0,
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
static const asn1::table::field_name choice_table_e2ap_pdu_c_names[]        = {{"initiatingMessage", nullptr},
                                                                               {"successfulOutcome", nullptr},
                                                                               {"unsuccessfulOutcome", nullptr}};
const asn1::table::choice_desc       e2ap_pdu_c::desc = {asn1::table::enum_int_type<e2ap_pdu_c::types>(),
                                                         e2ap_pdu_c::types::nof_types,
                                                         e2ap_pdu_c::types::nof_exts,
                                                         e2ap_pdu_c::types::has_ext,
                                                         true,
                                                         choice_table_e2ap_pdu_c_alternatives,
                                                         nullptr,
                                                         choice_table_e2ap_pdu_c_names,
                                                         "e2ap_pdu_c",
                                                         offsetof(e2ap_pdu_c, c.type),
                                                         offsetof(e2ap_pdu_c, c.value),
                                                         false,
                                                         &e2ap_pdu_c::storage_desc};

static const char*           enum_names_e2ap_pdu_c_types_values[] = {"initiatingMessage",
                                                                     "successfulOutcome",
                                                                     "unsuccessfulOutcome"};
const asn1::table::enum_desc e2ap_pdu_c::types_opts::desc         = {enum_names_e2ap_pdu_c_types_values,
                                                                     3,
                                                                     "e2ap_pdu_c::types"};
