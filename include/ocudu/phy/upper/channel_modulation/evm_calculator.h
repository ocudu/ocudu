// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"
#include "ocudu/ran/sch/modulation_scheme.h"

namespace ocudu {

/// Error Vector Magnitude calculator.
class evm_calculator
{
public:
  /// Default destructor.
  virtual ~evm_calculator() = default;

  /// \brief Calculates the EVM for a transmission.
  ///
  /// \param[in] soft_bits  The soft bits from the modulation demapper.
  /// \param[in] symbols    The complex-valued channel symbols, prior to the modulation demapper.
  /// \param[in] modulation The modulation scheme.
  /// \return The calculated EVM.
  /// \remark The size of \c soft_bits must be equal to the size of \c symbols times the number of bits per symbol
  /// corresponding to \c modulation.
  virtual float
  calculate(span<const log_likelihood_ratio> soft_bits, span<const cf_t> symbols, modulation_scheme modulation) = 0;
};

} // namespace ocudu
