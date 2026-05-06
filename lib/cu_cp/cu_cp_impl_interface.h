// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_cp/cell_meas_manager_config.h"
#include "ocudu/cu_cp/cu_cp_intra_cu_ho_types.h"
#include "ocudu/e1ap/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "ocudu/e1ap/cu_cp/inactivity_notification.h"
#include "ocudu/ngap/ngap.h"
#include "ocudu/ngap/ngap_pdu_session.h"
#include "ocudu/nrppa/nrppa.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/rrc/rrc_ue.h"
#include "ocudu/xnap/xnap_handover.h"

namespace ocudu::ocucp {

class cu_cp_ue_context_release_handler
{
public:
  virtual ~cu_cp_ue_context_release_handler() = default;

  /// \brief Handle the reception of a new UE Context Release Command.
  /// \param[in] command The UE Context Release Command.
  /// \returns The UE Context Release Complete.
  virtual async_task<cu_cp_ue_context_release_complete>
  handle_ue_context_release_command(const cu_cp_ue_context_release_command& command) = 0;
};

/// Interface for the CU-CP to schedule tasks for UEs.
class cu_cp_task_scheduler_handler
{
public:
  virtual ~cu_cp_task_scheduler_handler() = default;

  /// \brief Schedule a task for a UE.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] task The task to schedule.
  /// \returns True if the task was successfully scheduled, false otherwise.
  virtual bool schedule_ue_task(cu_cp_ue_index_t ue_index, async_task<void> task) = 0;
};

/// Interface for the inter-CU handover notifier to communicate with the CU-CP.
class cu_cp_inter_cu_handover_handler
{
public:
  virtual ~cu_cp_inter_cu_handover_handler() = default;

  /// \brief Handle the reception of a new RRC Handover Command.
  /// \param[in] ue_index The index of the UE that received the RRC Handover Command.
  /// \param[in] command The received RRC container containing the Handover Command.
  /// \param[in] xnc_index The XN-C index if the handover is a XN-C handover, std::nullopt otherwise.
  /// \returns True if the RRC Handover Command was successfully handled, false otherwise.
  virtual async_task<bool>
  handle_new_rrc_handover_command(cu_cp_ue_index_t                ue_index,
                                  byte_buffer                     command,
                                  std::optional<xnc_peer_index_t> xnc_index = std::nullopt) = 0;

  /// \brief Handles UE index allocation request for N2 handover at target gNB.
  virtual cu_cp_ue_index_t handle_ue_index_allocation_request(const nr_cell_global_id_t& cgi,
                                                              const plmn_identity&       plmn) = 0;

  /// \brief Handle a received handover request.
  /// \param[in] ue_index Index of the UE.
  /// \param[in] selected_plmn The selected PLMN identity of the UE.
  /// \param[in] sec_ctxt The received security context.
  /// \return True if the handover request was successfully handled, false otherwise.
  virtual bool handle_handover_request(cu_cp_ue_index_t                  ue_index,
                                       const plmn_identity&              selected_plmn,
                                       const security::security_context& sec_ctxt) = 0;

