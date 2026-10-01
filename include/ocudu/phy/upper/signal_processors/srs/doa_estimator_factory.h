// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/phy/upper/signal_processors/srs/doa_estimator.h"
#include <memory>

namespace ocudu {

/// Factory class for Direction of Arrival (DOA) estimator for Sounding Reference Signals.
class doa_estimator_factory
{
public:
  /// Default destructor,
  virtual ~doa_estimator_factory() = default;

  /// Creates an DOA estimator.
  virtual std::unique_ptr<doa_estimator> create() = 0;
};

// Forward declaration of the DOA estimator configuration structure.
struct doa_estimator_configuration;

/// Creates a DOA estimator factory.
std::shared_ptr<doa_estimator_factory> create_doa_estimator_factory(const doa_estimator_configuration& config);

} // namespace ocudu
