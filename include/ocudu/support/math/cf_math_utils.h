// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Mathematical utility functions for complex values.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/support/math/math_utils.h"

namespace ocudu {

/// \brief Calculates the squared modulus of a complex value.
/// \param[in] x Complex value.
/// \return The squared absolute of the given value, i.e. \f$\abs{x}^2=x\cdot\conj{x}=\Re(x)^2+\Im(x)^2\f$.
constexpr float abs_sq(cf_t x)
{
  // Equivalent to but computationally simpler than std::pow(std::abs(x),2).
  return x.real() * x.real() + x.imag() * x.imag();
}

/// Determines whether a complex floating point value is near zero.
inline bool is_near_zero(cf_t value)
{
  return abs_sq(value) < near_zero;
}

} // namespace ocudu
