// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"

namespace ocudu {

/// \brief Describes a Discrete Fourier Transform (DFT) processor interface for 16-bit complex integer values.
///
/// The DFT in 16-bit complex integer output is divided by the DFT size independently of the direction.
class dft_processor_ci16
{
public:
  /// DFT algorithm direction.
  enum class direction {
    /// Converts a time-domain signal into a frequency-domain signal.
    direct = 0,
    /// Converts a frequency-domain signal into a time-domain signal.
    inverse
  };

  /// Collects the DFT parameters.
  struct configuration {
    /// DFT size.
    unsigned size;
    /// DFT direction (direct or inverse).
    direction dir;
  };

  /// Converts a DFT direction to string.
  static std::string direction_to_string(direction dir) { return dir == direction::direct ? "direct" : "inverse"; }

  /// Default destructor.
  virtual ~dft_processor_ci16() = default;

  /// Gets the DFT direction.
  virtual direction get_direction() const = 0;

  /// Gets the DFT number of points.
  virtual unsigned get_size() const = 0;

  /// \brief Executes the DFT from the internal input data.
  /// \return A view of the internal output DFT buffer.
  /// \remark An assertion is triggered if the input and output sizes do not match the DFT size.
  virtual void run(span<ci16_t> out, span<const ci16_t> in) = 0;
};

} // namespace ocudu