  /// \brief Handle the handover execution phase of the inter-CU handover at target gNB.
  /// \param[in] ue_index The index of the UE that is performing the handover.
  /// \param[in] xnap_ho_target_execution_ctxt If the handover is a XN-C handover, the information required for the
  /// target handover execution is included.
  virtual void handle_inter_cu_target_handover_execution(
      cu_cp_ue_index_t                                             ue_index,
      const std::optional<xnap_handover_target_execution_context>& xnap_ho_target_execution_ctxt = std::nullopt) = 0;
};

/// Interface for the NGAP notifier to communicate with the CU-CP.
class cu_cp_ngap_handler : public cu_cp_ue_context_release_handler,
                           public cu_cp_task_scheduler_handler,
                           public cu_cp_inter_cu_handover_handler
{
public:
  virtual ~cu_cp_ngap_handler() = default;

  /// \brief Handle the creation of a new NGAP UE. This will add the NGAP adapters to the UE manager.
  /// \param[in] ue_index The index of the new NGAP UE.
  /// \returns Pointer to the NGAP UE notifier.
  virtual ngap_cu_cp_ue_notifier* handle_new_ngap_ue(cu_cp_ue_index_t ue_index) = 0;

  /// \brief Handle the reception of a new Initial Context Setup Request.
  /// \param[in] request The received Initial Context Setup Request.
  /// \returns The Initial Context Setup Response or the Initial Context Setup Failure.
  virtual async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>
  handle_new_initial_context_setup_request(const ngap_init_context_setup_request& request) = 0;

  /// \brief Handle the reception of a new UE Context Modification Request.
  /// \param[in] request The received UE Context Modification Request.
  /// \returns The UE Context Modification Response or the UE Context Modification Failure.
  virtual async_task<expected<ngap_ue_context_modification_response, ngap_ue_context_modification_failure>>
  handle_new_ue_context_modification_request(const ngap_ue_context_modification_request& request) = 0;

  /// \brief Handle the reception of a new PDU Session Resource Setup Request.
  /// \param[in] request The received PDU Session Resource Setup Request.
  /// \returns The PDU Session Resource Setup Response.
  virtual async_task<ngap_pdu_session_resource_setup_response>
  handle_new_pdu_session_resource_setup_request(ngap_pdu_session_resource_setup_request& request) = 0;

  /// \brief Handle the reception of a new PDU Session Resource Modify Request.
  /// \param[in] request The received PDU Session Resource Modify Request.
  /// \returns The PDU Session Resource Modify Response.
  virtual async_task<ngap_pdu_session_resource_modify_response>
  handle_new_pdu_session_resource_modify_request(const ngap_pdu_session_resource_modify_request& request) = 0;

  /// \brief Handle the reception of a new PDU Session Resource Release Command.
  /// \param[in] command The received PDU Session Resource Release Command.
  /// \returns The PDU Session Resource Release Response.
  virtual async_task<ngap_pdu_session_resource_release_response>
  handle_new_pdu_session_resource_release_command(const ngap_pdu_session_resource_release_command& command) = 0;

  /// \brief Handle the handover request of the handover resource allocation procedure handover procedure.
  /// See TS 38.413 section 8.4.2.2.
  virtual async_task<cu_cp_handover_resource_allocation_response>
  handle_ngap_handover_request(const ngap_handover_request& request) = 0;

  /// \brief Handle the transmission of a handover required message to the AMF.
  virtual void handle_transmission_of_handover_required() = 0;

  /// \brief Handles a DL UE associated NRPPa transport.
  virtual void handle_dl_ue_associated_nrppa_transport_pdu(cu_cp_ue_index_t ue_index, const byte_buffer& nrppa_pdu) = 0;

  /// \brief Handles a DL non UE associated NRPPa transport.
  /// \param[in] amf_index The index of the AMF that received the NRPPa transport.
  /// \param[in] nrppa_pdu The NRPPa transport PDU.
  virtual void handle_dl_non_ue_associated_nrppa_transport_pdu(cu_cp_amf_index_t  amf_index,
                                                               const byte_buffer& nrppa_pdu) = 0;

  /// \brief Handles Location Reporting Control message.
  virtual void handle_location_reporting_control_message(cu_cp_ue_index_t               ue_index,
                                                         const location_report_request& msg) = 0;

  /// \brief Handle N2 AMF connection drop.
  /// \param[in] amf_index The index of the dropped AMF.
  virtual void handle_n2_disconnection(cu_cp_amf_index_t amf_index) = 0;
};

/// Interface for the NRPPa notifier to communicate with the CU-CP.
class cu_cp_nrppa_handler
{
public:
  virtual ~cu_cp_nrppa_handler() = default;

  /// \brief Handle the creation of a new NRPPA UE. This will add the NRPPA adapters to the UE manager.
  /// \param[in] ue_index The index of the new NRPPA UE.
  /// \returns Pointer to the NRPPA UE notifier.
  virtual nrppa_cu_cp_ue_notifier* handle_new_nrppa_ue(cu_cp_ue_index_t ue_index) = 0;

  /// \brief Handle a UL NRPPa PDU.
  /// \param[in] msg The NRPPa PDU.
  /// \param[in] ue_or_amf_index The UE index for UE associated NRPPa messages or the AMF index for non UE associated
  virtual void handle_ul_nrppa_pdu(const byte_buffer&                                nrppa_pdu,
                                   std::variant<cu_cp_ue_index_t, cu_cp_amf_index_t> ue_or_amf_index) = 0;

