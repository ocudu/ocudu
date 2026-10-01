// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Conversion functions.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/ocuduvec/types.h"
#include <limits>

namespace ocudu {
namespace ocuduvec {

/// Default scaling factor for converting \c ci16_t samples to \c cf_t (maps int16 full scale to approximately 1.0).
inline constexpr float scaling_factor_ci16_to_cf = std::numeric_limits<int16_t>::max();

/// Default scaling factor for converting \c cf_t samples to \c ci16_t.
inline constexpr float scaling_factor_cf_to_ci16 = std::numeric_limits<int16_t>::max();

/// \brief Converts a sequence of numbers from complex float to int16 applying the given scaling and rounding the result
/// to the nearest integer.
///
/// \param [out] z       Converted sequence.
/// \param [in]  x       Input sequence.
/// \param [in]  scale   Scaling factor.
void convert(span<int16_t> z, span<const cf_t> x, float scale);

/// \brief Converts from int16 to complex float applying the given scaling.
///
/// \param [out] z       Converted sequence.
/// \param [in]  x       Input sequence.
/// \param [in]  scale   Scaling factor.
void convert(span<cf_t> z, span<const int16_t> x, float scale);

/// \brief Converts a sequence of numbers from float to int16 applying the given scaling and rounding the result to the
/// nearest integer.
///
/// \param [out] z       Converted sequence.
/// \param [in]  x       Input sequence.
/// \param [in]  scale   Scaling factor.
void convert(span<int16_t> z, span<const float> x, float scale);

/// Converts from int16 to float applying the given scaling
///
/// \param [out] z       Converted sequence.
/// \param [in]  x       Input sequence.
/// \param [in]  scale   Scaling factor.
void convert(span<float> z, span<const int16_t> x, float scale);

/// \brief Converts a sequence of numbers from complex float to complex brain float.
///
/// \param[out] out Resultant data.
/// \param[in]  in  Data to convert.
void convert(span<cbf16_t> out, span<const cf_t> in);

/// \brief Converts a sequence of numbers from single precision float to brain float.
///
/// \param[out] out Resultant data.
/// \param[in]  in  Data to convert.
void convert(span<bf16_t> out, span<const float> in);

/// \brief Converts a sequence of numbers from complex brain float to complex float.
///
/// \param[out] out Resultant data.
/// \param[in]  in  Data to convert.
void convert(span<cf_t> out, span<const cbf16_t> in);

/// \brief Converts a sequence of numbers from brain float to single precision float.
///
/// \param[out] out Resultant data.
/// \param[in]  in  Data to convert.
void convert(span<float> out, span<const bf16_t> in);

/// \brief Converts a sequence of numbers from complex brain float to int16 applying the given scaling and rounding the
/// result to the nearest integer.
///
/// \param [out] z     Resultant data.
/// \param [in]  x     Data to convert.
/// \param [in]  scale Input data scaling prior conversion.
void convert(span<int16_t> z, span<const cbf16_t> x, float scale);

/// \brief Converts a sequence of numbers from complex brain float to 16-bit complex integer applying the given scaling
/// and rounding the result to the nearest integer.
///
/// \param [out] z     Resultant data.
/// \param [in]  x     Data to convert.
/// \param [in]  scale Input data scaling prior conversion.
void convert(span<ci16_t> z, span<const cbf16_t> x, float scale);

/// \brief Converts from int16 to complex brain float applying the given scaling.
///
/// \param [out] z     Resultant data.
/// \param [in]  x     Data to convert.
/// \param [in]  scale Input data scaling after conversion.
void convert(span<cbf16_t> z, span<const int16_t> x, float scale);

/// \brief Converts from int16 to complex brain float applying the given scaling per input element.
///
/// \param [out] z     Resultant data.
/// \param [in]  x     Data to convert.
/// \param [in]  scale Vector of scaling factors to be applied after conversion.
void convert(span<cbf16_t> z, span<const int16_t> x, span<const float> scale);

/// \brief Converts from 16-bit complex integer to complex brain float applying the given scaling.
///
/// \param [out] z     Resultant data.
/// \param [in]  x     Data to convert.
/// \param [in]  scale Input data scaling after conversion.
void convert(span<cbf16_t> z, span<const ci16_t> x, float scale);

/// \brief Converts a sequence of numbers from brain float to int16 applying the given scaling and rounding the result
/// to the nearest integer.
///
/// \param [out] z     Resultant data.
/// \param [in]  x     Data to convert.
/// \param [in]  scale Input data scaling prior conversion.
void convert(span<int16_t> z, span<const bf16_t> x, float scale);

/// Converts from int16 to brain float applying the given scaling.
///
/// \param [out] z     Resultant data.
/// \param [in]  x     Data to convert.
/// \param [in]  scale Input data scaling after conversion.
void convert(span<bf16_t> z, span<const int16_t> x, float scale);

/// \brief Converts a sequence of numbers from complex float to complex int16 applying the given scaling and rounding
/// the result to the nearest integer.
///
/// \param [out] z       Converted sequence.
/// \param [in]  x       Input sequence.
/// \param [in]  scale   Scaling factor.
void convert(span<ci16_t> z, span<const cf_t> x, float scale);

/// \brief Converts from complex int16 to complex float applying the given scaling.
///
/// \param [out] z       Converted sequence.
/// \param [in]  x       Input sequence.
/// \param [in]  scale   Scaling factor.
void convert(span<cf_t> z, span<const ci16_t> x, float scale);

} // namespace ocuduvec
} // namespace ocudu
