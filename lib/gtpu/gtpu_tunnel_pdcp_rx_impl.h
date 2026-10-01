// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "extension_header/long_pdcp_pdu_number_packing.h"
#include "extension_header/pdcp_pdu_number_packing.h"
#include "gtpu_tunnel_base_rx.h"
#include "ocudu/gtpu/gtpu_config.h"
#include "ocudu/gtpu/gtpu_tunnel_pdcp_rx.h"
#include "ocudu/ran/cu_up_types.h"

namespace ocudu {

struct gtpu_pdcp_rx_tpdu_info {
  /// GTP-U T-PDU encapsuling a PDCP SDU.
  byte_buffer tpdu = {};
  /// PDCP PDU number. Conveys a 12-bit PDCP SN or a 18-bit PDCP SN.
  uint32_t pdcp_pdu_number = {};
};

/// Class used for receiving GTP-U PDCP tunnels, e.g. on Xn-U interface.
class gtpu_tunnel_pdcp_rx_impl : public gtpu_tunnel_base_rx
{
public:
  gtpu_tunnel_pdcp_rx_impl(cu_up_ue_index_t                                    ue_index,
                           gtpu_tunnel_pdcp_config::gtpu_tunnel_pdcp_rx_config cfg_,
                           gtpu_tunnel_pdcp_rx_lower_layer_notifier&           rx_lower_);
  ~gtpu_tunnel_pdcp_rx_impl() override = default;

  void stop();

protected:
  // domain-specific PDU handler
  void handle_pdu(gtpu_dissected_pdu&& pdu, const sockaddr_storage& src_addr) final;
  void deliver_sdu(gtpu_pdcp_rx_tpdu_info& sdu_info);

private:
  /// Rx config.
  gtpu_tunnel_pdcp_config::gtpu_tunnel_pdcp_rx_config cfg;
  /// Lower-layer interface.
  gtpu_tunnel_pdcp_rx_lower_layer_notifier& lower_dn;
  /// Packer for PDCP PDU number.
  gtpu::pdcp_pdu_number_packing pdcp_pdu_number_packer;
  /// Packer for long PDCP PDU number.
  gtpu::long_pdcp_pdu_number_packing long_pdcp_pdu_number_packer;
  /// Stop flag.
  bool stopped = false;
};

} // namespace ocudu
