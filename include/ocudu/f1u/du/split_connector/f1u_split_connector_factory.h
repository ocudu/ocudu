// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/du/f1u_gateway.h"
#include "ocudu/f1u/split_connector/f1u_five_qi_gw_maps.h"
#include "ocudu/gtpu/gtpu_demux.h"
#include "ocudu/pcap/dlt_pcap.h"
#include <cstdint>

namespace ocudu::odu {

struct f1u_du_split_gateway_creation_msg {
  const gtpu_gateway_maps& udp_gw_maps;
  gtpu_demux*              demux;
  dlt_pcap&                gtpu_pcap;
  uint16_t                 peer_port;
};

std::unique_ptr<f1u_du_udp_gateway> create_split_f1u_gw(f1u_du_split_gateway_creation_msg msg);

} // namespace ocudu::odu
