// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/ngap/ngap.h"
#include "ocudu/asn1/asn1_ap_codec.h"
#include "ocudu/asn1/ngap/ngap_pdu_contents.h"

using namespace asn1;
using namespace asn1::ngap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// NGAP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF NGAP-ELEMENTARY-PROCEDURE
static const uint32_t objset_proc_code_ngap_elem_procs_o[] = {
    0,  66, 67, 68, 80, 69, 70, 10, 12, 13, 14, 78, 71, 72, 73, 20, 21, 25, 26, 27, 28, 29, 32, 35, 76, 40, 41,
    58, 59, 43, 60, 51, 64, 1,  75, 2,  65, 3,  4,  5,  6,  63, 7,  54, 8,  9,  11, 61, 15, 18, 16, 17, 74, 19,
    22, 23, 24, 30, 31, 33, 34, 57, 79, 36, 55, 37, 52, 77, 38, 39, 42, 56, 44, 45, 46, 47, 48, 62, 49, 53, 50};
static const uint8_t objset_init_msg_ngap_elem_procs_o[] = {
    ngap_elem_procs_o::init_msg_c::types::amf_cfg_upd,
    ngap_elem_procs_o::init_msg_c::types::broadcast_session_mod_request,
    ngap_elem_procs_o::init_msg_c::types::broadcast_session_release_request,
    ngap_elem_procs_o::init_msg_c::types::broadcast_session_setup_request,
    ngap_elem_procs_o::init_msg_c::types::broadcast_session_transport_request,
    ngap_elem_procs_o::init_msg_c::types::distribution_setup_request,
    ngap_elem_procs_o::init_msg_c::types::distribution_release_request,
    ngap_elem_procs_o::init_msg_c::types::ho_cancel,
    ngap_elem_procs_o::init_msg_c::types::ho_required,
    ngap_elem_procs_o::init_msg_c::types::ho_request,
    ngap_elem_procs_o::init_msg_c::types::init_context_setup_request,
    ngap_elem_procs_o::init_msg_c::types::mt_communication_handling_request,
    ngap_elem_procs_o::init_msg_c::types::multicast_session_activation_request,
    ngap_elem_procs_o::init_msg_c::types::multicast_session_deactivation_request,
    ngap_elem_procs_o::init_msg_c::types::multicast_session_upd_request,
    ngap_elem_procs_o::init_msg_c::types::ng_reset,
    ngap_elem_procs_o::init_msg_c::types::ng_setup_request,
    ngap_elem_procs_o::init_msg_c::types::path_switch_request,
    ngap_elem_procs_o::init_msg_c::types::pdu_session_res_modify_request,
    ngap_elem_procs_o::init_msg_c::types::pdu_session_res_modify_ind,
    ngap_elem_procs_o::init_msg_c::types::pdu_session_res_release_cmd,
    ngap_elem_procs_o::init_msg_c::types::pdu_session_res_setup_request,
    ngap_elem_procs_o::init_msg_c::types::pws_cancel_request,
    ngap_elem_procs_o::init_msg_c::types::ran_cfg_upd,
    ngap_elem_procs_o::init_msg_c::types::timing_synchronisation_status_request,
    ngap_elem_procs_o::init_msg_c::types::ue_context_mod_request,
    ngap_elem_procs_o::init_msg_c::types::ue_context_release_cmd,
    ngap_elem_procs_o::init_msg_c::types::ue_context_resume_request,
    ngap_elem_procs_o::init_msg_c::types::ue_context_suspend_request,
    ngap_elem_procs_o::init_msg_c::types::ue_radio_cap_check_request,
    ngap_elem_procs_o::init_msg_c::types::ue_radio_cap_id_map_request,
    ngap_elem_procs_o::init_msg_c::types::write_replace_warning_request,
    ngap_elem_procs_o::init_msg_c::types::amf_cp_relocation_ind,
    ngap_elem_procs_o::init_msg_c::types::amf_status_ind,
    ngap_elem_procs_o::init_msg_c::types::broadcast_session_release_required,
    ngap_elem_procs_o::init_msg_c::types::cell_traffic_trace,
    ngap_elem_procs_o::init_msg_c::types::conn_establishment_ind,
    ngap_elem_procs_o::init_msg_c::types::deactiv_trace,
    ngap_elem_procs_o::init_msg_c::types::dl_nas_transport,
    ngap_elem_procs_o::init_msg_c::types::dl_non_ue_associated_nrppa_transport,
    ngap_elem_procs_o::init_msg_c::types::dl_ran_cfg_transfer,
    ngap_elem_procs_o::init_msg_c::types::dl_ran_early_status_transfer,
    ngap_elem_procs_o::init_msg_c::types::dl_ran_status_transfer,
    ngap_elem_procs_o::init_msg_c::types::dl_rim_info_transfer,
    ngap_elem_procs_o::init_msg_c::types::dl_ue_associated_nrppa_transport,
    ngap_elem_procs_o::init_msg_c::types::error_ind,
    ngap_elem_procs_o::init_msg_c::types::ho_notify,
    ngap_elem_procs_o::init_msg_c::types::ho_success,
    ngap_elem_procs_o::init_msg_c::types::init_ue_msg,
    ngap_elem_procs_o::init_msg_c::types::location_report,
    ngap_elem_procs_o::init_msg_c::types::location_report_ctrl,
    ngap_elem_procs_o::init_msg_c::types::location_report_fail_ind,
    ngap_elem_procs_o::init_msg_c::types::multicast_group_paging,
    ngap_elem_procs_o::init_msg_c::types::nas_non_delivery_ind,
    ngap_elem_procs_o::init_msg_c::types::overload_start,
    ngap_elem_procs_o::init_msg_c::types::overload_stop,
    ngap_elem_procs_o::init_msg_c::types::paging,
    ngap_elem_procs_o::init_msg_c::types::pdu_session_res_notify,
    ngap_elem_procs_o::init_msg_c::types::private_msg,
    ngap_elem_procs_o::init_msg_c::types::pws_fail_ind,
    ngap_elem_procs_o::init_msg_c::types::pws_restart_ind,
    ngap_elem_procs_o::init_msg_c::types::ran_cp_relocation_ind,
    ngap_elem_procs_o::init_msg_c::types::ran_paging_request,
    ngap_elem_procs_o::init_msg_c::types::reroute_nas_request,
    ngap_elem_procs_o::init_msg_c::types::retrieve_ue_info,
    ngap_elem_procs_o::init_msg_c::types::rrc_inactive_transition_report,
    ngap_elem_procs_o::init_msg_c::types::secondary_rat_data_usage_report,
    ngap_elem_procs_o::init_msg_c::types::timing_synchronisation_status_report,
    ngap_elem_procs_o::init_msg_c::types::trace_fail_ind,
    ngap_elem_procs_o::init_msg_c::types::trace_start,
    ngap_elem_procs_o::init_msg_c::types::ue_context_release_request,
    ngap_elem_procs_o::init_msg_c::types::ue_info_transfer,
    ngap_elem_procs_o::init_msg_c::types::ue_radio_cap_info_ind,
    ngap_elem_procs_o::init_msg_c::types::ue_tnla_binding_release_request,
    ngap_elem_procs_o::init_msg_c::types::ul_nas_transport,
    ngap_elem_procs_o::init_msg_c::types::ul_non_ue_associated_nrppa_transport,
    ngap_elem_procs_o::init_msg_c::types::ul_ran_cfg_transfer,
    ngap_elem_procs_o::init_msg_c::types::ul_ran_early_status_transfer,
    ngap_elem_procs_o::init_msg_c::types::ul_ran_status_transfer,
    ngap_elem_procs_o::init_msg_c::types::ul_rim_info_transfer,
    ngap_elem_procs_o::init_msg_c::types::ul_ue_associated_nrppa_transport};
static const uint8_t objset_successful_outcome_ngap_elem_procs_o[] = {
    ngap_elem_procs_o::successful_outcome_c::types::amf_cfg_upd_ack,
    ngap_elem_procs_o::successful_outcome_c::types::broadcast_session_mod_resp,
    ngap_elem_procs_o::successful_outcome_c::types::broadcast_session_release_resp,
    ngap_elem_procs_o::successful_outcome_c::types::broadcast_session_setup_resp,
    ngap_elem_procs_o::successful_outcome_c::types::broadcast_session_transport_resp,
    ngap_elem_procs_o::successful_outcome_c::types::distribution_setup_resp,
    ngap_elem_procs_o::successful_outcome_c::types::distribution_release_resp,
    ngap_elem_procs_o::successful_outcome_c::types::ho_cancel_ack,
    ngap_elem_procs_o::successful_outcome_c::types::ho_cmd,
    ngap_elem_procs_o::successful_outcome_c::types::ho_request_ack,
    ngap_elem_procs_o::successful_outcome_c::types::init_context_setup_resp,
    ngap_elem_procs_o::successful_outcome_c::types::mt_communication_handling_resp,
    ngap_elem_procs_o::successful_outcome_c::types::multicast_session_activation_resp,
    ngap_elem_procs_o::successful_outcome_c::types::multicast_session_deactivation_resp,
    ngap_elem_procs_o::successful_outcome_c::types::multicast_session_upd_resp,
    ngap_elem_procs_o::successful_outcome_c::types::ng_reset_ack,
    ngap_elem_procs_o::successful_outcome_c::types::ng_setup_resp,
    ngap_elem_procs_o::successful_outcome_c::types::path_switch_request_ack,
    ngap_elem_procs_o::successful_outcome_c::types::pdu_session_res_modify_resp,
    ngap_elem_procs_o::successful_outcome_c::types::pdu_session_res_modify_confirm,
    ngap_elem_procs_o::successful_outcome_c::types::pdu_session_res_release_resp,
    ngap_elem_procs_o::successful_outcome_c::types::pdu_session_res_setup_resp,
    ngap_elem_procs_o::successful_outcome_c::types::pws_cancel_resp,
    ngap_elem_procs_o::successful_outcome_c::types::ran_cfg_upd_ack,
    ngap_elem_procs_o::successful_outcome_c::types::timing_synchronisation_status_resp,
    ngap_elem_procs_o::successful_outcome_c::types::ue_context_mod_resp,
    ngap_elem_procs_o::successful_outcome_c::types::ue_context_release_complete,
    ngap_elem_procs_o::successful_outcome_c::types::ue_context_resume_resp,
    ngap_elem_procs_o::successful_outcome_c::types::ue_context_suspend_resp,
    ngap_elem_procs_o::successful_outcome_c::types::ue_radio_cap_check_resp,
    ngap_elem_procs_o::successful_outcome_c::types::ue_radio_cap_id_map_resp,
    ngap_elem_procs_o::successful_outcome_c::types::write_replace_warning_resp,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype,
    ngap_elem_procs_o::successful_outcome_c::types::nulltype};
static const uint8_t objset_unsuccessful_outcome_ngap_elem_procs_o[] = {
    ngap_elem_procs_o::unsuccessful_outcome_c::types::amf_cfg_upd_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::broadcast_session_mod_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::broadcast_session_setup_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::broadcast_session_transport_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::distribution_setup_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::ho_prep_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::ho_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::init_context_setup_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::mt_communication_handling_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::multicast_session_activation_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::multicast_session_upd_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::ng_setup_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::path_switch_request_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::ran_cfg_upd_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::timing_synchronisation_status_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::ue_context_mod_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::ue_context_resume_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::ue_context_suspend_fail,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype};
