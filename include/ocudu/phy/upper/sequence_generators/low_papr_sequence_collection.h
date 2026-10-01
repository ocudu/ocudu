// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"

namespace ocudu {

/// \brief Describes a low PAPR sequence collection as per TS38.211 Section 5.2.2.
///
/// This class describes a collection of \f$r^{(\alpha, \delta)}_{u,v}(n)\f$ sequences. The collection of sequences is
/// generated at construction time. The constructor is not suitable for real-time operation while the sequence retrieval
/// through method get() is suitable for real-time execution.
///
class low_papr_sequence_collection
{
public:
  /// Defines the number of sequence groups (\f$u\f$).
  static constexpr unsigned NOF_GROUPS = 30;

  /// Defines the number of sequence bases (\f$v\f$).
  static constexpr unsigned MAX_NOF_BASES = 2;

  /// Default destructor.
  virtual ~low_papr_sequence_collection() = default;

  /// \brief Get the sequence \f$r^{(\alpha, \delta)}_{u,v}(n)\f$ corresponding to the given indices.
  ///
  /// \param[in] u         Sequence group \f$u\f$ {0, ..., 29}.
  /// \param[in] v         Sequence base \f$v\f$ {0, 1}.
  /// \param[in] alpha_idx Corresponding index of parameter \f$\alpha\f$.
  /// \return A read-only view of the generated signal.
  /// \remark An assertion is triggered if any of \c u, \c v and \c alpha_idx is invalid.
  virtual span<const cf_t> get(unsigned u, unsigned v, unsigned alpha_idx) const = 0;
};

} // namespace ocudu
