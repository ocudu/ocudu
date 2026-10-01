// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/signal_processors/nzp_csi_rs/nzp_csi_rs_generator.h"

namespace ocudu {

/// Spy implementation of a CSI-RS processor.
class csi_rs_processor_spy : public nzp_csi_rs_generator
{
  bool map_method_called = false;

public:
  void map(resource_grid_writer& grid, const config_t& config) override { map_method_called = true; }

  /// Returns true if the map method has been called, false otherwise.
  bool is_map_called() const { return map_method_called; }
};

} // namespace ocudu
