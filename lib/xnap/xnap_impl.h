// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ue_context/xnap_ue_context.h"
#include "xnap_context.h"
#include "xnap_tx_pdu_notifier_with_log.h"
#include "ocudu/asn1/xnap/xnap_pdu_contents.h"
#include "ocudu/ran/gnb_id.h"
#include "ocudu/support/async/manual_event.h"
#include "ocudu/xnap/xnap.h"
#include "ocudu/xnap/xnap_configuration.h"
#include "ocudu/xnap/xnap_message.h"
#include <algorithm>

namespace ocudu::ocucp {

class xnap_impl final : public xnap_interface
{
public:
  xnap_impl(xnc_peer_index_t          xnc_index_,
            const xnap_configuration& xnap_cfg_,
            xnap_cu_cp_notifier&      cu_cp_notifier_,
            timer_manager&            timers_,
            task_executor&            ctrl_exec_);
  ~xnap_impl() override = default;

  // XNAP message handling.
  void handle_message(const xnap_message& msg) override;

  async_task<void> stop() override;

  // XNAP ue context removal handler functions.
  void remove_ue_context(cu_cp_ue_index_t ue_index) override;

  // XNAP connection manager functions.
  async_task<bool> handle_xn_setup_request_required() override;
  async_task<bool> handle_served_cells_update_required() override;
  void             set_tx_association_notifier(std::unique_ptr<xnap_message_notifier> tx_notifier_) override
  {
    tx_notifier.connect(std::move(tx_notifier_));
  }
  async_task<xnap_handover_preparation_response>
       handle_handover_request_required(const xnap_handover_request& request) override;
  void handle_cho_cancel_required(cu_cp_ue_index_t ue_index, const nr_cell_global_id_t& target_cgi) override;
  void handle_handover_success_required(cu_cp_ue_index_t ue_index, const nr_cell_global_id_t& cgi) override;
  void handle_sn_status_transfer_required(const cu_cp_status_transfer& sn_status_transfer) override;
  async_task<expected<cu_cp_status_transfer>> handle_sn_status_transfer_expected(cu_cp_ue_index_t ue_index) override;
  bool                                        handle_ue_context_release_required(cu_cp_ue_index_t ue_index) override;
  async_task<xnap_retrieve_ue_context_response>
  handle_retrieve_ue_context_required(const xnap_retrieve_ue_context_request& request) override;

  xnap_ue_context_removal_handler& get_xnap_ue_context_removal_handler() override { return *this; }

  bool has_peer_gnb_id(const gnb_id_t& peer_gnb_id) const override
  {
    return peer_ctxt.has_value() && peer_ctxt->gnb_id == peer_gnb_id;
  }

  bool has_peer_local_node_id(uint32_t node_id, unsigned nof_node_id_bits) const override
  {
    return peer_ctxt.has_value() && (peer_ctxt->gnb_id.id & ((1U << nof_node_id_bits) - 1)) == node_id;
  }

  bool has_peer_pci(pci_t peer_pci) const override
  {
    if (!peer_ctxt.has_value()) {
      return false;
    }
    return std::any_of(peer_ctxt->list_of_served_cells_nr.begin(),
                       peer_ctxt->list_of_served_cells_nr.end(),
                       [peer_pci](const cu_cp_served_cell_info& cell) { return cell.nr_pci == peer_pci; });
  }

private:
  /// \brief Notify about the reception of an initiating message.
  /// \param[in] msg The received initiating message.
  void handle_initiating_message(const asn1::xnap::init_msg_s& msg);

  /// \brief Notify about the reception of an XN Setup Request message.
  /// \param[in] request The received XN Setup Request message.
  void handle_xn_setup_request(const asn1::xnap::xn_setup_request_s& request);

  /// \brief Notify about the reception of an NG-RAN Node Configuration Update message.
  /// \param[in] msg The received NG-RAN Node Configuration Update message.
  void handle_ngran_node_cfg_update(const asn1::xnap::ngran_node_cfg_upd_s& msg);

