// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_metrics_notifier.h"
#include "ocudu/du/du_high/o_du_high_metrics_notifier.h"

namespace ocudu {
namespace odu {

/// Proxy class that listen to scheduler metrics and forwards them to the configured scheduler metrics notifier and O-DU
/// high metrics notifier.
class o_du_high_metrics_notifier_proxy : public du_metrics_notifier
{
  du_metrics_notifier*        du_notifier     = nullptr;
  o_du_high_metrics_notifier* odu_hi_notifier = nullptr;

public:
  explicit o_du_high_metrics_notifier_proxy(du_metrics_notifier* notifier_);

  // See interface for documentation.
  void on_new_metric_report(const du_metrics_report& report) override;

  /// Sets the O-DU high metrics notifier to the given one.
  void set_o_du_high_metrics_notifier(o_du_high_metrics_notifier& notifier);
};

} // namespace odu
} // namespace ocudu
