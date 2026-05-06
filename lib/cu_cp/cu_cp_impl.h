// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "adapters/cell_meas_manager_adapters.h"
#include "adapters/du_processor_adapters.h"
#include "adapters/e1ap_adapters.h"
#include "adapters/mobility_manager_adapters.h"
#include "adapters/ngap_adapters.h"
#include "adapters/nrppa_adapters.h"
#include "cu_configurator_impl.h"
#include "cu_cp_controller/cu_cp_controller.h"
#include "cu_cp_impl_interface.h"
#include "cu_up_processor/cu_up_processor_repository.h"
#include "du_processor/du_processor_repository.h"
#include "ngap_repository.h"
#include "routines/mobility/inter_cu_handover_target_routine.h"
#include "ue_manager/ue_manager_impl.h"
#include "xnap_repository.h"
#include "ocudu/cu_cp/cu_configurator.h"
#include "ocudu/cu_cp/cu_cp.h"
#include "ocudu/cu_cp/cu_cp_configuration.h"
#include "ocudu/cu_cp/inter_cu_handover_messages.h"
#include "ocudu/nrppa/nrppa.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/support/async/async_task_scheduler.h"
#include <dlfcn.h>
#include <memory>

namespace ocudu::ocucp {

class cu_cp_common_task_scheduler : public async_task_scheduler
{
public:
  cu_cp_common_task_scheduler() : main_ctrl_loop(128) {}

  bool schedule(async_task<void> task) override { return main_ctrl_loop.schedule(std::move(task)); }

private:
  // cu-cp task event loop
  fifo_async_task_scheduler main_ctrl_loop;
};

class cu_cp_impl final : public cu_cp,
                         public cu_cp_impl_interface,
                         public cu_cp_ng_handler,
                         public cu_cp_command_handler,
                         public cu_cp_ue_release_command_handler
{
public:
  explicit cu_cp_impl(const cu_cp_configuration& config_);
  ~cu_cp_impl() override;

  bool start() override;
  void stop() override;

  // NGAP interface.
  ngap_message_handler* get_ngap_message_handler(const plmn_identity& plmn) override;

  bool amfs_are_connected() override;

  // CU-UP handler.
  void handle_bearer_context_release_request(const cu_cp_bearer_context_release_request& msg) override;
  void handle_bearer_context_inactivity_notification(const e1ap_inactivity_notification& msg) override;
  void handle_dl_data_notification(cu_cp_ue_index_t ue_index) override;
  void handle_e1_release_request(cu_cp_cu_up_index_t cu_up_index) override;

  // cu_cp_rrc_ue_interface.
  bool handle_ue_plmn_selected(cu_cp_ue_index_t ue_index, const plmn_identity& plmn) override;
  rrc_ue_reestablishment_context_response
  handle_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, cu_cp_ue_index_t ue_index) override;
  async_task<bool> handle_rrc_reestablishment_context_modification_required(cu_cp_ue_index_t ue_index) override;

  void             handle_rrc_reestablishment_failure(const cu_cp_ue_context_release_request& request) override;
  void             handle_rrc_reestablishment_complete(cu_cp_ue_index_t old_ue_index) override;
  async_task<bool> handle_ue_context_transfer(cu_cp_ue_index_t ue_index, cu_cp_ue_index_t old_ue_index) override;
  async_task<void> handle_ue_context_release(const cu_cp_ue_context_release_request& request) override;
  async_task<void> handle_access_success(const cu_cp_access_success_indication& msg) override;
  async_task<rrc_resume_request_response> handle_rrc_resume_request(const rrc_resume_request& request) override;
  void                                    handle_ran_paging_required(cu_cp_ue_index_t ue_index) override;

