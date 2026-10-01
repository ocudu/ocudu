// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "iq_compression_bfp_impl.h"

namespace ocudu {
namespace ofh {

/// Implementation of the Block Floating Point IQ data compression using AVX512 intrinsics.
class iq_compression_bfp_avx512 : public iq_compression_bfp_impl
{
public:
  // Constructor.
  explicit iq_compression_bfp_avx512(ocudulog::basic_logger& logger_, float iq_scaling_ = 1.0) :
    iq_compression_bfp_impl(logger_, iq_scaling_)
  {
  }

  // See interface for the documentation.
  void compress(span<uint8_t> buffer, span<const cbf16_t> iq_data, const ru_compression_params& params) override;

  // See interface for the documentation.
  bool
  decompress(span<cbf16_t> iq_data, span<const uint8_t> compressed_data, const ru_compression_params& params) override;
};

} // namespace ofh
} // namespace ocudu