static const uint8_t objset_crit_ngap_elem_procs_o[] = {
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::reject,
    crit_e::ignore, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::reject,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::ignore,
    crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore};
static const asn1::table::objset_column objset_columns_ngap_elem_procs_o[] = {
    {objset_init_msg_ngap_elem_procs_o, ngap_elem_procs_o::init_msg_c::types::nulltype},
    {objset_successful_outcome_ngap_elem_procs_o, ngap_elem_procs_o::successful_outcome_c::types::nulltype},
    {objset_unsuccessful_outcome_ngap_elem_procs_o, ngap_elem_procs_o::unsuccessful_outcome_c::types::nulltype},
    {objset_crit_ngap_elem_procs_o, crit_e::nulltype}};
const asn1::table::objset_desc ngap_elem_procs_o::desc = {objset_proc_code_ngap_elem_procs_o,
                                                          81,
                                                          "proc_code",
                                                          objset_columns_ngap_elem_procs_o};

// InitiatingMessage ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ngap_elem_procs_o_init_msg_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<amf_cfg_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_transport_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<distribution_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<distribution_release_request_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ho_required_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<init_context_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mt_communication_handling_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_session_activation_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_session_deactivation_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_session_upd_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ng_reset_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ng_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<path_switch_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdu_session_res_modify_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdu_session_res_modify_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdu_session_res_release_cmd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdu_session_res_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pws_cancel_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ran_cfg_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<timing_synchronisation_status_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_release_cmd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_resume_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_suspend_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_radio_cap_check_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_radio_cap_id_map_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<write_replace_warning_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<amf_cp_relocation_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<amf_status_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_release_required_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<conn_establishment_ind_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<dl_nas_transport_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<dl_non_ue_associated_nrppa_transport_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<dl_ran_cfg_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<dl_ran_early_status_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<dl_ran_status_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<dl_rim_info_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<dl_ue_associated_nrppa_transport_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ho_notify_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<init_ue_msg_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<location_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<location_report_ctrl_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<location_report_fail_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_group_paging_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<nas_non_delivery_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<overload_start_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<overload_stop_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<paging_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdu_session_res_notify_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<pws_fail_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pws_restart_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ran_cp_relocation_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ran_paging_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<reroute_nas_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<retrieve_ue_info_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<rrc_inactive_transition_report_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<timing_synchronisation_status_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<trace_fail_ind_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ue_context_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_info_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_radio_cap_info_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_tnla_binding_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ul_nas_transport_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ul_non_ue_associated_nrppa_transport_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ul_ran_cfg_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ul_ran_early_status_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ul_ran_status_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ul_rim_info_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ul_ue_associated_nrppa_transport_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_ngap_elem_procs_o_init_msg_c_names[] = {
    {"AMFConfigurationUpdate", nullptr},
    {"BroadcastSessionModificationRequest", nullptr},
    {"BroadcastSessionReleaseRequest", nullptr},
    {"BroadcastSessionSetupRequest", nullptr},
    {"BroadcastSessionTransportRequest", nullptr},
    {"DistributionSetupRequest", nullptr},
    {"DistributionReleaseRequest", nullptr},
    {"HandoverCancel", nullptr},
    {"HandoverRequired", nullptr},
    {"HandoverRequest", nullptr},
    {"InitialContextSetupRequest", nullptr},
    {"MTCommunicationHandlingRequest", nullptr},
    {"MulticastSessionActivationRequest", nullptr},
    {"MulticastSessionDeactivationRequest", nullptr},
    {"MulticastSessionUpdateRequest", nullptr},
    {"NGReset", nullptr},
    {"NGSetupRequest", nullptr},
    {"PathSwitchRequest", nullptr},
    {"PDUSessionResourceModifyRequest", nullptr},
    {"PDUSessionResourceModifyIndication", nullptr},
    {"PDUSessionResourceReleaseCommand", nullptr},
    {"PDUSessionResourceSetupRequest", nullptr},
    {"PWSCancelRequest", nullptr},
    {"RANConfigurationUpdate", nullptr},
    {"TimingSynchronisationStatusRequest", nullptr},
    {"UEContextModificationRequest", nullptr},
    {"UEContextReleaseCommand", nullptr},
    {"UEContextResumeRequest", nullptr},
    {"UEContextSuspendRequest", nullptr},
    {"UERadioCapabilityCheckRequest", nullptr},
    {"UERadioCapabilityIDMappingRequest", nullptr},
    {"WriteReplaceWarningRequest", nullptr},
    {"AMFCPRelocationIndication", nullptr},
    {"AMFStatusIndication", nullptr},
    {"BroadcastSessionReleaseRequired", nullptr},
    {"CellTrafficTrace", nullptr},
    {"ConnectionEstablishmentIndication", nullptr},
    {"DeactivateTrace", nullptr},
    {"DownlinkNASTransport", nullptr},
    {"DownlinkNonUEAssociatedNRPPaTransport", nullptr},
    {"DownlinkRANConfigurationTransfer", nullptr},
    {"DownlinkRANEarlyStatusTransfer", nullptr},
    {"DownlinkRANStatusTransfer", nullptr},
    {"DownlinkRIMInformationTransfer", nullptr},
    {"DownlinkUEAssociatedNRPPaTransport", nullptr},
    {"ErrorIndication", nullptr},
    {"HandoverNotify", nullptr},
    {"HandoverSuccess", nullptr},
    {"InitialUEMessage", nullptr},
    {"LocationReport", nullptr},
    {"LocationReportingControl", nullptr},
    {"LocationReportingFailureIndication", nullptr},
    {"MulticastGroupPaging", nullptr},
    {"NASNonDeliveryIndication", nullptr},
    {"OverloadStart", nullptr},
    {"OverloadStop", nullptr},
    {"Paging", nullptr},
    {"PDUSessionResourceNotify", nullptr},
    {"PrivateMessage", nullptr},
    {"PWSFailureIndication", nullptr},
    {"PWSRestartIndication", nullptr},
    {"RANCPRelocationIndication", nullptr},
    {"RANPagingRequest", nullptr},
    {"RerouteNASRequest", nullptr},
    {"RetrieveUEInformation", nullptr},
    {"RRCInactiveTransitionReport", nullptr},
    {"SecondaryRATDataUsageReport", nullptr},
    {"TimingSynchronisationStatusReport", nullptr},
    {"TraceFailureIndication", nullptr},
    {"TraceStart", nullptr},
    {"UEContextReleaseRequest", nullptr},
    {"UEInformationTransfer", nullptr},
    {"UERadioCapabilityInfoIndication", nullptr},
    {"UETNLABindingReleaseRequest", nullptr},
    {"UplinkNASTransport", nullptr},
    {"UplinkNonUEAssociatedNRPPaTransport", nullptr},
    {"UplinkRANConfigurationTransfer", nullptr},
    {"UplinkRANEarlyStatusTransfer", nullptr},
    {"UplinkRANStatusTransfer", nullptr},
    {"UplinkRIMInformationTransfer", nullptr},
    {"UplinkUEAssociatedNRPPaTransport", nullptr}};
const asn1::table::choice_desc ngap_elem_procs_o::init_msg_c::desc = {
    asn1::table::enum_int_type<ngap_elem_procs_o::init_msg_c::types>(),
    ngap_elem_procs_o::init_msg_c::types::nof_types,
    ngap_elem_procs_o::init_msg_c::types::nof_exts,
    ngap_elem_procs_o::init_msg_c::types::has_ext,
    true,
    choice_table_ngap_elem_procs_o_init_msg_c_alternatives,
    &asn1::table::choice_set<ngap_elem_procs_o::init_msg_c>,
    choice_table_ngap_elem_procs_o_init_msg_c_names,
    "ngap_elem_procs_o::init_msg_c",
    offsetof(ngap_elem_procs_o::init_msg_c, type_),
    offsetof(ngap_elem_procs_o::init_msg_c, c),
    false,
    nullptr,
    true};

