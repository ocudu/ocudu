// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "metrics/ngap_metrics_aggregator.h"
#include "ngap_connection_handler.h"
#include "ngap_error_indication_helper.h"
#include "procedures/ngap_transaction_manager.h"
#include "ue_context/ngap_ue_context.h"
#include "ocudu/asn1/ngap/ngap.h"
#include "ocudu/ngap/gateways/n2_connection_client.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ngap/ngap_configuration.h"
#include "ocudu/ngap/ngap_ue_radio_capability_management.h"
#include "ocudu/support/executors/task_executor.h"
#include <memory>

namespace ocudu::ocucp {

class ngap_impl final : public ngap_interface
{
public:
  ngap_impl(const ngap_configuration& ngap_cfg_,
            ngap_cu_cp_notifier&      cu_cp_notifier_,
            n2_connection_client&     n2_gateway,
            timer_manager&            timers_,
            task_executor&            ctrl_exec_);
  ~ngap_impl();

  bool update_ue_index(cu_cp_ue_index_t        new_ue_index,
                       cu_cp_ue_index_t        old_ue_index,
                       ngap_cu_cp_ue_notifier& new_ue_notifier) override;
  std::optional<ngap_core_network_assist_info_for_inactive>
  get_cn_assist_info_for_inactive(cu_cp_ue_index_t ue_index) override;

  // NGAP connection manager functions.
  bool                             handle_amf_tnl_connection_request() override;
  async_task<void>                 handle_amf_disconnection_request() override;
  async_task<ngap_ng_setup_result> handle_ng_setup_request(unsigned max_setup_retries) override;
  async_task<void>                 handle_ng_reset_message(const cu_cp_reset& msg) override;

  // ngap_nas_message_handler.
  void handle_initial_ue_message(const cu_cp_initial_ue_message& msg) override;
  void handle_ul_nas_transport_message(const cu_cp_ul_nas_transport& msg) override;

  // ngap_location_reporting_handler.
  void handle_location_report_transmission(const location_report& msg) override;
  void
  handle_location_reporting_failure_indication_transmission(const location_report_failure_indication& msg) override;

  // ngap_ue_radio_capability_management_handler.
  void
  handle_tx_ue_radio_capability_info_indication_required(const ngap_ue_radio_capability_info_indication& msg) override;

  // NGAP message handler functions.
  void handle_message(const ngap_message& msg) override;
  void handle_connection_loss() override {}

  // ngap_control_message_handler
  async_task<bool> handle_ue_context_release_request(const cu_cp_ue_context_release_request& msg) override;
  async_task<ngap_handover_preparation_response>
       handle_handover_preparation_request(const ngap_handover_preparation_request& msg) override;
  void handle_ul_ran_status_transfer(const cu_cp_status_transfer& ul_ran_status_transfer) override;
  async_task<expected<cu_cp_status_transfer>>
                        handle_dl_ran_status_transfer_required(cu_cp_ue_index_t ue_index) override;
  void                  handle_inter_cu_ho_rrc_recfg_complete(const cu_cp_ue_index_t     ue_index,
                                                              const nr_cell_global_id_t& cgi,
                                                              const unsigned             tac) override;
  const ngap_context_t& get_ngap_context() const override { return context; }
  void handle_ul_ue_associated_nrppa_transport(cu_cp_ue_index_t ue_index, const byte_buffer& nrppa_pdu) override;
  async_task<void> handle_ul_non_ue_associated_nrppa_transport(const byte_buffer& nrppa_pdu) override;
  async_task<bool>
  handle_rrc_inactive_transition_report_required(const ngap_rrc_inactive_transition_report& report) override;
  async_task<cu_cp_path_switch_response>
  handle_path_switch_request_required(const cu_cp_path_switch_request& request) override;

  // ngap_metrics_handler.
  ngap_info handle_ngap_metrics_report_request() const override;

  // ngap_statistics_handler.
  size_t get_nof_ues() const override { return ue_ctxt_list.size(); }

  // ngap_ue_context_removal_handler.
  void remove_ue_context(cu_cp_ue_index_t ue_index) override;

