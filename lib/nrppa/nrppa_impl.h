// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_context/nrppa_du_context.h"
#include "meas_context/nrppa_meas_context.h"
#include "ue_context/nrppa_ue_context.h"
#include "ocudu/asn1/nrppa/nrppa.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/nrppa/nrppa.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/ran/positioning/positioning_ids.h"
#include "ocudu/support/async/async_task_scheduler.h"

namespace ocudu::ocucp {

class nrppa_impl : public nrppa_interface, public nrppa_message_handler, public nrppa_ue_context_removal_handler
{
public:
  nrppa_impl(const cu_cp_configuration& cfg,
             nrppa_cu_cp_notifier&      cu_cp_notifier_,
             async_task_scheduler&      common_task_sched_);
  ~nrppa_impl();

  // See nrppa_message_handler for documentation.
  void handle_new_nrppa_pdu(const byte_buffer&                                nrppa_pdu,
                            std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t> ue_or_amf_index) override;

  // See nrppa_ue_context_removal_handler for documentation.
  void remove_ue_context(cu_cp_ue_index_t ue_index) override;

  /// \brief Initialize the measurement report timer for the UE.
  /// \param[in] ue_index The UE index.
  /// \param[in] meas_periodicity_ms The measurement periodicity in milliseconds.
  void initialize_meas_report_timer(cu_cp_ue_index_t ue_index, std::chrono::milliseconds meas_periodicity_ms);

  nrppa_message_handler&            get_nrppa_message_handler() override { return *this; }
  nrppa_ue_context_removal_handler& get_nrppa_ue_context_removal_handler() override { return *this; }

private:
  /// \brief Send the measurement results to the LMF.
  void handle_e_cid_meas_result(cu_cp_ue_index_t ue_index, const nrppa_e_cid_meas_result& result);

  /// \brief Callback function for the measurement report timer. This sends a measurement report to the LMF.
  void on_meas_report_timer_expired(cu_cp_ue_index_t ue_index);

  /// \brief Handle the reception of an initiating message.
  /// \param[in] msg The received initiating message.
  /// \param[in] ue_or_amf_index The UE index for UE associated NRPPa messages or the AMF index for non UE associated
  /// NRPPa messages.
  void handle_initiating_message(const asn1::nrppa::init_msg_s&                    msg,
                                 std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t> ue_or_amf_index);

  /// \brief Handle an E-CID measurement initiation request.
  void handle_e_cid_meas_initiation_request(const asn1::nrppa::e_c_id_meas_initiation_request_s& msg,
                                            cu_cp_ue_index_t                                     ue_index,
                                            uint16_t                                             transaction_id);

  /// \brief Handle an E-CID measurement termination command.
  void handle_e_cid_meas_termination_command(const asn1::nrppa::e_c_id_meas_termination_cmd_s& msg,
                                             cu_cp_ue_index_t                                  ue_index);

  /// \brief Handle a TRP information request.
  void handle_trp_information_request(const asn1::nrppa::trp_info_request_s& msg,
                                      cu_cp_amf_index_t                      amf_index,
                                      uint16_t                               transaction_id);

  /// \brief Handle a positioning information request.
  void handle_positioning_information_request(const asn1::nrppa::positioning_info_request_s& msg,
                                              cu_cp_ue_index_t                               ue_index,
                                              uint16_t                                       transaction_id);

  /// \brief Handle a positioning activation request.
  void handle_positioning_activation_request(const asn1::nrppa::positioning_activation_request_s& msg,
                                             cu_cp_ue_index_t                                     ue_index,
                                             uint16_t                                             transaction_id);

  /// \brief Handle a measurement request.
  void handle_measurement_request(const asn1::nrppa::meas_request_s& msg,
                                  cu_cp_amf_index_t                  amf_index,
                                  uint16_t                           transaction_id);

  /// \brief Handle the reception of a successful outcome message.
  /// \param[in] outcome The successful outcome message.
  void handle_successful_outcome(const asn1::nrppa::successful_outcome_s& outcome);

  /// \brief Handle the reception of an unsuccessful outcome message.
  /// \param[in] outcome The unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::nrppa::unsuccessful_outcome_s& outcome);

  ocudulog::basic_logger& logger;
  nrppa_ue_context_list   ue_ctxt_list;
  nrppa_du_context_list   du_ctxt_list;
  nrppa_meas_context_list meas_ctxt_list;
  cu_cp_configuration     cu_cp_cfg;
  nrppa_cu_cp_notifier&   cu_cp_notifier;
  async_task_scheduler&   common_task_sched;

  std::map<plmn_identity, tac_t>       plmn_to_tac;
  std::map<trp_id_t, cu_cp_du_index_t> trp_id_to_du_idx;
};

} // namespace ocudu::ocucp
