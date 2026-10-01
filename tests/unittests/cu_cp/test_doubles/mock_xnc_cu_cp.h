// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/xnap/gateways/xnc_connection_gateway.h"
#include "ocudu/xnap/xnap.h"

namespace ocudu::ocucp {

/// Interface to XNC CU-CP node that can be accessed safely from the test main thread.
class mock_xnc_cu_cp : public xnc_connection_gateway
{
public:
  virtual ~mock_xnc_cu_cp() = default;

  /// Pop PDU sent by CU-CP and received by XNC CU-CP.
  virtual bool try_pop_rx_pdu(xnap_message& pdu) = 0;

  /// Push new Tx PDU from XNC CU-CP to CU-CP.
  virtual void push_tx_pdu(const xnap_message& pdu) = 0;

  /// Enqueue an XNAP PDU that the XNC CU-CP will automatically send as a response to the next CU-CP Tx PDU.
  virtual void enqueue_next_tx_pdu(const xnap_message& pdu) = 0;
};

/// Creates a mock XNC CU-CP to interface with the CU-CP.
std::unique_ptr<mock_xnc_cu_cp> create_mock_xnc_cu_cp();

} // namespace ocudu::ocucp
