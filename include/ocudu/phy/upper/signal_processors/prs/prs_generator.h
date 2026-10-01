// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

class resource_grid_writer;
struct prs_generator_configuration;

/// Positioning Reference Signals (PRS) generator interface.
class prs_generator
{
public:
  /// Default destructor.
  virtual ~prs_generator() = default;

  /// \brief Generates Positioning Reference Signals (PRS) according to TS38.211 Section 7.4.1.7.
  /// \param[out] grid   Resource grid.
  /// \param[in]  config PRS transmission configuration.
  virtual void generate(resource_grid_writer& grid, const prs_generator_configuration& config) = 0;
};

} // namespace ocudu
