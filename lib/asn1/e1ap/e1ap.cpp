// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/e1ap/e1ap.h"
#include "ocudu/asn1/asn1_ap_codec.h"
#include "ocudu/asn1/e1ap/e1ap_pdu_contents.h"

using namespace asn1;
using namespace asn1::e1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// E1AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF E1AP-ELEMENTARY-PROCEDURE
static const uint32_t objset_proc_code_e1ap_elem_procs_o[] = {0,  3,  4,  5,  6,  7,  8,  9,  10, 11, 22, 24, 29, 30,
                                                              31, 32, 34, 35, 36, 37, 1,  12, 13, 14, 18, 15, 16, 17,
                                                              19, 21, 20, 2,  25, 23, 26, 27, 28, 33, 38, 39};
static const uint8_t  objset_init_msg_e1ap_elem_procs_o[]  = {
    e1ap_elem_procs_o::init_msg_c::types::reset,
    e1ap_elem_procs_o::init_msg_c::types::gnb_cu_up_e1_setup_request,
    e1ap_elem_procs_o::init_msg_c::types::gnb_cu_cp_e1_setup_request,
    e1ap_elem_procs_o::init_msg_c::types::gnb_cu_up_cfg_upd,
    e1ap_elem_procs_o::init_msg_c::types::gnb_cu_cp_cfg_upd,
    e1ap_elem_procs_o::init_msg_c::types::e1_release_request,
    e1ap_elem_procs_o::init_msg_c::types::bearer_context_setup_request,
    e1ap_elem_procs_o::init_msg_c::types::bearer_context_mod_request,
    e1ap_elem_procs_o::init_msg_c::types::bearer_context_mod_required,
    e1ap_elem_procs_o::init_msg_c::types::bearer_context_release_cmd,
    e1ap_elem_procs_o::init_msg_c::types::res_status_request,
    e1ap_elem_procs_o::init_msg_c::types::iab_up_tnl_address_upd,
    e1ap_elem_procs_o::init_msg_c::types::bc_bearer_context_setup_request,
    e1ap_elem_procs_o::init_msg_c::types::bc_bearer_context_mod_request,
    e1ap_elem_procs_o::init_msg_c::types::bc_bearer_context_mod_required,
    e1ap_elem_procs_o::init_msg_c::types::bc_bearer_context_release_cmd,
    e1ap_elem_procs_o::init_msg_c::types::mc_bearer_context_setup_request,
    e1ap_elem_procs_o::init_msg_c::types::mc_bearer_context_mod_request,
    e1ap_elem_procs_o::init_msg_c::types::mc_bearer_context_mod_required,
    e1ap_elem_procs_o::init_msg_c::types::mc_bearer_context_release_cmd,
    e1ap_elem_procs_o::init_msg_c::types::error_ind,
    e1ap_elem_procs_o::init_msg_c::types::bearer_context_release_request,
    e1ap_elem_procs_o::init_msg_c::types::bearer_context_inactivity_notif,
    e1ap_elem_procs_o::init_msg_c::types::dl_data_notif,
    e1ap_elem_procs_o::init_msg_c::types::ul_data_notif,
    e1ap_elem_procs_o::init_msg_c::types::data_usage_report,
    e1ap_elem_procs_o::init_msg_c::types::gnb_cu_up_counter_check_request,
    e1ap_elem_procs_o::init_msg_c::types::gnb_cu_up_status_ind,
    e1ap_elem_procs_o::init_msg_c::types::mrdc_data_usage_report,
    e1ap_elem_procs_o::init_msg_c::types::deactiv_trace,
    e1ap_elem_procs_o::init_msg_c::types::trace_start,
    e1ap_elem_procs_o::init_msg_c::types::private_msg,
    e1ap_elem_procs_o::init_msg_c::types::cell_traffic_trace,
    e1ap_elem_procs_o::init_msg_c::types::res_status_upd,
    e1ap_elem_procs_o::init_msg_c::types::early_forwarding_sn_transfer,
    e1ap_elem_procs_o::init_msg_c::types::gnb_cu_cp_meas_results_info,
    e1ap_elem_procs_o::init_msg_c::types::iabpsk_notif,
    e1ap_elem_procs_o::init_msg_c::types::bc_bearer_context_release_request,
    e1ap_elem_procs_o::init_msg_c::types::mc_bearer_context_release_request,
    e1ap_elem_procs_o::init_msg_c::types::mc_bearer_notif};
static const uint8_t objset_successful_outcome_e1ap_elem_procs_o[] = {
    e1ap_elem_procs_o::successful_outcome_c::types::reset_ack,
    e1ap_elem_procs_o::successful_outcome_c::types::gnb_cu_up_e1_setup_resp,
    e1ap_elem_procs_o::successful_outcome_c::types::gnb_cu_cp_e1_setup_resp,
    e1ap_elem_procs_o::successful_outcome_c::types::gnb_cu_up_cfg_upd_ack,
    e1ap_elem_procs_o::successful_outcome_c::types::gnb_cu_cp_cfg_upd_ack,
    e1ap_elem_procs_o::successful_outcome_c::types::e1_release_resp,
    e1ap_elem_procs_o::successful_outcome_c::types::bearer_context_setup_resp,
    e1ap_elem_procs_o::successful_outcome_c::types::bearer_context_mod_resp,
    e1ap_elem_procs_o::successful_outcome_c::types::bearer_context_mod_confirm,
    e1ap_elem_procs_o::successful_outcome_c::types::bearer_context_release_complete,
    e1ap_elem_procs_o::successful_outcome_c::types::res_status_resp,
    e1ap_elem_procs_o::successful_outcome_c::types::iab_up_tnl_address_upd_ack,
    e1ap_elem_procs_o::successful_outcome_c::types::bc_bearer_context_setup_resp,
    e1ap_elem_procs_o::successful_outcome_c::types::bc_bearer_context_mod_resp,
    e1ap_elem_procs_o::successful_outcome_c::types::bc_bearer_context_mod_confirm,
    e1ap_elem_procs_o::successful_outcome_c::types::bc_bearer_context_release_complete,
    e1ap_elem_procs_o::successful_outcome_c::types::mc_bearer_context_setup_resp,
    e1ap_elem_procs_o::successful_outcome_c::types::mc_bearer_context_mod_resp,
    e1ap_elem_procs_o::successful_outcome_c::types::mc_bearer_context_mod_confirm,
    e1ap_elem_procs_o::successful_outcome_c::types::mc_bearer_context_release_complete,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::successful_outcome_c::types::nulltype};
