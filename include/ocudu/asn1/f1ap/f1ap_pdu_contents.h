// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/*******************************************************************************
 *
 *                     3GPP TS ASN1 F1AP v18.8.0 (2025-12)
 *
 ******************************************************************************/

#pragma once

#include "ocudu/asn1/f1ap/f1ap_pdu_contents_ue.h"

namespace asn1 {
namespace f1ap {

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// AccessAndMobilityIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct access_and_mob_ind_ies_container {
  bool                                        ra_report_list_present                            = false;
  bool                                        rlf_report_info_list_present                      = false;
  bool                                        successful_ho_report_info_list_present            = false;
  bool                                        successful_pscell_change_report_info_list_present = false;
  uint16_t                                    transaction_id;
  ra_report_list_l                            ra_report_list;
  rlf_report_info_list_l                      rlf_report_info_list;
  successful_ho_report_info_list_l            successful_ho_report_info_list;
  successful_pscell_change_report_info_list_l successful_pscell_change_report_info_list;

  static const asn1::table::ie_container_desc desc;
};

// AccessAndMobilityIndication ::= SEQUENCE
using access_and_mob_ind_s = elementary_procedure_option<access_and_mob_ind_ies_container>;

// AccessSuccessIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct access_success_ies_container {
  uint64_t gnb_cu_ue_f1ap_id;
  uint64_t gnb_du_ue_f1ap_id;
  nr_cgi_s nr_cgi;

  static const asn1::table::ie_container_desc desc;
};

// AccessSuccess ::= SEQUENCE
using access_success_s = elementary_procedure_option<access_success_ies_container>;

// AperiodicSRS-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct aperiodic_srs_ext_ies_container {
  bool                        aggr_pos_srs_res_set_list_present = false;
  aggr_pos_srs_res_set_list_l aggr_pos_srs_res_set_list;

  static const asn1::table::ie_container_desc desc;
};

// AperiodicSRS ::= SEQUENCE
struct aperiodic_srs_s {
  struct aperiodic_opts {
    enum options { true_value, /*...*/ nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using aperiodic_e_ = enumerated<aperiodic_opts, true>;

  // member variables
  bool                            srs_res_trigger_present = false;
  bool                            ie_exts_present         = false;
  aperiodic_e_                    aperiodic;
  srs_res_trigger_s               srs_res_trigger;
  aperiodic_srs_ext_ies_container ie_exts;
  // ...

  static const asn1::table::seq_desc desc;
};

// BAPMappingConfiguration-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bap_map_cfg_ies_container {
  bool                           bh_routing_info_added_list_present   = false;
  bool                           bh_routing_info_remd_list_present    = false;
  bool                           traffic_map_info_present             = false;
  bool                           buffer_size_thresh_present           = false;
  bool                           bap_hdr_rewriting_added_list_present = false;
  bool                           re_routing_enable_ind_present        = false;
  bool                           bap_hdr_rewriting_remd_list_present  = false;
  uint16_t                       transaction_id;
  bh_routing_info_added_list_l   bh_routing_info_added_list;
  bh_routing_info_remd_list_l    bh_routing_info_remd_list;
  traffic_map_info_c             traffic_map_info;
  uint32_t                       buffer_size_thresh;
  bap_hdr_rewriting_added_list_l bap_hdr_rewriting_added_list;
  re_routing_enable_ind_e        re_routing_enable_ind;
  bap_hdr_rewriting_remd_list_l  bap_hdr_rewriting_remd_list;

  static const asn1::table::ie_container_desc desc;
};

// BAPMappingConfiguration ::= SEQUENCE
using bap_map_cfg_s = elementary_procedure_option<bap_map_cfg_ies_container>;

// BAPMappingConfigurationAcknowledge-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bap_map_cfg_ack_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// BAPMappingConfigurationAcknowledge ::= SEQUENCE
using bap_map_cfg_ack_s = elementary_procedure_option<bap_map_cfg_ack_ies_container>;

// BAPMappingConfigurationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct bap_map_cfg_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// BAPMappingConfigurationFailure ::= SEQUENCE
using bap_map_cfg_fail_s = elementary_procedure_option<bap_map_cfg_fail_ies_container>;

// BroadcastContextModificationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_mod_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// BroadcastContextModificationFailure ::= SEQUENCE
using broadcast_context_mod_fail_s = elementary_procedure_option<broadcast_context_mod_fail_ies_container>;

// BroadcastContextModificationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_mod_request_ies_container {
  bool                                   mbs_service_area_present                     = false;
  bool                                   broadcast_m_rbs_to_be_setup_mod_list_present = false;
  bool                                   broadcast_m_rbs_to_be_modified_list_present  = false;
  bool                                   broadcast_m_rbs_to_be_released_list_present  = false;
  bool                                   supported_ue_type_list_present               = false;
  uint64_t                               gnb_cu_mbs_f1ap_id;
  uint64_t                               gnb_du_mbs_f1ap_id;
  mbs_service_area_c                     mbs_service_area;
  mbs_cu_to_du_rrc_info_s                mbs_cu_to_du_rrc_info;
  broadcast_m_rbs_to_be_setup_mod_list_l broadcast_m_rbs_to_be_setup_mod_list;
  broadcast_m_rbs_to_be_modified_list_l  broadcast_m_rbs_to_be_modified_list;
  broadcast_m_rbs_to_be_released_list_l  broadcast_m_rbs_to_be_released_list;
  supported_ue_type_list_l               supported_ue_type_list;

  static const asn1::table::ie_container_desc desc;
};

// BroadcastContextModificationRequest ::= SEQUENCE
using broadcast_context_mod_request_s = elementary_procedure_option<broadcast_context_mod_request_ies_container>;

// BroadcastContextModificationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_mod_resp_ies_container {
  bool                                          broadcast_m_rbs_setup_mod_list_present              = false;
  bool                                          broadcast_m_rbs_failed_to_be_setup_mod_list_present = false;
  bool                                          broadcast_m_rbs_modified_list_present               = false;
  bool                                          broadcast_m_rbs_failed_to_be_modified_list_present  = false;
  bool                                          crit_diagnostics_present                            = false;
  bool                                          broadcast_area_scope_present                        = false;
  uint64_t                                      gnb_cu_mbs_f1ap_id;
  uint64_t                                      gnb_du_mbs_f1ap_id;
  broadcast_m_rbs_setup_mod_list_l              broadcast_m_rbs_setup_mod_list;
  broadcast_m_rbs_failed_to_be_setup_mod_list_l broadcast_m_rbs_failed_to_be_setup_mod_list;
  broadcast_m_rbs_modified_list_l               broadcast_m_rbs_modified_list;
  broadcast_m_rbs_failed_to_be_modified_list_l  broadcast_m_rbs_failed_to_be_modified_list;
  crit_diagnostics_s                            crit_diagnostics;
  broadcast_area_scope_c                        broadcast_area_scope;

  static const asn1::table::ie_container_desc desc;
};

// BroadcastContextModificationResponse ::= SEQUENCE
using broadcast_context_mod_resp_s = elementary_procedure_option<broadcast_context_mod_resp_ies_container>;

// BroadcastContextReleaseCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_release_cmd_ies_container {
  uint64_t gnb_cu_mbs_f1ap_id;
  uint64_t gnb_du_mbs_f1ap_id;
  cause_c  cause;

  static const asn1::table::ie_container_desc desc;
};

// BroadcastContextReleaseCommand ::= SEQUENCE
using broadcast_context_release_cmd_s = elementary_procedure_option<broadcast_context_release_cmd_ies_container>;

// BroadcastContextReleaseCompleteIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_release_complete_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// BroadcastContextReleaseComplete ::= SEQUENCE
using broadcast_context_release_complete_s =
    elementary_procedure_option<broadcast_context_release_complete_ies_container>;

// BroadcastContextReleaseRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_release_request_ies_container {
  uint64_t gnb_cu_mbs_f1ap_id;
  uint64_t gnb_du_mbs_f1ap_id;
  cause_c  cause;

  static const asn1::table::ie_container_desc desc;
};

// BroadcastContextReleaseRequest ::= SEQUENCE
using broadcast_context_release_request_s =
    elementary_procedure_option<broadcast_context_release_request_ies_container>;

// BroadcastContextSetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_setup_fail_ies_container {
  bool               gnb_du_mbs_f1ap_id_present = false;
  bool               crit_diagnostics_present   = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// BroadcastContextSetupFailure ::= SEQUENCE
using broadcast_context_setup_fail_s = elementary_procedure_option<broadcast_context_setup_fail_ies_container>;

// BroadcastContextSetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_setup_request_ies_container {
  bool                               mbs_service_area_present        = false;
  bool                               supported_ue_type_list_present  = false;
  bool                               associated_session_id_present   = false;
  bool                               ran_sharing_assist_info_present = false;
  uint64_t                           gnb_cu_mbs_f1ap_id;
  mbs_session_id_s                   mbs_session_id;
  mbs_service_area_c                 mbs_service_area;
  mbs_cu_to_du_rrc_info_s            mbs_cu_to_du_rrc_info;
  snssai_s                           snssai;
  broadcast_m_rbs_to_be_setup_list_l broadcast_m_rbs_to_be_setup_list;
  supported_ue_type_list_l           supported_ue_type_list;
  unbounded_octstring<true>          associated_session_id;
  ran_sharing_assist_info_e          ran_sharing_assist_info;

  static const asn1::table::ie_container_desc desc;
};

// BroadcastContextSetupRequest ::= SEQUENCE
using broadcast_context_setup_request_s = elementary_procedure_option<broadcast_context_setup_request_ies_container>;

// BroadcastContextSetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_context_setup_resp_ies_container {
  bool                                      broadcast_m_rbs_failed_to_be_setup_list_present = false;
  bool                                      broadcast_area_scope_present                    = false;
  bool                                      crit_diagnostics_present                        = false;
  uint64_t                                  gnb_cu_mbs_f1ap_id;
  uint64_t                                  gnb_du_mbs_f1ap_id;
  broadcast_m_rbs_setup_list_l              broadcast_m_rbs_setup_list;
  broadcast_m_rbs_failed_to_be_setup_list_l broadcast_m_rbs_failed_to_be_setup_list;
  broadcast_area_scope_c                    broadcast_area_scope;
  crit_diagnostics_s                        crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// BroadcastContextSetupResponse ::= SEQUENCE
using broadcast_context_setup_resp_s = elementary_procedure_option<broadcast_context_setup_resp_ies_container>;

// BroadcastTransportResourceRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct broadcast_transport_res_request_ies_container {
  bool                                     broadcast_m_rbs_transport_request_list_present = false;
  bool                                     f1_u_path_fail_present                         = false;
  uint64_t                                 gnb_cu_mbs_f1ap_id;
  uint64_t                                 gnb_du_mbs_f1ap_id;
  broadcast_m_rbs_transport_request_list_l broadcast_m_rbs_transport_request_list;
  f1_u_path_fail_e                         f1_u_path_fail;

  static const asn1::table::ie_container_desc desc;
};

// BroadcastTransportResourceRequest ::= SEQUENCE
using broadcast_transport_res_request_s = elementary_procedure_option<broadcast_transport_res_request_ies_container>;

// CUDUCellSwitchNotificationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cu_du_cell_switch_notif_ies_container {
  bool                   ltm_cell_switch_info_present = false;
  bool                   tai_nformation_list_present  = false;
  uint64_t               gnb_cu_ue_f1ap_id;
  uint64_t               gnb_du_ue_f1ap_id;
  nr_cgi_s               nr_cgi;
  ltm_cell_switch_info_s ltm_cell_switch_info;
  tai_nformation_list_l  tai_nformation_list;

  static const asn1::table::ie_container_desc desc;
};

// CUDUCellSwitchNotification ::= SEQUENCE
using cu_du_cell_switch_notif_s = elementary_procedure_option<cu_du_cell_switch_notif_ies_container>;

// CUDUMobilityInitiationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cu_du_mob_initiation_request_ies_container {
  uint64_t         gnb_cu_ue_f1ap_id;
  uint64_t         gnb_du_ue_f1ap_id;
  mob_initiation_c mob_initiation;

  static const asn1::table::ie_container_desc desc;
};

// CUDUMobilityInitiationRequest ::= SEQUENCE
using cu_du_mob_initiation_request_s = elementary_procedure_option<cu_du_mob_initiation_request_ies_container>;

// CUDURadioInformationTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cu_du_radio_info_transfer_ies_container {
  uint16_t                transaction_id;
  cu_du_radio_info_type_c cu_du_radio_info_type;

  static const asn1::table::ie_container_desc desc;
};

// CUDURadioInformationTransfer ::= SEQUENCE
using cu_du_radio_info_transfer_s = elementary_procedure_option<cu_du_radio_info_transfer_ies_container>;

// CUDUTAInformationTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cu_du_tai_nformation_transfer_ies_container {
  uint16_t                       transaction_id;
  cu_to_du_tai_nformation_list_l cu_to_du_tai_nformation_list;

  static const asn1::table::ie_container_desc desc;
};

// CUDUTAInformationTransfer ::= SEQUENCE
using cu_du_tai_nformation_transfer_s = elementary_procedure_option<cu_du_tai_nformation_transfer_ies_container>;

// CellTrafficTraceIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct cell_traffic_trace_ies_container {
  bool                                                  privacy_ind_present                 = false;
  bool                                                  trace_collection_entity_uri_present = false;
  uint64_t                                              gnb_cu_ue_f1ap_id;
  uint64_t                                              gnb_du_ue_f1ap_id;
  fixed_octstring<8, true>                              trace_id;
  bounded_bitstring<1, 160, true, true>                 trace_collection_entity_ip_address;
  privacy_ind_e                                         privacy_ind;
  visible_string<0, MAX_ASN_STRING_LENGTH, false, true> trace_collection_entity_uri;

  static const asn1::table::ie_container_desc desc;
};

// CellTrafficTrace ::= SEQUENCE
using cell_traffic_trace_s = elementary_procedure_option<cell_traffic_trace_ies_container>;

// DUCUAccessAndMobilityIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct du_cu_access_and_mob_ind_ies_container {
  bool                    dl_lbt_fail_info_list_present = false;
  uint16_t                transaction_id;
  dl_lbt_fail_info_list_l dl_lbt_fail_info_list;

  static const asn1::table::ie_container_desc desc;
};

// DUCUAccessAndMobilityIndication ::= SEQUENCE
using du_cu_access_and_mob_ind_s = elementary_procedure_option<du_cu_access_and_mob_ind_ies_container>;

// DUCUCellSwitchNotificationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct du_cu_cell_switch_notif_ies_container {
  bool                   ltm_cell_switch_info_present = false;
  bool                   tai_nformation_list_present  = false;
  uint64_t               gnb_cu_ue_f1ap_id;
  uint64_t               gnb_du_ue_f1ap_id;
  nr_cgi_s               nr_cgi;
  ltm_cell_switch_info_s ltm_cell_switch_info;
  tai_nformation_list_l  tai_nformation_list;

  static const asn1::table::ie_container_desc desc;
};

// DUCUCellSwitchNotification ::= SEQUENCE
using du_cu_cell_switch_notif_s = elementary_procedure_option<du_cu_cell_switch_notif_ies_container>;

// DUCURadioInformationTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct du_cu_radio_info_transfer_ies_container {
  uint16_t                transaction_id;
  du_cu_radio_info_type_c du_cu_radio_info_type;

  static const asn1::table::ie_container_desc desc;
};

// DUCURadioInformationTransfer ::= SEQUENCE
using du_cu_radio_info_transfer_s = elementary_procedure_option<du_cu_radio_info_transfer_ies_container>;

// DUCUTAInformationTransferIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct du_cu_tai_nformation_transfer_ies_container {
  uint16_t                       transaction_id;
  du_to_cu_tai_nformation_list_l du_to_cu_tai_nformation_list;

  static const asn1::table::ie_container_desc desc;
};

// DUCUTAInformationTransfer ::= SEQUENCE
using du_cu_tai_nformation_transfer_s = elementary_procedure_option<du_cu_tai_nformation_transfer_ies_container>;

// DeactivateTraceIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct deactiv_trace_ies_container {
  uint64_t                 gnb_cu_ue_f1ap_id;
  uint64_t                 gnb_du_ue_f1ap_id;
  fixed_octstring<8, true> trace_id;

  static const asn1::table::ie_container_desc desc;
};

// DeactivateTrace ::= SEQUENCE
using deactiv_trace_s = elementary_procedure_option<deactiv_trace_ies_container>;

// E-CIDMeasurementFailureIndication-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_fail_ind_ies_container {
  uint64_t gnb_cu_ue_f1ap_id;
  uint64_t gnb_du_ue_f1ap_id;
  uint16_t lmf_ue_meas_id;
  uint16_t ran_ue_meas_id;
  cause_c  cause;

  static const asn1::table::ie_container_desc desc;
};

// E-CIDMeasurementFailureIndication ::= SEQUENCE
using e_c_id_meas_fail_ind_s = elementary_procedure_option<e_c_id_meas_fail_ind_ies_container>;

// E-CIDMeasurementInitiationFailure-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_initiation_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  uint16_t           lmf_ue_meas_id;
  uint16_t           ran_ue_meas_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// E-CIDMeasurementInitiationFailure ::= SEQUENCE
using e_c_id_meas_initiation_fail_s = elementary_procedure_option<e_c_id_meas_initiation_fail_ies_container>;

// E-CIDMeasurementInitiationRequest-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_initiation_request_ies_container {
  bool                            e_c_id_meas_periodicity_present      = false;
  bool                            pos_meas_periodicity_nr_ao_a_present = false;
  uint64_t                        gnb_cu_ue_f1ap_id;
  uint64_t                        gnb_du_ue_f1ap_id;
  uint16_t                        lmf_ue_meas_id;
  uint16_t                        ran_ue_meas_id;
  e_c_id_report_characteristics_e e_c_id_report_characteristics;
  meas_periodicity_e              e_c_id_meas_periodicity;
  e_c_id_meas_quantities_l        e_c_id_meas_quantities;
  pos_meas_periodicity_nr_ao_a_e  pos_meas_periodicity_nr_ao_a;

  static const asn1::table::ie_container_desc desc;
};

// E-CIDMeasurementInitiationRequest ::= SEQUENCE
using e_c_id_meas_initiation_request_s = elementary_procedure_option<e_c_id_meas_initiation_request_ies_container>;

// E-CIDMeasurementInitiationResponse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_initiation_resp_ies_container {
  bool                 e_c_id_meas_result_present = false;
  bool                 cell_portion_id_present    = false;
  bool                 crit_diagnostics_present   = false;
  uint64_t             gnb_cu_ue_f1ap_id;
  uint64_t             gnb_du_ue_f1ap_id;
  uint16_t             lmf_ue_meas_id;
  uint16_t             ran_ue_meas_id;
  e_c_id_meas_result_s e_c_id_meas_result;
  uint16_t             cell_portion_id;
  crit_diagnostics_s   crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// E-CIDMeasurementInitiationResponse ::= SEQUENCE
using e_c_id_meas_initiation_resp_s = elementary_procedure_option<e_c_id_meas_initiation_resp_ies_container>;

// E-CIDMeasurementReport-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_report_ies_container {
  bool                 cell_portion_id_present = false;
  uint64_t             gnb_cu_ue_f1ap_id;
  uint64_t             gnb_du_ue_f1ap_id;
  uint16_t             lmf_ue_meas_id;
  uint16_t             ran_ue_meas_id;
  e_c_id_meas_result_s e_c_id_meas_result;
  uint16_t             cell_portion_id;

  static const asn1::table::ie_container_desc desc;
};

// E-CIDMeasurementReport ::= SEQUENCE
using e_c_id_meas_report_s = elementary_procedure_option<e_c_id_meas_report_ies_container>;

// E-CIDMeasurementTerminationCommand-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct e_c_id_meas_termination_cmd_ies_container {
  uint64_t gnb_cu_ue_f1ap_id;
  uint64_t gnb_du_ue_f1ap_id;
  uint16_t lmf_ue_meas_id;
  uint16_t ran_ue_meas_id;

  static const asn1::table::ie_container_desc desc;
};

// E-CIDMeasurementTerminationCommand ::= SEQUENCE
using e_c_id_meas_termination_cmd_s = elementary_procedure_option<e_c_id_meas_termination_cmd_ies_container>;

// ErrorIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct error_ind_ies_container {
  bool               gnb_cu_ue_f1ap_id_present = false;
  bool               gnb_du_ue_f1ap_id_present = false;
  bool               cause_present             = false;
  bool               crit_diagnostics_present  = false;
  uint16_t           transaction_id;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// ErrorIndication ::= SEQUENCE
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;

// F1RemovalFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_removal_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// F1RemovalFailure ::= SEQUENCE
using f1_removal_fail_s = elementary_procedure_option<f1_removal_fail_ies_container>;

// F1RemovalRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_removal_request_ies_container {
  uint16_t transaction_id;

  static const asn1::table::ie_container_desc desc;
};

// F1RemovalRequest ::= SEQUENCE
using f1_removal_request_s = elementary_procedure_option<f1_removal_request_ies_container>;

// F1RemovalResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_removal_resp_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// F1RemovalResponse ::= SEQUENCE
using f1_removal_resp_s = elementary_procedure_option<f1_removal_resp_ies_container>;

// F1SetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_setup_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// F1SetupFailure ::= SEQUENCE
using f1_setup_fail_s = elementary_procedure_option<f1_setup_fail_ies_container>;

// F1SetupOutcome ::= ENUMERATED
struct f1_setup_outcome_opts {
  enum options { success, fail, /*...*/ nulltype } value;

  static const asn1::table::enum_desc desc;
};
using f1_setup_outcome_e = enumerated<f1_setup_outcome_opts, true>;

// F1SetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_setup_request_ies_container {
  bool                                 gnb_du_name_present                      = false;
  bool                                 gnb_du_served_cells_list_present         = false;
  bool                                 transport_layer_address_info_present     = false;
  bool                                 bap_address_present                      = false;
  bool                                 extended_gnb_du_name_present             = false;
  bool                                 rrc_terminating_iab_donor_gnb_id_present = false;
  bool                                 mobile_iab_mt_user_location_info_present = false;
  uint16_t                             transaction_id;
  uint64_t                             gnb_du_id;
  printable_string<1, 150, true, true> gnb_du_name;
  gnb_du_served_cells_list_l           gnb_du_served_cells_list;
  rrc_version_s                        gnb_du_rrc_version;
  transport_layer_address_info_s       transport_layer_address_info;
  fixed_bitstring<10, false, true>     bap_address;
  extended_gnb_du_name_s               extended_gnb_du_name;
  global_gnb_id_s                      rrc_terminating_iab_donor_gnb_id;
  mobile_iab_mt_user_location_info_s   mobile_iab_mt_user_location_info;

  static const asn1::table::ie_container_desc desc;
};

// F1SetupRequest ::= SEQUENCE
using f1_setup_request_s = elementary_procedure_option<f1_setup_request_ies_container>;

// F1SetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct f1_setup_resp_ies_container {
  bool                                 gnb_cu_name_present                  = false;
  bool                                 cells_to_be_activ_list_present       = false;
  bool                                 transport_layer_address_info_present = false;
  bool                                 ul_bh_non_up_traffic_map_present     = false;
  bool                                 bap_address_present                  = false;
  bool                                 extended_gnb_cu_name_present         = false;
  bool                                 ncgi_to_be_upd_list_present          = false;
  uint16_t                             transaction_id;
  printable_string<1, 150, true, true> gnb_cu_name;
  cells_to_be_activ_list_l             cells_to_be_activ_list;
  rrc_version_s                        gnb_cu_rrc_version;
  transport_layer_address_info_s       transport_layer_address_info;
  ul_bh_non_up_traffic_map_s           ul_bh_non_up_traffic_map;
  fixed_bitstring<10, false, true>     bap_address;
  extended_gnb_cu_name_s               extended_gnb_cu_name;
  ncgi_to_be_upd_list_l                ncgi_to_be_upd_list;

  static const asn1::table::ie_container_desc desc;
};

