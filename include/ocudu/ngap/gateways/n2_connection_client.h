// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ngap/ngap.h"

namespace ocudu::ocucp {

/// This interface notifies the receeption of new NGAP messages over the NGAP interface.
class ngap_rx_message_notifier
{
public:
  virtual ~ngap_rx_message_notifier() = default;

  /// \brief This callback is invoked on each received NGAP message.
  /// \param[in] msg The received NGAP message.
  virtual void on_new_message(const ngap_message& msg) = 0;
};

/// Handler of N2 connection between CU-CP and AMF.
class n2_connection_client
{
public:
  virtual ~n2_connection_client() = default;

  /// Establish a new TNL association with an AMF.
  ///
  /// \param cu_cp_rx_pdu_notifier Notifier that will be used to forward the NGAP PDUs sent by the AMF to the CU-CP.
  /// \return Notifier that the CU-CP can use to send NGAP Tx PDUs to the AMF it connected to.
  virtual std::unique_ptr<ngap_message_notifier>
  handle_cu_cp_connection_request(std::unique_ptr<ngap_rx_message_notifier> cu_cp_rx_pdu_notifier) = 0;
};

} // namespace ocudu::ocucp
