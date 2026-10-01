// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/bit_buffer.h"
#include "ocudu/adt/span.h"
#include "ocudu/support/units.h"

namespace ocudu {
namespace ofh {

/// Packs the given IQ samples into the given bit buffer object.
void pack_bytes(bit_buffer& buffer, span<const int16_t> compressed_iq, unsigned iq_width);

/// Extract \c length bits from the provided bit buffer starting from offset \c offset.
int16_t unpack_bits(const bit_buffer_reader& buffer, unsigned offset, unsigned length);

} // namespace ofh
} // namespace ocudu
