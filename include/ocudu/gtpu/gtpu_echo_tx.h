// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <stdint.h>

struct sockaddr_storage;

namespace ocudu {

/// This interface represents the data entry point of the transmitting side of a GTP-U echo.
/// The other-layers will use this interface to trigger echo requests or to reply with echo responses.
class gtpu_echo_tx_interface
{
public:
  gtpu_echo_tx_interface()                                         = default;
  virtual ~gtpu_echo_tx_interface()                                = default;
  gtpu_echo_tx_interface(const gtpu_echo_tx_interface&)            = delete;
  gtpu_echo_tx_interface& operator=(const gtpu_echo_tx_interface&) = delete;
  gtpu_echo_tx_interface(gtpu_echo_tx_interface&&)                 = delete;
  gtpu_echo_tx_interface& operator=(gtpu_echo_tx_interface&&)      = delete;

  /// Sends an echo request message to the \c dst_addr.
  /// \param dst_addr The destination address for the echo request message.
  virtual void send_echo_request(const sockaddr_storage& dst_addr) = 0;

  /// Sends an echo response message to the \c dst_addr using sequence number \c sn.
  /// The sequence number shall be copied from the echo request as specified in TS 29.281 Sec. 4.3.1.
  /// \param dst_addr The destination address for the echo response message.
  /// \param sn The sequence number of the initiating echo request message.
  virtual void send_echo_response(const sockaddr_storage& dst_addr, uint16_t sn) = 0;

  /// \brief Handles an echo response message, disables T3-RESPONSE for the request with sequence number \c sn.
  /// \param src_addr The source address of the echo response message.
  /// \param sn The sequence number of the echo response message and the initiating echo request message.
  virtual void handle_echo_response(const sockaddr_storage& src_addr, uint16_t sn) = 0;
};

} // namespace ocudu
