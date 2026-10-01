// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "flexible_o_du_metrics_aggregator.h"
#include "ntn_ru_doppler_adapter.h"
#include "ocudu/du/du.h"
#include "ocudu/du/du_operation_controller.h"
#include "ocudu/du/o_du.h"
#include "ocudu/ru/ru_adapters.h"
#include <memory>

namespace ocudu {

class radio_unit;

/// \brief Flexible O-RAN DU implementation.
///
/// One O-RAN DU can handle more than one cell.
class flexible_o_du_impl : public odu::du, public du_operation_controller
{
public:
  flexible_o_du_impl(unsigned nof_cells_, flexible_o_du_metrics_notifier* notifier);
  ~flexible_o_du_impl() override;

  // See interface for documentation.
  du_operation_controller& get_operation_controller() override { return *this; }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  /// Adds the given RU to this flexible O-RAN DU.
  void add_ru(std::unique_ptr<radio_unit> active_ru);

  /// Adds the given DU to this flexible O-RAN DU.
  void add_du(std::unique_ptr<odu::o_du> active_du);

  /// Returns the O-DU metrics notifier of this flexible O-DU implementation.
  odu::o_du_metrics_notifier& get_o_du_metrics_notifier() { return odu_metrics_handler; }

  /// Getters to the adaptors.
  upper_phy_ru_ul_adapter&         get_upper_ru_ul_adapter() { return ru_ul_adapt; }
  upper_phy_ru_timing_adapter&     get_upper_ru_timing_adapter() { return ru_timing_adapt; }
  upper_phy_ru_error_adapter&      get_upper_ru_error_adapter() { return ru_error_adapt; }
  upper_phy_ru_dl_rg_adapter&      get_upper_ru_dl_rg_adapter() { return ru_dl_rg_adapt; }
  upper_phy_ru_ul_request_adapter& get_upper_ru_ul_request_adapter() { return ru_ul_request_adapt; }
  ntn_ru_doppler_adapter&          get_ru_doppler_adapter() { return ru_doppler_adapt; }

private:
  const unsigned                   nof_cells;
  upper_phy_ru_ul_adapter          ru_ul_adapt;
  upper_phy_ru_timing_adapter      ru_timing_adapt;
  upper_phy_ru_error_adapter       ru_error_adapt;
  ntn_ru_doppler_adapter           ru_doppler_adapt;
  flexible_o_du_metrics_aggregator odu_metrics_handler;
  std::unique_ptr<odu::o_du>       du;
  std::unique_ptr<radio_unit>      ru;
  upper_phy_ru_dl_rg_adapter       ru_dl_rg_adapt;
  upper_phy_ru_ul_request_adapter  ru_ul_request_adapt;
};

} // namespace ocudu
