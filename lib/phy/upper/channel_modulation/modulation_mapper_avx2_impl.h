// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "modulation_mapper_lut_impl.h"
#include "ocudu/phy/upper/channel_modulation/modulation_mapper.h"

namespace ocudu {

/// Modulation mapper based on AVX2 instruction sets.
class modulation_mapper_avx2_impl : public modulation_mapper
{
public:
  // See interface for the documentation.
  void modulate(span<cf_t> symbols, const bit_buffer& input, modulation_scheme scheme) override;

  // See interface for the documentation.
  float modulate(span<ci8_t> symbols, const bit_buffer& input, modulation_scheme scheme) override;

private:
  /// Implements the modulation algorithm for 64-QAM.
  float modulate_qam64(span<ci8_t> symbols, const bit_buffer& input);

  /// Implements the modulation algorithm for 256-QAM.
  float modulate_qam256(span<ci8_t> symbols, const bit_buffer& input);

  /// Modulation mapper based on LUT for modulations that are not implemented with AVX2.
  modulation_mapper_lut_impl lut_modulator;
};

} // namespace ocudu
