// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/asn1/f1ap/f1ap.h"
#include "ocudu/asn1/asn1_ap_codec.h"
#include "ocudu/asn1/f1ap/f1ap_pdu_contents.h"

using namespace asn1;
using namespace asn1::f1ap;

/*******************************************************************************
 *                                Struct Methods
 ******************************************************************************/

// F1AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF F1AP-ELEMENTARY-PROCEDURE
static const uint32_t objset_proc_code_f1ap_elem_procs_o[] = {
    0,  1,  3,  4,  5,  6,  7,  8,  20, 21, 16, 26, 32, 33, 34, 35, 36, 41, 48, 49, 50, 52, 59, 60,
    62, 64, 65, 67, 68, 69, 70, 75, 76, 88, 92, 93, 2,  10, 12, 13, 15, 14, 11, 17, 18, 19, 22, 23,
    24, 25, 27, 28, 29, 30, 31, 37, 38, 58, 57, 39, 40, 42, 43, 44, 45, 46, 47, 51, 53, 54, 55, 56,
    63, 61, 66, 71, 79, 80, 77, 78, 81, 82, 83, 84, 85, 86, 87, 89, 90, 91, 94, 95, 96, 97};
static const uint8_t objset_init_msg_f1ap_elem_procs_o[] = {
    f1ap_elem_procs_o::init_msg_c::types::reset,
    f1ap_elem_procs_o::init_msg_c::types::f1_setup_request,
    f1ap_elem_procs_o::init_msg_c::types::gnb_du_cfg_upd,
    f1ap_elem_procs_o::init_msg_c::types::gnb_cu_cfg_upd,
    f1ap_elem_procs_o::init_msg_c::types::ue_context_setup_request,
    f1ap_elem_procs_o::init_msg_c::types::ue_context_release_cmd,
    f1ap_elem_procs_o::init_msg_c::types::ue_context_mod_request,
    f1ap_elem_procs_o::init_msg_c::types::ue_context_mod_required,
    f1ap_elem_procs_o::init_msg_c::types::write_replace_warning_request,
    f1ap_elem_procs_o::init_msg_c::types::pws_cancel_request,
    f1ap_elem_procs_o::init_msg_c::types::gnb_du_res_coordination_request,
    f1ap_elem_procs_o::init_msg_c::types::f1_removal_request,
    f1ap_elem_procs_o::init_msg_c::types::bap_map_cfg,
    f1ap_elem_procs_o::init_msg_c::types::gnb_du_res_cfg,
    f1ap_elem_procs_o::init_msg_c::types::iab_tnl_address_request,
    f1ap_elem_procs_o::init_msg_c::types::iab_up_cfg_upd_request,
    f1ap_elem_procs_o::init_msg_c::types::res_status_request,
    f1ap_elem_procs_o::init_msg_c::types::positioning_meas_request,
    f1ap_elem_procs_o::init_msg_c::types::trp_info_request,
    f1ap_elem_procs_o::init_msg_c::types::positioning_info_request,
    f1ap_elem_procs_o::init_msg_c::types::positioning_activation_request,
    f1ap_elem_procs_o::init_msg_c::types::e_c_id_meas_initiation_request,
    f1ap_elem_procs_o::init_msg_c::types::broadcast_context_setup_request,
    f1ap_elem_procs_o::init_msg_c::types::broadcast_context_release_cmd,
    f1ap_elem_procs_o::init_msg_c::types::broadcast_context_mod_request,
    f1ap_elem_procs_o::init_msg_c::types::multicast_context_setup_request,
    f1ap_elem_procs_o::init_msg_c::types::multicast_context_release_cmd,
    f1ap_elem_procs_o::init_msg_c::types::multicast_context_mod_request,
    f1ap_elem_procs_o::init_msg_c::types::multicast_distribution_setup_request,
    f1ap_elem_procs_o::init_msg_c::types::multicast_distribution_release_cmd,
    f1ap_elem_procs_o::init_msg_c::types::pdc_meas_initiation_request,
    f1ap_elem_procs_o::init_msg_c::types::prs_cfg_request,
    f1ap_elem_procs_o::init_msg_c::types::meas_precfg_required,
    f1ap_elem_procs_o::init_msg_c::types::timing_synchronisation_status_request,
    f1ap_elem_procs_o::init_msg_c::types::multicast_context_notif_ind,
    f1ap_elem_procs_o::init_msg_c::types::multicast_common_cfg_request,
    f1ap_elem_procs_o::init_msg_c::types::error_ind,
    f1ap_elem_procs_o::init_msg_c::types::ue_context_release_request,
    f1ap_elem_procs_o::init_msg_c::types::dl_rrc_msg_transfer,
    f1ap_elem_procs_o::init_msg_c::types::ul_rrc_msg_transfer,
    f1ap_elem_procs_o::init_msg_c::types::ue_inactivity_notif,
    f1ap_elem_procs_o::init_msg_c::types::private_msg,
    f1ap_elem_procs_o::init_msg_c::types::init_ul_rrc_msg_transfer,
    f1ap_elem_procs_o::init_msg_c::types::sys_info_delivery_cmd,
    f1ap_elem_procs_o::init_msg_c::types::paging,
    f1ap_elem_procs_o::init_msg_c::types::notify,
    f1ap_elem_procs_o::init_msg_c::types::pws_restart_ind,
    f1ap_elem_procs_o::init_msg_c::types::pws_fail_ind,
    f1ap_elem_procs_o::init_msg_c::types::gnb_du_status_ind,
    f1ap_elem_procs_o::init_msg_c::types::rrc_delivery_report,
    f1ap_elem_procs_o::init_msg_c::types::network_access_rate_reduction,
    f1ap_elem_procs_o::init_msg_c::types::trace_start,
    f1ap_elem_procs_o::init_msg_c::types::deactiv_trace,
    f1ap_elem_procs_o::init_msg_c::types::du_cu_radio_info_transfer,
    f1ap_elem_procs_o::init_msg_c::types::cu_du_radio_info_transfer,
    f1ap_elem_procs_o::init_msg_c::types::res_status_upd,
    f1ap_elem_procs_o::init_msg_c::types::access_and_mob_ind,
    f1ap_elem_procs_o::init_msg_c::types::ref_time_info_report_ctrl,
    f1ap_elem_procs_o::init_msg_c::types::ref_time_info_report,
    f1ap_elem_procs_o::init_msg_c::types::access_success,
    f1ap_elem_procs_o::init_msg_c::types::cell_traffic_trace,
    f1ap_elem_procs_o::init_msg_c::types::positioning_assist_info_ctrl,
    f1ap_elem_procs_o::init_msg_c::types::positioning_assist_info_feedback,
    f1ap_elem_procs_o::init_msg_c::types::positioning_meas_report,
    f1ap_elem_procs_o::init_msg_c::types::positioning_meas_abort,
    f1ap_elem_procs_o::init_msg_c::types::positioning_meas_fail_ind,
    f1ap_elem_procs_o::init_msg_c::types::positioning_meas_upd,
    f1ap_elem_procs_o::init_msg_c::types::positioning_deactivation,
    f1ap_elem_procs_o::init_msg_c::types::e_c_id_meas_fail_ind,
    f1ap_elem_procs_o::init_msg_c::types::e_c_id_meas_report,
    f1ap_elem_procs_o::init_msg_c::types::e_c_id_meas_termination_cmd,
    f1ap_elem_procs_o::init_msg_c::types::positioning_info_upd,
    f1ap_elem_procs_o::init_msg_c::types::multicast_group_paging,
    f1ap_elem_procs_o::init_msg_c::types::broadcast_context_release_request,
    f1ap_elem_procs_o::init_msg_c::types::multicast_context_release_request,
    f1ap_elem_procs_o::init_msg_c::types::pdc_meas_report,
    f1ap_elem_procs_o::init_msg_c::types::pdc_meas_termination_cmd,
    f1ap_elem_procs_o::init_msg_c::types::pdc_meas_fail_ind,
    f1ap_elem_procs_o::init_msg_c::types::meas_activation,
    f1ap_elem_procs_o::init_msg_c::types::qo_e_info_transfer,
    f1ap_elem_procs_o::init_msg_c::types::pos_sys_info_delivery_cmd,
    f1ap_elem_procs_o::init_msg_c::types::du_cu_cell_switch_notif,
    f1ap_elem_procs_o::init_msg_c::types::cu_du_cell_switch_notif,
    f1ap_elem_procs_o::init_msg_c::types::du_cu_tai_nformation_transfer,
    f1ap_elem_procs_o::init_msg_c::types::cu_du_tai_nformation_transfer,
    f1ap_elem_procs_o::init_msg_c::types::qo_e_info_transfer_ctrl,
    f1ap_elem_procs_o::init_msg_c::types::rach_ind,
    f1ap_elem_procs_o::init_msg_c::types::timing_synchronisation_status_report,
    f1ap_elem_procs_o::init_msg_c::types::miabf1_setup_trigger,
    f1ap_elem_procs_o::init_msg_c::types::miabf1_setup_outcome_notif,
    f1ap_elem_procs_o::init_msg_c::types::broadcast_transport_res_request,
    f1ap_elem_procs_o::init_msg_c::types::du_cu_access_and_mob_ind,
    f1ap_elem_procs_o::init_msg_c::types::srs_info_reserv_notif,
    f1ap_elem_procs_o::init_msg_c::types::cu_du_mob_initiation_request};
static const uint8_t objset_successful_outcome_f1ap_elem_procs_o[] = {
    f1ap_elem_procs_o::successful_outcome_c::types::reset_ack,
    f1ap_elem_procs_o::successful_outcome_c::types::f1_setup_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::gnb_du_cfg_upd_ack,
    f1ap_elem_procs_o::successful_outcome_c::types::gnb_cu_cfg_upd_ack,
    f1ap_elem_procs_o::successful_outcome_c::types::ue_context_setup_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::ue_context_release_complete,
    f1ap_elem_procs_o::successful_outcome_c::types::ue_context_mod_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::ue_context_mod_confirm,
    f1ap_elem_procs_o::successful_outcome_c::types::write_replace_warning_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::pws_cancel_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::gnb_du_res_coordination_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::f1_removal_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::bap_map_cfg_ack,
    f1ap_elem_procs_o::successful_outcome_c::types::gnb_du_res_cfg_ack,
    f1ap_elem_procs_o::successful_outcome_c::types::iab_tnl_address_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::iab_up_cfg_upd_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::res_status_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::positioning_meas_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::trp_info_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::positioning_info_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::positioning_activation_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::e_c_id_meas_initiation_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::broadcast_context_setup_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::broadcast_context_release_complete,
    f1ap_elem_procs_o::successful_outcome_c::types::broadcast_context_mod_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::multicast_context_setup_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::multicast_context_release_complete,
    f1ap_elem_procs_o::successful_outcome_c::types::multicast_context_mod_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::multicast_distribution_setup_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::multicast_distribution_release_complete,
    f1ap_elem_procs_o::successful_outcome_c::types::pdc_meas_initiation_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::prs_cfg_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::meas_precfg_confirm,
    f1ap_elem_procs_o::successful_outcome_c::types::timing_synchronisation_status_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::multicast_context_notif_confirm,
    f1ap_elem_procs_o::successful_outcome_c::types::multicast_common_cfg_resp,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::successful_outcome_c::types::nulltype};
static const uint8_t objset_unsuccessful_outcome_f1ap_elem_procs_o[] = {
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::f1_setup_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::gnb_du_cfg_upd_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::gnb_cu_cfg_upd_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::ue_context_setup_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::ue_context_mod_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::ue_context_mod_refuse,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::f1_removal_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::bap_map_cfg_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::gnb_du_res_cfg_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::iab_tnl_address_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::iab_up_cfg_upd_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::res_status_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::positioning_meas_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::trp_info_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::positioning_info_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::positioning_activation_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::e_c_id_meas_initiation_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::broadcast_context_setup_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::broadcast_context_mod_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::multicast_context_setup_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::multicast_context_mod_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::multicast_distribution_setup_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::pdc_meas_initiation_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::prs_cfg_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::meas_precfg_refuse,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::timing_synchronisation_status_fail,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::multicast_context_notif_refuse,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::multicast_common_cfg_refuse,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype};
static const uint8_t objset_crit_f1ap_elem_procs_o[] = {
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::reject, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::reject, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore,
    crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::ignore, crit_e::reject, crit_e::reject, crit_e::reject,
    crit_e::ignore, crit_e::reject, crit_e::ignore};
static const asn1::table::objset_column objset_columns_f1ap_elem_procs_o[] = {
    {objset_init_msg_f1ap_elem_procs_o, f1ap_elem_procs_o::init_msg_c::types::nulltype},
    {objset_successful_outcome_f1ap_elem_procs_o, f1ap_elem_procs_o::successful_outcome_c::types::nulltype},
    {objset_unsuccessful_outcome_f1ap_elem_procs_o, f1ap_elem_procs_o::unsuccessful_outcome_c::types::nulltype},
    {objset_crit_f1ap_elem_procs_o, crit_e::nulltype}};
const asn1::table::objset_desc f1ap_elem_procs_o::desc = {objset_proc_code_f1ap_elem_procs_o,
                                                          94,
                                                          "proc_code",
                                                          objset_columns_f1ap_elem_procs_o};

