// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "du_low_metrics_collector_impl.h"
#include "ocudu/du/du_low/du_low.h"
#include "ocudu/du/du_operation_controller.h"
#include "ocudu/phy/upper/upper_phy.h"
#include <memory>
#include <vector>

namespace ocudu {
namespace odu {

/// DU low implementation.
class du_low_impl final : public du_low, public du_operation_controller
{
public:
  explicit du_low_impl(std::vector<std::unique_ptr<upper_phy>> upper_);

  // See interface for documentation.
  upper_phy& get_upper_phy(unsigned cell_id) override;

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  du_low_metrics_collector* get_metrics_collector() override;

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::vector<std::unique_ptr<upper_phy>> upper;
  du_low_metrics_collector_impl           metrics_collector;
};

} // namespace odu
} // namespace ocudu
