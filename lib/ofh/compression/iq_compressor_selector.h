// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/compression/iq_compressor.h"
#include <memory>

namespace ocudu {
namespace ofh {

/// \brief IQ compressor selector implementation.
///
/// The selector will select the IQ compressor between the registered ones to compress IQ samples, based on the
/// given compression parameters.
class iq_compressor_selector : public iq_compressor
{
public:
  explicit iq_compressor_selector(
      std::array<std::unique_ptr<iq_compressor>, NOF_COMPRESSION_TYPES_SUPPORTED> compressors_);

  // See interface for documentation.
  void compress(span<uint8_t> buffer, span<const cbf16_t> iq_data, const ru_compression_params& params) override;

private:
  std::array<std::unique_ptr<iq_compressor>, NOF_COMPRESSION_TYPES_SUPPORTED> compressors;
};

} // namespace ofh
} // namespace ocudu
