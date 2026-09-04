// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/gtpu/gtpu_tunnel_psup_factory.h"
#include "gtpu_tunnel_psup_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace ocudu;

std::unique_ptr<gtpu_tunnel_psup> ocudu::create_gtpu_tunnel_psup(gtpu_tunnel_psup_creation_message& msg)
{
  return std::make_unique<gtpu_tunnel_psup_impl>(
      msg.ue_index, msg.cfg, *msg.gtpu_pcap, *msg.rx_lower, *msg.tx_upper, msg.ue_ctrl_timer_factory);
}
