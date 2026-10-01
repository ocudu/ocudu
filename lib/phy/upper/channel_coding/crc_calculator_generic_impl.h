// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/channel_coding/crc_calculator.h"

namespace ocudu {

/// CRC calculator generic implementation.
class crc_calculator_generic_impl : public crc_calculator
{
private:
  /// Generator polynomial order.
  unsigned order;
  /// String of \c order consecutive ones (used for isolating meaningful bits).
  uint32_t polynom;
  /// Identifier of the cyclic generator polynomial.
  const crc_generator_poly poly;

public:
  /// Initializes the CRC calculator with the provided cyclic generator polynomial.
  explicit crc_calculator_generic_impl(crc_generator_poly poly_);

  // See interface for the documentation.
  crc_calculator_checksum_t calculate_byte(span<const uint8_t> input) const override;

  // See interface for the documentation.
  crc_calculator_checksum_t calculate_bit(span<const uint8_t> input) const override;

  // See interface for the documentation.
  crc_calculator_checksum_t calculate(const bit_buffer& data) const override;

  // See interface for the documentation.
  crc_generator_poly get_generator_poly() const override { return poly; }
};

} // namespace ocudu