  // cu_cp_ue_context_manipulation_handler.
  void handle_handover_reconfiguration_sent(const cu_cp_intra_cu_handover_target_request& request) override;
  void handle_cho_reconfiguration_sent(const cu_cp_cho_target_request& request) override;
  void handle_handover_ue_context_push(cu_cp_ue_index_t source_ue_index, cu_cp_ue_index_t target_ue_index) override;
  void
       initialize_handover_ue_release_timer(cu_cp_ue_index_t                        ue_index,
                                            std::chrono::milliseconds               handover_ue_release_timeout,
                                            const cu_cp_ue_context_release_request& ue_context_release_request) override;
  void initialize_rna_update_timer(cu_cp_ue_index_t ue_index) override;
  void initialize_cho_execution_timer(cu_cp_ue_index_t source_ue_index, std::chrono::milliseconds timeout) override;

  // cu_cp_location_manager_handler.
  void handle_location_update(cu_cp_ue_index_t ue_index) override;

  // cu_cp_ngap_handler.
  async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>
  handle_new_initial_context_setup_request(const ngap_init_context_setup_request& request) override;
  async_task<expected<ngap_ue_context_modification_response, ngap_ue_context_modification_failure>>
  handle_new_ue_context_modification_request(const ngap_ue_context_modification_request& request) override;
  async_task<ngap_pdu_session_resource_setup_response>
  handle_new_pdu_session_resource_setup_request(ngap_pdu_session_resource_setup_request& request) override;
  async_task<ngap_pdu_session_resource_modify_response>
  handle_new_pdu_session_resource_modify_request(const ngap_pdu_session_resource_modify_request& request) override;
  async_task<ngap_pdu_session_resource_release_response>
  handle_new_pdu_session_resource_release_command(const ngap_pdu_session_resource_release_command& command) override;
  async_task<cu_cp_ue_context_release_complete>
  handle_ue_context_release_command(const cu_cp_ue_context_release_command& command) override;
  async_task<cu_cp_handover_resource_allocation_response>
       handle_ngap_handover_request(const ngap_handover_request& request) override;
  void handle_transmission_of_handover_required() override;
  void handle_dl_ue_associated_nrppa_transport_pdu(cu_cp_ue_index_t ue_index, const byte_buffer& nrppa_pdu) override;
  void handle_dl_non_ue_associated_nrppa_transport_pdu(cu_cp_amf_index_t  amf_index,
                                                       const byte_buffer& nrppa_pdu) override;
  void handle_location_reporting_control_message(cu_cp_ue_index_t               ue_index,
                                                 const location_report_request& msg) override;
  void handle_n2_disconnection(cu_cp_amf_index_t amf_index) override;

  // cu_cp_inter_cu_handover_handler.
  async_task<bool> handle_new_rrc_handover_command(cu_cp_ue_index_t                ue_index,
                                                   byte_buffer                     command,
                                                   std::optional<xnc_peer_index_t> xnc_index = std::nullopt) override;
  cu_cp_ue_index_t handle_ue_index_allocation_request(const nr_cell_global_id_t& cgi,
                                                      const plmn_identity&       plmn) override;
  bool             handle_handover_request(cu_cp_ue_index_t                  ue_index,
                                           const plmn_identity&              selected_plmn,
                                           const security::security_context& sec_ctxt) override;
  void             handle_inter_cu_target_handover_execution(cu_cp_ue_index_t ue_index,
                                                             const std::optional<xnap_handover_target_execution_context>&
                                                                 xnap_ho_target_execution_ctxt = std::nullopt) override;

  // cu_cp_xnap_handler.
  async_task<cu_cp_handover_resource_allocation_response>
       handle_xnap_handover_request(const xnap_handover_request& request) override;
  void handle_handover_cancel_received(cu_cp_ue_index_t ue_index) override;
  void handle_xnap_handover_success_received(cu_cp_ue_index_t  source_ue_index,
                                             peer_xnap_ue_id_t winner_peer_xnap_ue_id) override;
  void handle_xnap_ue_context_release_received(cu_cp_ue_index_t ue_index) override;

  // cu_cp_nrppa_handler.
  nrppa_cu_cp_ue_notifier* handle_new_nrppa_ue(cu_cp_ue_index_t ue_index) override;
  void                     handle_ul_nrppa_pdu(const byte_buffer&                                nrppa_pdu,
                                               std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t> ue_or_amf_index) override;
  async_task<trp_information_cu_cp_response_t>
  handle_trp_information_request(const trp_information_request_t& request) override;

