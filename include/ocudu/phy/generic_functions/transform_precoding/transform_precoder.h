// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Transform precoding interfaces.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"

namespace ocudu {

/// \brief Transform precoder interface.
///
/// The transform precoding process is described in TS38.211 Section 6.3.1.4 for PUSCH and Section 6.3.2.6.4 for PUCCH.
///
class transform_precoder
{
public:
  /// Default destructor.
  virtual ~transform_precoder() = default;

  /// \brief Reverts the transform precoding for a single OFDM symbol.
  ///
  /// The number of elements to deprecode \f$M_{sc}\f$ determines the number of resource blocks
  /// \f$M_{RB} = \frac{M_{sc}}{N_{sc}^{RB}}\f$.
  ///
  /// The function \ref transform_precoding::is_nof_prbs_valid determines whether a number of resource blocks is valid.
  ///
  /// \param[out] out Transform deprecoding output.
  /// \param[in]  in  Transform deprecoding input.
  /// \remark An assertion is triggered if the size of \c in and \c out are not equal.
  /// \remark An assertion is triggered if the number of RB to deprecode is invalid.
  /// \remark An assertion is triggered if the number of RB exceeds the maximum initialized number of RB.
  virtual void deprecode_ofdm_symbol(span<cf_t> out, span<const cf_t> in) = 0;

  /// \brief Calculates the resultant noise variance after reverting the transform precoding operation.
  ///
  /// Noise variances that are NaN, infinite or negative are not taken into account neither for the calculation of mean
  /// nor overwriting.
  /// \param[out] out Resultant noise variance.
  /// \param[in]  in  Input noise variance for each resource element.
  virtual void deprecode_ofdm_symbol_noise(span<float> out, span<const float> in) = 0;
};

} // namespace ocudu
