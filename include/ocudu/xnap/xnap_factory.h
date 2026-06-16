// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/support/timers.h"
#include "ocudu/xnap/xnap.h"
#include "ocudu/xnap/xnap_configuration.h"
#include <memory>

namespace ocudu::ocucp {

/// Creates an instance of an XNAP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<xnap_interface> create_xnap(xnc_peer_index_t          xnc_index,
                                            const xnap_configuration& xnap_cfg_,
                                            xnap_cu_cp_notifier&      cu_cp_notifier_,
                                            timer_manager&            timers_,
                                            task_executor&            ctrl_exec_);

} // namespace ocudu::ocucp
