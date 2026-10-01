// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/*******************************************************************************
 *
 *                     3GPP TS ASN1 E1AP v18.5.0 (2025-06)
 *
 ******************************************************************************/

#pragma once

#include "ocudu/asn1/asn1_ap_utils.h"

namespace asn1 {
namespace e1ap {

struct reset_ies_container;
using reset_s = elementary_procedure_option<reset_ies_container>;
struct gnb_cu_up_e1_setup_request_ies_container;
using gnb_cu_up_e1_setup_request_s = elementary_procedure_option<gnb_cu_up_e1_setup_request_ies_container>;
struct gnb_cu_cp_e1_setup_request_ies_container;
using gnb_cu_cp_e1_setup_request_s = elementary_procedure_option<gnb_cu_cp_e1_setup_request_ies_container>;
struct gnb_cu_up_cfg_upd_ies_container;
using gnb_cu_up_cfg_upd_s = elementary_procedure_option<gnb_cu_up_cfg_upd_ies_container>;
struct gnb_cu_cp_cfg_upd_ies_container;
using gnb_cu_cp_cfg_upd_s = elementary_procedure_option<gnb_cu_cp_cfg_upd_ies_container>;
struct e1_release_request_ies_container;
using e1_release_request_s = elementary_procedure_option<e1_release_request_ies_container>;
struct bearer_context_setup_request_ies_container;
using bearer_context_setup_request_s = elementary_procedure_option<bearer_context_setup_request_ies_container>;
struct bearer_context_mod_request_ies_container;
using bearer_context_mod_request_s = elementary_procedure_option<bearer_context_mod_request_ies_container>;
struct bearer_context_mod_required_ies_container;
using bearer_context_mod_required_s = elementary_procedure_option<bearer_context_mod_required_ies_container>;
struct bearer_context_release_cmd_ies_container;
using bearer_context_release_cmd_s = elementary_procedure_option<bearer_context_release_cmd_ies_container>;
struct res_status_request_ies_container;
using res_status_request_s = elementary_procedure_option<res_status_request_ies_container>;
struct iab_up_tnl_address_upd_ies_container;
using iab_up_tnl_address_upd_s = elementary_procedure_option<iab_up_tnl_address_upd_ies_container>;
struct bc_bearer_context_setup_request_ies_container;
using bc_bearer_context_setup_request_s = elementary_procedure_option<bc_bearer_context_setup_request_ies_container>;
struct bc_bearer_context_mod_request_ies_container;
using bc_bearer_context_mod_request_s = elementary_procedure_option<bc_bearer_context_mod_request_ies_container>;
struct bc_bearer_context_mod_required_ies_container;
using bc_bearer_context_mod_required_s = elementary_procedure_option<bc_bearer_context_mod_required_ies_container>;
struct bc_bearer_context_release_cmd_ies_container;
using bc_bearer_context_release_cmd_s = elementary_procedure_option<bc_bearer_context_release_cmd_ies_container>;
struct mc_bearer_context_setup_request_ies_container;
using mc_bearer_context_setup_request_s = elementary_procedure_option<mc_bearer_context_setup_request_ies_container>;
struct mc_bearer_context_mod_request_ies_container;
using mc_bearer_context_mod_request_s = elementary_procedure_option<mc_bearer_context_mod_request_ies_container>;
struct mc_bearer_context_mod_required_ies_container;
using mc_bearer_context_mod_required_s = elementary_procedure_option<mc_bearer_context_mod_required_ies_container>;
struct mc_bearer_context_release_cmd_ies_container;
using mc_bearer_context_release_cmd_s = elementary_procedure_option<mc_bearer_context_release_cmd_ies_container>;
struct error_ind_ies_container;
using error_ind_s = elementary_procedure_option<error_ind_ies_container>;
struct bearer_context_release_request_ies_container;
using bearer_context_release_request_s = elementary_procedure_option<bearer_context_release_request_ies_container>;
struct bearer_context_inactivity_notif_ies_container;
using bearer_context_inactivity_notif_s = elementary_procedure_option<bearer_context_inactivity_notif_ies_container>;
struct dl_data_notif_ies_container;
using dl_data_notif_s = elementary_procedure_option<dl_data_notif_ies_container>;
struct ul_data_notif_ies_container;
using ul_data_notif_s = elementary_procedure_option<ul_data_notif_ies_container>;
struct data_usage_report_ies_container;
using data_usage_report_s = elementary_procedure_option<data_usage_report_ies_container>;
struct gnb_cu_up_counter_check_request_ies_container;
using gnb_cu_up_counter_check_request_s = elementary_procedure_option<gnb_cu_up_counter_check_request_ies_container>;
struct gnb_cu_up_status_ind_ies_container;
using gnb_cu_up_status_ind_s = elementary_procedure_option<gnb_cu_up_status_ind_ies_container>;
struct mrdc_data_usage_report_ies_container;
using mrdc_data_usage_report_s = elementary_procedure_option<mrdc_data_usage_report_ies_container>;
struct deactiv_trace_ies_container;
using deactiv_trace_s = elementary_procedure_option<deactiv_trace_ies_container>;
struct trace_start_ies_container;
using trace_start_s = elementary_procedure_option<trace_start_ies_container>;
struct private_msg_s;
struct cell_traffic_trace_ies_container;
using cell_traffic_trace_s = elementary_procedure_option<cell_traffic_trace_ies_container>;
struct res_status_upd_ies_container;
using res_status_upd_s = elementary_procedure_option<res_status_upd_ies_container>;
struct early_forwarding_sn_transfer_ies_container;
using early_forwarding_sn_transfer_s = elementary_procedure_option<early_forwarding_sn_transfer_ies_container>;
struct gnb_cu_cp_meas_results_info_ies_container;
using gnb_cu_cp_meas_results_info_s = elementary_procedure_option<gnb_cu_cp_meas_results_info_ies_container>;
struct iabpsk_notif_ies_container;
using iabpsk_notif_s = elementary_procedure_option<iabpsk_notif_ies_container>;
struct bc_bearer_context_release_request_ies_container;
using bc_bearer_context_release_request_s =
    elementary_procedure_option<bc_bearer_context_release_request_ies_container>;
struct mc_bearer_context_release_request_ies_container;
using mc_bearer_context_release_request_s =
    elementary_procedure_option<mc_bearer_context_release_request_ies_container>;
struct mc_bearer_notif_ies_container;
using mc_bearer_notif_s = elementary_procedure_option<mc_bearer_notif_ies_container>;
struct reset_ack_ies_container;
using reset_ack_s = elementary_procedure_option<reset_ack_ies_container>;
struct gnb_cu_up_e1_setup_resp_ies_container;
using gnb_cu_up_e1_setup_resp_s = elementary_procedure_option<gnb_cu_up_e1_setup_resp_ies_container>;
struct gnb_cu_cp_e1_setup_resp_ies_container;
using gnb_cu_cp_e1_setup_resp_s = elementary_procedure_option<gnb_cu_cp_e1_setup_resp_ies_container>;
struct gnb_cu_up_cfg_upd_ack_ies_container;
using gnb_cu_up_cfg_upd_ack_s = elementary_procedure_option<gnb_cu_up_cfg_upd_ack_ies_container>;
struct gnb_cu_cp_cfg_upd_ack_ies_container;
using gnb_cu_cp_cfg_upd_ack_s = elementary_procedure_option<gnb_cu_cp_cfg_upd_ack_ies_container>;
struct e1_release_resp_ies_container;
using e1_release_resp_s = elementary_procedure_option<e1_release_resp_ies_container>;
struct bearer_context_setup_resp_ies_container;
using bearer_context_setup_resp_s = elementary_procedure_option<bearer_context_setup_resp_ies_container>;
struct bearer_context_mod_resp_ies_container;
using bearer_context_mod_resp_s = elementary_procedure_option<bearer_context_mod_resp_ies_container>;
struct bearer_context_mod_confirm_ies_container;
using bearer_context_mod_confirm_s = elementary_procedure_option<bearer_context_mod_confirm_ies_container>;
struct bearer_context_release_complete_ies_container;
using bearer_context_release_complete_s = elementary_procedure_option<bearer_context_release_complete_ies_container>;
struct res_status_resp_ies_container;
using res_status_resp_s = elementary_procedure_option<res_status_resp_ies_container>;
struct iab_up_tnl_address_upd_ack_ies_container;
using iab_up_tnl_address_upd_ack_s = elementary_procedure_option<iab_up_tnl_address_upd_ack_ies_container>;
struct bc_bearer_context_setup_resp_ies_container;
using bc_bearer_context_setup_resp_s = elementary_procedure_option<bc_bearer_context_setup_resp_ies_container>;
struct bc_bearer_context_mod_resp_ies_container;
using bc_bearer_context_mod_resp_s = elementary_procedure_option<bc_bearer_context_mod_resp_ies_container>;
struct bc_bearer_context_mod_confirm_ies_container;
using bc_bearer_context_mod_confirm_s = elementary_procedure_option<bc_bearer_context_mod_confirm_ies_container>;
struct bc_bearer_context_release_complete_ies_container;
using bc_bearer_context_release_complete_s =
    elementary_procedure_option<bc_bearer_context_release_complete_ies_container>;
struct mc_bearer_context_setup_resp_ies_container;
using mc_bearer_context_setup_resp_s = elementary_procedure_option<mc_bearer_context_setup_resp_ies_container>;
struct mc_bearer_context_mod_resp_ies_container;
using mc_bearer_context_mod_resp_s = elementary_procedure_option<mc_bearer_context_mod_resp_ies_container>;
struct mc_bearer_context_mod_confirm_ies_container;
using mc_bearer_context_mod_confirm_s = elementary_procedure_option<mc_bearer_context_mod_confirm_ies_container>;
struct mc_bearer_context_release_complete_ies_container;
using mc_bearer_context_release_complete_s =
    elementary_procedure_option<mc_bearer_context_release_complete_ies_container>;
struct gnb_cu_up_e1_setup_fail_ies_container;
using gnb_cu_up_e1_setup_fail_s = elementary_procedure_option<gnb_cu_up_e1_setup_fail_ies_container>;
struct gnb_cu_cp_e1_setup_fail_ies_container;
using gnb_cu_cp_e1_setup_fail_s = elementary_procedure_option<gnb_cu_cp_e1_setup_fail_ies_container>;
struct gnb_cu_up_cfg_upd_fail_ies_container;
using gnb_cu_up_cfg_upd_fail_s = elementary_procedure_option<gnb_cu_up_cfg_upd_fail_ies_container>;
struct gnb_cu_cp_cfg_upd_fail_ies_container;
using gnb_cu_cp_cfg_upd_fail_s = elementary_procedure_option<gnb_cu_cp_cfg_upd_fail_ies_container>;
struct bearer_context_setup_fail_ies_container;
using bearer_context_setup_fail_s = elementary_procedure_option<bearer_context_setup_fail_ies_container>;
struct bearer_context_mod_fail_ies_container;
using bearer_context_mod_fail_s = elementary_procedure_option<bearer_context_mod_fail_ies_container>;
struct res_status_fail_ies_container;
using res_status_fail_s = elementary_procedure_option<res_status_fail_ies_container>;
struct iab_up_tnl_address_upd_fail_ies_container;
using iab_up_tnl_address_upd_fail_s = elementary_procedure_option<iab_up_tnl_address_upd_fail_ies_container>;
struct bc_bearer_context_setup_fail_ies_container;
using bc_bearer_context_setup_fail_s = elementary_procedure_option<bc_bearer_context_setup_fail_ies_container>;
struct bc_bearer_context_mod_fail_ies_container;
using bc_bearer_context_mod_fail_s = elementary_procedure_option<bc_bearer_context_mod_fail_ies_container>;
struct mc_bearer_context_setup_fail_ies_container;
using mc_bearer_context_setup_fail_s = elementary_procedure_option<mc_bearer_context_setup_fail_ies_container>;
struct mc_bearer_context_mod_fail_ies_container;
using mc_bearer_context_mod_fail_s = elementary_procedure_option<mc_bearer_context_mod_fail_ies_container>;

/*******************************************************************************
 *                              Struct Definitions
 ******************************************************************************/

// E1AP-ELEMENTARY-PROCEDURES ::= OBJECT SET OF E1AP-ELEMENTARY-PROCEDURE
struct e1ap_elem_procs_o {
  // InitiatingMessage ::= OPEN TYPE
  struct init_msg_c {
    struct types_opts {
      enum options {
        reset,
        gnb_cu_up_e1_setup_request,
        gnb_cu_cp_e1_setup_request,
        gnb_cu_up_cfg_upd,
        gnb_cu_cp_cfg_upd,
        e1_release_request,
        bearer_context_setup_request,
        bearer_context_mod_request,
        bearer_context_mod_required,
        bearer_context_release_cmd,
        res_status_request,
        iab_up_tnl_address_upd,
        bc_bearer_context_setup_request,
        bc_bearer_context_mod_request,
        bc_bearer_context_mod_required,
        bc_bearer_context_release_cmd,
        mc_bearer_context_setup_request,
        mc_bearer_context_mod_request,
        mc_bearer_context_mod_required,
        mc_bearer_context_release_cmd,
        error_ind,
        bearer_context_release_request,
        bearer_context_inactivity_notif,
        dl_data_notif,
        ul_data_notif,
        data_usage_report,
        gnb_cu_up_counter_check_request,
        gnb_cu_up_status_ind,
        mrdc_data_usage_report,
        deactiv_trace,
        trace_start,
        private_msg,
        cell_traffic_trace,
        res_status_upd,
        early_forwarding_sn_transfer,
        gnb_cu_cp_meas_results_info,
        iabpsk_notif,
        bc_bearer_context_release_request,
        mc_bearer_context_release_request,
        mc_bearer_notif,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    init_msg_c() = default;
    void          set(types::options e = types::nulltype);
    types         type() const { return type_; }
    OCUDUASN_CODE pack(bit_ref& bref) const;
    OCUDUASN_CODE unpack(cbit_ref& bref);
    void          to_json(json_writer& j) const;
    // getters
    reset_s&                                   reset();
    gnb_cu_up_e1_setup_request_s&              gnb_cu_up_e1_setup_request();
    gnb_cu_cp_e1_setup_request_s&              gnb_cu_cp_e1_setup_request();
    gnb_cu_up_cfg_upd_s&                       gnb_cu_up_cfg_upd();
    gnb_cu_cp_cfg_upd_s&                       gnb_cu_cp_cfg_upd();
    e1_release_request_s&                      e1_release_request();
    bearer_context_setup_request_s&            bearer_context_setup_request();
    bearer_context_mod_request_s&              bearer_context_mod_request();
    bearer_context_mod_required_s&             bearer_context_mod_required();
    bearer_context_release_cmd_s&              bearer_context_release_cmd();
    res_status_request_s&                      res_status_request();
    iab_up_tnl_address_upd_s&                  iab_up_tnl_address_upd();
    bc_bearer_context_setup_request_s&         bc_bearer_context_setup_request();
    bc_bearer_context_mod_request_s&           bc_bearer_context_mod_request();
    bc_bearer_context_mod_required_s&          bc_bearer_context_mod_required();
    bc_bearer_context_release_cmd_s&           bc_bearer_context_release_cmd();
    mc_bearer_context_setup_request_s&         mc_bearer_context_setup_request();
    mc_bearer_context_mod_request_s&           mc_bearer_context_mod_request();
    mc_bearer_context_mod_required_s&          mc_bearer_context_mod_required();
    mc_bearer_context_release_cmd_s&           mc_bearer_context_release_cmd();
    error_ind_s&                               error_ind();
    bearer_context_release_request_s&          bearer_context_release_request();
    bearer_context_inactivity_notif_s&         bearer_context_inactivity_notif();
    dl_data_notif_s&                           dl_data_notif();
    ul_data_notif_s&                           ul_data_notif();
    data_usage_report_s&                       data_usage_report();
    gnb_cu_up_counter_check_request_s&         gnb_cu_up_counter_check_request();
    gnb_cu_up_status_ind_s&                    gnb_cu_up_status_ind();
    mrdc_data_usage_report_s&                  mrdc_data_usage_report();
    deactiv_trace_s&                           deactiv_trace();
    trace_start_s&                             trace_start();
    private_msg_s&                             private_msg();
    cell_traffic_trace_s&                      cell_traffic_trace();
    res_status_upd_s&                          res_status_upd();
    early_forwarding_sn_transfer_s&            early_forwarding_sn_transfer();
    gnb_cu_cp_meas_results_info_s&             gnb_cu_cp_meas_results_info();
    iabpsk_notif_s&                            iabpsk_notif();
    bc_bearer_context_release_request_s&       bc_bearer_context_release_request();
    mc_bearer_context_release_request_s&       mc_bearer_context_release_request();
    mc_bearer_notif_s&                         mc_bearer_notif();
    const reset_s&                             reset() const;
    const gnb_cu_up_e1_setup_request_s&        gnb_cu_up_e1_setup_request() const;
    const gnb_cu_cp_e1_setup_request_s&        gnb_cu_cp_e1_setup_request() const;
    const gnb_cu_up_cfg_upd_s&                 gnb_cu_up_cfg_upd() const;
    const gnb_cu_cp_cfg_upd_s&                 gnb_cu_cp_cfg_upd() const;
    const e1_release_request_s&                e1_release_request() const;
    const bearer_context_setup_request_s&      bearer_context_setup_request() const;
    const bearer_context_mod_request_s&        bearer_context_mod_request() const;
    const bearer_context_mod_required_s&       bearer_context_mod_required() const;
    const bearer_context_release_cmd_s&        bearer_context_release_cmd() const;
    const res_status_request_s&                res_status_request() const;
    const iab_up_tnl_address_upd_s&            iab_up_tnl_address_upd() const;
    const bc_bearer_context_setup_request_s&   bc_bearer_context_setup_request() const;
    const bc_bearer_context_mod_request_s&     bc_bearer_context_mod_request() const;
    const bc_bearer_context_mod_required_s&    bc_bearer_context_mod_required() const;
    const bc_bearer_context_release_cmd_s&     bc_bearer_context_release_cmd() const;
    const mc_bearer_context_setup_request_s&   mc_bearer_context_setup_request() const;
    const mc_bearer_context_mod_request_s&     mc_bearer_context_mod_request() const;
    const mc_bearer_context_mod_required_s&    mc_bearer_context_mod_required() const;
    const mc_bearer_context_release_cmd_s&     mc_bearer_context_release_cmd() const;
    const error_ind_s&                         error_ind() const;
    const bearer_context_release_request_s&    bearer_context_release_request() const;
    const bearer_context_inactivity_notif_s&   bearer_context_inactivity_notif() const;
    const dl_data_notif_s&                     dl_data_notif() const;
    const ul_data_notif_s&                     ul_data_notif() const;
    const data_usage_report_s&                 data_usage_report() const;
    const gnb_cu_up_counter_check_request_s&   gnb_cu_up_counter_check_request() const;
    const gnb_cu_up_status_ind_s&              gnb_cu_up_status_ind() const;
    const mrdc_data_usage_report_s&            mrdc_data_usage_report() const;
    const deactiv_trace_s&                     deactiv_trace() const;
    const trace_start_s&                       trace_start() const;
    const private_msg_s&                       private_msg() const;
    const cell_traffic_trace_s&                cell_traffic_trace() const;
    const res_status_upd_s&                    res_status_upd() const;
    const early_forwarding_sn_transfer_s&      early_forwarding_sn_transfer() const;
    const gnb_cu_cp_meas_results_info_s&       gnb_cu_cp_meas_results_info() const;
    const iabpsk_notif_s&                      iabpsk_notif() const;
    const bc_bearer_context_release_request_s& bc_bearer_context_release_request() const;
    const mc_bearer_context_release_request_s& mc_bearer_context_release_request() const;
    const mc_bearer_notif_s&                   mc_bearer_notif() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // SuccessfulOutcome ::= OPEN TYPE
  struct successful_outcome_c {
    struct types_opts {
      enum options {
        reset_ack,
        gnb_cu_up_e1_setup_resp,
        gnb_cu_cp_e1_setup_resp,
        gnb_cu_up_cfg_upd_ack,
        gnb_cu_cp_cfg_upd_ack,
        e1_release_resp,
        bearer_context_setup_resp,
        bearer_context_mod_resp,
        bearer_context_mod_confirm,
        bearer_context_release_complete,
        res_status_resp,
        iab_up_tnl_address_upd_ack,
        bc_bearer_context_setup_resp,
        bc_bearer_context_mod_resp,
        bc_bearer_context_mod_confirm,
        bc_bearer_context_release_complete,
        mc_bearer_context_setup_resp,
        mc_bearer_context_mod_resp,
        mc_bearer_context_mod_confirm,
        mc_bearer_context_release_complete,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    successful_outcome_c() = default;
    void          set(types::options e = types::nulltype);
    types         type() const { return type_; }
    OCUDUASN_CODE pack(bit_ref& bref) const;
    OCUDUASN_CODE unpack(cbit_ref& bref);
    void          to_json(json_writer& j) const;
    // getters
    reset_ack_s&                                reset_ack();
    gnb_cu_up_e1_setup_resp_s&                  gnb_cu_up_e1_setup_resp();
    gnb_cu_cp_e1_setup_resp_s&                  gnb_cu_cp_e1_setup_resp();
    gnb_cu_up_cfg_upd_ack_s&                    gnb_cu_up_cfg_upd_ack();
    gnb_cu_cp_cfg_upd_ack_s&                    gnb_cu_cp_cfg_upd_ack();
    e1_release_resp_s&                          e1_release_resp();
    bearer_context_setup_resp_s&                bearer_context_setup_resp();
    bearer_context_mod_resp_s&                  bearer_context_mod_resp();
    bearer_context_mod_confirm_s&               bearer_context_mod_confirm();
    bearer_context_release_complete_s&          bearer_context_release_complete();
    res_status_resp_s&                          res_status_resp();
    iab_up_tnl_address_upd_ack_s&               iab_up_tnl_address_upd_ack();
    bc_bearer_context_setup_resp_s&             bc_bearer_context_setup_resp();
    bc_bearer_context_mod_resp_s&               bc_bearer_context_mod_resp();
    bc_bearer_context_mod_confirm_s&            bc_bearer_context_mod_confirm();
    bc_bearer_context_release_complete_s&       bc_bearer_context_release_complete();
    mc_bearer_context_setup_resp_s&             mc_bearer_context_setup_resp();
    mc_bearer_context_mod_resp_s&               mc_bearer_context_mod_resp();
    mc_bearer_context_mod_confirm_s&            mc_bearer_context_mod_confirm();
    mc_bearer_context_release_complete_s&       mc_bearer_context_release_complete();
    const reset_ack_s&                          reset_ack() const;
    const gnb_cu_up_e1_setup_resp_s&            gnb_cu_up_e1_setup_resp() const;
    const gnb_cu_cp_e1_setup_resp_s&            gnb_cu_cp_e1_setup_resp() const;
    const gnb_cu_up_cfg_upd_ack_s&              gnb_cu_up_cfg_upd_ack() const;
    const gnb_cu_cp_cfg_upd_ack_s&              gnb_cu_cp_cfg_upd_ack() const;
    const e1_release_resp_s&                    e1_release_resp() const;
    const bearer_context_setup_resp_s&          bearer_context_setup_resp() const;
    const bearer_context_mod_resp_s&            bearer_context_mod_resp() const;
    const bearer_context_mod_confirm_s&         bearer_context_mod_confirm() const;
    const bearer_context_release_complete_s&    bearer_context_release_complete() const;
    const res_status_resp_s&                    res_status_resp() const;
    const iab_up_tnl_address_upd_ack_s&         iab_up_tnl_address_upd_ack() const;
    const bc_bearer_context_setup_resp_s&       bc_bearer_context_setup_resp() const;
    const bc_bearer_context_mod_resp_s&         bc_bearer_context_mod_resp() const;
    const bc_bearer_context_mod_confirm_s&      bc_bearer_context_mod_confirm() const;
    const bc_bearer_context_release_complete_s& bc_bearer_context_release_complete() const;
    const mc_bearer_context_setup_resp_s&       mc_bearer_context_setup_resp() const;
    const mc_bearer_context_mod_resp_s&         mc_bearer_context_mod_resp() const;
    const mc_bearer_context_mod_confirm_s&      mc_bearer_context_mod_confirm() const;
    const mc_bearer_context_release_complete_s& mc_bearer_context_release_complete() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };
  // UnsuccessfulOutcome ::= OPEN TYPE
  struct unsuccessful_outcome_c {
    struct types_opts {
      enum options {
        gnb_cu_up_e1_setup_fail,
        gnb_cu_cp_e1_setup_fail,
        gnb_cu_up_cfg_upd_fail,
        gnb_cu_cp_cfg_upd_fail,
        bearer_context_setup_fail,
        bearer_context_mod_fail,
        res_status_fail,
        iab_up_tnl_address_upd_fail,
        bc_bearer_context_setup_fail,
        bc_bearer_context_mod_fail,
        mc_bearer_context_setup_fail,
        mc_bearer_context_mod_fail,
        nulltype
      } value;

