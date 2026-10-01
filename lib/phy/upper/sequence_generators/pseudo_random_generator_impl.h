// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file
/// \brief Pseudo-random generator implementation declaration.

#pragma once

#include "ocudu/phy/upper/sequence_generators/pseudo_random_generator.h"
#include <cstdint>
#include <memory>

namespace ocudu {

/// SSE-optimized implementation of the pseudo-random generator.
class pseudo_random_generator_impl : public pseudo_random_generator
{
private:
  /// Memory register for sequence \f$x_1(n)\f$.
  uint32_t x1 = 0;
  /// Memory register for sequence \f$x_2(n)\f$.
  uint32_t x2 = 0;

public:
  /// Default constructor.
  pseudo_random_generator_impl() = default;

  // See interface for the documentation.
  void init(unsigned c_init) override;

  // See interface for the documentation.
  void init(const state_s& c_init) override;

  // See interface for the documentation.
  state_s get_state() const override;

  // See interface for the documentation.
  void advance(unsigned count) override;

  // See interface for the documentation.
  void apply_xor(bit_buffer& out, const bit_buffer& in) override;

  // See interface for the documentation.
  void apply_xor(span<uint8_t> out, span<const uint8_t> in) override;

  // See interface for the documentation.
  void apply_xor(span<log_likelihood_ratio> out, span<const log_likelihood_ratio> in) override;

  // See interface for the documentation.
  void generate(bit_buffer& data) override;

  // See interface for the documentation.
  void generate(span<float> buffer, float value) override;
};
} // namespace ocudu
