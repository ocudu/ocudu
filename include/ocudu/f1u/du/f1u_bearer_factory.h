// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/du/f1u_bearer.h"
#include "ocudu/f1u/du/f1u_config.h"
#include "ocudu/f1u/du/f1u_rx_sdu_notifier.h"
#include "ocudu/f1u/du/f1u_tx_pdu_notifier.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/ran/up_transport_layer_info.h"
#include "ocudu/support/timers.h"

namespace ocudu {
namespace odu {

struct f1u_bearer_creation_message {
  uint32_t                 ue_index;
  drb_id_t                 drb_id;
  up_transport_layer_info  dl_tnl_info;
  f1u_config               config;
  f1u_rx_sdu_notifier*     rx_sdu_notifier;
  f1u_tx_pdu_notifier*     tx_pdu_notifier;
  timer_factory            timers;
  task_executor*           ue_executor;
  f1u_bearer_disconnector* disconnector;
};

/// \brief Creates an F1-U bearer for the DU.
std::unique_ptr<f1u_bearer> create_f1u_bearer(const f1u_bearer_creation_message& msg);

} // namespace odu
} // namespace ocudu
