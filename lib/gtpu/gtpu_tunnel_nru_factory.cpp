// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/gtpu/gtpu_tunnel_nru_factory.h"
#include "gtpu_tunnel_impl_nru.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;

std::unique_ptr<gtpu_tunnel_nru> ocudu::create_gtpu_tunnel_nru(gtpu_tunnel_nru_creation_message& msg)
{
  return std::make_unique<gtpu_tunnel_nru_impl>(msg.ue_index, msg.cfg, *msg.gtpu_pcap, *msg.rx_lower, *msg.tx_upper);
}

std::unique_ptr<gtpu_tunnel_common_rx_upper_layer_interface>
ocudu::create_gtpu_tunnel_nru_rx(gtpu_tunnel_nru_rx_creation_message& msg)
{
  return std::make_unique<gtpu_tunnel_nru_rx_impl>(msg.ue_index, msg.rx_cfg, *msg.rx_lower);
}

std::unique_ptr<gtpu_tunnel_nru_tx_lower_layer_interface>
ocudu::create_gtpu_tunnel_nru_tx(gtpu_tunnel_nru_tx_creation_message& msg)
{
  return std::make_unique<gtpu_tunnel_nru_tx_impl>(msg.ue_index, msg.tx_cfg, *msg.gtpu_pcap, *msg.tx_upper);
}
