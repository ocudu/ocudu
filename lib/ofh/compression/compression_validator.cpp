// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/ofh/compression/compression_validator.h"
#include "ocudu/adt/format.h"
#include "ocudu/ofh/compression/compression_params.h"
#include <algorithm>
#include <array>

using namespace ocudu;
using namespace ofh;

error_type<std::string> ofh::validate_compression_params(const ru_compression_params& params)
{
  if ((params.type != compression_type::none) && (params.type != compression_type::BFP)) {
    return make_unexpected(
        fmt::format("compression method '{}' is not supported. Valid values are [none,bfp]", to_string(params.type)));
  }

  if ((params.type == compression_type::none) && (params.data_width < MIN_IQ_WIDTH)) {
    return make_unexpected(
        fmt::format("compression bit width '{}' is not supported without compression. The minimum is {}",
                    params.data_width,
                    MIN_IQ_WIDTH));
  }

  static constexpr std::array<unsigned, 5> supported_bfp_bitwidths = {8, 9, 12, 14, 16};
  if ((params.type == compression_type::BFP) &&
      (std::find(supported_bfp_bitwidths.begin(), supported_bfp_bitwidths.end(), params.data_width) ==
       supported_bfp_bitwidths.end())) {
    return make_unexpected(fmt::format(
        "BFP compression bit width '{}' is not supported. Valid values are [8,9,12,14,16]", params.data_width));
  }

  return default_success_t();
}
