// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/tensor.h"

namespace ocudu {

/// Dimension, i.e. number of coordinates, of each indexing level of the resource grid.
enum class resource_grid_dimensions : unsigned { subc = 0, symbol = 1, port = 2, all = 3 };

} // namespace ocudu