  /// \brief Handle a TRP information request.
  /// \param[in] request The TRP information request.
  /// \returns The TRP information CU-CP response.
  virtual async_task<trp_information_cu_cp_response_t>
  handle_trp_information_request(const trp_information_request_t& request) = 0;
};

/// Handler of E1AP-CU-CP events.
class cu_cp_e1ap_event_handler : public cu_cp_task_scheduler_handler
{
public:
  virtual ~cu_cp_e1ap_event_handler() = default;

  /// \brief Handle the reception of an Bearer Context Release Request message.
  /// \param[in] msg The received Bearer Context Release Request message.
  virtual void handle_bearer_context_release_request(const cu_cp_bearer_context_release_request& msg) = 0;

  /// \brief Handle the reception of an Bearer Context Inactivity Notification message.
  /// \param[in] msg The received Bearer Context Inactivity Notification message.
  virtual void handle_bearer_context_inactivity_notification(const e1ap_inactivity_notification& msg) = 0;

  /// \brief Handles the reception of a DL Data Notification message.
  /// \param[in] ue_index The index of the UE.
  virtual void handle_dl_data_notification(cu_cp_ue_index_t ue_index) = 0;

  /// \brief Handle a UE release request.
  /// \param[in] request The release request.
  virtual async_task<void> handle_ue_context_release(const cu_cp_ue_context_release_request& request) = 0;

  /// \brief Handles the reception of an E1 Release Request message.
  /// \param[in] cu_up_index The index of the CU-UP processor.
  virtual void handle_e1_release_request(cu_cp_cu_up_index_t cu_up_index) = 0;

  /// \brief Handle transaction information loss in the E1AP.
  virtual async_task<void> handle_transaction_info_loss(const ue_transaction_info_loss_event& ev) = 0;
};

/// Interface used to handle DU specific procedures.
class cu_cp_du_event_handler
{
public:
  virtual ~cu_cp_du_event_handler() = default;

  /// \brief Handle a RRC UE creation notification from the DU processor.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] rrc_ue The interface of the created RRC UE.
  virtual void handle_rrc_ue_creation(cu_cp_ue_index_t ue_index, rrc_ue_interface& rrc_ue) = 0;

  /// \brief Handle a SIB1 request for a given cell.
  /// \param[in] du_index The index of the DU the cell is connected to.
  /// \param[in] cgi The cell global id of the cell.
  /// \returns The packed SIB1 for the cell, if available. An empty byte_buffer otherwise.
  virtual byte_buffer handle_target_cell_sib1_required(cu_cp_du_index_t du_index, nr_cell_global_id_t cgi) = 0;

  /// \brief Handle transaction information loss in the F1AP.
  virtual async_task<void> handle_transaction_info_loss(const ue_transaction_info_loss_event& ev) = 0;
};

/// Interface for an RRC UE entity to communicate with the CU-CP.
class cu_cp_rrc_ue_interface
{
public:
  virtual ~cu_cp_rrc_ue_interface() = default;

  /// \brief Handles a new UE connection with a selected PLMN.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] plmn The selected PLMN of the UE.
  /// \return True if the UE connection is accepted, false otherwise.
  virtual bool handle_ue_plmn_selected(cu_cp_ue_index_t ue_index, const plmn_identity& plmn) = 0;

  /// \brief Handle the reception of an RRC Reestablishment Request by transfering UE Contexts at the RRC.
  /// \param[in] old_pci The old PCI contained in the RRC Reestablishment Request.
  /// \param[in] old_c_rnti The old C-RNTI contained in the RRC Reestablishment Request.
  /// \param[in] ue_index The new UE index of the UE that sent the Reestablishment Request.
  /// \returns The RRC Reestablishment UE context for the old UE.
  virtual rrc_ue_reestablishment_context_response
  handle_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, cu_cp_ue_index_t ue_index) = 0;

  /// \brief Handle a required reestablishment context modification.
  /// \param[in] ue_index The index of the UE that needs the context modification.
  virtual async_task<bool> handle_rrc_reestablishment_context_modification_required(cu_cp_ue_index_t ue_index) = 0;

  /// \brief Handle reestablishment failure by releasing the old UE.
  /// \param[in] request The release request.
  virtual void handle_rrc_reestablishment_failure(const cu_cp_ue_context_release_request& request) = 0;

  /// \brief Handle an successful reestablishment by removing the old UE.
  /// \param[in] ue_index The index of the old UE to remove.
  virtual void handle_rrc_reestablishment_complete(cu_cp_ue_index_t old_ue_index) = 0;

