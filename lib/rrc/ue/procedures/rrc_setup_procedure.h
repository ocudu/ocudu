// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../rrc_ue_context.h"
#include "../rrc_ue_logger.h"
#include "rrc_ue_event_manager.h"
#include "ocudu/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "ocudu/ran/plmn_identity.h"
#include "ocudu/rrc/rrc_du.h"
#include "ocudu/rrc/rrc_ue.h"
#include "ocudu/support/async/async_task.h"
#include "ocudu/support/async/eager_async_task.h"
#include <chrono>

namespace ocudu::ocucp {

/// \brief Handles the creation of SRBs in the RRC UE.
///  \startuml
///    queue DU
///    participant F1AP
///    participant PDCP
///    participant RRC
///    participant "DU processor" as DUMNG
///    DU->F1AP: initial UL RRC message
///    activate F1AP
///    F1AP->DUMNG: ue_creation_request
///    activate DUMNG
///    Note over DUMNG: Allocate UE index
///    Note over DUMNG: Create UE object
///    Note over DUMNG: Create SRB0 notifiers
///    DUMNG->RRC: create_ue (with SRB0 handle)
///    DUMNG-->F1AP: return of "ue_creation_request" (with UE index and SRB0 handle)
///    deactivate DUMNG
///    Note over F1AP: Allocate F1AP-CU-UE-ID
///    Note over F1AP: Association of UE index\nwith F1AP-CU-UE-ID
///    F1AP->RRC: notify_init_ul_rrc_msg
///    deactivate F1AP
///    Note over RRC: Decide RRC Setup vs Reject
///    activate RRC
///    Note over RRC: Create SRB1 (SRB1)
///    Note over RRC: Initiate RRCSetup procedure
///    RRC->F1AP: send_rrc_setup
///    F1AP->DU: DL RRC Message Transfer
///    DU->F1AP: UL RRC Message Transfer
///    F1AP->RRC: rrc_setup_complete
///    deactivate RRC
///    Note over RRC: Decide next step
///  \enduml
class rrc_setup_procedure
{
public:
  rrc_setup_procedure(rrc_ue_context_t&               context_,
                      const byte_buffer&              du_to_cu_container_,
                      rrc_ue_setup_proc_notifier&     rrc_ue_notifier_,
                      rrc_ue_control_message_handler& srb_notifier_,
                      rrc_ue_context_update_notifier& cu_cp_notifier_,
                      rrc_ue_event_notifier&          metrics_notifier_,
                      rrc_ue_ngap_notifier&           ngap_notifier_,
                      rrc_ue_event_manager&           event_mng_,
                      rrc_ue_logger&                  logger_,
                      bool                            is_reestablishment_fallback_ = false,
                      bool                            is_resume_fallback_          = false);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RRC Setup Procedure"; }

private:
  /// Instruct DU processor to create SRB1 bearer.
  void create_srb1();

  /// \remark Send RRC Setup, see section 5.3.3 in TS 36.331.
  void send_rrc_setup();

  /// \remark Forward the Initial UE Message to the NGAP
  void send_initial_ue_msg();

  static resume_cause_t establishment_cause_to_resume_cause(const establishment_cause_t& establishment_cause);

  rrc_ue_context_t&  context;
  const byte_buffer& du_to_cu_container;

  rrc_ue_setup_proc_notifier&     rrc_ue;           // handler to the parent RRC UE object
  rrc_ue_control_message_handler& srb_notifier;     // for creation of SRBs
  rrc_ue_context_update_notifier& cu_cp_notifier;   // notifier to the CU-CP
  rrc_ue_event_notifier&          metrics_notifier; // notifier to the metrics
  rrc_ue_ngap_notifier&           ngap_notifier;    // notifier to the NGAP
  rrc_ue_event_manager&           event_mng;        // event manager for the RRC UE entity
  bool                            is_reestablishment_fallback = false;
  bool                            is_resume_fallback          = false;
  rrc_ue_logger&                  logger;

  std::chrono::milliseconds procedure_timeout{0};
  /// Time at which the procedure was created, i.e. when the RRC UE received the request that starts the RRC setup.
  std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
  rrc_transaction                       transaction;
  eager_async_task<rrc_outcome>         task;

  asn1::rrc_nr::rrc_setup_complete_s rrc_setup_complete_msg;
  uint8_t                            sel_plmn_id   = 0;
  plmn_identity                      selected_plmn = plmn_identity::test_value();
};

} // namespace ocudu::ocucp
