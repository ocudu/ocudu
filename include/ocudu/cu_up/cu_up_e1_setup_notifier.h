// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/gnb_cu_up_id.h"

namespace ocudu {
namespace ocuup {

/// Notifier invoked once after a successful E1 Setup, delivering the packed E1SetupRequest and
/// E1SetupResponse bytes together with the gNB-CU-UP-ID. Intended for use by out-of-band consumers
/// that need the raw PDU bytes without being coupled to the E1AP or CU-UP internals.
class cu_up_e1_setup_complete_notifier
{
public:
  virtual ~cu_up_e1_setup_complete_notifier() = default;

  /// \brief Invoked once after a successful E1 Setup exchange.
  /// \param req  Packed E1SetupRequest PDU bytes.
  /// \param resp Packed E1SetupResponse PDU bytes.
  /// \param gnb_cu_up_id gNB-CU-UP identifier.
  virtual void on_e1_setup_complete(byte_buffer req, byte_buffer resp, gnb_cu_up_id_t gnb_cu_up_id) = 0;
};

} // namespace ocuup
} // namespace ocudu
