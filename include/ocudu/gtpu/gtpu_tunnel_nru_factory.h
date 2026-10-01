// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_config.h"
#include "ocudu/gtpu/gtpu_tunnel_common_tx.h"
#include "ocudu/gtpu/gtpu_tunnel_nru.h"
#include "ocudu/gtpu/gtpu_tunnel_nru_rx.h"
#include "ocudu/pcap/dlt_pcap.h"
#include <memory>

/// This factory header file depends on the GTP-U tunnel interfaces (see above include list). It is kept separate as
/// clients of the GTP-U tunnel interfaces do not need to call factory methods.
namespace ocudu {

struct gtpu_tunnel_nru_creation_message {
  uint32_t                                    ue_index;
  gtpu_tunnel_nru_config                      cfg;
  dlt_pcap*                                   gtpu_pcap;
  gtpu_tunnel_nru_rx_lower_layer_notifier*    rx_lower;
  gtpu_tunnel_common_tx_upper_layer_notifier* tx_upper;
};

/// Creates an instance of a GTP-U entity at DU.
std::unique_ptr<gtpu_tunnel_nru> create_gtpu_tunnel_nru(gtpu_tunnel_nru_creation_message& msg);

struct gtpu_tunnel_nru_rx_creation_message {
  uint32_t                                          ue_index;
  gtpu_tunnel_nru_config::gtpu_tunnel_nru_rx_config rx_cfg;
  gtpu_tunnel_nru_rx_lower_layer_notifier*          rx_lower;
};

/// Create an instance of a GTP-U Rx entity at CU-UP.
std::unique_ptr<gtpu_tunnel_common_rx_upper_layer_interface>
create_gtpu_tunnel_nru_rx(gtpu_tunnel_nru_rx_creation_message& msg);

struct gtpu_tunnel_nru_tx_creation_message {
  uint32_t                                          ue_index;
  gtpu_tunnel_nru_config::gtpu_tunnel_nru_tx_config tx_cfg;
  dlt_pcap*                                         gtpu_pcap;
  gtpu_tunnel_common_tx_upper_layer_notifier*       tx_upper;
};

/// Create an instance of a GTP-U Tx entity at CU-UP.
std::unique_ptr<gtpu_tunnel_nru_tx_lower_layer_interface>
create_gtpu_tunnel_nru_tx(gtpu_tunnel_nru_tx_creation_message& msg);

} // namespace ocudu
