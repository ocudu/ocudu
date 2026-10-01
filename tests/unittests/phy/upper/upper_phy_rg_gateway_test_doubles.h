// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/upper/upper_phy_rg_gateway.h"

namespace ocudu {

/// Implemetantion of an upper phy resource grid gateway for test only. It provides a bool that will be set to true when
/// the send method is called.
class upper_phy_rg_gateway_fto : public upper_phy_rg_gateway
{
public:
  bool sent = false;
  void send(const resource_grid_context& context, shared_resource_grid grid) override { sent = true; }
  void clear_sent() { sent = false; }
};

} // namespace ocudu
