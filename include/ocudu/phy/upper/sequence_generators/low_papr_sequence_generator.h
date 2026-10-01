// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"

namespace ocudu {

/// Describes a low PAPR sequence generator interface that implements TS38.211 Section 5.2.2 for any possible length.
class low_papr_sequence_generator
{
public:
  /// Default destructor.
  virtual ~low_papr_sequence_generator() = default;

  /// \brief Generates a \f$r^{(\alpha, \delta)}_{u,v}\f$ sequence as per TS38.211 Section 5.2.2.
  ///
  /// The cyclic shift \f$\alpha=2\pi\frac{n_{cs}}{n_{cs}^{max}}\f$ is split into the numerator and denominator.
  ///
  /// \param[out] sequence Provides the destination buffer and the sequence length.
  /// \param[in] u         Sequence group {0, ..., 29}.
  /// \param[in] v         Sequence number {0, 1}.
  /// \param[in] alpha_num Cyclic shift numerator, parameter \f$n_{cs}\f$.
  /// \param[in] alpha_den Cyclic shift denominator, parameter \f$n_{cs}^{max}\f$.
  virtual void generate(span<cf_t> sequence, unsigned u, unsigned v, unsigned alpha_num, unsigned alpha_den) = 0;
};

} // namespace ocudu
