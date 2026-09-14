// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/ntn_location_mapping.h"
#include <algorithm>

using namespace ocudu;

/// Returns the configured area holding the position, or nullptr when the mapping cannot place it.
static const ntn_location_area* find_area(const ntn_location_mapping& mapping, const reference_location& position)
{
  if (mapping.empty()) {
    return nullptr;
  }

  // Areas may overlap, so the first match in configuration order wins.
  auto area = std::find_if(mapping.location_areas.begin(), mapping.location_areas.end(), [&position](const auto& a) {
    return a.contains(position);
  });
  if (area == mapping.location_areas.end()) {
    // An NTN footprint is large and coverage plans are approximate, so a position outside every area is a normal
    // case, not an error.
    return nullptr;
  }

  return &*area;
}

std::optional<tac_t> ocudu::derive_tac_from_location(const ntn_location_mapping& mapping,
                                                     const reference_location&   position)
{
  const ntn_location_area* area = find_area(mapping, position);
  if (area == nullptr) {
    return std::nullopt;
  }

  if (not area->tac.has_value()) {
    return std::nullopt;
  }

  return area->tac;
}

std::optional<nr_cell_identity> ocudu::derive_mapped_cell_id_from_location(const ntn_location_mapping& mapping,
                                                                           const reference_location&   position)
{
  const ntn_location_area* area = find_area(mapping, position);
  if (area == nullptr) {
    return std::nullopt;
  }

  return area->mapped_nci;
}
