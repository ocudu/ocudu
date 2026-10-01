// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Detector interface for short blocks.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"
#include "ocudu/ran/sch/modulation_scheme.h"
#include <memory>

namespace ocudu {

/// Short-block detector interface.
class short_block_detector
{
public:
  /// Default destructor.
  virtual ~short_block_detector() = default;

  /// \brief Detection, decoding and rate-dematching of short blocks.
  ///
  /// Checks whether the provided signal contains a codeblock and, if so, decodes its content assuming the encoding
  /// followed TS38.212 Section 5.3.3 and the rate-matching scheme of TS38.212 Section 5.4.3.
  /// \param[out] output Detected message (one bit per entry).
  /// \param[in]  input  Received codeblock (log-likelihood ratios).
  /// \param[in]  mod    Modulation scheme.
  /// \return True if \c input passes a detection test.
  /// \remark When the detection test fails, the function returns \c false and the content of \c output is unreliable.
  virtual bool detect(span<uint8_t> output, span<const log_likelihood_ratio> input, modulation_scheme mod) = 0;
};
} // namespace ocudu
