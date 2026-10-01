// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e2/e2.h"
#include <memory>

namespace ocudu {

/// Interface used by the E2 agent to establish a new connection with a ORAN Near Real-Time RIC.
class e2_connection_client
{
public:
  virtual ~e2_connection_client() = default;

  /// Establish a new connection with a Near Real-Time RIC.
  ///
  /// \param e2ap_rx_pdu_notifier Notifier that will be used to forward the E2AP PDUs sent by the RIC to the E2 Agent.
  /// \return Notifier that the E2 Agent can use to send E2AP Tx PDUs to the RIC it connected to.
  virtual std::unique_ptr<e2_message_notifier>
  handle_e2_connection_request(std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier) = 0;
};

} // namespace ocudu