void ngap_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, ngap_elem_procs_o::init_msg_c::desc);
}
amf_cfg_upd_s& ngap_elem_procs_o::init_msg_c::amf_cfg_upd()
{
  assert_choice_type(types::amf_cfg_upd, type_, "InitiatingMessage");
  return c.get<amf_cfg_upd_s>();
}
broadcast_session_mod_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_mod_request()
{
  assert_choice_type(types::broadcast_session_mod_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_mod_request_s>();
}
broadcast_session_release_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_release_request()
{
  assert_choice_type(types::broadcast_session_release_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_release_request_s>();
}
broadcast_session_setup_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_setup_request()
{
  assert_choice_type(types::broadcast_session_setup_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_setup_request_s>();
}
broadcast_session_transport_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_transport_request()
{
  assert_choice_type(types::broadcast_session_transport_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_transport_request_s>();
}
distribution_setup_request_s& ngap_elem_procs_o::init_msg_c::distribution_setup_request()
{
  assert_choice_type(types::distribution_setup_request, type_, "InitiatingMessage");
  return c.get<distribution_setup_request_s>();
}
distribution_release_request_s& ngap_elem_procs_o::init_msg_c::distribution_release_request()
{
  assert_choice_type(types::distribution_release_request, type_, "InitiatingMessage");
  return c.get<distribution_release_request_s>();
}
ho_cancel_s& ngap_elem_procs_o::init_msg_c::ho_cancel()
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
ho_required_s& ngap_elem_procs_o::init_msg_c::ho_required()
{
  assert_choice_type(types::ho_required, type_, "InitiatingMessage");
  return c.get<ho_required_s>();
}
ho_request_s& ngap_elem_procs_o::init_msg_c::ho_request()
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
init_context_setup_request_s& ngap_elem_procs_o::init_msg_c::init_context_setup_request()
{
  assert_choice_type(types::init_context_setup_request, type_, "InitiatingMessage");
  return c.get<init_context_setup_request_s>();
}
mt_communication_handling_request_s& ngap_elem_procs_o::init_msg_c::mt_communication_handling_request()
{
  assert_choice_type(types::mt_communication_handling_request, type_, "InitiatingMessage");
  return c.get<mt_communication_handling_request_s>();
}
multicast_session_activation_request_s& ngap_elem_procs_o::init_msg_c::multicast_session_activation_request()
{
  assert_choice_type(types::multicast_session_activation_request, type_, "InitiatingMessage");
  return c.get<multicast_session_activation_request_s>();
}
multicast_session_deactivation_request_s& ngap_elem_procs_o::init_msg_c::multicast_session_deactivation_request()
{
  assert_choice_type(types::multicast_session_deactivation_request, type_, "InitiatingMessage");
  return c.get<multicast_session_deactivation_request_s>();
}
multicast_session_upd_request_s& ngap_elem_procs_o::init_msg_c::multicast_session_upd_request()
{
  assert_choice_type(types::multicast_session_upd_request, type_, "InitiatingMessage");
  return c.get<multicast_session_upd_request_s>();
}
ng_reset_s& ngap_elem_procs_o::init_msg_c::ng_reset()
{
  assert_choice_type(types::ng_reset, type_, "InitiatingMessage");
  return c.get<ng_reset_s>();
}
ng_setup_request_s& ngap_elem_procs_o::init_msg_c::ng_setup_request()
{
  assert_choice_type(types::ng_setup_request, type_, "InitiatingMessage");
  return c.get<ng_setup_request_s>();
}
path_switch_request_s& ngap_elem_procs_o::init_msg_c::path_switch_request()
{
  assert_choice_type(types::path_switch_request, type_, "InitiatingMessage");
  return c.get<path_switch_request_s>();
}
pdu_session_res_modify_request_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_modify_request()
{
  assert_choice_type(types::pdu_session_res_modify_request, type_, "InitiatingMessage");
  return c.get<pdu_session_res_modify_request_s>();
}
pdu_session_res_modify_ind_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_modify_ind()
{
  assert_choice_type(types::pdu_session_res_modify_ind, type_, "InitiatingMessage");
  return c.get<pdu_session_res_modify_ind_s>();
}
pdu_session_res_release_cmd_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_release_cmd()
{
  assert_choice_type(types::pdu_session_res_release_cmd, type_, "InitiatingMessage");
  return c.get<pdu_session_res_release_cmd_s>();
}
pdu_session_res_setup_request_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_setup_request()
{
  assert_choice_type(types::pdu_session_res_setup_request, type_, "InitiatingMessage");
  return c.get<pdu_session_res_setup_request_s>();
}
pws_cancel_request_s& ngap_elem_procs_o::init_msg_c::pws_cancel_request()
{
  assert_choice_type(types::pws_cancel_request, type_, "InitiatingMessage");
  return c.get<pws_cancel_request_s>();
}
ran_cfg_upd_s& ngap_elem_procs_o::init_msg_c::ran_cfg_upd()
{
  assert_choice_type(types::ran_cfg_upd, type_, "InitiatingMessage");
  return c.get<ran_cfg_upd_s>();
}
timing_synchronisation_status_request_s& ngap_elem_procs_o::init_msg_c::timing_synchronisation_status_request()
{
  assert_choice_type(types::timing_synchronisation_status_request, type_, "InitiatingMessage");
  return c.get<timing_synchronisation_status_request_s>();
}
ue_context_mod_request_s& ngap_elem_procs_o::init_msg_c::ue_context_mod_request()
{
  assert_choice_type(types::ue_context_mod_request, type_, "InitiatingMessage");
  return c.get<ue_context_mod_request_s>();
}
ue_context_release_cmd_s& ngap_elem_procs_o::init_msg_c::ue_context_release_cmd()
{
  assert_choice_type(types::ue_context_release_cmd, type_, "InitiatingMessage");
  return c.get<ue_context_release_cmd_s>();
}
ue_context_resume_request_s& ngap_elem_procs_o::init_msg_c::ue_context_resume_request()
{
  assert_choice_type(types::ue_context_resume_request, type_, "InitiatingMessage");
  return c.get<ue_context_resume_request_s>();
}
ue_context_suspend_request_s& ngap_elem_procs_o::init_msg_c::ue_context_suspend_request()
{
  assert_choice_type(types::ue_context_suspend_request, type_, "InitiatingMessage");
  return c.get<ue_context_suspend_request_s>();
}
ue_radio_cap_check_request_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_check_request()
{
  assert_choice_type(types::ue_radio_cap_check_request, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_check_request_s>();
}
ue_radio_cap_id_map_request_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_id_map_request()
{
  assert_choice_type(types::ue_radio_cap_id_map_request, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_id_map_request_s>();
}
write_replace_warning_request_s& ngap_elem_procs_o::init_msg_c::write_replace_warning_request()
{
  assert_choice_type(types::write_replace_warning_request, type_, "InitiatingMessage");
  return c.get<write_replace_warning_request_s>();
}
amf_cp_relocation_ind_s& ngap_elem_procs_o::init_msg_c::amf_cp_relocation_ind()
{
  assert_choice_type(types::amf_cp_relocation_ind, type_, "InitiatingMessage");
  return c.get<amf_cp_relocation_ind_s>();
}
amf_status_ind_s& ngap_elem_procs_o::init_msg_c::amf_status_ind()
{
  assert_choice_type(types::amf_status_ind, type_, "InitiatingMessage");
  return c.get<amf_status_ind_s>();
}
broadcast_session_release_required_s& ngap_elem_procs_o::init_msg_c::broadcast_session_release_required()
{
  assert_choice_type(types::broadcast_session_release_required, type_, "InitiatingMessage");
  return c.get<broadcast_session_release_required_s>();
}
cell_traffic_trace_s& ngap_elem_procs_o::init_msg_c::cell_traffic_trace()
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
conn_establishment_ind_s& ngap_elem_procs_o::init_msg_c::conn_establishment_ind()
{
  assert_choice_type(types::conn_establishment_ind, type_, "InitiatingMessage");
  return c.get<conn_establishment_ind_s>();
}
deactiv_trace_s& ngap_elem_procs_o::init_msg_c::deactiv_trace()
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
dl_nas_transport_s& ngap_elem_procs_o::init_msg_c::dl_nas_transport()
{
  assert_choice_type(types::dl_nas_transport, type_, "InitiatingMessage");
  return c.get<dl_nas_transport_s>();
}
dl_non_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::dl_non_ue_associated_nrppa_transport()
{
  assert_choice_type(types::dl_non_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<dl_non_ue_associated_nrppa_transport_s>();
}
dl_ran_cfg_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_cfg_transfer()
{
  assert_choice_type(types::dl_ran_cfg_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_cfg_transfer_s>();
}
dl_ran_early_status_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_early_status_transfer()
{
  assert_choice_type(types::dl_ran_early_status_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_early_status_transfer_s>();
}
dl_ran_status_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_status_transfer()
{
  assert_choice_type(types::dl_ran_status_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_status_transfer_s>();
}
dl_rim_info_transfer_s& ngap_elem_procs_o::init_msg_c::dl_rim_info_transfer()
{
  assert_choice_type(types::dl_rim_info_transfer, type_, "InitiatingMessage");
  return c.get<dl_rim_info_transfer_s>();
}
dl_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::dl_ue_associated_nrppa_transport()
{
  assert_choice_type(types::dl_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<dl_ue_associated_nrppa_transport_s>();
}
error_ind_s& ngap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
ho_notify_s& ngap_elem_procs_o::init_msg_c::ho_notify()
{
  assert_choice_type(types::ho_notify, type_, "InitiatingMessage");
  return c.get<ho_notify_s>();
}
ho_success_s& ngap_elem_procs_o::init_msg_c::ho_success()
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
init_ue_msg_s& ngap_elem_procs_o::init_msg_c::init_ue_msg()
{
  assert_choice_type(types::init_ue_msg, type_, "InitiatingMessage");
  return c.get<init_ue_msg_s>();
}
location_report_s& ngap_elem_procs_o::init_msg_c::location_report()
{
  assert_choice_type(types::location_report, type_, "InitiatingMessage");
  return c.get<location_report_s>();
}
location_report_ctrl_s& ngap_elem_procs_o::init_msg_c::location_report_ctrl()
{
  assert_choice_type(types::location_report_ctrl, type_, "InitiatingMessage");
  return c.get<location_report_ctrl_s>();
}
location_report_fail_ind_s& ngap_elem_procs_o::init_msg_c::location_report_fail_ind()
{
  assert_choice_type(types::location_report_fail_ind, type_, "InitiatingMessage");
  return c.get<location_report_fail_ind_s>();
}
multicast_group_paging_s& ngap_elem_procs_o::init_msg_c::multicast_group_paging()
{
  assert_choice_type(types::multicast_group_paging, type_, "InitiatingMessage");
  return c.get<multicast_group_paging_s>();
}
nas_non_delivery_ind_s& ngap_elem_procs_o::init_msg_c::nas_non_delivery_ind()
{
  assert_choice_type(types::nas_non_delivery_ind, type_, "InitiatingMessage");
  return c.get<nas_non_delivery_ind_s>();
}
overload_start_s& ngap_elem_procs_o::init_msg_c::overload_start()
{
  assert_choice_type(types::overload_start, type_, "InitiatingMessage");
  return c.get<overload_start_s>();
}
overload_stop_s& ngap_elem_procs_o::init_msg_c::overload_stop()
{
  assert_choice_type(types::overload_stop, type_, "InitiatingMessage");
  return c.get<overload_stop_s>();
}
paging_s& ngap_elem_procs_o::init_msg_c::paging()
{
  assert_choice_type(types::paging, type_, "InitiatingMessage");
  return c.get<paging_s>();
}
pdu_session_res_notify_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_notify()
{
  assert_choice_type(types::pdu_session_res_notify, type_, "InitiatingMessage");
  return c.get<pdu_session_res_notify_s>();
}
private_msg_s& ngap_elem_procs_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
pws_fail_ind_s& ngap_elem_procs_o::init_msg_c::pws_fail_ind()
{
  assert_choice_type(types::pws_fail_ind, type_, "InitiatingMessage");
  return c.get<pws_fail_ind_s>();
}
pws_restart_ind_s& ngap_elem_procs_o::init_msg_c::pws_restart_ind()
{
  assert_choice_type(types::pws_restart_ind, type_, "InitiatingMessage");
  return c.get<pws_restart_ind_s>();
}
ran_cp_relocation_ind_s& ngap_elem_procs_o::init_msg_c::ran_cp_relocation_ind()
{
  assert_choice_type(types::ran_cp_relocation_ind, type_, "InitiatingMessage");
  return c.get<ran_cp_relocation_ind_s>();
}
ran_paging_request_s& ngap_elem_procs_o::init_msg_c::ran_paging_request()
{
  assert_choice_type(types::ran_paging_request, type_, "InitiatingMessage");
  return c.get<ran_paging_request_s>();
}
reroute_nas_request_s& ngap_elem_procs_o::init_msg_c::reroute_nas_request()
{
  assert_choice_type(types::reroute_nas_request, type_, "InitiatingMessage");
  return c.get<reroute_nas_request_s>();
}
retrieve_ue_info_s& ngap_elem_procs_o::init_msg_c::retrieve_ue_info()
{
  assert_choice_type(types::retrieve_ue_info, type_, "InitiatingMessage");
  return c.get<retrieve_ue_info_s>();
}
rrc_inactive_transition_report_s& ngap_elem_procs_o::init_msg_c::rrc_inactive_transition_report()
{
  assert_choice_type(types::rrc_inactive_transition_report, type_, "InitiatingMessage");
  return c.get<rrc_inactive_transition_report_s>();
}
secondary_rat_data_usage_report_s& ngap_elem_procs_o::init_msg_c::secondary_rat_data_usage_report()
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
timing_synchronisation_status_report_s& ngap_elem_procs_o::init_msg_c::timing_synchronisation_status_report()
{
  assert_choice_type(types::timing_synchronisation_status_report, type_, "InitiatingMessage");
  return c.get<timing_synchronisation_status_report_s>();
}
trace_fail_ind_s& ngap_elem_procs_o::init_msg_c::trace_fail_ind()
{
  assert_choice_type(types::trace_fail_ind, type_, "InitiatingMessage");
  return c.get<trace_fail_ind_s>();
}
trace_start_s& ngap_elem_procs_o::init_msg_c::trace_start()
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
ue_context_release_request_s& ngap_elem_procs_o::init_msg_c::ue_context_release_request()
{
  assert_choice_type(types::ue_context_release_request, type_, "InitiatingMessage");
  return c.get<ue_context_release_request_s>();
}
ue_info_transfer_s& ngap_elem_procs_o::init_msg_c::ue_info_transfer()
{
  assert_choice_type(types::ue_info_transfer, type_, "InitiatingMessage");
  return c.get<ue_info_transfer_s>();
}
ue_radio_cap_info_ind_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_info_ind()
{
  assert_choice_type(types::ue_radio_cap_info_ind, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_info_ind_s>();
}
ue_tnla_binding_release_request_s& ngap_elem_procs_o::init_msg_c::ue_tnla_binding_release_request()
{
  assert_choice_type(types::ue_tnla_binding_release_request, type_, "InitiatingMessage");
  return c.get<ue_tnla_binding_release_request_s>();
}
ul_nas_transport_s& ngap_elem_procs_o::init_msg_c::ul_nas_transport()
{
  assert_choice_type(types::ul_nas_transport, type_, "InitiatingMessage");
  return c.get<ul_nas_transport_s>();
}
ul_non_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::ul_non_ue_associated_nrppa_transport()
{
  assert_choice_type(types::ul_non_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<ul_non_ue_associated_nrppa_transport_s>();
}
ul_ran_cfg_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_cfg_transfer()
{
  assert_choice_type(types::ul_ran_cfg_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_cfg_transfer_s>();
}
ul_ran_early_status_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_early_status_transfer()
{
  assert_choice_type(types::ul_ran_early_status_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_early_status_transfer_s>();
}
ul_ran_status_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_status_transfer()
{
  assert_choice_type(types::ul_ran_status_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_status_transfer_s>();
}
ul_rim_info_transfer_s& ngap_elem_procs_o::init_msg_c::ul_rim_info_transfer()
{
  assert_choice_type(types::ul_rim_info_transfer, type_, "InitiatingMessage");
  return c.get<ul_rim_info_transfer_s>();
}
ul_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::ul_ue_associated_nrppa_transport()
{
  assert_choice_type(types::ul_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<ul_ue_associated_nrppa_transport_s>();
}
const amf_cfg_upd_s& ngap_elem_procs_o::init_msg_c::amf_cfg_upd() const
{
  assert_choice_type(types::amf_cfg_upd, type_, "InitiatingMessage");
  return c.get<amf_cfg_upd_s>();
}
const broadcast_session_mod_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_mod_request() const
{
  assert_choice_type(types::broadcast_session_mod_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_mod_request_s>();
}
const broadcast_session_release_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_release_request() const
{
  assert_choice_type(types::broadcast_session_release_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_release_request_s>();
}
const broadcast_session_setup_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_setup_request() const
{
  assert_choice_type(types::broadcast_session_setup_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_setup_request_s>();
}
const broadcast_session_transport_request_s& ngap_elem_procs_o::init_msg_c::broadcast_session_transport_request() const
{
  assert_choice_type(types::broadcast_session_transport_request, type_, "InitiatingMessage");
  return c.get<broadcast_session_transport_request_s>();
}
const distribution_setup_request_s& ngap_elem_procs_o::init_msg_c::distribution_setup_request() const
{
  assert_choice_type(types::distribution_setup_request, type_, "InitiatingMessage");
  return c.get<distribution_setup_request_s>();
}
const distribution_release_request_s& ngap_elem_procs_o::init_msg_c::distribution_release_request() const
{
  assert_choice_type(types::distribution_release_request, type_, "InitiatingMessage");
  return c.get<distribution_release_request_s>();
}
const ho_cancel_s& ngap_elem_procs_o::init_msg_c::ho_cancel() const
{
  assert_choice_type(types::ho_cancel, type_, "InitiatingMessage");
  return c.get<ho_cancel_s>();
}
const ho_required_s& ngap_elem_procs_o::init_msg_c::ho_required() const
{
  assert_choice_type(types::ho_required, type_, "InitiatingMessage");
  return c.get<ho_required_s>();
}
const ho_request_s& ngap_elem_procs_o::init_msg_c::ho_request() const
{
  assert_choice_type(types::ho_request, type_, "InitiatingMessage");
  return c.get<ho_request_s>();
}
const init_context_setup_request_s& ngap_elem_procs_o::init_msg_c::init_context_setup_request() const
{
  assert_choice_type(types::init_context_setup_request, type_, "InitiatingMessage");
  return c.get<init_context_setup_request_s>();
}
const mt_communication_handling_request_s& ngap_elem_procs_o::init_msg_c::mt_communication_handling_request() const
{
  assert_choice_type(types::mt_communication_handling_request, type_, "InitiatingMessage");
  return c.get<mt_communication_handling_request_s>();
}
const multicast_session_activation_request_s&
ngap_elem_procs_o::init_msg_c::multicast_session_activation_request() const
{
  assert_choice_type(types::multicast_session_activation_request, type_, "InitiatingMessage");
  return c.get<multicast_session_activation_request_s>();
}
const multicast_session_deactivation_request_s&
ngap_elem_procs_o::init_msg_c::multicast_session_deactivation_request() const
{
  assert_choice_type(types::multicast_session_deactivation_request, type_, "InitiatingMessage");
  return c.get<multicast_session_deactivation_request_s>();
}
const multicast_session_upd_request_s& ngap_elem_procs_o::init_msg_c::multicast_session_upd_request() const
{
  assert_choice_type(types::multicast_session_upd_request, type_, "InitiatingMessage");
  return c.get<multicast_session_upd_request_s>();
}
const ng_reset_s& ngap_elem_procs_o::init_msg_c::ng_reset() const
{
  assert_choice_type(types::ng_reset, type_, "InitiatingMessage");
  return c.get<ng_reset_s>();
}
const ng_setup_request_s& ngap_elem_procs_o::init_msg_c::ng_setup_request() const
{
  assert_choice_type(types::ng_setup_request, type_, "InitiatingMessage");
  return c.get<ng_setup_request_s>();
}
const path_switch_request_s& ngap_elem_procs_o::init_msg_c::path_switch_request() const
{
  assert_choice_type(types::path_switch_request, type_, "InitiatingMessage");
  return c.get<path_switch_request_s>();
}
const pdu_session_res_modify_request_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_modify_request() const
{
  assert_choice_type(types::pdu_session_res_modify_request, type_, "InitiatingMessage");
  return c.get<pdu_session_res_modify_request_s>();
}
const pdu_session_res_modify_ind_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_modify_ind() const
{
  assert_choice_type(types::pdu_session_res_modify_ind, type_, "InitiatingMessage");
  return c.get<pdu_session_res_modify_ind_s>();
}
const pdu_session_res_release_cmd_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_release_cmd() const
{
  assert_choice_type(types::pdu_session_res_release_cmd, type_, "InitiatingMessage");
  return c.get<pdu_session_res_release_cmd_s>();
}
const pdu_session_res_setup_request_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_setup_request() const
{
  assert_choice_type(types::pdu_session_res_setup_request, type_, "InitiatingMessage");
  return c.get<pdu_session_res_setup_request_s>();
}
const pws_cancel_request_s& ngap_elem_procs_o::init_msg_c::pws_cancel_request() const
{
  assert_choice_type(types::pws_cancel_request, type_, "InitiatingMessage");
  return c.get<pws_cancel_request_s>();
}
const ran_cfg_upd_s& ngap_elem_procs_o::init_msg_c::ran_cfg_upd() const
{
  assert_choice_type(types::ran_cfg_upd, type_, "InitiatingMessage");
  return c.get<ran_cfg_upd_s>();
}
const timing_synchronisation_status_request_s&
ngap_elem_procs_o::init_msg_c::timing_synchronisation_status_request() const
{
  assert_choice_type(types::timing_synchronisation_status_request, type_, "InitiatingMessage");
  return c.get<timing_synchronisation_status_request_s>();
}
const ue_context_mod_request_s& ngap_elem_procs_o::init_msg_c::ue_context_mod_request() const
{
  assert_choice_type(types::ue_context_mod_request, type_, "InitiatingMessage");
  return c.get<ue_context_mod_request_s>();
}
const ue_context_release_cmd_s& ngap_elem_procs_o::init_msg_c::ue_context_release_cmd() const
{
  assert_choice_type(types::ue_context_release_cmd, type_, "InitiatingMessage");
  return c.get<ue_context_release_cmd_s>();
}
const ue_context_resume_request_s& ngap_elem_procs_o::init_msg_c::ue_context_resume_request() const
{
  assert_choice_type(types::ue_context_resume_request, type_, "InitiatingMessage");
  return c.get<ue_context_resume_request_s>();
}
const ue_context_suspend_request_s& ngap_elem_procs_o::init_msg_c::ue_context_suspend_request() const
{
  assert_choice_type(types::ue_context_suspend_request, type_, "InitiatingMessage");
  return c.get<ue_context_suspend_request_s>();
}
const ue_radio_cap_check_request_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_check_request() const
{
  assert_choice_type(types::ue_radio_cap_check_request, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_check_request_s>();
}
const ue_radio_cap_id_map_request_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_id_map_request() const
{
  assert_choice_type(types::ue_radio_cap_id_map_request, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_id_map_request_s>();
}
const write_replace_warning_request_s& ngap_elem_procs_o::init_msg_c::write_replace_warning_request() const
{
  assert_choice_type(types::write_replace_warning_request, type_, "InitiatingMessage");
  return c.get<write_replace_warning_request_s>();
}
const amf_cp_relocation_ind_s& ngap_elem_procs_o::init_msg_c::amf_cp_relocation_ind() const
{
  assert_choice_type(types::amf_cp_relocation_ind, type_, "InitiatingMessage");
  return c.get<amf_cp_relocation_ind_s>();
}
const amf_status_ind_s& ngap_elem_procs_o::init_msg_c::amf_status_ind() const
{
  assert_choice_type(types::amf_status_ind, type_, "InitiatingMessage");
  return c.get<amf_status_ind_s>();
}
const broadcast_session_release_required_s& ngap_elem_procs_o::init_msg_c::broadcast_session_release_required() const
{
  assert_choice_type(types::broadcast_session_release_required, type_, "InitiatingMessage");
  return c.get<broadcast_session_release_required_s>();
}
const cell_traffic_trace_s& ngap_elem_procs_o::init_msg_c::cell_traffic_trace() const
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
const conn_establishment_ind_s& ngap_elem_procs_o::init_msg_c::conn_establishment_ind() const
{
  assert_choice_type(types::conn_establishment_ind, type_, "InitiatingMessage");
  return c.get<conn_establishment_ind_s>();
}
const deactiv_trace_s& ngap_elem_procs_o::init_msg_c::deactiv_trace() const
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
const dl_nas_transport_s& ngap_elem_procs_o::init_msg_c::dl_nas_transport() const
{
  assert_choice_type(types::dl_nas_transport, type_, "InitiatingMessage");
  return c.get<dl_nas_transport_s>();
}
const dl_non_ue_associated_nrppa_transport_s&
ngap_elem_procs_o::init_msg_c::dl_non_ue_associated_nrppa_transport() const
{
  assert_choice_type(types::dl_non_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<dl_non_ue_associated_nrppa_transport_s>();
}
const dl_ran_cfg_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_cfg_transfer() const
{
  assert_choice_type(types::dl_ran_cfg_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_cfg_transfer_s>();
}
const dl_ran_early_status_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_early_status_transfer() const
{
  assert_choice_type(types::dl_ran_early_status_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_early_status_transfer_s>();
}
const dl_ran_status_transfer_s& ngap_elem_procs_o::init_msg_c::dl_ran_status_transfer() const
{
  assert_choice_type(types::dl_ran_status_transfer, type_, "InitiatingMessage");
  return c.get<dl_ran_status_transfer_s>();
}
const dl_rim_info_transfer_s& ngap_elem_procs_o::init_msg_c::dl_rim_info_transfer() const
{
  assert_choice_type(types::dl_rim_info_transfer, type_, "InitiatingMessage");
  return c.get<dl_rim_info_transfer_s>();
}
const dl_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::dl_ue_associated_nrppa_transport() const
{
  assert_choice_type(types::dl_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<dl_ue_associated_nrppa_transport_s>();
}
const error_ind_s& ngap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const ho_notify_s& ngap_elem_procs_o::init_msg_c::ho_notify() const
{
  assert_choice_type(types::ho_notify, type_, "InitiatingMessage");
  return c.get<ho_notify_s>();
}
const ho_success_s& ngap_elem_procs_o::init_msg_c::ho_success() const
{
  assert_choice_type(types::ho_success, type_, "InitiatingMessage");
  return c.get<ho_success_s>();
}
const init_ue_msg_s& ngap_elem_procs_o::init_msg_c::init_ue_msg() const
{
  assert_choice_type(types::init_ue_msg, type_, "InitiatingMessage");
  return c.get<init_ue_msg_s>();
}
const location_report_s& ngap_elem_procs_o::init_msg_c::location_report() const
{
  assert_choice_type(types::location_report, type_, "InitiatingMessage");
  return c.get<location_report_s>();
}
const location_report_ctrl_s& ngap_elem_procs_o::init_msg_c::location_report_ctrl() const
{
  assert_choice_type(types::location_report_ctrl, type_, "InitiatingMessage");
  return c.get<location_report_ctrl_s>();
}
const location_report_fail_ind_s& ngap_elem_procs_o::init_msg_c::location_report_fail_ind() const
{
  assert_choice_type(types::location_report_fail_ind, type_, "InitiatingMessage");
  return c.get<location_report_fail_ind_s>();
}
const multicast_group_paging_s& ngap_elem_procs_o::init_msg_c::multicast_group_paging() const
{
  assert_choice_type(types::multicast_group_paging, type_, "InitiatingMessage");
  return c.get<multicast_group_paging_s>();
}
const nas_non_delivery_ind_s& ngap_elem_procs_o::init_msg_c::nas_non_delivery_ind() const
{
  assert_choice_type(types::nas_non_delivery_ind, type_, "InitiatingMessage");
  return c.get<nas_non_delivery_ind_s>();
}
const overload_start_s& ngap_elem_procs_o::init_msg_c::overload_start() const
{
  assert_choice_type(types::overload_start, type_, "InitiatingMessage");
  return c.get<overload_start_s>();
}
const overload_stop_s& ngap_elem_procs_o::init_msg_c::overload_stop() const
{
  assert_choice_type(types::overload_stop, type_, "InitiatingMessage");
  return c.get<overload_stop_s>();
}
const paging_s& ngap_elem_procs_o::init_msg_c::paging() const
{
  assert_choice_type(types::paging, type_, "InitiatingMessage");
  return c.get<paging_s>();
}
const pdu_session_res_notify_s& ngap_elem_procs_o::init_msg_c::pdu_session_res_notify() const
{
  assert_choice_type(types::pdu_session_res_notify, type_, "InitiatingMessage");
  return c.get<pdu_session_res_notify_s>();
}
const private_msg_s& ngap_elem_procs_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const pws_fail_ind_s& ngap_elem_procs_o::init_msg_c::pws_fail_ind() const
{
  assert_choice_type(types::pws_fail_ind, type_, "InitiatingMessage");
  return c.get<pws_fail_ind_s>();
}
const pws_restart_ind_s& ngap_elem_procs_o::init_msg_c::pws_restart_ind() const
{
  assert_choice_type(types::pws_restart_ind, type_, "InitiatingMessage");
  return c.get<pws_restart_ind_s>();
}
const ran_cp_relocation_ind_s& ngap_elem_procs_o::init_msg_c::ran_cp_relocation_ind() const
{
  assert_choice_type(types::ran_cp_relocation_ind, type_, "InitiatingMessage");
  return c.get<ran_cp_relocation_ind_s>();
}
const ran_paging_request_s& ngap_elem_procs_o::init_msg_c::ran_paging_request() const
{
  assert_choice_type(types::ran_paging_request, type_, "InitiatingMessage");
  return c.get<ran_paging_request_s>();
}
const reroute_nas_request_s& ngap_elem_procs_o::init_msg_c::reroute_nas_request() const
{
  assert_choice_type(types::reroute_nas_request, type_, "InitiatingMessage");
  return c.get<reroute_nas_request_s>();
}
const retrieve_ue_info_s& ngap_elem_procs_o::init_msg_c::retrieve_ue_info() const
{
  assert_choice_type(types::retrieve_ue_info, type_, "InitiatingMessage");
  return c.get<retrieve_ue_info_s>();
}
const rrc_inactive_transition_report_s& ngap_elem_procs_o::init_msg_c::rrc_inactive_transition_report() const
{
  assert_choice_type(types::rrc_inactive_transition_report, type_, "InitiatingMessage");
  return c.get<rrc_inactive_transition_report_s>();
}
const secondary_rat_data_usage_report_s& ngap_elem_procs_o::init_msg_c::secondary_rat_data_usage_report() const
{
  assert_choice_type(types::secondary_rat_data_usage_report, type_, "InitiatingMessage");
  return c.get<secondary_rat_data_usage_report_s>();
}
const timing_synchronisation_status_report_s&
ngap_elem_procs_o::init_msg_c::timing_synchronisation_status_report() const
{
  assert_choice_type(types::timing_synchronisation_status_report, type_, "InitiatingMessage");
  return c.get<timing_synchronisation_status_report_s>();
}
const trace_fail_ind_s& ngap_elem_procs_o::init_msg_c::trace_fail_ind() const
{
  assert_choice_type(types::trace_fail_ind, type_, "InitiatingMessage");
  return c.get<trace_fail_ind_s>();
}
const trace_start_s& ngap_elem_procs_o::init_msg_c::trace_start() const
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
const ue_context_release_request_s& ngap_elem_procs_o::init_msg_c::ue_context_release_request() const
{
  assert_choice_type(types::ue_context_release_request, type_, "InitiatingMessage");
  return c.get<ue_context_release_request_s>();
}
const ue_info_transfer_s& ngap_elem_procs_o::init_msg_c::ue_info_transfer() const
{
  assert_choice_type(types::ue_info_transfer, type_, "InitiatingMessage");
  return c.get<ue_info_transfer_s>();
}
const ue_radio_cap_info_ind_s& ngap_elem_procs_o::init_msg_c::ue_radio_cap_info_ind() const
{
  assert_choice_type(types::ue_radio_cap_info_ind, type_, "InitiatingMessage");
  return c.get<ue_radio_cap_info_ind_s>();
}
const ue_tnla_binding_release_request_s& ngap_elem_procs_o::init_msg_c::ue_tnla_binding_release_request() const
{
  assert_choice_type(types::ue_tnla_binding_release_request, type_, "InitiatingMessage");
  return c.get<ue_tnla_binding_release_request_s>();
}
const ul_nas_transport_s& ngap_elem_procs_o::init_msg_c::ul_nas_transport() const
{
  assert_choice_type(types::ul_nas_transport, type_, "InitiatingMessage");
  return c.get<ul_nas_transport_s>();
}
const ul_non_ue_associated_nrppa_transport_s&
ngap_elem_procs_o::init_msg_c::ul_non_ue_associated_nrppa_transport() const
{
  assert_choice_type(types::ul_non_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<ul_non_ue_associated_nrppa_transport_s>();
}
const ul_ran_cfg_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_cfg_transfer() const
{
  assert_choice_type(types::ul_ran_cfg_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_cfg_transfer_s>();
}
const ul_ran_early_status_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_early_status_transfer() const
{
  assert_choice_type(types::ul_ran_early_status_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_early_status_transfer_s>();
}
const ul_ran_status_transfer_s& ngap_elem_procs_o::init_msg_c::ul_ran_status_transfer() const
{
  assert_choice_type(types::ul_ran_status_transfer, type_, "InitiatingMessage");
  return c.get<ul_ran_status_transfer_s>();
}
const ul_rim_info_transfer_s& ngap_elem_procs_o::init_msg_c::ul_rim_info_transfer() const
{
  assert_choice_type(types::ul_rim_info_transfer, type_, "InitiatingMessage");
  return c.get<ul_rim_info_transfer_s>();
}
const ul_ue_associated_nrppa_transport_s& ngap_elem_procs_o::init_msg_c::ul_ue_associated_nrppa_transport() const
{
  assert_choice_type(types::ul_ue_associated_nrppa_transport, type_, "InitiatingMessage");
  return c.get<ul_ue_associated_nrppa_transport_s>();
}

static const char*           enum_names_ngap_elem_procs_o_init_msg_c_types_values[] = {"AMFConfigurationUpdate",
                                                                                       "BroadcastSessionModificationRequest",
                                                                                       "BroadcastSessionReleaseRequest",
                                                                                       "BroadcastSessionSetupRequest",
                                                                                       "BroadcastSessionTransportRequest",
                                                                                       "DistributionSetupRequest",
                                                                                       "DistributionReleaseRequest",
                                                                                       "HandoverCancel",
                                                                                       "HandoverRequired",
                                                                                       "HandoverRequest",
                                                                                       "InitialContextSetupRequest",
                                                                                       "MTCommunicationHandlingRequest",
                                                                                       "MulticastSessionActivationRequest",
                                                                                       "MulticastSessionDeactivationRequest",
                                                                                       "MulticastSessionUpdateRequest",
                                                                                       "NGReset",
                                                                                       "NGSetupRequest",
                                                                                       "PathSwitchRequest",
                                                                                       "PDUSessionResourceModifyRequest",
                                                                                       "PDUSessionResourceModifyIndication",
                                                                                       "PDUSessionResourceReleaseCommand",
                                                                                       "PDUSessionResourceSetupRequest",
                                                                                       "PWSCancelRequest",
                                                                                       "RANConfigurationUpdate",
                                                                                       "TimingSynchronisationStatusRequest",
                                                                                       "UEContextModificationRequest",
                                                                                       "UEContextReleaseCommand",
                                                                                       "UEContextResumeRequest",
                                                                                       "UEContextSuspendRequest",
                                                                                       "UERadioCapabilityCheckRequest",
                                                                                       "UERadioCapabilityIDMappingRequest",
                                                                                       "WriteReplaceWarningRequest",
                                                                                       "AMFCPRelocationIndication",
                                                                                       "AMFStatusIndication",
                                                                                       "BroadcastSessionReleaseRequired",
                                                                                       "CellTrafficTrace",
                                                                                       "ConnectionEstablishmentIndication",
                                                                                       "DeactivateTrace",
                                                                                       "DownlinkNASTransport",
                                                                                       "DownlinkNonUEAssociatedNRPPaTransport",
                                                                                       "DownlinkRANConfigurationTransfer",
                                                                                       "DownlinkRANEarlyStatusTransfer",
                                                                                       "DownlinkRANStatusTransfer",
                                                                                       "DownlinkRIMInformationTransfer",
                                                                                       "DownlinkUEAssociatedNRPPaTransport",
                                                                                       "ErrorIndication",
                                                                                       "HandoverNotify",
                                                                                       "HandoverSuccess",
                                                                                       "InitialUEMessage",
                                                                                       "LocationReport",
                                                                                       "LocationReportingControl",
                                                                                       "LocationReportingFailureIndication",
                                                                                       "MulticastGroupPaging",
                                                                                       "NASNonDeliveryIndication",
                                                                                       "OverloadStart",
                                                                                       "OverloadStop",
                                                                                       "Paging",
                                                                                       "PDUSessionResourceNotify",
                                                                                       "PrivateMessage",
                                                                                       "PWSFailureIndication",
                                                                                       "PWSRestartIndication",
                                                                                       "RANCPRelocationIndication",
                                                                                       "RANPagingRequest",
                                                                                       "RerouteNASRequest",
                                                                                       "RetrieveUEInformation",
                                                                                       "RRCInactiveTransitionReport",
                                                                                       "SecondaryRATDataUsageReport",
                                                                                       "TimingSynchronisationStatusReport",
                                                                                       "TraceFailureIndication",
                                                                                       "TraceStart",
                                                                                       "UEContextReleaseRequest",
                                                                                       "UEInformationTransfer",
                                                                                       "UERadioCapabilityInfoIndication",
                                                                                       "UETNLABindingReleaseRequest",
                                                                                       "UplinkNASTransport",
                                                                                       "UplinkNonUEAssociatedNRPPaTransport",
                                                                                       "UplinkRANConfigurationTransfer",
                                                                                       "UplinkRANEarlyStatusTransfer",
                                                                                       "UplinkRANStatusTransfer",
                                                                                       "UplinkRIMInformationTransfer",
                                                                                       "UplinkUEAssociatedNRPPaTransport"};
const asn1::table::enum_desc ngap_elem_procs_o::init_msg_c::types_opts::desc        = {
    enum_names_ngap_elem_procs_o_init_msg_c_types_values,
    81,
    "ngap_elem_procs_o::init_msg_c::types"};

// SuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ngap_elem_procs_o_successful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<amf_cfg_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_mod_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_release_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_transport_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<distribution_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<distribution_release_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_cancel_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ho_cmd_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<init_context_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mt_communication_handling_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_session_activation_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_session_deactivation_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_session_upd_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ng_reset_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ng_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<path_switch_request_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdu_session_res_modify_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdu_session_res_modify_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdu_session_res_release_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdu_session_res_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pws_cancel_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ran_cfg_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<timing_synchronisation_status_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_mod_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_release_complete_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_resume_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_suspend_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_radio_cap_check_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_radio_cap_id_map_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<write_replace_warning_resp_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_ngap_elem_procs_o_successful_outcome_c_names[] = {
    {"AMFConfigurationUpdateAcknowledge", nullptr},
    {"BroadcastSessionModificationResponse", nullptr},
    {"BroadcastSessionReleaseResponse", nullptr},
    {"BroadcastSessionSetupResponse", nullptr},
    {"BroadcastSessionTransportResponse", nullptr},
    {"DistributionSetupResponse", nullptr},
    {"DistributionReleaseResponse", nullptr},
    {"HandoverCancelAcknowledge", nullptr},
    {"HandoverCommand", nullptr},
    {"HandoverRequestAcknowledge", nullptr},
    {"InitialContextSetupResponse", nullptr},
    {"MTCommunicationHandlingResponse", nullptr},
    {"MulticastSessionActivationResponse", nullptr},
    {"MulticastSessionDeactivationResponse", nullptr},
    {"MulticastSessionUpdateResponse", nullptr},
    {"NGResetAcknowledge", nullptr},
    {"NGSetupResponse", nullptr},
    {"PathSwitchRequestAcknowledge", nullptr},
    {"PDUSessionResourceModifyResponse", nullptr},
    {"PDUSessionResourceModifyConfirm", nullptr},
    {"PDUSessionResourceReleaseResponse", nullptr},
    {"PDUSessionResourceSetupResponse", nullptr},
    {"PWSCancelResponse", nullptr},
    {"RANConfigurationUpdateAcknowledge", nullptr},
    {"TimingSynchronisationStatusResponse", nullptr},
    {"UEContextModificationResponse", nullptr},
    {"UEContextReleaseComplete", nullptr},
    {"UEContextResumeResponse", nullptr},
    {"UEContextSuspendResponse", nullptr},
    {"UERadioCapabilityCheckResponse", nullptr},
    {"UERadioCapabilityIDMappingResponse", nullptr},
    {"WriteReplaceWarningResponse", nullptr}};
const asn1::table::choice_desc ngap_elem_procs_o::successful_outcome_c::desc = {
    asn1::table::enum_int_type<ngap_elem_procs_o::successful_outcome_c::types>(),
    ngap_elem_procs_o::successful_outcome_c::types::nof_types,
    ngap_elem_procs_o::successful_outcome_c::types::nof_exts,
    ngap_elem_procs_o::successful_outcome_c::types::has_ext,
    true,
    choice_table_ngap_elem_procs_o_successful_outcome_c_alternatives,
    &asn1::table::choice_set<ngap_elem_procs_o::successful_outcome_c>,
    choice_table_ngap_elem_procs_o_successful_outcome_c_names,
    "ngap_elem_procs_o::successful_outcome_c",
    offsetof(ngap_elem_procs_o::successful_outcome_c, type_),
    offsetof(ngap_elem_procs_o::successful_outcome_c, c),
    false,
    nullptr,
    true};

void ngap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, ngap_elem_procs_o::successful_outcome_c::desc);
}
amf_cfg_upd_ack_s& ngap_elem_procs_o::successful_outcome_c::amf_cfg_upd_ack()
{
  assert_choice_type(types::amf_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<amf_cfg_upd_ack_s>();
}
broadcast_session_mod_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_mod_resp()
{
  assert_choice_type(types::broadcast_session_mod_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_mod_resp_s>();
}
broadcast_session_release_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_release_resp()
{
  assert_choice_type(types::broadcast_session_release_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_release_resp_s>();
}
broadcast_session_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_setup_resp()
{
  assert_choice_type(types::broadcast_session_setup_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_setup_resp_s>();
}
broadcast_session_transport_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_transport_resp()
{
  assert_choice_type(types::broadcast_session_transport_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_transport_resp_s>();
}
distribution_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::distribution_setup_resp()
{
  assert_choice_type(types::distribution_setup_resp, type_, "SuccessfulOutcome");
  return c.get<distribution_setup_resp_s>();
}
distribution_release_resp_s& ngap_elem_procs_o::successful_outcome_c::distribution_release_resp()
{
  assert_choice_type(types::distribution_release_resp, type_, "SuccessfulOutcome");
  return c.get<distribution_release_resp_s>();
}
ho_cancel_ack_s& ngap_elem_procs_o::successful_outcome_c::ho_cancel_ack()
{
  assert_choice_type(types::ho_cancel_ack, type_, "SuccessfulOutcome");
  return c.get<ho_cancel_ack_s>();
}
ho_cmd_s& ngap_elem_procs_o::successful_outcome_c::ho_cmd()
{
  assert_choice_type(types::ho_cmd, type_, "SuccessfulOutcome");
  return c.get<ho_cmd_s>();
}
ho_request_ack_s& ngap_elem_procs_o::successful_outcome_c::ho_request_ack()
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
init_context_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::init_context_setup_resp()
{
  assert_choice_type(types::init_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<init_context_setup_resp_s>();
}
mt_communication_handling_resp_s& ngap_elem_procs_o::successful_outcome_c::mt_communication_handling_resp()
{
  assert_choice_type(types::mt_communication_handling_resp, type_, "SuccessfulOutcome");
  return c.get<mt_communication_handling_resp_s>();
}
multicast_session_activation_resp_s& ngap_elem_procs_o::successful_outcome_c::multicast_session_activation_resp()
{
  assert_choice_type(types::multicast_session_activation_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_activation_resp_s>();
}
multicast_session_deactivation_resp_s& ngap_elem_procs_o::successful_outcome_c::multicast_session_deactivation_resp()
{
  assert_choice_type(types::multicast_session_deactivation_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_deactivation_resp_s>();
}
multicast_session_upd_resp_s& ngap_elem_procs_o::successful_outcome_c::multicast_session_upd_resp()
{
  assert_choice_type(types::multicast_session_upd_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_upd_resp_s>();
}
ng_reset_ack_s& ngap_elem_procs_o::successful_outcome_c::ng_reset_ack()
{
  assert_choice_type(types::ng_reset_ack, type_, "SuccessfulOutcome");
  return c.get<ng_reset_ack_s>();
}
ng_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::ng_setup_resp()
{
  assert_choice_type(types::ng_setup_resp, type_, "SuccessfulOutcome");
  return c.get<ng_setup_resp_s>();
}
path_switch_request_ack_s& ngap_elem_procs_o::successful_outcome_c::path_switch_request_ack()
{
  assert_choice_type(types::path_switch_request_ack, type_, "SuccessfulOutcome");
  return c.get<path_switch_request_ack_s>();
}
pdu_session_res_modify_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_modify_resp()
{
  assert_choice_type(types::pdu_session_res_modify_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_modify_resp_s>();
}
pdu_session_res_modify_confirm_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_modify_confirm()
{
  assert_choice_type(types::pdu_session_res_modify_confirm, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_modify_confirm_s>();
}
pdu_session_res_release_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_release_resp()
{
  assert_choice_type(types::pdu_session_res_release_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_release_resp_s>();
}
pdu_session_res_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_setup_resp()
{
  assert_choice_type(types::pdu_session_res_setup_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_setup_resp_s>();
}
pws_cancel_resp_s& ngap_elem_procs_o::successful_outcome_c::pws_cancel_resp()
{
  assert_choice_type(types::pws_cancel_resp, type_, "SuccessfulOutcome");
  return c.get<pws_cancel_resp_s>();
}
ran_cfg_upd_ack_s& ngap_elem_procs_o::successful_outcome_c::ran_cfg_upd_ack()
{
  assert_choice_type(types::ran_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ran_cfg_upd_ack_s>();
}
timing_synchronisation_status_resp_s& ngap_elem_procs_o::successful_outcome_c::timing_synchronisation_status_resp()
{
  assert_choice_type(types::timing_synchronisation_status_resp, type_, "SuccessfulOutcome");
  return c.get<timing_synchronisation_status_resp_s>();
}
ue_context_mod_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_mod_resp()
{
  assert_choice_type(types::ue_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_resp_s>();
}
ue_context_release_complete_s& ngap_elem_procs_o::successful_outcome_c::ue_context_release_complete()
{
  assert_choice_type(types::ue_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<ue_context_release_complete_s>();
}
ue_context_resume_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_resume_resp()
{
  assert_choice_type(types::ue_context_resume_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_resume_resp_s>();
}
ue_context_suspend_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_suspend_resp()
{
  assert_choice_type(types::ue_context_suspend_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_suspend_resp_s>();
}
ue_radio_cap_check_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_radio_cap_check_resp()
{
  assert_choice_type(types::ue_radio_cap_check_resp, type_, "SuccessfulOutcome");
  return c.get<ue_radio_cap_check_resp_s>();
}
ue_radio_cap_id_map_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_radio_cap_id_map_resp()
{
  assert_choice_type(types::ue_radio_cap_id_map_resp, type_, "SuccessfulOutcome");
  return c.get<ue_radio_cap_id_map_resp_s>();
}
write_replace_warning_resp_s& ngap_elem_procs_o::successful_outcome_c::write_replace_warning_resp()
{
  assert_choice_type(types::write_replace_warning_resp, type_, "SuccessfulOutcome");
  return c.get<write_replace_warning_resp_s>();
}
const amf_cfg_upd_ack_s& ngap_elem_procs_o::successful_outcome_c::amf_cfg_upd_ack() const
{
  assert_choice_type(types::amf_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<amf_cfg_upd_ack_s>();
}
const broadcast_session_mod_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_mod_resp() const
{
  assert_choice_type(types::broadcast_session_mod_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_mod_resp_s>();
}
const broadcast_session_release_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_release_resp() const
{
  assert_choice_type(types::broadcast_session_release_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_release_resp_s>();
}
const broadcast_session_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::broadcast_session_setup_resp() const
{
  assert_choice_type(types::broadcast_session_setup_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_setup_resp_s>();
}
const broadcast_session_transport_resp_s&
ngap_elem_procs_o::successful_outcome_c::broadcast_session_transport_resp() const
{
  assert_choice_type(types::broadcast_session_transport_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_session_transport_resp_s>();
}
const distribution_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::distribution_setup_resp() const
{
  assert_choice_type(types::distribution_setup_resp, type_, "SuccessfulOutcome");
  return c.get<distribution_setup_resp_s>();
}
const distribution_release_resp_s& ngap_elem_procs_o::successful_outcome_c::distribution_release_resp() const
{
  assert_choice_type(types::distribution_release_resp, type_, "SuccessfulOutcome");
  return c.get<distribution_release_resp_s>();
}
const ho_cancel_ack_s& ngap_elem_procs_o::successful_outcome_c::ho_cancel_ack() const
{
  assert_choice_type(types::ho_cancel_ack, type_, "SuccessfulOutcome");
  return c.get<ho_cancel_ack_s>();
}
const ho_cmd_s& ngap_elem_procs_o::successful_outcome_c::ho_cmd() const
{
  assert_choice_type(types::ho_cmd, type_, "SuccessfulOutcome");
  return c.get<ho_cmd_s>();
}
const ho_request_ack_s& ngap_elem_procs_o::successful_outcome_c::ho_request_ack() const
{
  assert_choice_type(types::ho_request_ack, type_, "SuccessfulOutcome");
  return c.get<ho_request_ack_s>();
}
const init_context_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::init_context_setup_resp() const
{
  assert_choice_type(types::init_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<init_context_setup_resp_s>();
}
const mt_communication_handling_resp_s& ngap_elem_procs_o::successful_outcome_c::mt_communication_handling_resp() const
{
  assert_choice_type(types::mt_communication_handling_resp, type_, "SuccessfulOutcome");
  return c.get<mt_communication_handling_resp_s>();
}
const multicast_session_activation_resp_s&
ngap_elem_procs_o::successful_outcome_c::multicast_session_activation_resp() const
{
  assert_choice_type(types::multicast_session_activation_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_activation_resp_s>();
}
const multicast_session_deactivation_resp_s&
ngap_elem_procs_o::successful_outcome_c::multicast_session_deactivation_resp() const
{
  assert_choice_type(types::multicast_session_deactivation_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_deactivation_resp_s>();
}
const multicast_session_upd_resp_s& ngap_elem_procs_o::successful_outcome_c::multicast_session_upd_resp() const
{
  assert_choice_type(types::multicast_session_upd_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_session_upd_resp_s>();
}
const ng_reset_ack_s& ngap_elem_procs_o::successful_outcome_c::ng_reset_ack() const
{
  assert_choice_type(types::ng_reset_ack, type_, "SuccessfulOutcome");
  return c.get<ng_reset_ack_s>();
}
const ng_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::ng_setup_resp() const
{
  assert_choice_type(types::ng_setup_resp, type_, "SuccessfulOutcome");
  return c.get<ng_setup_resp_s>();
}
const path_switch_request_ack_s& ngap_elem_procs_o::successful_outcome_c::path_switch_request_ack() const
{
  assert_choice_type(types::path_switch_request_ack, type_, "SuccessfulOutcome");
  return c.get<path_switch_request_ack_s>();
}
const pdu_session_res_modify_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_modify_resp() const
{
  assert_choice_type(types::pdu_session_res_modify_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_modify_resp_s>();
}
const pdu_session_res_modify_confirm_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_modify_confirm() const
{
  assert_choice_type(types::pdu_session_res_modify_confirm, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_modify_confirm_s>();
}
const pdu_session_res_release_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_release_resp() const
{
  assert_choice_type(types::pdu_session_res_release_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_release_resp_s>();
}
const pdu_session_res_setup_resp_s& ngap_elem_procs_o::successful_outcome_c::pdu_session_res_setup_resp() const
{
  assert_choice_type(types::pdu_session_res_setup_resp, type_, "SuccessfulOutcome");
  return c.get<pdu_session_res_setup_resp_s>();
}
const pws_cancel_resp_s& ngap_elem_procs_o::successful_outcome_c::pws_cancel_resp() const
{
  assert_choice_type(types::pws_cancel_resp, type_, "SuccessfulOutcome");
  return c.get<pws_cancel_resp_s>();
}
const ran_cfg_upd_ack_s& ngap_elem_procs_o::successful_outcome_c::ran_cfg_upd_ack() const
{
  assert_choice_type(types::ran_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<ran_cfg_upd_ack_s>();
}
const timing_synchronisation_status_resp_s&
ngap_elem_procs_o::successful_outcome_c::timing_synchronisation_status_resp() const
{
  assert_choice_type(types::timing_synchronisation_status_resp, type_, "SuccessfulOutcome");
  return c.get<timing_synchronisation_status_resp_s>();
}
const ue_context_mod_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_mod_resp() const
{
  assert_choice_type(types::ue_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_resp_s>();
}
const ue_context_release_complete_s& ngap_elem_procs_o::successful_outcome_c::ue_context_release_complete() const
{
  assert_choice_type(types::ue_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<ue_context_release_complete_s>();
}
const ue_context_resume_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_resume_resp() const
{
  assert_choice_type(types::ue_context_resume_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_resume_resp_s>();
}
const ue_context_suspend_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_context_suspend_resp() const
{
  assert_choice_type(types::ue_context_suspend_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_suspend_resp_s>();
}
const ue_radio_cap_check_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_radio_cap_check_resp() const
{
  assert_choice_type(types::ue_radio_cap_check_resp, type_, "SuccessfulOutcome");
  return c.get<ue_radio_cap_check_resp_s>();
}
const ue_radio_cap_id_map_resp_s& ngap_elem_procs_o::successful_outcome_c::ue_radio_cap_id_map_resp() const
{
  assert_choice_type(types::ue_radio_cap_id_map_resp, type_, "SuccessfulOutcome");
  return c.get<ue_radio_cap_id_map_resp_s>();
}
const write_replace_warning_resp_s& ngap_elem_procs_o::successful_outcome_c::write_replace_warning_resp() const
{
  assert_choice_type(types::write_replace_warning_resp, type_, "SuccessfulOutcome");
  return c.get<write_replace_warning_resp_s>();
}

static const char* enum_names_ngap_elem_procs_o_successful_outcome_c_types_values[] = {
    "AMFConfigurationUpdateAcknowledge",
    "BroadcastSessionModificationResponse",
    "BroadcastSessionReleaseResponse",
    "BroadcastSessionSetupResponse",
    "BroadcastSessionTransportResponse",
    "DistributionSetupResponse",
    "DistributionReleaseResponse",
    "HandoverCancelAcknowledge",
    "HandoverCommand",
    "HandoverRequestAcknowledge",
    "InitialContextSetupResponse",
    "MTCommunicationHandlingResponse",
    "MulticastSessionActivationResponse",
    "MulticastSessionDeactivationResponse",
    "MulticastSessionUpdateResponse",
    "NGResetAcknowledge",
    "NGSetupResponse",
    "PathSwitchRequestAcknowledge",
    "PDUSessionResourceModifyResponse",
    "PDUSessionResourceModifyConfirm",
    "PDUSessionResourceReleaseResponse",
    "PDUSessionResourceSetupResponse",
    "PWSCancelResponse",
    "RANConfigurationUpdateAcknowledge",
    "TimingSynchronisationStatusResponse",
    "UEContextModificationResponse",
    "UEContextReleaseComplete",
    "UEContextResumeResponse",
    "UEContextSuspendResponse",
    "UERadioCapabilityCheckResponse",
    "UERadioCapabilityIDMappingResponse",
    "WriteReplaceWarningResponse"};
const asn1::table::enum_desc ngap_elem_procs_o::successful_outcome_c::types_opts::desc = {
    enum_names_ngap_elem_procs_o_successful_outcome_c_types_values,
    32,
    "ngap_elem_procs_o::successful_outcome_c::types"};

// UnsuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_ngap_elem_procs_o_unsuccessful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<amf_cfg_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_mod_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_session_transport_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<distribution_setup_fail_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ho_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<init_context_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<mt_communication_handling_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_session_activation_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_session_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ng_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<path_switch_request_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ran_cfg_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<timing_synchronisation_status_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_mod_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_resume_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_suspend_fail_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_ngap_elem_procs_o_unsuccessful_outcome_c_names[] = {
    {"AMFConfigurationUpdateFailure", nullptr},
    {"BroadcastSessionModificationFailure", nullptr},
    {"BroadcastSessionSetupFailure", nullptr},
    {"BroadcastSessionTransportFailure", nullptr},
    {"DistributionSetupFailure", nullptr},
    {"HandoverPreparationFailure", nullptr},
    {"HandoverFailure", nullptr},
    {"InitialContextSetupFailure", nullptr},
    {"MTCommunicationHandlingFailure", nullptr},
    {"MulticastSessionActivationFailure", nullptr},
    {"MulticastSessionUpdateFailure", nullptr},
    {"NGSetupFailure", nullptr},
    {"PathSwitchRequestFailure", nullptr},
    {"RANConfigurationUpdateFailure", nullptr},
    {"TimingSynchronisationStatusFailure", nullptr},
    {"UEContextModificationFailure", nullptr},
    {"UEContextResumeFailure", nullptr},
    {"UEContextSuspendFailure", nullptr}};
const asn1::table::choice_desc ngap_elem_procs_o::unsuccessful_outcome_c::desc = {
    asn1::table::enum_int_type<ngap_elem_procs_o::unsuccessful_outcome_c::types>(),
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nof_types,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::nof_exts,
    ngap_elem_procs_o::unsuccessful_outcome_c::types::has_ext,
    true,
    choice_table_ngap_elem_procs_o_unsuccessful_outcome_c_alternatives,
    &asn1::table::choice_set<ngap_elem_procs_o::unsuccessful_outcome_c>,
    choice_table_ngap_elem_procs_o_unsuccessful_outcome_c_names,
    "ngap_elem_procs_o::unsuccessful_outcome_c",
    offsetof(ngap_elem_procs_o::unsuccessful_outcome_c, type_),
    offsetof(ngap_elem_procs_o::unsuccessful_outcome_c, c),
    false,
    nullptr,
    true};

void ngap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, ngap_elem_procs_o::unsuccessful_outcome_c::desc);
}
amf_cfg_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::amf_cfg_upd_fail()
{
  assert_choice_type(types::amf_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<amf_cfg_upd_fail_s>();
}
broadcast_session_mod_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::broadcast_session_mod_fail()
{
  assert_choice_type(types::broadcast_session_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_session_mod_fail_s>();
}
broadcast_session_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::broadcast_session_setup_fail()
{
  assert_choice_type(types::broadcast_session_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_session_setup_fail_s>();
}
broadcast_session_transport_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::broadcast_session_transport_fail()
{
  assert_choice_type(types::broadcast_session_transport_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_session_transport_fail_s>();
}
distribution_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::distribution_setup_fail()
{
  assert_choice_type(types::distribution_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<distribution_setup_fail_s>();
}
ho_prep_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ho_prep_fail()
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
ho_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ho_fail()
{
  assert_choice_type(types::ho_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_fail_s>();
}
init_context_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::init_context_setup_fail()
{
  assert_choice_type(types::init_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<init_context_setup_fail_s>();
}
mt_communication_handling_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::mt_communication_handling_fail()
{
  assert_choice_type(types::mt_communication_handling_fail, type_, "UnsuccessfulOutcome");
  return c.get<mt_communication_handling_fail_s>();
}
multicast_session_activation_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::multicast_session_activation_fail()
{
  assert_choice_type(types::multicast_session_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_session_activation_fail_s>();
}
multicast_session_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::multicast_session_upd_fail()
{
  assert_choice_type(types::multicast_session_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_session_upd_fail_s>();
}
ng_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ng_setup_fail()
{
  assert_choice_type(types::ng_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<ng_setup_fail_s>();
}
path_switch_request_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::path_switch_request_fail()
{
  assert_choice_type(types::path_switch_request_fail, type_, "UnsuccessfulOutcome");
  return c.get<path_switch_request_fail_s>();
}
ran_cfg_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ran_cfg_upd_fail()
{
  assert_choice_type(types::ran_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ran_cfg_upd_fail_s>();
}
timing_synchronisation_status_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::timing_synchronisation_status_fail()
{
  assert_choice_type(types::timing_synchronisation_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<timing_synchronisation_status_fail_s>();
}
ue_context_mod_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_fail()
{
  assert_choice_type(types::ue_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_fail_s>();
}
ue_context_resume_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_resume_fail()
{
  assert_choice_type(types::ue_context_resume_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_resume_fail_s>();
}
ue_context_suspend_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_suspend_fail()
{
  assert_choice_type(types::ue_context_suspend_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_suspend_fail_s>();
}
const amf_cfg_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::amf_cfg_upd_fail() const
{
  assert_choice_type(types::amf_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<amf_cfg_upd_fail_s>();
}
const broadcast_session_mod_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::broadcast_session_mod_fail() const
{
  assert_choice_type(types::broadcast_session_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_session_mod_fail_s>();
}
const broadcast_session_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::broadcast_session_setup_fail() const
{
  assert_choice_type(types::broadcast_session_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_session_setup_fail_s>();
}
const broadcast_session_transport_fail_s&
ngap_elem_procs_o::unsuccessful_outcome_c::broadcast_session_transport_fail() const
{
  assert_choice_type(types::broadcast_session_transport_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_session_transport_fail_s>();
}
const distribution_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::distribution_setup_fail() const
{
  assert_choice_type(types::distribution_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<distribution_setup_fail_s>();
}
const ho_prep_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ho_prep_fail() const
{
  assert_choice_type(types::ho_prep_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_prep_fail_s>();
}
const ho_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ho_fail() const
{
  assert_choice_type(types::ho_fail, type_, "UnsuccessfulOutcome");
  return c.get<ho_fail_s>();
}
const init_context_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::init_context_setup_fail() const
{
  assert_choice_type(types::init_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<init_context_setup_fail_s>();
}
const mt_communication_handling_fail_s&
ngap_elem_procs_o::unsuccessful_outcome_c::mt_communication_handling_fail() const
{
  assert_choice_type(types::mt_communication_handling_fail, type_, "UnsuccessfulOutcome");
  return c.get<mt_communication_handling_fail_s>();
}
const multicast_session_activation_fail_s&
ngap_elem_procs_o::unsuccessful_outcome_c::multicast_session_activation_fail() const
{
  assert_choice_type(types::multicast_session_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_session_activation_fail_s>();
}
const multicast_session_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::multicast_session_upd_fail() const
{
  assert_choice_type(types::multicast_session_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_session_upd_fail_s>();
}
const ng_setup_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ng_setup_fail() const
{
  assert_choice_type(types::ng_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<ng_setup_fail_s>();
}
const path_switch_request_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::path_switch_request_fail() const
{
  assert_choice_type(types::path_switch_request_fail, type_, "UnsuccessfulOutcome");
  return c.get<path_switch_request_fail_s>();
}
const ran_cfg_upd_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ran_cfg_upd_fail() const
{
  assert_choice_type(types::ran_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<ran_cfg_upd_fail_s>();
}
const timing_synchronisation_status_fail_s&
ngap_elem_procs_o::unsuccessful_outcome_c::timing_synchronisation_status_fail() const
{
  assert_choice_type(types::timing_synchronisation_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<timing_synchronisation_status_fail_s>();
}
const ue_context_mod_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_fail() const
{
  assert_choice_type(types::ue_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_fail_s>();
}
const ue_context_resume_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_resume_fail() const
{
  assert_choice_type(types::ue_context_resume_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_resume_fail_s>();
}
const ue_context_suspend_fail_s& ngap_elem_procs_o::unsuccessful_outcome_c::ue_context_suspend_fail() const
{
  assert_choice_type(types::ue_context_suspend_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_suspend_fail_s>();
}

static const char* enum_names_ngap_elem_procs_o_unsuccessful_outcome_c_types_values[] = {
    "AMFConfigurationUpdateFailure",
    "BroadcastSessionModificationFailure",
    "BroadcastSessionSetupFailure",
    "BroadcastSessionTransportFailure",
    "DistributionSetupFailure",
    "HandoverPreparationFailure",
    "HandoverFailure",
    "InitialContextSetupFailure",
    "MTCommunicationHandlingFailure",
    "MulticastSessionActivationFailure",
    "MulticastSessionUpdateFailure",
    "NGSetupFailure",
    "PathSwitchRequestFailure",
    "RANConfigurationUpdateFailure",
    "TimingSynchronisationStatusFailure",
    "UEContextModificationFailure",
    "UEContextResumeFailure",
    "UEContextSuspendFailure"};
const asn1::table::enum_desc ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::desc = {
    enum_names_ngap_elem_procs_o_unsuccessful_outcome_c_types_values,
    18,
    "ngap_elem_procs_o::unsuccessful_outcome_c::types"};

// Columns of the object sets of NGAP-ELEMENTARY-PROCEDURE.
enum class ngap_elem_proc_p_column : uint32_t { init_msg, successful_outcome, unsuccessful_outcome, crit };

// InitiatingMessage ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
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
                                 ngap_elem_procs_o::desc,
                                 ngap_elem_proc_p_column::crit,
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
                                  ngap_elem_procs_o::desc,
                                  ngap_elem_proc_p_column::init_msg,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_init_msg_s_names[] = {{"procedureCode", nullptr},
                                                                     {"criticality", nullptr},
                                                                     {"value", nullptr}};
const asn1::table::seq_desc          init_msg_s::desc =
    {{seq_table_init_msg_s_fields, uint64_t{0x7}, nullptr, seq_table_init_msg_s_names, 3, 0}, false, true, nullptr, 0};
bool init_msg_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(ngap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(ngap_elem_procs_o::desc, ngap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, ngap_elem_procs_o::desc, ngap_elem_proc_p_column::init_msg, proc_code);
  return value.type().value != ngap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
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
                                 ngap_elem_procs_o::desc,
                                 ngap_elem_proc_p_column::crit,
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
                                  ngap_elem_procs_o::desc,
                                  ngap_elem_proc_p_column::successful_outcome,
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
  if (not asn1::table::objset_has_key(ngap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(ngap_elem_procs_o::desc, ngap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, ngap_elem_procs_o::desc, ngap_elem_proc_p_column::successful_outcome, proc_code);
  return value.type().value != ngap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{NGAP-ELEMENTARY-PROCEDURE}}
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
                                 ngap_elem_procs_o::desc,
                                 ngap_elem_proc_p_column::crit,
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
                                  ngap_elem_procs_o::desc,
                                  ngap_elem_proc_p_column::unsuccessful_outcome,
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
  if (not asn1::table::objset_has_key(ngap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(ngap_elem_procs_o::desc, ngap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, ngap_elem_procs_o::desc, ngap_elem_proc_p_column::unsuccessful_outcome, proc_code);
  return value.type().value != ngap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// NGAP-PDU ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_ngap_pdu_c_alternatives[] = {
    &asn1::table::alternative_ops<init_msg_s, false, false>::value,
    &asn1::table::alternative_ops<successful_outcome_s, false, false>::value,
    &asn1::table::alternative_ops<unsuccessful_outcome_s, false, false>::value};
const asn1::table::choice_storage_desc ngap_pdu_c::storage_desc = {choice_storage_ngap_pdu_c_alternatives,
                                                                   3,
                                                                   "ngap_pdu_c"};

init_msg_s& ngap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& ngap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& ngap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
static const asn1::table::seq_field  choice_table_ngap_pdu_c_alternatives[] = {{0,
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
static const asn1::table::field_name choice_table_ngap_pdu_c_names[]        = {{"initiatingMessage", nullptr},
                                                                               {"successfulOutcome", nullptr},
                                                                               {"unsuccessfulOutcome", nullptr}};
const asn1::table::choice_desc       ngap_pdu_c::desc = {asn1::table::enum_int_type<ngap_pdu_c::types>(),
                                                         ngap_pdu_c::types::nof_types,
                                                         ngap_pdu_c::types::nof_exts,
                                                         ngap_pdu_c::types::has_ext,
                                                         true,
                                                         choice_table_ngap_pdu_c_alternatives,
                                                         nullptr,
                                                         choice_table_ngap_pdu_c_names,
                                                         "ngap_pdu_c",
                                                         offsetof(ngap_pdu_c, c.type),
                                                         offsetof(ngap_pdu_c, c.value),
                                                         false,
                                                         &ngap_pdu_c::storage_desc};

static const char*           enum_names_ngap_pdu_c_types_values[] = {"initiatingMessage",
                                                                     "successfulOutcome",
                                                                     "unsuccessfulOutcome"};
const asn1::table::enum_desc ngap_pdu_c::types_opts::desc         = {enum_names_ngap_pdu_c_types_values,
                                                                     3,
                                                                     "ngap_pdu_c::types"};