  /// \brief Notify about the reception of a Handover Request message.
  /// \param[in] msg The received handover request message.
  void handle_handover_request(const asn1::xnap::ho_request_s& msg);

  /// \brief Notify about the reception of a Handover Cancel message.
  /// \param[in] msg The received handover cancel message.
  void handle_handover_cancel(const asn1::xnap::ho_cancel_s& msg);

  /// \brief Notify about the reception of a SN Status Transfer message.
  /// \param[in] msg The received SN Status Transfer message.
  void handle_sn_status_transfer(const asn1::xnap::sn_status_transfer_s& msg);

  /// \brief Notify about the reception of a UE Context Release message.
  /// \param[in] msg The received UE Context Release message.
  void handle_ue_context_release(const asn1::xnap::ue_context_release_s& msg);

  /// \brief Handle incoming HandoverSuccess (target notifies source that UE arrived via CHO).
  void handle_handover_success(const asn1::xnap::ho_success_s& msg);

  /// \brief Handle incoming ConditionalHandoverCancel (source cancels CHO resources at target).
  void handle_conditional_ho_cancel(const asn1::xnap::conditional_ho_cancel_s& msg);

  /// \brief Notify about the reception of a Retrieve UE Context Request message.
  /// \param[in] msg The received Retrieve UE Context Request message.
  void handle_retrieve_ue_context_request(const asn1::xnap::retrieve_ue_context_request_s& msg);

  /// \brief Resolve the configuration of a cell served by the XN-C peer.
  /// \param[in] nci Identity of the served cell.
  /// \return The served cell configuration, or std::nullopt if the peer did not advertise the cell at XN setup.
  std::optional<cu_cp_served_cell_info> find_peer_served_cell(nr_cell_identity nci) const;

  /// \brief Notify about the reception of a successful outcome message.
  /// \param[in] outcome The successful outcome message.
  void handle_successful_outcome(const asn1::xnap::successful_outcome_s& outcome);

  /// \brief Notify about the reception of an unsuccessful outcome message.
  /// \param[in] outcome The unsuccessful outcome message.
  void handle_unsuccessful_outcome(const asn1::xnap::unsuccessful_outcome_s& outcome);

  /// \brief Wraps a UE-associated procedure so that \ref stop can await its completion.
  ///
  /// A procedure suspended on a CU-CP notifier is not resumed by cancelling the XNAP transactions, so without
  /// this it can outlive the XNAP instance and access its members after they are destroyed.
  template <typename Result>
  async_task<Result> track_ue_procedure(async_task<Result> proc);
  async_task<void>   track_ue_procedure(async_task<void> proc);

  ocudulog::basic_logger& logger;

  /// Repository of UE Contexts.
  xnap_ue_context_list ue_ctxt_list;

  const xnc_peer_index_t xnc_index;
  xnap_configuration     xnap_cfg;
  /// NR cells this node advertised to the XN-C peer.
  std::vector<cu_cp_served_cell_info> advertised_cells;
  std::optional<xnap_context>         peer_ctxt;
  xnap_cu_cp_notifier&                cu_cp_notifier;
  timer_manager&                      timers;
  task_executor&                      ctrl_exec;

  xnap_tx_pdu_notifier_with_logging tx_notifier;

  /// XN Setup Response/Failure Event Source.
  protocol_transaction_event_source<asn1::xnap::xn_setup_resp_s, asn1::xnap::xn_setup_fail_s> xn_setup_outcome;

  /// NG-RAN Node Configuration Update Acknowledge/Failure Event Source.
  protocol_transaction_event_source<asn1::xnap::ngran_node_cfg_upd_ack_s, asn1::xnap::ngran_node_cfg_upd_fail_s>
      cfg_update_outcome;

  /// Number of UE-associated procedures that have not completed yet.
  unsigned nof_ue_procedures = 0;

  /// Set whenever the number of UE-associated procedures in flight drops to zero.
  manual_event_flag ue_procedures_done;
};

} // namespace ocudu::ocucp
