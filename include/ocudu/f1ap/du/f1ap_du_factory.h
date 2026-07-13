// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/du/f1ap_du.h"
#include <memory>

namespace ocudu {

class timer_manager;

namespace odu {

class f1ap_ue_executor_mapper;
class f1c_connection_client;

/// Creates an instance of an F1AP DU interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1ap_du> create_f1ap(f1c_connection_client&   f1c_client_handler,
                                     f1ap_du_configurator&    du_mng,
                                     task_executor&           ctrl_exec,
                                     f1ap_ue_executor_mapper& ue_exec_mapper,
                                     f1ap_du_paging_notifier& paging_notifier,
                                     f1ap_du_pws_notifier&    pws_notifier,
                                     timer_manager&           timers);

} // namespace odu
} // namespace ocudu
