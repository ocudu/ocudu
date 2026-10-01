// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_tunnel_common_rx.h"
#include "ocudu/gtpu/gtpu_tunnel_nru_tx.h"

namespace ocudu {

/// Class used to interface with an GTP-U tunnel.
/// It will contain getters for the TX and RX entities interfaces.
/// TX and RX is considered from the perspective of the GTP-U.
class gtpu_tunnel_nru
{
public:
  gtpu_tunnel_nru()                                  = default;
  virtual ~gtpu_tunnel_nru()                         = default;
  gtpu_tunnel_nru(const gtpu_tunnel_nru&)            = delete;
  gtpu_tunnel_nru& operator=(const gtpu_tunnel_nru&) = delete;
  gtpu_tunnel_nru(gtpu_tunnel_nru&&)                 = delete;
  gtpu_tunnel_nru& operator=(gtpu_tunnel_nru&&)      = delete;

  virtual gtpu_tunnel_common_rx_upper_layer_interface* get_rx_upper_layer_interface() = 0;
  virtual gtpu_tunnel_nru_tx_lower_layer_interface*    get_tx_lower_layer_interface() = 0;
};

} // namespace ocudu
