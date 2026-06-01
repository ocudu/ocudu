// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/common/e1ap_common.h"
#include <memory>

namespace ocudu::ocucp {

/// \brief Handler of the E1 interface of the CU-CP.
///
/// This interface is used to forward E1AP messages or CU-UP connection updates to the CU-CP.
class cu_cp_e1_handler
{
public:
  virtual ~cu_cp_e1_handler() = default;

  /// \brief Handles the start of a new CU-UP connection.
  ///
  /// \param e1ap_tx_pdu_notifier Notifier that the CU-CP will use to push E1AP Tx messages to the CU-UP. Once this
  /// object goes out of scope, the CU-UP connection will be closed.
  /// \return Notifier that the E1AP will use to forward to the CU-CP received E1AP messages from the CU-UP via E1AP. If
  /// the caller lets the returned object go out of scope, the CU-UP connection will be closed.
  virtual std::unique_ptr<e1ap_message_notifier>
  handle_new_cu_up_connection(std::unique_ptr<e1ap_message_notifier> e1ap_tx_pdu_notifier) = 0;
};

} // namespace ocudu::ocucp
