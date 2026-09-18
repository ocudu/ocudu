// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ofh/compression/compression_params.h"
#include "ocudu/ran/beamforming/beam_identifier.h"
#include <cstdint>
#include <vector>

namespace ocudu {

class beam_weights_codebook;

namespace ofh {

/// \brief Repository of beamforming weights, ready to be sent over the C-Plane Section Extension 1.
///
/// Every beam of the given codebook is compressed once at construction time, so that Section Extension 1 serializer
/// never compresses or allocates at run time.
class beamforming_weights_repository
{
public:
  beamforming_weights_repository(const beam_weights_codebook& codebook, const ru_compression_params& params);

  /// \brief Returns the compressed weights of the given beam.
  ///
  /// \remark The returned span of compressed weights includes optional compression parameter.
  span<const uint8_t> get_weights(beam_identifier beam_id) const;

  /// Returns the compression parameters applied to the stored weights.
  const ru_compression_params& get_compression_params() const { return compr_params; }

private:
  const ru_compression_params compr_params;
  unsigned                    nof_beams;
  unsigned                    weights_size_per_beam;
  std::vector<uint8_t>        packed_weights;
};

} // namespace ofh
} // namespace ocudu
