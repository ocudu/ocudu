// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "beamforming_weights_repository.h"
#include "../compression/beamforming_weights_compressor.h"
#include "ocudu/ofh/compression/compression_properties.h"
#include "ocudu/ofh/ofh_constants.h"
#include "ocudu/ran/beamforming/beam_weights_codebook.h"
#include "ocudu/support/ocudu_assert.h"

using namespace ocudu;
using namespace ofh;

/// Returns the size in Bytes of the compressed weights of a single beam of the given codebook.
static unsigned get_weights_size_per_beam(const beam_weights_codebook& codebook,
                                          const ru_compression_params& compr_params)
{
  return get_packed_beamforming_weights_size(codebook.get_nof_antennas(), compr_params).value();
}

beamforming_weights_repository::beamforming_weights_repository(const beam_weights_codebook& codebook,
                                                               const ru_compression_params& params) :
  compr_params(params),
  nof_beams(codebook.get_nof_beams()),
  weights_size_per_beam(get_weights_size_per_beam(codebook, params)),
  packed_weights(nof_beams * weights_size_per_beam)
{
  for (unsigned i_beam = 0; i_beam != nof_beams; ++i_beam) {
    beam_identifier beam_id = to_beam_id(i_beam);

    static_vector<cf_t, MAX_NOF_BEAMFORMING_WEIGHTS> coefficients =
        codebook.get_beam_coefficients<MAX_NOF_BEAMFORMING_WEIGHTS>(beam_id);

    span<uint8_t> beam_buffer =
        span<uint8_t>(packed_weights).subspan(i_beam * weights_size_per_beam, weights_size_per_beam);
    compress_beamforming_weights(beam_buffer, coefficients, compr_params);
  }
}

span<const uint8_t> beamforming_weights_repository::get_weights(beam_identifier beam_id) const
{
  unsigned i_beam = to_underlying(beam_id);
  ocudu_assert(i_beam < nof_beams,
               "Beam identifier '{}' is out of range for a repository holding '{}' beams",
               i_beam,
               nof_beams);

  return span<const uint8_t>(packed_weights).subspan(i_beam * weights_size_per_beam, weights_size_per_beam);
}