  // cu_cp_measurement_handler.
  std::optional<rrc_meas_cfg>
       handle_measurement_config_request(cu_cp_ue_index_t                   ue_index,
                                         nr_cell_identity                   nci,
                                         const std::optional<rrc_meas_cfg>& current_meas_config = std::nullopt,
                                         bool                               cond_meas           = false,
                                         span<const pci_t>                  candidate_pcis      = {}) override;
  void handle_measurement_report(cu_cp_ue_index_t ue_index, const rrc_meas_results& meas_results) override;

  // cu_cp_measurement_config_handler.
  bool handle_cell_config_update_request(nr_cell_identity nci, const serving_cell_meas_config& serv_cell_cfg) override;

  // cu_cp_mobility_manager_handler.
  async_task<cu_cp_intra_cu_handover_response>
  handle_intra_cu_handover_request(const cu_cp_intra_cu_handover_request& request,
                                   cu_cp_du_index_t&                      source_du_index,
                                   cu_cp_du_index_t&                      target_du_index) override;

  async_task<cu_cp_intra_cu_cho_response>
  handle_intra_cu_cho_request(const cu_cp_intra_cu_cho_request& request) override;

  void handle_intra_cell_handover_required(cu_cp_ue_index_t ue_index) override;

  // cu_cp_ue_removal_handler.
  async_task<void> handle_ue_removal_request(cu_cp_ue_index_t ue_index) override;
  void             handle_pending_ue_task_cancellation(cu_cp_ue_index_t ue_index) override;

  // cu_cp_ue_release_command_handler.
  void trigger_release(pci_t                                         source_pci,
                       rnti_t                                        rnti,
                       std::optional<cu_cp_release_redirect_nr_info> redirect_info = std::nullopt) override;

  cu_cp_mobility_command_handler&   get_mobility_command_handler() override { return mobility_mng; }
  cu_cp_ue_release_command_handler& get_ue_release_command_handler() override { return *this; }
  metrics_handler&                  get_metrics_handler() override { return *metrics_hdlr; }

  // cu_cp_amf_reconnection_handler.
  void handle_amf_reconnection(cu_cp_amf_index_t amf_index) override;

  // cu_cp public interface.
  cu_cp_f1c_handler&                     get_f1c_handler() override { return controller.get_f1c_handler(); }
  cu_cp_e1_handler&                      get_e1_handler() override { return controller.get_e1_handler(); }
  cu_cp_xnc_handler&                     get_xnc_handler() override { return controller.get_xnc_handler(); }
  cu_cp_e1ap_event_handler&              get_cu_cp_e1ap_handler() override { return *this; }
  cu_cp_ng_handler&                      get_ng_handler() override { return *this; }
  cu_cp_ngap_handler&                    get_cu_cp_ngap_handler() override { return *this; }
  cu_cp_nrppa_handler&                   get_cu_cp_nrppa_handler() override { return *this; }
  cu_cp_command_handler&                 get_command_handler() override { return *this; }
  cu_cp_rrc_ue_interface&                get_cu_cp_rrc_ue_interface() override { return *this; }
  cu_cp_measurement_handler&             get_cu_cp_measurement_handler() override { return *this; }
  cu_cp_measurement_config_handler&      get_cu_cp_measurement_config_handler() override { return *this; }
  cu_cp_mobility_manager_handler&        get_cu_cp_mobility_manager_handler() override { return *this; }
  cu_cp_location_manager_handler&        get_cu_cp_location_manager_handler() override { return *this; }
  cu_cp_ue_removal_handler&              get_cu_cp_ue_removal_handler() override { return *this; }
  cu_cp_ue_context_manipulation_handler& get_cu_cp_ue_context_handler() override { return *this; }
  cu_cp_amf_reconnection_handler&        get_cu_cp_amf_reconnection_handler() override { return *this; }
  cu_cp_xnap_handler&                    get_cu_cp_xnap_handler() override { return *this; }
  cu_configurator&                       get_cu_configurator() override { return cu_cp_cfgtr; }

private:
  // Handling of DU events.
  void handle_rrc_ue_creation(cu_cp_ue_index_t ue_index, rrc_ue_interface& rrc_ue) override;

