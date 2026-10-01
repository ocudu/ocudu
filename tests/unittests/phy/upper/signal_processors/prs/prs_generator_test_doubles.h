// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/signal_processors/prs/prs_generator.h"

namespace ocudu {

/// Spy implementation of a PRS generator.
class prs_processor_spy : public prs_generator
{
  bool generate_method_called = false;

public:
  void generate(resource_grid_writer& grid, const prs_generator_configuration& config) override
  {
    generate_method_called = true;
  }

  /// Returns true if the map method has been called, false otherwise.
  bool is_generate_called() const { return generate_method_called; }
};

} // namespace ocudu
