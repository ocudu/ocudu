// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {

/// \brief Resource grid base interface.
///
/// Provides getters.
class resource_grid_base
{
public:
  /// Default destructor.
  virtual ~resource_grid_base() = default;

  /// Gets the resource grid number of ports.
  virtual unsigned get_nof_ports() const = 0;

  /// Gets the resource grid number of subcarriers.
  virtual unsigned get_nof_subc() const = 0;

  /// Gets the resource grid number of symbols.
  virtual unsigned get_nof_symbols() const = 0;
};

} // namespace ocudu
