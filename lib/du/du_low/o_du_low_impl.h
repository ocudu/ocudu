// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "o_du_low_metrics_collector_impl.h"
#include "ocudu/du/du_low/du_low.h"
#include "ocudu/du/du_low/o_du_low.h"
#include "ocudu/du/du_operation_controller.h"
#include "ocudu/fapi_adaptor/phy/phy_fapi_fastpath_adaptor.h"
#include <memory>

namespace ocudu {
namespace odu {

class o_du_low_impl final : public o_du_low, public du_operation_controller
{
public:
  o_du_low_impl(std::unique_ptr<du_low>                                  du_lo_,
                std::unique_ptr<fapi_adaptor::phy_fapi_fastpath_adaptor> fapi_adaptor_,
                unsigned                                                 nof_cells);

  // See interface for documentation.
  du_low& get_du_low() override;

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  fapi_adaptor::phy_fapi_fastpath_adaptor& get_phy_fapi_fastpath_adaptor() override;

  // See interface for documentation.
  o_du_low_metrics_collector_impl* get_metrics_collector() override;

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  std::unique_ptr<du_low>                                  du_lo;
  std::unique_ptr<fapi_adaptor::phy_fapi_fastpath_adaptor> fapi_fastpath_adaptor;
  o_du_low_metrics_collector_impl                          metrics_collector;
};

} // namespace odu
} // namespace ocudu
