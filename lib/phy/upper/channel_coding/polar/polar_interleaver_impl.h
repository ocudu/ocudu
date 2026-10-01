// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar interleaver - Declaration of implementation class.

#pragma once

#include "ocudu/phy/upper/channel_coding/polar/polar_interleaver.h"

namespace ocudu {

/// Polar interleaver implementation.
class polar_interleaver_impl : public polar_interleaver
{
private:
  static constexpr unsigned                  K_MAX_IL = 164;
  static const std::array<uint8_t, K_MAX_IL> pattern;

public:
  ~polar_interleaver_impl() override = default;

  // See interface for the documentation.
  void interleave(span<uint8_t> out, span<const uint8_t> in, polar_interleaver_direction direction) override;
};

} // namespace ocudu