  // ngap_ue_id_translator.
  cu_cp_ue_index_t get_ue_index(const amf_ue_id_t& amf_ue_ngap_id) override;
  amf_ue_id_t      get_amf_ue_id(const cu_cp_ue_index_t& ue_index) override;

  ngap_message_handler&                        get_ngap_message_handler() override { return *this; }
  ngap_event_handler&                          get_ngap_event_handler() override { return *this; }
  ngap_connection_manager&                     get_ngap_connection_manager() override { return *this; }
  ngap_nas_message_handler&                    get_ngap_nas_message_handler() override { return *this; }
  ngap_location_reporting_handler&             get_ngap_location_reporting_handler() override { return *this; }
  ngap_ue_radio_capability_management_handler& get_ngap_ue_radio_cap_management_handler() override { return *this; }
  ngap_control_message_handler&                get_ngap_control_message_handler() override { return *this; }
  ngap_ue_control_manager&                     get_ngap_ue_control_manager() override { return *this; }
  ngap_metrics_handler&                        get_metrics_handler() override { return *this; }
  ngap_statistics_handler&                     get_ngap_statistics_handler() override { return *this; }
  ngap_ue_context_removal_handler&             get_ngap_ue_context_removal_handler() override { return *this; }
  ngap_ue_id_translator&                       get_ngap_ue_id_translator() override { return *this; }

private:
  class tx_pdu_notifier_with_logging final : public ngap_message_notifier
  {
  public:
    tx_pdu_notifier_with_logging(ngap_impl& parent_) : parent(parent_) {}

    ~tx_pdu_notifier_with_logging()
    {
      if (decorated) {
        decorated.reset();
      }
    }

    void connect(std::unique_ptr<ngap_message_notifier> decorated_) { decorated = std::move(decorated_); }

    void disconnect()
    {
      if (is_connected()) {
        decorated.reset();
      }
    }

    bool is_connected() const { return decorated != nullptr; }

    [[nodiscard]] bool on_new_message(const ngap_message& msg) override;

  private:
    ngap_impl&                             parent;
    std::unique_ptr<ngap_message_notifier> decorated;
  };

  /// \brief Notify about the reception of an initiating message.
  /// \param[in] msg The received initiating message.
  void handle_initiating_message(const asn1::ngap::init_msg_s& msg);

  /// \brief Notify about the reception of an DL NAS Transport message.
  /// \param[in] msg The received DL NAS Transport message.
  void handle_dl_nas_transport_message(const asn1::ngap::dl_nas_transport_s& msg);

  /// \brief Notify about the reception of an Initial Context Setup Request.
  /// \param[in] msg The received Initial Context Setup Request.
  void handle_initial_context_setup_request(const asn1::ngap::init_context_setup_request_s& request);

  /// \brief Notify about the reception of an UE Context Modification Request.
  /// \param[in] msg The received UE Context Modification Request.
  void handle_ue_context_modification_request(const asn1::ngap::ue_context_mod_request_s& request);

  /// \brief Notify about the reception of an PDU Session Resource Setup Request.
  /// \param[in] msg The received PDU Session Resource Setup Request.
  void handle_pdu_session_resource_setup_request(const asn1::ngap::pdu_session_res_setup_request_s& request);

  /// \brief Notify about the reception of an PDU Session Resource Modify Request.
  /// \param[in] msg The received PDU Session Resource Modify Request.
  void handle_pdu_session_resource_modify_request(const asn1::ngap::pdu_session_res_modify_request_s& request);

  /// \brief Notify about the reception of an PDU Session Resource Release Command.
  /// \param[in] msg The received PDU Session Resource Release Command.
  void handle_pdu_session_resource_release_command(const asn1::ngap::pdu_session_res_release_cmd_s& command);

  /// \brief Notify about the reception of an UE Context Release Command.
  /// \param[in] msg The received UE Context Release Command.
  void handle_ue_context_release_command(const asn1::ngap::ue_context_release_cmd_s& cmd);

  /// \brief Notify about the reception of a Paging message.
  /// \param[in] msg The received Paging message.
  void handle_paging(const asn1::ngap::paging_s& msg);

  /// \brief Send a handover failure to the AMF.
  /// \param[in] amf_ue_id The AMF UE NGAP ID.
  void send_handover_failure(uint64_t amf_ue_id);