  /// \brief Transfer and remove UE contexts for an ongoing Reestablishment.
  /// \param[in] ue_index The new UE index of the UE that sent the Reestablishment Request.
  /// \param[in] old_ue_index The old UE index of the UE that sent the Reestablishment Request.
  virtual async_task<bool> handle_ue_context_transfer(cu_cp_ue_index_t ue_index, cu_cp_ue_index_t old_ue_index) = 0;

  /// \brief Handle a UE release request.
  /// \param[in] request The release request.
  virtual async_task<void> handle_ue_context_release(const cu_cp_ue_context_release_request& request) = 0;

  /// \brief Handle the reception of an RRC Resume Request.
  /// \param[in] request The resume request.
  /// \returns The RRC Resume Request response.
  virtual async_task<rrc_resume_request_response> handle_rrc_resume_request(const rrc_resume_request& request) = 0;

  /// \brief Initiate RAN paging for a UE in RRC Inactive state.
  virtual void handle_ran_paging_required(cu_cp_ue_index_t ue_index) = 0;
};

// Request with information for the target handler of the intra cu handover.
struct cu_cp_intra_cu_handover_target_request {
  cu_cp_ue_index_t                         target_ue_index = cu_cp_ue_index_t::invalid;
  cu_cp_ue_index_t                         source_ue_index = cu_cp_ue_index_t::invalid;
  uint8_t                                  transaction_id;
  std::chrono::milliseconds                timeout;
  e1ap_bearer_context_modification_request bearer_context_modification_request;
};

/// Request with information for the target handler of conditional handover.
struct cu_cp_cho_target_request {
  cu_cp_ue_index_t          source_ue_index = cu_cp_ue_index_t::invalid;
  cu_cp_ue_index_t          target_ue_index = cu_cp_ue_index_t::invalid;
  cond_recfg_id_t           cond_recfg_id   = cond_recfg_id_t(bounded_integer_invalid_tag{});
  pci_t                     target_pci      = INVALID_PCI;
  unsigned                  transaction_id  = 0;
  std::chrono::milliseconds timeout;

  /// \brief E1AP bearer context modification request for CU-UP tunnel update after CHO completion.
  e1ap_bearer_context_modification_request bearer_context_mod_request;
};

/// Interface for entities (e.g. DU processor) that wish to manipulate the context of a UE.
class cu_cp_ue_context_manipulation_handler
{
public:
  virtual ~cu_cp_ue_context_manipulation_handler() = default;

  /// \brief Handle a UE release request.
  /// \param[in] request The release request.
  virtual async_task<void> handle_ue_context_release(const cu_cp_ue_context_release_request& request) = 0;

  /// \brief Handle an access success indication (UE has accessed a CHO target cell).
  /// \param[in] msg The access success indication.
  virtual async_task<void> handle_access_success(const cu_cp_access_success_indication& msg) = 0;

  /// \brief Handle the trasmission of the handover reconfiguration by notifying the target RRC UE to await a RRC
  /// Reconfiguration Complete.
  /// \param[in] request The intra CU handover target request.
  virtual void handle_handover_reconfiguration_sent(const cu_cp_intra_cu_handover_target_request& request) = 0;

  /// \brief Handle the transmission of the CHO reconfiguration by notifying a CHO target UE to await
  /// RRCReconfigurationComplete.
  ///
  /// This starts the conditional_handover_target_routine on the specified target UE. CHO completion and cleanup
  /// are finalized on the source side (via Access Success handling / conditional_handover_source_routine).
  ///
  /// \param[in] request The CHO target request.
  virtual void handle_cho_reconfiguration_sent(const cu_cp_cho_target_request& request) = 0;

  /// \brief Handle a UE context push during handover.
  /// \param[in] source_ue_index The index of the UE that is the source of the handover.
  /// \param[in] target_ue_index The index of the UE that is the target of the handover.
  virtual void handle_handover_ue_context_push(cu_cp_ue_index_t source_ue_index, cu_cp_ue_index_t target_ue_index) = 0;

  /// \brief Initialize a handover UE release timer. When the timeout is reached, a release request is sent to the AMF.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] handover_ue_release_timeout The timeout for the release.
  /// \param[in] ue_context_release_request The release request.
  virtual void
  initialize_handover_ue_release_timer(cu_cp_ue_index_t                        ue_index,
                                       std::chrono::milliseconds               handover_ue_release_timeout,
                                       const cu_cp_ue_context_release_request& ue_context_release_request) = 0;

