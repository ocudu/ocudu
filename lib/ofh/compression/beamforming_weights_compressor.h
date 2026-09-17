// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/complex.h"
#include "ocudu/adt/span.h"
#include "ocudu/ofh/compression/compression_params.h"

namespace ocudu {
namespace ofh {

/// \brief Compresses the beamforming weights of a single beam.
///
/// Quantizes and compresses the given weights and serializes them as the optional bfwCompParam field followed by the
/// bfwI and bfwQ pair of each weight, as required by the C-Plane section extension 1, see O-RAN.WG4.CUS, 7.7.1.
///
/// Unlike the U-Plane compression, the compression block is the entire weight vector instead of a resource block,
/// see O-RAN.WG4.CUS, 7.7.1.2.
///
/// Supported compression methods are \c compression_type::none and \c compression_type::BFP.
///
/// \param[out] buffer       Buffer where the compressed weights are stored.
/// \param[in]  weights      Beamforming weights of a beam, one per O-RU TRX.
/// \param[in]  compr_params Compression parameters applied to the weights.
/// \remark Asserts if the buffer size does not match the size given by \ref get_packed_beamforming_weights_size.
void compress_beamforming_weights(span<uint8_t>                buffer,
                                  span<const cf_t>             weights,
                                  const ru_compression_params& compr_params);

} // namespace ofh
} // namespace ocudu
