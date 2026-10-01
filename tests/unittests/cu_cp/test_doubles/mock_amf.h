// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ngap/gateways/n2_connection_client.h"
#include "ocudu/ngap/ngap.h"

namespace ocudu {
namespace ocucp {

/// Interface to AMF node that can be accessed safely from the test main thread.
class mock_amf : public n2_connection_client
{
public:
  /// Pop PDU sent by CU-CP and received by AMF.
  virtual bool try_pop_rx_pdu(ngap_message& pdu) = 0;

  /// Push new Tx PDU from AMF to CU-CP.
  virtual void push_tx_pdu(const ngap_message& pdu) = 0;

  /// Enqueue an NGAP PDU that the AMF will automatically send as a response to the next CU-CP Tx PDU.
  virtual void enqueue_next_tx_pdu(const ngap_message& pdu) = 0;

  /// Simulate a drop of the AMF connection.
  virtual void drop_connection() = 0;

  /// Allow a new TNL connection after a previous drop (simulates AMF becoming reachable again).
  virtual void allow_reconnection() = 0;
};

/// Creates a mock AMF to interface with the CU-CP.
std::unique_ptr<mock_amf> create_mock_amf();

} // namespace ocucp
} // namespace ocudu
