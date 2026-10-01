// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "extension_header/long_pdcp_pdu_number_packing.h"
#include "extension_header/pdcp_pdu_number_packing.h"
#include "gtpu_pdu.h"
#include "gtpu_tunnel_base_tx.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/gtpu/gtpu_config.h"
#include "ocudu/gtpu/gtpu_tunnel_pdcp_tx.h"
#include "ocudu/ran/cu_up_types.h"
#include <arpa/inet.h>
#include <netinet/in.h>

namespace ocudu {

/// Class used for transmitting GTP-U PDCP tunnels, e.g. on Xn-U interface.
class gtpu_tunnel_pdcp_tx_impl final : public gtpu_tunnel_base_tx, public gtpu_tunnel_pdcp_tx_lower_layer_interface
{
public:
  gtpu_tunnel_pdcp_tx_impl(cu_up_ue_index_t                                           ue_index,
                           const gtpu_tunnel_pdcp_config::gtpu_tunnel_pdcp_tx_config& cfg_,
                           dlt_pcap&                                                  gtpu_pcap_,
                           gtpu_tunnel_common_tx_upper_layer_notifier&                upper_dn_);
  ~gtpu_tunnel_pdcp_tx_impl() override = default;

  void stop();
  void handle_sdu(byte_buffer buf, uint32_t pdcp_pdu_number) override;

private:
  /// Tx config.
  const gtpu_tunnel_pdcp_config::gtpu_tunnel_pdcp_tx_config cfg;
  /// Configured extension type used for PDCP PDU number, either pdcp_pdu_number or long_pdcp_pdu_number.
  const gtpu_extension_header_type cfg_pdcp_pdu_number_extension_type;
  /// Packer for PDCP PDU number.
  gtpu::pdcp_pdu_number_packing pdcp_pdu_number_packer;
  /// Packer for long PDCP PDU number.
  gtpu::long_pdcp_pdu_number_packing long_pdcp_pdu_number_packer;
  /// TEID of the current peer.
  gtpu_teid_t current_peer_teid = {};
  /// Address of the current peer.
  sockaddr_storage peer_sockaddr = {};
  /// Stop flag.
  bool stopped = false;
};

} // namespace ocudu
