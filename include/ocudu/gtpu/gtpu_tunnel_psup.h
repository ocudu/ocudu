// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_tunnel_common_rx.h"
#include "ocudu/gtpu/gtpu_tunnel_psup_tx.h"

namespace ocudu {

/// Class used to interface with an GTP-U tunnel.
/// It will contain getters for the TX and RX entities interfaces.
/// TX and RX is considered from the perspective of the GTP-U.
class gtpu_tunnel_psup
{
public:
  gtpu_tunnel_psup()                                   = default;
  virtual ~gtpu_tunnel_psup()                          = default;
  gtpu_tunnel_psup(const gtpu_tunnel_psup&)            = delete;
  gtpu_tunnel_psup& operator=(const gtpu_tunnel_psup&) = delete;
  gtpu_tunnel_psup(gtpu_tunnel_psup&&)                 = delete;
  gtpu_tunnel_psup& operator=(gtpu_tunnel_psup&&)      = delete;

  virtual void                                         stop()                         = 0;
  virtual gtpu_tunnel_common_rx_upper_layer_interface* get_rx_upper_layer_interface() = 0;
  virtual gtpu_tunnel_psup_tx_lower_layer_interface*   get_tx_lower_layer_interface() = 0;
};

} // namespace ocudu
