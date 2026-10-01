// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_bitset.h"
#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"

namespace ocudu {

class resource_grid_reader;
class resource_grid_writer;

/// Describes a resource grid class with writer and reader interfaces.
class resource_grid
{
public:
  virtual ~resource_grid() = default;

  /// Sets all resource elements in the grid to zero.
  virtual void set_all_zero() = 0;

  /// Gets the resource grid writer.
  virtual resource_grid_writer& get_writer() = 0;

  /// Gets the resource grid reader.
  virtual const resource_grid_reader& get_reader() const = 0;
};

} // namespace ocudu
