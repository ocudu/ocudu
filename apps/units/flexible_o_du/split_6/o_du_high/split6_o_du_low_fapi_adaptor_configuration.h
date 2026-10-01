// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ran/subcarrier_spacing.h"

namespace ocudu {
namespace fapi_adaptor {

/// Split 6 FAPI-specific cell configuration.
struct split6_o_du_low_fapi_adaptor_cell_config {
  subcarrier_spacing scs_common;
  uint16_t           num_tx_ant;
};

/// Split 6 FAPI-specific adaptor configuration.
struct split6_o_du_low_fapi_adaptor_configuration {
  std::vector<split6_o_du_low_fapi_adaptor_cell_config> cells;
};

} // namespace fapi_adaptor
} // namespace ocudu
