// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../rrc_ue_context.h"
#include "../rrc_ue_logger.h"
#include "rrc_ue_event_manager.h"
#include "ocudu/rrc/rrc_ue.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/eager_async_task.h"
#include <chrono>

namespace ocudu {
namespace ocucp {

class rrc_reestablishment_procedure
{
public:
  rrc_reestablishment_procedure(const asn1::rrc_nr::rrc_reest_request_s& request_,
                                rrc_ue_context_t&                        context_,
                                const byte_buffer&                       du_to_cu_container_,
                                rrc_ue_setup_proc_notifier&              rrc_ue_setup_notifier_,
                                rrc_ue_msg4_proc_notifier&               rrc_ue_reest_notifier_,
                                rrc_ue_control_message_handler&          srb_notifier_,
                                rrc_ue_context_update_notifier&          cu_cp_notifier_,
                                rrc_ue_cu_cp_ue_notifier&                cu_cp_ue_notifier_,
                                rrc_ue_event_notifier&                   metrics_notifier_,
                                rrc_ue_ngap_notifier&                    ngap_notifier_,
                                rrc_ue_event_manager&                    event_mng_,
                                rrc_ue_logger&                           logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RRC Reestablishment Procedure"; }

private:
  /// \brief Outcome of the checks against the local UE contexts.
  enum class local_reestablishment_outcome {
    /// A local old UE context matched and passed all checks.
    accepted,
    /// No local UE context matches the reestablishment identity. The UE may have come from a peer NG-RAN node, so the
    /// context can still be retrieved over Xn.
    context_not_found,
    /// A local context matched but the reestablishment cannot proceed. Retrying over Xn would be pointless.
    rejected
  };

  /// \brief Determine whether the Reestablishment Request is accepted or rejected based on the local UE contexts.
  local_reestablishment_outcome is_local_reestablishment_accepted();

  /// \brief Get and verify the ShortMAC-I and update the keys.
  bool verify_security_context();

  /// \brief Whether the UE context was retrieved from a peer NG-RAN node.
  bool is_context_retrieved() const { return context_retrieval_response.success; }

  /// \brief Build the request to retrieve the UE context from the peer serving the failure cell.
  rrc_ue_context_retrieval_request make_context_retrieval_request() const;

  /// \brief Transfer the reestablishment context e.g. ue capabilities and update the security keys
  void transfer_reestablishment_context_and_update_keys();

  /// \brief Transfer the context retrieved from the peer and set up the security keys. The peer derived KgNB* for
  /// this node's cell before answering (TS 33.501 section 6.11), so that key is taken as it is.
  bool transfer_retrieved_context_and_update_keys();

  /// \brief Instruct DU processor to create SRB1 bearer.
  void create_srb1();

  /// \remark Send RRC Reestablishment, see section 5.3.7 in TS 36.331.
  void send_rrc_reestablishment();

  /// \brief Enable ciphering for SRB1.
  void enable_srb1_ciphering();

  async_task<void> handle_rrc_reestablishment_fallback();

  void log_rejected_reestablishment(const char* cause_str);

  const asn1::rrc_nr::rrc_reest_request_s reestablishment_request;
  rrc_ue_context_t&                       context;
  const byte_buffer&                      du_to_cu_container;
  rrc_ue_setup_proc_notifier&             rrc_ue_setup_notifier;
  rrc_ue_msg4_proc_notifier&              rrc_ue_reest_notifier; // handler to the parent RRC UE object
  rrc_ue_control_message_handler&         srb_notifier;          // for creating SRBs
  rrc_ue_context_update_notifier&         cu_cp_notifier;        // notifier to the CU-CP
  rrc_ue_cu_cp_ue_notifier&               cu_cp_ue_notifier;     // notifier to the CU-CP UE
  rrc_ue_event_notifier&                  metrics_notifier;      // metrics notifier
  rrc_ue_ngap_notifier&                   ngap_notifier;         // notifier to the NGAP
  rrc_ue_event_manager&                   event_mng;             // event manager for the RRC UE entity
  rrc_ue_logger&                          logger;

  const asn1::rrc_nr::pdcp_cfg_s          srb1_pdcp_cfg;
  std::chrono::milliseconds               procedure_timeout{0};
  rrc_transaction                         transaction;
  eager_async_task<rrc_outcome>           task;
  rrc_ue_reestablishment_context_response old_ue_reest_context;
  /// Context retrieved from a peer NG-RAN node; only set when no local old UE context matched.
  rrc_ue_context_retrieval_response context_retrieval_response;
  local_reestablishment_outcome     local_outcome                = local_reestablishment_outcome::rejected;
  bool                              context_transfer_success     = false;
  bool                              context_modification_success = false;
  cu_cp_ue_context_release_request  ue_context_release_request;
};

} // namespace ocucp
} // namespace ocudu
