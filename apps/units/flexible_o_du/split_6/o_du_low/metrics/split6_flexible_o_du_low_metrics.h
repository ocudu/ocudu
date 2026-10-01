// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_low/o_du_low_metrics.h"
#include "ocudu/ru/ru_metrics.h"

namespace ocudu {

/// Flexible O-RAN DU metrics.
struct split6_flexible_o_du_low_metrics {
  odu::o_du_low_metrics du_low;
  ru_metrics            ru;
};

} // namespace ocudu
