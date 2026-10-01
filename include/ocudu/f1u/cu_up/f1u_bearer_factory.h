// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/cu_up/f1u_bearer.h"
#include "ocudu/f1u/cu_up/f1u_config.h"
#include "ocudu/f1u/cu_up/f1u_rx_delivery_notifier.h"
#include "ocudu/f1u/cu_up/f1u_rx_sdu_notifier.h"
#include "ocudu/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/support/timers.h"

namespace ocudu::ocuup {

/// \brief Creates an F1-U bearer for the CU-UP.
std::unique_ptr<f1u_bearer> create_f1u_bearer(uint32_t                       ue_index,
                                              drb_id_t                       drb_id,
                                              const up_transport_layer_info& ul_up_tnl_info,
                                              const f1u_config&              config,
                                              f1u_tx_pdu_notifier&           tx_pdu_notifier,
                                              f1u_rx_delivery_notifier&      rx_delivery_notifier,
                                              f1u_rx_sdu_notifier&           rx_sdu_notifier,
                                              timer_factory                  ue_dl_timer_factory,
                                              timer_factory                  ue_ctrl_timer_factory,
                                              unique_timer&                  ue_inactivity_timer,
                                              task_executor&                 dl_exec,
                                              task_executor&                 ul_exec,
                                              task_executor&                 ctrl_exec);

} // namespace ocudu::ocuup
