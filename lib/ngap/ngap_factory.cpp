// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ngap/ngap_factory.h"
#include "ngap_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;
using namespace ocucp;

std::unique_ptr<ngap_interface> ocudu::ocucp::create_ngap(const ngap_configuration& ngap_cfg_,
                                                          ngap_cu_cp_notifier&      cu_cp_notifier_,
                                                          n2_connection_client&     n2_gateway_handler_,
                                                          timer_manager&            timers_,
                                                          task_executor&            ctrl_exec_)
{
  auto ngap = std::make_unique<ngap_impl>(ngap_cfg_, cu_cp_notifier_, n2_gateway_handler_, timers_, ctrl_exec_);
  return ngap;
}