// InitiatingMessage ::= OPEN TYPE
static const asn1::table::seq_field choice_table_f1ap_elem_procs_o_init_msg_c_alternatives[] = {
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
     &asn1::table::any_codec<f1_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_du_cfg_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_cfg_upd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_setup_request_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ue_context_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_mod_required_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<pws_cancel_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_du_res_coordination_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<f1_removal_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bap_map_cfg_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_du_res_cfg_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_tnl_address_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_up_cfg_upd_request_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<positioning_meas_request_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<positioning_info_request_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<e_c_id_meas_initiation_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_context_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_context_release_cmd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_context_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_context_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_context_release_cmd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_context_mod_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_distribution_setup_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_distribution_release_cmd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdc_meas_initiation_request_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<timing_synchronisation_status_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_context_notif_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_common_cfg_request_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ue_context_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<dl_rrc_msg_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ul_rrc_msg_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_inactivity_notif_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<init_ul_rrc_msg_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<sys_info_delivery_cmd_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<notify_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<pws_fail_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_du_status_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<rrc_delivery_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<network_access_rate_reduction_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<deactiv_trace_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<du_cu_radio_info_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cu_du_radio_info_transfer_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ref_time_info_report_ctrl_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ref_time_info_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<access_success_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<positioning_assist_info_ctrl_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_assist_info_feedback_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_meas_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_meas_abort_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_meas_fail_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<positioning_meas_upd_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<positioning_info_upd_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<broadcast_context_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_context_release_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdc_meas_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdc_meas_termination_cmd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdc_meas_fail_ind_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<qo_e_info_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pos_sys_info_delivery_cmd_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<du_cu_cell_switch_notif_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cu_du_cell_switch_notif_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<du_cu_tai_nformation_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cu_du_tai_nformation_transfer_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<qo_e_info_transfer_ctrl_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<timing_synchronisation_status_report_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<miabf1_setup_trigger_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<miabf1_setup_outcome_notif_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_transport_res_request_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<du_cu_access_and_mob_ind_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<srs_info_reserv_notif_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<cu_du_mob_initiation_request_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_f1ap_elem_procs_o_init_msg_c_names[] = {
    {"Reset", nullptr},
    {"F1SetupRequest", nullptr},
    {"GNBDUConfigurationUpdate", nullptr},
    {"GNBCUConfigurationUpdate", nullptr},
    {"UEContextSetupRequest", nullptr},
    {"UEContextReleaseCommand", nullptr},
    {"UEContextModificationRequest", nullptr},
    {"UEContextModificationRequired", nullptr},
    {"WriteReplaceWarningRequest", nullptr},
    {"PWSCancelRequest", nullptr},
    {"GNBDUResourceCoordinationRequest", nullptr},
    {"F1RemovalRequest", nullptr},
    {"BAPMappingConfiguration", nullptr},
    {"GNBDUResourceConfiguration", nullptr},
    {"IABTNLAddressRequest", nullptr},
    {"IABUPConfigurationUpdateRequest", nullptr},
    {"ResourceStatusRequest", nullptr},
    {"PositioningMeasurementRequest", nullptr},
    {"TRPInformationRequest", nullptr},
    {"PositioningInformationRequest", nullptr},
    {"PositioningActivationRequest", nullptr},
    {"E-CIDMeasurementInitiationRequest", nullptr},
    {"BroadcastContextSetupRequest", nullptr},
    {"BroadcastContextReleaseCommand", nullptr},
    {"BroadcastContextModificationRequest", nullptr},
    {"MulticastContextSetupRequest", nullptr},
    {"MulticastContextReleaseCommand", nullptr},
    {"MulticastContextModificationRequest", nullptr},
    {"MulticastDistributionSetupRequest", nullptr},
    {"MulticastDistributionReleaseCommand", nullptr},
    {"PDCMeasurementInitiationRequest", nullptr},
    {"PRSConfigurationRequest", nullptr},
    {"MeasurementPreconfigurationRequired", nullptr},
    {"TimingSynchronisationStatusRequest", nullptr},
    {"MulticastContextNotificationIndication", nullptr},
    {"MulticastCommonConfigurationRequest", nullptr},
    {"ErrorIndication", nullptr},
    {"UEContextReleaseRequest", nullptr},
    {"DLRRCMessageTransfer", nullptr},
    {"ULRRCMessageTransfer", nullptr},
    {"UEInactivityNotification", nullptr},
    {"PrivateMessage", nullptr},
    {"InitialULRRCMessageTransfer", nullptr},
    {"SystemInformationDeliveryCommand", nullptr},
    {"Paging", nullptr},
    {"Notify", nullptr},
    {"PWSRestartIndication", nullptr},
    {"PWSFailureIndication", nullptr},
    {"GNBDUStatusIndication", nullptr},
    {"RRCDeliveryReport", nullptr},
    {"NetworkAccessRateReduction", nullptr},
    {"TraceStart", nullptr},
    {"DeactivateTrace", nullptr},
    {"DUCURadioInformationTransfer", nullptr},
    {"CUDURadioInformationTransfer", nullptr},
    {"ResourceStatusUpdate", nullptr},
    {"AccessAndMobilityIndication", nullptr},
    {"ReferenceTimeInformationReportingControl", nullptr},
    {"ReferenceTimeInformationReport", nullptr},
    {"AccessSuccess", nullptr},
    {"CellTrafficTrace", nullptr},
    {"PositioningAssistanceInformationControl", nullptr},
    {"PositioningAssistanceInformationFeedback", nullptr},
    {"PositioningMeasurementReport", nullptr},
    {"PositioningMeasurementAbort", nullptr},
    {"PositioningMeasurementFailureIndication", nullptr},
    {"PositioningMeasurementUpdate", nullptr},
    {"PositioningDeactivation", nullptr},
    {"E-CIDMeasurementFailureIndication", nullptr},
    {"E-CIDMeasurementReport", nullptr},
    {"E-CIDMeasurementTerminationCommand", nullptr},
    {"PositioningInformationUpdate", nullptr},
    {"MulticastGroupPaging", nullptr},
    {"BroadcastContextReleaseRequest", nullptr},
    {"MulticastContextReleaseRequest", nullptr},
    {"PDCMeasurementReport", nullptr},
    {"PDCMeasurementTerminationCommand", nullptr},
    {"PDCMeasurementFailureIndication", nullptr},
    {"MeasurementActivation", nullptr},
    {"QoEInformationTransfer", nullptr},
    {"PosSystemInformationDeliveryCommand", nullptr},
    {"DUCUCellSwitchNotification", nullptr},
    {"CUDUCellSwitchNotification", nullptr},
    {"DUCUTAInformationTransfer", nullptr},
    {"CUDUTAInformationTransfer", nullptr},
    {"QoEInformationTransferControl", nullptr},
    {"RachIndication", nullptr},
    {"TimingSynchronisationStatusReport", nullptr},
    {"MIABF1SetupTriggering", nullptr},
    {"MIABF1SetupOutcomeNotification", nullptr},
    {"BroadcastTransportResourceRequest", nullptr},
    {"DUCUAccessAndMobilityIndication", nullptr},
    {"SRSInformationReservationNotification", nullptr},
    {"CUDUMobilityInitiationRequest", nullptr}};
const asn1::table::choice_desc f1ap_elem_procs_o::init_msg_c::desc = {
    asn1::table::enum_int_type<f1ap_elem_procs_o::init_msg_c::types>(),
    f1ap_elem_procs_o::init_msg_c::types::nof_types,
    f1ap_elem_procs_o::init_msg_c::types::nof_exts,
    f1ap_elem_procs_o::init_msg_c::types::has_ext,
    true,
    choice_table_f1ap_elem_procs_o_init_msg_c_alternatives,
    &asn1::table::choice_set<f1ap_elem_procs_o::init_msg_c>,
    choice_table_f1ap_elem_procs_o_init_msg_c_names,
    "f1ap_elem_procs_o::init_msg_c",
    offsetof(f1ap_elem_procs_o::init_msg_c, type_),
    offsetof(f1ap_elem_procs_o::init_msg_c, c),
    false,
    nullptr,
    true};

void f1ap_elem_procs_o::init_msg_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, f1ap_elem_procs_o::init_msg_c::desc);
}
reset_s& f1ap_elem_procs_o::init_msg_c::reset()
{
  assert_choice_type(types::reset, type_, "InitiatingMessage");
  return c.get<reset_s>();
}
f1_setup_request_s& f1ap_elem_procs_o::init_msg_c::f1_setup_request()
{
  assert_choice_type(types::f1_setup_request, type_, "InitiatingMessage");
  return c.get<f1_setup_request_s>();
}
gnb_du_cfg_upd_s& f1ap_elem_procs_o::init_msg_c::gnb_du_cfg_upd()
{
  assert_choice_type(types::gnb_du_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_du_cfg_upd_s>();
}
gnb_cu_cfg_upd_s& f1ap_elem_procs_o::init_msg_c::gnb_cu_cfg_upd()
{
  assert_choice_type(types::gnb_cu_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_cfg_upd_s>();
}
ue_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_setup_request()
{
  assert_choice_type(types::ue_context_setup_request, type_, "InitiatingMessage");
  return c.get<ue_context_setup_request_s>();
}
ue_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::ue_context_release_cmd()
{
  assert_choice_type(types::ue_context_release_cmd, type_, "InitiatingMessage");
  return c.get<ue_context_release_cmd_s>();
}
ue_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_mod_request()
{
  assert_choice_type(types::ue_context_mod_request, type_, "InitiatingMessage");
  return c.get<ue_context_mod_request_s>();
}
ue_context_mod_required_s& f1ap_elem_procs_o::init_msg_c::ue_context_mod_required()
{
  assert_choice_type(types::ue_context_mod_required, type_, "InitiatingMessage");
  return c.get<ue_context_mod_required_s>();
}
write_replace_warning_request_s& f1ap_elem_procs_o::init_msg_c::write_replace_warning_request()
{
  assert_choice_type(types::write_replace_warning_request, type_, "InitiatingMessage");
  return c.get<write_replace_warning_request_s>();
}
pws_cancel_request_s& f1ap_elem_procs_o::init_msg_c::pws_cancel_request()
{
  assert_choice_type(types::pws_cancel_request, type_, "InitiatingMessage");
  return c.get<pws_cancel_request_s>();
}
gnb_du_res_coordination_request_s& f1ap_elem_procs_o::init_msg_c::gnb_du_res_coordination_request()
{
  assert_choice_type(types::gnb_du_res_coordination_request, type_, "InitiatingMessage");
  return c.get<gnb_du_res_coordination_request_s>();
}
f1_removal_request_s& f1ap_elem_procs_o::init_msg_c::f1_removal_request()
{
  assert_choice_type(types::f1_removal_request, type_, "InitiatingMessage");
  return c.get<f1_removal_request_s>();
}
bap_map_cfg_s& f1ap_elem_procs_o::init_msg_c::bap_map_cfg()
{
  assert_choice_type(types::bap_map_cfg, type_, "InitiatingMessage");
  return c.get<bap_map_cfg_s>();
}
gnb_du_res_cfg_s& f1ap_elem_procs_o::init_msg_c::gnb_du_res_cfg()
{
  assert_choice_type(types::gnb_du_res_cfg, type_, "InitiatingMessage");
  return c.get<gnb_du_res_cfg_s>();
}
iab_tnl_address_request_s& f1ap_elem_procs_o::init_msg_c::iab_tnl_address_request()
{
  assert_choice_type(types::iab_tnl_address_request, type_, "InitiatingMessage");
  return c.get<iab_tnl_address_request_s>();
}
iab_up_cfg_upd_request_s& f1ap_elem_procs_o::init_msg_c::iab_up_cfg_upd_request()
{
  assert_choice_type(types::iab_up_cfg_upd_request, type_, "InitiatingMessage");
  return c.get<iab_up_cfg_upd_request_s>();
}
res_status_request_s& f1ap_elem_procs_o::init_msg_c::res_status_request()
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
positioning_meas_request_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_request()
{
  assert_choice_type(types::positioning_meas_request, type_, "InitiatingMessage");
  return c.get<positioning_meas_request_s>();
}
trp_info_request_s& f1ap_elem_procs_o::init_msg_c::trp_info_request()
{
  assert_choice_type(types::trp_info_request, type_, "InitiatingMessage");
  return c.get<trp_info_request_s>();
}
positioning_info_request_s& f1ap_elem_procs_o::init_msg_c::positioning_info_request()
{
  assert_choice_type(types::positioning_info_request, type_, "InitiatingMessage");
  return c.get<positioning_info_request_s>();
}
positioning_activation_request_s& f1ap_elem_procs_o::init_msg_c::positioning_activation_request()
{
  assert_choice_type(types::positioning_activation_request, type_, "InitiatingMessage");
  return c.get<positioning_activation_request_s>();
}
e_c_id_meas_initiation_request_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_initiation_request()
{
  assert_choice_type(types::e_c_id_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_initiation_request_s>();
}
broadcast_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_setup_request()
{
  assert_choice_type(types::broadcast_context_setup_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_setup_request_s>();
}
broadcast_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_release_cmd()
{
  assert_choice_type(types::broadcast_context_release_cmd, type_, "InitiatingMessage");
  return c.get<broadcast_context_release_cmd_s>();
}
broadcast_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_mod_request()
{
  assert_choice_type(types::broadcast_context_mod_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_mod_request_s>();
}
multicast_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_setup_request()
{
  assert_choice_type(types::multicast_context_setup_request, type_, "InitiatingMessage");
  return c.get<multicast_context_setup_request_s>();
}
multicast_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::multicast_context_release_cmd()
{
  assert_choice_type(types::multicast_context_release_cmd, type_, "InitiatingMessage");
  return c.get<multicast_context_release_cmd_s>();
}
multicast_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_mod_request()
{
  assert_choice_type(types::multicast_context_mod_request, type_, "InitiatingMessage");
  return c.get<multicast_context_mod_request_s>();
}
multicast_distribution_setup_request_s& f1ap_elem_procs_o::init_msg_c::multicast_distribution_setup_request()
{
  assert_choice_type(types::multicast_distribution_setup_request, type_, "InitiatingMessage");
  return c.get<multicast_distribution_setup_request_s>();
}
multicast_distribution_release_cmd_s& f1ap_elem_procs_o::init_msg_c::multicast_distribution_release_cmd()
{
  assert_choice_type(types::multicast_distribution_release_cmd, type_, "InitiatingMessage");
  return c.get<multicast_distribution_release_cmd_s>();
}
pdc_meas_initiation_request_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_initiation_request()
{
  assert_choice_type(types::pdc_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<pdc_meas_initiation_request_s>();
}
prs_cfg_request_s& f1ap_elem_procs_o::init_msg_c::prs_cfg_request()
{
  assert_choice_type(types::prs_cfg_request, type_, "InitiatingMessage");
  return c.get<prs_cfg_request_s>();
}
meas_precfg_required_s& f1ap_elem_procs_o::init_msg_c::meas_precfg_required()
{
  assert_choice_type(types::meas_precfg_required, type_, "InitiatingMessage");
  return c.get<meas_precfg_required_s>();
}
timing_synchronisation_status_request_s& f1ap_elem_procs_o::init_msg_c::timing_synchronisation_status_request()
{
  assert_choice_type(types::timing_synchronisation_status_request, type_, "InitiatingMessage");
  return c.get<timing_synchronisation_status_request_s>();
}
multicast_context_notif_ind_s& f1ap_elem_procs_o::init_msg_c::multicast_context_notif_ind()
{
  assert_choice_type(types::multicast_context_notif_ind, type_, "InitiatingMessage");
  return c.get<multicast_context_notif_ind_s>();
}
multicast_common_cfg_request_s& f1ap_elem_procs_o::init_msg_c::multicast_common_cfg_request()
{
  assert_choice_type(types::multicast_common_cfg_request, type_, "InitiatingMessage");
  return c.get<multicast_common_cfg_request_s>();
}
error_ind_s& f1ap_elem_procs_o::init_msg_c::error_ind()
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
ue_context_release_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_release_request()
{
  assert_choice_type(types::ue_context_release_request, type_, "InitiatingMessage");
  return c.get<ue_context_release_request_s>();
}
dl_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::dl_rrc_msg_transfer()
{
  assert_choice_type(types::dl_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<dl_rrc_msg_transfer_s>();
}
ul_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::ul_rrc_msg_transfer()
{
  assert_choice_type(types::ul_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<ul_rrc_msg_transfer_s>();
}
ue_inactivity_notif_s& f1ap_elem_procs_o::init_msg_c::ue_inactivity_notif()
{
  assert_choice_type(types::ue_inactivity_notif, type_, "InitiatingMessage");
  return c.get<ue_inactivity_notif_s>();
}
private_msg_s& f1ap_elem_procs_o::init_msg_c::private_msg()
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
init_ul_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::init_ul_rrc_msg_transfer()
{
  assert_choice_type(types::init_ul_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<init_ul_rrc_msg_transfer_s>();
}
sys_info_delivery_cmd_s& f1ap_elem_procs_o::init_msg_c::sys_info_delivery_cmd()
{
  assert_choice_type(types::sys_info_delivery_cmd, type_, "InitiatingMessage");
  return c.get<sys_info_delivery_cmd_s>();
}
paging_s& f1ap_elem_procs_o::init_msg_c::paging()
{
  assert_choice_type(types::paging, type_, "InitiatingMessage");
  return c.get<paging_s>();
}
notify_s& f1ap_elem_procs_o::init_msg_c::notify()
{
  assert_choice_type(types::notify, type_, "InitiatingMessage");
  return c.get<notify_s>();
}
pws_restart_ind_s& f1ap_elem_procs_o::init_msg_c::pws_restart_ind()
{
  assert_choice_type(types::pws_restart_ind, type_, "InitiatingMessage");
  return c.get<pws_restart_ind_s>();
}
pws_fail_ind_s& f1ap_elem_procs_o::init_msg_c::pws_fail_ind()
{
  assert_choice_type(types::pws_fail_ind, type_, "InitiatingMessage");
  return c.get<pws_fail_ind_s>();
}
gnb_du_status_ind_s& f1ap_elem_procs_o::init_msg_c::gnb_du_status_ind()
{
  assert_choice_type(types::gnb_du_status_ind, type_, "InitiatingMessage");
  return c.get<gnb_du_status_ind_s>();
}
rrc_delivery_report_s& f1ap_elem_procs_o::init_msg_c::rrc_delivery_report()
{
  assert_choice_type(types::rrc_delivery_report, type_, "InitiatingMessage");
  return c.get<rrc_delivery_report_s>();
}
network_access_rate_reduction_s& f1ap_elem_procs_o::init_msg_c::network_access_rate_reduction()
{
  assert_choice_type(types::network_access_rate_reduction, type_, "InitiatingMessage");
  return c.get<network_access_rate_reduction_s>();
}
trace_start_s& f1ap_elem_procs_o::init_msg_c::trace_start()
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
deactiv_trace_s& f1ap_elem_procs_o::init_msg_c::deactiv_trace()
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
du_cu_radio_info_transfer_s& f1ap_elem_procs_o::init_msg_c::du_cu_radio_info_transfer()
{
  assert_choice_type(types::du_cu_radio_info_transfer, type_, "InitiatingMessage");
  return c.get<du_cu_radio_info_transfer_s>();
}
cu_du_radio_info_transfer_s& f1ap_elem_procs_o::init_msg_c::cu_du_radio_info_transfer()
{
  assert_choice_type(types::cu_du_radio_info_transfer, type_, "InitiatingMessage");
  return c.get<cu_du_radio_info_transfer_s>();
}
res_status_upd_s& f1ap_elem_procs_o::init_msg_c::res_status_upd()
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
access_and_mob_ind_s& f1ap_elem_procs_o::init_msg_c::access_and_mob_ind()
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
ref_time_info_report_ctrl_s& f1ap_elem_procs_o::init_msg_c::ref_time_info_report_ctrl()
{
  assert_choice_type(types::ref_time_info_report_ctrl, type_, "InitiatingMessage");
  return c.get<ref_time_info_report_ctrl_s>();
}
ref_time_info_report_s& f1ap_elem_procs_o::init_msg_c::ref_time_info_report()
{
  assert_choice_type(types::ref_time_info_report, type_, "InitiatingMessage");
  return c.get<ref_time_info_report_s>();
}
access_success_s& f1ap_elem_procs_o::init_msg_c::access_success()
{
  assert_choice_type(types::access_success, type_, "InitiatingMessage");
  return c.get<access_success_s>();
}
cell_traffic_trace_s& f1ap_elem_procs_o::init_msg_c::cell_traffic_trace()
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
positioning_assist_info_ctrl_s& f1ap_elem_procs_o::init_msg_c::positioning_assist_info_ctrl()
{
  assert_choice_type(types::positioning_assist_info_ctrl, type_, "InitiatingMessage");
  return c.get<positioning_assist_info_ctrl_s>();
}
positioning_assist_info_feedback_s& f1ap_elem_procs_o::init_msg_c::positioning_assist_info_feedback()
{
  assert_choice_type(types::positioning_assist_info_feedback, type_, "InitiatingMessage");
  return c.get<positioning_assist_info_feedback_s>();
}
positioning_meas_report_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_report()
{
  assert_choice_type(types::positioning_meas_report, type_, "InitiatingMessage");
  return c.get<positioning_meas_report_s>();
}
positioning_meas_abort_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_abort()
{
  assert_choice_type(types::positioning_meas_abort, type_, "InitiatingMessage");
  return c.get<positioning_meas_abort_s>();
}
positioning_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_fail_ind()
{
  assert_choice_type(types::positioning_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<positioning_meas_fail_ind_s>();
}
positioning_meas_upd_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_upd()
{
  assert_choice_type(types::positioning_meas_upd, type_, "InitiatingMessage");
  return c.get<positioning_meas_upd_s>();
}
positioning_deactivation_s& f1ap_elem_procs_o::init_msg_c::positioning_deactivation()
{
  assert_choice_type(types::positioning_deactivation, type_, "InitiatingMessage");
  return c.get<positioning_deactivation_s>();
}
e_c_id_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_fail_ind()
{
  assert_choice_type(types::e_c_id_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_fail_ind_s>();
}
e_c_id_meas_report_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_report()
{
  assert_choice_type(types::e_c_id_meas_report, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_report_s>();
}
e_c_id_meas_termination_cmd_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_termination_cmd()
{
  assert_choice_type(types::e_c_id_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_termination_cmd_s>();
}
positioning_info_upd_s& f1ap_elem_procs_o::init_msg_c::positioning_info_upd()
{
  assert_choice_type(types::positioning_info_upd, type_, "InitiatingMessage");
  return c.get<positioning_info_upd_s>();
}
multicast_group_paging_s& f1ap_elem_procs_o::init_msg_c::multicast_group_paging()
{
  assert_choice_type(types::multicast_group_paging, type_, "InitiatingMessage");
  return c.get<multicast_group_paging_s>();
}
broadcast_context_release_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_release_request()
{
  assert_choice_type(types::broadcast_context_release_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_release_request_s>();
}
multicast_context_release_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_release_request()
{
  assert_choice_type(types::multicast_context_release_request, type_, "InitiatingMessage");
  return c.get<multicast_context_release_request_s>();
}
pdc_meas_report_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_report()
{
  assert_choice_type(types::pdc_meas_report, type_, "InitiatingMessage");
  return c.get<pdc_meas_report_s>();
}
pdc_meas_termination_cmd_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_termination_cmd()
{
  assert_choice_type(types::pdc_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<pdc_meas_termination_cmd_s>();
}
pdc_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_fail_ind()
{
  assert_choice_type(types::pdc_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<pdc_meas_fail_ind_s>();
}
meas_activation_s& f1ap_elem_procs_o::init_msg_c::meas_activation()
{
  assert_choice_type(types::meas_activation, type_, "InitiatingMessage");
  return c.get<meas_activation_s>();
}
qo_e_info_transfer_s& f1ap_elem_procs_o::init_msg_c::qo_e_info_transfer()
{
  assert_choice_type(types::qo_e_info_transfer, type_, "InitiatingMessage");
  return c.get<qo_e_info_transfer_s>();
}
pos_sys_info_delivery_cmd_s& f1ap_elem_procs_o::init_msg_c::pos_sys_info_delivery_cmd()
{
  assert_choice_type(types::pos_sys_info_delivery_cmd, type_, "InitiatingMessage");
  return c.get<pos_sys_info_delivery_cmd_s>();
}
du_cu_cell_switch_notif_s& f1ap_elem_procs_o::init_msg_c::du_cu_cell_switch_notif()
{
  assert_choice_type(types::du_cu_cell_switch_notif, type_, "InitiatingMessage");
  return c.get<du_cu_cell_switch_notif_s>();
}
cu_du_cell_switch_notif_s& f1ap_elem_procs_o::init_msg_c::cu_du_cell_switch_notif()
{
  assert_choice_type(types::cu_du_cell_switch_notif, type_, "InitiatingMessage");
  return c.get<cu_du_cell_switch_notif_s>();
}
du_cu_tai_nformation_transfer_s& f1ap_elem_procs_o::init_msg_c::du_cu_tai_nformation_transfer()
{
  assert_choice_type(types::du_cu_tai_nformation_transfer, type_, "InitiatingMessage");
  return c.get<du_cu_tai_nformation_transfer_s>();
}
cu_du_tai_nformation_transfer_s& f1ap_elem_procs_o::init_msg_c::cu_du_tai_nformation_transfer()
{
  assert_choice_type(types::cu_du_tai_nformation_transfer, type_, "InitiatingMessage");
  return c.get<cu_du_tai_nformation_transfer_s>();
}
qo_e_info_transfer_ctrl_s& f1ap_elem_procs_o::init_msg_c::qo_e_info_transfer_ctrl()
{
  assert_choice_type(types::qo_e_info_transfer_ctrl, type_, "InitiatingMessage");
  return c.get<qo_e_info_transfer_ctrl_s>();
}
rach_ind_s& f1ap_elem_procs_o::init_msg_c::rach_ind()
{
  assert_choice_type(types::rach_ind, type_, "InitiatingMessage");
  return c.get<rach_ind_s>();
}
timing_synchronisation_status_report_s& f1ap_elem_procs_o::init_msg_c::timing_synchronisation_status_report()
{
  assert_choice_type(types::timing_synchronisation_status_report, type_, "InitiatingMessage");
  return c.get<timing_synchronisation_status_report_s>();
}
miabf1_setup_trigger_s& f1ap_elem_procs_o::init_msg_c::miabf1_setup_trigger()
{
  assert_choice_type(types::miabf1_setup_trigger, type_, "InitiatingMessage");
  return c.get<miabf1_setup_trigger_s>();
}
miabf1_setup_outcome_notif_s& f1ap_elem_procs_o::init_msg_c::miabf1_setup_outcome_notif()
{
  assert_choice_type(types::miabf1_setup_outcome_notif, type_, "InitiatingMessage");
  return c.get<miabf1_setup_outcome_notif_s>();
}
broadcast_transport_res_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_transport_res_request()
{
  assert_choice_type(types::broadcast_transport_res_request, type_, "InitiatingMessage");
  return c.get<broadcast_transport_res_request_s>();
}
du_cu_access_and_mob_ind_s& f1ap_elem_procs_o::init_msg_c::du_cu_access_and_mob_ind()
{
  assert_choice_type(types::du_cu_access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<du_cu_access_and_mob_ind_s>();
}
srs_info_reserv_notif_s& f1ap_elem_procs_o::init_msg_c::srs_info_reserv_notif()
{
  assert_choice_type(types::srs_info_reserv_notif, type_, "InitiatingMessage");
  return c.get<srs_info_reserv_notif_s>();
}
cu_du_mob_initiation_request_s& f1ap_elem_procs_o::init_msg_c::cu_du_mob_initiation_request()
{
  assert_choice_type(types::cu_du_mob_initiation_request, type_, "InitiatingMessage");
  return c.get<cu_du_mob_initiation_request_s>();
}
const reset_s& f1ap_elem_procs_o::init_msg_c::reset() const
{
  assert_choice_type(types::reset, type_, "InitiatingMessage");
  return c.get<reset_s>();
}
const f1_setup_request_s& f1ap_elem_procs_o::init_msg_c::f1_setup_request() const
{
  assert_choice_type(types::f1_setup_request, type_, "InitiatingMessage");
  return c.get<f1_setup_request_s>();
}
const gnb_du_cfg_upd_s& f1ap_elem_procs_o::init_msg_c::gnb_du_cfg_upd() const
{
  assert_choice_type(types::gnb_du_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_du_cfg_upd_s>();
}
const gnb_cu_cfg_upd_s& f1ap_elem_procs_o::init_msg_c::gnb_cu_cfg_upd() const
{
  assert_choice_type(types::gnb_cu_cfg_upd, type_, "InitiatingMessage");
  return c.get<gnb_cu_cfg_upd_s>();
}
const ue_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_setup_request() const
{
  assert_choice_type(types::ue_context_setup_request, type_, "InitiatingMessage");
  return c.get<ue_context_setup_request_s>();
}
const ue_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::ue_context_release_cmd() const
{
  assert_choice_type(types::ue_context_release_cmd, type_, "InitiatingMessage");
  return c.get<ue_context_release_cmd_s>();
}
const ue_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_mod_request() const
{
  assert_choice_type(types::ue_context_mod_request, type_, "InitiatingMessage");
  return c.get<ue_context_mod_request_s>();
}
const ue_context_mod_required_s& f1ap_elem_procs_o::init_msg_c::ue_context_mod_required() const
{
  assert_choice_type(types::ue_context_mod_required, type_, "InitiatingMessage");
  return c.get<ue_context_mod_required_s>();
}
const write_replace_warning_request_s& f1ap_elem_procs_o::init_msg_c::write_replace_warning_request() const
{
  assert_choice_type(types::write_replace_warning_request, type_, "InitiatingMessage");
  return c.get<write_replace_warning_request_s>();
}
const pws_cancel_request_s& f1ap_elem_procs_o::init_msg_c::pws_cancel_request() const
{
  assert_choice_type(types::pws_cancel_request, type_, "InitiatingMessage");
  return c.get<pws_cancel_request_s>();
}
const gnb_du_res_coordination_request_s& f1ap_elem_procs_o::init_msg_c::gnb_du_res_coordination_request() const
{
  assert_choice_type(types::gnb_du_res_coordination_request, type_, "InitiatingMessage");
  return c.get<gnb_du_res_coordination_request_s>();
}
const f1_removal_request_s& f1ap_elem_procs_o::init_msg_c::f1_removal_request() const
{
  assert_choice_type(types::f1_removal_request, type_, "InitiatingMessage");
  return c.get<f1_removal_request_s>();
}
const bap_map_cfg_s& f1ap_elem_procs_o::init_msg_c::bap_map_cfg() const
{
  assert_choice_type(types::bap_map_cfg, type_, "InitiatingMessage");
  return c.get<bap_map_cfg_s>();
}
const gnb_du_res_cfg_s& f1ap_elem_procs_o::init_msg_c::gnb_du_res_cfg() const
{
  assert_choice_type(types::gnb_du_res_cfg, type_, "InitiatingMessage");
  return c.get<gnb_du_res_cfg_s>();
}
const iab_tnl_address_request_s& f1ap_elem_procs_o::init_msg_c::iab_tnl_address_request() const
{
  assert_choice_type(types::iab_tnl_address_request, type_, "InitiatingMessage");
  return c.get<iab_tnl_address_request_s>();
}
const iab_up_cfg_upd_request_s& f1ap_elem_procs_o::init_msg_c::iab_up_cfg_upd_request() const
{
  assert_choice_type(types::iab_up_cfg_upd_request, type_, "InitiatingMessage");
  return c.get<iab_up_cfg_upd_request_s>();
}
const res_status_request_s& f1ap_elem_procs_o::init_msg_c::res_status_request() const
{
  assert_choice_type(types::res_status_request, type_, "InitiatingMessage");
  return c.get<res_status_request_s>();
}
const positioning_meas_request_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_request() const
{
  assert_choice_type(types::positioning_meas_request, type_, "InitiatingMessage");
  return c.get<positioning_meas_request_s>();
}
const trp_info_request_s& f1ap_elem_procs_o::init_msg_c::trp_info_request() const
{
  assert_choice_type(types::trp_info_request, type_, "InitiatingMessage");
  return c.get<trp_info_request_s>();
}
const positioning_info_request_s& f1ap_elem_procs_o::init_msg_c::positioning_info_request() const
{
  assert_choice_type(types::positioning_info_request, type_, "InitiatingMessage");
  return c.get<positioning_info_request_s>();
}
const positioning_activation_request_s& f1ap_elem_procs_o::init_msg_c::positioning_activation_request() const
{
  assert_choice_type(types::positioning_activation_request, type_, "InitiatingMessage");
  return c.get<positioning_activation_request_s>();
}
const e_c_id_meas_initiation_request_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_initiation_request() const
{
  assert_choice_type(types::e_c_id_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_initiation_request_s>();
}
const broadcast_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_setup_request() const
{
  assert_choice_type(types::broadcast_context_setup_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_setup_request_s>();
}
const broadcast_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_release_cmd() const
{
  assert_choice_type(types::broadcast_context_release_cmd, type_, "InitiatingMessage");
  return c.get<broadcast_context_release_cmd_s>();
}
const broadcast_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_mod_request() const
{
  assert_choice_type(types::broadcast_context_mod_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_mod_request_s>();
}
const multicast_context_setup_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_setup_request() const
{
  assert_choice_type(types::multicast_context_setup_request, type_, "InitiatingMessage");
  return c.get<multicast_context_setup_request_s>();
}
const multicast_context_release_cmd_s& f1ap_elem_procs_o::init_msg_c::multicast_context_release_cmd() const
{
  assert_choice_type(types::multicast_context_release_cmd, type_, "InitiatingMessage");
  return c.get<multicast_context_release_cmd_s>();
}
const multicast_context_mod_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_mod_request() const
{
  assert_choice_type(types::multicast_context_mod_request, type_, "InitiatingMessage");
  return c.get<multicast_context_mod_request_s>();
}
const multicast_distribution_setup_request_s&
f1ap_elem_procs_o::init_msg_c::multicast_distribution_setup_request() const
{
  assert_choice_type(types::multicast_distribution_setup_request, type_, "InitiatingMessage");
  return c.get<multicast_distribution_setup_request_s>();
}
const multicast_distribution_release_cmd_s& f1ap_elem_procs_o::init_msg_c::multicast_distribution_release_cmd() const
{
  assert_choice_type(types::multicast_distribution_release_cmd, type_, "InitiatingMessage");
  return c.get<multicast_distribution_release_cmd_s>();
}
const pdc_meas_initiation_request_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_initiation_request() const
{
  assert_choice_type(types::pdc_meas_initiation_request, type_, "InitiatingMessage");
  return c.get<pdc_meas_initiation_request_s>();
}
const prs_cfg_request_s& f1ap_elem_procs_o::init_msg_c::prs_cfg_request() const
{
  assert_choice_type(types::prs_cfg_request, type_, "InitiatingMessage");
  return c.get<prs_cfg_request_s>();
}
const meas_precfg_required_s& f1ap_elem_procs_o::init_msg_c::meas_precfg_required() const
{
  assert_choice_type(types::meas_precfg_required, type_, "InitiatingMessage");
  return c.get<meas_precfg_required_s>();
}
const timing_synchronisation_status_request_s&
f1ap_elem_procs_o::init_msg_c::timing_synchronisation_status_request() const
{
  assert_choice_type(types::timing_synchronisation_status_request, type_, "InitiatingMessage");
  return c.get<timing_synchronisation_status_request_s>();
}
const multicast_context_notif_ind_s& f1ap_elem_procs_o::init_msg_c::multicast_context_notif_ind() const
{
  assert_choice_type(types::multicast_context_notif_ind, type_, "InitiatingMessage");
  return c.get<multicast_context_notif_ind_s>();
}
const multicast_common_cfg_request_s& f1ap_elem_procs_o::init_msg_c::multicast_common_cfg_request() const
{
  assert_choice_type(types::multicast_common_cfg_request, type_, "InitiatingMessage");
  return c.get<multicast_common_cfg_request_s>();
}
const error_ind_s& f1ap_elem_procs_o::init_msg_c::error_ind() const
{
  assert_choice_type(types::error_ind, type_, "InitiatingMessage");
  return c.get<error_ind_s>();
}
const ue_context_release_request_s& f1ap_elem_procs_o::init_msg_c::ue_context_release_request() const
{
  assert_choice_type(types::ue_context_release_request, type_, "InitiatingMessage");
  return c.get<ue_context_release_request_s>();
}
const dl_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::dl_rrc_msg_transfer() const
{
  assert_choice_type(types::dl_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<dl_rrc_msg_transfer_s>();
}
const ul_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::ul_rrc_msg_transfer() const
{
  assert_choice_type(types::ul_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<ul_rrc_msg_transfer_s>();
}
const ue_inactivity_notif_s& f1ap_elem_procs_o::init_msg_c::ue_inactivity_notif() const
{
  assert_choice_type(types::ue_inactivity_notif, type_, "InitiatingMessage");
  return c.get<ue_inactivity_notif_s>();
}
const private_msg_s& f1ap_elem_procs_o::init_msg_c::private_msg() const
{
  assert_choice_type(types::private_msg, type_, "InitiatingMessage");
  return c.get<private_msg_s>();
}
const init_ul_rrc_msg_transfer_s& f1ap_elem_procs_o::init_msg_c::init_ul_rrc_msg_transfer() const
{
  assert_choice_type(types::init_ul_rrc_msg_transfer, type_, "InitiatingMessage");
  return c.get<init_ul_rrc_msg_transfer_s>();
}
const sys_info_delivery_cmd_s& f1ap_elem_procs_o::init_msg_c::sys_info_delivery_cmd() const
{
  assert_choice_type(types::sys_info_delivery_cmd, type_, "InitiatingMessage");
  return c.get<sys_info_delivery_cmd_s>();
}
const paging_s& f1ap_elem_procs_o::init_msg_c::paging() const
{
  assert_choice_type(types::paging, type_, "InitiatingMessage");
  return c.get<paging_s>();
}
const notify_s& f1ap_elem_procs_o::init_msg_c::notify() const
{
  assert_choice_type(types::notify, type_, "InitiatingMessage");
  return c.get<notify_s>();
}
const pws_restart_ind_s& f1ap_elem_procs_o::init_msg_c::pws_restart_ind() const
{
  assert_choice_type(types::pws_restart_ind, type_, "InitiatingMessage");
  return c.get<pws_restart_ind_s>();
}
const pws_fail_ind_s& f1ap_elem_procs_o::init_msg_c::pws_fail_ind() const
{
  assert_choice_type(types::pws_fail_ind, type_, "InitiatingMessage");
  return c.get<pws_fail_ind_s>();
}
const gnb_du_status_ind_s& f1ap_elem_procs_o::init_msg_c::gnb_du_status_ind() const
{
  assert_choice_type(types::gnb_du_status_ind, type_, "InitiatingMessage");
  return c.get<gnb_du_status_ind_s>();
}
const rrc_delivery_report_s& f1ap_elem_procs_o::init_msg_c::rrc_delivery_report() const
{
  assert_choice_type(types::rrc_delivery_report, type_, "InitiatingMessage");
  return c.get<rrc_delivery_report_s>();
}
const network_access_rate_reduction_s& f1ap_elem_procs_o::init_msg_c::network_access_rate_reduction() const
{
  assert_choice_type(types::network_access_rate_reduction, type_, "InitiatingMessage");
  return c.get<network_access_rate_reduction_s>();
}
const trace_start_s& f1ap_elem_procs_o::init_msg_c::trace_start() const
{
  assert_choice_type(types::trace_start, type_, "InitiatingMessage");
  return c.get<trace_start_s>();
}
const deactiv_trace_s& f1ap_elem_procs_o::init_msg_c::deactiv_trace() const
{
  assert_choice_type(types::deactiv_trace, type_, "InitiatingMessage");
  return c.get<deactiv_trace_s>();
}
const du_cu_radio_info_transfer_s& f1ap_elem_procs_o::init_msg_c::du_cu_radio_info_transfer() const
{
  assert_choice_type(types::du_cu_radio_info_transfer, type_, "InitiatingMessage");
  return c.get<du_cu_radio_info_transfer_s>();
}
const cu_du_radio_info_transfer_s& f1ap_elem_procs_o::init_msg_c::cu_du_radio_info_transfer() const
{
  assert_choice_type(types::cu_du_radio_info_transfer, type_, "InitiatingMessage");
  return c.get<cu_du_radio_info_transfer_s>();
}
const res_status_upd_s& f1ap_elem_procs_o::init_msg_c::res_status_upd() const
{
  assert_choice_type(types::res_status_upd, type_, "InitiatingMessage");
  return c.get<res_status_upd_s>();
}
const access_and_mob_ind_s& f1ap_elem_procs_o::init_msg_c::access_and_mob_ind() const
{
  assert_choice_type(types::access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<access_and_mob_ind_s>();
}
const ref_time_info_report_ctrl_s& f1ap_elem_procs_o::init_msg_c::ref_time_info_report_ctrl() const
{
  assert_choice_type(types::ref_time_info_report_ctrl, type_, "InitiatingMessage");
  return c.get<ref_time_info_report_ctrl_s>();
}
const ref_time_info_report_s& f1ap_elem_procs_o::init_msg_c::ref_time_info_report() const
{
  assert_choice_type(types::ref_time_info_report, type_, "InitiatingMessage");
  return c.get<ref_time_info_report_s>();
}
const access_success_s& f1ap_elem_procs_o::init_msg_c::access_success() const
{
  assert_choice_type(types::access_success, type_, "InitiatingMessage");
  return c.get<access_success_s>();
}
const cell_traffic_trace_s& f1ap_elem_procs_o::init_msg_c::cell_traffic_trace() const
{
  assert_choice_type(types::cell_traffic_trace, type_, "InitiatingMessage");
  return c.get<cell_traffic_trace_s>();
}
const positioning_assist_info_ctrl_s& f1ap_elem_procs_o::init_msg_c::positioning_assist_info_ctrl() const
{
  assert_choice_type(types::positioning_assist_info_ctrl, type_, "InitiatingMessage");
  return c.get<positioning_assist_info_ctrl_s>();
}
const positioning_assist_info_feedback_s& f1ap_elem_procs_o::init_msg_c::positioning_assist_info_feedback() const
{
  assert_choice_type(types::positioning_assist_info_feedback, type_, "InitiatingMessage");
  return c.get<positioning_assist_info_feedback_s>();
}
const positioning_meas_report_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_report() const
{
  assert_choice_type(types::positioning_meas_report, type_, "InitiatingMessage");
  return c.get<positioning_meas_report_s>();
}
const positioning_meas_abort_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_abort() const
{
  assert_choice_type(types::positioning_meas_abort, type_, "InitiatingMessage");
  return c.get<positioning_meas_abort_s>();
}
const positioning_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_fail_ind() const
{
  assert_choice_type(types::positioning_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<positioning_meas_fail_ind_s>();
}
const positioning_meas_upd_s& f1ap_elem_procs_o::init_msg_c::positioning_meas_upd() const
{
  assert_choice_type(types::positioning_meas_upd, type_, "InitiatingMessage");
  return c.get<positioning_meas_upd_s>();
}
const positioning_deactivation_s& f1ap_elem_procs_o::init_msg_c::positioning_deactivation() const
{
  assert_choice_type(types::positioning_deactivation, type_, "InitiatingMessage");
  return c.get<positioning_deactivation_s>();
}
const e_c_id_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_fail_ind() const
{
  assert_choice_type(types::e_c_id_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_fail_ind_s>();
}
const e_c_id_meas_report_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_report() const
{
  assert_choice_type(types::e_c_id_meas_report, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_report_s>();
}
const e_c_id_meas_termination_cmd_s& f1ap_elem_procs_o::init_msg_c::e_c_id_meas_termination_cmd() const
{
  assert_choice_type(types::e_c_id_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<e_c_id_meas_termination_cmd_s>();
}
const positioning_info_upd_s& f1ap_elem_procs_o::init_msg_c::positioning_info_upd() const
{
  assert_choice_type(types::positioning_info_upd, type_, "InitiatingMessage");
  return c.get<positioning_info_upd_s>();
}
const multicast_group_paging_s& f1ap_elem_procs_o::init_msg_c::multicast_group_paging() const
{
  assert_choice_type(types::multicast_group_paging, type_, "InitiatingMessage");
  return c.get<multicast_group_paging_s>();
}
const broadcast_context_release_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_context_release_request() const
{
  assert_choice_type(types::broadcast_context_release_request, type_, "InitiatingMessage");
  return c.get<broadcast_context_release_request_s>();
}
const multicast_context_release_request_s& f1ap_elem_procs_o::init_msg_c::multicast_context_release_request() const
{
  assert_choice_type(types::multicast_context_release_request, type_, "InitiatingMessage");
  return c.get<multicast_context_release_request_s>();
}
const pdc_meas_report_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_report() const
{
  assert_choice_type(types::pdc_meas_report, type_, "InitiatingMessage");
  return c.get<pdc_meas_report_s>();
}
const pdc_meas_termination_cmd_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_termination_cmd() const
{
  assert_choice_type(types::pdc_meas_termination_cmd, type_, "InitiatingMessage");
  return c.get<pdc_meas_termination_cmd_s>();
}
const pdc_meas_fail_ind_s& f1ap_elem_procs_o::init_msg_c::pdc_meas_fail_ind() const
{
  assert_choice_type(types::pdc_meas_fail_ind, type_, "InitiatingMessage");
  return c.get<pdc_meas_fail_ind_s>();
}
const meas_activation_s& f1ap_elem_procs_o::init_msg_c::meas_activation() const
{
  assert_choice_type(types::meas_activation, type_, "InitiatingMessage");
  return c.get<meas_activation_s>();
}
const qo_e_info_transfer_s& f1ap_elem_procs_o::init_msg_c::qo_e_info_transfer() const
{
  assert_choice_type(types::qo_e_info_transfer, type_, "InitiatingMessage");
  return c.get<qo_e_info_transfer_s>();
}
const pos_sys_info_delivery_cmd_s& f1ap_elem_procs_o::init_msg_c::pos_sys_info_delivery_cmd() const
{
  assert_choice_type(types::pos_sys_info_delivery_cmd, type_, "InitiatingMessage");
  return c.get<pos_sys_info_delivery_cmd_s>();
}
const du_cu_cell_switch_notif_s& f1ap_elem_procs_o::init_msg_c::du_cu_cell_switch_notif() const
{
  assert_choice_type(types::du_cu_cell_switch_notif, type_, "InitiatingMessage");
  return c.get<du_cu_cell_switch_notif_s>();
}
const cu_du_cell_switch_notif_s& f1ap_elem_procs_o::init_msg_c::cu_du_cell_switch_notif() const
{
  assert_choice_type(types::cu_du_cell_switch_notif, type_, "InitiatingMessage");
  return c.get<cu_du_cell_switch_notif_s>();
}
const du_cu_tai_nformation_transfer_s& f1ap_elem_procs_o::init_msg_c::du_cu_tai_nformation_transfer() const
{
  assert_choice_type(types::du_cu_tai_nformation_transfer, type_, "InitiatingMessage");
  return c.get<du_cu_tai_nformation_transfer_s>();
}
const cu_du_tai_nformation_transfer_s& f1ap_elem_procs_o::init_msg_c::cu_du_tai_nformation_transfer() const
{
  assert_choice_type(types::cu_du_tai_nformation_transfer, type_, "InitiatingMessage");
  return c.get<cu_du_tai_nformation_transfer_s>();
}
const qo_e_info_transfer_ctrl_s& f1ap_elem_procs_o::init_msg_c::qo_e_info_transfer_ctrl() const
{
  assert_choice_type(types::qo_e_info_transfer_ctrl, type_, "InitiatingMessage");
  return c.get<qo_e_info_transfer_ctrl_s>();
}
const rach_ind_s& f1ap_elem_procs_o::init_msg_c::rach_ind() const
{
  assert_choice_type(types::rach_ind, type_, "InitiatingMessage");
  return c.get<rach_ind_s>();
}
const timing_synchronisation_status_report_s&
f1ap_elem_procs_o::init_msg_c::timing_synchronisation_status_report() const
{
  assert_choice_type(types::timing_synchronisation_status_report, type_, "InitiatingMessage");
  return c.get<timing_synchronisation_status_report_s>();
}
const miabf1_setup_trigger_s& f1ap_elem_procs_o::init_msg_c::miabf1_setup_trigger() const
{
  assert_choice_type(types::miabf1_setup_trigger, type_, "InitiatingMessage");
  return c.get<miabf1_setup_trigger_s>();
}
const miabf1_setup_outcome_notif_s& f1ap_elem_procs_o::init_msg_c::miabf1_setup_outcome_notif() const
{
  assert_choice_type(types::miabf1_setup_outcome_notif, type_, "InitiatingMessage");
  return c.get<miabf1_setup_outcome_notif_s>();
}
const broadcast_transport_res_request_s& f1ap_elem_procs_o::init_msg_c::broadcast_transport_res_request() const
{
  assert_choice_type(types::broadcast_transport_res_request, type_, "InitiatingMessage");
  return c.get<broadcast_transport_res_request_s>();
}
const du_cu_access_and_mob_ind_s& f1ap_elem_procs_o::init_msg_c::du_cu_access_and_mob_ind() const
{
  assert_choice_type(types::du_cu_access_and_mob_ind, type_, "InitiatingMessage");
  return c.get<du_cu_access_and_mob_ind_s>();
}
const srs_info_reserv_notif_s& f1ap_elem_procs_o::init_msg_c::srs_info_reserv_notif() const
{
  assert_choice_type(types::srs_info_reserv_notif, type_, "InitiatingMessage");
  return c.get<srs_info_reserv_notif_s>();
}
const cu_du_mob_initiation_request_s& f1ap_elem_procs_o::init_msg_c::cu_du_mob_initiation_request() const
{
  assert_choice_type(types::cu_du_mob_initiation_request, type_, "InitiatingMessage");
  return c.get<cu_du_mob_initiation_request_s>();
}

static const char*           enum_names_f1ap_elem_procs_o_init_msg_c_types_values[] = {"Reset",
                                                                                       "F1SetupRequest",
                                                                                       "GNBDUConfigurationUpdate",
                                                                                       "GNBCUConfigurationUpdate",
                                                                                       "UEContextSetupRequest",
                                                                                       "UEContextReleaseCommand",
                                                                                       "UEContextModificationRequest",
                                                                                       "UEContextModificationRequired",
                                                                                       "WriteReplaceWarningRequest",
                                                                                       "PWSCancelRequest",
                                                                                       "GNBDUResourceCoordinationRequest",
                                                                                       "F1RemovalRequest",
                                                                                       "BAPMappingConfiguration",
                                                                                       "GNBDUResourceConfiguration",
                                                                                       "IABTNLAddressRequest",
                                                                                       "IABUPConfigurationUpdateRequest",
                                                                                       "ResourceStatusRequest",
                                                                                       "PositioningMeasurementRequest",
                                                                                       "TRPInformationRequest",
                                                                                       "PositioningInformationRequest",
                                                                                       "PositioningActivationRequest",
                                                                                       "E-CIDMeasurementInitiationRequest",
                                                                                       "BroadcastContextSetupRequest",
                                                                                       "BroadcastContextReleaseCommand",
                                                                                       "BroadcastContextModificationRequest",
                                                                                       "MulticastContextSetupRequest",
                                                                                       "MulticastContextReleaseCommand",
                                                                                       "MulticastContextModificationRequest",
                                                                                       "MulticastDistributionSetupRequest",
                                                                                       "MulticastDistributionReleaseCommand",
                                                                                       "PDCMeasurementInitiationRequest",
                                                                                       "PRSConfigurationRequest",
                                                                                       "MeasurementPreconfigurationRequired",
                                                                                       "TimingSynchronisationStatusRequest",
                                                                                       "MulticastContextNotificationIndication",
                                                                                       "MulticastCommonConfigurationRequest",
                                                                                       "ErrorIndication",
                                                                                       "UEContextReleaseRequest",
                                                                                       "DLRRCMessageTransfer",
                                                                                       "ULRRCMessageTransfer",
                                                                                       "UEInactivityNotification",
                                                                                       "PrivateMessage",
                                                                                       "InitialULRRCMessageTransfer",
                                                                                       "SystemInformationDeliveryCommand",
                                                                                       "Paging",
                                                                                       "Notify",
                                                                                       "PWSRestartIndication",
                                                                                       "PWSFailureIndication",
                                                                                       "GNBDUStatusIndication",
                                                                                       "RRCDeliveryReport",
                                                                                       "NetworkAccessRateReduction",
                                                                                       "TraceStart",
                                                                                       "DeactivateTrace",
                                                                                       "DUCURadioInformationTransfer",
                                                                                       "CUDURadioInformationTransfer",
                                                                                       "ResourceStatusUpdate",
                                                                                       "AccessAndMobilityIndication",
                                                                                       "ReferenceTimeInformationReportingControl",
                                                                                       "ReferenceTimeInformationReport",
                                                                                       "AccessSuccess",
                                                                                       "CellTrafficTrace",
                                                                                       "PositioningAssistanceInformationControl",
                                                                                       "PositioningAssistanceInformationFeedback",
                                                                                       "PositioningMeasurementReport",
                                                                                       "PositioningMeasurementAbort",
                                                                                       "PositioningMeasurementFailureIndication",
                                                                                       "PositioningMeasurementUpdate",
                                                                                       "PositioningDeactivation",
                                                                                       "E-CIDMeasurementFailureIndication",
                                                                                       "E-CIDMeasurementReport",
                                                                                       "E-CIDMeasurementTerminationCommand",
                                                                                       "PositioningInformationUpdate",
                                                                                       "MulticastGroupPaging",
                                                                                       "BroadcastContextReleaseRequest",
                                                                                       "MulticastContextReleaseRequest",
                                                                                       "PDCMeasurementReport",
                                                                                       "PDCMeasurementTerminationCommand",
                                                                                       "PDCMeasurementFailureIndication",
                                                                                       "MeasurementActivation",
                                                                                       "QoEInformationTransfer",
                                                                                       "PosSystemInformationDeliveryCommand",
                                                                                       "DUCUCellSwitchNotification",
                                                                                       "CUDUCellSwitchNotification",
                                                                                       "DUCUTAInformationTransfer",
                                                                                       "CUDUTAInformationTransfer",
                                                                                       "QoEInformationTransferControl",
                                                                                       "RachIndication",
                                                                                       "TimingSynchronisationStatusReport",
                                                                                       "MIABF1SetupTriggering",
                                                                                       "MIABF1SetupOutcomeNotification",
                                                                                       "BroadcastTransportResourceRequest",
                                                                                       "DUCUAccessAndMobilityIndication",
                                                                                       "SRSInformationReservationNotification",
                                                                                       "CUDUMobilityInitiationRequest"};
const asn1::table::enum_desc f1ap_elem_procs_o::init_msg_c::types_opts::desc        = {
    enum_names_f1ap_elem_procs_o_init_msg_c_types_values,
    94,
    "f1ap_elem_procs_o::init_msg_c::types"};

// SuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_f1ap_elem_procs_o_successful_outcome_c_alternatives[] = {
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
     &asn1::table::any_codec<f1_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_du_cfg_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_cfg_upd_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_setup_resp_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ue_context_mod_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_mod_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<write_replace_warning_resp_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<gnb_du_res_coordination_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<f1_removal_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bap_map_cfg_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_du_res_cfg_ack_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_tnl_address_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_up_cfg_upd_resp_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<positioning_meas_resp_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<positioning_info_resp_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<e_c_id_meas_initiation_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_context_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_context_release_complete_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_context_mod_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_context_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_context_release_complete_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_context_mod_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_distribution_setup_resp_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_distribution_release_complete_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdc_meas_initiation_resp_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<meas_precfg_confirm_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<multicast_context_notif_confirm_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_common_cfg_resp_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_f1ap_elem_procs_o_successful_outcome_c_names[] = {
    {"ResetAcknowledge", nullptr},
    {"F1SetupResponse", nullptr},
    {"GNBDUConfigurationUpdateAcknowledge", nullptr},
    {"GNBCUConfigurationUpdateAcknowledge", nullptr},
    {"UEContextSetupResponse", nullptr},
    {"UEContextReleaseComplete", nullptr},
    {"UEContextModificationResponse", nullptr},
    {"UEContextModificationConfirm", nullptr},
    {"WriteReplaceWarningResponse", nullptr},
    {"PWSCancelResponse", nullptr},
    {"GNBDUResourceCoordinationResponse", nullptr},
    {"F1RemovalResponse", nullptr},
    {"BAPMappingConfigurationAcknowledge", nullptr},
    {"GNBDUResourceConfigurationAcknowledge", nullptr},
    {"IABTNLAddressResponse", nullptr},
    {"IABUPConfigurationUpdateResponse", nullptr},
    {"ResourceStatusResponse", nullptr},
    {"PositioningMeasurementResponse", nullptr},
    {"TRPInformationResponse", nullptr},
    {"PositioningInformationResponse", nullptr},
    {"PositioningActivationResponse", nullptr},
    {"E-CIDMeasurementInitiationResponse", nullptr},
    {"BroadcastContextSetupResponse", nullptr},
    {"BroadcastContextReleaseComplete", nullptr},
    {"BroadcastContextModificationResponse", nullptr},
    {"MulticastContextSetupResponse", nullptr},
    {"MulticastContextReleaseComplete", nullptr},
    {"MulticastContextModificationResponse", nullptr},
    {"MulticastDistributionSetupResponse", nullptr},
    {"MulticastDistributionReleaseComplete", nullptr},
    {"PDCMeasurementInitiationResponse", nullptr},
    {"PRSConfigurationResponse", nullptr},
    {"MeasurementPreconfigurationConfirm", nullptr},
    {"TimingSynchronisationStatusResponse", nullptr},
    {"MulticastContextNotificationConfirm", nullptr},
    {"MulticastCommonConfigurationResponse", nullptr}};
const asn1::table::choice_desc f1ap_elem_procs_o::successful_outcome_c::desc = {
    asn1::table::enum_int_type<f1ap_elem_procs_o::successful_outcome_c::types>(),
    f1ap_elem_procs_o::successful_outcome_c::types::nof_types,
    f1ap_elem_procs_o::successful_outcome_c::types::nof_exts,
    f1ap_elem_procs_o::successful_outcome_c::types::has_ext,
    true,
    choice_table_f1ap_elem_procs_o_successful_outcome_c_alternatives,
    &asn1::table::choice_set<f1ap_elem_procs_o::successful_outcome_c>,
    choice_table_f1ap_elem_procs_o_successful_outcome_c_names,
    "f1ap_elem_procs_o::successful_outcome_c",
    offsetof(f1ap_elem_procs_o::successful_outcome_c, type_),
    offsetof(f1ap_elem_procs_o::successful_outcome_c, c),
    false,
    nullptr,
    true};

void f1ap_elem_procs_o::successful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, f1ap_elem_procs_o::successful_outcome_c::desc);
}
reset_ack_s& f1ap_elem_procs_o::successful_outcome_c::reset_ack()
{
  assert_choice_type(types::reset_ack, type_, "SuccessfulOutcome");
  return c.get<reset_ack_s>();
}
f1_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::f1_setup_resp()
{
  assert_choice_type(types::f1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<f1_setup_resp_s>();
}
gnb_du_cfg_upd_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_cfg_upd_ack()
{
  assert_choice_type(types::gnb_du_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_du_cfg_upd_ack_s>();
}
gnb_cu_cfg_upd_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_cu_cfg_upd_ack()
{
  assert_choice_type(types::gnb_cu_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cfg_upd_ack_s>();
}
ue_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_setup_resp()
{
  assert_choice_type(types::ue_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_setup_resp_s>();
}
ue_context_release_complete_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_release_complete()
{
  assert_choice_type(types::ue_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<ue_context_release_complete_s>();
}
ue_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_mod_resp()
{
  assert_choice_type(types::ue_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_resp_s>();
}
ue_context_mod_confirm_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_mod_confirm()
{
  assert_choice_type(types::ue_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_confirm_s>();
}
write_replace_warning_resp_s& f1ap_elem_procs_o::successful_outcome_c::write_replace_warning_resp()
{
  assert_choice_type(types::write_replace_warning_resp, type_, "SuccessfulOutcome");
  return c.get<write_replace_warning_resp_s>();
}
pws_cancel_resp_s& f1ap_elem_procs_o::successful_outcome_c::pws_cancel_resp()
{
  assert_choice_type(types::pws_cancel_resp, type_, "SuccessfulOutcome");
  return c.get<pws_cancel_resp_s>();
}
gnb_du_res_coordination_resp_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_res_coordination_resp()
{
  assert_choice_type(types::gnb_du_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_du_res_coordination_resp_s>();
}
f1_removal_resp_s& f1ap_elem_procs_o::successful_outcome_c::f1_removal_resp()
{
  assert_choice_type(types::f1_removal_resp, type_, "SuccessfulOutcome");
  return c.get<f1_removal_resp_s>();
}
bap_map_cfg_ack_s& f1ap_elem_procs_o::successful_outcome_c::bap_map_cfg_ack()
{
  assert_choice_type(types::bap_map_cfg_ack, type_, "SuccessfulOutcome");
  return c.get<bap_map_cfg_ack_s>();
}
gnb_du_res_cfg_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_res_cfg_ack()
{
  assert_choice_type(types::gnb_du_res_cfg_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_du_res_cfg_ack_s>();
}
iab_tnl_address_resp_s& f1ap_elem_procs_o::successful_outcome_c::iab_tnl_address_resp()
{
  assert_choice_type(types::iab_tnl_address_resp, type_, "SuccessfulOutcome");
  return c.get<iab_tnl_address_resp_s>();
}
iab_up_cfg_upd_resp_s& f1ap_elem_procs_o::successful_outcome_c::iab_up_cfg_upd_resp()
{
  assert_choice_type(types::iab_up_cfg_upd_resp, type_, "SuccessfulOutcome");
  return c.get<iab_up_cfg_upd_resp_s>();
}
res_status_resp_s& f1ap_elem_procs_o::successful_outcome_c::res_status_resp()
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
positioning_meas_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_meas_resp()
{
  assert_choice_type(types::positioning_meas_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_meas_resp_s>();
}
trp_info_resp_s& f1ap_elem_procs_o::successful_outcome_c::trp_info_resp()
{
  assert_choice_type(types::trp_info_resp, type_, "SuccessfulOutcome");
  return c.get<trp_info_resp_s>();
}
positioning_info_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_info_resp()
{
  assert_choice_type(types::positioning_info_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_info_resp_s>();
}
positioning_activation_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_activation_resp()
{
  assert_choice_type(types::positioning_activation_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_activation_resp_s>();
}
e_c_id_meas_initiation_resp_s& f1ap_elem_procs_o::successful_outcome_c::e_c_id_meas_initiation_resp()
{
  assert_choice_type(types::e_c_id_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_resp_s>();
}
broadcast_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::broadcast_context_setup_resp()
{
  assert_choice_type(types::broadcast_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_setup_resp_s>();
}
broadcast_context_release_complete_s& f1ap_elem_procs_o::successful_outcome_c::broadcast_context_release_complete()
{
  assert_choice_type(types::broadcast_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_release_complete_s>();
}
broadcast_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::broadcast_context_mod_resp()
{
  assert_choice_type(types::broadcast_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_mod_resp_s>();
}
multicast_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_context_setup_resp()
{
  assert_choice_type(types::multicast_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_context_setup_resp_s>();
}
multicast_context_release_complete_s& f1ap_elem_procs_o::successful_outcome_c::multicast_context_release_complete()
{
  assert_choice_type(types::multicast_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<multicast_context_release_complete_s>();
}
multicast_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_context_mod_resp()
{
  assert_choice_type(types::multicast_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_context_mod_resp_s>();
}
multicast_distribution_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_distribution_setup_resp()
{
  assert_choice_type(types::multicast_distribution_setup_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_distribution_setup_resp_s>();
}
multicast_distribution_release_complete_s&
f1ap_elem_procs_o::successful_outcome_c::multicast_distribution_release_complete()
{
  assert_choice_type(types::multicast_distribution_release_complete, type_, "SuccessfulOutcome");
  return c.get<multicast_distribution_release_complete_s>();
}
pdc_meas_initiation_resp_s& f1ap_elem_procs_o::successful_outcome_c::pdc_meas_initiation_resp()
{
  assert_choice_type(types::pdc_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<pdc_meas_initiation_resp_s>();
}
prs_cfg_resp_s& f1ap_elem_procs_o::successful_outcome_c::prs_cfg_resp()
{
  assert_choice_type(types::prs_cfg_resp, type_, "SuccessfulOutcome");
  return c.get<prs_cfg_resp_s>();
}
meas_precfg_confirm_s& f1ap_elem_procs_o::successful_outcome_c::meas_precfg_confirm()
{
  assert_choice_type(types::meas_precfg_confirm, type_, "SuccessfulOutcome");
  return c.get<meas_precfg_confirm_s>();
}
timing_synchronisation_status_resp_s& f1ap_elem_procs_o::successful_outcome_c::timing_synchronisation_status_resp()
{
  assert_choice_type(types::timing_synchronisation_status_resp, type_, "SuccessfulOutcome");
  return c.get<timing_synchronisation_status_resp_s>();
}
multicast_context_notif_confirm_s& f1ap_elem_procs_o::successful_outcome_c::multicast_context_notif_confirm()
{
  assert_choice_type(types::multicast_context_notif_confirm, type_, "SuccessfulOutcome");
  return c.get<multicast_context_notif_confirm_s>();
}
multicast_common_cfg_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_common_cfg_resp()
{
  assert_choice_type(types::multicast_common_cfg_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_common_cfg_resp_s>();
}
const reset_ack_s& f1ap_elem_procs_o::successful_outcome_c::reset_ack() const
{
  assert_choice_type(types::reset_ack, type_, "SuccessfulOutcome");
  return c.get<reset_ack_s>();
}
const f1_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::f1_setup_resp() const
{
  assert_choice_type(types::f1_setup_resp, type_, "SuccessfulOutcome");
  return c.get<f1_setup_resp_s>();
}
const gnb_du_cfg_upd_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_cfg_upd_ack() const
{
  assert_choice_type(types::gnb_du_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_du_cfg_upd_ack_s>();
}
const gnb_cu_cfg_upd_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_cu_cfg_upd_ack() const
{
  assert_choice_type(types::gnb_cu_cfg_upd_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_cu_cfg_upd_ack_s>();
}
const ue_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_setup_resp() const
{
  assert_choice_type(types::ue_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_setup_resp_s>();
}
const ue_context_release_complete_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_release_complete() const
{
  assert_choice_type(types::ue_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<ue_context_release_complete_s>();
}
const ue_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_mod_resp() const
{
  assert_choice_type(types::ue_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_resp_s>();
}
const ue_context_mod_confirm_s& f1ap_elem_procs_o::successful_outcome_c::ue_context_mod_confirm() const
{
  assert_choice_type(types::ue_context_mod_confirm, type_, "SuccessfulOutcome");
  return c.get<ue_context_mod_confirm_s>();
}
const write_replace_warning_resp_s& f1ap_elem_procs_o::successful_outcome_c::write_replace_warning_resp() const
{
  assert_choice_type(types::write_replace_warning_resp, type_, "SuccessfulOutcome");
  return c.get<write_replace_warning_resp_s>();
}
const pws_cancel_resp_s& f1ap_elem_procs_o::successful_outcome_c::pws_cancel_resp() const
{
  assert_choice_type(types::pws_cancel_resp, type_, "SuccessfulOutcome");
  return c.get<pws_cancel_resp_s>();
}
const gnb_du_res_coordination_resp_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_res_coordination_resp() const
{
  assert_choice_type(types::gnb_du_res_coordination_resp, type_, "SuccessfulOutcome");
  return c.get<gnb_du_res_coordination_resp_s>();
}
const f1_removal_resp_s& f1ap_elem_procs_o::successful_outcome_c::f1_removal_resp() const
{
  assert_choice_type(types::f1_removal_resp, type_, "SuccessfulOutcome");
  return c.get<f1_removal_resp_s>();
}
const bap_map_cfg_ack_s& f1ap_elem_procs_o::successful_outcome_c::bap_map_cfg_ack() const
{
  assert_choice_type(types::bap_map_cfg_ack, type_, "SuccessfulOutcome");
  return c.get<bap_map_cfg_ack_s>();
}
const gnb_du_res_cfg_ack_s& f1ap_elem_procs_o::successful_outcome_c::gnb_du_res_cfg_ack() const
{
  assert_choice_type(types::gnb_du_res_cfg_ack, type_, "SuccessfulOutcome");
  return c.get<gnb_du_res_cfg_ack_s>();
}
const iab_tnl_address_resp_s& f1ap_elem_procs_o::successful_outcome_c::iab_tnl_address_resp() const
{
  assert_choice_type(types::iab_tnl_address_resp, type_, "SuccessfulOutcome");
  return c.get<iab_tnl_address_resp_s>();
}
const iab_up_cfg_upd_resp_s& f1ap_elem_procs_o::successful_outcome_c::iab_up_cfg_upd_resp() const
{
  assert_choice_type(types::iab_up_cfg_upd_resp, type_, "SuccessfulOutcome");
  return c.get<iab_up_cfg_upd_resp_s>();
}
const res_status_resp_s& f1ap_elem_procs_o::successful_outcome_c::res_status_resp() const
{
  assert_choice_type(types::res_status_resp, type_, "SuccessfulOutcome");
  return c.get<res_status_resp_s>();
}
const positioning_meas_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_meas_resp() const
{
  assert_choice_type(types::positioning_meas_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_meas_resp_s>();
}
const trp_info_resp_s& f1ap_elem_procs_o::successful_outcome_c::trp_info_resp() const
{
  assert_choice_type(types::trp_info_resp, type_, "SuccessfulOutcome");
  return c.get<trp_info_resp_s>();
}
const positioning_info_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_info_resp() const
{
  assert_choice_type(types::positioning_info_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_info_resp_s>();
}
const positioning_activation_resp_s& f1ap_elem_procs_o::successful_outcome_c::positioning_activation_resp() const
{
  assert_choice_type(types::positioning_activation_resp, type_, "SuccessfulOutcome");
  return c.get<positioning_activation_resp_s>();
}
const e_c_id_meas_initiation_resp_s& f1ap_elem_procs_o::successful_outcome_c::e_c_id_meas_initiation_resp() const
{
  assert_choice_type(types::e_c_id_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_resp_s>();
}
const broadcast_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::broadcast_context_setup_resp() const
{
  assert_choice_type(types::broadcast_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_setup_resp_s>();
}
const broadcast_context_release_complete_s&
f1ap_elem_procs_o::successful_outcome_c::broadcast_context_release_complete() const
{
  assert_choice_type(types::broadcast_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_release_complete_s>();
}
const broadcast_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::broadcast_context_mod_resp() const
{
  assert_choice_type(types::broadcast_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<broadcast_context_mod_resp_s>();
}
const multicast_context_setup_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_context_setup_resp() const
{
  assert_choice_type(types::multicast_context_setup_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_context_setup_resp_s>();
}
const multicast_context_release_complete_s&
f1ap_elem_procs_o::successful_outcome_c::multicast_context_release_complete() const
{
  assert_choice_type(types::multicast_context_release_complete, type_, "SuccessfulOutcome");
  return c.get<multicast_context_release_complete_s>();
}
const multicast_context_mod_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_context_mod_resp() const
{
  assert_choice_type(types::multicast_context_mod_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_context_mod_resp_s>();
}
const multicast_distribution_setup_resp_s&
f1ap_elem_procs_o::successful_outcome_c::multicast_distribution_setup_resp() const
{
  assert_choice_type(types::multicast_distribution_setup_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_distribution_setup_resp_s>();
}
const multicast_distribution_release_complete_s&
f1ap_elem_procs_o::successful_outcome_c::multicast_distribution_release_complete() const
{
  assert_choice_type(types::multicast_distribution_release_complete, type_, "SuccessfulOutcome");
  return c.get<multicast_distribution_release_complete_s>();
}
const pdc_meas_initiation_resp_s& f1ap_elem_procs_o::successful_outcome_c::pdc_meas_initiation_resp() const
{
  assert_choice_type(types::pdc_meas_initiation_resp, type_, "SuccessfulOutcome");
  return c.get<pdc_meas_initiation_resp_s>();
}
const prs_cfg_resp_s& f1ap_elem_procs_o::successful_outcome_c::prs_cfg_resp() const
{
  assert_choice_type(types::prs_cfg_resp, type_, "SuccessfulOutcome");
  return c.get<prs_cfg_resp_s>();
}
const meas_precfg_confirm_s& f1ap_elem_procs_o::successful_outcome_c::meas_precfg_confirm() const
{
  assert_choice_type(types::meas_precfg_confirm, type_, "SuccessfulOutcome");
  return c.get<meas_precfg_confirm_s>();
}
const timing_synchronisation_status_resp_s&
f1ap_elem_procs_o::successful_outcome_c::timing_synchronisation_status_resp() const
{
  assert_choice_type(types::timing_synchronisation_status_resp, type_, "SuccessfulOutcome");
  return c.get<timing_synchronisation_status_resp_s>();
}
const multicast_context_notif_confirm_s&
f1ap_elem_procs_o::successful_outcome_c::multicast_context_notif_confirm() const
{
  assert_choice_type(types::multicast_context_notif_confirm, type_, "SuccessfulOutcome");
  return c.get<multicast_context_notif_confirm_s>();
}
const multicast_common_cfg_resp_s& f1ap_elem_procs_o::successful_outcome_c::multicast_common_cfg_resp() const
{
  assert_choice_type(types::multicast_common_cfg_resp, type_, "SuccessfulOutcome");
  return c.get<multicast_common_cfg_resp_s>();
}

static const char* enum_names_f1ap_elem_procs_o_successful_outcome_c_types_values[] = {
    "ResetAcknowledge",
    "F1SetupResponse",
    "GNBDUConfigurationUpdateAcknowledge",
    "GNBCUConfigurationUpdateAcknowledge",
    "UEContextSetupResponse",
    "UEContextReleaseComplete",
    "UEContextModificationResponse",
    "UEContextModificationConfirm",
    "WriteReplaceWarningResponse",
    "PWSCancelResponse",
    "GNBDUResourceCoordinationResponse",
    "F1RemovalResponse",
    "BAPMappingConfigurationAcknowledge",
    "GNBDUResourceConfigurationAcknowledge",
    "IABTNLAddressResponse",
    "IABUPConfigurationUpdateResponse",
    "ResourceStatusResponse",
    "PositioningMeasurementResponse",
    "TRPInformationResponse",
    "PositioningInformationResponse",
    "PositioningActivationResponse",
    "E-CIDMeasurementInitiationResponse",
    "BroadcastContextSetupResponse",
    "BroadcastContextReleaseComplete",
    "BroadcastContextModificationResponse",
    "MulticastContextSetupResponse",
    "MulticastContextReleaseComplete",
    "MulticastContextModificationResponse",
    "MulticastDistributionSetupResponse",
    "MulticastDistributionReleaseComplete",
    "PDCMeasurementInitiationResponse",
    "PRSConfigurationResponse",
    "MeasurementPreconfigurationConfirm",
    "TimingSynchronisationStatusResponse",
    "MulticastContextNotificationConfirm",
    "MulticastCommonConfigurationResponse"};
const asn1::table::enum_desc f1ap_elem_procs_o::successful_outcome_c::types_opts::desc = {
    enum_names_f1ap_elem_procs_o_successful_outcome_c_types_values,
    36,
    "f1ap_elem_procs_o::successful_outcome_c::types"};

// UnsuccessfulOutcome ::= OPEN TYPE
static const asn1::table::seq_field choice_table_f1ap_elem_procs_o_unsuccessful_outcome_c_alternatives[] = {
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<f1_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_du_cfg_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_cu_cfg_upd_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<ue_context_setup_fail_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<ue_context_mod_refuse_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<f1_removal_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<bap_map_cfg_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<gnb_du_res_cfg_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_tnl_address_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<iab_up_cfg_upd_fail_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<positioning_meas_fail_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<positioning_info_fail_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<e_c_id_meas_initiation_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_context_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<broadcast_context_mod_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_context_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_context_mod_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_distribution_setup_fail_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<pdc_meas_initiation_fail_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<meas_precfg_refuse_s, asn1::table::struct_json>::value},
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
     &asn1::table::any_codec<multicast_context_notif_refuse_s, asn1::table::struct_json>::value},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::any_codec<multicast_common_cfg_refuse_s, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_f1ap_elem_procs_o_unsuccessful_outcome_c_names[] = {
    {"F1SetupFailure", nullptr},
    {"GNBDUConfigurationUpdateFailure", nullptr},
    {"GNBCUConfigurationUpdateFailure", nullptr},
    {"UEContextSetupFailure", nullptr},
    {"UEContextModificationFailure", nullptr},
    {"UEContextModificationRefuse", nullptr},
    {"F1RemovalFailure", nullptr},
    {"BAPMappingConfigurationFailure", nullptr},
    {"GNBDUResourceConfigurationFailure", nullptr},
    {"IABTNLAddressFailure", nullptr},
    {"IABUPConfigurationUpdateFailure", nullptr},
    {"ResourceStatusFailure", nullptr},
    {"PositioningMeasurementFailure", nullptr},
    {"TRPInformationFailure", nullptr},
    {"PositioningInformationFailure", nullptr},
    {"PositioningActivationFailure", nullptr},
    {"E-CIDMeasurementInitiationFailure", nullptr},
    {"BroadcastContextSetupFailure", nullptr},
    {"BroadcastContextModificationFailure", nullptr},
    {"MulticastContextSetupFailure", nullptr},
    {"MulticastContextModificationFailure", nullptr},
    {"MulticastDistributionSetupFailure", nullptr},
    {"PDCMeasurementInitiationFailure", nullptr},
    {"PRSConfigurationFailure", nullptr},
    {"MeasurementPreconfigurationRefuse", nullptr},
    {"TimingSynchronisationStatusFailure", nullptr},
    {"MulticastContextNotificationRefuse", nullptr},
    {"MulticastCommonConfigurationRefuse", nullptr}};
const asn1::table::choice_desc f1ap_elem_procs_o::unsuccessful_outcome_c::desc = {
    asn1::table::enum_int_type<f1ap_elem_procs_o::unsuccessful_outcome_c::types>(),
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nof_types,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::nof_exts,
    f1ap_elem_procs_o::unsuccessful_outcome_c::types::has_ext,
    true,
    choice_table_f1ap_elem_procs_o_unsuccessful_outcome_c_alternatives,
    &asn1::table::choice_set<f1ap_elem_procs_o::unsuccessful_outcome_c>,
    choice_table_f1ap_elem_procs_o_unsuccessful_outcome_c_names,
    "f1ap_elem_procs_o::unsuccessful_outcome_c",
    offsetof(f1ap_elem_procs_o::unsuccessful_outcome_c, type_),
    offsetof(f1ap_elem_procs_o::unsuccessful_outcome_c, c),
    false,
    nullptr,
    true};

void f1ap_elem_procs_o::unsuccessful_outcome_c::set(types::options e)
{
  type_ = e;
  asn1::table::set_any_choice(&c, e, f1ap_elem_procs_o::unsuccessful_outcome_c::desc);
}
f1_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::f1_setup_fail()
{
  assert_choice_type(types::f1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<f1_setup_fail_s>();
}
gnb_du_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_du_cfg_upd_fail()
{
  assert_choice_type(types::gnb_du_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_du_cfg_upd_fail_s>();
}
gnb_cu_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cfg_upd_fail()
{
  assert_choice_type(types::gnb_cu_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cfg_upd_fail_s>();
}
ue_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_setup_fail()
{
  assert_choice_type(types::ue_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_setup_fail_s>();
}
ue_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_fail()
{
  assert_choice_type(types::ue_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_fail_s>();
}
ue_context_mod_refuse_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_refuse()
{
  assert_choice_type(types::ue_context_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_refuse_s>();
}
f1_removal_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::f1_removal_fail()
{
  assert_choice_type(types::f1_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<f1_removal_fail_s>();
}
bap_map_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::bap_map_cfg_fail()
{
  assert_choice_type(types::bap_map_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<bap_map_cfg_fail_s>();
}
gnb_du_res_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_du_res_cfg_fail()
{
  assert_choice_type(types::gnb_du_res_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_du_res_cfg_fail_s>();
}
iab_tnl_address_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::iab_tnl_address_fail()
{
  assert_choice_type(types::iab_tnl_address_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_tnl_address_fail_s>();
}
iab_up_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::iab_up_cfg_upd_fail()
{
  assert_choice_type(types::iab_up_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_up_cfg_upd_fail_s>();
}
res_status_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::res_status_fail()
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
positioning_meas_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_meas_fail()
{
  assert_choice_type(types::positioning_meas_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_meas_fail_s>();
}
trp_info_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::trp_info_fail()
{
  assert_choice_type(types::trp_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<trp_info_fail_s>();
}
positioning_info_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_info_fail()
{
  assert_choice_type(types::positioning_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_info_fail_s>();
}
positioning_activation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_activation_fail()
{
  assert_choice_type(types::positioning_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_activation_fail_s>();
}
e_c_id_meas_initiation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::e_c_id_meas_initiation_fail()
{
  assert_choice_type(types::e_c_id_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_fail_s>();
}
broadcast_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::broadcast_context_setup_fail()
{
  assert_choice_type(types::broadcast_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_context_setup_fail_s>();
}
broadcast_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::broadcast_context_mod_fail()
{
  assert_choice_type(types::broadcast_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_context_mod_fail_s>();
}
multicast_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_context_setup_fail()
{
  assert_choice_type(types::multicast_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_context_setup_fail_s>();
}
multicast_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_context_mod_fail()
{
  assert_choice_type(types::multicast_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_context_mod_fail_s>();
}
multicast_distribution_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_distribution_setup_fail()
{
  assert_choice_type(types::multicast_distribution_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_distribution_setup_fail_s>();
}
pdc_meas_initiation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::pdc_meas_initiation_fail()
{
  assert_choice_type(types::pdc_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<pdc_meas_initiation_fail_s>();
}
prs_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::prs_cfg_fail()
{
  assert_choice_type(types::prs_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<prs_cfg_fail_s>();
}
meas_precfg_refuse_s& f1ap_elem_procs_o::unsuccessful_outcome_c::meas_precfg_refuse()
{
  assert_choice_type(types::meas_precfg_refuse, type_, "UnsuccessfulOutcome");
  return c.get<meas_precfg_refuse_s>();
}
timing_synchronisation_status_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::timing_synchronisation_status_fail()
{
  assert_choice_type(types::timing_synchronisation_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<timing_synchronisation_status_fail_s>();
}
multicast_context_notif_refuse_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_context_notif_refuse()
{
  assert_choice_type(types::multicast_context_notif_refuse, type_, "UnsuccessfulOutcome");
  return c.get<multicast_context_notif_refuse_s>();
}
multicast_common_cfg_refuse_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_common_cfg_refuse()
{
  assert_choice_type(types::multicast_common_cfg_refuse, type_, "UnsuccessfulOutcome");
  return c.get<multicast_common_cfg_refuse_s>();
}
const f1_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::f1_setup_fail() const
{
  assert_choice_type(types::f1_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<f1_setup_fail_s>();
}
const gnb_du_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_du_cfg_upd_fail() const
{
  assert_choice_type(types::gnb_du_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_du_cfg_upd_fail_s>();
}
const gnb_cu_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_cu_cfg_upd_fail() const
{
  assert_choice_type(types::gnb_cu_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_cu_cfg_upd_fail_s>();
}
const ue_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_setup_fail() const
{
  assert_choice_type(types::ue_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_setup_fail_s>();
}
const ue_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_fail() const
{
  assert_choice_type(types::ue_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_fail_s>();
}
const ue_context_mod_refuse_s& f1ap_elem_procs_o::unsuccessful_outcome_c::ue_context_mod_refuse() const
{
  assert_choice_type(types::ue_context_mod_refuse, type_, "UnsuccessfulOutcome");
  return c.get<ue_context_mod_refuse_s>();
}
const f1_removal_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::f1_removal_fail() const
{
  assert_choice_type(types::f1_removal_fail, type_, "UnsuccessfulOutcome");
  return c.get<f1_removal_fail_s>();
}
const bap_map_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::bap_map_cfg_fail() const
{
  assert_choice_type(types::bap_map_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<bap_map_cfg_fail_s>();
}
const gnb_du_res_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::gnb_du_res_cfg_fail() const
{
  assert_choice_type(types::gnb_du_res_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<gnb_du_res_cfg_fail_s>();
}
const iab_tnl_address_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::iab_tnl_address_fail() const
{
  assert_choice_type(types::iab_tnl_address_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_tnl_address_fail_s>();
}
const iab_up_cfg_upd_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::iab_up_cfg_upd_fail() const
{
  assert_choice_type(types::iab_up_cfg_upd_fail, type_, "UnsuccessfulOutcome");
  return c.get<iab_up_cfg_upd_fail_s>();
}
const res_status_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::res_status_fail() const
{
  assert_choice_type(types::res_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<res_status_fail_s>();
}
const positioning_meas_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_meas_fail() const
{
  assert_choice_type(types::positioning_meas_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_meas_fail_s>();
}
const trp_info_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::trp_info_fail() const
{
  assert_choice_type(types::trp_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<trp_info_fail_s>();
}
const positioning_info_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_info_fail() const
{
  assert_choice_type(types::positioning_info_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_info_fail_s>();
}
const positioning_activation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::positioning_activation_fail() const
{
  assert_choice_type(types::positioning_activation_fail, type_, "UnsuccessfulOutcome");
  return c.get<positioning_activation_fail_s>();
}
const e_c_id_meas_initiation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::e_c_id_meas_initiation_fail() const
{
  assert_choice_type(types::e_c_id_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<e_c_id_meas_initiation_fail_s>();
}
const broadcast_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::broadcast_context_setup_fail() const
{
  assert_choice_type(types::broadcast_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_context_setup_fail_s>();
}
const broadcast_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::broadcast_context_mod_fail() const
{
  assert_choice_type(types::broadcast_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<broadcast_context_mod_fail_s>();
}
const multicast_context_setup_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_context_setup_fail() const
{
  assert_choice_type(types::multicast_context_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_context_setup_fail_s>();
}
const multicast_context_mod_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_context_mod_fail() const
{
  assert_choice_type(types::multicast_context_mod_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_context_mod_fail_s>();
}
const multicast_distribution_setup_fail_s&
f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_distribution_setup_fail() const
{
  assert_choice_type(types::multicast_distribution_setup_fail, type_, "UnsuccessfulOutcome");
  return c.get<multicast_distribution_setup_fail_s>();
}
const pdc_meas_initiation_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::pdc_meas_initiation_fail() const
{
  assert_choice_type(types::pdc_meas_initiation_fail, type_, "UnsuccessfulOutcome");
  return c.get<pdc_meas_initiation_fail_s>();
}
const prs_cfg_fail_s& f1ap_elem_procs_o::unsuccessful_outcome_c::prs_cfg_fail() const
{
  assert_choice_type(types::prs_cfg_fail, type_, "UnsuccessfulOutcome");
  return c.get<prs_cfg_fail_s>();
}
const meas_precfg_refuse_s& f1ap_elem_procs_o::unsuccessful_outcome_c::meas_precfg_refuse() const
{
  assert_choice_type(types::meas_precfg_refuse, type_, "UnsuccessfulOutcome");
  return c.get<meas_precfg_refuse_s>();
}
const timing_synchronisation_status_fail_s&
f1ap_elem_procs_o::unsuccessful_outcome_c::timing_synchronisation_status_fail() const
{
  assert_choice_type(types::timing_synchronisation_status_fail, type_, "UnsuccessfulOutcome");
  return c.get<timing_synchronisation_status_fail_s>();
}
const multicast_context_notif_refuse_s&
f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_context_notif_refuse() const
{
  assert_choice_type(types::multicast_context_notif_refuse, type_, "UnsuccessfulOutcome");
  return c.get<multicast_context_notif_refuse_s>();
}
const multicast_common_cfg_refuse_s& f1ap_elem_procs_o::unsuccessful_outcome_c::multicast_common_cfg_refuse() const
{
  assert_choice_type(types::multicast_common_cfg_refuse, type_, "UnsuccessfulOutcome");
  return c.get<multicast_common_cfg_refuse_s>();
}

static const char* enum_names_f1ap_elem_procs_o_unsuccessful_outcome_c_types_values[] = {
    "F1SetupFailure",
    "GNBDUConfigurationUpdateFailure",
    "GNBCUConfigurationUpdateFailure",
    "UEContextSetupFailure",
    "UEContextModificationFailure",
    "UEContextModificationRefuse",
    "F1RemovalFailure",
    "BAPMappingConfigurationFailure",
    "GNBDUResourceConfigurationFailure",
    "IABTNLAddressFailure",
    "IABUPConfigurationUpdateFailure",
    "ResourceStatusFailure",
    "PositioningMeasurementFailure",
    "TRPInformationFailure",
    "PositioningInformationFailure",
    "PositioningActivationFailure",
    "E-CIDMeasurementInitiationFailure",
    "BroadcastContextSetupFailure",
    "BroadcastContextModificationFailure",
    "MulticastContextSetupFailure",
    "MulticastContextModificationFailure",
    "MulticastDistributionSetupFailure",
    "PDCMeasurementInitiationFailure",
    "PRSConfigurationFailure",
    "MeasurementPreconfigurationRefuse",
    "TimingSynchronisationStatusFailure",
    "MulticastContextNotificationRefuse",
    "MulticastCommonConfigurationRefuse"};
const asn1::table::enum_desc f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::desc = {
    enum_names_f1ap_elem_procs_o_unsuccessful_outcome_c_types_values,
    28,
    "f1ap_elem_procs_o::unsuccessful_outcome_c::types"};

// Columns of the object sets of F1AP-ELEMENTARY-PROCEDURE.
enum class f1ap_elem_proc_p_column : uint32_t { init_msg, successful_outcome, unsuccessful_outcome, crit };

// InitiatingMessage ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
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
                                 f1ap_elem_procs_o::desc,
                                 f1ap_elem_proc_p_column::crit,
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
                                  f1ap_elem_procs_o::desc,
                                  f1ap_elem_proc_p_column::init_msg,
                                  asn1::table::struct_json>::value}};
static const asn1::table::field_name seq_table_init_msg_s_names[] = {{"procedureCode", nullptr},
                                                                     {"criticality", nullptr},
                                                                     {"value", nullptr}};
const asn1::table::seq_desc          init_msg_s::desc =
    {{seq_table_init_msg_s_fields, 3, UINT64_C(0x7), nullptr, 0}, false, true, nullptr, 0, seq_table_init_msg_s_names};
bool init_msg_s::load_info_obj(const uint16_t& proc_code_)
{
  if (not asn1::table::objset_has_key(f1ap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(f1ap_elem_procs_o::desc, f1ap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, f1ap_elem_procs_o::desc, f1ap_elem_proc_p_column::init_msg, proc_code);
  return value.type().value != f1ap_elem_procs_o::init_msg_c::types_opts::nulltype;
}

// SuccessfulOutcome ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
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
                                 f1ap_elem_procs_o::desc,
                                 f1ap_elem_proc_p_column::crit,
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
                                  f1ap_elem_procs_o::desc,
                                  f1ap_elem_proc_p_column::successful_outcome,
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
  if (not asn1::table::objset_has_key(f1ap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(f1ap_elem_procs_o::desc, f1ap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, f1ap_elem_procs_o::desc, f1ap_elem_proc_p_column::successful_outcome, proc_code);
  return value.type().value != f1ap_elem_procs_o::successful_outcome_c::types_opts::nulltype;
}

// UnsuccessfulOutcome ::= SEQUENCE{{F1AP-ELEMENTARY-PROCEDURE}}
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
                                 f1ap_elem_procs_o::desc,
                                 f1ap_elem_proc_p_column::crit,
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
                                  f1ap_elem_procs_o::desc,
                                  f1ap_elem_proc_p_column::unsuccessful_outcome,
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
  if (not asn1::table::objset_has_key(f1ap_elem_procs_o::desc, proc_code_)) {
    return false;
  }
  proc_code = proc_code_;
  crit      = asn1::table::objset_enum<crit_e>(f1ap_elem_procs_o::desc, f1ap_elem_proc_p_column::crit, proc_code);
  asn1::table::objset_select(value, f1ap_elem_procs_o::desc, f1ap_elem_proc_p_column::unsuccessful_outcome, proc_code);
  return value.type().value != f1ap_elem_procs_o::unsuccessful_outcome_c::types_opts::nulltype;
}

// F1AP-PDU ::= CHOICE
static const asn1::table::storage_ops* const choice_storage_f1ap_pdu_c_alternatives[] = {
    &asn1::table::alternative_ops<init_msg_s, false>::value,
    &asn1::table::alternative_ops<successful_outcome_s, false>::value,
    &asn1::table::alternative_ops<unsuccessful_outcome_s, false>::value,
    &asn1::table::alternative_ops<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>, false>::value};
const asn1::table::choice_storage_desc f1ap_pdu_c::storage_desc = {choice_storage_f1ap_pdu_c_alternatives,
                                                                   4,
                                                                   "f1ap_pdu_c"};

init_msg_s& f1ap_pdu_c::set_init_msg()
{
  set(types::init_msg);
  return c.get<init_msg_s>();
}
successful_outcome_s& f1ap_pdu_c::set_successful_outcome()
{
  set(types::successful_outcome);
  return c.get<successful_outcome_s>();
}
unsuccessful_outcome_s& f1ap_pdu_c::set_unsuccessful_outcome()
{
  set(types::unsuccessful_outcome);
  return c.get<unsuccessful_outcome_s>();
}
protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>& f1ap_pdu_c::set_choice_ext()
{
  set(types::choice_ext);
  return c.get<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>>();
}
static const asn1::table::seq_field choice_table_f1ap_pdu_c_alternatives[] = {
    {0,
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
     &unsuccessful_outcome_s::desc},
    {0,
     asn1::table::seq_field::mandatory,
     asn1::table::field_op::codec,
     false,
     false,
     0,
     0,
     &asn1::table::object_codec<protocol_ie_single_container_s<f1ap_pdu_ext_ies_o>, asn1::table::struct_json>::value}};
static const asn1::table::field_name choice_table_f1ap_pdu_c_names[] = {{"initiatingMessage", nullptr},
                                                                        {"successfulOutcome", nullptr},
                                                                        {"unsuccessfulOutcome", nullptr},
                                                                        {"choice-extension", nullptr}};
const asn1::table::choice_desc       f1ap_pdu_c::desc                = {asn1::table::enum_int_type<f1ap_pdu_c::types>(),
                                                                        f1ap_pdu_c::types::nof_types,
                                                                        f1ap_pdu_c::types::nof_exts,
                                                                        f1ap_pdu_c::types::has_ext,
                                                                        true,
                                                                        choice_table_f1ap_pdu_c_alternatives,
                                                                        nullptr,
                                                                        choice_table_f1ap_pdu_c_names,
                                                                        "f1ap_pdu_c",
                                                                        offsetof(f1ap_pdu_c, c.type),
                                                                        offsetof(f1ap_pdu_c, c.value),
                                                                        false,
                                                                        &f1ap_pdu_c::storage_desc};

static const char*           enum_names_f1ap_pdu_c_types_values[] = {"initiatingMessage",
                                                                     "successfulOutcome",
                                                                     "unsuccessfulOutcome",
                                                                     "choice-extension"};
const asn1::table::enum_desc f1ap_pdu_c::types_opts::desc         = {enum_names_f1ap_pdu_c_types_values,
                                                                     4,
                                                                     "f1ap_pdu_c::types"};
