// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "xnap_message_notifier.h"
#include "ocudu/ran/cu_cp_cell_configuration.h"
#include "ocudu/ran/cu_cp_types.h"
#include "ocudu/ran/gnb_id.h"
#include "ocudu/ran/inter_cu_handover_messages.h"
#include "ocudu/ran/pci.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/xnap/xnap_handover.h"
#include "ocudu/xnap/xnap_ue_context_retrieval.h"

namespace ocudu::ocucp {

struct xnap_message;

/// This interface is used to push XNAP messages to the XNAP interface.
class xnap_message_handler
{
public:
  virtual ~xnap_message_handler() = default;

  /// Handle the incoming XNAP message.
  virtual void handle_message(const xnap_message& msg) = 0;
};

/// XNAP interface for CU-CP to initiate the XN interface.
class xnap_connection_manager
{
public:
  virtual ~xnap_connection_manager() = default;

  /// \brief Trigger the initiation of the XN setup procedure.
  /// \returns true if the procedure completed successfully, false otherwise.
  virtual async_task<bool> handle_xn_setup_request_required() = 0;

  /// \brief Trigger the report of the cells this node serves to the XN-C peer (TS 38.423 section 8.4.1).
  /// \returns true if the XN-C peer acknowledged the reported cells, false otherwise.
  virtual async_task<bool> handle_served_cells_update_required() = 0;

  /// \brief Provide the SCTP association notifier after the SCTP association establishment.
  /// \param[in] tx_notifier_ The SCTP association notifier.
  virtual void set_tx_association_notifier(std::unique_ptr<xnap_message_notifier> tx_notifier_) = 0;
};

/// Handle UE context removal.
class xnap_ue_context_removal_handler
{
public:
  virtual ~xnap_ue_context_removal_handler() = default;

  /// \brief Remove the context of an UE.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void remove_ue_context(cu_cp_ue_index_t ue_index) = 0;
};

class xnap_control_message_handler
{
public:
  virtual ~xnap_control_message_handler() = default;

  /// \brief Initiates a Handover Preparation procedure as defined in TS 38.423 section 8.2.1.
  virtual async_task<xnap_handover_preparation_response>
  handle_handover_request_required(const xnap_handover_request& request) = 0;

  /// \brief Sends HandoverCancel to a non-winning target CU-CP (TS 38.423 section 8.2.3).
  /// \param[in] ue_index    Source UE index used to look up the local XNAP UE context.
  /// \param[in] target_cgi  Candidate cell CGI, placed in the Candidate Cells To Be Cancelled List IE.
  virtual void handle_cho_cancel_required(cu_cp_ue_index_t ue_index, const nr_cell_global_id_t& target_cgi) = 0;

  /// \brief Sends HandoverSuccess to the source CU-CP (TS 38.423 section 8.2.4).
  /// Called by the target after RRCReconfigurationComplete is received from the UE.
  virtual void handle_handover_success_required(cu_cp_ue_index_t ue_index, const nr_cell_global_id_t& cgi) = 0;

  /// \brief Initiate the transmission of a SN Status Transfer message as defined in TS 38.423 section 8.2.2.
  virtual void handle_sn_status_transfer_required(const cu_cp_status_transfer& sn_status_transfer) = 0;

  /// \brief Prepares the reception of a SN status transfer message.
  virtual async_task<expected<cu_cp_status_transfer>> handle_sn_status_transfer_expected(cu_cp_ue_index_t ue_index) = 0;

  /// \brief Initiate the transmission of a UE Context Release message as defined in TS 38.423 section 8.2.7.
  virtual bool handle_ue_context_release_required(cu_cp_ue_index_t ue_index) = 0;

  /// \brief Initiates a Retrieve UE Context procedure as defined in TS 38.423 section 8.2.4, to fetch the context of a
  /// UE that arrived at this node from the peer that still holds it.
  virtual async_task<xnap_retrieve_ue_context_response>
  handle_retrieve_ue_context_required(const xnap_retrieve_ue_context_request& request) = 0;
};

/// This interface for the CU-CP to stop an XNAP instance.
class xnap_controller
{
public:
  virtual ~xnap_controller()      = default;
  virtual async_task<void> stop() = 0;
};

/// XNAP notifier to the CU-CP.
class xnap_cu_cp_notifier
{
public:
  virtual ~xnap_cu_cp_notifier() = default;

  /// \brief Notify about the reception of a new RRC Handover Command (TS 38.331 section 11.2.2).
  /// \param[in] command The Handover Command, including the data forwarding tunnels of the target.
  /// \returns True if the Handover command is valid and was successfully handled by the DU.
  virtual async_task<bool> on_new_rrc_handover_command(cu_cp_rrc_handover_command command) = 0;

  /// \brief Request UE index allocation on the CU-CP on XNAP handover request.
  virtual cu_cp_ue_index_t request_new_ue_index_allocation(const nr_cell_global_id_t& cgi,
                                                           const plmn_identity&       plmn) = 0;

