// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/gnb_constants.h"
#include <type_traits>

namespace ocudu {

/// Maximum number of cells supported by DU (implementation-defined).
enum du_cell_index_t : uint16_t {
  MIN_DU_CELL_INDEX     = 0,
  MAX_DU_CELL_INDEX     = MAX_CELLS_PER_DU - 1,
  MAX_NOF_DU_CELLS      = MAX_CELLS_PER_DU,
  INVALID_DU_CELL_INDEX = MAX_NOF_DU_CELLS
};

/// Convert integer to DU cell index type.
constexpr du_cell_index_t to_du_cell_index(std::underlying_type_t<du_cell_index_t> idx)
{
  return static_cast<du_cell_index_t>(idx);
}

constexpr std::underlying_type_t<du_cell_index_t> format_as(du_cell_index_t cell_idx)
{
  return static_cast<std::underlying_type_t<du_cell_index_t>>(cell_idx);
}

} // namespace ocudu
