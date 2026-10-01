// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ru_ofh_controller_impl.h"
#include "ru_ofh_downlink_plane_handler_proxy.h"
#include "ru_ofh_error_handler_impl.h"
#include "ru_ofh_metrics_collector_impl.h"
#include "ru_ofh_rx_symbol_handler_impl.h"
#include "ru_ofh_timing_notifier_impl.h"
#include "ru_ofh_uplink_plane_handler_proxy.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/ofh/ofh_sector.h"
#include "ocudu/ofh/timing/ofh_timing_manager.h"
#include "ocudu/ru/ofh/ru_ofh_configuration.h"
#include "ocudu/ru/ru.h"
#include "ocudu/ru/ru_timing_notifier.h"
#include "ocudu/ru/ru_uplink_plane.h"

namespace ocudu {

/// Open Fronthaul implementation configuration.
struct ru_ofh_impl_config {
  unsigned           nof_slot_offset_du_ru;
  unsigned           nof_symbols_per_slot;
  subcarrier_spacing scs;
};

/// Open Fronthaul implementation dependencies.
struct ru_ofh_impl_dependencies {
  ocudulog::basic_logger*              logger;
  std::unique_ptr<ofh::timing_manager> timing_mngr;
  ru_timing_notifier*                  timing_notifier    = nullptr;
  ru_error_notifier*                   error_notifier     = nullptr;
  ru_uplink_plane_rx_symbol_notifier*  rx_symbol_notifier = nullptr;
};

/// Open Fronthaul Radio Unit implementation.
class ru_ofh_impl : public radio_unit
{
public:
  ru_ofh_impl(const ru_ofh_impl_config& config, ru_ofh_impl_dependencies&& dependencies);

  // See interface for documentation.
  ru_controller& get_controller() override;

  // See interface for documentation.
  ru_downlink_plane_handler& get_downlink_plane_handler() override;

  // See interface for documentation.
  ru_uplink_plane_handler& get_uplink_plane_handler() override;

  // See interface for documentation.
  ru_metrics_collector* get_metrics_collector() override;

  /// Returns the error notifier of this RU.
  ofh::error_notifier& get_error_notifier() { return error_handler; }

  /// Returns the Open Fronthaul received symbol notifier of this Radio Unit.
  ofh::uplane_rx_symbol_notifier& get_uplane_rx_symbol_notifier() { return rx_symbol_handler; }

  /// Sets the OFH sectors managed by this RU.
  void set_ofh_sectors(std::vector<std::unique_ptr<ofh::sector>> ofh_sectors);

private:
  ru_ofh_timing_notifier_impl               timing_notifier;
  ru_ofh_error_handler_impl                 error_handler;
  ru_ofh_rx_symbol_handler_impl             rx_symbol_handler;
  std::unique_ptr<ofh::timing_manager>      ofh_timing_mngr;
  std::vector<std::unique_ptr<ofh::sector>> sectors;
  ru_ofh_controller_impl                    controller;
  ru_downlink_plane_handler_proxy           downlink_handler;
  ru_uplink_plane_handler_proxy             uplink_handler;
  ru_ofh_metrics_collector_impl             metrics_collector;
};

} // namespace ocudu
