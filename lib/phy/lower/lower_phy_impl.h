// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lower_phy_baseband_processor.h"
#include "processors/adaptors/processor_handler_adaptor.h"
#include "processors/adaptors/processor_notifier_adaptor.h"
#include "ocudu/adt/circular_array.h"
#include "ocudu/gateways/baseband/baseband_gateway.h"
#include "ocudu/phy/lower/amplitude_controller/amplitude_controller.h"
#include "ocudu/phy/lower/lower_phy.h"
#include "ocudu/phy/lower/lower_phy_configuration.h"
#include "ocudu/phy/lower/lower_phy_controller.h"
#include "ocudu/phy/lower/lower_phy_downlink_handler.h"
#include "ocudu/phy/lower/lower_phy_error_notifier.h"
#include "ocudu/phy/lower/lower_phy_metrics_notifier.h"
#include "ocudu/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "ocudu/phy/lower/lower_phy_timing_notifier.h"
#include "ocudu/phy/lower/lower_phy_uplink_request_handler.h"
#include "ocudu/phy/lower/modulation/ofdm_modulator.h"
#include "ocudu/phy/lower/processors/downlink/downlink_processor.h"
#include "ocudu/phy/lower/processors/uplink/uplink_processor.h"
#include "ocudu/phy/support/resource_grid_pool.h"
#include "ocudu/support/math/stats.h"

namespace ocudu {

/// Lower physical layer implementation.
class lower_phy_impl : public lower_phy, private lower_phy_controller
{
public:
  /// Collects the injected dependencies of the lower physical layer.
  struct dependencies {
    /// Downlink processor.
    std::unique_ptr<lower_phy_downlink_processor> downlink_proc;
    /// Uplink processor.
    std::unique_ptr<lower_phy_uplink_processor> uplink_proc;
    /// Baseband controller.
    std::unique_ptr<lower_phy_controller> controller;
    /// Symbol handler to notify the reception of symbols.
    lower_phy_rx_symbol_notifier& rx_symbol_notifier;
    /// The timing handler to notify the timing boundaries.
    lower_phy_timing_notifier& timing_notifier;
    /// Error handler to notify runtime errors.
    lower_phy_error_notifier& error_notifier;
    /// Metrics handler to notify metrics.
    lower_phy_metrics_notifier& metrics_notifier;
  };

  /// Constructs a generic lower physical layer.
  explicit lower_phy_impl(dependencies deps);

  // See interface for documentation.
  lower_phy_controller& get_controller() override { return *this; }

  // See interface for documentation.
  lower_phy_uplink_request_handler& get_uplink_request_handler() override
  {
    return handler_adaptor.get_uplink_request_handler();
  }

  // See interface for documentation.
  lower_phy_downlink_handler& get_downlink_handler() override { return handler_adaptor.get_downlink_handler(); }

  // See interface for documentation.
  lower_phy_cfo_controller& get_tx_cfo_control() override;

  // See interface for documentation.
  lower_phy_cfo_controller& get_rx_cfo_control() override;

  // See interface for documentation.
  lower_phy_center_freq_controller& get_tx_center_freq_control() override;

  // See interface for documentation.
  lower_phy_center_freq_controller& get_rx_center_freq_control() override;

  // See interface for documentation.
  lower_phy_tx_time_offset_controller& get_tx_time_offset_control() override;

  // See lower_phy_controller interface for documentation.
  void start(baseband_gateway_timestamp init_time, baseband_gateway_timestamp sfn0_ref_time) override
  {
    controller->start(init_time, sfn0_ref_time);
  }

  // See lower_phy_controller interface for documentation.
  void stop() override
  {
    downlink_proc->stop();
    uplink_proc->stop();
    controller->stop();
  }

private:
  /// Processor notification adaptor.
  processor_notifier_adaptor notification_adaptor;
  /// Downlink processor.
  std::unique_ptr<lower_phy_downlink_processor> downlink_proc;
  /// Uplink processor.
  std::unique_ptr<lower_phy_uplink_processor> uplink_proc;
  /// Processor handler adaptor.
  processor_handler_adaptor handler_adaptor;
  /// Lower physical layer controller.
  std::unique_ptr<lower_phy_controller> controller;
};

} // namespace ocudu
