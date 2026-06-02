// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../cell_meas_manager/cell_meas_manager_impl.h"
#include "../../du_processor/du_processor.h"
#include "../../ue_manager/ue_manager_impl.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp.h"
#include "ocudu/ngap/ngap_handover.h"
#include "ocudu/support/async/async_task.h"

namespace ocudu::ocucp {

struct cu_cp_inter_cu_handover_request {
  // NG handover specific fields.
  ngap_handov_type                                           handov_type;
  std::optional<ngap_core_network_assist_info_for_inactive>  core_network_assist_info_for_inactive;
  std::optional<bool>                                        new_security_context_ind;
  byte_buffer                                                nasc;
  std::vector<s_nssai_t>                                     allowed_nssai;
  std::optional<uint64_t>                                    masked_imeisv;
  std::optional<ngap_rrc_inactive_transition_report_request> rrc_inactive_transition_report_request;
  std::vector<ngap_pdu_session_res_info_item>                pdu_session_res_info_list;
  std::vector<ngap_erab_info_item>                           erab_info_list;
  std::optional<uint16_t>                                    idx_to_rfsp;
  std::vector<ngap_last_visited_cell_item>                   ue_history_info;
  // TODO: Add missing optional fields.

  // XNAP handover specific fields.
  std::optional<unsigned>                amf_ue_id; // < Mandatory for XNAP handover.
  std::optional<transport_layer_address> amf_addr;  // < Mandatory for XNAP handover.
  // TODO: Add optional fields.

  // Common fields.
  cu_cp_ue_index_t                                                      ue_index;
  std::variant<ngap_cause_t, xnap_cause_t>                              cause;
  nr_cell_global_id_t                                                   target_cell_id;
  security::security_context                                            security_context;
  guami_t                                                               guami;
  aggregate_maximum_bit_rate_t                                          ue_ambr;
  slotted_id_vector<pdu_session_id_t, cu_cp_pdu_session_res_setup_item> pdu_session_res_setup_list;
  byte_buffer                                                           rrc_handover_preparation_information;
  std::optional<location_report_request>                                location_report_request_type;

  void from_ngap_handover_request(const ngap_handover_request& ng_handover_request)
  {
    // Fill common fields.
    ue_index         = ng_handover_request.ue_index;
    cause            = ng_handover_request.cause;
    target_cell_id   = ng_handover_request.source_to_target_transparent_container.target_cell_id;
    security_context = ng_handover_request.security_context;
    guami            = ng_handover_request.guami;
    ue_ambr          = ng_handover_request.ue_aggr_max_bit_rate;
    for (const auto& pdu_session_res_info : ng_handover_request.pdu_session_res_setup_list_ho_req) {
      pdu_session_res_setup_list.emplace(pdu_session_res_info.pdu_session_id, pdu_session_res_info);
    }
    rrc_handover_preparation_information =
        ng_handover_request.source_to_target_transparent_container.rrc_container.copy();
    location_report_request_type = ng_handover_request.location_report_request_type;

    // Fill NG handover specific fields.
    handov_type                            = ng_handover_request.handov_type;
    core_network_assist_info_for_inactive  = ng_handover_request.core_network_assist_info_for_inactive;
    new_security_context_ind               = ng_handover_request.new_security_context_ind;
    nasc                                   = ng_handover_request.nasc.copy();
    allowed_nssai                          = ng_handover_request.allowed_nssai;
    masked_imeisv                          = ng_handover_request.masked_imeisv;
    rrc_inactive_transition_report_request = ng_handover_request.rrc_inactive_transition_report_request;
    pdu_session_res_info_list = ng_handover_request.source_to_target_transparent_container.pdu_session_res_info_list;
    erab_info_list            = ng_handover_request.source_to_target_transparent_container.erab_info_list;
    idx_to_rfsp               = ng_handover_request.source_to_target_transparent_container.idx_to_rfsp;
    ue_history_info           = ng_handover_request.source_to_target_transparent_container.ue_history_info;
  }

  void from_xnap_handover_request(const xnap_handover_request& xnap_request)
  {
    // Fill common fields.
    ue_index         = xnap_request.ue_index;
    cause            = xnap_request.cause;
    target_cell_id   = xnap_request.nr_cgi;
    security_context = xnap_request.ue_context_info_ho_request.security_context;
    guami            = xnap_request.guami;
    ue_ambr          = xnap_request.ue_context_info_ho_request.ue_ambr;
    for (const auto& pdu_session_res_info : xnap_request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list) {
      pdu_session_res_setup_list.emplace(pdu_session_res_info.pdu_session_id, pdu_session_res_info);
    }
    rrc_handover_preparation_information =
        xnap_request.ue_context_info_ho_request.rrc_handover_preparation_information.copy();
    location_report_request_type = xnap_request.ue_context_info_ho_request.location_report_info;

    // Fill XNAP handover specific fields.
    amf_ue_id = xnap_request.ue_context_info_ho_request.amf_ue_id;
    amf_addr  = xnap_request.ue_context_info_ho_request.amf_addr;
  }
};

class inter_cu_handover_target_routine
{
public:
  inter_cu_handover_target_routine(const cu_cp_inter_cu_handover_request& request_,
                                   e1ap_bearer_context_manager&           e1ap_bearer_ctxt_mng_,
                                   du_processor&                          du_proc_,
                                   cu_cp_ue_removal_handler&              ue_removal_handler_,
                                   ue_manager&                            ue_mng_,
                                   cell_meas_manager&                     cell_meas_mng_,
                                   const security_indication_t&           default_security_indication_,
                                   ocudulog::basic_logger&                logger_);

  void operator()(coro_context<async_task<cu_cp_handover_resource_allocation_response>>& ctx);

  static const char* name() { return "Inter CU Handover Target Routine"; }

private:
  bool fill_e1ap_bearer_context_setup_request(const security::sec_as_config& sec_info);
  void create_srb(srb_id_t srb_id);
  cu_cp_handover_resource_allocation_response generate_handover_resource_allocation_response(bool success);
  bool                                        is_xn_handover() const { return request.amf_ue_id.has_value(); }

  const cu_cp_inter_cu_handover_request request;

  e1ap_bearer_context_manager& e1ap_bearer_ctxt_mng; // to trigger bearer context modification at CU-UP.
  du_processor&                du_proc; // to trigger UE context creation and access RRC-DU for measConfig packing.
  cu_cp_ue_removal_handler&    ue_removal_handler; // to trigger UE removal if the UE Context Setup fails.
  ue_manager&                  ue_mng;
  cell_meas_manager&           cell_meas_mng;

  ocudulog::basic_logger& logger;

  cu_cp_ue*                    ue = nullptr;
  rrc_ue_transfer_context      rrc_context; // Passed to new RRC UE upon creation.
  up_config_update             next_config;
  const security_indication_t& default_security_indication; // default if not signaled via NGAP.

  // (sub-)routine requests.
  e1ap_bearer_context_setup_request        bearer_context_setup_request;
  f1ap_ue_context_setup_request            ue_context_setup_request;
  e1ap_bearer_context_modification_request bearer_context_modification_request;
  rrc_reconfiguration_procedure_request    rrc_reconfig_args;

  // (sub-)routine results.
  std::optional<rrc_meas_cfg>               meas_cfg;
  e1ap_bearer_context_setup_response        bearer_context_setup_response;
  f1ap_ue_context_setup_response            ue_context_setup_response;
  e1ap_bearer_context_modification_response bearer_context_modification_response;
  byte_buffer                               handover_command_pdu;

  std::optional<cell_meas_config> cell_cfg;
};

} // namespace ocudu::ocucp
