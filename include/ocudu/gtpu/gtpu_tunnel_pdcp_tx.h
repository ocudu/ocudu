// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/cu_types.h"

struct sockaddr_storage;

/*
 * This file will hold the interfaces and notifiers for the GTP-U tunnel.
 * They follow the following nomenclature:
 *
 *   gtpu_tunnel_{DOMAIN}_{tx/rx}_{lower/upper}_layer_{interface/notifier}
 *
 * 1. TX/RX indicates whether the interface is intended for the
 *    TX or RX side of the tunnel. TX/RX terminology is used from the
 *    perspective of the GTP-U, i.e. are we receiving or sending packets to
 *    the socket gateway.
 * 2. Lower/Upper indicates whether the interface/notifier interacts
 *    with the upper or lower layers.
 * 3. Interface/Notifier: whether this is an interface the GTP-U tunnel will
 *    inherit or a notifier that the GTP-U will keep as a member.
 * 4. DOMAIN indicates the GTP-U specialization for a particular domain {Xn-U, F1-U, ...}
 *
 */

namespace ocudu {

/****************************************
 * Interfaces/notifiers for the gateway
 ****************************************/
/// This interface represents the data entry point of the transmitting side of a GTP-U PDCP entity.
/// The lower layer (i.e. PDCP) will use this interface to pass GTP-U T-PDUs (i.e. PDCP SDUs) into the TX entity.
class gtpu_tunnel_pdcp_tx_lower_layer_interface
{
public:
  gtpu_tunnel_pdcp_tx_lower_layer_interface()                                                            = default;
  virtual ~gtpu_tunnel_pdcp_tx_lower_layer_interface()                                                   = default;
  gtpu_tunnel_pdcp_tx_lower_layer_interface(const gtpu_tunnel_pdcp_tx_lower_layer_interface&)            = delete;
  gtpu_tunnel_pdcp_tx_lower_layer_interface& operator=(const gtpu_tunnel_pdcp_tx_lower_layer_interface&) = delete;
  gtpu_tunnel_pdcp_tx_lower_layer_interface(gtpu_tunnel_pdcp_tx_lower_layer_interface&&)                 = delete;
  gtpu_tunnel_pdcp_tx_lower_layer_interface& operator=(gtpu_tunnel_pdcp_tx_lower_layer_interface&&)      = delete;

  /// \brief Interface for the lower layer (i.e. PDCP) to pass a T-PDU (i.e. PDCP SDU) into the GTP-U.
  /// \param tpdu T-PDU (PDCP SDU) to be handled.
  /// \param pdcp_pdu_number PDCP PDU number. Conveys a 12-bit PDCP SN or a 18-bit PDCP SN.
  virtual void handle_sdu(byte_buffer tpdu, uint32_t pdcp_pdu_number) = 0;
};

} // namespace ocudu
