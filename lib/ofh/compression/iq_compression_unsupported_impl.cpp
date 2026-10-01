// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "iq_compression_unsupported_impl.h"
#include "ocudu/support/error_handling.h"

using namespace ocudu;
using namespace ofh;

void iq_compression_unsupported_impl::compress(span<uint8_t>                buffer,
                                               span<const cbf16_t>          iq_data,
                                               const ru_compression_params& params)
{
  report_error("Compression type '{}' is not supported", to_string(params.type));
}

bool iq_compression_unsupported_impl::decompress(span<cbf16_t>                iq_data,
                                                 span<const uint8_t>          compressed_data,
                                                 const ru_compression_params& params)
{
  return false;
}
