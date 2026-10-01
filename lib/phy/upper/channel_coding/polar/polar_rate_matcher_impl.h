// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar rate-matcher - Declaration of implementation class.

#pragma once

#include "ocudu/phy/upper/channel_coding/polar/polar_code.h"
#include "ocudu/phy/upper/channel_coding/polar/polar_rate_matcher.h"

namespace ocudu {

/// Polar rate-matcher implementation.
class polar_rate_matcher_impl : public polar_rate_matcher
{
private:
  std::array<uint8_t, polar_code::EMAX> y_e;

public:
  polar_rate_matcher_impl()           = default;
  ~polar_rate_matcher_impl() override = default;

  // See interface for the documentation.
  void rate_match(span<uint8_t> output, span<const uint8_t> input, const polar_code& code) override;
};
} // namespace ocudu
