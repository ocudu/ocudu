// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ofh/compression/compression_params.h"
#include "ocudu/ran/resource_block.h"
#include "ocudu/support/units.h"

namespace ocudu {
namespace ofh {

/// Returns true if the compression parameter is present based on the given compression type.
constexpr bool is_compression_param_present(compression_type type)
{
  return (type == compression_type::BFP) || (type == compression_type::mu_law) ||
         (type == compression_type::bfp_selective) || (type == compression_type::mod_selective);
}

/// Returns size of a PRB compressed according to the given compression parameters.
inline units::bytes get_compressed_prb_size(const ru_compression_params& params)
{
  static constexpr units::bytes compr_param_size{1};

  unsigned prb_size = NOF_SUBCARRIERS_PER_RB * 2 * params.data_width;
  if (is_compression_param_present(params.type)) {
    prb_size += compr_param_size.to_bits().value();
  }
  return units::bits(prb_size).round_up_to_bytes();
}

/// \brief Returns the size of a set of beamforming weights packed with the given compression parameters.
///
/// The size comprises the optional bfwCompParam field and the bfwI and bfwQ pair of each weight, see O-RAN.WG4.CUS,
/// 7.7.1.
inline units::bytes get_packed_beamforming_weights_size(unsigned nof_weights, const ru_compression_params& params)
{
  static constexpr units::bytes compr_param_size{1};

  units::bytes size = units::bits(nof_weights * 2 * params.data_width).round_up_to_bytes();
  if (is_compression_param_present(params.type)) {
    size += compr_param_size;
  }

  return size;
}

} // namespace ofh
} // namespace ocudu
