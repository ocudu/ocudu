// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"

namespace ocudu::ocucp {

/// Notifier invoked once after a successful NG Setup, delivering the packed NGSetupRequest and
/// NGSetupResponse bytes. Intended for use by out-of-band consumers that need the raw PDU bytes without being coupled
/// to the NGAP or CU-CP internals.
class cu_cp_ng_setup_complete_notifier
{
public:
  virtual ~cu_cp_ng_setup_complete_notifier() = default;

  /// \brief Invoked once after a successful NG Setup exchange.
  /// \param req      Packed NGSetupRequest PDU bytes.
  /// \param resp     Packed NGSetupResponse PDU bytes.
  /// \param amf_name AMF name received in the NG Setup Response.
  virtual void on_ng_setup_complete(byte_buffer req, byte_buffer resp, std::string amf_name) = 0;
};

} // namespace ocudu::ocucp
