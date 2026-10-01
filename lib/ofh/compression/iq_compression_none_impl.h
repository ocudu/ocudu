// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/compression/iq_compressor.h"
#include "ocudu/ofh/compression/iq_decompressor.h"

namespace ocudu {
namespace ofh {

/// Implementation of the IQ data compression by quantizing floating point numbers without compression.
class iq_compression_none_impl : public iq_compressor, public iq_decompressor
{
public:
  // Constructor.
  explicit iq_compression_none_impl(ocudulog::basic_logger& logger_, float iq_scaling_ = 1.0) :
    logger(logger_), iq_scaling(iq_scaling_)
  {
  }

  // See interface for the documentation.
  virtual void
  compress(span<uint8_t> buffer, span<const cbf16_t> iq_data, const ru_compression_params& params) override;

  // See interface for the documentation.
  virtual bool
  decompress(span<cbf16_t> iq_data, span<const uint8_t> compressed_data, const ru_compression_params& params) override;

protected:
  /// \brief Prints to the log the root mean square (RMS) value of the given samples.
  ///
  /// \param[in] samples - Quantized samples.
  void log_post_quantization_rms(span<const int16_t> samples);

  ocudulog::basic_logger& logger;
  /// Scaling factor applied to IQ data prior to quantization.
  const float iq_scaling;
};

} // namespace ofh
} // namespace ocudu