  /// \brief Notify the CU-CP about a handover request received.
  /// \param[in] ue_index Index of the UE.
  /// \param[in] selected_plmn The selected PLMN identity of the UE.
  /// \param[in] sec_ctxt The received security context.
  /// \return True if the handover request handling is successful, false otherwise.
  virtual bool on_handover_request_received(cu_cp_ue_index_t                  ue_index,
                                            const plmn_identity&              selected_plmn,
                                            const security::security_context& sec_ctxt) = 0;

  /// \brief Request scheduling a task for a UE.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] task The task to schedule.
  /// \returns True if the task was successfully scheduled, false otherwise.
  virtual bool schedule_async_task(cu_cp_ue_index_t ue_index, async_task<void> task) = 0;

  /// \brief Notifies the CU-CP about a Handover Request.
  virtual async_task<cu_cp_handover_resource_allocation_response>
  on_xnap_handover_request(const xnap_handover_request& request) = 0;

  /// \brief Notify the CU-CP to await the RRC Reconfiguration Complete and the DL Status Transfer.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] xnap_ho_target_execution_ctxt The information required for the XNAP target handover execution.
  virtual void
  on_xn_handover_execution(cu_cp_ue_index_t                              ue_index,
                           const xnap_handover_target_execution_context& xnap_ho_target_execution_ctxt) = 0;

  /// \brief Notify the CU-CP about the reception of a Handover Cancel message.
  /// \param[in] ue_index The index of the UE.
  virtual void on_handover_cancel_received(cu_cp_ue_index_t ue_index) = 0;

  /// \brief Notify the CU-CP about the reception of a HandoverSuccess message (TS 38.423 section 8.2.8).
  /// Signals that the UE has arrived at the target CU-CP via CHO execution.
  /// The source CU-CP must send SN Status Transfer and release the source UE context.
  /// \param[in] source_ue_index The source UE index derived from the XNAP UE ID mapping.
  /// \param[in] winner_peer_xnap_ue_id The target's XNAP UE ID (used to identify the winning candidate).
  virtual void on_handover_success_received(cu_cp_ue_index_t  source_ue_index,
                                            peer_xnap_ue_id_t winner_peer_xnap_ue_id) = 0;

  /// \brief Notify the CU-CP about the reception of a UE Context Release message.
  /// \param[in] ue_index The index of the UE.
  virtual void on_ue_context_release_received(cu_cp_ue_index_t ue_index) = 0;

  /// \brief Request the NR cells this node serves, to advertise them to the XN-C peer.
  /// \returns The cells served by the connected DUs.
  virtual std::vector<cu_cp_served_cell_info> on_served_cells_required() = 0;

  /// \brief Resolve the UE the peer identified in a Retrieve UE Context Request (TS 38.423 section 8.2.4).
  /// \param[in] ue_context_id The UE Context ID received from the peer.
  /// \returns The index of the local UE holding the context, or cu_cp_ue_index_t::invalid if the UE Context ID does
  /// not match any local UE.
  virtual cu_cp_ue_index_t on_xnap_ue_context_id_lookup(const xnap_ue_context_id& ue_context_id) = 0;

  /// \brief Notify the CU-CP about the reception of a Retrieve UE Context Request (TS 38.423 section 8.2.4).
  /// The CU-CP verifies the MAC-I, derives KgNB* for the target cell and collects the UE context to transfer.
  /// \param[in] request The received request, with the UE index already resolved.
  /// \returns The UE context to transfer, or a failed response carrying the rejection cause.
  virtual async_task<xnap_retrieve_ue_context_response>
  on_xnap_retrieve_ue_context_request(const xnap_retrieve_ue_context_request& request) = 0;
};

/// Combined entry point for the XNAP object.
class xnap_interface : public xnap_message_handler,
                       public xnap_connection_manager,
                       public xnap_ue_context_removal_handler,
                       public xnap_control_message_handler,
                       public xnap_controller
{
public:
  virtual ~xnap_interface() = default;

  virtual xnap_ue_context_removal_handler& get_xnap_ue_context_removal_handler() = 0;

  /// \brief Check if the connected XN-C peer has the given GNB ID.
  virtual bool has_peer_gnb_id(const gnb_id_t& peer_gnb_id) const = 0;

  /// \brief Check if the gNB ID of the connected XN-C peer carries the given Local NG-RAN Node Identifier.
  /// \param[in] node_id Local NG-RAN Node Identifier read out of an I-RNTI.
  /// \param[in] nof_node_id_bits Width the I-RNTI profile gives the identifier.
  virtual bool has_peer_local_node_id(uint32_t node_id, unsigned nof_node_id_bits) const = 0;

  /// \brief Check if the connected XN-C peer serves a NR cell with the given PCI.
  /// \remark The served cell list is only known once the XN setup procedure has completed with the peer.
  virtual bool has_peer_pci(pci_t peer_pci) const = 0;
};

} // namespace ocudu::ocucp
