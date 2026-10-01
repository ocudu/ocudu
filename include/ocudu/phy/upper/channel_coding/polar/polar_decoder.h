// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Polar decoding.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/phy/upper/channel_coding/polar/polar_code.h"
#include "ocudu/phy/upper/channel_coding/polar/polar_encoder.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"
#include <cstdint>

namespace ocudu {

/// Polar decoder interface.
class polar_decoder
{
public:
  /// Default destructor.
  virtual ~polar_decoder() = default;

  /// \brief Decodes the input (LLR) codeword with the specified polar decoder.
  /// \param[out] data_decoded The decoder output vector.
  /// \param[in]  input_llr    The decoder LLR input vector.
  /// \param[in]  code         Polar code descriptor.
  virtual void
  decode(span<uint8_t> data_decoded, span<const log_likelihood_ratio> input_llr, const polar_code& code) = 0;
};

} // namespace ocudu