// F1SetupResponse ::= SEQUENCE
using f1_setup_resp_s = elementary_procedure_option<f1_setup_resp_ies_container>;

// GNBCUConfigurationUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_cfg_upd_ies_container {
  bool                                 cells_to_be_activ_list_present           = false;
  bool                                 cells_to_be_deactiv_list_present         = false;
  bool                                 gnb_cu_tnl_assoc_to_add_list_present     = false;
  bool                                 gnb_cu_tnl_assoc_to_rem_list_present     = false;
  bool                                 gnb_cu_tnl_assoc_to_upd_list_present     = false;
  bool                                 cells_to_be_barred_list_present          = false;
  bool                                 protected_eutra_res_list_present         = false;
  bool                                 neighbour_cell_info_list_present         = false;
  bool                                 transport_layer_address_info_present     = false;
  bool                                 ul_bh_non_up_traffic_map_present         = false;
  bool                                 bap_address_present                      = false;
  bool                                 cco_assist_info_present                  = false;
  bool                                 cells_for_son_list_present               = false;
  bool                                 gnb_cu_name_present                      = false;
  bool                                 extended_gnb_cu_name_present             = false;
  bool                                 cells_allowed_to_be_deactiv_list_present = false;
  uint16_t                             transaction_id;
  cells_to_be_activ_list_l             cells_to_be_activ_list;
  cells_to_be_deactiv_list_l           cells_to_be_deactiv_list;
  gnb_cu_tnl_assoc_to_add_list_l       gnb_cu_tnl_assoc_to_add_list;
  gnb_cu_tnl_assoc_to_rem_list_l       gnb_cu_tnl_assoc_to_rem_list;
  gnb_cu_tnl_assoc_to_upd_list_l       gnb_cu_tnl_assoc_to_upd_list;
  cells_to_be_barred_list_l            cells_to_be_barred_list;
  protected_eutra_res_list_l           protected_eutra_res_list;
  neighbour_cell_info_list_l           neighbour_cell_info_list;
  transport_layer_address_info_s       transport_layer_address_info;
  ul_bh_non_up_traffic_map_s           ul_bh_non_up_traffic_map;
  fixed_bitstring<10, false, true>     bap_address;
  cco_assist_info_s                    cco_assist_info;
  cells_for_son_list_l                 cells_for_son_list;
  printable_string<1, 150, true, true> gnb_cu_name;
  extended_gnb_cu_name_s               extended_gnb_cu_name;
  cells_allowed_to_be_deactiv_list_l   cells_allowed_to_be_deactiv_list;

  static const asn1::table::ie_container_desc desc;
};

// GNBCUConfigurationUpdate ::= SEQUENCE
using gnb_cu_cfg_upd_s = elementary_procedure_option<gnb_cu_cfg_upd_ies_container>;

// GNBCUConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_cfg_upd_ack_ies_container {
  bool                                    cells_failed_to_be_activ_list_present         = false;
  bool                                    crit_diagnostics_present                      = false;
  bool                                    gnb_cu_tnl_assoc_setup_list_present           = false;
  bool                                    gnb_cu_tnl_assoc_failed_to_setup_list_present = false;
  bool                                    ded_si_delivery_needed_ue_list_present        = false;
  bool                                    transport_layer_address_info_present          = false;
  bool                                    cells_with_ss_bs_activ_list_present           = false;
  uint16_t                                transaction_id;
  cells_failed_to_be_activ_list_l         cells_failed_to_be_activ_list;
  crit_diagnostics_s                      crit_diagnostics;
  gnb_cu_tnl_assoc_setup_list_l           gnb_cu_tnl_assoc_setup_list;
  gnb_cu_tnl_assoc_failed_to_setup_list_l gnb_cu_tnl_assoc_failed_to_setup_list;
  ded_si_delivery_needed_ue_list_l        ded_si_delivery_needed_ue_list;
  transport_layer_address_info_s          transport_layer_address_info;
  cells_with_ss_bs_activ_list_l           cells_with_ss_bs_activ_list;

  static const asn1::table::ie_container_desc desc;
};

// GNBCUConfigurationUpdateAcknowledge ::= SEQUENCE
using gnb_cu_cfg_upd_ack_s = elementary_procedure_option<gnb_cu_cfg_upd_ack_ies_container>;

// GNBCUConfigurationUpdateFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_cu_cfg_upd_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// GNBCUConfigurationUpdateFailure ::= SEQUENCE
using gnb_cu_cfg_upd_fail_s = elementary_procedure_option<gnb_cu_cfg_upd_fail_ies_container>;

// GNBDUConfigurationUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_cfg_upd_ies_container {
  bool                                     served_cells_to_add_list_present               = false;
  bool                                     served_cells_to_modify_list_present            = false;
  bool                                     served_cells_to_delete_list_present            = false;
  bool                                     cells_status_list_present                      = false;
  bool                                     ded_si_delivery_needed_ue_list_present         = false;
  bool                                     gnb_du_id_present                              = false;
  bool                                     gnb_du_tnl_assoc_to_rem_list_present           = false;
  bool                                     transport_layer_address_info_present           = false;
  bool                                     coverage_mod_notif_present                     = false;
  bool                                     gnb_du_name_present                            = false;
  bool                                     extended_gnb_du_name_present                   = false;
  bool                                     rrc_terminating_iab_donor_related_info_present = false;
  bool                                     mobile_iab_mt_user_location_info_present       = false;
  uint16_t                                 transaction_id;
  served_cells_to_add_list_l               served_cells_to_add_list;
  served_cells_to_modify_list_l            served_cells_to_modify_list;
  served_cells_to_delete_list_l            served_cells_to_delete_list;
  cells_status_list_l                      cells_status_list;
  ded_si_delivery_needed_ue_list_l         ded_si_delivery_needed_ue_list;
  uint64_t                                 gnb_du_id;
  gnb_du_tnl_assoc_to_rem_list_l           gnb_du_tnl_assoc_to_rem_list;
  transport_layer_address_info_s           transport_layer_address_info;
  coverage_mod_notif_s                     coverage_mod_notif;
  printable_string<1, 150, true, true>     gnb_du_name;
  extended_gnb_du_name_s                   extended_gnb_du_name;
  rrc_terminating_iab_donor_related_info_s rrc_terminating_iab_donor_related_info;
  mobile_iab_mt_user_location_info_s       mobile_iab_mt_user_location_info;

  static const asn1::table::ie_container_desc desc;
};

// GNBDUConfigurationUpdate ::= SEQUENCE
using gnb_du_cfg_upd_s = elementary_procedure_option<gnb_du_cfg_upd_ies_container>;

// GNBDUConfigurationUpdateAcknowledgeIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_cfg_upd_ack_ies_container {
  bool                             cells_to_be_activ_list_present       = false;
  bool                             crit_diagnostics_present             = false;
  bool                             cells_to_be_deactiv_list_present     = false;
  bool                             transport_layer_address_info_present = false;
  bool                             ul_bh_non_up_traffic_map_present     = false;
  bool                             bap_address_present                  = false;
  bool                             cells_for_son_list_present           = false;
  uint16_t                         transaction_id;
  cells_to_be_activ_list_l         cells_to_be_activ_list;
  crit_diagnostics_s               crit_diagnostics;
  cells_to_be_deactiv_list_l       cells_to_be_deactiv_list;
  transport_layer_address_info_s   transport_layer_address_info;
  ul_bh_non_up_traffic_map_s       ul_bh_non_up_traffic_map;
  fixed_bitstring<10, false, true> bap_address;
  cells_for_son_list_l             cells_for_son_list;

  static const asn1::table::ie_container_desc desc;
};

// GNBDUConfigurationUpdateAcknowledge ::= SEQUENCE
using gnb_du_cfg_upd_ack_s = elementary_procedure_option<gnb_du_cfg_upd_ack_ies_container>;

// GNBDUConfigurationUpdateFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_cfg_upd_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// GNBDUConfigurationUpdateFailure ::= SEQUENCE
using gnb_du_cfg_upd_fail_s = elementary_procedure_option<gnb_du_cfg_upd_fail_ies_container>;

// GNBDUResourceConfigurationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_cfg_ies_container {
  bool                         activ_cells_to_be_upd_list_present = false;
  bool                         child_nodes_list_present           = false;
  bool                         neighbour_node_cells_list_present  = false;
  bool                         serving_cells_list_present         = false;
  uint16_t                     transaction_id;
  activ_cells_to_be_upd_list_l activ_cells_to_be_upd_list;
  child_nodes_list_l           child_nodes_list;
  neighbour_node_cells_list_l  neighbour_node_cells_list;
  serving_cells_list_l         serving_cells_list;

  static const asn1::table::ie_container_desc desc;
};

// GNBDUResourceConfiguration ::= SEQUENCE
using gnb_du_res_cfg_s = elementary_procedure_option<gnb_du_res_cfg_ies_container>;

// GNBDUResourceConfigurationAcknowledgeIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_cfg_ack_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// GNBDUResourceConfigurationAcknowledge ::= SEQUENCE
using gnb_du_res_cfg_ack_s = elementary_procedure_option<gnb_du_res_cfg_ack_ies_container>;

// GNBDUResourceConfigurationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_cfg_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// GNBDUResourceConfigurationFailure ::= SEQUENCE
using gnb_du_res_cfg_fail_s = elementary_procedure_option<gnb_du_res_cfg_fail_ies_container>;

// GNBDUResourceCoordinationRequest-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_coordination_request_ies_container {
  bool                                ignore_res_coordination_container_present = false;
  uint16_t                            transaction_id;
  request_type_e                      request_type;
  unbounded_octstring<true>           eutra_nr_cell_res_coordination_req_container;
  ignore_res_coordination_container_e ignore_res_coordination_container;

  static const asn1::table::ie_container_desc desc;
};

// GNBDUResourceCoordinationRequest ::= SEQUENCE
using gnb_du_res_coordination_request_s = elementary_procedure_option<gnb_du_res_coordination_request_ies_container>;

// GNBDUResourceCoordinationResponse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_res_coordination_resp_ies_container {
  uint16_t                  transaction_id;
  unbounded_octstring<true> eutra_nr_cell_res_coordination_req_ack_container;

  static const asn1::table::ie_container_desc desc;
};

// GNBDUResourceCoordinationResponse ::= SEQUENCE
using gnb_du_res_coordination_resp_s = elementary_procedure_option<gnb_du_res_coordination_resp_ies_container>;

// GNBDUStatusIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct gnb_du_status_ind_ies_container {
  bool                   iab_congestion_ind_present = false;
  uint16_t               transaction_id;
  gnb_du_overload_info_e gnb_du_overload_info;
  iab_congestion_ind_s   iab_congestion_ind;

  static const asn1::table::ie_container_desc desc;
};

// GNBDUStatusIndication ::= SEQUENCE
using gnb_du_status_ind_s = elementary_procedure_option<gnb_du_status_ind_ies_container>;

// IABTNLAddressFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_tnl_address_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// IABTNLAddressFailure ::= SEQUENCE
using iab_tnl_address_fail_s = elementary_procedure_option<iab_tnl_address_fail_ies_container>;

// IABTNLAddressRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_tnl_address_request_ies_container {
  bool                            ia_bv4_addresses_requested_present    = false;
  bool                            iab_ip_v6_request_type_present        = false;
  bool                            iab_tnl_addresses_to_rem_list_present = false;
  bool                            iab_tnl_addresses_exception_present   = false;
  uint16_t                        transaction_id;
  ia_bv4_addresses_requested_s    ia_bv4_addresses_requested;
  iab_ip_v6_request_type_c        iab_ip_v6_request_type;
  iab_tnl_addresses_to_rem_list_l iab_tnl_addresses_to_rem_list;
  iab_tnl_addresses_exception_s   iab_tnl_addresses_exception;

  static const asn1::table::ie_container_desc desc;
};

