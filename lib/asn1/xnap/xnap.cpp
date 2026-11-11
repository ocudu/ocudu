// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/xnap/xnap.h"
#include "ocudu/asn1/asn1_ap_codec.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"

using namespace asn1;
using namespace asn1::xnap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// XNAP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF XNAP-ELEMENTARY-PROCEDURE
static const uint32_t objset_proc_code_xnap_elem_procs_o[] = {
    0, 3,  7,  9,  10, 11, 12, 14, 16, 17, 18, 25, 19, 20, 34, 36, 44, 45, 46, 49, 51, 1,  2,  4,  5,  6,
    8, 13, 15, 21, 22, 23, 24, 26, 27, 28, 29, 30, 31, 32, 33, 35, 37, 38, 39, 40, 42, 43, 47, 48, 50, 52};
static const uint8_t objset_init_msg_xnap_elem_procs_o[] = {
    xnap_elem_procs_o::init_msg_c::types::ho_request,
    xnap_elem_procs_o::init_msg_c::types::retrieve_ue_context_request,
    xnap_elem_procs_o::init_msg_c::types::sn_ode_addition_request,
    xnap_elem_procs_o::init_msg_c::types::sn_ode_mod_request,
    xnap_elem_procs_o::init_msg_c::types::sn_ode_mod_required,
    xnap_elem_procs_o::init_msg_c::types::sn_ode_release_request,
    xnap_elem_procs_o::init_msg_c::types::sn_ode_release_required,
    xnap_elem_procs_o::init_msg_c::types::sn_ode_change_required,
    xnap_elem_procs_o::init_msg_c::types::xn_removal_request,
    xnap_elem_procs_o::init_msg_c::types::xn_setup_request,
    xnap_elem_procs_o::init_msg_c::types::ngran_node_cfg_upd,
    xnap_elem_procs_o::init_msg_c::types::e_utra_nr_cell_res_coordination_request,
    xnap_elem_procs_o::init_msg_c::types::cell_activation_request,
    xnap_elem_procs_o::init_msg_c::types::reset_request,
    xnap_elem_procs_o::init_msg_c::types::res_status_request,
    xnap_elem_procs_o::init_msg_c::types::mob_change_request,
    xnap_elem_procs_o::init_msg_c::types::iab_transport_migration_management_request,
    xnap_elem_procs_o::init_msg_c::types::iab_transport_migration_mod_request,
    xnap_elem_procs_o::init_msg_c::types::iab_res_coordination_request,
    xnap_elem_procs_o::init_msg_c::types::partial_ue_context_transfer,
    xnap_elem_procs_o::init_msg_c::types::data_collection_request,
    xnap_elem_procs_o::init_msg_c::types::sn_status_transfer,
    xnap_elem_procs_o::init_msg_c::types::ho_cancel,
    xnap_elem_procs_o::init_msg_c::types::ran_paging,
    xnap_elem_procs_o::init_msg_c::types::xn_u_address_ind,
    xnap_elem_procs_o::init_msg_c::types::ue_context_release,
    xnap_elem_procs_o::init_msg_c::types::sn_ode_recfg_complete,
    xnap_elem_procs_o::init_msg_c::types::sn_ode_counter_check_request,
    xnap_elem_procs_o::init_msg_c::types::rrc_transfer,
    xnap_elem_procs_o::init_msg_c::types::error_ind,
    xnap_elem_procs_o::init_msg_c::types::private_msg,
    xnap_elem_procs_o::init_msg_c::types::notif_ctrl_ind,
    xnap_elem_procs_o::init_msg_c::types::activity_notif,
    xnap_elem_procs_o::init_msg_c::types::secondary_rat_data_usage_report,
    xnap_elem_procs_o::init_msg_c::types::deactiv_trace,
    xnap_elem_procs_o::init_msg_c::types::trace_start,
    xnap_elem_procs_o::init_msg_c::types::ho_success,
    xnap_elem_procs_o::init_msg_c::types::conditional_ho_cancel,
    xnap_elem_procs_o::init_msg_c::types::early_status_transfer,
    xnap_elem_procs_o::init_msg_c::types::fail_ind,
    xnap_elem_procs_o::init_msg_c::types::ho_report,
    xnap_elem_procs_o::init_msg_c::types::res_status_upd,
    xnap_elem_procs_o::init_msg_c::types::access_and_mob_ind,
    xnap_elem_procs_o::init_msg_c::types::cell_traffic_trace,
    xnap_elem_procs_o::init_msg_c::types::ran_multicast_group_paging,
    xnap_elem_procs_o::init_msg_c::types::scg_fail_info_report,
    xnap_elem_procs_o::init_msg_c::types::scg_fail_transfer,
    xnap_elem_procs_o::init_msg_c::types::f1_c_traffic_transfer,
    xnap_elem_procs_o::init_msg_c::types::retrieve_ue_context_confirm,
    xnap_elem_procs_o::init_msg_c::types::cp_c_cancel,
    xnap_elem_procs_o::init_msg_c::types::rach_ind,
    xnap_elem_procs_o::init_msg_c::types::data_collection_upd};
static const uint8_t objset_successful_outcome_xnap_elem_procs_o[] = {
    xnap_elem_procs_o::successful_outcome_c::types::ho_request_ack,
    xnap_elem_procs_o::successful_outcome_c::types::retrieve_ue_context_resp,
    xnap_elem_procs_o::successful_outcome_c::types::sn_ode_addition_request_ack,
    xnap_elem_procs_o::successful_outcome_c::types::sn_ode_mod_request_ack,
    xnap_elem_procs_o::successful_outcome_c::types::sn_ode_mod_confirm,
    xnap_elem_procs_o::successful_outcome_c::types::sn_ode_release_request_ack,
    xnap_elem_procs_o::successful_outcome_c::types::sn_ode_release_confirm,
    xnap_elem_procs_o::successful_outcome_c::types::sn_ode_change_confirm,
    xnap_elem_procs_o::successful_outcome_c::types::xn_removal_resp,
    xnap_elem_procs_o::successful_outcome_c::types::xn_setup_resp,
    xnap_elem_procs_o::successful_outcome_c::types::ngran_node_cfg_upd_ack,
    xnap_elem_procs_o::successful_outcome_c::types::e_utra_nr_cell_res_coordination_resp,
    xnap_elem_procs_o::successful_outcome_c::types::cell_activation_resp,
    xnap_elem_procs_o::successful_outcome_c::types::reset_resp,
    xnap_elem_procs_o::successful_outcome_c::types::res_status_resp,
    xnap_elem_procs_o::successful_outcome_c::types::mob_change_ack,
    xnap_elem_procs_o::successful_outcome_c::types::iab_transport_migration_management_resp,
    xnap_elem_procs_o::successful_outcome_c::types::iab_transport_migration_mod_resp,
    xnap_elem_procs_o::successful_outcome_c::types::iab_res_coordination_resp,
    xnap_elem_procs_o::successful_outcome_c::types::partial_ue_context_transfer_ack,
    xnap_elem_procs_o::successful_outcome_c::types::data_collection_resp,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_o::successful_outcome_c::types::nulltype};
static const uint8_t objset_unsuccessful_outcome_xnap_elem_procs_o[] = {
    xnap_elem_procs_o::unsuccessful_outcome_c::types::ho_prep_fail,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::retrieve_ue_context_fail,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::sn_ode_addition_request_reject,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::sn_ode_mod_request_reject,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::sn_ode_mod_refuse,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::sn_ode_release_reject,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::sn_ode_change_refuse,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::xn_removal_fail,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::xn_setup_fail,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::ngran_node_cfg_upd_fail,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::cell_activation_fail,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::res_status_fail,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::mob_change_fail,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::iab_transport_migration_management_reject,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::partial_ue_context_transfer_fail,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::data_collection_fail,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype};
static const uint8_t objset_crit_xnap_elem_procs_o[] = {
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::reject,
    crit_e::ignore, crit_e::ignore, crit_e::ignore};
static const asn1::table::objset_column objset_columns_xnap_elem_procs_o[] = {
    {objset_init_msg_xnap_elem_procs_o, xnap_elem_procs_o::init_msg_c::types::nulltype},
    {objset_successful_outcome_xnap_elem_procs_o, xnap_elem_procs_o::successful_outcome_c::types::nulltype},
    {objset_unsuccessful_outcome_xnap_elem_procs_o, xnap_elem_procs_o::unsuccessful_outcome_c::types::nulltype},
    {objset_crit_xnap_elem_procs_o, crit_e::nulltype}};
const asn1::table::objset_desc xnap_elem_procs_o::desc = {objset_proc_code_xnap_elem_procs_o,
                                                          52,
                                                          "proc_code",
                                                          objset_columns_xnap_elem_procs_o};

// InitiatingMessage ::= OPEN TYPE
static const asn1::table::seq_field choice_table_xnap_elem_procs_o_init_msg_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<retrieve_ue_context_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_addition_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_release_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_change_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_removal_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ngran_node_cfg_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e_utra_nr_cell_res_coordination_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cell_activation_request_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<res_status_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mob_change_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_transport_migration_management_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_transport_migration_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_res_coordination_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<partial_ue_context_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<data_collection_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_status_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_cancel_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ran_paging_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_u_address_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_release_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_recfg_complete_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_counter_check_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<rrc_transfer_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<notif_ctrl_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<activity_notif_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<secondary_rat_data_usage_report_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ho_success_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<conditional_ho_cancel_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<early_status_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<fail_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_report_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<access_and_mob_ind_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ran_multicast_group_paging_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<scg_fail_info_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<scg_fail_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<f1_c_traffic_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<retrieve_ue_context_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cp_c_cancel_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<rach_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<data_collection_upd_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_xnap_elem_procs_o_init_msg_c_names[] = {
    {"HandoverRequest", nullptr},
    {"RetrieveUEContextRequest", nullptr},
    {"SNodeAdditionRequest", nullptr},
    {"SNodeModificationRequest", nullptr},
    {"SNodeModificationRequired", nullptr},
    {"SNodeReleaseRequest", nullptr},
    {"SNodeReleaseRequired", nullptr},
    {"SNodeChangeRequired", nullptr},
    {"XnRemovalRequest", nullptr},
    {"XnSetupRequest", nullptr},
    {"NGRANNodeConfigurationUpdate", nullptr},
    {"E-UTRA-NR-CellResourceCoordinationRequest", nullptr},
    {"CellActivationRequest", nullptr},
    {"ResetRequest", nullptr},
    {"ResourceStatusRequest", nullptr},
    {"MobilityChangeRequest", nullptr},
    {"IABTransportMigrationManagementRequest", nullptr},
    {"IABTransportMigrationModificationRequest", nullptr},
    {"IABResourceCoordinationRequest", nullptr},
    {"PartialUEContextTransfer", nullptr},
    {"DataCollectionRequest", nullptr},
    {"SNStatusTransfer", nullptr},
    {"HandoverCancel", nullptr},
    {"RANPaging", nullptr},
    {"XnUAddressIndication", nullptr},
    {"UEContextRelease", nullptr},
    {"SNodeReconfigurationComplete", nullptr},
    {"SNodeCounterCheckRequest", nullptr},
    {"RRCTransfer", nullptr},
    {"ErrorIndication", nullptr},
    {"PrivateMessage", nullptr},
    {"NotificationControlIndication", nullptr},
    {"ActivityNotification", nullptr},
    {"SecondaryRATDataUsageReport", nullptr},
    {"DeactivateTrace", nullptr},
    {"TraceStart", nullptr},
    {"HandoverSuccess", nullptr},
    {"ConditionalHandoverCancel", nullptr},
    {"EarlyStatusTransfer", nullptr},
    {"FailureIndication", nullptr},
    {"HandoverReport", nullptr},
    {"ResourceStatusUpdate", nullptr},
    {"AccessAndMobilityIndication", nullptr},
    {"CellTrafficTrace", nullptr},
    {"RANMulticastGroupPaging", nullptr},
    {"ScgFailureInformationReport", nullptr},
    {"ScgFailureTransfer", nullptr},
    {"F1CTrafficTransfer", nullptr},
    {"RetrieveUEContextConfirm", nullptr},
    {"CPCCancel", nullptr},
    {"RachIndication", nullptr},
    {"DataCollectionUpdate", nullptr}};
const asn1::table::choice_desc xnap_elem_procs_o::init_msg_c::desc = {
    asn1::table::enum_int_type<xnap_elem_procs_o::init_msg_c::types>(),
    xnap_elem_procs_o::init_msg_c::types::nof_types,
    xnap_elem_procs_o::init_msg_c::types::nof_exts,
    xnap_elem_procs_o::init_msg_c::types::has_ext,
    true,
    choice_table_xnap_elem_procs_o_init_msg_c_alternatives,
    &asn1::table::choice_set<xnap_elem_procs_o::init_msg_c>,
    choice_table_xnap_elem_procs_o_init_msg_c_names,
    "xnap_elem_procs_o::init_msg_c",
    offsetof(xnap_elem_procs_o::init_msg_c, type_),
    offsetof(xnap_elem_procs_o::init_msg_c, c),
    false,
    nullptr,
    true};

