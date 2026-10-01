// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../rrc_ue_context.h"
#include "../rrc_ue_logger.h"
#include "rrc_ue_event_manager.h"
#include "ocudu/asn1/rrc_nr/rrc_nr.h"
#include "ocudu/rrc/rrc_ue.h"
#include "ocudu/support/async/async_task.h"
#include <chrono>

namespace ocudu::ocucp {

/// \brief Requests the coarse UE location from the UE, TS 38.300 sec. 16.14.8.
///
/// Runs the UEInformationRequest/UEInformationResponse exchange of TS 38.331 sec. 5.7.10 with coarseLocationRequest
/// set. The response carries coarseLocationInfo only "if available", so the outcome is a position or nothing.
///
/// The caller decides whether the serving cell is one whose location is worth asking for, and runs this only once AS
/// security is established, which is what sec. 16.14.8 allows the request from.
class rrc_ue_information_procedure
{
public:
  rrc_ue_information_procedure(rrc_ue_context_t&                           context_,
                               rrc_ue_security_mode_command_proc_notifier& rrc_ue_notifier_,
                               rrc_ue_context_update_notifier&             cu_cp_notifier_,
                               rrc_ue_cu_cp_ue_notifier&                   cu_cp_ue_notifier_,
                               rrc_ue_event_manager&                       event_mng_,
                               rrc_ue_logger&                              logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RRC UE Information Procedure"; }

private:
  /// \remark Sends UEInformationRequest, see section 5.7.10.2 in TS 38.331.
  void send_rrc_ue_information_request();

  /// Decodes coarseLocationInfo from the response and stores it in the UE context.
  void store_coarse_location(const asn1::rrc_nr::ue_info_resp_r16_s& ue_info_resp);

  rrc_ue_context_t& context;

  rrc_ue_security_mode_command_proc_notifier& rrc_ue;            // handler to the parent RRC UE object
  rrc_ue_context_update_notifier&             cu_cp_notifier;    // handler to the CU-CP
  rrc_ue_cu_cp_ue_notifier&                   cu_cp_ue_notifier; // handler to the CU-CP UE
  rrc_ue_event_manager&                       event_mng;         // event manager for the RRC UE entity
  rrc_ue_logger&                              logger;

  std::chrono::milliseconds procedure_timeout{0};
  rrc_transaction           transaction;
};

} // namespace ocudu::ocucp
