// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/compression/iq_compressor.h"
#include "ocudu/ofh/compression/iq_decompressor.h"

namespace ocudu {
namespace ofh {

/// \brief IQ compression implementation used for the compression types that OCUDU does not implement.
///
/// Decompression always fails, so the Open Fronthaul decoder drops the message. Using the compression method
/// will kill the application.
class iq_compression_unsupported_impl : public iq_compressor, public iq_decompressor
{
public:
  // See interface for documentation.
  void compress(span<uint8_t> buffer, span<const cbf16_t> iq_data, const ru_compression_params& params) override;

  // See interface for documentation.
  bool
  decompress(span<cbf16_t> iq_data, span<const uint8_t> compressed_data, const ru_compression_params& params) override;
};

} // namespace ofh
} // namespace ocudu
