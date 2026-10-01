// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/o_du_high.h"
#include "ocudu/du/du_low/o_du_low.h"
#include <memory>

namespace ocudu {
namespace odu {

class o_du_metrics_notifier;

/// O-RAN Distributed Unit dependencies.
struct o_du_dependencies {
  o_du_metrics_notifier*     metrics_notifier = nullptr;
  std::unique_ptr<o_du_high> odu_hi;
  std::unique_ptr<o_du_low>  odu_lo;
};

} // namespace odu
} // namespace ocudu
