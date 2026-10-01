// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/compression/iq_decompressor.h"
#include <memory>

namespace ocudu {
namespace ofh {

/// \brief IQ decompressor selector implementation.
///
/// The selector will select the IQ decompressor between the registered ones to decompress IQ samples, based on the
/// given compression parameters.
class iq_decompressor_selector : public iq_decompressor
{
public:
  explicit iq_decompressor_selector(
      std::array<std::unique_ptr<iq_decompressor>, NOF_COMPRESSION_TYPES_SUPPORTED> decompressors_);

  // See interface for documentation.
  bool
  decompress(span<cbf16_t> iq_data, span<const uint8_t> compressed_data, const ru_compression_params& params) override;

private:
  std::array<std::unique_ptr<iq_decompressor>, NOF_COMPRESSION_TYPES_SUPPORTED> decompressors;
};

} // namespace ofh
} // namespace ocudu
