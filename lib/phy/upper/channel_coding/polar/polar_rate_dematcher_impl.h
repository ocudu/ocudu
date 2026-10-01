// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar code rate dematcher declaration.

#pragma once

#include "ocudu/phy/upper/channel_coding/polar/polar_code.h"
#include "ocudu/phy/upper/channel_coding/polar/polar_rate_dematcher.h"

namespace ocudu {

/// \brief Polar code rate dematching implementation.
///
/// It reverts the rate matching procedure described in TS38.212 Section 5.4.1.
class polar_rate_dematcher_impl : public polar_rate_dematcher
{
private:
  /// Internal buffer that contains both codeblock and rate-matched codeblock.
  std::array<log_likelihood_ratio, polar_code::NMAX + polar_code::EMAX> y_e;
  /// Pointer to the rate-matched part of the buffer.
  log_likelihood_ratio* e = &y_e[polar_code::NMAX];

public:
  /// Default constructor.
  polar_rate_dematcher_impl() = default;
  /// Default destructor.
  ~polar_rate_dematcher_impl() override = default;

  // See interface for the documentation.
  void rate_dematch(span<log_likelihood_ratio>       output,
                    span<const log_likelihood_ratio> input,
                    const polar_code&                code) override;
};
} // namespace ocudu
