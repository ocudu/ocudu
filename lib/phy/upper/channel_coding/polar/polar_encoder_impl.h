// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar encoder - Declaration of implementation class.
#pragma once

#include "ocudu/phy/upper/channel_coding/polar/polar_encoder.h"

namespace ocudu {

/// Polar encoder implementation.
class polar_encoder_impl : public polar_encoder
{
public:
  // See interface for documentation.
  void encode(span<uint8_t> output, span<const uint8_t> input, unsigned code_size_log) override;
};

} // namespace ocudu
