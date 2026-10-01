// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include <memory>

namespace ocudu {

/// Unidirectional notifier of SDUs for a given SCTP association.
class sctp_association_sdu_notifier
{
public:
  virtual ~sctp_association_sdu_notifier() = default;

  /// Forward a SDU.
  /// \return True if the SDU was successfully forwarded, false otherwise.
  virtual bool on_new_sdu(byte_buffer pdu) = 0;
};

class sctp_network_gateway_info
{
public:
  virtual ~sctp_network_gateway_info() = default;

  /// \brief Retrieve the SCTP network node opened socket file descriptor.
  virtual int get_socket_fd() const = 0;
};

/// SCTP network client interface, which can be used to initiate new SCTP associations to a remote SCTP server.
class sctp_network_client : public sctp_network_gateway_info
{
public:
  virtual ~sctp_network_client() = default;

  /// \brief Connect to remote SCTP server.
  [[nodiscard]] virtual std::unique_ptr<sctp_association_sdu_notifier>
  connect(std::unique_ptr<sctp_association_sdu_notifier> recv_handler) = 0;
};

} // namespace ocudu
