// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "apps/units/flexible_o_du/o_du_low/o_du_low_unit_factory.h"
#include "fapi_adaptor/mac_fapi_p7_sector_adaptor_factory.h"
#include "split6_flexible_o_du_low_session.h"
#include "split6_o_du_low_unit_config.h"

namespace ocudu {

namespace fapi {
class p5_requests_gateway;
class p7_last_request_notifier;
class p7_requests_gateway;
struct cell_configuration;
} // namespace fapi

struct worker_manager;

/// Split 6 flexible O-DU low session factory.
class split6_flexible_o_du_low_session_factory
{
  /// Class to calculate the time start of the session.
  class start_time_calculator
  {
    /// Minimum value of the window in milliseconds.
    static constexpr std::chrono::milliseconds MINIMUM_WINDOW_SIZE_MS{5000};

    const std::chrono::milliseconds jitter;
    const std::chrono::milliseconds window_size;
    const std::chrono::milliseconds jitter_upper_border;
    ocudulog::basic_logger&         logger;

  public:
    start_time_calculator(std::chrono::milliseconds jitter_, ocudulog::basic_logger& logger_) :
      jitter(jitter_),
      window_size{std::max((jitter * 100U), MINIMUM_WINDOW_SIZE_MS)},
      jitter_upper_border(window_size - jitter),
      logger(logger_)
    {
    }

    /// \brief Returns the start time based on the class configuration.
    ///
    /// This algorithm calculates the start time based on the current time and a configured jitter. It consists on
    /// splitting the time in window_size and calculating the window for the current time. Then \c start_time is
    /// calculated at the end of the next window.
    ///
    ///                                                 border (will log a warning)
    ///           window     window    window             |
    ///    ...X|X........X|X........X|X.......X|X.......X|X...   time
    ///             |                |
    ///             |--------------->|
    ///        current_time       start_time
    ///
    /// Each window defines a lower and upper border, if the current time matches one of these borders, a warning
    /// message that the start might not be synchronized will be logged.
    /// In case that the configured jitter is 0, \c nullopt is returned.
    std::optional<std::chrono::system_clock::time_point> calculate_start_time() const;
  };

  const split6_o_du_low_unit_config                                 unit_config;
  worker_manager&                                                   workers;
  timer_manager&                                                    timers;
  split6_flexible_o_du_low_metrics_notifier*                        notifier;
  std::unique_ptr<fapi_adaptor::mac_fapi_p7_sector_adaptor_factory> p7_requests_adaptor_factory;
  start_time_calculator                                             start_time_calc;

public:
  split6_flexible_o_du_low_session_factory(
      split6_o_du_low_unit_config                                       unit_config_,
      worker_manager&                                                   workers_,
      timer_manager&                                                    timers_,
      split6_flexible_o_du_low_metrics_notifier*                        notifier_,
      std::unique_ptr<fapi_adaptor::mac_fapi_p7_sector_adaptor_factory> p7_requests_adaptor_factory_) :
    unit_config(std::move(unit_config_)),
    workers(workers_),
    timers(timers_),
    notifier(notifier_),
    p7_requests_adaptor_factory(std::move(p7_requests_adaptor_factory_)),
    start_time_calc(std::chrono::milliseconds{unit_config.start_time_jitter_ms}, ocudulog::fetch_basic_logger("APP"))
  {
    report_error_if_not(p7_requests_adaptor_factory, "Invalid FAPI P7 messages adaptor factory");
  }

  /// Creates the split 6 flexible O-DU low session.
  std::unique_ptr<split6_flexible_o_du_low_session> create_o_du_low_session(const fapi::cell_configuration& config);

private:
  /// Creates Radio Unit.
  std::unique_ptr<radio_unit> create_radio_unit(split6_flexible_o_du_low_session& odu_low,
                                                const fapi::cell_configuration&   config);

  /// Creates O-DU low.
  o_du_low_unit create_o_du_low(const fapi::cell_configuration&   config,
                                const o_du_low_unit_dependencies& odu_low_dependencies);
};

} // namespace ocudu
