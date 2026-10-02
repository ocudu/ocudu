// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Byte swapping of 16-bit complex samples, for converting between host and wire byte order.

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/ocuduvec/types.h"

namespace ocudu {
namespace ocuduvec {

/// \brief Serializes complex 16-bit samples into a byte buffer with the two bytes of every component swapped.
///
/// On a little-endian host this writes the samples in big-endian (network) byte order, real part first.
///
/// \param [out] out Serialized samples, four bytes per complex sample.
/// \param [in]  in  Complex 16-bit samples.
/// \remark The output size must be four times the number of input samples.
void swap_bytes(span<uint8_t> out, span<const ci16_t> in);

/// \brief Deserializes complex 16-bit samples from a byte buffer, swapping the two bytes of every component.
///
/// Inverse of \ref swap_bytes(span<uint8_t>, span<const ci16_t>).
///
/// \param [out] out Complex 16-bit samples.
/// \param [in]  in  Serialized samples, four bytes per complex sample.
/// \remark The input size must be four times the number of output samples.
void swap_bytes(span<ci16_t> out, span<const uint8_t> in);

} // namespace ocuduvec
} // namespace ocudu
