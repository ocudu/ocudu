// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

#include "ocudu/adt/span.h"
#include "ocudu/ran/uci/uci_part2_size_description.h"
#include <cstdint>
#include <vector>

namespace ocudu {

struct csi_report_configuration;

namespace fapi_adaptor {

/// UCI Part2 information fields that describe the UCI Part 1 correspondence to Part 2 sizes.
struct uci_part2_correspondence_information {
  /// Priority of the Part2 report.
  uint16_t priority;
  /// Part1 parameters offsets.
  static_vector<uint16_t, uci_part2_size_description::max_nof_parameters> part1_param_offsets;
  /// Part1 parameters sizes.
  static_vector<uint8_t, uci_part2_size_description::max_nof_parameters> part1_param_sizes;
  /// Map index into the repository.
  uint16_t part2_map_index;
  /// Map scope.
  uint8_t part2_map_scope;
};

/// \brief UCI Part2 correspondence mapper.
///
/// Maps the given arguments to a a list of UCI Part2 correspondence entries.
class uci_part2_correspondence_mapper
{
public:
  using correspondence_info_container =
      static_vector<uci_part2_correspondence_information, uci_part2_size_description::max_nof_entries>;

  explicit uci_part2_correspondence_mapper(std::vector<correspondence_info_container> correspondence_map_) :
    correspondence_map(std::move(correspondence_map_))
  {
    ocudu_assert(!correspondence_map.empty(), "Empty container");
  }

  /// Maps the given CSI report configuration into a list of correspondence entries.
  span<const uci_part2_correspondence_information> map(const csi_report_configuration& csi_report) const;

private:
  /// Each entry contains a list of UCI Part2 entries.
  std::vector<correspondence_info_container> correspondence_map;
};

} // namespace fapi_adaptor
} // namespace ocudu
