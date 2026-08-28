// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/common/e1ap_common.h"
#include <memory>

namespace ocudu::ocuup {

/// Interface used by the CU-UP to establish a new connection with a CU-CP via the E1 interface.
class e1_connection_client
{
public:
  virtual ~e1_connection_client() = default;

  /// Establish a new connection with a CU-CP.
  ///
  /// \param cu_up_rx_pdu_notifier Notifier that will be used to forward the E1AP PDUs sent by the CU-CP to the CU-UP.
  /// \return Notifier that the CU-UP can use to send E1AP Tx PDUs to the CU-CP it connected to.
  virtual std::unique_ptr<e1ap_message_notifier>
  handle_cu_up_connection_request(std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier) = 0;
};

} // namespace ocudu::ocuup
