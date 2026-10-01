// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar deallocator implementation - Declaration.
#pragma once

#include "ocudu/phy/upper/channel_coding/polar/polar_deallocator.h"

namespace ocudu {

/// Polar deallocator implementation.
class polar_deallocator_impl : public polar_deallocator
{
public:
  // See interface for the documentation.
  void deallocate(span<uint8_t> message, span<const uint8_t> output_decoder, const polar_code& code) override;
};

} // namespace ocudu
