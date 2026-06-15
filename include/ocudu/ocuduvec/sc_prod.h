// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Product of a vector by a scalar.

#pragma once

#include "ocudu/ocuduvec/types.h"

namespace ocudu {
namespace ocuduvec {

///@{
/// \brief Product of a vector by a scalar.
/// \param[out]  z Output vector.
/// \param[in]   x Input vector.
/// \param[in]   h Scalar factor.
/// \warning An assertion is triggered if the input and output vectors have different sizes.
void sc_prod(span<cf_t> z, span<const cf_t> x, cf_t h);
void sc_prod(span<cbf16_t> z, span<const cbf16_t> x, cf_t h);
void sc_prod(span<cbf16_t> z, span<const cbf16_t> x, float h);
void sc_prod(span<cf_t> z, span<const cf_t> x, float h);
void sc_prod(span<float> z, span<const float> x, float h);
void sc_prod(span<cbf16_t> z, span<const cf_t> x, cf_t h);
void sc_prod(span<cbf16_t> z, span<const cf_t> x, float h);
///@}

///@{
/// \brief Scales the first input vector and adds the second one.
///
/// The result can be expressed as \f$\mathbf{z} = h\mathbf{x} + \mathbf{y}\f$.
/// \param[out]  z Output vector.
/// \param[in]   x Scaled input vector.
/// \param[in]   y Added input vector.
/// \param[in]   h Scalar factor.
/// \warning An assertion is triggered if the input and output vectors have different sizes.
void sc_prod_and_add(span<cf_t> z, span<const cf_t> x, span<const cf_t> y, cf_t h);
void sc_prod_and_add(span<cf_t> z, span<const cf_t> x, span<const cf_t> y, float h);
void sc_prod_and_add(span<float> z, span<const float> x, span<const float> y, float h);
void sc_prod_and_add(span<cbf16_t> z, span<const cbf16_t> x, span<const cbf16_t> y, cf_t h);
void sc_prod_and_add(span<cbf16_t> z, span<const cbf16_t> x, span<const cbf16_t> y, float h);
void sc_prod_and_add(span<cf_t> z, span<const cbf16_t> x, span<const cf_t> y, cf_t h);
///@}

} // namespace ocuduvec
} // namespace ocudu
