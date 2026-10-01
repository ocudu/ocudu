// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ofh/ofh_metrics.h"
#include "ocudu/ru/dummy/ru_dummy_metrics.h"
#include "ocudu/ru/sdr/ru_sdr_metrics.h"
#include <variant>

namespace ocudu {

/// Radio Unit metrics.
struct ru_metrics {
  std::variant<ru_dummy_metrics, ru_sdr_metrics, ofh::metrics> metrics;
};

} // namespace ocudu
