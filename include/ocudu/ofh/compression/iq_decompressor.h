// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/ofh/compression/compression_params.h"

namespace ocudu {
namespace ofh {

/// \brief Describes the IQ data decompressor.
///
/// Deserializes compression parameters and decompresses received PRBs with compressed IQ data according to compression
/// methods specified in WG4.CUS.0 document.
class iq_decompressor
{
public:
  /// Default destructor.
  virtual ~iq_decompressor() = default;

  /// \brief Decompress received compressed PRBs.
  ///
  /// Decompresses compressed PRBs from the input buffer according to received compression parameters and puts the
  /// results into an array of brain floating point IQ samples.
  ///
  /// \param[out] iq_data  Resulting IQ samples after decompression, unspecified when this returns false.
  /// \param[in]  compressed_data A span containing received compressed IQ data and compression parameters.
  /// \param[in]  params  Compression parameters.
  /// \return True on success, false if the IQ data could not be decompressed.
  virtual bool
  decompress(span<cbf16_t> iq_data, span<const uint8_t> compressed_data, const ru_compression_params& params) = 0;
};

} // namespace ofh
} // namespace ocudu
