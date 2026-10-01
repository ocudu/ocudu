// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include <netinet/sctp.h>

namespace ocudu {

/// Interface for DTLS associations to communicate back to the SCTP server.
/// Useful, e.g., for handling notifications.
class sctp_network_gateway_dtls_interface
{
public:
  virtual ~sctp_network_gateway_dtls_interface()                                         = default;
  virtual void handle_dtls_notification(const union sctp_notification* notif, int assoc) = 0;
};
} // namespace ocudu
