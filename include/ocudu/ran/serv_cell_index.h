// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include <cstdint>
#include <type_traits>

namespace ocudu {

/// \c ServCellIndex, as per TS 38.331. It concerns a short identity, used to uniquely identify a serving cell (from
/// a UE's perspective) across cell groups. Value 0 applies to the PCell (Master Cell Group).
enum serv_cell_index_t : uint8_t {
  SERVING_PCELL_IDX     = 0,
  MAX_SERVING_CELL_IDX  = 31,
  MAX_NOF_SCELLS        = 31,
  MAX_NOF_SERVING_CELLS = 32,
  SERVING_CELL_INVALID  = MAX_NOF_SERVING_CELLS
};

constexpr serv_cell_index_t to_serv_cell_index(std::underlying_type_t<serv_cell_index_t> val)
{
  return static_cast<serv_cell_index_t>(val);
}

constexpr std::underlying_type_t<serv_cell_index_t> format_as(serv_cell_index_t ue_idx)
{
  return static_cast<std::underlying_type_t<serv_cell_index_t>>(ue_idx);
}

} // namespace ocudu
