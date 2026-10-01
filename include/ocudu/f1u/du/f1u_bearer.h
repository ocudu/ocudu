// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/f1u/du/f1u_rx_pdu_handler.h"
#include "ocudu/f1u/du/f1u_tx_delivery_handler.h"
#include "ocudu/f1u/du/f1u_tx_sdu_handler.h"
#include "ocudu/ran/up_transport_layer_info.h"

namespace ocudu {
namespace odu {

class f1u_bearer
{
public:
  virtual ~f1u_bearer() = default;

  virtual f1u_rx_pdu_handler&      get_rx_pdu_handler()      = 0;
  virtual f1u_tx_delivery_handler& get_tx_delivery_handler() = 0;
  virtual f1u_tx_sdu_handler&      get_tx_sdu_handler()      = 0;

  virtual void stop() = 0;
};

/// This class represents the interface through which a F1-U bearer disconnects from its gateway upon removal
class f1u_bearer_disconnector
{
public:
  virtual ~f1u_bearer_disconnector() = default;

  virtual void remove_du_bearer(const up_transport_layer_info& dl_up_tnl_info) = 0;
};

} // namespace odu
} // namespace ocudu
