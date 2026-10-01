// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/uci/uci_part2_size_calculator.h"

using namespace ocudu;

static unsigned extract_parameter(unsigned offset, unsigned width, const uci_payload_type& payload)
{
  // Verify the payload read range is valid.
  ocudu_assert(offset + width <= payload.size(),
               "The offset (i.e., {}) plus the width (i.e., {}) exceeds the payload size (i.e., {}).",
               offset,
               width,
               payload.size());

  // Ignore for zero width.
  if (width == 0) {
    return 0;
  }

  // Get slice of the CSI Part 1.
  uci_payload_type slice = payload.slice(offset, offset + width);

  // Extract parameter value.
  uint64_t param = slice.to_uint64();

  // Reverse parameter bits.
  param = bit_reverse(param) >> (64 - width);

  return static_cast<unsigned>(param);
}

units::bits ocudu::uci_part2_get_size(const uci_payload_type& part1, const uci_part2_size_description& descr)
{
  unsigned result = 0;

  // Iterate all entries.
  for (const uci_part2_size_description::entry& entry : descr.entries) {
    unsigned index          = 0;
    unsigned index_bitwidth = 0;

    for (const uci_part2_size_description::parameter& parameter : entry.parameters) {
      // Extract the value of the parameter.
      unsigned value = extract_parameter(parameter.offset, parameter.width, part1);

      // Combine the parameter value with the current index.
      index = (index << parameter.width) | value;

      // Accumulate the index bit width.
      index_bitwidth += parameter.width;
    }

    // Verify the map size is according to the index bit width.
    ocudu_assert(entry.map.size() == (1U << index_bitwidth),
                 "Invalid map size (i.e., {}), expected {} entries.",
                 entry.map.size(),
                 (1U << index_bitwidth));

    // Verify that the index is within the map size.
    ocudu_assert(
        index < entry.map.size(), "Index value (i.e., {}) exceeds the map size (i.e., {}).", index, entry.map.size());

    // Add the Part 2 size corresponding to this entry.
    result += entry.map[index];
  }

  return units::bits(result);
}