  /// \brief Notify about the reception of a Handover request message.
  /// \param[in] msg The received handover request message.
  void handle_handover_request(const asn1::ngap::ho_request_s& msg);

  /// \brief Handle the reception of the DL RAN Status Transfer message.
  /// \param[in] msg The received DL RAN Status Transfer.
  void handle_dl_ran_status_transfer(const asn1::ngap::dl_ran_status_transfer_s& msg);

  /// \brief Notifiy about the reception of a DL UE Associated NRPPA Transport message.
  void handle_dl_ue_associated_nrppa_transport(const asn1::ngap::dl_ue_associated_nrppa_transport_s& msg);

  /// \brief Notifiy about the reception of a DL Non UE Associated NRPPA Transport message.
  void handle_dl_non_ue_associated_nrppa_transport(const asn1::ngap::dl_non_ue_associated_nrppa_transport_s& msg);

  /// \brief Notifiy about the reception of a Location Reporting Control message.
  void handle_location_reporting_control_message(const asn1::ngap::location_report_ctrl_s& msg);

  /// \brief Notify about the reception of an Error Indication message.
  /// \param[in] msg The received Error Indication message.
  void handle_error_indication(const asn1::ngap::error_ind_s& msg);

  /// \brief Notify about the reception of a successful outcome message.
  /// \param[in] outcome The successful outcome message.
  void handle_successful_outcome(const asn1::ngap::successful_outcome_s& outcome);

  /// \brief Notify about the reception of an unsuccessful outcome message.
  /// \param[in] outcome The unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::ngap::unsuccessful_outcome_s& outcome);

  /// \brief Schedule the transmission of an Error Indication message on the UE task executor.
  /// \param[in] ue_index The index of the related UE.
  /// \param[in] cause The cause of the Error Indication.
  /// \param[in] amf_ue_id The AMF UE ID.
  void
  schedule_error_indication(cu_cp_ue_index_t ue_index, ngap_cause_t cause, std::optional<amf_ue_id_t> amf_ue_id = {});

  /// \brief Callback for the PDU Session Request Timer expiration. Triggers the release of the UE.
  void on_request_pdu_session_timer_expired(cu_cp_ue_index_t ue_index);

  /// \brief Validates consistent UE id pair. It checks if an existing context already exists
  /// for the received AMF-UE-NGAP-ID and checks if it matches the received RAN-UE-NGAP-ID.
  /// \param[in] ran_ue_ngap_id The received RAN-UE-NGAP-ID.
  /// \param[in] amf_ue_ngap_id The received AMF-UE-NGAP-ID.
  /// \return True if the pair is consistent, false otherwise.
  [[nodiscard]] bool validate_consistent_ue_id_pair(ran_ue_id_t ran_ue_ngap_id, amf_ue_id_t amf_ue_ngap_id);

  /// \brief Handles an inconsistent UE id pair. It releases the old UE context and sends an error indication to the
  /// AMF.
  /// \param[in] ran_ue_ngap_id The inconsistent RAN-UE-NGAP-ID.
  /// \param[in] amf_ue_ngap_id The inconsistent AMF-UE-NGAP-ID.
  void handle_inconsistent_ue_id_pair(ran_ue_id_t ran_ue_ngap_id, amf_ue_id_t amf_ue_ngap_id);

  /// \brief Log NGAP RX PDU.
  void log_rx_pdu(const ngap_message& msg);

  ngap_context_t context;

  ocudulog::basic_logger& logger;

  /// Repository of UE Contexts.
  ngap_ue_context_list ue_ctxt_list;

  std::unordered_map<cu_cp_ue_index_t, error_indication_request_t> stored_error_indications;

  ngap_cu_cp_notifier& cu_cp_notifier;
  timer_manager&       timers;
  task_executor&       ctrl_exec;

  // Metrics aggregator.
  ngap_metrics_aggregator metrics_handler;

  ngap_transaction_manager ev_mng;

  ngap_connection_handler conn_handler;

  tx_pdu_notifier_with_logging tx_pdu_notifier;
};

} // namespace ocudu::ocucp
