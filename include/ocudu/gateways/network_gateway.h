// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include <chrono>

struct sockaddr_storage;

namespace ocudu {

class io_broker;

/// \brief Generic network gateway interfaces to connect components to the outside world.

/// \brief Common parameters to all network gateways. Specific gateway implementations will further specify parameters
/// according to their needs.
struct common_network_gateway_config {
  std::string bind_interface;
  int         bind_port         = 0;
  bool        non_blocking_mode = false;
  /// Socket received timeout in seconds
  std::chrono::seconds rx_timeout_sec = std::chrono::seconds(1U);
  bool                 reuse_addr     = false;
};

/// \brief Interface to inform upper layers about reception of new PDUs.
class network_gateway_data_notifier
{
public:
  virtual ~network_gateway_data_notifier() = default;

  /// \brief This callback is invoked on each received PDU.
  /// \param[in] pdu Byte-buffer with new PDU.
  virtual void on_new_pdu(byte_buffer pdu) = 0;
};

/// \brief Interface to inform upper layers about reception of new PDUs and source address.
class network_gateway_data_notifier_with_src_addr
{
public:
  virtual ~network_gateway_data_notifier_with_src_addr() = default;

  /// \brief This callback is invoked on each received PDU.
  /// \param[in] pdu Byte-buffer with new PDU.
  /// \param[in] src_addr Source address
  virtual void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) = 0;
};

/// \brief Interface to control common parameters to all network gateways, such as create(), bind(), etc. Gateway
/// specializations should add specific methods as required.
class network_gateway_controller
{
public:
  virtual ~network_gateway_controller() = default;

  /// \brief Create and bind the configured address and port.
  virtual bool create_and_bind() = 0;

  /// \brief Trigger receive call on socket.
  virtual void receive() = 0;

  /// \brief Return socket file descriptor.
  virtual int get_socket_fd() = 0;

  /// \brief Subscribe GW to IO broker for automatic notification of events.
  virtual bool subscribe_to(io_broker& broker) = 0;
};

} // namespace ocudu
