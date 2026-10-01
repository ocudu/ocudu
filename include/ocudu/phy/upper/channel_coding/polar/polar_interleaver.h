// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar interleaver interface.

#pragma once

#include "ocudu/adt/span.h"
#include <cstdint>

namespace ocudu {

/// Identifiers for Tx and Rx interleavers.
enum class polar_interleaver_direction { tx, rx };

/// Polar interleaver interface.
class polar_interleaver
{
public:
  virtual ~polar_interleaver() = default;

  /// \brief Implements Polar code interleaver as described in TS38.212 V15.9.0 Section 5.3.1.1
  ///
  /// \attention The input and output data cannot be the same.
  ///
  /// \param[out] out       Output data.
  /// \param[in]  in        Input data.
  /// \param[in]  direction Set to TX or RX for encoder or decoder, respectively.
  virtual void interleave(span<uint8_t> out, span<const uint8_t> in, polar_interleaver_direction direction) = 0;
};

} // namespace ocudu
