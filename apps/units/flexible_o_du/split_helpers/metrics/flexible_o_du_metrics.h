// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/o_du_metrics.h"
#include "ocudu/ru/ru_metrics.h"

namespace ocudu {

/// Flexible O-RAN DU metrics.
struct flexible_o_du_metrics {
  odu::o_du_metrics du;
  ru_metrics        ru;
};

} // namespace ocudu
