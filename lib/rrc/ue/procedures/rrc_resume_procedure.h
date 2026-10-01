// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../rrc_ue_context.h"
#include "../rrc_ue_logger.h"
#include "rrc_ue_event_manager.h"
#include "ocudu/rrc/rrc_resume.h"
#include "ocudu/rrc/rrc_ue.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/eager_async_task.h"
#include <chrono>

namespace ocudu::ocucp {

class rrc_resume_procedure
{
public:
  rrc_resume_procedure(const asn1::rrc_nr::rrc_resume_request_s& request_,
                       rrc_ue_context_t&                         context_,
                       rnti_t                                    new_c_rnti_,
                       const byte_buffer&                        du_to_cu_container_,
                       rrc_ue_msg4_proc_notifier&                rrc_ue_resume_notifier_,
                       rrc_ue_setup_proc_notifier&               rrc_ue_setup_notifier_,
                       rrc_ue_control_message_handler&           srb_notifier_,
                       rrc_ue_context_update_notifier&           cu_cp_notifier_,
                       rrc_ue_cu_cp_ue_notifier&                 cu_cp_ue_notifier_,
                       rrc_ue_event_notifier&                    metrics_notifier_,
                       rrc_ue_ngap_notifier&                     ngap_notifier_,
                       rrc_ue_event_manager&                     event_mng_,
                       rrc_ue_logger&                            logger_,
                       bool                                      context_retrieval_required_ = false);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RRC Resume Procedure"; }

private:
  /// \brief Build the request for retrieving this UE's context from the peer that allocated its I-RNTI.
  rrc_ue_context_retrieval_request make_context_retrieval_request() const;

  /// \brief Take over the context the peer transferred. The peer verified the ResumeMAC-I and derived the keys for
  /// this cell before answering, so the context is taken over as it arrived.
  /// \return True if the context could be taken over, false otherwise.
  bool transfer_retrieved_context_and_update_keys();

  /// \brief Fall back to establishing a new RRC connection, as TS 38.300 section 9.2.2.4.1 has the node do when the
  /// context cannot be retrieved.
  async_task<void> handle_rrc_resume_fallback();

  /// \brief Get and verify the ResumeMAC-I and update the keys.
  bool verify_and_update_security_context();

  /// \brief Update the security keys.
  void update_security_keys();

  /// \brief Reestablish the SRBs.
  void reestablish_srbs();

  /// \remark Send RRC Resume, see section 5.3.13 in TS 38.331.
  /// \brief Creates at this RRC UE the SRBs the RRCResume adds, other than the SRB1 that carries it.
  void create_retrieved_context_srbs();

  void send_rrc_resume();

  /// \brief Send the pending DL NAS Transport message to the UE after a successful resume.
  void send_pending_dl_nas(byte_buffer& nas_pdu);

  async_task<void> handle_rrc_resume_failure();

  const asn1::rrc_nr::rrc_resume_request_s resume_request;
  rrc_ue_context_t&                        context;
  rnti_t                                   new_c_rnti;
  const byte_buffer&                       du_to_cu_container;
  rrc_ue_msg4_proc_notifier&               rrc_ue_resume_notifier; // handler to the parent RRC UE object
  rrc_ue_setup_proc_notifier&              rrc_ue_setup_notifier;  // handler to the parent RRC UE object
  rrc_ue_control_message_handler&          srb_notifier;           // notifier to the parent RRC UE object
  rrc_ue_context_update_notifier&          cu_cp_notifier;         // notifier to the CU-CP
  rrc_ue_cu_cp_ue_notifier&                cu_cp_ue_notifier;      // notifier to the CU-CP UE
  rrc_ue_event_notifier&                   metrics_notifier;       // metrics notifier
  rrc_ue_ngap_notifier&                    ngap_notifier;          // notifier to the NGAP
  rrc_ue_event_manager&                    event_mng;              // event manager for the RRC UE entity
  rrc_ue_logger&                           logger;
  /// Set when this node holds no context for the resuming UE and has to retrieve it from the peer that allocated the
  /// I-RNTI (TS 38.423 section 8.2.4).
  const bool context_retrieval_required;

  rrc_resume_request request;

  const asn1::rrc_nr::pdcp_cfg_s    srb1_pdcp_cfg;
  std::chrono::milliseconds         procedure_timeout{0};
  rrc_transaction                   transaction;
  eager_async_task<rrc_outcome>     task;
  rrc_resume_request_response       rrc_resume_context;
  cu_cp_ue_context_release_request  ue_context_release_request;
  rrc_ue_context_retrieval_response context_retrieval_response;
  bool                              path_switch_success = false;
};

} // namespace ocudu::ocucp