static const uint8_t objset_unsuccessful_outcome_e1ap_elem_procs_o[] = {
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::gnb_cu_up_e1_setup_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::gnb_cu_cp_e1_setup_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::gnb_cu_up_cfg_upd_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::gnb_cu_cp_cfg_upd_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::bearer_context_setup_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::bearer_context_mod_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::res_status_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::iab_up_tnl_address_upd_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::bc_bearer_context_setup_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::bc_bearer_context_mod_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::mc_bearer_context_setup_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::mc_bearer_context_mod_fail,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype};
static const uint8_t objset_crit_e1ap_elem_procs_o[] = {
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject};
static const asn1::table::objset_column objset_columns_e1ap_elem_procs_o[] = {
    {objset_init_msg_e1ap_elem_procs_o, e1ap_elem_procs_o::init_msg_c::types::nulltype},
    {objset_successful_outcome_e1ap_elem_procs_o, e1ap_elem_procs_o::successful_outcome_c::types::nulltype},
    {objset_unsuccessful_outcome_e1ap_elem_procs_o, e1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype},
    {objset_crit_e1ap_elem_procs_o, crit_e::nulltype}};
const asn1::table::objset_desc e1ap_elem_procs_o::desc = {objset_proc_code_e1ap_elem_procs_o,
                                                          40,
                                                          "proc_code",
                                                          objset_columns_e1ap_elem_procs_o};

// InitiatingMessage ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e1ap_elem_procs_o_init_msg_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<reset_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_up_e1_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_cp_e1_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_up_cfg_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_cp_cfg_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e1_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bearer_context_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bearer_context_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bearer_context_mod_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bearer_context_release_cmd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<res_status_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_up_tnl_address_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bc_bearer_context_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bc_bearer_context_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bc_bearer_context_mod_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bc_bearer_context_release_cmd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_context_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_context_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_context_mod_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_context_release_cmd_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<bearer_context_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bearer_context_inactivity_notif_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<dl_data_notif_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ul_data_notif_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<data_usage_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_up_counter_check_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_up_status_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mrdc_data_usage_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<deactiv_trace_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<trace_start_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<cell_traffic_trace_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<res_status_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<early_forwarding_sn_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_cp_meas_results_info_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iabpsk_notif_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bc_bearer_context_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_context_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_notif_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_e1ap_elem_procs_o_init_msg_c_names[] = {
    {"Reset", nullptr},
    {"GNB-CU-UP-E1SetupRequest", nullptr},
    {"GNB-CU-CP-E1SetupRequest", nullptr},
    {"GNB-CU-UP-ConfigurationUpdate", nullptr},
    {"GNB-CU-CP-ConfigurationUpdate", nullptr},
    {"E1ReleaseRequest", nullptr},
    {"BearerContextSetupRequest", nullptr},
    {"BearerContextModificationRequest", nullptr},
    {"BearerContextModificationRequired", nullptr},
    {"BearerContextReleaseCommand", nullptr},
    {"ResourceStatusRequest", nullptr},
    {"IAB-UPTNLAddressUpdate", nullptr},
    {"BCBearerContextSetupRequest", nullptr},
    {"BCBearerContextModificationRequest", nullptr},
    {"BCBearerContextModificationRequired", nullptr},
    {"BCBearerContextReleaseCommand", nullptr},
    {"MCBearerContextSetupRequest", nullptr},
    {"MCBearerContextModificationRequest", nullptr},
    {"MCBearerContextModificationRequired", nullptr},
    {"MCBearerContextReleaseCommand", nullptr},
    {"ErrorIndication", nullptr},
    {"BearerContextReleaseRequest", nullptr},
    {"BearerContextInactivityNotification", nullptr},
    {"DLDataNotification", nullptr},
    {"ULDataNotification", nullptr},
    {"DataUsageReport", nullptr},
    {"GNB-CU-UP-CounterCheckRequest", nullptr},
    {"GNB-CU-UP-StatusIndication", nullptr},
    {"MRDC-DataUsageReport", nullptr},
    {"DeactivateTrace", nullptr},
    {"TraceStart", nullptr},
    {"PrivateMessage", nullptr},
    {"CellTrafficTrace", nullptr},
    {"ResourceStatusUpdate", nullptr},
    {"EarlyForwardingSNTransfer", nullptr},
    {"GNB-CU-CPMeasurementResultsInformation", nullptr},
    {"IABPSKNotification", nullptr},
    {"BCBearerContextReleaseRequest", nullptr},
    {"MCBearerContextReleaseRequest", nullptr},
    {"MCBearerNotification", nullptr}};
const asn1::table::choice_desc e1ap_elem_procs_o::init_msg_c::desc = {
    asn1::table::enum_int_type<e1ap_elem_procs_o::init_msg_c::types>(),
    e1ap_elem_procs_o::init_msg_c::types::nof_types,
    e1ap_elem_procs_o::init_msg_c::types::nof_exts,
    e1ap_elem_procs_o::init_msg_c::types::has_ext,
    true,
    choice_table_e1ap_elem_procs_o_init_msg_c_alternatives,
    &asn1::table::choice_set<e1ap_elem_procs_o::init_msg_c>,
    choice_table_e1ap_elem_procs_o_init_msg_c_names,
    "e1ap_elem_procs_o::init_msg_c",
    offsetof(e1ap_elem_procs_o::init_msg_c, type_),
    offsetof(e1ap_elem_procs_o::init_msg_c, c),
    false,
    nullptr,
    true};