// IABTNLAddressRequest ::= SEQUENCE
using iab_tnl_address_request_s = elementary_procedure_option<iab_tnl_address_request_ies_container>;

// IABTNLAddressResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_tnl_address_resp_ies_container {
  uint16_t                         transaction_id;
  iab_allocated_tnl_address_list_l iab_allocated_tnl_address_list;

  static const asn1::table::ie_container_desc desc;
};

// IABTNLAddressResponse ::= SEQUENCE
using iab_tnl_address_resp_s = elementary_procedure_option<iab_tnl_address_resp_ies_container>;

// IABUPConfigurationUpdateFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_up_cfg_upd_fail_ies_container {
  bool               time_to_wait_present     = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  time_to_wait_e     time_to_wait;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// IABUPConfigurationUpdateFailure ::= SEQUENCE
using iab_up_cfg_upd_fail_s = elementary_procedure_option<iab_up_cfg_upd_fail_ies_container>;

// IABUPConfigurationUpdateRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_up_cfg_upd_request_ies_container {
  bool                            ul_up_tnl_info_to_upd_list_present    = false;
  bool                            ul_up_tnl_address_to_upd_list_present = false;
  uint16_t                        transaction_id;
  ul_up_tnl_info_to_upd_list_l    ul_up_tnl_info_to_upd_list;
  ul_up_tnl_address_to_upd_list_l ul_up_tnl_address_to_upd_list;

  static const asn1::table::ie_container_desc desc;
};

// IABUPConfigurationUpdateRequest ::= SEQUENCE
using iab_up_cfg_upd_request_s = elementary_procedure_option<iab_up_cfg_upd_request_ies_container>;

// IABUPConfigurationUpdateResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct iab_up_cfg_upd_resp_ies_container {
  bool                            crit_diagnostics_present              = false;
  bool                            dl_up_tnl_address_to_upd_list_present = false;
  uint16_t                        transaction_id;
  crit_diagnostics_s              crit_diagnostics;
  dl_up_tnl_address_to_upd_list_l dl_up_tnl_address_to_upd_list;

  static const asn1::table::ie_container_desc desc;
};

// IABUPConfigurationUpdateResponse ::= SEQUENCE
using iab_up_cfg_upd_resp_s = elementary_procedure_option<iab_up_cfg_upd_resp_ies_container>;

// MC-PagingCell-list ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-Field{F1AP-PROTOCOL-IES : IEsSetParam}
using mc_paging_cell_list_l = dyn_array<protocol_ie_single_container_s<mc_paging_cell_item_ies_o>>;

// MIABF1SetupOutcomeNotificationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct miabf1_setup_outcome_notif_ies_container {
  bool                   activ_cells_map_list_present               = false;
  bool                   target_f1_terminating_donor_gnb_id_present = false;
  uint16_t               transaction_id;
  f1_setup_outcome_e     f1_setup_outcome;
  activ_cells_map_list_l activ_cells_map_list;
  global_gnb_id_s        target_f1_terminating_donor_gnb_id;

  static const asn1::table::ie_container_desc desc;
};

// MIABF1SetupOutcomeNotification ::= SEQUENCE
using miabf1_setup_outcome_notif_s = elementary_procedure_option<miabf1_setup_outcome_notif_ies_container>;

// MIABF1SetupTriggeringIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct miabf1_setup_trigger_ies_container {
  bool                                  target_gnb_ip_address_present   = false;
  bool                                  target_se_gw_ip_address_present = false;
  uint16_t                              transaction_id;
  global_gnb_id_s                       target_gnb_id;
  bounded_bitstring<1, 160, true, true> target_gnb_ip_address;
  bounded_bitstring<1, 160, true, true> target_se_gw_ip_address;

  static const asn1::table::ie_container_desc desc;
};

// MIABF1SetupTriggering ::= SEQUENCE
using miabf1_setup_trigger_s = elementary_procedure_option<miabf1_setup_trigger_ies_container>;

// MeasurementActivation-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct meas_activation_ies_container {
  bool                      prs_meas_info_list_present = false;
  uint64_t                  gnb_cu_ue_f1ap_id;
  uint64_t                  gnb_du_ue_f1ap_id;
  activation_request_type_e activation_request_type;
  prs_meas_info_list_l      prs_meas_info_list;

  static const asn1::table::ie_container_desc desc;
};

// MeasurementActivation ::= SEQUENCE
using meas_activation_s = elementary_procedure_option<meas_activation_ies_container>;

// MeasurementPreconfigurationConfirm-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct meas_precfg_confirm_ies_container {
  bool                        pos_meas_gap_pre_cfg_list_present = false;
  bool                        crit_diagnostics_present          = false;
  uint64_t                    gnb_cu_ue_f1ap_id;
  uint64_t                    gnb_du_ue_f1ap_id;
  pos_meas_gap_pre_cfg_list_s pos_meas_gap_pre_cfg_list;
  crit_diagnostics_s          crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// MeasurementPreconfigurationConfirm ::= SEQUENCE
using meas_precfg_confirm_s = elementary_procedure_option<meas_precfg_confirm_ies_container>;

// MeasurementPreconfigurationRefuse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct meas_precfg_refuse_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// MeasurementPreconfigurationRefuse ::= SEQUENCE
using meas_precfg_refuse_s = elementary_procedure_option<meas_precfg_refuse_ies_container>;

// MeasurementPreconfigurationRequired-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct meas_precfg_required_ies_container {
  uint64_t            gnb_cu_ue_f1ap_id;
  uint64_t            gnb_du_ue_f1ap_id;
  trp_prs_info_list_l trp_prs_info_list;

  static const asn1::table::ie_container_desc desc;
};

// MeasurementPreconfigurationRequired ::= SEQUENCE
using meas_precfg_required_s = elementary_procedure_option<meas_precfg_required_ies_container>;

// MulticastCommonConfigurationRefuseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_common_cfg_refuse_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// MulticastCommonConfigurationRefuse ::= SEQUENCE
using multicast_common_cfg_refuse_s = elementary_procedure_option<multicast_common_cfg_refuse_ies_container>;

// MulticastCommonConfigurationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_common_cfg_request_ies_container {
  bool                               multicast_cu2_du_common_rrc_info_present = false;
  uint16_t                           transaction_id;
  multicast_cu2_du_common_rrc_info_s multicast_cu2_du_common_rrc_info;

  static const asn1::table::ie_container_desc desc;
};

// MulticastCommonConfigurationRequest ::= SEQUENCE
using multicast_common_cfg_request_s = elementary_procedure_option<multicast_common_cfg_request_ies_container>;

// MulticastCommonConfigurationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_common_cfg_resp_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// MulticastCommonConfigurationResponse ::= SEQUENCE
using multicast_common_cfg_resp_s = elementary_procedure_option<multicast_common_cfg_resp_ies_container>;

// MulticastContextModificationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_mod_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextModificationFailure ::= SEQUENCE
using multicast_context_mod_fail_s = elementary_procedure_option<multicast_context_mod_fail_ies_container>;

// MulticastContextModificationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_mod_request_ies_container {
  bool                                    mbs_service_area_present                      = false;
  bool                                    multicast_m_rbs_to_be_setup_mod_list_present  = false;
  bool                                    multicast_m_rbs_to_be_modified_list_present   = false;
  bool                                    multicast_m_rbs_to_be_released_list_present   = false;
  bool                                    multicast_cu2_du_rrc_info_present             = false;
  bool                                    mbs_multicast_session_reception_state_present = false;
  uint64_t                                gnb_cu_mbs_f1ap_id;
  uint64_t                                gnb_du_mbs_f1ap_id;
  mbs_service_area_c                      mbs_service_area;
  multicast_m_rbs_to_be_setup_mod_list_l  multicast_m_rbs_to_be_setup_mod_list;
  multicast_m_rbs_to_be_modified_list_l   multicast_m_rbs_to_be_modified_list;
  multicast_m_rbs_to_be_released_list_l   multicast_m_rbs_to_be_released_list;
  multicast_cu2_du_rrc_info_s             multicast_cu2_du_rrc_info;
  mbs_multicast_session_reception_state_e mbs_multicast_session_reception_state;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextModificationRequest ::= SEQUENCE
using multicast_context_mod_request_s = elementary_procedure_option<multicast_context_mod_request_ies_container>;

// MulticastContextModificationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_mod_resp_ies_container {
  bool                                          multicast_m_rbs_setup_mod_list_present              = false;
  bool                                          multicast_m_rbs_failed_to_be_setup_mod_list_present = false;
  bool                                          multicast_m_rbs_modified_list_present               = false;
  bool                                          multicast_m_rbs_failed_to_be_modified_list_present  = false;
  bool                                          crit_diagnostics_present                            = false;
  bool                                          multicast_du2_cu_rrc_info_present                   = false;
  uint64_t                                      gnb_cu_mbs_f1ap_id;
  uint64_t                                      gnb_du_mbs_f1ap_id;
  multicast_m_rbs_setup_mod_list_l              multicast_m_rbs_setup_mod_list;
  multicast_m_rbs_failed_to_be_setup_mod_list_l multicast_m_rbs_failed_to_be_setup_mod_list;
  multicast_m_rbs_modified_list_l               multicast_m_rbs_modified_list;
  multicast_m_rbs_failed_to_be_modified_list_l  multicast_m_rbs_failed_to_be_modified_list;
  crit_diagnostics_s                            crit_diagnostics;
  multicast_du2_cu_rrc_info_s                   multicast_du2_cu_rrc_info;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextModificationResponse ::= SEQUENCE
using multicast_context_mod_resp_s = elementary_procedure_option<multicast_context_mod_resp_ies_container>;

// MulticastContextNotificationConfirmIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_notif_confirm_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextNotificationConfirm ::= SEQUENCE
using multicast_context_notif_confirm_s = elementary_procedure_option<multicast_context_notif_confirm_ies_container>;

// MulticastContextNotificationIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_notif_ind_ies_container {
  bool                        multicast_du2_cu_rrc_info_present = false;
  uint64_t                    gnb_cu_mbs_f1ap_id;
  uint64_t                    gnb_du_mbs_f1ap_id;
  multicast_du2_cu_rrc_info_s multicast_du2_cu_rrc_info;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextNotificationIndication ::= SEQUENCE
using multicast_context_notif_ind_s = elementary_procedure_option<multicast_context_notif_ind_ies_container>;

// MulticastContextNotificationRefuseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_notif_refuse_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  crit_diagnostics_s crit_diagnostics;
  cause_c            cause;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextNotificationRefuse ::= SEQUENCE
using multicast_context_notif_refuse_s = elementary_procedure_option<multicast_context_notif_refuse_ies_container>;

// MulticastContextReleaseCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_release_cmd_ies_container {
  uint64_t gnb_cu_mbs_f1ap_id;
  uint64_t gnb_du_mbs_f1ap_id;
  cause_c  cause;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextReleaseCommand ::= SEQUENCE
using multicast_context_release_cmd_s = elementary_procedure_option<multicast_context_release_cmd_ies_container>;

// MulticastContextReleaseCompleteIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_release_complete_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextReleaseComplete ::= SEQUENCE
using multicast_context_release_complete_s =
    elementary_procedure_option<multicast_context_release_complete_ies_container>;

// MulticastContextReleaseRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_release_request_ies_container {
  uint64_t gnb_cu_mbs_f1ap_id;
  uint64_t gnb_du_mbs_f1ap_id;
  cause_c  cause;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextReleaseRequest ::= SEQUENCE
using multicast_context_release_request_s =
    elementary_procedure_option<multicast_context_release_request_ies_container>;

// MulticastContextSetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_setup_fail_ies_container {
  bool               gnb_du_mbs_f1ap_id_present = false;
  bool               crit_diagnostics_present   = false;
  uint64_t           gnb_cu_mbs_f1ap_id;
  uint64_t           gnb_du_mbs_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextSetupFailure ::= SEQUENCE
using multicast_context_setup_fail_s = elementary_procedure_option<multicast_context_setup_fail_ies_container>;

// MulticastContextSetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_setup_request_ies_container {
  bool                                    mbs_service_area_present                      = false;
  bool                                    multicast_cu2_du_rrc_info_present             = false;
  bool                                    mbs_multicast_session_reception_state_present = false;
  uint64_t                                gnb_cu_mbs_f1ap_id;
  mbs_session_id_s                        mbs_session_id;
  mbs_service_area_c                      mbs_service_area;
  snssai_s                                snssai;
  multicast_m_rbs_to_be_setup_list_l      multicast_m_rbs_to_be_setup_list;
  multicast_cu2_du_rrc_info_s             multicast_cu2_du_rrc_info;
  mbs_multicast_session_reception_state_e mbs_multicast_session_reception_state;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextSetupRequest ::= SEQUENCE
using multicast_context_setup_request_s = elementary_procedure_option<multicast_context_setup_request_ies_container>;

// MulticastContextSetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_context_setup_resp_ies_container {
  bool                                      multicast_m_rbs_failed_to_be_setup_list_present = false;
  bool                                      crit_diagnostics_present                        = false;
  bool                                      multicast_du2_cu_rrc_info_present               = false;
  uint64_t                                  gnb_cu_mbs_f1ap_id;
  uint64_t                                  gnb_du_mbs_f1ap_id;
  multicast_m_rbs_setup_list_l              multicast_m_rbs_setup_list;
  multicast_m_rbs_failed_to_be_setup_list_l multicast_m_rbs_failed_to_be_setup_list;
  crit_diagnostics_s                        crit_diagnostics;
  multicast_du2_cu_rrc_info_s               multicast_du2_cu_rrc_info;

  static const asn1::table::ie_container_desc desc;
};

// MulticastContextSetupResponse ::= SEQUENCE
using multicast_context_setup_resp_s = elementary_procedure_option<multicast_context_setup_resp_ies_container>;

// MulticastDistributionReleaseCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_distribution_release_cmd_ies_container {
  uint64_t                                gnb_cu_mbs_f1ap_id;
  uint64_t                                gnb_du_mbs_f1ap_id;
  mbs_multicast_f1_u_context_descriptor_s mbs_multicast_f1_u_context_descriptor;
  cause_c                                 cause;

  static const asn1::table::ie_container_desc desc;
};

// MulticastDistributionReleaseCommand ::= SEQUENCE
using multicast_distribution_release_cmd_s =
    elementary_procedure_option<multicast_distribution_release_cmd_ies_container>;

// MulticastDistributionReleaseCompleteIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_distribution_release_complete_ies_container {
  bool                                    crit_diagnostics_present = false;
  uint64_t                                gnb_cu_mbs_f1ap_id;
  uint64_t                                gnb_du_mbs_f1ap_id;
  mbs_multicast_f1_u_context_descriptor_s mbs_multicast_f1_u_context_descriptor;
  crit_diagnostics_s                      crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// MulticastDistributionReleaseComplete ::= SEQUENCE
using multicast_distribution_release_complete_s =
    elementary_procedure_option<multicast_distribution_release_complete_ies_container>;

// MulticastDistributionSetupFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_distribution_setup_fail_ies_container {
  bool                                    gnb_du_mbs_f1ap_id_present = false;
  bool                                    crit_diagnostics_present   = false;
  uint64_t                                gnb_cu_mbs_f1ap_id;
  uint64_t                                gnb_du_mbs_f1ap_id;
  mbs_multicast_f1_u_context_descriptor_s mbs_multicast_f1_u_context_descriptor;
  cause_c                                 cause;
  crit_diagnostics_s                      crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// MulticastDistributionSetupFailure ::= SEQUENCE
using multicast_distribution_setup_fail_s =
    elementary_procedure_option<multicast_distribution_setup_fail_ies_container>;

// MulticastDistributionSetupRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_distribution_setup_request_ies_container {
  uint64_t                                  gnb_cu_mbs_f1ap_id;
  uint64_t                                  gnb_du_mbs_f1ap_id;
  mbs_multicast_f1_u_context_descriptor_s   mbs_multicast_f1_u_context_descriptor;
  multicast_f1_u_context_to_be_setup_list_l multicast_f1_u_context_to_be_setup_list;

  static const asn1::table::ie_container_desc desc;
};

// MulticastDistributionSetupRequest ::= SEQUENCE
using multicast_distribution_setup_request_s =
    elementary_procedure_option<multicast_distribution_setup_request_ies_container>;

// MulticastDistributionSetupResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_distribution_setup_resp_ies_container {
  bool                                             multicast_f1_u_context_failed_to_be_setup_list_present = false;
  bool                                             crit_diagnostics_present                               = false;
  uint64_t                                         gnb_cu_mbs_f1ap_id;
  uint64_t                                         gnb_du_mbs_f1ap_id;
  mbs_multicast_f1_u_context_descriptor_s          mbs_multicast_f1_u_context_descriptor;
  multicast_f1_u_context_setup_list_l              multicast_f1_u_context_setup_list;
  multicast_f1_u_context_failed_to_be_setup_list_l multicast_f1_u_context_failed_to_be_setup_list;
  crit_diagnostics_s                               crit_diagnostics;
  fixed_octstring<4, true>                         multicast_f1_u_context_ref_cu;

  static const asn1::table::ie_container_desc desc;
};

// MulticastDistributionSetupResponse ::= SEQUENCE
using multicast_distribution_setup_resp_s =
    elementary_procedure_option<multicast_distribution_setup_resp_ies_container>;

// MulticastGroupPagingIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct multicast_group_paging_ies_container {
  bool                         ue_id_list_for_paging_list_present = false;
  bool                         mc_paging_cell_list_present        = false;
  bool                         ind_mc_inactive_reception_present  = false;
  mbs_session_id_s             mbs_session_id;
  ue_id_list_for_paging_list_l ue_id_list_for_paging_list;
  mc_paging_cell_list_l        mc_paging_cell_list;
  ind_mc_inactive_reception_e  ind_mc_inactive_reception;

  static const asn1::table::ie_container_desc desc;
};

// MulticastGroupPaging ::= SEQUENCE
using multicast_group_paging_s = elementary_procedure_option<multicast_group_paging_ies_container>;

// NetworkAccessRateReductionIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct network_access_rate_reduction_ies_container {
  uint16_t          transaction_id;
  uac_assist_info_s uac_assist_info;

  static const asn1::table::ie_container_desc desc;
};

// NetworkAccessRateReduction ::= SEQUENCE
using network_access_rate_reduction_s = elementary_procedure_option<network_access_rate_reduction_ies_container>;

// NotifyIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct notify_ies_container {
  uint64_t          gnb_cu_ue_f1ap_id;
  uint64_t          gnb_du_ue_f1ap_id;
  drb_notify_list_l drb_notify_list;

  static const asn1::table::ie_container_desc desc;
};

// Notify ::= SEQUENCE
using notify_s = elementary_procedure_option<notify_ies_container>;

// PDCMeasurementFailureIndication-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_fail_ind_ies_container {
  uint64_t gnb_cu_ue_f1ap_id;
  uint64_t gnb_du_ue_f1ap_id;
  uint8_t  ran_ue_pdc_meas_id;
  cause_c  cause;

  static const asn1::table::ie_container_desc desc;
};

// PDCMeasurementFailureIndication ::= SEQUENCE
using pdc_meas_fail_ind_s = elementary_procedure_option<pdc_meas_fail_ind_ies_container>;

// PDCMeasurementInitiationFailure-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_initiation_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  uint8_t            ran_ue_pdc_meas_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// PDCMeasurementInitiationFailure ::= SEQUENCE
using pdc_meas_initiation_fail_s = elementary_procedure_option<pdc_meas_initiation_fail_ies_container>;

// PDCMeasurementInitiationRequest-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_initiation_request_ies_container {
  bool                   pdc_meas_periodicity_present = false;
  uint64_t               gnb_cu_ue_f1ap_id;
  uint64_t               gnb_du_ue_f1ap_id;
  uint8_t                ran_ue_pdc_meas_id;
  pdc_report_type_e      pdc_report_type;
  pdc_meas_periodicity_e pdc_meas_periodicity;
  pdc_meas_quantities_l  pdc_meas_quantities;

  static const asn1::table::ie_container_desc desc;
};

// PDCMeasurementInitiationRequest ::= SEQUENCE
using pdc_meas_initiation_request_s = elementary_procedure_option<pdc_meas_initiation_request_ies_container>;

// PDCMeasurementInitiationResponse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_initiation_resp_ies_container {
  bool               pdc_meas_result_present  = false;
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  uint8_t            ran_ue_pdc_meas_id;
  pdc_meas_result_s  pdc_meas_result;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// PDCMeasurementInitiationResponse ::= SEQUENCE
using pdc_meas_initiation_resp_s = elementary_procedure_option<pdc_meas_initiation_resp_ies_container>;

// PDCMeasurementReport-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_report_ies_container {
  uint64_t          gnb_cu_ue_f1ap_id;
  uint64_t          gnb_du_ue_f1ap_id;
  uint8_t           ran_ue_pdc_meas_id;
  pdc_meas_result_s pdc_meas_result;

  static const asn1::table::ie_container_desc desc;
};

// PDCMeasurementReport ::= SEQUENCE
using pdc_meas_report_s = elementary_procedure_option<pdc_meas_report_ies_container>;

// PDCMeasurementTerminationCommand-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pdc_meas_termination_cmd_ies_container {
  uint64_t gnb_cu_ue_f1ap_id;
  uint64_t gnb_du_ue_f1ap_id;
  uint8_t  ran_ue_pdc_meas_id;

  static const asn1::table::ie_container_desc desc;
};

// PDCMeasurementTerminationCommand ::= SEQUENCE
using pdc_meas_termination_cmd_s = elementary_procedure_option<pdc_meas_termination_cmd_ies_container>;

// PRSConfigurationFailure-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct prs_cfg_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// PRSConfigurationFailure ::= SEQUENCE
using prs_cfg_fail_s = elementary_procedure_option<prs_cfg_fail_ies_container>;

// PRSConfigurationRequest-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct prs_cfg_request_ies_container {
  uint16_t               transaction_id;
  prs_cfg_request_type_e prs_cfg_request_type;
  prstrp_list_l          prstrp_list;

  static const asn1::table::ie_container_desc desc;
};

// PRSConfigurationRequest ::= SEQUENCE
using prs_cfg_request_s = elementary_procedure_option<prs_cfg_request_ies_container>;

// PRSConfigurationResponse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct prs_cfg_resp_ies_container {
  bool               prs_tx_trp_list_present  = false;
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  prs_tx_trp_list_l  prs_tx_trp_list;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// PRSConfigurationResponse ::= SEQUENCE
using prs_cfg_resp_s = elementary_procedure_option<prs_cfg_resp_ies_container>;

// PWSCancelRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_cancel_request_ies_container {
  bool                             broadcast_to_be_cancelled_list_present = false;
  bool                             cancel_all_warning_msgs_ind_present    = false;
  bool                             notif_info_present                     = false;
  uint16_t                         transaction_id;
  uint32_t                         numof_broadcast_request;
  broadcast_to_be_cancelled_list_l broadcast_to_be_cancelled_list;
  cancel_all_warning_msgs_ind_e    cancel_all_warning_msgs_ind;
  notif_info_s                     notif_info;

  static const asn1::table::ie_container_desc desc;
};

// PWSCancelRequest ::= SEQUENCE
using pws_cancel_request_s = elementary_procedure_option<pws_cancel_request_ies_container>;

// PWSCancelResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_cancel_resp_ies_container {
  bool                             cells_broadcast_cancelled_list_present = false;
  bool                             crit_diagnostics_present               = false;
  uint16_t                         transaction_id;
  cells_broadcast_cancelled_list_l cells_broadcast_cancelled_list;
  crit_diagnostics_s               crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// PWSCancelResponse ::= SEQUENCE
using pws_cancel_resp_s = elementary_procedure_option<pws_cancel_resp_ies_container>;

// PWSFailureIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_fail_ind_ies_container {
  bool                     pws_failed_nr_cgi_list_present = false;
  uint16_t                 transaction_id;
  pws_failed_nr_cgi_list_l pws_failed_nr_cgi_list;

  static const asn1::table::ie_container_desc desc;
};

// PWSFailureIndication ::= SEQUENCE
using pws_fail_ind_s = elementary_procedure_option<pws_fail_ind_ies_container>;

// PWSRestartIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pws_restart_ind_ies_container {
  uint16_t                       transaction_id;
  nr_cgi_list_for_restart_list_l nr_cgi_list_for_restart_list;

  static const asn1::table::ie_container_desc desc;
};

// PWSRestartIndication ::= SEQUENCE
using pws_restart_ind_s = elementary_procedure_option<pws_restart_ind_ies_container>;

// PagingCell-list ::= SEQUENCE (SIZE (1..512)) OF ProtocolIE-Field{F1AP-PROTOCOL-IES : IEsSetParam}
using paging_cell_list_l = dyn_array<protocol_ie_single_container_s<paging_cell_item_ies_o>>;

// PagingIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct paging_ies_container {
  bool                                      paging_drx_present                              = false;
  bool                                      paging_prio_present                             = false;
  bool                                      paging_origin_present                           = false;
  bool                                      ran_ue_paging_drx_present                       = false;
  bool                                      cn_ue_paging_drx_present                        = false;
  bool                                      nr_paginge_drx_info_present                     = false;
  bool                                      nr_paginge_drx_infofor_rrc_inactive_present     = false;
  bool                                      paging_cause_present                            = false;
  bool                                      pe_ip_s_assist_info_present                     = false;
  bool                                      ue_paging_cap_present                           = false;
  bool                                      extended_ue_id_idx_value_present                = false;
  bool                                      hashed_ue_id_idx_value_present                  = false;
  bool                                      mt_sdt_info_present                             = false;
  bool                                      nr_paginglonge_drx_infofor_rrc_inactive_present = false;
  ue_id_idx_value_c                         ue_id_idx_value;
  paging_id_c                               paging_id;
  paging_drx_e                              paging_drx;
  paging_prio_e                             paging_prio;
  paging_cell_list_l                        paging_cell_list;
  paging_origin_e                           paging_origin;
  paging_drx_e                              ran_ue_paging_drx;
  paging_drx_e                              cn_ue_paging_drx;
  nr_paginge_drx_info_s                     nr_paginge_drx_info;
  nr_paginge_drx_infofor_rrc_inactive_s     nr_paginge_drx_infofor_rrc_inactive;
  paging_cause_e                            paging_cause;
  pe_ip_s_assist_info_s                     pe_ip_s_assist_info;
  ue_paging_cap_s                           ue_paging_cap;
  fixed_bitstring<16, false, true>          extended_ue_id_idx_value;
  fixed_bitstring<13, true, true>           hashed_ue_id_idx_value;
  mt_sdt_info_s                             mt_sdt_info;
  nr_paginglonge_drx_infofor_rrc_inactive_s nr_paginglonge_drx_infofor_rrc_inactive;

  static const asn1::table::ie_container_desc desc;
};

// Paging ::= SEQUENCE
using paging_s = elementary_procedure_option<paging_ies_container>;

// PosSystemInformationDeliveryCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct pos_sys_info_delivery_cmd_ies_container {
  uint16_t          transaction_id;
  nr_cgi_s          nr_cgi;
  pos_sitype_list_l pos_sitype_list;
  uint64_t          confirmed_ue_id;

  static const asn1::table::ie_container_desc desc;
};

// PosSystemInformationDeliveryCommand ::= SEQUENCE
using pos_sys_info_delivery_cmd_s = elementary_procedure_option<pos_sys_info_delivery_cmd_ies_container>;

// PositioningActivationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_activation_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// PositioningActivationFailure ::= SEQUENCE
using positioning_activation_fail_s = elementary_procedure_option<positioning_activation_fail_ies_container>;

// SemipersistentSRS-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-EXTENSION
struct semipersistent_srs_ext_ies_container {
  bool                           srs_spatial_relation_per_srs_res_present = false;
  bool                           aggr_pos_srs_res_set_list_present        = false;
  spatial_relation_per_srs_res_s srs_spatial_relation_per_srs_res;
  aggr_pos_srs_res_set_list_l    aggr_pos_srs_res_set_list;

  static const asn1::table::ie_container_desc desc;
};

// SemipersistentSRS ::= SEQUENCE
struct semipersistent_srs_s {
  bool                                 srs_spatial_relation_present = false;
  bool                                 ie_exts_present              = false;
  uint8_t                              srs_res_set_id               = 0;
  spatial_relation_info_s              srs_spatial_relation;
  semipersistent_srs_ext_ies_container ie_exts;
  // ...

  static const asn1::table::seq_desc desc;
};

// SRSType-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using srs_type_ext_ies_o = protocol_ies_empty_o;

// SRSType ::= CHOICE
struct srs_type_c {
  struct types_opts {
    enum options { semipersistent_srs, aperiodic_srs, choice_ext, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  semipersistent_srs_s&                         semipersistent_srs()
  {
    assert_choice_type(types::semipersistent_srs, c.type, "SRSType");
    return c.get<semipersistent_srs_s>();
  }
  aperiodic_srs_s& aperiodic_srs()
  {
    assert_choice_type(types::aperiodic_srs, c.type, "SRSType");
    return c.get<aperiodic_srs_s>();
  }
  protocol_ie_single_container_s<srs_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, c.type, "SRSType");
    return c.get<protocol_ie_single_container_s<srs_type_ext_ies_o>>();
  }
  const semipersistent_srs_s& semipersistent_srs() const
  {
    assert_choice_type(types::semipersistent_srs, c.type, "SRSType");
    return c.get<semipersistent_srs_s>();
  }
  const aperiodic_srs_s& aperiodic_srs() const
  {
    assert_choice_type(types::aperiodic_srs, c.type, "SRSType");
    return c.get<aperiodic_srs_s>();
  }
  const protocol_ie_single_container_s<srs_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, c.type, "SRSType");
    return c.get<protocol_ie_single_container_s<srs_type_ext_ies_o>>();
  }
  semipersistent_srs_s&                               set_semipersistent_srs();
  aperiodic_srs_s&                                    set_aperiodic_srs();
  protocol_ie_single_container_s<srs_type_ext_ies_o>& set_choice_ext();

private:
  asn1::table::choice_storage<
      srs_type_c,
      types,
      choice_buffer_t<aperiodic_srs_s, protocol_ie_single_container_s<srs_type_ext_ies_o>, semipersistent_srs_s>>
      c;
};

// PositioningActivationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_activation_request_ies_container {
  bool                             activation_time_present = false;
  uint64_t                         gnb_cu_ue_f1ap_id;
  uint64_t                         gnb_du_ue_f1ap_id;
  srs_type_c                       srs_type;
  fixed_bitstring<64, false, true> activation_time;

  static const asn1::table::ie_container_desc desc;
};

// PositioningActivationRequest ::= SEQUENCE
using positioning_activation_request_s = elementary_procedure_option<positioning_activation_request_ies_container>;

// PositioningActivationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_activation_resp_ies_container {
  bool               sys_frame_num_present    = false;
  bool               slot_num_present         = false;
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  uint16_t           sys_frame_num;
  uint8_t            slot_num;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// PositioningActivationResponse ::= SEQUENCE
using positioning_activation_resp_s = elementary_procedure_option<positioning_activation_resp_ies_container>;

// PositioningAssistanceInformationControlIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_assist_info_ctrl_ies_container {
  bool                          pos_assist_info_present             = false;
  bool                          pos_broadcast_present               = false;
  bool                          positioning_broadcast_cells_present = false;
  bool                          routing_id_present                  = false;
  uint16_t                      transaction_id;
  unbounded_octstring<true>     pos_assist_info;
  pos_broadcast_e               pos_broadcast;
  positioning_broadcast_cells_l positioning_broadcast_cells;
  unbounded_octstring<true>     routing_id;

  static const asn1::table::ie_container_desc desc;
};

// PositioningAssistanceInformationControl ::= SEQUENCE
using positioning_assist_info_ctrl_s = elementary_procedure_option<positioning_assist_info_ctrl_ies_container>;

// PositioningAssistanceInformationFeedbackIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_assist_info_feedback_ies_container {
  bool                          pos_assist_info_fail_list_present   = false;
  bool                          positioning_broadcast_cells_present = false;
  bool                          routing_id_present                  = false;
  bool                          crit_diagnostics_present            = false;
  uint16_t                      transaction_id;
  unbounded_octstring<true>     pos_assist_info_fail_list;
  positioning_broadcast_cells_l positioning_broadcast_cells;
  unbounded_octstring<true>     routing_id;
  crit_diagnostics_s            crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// PositioningAssistanceInformationFeedback ::= SEQUENCE
using positioning_assist_info_feedback_s = elementary_procedure_option<positioning_assist_info_feedback_ies_container>;

// PositioningDeactivationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_deactivation_ies_container {
  uint64_t   gnb_cu_ue_f1ap_id;
  uint64_t   gnb_du_ue_f1ap_id;
  abort_tx_c abort_tx;

  static const asn1::table::ie_container_desc desc;
};

// PositioningDeactivation ::= SEQUENCE
using positioning_deactivation_s = elementary_procedure_option<positioning_deactivation_ies_container>;

// PositioningInformationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_info_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// PositioningInformationFailure ::= SEQUENCE
using positioning_info_fail_s = elementary_procedure_option<positioning_info_fail_ies_container>;

// PositioningInformationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_info_request_ies_container {
  bool                                        requested_srs_tx_characteristics_present          = false;
  bool                                        ue_report_info_present                            = false;
  bool                                        srs_pos_rrc_inactive_query_ind_present            = false;
  bool                                        time_win_info_srs_list_present                    = false;
  bool                                        requested_srs_precfg_characteristics_list_present = false;
  uint64_t                                    gnb_cu_ue_f1ap_id;
  uint64_t                                    gnb_du_ue_f1ap_id;
  requested_srs_tx_characteristics_s          requested_srs_tx_characteristics;
  ue_report_info_s                            ue_report_info;
  srs_pos_rrc_inactive_query_ind_e            srs_pos_rrc_inactive_query_ind;
  time_win_info_srs_list_l                    time_win_info_srs_list;
  requested_srs_precfg_characteristics_list_l requested_srs_precfg_characteristics_list;

  static const asn1::table::ie_container_desc desc;
};

// PositioningInformationRequest ::= SEQUENCE
using positioning_info_request_s = elementary_procedure_option<positioning_info_request_ies_container>;

// PositioningInformationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_info_resp_ies_container {
  bool                             srs_cfg_present                                = false;
  bool                             sfn_initisation_time_present                   = false;
  bool                             crit_diagnostics_present                       = false;
  bool                             srs_pos_rrc_inactive_cfg_present               = false;
  bool                             srs_pos_rrc_inactive_validity_area_cfg_present = false;
  bool                             srs_precfg_list_present                        = false;
  uint64_t                         gnb_cu_ue_f1ap_id;
  uint64_t                         gnb_du_ue_f1ap_id;
  srs_configuration_s              srs_cfg;
  fixed_bitstring<64, false, true> sfn_initisation_time;
  crit_diagnostics_s               crit_diagnostics;
  unbounded_octstring<true>        srs_pos_rrc_inactive_cfg;
  unbounded_octstring<true>        srs_pos_rrc_inactive_validity_area_cfg;
  srs_precfg_list_l                srs_precfg_list;

  static const asn1::table::ie_container_desc desc;
};

