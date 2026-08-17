// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocuduvec/conversion.h"
#include <cmath>
#include <cstdint>

namespace ocudu {
namespace ofh {

/// Describes quantization of floating point number in the range [-1; +1] to a fixed point number of a given bit width.
class quantizer
{
public:
  /// Constructor receives width in bits of a resulting fixed point number.
  explicit quantizer(unsigned bit_width_) : bit_width(bit_width_), gain((1 << (bit_width - 1)) - 1.0f) {}

  /// \brief Quantizes a floating point value into 16-bit integer.
  ///
  /// \param[in] value A floating point value.
  /// \return Resultant quantized value.
  int16_t to_fixed_point(float value) const
  {
    float clipped = value;
    if (std::abs(value) > 1.0) {
      clipped = std::copysign(1.0, value);
    }
    return static_cast<int16_t>(std::round(clipped * gain));
  }

  /// \brief Quantizes a brain float value into a 16-bit integer.
  ///
  /// \param[in] value A brain float value.
  /// \return Resultant quantized value.
  int16_t to_fixed_point(bf16_t value) const
  {
    float input_value = ocudu::to_float(value);
    return to_fixed_point(input_value);
  }

  /// \brief Quantizes a sequence of floating point values into 16bit integers.
  ///
  /// \param[out] z       Resultant quantized values.
  /// \param[in] x        Sequence of floating point values.
  /// \param[in] in_scale Scaling factor applied to the input prior conversion.
  ///
  /// \remark \c z and \c x must have the same size.
  void to_fixed_point(span<int16_t> z, span<const float> x, float in_scale) const
  {
    float scale = gain * in_scale;
    ocuduvec::convert(z, x, scale);
  }

  /// \brief Quantizes a sequence of brain float values into 16-bit integers.
  ///
  /// \param[out] z       Resultant quantized values.
  /// \param[in] x        Sequence of brain float values.
  /// \param[in] in_scale Scaling factor applied to the input prior conversion.
  ///
  /// \remark \c z and \c x must have the same size.
  void to_fixed_point(span<int16_t> z, span<const bf16_t> x, float in_scale) const
  {
    float scale = gain * in_scale;
    ocuduvec::convert(z, x, scale);
  }

  /// \brief Converts fixed point value to floating point.
  ///
  /// \param[in] p 16-bit integer value.
  /// \return Resultant floating point value.
  float to_float(int p) const { return (p / gain); }

  /// \brief Converts a sequence of fixed point values to complex brain floating point values.
  ///
  /// \param[out] z       Resultant complex brain floating point values.
  /// \param[in] x        Sequence of 16-bit integer values.
  /// \param[in] in_scale Scaling factor (expected to be bigger or equal to 1) applied to the input prior conversion.
  ///
  /// \remark The size of \c x must be twice the size of \c z as \x is comprised by the quantized pairs of real and
  /// imaginary parts of complex values.
  void to_brain_float(span<cbf16_t> z, span<const int16_t> x, float in_scale) const
  {
    float scale = gain / in_scale;
    ocuduvec::convert(z, x, scale);
  }

  /// Sign extends the input value.
  int16_t sign_extend(int16_t value) const
  {
    // Bit holding the sign of a \c bit_width-bit mantissa.
    uint16_t sign = uint16_t{1} << (bit_width - 1U);
    // Mask the input value to keep only \c bit_width number of bits.
    uint16_t bits = static_cast<uint16_t>(value) & static_cast<uint16_t>((1U << bit_width) - 1U);
    // Avoid the common shift-left/shift-right sign-extension trick because left-shifting into the sign bit of a
    // signed type is undefined in C++17.
    return static_cast<int16_t>(static_cast<int32_t>(bits ^ sign) - static_cast<int32_t>(sign));
  }

private:
  /// Width of fixed point number in bits.
  const unsigned bit_width;
  /// Quantization gain.
  const float gain;
};

} // namespace ofh
} // namespace ocudu
