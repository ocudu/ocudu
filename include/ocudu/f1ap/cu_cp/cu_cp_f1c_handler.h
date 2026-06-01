// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1ap/f1ap_message_notifier.h"
#include <memory>

namespace ocudu::ocucp {

/// \brief Handler of the F1-C interface of the CU-CP.
///
/// This interface is used to forward F1AP messages or DU connection updates to the CU-CP.
class cu_cp_f1c_handler
{
public:
  virtual ~cu_cp_f1c_handler() = default;

  /// \brief Handles the establishment of a new DU-to-CU-CP TNL association.
  ///
  /// \param f1ap_tx_pdu_notifier Notifier that the CU-CP will use to push F1AP Tx PDUs to the F1-C GW. Once this
  /// notifier instance goes out of scope, the F1-C GW will be notified that the CU-CP wants to shutdown the connection.
  /// \return Notifier that the F1-C GW will use to forward F1AP PDUs to the CU-CP. If the caller lets the returned
  /// object go out of scope, the CU-CP will be notified that a GW event occurred that resulted in the association
  /// being shutdown.
  virtual std::unique_ptr<f1ap_message_notifier>
  handle_new_du_connection(std::unique_ptr<f1ap_message_notifier> f1ap_tx_pdu_notifier) = 0;
};

} // namespace ocudu::ocucp
