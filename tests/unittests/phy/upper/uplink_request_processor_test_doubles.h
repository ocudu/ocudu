// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/support/shared_resource_grid.h"
#include "ocudu/phy/upper/uplink_request_processor.h"

namespace ocudu {

/// Dummy implementation of an uplink request processor.
class uplink_request_processor_dummy : public uplink_request_processor
{
public:
  void process_prach_request(const prach_buffer_context& context) override {}
  void process_uplink_slot_request(const resource_grid_context& context, const shared_resource_grid& grid) override {}
};

} // namespace ocudu
