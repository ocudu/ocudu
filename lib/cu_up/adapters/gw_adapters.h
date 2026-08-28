// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gateways/network_gateway.h"
#include "ocudu/gtpu/gtpu_demux.h"

namespace ocudu::ocuup {

/// Adapter between Network Gateway (Data) and GTP-U demux.
class network_gateway_data_gtpu_demux_adapter : public network_gateway_data_notifier_with_src_addr
{
public:
  explicit network_gateway_data_gtpu_demux_adapter(gtpu_demux_rx_upper_layer_interface& gtpu_demux_) :
    gtpu_demux(gtpu_demux_)
  {
  }

  // See interface for documentation.
  void on_new_pdu(byte_buffer pdu, const sockaddr_storage& src_addr) override
  {
    gtpu_demux.handle_pdu(std::move(pdu), src_addr);
  }

private:
  gtpu_demux_rx_upper_layer_interface& gtpu_demux;
};

} // namespace ocudu::ocuup
