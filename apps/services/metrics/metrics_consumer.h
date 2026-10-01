// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace app_services {

/// \brief Metrics subscriber.
///
/// A subscriber will listen to a configured metric. The metric type is passed as a template
/// parameter, so consumers receive the concrete type directly, with no downcast required.
template <typename MetricType>
class metrics_consumer
{
public:
  /// The concrete metric type this consumer subscribes to.
  using metric_type = MetricType;

  /// Default destructor.
  virtual ~metrics_consumer() = default;

  /// Handles the given metrics.
  virtual void handle_metric(const MetricType& metric) = 0;
};

} // namespace app_services
} // namespace ocudu