void e1ap_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, e1ap_elem_procs_o::init_msg_c::desc);
}
reset_s& e1ap_elem_procs_o::init_msg_c::reset()
{
  assert_choice_type(types::reset, type_, "InitiatingMessage");
  return c.get<reset_s>();
}
gnb_cu_up_e1_setup_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_e1_setup_request()
{
  assert_choice_type(types::gnb_cu_up_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_e1_setup_request_s>();
}
gnb_cu_cp_e1_setup_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_e1_setup_request()
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_e1_setup_request_s>();
}
gnb_cu_up_cfg_upd_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_cfg_upd()
{
  assert_choice_type(types::gnb_cu_up_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_cfg_upd_s>();
}
gnb_cu_cp_cfg_upd_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_cfg_upd()
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_cfg_upd_s>();
}
e1_release_request_s& e1ap_elem_procs_o::init_msg_c::e1_release_request()
{
  assert_choice_type(types::e1_release_request, type_, "InitiatingMessage");
  return c.get<e1_release_request_s>();
}
bearer_context_setup_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_setup_request()
{
  assert_choice_type(types::bearer_context_setup_request, type_, "InitiatingMessage");
  return c.get<bearer_context_setup_request_s>();
}
bearer_context_mod_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_mod_request()
{
  assert_choice_type(types::bearer_context_mod_request, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_request_s>();
}
bearer_context_mod_required_s& e1ap_elem_procs_o::init_msg_c::bearer_context_mod_required()
{
  assert_choice_type(types::bearer_context_mod_required, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_required_s>();
}
bearer_context_release_cmd_s& e1ap_elem_procs_o::init_msg_c::bearer_context_release_cmd()
{
  assert_choice_type(types::bearer_context_release_cmd, type_, "InitiatingMessage");
  return c.get<bearer_context_release_cmd_s>();
}
res_status_request_s& e1ap_elem_procs_o::init_msg_c::res_status_request()
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
iab_up_tnl_address_upd_s& e1ap_elem_procs_o::init_msg_c::iab_up_tnl_address_upd()
{
  assert_choice_type(types::iab_up_tnl_address_upd, type_, "InitiatingMessage");
  return c.get<iab_up_tnl_address_upd_s>();
}
bc_bearer_context_setup_request_s& e1ap_elem_procs_o::init_msg_c::bc_bearer_context_setup_request()
{
  assert_choice_type(types::bc_bearer_context_setup_request, type_, "InitiatingMessage");
  return c.get<bc_bearer_context_setup_request_s>();
}
bc_bearer_context_mod_request_s& e1ap_elem_procs_o::init_msg_c::bc_bearer_context_mod_request()
{
  assert_choice_type(types::bc_bearer_context_mod_request, type_, "InitiatingMessage");
  return c.get<bc_bearer_context_mod_request_s>();
}
bc_bearer_context_mod_required_s& e1ap_elem_procs_o::init_msg_c::bc_bearer_context_mod_required()
{
  assert_choice_type(types::bc_bearer_context_mod_required, type_, "InitiatingMessage");
  return c.get<bc_bearer_context_mod_required_s>();
}
bc_bearer_context_release_cmd_s& e1ap_elem_procs_o::init_msg_c::bc_bearer_context_release_cmd()
{
  assert_choice_type(types::bc_bearer_context_release_cmd, type_, "InitiatingMessage");
  return c.get<bc_bearer_context_release_cmd_s>();
}
mc_bearer_context_setup_request_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_context_setup_request()
{
  assert_choice_type(types::mc_bearer_context_setup_request, type_, "InitiatingMessage");
  return c.get<mc_bearer_context_setup_request_s>();
}
mc_bearer_context_mod_request_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_context_mod_request()
{
  assert_choice_type(types::mc_bearer_context_mod_request, type_, "InitiatingMessage");
  return c.get<mc_bearer_context_mod_request_s>();
}
mc_bearer_context_mod_required_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_context_mod_required()
{
  assert_choice_type(types::mc_bearer_context_mod_required, type_, "InitiatingMessage");
  return c.get<mc_bearer_context_mod_required_s>();
}
mc_bearer_context_release_cmd_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_context_release_cmd()
{
  assert_choice_type(types::mc_bearer_context_release_cmd, type_, "InitiatingMessage");
  return c.get<mc_bearer_context_release_cmd_s>();
}
error_ind_s& e1ap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
bearer_context_release_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_release_request()
{
  assert_choice_type(types::bearer_context_release_request, type_, "InitiatingMessage");
  return c.get<bearer_context_release_request_s>();
}
bearer_context_inactivity_notif_s& e1ap_elem_procs_o::init_msg_c::bearer_context_inactivity_notif()
{
  assert_choice_type(types::bearer_context_inactivity_notif, type_, "InitiatingMessage");
  return c.get<bearer_context_inactivity_notif_s>();
}
dl_data_notif_s& e1ap_elem_procs_o::init_msg_c::dl_data_notif()
{
  assert_choice_type(types::dl_data_notif, type_, "InitiatingMessage");
  return c.get<dl_data_notif_s>();
}
ul_data_notif_s& e1ap_elem_procs_o::init_msg_c::ul_data_notif()
{
  assert_choice_type(types::ul_data_notif, type_, "InitiatingMessage");
  return c.get<ul_data_notif_s>();
}
data_usage_report_s& e1ap_elem_procs_o::init_msg_c::data_usage_report()
{
  assert_choice_type(types::data_usage_report, type_, "InitiatingMessage");
  return c.get<data_usage_report_s>();
}
gnb_cu_up_counter_check_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_counter_check_request()
{
  assert_choice_type(types::gnb_cu_up_counter_check_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_counter_check_request_s>();
}
gnb_cu_up_status_ind_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_status_ind()
{
  assert_choice_type(types::gnb_cu_up_status_ind, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_status_ind_s>();
}
mrdc_data_usage_report_s& e1ap_elem_procs_o::init_msg_c::mrdc_data_usage_report()
{
  assert_choice_type(types::mrdc_data_usage_report, type_, "InitiatingMessage");
  return c.get<mrdc_data_usage_report_s>();
}
deactiv_trace_s& e1ap_elem_procs_o::init_msg_c::deactiv_trace()
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
trace_start_s& e1ap_elem_procs_o::init_msg_c::trace_start()
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
private_msg_s& e1ap_elem_procs_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
cell_traffic_trace_s& e1ap_elem_procs_o::init_msg_c::cell_traffic_trace()
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
res_status_upd_s& e1ap_elem_procs_o::init_msg_c::res_status_upd()
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
early_forwarding_sn_transfer_s& e1ap_elem_procs_o::init_msg_c::early_forwarding_sn_transfer()
{
  assert_choice_type(types::early_forwarding_sn_transfer, type_, "InitiatingMessage");
  return c.get<early_forwarding_sn_transfer_s>();
}
gnb_cu_cp_meas_results_info_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_meas_results_info()
{
  assert_choice_type(types::gnb_cu_cp_meas_results_info, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_meas_results_info_s>();
}
iabpsk_notif_s& e1ap_elem_procs_o::init_msg_c::iabpsk_notif()
{
  assert_choice_type(types::iabpsk_notif, type_, "InitiatingMessage");
  return c.get<iabpsk_notif_s>();
}
bc_bearer_context_release_request_s& e1ap_elem_procs_o::init_msg_c::bc_bearer_context_release_request()
{
  assert_choice_type(types::bc_bearer_context_release_request, type_, "InitiatingMessage");
  return c.get<bc_bearer_context_release_request_s>();
}
mc_bearer_context_release_request_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_context_release_request()
{
  assert_choice_type(types::mc_bearer_context_release_request, type_, "InitiatingMessage");
  return c.get<mc_bearer_context_release_request_s>();
}
mc_bearer_notif_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_notif()
{
  assert_choice_type(types::mc_bearer_notif, type_, "InitiatingMessage");
  return c.get<mc_bearer_notif_s>();
}
const reset_s& e1ap_elem_procs_o::init_msg_c::reset() const
{
  assert_choice_type(types::reset, type_, "InitiatingMessage");
  return c.get<reset_s>();
}
const gnb_cu_up_e1_setup_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_e1_setup_request() const
{
  assert_choice_type(types::gnb_cu_up_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_e1_setup_request_s>();
}
const gnb_cu_cp_e1_setup_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_e1_setup_request() const
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_e1_setup_request_s>();
}
const gnb_cu_up_cfg_upd_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_cfg_upd() const
{
  assert_choice_type(types::gnb_cu_up_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_cfg_upd_s>();
}
const gnb_cu_cp_cfg_upd_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_cfg_upd() const
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_cfg_upd_s>();
}
const e1_release_request_s& e1ap_elem_procs_o::init_msg_c::e1_release_request() const
{
  assert_choice_type(types::e1_release_request, type_, "InitiatingMessage");
  return c.get<e1_release_request_s>();
}
const bearer_context_setup_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_setup_request() const
{
  assert_choice_type(types::bearer_context_setup_request, type_, "InitiatingMessage");
  return c.get<bearer_context_setup_request_s>();
}
const bearer_context_mod_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_mod_request() const
{
  assert_choice_type(types::bearer_context_mod_request, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_request_s>();
}
const bearer_context_mod_required_s& e1ap_elem_procs_o::init_msg_c::bearer_context_mod_required() const
{
  assert_choice_type(types::bearer_context_mod_required, type_, "InitiatingMessage");
  return c.get<bearer_context_mod_required_s>();
}
const bearer_context_release_cmd_s& e1ap_elem_procs_o::init_msg_c::bearer_context_release_cmd() const
{
  assert_choice_type(types::bearer_context_release_cmd, type_, "InitiatingMessage");
  return c.get<bearer_context_release_cmd_s>();
}
const res_status_request_s& e1ap_elem_procs_o::init_msg_c::res_status_request() const
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
const iab_up_tnl_address_upd_s& e1ap_elem_procs_o::init_msg_c::iab_up_tnl_address_upd() const
{
  assert_choice_type(types::iab_up_tnl_address_upd, type_, "InitiatingMessage");
  return c.get<iab_up_tnl_address_upd_s>();
}
const bc_bearer_context_setup_request_s& e1ap_elem_procs_o::init_msg_c::bc_bearer_context_setup_request() const
{
  assert_choice_type(types::bc_bearer_context_setup_request, type_, "InitiatingMessage");
  return c.get<bc_bearer_context_setup_request_s>();
}
const bc_bearer_context_mod_request_s& e1ap_elem_procs_o::init_msg_c::bc_bearer_context_mod_request() const
{
  assert_choice_type(types::bc_bearer_context_mod_request, type_, "InitiatingMessage");
  return c.get<bc_bearer_context_mod_request_s>();
}
const bc_bearer_context_mod_required_s& e1ap_elem_procs_o::init_msg_c::bc_bearer_context_mod_required() const
{
  assert_choice_type(types::bc_bearer_context_mod_required, type_, "InitiatingMessage");
  return c.get<bc_bearer_context_mod_required_s>();
}
const bc_bearer_context_release_cmd_s& e1ap_elem_procs_o::init_msg_c::bc_bearer_context_release_cmd() const
{
  assert_choice_type(types::bc_bearer_context_release_cmd, type_, "InitiatingMessage");
  return c.get<bc_bearer_context_release_cmd_s>();
}
const mc_bearer_context_setup_request_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_context_setup_request() const
{
  assert_choice_type(types::mc_bearer_context_setup_request, type_, "InitiatingMessage");
  return c.get<mc_bearer_context_setup_request_s>();
}
const mc_bearer_context_mod_request_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_context_mod_request() const
{
  assert_choice_type(types::mc_bearer_context_mod_request, type_, "InitiatingMessage");
  return c.get<mc_bearer_context_mod_request_s>();
}
const mc_bearer_context_mod_required_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_context_mod_required() const
{
  assert_choice_type(types::mc_bearer_context_mod_required, type_, "InitiatingMessage");
  return c.get<mc_bearer_context_mod_required_s>();
}
const mc_bearer_context_release_cmd_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_context_release_cmd() const
{
  assert_choice_type(types::mc_bearer_context_release_cmd, type_, "InitiatingMessage");
  return c.get<mc_bearer_context_release_cmd_s>();
}
const error_ind_s& e1ap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const bearer_context_release_request_s& e1ap_elem_procs_o::init_msg_c::bearer_context_release_request() const
{
  assert_choice_type(types::bearer_context_release_request, type_, "InitiatingMessage");
  return c.get<bearer_context_release_request_s>();
}
const bearer_context_inactivity_notif_s& e1ap_elem_procs_o::init_msg_c::bearer_context_inactivity_notif() const
{
  assert_choice_type(types::bearer_context_inactivity_notif, type_, "InitiatingMessage");
  return c.get<bearer_context_inactivity_notif_s>();
}
const dl_data_notif_s& e1ap_elem_procs_o::init_msg_c::dl_data_notif() const
{
  assert_choice_type(types::dl_data_notif, type_, "InitiatingMessage");
  return c.get<dl_data_notif_s>();
}
const ul_data_notif_s& e1ap_elem_procs_o::init_msg_c::ul_data_notif() const
{
  assert_choice_type(types::ul_data_notif, type_, "InitiatingMessage");
  return c.get<ul_data_notif_s>();
}
const data_usage_report_s& e1ap_elem_procs_o::init_msg_c::data_usage_report() const
{
  assert_choice_type(types::data_usage_report, type_, "InitiatingMessage");
  return c.get<data_usage_report_s>();
}
const gnb_cu_up_counter_check_request_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_counter_check_request() const
{
  assert_choice_type(types::gnb_cu_up_counter_check_request, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_counter_check_request_s>();
}
const gnb_cu_up_status_ind_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_up_status_ind() const
{
  assert_choice_type(types::gnb_cu_up_status_ind, type_, "InitiatingMessage");
  return c.get<gnb_cu_up_status_ind_s>();
}
const mrdc_data_usage_report_s& e1ap_elem_procs_o::init_msg_c::mrdc_data_usage_report() const
{
  assert_choice_type(types::mrdc_data_usage_report, type_, "InitiatingMessage");
  return c.get<mrdc_data_usage_report_s>();
}
const deactiv_trace_s& e1ap_elem_procs_o::init_msg_c::deactiv_trace() const
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
const trace_start_s& e1ap_elem_procs_o::init_msg_c::trace_start() const
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
const private_msg_s& e1ap_elem_procs_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const cell_traffic_trace_s& e1ap_elem_procs_o::init_msg_c::cell_traffic_trace() const
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
const res_status_upd_s& e1ap_elem_procs_o::init_msg_c::res_status_upd() const
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
const early_forwarding_sn_transfer_s& e1ap_elem_procs_o::init_msg_c::early_forwarding_sn_transfer() const
{
  assert_choice_type(types::early_forwarding_sn_transfer, type_, "InitiatingMessage");
  return c.get<early_forwarding_sn_transfer_s>();
}
const gnb_cu_cp_meas_results_info_s& e1ap_elem_procs_o::init_msg_c::gnb_cu_cp_meas_results_info() const
{
  assert_choice_type(types::gnb_cu_cp_meas_results_info, type_, "InitiatingMessage");
  return c.get<gnb_cu_cp_meas_results_info_s>();
}
const iabpsk_notif_s& e1ap_elem_procs_o::init_msg_c::iabpsk_notif() const
{
  assert_choice_type(types::iabpsk_notif, type_, "InitiatingMessage");
  return c.get<iabpsk_notif_s>();
}
const bc_bearer_context_release_request_s& e1ap_elem_procs_o::init_msg_c::bc_bearer_context_release_request() const
{
  assert_choice_type(types::bc_bearer_context_release_request, type_, "InitiatingMessage");
  return c.get<bc_bearer_context_release_request_s>();
}
const mc_bearer_context_release_request_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_context_release_request() const
{
  assert_choice_type(types::mc_bearer_context_release_request, type_, "InitiatingMessage");
  return c.get<mc_bearer_context_release_request_s>();
}
const mc_bearer_notif_s& e1ap_elem_procs_o::init_msg_c::mc_bearer_notif() const
{
  assert_choice_type(types::mc_bearer_notif, type_, "InitiatingMessage");
  return c.get<mc_bearer_notif_s>();
}

static const char*           enum_names_e1ap_elem_procs_o_init_msg_c_types_values[] = {"Reset",
                                                                                       "GNB-CU-UP-E1SetupRequest",
                                                                                       "GNB-CU-CP-E1SetupRequest",
                                                                                       "GNB-CU-UP-ConfigurationUpdate",
                                                                                       "GNB-CU-CP-ConfigurationUpdate",
                                                                                       "E1ReleaseRequest",
                                                                                       "BearerContextSetupRequest",
                                                                                       "BearerContextModificationRequest",
                                                                                       "BearerContextModificationRequired",
                                                                                       "BearerContextReleaseCommand",
                                                                                       "ResourceStatusRequest",
                                                                                       "IAB-UPTNLAddressUpdate",
                                                                                       "BCBearerContextSetupRequest",
                                                                                       "BCBearerContextModificationRequest",
                                                                                       "BCBearerContextModificationRequired",
                                                                                       "BCBearerContextReleaseCommand",
                                                                                       "MCBearerContextSetupRequest",
                                                                                       "MCBearerContextModificationRequest",
                                                                                       "MCBearerContextModificationRequired",
                                                                                       "MCBearerContextReleaseCommand",
                                                                                       "ErrorIndication",
                                                                                       "BearerContextReleaseRequest",
                                                                                       "BearerContextInactivityNotification",
                                                                                       "DLDataNotification",
                                                                                       "ULDataNotification",
                                                                                       "DataUsageReport",
                                                                                       "GNB-CU-UP-CounterCheckRequest",
                                                                                       "GNB-CU-UP-StatusIndication",
                                                                                       "MRDC-DataUsageReport",
                                                                                       "DeactivateTrace",
                                                                                       "TraceStart",
                                                                                       "PrivateMessage",
                                                                                       "CellTrafficTrace",
                                                                                       "ResourceStatusUpdate",
                                                                                       "EarlyForwardingSNTransfer",
                                                                                       "GNB-CU-CPMeasurementResultsInformation",
                                                                                       "IABPSKNotification",
                                                                                       "BCBearerContextReleaseRequest",
                                                                                       "MCBearerContextReleaseRequest",
                                                                                       "MCBearerNotification"};
const asn1::table::enum_desc e1ap_elem_procs_o::init_msg_c::types_opts::desc        = {
    enum_names_e1ap_elem_procs_o_init_msg_c_types_values,
    40,
    "e1ap_elem_procs_o::init_msg_c::types"};

// SuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e1ap_elem_procs_o_successful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<reset_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_up_e1_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_cp_e1_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_up_cfg_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_cp_cfg_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e1_release_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bearer_context_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bearer_context_mod_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bearer_context_mod_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bearer_context_release_complete_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<res_status_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_up_tnl_address_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bc_bearer_context_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bc_bearer_context_mod_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bc_bearer_context_mod_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bc_bearer_context_release_complete_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_context_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_context_mod_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_context_mod_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_context_release_complete_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_e1ap_elem_procs_o_successful_outcome_c_names[] = {
    {"ResetAcknowledge", nullptr},
    {"GNB-CU-UP-E1SetupResponse", nullptr},
    {"GNB-CU-CP-E1SetupResponse", nullptr},
    {"GNB-CU-UP-ConfigurationUpdateAcknowledge", nullptr},
    {"GNB-CU-CP-ConfigurationUpdateAcknowledge", nullptr},
    {"E1ReleaseResponse", nullptr},
    {"BearerContextSetupResponse", nullptr},
    {"BearerContextModificationResponse", nullptr},
    {"BearerContextModificationConfirm", nullptr},
    {"BearerContextReleaseComplete", nullptr},
    {"ResourceStatusResponse", nullptr},
    {"IAB-UPTNLAddressUpdateAcknowledge", nullptr},
    {"BCBearerContextSetupResponse", nullptr},
    {"BCBearerContextModificationResponse", nullptr},
    {"BCBearerContextModificationConfirm", nullptr},
    {"BCBearerContextReleaseComplete", nullptr},
    {"MCBearerContextSetupResponse", nullptr},
    {"MCBearerContextModificationResponse", nullptr},
    {"MCBearerContextModificationConfirm", nullptr},
    {"MCBearerContextReleaseComplete", nullptr}};
const asn1::table::choice_desc e1ap_elem_procs_o::successful_outcome_c::desc = {
    asn1::table::enum_int_type<e1ap_elem_procs_o::successful_outcome_c::types>(),
    e1ap_elem_procs_o::successful_outcome_c::types::nof_types,
    e1ap_elem_procs_o::successful_outcome_c::types::nof_exts,
    e1ap_elem_procs_o::successful_outcome_c::types::has_ext,
    true,
    choice_table_e1ap_elem_procs_o_successful_outcome_c_alternatives,
    &asn1::table::choice_set<e1ap_elem_procs_o::successful_outcome_c>,
    choice_table_e1ap_elem_procs_o_successful_outcome_c_names,
    "e1ap_elem_procs_o::successful_outcome_c",
    offsetof(e1ap_elem_procs_o::successful_outcome_c, type_),
    offsetof(e1ap_elem_procs_o::successful_outcome_c, c),
    false,
    nullptr,
    true};

void e1ap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, e1ap_elem_procs_o::successful_outcome_c::desc);
}
reset_ack_s& e1ap_elem_procs_o::successful_outcome_c::reset_ack()
{
  assert_choice_type(types::reset_ack, type_, "SuccessfulOutcome");
  return c.get<reset_ack_s>();
}
gnb_cu_up_e1_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_up_e1_setup_resp()
{
  assert_choice_type(types::gnb_cu_up_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_resp_s>();
}
gnb_cu_cp_e1_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_e1_setup_resp()
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_resp_s>();
}
gnb_cu_up_cfg_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_up_cfg_upd_ack()
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_ack_s>();
}
gnb_cu_cp_cfg_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_cfg_upd_ack()
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_ack_s>();
}
e1_release_resp_s& e1ap_elem_procs_o::successful_outcome_c::e1_release_resp()
{
  assert_choice_type(types::e1_release_resp, type_, "SuccessfulOutcome");
  return c.get<e1_release_resp_s>();
}
bearer_context_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_setup_resp()
{
  assert_choice_type(types::bearer_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_setup_resp_s>();
}
bearer_context_mod_resp_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_mod_resp()
{
  assert_choice_type(types::bearer_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_resp_s>();
}
bearer_context_mod_confirm_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_mod_confirm()
{
  assert_choice_type(types::bearer_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_confirm_s>();
}
bearer_context_release_complete_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_release_complete()
{
  assert_choice_type(types::bearer_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<bearer_context_release_complete_s>();
}
res_status_resp_s& e1ap_elem_procs_o::successful_outcome_c::res_status_resp()
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
iab_up_tnl_address_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::iab_up_tnl_address_upd_ack()
{
  assert_choice_type(types::iab_up_tnl_address_upd_ack, type_, "SuccessfulOutcome");
  return c.get<iab_up_tnl_address_upd_ack_s>();
}
bc_bearer_context_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::bc_bearer_context_setup_resp()
{
  assert_choice_type(types::bc_bearer_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<bc_bearer_context_setup_resp_s>();
}
bc_bearer_context_mod_resp_s& e1ap_elem_procs_o::successful_outcome_c::bc_bearer_context_mod_resp()
{
  assert_choice_type(types::bc_bearer_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<bc_bearer_context_mod_resp_s>();
}
bc_bearer_context_mod_confirm_s& e1ap_elem_procs_o::successful_outcome_c::bc_bearer_context_mod_confirm()
{
  assert_choice_type(types::bc_bearer_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<bc_bearer_context_mod_confirm_s>();
}
bc_bearer_context_release_complete_s& e1ap_elem_procs_o::successful_outcome_c::bc_bearer_context_release_complete()
{
  assert_choice_type(types::bc_bearer_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<bc_bearer_context_release_complete_s>();
}
mc_bearer_context_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::mc_bearer_context_setup_resp()
{
  assert_choice_type(types::mc_bearer_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<mc_bearer_context_setup_resp_s>();
}
mc_bearer_context_mod_resp_s& e1ap_elem_procs_o::successful_outcome_c::mc_bearer_context_mod_resp()
{
  assert_choice_type(types::mc_bearer_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<mc_bearer_context_mod_resp_s>();
}
mc_bearer_context_mod_confirm_s& e1ap_elem_procs_o::successful_outcome_c::mc_bearer_context_mod_confirm()
{
  assert_choice_type(types::mc_bearer_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<mc_bearer_context_mod_confirm_s>();
}
mc_bearer_context_release_complete_s& e1ap_elem_procs_o::successful_outcome_c::mc_bearer_context_release_complete()
{
  assert_choice_type(types::mc_bearer_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<mc_bearer_context_release_complete_s>();
}
const reset_ack_s& e1ap_elem_procs_o::successful_outcome_c::reset_ack() const
{
  assert_choice_type(types::reset_ack, type_, "SuccessfulOutcome");
  return c.get<reset_ack_s>();
}
const gnb_cu_up_e1_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_up_e1_setup_resp() const
{
  assert_choice_type(types::gnb_cu_up_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_resp_s>();
}
const gnb_cu_cp_e1_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_e1_setup_resp() const
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_resp_s>();
}
const gnb_cu_up_cfg_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_up_cfg_upd_ack() const
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_ack_s>();
}
const gnb_cu_cp_cfg_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::gnb_cu_cp_cfg_upd_ack() const
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_ack_s>();
}
const e1_release_resp_s& e1ap_elem_procs_o::successful_outcome_c::e1_release_resp() const
{
  assert_choice_type(types::e1_release_resp, type_, "SuccessfulOutcome");
  return c.get<e1_release_resp_s>();
}
const bearer_context_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_setup_resp() const
{
  assert_choice_type(types::bearer_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_setup_resp_s>();
}
const bearer_context_mod_resp_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_mod_resp() const
{
  assert_choice_type(types::bearer_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_resp_s>();
}
const bearer_context_mod_confirm_s& e1ap_elem_procs_o::successful_outcome_c::bearer_context_mod_confirm() const
{
  assert_choice_type(types::bearer_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<bearer_context_mod_confirm_s>();
}
const bearer_context_release_complete_s&
e1ap_elem_procs_o::successful_outcome_c::bearer_context_release_complete() const
{
  assert_choice_type(types::bearer_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<bearer_context_release_complete_s>();
}
const res_status_resp_s& e1ap_elem_procs_o::successful_outcome_c::res_status_resp() const
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
const iab_up_tnl_address_upd_ack_s& e1ap_elem_procs_o::successful_outcome_c::iab_up_tnl_address_upd_ack() const
{
  assert_choice_type(types::iab_up_tnl_address_upd_ack, type_, "SuccessfulOutcome");
  return c.get<iab_up_tnl_address_upd_ack_s>();
}
const bc_bearer_context_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::bc_bearer_context_setup_resp() const
{
  assert_choice_type(types::bc_bearer_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<bc_bearer_context_setup_resp_s>();
}
const bc_bearer_context_mod_resp_s& e1ap_elem_procs_o::successful_outcome_c::bc_bearer_context_mod_resp() const
{
  assert_choice_type(types::bc_bearer_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<bc_bearer_context_mod_resp_s>();
}
const bc_bearer_context_mod_confirm_s& e1ap_elem_procs_o::successful_outcome_c::bc_bearer_context_mod_confirm() const
{
  assert_choice_type(types::bc_bearer_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<bc_bearer_context_mod_confirm_s>();
}
const bc_bearer_context_release_complete_s&
e1ap_elem_procs_o::successful_outcome_c::bc_bearer_context_release_complete() const
{
  assert_choice_type(types::bc_bearer_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<bc_bearer_context_release_complete_s>();
}
const mc_bearer_context_setup_resp_s& e1ap_elem_procs_o::successful_outcome_c::mc_bearer_context_setup_resp() const
{
  assert_choice_type(types::mc_bearer_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<mc_bearer_context_setup_resp_s>();
}
const mc_bearer_context_mod_resp_s& e1ap_elem_procs_o::successful_outcome_c::mc_bearer_context_mod_resp() const
{
  assert_choice_type(types::mc_bearer_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<mc_bearer_context_mod_resp_s>();
}
const mc_bearer_context_mod_confirm_s& e1ap_elem_procs_o::successful_outcome_c::mc_bearer_context_mod_confirm() const
{
  assert_choice_type(types::mc_bearer_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<mc_bearer_context_mod_confirm_s>();
}
const mc_bearer_context_release_complete_s&
e1ap_elem_procs_o::successful_outcome_c::mc_bearer_context_release_complete() const
{
  assert_choice_type(types::mc_bearer_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<mc_bearer_context_release_complete_s>();
}

static const char* enum_names_e1ap_elem_procs_o_successful_outcome_c_types_values[] = {
    "ResetAcknowledge",
    "GNB-CU-UP-E1SetupResponse",
    "GNB-CU-CP-E1SetupResponse",
    "GNB-CU-UP-ConfigurationUpdateAcknowledge",
    "GNB-CU-CP-ConfigurationUpdateAcknowledge",
    "E1ReleaseResponse",
    "BearerContextSetupResponse",
    "BearerContextModificationResponse",
    "BearerContextModificationConfirm",
    "BearerContextReleaseComplete",
    "ResourceStatusResponse",
    "IAB-UPTNLAddressUpdateAcknowledge",
    "BCBearerContextSetupResponse",
    "BCBearerContextModificationResponse",
    "BCBearerContextModificationConfirm",
    "BCBearerContextReleaseComplete",
    "MCBearerContextSetupResponse",
    "MCBearerContextModificationResponse",
    "MCBearerContextModificationConfirm",
    "MCBearerContextReleaseComplete"};
const asn1::table::enum_desc e1ap_elem_procs_o::successful_outcome_c::types_opts::desc = {
    enum_names_e1ap_elem_procs_o_successful_outcome_c_types_values,
    20,
    "e1ap_elem_procs_o::successful_outcome_c::types"};

// UnsuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_e1ap_elem_procs_o_unsuccessful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_up_e1_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_cp_e1_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_up_cfg_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_cp_cfg_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bearer_context_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bearer_context_mod_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<res_status_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_up_tnl_address_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bc_bearer_context_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bc_bearer_context_mod_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_context_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mc_bearer_context_mod_fail_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_e1ap_elem_procs_o_unsuccessful_outcome_c_names[] = {
    {"GNB-CU-UP-E1SetupFailure", nullptr},
    {"GNB-CU-CP-E1SetupFailure", nullptr},
    {"GNB-CU-UP-ConfigurationUpdateFailure", nullptr},
    {"GNB-CU-CP-ConfigurationUpdateFailure", nullptr},
    {"BearerContextSetupFailure", nullptr},
    {"BearerContextModificationFailure", nullptr},
    {"ResourceStatusFailure", nullptr},
    {"IAB-UPTNLAddressUpdateFailure", nullptr},
    {"BCBearerContextSetupFailure", nullptr},
    {"BCBearerContextModificationFailure", nullptr},
    {"MCBearerContextSetupFailure", nullptr},
    {"MCBearerContextModificationFailure", nullptr}};
const asn1::table::choice_desc e1ap_elem_procs_o::unsuccessful_outcome_c::desc = {
    asn1::table::enum_int_type<e1ap_elem_procs_o::unsuccessful_outcome_c::types>(),
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nof_types,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::nof_exts,
    e1ap_elem_procs_o::unsuccessful_outcome_c::types::has_ext,
    true,
    choice_table_e1ap_elem_procs_o_unsuccessful_outcome_c_alternatives,
    &asn1::table::choice_set<e1ap_elem_procs_o::unsuccessful_outcome_c>,
    choice_table_e1ap_elem_procs_o_unsuccessful_outcome_c_names,
    "e1ap_elem_procs_o::unsuccessful_outcome_c",
    offsetof(e1ap_elem_procs_o::unsuccessful_outcome_c, type_),
    offsetof(e1ap_elem_procs_o::unsuccessful_outcome_c, c),
    false,
    nullptr,
    true};

void e1ap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, e1ap_elem_procs_o::unsuccessful_outcome_c::desc);
}
gnb_cu_up_e1_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_e1_setup_fail()
{
  assert_choice_type(types::gnb_cu_up_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_fail_s>();
}
gnb_cu_cp_e1_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_e1_setup_fail()
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_fail_s>();
}
gnb_cu_up_cfg_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_cfg_upd_fail()
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_fail_s>();
}
gnb_cu_cp_cfg_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_cfg_upd_fail()
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_fail_s>();
}
bearer_context_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_setup_fail()
{
  assert_choice_type(types::bearer_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_setup_fail_s>();
}
bearer_context_mod_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_mod_fail()
{
  assert_choice_type(types::bearer_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_mod_fail_s>();
}
res_status_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::res_status_fail()
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
iab_up_tnl_address_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::iab_up_tnl_address_upd_fail()
{
  assert_choice_type(types::iab_up_tnl_address_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_up_tnl_address_upd_fail_s>();
}
bc_bearer_context_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bc_bearer_context_setup_fail()
{
  assert_choice_type(types::bc_bearer_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<bc_bearer_context_setup_fail_s>();
}
bc_bearer_context_mod_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bc_bearer_context_mod_fail()
{
  assert_choice_type(types::bc_bearer_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<bc_bearer_context_mod_fail_s>();
}
mc_bearer_context_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::mc_bearer_context_setup_fail()
{
  assert_choice_type(types::mc_bearer_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<mc_bearer_context_setup_fail_s>();
}
mc_bearer_context_mod_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::mc_bearer_context_mod_fail()
{
  assert_choice_type(types::mc_bearer_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<mc_bearer_context_mod_fail_s>();
}
const gnb_cu_up_e1_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_e1_setup_fail() const
{
  assert_choice_type(types::gnb_cu_up_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_e1_setup_fail_s>();
}
const gnb_cu_cp_e1_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_e1_setup_fail() const
{
  assert_choice_type(types::gnb_cu_cp_e1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_e1_setup_fail_s>();
}
const gnb_cu_up_cfg_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_up_cfg_upd_fail() const
{
  assert_choice_type(types::gnb_cu_up_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_up_cfg_upd_fail_s>();
}
const gnb_cu_cp_cfg_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cp_cfg_upd_fail() const
{
  assert_choice_type(types::gnb_cu_cp_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cp_cfg_upd_fail_s>();
}
const bearer_context_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_setup_fail() const
{
  assert_choice_type(types::bearer_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_setup_fail_s>();
}
const bearer_context_mod_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bearer_context_mod_fail() const
{
  assert_choice_type(types::bearer_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<bearer_context_mod_fail_s>();
}
const res_status_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::res_status_fail() const
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
const iab_up_tnl_address_upd_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::iab_up_tnl_address_upd_fail() const
{
  assert_choice_type(types::iab_up_tnl_address_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_up_tnl_address_upd_fail_s>();
}
const bc_bearer_context_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bc_bearer_context_setup_fail() const
{
  assert_choice_type(types::bc_bearer_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<bc_bearer_context_setup_fail_s>();
}
const bc_bearer_context_mod_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::bc_bearer_context_mod_fail() const
{
  assert_choice_type(types::bc_bearer_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<bc_bearer_context_mod_fail_s>();
}
const mc_bearer_context_setup_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::mc_bearer_context_setup_fail() const
{
  assert_choice_type(types::mc_bearer_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<mc_bearer_context_setup_fail_s>();
}
const mc_bearer_context_mod_fail_s& e1ap_elem_procs_o::unsuccessful_outcome_c::mc_bearer_context_mod_fail() const
{
  assert_choice_type(types::mc_bearer_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<mc_bearer_context_mod_fail_s>();
}

static const char* enum_names_e1ap_elem_procs_o_unsuccessful_outcome_c_types_values[] = {
    "GNB-CU-UP-E1SetupFailure",
    "GNB-CU-CP-E1SetupFailure",
    "GNB-CU-UP-ConfigurationUpdateFailure",
    "GNB-CU-CP-ConfigurationUpdateFailure",
    "BearerContextSetupFailure",
    "BearerContextModificationFailure",
    "ResourceStatusFailure",
    "IAB-UPTNLAddressUpdateFailure",
    "BCBearerContextSetupFailure",
    "BCBearerContextModificationFailure",
    "MCBearerContextSetupFailure",
    "MCBearerContextModificationFailure"};
const asn1::table::enum_desc e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::desc = {
    enum_names_e1ap_elem_procs_o_unsuccessful_outcome_c_types_values,
    12,
    "e1ap_elem_procs_o::unsuccessful_outcome_c::types"};

// Columns of the object sets of E1AP-ELEMENTARY-PROCEDURE.
enum class e1ap_elem_proc_p_column : uint32_t { init_msg, successful_outcome, unsuccessful_outcome, crit };

// InitiatingMessage ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
static const asn1::table::seq_field seq_table_init_msg_s_fields[] = {
    {offsetof(init_msg_s, proc_code),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
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
                                 e1ap_elem_procs_o::desc,
                                 e1ap_elem_proc_p_column::crit,
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
                                  e1ap_elem_procs_o::desc,
                                  e1ap_elem_proc_p_column::init_msg,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_init_msg_s_names[] = {{"procedureCode", nullptr},
                                                                     {"criticality", nullptr},
                                                                     {"value", nullptr}};
const asn1::table::seq_desc          init_msg_s::desc =
    {{seq_table_init_msg_s_fields, 3, UINT64_C(0x7), nullptr, 0}, false, true, nullptr, 0, seq_table_init_msg_s_names};
bool init_msg_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(e1ap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(e1ap_elem_procs_o::desc, e1ap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, e1ap_elem_procs_o::desc, e1ap_elem_proc_p_column::init_msg, proc_code);
  return value.type().value != e1ap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
static const asn1::table::seq_field seq_table_successful_outcome_s_fields[] = {
    {offsetof(successful_outcome_s, proc_code),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
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
                                 e1ap_elem_procs_o::desc,
                                 e1ap_elem_proc_p_column::crit,
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
                                  e1ap_elem_procs_o::desc,
                                  e1ap_elem_proc_p_column::successful_outcome,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_successful_outcome_s_names[] = {{"procedureCode", nullptr},
                                                                               {"criticality", nullptr},
                                                                               {"value", nullptr}};
const asn1::table::seq_desc          successful_outcome_s::desc             = {
    {seq_table_successful_outcome_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    true,
    nullptr,
    0,
    seq_table_successful_outcome_s_names};
bool successful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(e1ap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(e1ap_elem_procs_o::desc, e1ap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, e1ap_elem_procs_o::desc, e1ap_elem_proc_p_column::successful_outcome, proc_code);
  return value.type().value != e1ap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
static const asn1::table::seq_field seq_table_unsuccessful_outcome_s_fields[] = {
    {offsetof(unsuccessful_outcome_s, proc_code),
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::int_u16,
     false,
     true,
     INT64_C(0),
     UINT64_C(255),
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
                                 e1ap_elem_procs_o::desc,
                                 e1ap_elem_proc_p_column::crit,
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
                                  e1ap_elem_procs_o::desc,
                                  e1ap_elem_proc_p_column::unsuccessful_outcome,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_unsuccessful_outcome_s_names[] = {{"procedureCode", nullptr},
                                                                                 {"criticality", nullptr},
                                                                                 {"value", nullptr}};
const asn1::table::seq_desc          unsuccessful_outcome_s::desc             = {
    {seq_table_unsuccessful_outcome_s_fields, 3, UINT64_C(0x7), nullptr, 0},
    false,
    true,
    nullptr,
    0,
    seq_table_unsuccessful_outcome_s_names};
bool unsuccessful_outcome_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(e1ap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(e1ap_elem_procs_o::desc, e1ap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, e1ap_elem_procs_o::desc, e1ap_elem_proc_p_column::unsuccessful_outcome, proc_code);
  return value.type().value != e1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// E1AP-PDU ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_e1ap_pdu_c_alternatives[] = {
    &asn1::table::alternative_ops<init_msg_s, false>::value,
    &asn1::table::alternative_ops<successful_outcome_s, false>::value,
    &asn1::table::alternative_ops<unsuccessful_outcome_s, false>::value};
const asn1::table::choice_storage_desc e1ap_pdu_c::storage_desc = {choice_storage_e1ap_pdu_c_alternatives,
                                                                   3,
                                                                   "e1ap_pdu_c"};

init_msg_s& e1ap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& e1ap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& e1ap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
static const asn1::table::seq_field  choice_table_e1ap_pdu_c_alternatives[] = {{0,
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
static const asn1::table::field_name choice_table_e1ap_pdu_c_names[]        = {{"initiatingMessage", nullptr},
                                                                               {"successfulOutcome", nullptr},
                                                                               {"unsuccessfulOutcome", nullptr}};
const asn1::table::choice_desc       e1ap_pdu_c::desc = {asn1::table::enum_int_type<e1ap_pdu_c::types>(),
                                                         e1ap_pdu_c::types::nof_types,
                                                         e1ap_pdu_c::types::nof_exts,
                                                         e1ap_pdu_c::types::has_ext,
                                                         true,
                                                         choice_table_e1ap_pdu_c_alternatives,
                                                         nullptr,
                                                         choice_table_e1ap_pdu_c_names,
                                                         "e1ap_pdu_c",
                                                         offsetof(e1ap_pdu_c, c.type),
                                                         offsetof(e1ap_pdu_c, c.value),
                                                         false,
                                                         &e1ap_pdu_c::storage_desc};

static const char*           enum_names_e1ap_pdu_c_types_values[] = {"initiatingMessage",
                                                                     "successfulOutcome",
                                                                     "unsuccessfulOutcome"};
const asn1::table::enum_desc e1ap_pdu_c::types_opts::desc         = {enum_names_e1ap_pdu_c_types_values,
                                                                     3,
                                                                     "e1ap_pdu_c::types"};