void xnap_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, xnap_elem_procs_o::init_msg_c::desc);
}
ho_request_s& xnap_elem_procs_o::init_msg_c::ho_request()
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
retrieve_ue_context_request_s& xnap_elem_procs_o::init_msg_c::retrieve_ue_context_request()
{
  assert_choice_type(types::retrieve_ue_context_request, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_request_s>();
}
sn_ode_addition_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_addition_request()
{
  assert_choice_type(types::sn_ode_addition_request, type_, "InitiatingMessage");
  return c.get<sn_ode_addition_request_s>();
}
sn_ode_mod_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_mod_request()
{
  assert_choice_type(types::sn_ode_mod_request, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_request_s>();
}
sn_ode_mod_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_mod_required()
{
  assert_choice_type(types::sn_ode_mod_required, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_required_s>();
}
sn_ode_release_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_release_request()
{
  assert_choice_type(types::sn_ode_release_request, type_, "InitiatingMessage");
  return c.get<sn_ode_release_request_s>();
}
sn_ode_release_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_release_required()
{
  assert_choice_type(types::sn_ode_release_required, type_, "InitiatingMessage");
  return c.get<sn_ode_release_required_s>();
}
sn_ode_change_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_change_required()
{
  assert_choice_type(types::sn_ode_change_required, type_, "InitiatingMessage");
  return c.get<sn_ode_change_required_s>();
}
xn_removal_request_s& xnap_elem_procs_o::init_msg_c::xn_removal_request()
{
  assert_choice_type(types::xn_removal_request, type_, "InitiatingMessage");
  return c.get<xn_removal_request_s>();
}
xn_setup_request_s& xnap_elem_procs_o::init_msg_c::xn_setup_request()
{
  assert_choice_type(types::xn_setup_request, type_, "InitiatingMessage");
  return c.get<xn_setup_request_s>();
}
ngran_node_cfg_upd_s& xnap_elem_procs_o::init_msg_c::ngran_node_cfg_upd()
{
  assert_choice_type(types::ngran_node_cfg_upd, type_, "InitiatingMessage");
  return c.get<ngran_node_cfg_upd_s>();
}
e_utra_nr_cell_res_coordination_request_s& xnap_elem_procs_o::init_msg_c::e_utra_nr_cell_res_coordination_request()
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_request, type_, "InitiatingMessage");
  return c.get<e_utra_nr_cell_res_coordination_request_s>();
}
cell_activation_request_s& xnap_elem_procs_o::init_msg_c::cell_activation_request()
{
  assert_choice_type(types::cell_activation_request, type_, "InitiatingMessage");
  return c.get<cell_activation_request_s>();
}
reset_request_s& xnap_elem_procs_o::init_msg_c::reset_request()
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
res_status_request_s& xnap_elem_procs_o::init_msg_c::res_status_request()
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
mob_change_request_s& xnap_elem_procs_o::init_msg_c::mob_change_request()
{
  assert_choice_type(types::mob_change_request, type_, "InitiatingMessage");
  return c.get<mob_change_request_s>();
}
iab_transport_migration_management_request_s&
xnap_elem_procs_o::init_msg_c::iab_transport_migration_management_request()
{
  assert_choice_type(types::iab_transport_migration_management_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_management_request_s>();
}
iab_transport_migration_mod_request_s& xnap_elem_procs_o::init_msg_c::iab_transport_migration_mod_request()
{
  assert_choice_type(types::iab_transport_migration_mod_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_mod_request_s>();
}
iab_res_coordination_request_s& xnap_elem_procs_o::init_msg_c::iab_res_coordination_request()
{
  assert_choice_type(types::iab_res_coordination_request, type_, "InitiatingMessage");
  return c.get<iab_res_coordination_request_s>();
}
partial_ue_context_transfer_s& xnap_elem_procs_o::init_msg_c::partial_ue_context_transfer()
{
  assert_choice_type(types::partial_ue_context_transfer, type_, "InitiatingMessage");
  return c.get<partial_ue_context_transfer_s>();
}
data_collection_request_s& xnap_elem_procs_o::init_msg_c::data_collection_request()
{
  assert_choice_type(types::data_collection_request, type_, "InitiatingMessage");
  return c.get<data_collection_request_s>();
}
sn_status_transfer_s& xnap_elem_procs_o::init_msg_c::sn_status_transfer()
{
  assert_choice_type(types::sn_status_transfer, type_, "InitiatingMessage");
  return c.get<sn_status_transfer_s>();
}
ho_cancel_s& xnap_elem_procs_o::init_msg_c::ho_cancel()
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
ran_paging_s& xnap_elem_procs_o::init_msg_c::ran_paging()
{
  assert_choice_type(types::ran_paging, type_, "InitiatingMessage");
  return c.get<ran_paging_s>();
}
xn_u_address_ind_s& xnap_elem_procs_o::init_msg_c::xn_u_address_ind()
{
  assert_choice_type(types::xn_u_address_ind, type_, "InitiatingMessage");
  return c.get<xn_u_address_ind_s>();
}
ue_context_release_s& xnap_elem_procs_o::init_msg_c::ue_context_release()
{
  assert_choice_type(types::ue_context_release, type_, "InitiatingMessage");
  return c.get<ue_context_release_s>();
}
sn_ode_recfg_complete_s& xnap_elem_procs_o::init_msg_c::sn_ode_recfg_complete()
{
  assert_choice_type(types::sn_ode_recfg_complete, type_, "InitiatingMessage");
  return c.get<sn_ode_recfg_complete_s>();
}
sn_ode_counter_check_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_counter_check_request()
{
  assert_choice_type(types::sn_ode_counter_check_request, type_, "InitiatingMessage");
  return c.get<sn_ode_counter_check_request_s>();
}
rrc_transfer_s& xnap_elem_procs_o::init_msg_c::rrc_transfer()
{
  assert_choice_type(types::rrc_transfer, type_, "InitiatingMessage");
  return c.get<rrc_transfer_s>();
}
error_ind_s& xnap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
private_msg_s& xnap_elem_procs_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
notif_ctrl_ind_s& xnap_elem_procs_o::init_msg_c::notif_ctrl_ind()
{
  assert_choice_type(types::notif_ctrl_ind, type_, "InitiatingMessage");
  return c.get<notif_ctrl_ind_s>();
}
activity_notif_s& xnap_elem_procs_o::init_msg_c::activity_notif()
{
  assert_choice_type(types::activity_notif, type_, "InitiatingMessage");
  return c.get<activity_notif_s>();
}
secondary_rat_data_usage_report_s& xnap_elem_procs_o::init_msg_c::secondary_rat_data_usage_report()
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
deactiv_trace_s& xnap_elem_procs_o::init_msg_c::deactiv_trace()
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
trace_start_s& xnap_elem_procs_o::init_msg_c::trace_start()
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
ho_success_s& xnap_elem_procs_o::init_msg_c::ho_success()
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
conditional_ho_cancel_s& xnap_elem_procs_o::init_msg_c::conditional_ho_cancel()
{
  assert_choice_type(types::conditional_ho_cancel, type_, "InitiatingMessage");
  return c.get<conditional_ho_cancel_s>();
}
early_status_transfer_s& xnap_elem_procs_o::init_msg_c::early_status_transfer()
{
  assert_choice_type(types::early_status_transfer, type_, "InitiatingMessage");
  return c.get<early_status_transfer_s>();
}
fail_ind_s& xnap_elem_procs_o::init_msg_c::fail_ind()
{
  assert_choice_type(types::fail_ind, type_, "InitiatingMessage");
  return c.get<fail_ind_s>();
}
ho_report_s& xnap_elem_procs_o::init_msg_c::ho_report()
{
  assert_choice_type(types::ho_report, type_, "InitiatingMessage");
  return c.get<ho_report_s>();
}
res_status_upd_s& xnap_elem_procs_o::init_msg_c::res_status_upd()
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
access_and_mob_ind_s& xnap_elem_procs_o::init_msg_c::access_and_mob_ind()
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
cell_traffic_trace_s& xnap_elem_procs_o::init_msg_c::cell_traffic_trace()
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
ran_multicast_group_paging_s& xnap_elem_procs_o::init_msg_c::ran_multicast_group_paging()
{
  assert_choice_type(types::ran_multicast_group_paging, type_, "InitiatingMessage");
  return c.get<ran_multicast_group_paging_s>();
}
scg_fail_info_report_s& xnap_elem_procs_o::init_msg_c::scg_fail_info_report()
{
  assert_choice_type(types::scg_fail_info_report, type_, "InitiatingMessage");
  return c.get<scg_fail_info_report_s>();
}
scg_fail_transfer_s& xnap_elem_procs_o::init_msg_c::scg_fail_transfer()
{
  assert_choice_type(types::scg_fail_transfer, type_, "InitiatingMessage");
  return c.get<scg_fail_transfer_s>();
}
f1_c_traffic_transfer_s& xnap_elem_procs_o::init_msg_c::f1_c_traffic_transfer()
{
  assert_choice_type(types::f1_c_traffic_transfer, type_, "InitiatingMessage");
  return c.get<f1_c_traffic_transfer_s>();
}
retrieve_ue_context_confirm_s& xnap_elem_procs_o::init_msg_c::retrieve_ue_context_confirm()
{
  assert_choice_type(types::retrieve_ue_context_confirm, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_confirm_s>();
}
cp_c_cancel_s& xnap_elem_procs_o::init_msg_c::cp_c_cancel()
{
  assert_choice_type(types::cp_c_cancel, type_, "InitiatingMessage");
  return c.get<cp_c_cancel_s>();
}
rach_ind_s& xnap_elem_procs_o::init_msg_c::rach_ind()
{
  assert_choice_type(types::rach_ind, type_, "InitiatingMessage");
  return c.get<rach_ind_s>();
}
data_collection_upd_s& xnap_elem_procs_o::init_msg_c::data_collection_upd()
{
  assert_choice_type(types::data_collection_upd, type_, "InitiatingMessage");
  return c.get<data_collection_upd_s>();
}
const ho_request_s& xnap_elem_procs_o::init_msg_c::ho_request() const
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
const retrieve_ue_context_request_s& xnap_elem_procs_o::init_msg_c::retrieve_ue_context_request() const
{
  assert_choice_type(types::retrieve_ue_context_request, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_request_s>();
}
const sn_ode_addition_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_addition_request() const
{
  assert_choice_type(types::sn_ode_addition_request, type_, "InitiatingMessage");
  return c.get<sn_ode_addition_request_s>();
}
const sn_ode_mod_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_mod_request() const
{
  assert_choice_type(types::sn_ode_mod_request, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_request_s>();
}
const sn_ode_mod_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_mod_required() const
{
  assert_choice_type(types::sn_ode_mod_required, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_required_s>();
}
const sn_ode_release_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_release_request() const
{
  assert_choice_type(types::sn_ode_release_request, type_, "InitiatingMessage");
  return c.get<sn_ode_release_request_s>();
}
const sn_ode_release_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_release_required() const
{
  assert_choice_type(types::sn_ode_release_required, type_, "InitiatingMessage");
  return c.get<sn_ode_release_required_s>();
}
const sn_ode_change_required_s& xnap_elem_procs_o::init_msg_c::sn_ode_change_required() const
{
  assert_choice_type(types::sn_ode_change_required, type_, "InitiatingMessage");
  return c.get<sn_ode_change_required_s>();
}
const xn_removal_request_s& xnap_elem_procs_o::init_msg_c::xn_removal_request() const
{
  assert_choice_type(types::xn_removal_request, type_, "InitiatingMessage");
  return c.get<xn_removal_request_s>();
}
const xn_setup_request_s& xnap_elem_procs_o::init_msg_c::xn_setup_request() const
{
  assert_choice_type(types::xn_setup_request, type_, "InitiatingMessage");
  return c.get<xn_setup_request_s>();
}
const ngran_node_cfg_upd_s& xnap_elem_procs_o::init_msg_c::ngran_node_cfg_upd() const
{
  assert_choice_type(types::ngran_node_cfg_upd, type_, "InitiatingMessage");
  return c.get<ngran_node_cfg_upd_s>();
}
const e_utra_nr_cell_res_coordination_request_s&
xnap_elem_procs_o::init_msg_c::e_utra_nr_cell_res_coordination_request() const
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_request, type_, "InitiatingMessage");
  return c.get<e_utra_nr_cell_res_coordination_request_s>();
}
const cell_activation_request_s& xnap_elem_procs_o::init_msg_c::cell_activation_request() const
{
  assert_choice_type(types::cell_activation_request, type_, "InitiatingMessage");
  return c.get<cell_activation_request_s>();
}
const reset_request_s& xnap_elem_procs_o::init_msg_c::reset_request() const
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
const res_status_request_s& xnap_elem_procs_o::init_msg_c::res_status_request() const
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
const mob_change_request_s& xnap_elem_procs_o::init_msg_c::mob_change_request() const
{
  assert_choice_type(types::mob_change_request, type_, "InitiatingMessage");
  return c.get<mob_change_request_s>();
}
const iab_transport_migration_management_request_s&
xnap_elem_procs_o::init_msg_c::iab_transport_migration_management_request() const
{
  assert_choice_type(types::iab_transport_migration_management_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_management_request_s>();
}
const iab_transport_migration_mod_request_s& xnap_elem_procs_o::init_msg_c::iab_transport_migration_mod_request() const
{
  assert_choice_type(types::iab_transport_migration_mod_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_mod_request_s>();
}
const iab_res_coordination_request_s& xnap_elem_procs_o::init_msg_c::iab_res_coordination_request() const
{
  assert_choice_type(types::iab_res_coordination_request, type_, "InitiatingMessage");
  return c.get<iab_res_coordination_request_s>();
}
const partial_ue_context_transfer_s& xnap_elem_procs_o::init_msg_c::partial_ue_context_transfer() const
{
  assert_choice_type(types::partial_ue_context_transfer, type_, "InitiatingMessage");
  return c.get<partial_ue_context_transfer_s>();
}
const data_collection_request_s& xnap_elem_procs_o::init_msg_c::data_collection_request() const
{
  assert_choice_type(types::data_collection_request, type_, "InitiatingMessage");
  return c.get<data_collection_request_s>();
}
const sn_status_transfer_s& xnap_elem_procs_o::init_msg_c::sn_status_transfer() const
{
  assert_choice_type(types::sn_status_transfer, type_, "InitiatingMessage");
  return c.get<sn_status_transfer_s>();
}
const ho_cancel_s& xnap_elem_procs_o::init_msg_c::ho_cancel() const
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
const ran_paging_s& xnap_elem_procs_o::init_msg_c::ran_paging() const
{
  assert_choice_type(types::ran_paging, type_, "InitiatingMessage");
  return c.get<ran_paging_s>();
}
const xn_u_address_ind_s& xnap_elem_procs_o::init_msg_c::xn_u_address_ind() const
{
  assert_choice_type(types::xn_u_address_ind, type_, "InitiatingMessage");
  return c.get<xn_u_address_ind_s>();
}
const ue_context_release_s& xnap_elem_procs_o::init_msg_c::ue_context_release() const
{
  assert_choice_type(types::ue_context_release, type_, "InitiatingMessage");
  return c.get<ue_context_release_s>();
}
const sn_ode_recfg_complete_s& xnap_elem_procs_o::init_msg_c::sn_ode_recfg_complete() const
{
  assert_choice_type(types::sn_ode_recfg_complete, type_, "InitiatingMessage");
  return c.get<sn_ode_recfg_complete_s>();
}
const sn_ode_counter_check_request_s& xnap_elem_procs_o::init_msg_c::sn_ode_counter_check_request() const
{
  assert_choice_type(types::sn_ode_counter_check_request, type_, "InitiatingMessage");
  return c.get<sn_ode_counter_check_request_s>();
}
const rrc_transfer_s& xnap_elem_procs_o::init_msg_c::rrc_transfer() const
{
  assert_choice_type(types::rrc_transfer, type_, "InitiatingMessage");
  return c.get<rrc_transfer_s>();
}
const error_ind_s& xnap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const private_msg_s& xnap_elem_procs_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const notif_ctrl_ind_s& xnap_elem_procs_o::init_msg_c::notif_ctrl_ind() const
{
  assert_choice_type(types::notif_ctrl_ind, type_, "InitiatingMessage");
  return c.get<notif_ctrl_ind_s>();
}
const activity_notif_s& xnap_elem_procs_o::init_msg_c::activity_notif() const
{
  assert_choice_type(types::activity_notif, type_, "InitiatingMessage");
  return c.get<activity_notif_s>();
}
const secondary_rat_data_usage_report_s& xnap_elem_procs_o::init_msg_c::secondary_rat_data_usage_report() const
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
const deactiv_trace_s& xnap_elem_procs_o::init_msg_c::deactiv_trace() const
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
const trace_start_s& xnap_elem_procs_o::init_msg_c::trace_start() const
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
const ho_success_s& xnap_elem_procs_o::init_msg_c::ho_success() const
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
const conditional_ho_cancel_s& xnap_elem_procs_o::init_msg_c::conditional_ho_cancel() const
{
  assert_choice_type(types::conditional_ho_cancel, type_, "InitiatingMessage");
  return c.get<conditional_ho_cancel_s>();
}
const early_status_transfer_s& xnap_elem_procs_o::init_msg_c::early_status_transfer() const
{
  assert_choice_type(types::early_status_transfer, type_, "InitiatingMessage");
  return c.get<early_status_transfer_s>();
}
const fail_ind_s& xnap_elem_procs_o::init_msg_c::fail_ind() const
{
  assert_choice_type(types::fail_ind, type_, "InitiatingMessage");
  return c.get<fail_ind_s>();
}
const ho_report_s& xnap_elem_procs_o::init_msg_c::ho_report() const
{
  assert_choice_type(types::ho_report, type_, "InitiatingMessage");
  return c.get<ho_report_s>();
}
const res_status_upd_s& xnap_elem_procs_o::init_msg_c::res_status_upd() const
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
const access_and_mob_ind_s& xnap_elem_procs_o::init_msg_c::access_and_mob_ind() const
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
const cell_traffic_trace_s& xnap_elem_procs_o::init_msg_c::cell_traffic_trace() const
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
const ran_multicast_group_paging_s& xnap_elem_procs_o::init_msg_c::ran_multicast_group_paging() const
{
  assert_choice_type(types::ran_multicast_group_paging, type_, "InitiatingMessage");
  return c.get<ran_multicast_group_paging_s>();
}
const scg_fail_info_report_s& xnap_elem_procs_o::init_msg_c::scg_fail_info_report() const
{
  assert_choice_type(types::scg_fail_info_report, type_, "InitiatingMessage");
  return c.get<scg_fail_info_report_s>();
}
const scg_fail_transfer_s& xnap_elem_procs_o::init_msg_c::scg_fail_transfer() const
{
  assert_choice_type(types::scg_fail_transfer, type_, "InitiatingMessage");
  return c.get<scg_fail_transfer_s>();
}
const f1_c_traffic_transfer_s& xnap_elem_procs_o::init_msg_c::f1_c_traffic_transfer() const
{
  assert_choice_type(types::f1_c_traffic_transfer, type_, "InitiatingMessage");
  return c.get<f1_c_traffic_transfer_s>();
}
const retrieve_ue_context_confirm_s& xnap_elem_procs_o::init_msg_c::retrieve_ue_context_confirm() const
{
  assert_choice_type(types::retrieve_ue_context_confirm, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_confirm_s>();
}
const cp_c_cancel_s& xnap_elem_procs_o::init_msg_c::cp_c_cancel() const
{
  assert_choice_type(types::cp_c_cancel, type_, "InitiatingMessage");
  return c.get<cp_c_cancel_s>();
}
const rach_ind_s& xnap_elem_procs_o::init_msg_c::rach_ind() const
{
  assert_choice_type(types::rach_ind, type_, "InitiatingMessage");
  return c.get<rach_ind_s>();
}
const data_collection_upd_s& xnap_elem_procs_o::init_msg_c::data_collection_upd() const
{
  assert_choice_type(types::data_collection_upd, type_, "InitiatingMessage");
  return c.get<data_collection_upd_s>();
}

static const char* enum_names_xnap_elem_procs_o_init_msg_c_types_values[] = {
    "HandoverRequest",
    "RetrieveUEContextRequest",
    "SNodeAdditionRequest",
    "SNodeModificationRequest",
    "SNodeModificationRequired",
    "SNodeReleaseRequest",
    "SNodeReleaseRequired",
    "SNodeChangeRequired",
    "XnRemovalRequest",
    "XnSetupRequest",
    "NGRANNodeConfigurationUpdate",
    "E-UTRA-NR-CellResourceCoordinationRequest",
    "CellActivationRequest",
    "ResetRequest",
    "ResourceStatusRequest",
    "MobilityChangeRequest",
    "IABTransportMigrationManagementRequest",
    "IABTransportMigrationModificationRequest",
    "IABResourceCoordinationRequest",
    "PartialUEContextTransfer",
    "DataCollectionRequest",
    "SNStatusTransfer",
    "HandoverCancel",
    "RANPaging",
    "XnUAddressIndication",
    "UEContextRelease",
    "SNodeReconfigurationComplete",
    "SNodeCounterCheckRequest",
    "RRCTransfer",
    "ErrorIndication",
    "PrivateMessage",
    "NotificationControlIndication",
    "ActivityNotification",
    "SecondaryRATDataUsageReport",
    "DeactivateTrace",
    "TraceStart",
    "HandoverSuccess",
    "ConditionalHandoverCancel",
    "EarlyStatusTransfer",
    "FailureIndication",
    "HandoverReport",
    "ResourceStatusUpdate",
    "AccessAndMobilityIndication",
    "CellTrafficTrace",
    "RANMulticastGroupPaging",
    "ScgFailureInformationReport",
    "ScgFailureTransfer",
    "F1CTrafficTransfer",
    "RetrieveUEContextConfirm",
    "CPCCancel",
    "RachIndication",
    "DataCollectionUpdate"};
const asn1::table::enum_desc xnap_elem_procs_o::init_msg_c::types_opts::desc = {
    enum_names_xnap_elem_procs_o_init_msg_c_types_values,
    52,
    "xnap_elem_procs_o::init_msg_c::types"};
uint8_t xnap_elem_procs_o::init_msg_c::types_opts::to_number() const
{
  if (value == f1_c_traffic_transfer) {
    return 1;
  }
  invalid_enum_number(value, "xnap_elem_procs_o::init_msg_c::types");
  return 0;
}

// SuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_xnap_elem_procs_o_successful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_request_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<retrieve_ue_context_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_addition_request_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_request_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_release_request_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_release_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_change_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_removal_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ngran_node_cfg_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e_utra_nr_cell_res_coordination_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cell_activation_resp_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<res_status_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mob_change_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_transport_migration_management_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_transport_migration_mod_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_res_coordination_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<partial_ue_context_transfer_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<data_collection_resp_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_xnap_elem_procs_o_successful_outcome_c_names[] = {
    {"HandoverRequestAcknowledge", nullptr},
    {"RetrieveUEContextResponse", nullptr},
    {"SNodeAdditionRequestAcknowledge", nullptr},
    {"SNodeModificationRequestAcknowledge", nullptr},
    {"SNodeModificationConfirm", nullptr},
    {"SNodeReleaseRequestAcknowledge", nullptr},
    {"SNodeReleaseConfirm", nullptr},
    {"SNodeChangeConfirm", nullptr},
    {"XnRemovalResponse", nullptr},
    {"XnSetupResponse", nullptr},
    {"NGRANNodeConfigurationUpdateAcknowledge", nullptr},
    {"E-UTRA-NR-CellResourceCoordinationResponse", nullptr},
    {"CellActivationResponse", nullptr},
    {"ResetResponse", nullptr},
    {"ResourceStatusResponse", nullptr},
    {"MobilityChangeAcknowledge", nullptr},
    {"IABTransportMigrationManagementResponse", nullptr},
    {"IABTransportMigrationModificationResponse", nullptr},
    {"IABResourceCoordinationResponse", nullptr},
    {"PartialUEContextTransferAcknowledge", nullptr},
    {"DataCollectionResponse", nullptr}};
const asn1::table::choice_desc xnap_elem_procs_o::successful_outcome_c::desc = {
    asn1::table::enum_int_type<xnap_elem_procs_o::successful_outcome_c::types>(),
    xnap_elem_procs_o::successful_outcome_c::types::nof_types,
    xnap_elem_procs_o::successful_outcome_c::types::nof_exts,
    xnap_elem_procs_o::successful_outcome_c::types::has_ext,
    true,
    choice_table_xnap_elem_procs_o_successful_outcome_c_alternatives,
    &asn1::table::choice_set<xnap_elem_procs_o::successful_outcome_c>,
    choice_table_xnap_elem_procs_o_successful_outcome_c_names,
    "xnap_elem_procs_o::successful_outcome_c",
    offsetof(xnap_elem_procs_o::successful_outcome_c, type_),
    offsetof(xnap_elem_procs_o::successful_outcome_c, c),
    false,
    nullptr,
    true};

void xnap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, xnap_elem_procs_o::successful_outcome_c::desc);
}
ho_request_ack_s& xnap_elem_procs_o::successful_outcome_c::ho_request_ack()
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
retrieve_ue_context_resp_s& xnap_elem_procs_o::successful_outcome_c::retrieve_ue_context_resp()
{
  assert_choice_type(types::retrieve_ue_context_resp, type_, "SuccessfulOutcome");
  return c.get<retrieve_ue_context_resp_s>();
}
sn_ode_addition_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_addition_request_ack()
{
  assert_choice_type(types::sn_ode_addition_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_addition_request_ack_s>();
}
sn_ode_mod_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_mod_request_ack()
{
  assert_choice_type(types::sn_ode_mod_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_request_ack_s>();
}
sn_ode_mod_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_mod_confirm()
{
  assert_choice_type(types::sn_ode_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_confirm_s>();
}
sn_ode_release_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_release_request_ack()
{
  assert_choice_type(types::sn_ode_release_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_request_ack_s>();
}
sn_ode_release_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_release_confirm()
{
  assert_choice_type(types::sn_ode_release_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_confirm_s>();
}
sn_ode_change_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_change_confirm()
{
  assert_choice_type(types::sn_ode_change_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_change_confirm_s>();
}
xn_removal_resp_s& xnap_elem_procs_o::successful_outcome_c::xn_removal_resp()
{
  assert_choice_type(types::xn_removal_resp, type_, "SuccessfulOutcome");
  return c.get<xn_removal_resp_s>();
}
xn_setup_resp_s& xnap_elem_procs_o::successful_outcome_c::xn_setup_resp()
{
  assert_choice_type(types::xn_setup_resp, type_, "SuccessfulOutcome");
  return c.get<xn_setup_resp_s>();
}
ngran_node_cfg_upd_ack_s& xnap_elem_procs_o::successful_outcome_c::ngran_node_cfg_upd_ack()
{
  assert_choice_type(types::ngran_node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_ack_s>();
}
e_utra_nr_cell_res_coordination_resp_s& xnap_elem_procs_o::successful_outcome_c::e_utra_nr_cell_res_coordination_resp()
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<e_utra_nr_cell_res_coordination_resp_s>();
}
cell_activation_resp_s& xnap_elem_procs_o::successful_outcome_c::cell_activation_resp()
{
  assert_choice_type(types::cell_activation_resp, type_, "SuccessfulOutcome");
  return c.get<cell_activation_resp_s>();
}
reset_resp_s& xnap_elem_procs_o::successful_outcome_c::reset_resp()
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
res_status_resp_s& xnap_elem_procs_o::successful_outcome_c::res_status_resp()
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
mob_change_ack_s& xnap_elem_procs_o::successful_outcome_c::mob_change_ack()
{
  assert_choice_type(types::mob_change_ack, type_, "SuccessfulOutcome");
  return c.get<mob_change_ack_s>();
}
iab_transport_migration_management_resp_s&
xnap_elem_procs_o::successful_outcome_c::iab_transport_migration_management_resp()
{
  assert_choice_type(types::iab_transport_migration_management_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_management_resp_s>();
}
iab_transport_migration_mod_resp_s& xnap_elem_procs_o::successful_outcome_c::iab_transport_migration_mod_resp()
{
  assert_choice_type(types::iab_transport_migration_mod_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_mod_resp_s>();
}
iab_res_coordination_resp_s& xnap_elem_procs_o::successful_outcome_c::iab_res_coordination_resp()
{
  assert_choice_type(types::iab_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<iab_res_coordination_resp_s>();
}
partial_ue_context_transfer_ack_s& xnap_elem_procs_o::successful_outcome_c::partial_ue_context_transfer_ack()
{
  assert_choice_type(types::partial_ue_context_transfer_ack, type_, "SuccessfulOutcome");
  return c.get<partial_ue_context_transfer_ack_s>();
}
data_collection_resp_s& xnap_elem_procs_o::successful_outcome_c::data_collection_resp()
{
  assert_choice_type(types::data_collection_resp, type_, "SuccessfulOutcome");
  return c.get<data_collection_resp_s>();
}
const ho_request_ack_s& xnap_elem_procs_o::successful_outcome_c::ho_request_ack() const
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
const retrieve_ue_context_resp_s& xnap_elem_procs_o::successful_outcome_c::retrieve_ue_context_resp() const
{
  assert_choice_type(types::retrieve_ue_context_resp, type_, "SuccessfulOutcome");
  return c.get<retrieve_ue_context_resp_s>();
}
const sn_ode_addition_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_addition_request_ack() const
{
  assert_choice_type(types::sn_ode_addition_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_addition_request_ack_s>();
}
const sn_ode_mod_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_mod_request_ack() const
{
  assert_choice_type(types::sn_ode_mod_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_request_ack_s>();
}
const sn_ode_mod_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_mod_confirm() const
{
  assert_choice_type(types::sn_ode_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_confirm_s>();
}
const sn_ode_release_request_ack_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_release_request_ack() const
{
  assert_choice_type(types::sn_ode_release_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_request_ack_s>();
}
const sn_ode_release_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_release_confirm() const
{
  assert_choice_type(types::sn_ode_release_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_confirm_s>();
}
const sn_ode_change_confirm_s& xnap_elem_procs_o::successful_outcome_c::sn_ode_change_confirm() const
{
  assert_choice_type(types::sn_ode_change_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_change_confirm_s>();
}
const xn_removal_resp_s& xnap_elem_procs_o::successful_outcome_c::xn_removal_resp() const
{
  assert_choice_type(types::xn_removal_resp, type_, "SuccessfulOutcome");
  return c.get<xn_removal_resp_s>();
}
const xn_setup_resp_s& xnap_elem_procs_o::successful_outcome_c::xn_setup_resp() const
{
  assert_choice_type(types::xn_setup_resp, type_, "SuccessfulOutcome");
  return c.get<xn_setup_resp_s>();
}
const ngran_node_cfg_upd_ack_s& xnap_elem_procs_o::successful_outcome_c::ngran_node_cfg_upd_ack() const
{
  assert_choice_type(types::ngran_node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_ack_s>();
}
const e_utra_nr_cell_res_coordination_resp_s&
xnap_elem_procs_o::successful_outcome_c::e_utra_nr_cell_res_coordination_resp() const
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<e_utra_nr_cell_res_coordination_resp_s>();
}
const cell_activation_resp_s& xnap_elem_procs_o::successful_outcome_c::cell_activation_resp() const
{
  assert_choice_type(types::cell_activation_resp, type_, "SuccessfulOutcome");
  return c.get<cell_activation_resp_s>();
}
const reset_resp_s& xnap_elem_procs_o::successful_outcome_c::reset_resp() const
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
const res_status_resp_s& xnap_elem_procs_o::successful_outcome_c::res_status_resp() const
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
const mob_change_ack_s& xnap_elem_procs_o::successful_outcome_c::mob_change_ack() const
{
  assert_choice_type(types::mob_change_ack, type_, "SuccessfulOutcome");
  return c.get<mob_change_ack_s>();
}
const iab_transport_migration_management_resp_s&
xnap_elem_procs_o::successful_outcome_c::iab_transport_migration_management_resp() const
{
  assert_choice_type(types::iab_transport_migration_management_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_management_resp_s>();
}
const iab_transport_migration_mod_resp_s&
xnap_elem_procs_o::successful_outcome_c::iab_transport_migration_mod_resp() const
{
  assert_choice_type(types::iab_transport_migration_mod_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_mod_resp_s>();
}
const iab_res_coordination_resp_s& xnap_elem_procs_o::successful_outcome_c::iab_res_coordination_resp() const
{
  assert_choice_type(types::iab_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<iab_res_coordination_resp_s>();
}
const partial_ue_context_transfer_ack_s&
xnap_elem_procs_o::successful_outcome_c::partial_ue_context_transfer_ack() const
{
  assert_choice_type(types::partial_ue_context_transfer_ack, type_, "SuccessfulOutcome");
  return c.get<partial_ue_context_transfer_ack_s>();
}
const data_collection_resp_s& xnap_elem_procs_o::successful_outcome_c::data_collection_resp() const
{
  assert_choice_type(types::data_collection_resp, type_, "SuccessfulOutcome");
  return c.get<data_collection_resp_s>();
}

static const char* enum_names_xnap_elem_procs_o_successful_outcome_c_types_values[] = {
    "HandoverRequestAcknowledge",
    "RetrieveUEContextResponse",
    "SNodeAdditionRequestAcknowledge",
    "SNodeModificationRequestAcknowledge",
    "SNodeModificationConfirm",
    "SNodeReleaseRequestAcknowledge",
    "SNodeReleaseConfirm",
    "SNodeChangeConfirm",
    "XnRemovalResponse",
    "XnSetupResponse",
    "NGRANNodeConfigurationUpdateAcknowledge",
    "E-UTRA-NR-CellResourceCoordinationResponse",
    "CellActivationResponse",
    "ResetResponse",
    "ResourceStatusResponse",
    "MobilityChangeAcknowledge",
    "IABTransportMigrationManagementResponse",
    "IABTransportMigrationModificationResponse",
    "IABResourceCoordinationResponse",
    "PartialUEContextTransferAcknowledge",
    "DataCollectionResponse"};
const asn1::table::enum_desc xnap_elem_procs_o::successful_outcome_c::types_opts::desc = {
    enum_names_xnap_elem_procs_o_successful_outcome_c_types_values,
    21,
    "xnap_elem_procs_o::successful_outcome_c::types"};

// UnsuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_xnap_elem_procs_o_unsuccessful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_prep_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<retrieve_ue_context_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_addition_request_reject_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_request_reject_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_refuse_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_release_reject_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_change_refuse_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_removal_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ngran_node_cfg_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cell_activation_fail_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<mob_change_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_transport_migration_management_reject_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<partial_ue_context_transfer_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<data_collection_fail_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_xnap_elem_procs_o_unsuccessful_outcome_c_names[] = {
    {"HandoverPreparationFailure", nullptr},
    {"RetrieveUEContextFailure", nullptr},
    {"SNodeAdditionRequestReject", nullptr},
    {"SNodeModificationRequestReject", nullptr},
    {"SNodeModificationRefuse", nullptr},
    {"SNodeReleaseReject", nullptr},
    {"SNodeChangeRefuse", nullptr},
    {"XnRemovalFailure", nullptr},
    {"XnSetupFailure", nullptr},
    {"NGRANNodeConfigurationUpdateFailure", nullptr},
    {"CellActivationFailure", nullptr},
    {"ResourceStatusFailure", nullptr},
    {"MobilityChangeFailure", nullptr},
    {"IABTransportMigrationManagementReject", nullptr},
    {"PartialUEContextTransferFailure", nullptr},
    {"DataCollectionFailure", nullptr}};
const asn1::table::choice_desc xnap_elem_procs_o::unsuccessful_outcome_c::desc = {
    asn1::table::enum_int_type<xnap_elem_procs_o::unsuccessful_outcome_c::types>(),
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nof_types,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::nof_exts,
    xnap_elem_procs_o::unsuccessful_outcome_c::types::has_ext,
    true,
    choice_table_xnap_elem_procs_o_unsuccessful_outcome_c_alternatives,
    &asn1::table::choice_set<xnap_elem_procs_o::unsuccessful_outcome_c>,
    choice_table_xnap_elem_procs_o_unsuccessful_outcome_c_names,
    "xnap_elem_procs_o::unsuccessful_outcome_c",
    offsetof(xnap_elem_procs_o::unsuccessful_outcome_c, type_),
    offsetof(xnap_elem_procs_o::unsuccessful_outcome_c, c),
    false,
    nullptr,
    true};

void xnap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, xnap_elem_procs_o::unsuccessful_outcome_c::desc);
}
ho_prep_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::ho_prep_fail()
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
retrieve_ue_context_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::retrieve_ue_context_fail()
{
  assert_choice_type(types::retrieve_ue_context_fail, type_, "UnsuccessfulOutcome");
  return c.get<retrieve_ue_context_fail_s>();
}
sn_ode_addition_request_reject_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_addition_request_reject()
{
  assert_choice_type(types::sn_ode_addition_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_addition_request_reject_s>();
}
sn_ode_mod_request_reject_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_mod_request_reject()
{
  assert_choice_type(types::sn_ode_mod_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_request_reject_s>();
}
sn_ode_mod_refuse_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_mod_refuse()
{
  assert_choice_type(types::sn_ode_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_refuse_s>();
}
sn_ode_release_reject_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_release_reject()
{
  assert_choice_type(types::sn_ode_release_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_release_reject_s>();
}
sn_ode_change_refuse_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_change_refuse()
{
  assert_choice_type(types::sn_ode_change_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_change_refuse_s>();
}
xn_removal_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::xn_removal_fail()
{
  assert_choice_type(types::xn_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_removal_fail_s>();
}
xn_setup_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::xn_setup_fail()
{
  assert_choice_type(types::xn_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_setup_fail_s>();
}
ngran_node_cfg_upd_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::ngran_node_cfg_upd_fail()
{
  assert_choice_type(types::ngran_node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_fail_s>();
}
cell_activation_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::cell_activation_fail()
{
  assert_choice_type(types::cell_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<cell_activation_fail_s>();
}
res_status_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::res_status_fail()
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
mob_change_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::mob_change_fail()
{
  assert_choice_type(types::mob_change_fail, type_, "UnsuccessfulOutcome");
  return c.get<mob_change_fail_s>();
}
iab_transport_migration_management_reject_s&
xnap_elem_procs_o::unsuccessful_outcome_c::iab_transport_migration_management_reject()
{
  assert_choice_type(types::iab_transport_migration_management_reject, type_, "UnsuccessfulOutcome");
  return c.get<iab_transport_migration_management_reject_s>();
}
partial_ue_context_transfer_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::partial_ue_context_transfer_fail()
{
  assert_choice_type(types::partial_ue_context_transfer_fail, type_, "UnsuccessfulOutcome");
  return c.get<partial_ue_context_transfer_fail_s>();
}
data_collection_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::data_collection_fail()
{
  assert_choice_type(types::data_collection_fail, type_, "UnsuccessfulOutcome");
  return c.get<data_collection_fail_s>();
}
const ho_prep_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::ho_prep_fail() const
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
const retrieve_ue_context_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::retrieve_ue_context_fail() const
{
  assert_choice_type(types::retrieve_ue_context_fail, type_, "UnsuccessfulOutcome");
  return c.get<retrieve_ue_context_fail_s>();
}
const sn_ode_addition_request_reject_s&
xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_addition_request_reject() const
{
  assert_choice_type(types::sn_ode_addition_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_addition_request_reject_s>();
}
const sn_ode_mod_request_reject_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_mod_request_reject() const
{
  assert_choice_type(types::sn_ode_mod_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_request_reject_s>();
}
const sn_ode_mod_refuse_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_mod_refuse() const
{
  assert_choice_type(types::sn_ode_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_refuse_s>();
}
const sn_ode_release_reject_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_release_reject() const
{
  assert_choice_type(types::sn_ode_release_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_release_reject_s>();
}
const sn_ode_change_refuse_s& xnap_elem_procs_o::unsuccessful_outcome_c::sn_ode_change_refuse() const
{
  assert_choice_type(types::sn_ode_change_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_change_refuse_s>();
}
const xn_removal_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::xn_removal_fail() const
{
  assert_choice_type(types::xn_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_removal_fail_s>();
}
const xn_setup_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::xn_setup_fail() const
{
  assert_choice_type(types::xn_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_setup_fail_s>();
}
const ngran_node_cfg_upd_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::ngran_node_cfg_upd_fail() const
{
  assert_choice_type(types::ngran_node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_fail_s>();
}
const cell_activation_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::cell_activation_fail() const
{
  assert_choice_type(types::cell_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<cell_activation_fail_s>();
}
const res_status_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::res_status_fail() const
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
const mob_change_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::mob_change_fail() const
{
  assert_choice_type(types::mob_change_fail, type_, "UnsuccessfulOutcome");
  return c.get<mob_change_fail_s>();
}
const iab_transport_migration_management_reject_s&
xnap_elem_procs_o::unsuccessful_outcome_c::iab_transport_migration_management_reject() const
{
  assert_choice_type(types::iab_transport_migration_management_reject, type_, "UnsuccessfulOutcome");
  return c.get<iab_transport_migration_management_reject_s>();
}
const partial_ue_context_transfer_fail_s&
xnap_elem_procs_o::unsuccessful_outcome_c::partial_ue_context_transfer_fail() const
{
  assert_choice_type(types::partial_ue_context_transfer_fail, type_, "UnsuccessfulOutcome");
  return c.get<partial_ue_context_transfer_fail_s>();
}
const data_collection_fail_s& xnap_elem_procs_o::unsuccessful_outcome_c::data_collection_fail() const
{
  assert_choice_type(types::data_collection_fail, type_, "UnsuccessfulOutcome");
  return c.get<data_collection_fail_s>();
}

static const char* enum_names_xnap_elem_procs_o_unsuccessful_outcome_c_types_values[] = {
    "HandoverPreparationFailure",
    "RetrieveUEContextFailure",
    "SNodeAdditionRequestReject",
    "SNodeModificationRequestReject",
    "SNodeModificationRefuse",
    "SNodeReleaseReject",
    "SNodeChangeRefuse",
    "XnRemovalFailure",
    "XnSetupFailure",
    "NGRANNodeConfigurationUpdateFailure",
    "CellActivationFailure",
    "ResourceStatusFailure",
    "MobilityChangeFailure",
    "IABTransportMigrationManagementReject",
    "PartialUEContextTransferFailure",
    "DataCollectionFailure"};
const asn1::table::enum_desc xnap_elem_procs_o::unsuccessful_outcome_c::types_opts::desc = {
    enum_names_xnap_elem_procs_o_unsuccessful_outcome_c_types_values,
    16,
    "xnap_elem_procs_o::unsuccessful_outcome_c::types"};

// Columns of the object sets of XNAP-ELEMENTARY-PROCEDURE.
enum class xnap_elem_proc_p_column : uint32_t { init_msg, successful_outcome, unsuccessful_outcome, crit };

// InitiatingMessage ::= SEQUENCE{{XNAP-ELEMENTARY-PROCEDURE}}
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
                                 xnap_elem_procs_o::desc,
                                 xnap_elem_proc_p_column::crit,
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
                                  xnap_elem_procs_o::desc,
                                  xnap_elem_proc_p_column::init_msg,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_init_msg_s_names[] = {{"procedureCode", nullptr},
                                                                     {"criticality", nullptr},
                                                                     {"value", nullptr}};
const asn1::table::seq_desc          init_msg_s::desc =
    {{seq_table_init_msg_s_fields, 3, UINT64_C(0x7), nullptr, 0}, false, true, nullptr, 0, seq_table_init_msg_s_names};
bool init_msg_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(xnap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(xnap_elem_procs_o::desc, xnap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, xnap_elem_procs_o::desc, xnap_elem_proc_p_column::init_msg, proc_code);
  return value.type().value != xnap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{XNAP-ELEMENTARY-PROCEDURE}}
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
                                 xnap_elem_procs_o::desc,
                                 xnap_elem_proc_p_column::crit,
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
                                  xnap_elem_procs_o::desc,
                                  xnap_elem_proc_p_column::successful_outcome,
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
  if (not asn1::table::objset_has_key(xnap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(xnap_elem_procs_o::desc, xnap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, xnap_elem_procs_o::desc, xnap_elem_proc_p_column::successful_outcome, proc_code);
  return value.type().value != xnap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{XNAP-ELEMENTARY-PROCEDURE}}
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
                                 xnap_elem_procs_o::desc,
                                 xnap_elem_proc_p_column::crit,
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
                                  xnap_elem_procs_o::desc,
                                  xnap_elem_proc_p_column::unsuccessful_outcome,
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
  if (not asn1::table::objset_has_key(xnap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(xnap_elem_procs_o::desc, xnap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, xnap_elem_procs_o::desc, xnap_elem_proc_p_column::unsuccessful_outcome, proc_code);
  return value.type().value != xnap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// XNAP-ELEMENTARY-PROCEDURES-CLASS-1 ::= OBJECT SET OF XNAP-ELEMENTARY-PROCEDURE
static const uint32_t objset_proc_code_xnap_elem_procs_class_1_o[] = {0,  3,  7,  9,  10, 11, 12, 14, 16, 17, 18,
                                                                      25, 19, 20, 34, 36, 44, 45, 46, 49, 51};
static const uint8_t  objset_init_msg_xnap_elem_procs_class_1_o[]  = {
    xnap_elem_procs_class_1_o::init_msg_c::types::ho_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::retrieve_ue_context_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::sn_ode_addition_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::sn_ode_mod_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::sn_ode_mod_required,
    xnap_elem_procs_class_1_o::init_msg_c::types::sn_ode_release_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::sn_ode_release_required,
    xnap_elem_procs_class_1_o::init_msg_c::types::sn_ode_change_required,
    xnap_elem_procs_class_1_o::init_msg_c::types::xn_removal_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::xn_setup_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::ngran_node_cfg_upd,
    xnap_elem_procs_class_1_o::init_msg_c::types::e_utra_nr_cell_res_coordination_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::cell_activation_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::reset_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::res_status_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::mob_change_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::iab_transport_migration_management_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::iab_transport_migration_mod_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::iab_res_coordination_request,
    xnap_elem_procs_class_1_o::init_msg_c::types::partial_ue_context_transfer,
    xnap_elem_procs_class_1_o::init_msg_c::types::data_collection_request};
static const uint8_t objset_successful_outcome_xnap_elem_procs_class_1_o[] = {
    xnap_elem_procs_class_1_o::successful_outcome_c::types::ho_request_ack,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::retrieve_ue_context_resp,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::sn_ode_addition_request_ack,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::sn_ode_mod_request_ack,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::sn_ode_mod_confirm,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::sn_ode_release_request_ack,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::sn_ode_release_confirm,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::sn_ode_change_confirm,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::xn_removal_resp,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::xn_setup_resp,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::ngran_node_cfg_upd_ack,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::e_utra_nr_cell_res_coordination_resp,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::cell_activation_resp,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::reset_resp,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::res_status_resp,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::mob_change_ack,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::iab_transport_migration_management_resp,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::iab_transport_migration_mod_resp,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::iab_res_coordination_resp,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::partial_ue_context_transfer_ack,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::data_collection_resp};
static const uint8_t objset_unsuccessful_outcome_xnap_elem_procs_class_1_o[] = {
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::ho_prep_fail,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::retrieve_ue_context_fail,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::sn_ode_addition_request_reject,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::sn_ode_mod_request_reject,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::sn_ode_mod_refuse,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::sn_ode_release_reject,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::sn_ode_change_refuse,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::xn_removal_fail,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::xn_setup_fail,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::ngran_node_cfg_upd_fail,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::cell_activation_fail,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::res_status_fail,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::mob_change_fail,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::iab_transport_migration_management_reject,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::partial_ue_context_transfer_fail,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::data_collection_fail};
static const uint8_t objset_crit_xnap_elem_procs_class_1_o[] = {
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject};
static const asn1::table::objset_column objset_columns_xnap_elem_procs_class_1_o[] = {
    {objset_init_msg_xnap_elem_procs_class_1_o, xnap_elem_procs_class_1_o::init_msg_c::types::nulltype},
    {objset_successful_outcome_xnap_elem_procs_class_1_o,
     xnap_elem_procs_class_1_o::successful_outcome_c::types::nulltype},
    {objset_unsuccessful_outcome_xnap_elem_procs_class_1_o,
     xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::nulltype},
    {objset_crit_xnap_elem_procs_class_1_o, crit_e::nulltype}};
const asn1::table::objset_desc xnap_elem_procs_class_1_o::desc = {objset_proc_code_xnap_elem_procs_class_1_o,
                                                                  21,
                                                                  "proc_code",
                                                                  objset_columns_xnap_elem_procs_class_1_o};

// InitiatingMessage ::= OPEN TYPE
static const asn1::table::seq_field choice_table_xnap_elem_procs_class_1_o_init_msg_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<retrieve_ue_context_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_addition_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_release_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_change_required_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_removal_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ngran_node_cfg_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e_utra_nr_cell_res_coordination_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cell_activation_request_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<res_status_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mob_change_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_transport_migration_management_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_transport_migration_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_res_coordination_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<partial_ue_context_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<data_collection_request_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_xnap_elem_procs_class_1_o_init_msg_c_names[] = {
    {"HandoverRequest", nullptr},
    {"RetrieveUEContextRequest", nullptr},
    {"SNodeAdditionRequest", nullptr},
    {"SNodeModificationRequest", nullptr},
    {"SNodeModificationRequired", nullptr},
    {"SNodeReleaseRequest", nullptr},
    {"SNodeReleaseRequired", nullptr},
    {"SNodeChangeRequired", nullptr},
    {"XnRemovalRequest", nullptr},
    {"XnSetupRequest", nullptr},
    {"NGRANNodeConfigurationUpdate", nullptr},
    {"E-UTRA-NR-CellResourceCoordinationRequest", nullptr},
    {"CellActivationRequest", nullptr},
    {"ResetRequest", nullptr},
    {"ResourceStatusRequest", nullptr},
    {"MobilityChangeRequest", nullptr},
    {"IABTransportMigrationManagementRequest", nullptr},
    {"IABTransportMigrationModificationRequest", nullptr},
    {"IABResourceCoordinationRequest", nullptr},
    {"PartialUEContextTransfer", nullptr},
    {"DataCollectionRequest", nullptr}};
const asn1::table::choice_desc xnap_elem_procs_class_1_o::init_msg_c::desc = {
    asn1::table::enum_int_type<xnap_elem_procs_class_1_o::init_msg_c::types>(),
    xnap_elem_procs_class_1_o::init_msg_c::types::nof_types,
    xnap_elem_procs_class_1_o::init_msg_c::types::nof_exts,
    xnap_elem_procs_class_1_o::init_msg_c::types::has_ext,
    true,
    choice_table_xnap_elem_procs_class_1_o_init_msg_c_alternatives,
    &asn1::table::choice_set<xnap_elem_procs_class_1_o::init_msg_c>,
    choice_table_xnap_elem_procs_class_1_o_init_msg_c_names,
    "xnap_elem_procs_class_1_o::init_msg_c",
    offsetof(xnap_elem_procs_class_1_o::init_msg_c, type_),
    offsetof(xnap_elem_procs_class_1_o::init_msg_c, c),
    false,
    nullptr,
    true};

void xnap_elem_procs_class_1_o::init_msg_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, xnap_elem_procs_class_1_o::init_msg_c::desc);
}
ho_request_s& xnap_elem_procs_class_1_o::init_msg_c::ho_request()
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
retrieve_ue_context_request_s& xnap_elem_procs_class_1_o::init_msg_c::retrieve_ue_context_request()
{
  assert_choice_type(types::retrieve_ue_context_request, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_request_s>();
}
sn_ode_addition_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_addition_request()
{
  assert_choice_type(types::sn_ode_addition_request, type_, "InitiatingMessage");
  return c.get<sn_ode_addition_request_s>();
}
sn_ode_mod_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_mod_request()
{
  assert_choice_type(types::sn_ode_mod_request, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_request_s>();
}
sn_ode_mod_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_mod_required()
{
  assert_choice_type(types::sn_ode_mod_required, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_required_s>();
}
sn_ode_release_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_release_request()
{
  assert_choice_type(types::sn_ode_release_request, type_, "InitiatingMessage");
  return c.get<sn_ode_release_request_s>();
}
sn_ode_release_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_release_required()
{
  assert_choice_type(types::sn_ode_release_required, type_, "InitiatingMessage");
  return c.get<sn_ode_release_required_s>();
}
sn_ode_change_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_change_required()
{
  assert_choice_type(types::sn_ode_change_required, type_, "InitiatingMessage");
  return c.get<sn_ode_change_required_s>();
}
xn_removal_request_s& xnap_elem_procs_class_1_o::init_msg_c::xn_removal_request()
{
  assert_choice_type(types::xn_removal_request, type_, "InitiatingMessage");
  return c.get<xn_removal_request_s>();
}
xn_setup_request_s& xnap_elem_procs_class_1_o::init_msg_c::xn_setup_request()
{
  assert_choice_type(types::xn_setup_request, type_, "InitiatingMessage");
  return c.get<xn_setup_request_s>();
}
ngran_node_cfg_upd_s& xnap_elem_procs_class_1_o::init_msg_c::ngran_node_cfg_upd()
{
  assert_choice_type(types::ngran_node_cfg_upd, type_, "InitiatingMessage");
  return c.get<ngran_node_cfg_upd_s>();
}
e_utra_nr_cell_res_coordination_request_s&
xnap_elem_procs_class_1_o::init_msg_c::e_utra_nr_cell_res_coordination_request()
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_request, type_, "InitiatingMessage");
  return c.get<e_utra_nr_cell_res_coordination_request_s>();
}
cell_activation_request_s& xnap_elem_procs_class_1_o::init_msg_c::cell_activation_request()
{
  assert_choice_type(types::cell_activation_request, type_, "InitiatingMessage");
  return c.get<cell_activation_request_s>();
}
reset_request_s& xnap_elem_procs_class_1_o::init_msg_c::reset_request()
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
res_status_request_s& xnap_elem_procs_class_1_o::init_msg_c::res_status_request()
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
mob_change_request_s& xnap_elem_procs_class_1_o::init_msg_c::mob_change_request()
{
  assert_choice_type(types::mob_change_request, type_, "InitiatingMessage");
  return c.get<mob_change_request_s>();
}
iab_transport_migration_management_request_s&
xnap_elem_procs_class_1_o::init_msg_c::iab_transport_migration_management_request()
{
  assert_choice_type(types::iab_transport_migration_management_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_management_request_s>();
}
iab_transport_migration_mod_request_s& xnap_elem_procs_class_1_o::init_msg_c::iab_transport_migration_mod_request()
{
  assert_choice_type(types::iab_transport_migration_mod_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_mod_request_s>();
}
iab_res_coordination_request_s& xnap_elem_procs_class_1_o::init_msg_c::iab_res_coordination_request()
{
  assert_choice_type(types::iab_res_coordination_request, type_, "InitiatingMessage");
  return c.get<iab_res_coordination_request_s>();
}
partial_ue_context_transfer_s& xnap_elem_procs_class_1_o::init_msg_c::partial_ue_context_transfer()
{
  assert_choice_type(types::partial_ue_context_transfer, type_, "InitiatingMessage");
  return c.get<partial_ue_context_transfer_s>();
}
data_collection_request_s& xnap_elem_procs_class_1_o::init_msg_c::data_collection_request()
{
  assert_choice_type(types::data_collection_request, type_, "InitiatingMessage");
  return c.get<data_collection_request_s>();
}
const ho_request_s& xnap_elem_procs_class_1_o::init_msg_c::ho_request() const
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
const retrieve_ue_context_request_s& xnap_elem_procs_class_1_o::init_msg_c::retrieve_ue_context_request() const
{
  assert_choice_type(types::retrieve_ue_context_request, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_request_s>();
}
const sn_ode_addition_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_addition_request() const
{
  assert_choice_type(types::sn_ode_addition_request, type_, "InitiatingMessage");
  return c.get<sn_ode_addition_request_s>();
}
const sn_ode_mod_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_mod_request() const
{
  assert_choice_type(types::sn_ode_mod_request, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_request_s>();
}
const sn_ode_mod_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_mod_required() const
{
  assert_choice_type(types::sn_ode_mod_required, type_, "InitiatingMessage");
  return c.get<sn_ode_mod_required_s>();
}
const sn_ode_release_request_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_release_request() const
{
  assert_choice_type(types::sn_ode_release_request, type_, "InitiatingMessage");
  return c.get<sn_ode_release_request_s>();
}
const sn_ode_release_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_release_required() const
{
  assert_choice_type(types::sn_ode_release_required, type_, "InitiatingMessage");
  return c.get<sn_ode_release_required_s>();
}
const sn_ode_change_required_s& xnap_elem_procs_class_1_o::init_msg_c::sn_ode_change_required() const
{
  assert_choice_type(types::sn_ode_change_required, type_, "InitiatingMessage");
  return c.get<sn_ode_change_required_s>();
}
const xn_removal_request_s& xnap_elem_procs_class_1_o::init_msg_c::xn_removal_request() const
{
  assert_choice_type(types::xn_removal_request, type_, "InitiatingMessage");
  return c.get<xn_removal_request_s>();
}
const xn_setup_request_s& xnap_elem_procs_class_1_o::init_msg_c::xn_setup_request() const
{
  assert_choice_type(types::xn_setup_request, type_, "InitiatingMessage");
  return c.get<xn_setup_request_s>();
}
const ngran_node_cfg_upd_s& xnap_elem_procs_class_1_o::init_msg_c::ngran_node_cfg_upd() const
{
  assert_choice_type(types::ngran_node_cfg_upd, type_, "InitiatingMessage");
  return c.get<ngran_node_cfg_upd_s>();
}
const e_utra_nr_cell_res_coordination_request_s&
xnap_elem_procs_class_1_o::init_msg_c::e_utra_nr_cell_res_coordination_request() const
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_request, type_, "InitiatingMessage");
  return c.get<e_utra_nr_cell_res_coordination_request_s>();
}
const cell_activation_request_s& xnap_elem_procs_class_1_o::init_msg_c::cell_activation_request() const
{
  assert_choice_type(types::cell_activation_request, type_, "InitiatingMessage");
  return c.get<cell_activation_request_s>();
}
const reset_request_s& xnap_elem_procs_class_1_o::init_msg_c::reset_request() const
{
  assert_choice_type(types::reset_request, type_, "InitiatingMessage");
  return c.get<reset_request_s>();
}
const res_status_request_s& xnap_elem_procs_class_1_o::init_msg_c::res_status_request() const
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
const mob_change_request_s& xnap_elem_procs_class_1_o::init_msg_c::mob_change_request() const
{
  assert_choice_type(types::mob_change_request, type_, "InitiatingMessage");
  return c.get<mob_change_request_s>();
}
const iab_transport_migration_management_request_s&
xnap_elem_procs_class_1_o::init_msg_c::iab_transport_migration_management_request() const
{
  assert_choice_type(types::iab_transport_migration_management_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_management_request_s>();
}
const iab_transport_migration_mod_request_s&
xnap_elem_procs_class_1_o::init_msg_c::iab_transport_migration_mod_request() const
{
  assert_choice_type(types::iab_transport_migration_mod_request, type_, "InitiatingMessage");
  return c.get<iab_transport_migration_mod_request_s>();
}
const iab_res_coordination_request_s& xnap_elem_procs_class_1_o::init_msg_c::iab_res_coordination_request() const
{
  assert_choice_type(types::iab_res_coordination_request, type_, "InitiatingMessage");
  return c.get<iab_res_coordination_request_s>();
}
const partial_ue_context_transfer_s& xnap_elem_procs_class_1_o::init_msg_c::partial_ue_context_transfer() const
{
  assert_choice_type(types::partial_ue_context_transfer, type_, "InitiatingMessage");
  return c.get<partial_ue_context_transfer_s>();
}
const data_collection_request_s& xnap_elem_procs_class_1_o::init_msg_c::data_collection_request() const
{
  assert_choice_type(types::data_collection_request, type_, "InitiatingMessage");
  return c.get<data_collection_request_s>();
}

static const char* enum_names_xnap_elem_procs_class_1_o_init_msg_c_types_values[] = {
    "HandoverRequest",
    "RetrieveUEContextRequest",
    "SNodeAdditionRequest",
    "SNodeModificationRequest",
    "SNodeModificationRequired",
    "SNodeReleaseRequest",
    "SNodeReleaseRequired",
    "SNodeChangeRequired",
    "XnRemovalRequest",
    "XnSetupRequest",
    "NGRANNodeConfigurationUpdate",
    "E-UTRA-NR-CellResourceCoordinationRequest",
    "CellActivationRequest",
    "ResetRequest",
    "ResourceStatusRequest",
    "MobilityChangeRequest",
    "IABTransportMigrationManagementRequest",
    "IABTransportMigrationModificationRequest",
    "IABResourceCoordinationRequest",
    "PartialUEContextTransfer",
    "DataCollectionRequest"};
const asn1::table::enum_desc xnap_elem_procs_class_1_o::init_msg_c::types_opts::desc = {
    enum_names_xnap_elem_procs_class_1_o_init_msg_c_types_values,
    21,
    "xnap_elem_procs_class_1_o::init_msg_c::types"};

// SuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_xnap_elem_procs_class_1_o_successful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_request_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<retrieve_ue_context_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_addition_request_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_request_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_release_request_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_release_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_change_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_removal_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ngran_node_cfg_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<e_utra_nr_cell_res_coordination_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cell_activation_resp_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<res_status_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mob_change_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_transport_migration_management_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_transport_migration_mod_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_res_coordination_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<partial_ue_context_transfer_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<data_collection_resp_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_xnap_elem_procs_class_1_o_successful_outcome_c_names[] = {
    {"HandoverRequestAcknowledge", nullptr},
    {"RetrieveUEContextResponse", nullptr},
    {"SNodeAdditionRequestAcknowledge", nullptr},
    {"SNodeModificationRequestAcknowledge", nullptr},
    {"SNodeModificationConfirm", nullptr},
    {"SNodeReleaseRequestAcknowledge", nullptr},
    {"SNodeReleaseConfirm", nullptr},
    {"SNodeChangeConfirm", nullptr},
    {"XnRemovalResponse", nullptr},
    {"XnSetupResponse", nullptr},
    {"NGRANNodeConfigurationUpdateAcknowledge", nullptr},
    {"E-UTRA-NR-CellResourceCoordinationResponse", nullptr},
    {"CellActivationResponse", nullptr},
    {"ResetResponse", nullptr},
    {"ResourceStatusResponse", nullptr},
    {"MobilityChangeAcknowledge", nullptr},
    {"IABTransportMigrationManagementResponse", nullptr},
    {"IABTransportMigrationModificationResponse", nullptr},
    {"IABResourceCoordinationResponse", nullptr},
    {"PartialUEContextTransferAcknowledge", nullptr},
    {"DataCollectionResponse", nullptr}};
const asn1::table::choice_desc xnap_elem_procs_class_1_o::successful_outcome_c::desc = {
    asn1::table::enum_int_type<xnap_elem_procs_class_1_o::successful_outcome_c::types>(),
    xnap_elem_procs_class_1_o::successful_outcome_c::types::nof_types,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::nof_exts,
    xnap_elem_procs_class_1_o::successful_outcome_c::types::has_ext,
    true,
    choice_table_xnap_elem_procs_class_1_o_successful_outcome_c_alternatives,
    &asn1::table::choice_set<xnap_elem_procs_class_1_o::successful_outcome_c>,
    choice_table_xnap_elem_procs_class_1_o_successful_outcome_c_names,
    "xnap_elem_procs_class_1_o::successful_outcome_c",
    offsetof(xnap_elem_procs_class_1_o::successful_outcome_c, type_),
    offsetof(xnap_elem_procs_class_1_o::successful_outcome_c, c),
    false,
    nullptr,
    true};

void xnap_elem_procs_class_1_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, xnap_elem_procs_class_1_o::successful_outcome_c::desc);
}
ho_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::ho_request_ack()
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
retrieve_ue_context_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::retrieve_ue_context_resp()
{
  assert_choice_type(types::retrieve_ue_context_resp, type_, "SuccessfulOutcome");
  return c.get<retrieve_ue_context_resp_s>();
}
sn_ode_addition_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_addition_request_ack()
{
  assert_choice_type(types::sn_ode_addition_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_addition_request_ack_s>();
}
sn_ode_mod_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_mod_request_ack()
{
  assert_choice_type(types::sn_ode_mod_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_request_ack_s>();
}
sn_ode_mod_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_mod_confirm()
{
  assert_choice_type(types::sn_ode_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_confirm_s>();
}
sn_ode_release_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_release_request_ack()
{
  assert_choice_type(types::sn_ode_release_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_request_ack_s>();
}
sn_ode_release_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_release_confirm()
{
  assert_choice_type(types::sn_ode_release_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_confirm_s>();
}
sn_ode_change_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_change_confirm()
{
  assert_choice_type(types::sn_ode_change_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_change_confirm_s>();
}
xn_removal_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::xn_removal_resp()
{
  assert_choice_type(types::xn_removal_resp, type_, "SuccessfulOutcome");
  return c.get<xn_removal_resp_s>();
}
xn_setup_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::xn_setup_resp()
{
  assert_choice_type(types::xn_setup_resp, type_, "SuccessfulOutcome");
  return c.get<xn_setup_resp_s>();
}
ngran_node_cfg_upd_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::ngran_node_cfg_upd_ack()
{
  assert_choice_type(types::ngran_node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_ack_s>();
}
e_utra_nr_cell_res_coordination_resp_s&
xnap_elem_procs_class_1_o::successful_outcome_c::e_utra_nr_cell_res_coordination_resp()
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<e_utra_nr_cell_res_coordination_resp_s>();
}
cell_activation_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::cell_activation_resp()
{
  assert_choice_type(types::cell_activation_resp, type_, "SuccessfulOutcome");
  return c.get<cell_activation_resp_s>();
}
reset_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::reset_resp()
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
res_status_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::res_status_resp()
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
mob_change_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::mob_change_ack()
{
  assert_choice_type(types::mob_change_ack, type_, "SuccessfulOutcome");
  return c.get<mob_change_ack_s>();
}
iab_transport_migration_management_resp_s&
xnap_elem_procs_class_1_o::successful_outcome_c::iab_transport_migration_management_resp()
{
  assert_choice_type(types::iab_transport_migration_management_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_management_resp_s>();
}
iab_transport_migration_mod_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::iab_transport_migration_mod_resp()
{
  assert_choice_type(types::iab_transport_migration_mod_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_mod_resp_s>();
}
iab_res_coordination_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::iab_res_coordination_resp()
{
  assert_choice_type(types::iab_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<iab_res_coordination_resp_s>();
}
partial_ue_context_transfer_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::partial_ue_context_transfer_ack()
{
  assert_choice_type(types::partial_ue_context_transfer_ack, type_, "SuccessfulOutcome");
  return c.get<partial_ue_context_transfer_ack_s>();
}
data_collection_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::data_collection_resp()
{
  assert_choice_type(types::data_collection_resp, type_, "SuccessfulOutcome");
  return c.get<data_collection_resp_s>();
}
const ho_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::ho_request_ack() const
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
const retrieve_ue_context_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::retrieve_ue_context_resp() const
{
  assert_choice_type(types::retrieve_ue_context_resp, type_, "SuccessfulOutcome");
  return c.get<retrieve_ue_context_resp_s>();
}
const sn_ode_addition_request_ack_s&
xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_addition_request_ack() const
{
  assert_choice_type(types::sn_ode_addition_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_addition_request_ack_s>();
}
const sn_ode_mod_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_mod_request_ack() const
{
  assert_choice_type(types::sn_ode_mod_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_request_ack_s>();
}
const sn_ode_mod_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_mod_confirm() const
{
  assert_choice_type(types::sn_ode_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_mod_confirm_s>();
}
const sn_ode_release_request_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_release_request_ack() const
{
  assert_choice_type(types::sn_ode_release_request_ack, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_request_ack_s>();
}
const sn_ode_release_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_release_confirm() const
{
  assert_choice_type(types::sn_ode_release_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_release_confirm_s>();
}
const sn_ode_change_confirm_s& xnap_elem_procs_class_1_o::successful_outcome_c::sn_ode_change_confirm() const
{
  assert_choice_type(types::sn_ode_change_confirm, type_, "SuccessfulOutcome");
  return c.get<sn_ode_change_confirm_s>();
}
const xn_removal_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::xn_removal_resp() const
{
  assert_choice_type(types::xn_removal_resp, type_, "SuccessfulOutcome");
  return c.get<xn_removal_resp_s>();
}
const xn_setup_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::xn_setup_resp() const
{
  assert_choice_type(types::xn_setup_resp, type_, "SuccessfulOutcome");
  return c.get<xn_setup_resp_s>();
}
const ngran_node_cfg_upd_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::ngran_node_cfg_upd_ack() const
{
  assert_choice_type(types::ngran_node_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_ack_s>();
}
const e_utra_nr_cell_res_coordination_resp_s&
xnap_elem_procs_class_1_o::successful_outcome_c::e_utra_nr_cell_res_coordination_resp() const
{
  assert_choice_type(types::e_utra_nr_cell_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<e_utra_nr_cell_res_coordination_resp_s>();
}
const cell_activation_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::cell_activation_resp() const
{
  assert_choice_type(types::cell_activation_resp, type_, "SuccessfulOutcome");
  return c.get<cell_activation_resp_s>();
}
const reset_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::reset_resp() const
{
  assert_choice_type(types::reset_resp, type_, "SuccessfulOutcome");
  return c.get<reset_resp_s>();
}
const res_status_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::res_status_resp() const
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
const mob_change_ack_s& xnap_elem_procs_class_1_o::successful_outcome_c::mob_change_ack() const
{
  assert_choice_type(types::mob_change_ack, type_, "SuccessfulOutcome");
  return c.get<mob_change_ack_s>();
}
const iab_transport_migration_management_resp_s&
xnap_elem_procs_class_1_o::successful_outcome_c::iab_transport_migration_management_resp() const
{
  assert_choice_type(types::iab_transport_migration_management_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_management_resp_s>();
}
const iab_transport_migration_mod_resp_s&
xnap_elem_procs_class_1_o::successful_outcome_c::iab_transport_migration_mod_resp() const
{
  assert_choice_type(types::iab_transport_migration_mod_resp, type_, "SuccessfulOutcome");
  return c.get<iab_transport_migration_mod_resp_s>();
}
const iab_res_coordination_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::iab_res_coordination_resp() const
{
  assert_choice_type(types::iab_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<iab_res_coordination_resp_s>();
}
const partial_ue_context_transfer_ack_s&
xnap_elem_procs_class_1_o::successful_outcome_c::partial_ue_context_transfer_ack() const
{
  assert_choice_type(types::partial_ue_context_transfer_ack, type_, "SuccessfulOutcome");
  return c.get<partial_ue_context_transfer_ack_s>();
}
const data_collection_resp_s& xnap_elem_procs_class_1_o::successful_outcome_c::data_collection_resp() const
{
  assert_choice_type(types::data_collection_resp, type_, "SuccessfulOutcome");
  return c.get<data_collection_resp_s>();
}

static const char* enum_names_xnap_elem_procs_class_1_o_successful_outcome_c_types_values[] = {
    "HandoverRequestAcknowledge",
    "RetrieveUEContextResponse",
    "SNodeAdditionRequestAcknowledge",
    "SNodeModificationRequestAcknowledge",
    "SNodeModificationConfirm",
    "SNodeReleaseRequestAcknowledge",
    "SNodeReleaseConfirm",
    "SNodeChangeConfirm",
    "XnRemovalResponse",
    "XnSetupResponse",
    "NGRANNodeConfigurationUpdateAcknowledge",
    "E-UTRA-NR-CellResourceCoordinationResponse",
    "CellActivationResponse",
    "ResetResponse",
    "ResourceStatusResponse",
    "MobilityChangeAcknowledge",
    "IABTransportMigrationManagementResponse",
    "IABTransportMigrationModificationResponse",
    "IABResourceCoordinationResponse",
    "PartialUEContextTransferAcknowledge",
    "DataCollectionResponse"};
const asn1::table::enum_desc xnap_elem_procs_class_1_o::successful_outcome_c::types_opts::desc = {
    enum_names_xnap_elem_procs_class_1_o_successful_outcome_c_types_values,
    21,
    "xnap_elem_procs_class_1_o::successful_outcome_c::types"};

// UnsuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_xnap_elem_procs_class_1_o_unsuccessful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_prep_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<retrieve_ue_context_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_addition_request_reject_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_request_reject_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_mod_refuse_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_release_reject_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_change_refuse_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_removal_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ngran_node_cfg_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cell_activation_fail_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<mob_change_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_transport_migration_management_reject_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<partial_ue_context_transfer_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<data_collection_fail_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_xnap_elem_procs_class_1_o_unsuccessful_outcome_c_names[] = {
    {"HandoverPreparationFailure", nullptr},
    {"RetrieveUEContextFailure", nullptr},
    {"SNodeAdditionRequestReject", nullptr},
    {"SNodeModificationRequestReject", nullptr},
    {"SNodeModificationRefuse", nullptr},
    {"SNodeReleaseReject", nullptr},
    {"SNodeChangeRefuse", nullptr},
    {"XnRemovalFailure", nullptr},
    {"XnSetupFailure", nullptr},
    {"NGRANNodeConfigurationUpdateFailure", nullptr},
    {"CellActivationFailure", nullptr},
    {"ResourceStatusFailure", nullptr},
    {"MobilityChangeFailure", nullptr},
    {"IABTransportMigrationManagementReject", nullptr},
    {"PartialUEContextTransferFailure", nullptr},
    {"DataCollectionFailure", nullptr}};
const asn1::table::choice_desc xnap_elem_procs_class_1_o::unsuccessful_outcome_c::desc = {
    asn1::table::enum_int_type<xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types>(),
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::nof_types,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::nof_exts,
    xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types::has_ext,
    true,
    choice_table_xnap_elem_procs_class_1_o_unsuccessful_outcome_c_alternatives,
    &asn1::table::choice_set<xnap_elem_procs_class_1_o::unsuccessful_outcome_c>,
    choice_table_xnap_elem_procs_class_1_o_unsuccessful_outcome_c_names,
    "xnap_elem_procs_class_1_o::unsuccessful_outcome_c",
    offsetof(xnap_elem_procs_class_1_o::unsuccessful_outcome_c, type_),
    offsetof(xnap_elem_procs_class_1_o::unsuccessful_outcome_c, c),
    false,
    nullptr,
    true};

void xnap_elem_procs_class_1_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, xnap_elem_procs_class_1_o::unsuccessful_outcome_c::desc);
}
ho_prep_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::ho_prep_fail()
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
retrieve_ue_context_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::retrieve_ue_context_fail()
{
  assert_choice_type(types::retrieve_ue_context_fail, type_, "UnsuccessfulOutcome");
  return c.get<retrieve_ue_context_fail_s>();
}
sn_ode_addition_request_reject_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_addition_request_reject()
{
  assert_choice_type(types::sn_ode_addition_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_addition_request_reject_s>();
}
sn_ode_mod_request_reject_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_mod_request_reject()
{
  assert_choice_type(types::sn_ode_mod_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_request_reject_s>();
}
sn_ode_mod_refuse_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_mod_refuse()
{
  assert_choice_type(types::sn_ode_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_refuse_s>();
}
sn_ode_release_reject_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_release_reject()
{
  assert_choice_type(types::sn_ode_release_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_release_reject_s>();
}
sn_ode_change_refuse_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_change_refuse()
{
  assert_choice_type(types::sn_ode_change_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_change_refuse_s>();
}
xn_removal_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::xn_removal_fail()
{
  assert_choice_type(types::xn_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_removal_fail_s>();
}
xn_setup_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::xn_setup_fail()
{
  assert_choice_type(types::xn_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_setup_fail_s>();
}
ngran_node_cfg_upd_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::ngran_node_cfg_upd_fail()
{
  assert_choice_type(types::ngran_node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_fail_s>();
}
cell_activation_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::cell_activation_fail()
{
  assert_choice_type(types::cell_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<cell_activation_fail_s>();
}
res_status_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::res_status_fail()
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
mob_change_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::mob_change_fail()
{
  assert_choice_type(types::mob_change_fail, type_, "UnsuccessfulOutcome");
  return c.get<mob_change_fail_s>();
}
iab_transport_migration_management_reject_s&
xnap_elem_procs_class_1_o::unsuccessful_outcome_c::iab_transport_migration_management_reject()
{
  assert_choice_type(types::iab_transport_migration_management_reject, type_, "UnsuccessfulOutcome");
  return c.get<iab_transport_migration_management_reject_s>();
}
partial_ue_context_transfer_fail_s&
xnap_elem_procs_class_1_o::unsuccessful_outcome_c::partial_ue_context_transfer_fail()
{
  assert_choice_type(types::partial_ue_context_transfer_fail, type_, "UnsuccessfulOutcome");
  return c.get<partial_ue_context_transfer_fail_s>();
}
data_collection_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::data_collection_fail()
{
  assert_choice_type(types::data_collection_fail, type_, "UnsuccessfulOutcome");
  return c.get<data_collection_fail_s>();
}
const ho_prep_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::ho_prep_fail() const
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
const retrieve_ue_context_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::retrieve_ue_context_fail() const
{
  assert_choice_type(types::retrieve_ue_context_fail, type_, "UnsuccessfulOutcome");
  return c.get<retrieve_ue_context_fail_s>();
}
const sn_ode_addition_request_reject_s&
xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_addition_request_reject() const
{
  assert_choice_type(types::sn_ode_addition_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_addition_request_reject_s>();
}
const sn_ode_mod_request_reject_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_mod_request_reject() const
{
  assert_choice_type(types::sn_ode_mod_request_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_request_reject_s>();
}
const sn_ode_mod_refuse_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_mod_refuse() const
{
  assert_choice_type(types::sn_ode_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_mod_refuse_s>();
}
const sn_ode_release_reject_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_release_reject() const
{
  assert_choice_type(types::sn_ode_release_reject, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_release_reject_s>();
}
const sn_ode_change_refuse_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::sn_ode_change_refuse() const
{
  assert_choice_type(types::sn_ode_change_refuse, type_, "UnsuccessfulOutcome");
  return c.get<sn_ode_change_refuse_s>();
}
const xn_removal_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::xn_removal_fail() const
{
  assert_choice_type(types::xn_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_removal_fail_s>();
}
const xn_setup_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::xn_setup_fail() const
{
  assert_choice_type(types::xn_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<xn_setup_fail_s>();
}
const ngran_node_cfg_upd_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::ngran_node_cfg_upd_fail() const
{
  assert_choice_type(types::ngran_node_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ngran_node_cfg_upd_fail_s>();
}
const cell_activation_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::cell_activation_fail() const
{
  assert_choice_type(types::cell_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<cell_activation_fail_s>();
}
const res_status_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::res_status_fail() const
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
const mob_change_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::mob_change_fail() const
{
  assert_choice_type(types::mob_change_fail, type_, "UnsuccessfulOutcome");
  return c.get<mob_change_fail_s>();
}
const iab_transport_migration_management_reject_s&
xnap_elem_procs_class_1_o::unsuccessful_outcome_c::iab_transport_migration_management_reject() const
{
  assert_choice_type(types::iab_transport_migration_management_reject, type_, "UnsuccessfulOutcome");
  return c.get<iab_transport_migration_management_reject_s>();
}
const partial_ue_context_transfer_fail_s&
xnap_elem_procs_class_1_o::unsuccessful_outcome_c::partial_ue_context_transfer_fail() const
{
  assert_choice_type(types::partial_ue_context_transfer_fail, type_, "UnsuccessfulOutcome");
  return c.get<partial_ue_context_transfer_fail_s>();
}
const data_collection_fail_s& xnap_elem_procs_class_1_o::unsuccessful_outcome_c::data_collection_fail() const
{
  assert_choice_type(types::data_collection_fail, type_, "UnsuccessfulOutcome");
  return c.get<data_collection_fail_s>();
}

static const char* enum_names_xnap_elem_procs_class_1_o_unsuccessful_outcome_c_types_values[] = {
    "HandoverPreparationFailure",
    "RetrieveUEContextFailure",
    "SNodeAdditionRequestReject",
    "SNodeModificationRequestReject",
    "SNodeModificationRefuse",
    "SNodeReleaseReject",
    "SNodeChangeRefuse",
    "XnRemovalFailure",
    "XnSetupFailure",
    "NGRANNodeConfigurationUpdateFailure",
    "CellActivationFailure",
    "ResourceStatusFailure",
    "MobilityChangeFailure",
    "IABTransportMigrationManagementReject",
    "PartialUEContextTransferFailure",
    "DataCollectionFailure"};
const asn1::table::enum_desc xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types_opts::desc = {
    enum_names_xnap_elem_procs_class_1_o_unsuccessful_outcome_c_types_values,
    16,
    "xnap_elem_procs_class_1_o::unsuccessful_outcome_c::types"};

// XNAP-ELEMENTARY-PROCEDURES-CLASS-2 ::= OBJECT SET OF XNAP-ELEMENTARY-PROCEDURE
static const uint32_t objset_proc_code_xnap_elem_procs_class_2_o[] = {1,  2,  4,  5,  6,  8,  13, 15, 21, 22, 23,
                                                                      24, 26, 27, 28, 29, 30, 31, 32, 33, 35, 37,
                                                                      38, 39, 40, 42, 43, 47, 48, 50, 52};
static const uint8_t  objset_init_msg_xnap_elem_procs_class_2_o[]  = {
    xnap_elem_procs_class_2_o::init_msg_c::types::sn_status_transfer,
    xnap_elem_procs_class_2_o::init_msg_c::types::ho_cancel,
    xnap_elem_procs_class_2_o::init_msg_c::types::ran_paging,
    xnap_elem_procs_class_2_o::init_msg_c::types::xn_u_address_ind,
    xnap_elem_procs_class_2_o::init_msg_c::types::ue_context_release,
    xnap_elem_procs_class_2_o::init_msg_c::types::sn_ode_recfg_complete,
    xnap_elem_procs_class_2_o::init_msg_c::types::sn_ode_counter_check_request,
    xnap_elem_procs_class_2_o::init_msg_c::types::rrc_transfer,
    xnap_elem_procs_class_2_o::init_msg_c::types::error_ind,
    xnap_elem_procs_class_2_o::init_msg_c::types::private_msg,
    xnap_elem_procs_class_2_o::init_msg_c::types::notif_ctrl_ind,
    xnap_elem_procs_class_2_o::init_msg_c::types::activity_notif,
    xnap_elem_procs_class_2_o::init_msg_c::types::secondary_rat_data_usage_report,
    xnap_elem_procs_class_2_o::init_msg_c::types::deactiv_trace,
    xnap_elem_procs_class_2_o::init_msg_c::types::trace_start,
    xnap_elem_procs_class_2_o::init_msg_c::types::ho_success,
    xnap_elem_procs_class_2_o::init_msg_c::types::conditional_ho_cancel,
    xnap_elem_procs_class_2_o::init_msg_c::types::early_status_transfer,
    xnap_elem_procs_class_2_o::init_msg_c::types::fail_ind,
    xnap_elem_procs_class_2_o::init_msg_c::types::ho_report,
    xnap_elem_procs_class_2_o::init_msg_c::types::res_status_upd,
    xnap_elem_procs_class_2_o::init_msg_c::types::access_and_mob_ind,
    xnap_elem_procs_class_2_o::init_msg_c::types::cell_traffic_trace,
    xnap_elem_procs_class_2_o::init_msg_c::types::ran_multicast_group_paging,
    xnap_elem_procs_class_2_o::init_msg_c::types::scg_fail_info_report,
    xnap_elem_procs_class_2_o::init_msg_c::types::scg_fail_transfer,
    xnap_elem_procs_class_2_o::init_msg_c::types::f1_c_traffic_transfer,
    xnap_elem_procs_class_2_o::init_msg_c::types::retrieve_ue_context_confirm,
    xnap_elem_procs_class_2_o::init_msg_c::types::cp_c_cancel,
    xnap_elem_procs_class_2_o::init_msg_c::types::rach_ind,
    xnap_elem_procs_class_2_o::init_msg_c::types::data_collection_upd};
static const uint8_t objset_successful_outcome_xnap_elem_procs_class_2_o[] = {
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype};
static const uint8_t objset_unsuccessful_outcome_xnap_elem_procs_class_2_o[] = {
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype};
static const uint8_t objset_crit_xnap_elem_procs_class_2_o[] = {
    crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::reject,
    crit_e::ignore, crit_e::ignore, crit_e::ignore};
static const asn1::table::objset_column objset_columns_xnap_elem_procs_class_2_o[] = {
    {objset_init_msg_xnap_elem_procs_class_2_o, xnap_elem_procs_class_2_o::init_msg_c::types::nulltype},
    {objset_successful_outcome_xnap_elem_procs_class_2_o,
     xnap_elem_procs_class_2_o::successful_outcome_c::types::nulltype},
    {objset_unsuccessful_outcome_xnap_elem_procs_class_2_o,
     xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nulltype},
    {objset_crit_xnap_elem_procs_class_2_o, crit_e::nulltype}};
const asn1::table::objset_desc xnap_elem_procs_class_2_o::desc = {objset_proc_code_xnap_elem_procs_class_2_o,
                                                                  31,
                                                                  "proc_code",
                                                                  objset_columns_xnap_elem_procs_class_2_o};

// InitiatingMessage ::= OPEN TYPE
static const asn1::table::seq_field choice_table_xnap_elem_procs_class_2_o_init_msg_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_status_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_cancel_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ran_paging_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<xn_u_address_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_release_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_recfg_complete_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sn_ode_counter_check_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<rrc_transfer_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<notif_ctrl_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<activity_notif_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<secondary_rat_data_usage_report_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ho_success_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<conditional_ho_cancel_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<early_status_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<fail_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_report_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<access_and_mob_ind_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ran_multicast_group_paging_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<scg_fail_info_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<scg_fail_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<f1_c_traffic_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<retrieve_ue_context_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cp_c_cancel_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<rach_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<data_collection_upd_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_xnap_elem_procs_class_2_o_init_msg_c_names[] = {
    {"SNStatusTransfer", nullptr},
    {"HandoverCancel", nullptr},
    {"RANPaging", nullptr},
    {"XnUAddressIndication", nullptr},
    {"UEContextRelease", nullptr},
    {"SNodeReconfigurationComplete", nullptr},
    {"SNodeCounterCheckRequest", nullptr},
    {"RRCTransfer", nullptr},
    {"ErrorIndication", nullptr},
    {"PrivateMessage", nullptr},
    {"NotificationControlIndication", nullptr},
    {"ActivityNotification", nullptr},
    {"SecondaryRATDataUsageReport", nullptr},
    {"DeactivateTrace", nullptr},
    {"TraceStart", nullptr},
    {"HandoverSuccess", nullptr},
    {"ConditionalHandoverCancel", nullptr},
    {"EarlyStatusTransfer", nullptr},
    {"FailureIndication", nullptr},
    {"HandoverReport", nullptr},
    {"ResourceStatusUpdate", nullptr},
    {"AccessAndMobilityIndication", nullptr},
    {"CellTrafficTrace", nullptr},
    {"RANMulticastGroupPaging", nullptr},
    {"ScgFailureInformationReport", nullptr},
    {"ScgFailureTransfer", nullptr},
    {"F1CTrafficTransfer", nullptr},
    {"RetrieveUEContextConfirm", nullptr},
    {"CPCCancel", nullptr},
    {"RachIndication", nullptr},
    {"DataCollectionUpdate", nullptr}};
const asn1::table::choice_desc xnap_elem_procs_class_2_o::init_msg_c::desc = {
    asn1::table::enum_int_type<xnap_elem_procs_class_2_o::init_msg_c::types>(),
    xnap_elem_procs_class_2_o::init_msg_c::types::nof_types,
    xnap_elem_procs_class_2_o::init_msg_c::types::nof_exts,
    xnap_elem_procs_class_2_o::init_msg_c::types::has_ext,
    true,
    choice_table_xnap_elem_procs_class_2_o_init_msg_c_alternatives,
    &asn1::table::choice_set<xnap_elem_procs_class_2_o::init_msg_c>,
    choice_table_xnap_elem_procs_class_2_o_init_msg_c_names,
    "xnap_elem_procs_class_2_o::init_msg_c",
    offsetof(xnap_elem_procs_class_2_o::init_msg_c, type_),
    offsetof(xnap_elem_procs_class_2_o::init_msg_c, c),
    false,
    nullptr,
    true};

void xnap_elem_procs_class_2_o::init_msg_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, xnap_elem_procs_class_2_o::init_msg_c::desc);
}
sn_status_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::sn_status_transfer()
{
  assert_choice_type(types::sn_status_transfer, type_, "InitiatingMessage");
  return c.get<sn_status_transfer_s>();
}
ho_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::ho_cancel()
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
ran_paging_s& xnap_elem_procs_class_2_o::init_msg_c::ran_paging()
{
  assert_choice_type(types::ran_paging, type_, "InitiatingMessage");
  return c.get<ran_paging_s>();
}
xn_u_address_ind_s& xnap_elem_procs_class_2_o::init_msg_c::xn_u_address_ind()
{
  assert_choice_type(types::xn_u_address_ind, type_, "InitiatingMessage");
  return c.get<xn_u_address_ind_s>();
}
ue_context_release_s& xnap_elem_procs_class_2_o::init_msg_c::ue_context_release()
{
  assert_choice_type(types::ue_context_release, type_, "InitiatingMessage");
  return c.get<ue_context_release_s>();
}
sn_ode_recfg_complete_s& xnap_elem_procs_class_2_o::init_msg_c::sn_ode_recfg_complete()
{
  assert_choice_type(types::sn_ode_recfg_complete, type_, "InitiatingMessage");
  return c.get<sn_ode_recfg_complete_s>();
}
sn_ode_counter_check_request_s& xnap_elem_procs_class_2_o::init_msg_c::sn_ode_counter_check_request()
{
  assert_choice_type(types::sn_ode_counter_check_request, type_, "InitiatingMessage");
  return c.get<sn_ode_counter_check_request_s>();
}
rrc_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::rrc_transfer()
{
  assert_choice_type(types::rrc_transfer, type_, "InitiatingMessage");
  return c.get<rrc_transfer_s>();
}
error_ind_s& xnap_elem_procs_class_2_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
private_msg_s& xnap_elem_procs_class_2_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
notif_ctrl_ind_s& xnap_elem_procs_class_2_o::init_msg_c::notif_ctrl_ind()
{
  assert_choice_type(types::notif_ctrl_ind, type_, "InitiatingMessage");
  return c.get<notif_ctrl_ind_s>();
}
activity_notif_s& xnap_elem_procs_class_2_o::init_msg_c::activity_notif()
{
  assert_choice_type(types::activity_notif, type_, "InitiatingMessage");
  return c.get<activity_notif_s>();
}
secondary_rat_data_usage_report_s& xnap_elem_procs_class_2_o::init_msg_c::secondary_rat_data_usage_report()
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
deactiv_trace_s& xnap_elem_procs_class_2_o::init_msg_c::deactiv_trace()
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
trace_start_s& xnap_elem_procs_class_2_o::init_msg_c::trace_start()
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
ho_success_s& xnap_elem_procs_class_2_o::init_msg_c::ho_success()
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
conditional_ho_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::conditional_ho_cancel()
{
  assert_choice_type(types::conditional_ho_cancel, type_, "InitiatingMessage");
  return c.get<conditional_ho_cancel_s>();
}
early_status_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::early_status_transfer()
{
  assert_choice_type(types::early_status_transfer, type_, "InitiatingMessage");
  return c.get<early_status_transfer_s>();
}
fail_ind_s& xnap_elem_procs_class_2_o::init_msg_c::fail_ind()
{
  assert_choice_type(types::fail_ind, type_, "InitiatingMessage");
  return c.get<fail_ind_s>();
}
ho_report_s& xnap_elem_procs_class_2_o::init_msg_c::ho_report()
{
  assert_choice_type(types::ho_report, type_, "InitiatingMessage");
  return c.get<ho_report_s>();
}
res_status_upd_s& xnap_elem_procs_class_2_o::init_msg_c::res_status_upd()
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
access_and_mob_ind_s& xnap_elem_procs_class_2_o::init_msg_c::access_and_mob_ind()
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
cell_traffic_trace_s& xnap_elem_procs_class_2_o::init_msg_c::cell_traffic_trace()
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
ran_multicast_group_paging_s& xnap_elem_procs_class_2_o::init_msg_c::ran_multicast_group_paging()
{
  assert_choice_type(types::ran_multicast_group_paging, type_, "InitiatingMessage");
  return c.get<ran_multicast_group_paging_s>();
}
scg_fail_info_report_s& xnap_elem_procs_class_2_o::init_msg_c::scg_fail_info_report()
{
  assert_choice_type(types::scg_fail_info_report, type_, "InitiatingMessage");
  return c.get<scg_fail_info_report_s>();
}
scg_fail_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::scg_fail_transfer()
{
  assert_choice_type(types::scg_fail_transfer, type_, "InitiatingMessage");
  return c.get<scg_fail_transfer_s>();
}
f1_c_traffic_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::f1_c_traffic_transfer()
{
  assert_choice_type(types::f1_c_traffic_transfer, type_, "InitiatingMessage");
  return c.get<f1_c_traffic_transfer_s>();
}
retrieve_ue_context_confirm_s& xnap_elem_procs_class_2_o::init_msg_c::retrieve_ue_context_confirm()
{
  assert_choice_type(types::retrieve_ue_context_confirm, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_confirm_s>();
}
cp_c_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::cp_c_cancel()
{
  assert_choice_type(types::cp_c_cancel, type_, "InitiatingMessage");
  return c.get<cp_c_cancel_s>();
}
rach_ind_s& xnap_elem_procs_class_2_o::init_msg_c::rach_ind()
{
  assert_choice_type(types::rach_ind, type_, "InitiatingMessage");
  return c.get<rach_ind_s>();
}
data_collection_upd_s& xnap_elem_procs_class_2_o::init_msg_c::data_collection_upd()
{
  assert_choice_type(types::data_collection_upd, type_, "InitiatingMessage");
  return c.get<data_collection_upd_s>();
}
const sn_status_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::sn_status_transfer() const
{
  assert_choice_type(types::sn_status_transfer, type_, "InitiatingMessage");
  return c.get<sn_status_transfer_s>();
}
const ho_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::ho_cancel() const
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
const ran_paging_s& xnap_elem_procs_class_2_o::init_msg_c::ran_paging() const
{
  assert_choice_type(types::ran_paging, type_, "InitiatingMessage");
  return c.get<ran_paging_s>();
}
const xn_u_address_ind_s& xnap_elem_procs_class_2_o::init_msg_c::xn_u_address_ind() const
{
  assert_choice_type(types::xn_u_address_ind, type_, "InitiatingMessage");
  return c.get<xn_u_address_ind_s>();
}
const ue_context_release_s& xnap_elem_procs_class_2_o::init_msg_c::ue_context_release() const
{
  assert_choice_type(types::ue_context_release, type_, "InitiatingMessage");
  return c.get<ue_context_release_s>();
}
const sn_ode_recfg_complete_s& xnap_elem_procs_class_2_o::init_msg_c::sn_ode_recfg_complete() const
{
  assert_choice_type(types::sn_ode_recfg_complete, type_, "InitiatingMessage");
  return c.get<sn_ode_recfg_complete_s>();
}
const sn_ode_counter_check_request_s& xnap_elem_procs_class_2_o::init_msg_c::sn_ode_counter_check_request() const
{
  assert_choice_type(types::sn_ode_counter_check_request, type_, "InitiatingMessage");
  return c.get<sn_ode_counter_check_request_s>();
}
const rrc_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::rrc_transfer() const
{
  assert_choice_type(types::rrc_transfer, type_, "InitiatingMessage");
  return c.get<rrc_transfer_s>();
}
const error_ind_s& xnap_elem_procs_class_2_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const private_msg_s& xnap_elem_procs_class_2_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const notif_ctrl_ind_s& xnap_elem_procs_class_2_o::init_msg_c::notif_ctrl_ind() const
{
  assert_choice_type(types::notif_ctrl_ind, type_, "InitiatingMessage");
  return c.get<notif_ctrl_ind_s>();
}
const activity_notif_s& xnap_elem_procs_class_2_o::init_msg_c::activity_notif() const
{
  assert_choice_type(types::activity_notif, type_, "InitiatingMessage");
  return c.get<activity_notif_s>();
}
const secondary_rat_data_usage_report_s& xnap_elem_procs_class_2_o::init_msg_c::secondary_rat_data_usage_report() const
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
const deactiv_trace_s& xnap_elem_procs_class_2_o::init_msg_c::deactiv_trace() const
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
const trace_start_s& xnap_elem_procs_class_2_o::init_msg_c::trace_start() const
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
const ho_success_s& xnap_elem_procs_class_2_o::init_msg_c::ho_success() const
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
const conditional_ho_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::conditional_ho_cancel() const
{
  assert_choice_type(types::conditional_ho_cancel, type_, "InitiatingMessage");
  return c.get<conditional_ho_cancel_s>();
}
const early_status_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::early_status_transfer() const
{
  assert_choice_type(types::early_status_transfer, type_, "InitiatingMessage");
  return c.get<early_status_transfer_s>();
}
const fail_ind_s& xnap_elem_procs_class_2_o::init_msg_c::fail_ind() const
{
  assert_choice_type(types::fail_ind, type_, "InitiatingMessage");
  return c.get<fail_ind_s>();
}
const ho_report_s& xnap_elem_procs_class_2_o::init_msg_c::ho_report() const
{
  assert_choice_type(types::ho_report, type_, "InitiatingMessage");
  return c.get<ho_report_s>();
}
const res_status_upd_s& xnap_elem_procs_class_2_o::init_msg_c::res_status_upd() const
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
const access_and_mob_ind_s& xnap_elem_procs_class_2_o::init_msg_c::access_and_mob_ind() const
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
const cell_traffic_trace_s& xnap_elem_procs_class_2_o::init_msg_c::cell_traffic_trace() const
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
const ran_multicast_group_paging_s& xnap_elem_procs_class_2_o::init_msg_c::ran_multicast_group_paging() const
{
  assert_choice_type(types::ran_multicast_group_paging, type_, "InitiatingMessage");
  return c.get<ran_multicast_group_paging_s>();
}
const scg_fail_info_report_s& xnap_elem_procs_class_2_o::init_msg_c::scg_fail_info_report() const
{
  assert_choice_type(types::scg_fail_info_report, type_, "InitiatingMessage");
  return c.get<scg_fail_info_report_s>();
}
const scg_fail_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::scg_fail_transfer() const
{
  assert_choice_type(types::scg_fail_transfer, type_, "InitiatingMessage");
  return c.get<scg_fail_transfer_s>();
}
const f1_c_traffic_transfer_s& xnap_elem_procs_class_2_o::init_msg_c::f1_c_traffic_transfer() const
{
  assert_choice_type(types::f1_c_traffic_transfer, type_, "InitiatingMessage");
  return c.get<f1_c_traffic_transfer_s>();
}
const retrieve_ue_context_confirm_s& xnap_elem_procs_class_2_o::init_msg_c::retrieve_ue_context_confirm() const
{
  assert_choice_type(types::retrieve_ue_context_confirm, type_, "InitiatingMessage");
  return c.get<retrieve_ue_context_confirm_s>();
}
const cp_c_cancel_s& xnap_elem_procs_class_2_o::init_msg_c::cp_c_cancel() const
{
  assert_choice_type(types::cp_c_cancel, type_, "InitiatingMessage");
  return c.get<cp_c_cancel_s>();
}
const rach_ind_s& xnap_elem_procs_class_2_o::init_msg_c::rach_ind() const
{
  assert_choice_type(types::rach_ind, type_, "InitiatingMessage");
  return c.get<rach_ind_s>();
}
const data_collection_upd_s& xnap_elem_procs_class_2_o::init_msg_c::data_collection_upd() const
{
  assert_choice_type(types::data_collection_upd, type_, "InitiatingMessage");
  return c.get<data_collection_upd_s>();
}

static const char*           enum_names_xnap_elem_procs_class_2_o_init_msg_c_types_values[] = {"SNStatusTransfer",
                                                                                               "HandoverCancel",
                                                                                               "RANPaging",
                                                                                               "XnUAddressIndication",
                                                                                               "UEContextRelease",
                                                                                               "SNodeReconfigurationComplete",
                                                                                               "SNodeCounterCheckRequest",
                                                                                               "RRCTransfer",
                                                                                               "ErrorIndication",
                                                                                               "PrivateMessage",
                                                                                               "NotificationControlIndication",
                                                                                               "ActivityNotification",
                                                                                               "SecondaryRATDataUsageReport",
                                                                                               "DeactivateTrace",
                                                                                               "TraceStart",
                                                                                               "HandoverSuccess",
                                                                                               "ConditionalHandoverCancel",
                                                                                               "EarlyStatusTransfer",
                                                                                               "FailureIndication",
                                                                                               "HandoverReport",
                                                                                               "ResourceStatusUpdate",
                                                                                               "AccessAndMobilityIndication",
                                                                                               "CellTrafficTrace",
                                                                                               "RANMulticastGroupPaging",
                                                                                               "ScgFailureInformationReport",
                                                                                               "ScgFailureTransfer",
                                                                                               "F1CTrafficTransfer",
                                                                                               "RetrieveUEContextConfirm",
                                                                                               "CPCCancel",
                                                                                               "RachIndication",
                                                                                               "DataCollectionUpdate"};
const asn1::table::enum_desc xnap_elem_procs_class_2_o::init_msg_c::types_opts::desc        = {
    enum_names_xnap_elem_procs_class_2_o_init_msg_c_types_values,
    31,
    "xnap_elem_procs_class_2_o::init_msg_c::types"};
uint8_t xnap_elem_procs_class_2_o::init_msg_c::types_opts::to_number() const
{
  if (value == f1_c_traffic_transfer) {
    return 1;
  }
  invalid_enum_number(value, "xnap_elem_procs_class_2_o::init_msg_c::types");
  return 0;
}

// SuccessfulOutcome ::= OPEN TYPE
static const asn1::table::field_name choice_table_xnap_elem_procs_class_2_o_successful_outcome_c_names[] = {};
const asn1::table::choice_desc       xnap_elem_procs_class_2_o::successful_outcome_c::desc               = {
    asn1::table::enum_int_type<xnap_elem_procs_class_2_o::successful_outcome_c::types>(),
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nof_types,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::nof_exts,
    xnap_elem_procs_class_2_o::successful_outcome_c::types::has_ext,
    true,
    nullptr,
    nullptr,
    choice_table_xnap_elem_procs_class_2_o_successful_outcome_c_names,
    "xnap_elem_procs_class_2_o::successful_outcome_c",
    asn1::table::choice_desc::no_member,
    asn1::table::choice_desc::no_member,
    false,
    nullptr,
    true};

static const char*           enum_names_xnap_elem_procs_class_2_o_successful_outcome_c_types_values[] = {};
const asn1::table::enum_desc xnap_elem_procs_class_2_o::successful_outcome_c::types_opts::desc        = {
    enum_names_xnap_elem_procs_class_2_o_successful_outcome_c_types_values,
    0,
    "xnap_elem_procs_class_2_o::successful_outcome_c::types"};

// UnsuccessfulOutcome ::= OPEN TYPE
static const asn1::table::field_name choice_table_xnap_elem_procs_class_2_o_unsuccessful_outcome_c_names[] = {};
const asn1::table::choice_desc       xnap_elem_procs_class_2_o::unsuccessful_outcome_c::desc               = {
    asn1::table::enum_int_type<xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types>(),
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nof_types,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::nof_exts,
    xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types::has_ext,
    true,
    nullptr,
    nullptr,
    choice_table_xnap_elem_procs_class_2_o_unsuccessful_outcome_c_names,
    "xnap_elem_procs_class_2_o::unsuccessful_outcome_c",
    asn1::table::choice_desc::no_member,
    asn1::table::choice_desc::no_member,
    false,
    nullptr,
    true};

static const char*           enum_names_xnap_elem_procs_class_2_o_unsuccessful_outcome_c_types_values[] = {};
const asn1::table::enum_desc xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types_opts::desc        = {
    enum_names_xnap_elem_procs_class_2_o_unsuccessful_outcome_c_types_values,
    0,
    "xnap_elem_procs_class_2_o::unsuccessful_outcome_c::types"};

// XnAP-PDU ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_xnap_pdu_c_alternatives[] = {
    &asn1::table::alternative_ops<init_msg_s, false>::value,
    &asn1::table::alternative_ops<successful_outcome_s, false>::value,
    &asn1::table::alternative_ops<unsuccessful_outcome_s, false>::value};
const asn1::table::choice_storage_desc xnap_pdu_c::storage_desc = {choice_storage_xnap_pdu_c_alternatives,
                                                                   3,
                                                                   "xnap_pdu_c"};

init_msg_s& xnap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& xnap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& xnap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
static const asn1::table::seq_field  choice_table_xnap_pdu_c_alternatives[] = {{0,
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
static const asn1::table::field_name choice_table_xnap_pdu_c_names[]        = {{"initiatingMessage", nullptr},
                                                                               {"successfulOutcome", nullptr},
                                                                               {"unsuccessfulOutcome", nullptr}};
const asn1::table::choice_desc       xnap_pdu_c::desc = {asn1::table::enum_int_type<xnap_pdu_c::types>(),
                                                         xnap_pdu_c::types::nof_types,
                                                         xnap_pdu_c::types::nof_exts,
                                                         xnap_pdu_c::types::has_ext,
                                                         true,
                                                         choice_table_xnap_pdu_c_alternatives,
                                                         nullptr,
                                                         choice_table_xnap_pdu_c_names,
                                                         "xnap_pdu_c",
                                                         offsetof(xnap_pdu_c, c.type),
                                                         offsetof(xnap_pdu_c, c.value),
                                                         false,
                                                         &xnap_pdu_c::storage_desc};

static const char*           enum_names_xnap_pdu_c_types_values[] = {"initiatingMessage",
                                                                     "successfulOutcome",
                                                                     "unsuccessfulOutcome"};
const asn1::table::enum_desc xnap_pdu_c::types_opts::desc         = {enum_names_xnap_pdu_c_types_values,
                                                                     3,
                                                                     "xnap_pdu_c::types"};
