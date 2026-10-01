// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/fapi/cell_config.h"
#include "ocudu/fapi/p5/p5_operational_change_request_notifier.h"
#include "ocudu/fapi/p5/p5_requests_gateway.h"
#include "ocudu/ocudulog/logger.h"

namespace ocudu {
namespace fapi {

class error_indication_notifier;
class p5_responses_notifier;

/// FAPI configuration procedure.
class configuration_procedure : public p5_requests_gateway
{
  enum class cell_status : uint8_t { IDLE, CONFIGURED, RUNNING };

public:
  explicit configuration_procedure(ocudulog::basic_logger& logger_);

  // See interface for documentation.
  void send_param_request(const fapi::param_request& msg) override;

  // See interface for documentation.
  void send_config_request(const fapi::config_request& msg) override;

  // See interface for documentation.
  void send_start_request(const fapi::start_request& msg) override;

  // See interface for documentation.
  void send_stop_request(const fapi::stop_request& msg) override;

  /// Sets the FAPI P5 responses notifier to the given one.
  void set_p5_responses_notifier(p5_responses_notifier& config_notifier) { p5_notifier = &config_notifier; }

  /// Sets the error indication notifier to the given one.
  void set_error_indication_notifier(error_indication_notifier& err_notifier) { error_notifier = &err_notifier; }

  /// Sets the cell operation request notifier to the given one.
  void set_cell_operation_request_notifier(p5_operational_change_request_notifier& cell_notifier)
  {
    cell_operation_notifier = &cell_notifier;
  }

private:
  /// Updates the cell config using the given FAPI CONFIG.request message and returns true if the config was
  /// successfully updated, otherwise false.
  bool update_cell_config(const fapi::config_request& msg);

private:
  ocudulog::basic_logger&                 logger;
  cell_configuration                      cell_cfg;
  p5_responses_notifier*                  p5_notifier             = nullptr;
  error_indication_notifier*              error_notifier          = nullptr;
  p5_operational_change_request_notifier* cell_operation_notifier = nullptr;
  cell_status                             status                  = cell_status::IDLE;
};

} // namespace fapi
} // namespace ocudu
