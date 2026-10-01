// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

namespace ocudu {
namespace app_services {

/// \brief Typed interface used by metric producers to push new metric reports.
///
/// The concrete metric type is a template parameter, so a producer hands the report over through
/// its exact type: there is no common base class, no type erasure and no cast anywhere on the push
/// path. Passing a consumer of another metric type simply does not compile.
///
/// The report is taken **by value**, i.e. the caller hands the report over to the notifier:
/// passing an rvalue (the usual producer pattern, e.g.
/// \c notifier.on_new_metric(my_metrics_impl(report))) transfers the report without copying it,
/// while passing an lvalue makes exactly one copy, at the call boundary.
///
/// \note Pass the exact concrete \c *_metrics_impl type, never a derived type: value semantics
/// slice the object, and the report is delivered to the consumers of \c MetricType only. All
/// \c *_metrics_impl types are \c final, so a derived type cannot be passed accidentally.
template <typename MetricType>
class metrics_notifier
{
public:
  /// Default destructor.
  virtual ~metrics_notifier() = default;

  /// \brief Notifies a new metric report of the concrete type \c MetricType.
  virtual void on_new_metric(MetricType metric) = 0;
};

} // namespace app_services
} // namespace ocudu
