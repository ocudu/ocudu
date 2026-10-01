// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/compression/iq_decompressor.h"

namespace ocudu {
namespace ofh {
namespace testing {

/// Dummy IQ decompressor implementation
class iq_decompressor_dummy : public iq_decompressor
{
public:
  // See interface for documentation.
  bool
  decompress(span<cbf16_t> iq_data, span<const uint8_t> compressed_data, const ru_compression_params& params) override
  {
    return true;
  }
};

} // namespace testing
} // namespace ofh
} // namespace ocudu
