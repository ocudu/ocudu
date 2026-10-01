// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/ofh/compression/compression_params.h"

namespace ocudu {
namespace ofh {

/// \brief Describes the IQ data compressor.
///
/// Compresses and serializes floating point IQ samples together with compression parameters according to compression
/// methods specified in WG4.CUS.0 document.
class iq_compressor
{
public:
  /// Default destructor.
  virtual ~iq_compressor() = default;

  /// \brief Compress input IQ samples.
  ///
  /// Compresses IQ samples from the input buffer according to received compression parameters and puts the results into
  /// an array of compressed PRBs.
  ///
  /// \param[out] buffer   Buffer where the compressed IQ data and compression parameters will be stored.
  /// \param[in]  iq_data  IQ samples to be compressed.
  /// \param[in]  params   Compression parameters.
  virtual void compress(span<uint8_t> buffer, span<const cbf16_t> iq_data, const ru_compression_params& params) = 0;
};

} // namespace ofh
} // namespace ocudu