      const char* to_string() const;
    };
    using types = enumerated<types_opts>;

    // choice methods
    unsuccessful_outcome_c() = default;
    void          set(types::options e = types::nulltype);
    types         type() const { return type_; }
    OCUDUASN_CODE pack(bit_ref& bref) const;
    OCUDUASN_CODE unpack(cbit_ref& bref);
    void          to_json(json_writer& j) const;
    // getters
    gnb_cu_up_e1_setup_fail_s&            gnb_cu_up_e1_setup_fail();
    gnb_cu_cp_e1_setup_fail_s&            gnb_cu_cp_e1_setup_fail();
    gnb_cu_up_cfg_upd_fail_s&             gnb_cu_up_cfg_upd_fail();
    gnb_cu_cp_cfg_upd_fail_s&             gnb_cu_cp_cfg_upd_fail();
    bearer_context_setup_fail_s&          bearer_context_setup_fail();
    bearer_context_mod_fail_s&            bearer_context_mod_fail();
    res_status_fail_s&                    res_status_fail();
    iab_up_tnl_address_upd_fail_s&        iab_up_tnl_address_upd_fail();
    bc_bearer_context_setup_fail_s&       bc_bearer_context_setup_fail();
    bc_bearer_context_mod_fail_s&         bc_bearer_context_mod_fail();
    mc_bearer_context_setup_fail_s&       mc_bearer_context_setup_fail();
    mc_bearer_context_mod_fail_s&         mc_bearer_context_mod_fail();
    const gnb_cu_up_e1_setup_fail_s&      gnb_cu_up_e1_setup_fail() const;
    const gnb_cu_cp_e1_setup_fail_s&      gnb_cu_cp_e1_setup_fail() const;
    const gnb_cu_up_cfg_upd_fail_s&       gnb_cu_up_cfg_upd_fail() const;
    const gnb_cu_cp_cfg_upd_fail_s&       gnb_cu_cp_cfg_upd_fail() const;
    const bearer_context_setup_fail_s&    bearer_context_setup_fail() const;
    const bearer_context_mod_fail_s&      bearer_context_mod_fail() const;
    const res_status_fail_s&              res_status_fail() const;
    const iab_up_tnl_address_upd_fail_s&  iab_up_tnl_address_upd_fail() const;
    const bc_bearer_context_setup_fail_s& bc_bearer_context_setup_fail() const;
    const bc_bearer_context_mod_fail_s&   bc_bearer_context_mod_fail() const;
    const mc_bearer_context_setup_fail_s& mc_bearer_context_setup_fail() const;
    const mc_bearer_context_mod_fail_s&   mc_bearer_context_mod_fail() const;

