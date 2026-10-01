// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/gtpu/gtpu_tunnel_common_rx.h"

namespace ocudu {

/// Class used to interface with an GTP-U path management entity.
/// It will contain getters for the RX entities interfaces.
/// TX and RX is considered from the perspective of the GTP-U.
class gtpu_echo
{
public:
  gtpu_echo()                            = default;
  virtual ~gtpu_echo()                   = default;
  gtpu_echo(const gtpu_echo&)            = delete;
  gtpu_echo& operator=(const gtpu_echo&) = delete;
  gtpu_echo(gtpu_echo&&)                 = delete;
  gtpu_echo& operator=(gtpu_echo&&)      = delete;

  virtual gtpu_tunnel_common_rx_upper_layer_interface* get_rx_upper_layer_interface() = 0;
};

} // namespace ocudu
