// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/f1ap_message_notifier.h"
#include <memory>

namespace ocudu {
namespace odu {

/// Interface used by the DU to establish a new connection with a CU-CP.
class f1c_connection_client
{
public:
  virtual ~f1c_connection_client() = default;

  /// Establish a new connection with a CU-CP.
  ///
  /// \param du_rx_pdu_notifier Notifier that will be used to forward the F1AP PDUs sent by the CU-CP to the DU.
  /// \return Notifier that the DU can use to send F1AP Tx PDUs to the CU-CP it connected to.
  virtual std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) = 0;
};

} // namespace odu
} // namespace ocudu
