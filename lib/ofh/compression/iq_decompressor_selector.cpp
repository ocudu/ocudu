// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "iq_decompressor_selector.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;
using namespace ofh;

iq_decompressor_selector::iq_decompressor_selector(
    std::array<std::unique_ptr<iq_decompressor>, NOF_COMPRESSION_TYPES_SUPPORTED> decompressors_) :
  decompressors(std::move(decompressors_))
{
  // Sanity check that all the positions in the array has a decompressor.
  for (unsigned i = 0, e = decompressors.size(); i != e; ++i) {
    report_fatal_error_if_not(decompressors[i],
                              "Null decompressor detected for compression type '{}'",
                              to_string(static_cast<compression_type>(i)));
  }
}

bool iq_decompressor_selector::decompress(span<cbf16_t>                iq_data,
                                          span<const uint8_t>          compressed_data,
                                          const ru_compression_params& params)
{
  return decompressors[static_cast<unsigned>(params.type)]->decompress(iq_data, compressed_data, params);
}
