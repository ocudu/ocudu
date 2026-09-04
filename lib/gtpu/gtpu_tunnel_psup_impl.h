// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "gtpu_tunnel_psup_rx_impl.h"
#include "gtpu_tunnel_psup_tx_impl.h"
#include "ocudu/gtpu/gtpu_config.h"
#include "ocudu/gtpu/gtpu_tunnel_psup.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/ran/cu_up_types.h"

namespace ocudu {

class gtpu_tunnel_psup_impl : public gtpu_tunnel_psup
{
public:
  gtpu_tunnel_psup_impl(cu_up_ue_index_t                            ue_index,
                        gtpu_tunnel_psup_config                     cfg,
                        dlt_pcap&                                   gtpu_pcap,
                        gtpu_tunnel_psup_rx_lower_layer_notifier&   rx_lower,
                        gtpu_tunnel_common_tx_upper_layer_notifier& tx_upper,
                        timer_factory                               ue_ctrl_timer_factory) :
    logger(ocudulog::fetch_basic_logger("GTPU"))
  {
    rx = std::make_unique<gtpu_tunnel_psup_rx_impl>(ue_index, cfg.rx, rx_lower, ue_ctrl_timer_factory);
    tx = std::make_unique<gtpu_tunnel_psup_tx_impl>(ue_index, cfg.tx, gtpu_pcap, tx_upper);
  }
  ~gtpu_tunnel_psup_impl() override = default;

  void stop() final
  {
    tx->stop();
    rx->stop();
  }
  gtpu_tunnel_common_rx_upper_layer_interface* get_rx_upper_layer_interface() final { return rx.get(); }
  gtpu_tunnel_psup_tx_lower_layer_interface*   get_tx_lower_layer_interface() final { return tx.get(); }

private:
  ocudulog::basic_logger& logger;

  std::unique_ptr<gtpu_tunnel_psup_rx_impl> rx = {};
  std::unique_ptr<gtpu_tunnel_psup_tx_impl> tx = {};
};
} // namespace ocudu
