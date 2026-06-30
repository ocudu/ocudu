// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/adt/bounded_integer.h"
#include "ocudu/ran/configured_grant/cg_configuration.h"
#include <optional>

namespace ocudu {

struct cg_builder_params {
  /// If set, enables Configured Grants and sets its periodicity.
  /// Normal CP (14 symbols/slot).
  /// For 14 symbols slots, values={1, 2, 4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1024, 1280,
  /// 2560, 5120}.
  /// For 12 symbols slots, values={1, 2, 4, 5, 8, 10, 16, 20, 32, 40, 64, 80, 128, 160, 256, 320, 512, 640, 1280,
  /// 5120}.
  std::optional<cg_configuration::periodicity_t> periodicity = cg_configuration::periodicity_t::sl40;
  /// Number of RBs that are configured for the UE configured grant.
  unsigned nof_rbs = 10;
  /// MCS configured for the UE configured grant. Values: {1,...,27}.
  unsigned mcs = 5;
  /// Number of HARQ processes reserved for configured grant. Values: {1,...,16}.
  unsigned nof_harq_processes = 4;
  /// Number of RBs that are available for configured grants at cell-level. Values: {1,...,275}.
  unsigned max_nof_cell_cg_rbs = 10;
};

} // namespace ocudu
