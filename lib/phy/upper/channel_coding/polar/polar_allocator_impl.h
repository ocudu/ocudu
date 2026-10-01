// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar allocator - Declaration of implementation class.

#pragma once

#include "ocudu/phy/upper/channel_coding/polar/polar_allocator.h"

namespace ocudu {

/// Polar allocator implementation.
class polar_allocator_impl : public polar_allocator
{
public:
  // See interface for the documentation.
  void allocate(span<uint8_t> input_encoder, span<const uint8_t> message, const polar_code& code) override;
};

} // namespace ocudu
