// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/signal_processors/srs/srs_estimator.h"
#include "ocudu/phy/upper/signal_processors/srs/srs_estimator_result.h"

namespace ocudu {

/// Dummy SRS estimator that always returns an empty result.
class srs_estimator_dummy : public srs_estimator
{
public:
  srs_estimator_result estimate(const resource_grid_reader& grid, const srs_estimator_configuration& config) override
  {
    return srs_estimator_result();
  }
};

/// Spy for \ref srs_estimator that tracks whether \c estimate() was called.
class srs_estimator_spy : public srs_estimator
{
public:
  srs_estimator_result estimate(const resource_grid_reader& grid, const srs_estimator_configuration& config) override
  {
    estimate_called = true;
    return srs_estimator_result();
  }

  bool has_estimate_method_been_called() const { return estimate_called; }

  void clear() { estimate_called = false; }

private:
  bool estimate_called = false;
};

} // namespace ocudu
