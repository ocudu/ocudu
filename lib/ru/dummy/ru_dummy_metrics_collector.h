// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ru_dummy_sector.h"
#include "ocudu/adt/span.h"
#include "ocudu/ru/ru_metrics_collector.h"
#include <vector>

namespace ocudu {

/// Dummy RU metrics collector implementation
class ru_dummy_metrics_collector : public ru_metrics_collector
{
  std::vector<ru_dummy_sector*> sectors;

public:
  explicit ru_dummy_metrics_collector(std::vector<ru_dummy_sector*> sectors_) : sectors(std::move(sectors_)) {}

  // See interface for documentation.
  void collect_metrics(ru_metrics& metrics) override;
};

} // namespace ocudu
