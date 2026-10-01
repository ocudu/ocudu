// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/bf16.h"
#include <complex>

namespace ocudu {

namespace detail {

template <typename SrcType, typename DstType>
std::complex<DstType> convert(std::complex<SrcType> value)
{
  return {static_cast<DstType>(value.real()), static_cast<DstType>(value.imag())};
}

} // namespace detail

/// Type to store complex samples.
using cf_t = std::complex<float>;

using ci8_t = std::complex<int8_t>;

using ci16_t = std::complex<int16_t>;

/// \brief Complex based on 16-bit floating point.
/// \note This type is meant for storage purposes only, no operations other than equality comparison are allowed.
struct cbf16_t {
  bf16_t real;
  bf16_t imag;

  cbf16_t(float real_ = 0.0F, float imag_ = 0.0F) : real(to_bf16(real_)), imag(to_bf16(imag_)) {}

  cbf16_t(cf_t value) : cbf16_t(value.real(), value.imag()) {}

  cbf16_t(std::complex<double> value) : real(to_bf16(value.real())), imag(to_bf16(value.imag())) {}

  bool operator==(cbf16_t other) const { return (real == other.real) && (imag == other.imag); }

  bool operator!=(cbf16_t other) const { return !(*this == other); }
};

inline ci8_t to_ci8(cf_t value)
{
  return detail::convert<float, int8_t>(value);
}

inline ci16_t to_ci16(cf_t value)
{
  return detail::convert<float, int16_t>(value);
}

inline cf_t to_cf(cbf16_t value)
{
  return {to_float(value.real), to_float(value.imag)};
}

inline cbf16_t to_cbf16(cf_t value)
{
  return cbf16_t(value);
}

inline cf_t to_cf(ci8_t value)
{
  return detail::convert<int8_t, float>(value);
}

inline cf_t to_cf(cf_t value)
{
  return value;
}

inline cf_t to_cf(ci16_t value)
{
  return detail::convert<int16_t, float>(value);
}

/// Checks if T is compatible with a complex floating point.
template <typename T>
struct is_complex : std::false_type {};

template <typename T>
struct is_complex<std::complex<T>> : std::true_type {};

template <typename T>
struct is_complex<const std::complex<T>> : std::true_type {};

} // namespace ocudu
