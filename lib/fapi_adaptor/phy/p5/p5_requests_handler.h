// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/p5/p5_requests_gateway.h"
#include "ocudu/ocudulog/logger.h"
#include "ocudu/support/synchronization/stop_event.h"

namespace ocudu {
class task_executor;

class upper_phy_operation_controller;

namespace fapi {
class p5_responses_notifier;
class error_indication_notifier;
} // namespace fapi

namespace fapi_adaptor {

struct phy_fapi_p5_sector_fastpath_adaptor_config;
struct phy_fapi_p5_sector_fastpath_adaptor_dependencies;

/// \brief FAPI P5 requests handler.
///
/// This class process incoming FAPI P5 request messages (PARAM.request, CONFIG.request, START.request and STOP.request)
/// by dispatching the start/stop to the upper PHY operation controller and generating dummy responses for the
/// param/config requests.
class p5_requests_handler : public fapi::p5_requests_gateway
{
public:
  p5_requests_handler(const phy_fapi_p5_sector_fastpath_adaptor_config&       config,
                      const phy_fapi_p5_sector_fastpath_adaptor_dependencies& dependencies);

  ~p5_requests_handler() override;

  // See interface for documentation.
  void send_param_request(const fapi::param_request& msg) override;

  // See interface for documentation.
  void send_config_request(const fapi::config_request& msg) override;

  // See interface for documentation.
  void send_start_request(const fapi::start_request& msg) override;

  // See interface for documentation.
  void send_stop_request(const fapi::stop_request& msg) override;

  /// Sets the P5 responses notifier of this gateway.
  void set_p5_responses_notifier(fapi::p5_responses_notifier& p5_resp_notifier) { p5_notifier = &p5_resp_notifier; }

  /// Sets the error indication notifier of this gateway.
  void set_error_indication_notifier(fapi::error_indication_notifier& err_notifier) { error_notifier = &err_notifier; }

private:
  const unsigned                   sector;
  ocudulog::basic_logger&          logger;
  task_executor&                   executor;
  upper_phy_operation_controller&  upper_phy_controller;
  fapi::p5_responses_notifier*     p5_notifier    = nullptr;
  fapi::error_indication_notifier* error_notifier = nullptr;
  stop_event_source                stop_manager;
};

} // namespace fapi_adaptor
} // namespace ocudu
