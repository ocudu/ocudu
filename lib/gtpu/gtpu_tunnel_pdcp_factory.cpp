// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/gtpu/gtpu_tunnel_pdcp_factory.h"
#include "gtpu_tunnel_pdcp_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;

std::unique_ptr<gtpu_tunnel_pdcp> ocudu::create_gtpu_tunnel_pdcp(gtpu_tunnel_pdcp_creation_message& msg)
{
  return std::make_unique<gtpu_tunnel_pdcp_impl>(msg.ue_index, msg.cfg, *msg.gtpu_pcap, *msg.rx_lower, *msg.tx_upper);
}
