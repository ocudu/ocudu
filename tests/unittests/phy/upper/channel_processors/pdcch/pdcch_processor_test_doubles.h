// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../../../phy_test_utils.h"
#include "ocudu/phy/upper/channel_processors/pdcch/factories.h"

namespace ocudu {

/// Spy implementation of a PDCCH processor.
class pdcch_processor_spy : public pdcch_processor
{
  bool process_method_called = false;

public:
  // See interface for documentation.
  void process(resource_grid_writer& grid, const pdu_t& pdu) override { process_method_called = true; }

  /// Returns true if the process method has been called, false otherwise.
  bool is_process_called() const { return process_method_called; }
};

PHY_SPY_FACTORY_TEMPLATE(pdcch_processor);

} // namespace ocudu
