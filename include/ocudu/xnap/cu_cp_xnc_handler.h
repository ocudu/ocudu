// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gateways/sctp_network_server.h"
#include "ocudu/xnap/xnap_message_notifier.h"
#include <memory>

namespace ocudu::ocucp {

/// \brief Handler of the Xn-C interface of the CU-CP.
///
/// This interface is used to forward XNAP messages or connection updates to the CU-CP.
class cu_cp_xnc_handler
{
public:
  virtual ~cu_cp_xnc_handler() = default;

  /// \brief Handles the establishment of a new CU-CP-to-CU-CP  Xn-C TNL association.
  ///
  /// \param xnap_tx_pdu_notifier Notifier that the CU-CP will use to push XNAP Tx PDUs to the Xn-C GW. Once this
  /// notifier instance goes out of scope, the Xn-C GW will be notified that the CU-CP wants to shutdown the connection.
  /// \return Notifier that the Xn-C GW will use to forward XNAP PDUs to the CU-CP. If the caller lets the returned
  /// object go out of scope, the CU-CP will be notified that a GW event occurred that resulted in the association
  /// being shutdown.
  virtual std::unique_ptr<xnap_message_notifier>
  handle_new_xnc_cu_cp_connection(std::unique_ptr<xnap_message_notifier> xnap_tx_pdu_notifier,
                                  const sctp_association_info&           assoc_info) = 0;
};

} // namespace ocudu::ocucp