  private:
    types             type_;
    choice_buffer_ptr c;
  };

  // members lookup methods
  static uint16_t               idx_to_proc_code(uint32_t idx);
  static bool                   is_proc_code_valid(const uint16_t& proc_code);
  static init_msg_c             get_init_msg(const uint16_t& proc_code);
  static successful_outcome_c   get_successful_outcome(const uint16_t& proc_code);
  static unsuccessful_outcome_c get_unsuccessful_outcome(const uint16_t& proc_code);
  static crit_e                 get_crit(const uint16_t& proc_code);
};

// InitiatingMessage ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
struct init_msg_s {
  uint16_t                      proc_code = 0;
  crit_e                        crit;
  e1ap_elem_procs_o::init_msg_c value;

  OCUDUASN_CODE pack(bit_ref& bref) const;
  OCUDUASN_CODE unpack(cbit_ref& bref);
  void          to_json(json_writer& j) const;
  bool          load_info_obj(const uint16_t& proc_code_);
};

// SuccessfulOutcome ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
struct successful_outcome_s {
  uint16_t                                proc_code = 0;
  crit_e                                  crit;
  e1ap_elem_procs_o::successful_outcome_c value;

  OCUDUASN_CODE pack(bit_ref& bref) const;
  OCUDUASN_CODE unpack(cbit_ref& bref);
  void          to_json(json_writer& j) const;
  bool          load_info_obj(const uint16_t& proc_code_);
};

// UnsuccessfulOutcome ::= SEQUENCE{{E1AP-ELEMENTARY-PROCEDURE}}
struct unsuccessful_outcome_s {
  uint16_t                                  proc_code = 0;
  crit_e                                    crit;
  e1ap_elem_procs_o::unsuccessful_outcome_c value;