// PositioningInformationResponse ::= SEQUENCE
using positioning_info_resp_s = elementary_procedure_option<positioning_info_resp_ies_container>;

// PositioningInformationUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_info_upd_ies_container {
  bool                             srs_cfg_present              = false;
  bool                             sfn_initisation_time_present = false;
  uint64_t                         gnb_cu_ue_f1ap_id;
  uint64_t                         gnb_du_ue_f1ap_id;
  srs_configuration_s              srs_cfg;
  fixed_bitstring<64, false, true> sfn_initisation_time;

  static const asn1::table::ie_container_desc desc;
};

// PositioningInformationUpdate ::= SEQUENCE
using positioning_info_upd_s = elementary_procedure_option<positioning_info_upd_ies_container>;

// PositioningMeasurementAbortIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_abort_ies_container {
  uint16_t transaction_id;
  uint32_t lmf_meas_id;
  uint32_t ran_meas_id;

  static const asn1::table::ie_container_desc desc;
};

// PositioningMeasurementAbort ::= SEQUENCE
using positioning_meas_abort_s = elementary_procedure_option<positioning_meas_abort_ies_container>;

// PositioningMeasurementFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  uint32_t           lmf_meas_id;
  uint32_t           ran_meas_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// PositioningMeasurementFailure ::= SEQUENCE
using positioning_meas_fail_s = elementary_procedure_option<positioning_meas_fail_ies_container>;

// PositioningMeasurementFailureIndicationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_fail_ind_ies_container {
  uint16_t transaction_id;
  uint32_t lmf_meas_id;
  uint32_t ran_meas_id;
  cause_c  cause;

  static const asn1::table::ie_container_desc desc;
};

// PositioningMeasurementFailureIndication ::= SEQUENCE
using positioning_meas_fail_ind_s = elementary_procedure_option<positioning_meas_fail_ind_ies_container>;

// PositioningMeasurementReportIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_report_ies_container {
  uint16_t               transaction_id;
  uint32_t               lmf_meas_id;
  uint32_t               ran_meas_id;
  pos_meas_result_list_l pos_meas_result_list;

  static const asn1::table::ie_container_desc desc;
};

// PositioningMeasurementReport ::= SEQUENCE
using positioning_meas_report_s = elementary_procedure_option<positioning_meas_report_ies_container>;

// PositioningMeasurementRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_request_ies_container {
  bool                             pos_meas_periodicity_present             = false;
  bool                             sfn_initisation_time_present             = false;
  bool                             srs_cfg_present                          = false;
  bool                             meas_beam_info_request_present           = false;
  bool                             sys_frame_num_present                    = false;
  bool                             slot_num_present                         = false;
  bool                             pos_meas_periodicity_extended_present    = false;
  bool                             resp_time_present                        = false;
  bool                             meas_characteristics_request_ind_present = false;
  bool                             meas_time_occasion_present               = false;
  bool                             pos_meas_amount_present                  = false;
  bool                             time_win_info_meas_list_present          = false;
  uint16_t                         transaction_id;
  uint32_t                         lmf_meas_id;
  uint32_t                         ran_meas_id;
  trp_meas_request_list_l          trp_meas_request_list;
  pos_report_characteristics_e     pos_report_characteristics;
  meas_periodicity_e               pos_meas_periodicity;
  pos_meas_quantities_l            pos_meas_quantities;
  fixed_bitstring<64, false, true> sfn_initisation_time;
  srs_configuration_s              srs_cfg;
  meas_beam_info_request_e         meas_beam_info_request;
  uint16_t                         sys_frame_num;
  uint8_t                          slot_num;
  meas_periodicity_extended_e      pos_meas_periodicity_extended;
  resp_time_s                      resp_time;
  fixed_bitstring<16, false, true> meas_characteristics_request_ind;
  meas_time_occasion_e             meas_time_occasion;
  pos_meas_amount_e                pos_meas_amount;
  time_win_info_meas_list_l        time_win_info_meas_list;

  static const asn1::table::ie_container_desc desc;
};

// PositioningMeasurementRequest ::= SEQUENCE
using positioning_meas_request_s = elementary_procedure_option<positioning_meas_request_ies_container>;

// PositioningMeasurementResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_resp_ies_container {
  bool                   pos_meas_result_list_present = false;
  bool                   crit_diagnostics_present     = false;
  uint16_t               transaction_id;
  uint32_t               lmf_meas_id;
  uint32_t               ran_meas_id;
  pos_meas_result_list_l pos_meas_result_list;
  crit_diagnostics_s     crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// PositioningMeasurementResponse ::= SEQUENCE
using positioning_meas_resp_s = elementary_procedure_option<positioning_meas_resp_ies_container>;

// PositioningMeasurementUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct positioning_meas_upd_ies_container {
  bool                             srs_cfg_present                          = false;
  bool                             trp_meas_upd_list_present                = false;
  bool                             meas_characteristics_request_ind_present = false;
  bool                             meas_time_occasion_present               = false;
  uint16_t                         transaction_id;
  uint32_t                         lmf_meas_id;
  uint32_t                         ran_meas_id;
  srs_configuration_s              srs_cfg;
  trp_meas_upd_list_l              trp_meas_upd_list;
  fixed_bitstring<16, false, true> meas_characteristics_request_ind;
  meas_time_occasion_e             meas_time_occasion;

  static const asn1::table::ie_container_desc desc;
};

// PositioningMeasurementUpdate ::= SEQUENCE
using positioning_meas_upd_s = elementary_procedure_option<positioning_meas_upd_ies_container>;

struct f1ap_private_ies_empty_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    types                                 type() const { return types::nulltype; }
    static const asn1::table::choice_desc desc;
  };
};
// PrivateMessage-IEs ::= OBJECT SET OF F1AP-PRIVATE-IES
using private_msg_ies_o = f1ap_private_ies_empty_o;

using private_msg_ies_container = private_ie_container_empty_l;

// PrivateMessage ::= SEQUENCE
struct private_msg_s {
  private_msg_ies_container private_ies;
  // ...

  static const asn1::table::seq_desc desc;
};

// QoEInformationTransfer-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct qo_e_info_transfer_ies_container {
  bool        qo_e_info_present = false;
  uint64_t    gnb_cu_ue_f1ap_id;
  uint64_t    gnb_du_ue_f1ap_id;
  qo_e_info_s qo_e_info;

  static const asn1::table::ie_container_desc desc;
};

// QoEInformationTransfer ::= SEQUENCE
using qo_e_info_transfer_s = elementary_procedure_option<qo_e_info_transfer_ies_container>;

// QoEInformationTransferControl-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct qo_e_info_transfer_ctrl_ies_container {
  bool               deactivation_ind_present = false;
  uint16_t           transaction_id;
  deactivation_ind_c deactivation_ind;

  static const asn1::table::ie_container_desc desc;
};

// QoEInformationTransferControl ::= SEQUENCE
using qo_e_info_transfer_ctrl_s = elementary_procedure_option<qo_e_info_transfer_ctrl_ies_container>;

// RRCDeliveryReportIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct rrc_delivery_report_ies_container {
  uint64_t              gnb_cu_ue_f1ap_id;
  uint64_t              gnb_du_ue_f1ap_id;
  rrc_delivery_status_s rrc_delivery_status;
  uint8_t               srb_id;

  static const asn1::table::ie_container_desc desc;
};

// RRCDeliveryReport ::= SEQUENCE
using rrc_delivery_report_s = elementary_procedure_option<rrc_delivery_report_ies_container>;

// RachIndication-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct rach_ind_ies_container {
  uint16_t             transaction_id;
  ra_report_ind_list_l ra_report_ind_list;

  static const asn1::table::ie_container_desc desc;
};

// RachIndication ::= SEQUENCE
using rach_ind_s = elementary_procedure_option<rach_ind_ies_container>;

// ReferenceTimeInformationReportIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ref_time_info_report_ies_container {
  uint16_t        transaction_id;
  time_ref_info_s time_ref_info;

  static const asn1::table::ie_container_desc desc;
};

// ReferenceTimeInformationReport ::= SEQUENCE
using ref_time_info_report_s = elementary_procedure_option<ref_time_info_report_ies_container>;

// ReferenceTimeInformationReportingControlIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ref_time_info_report_ctrl_ies_container {
  uint16_t              transaction_id;
  report_request_type_s report_request_type;

  static const asn1::table::ie_container_desc desc;
};

// ReferenceTimeInformationReportingControl ::= SEQUENCE
using ref_time_info_report_ctrl_s = elementary_procedure_option<ref_time_info_report_ctrl_ies_container>;

// ResetAll ::= ENUMERATED
struct reset_all_opts {
  enum options { reset_all, /*...*/ nulltype } value;

  static const asn1::table::enum_desc desc;
};
using reset_all_e = enumerated<reset_all_opts, true>;

// UE-associatedLogicalF1-ConnectionItemRes ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_associated_lc_f1_conn_item_res_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_f1_conn_item, nulltype } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts>;

    types                                  type() const { return types::ue_associated_lc_f1_conn_item; }
    static const asn1::table::choice_desc  desc;
    ue_associated_lc_f1_conn_item_s&       ue_associated_lc_f1_conn_item() { return c; }
    const ue_associated_lc_f1_conn_item_s& ue_associated_lc_f1_conn_item() const { return c; }

  private:
    ue_associated_lc_f1_conn_item_s c;
  };

  static const asn1::table::objset_desc desc;
};

// UE-associatedLogicalF1-ConnectionListRes ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-Field{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ue_associated_lc_f1_conn_list_res_l =
    dyn_array<protocol_ie_single_container_s<ue_associated_lc_f1_conn_item_res_o>>;

// ResetType-ExtIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
using reset_type_ext_ies_o = protocol_ies_empty_o;

// ResetType ::= CHOICE
struct reset_type_c {
  struct types_opts {
    enum options { f1_interface, part_of_f1_interface, choice_ext, nulltype } value;

    static const asn1::table::enum_desc desc;
  };
  using types = enumerated<types_opts>;

  static const asn1::table::choice_storage_desc storage_desc;
  void                                          set(types::options e = types::nulltype) { c.set(e); }
  types                                         type() const { return c.type; }
  static const asn1::table::choice_desc         desc;
  reset_all_e&                                  f1_interface()
  {
    assert_choice_type(types::f1_interface, c.type, "ResetType");
    return c.get<reset_all_e>();
  }
  ue_associated_lc_f1_conn_list_res_l& part_of_f1_interface()
  {
    assert_choice_type(types::part_of_f1_interface, c.type, "ResetType");
    return c.get<ue_associated_lc_f1_conn_list_res_l>();
  }
  protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_ext()
  {
    assert_choice_type(types::choice_ext, c.type, "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
  }
  const reset_all_e& f1_interface() const
  {
    assert_choice_type(types::f1_interface, c.type, "ResetType");
    return c.get<reset_all_e>();
  }
  const ue_associated_lc_f1_conn_list_res_l& part_of_f1_interface() const
  {
    assert_choice_type(types::part_of_f1_interface, c.type, "ResetType");
    return c.get<ue_associated_lc_f1_conn_list_res_l>();
  }
  const protocol_ie_single_container_s<reset_type_ext_ies_o>& choice_ext() const
  {
    assert_choice_type(types::choice_ext, c.type, "ResetType");
    return c.get<protocol_ie_single_container_s<reset_type_ext_ies_o>>();
  }
  reset_all_e&                                          set_f1_interface();
  ue_associated_lc_f1_conn_list_res_l&                  set_part_of_f1_interface();
  protocol_ie_single_container_s<reset_type_ext_ies_o>& set_choice_ext();

private:
  asn1::table::choice_storage<reset_type_c,
                              types,
                              choice_buffer_t<protocol_ie_single_container_s<reset_type_ext_ies_o>,
                                              reset_all_e,
                                              ue_associated_lc_f1_conn_list_res_l>>
      c;
};

// ResetIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct reset_ies_container {
  uint16_t     transaction_id;
  cause_c      cause;
  reset_type_c reset_type;

