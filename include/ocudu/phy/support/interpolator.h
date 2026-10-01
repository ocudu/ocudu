// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Interpolator interface.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"

namespace ocudu {

/// \brief Interpolator interface.
///
/// Describes a generic interpolator, agnostic to the interpolation algorithm.
class interpolator
{
public:
  /// Interpolator configuration parameters.
  struct configuration {
    /// Index of the first known value (i.e., number of unknown values at the beginning of the sequence).
    unsigned offset = 0;
    /// Distance between two consecutive known values (a value of one means that there are no gaps).
    unsigned stride = 1;
  };

  /// Defalut destructor.
  virtual ~interpolator() = default;

  /// \brief Interpolates the missing data in a sequence.
  ///
  /// \param[out] output Interpolated data sequence.
  /// \param[in]  input  Known data.
  /// \param[in]  cfg    Configuration: describes the position of the known values in the output data sequence.
  /// \remark The output length should be sufficient to process all input data according to the configured pattern.
  /// \remark The function extrapolates all values at the beginning and at the end of the output sequence (that is,
  /// before the first and after the last known values, respectively).
  virtual void interpolate(span<cf_t> output, span<const cf_t> input, const configuration& cfg) = 0;
};

} // namespace ocudu
