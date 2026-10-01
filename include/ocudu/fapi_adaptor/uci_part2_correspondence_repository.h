// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/uci/uci_part2_size_description.h"
#include <vector>

namespace ocudu {
namespace fapi_adaptor {

/// \brief UCI Part2 sizes repository.
///
/// The repository stores arrays of Part2 sizes as a function of Part1 parameters.
class uci_part2_correspondence_repository
{
  /// Each entry contains a list of UCI Part2 sizes.
  std::vector<static_vector<uint16_t, uci_part2_size_description::max_size_table>> repo_map;

public:
  explicit uci_part2_correspondence_repository(
      std::vector<static_vector<uint16_t, uci_part2_size_description::max_size_table>> repo_map_) :
    repo_map(std::move(repo_map_))
  {
    ocudu_assert(!repo_map.empty(), "Empty container");
  }

  /// \brief Returns a list of Part2 sizes related to Part1 parameters associated to the given index.
  ///
  /// \note Index value must be valid.
  span<const uint16_t> get_uci_part2_correspondence(unsigned index) const;
};

} // namespace fapi_adaptor
} // namespace ocudu
