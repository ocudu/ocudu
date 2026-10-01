// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Short-block encoder implementation.

#pragma once

#include "ocudu/phy/upper/channel_coding/short/short_block_encoder.h"

namespace ocudu {

/// Generic implementation of the short-block encoder.
class short_block_encoder_impl : public short_block_encoder
{
public:
  // See interface for the documentation.
  void encode(span<uint8_t> output, span<const uint8_t> input, modulation_scheme mod) override;
};

} // namespace ocudu