  byte_buffer handle_target_cell_sib1_required(cu_cp_du_index_t du_index, nr_cell_global_id_t cgi) override;

  async_task<void> handle_transaction_info_loss(const ue_transaction_info_loss_event& ev) override;

  // NGAP UE creation handler.
  ngap_cu_cp_ue_notifier* handle_new_ngap_ue(cu_cp_ue_index_t ue_index) override;

  // cu_cp_task_scheduler_handler.
  bool schedule_ue_task(cu_cp_ue_index_t ue_index, async_task<void> task) override;

  void request_ue_release(cu_cp_ue& ue, const ngap_cause_t& cause);

  /// \brief Initiate RAN paging of an inactive UE.
  /// \param[in] ue_index The index of the UE to page.
  /// \param[in] full_i_rnti Full I-RNTI of the UE to page.
  /// message.
  void send_ran_paging(cu_cp_ue_index_t ue_index, full_i_rnti_t full_i_rnti);

  /// \brief Request the release of an inactive UE that failed to resume within the expected time.
  /// \param[in] ue_index The index of the UE to release.
  void request_release_of_inactive_ue(cu_cp_ue_index_t ue_index);

  /// \brief Handle the reception of an inter-CU handover request message from the source CU.
  /// \param[in] request The inter-CU handover request message.
  /// \returns An asynchronous task that will complete with the response to the inter-CU handover request. The response
  /// indicates whether the handover preparation was successful and, if not, the cause of the failure.
  async_task<cu_cp_handover_resource_allocation_response>
  handle_inter_cu_handover_request(const cu_cp_inter_cu_handover_request& request);

  void on_statistics_report_timer_expired();

  cu_cp_configuration cfg;

  // Logger.
  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("CU-CP");

  // Components.
  // UE manager.
  ue_manager ue_mng;

  // Cell measurement manager.
  cell_meas_manager cell_meas_mng;

  cu_cp_common_task_scheduler common_task_sched;

  // DU repository to Node Manager adapter.
  du_processor_cu_cp_connection_adapter conn_notifier;

  // Cell Measurement Manager to mobility manager adapters.
  cell_meas_mobility_manager_adapter cell_meas_mobility_notifier;

  // E1AP to CU-CP adapter.
  e1ap_cu_cp_adapter e1ap_ev_notifier;

  // NGAP to CU-CP adapters.
  ngap_cu_cp_adapter ngap_cu_cp_ev_notifier;

  // Mobility manager to CU-CP adapter.
  mobility_manager_adapter mobility_manager_ev_notifier;

  // DU connections being managed by the CU-CP.
  du_processor_repository du_db;

  // CU-UP connections being managed by the CU-CP.
  cu_up_processor_repository cu_up_db;

  // NRPPa to CU-CP adapter.
  nrppa_cu_cp_adapter nrppa_cu_cp_ev_notifier;

  // NRPPa to F1AP adapter.
  std::map<cu_cp_du_index_t, nrppa_f1ap_adapter> nrppa_f1ap_ev_notifiers;

  // NRPPA entity.
  std::unique_ptr<nrppa_interface> nrppa_entity;

  // Handler of paging messages.
  paging_message_handler paging_handler;

  // AMF connections beeing managed by the CU-CP.
  ngap_repository ngap_db;

  // XNAP connections beeing managed by the CU-CP.
  xnap_repository xnap_db;

  // Mobility manager.
  mobility_manager mobility_mng;

  // Handler of the CU-CP connections to other remote nodes (e.g. AMF, CU-UPs, DUs).
  cu_cp_controller controller;

  std::unique_ptr<metrics_handler> metrics_hdlr;

  unique_timer statistics_report_timer;

  std::atomic<bool> stopped{false};

  cu_configurator_impl cu_cp_cfgtr;

  // Metrics report session for the lifetime of the CU-CP.
  // Used, e.g., for logging metrics and JSON metrics.
  std::unique_ptr<metrics_report_session> metrics_session;
};

} // namespace ocudu::ocucp
