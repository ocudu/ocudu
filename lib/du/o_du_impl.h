// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/o_du_high.h"
#include "ocudu/du/du_high/o_du_high_metrics_notifier.h"
#include "ocudu/du/du_low/o_du_low.h"
#include "ocudu/du/du_operation_controller.h"
#include "ocudu/du/o_du.h"
#include "ocudu/du/o_du_config.h"
#include <memory>

namespace ocudu {
namespace odu {

/// O-RAN DU implementation.
class o_du_impl final : public o_du, public du_operation_controller, public o_du_high_metrics_notifier
{
public:
  explicit o_du_impl(o_du_dependencies&& dependencies);

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void on_new_metrics(const o_du_high_metrics& metrics) override;

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  o_du_high& get_o_du_high() override;

  // See interface for documentation.
  o_du_low& get_o_du_low() override;

private:
  o_du_metrics_notifier&     metrics_notifier;
  std::unique_ptr<o_du_high> odu_hi;
  std::unique_ptr<o_du_low>  odu_lo;
};

} // namespace odu
} // namespace ocudu
