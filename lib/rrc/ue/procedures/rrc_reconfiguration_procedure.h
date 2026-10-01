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

/// \brief Modifies the RRC connection of a UE in RRC connected mode. It's used to establishe bearers, etc.
/// TODO Add seqdiag
class rrc_reconfiguration_procedure
{
public:
  rrc_reconfiguration_procedure(rrc_ue_context_t&                            context_,
                                const rrc_reconfiguration_procedure_request& args_,
                                rrc_ue_reconfiguration_proc_notifier&        rrc_ue_notifier_,
                                rrc_ue_event_manager&                        event_mng_,
                                rrc_ue_control_message_handler&              srb_notifier_,
                                rrc_ue_logger&                               logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "RRC Reconfiguration Procedure"; }

private:
  /// \remark Send RRC Reconfiguration, see section 5.3.5 in TS 38.331
  void send_rrc_reconfiguration();

  rrc_ue_context_t&                           context;
  const rrc_reconfiguration_procedure_request args;

  rrc_ue_reconfiguration_proc_notifier& rrc_ue;       // handler to the parent RRC UE object
  rrc_ue_event_manager&                 event_mng;    // event manager for the RRC UE entity
  rrc_ue_control_message_handler&       srb_notifier; // For creating SRBs
  rrc_ue_logger&                        logger;

  std::chrono::milliseconds     procedure_timeout{0};
  rrc_transaction               transaction;
  eager_async_task<rrc_outcome> task;

  bool procedure_result = false;
};

} // namespace ocucp
} // namespace ocudu