  /// \brief Initialize a RNA update timer. When the timeout is reached, a release request is sent to the AMF.
  /// \param[in] ue_index The index of the UE.
  virtual void initialize_rna_update_timer(cu_cp_ue_index_t ue_index) = 0;

  /// \brief Start CHO execution cancellation timer. Fires conditional_handover_cancellation_routine on expiry.
  /// \param[in] source_ue_index Index of the source UE.
  /// \param[in] timeout Duration; 0ms disables the timer (no-op).
  virtual void initialize_cho_execution_timer(cu_cp_ue_index_t source_ue_index, std::chrono::milliseconds timeout) = 0;
};

/// Methods used by CU-CP to transfer the RRC UE context e.g. for RRC Reestablishments.
class cu_cp_rrc_ue_context_transfer_notifier
{
public:
  virtual ~cu_cp_rrc_ue_context_transfer_notifier() = default;

  /// \brief Notifies the RRC UE to return the RRC Reestablishment UE context.
  virtual rrc_ue_reestablishment_context_response on_rrc_ue_context_transfer() = 0;
};

/// Interface to handle measurement requests.
class cu_cp_measurement_handler
{
public:
  virtual ~cu_cp_measurement_handler() = default;

  /// \brief Handle a measurement config request (for any UE) connected to the given serving cell.
  /// \param[in] ue_index The index of the UE to update the measurement config for.
  /// \param[in] nci The cell id of the serving cell to update.
  /// \param[in] current_meas_config The current meas config of the UE (if applicable).
  /// \param[in] cond_meas True if this is a conditional measurement config request (e.g. CHO).
  /// \param[in] candidate_pcis List of candidate target PCIs (when cond_meas is true); if empty, use all neighbors.
  virtual std::optional<rrc_meas_cfg>
  handle_measurement_config_request(cu_cp_ue_index_t                   ue_index,
                                    nr_cell_identity                   nci,
                                    const std::optional<rrc_meas_cfg>& current_meas_config = std::nullopt,
                                    bool                               cond_meas           = false,
                                    span<const pci_t>                  candidate_pcis      = {}) = 0;

  /// \brief Handle a measurement report for given UE.
  virtual void handle_measurement_report(cu_cp_ue_index_t ue_index, const rrc_meas_results& meas_results) = 0;
};

/// Interface to handle measurement config update requests.
class cu_cp_measurement_config_handler
{
public:
  virtual ~cu_cp_measurement_config_handler() = default;

  /// \brief Handle a request to update the measurement related parameters for the given cell id.
  /// \param[in] nci The cell id of the serving cell to update.
  /// \param[in] serv_cell_cfg_ The serving cell meas config to update.
  virtual bool handle_cell_config_update_request(nr_cell_identity                nci,
                                                 const serving_cell_meas_config& serv_cell_cfg) = 0;
};

/// Interface to request handover.
class cu_cp_mobility_manager_handler
{
public:
  virtual ~cu_cp_mobility_manager_handler() = default;

  /// \brief Handle an Intra CU handover.
  virtual async_task<cu_cp_intra_cu_handover_response>
  handle_intra_cu_handover_request(const cu_cp_intra_cu_handover_request& request,
                                   cu_cp_du_index_t&                      source_du_index,
                                   cu_cp_du_index_t&                      target_du_index) = 0;

  /// \brief Handle full intra-CU CHO coordinator request (prepare + execute/cancel).
  virtual async_task<cu_cp_intra_cu_cho_response>
  handle_intra_cu_cho_request(const cu_cp_intra_cu_cho_request& request) = 0;

  /// \brief Handle a intra-cell handover required to refresh KgNB key for the UE
  /// \param[in] ue_index The index of the UE that needs the intra-cell handover
  virtual void handle_intra_cell_handover_required(cu_cp_ue_index_t ue_index) = 0;
};

/// Interface to handle location reporting updates for UEs.
class cu_cp_location_manager_handler
{
public:
  virtual ~cu_cp_location_manager_handler() = default;

  /// \brief Handle a UE location change and send NGAP Location Report if needed.
  /// \param[in] ue_index The index of the UE.
  virtual void handle_location_update(cu_cp_ue_index_t ue_index) = 0;
};

/// Interface to handle ue removals.
class cu_cp_ue_removal_handler
{
public:
  virtual ~cu_cp_ue_removal_handler() = default;

