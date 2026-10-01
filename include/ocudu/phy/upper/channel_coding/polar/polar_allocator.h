// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar allocator interface.

#pragma once

#include "ocudu/phy/upper/channel_coding/polar/polar_code.h"

namespace ocudu {

/// Polar allocator interface.
class polar_allocator
{
public:
  /// Default destructor.
  virtual ~polar_allocator() = default;

  /// \brief Allocates the message bits into the encoder input.
  ///
  /// \param[out] input_encoder   Sequence of allocated bits to be fed to the polar encoder.
  /// \param[in]  message         Information bits to be allocated.
  /// \param[in]  code            Polar code description.
  virtual void allocate(span<uint8_t> input_encoder, span<const uint8_t> message, const polar_code& code) = 0;
};

} // namespace ocudu
