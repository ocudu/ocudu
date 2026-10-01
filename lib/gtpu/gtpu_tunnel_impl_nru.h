// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "gtpu_tunnel_nru_rx_impl.h"
#include "gtpu_tunnel_nru_tx_impl.h"
#include "ocudu/gtpu/gtpu_config.h"
#include "ocudu/gtpu/gtpu_tunnel_nru.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/pcap/dlt_pcap.h"

namespace ocudu {

class gtpu_tunnel_nru_impl : public gtpu_tunnel_nru
{
public:
  gtpu_tunnel_nru_impl(uint32_t                                    ue_index,
                       gtpu_tunnel_nru_config                      cfg,
                       dlt_pcap&                                   gtpu_pcap,
                       gtpu_tunnel_nru_rx_lower_layer_notifier&    rx_lower,
                       gtpu_tunnel_common_tx_upper_layer_notifier& tx_upper) :
    logger(ocudulog::fetch_basic_logger("GTPU"))
  {
    rx = std::make_unique<gtpu_tunnel_nru_rx_impl>(ue_index, cfg.rx, rx_lower);
    tx = std::make_unique<gtpu_tunnel_nru_tx_impl>(ue_index, cfg.tx, gtpu_pcap, tx_upper);
  }
  ~gtpu_tunnel_nru_impl() override = default;

  gtpu_tunnel_common_rx_upper_layer_interface* get_rx_upper_layer_interface() final { return rx.get(); }
  gtpu_tunnel_nru_tx_lower_layer_interface*    get_tx_lower_layer_interface() final { return tx.get(); }

private:
  ocudulog::basic_logger& logger;

  std::unique_ptr<gtpu_tunnel_nru_rx_impl> rx = {};
  std::unique_ptr<gtpu_tunnel_nru_tx_impl> tx = {};
};
} // namespace ocudu