  OCUDUASN_CODE pack(bit_ref& bref) const;
  OCUDUASN_CODE unpack(cbit_ref& bref);
  void          to_json(json_writer& j) const;
  bool          load_info_obj(const uint16_t& proc_code_);
};

// E1AP-PDU ::= CHOICE
struct e1ap_pdu_c {
  struct types_opts {
    enum options { init_msg, successful_outcome, unsuccessful_outcome, /*...*/ nulltype } value;

    const char* to_string() const;
  };
  using types = enumerated<types_opts, true>;

  // choice methods
  e1ap_pdu_c() = default;
  e1ap_pdu_c(const e1ap_pdu_c& other);
  e1ap_pdu_c& operator=(const e1ap_pdu_c& other);
  ~e1ap_pdu_c() { destroy_(); }
  void          set(types::options e = types::nulltype);
  types         type() const { return type_; }
  OCUDUASN_CODE pack(bit_ref& bref) const;
  OCUDUASN_CODE unpack(cbit_ref& bref);
  void          to_json(json_writer& j) const;
  // getters
  init_msg_s& init_msg()
  {
    assert_choice_type(types::init_msg, type_, "E1AP-PDU");
    return c.get<init_msg_s>();
  }
  successful_outcome_s& successful_outcome()
  {
    assert_choice_type(types::successful_outcome, type_, "E1AP-PDU");
    return c.get<successful_outcome_s>();
  }
  unsuccessful_outcome_s& unsuccessful_outcome()
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "E1AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  const init_msg_s& init_msg() const
  {
    assert_choice_type(types::init_msg, type_, "E1AP-PDU");
    return c.get<init_msg_s>();
  }
  const successful_outcome_s& successful_outcome() const
  {
    assert_choice_type(types::successful_outcome, type_, "E1AP-PDU");
    return c.get<successful_outcome_s>();
  }
  const unsuccessful_outcome_s& unsuccessful_outcome() const
  {
    assert_choice_type(types::unsuccessful_outcome, type_, "E1AP-PDU");
    return c.get<unsuccessful_outcome_s>();
  }
  init_msg_s&             set_init_msg();
  successful_outcome_s&   set_successful_outcome();
  unsuccessful_outcome_s& set_unsuccessful_outcome();

private:
  types                                                                     type_;
  choice_buffer_t<init_msg_s, successful_outcome_s, unsuccessful_outcome_s> c;

  void destroy_();
};

} // namespace e1ap
} // namespace asn1