  static const asn1::table::ie_container_desc desc;
};

// Reset ::= SEQUENCE
using reset_s = elementary_procedure_option<reset_ies_container>;

// UE-associatedLogicalF1-ConnectionItemResAck ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct ue_associated_lc_f1_conn_item_res_ack_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { ue_associated_lc_f1_conn_item, nulltype } value;

      static const asn1::table::enum_desc desc;
      uint8_t                             to_number() const;
    };
    using types = enumerated<types_opts>;

    types                                  type() const { return types::ue_associated_lc_f1_conn_item; }
    static const asn1::table::choice_desc  desc;
    ue_associated_lc_f1_conn_item_s&       ue_associated_lc_f1_conn_item() { return c; }
    const ue_associated_lc_f1_conn_item_s& ue_associated_lc_f1_conn_item() const { return c; }

  private:
    ue_associated_lc_f1_conn_item_s c;
  };

  static const asn1::table::objset_desc desc;
};

// UE-associatedLogicalF1-ConnectionListResAck ::= SEQUENCE (SIZE (1..65536)) OF ProtocolIE-Field{F1AP-PROTOCOL-IES :
// IEsSetParam}
using ue_associated_lc_f1_conn_list_res_ack_l =
    dyn_array<protocol_ie_single_container_s<ue_associated_lc_f1_conn_item_res_ack_o>>;

// ResetAcknowledgeIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct reset_ack_ies_container {
  bool                                    ue_associated_lc_f1_conn_list_res_ack_present = false;
  bool                                    crit_diagnostics_present                      = false;
  uint16_t                                transaction_id;
  ue_associated_lc_f1_conn_list_res_ack_l ue_associated_lc_f1_conn_list_res_ack;
  crit_diagnostics_s                      crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// ResetAcknowledge ::= SEQUENCE
using reset_ack_s = elementary_procedure_option<reset_ack_ies_container>;

// ResourceStatusFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct res_status_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  uint16_t           gnb_cu_meas_id;
  uint16_t           gnb_du_meas_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// ResourceStatusFailure ::= SEQUENCE
using res_status_fail_s = elementary_procedure_option<res_status_fail_ies_container>;

// ResourceStatusRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct res_status_request_ies_container {
  bool                             gnb_du_meas_id_present         = false;
  bool                             report_characteristics_present = false;
  bool                             cell_to_report_list_present    = false;
  bool                             report_periodicity_present     = false;
  uint16_t                         transaction_id;
  uint16_t                         gnb_cu_meas_id;
  uint16_t                         gnb_du_meas_id;
  regist_request_e                 regist_request;
  fixed_bitstring<32, false, true> report_characteristics;
  cell_to_report_list_l            cell_to_report_list;
  report_periodicity_e             report_periodicity;

  static const asn1::table::ie_container_desc desc;
};

// ResourceStatusRequest ::= SEQUENCE
using res_status_request_s = elementary_procedure_option<res_status_request_ies_container>;

// ResourceStatusResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct res_status_resp_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  uint16_t           gnb_cu_meas_id;
  uint16_t           gnb_du_meas_id;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// ResourceStatusResponse ::= SEQUENCE
using res_status_resp_s = elementary_procedure_option<res_status_resp_ies_container>;

// ResourceStatusUpdateIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct res_status_upd_ies_container {
  bool                    hardware_load_ind_present     = false;
  bool                    tnl_capacity_ind_present      = false;
  bool                    cell_meas_result_list_present = false;
  uint16_t                transaction_id;
  uint16_t                gnb_cu_meas_id;
  uint16_t                gnb_du_meas_id;
  hardware_load_ind_s     hardware_load_ind;
  tnl_capacity_ind_s      tnl_capacity_ind;
  cell_meas_result_list_l cell_meas_result_list;

  static const asn1::table::ie_container_desc desc;
};

// ResourceStatusUpdate ::= SEQUENCE
using res_status_upd_s = elementary_procedure_option<res_status_upd_ies_container>;

// SRSInformationReservationNotificationIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct srs_info_reserv_notif_ies_container {
  bool                                        srs_info_present        = false;
  bool                                        precfg_srs_info_present = false;
  uint16_t                                    transaction_id;
  srs_reserv_type_e                           srs_reserv_type;
  requested_srs_tx_characteristics_s          srs_info;
  requested_srs_precfg_characteristics_list_l precfg_srs_info;

  static const asn1::table::ie_container_desc desc;
};

// SRSInformationReservationNotification ::= SEQUENCE
using srs_info_reserv_notif_s = elementary_procedure_option<srs_info_reserv_notif_ies_container>;

// SystemInformationDeliveryCommandIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct sys_info_delivery_cmd_ies_container {
  uint16_t      transaction_id;
  nr_cgi_s      nr_cgi;
  sitype_list_l sitype_list;
  uint64_t      confirmed_ue_id;

  static const asn1::table::ie_container_desc desc;
};

// SystemInformationDeliveryCommand ::= SEQUENCE
using sys_info_delivery_cmd_s = elementary_procedure_option<sys_info_delivery_cmd_ies_container>;

// TRPInformationFailureIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trp_info_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// TRPInformationFailure ::= SEQUENCE
using trp_info_fail_s = elementary_procedure_option<trp_info_fail_ies_container>;

// TRPInformationItemTRPResp ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trp_info_item_trp_resp_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { trp_info_item, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    types                                 type() const { return types::trp_info_item; }
    static const asn1::table::choice_desc desc;
    trp_info_item_s&                      trp_info_item() { return c; }
    const trp_info_item_s&                trp_info_item() const { return c; }

  private:
    trp_info_item_s c;
  };

  static const asn1::table::objset_desc desc;
};

// TRPInformationListTRPResp ::= SEQUENCE (SIZE (1..65535)) OF ProtocolIE-Field{F1AP-PROTOCOL-IES : IEsSetParam}
using trp_info_list_trp_resp_l = dyn_array<protocol_ie_single_container_s<trp_info_item_trp_resp_o>>;

// TRPInformationTypeItemTRPReq ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trp_info_type_item_trp_req_o {
  // Value ::= OPEN TYPE
  struct value_c {
    struct types_opts {
      enum options { trp_info_type_item, nulltype } value;

      static const asn1::table::enum_desc desc;
    };
    using types = enumerated<types_opts>;

    types                                 type() const { return types::trp_info_type_item; }
    static const asn1::table::choice_desc desc;
    trp_info_type_item_e&                 trp_info_type_item() { return c; }
    const trp_info_type_item_e&           trp_info_type_item() const { return c; }

  private:
    trp_info_type_item_e c;
  };

  static const asn1::table::objset_desc desc;
};

// TRPInformationTypeListTRPReq ::= SEQUENCE (SIZE (1..64)) OF ProtocolIE-Field{F1AP-PROTOCOL-IES : IEsSetParam}
using trp_info_type_list_trp_req_l = dyn_array<protocol_ie_single_container_s<trp_info_type_item_trp_req_o>>;

// TRPInformationRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trp_info_request_ies_container {
  bool                         trp_list_present = false;
  uint16_t                     transaction_id;
  trp_list_l                   trp_list;
  trp_info_type_list_trp_req_l trp_info_type_list_trp_req;

  static const asn1::table::ie_container_desc desc;
};

// TRPInformationRequest ::= SEQUENCE
using trp_info_request_s = elementary_procedure_option<trp_info_request_ies_container>;

// TRPInformationResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trp_info_resp_ies_container {
  bool                     crit_diagnostics_present = false;
  uint16_t                 transaction_id;
  trp_info_list_trp_resp_l trp_info_list_trp_resp;
  crit_diagnostics_s       crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// TRPInformationResponse ::= SEQUENCE
using trp_info_resp_s = elementary_procedure_option<trp_info_resp_ies_container>;

// TimingSynchronisationStatusFailure-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct timing_synchronisation_status_fail_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  cause_c            cause;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// TimingSynchronisationStatusFailure ::= SEQUENCE
using timing_synchronisation_status_fail_s =
    elementary_procedure_option<timing_synchronisation_status_fail_ies_container>;

// TimingSynchronisationStatusReport-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct timing_synchronisation_status_report_ies_container {
  uint16_t                                 transaction_id;
  ran_timing_synchronisation_status_info_s ran_timing_synchronisation_status_info;

  static const asn1::table::ie_container_desc desc;
};

// TimingSynchronisationStatusReport ::= SEQUENCE
using timing_synchronisation_status_report_s =
    elementary_procedure_option<timing_synchronisation_status_report_ies_container>;

// TimingSynchronisationStatusRequest-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct timing_synchronisation_status_request_ies_container {
  uint16_t              transaction_id;
  rantss_request_type_e rantss_request_type;

  static const asn1::table::ie_container_desc desc;
};

// TimingSynchronisationStatusRequest ::= SEQUENCE
using timing_synchronisation_status_request_s =
    elementary_procedure_option<timing_synchronisation_status_request_ies_container>;

// TimingSynchronisationStatusResponse-IEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct timing_synchronisation_status_resp_ies_container {
  bool               crit_diagnostics_present = false;
  uint16_t           transaction_id;
  crit_diagnostics_s crit_diagnostics;

  static const asn1::table::ie_container_desc desc;
};

// TimingSynchronisationStatusResponse ::= SEQUENCE
using timing_synchronisation_status_resp_s =
    elementary_procedure_option<timing_synchronisation_status_resp_ies_container>;

// TraceStartIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct trace_start_ies_container {
  uint64_t           gnb_cu_ue_f1ap_id;
  uint64_t           gnb_du_ue_f1ap_id;
  trace_activation_s trace_activation;

  static const asn1::table::ie_container_desc desc;
};

// TraceStart ::= SEQUENCE
using trace_start_s = elementary_procedure_option<trace_start_ies_container>;

// WriteReplaceWarningRequestIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct write_replace_warning_request_ies_container {
  bool                         cells_to_be_broadcast_list_present = false;
  uint16_t                     transaction_id;
  pws_sys_info_s               pws_sys_info;
  uint32_t                     repeat_period;
  uint32_t                     numof_broadcast_request;
  cells_to_be_broadcast_list_l cells_to_be_broadcast_list;

  static const asn1::table::ie_container_desc desc;
};

// WriteReplaceWarningRequest ::= SEQUENCE
using write_replace_warning_request_s = elementary_procedure_option<write_replace_warning_request_ies_container>;

// WriteReplaceWarningResponseIEs ::= OBJECT SET OF F1AP-PROTOCOL-IES
struct write_replace_warning_resp_ies_container {
  bool                             cells_broadcast_completed_list_present = false;
  bool                             crit_diagnostics_present               = false;
  bool                             ded_si_delivery_needed_ue_list_present = false;
  uint16_t                         transaction_id;
  cells_broadcast_completed_list_l cells_broadcast_completed_list;
  crit_diagnostics_s               crit_diagnostics;
  ded_si_delivery_needed_ue_list_l ded_si_delivery_needed_ue_list;

  static const asn1::table::ie_container_desc desc;
};

// WriteReplaceWarningResponse ::= SEQUENCE
using write_replace_warning_resp_s = elementary_procedure_option<write_replace_warning_resp_ies_container>;

} // namespace f1ap
} // namespace asn1

extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::f1ap::mc_paging_cell_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops asn1::dyn_array<asn1::f1ap::paging_cell_list_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::f1ap::ue_associated_lc_f1_conn_list_res_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::f1ap::ue_associated_lc_f1_conn_list_res_ack_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::f1ap::trp_info_list_trp_resp_l::value_type>::ops;
extern template const asn1::detail::dyn_array_ops
    asn1::dyn_array<asn1::f1ap::trp_info_type_list_trp_req_l::value_type>::ops;
