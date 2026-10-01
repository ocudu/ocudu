// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/ran/uci/uci_part2_size_description.h"

namespace ocudu {
namespace fapi {

/// UCI information for determining UCI Part1 to Part2 correspondence.
struct uci_part1_to_part2_correspondence {
  struct part2_info {
    static_vector<uint16_t, 4> param_offsets;
    static_vector<uint8_t, 4>  param_sizes;
    uint16_t                   part2_size_map_index;
  };

  static_vector<part2_info, uci_part2_size_description::max_nof_entries> part2;
};

} // namespace fapi
} // namespace ocudu
