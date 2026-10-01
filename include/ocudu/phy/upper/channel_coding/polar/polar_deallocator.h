// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar deallocator - Interface.

#pragma once

#include "ocudu/phy/upper/channel_coding/polar/polar_code.h"

namespace ocudu {

/// Polar deallocator interface.
class polar_deallocator
{
public:
  /// Default destructor.
  virtual ~polar_deallocator() = default;

  /// \brief Extracts the information bits from the polar decoder output.
  ///
  /// \param[out] message         Information bits.
  /// \param[in]  output_decoder  Output bits of the polar decoder.
  /// \param[in]  code            Polar code description.
  virtual void deallocate(span<uint8_t> message, span<const uint8_t> output_decoder, const polar_code& code) = 0;
};

} // namespace ocudu
