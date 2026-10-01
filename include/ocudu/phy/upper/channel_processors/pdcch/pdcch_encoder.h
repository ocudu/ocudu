// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/rnti.h"

namespace ocudu {

/// Describes the PDCCH encoder interface.
class pdcch_encoder
{
public:
  /// Describes the necessary parameters to encode PDCCH message.
  struct config_t {
    /// Number of rate-matched bits.
    unsigned E;
    /// RNTI used for CRC bits scrambling according to TS38.212 Section 7.3.2.
    rnti_t rnti;
  };

  /// Default destructor.
  virtual ~pdcch_encoder() = default;

  /// \brief Encodes a PDCCH message.
  /// \param [in]  data    - unencoded message bits
  /// \param [out] encoded - encoded bits
  /// \param [in]  config  - encoder configuration
  ///
  /// \remark size of + 24 bits of CRC gives K parameter for the Polar encoder

  virtual void encode(span<uint8_t> encoded, span<const uint8_t> data, const config_t& config) = 0;
};

} // namespace ocudu
