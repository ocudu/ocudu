// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar rate matcher interface.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/phy/upper/channel_coding/polar/polar_code.h"
#include <cstdint>

namespace ocudu {

/// Polar rate matcher interface.
class polar_rate_matcher
{
public:
  /// Default destructor.
  virtual ~polar_rate_matcher() = default;

  /// \brief Carries out the actual rate-matching.
  /// \param[out] output  The rate-matched codeword resulting from the rate-matching operation.
  /// \param[in]  input   The codeword obtained from the polar encoder.
  /// \param[in]  code    The polar code description.
  virtual void rate_match(span<uint8_t> output, span<const uint8_t> input, const polar_code& code) = 0;
};

} // namespace ocudu
