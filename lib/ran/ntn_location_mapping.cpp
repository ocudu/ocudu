// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/ntn_location_mapping.h"
#include <algorithm>

using namespace ocudu;

std::optional<tac_t> ocudu::derive_tac_from_location(const ntn_location_mapping& mapping,
                                                     const reference_location&   position)
{
  if (mapping.empty()) {
    return std::nullopt;
  }

  // Areas may overlap, so the first match in configuration order wins.
  auto area = std::find_if(mapping.location_areas.begin(), mapping.location_areas.end(), [&position](const auto& a) {
    return a.contains(position);
  });
  if (area == mapping.location_areas.end()) {
    // An NTN footprint is large and coverage plans are approximate, so a position outside every area is a normal
    // case, not an error.
    return std::nullopt;
  }

  return area->tac;
}
