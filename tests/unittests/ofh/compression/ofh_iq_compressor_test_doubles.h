// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/compression/iq_compressor.h"

namespace ocudu {
namespace ofh {
namespace testing {

/// Dummy IQ compressor implementation
class iq_compressor_dummy : public iq_compressor
{
public:
  // See interface for documentation.
  void compress(span<uint8_t> buffer, span<const cbf16_t> iq_data, const ru_compression_params& params) override {}
};

} // namespace testing
} // namespace ofh
} // namespace ocudu
