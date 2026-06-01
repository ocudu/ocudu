// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/e1ap/common/e1ap_message.h"
#include "ocudu/e1ap/cu_cp/cu_cp_e1_handler.h"

namespace ocudu::ocucp {

/// \brief Mock class for the interface between CU-UP and CU-CP.
class mock_cu_up
{
public:
  virtual ~mock_cu_up() = default;

  /// Request from the CU-UP to the CU-CP to establish a E1 connection.
  virtual bool request_cu_cp_connection() = 0;

  /// Called when the CU-UP pushes a E1 Tx PDU to the CU-CP.
  virtual void push_tx_pdu(const e1ap_message& rx_pdu) = 0;

  /// Pop the last E1AP PDU received by the CU-UP.
  virtual bool try_pop_rx_pdu(e1ap_message& pdu) = 0;
};

/// Create a mock of the CU-UP for CU-CP testing.
std::unique_ptr<mock_cu_up> create_mock_cu_up(cu_cp_e1_handler& cu_cp_e1_handler);

} // namespace ocudu::ocucp