  /// \brief Completly remove a UE from the CU-CP.
  /// \param[in] ue_index The index of the UE to remove.
  virtual async_task<void> handle_ue_removal_request(cu_cp_ue_index_t ue_index) = 0;

  /// \brief Cancel pending UE tasks.
  virtual void handle_pending_ue_task_cancellation(cu_cp_ue_index_t ue_index) = 0;
};

/// Interface to handle AMF reconnections.
class cu_cp_amf_reconnection_handler
{
public:
  virtual ~cu_cp_amf_reconnection_handler() = default;

  /// \brief Handle AMF reconnections.
  /// \param[in] amf_index The index of the AMF that reconnected.
  virtual void handle_amf_reconnection(cu_cp_amf_index_t amf_index) = 0;
};

/// \brief Handler of the XNAP of the CU-CP. This interface is used to forward XNAP messages to the CU-CP.
class cu_cp_xnap_handler : public cu_cp_inter_cu_handover_handler, public cu_cp_task_scheduler_handler
{
public:
  virtual ~cu_cp_xnap_handler() = default;

  /// \brief Handle the received XNAP handover request of the handover preparation procedure.
  /// See TS 38.423 section 8.2.1.
  virtual async_task<cu_cp_handover_resource_allocation_response>
  handle_xnap_handover_request(const xnap_handover_request& request) = 0;

  /// \brief Handle the reception of a Handover Cancel message.
  /// \param[in] ue_index The index of the UE that is the target of the handover cancel.
  virtual void handle_handover_cancel_received(cu_cp_ue_index_t ue_index) = 0;

  /// \brief Handle the reception of a HandoverSuccess message (TS 38.423 section 8.2.4).
  /// Indicates that the source UE has successfully executed CHO to a remote target CU-CP.
  /// \param[in] source_ue_index The source UE index resolved from the XNAP UE ID mapping.
  /// \param[in] winner_peer_xnap_ue_id The target's XNAP UE ID identifying the winning candidate.
  virtual void handle_xnap_handover_success_received(cu_cp_ue_index_t  source_ue_index,
                                                     peer_xnap_ue_id_t winner_peer_xnap_ue_id) = 0;

  /// \brief Handle the reception of an XNAP UE Context Release message.
  /// \param[in] ue_index The index of the UE to be released.
  virtual void handle_xnap_ue_context_release_received(cu_cp_ue_index_t ue_index) = 0;
};

class cu_cp_impl_interface : public cu_cp_e1ap_event_handler,
                             public cu_cp_du_event_handler,
                             public cu_cp_rrc_ue_interface,
                             public cu_cp_measurement_handler,
                             public cu_cp_measurement_config_handler,
                             public cu_cp_ngap_handler,
                             public cu_cp_nrppa_handler,
                             public cu_cp_ue_context_manipulation_handler,
                             public cu_cp_mobility_manager_handler,
                             public cu_cp_location_manager_handler,
                             public cu_cp_ue_removal_handler,
                             public cu_cp_amf_reconnection_handler,
                             public cu_cp_xnap_handler
{
public:
  virtual ~cu_cp_impl_interface() = default;

  virtual cu_cp_e1ap_event_handler&              get_cu_cp_e1ap_handler()               = 0;
  virtual cu_cp_ngap_handler&                    get_cu_cp_ngap_handler()               = 0;
  virtual cu_cp_nrppa_handler&                   get_cu_cp_nrppa_handler()              = 0;
  virtual cu_cp_rrc_ue_interface&                get_cu_cp_rrc_ue_interface()           = 0;
  virtual cu_cp_ue_context_manipulation_handler& get_cu_cp_ue_context_handler()         = 0;
  virtual cu_cp_measurement_handler&             get_cu_cp_measurement_handler()        = 0;
  virtual cu_cp_measurement_config_handler&      get_cu_cp_measurement_config_handler() = 0;
  virtual cu_cp_mobility_manager_handler&        get_cu_cp_mobility_manager_handler()   = 0;
  virtual cu_cp_location_manager_handler&        get_cu_cp_location_manager_handler()   = 0;
  virtual cu_cp_ue_removal_handler&              get_cu_cp_ue_removal_handler()         = 0;
  virtual cu_cp_amf_reconnection_handler&        get_cu_cp_amf_reconnection_handler()   = 0;
  virtual cu_cp_xnap_handler&                    get_cu_cp_xnap_handler()               = 0;
};

} // namespace ocudu::ocucp
