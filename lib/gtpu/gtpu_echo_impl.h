// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "gtpu_echo_rx_impl.h"
#include "gtpu_echo_tx_impl.h"
#include "ocudu/gtpu/gtpu_echo.h"
#include "ocudu/pcap/dlt_pcap.h"

namespace ocudu {

/// Implementation of the GTP-U path management entity, i.e. echo request/response, supported extension headers notif.
class gtpu_echo_impl final : public gtpu_echo
{
public:
  gtpu_echo_impl(dlt_pcap& gtpu_pcap, gtpu_tunnel_common_tx_upper_layer_notifier& tx_upper) :
    logger(ocudulog::fetch_basic_logger("GTPU"))
  {
    tx = std::make_unique<gtpu_echo_tx>(gtpu_pcap, tx_upper);
    rx = std::make_unique<gtpu_echo_rx>(*tx.get());
  }
  ~gtpu_echo_impl() override = default;

  gtpu_tunnel_common_rx_upper_layer_interface* get_rx_upper_layer_interface() override { return rx.get(); }

private:
  ocudulog::basic_logger& logger;

  std::unique_ptr<gtpu_echo_tx> tx = {};
  std::unique_ptr<gtpu_echo_rx> rx = {};
};
} // namespace ocudu
