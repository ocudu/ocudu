// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Linear interpolator declaration.

#pragma once

#include "ocudu/phy/support/interpolator.h"

namespace ocudu {

/// \brief Linear interpolator.
///
/// Missing data between two known values are interpolated linearly. Missing data at the beginning (end, respectively)
/// of the sequence are extrapolated by repeating the first (last, respectively) known value.
class interpolator_linear_impl : public interpolator
{
public:
  // See interface for the documentation.
  void interpolate(span<cf_t> output, span<const cf_t> input, const configuration& cfg) override;
};

} // namespace ocudu
