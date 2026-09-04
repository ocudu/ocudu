// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_config.h"
#include "ocudu/gtpu/gtpu_tunnel_common_tx.h"
#include "ocudu/gtpu/gtpu_tunnel_psup.h"
#include "ocudu/gtpu/gtpu_tunnel_psup_rx.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/ran/cu_up_types.h"
#include "ocudu/support/timers.h"
#include <memory>

/// This factory header file depends on the GTP-U tunnel interfaces (see above include list). It is kept separate as
/// clients of the GTP-U tunnel interfaces do not need to call factory methods.
namespace ocudu {

struct gtpu_tunnel_psup_creation_message {
  cu_up_ue_index_t                            ue_index;
  gtpu_tunnel_psup_config                     cfg;
  dlt_pcap*                                   gtpu_pcap;
  gtpu_tunnel_psup_rx_lower_layer_notifier*   rx_lower;
  gtpu_tunnel_common_tx_upper_layer_notifier* tx_upper;
  timer_factory                               ue_ctrl_timer_factory;
};

/// Creates an instance of a GTP-U entity.
std::unique_ptr<gtpu_tunnel_psup> create_gtpu_tunnel_psup(gtpu_tunnel_psup_creation_message& msg);

} // namespace ocudu
