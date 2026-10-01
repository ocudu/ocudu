// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "configuration_procedure.h"
#include "ocudu/fapi/common/error_indication.h"
#include "ocudu/fapi/common/error_indication_notifier.h"
#include "ocudu/fapi/p5/p5_messages.h"
#include "ocudu/fapi/p5/p5_responses_notifier.h"

using namespace ocudu;
using namespace fapi;

namespace {

/// Dummy FAPI P5 responses notifier.
class p5_notifier_dummy : public p5_responses_notifier
{
public:
  void on_param_response(const param_response& msg) override
  {
    report_error("Dummy FAPI config message notifier cannot notify on PARAM.response");
  }

  void on_config_response(const config_response& msg) override
  {
    report_error("Dummy FAPI config message notifier cannot notify on CONFIG.response");
  }

  void on_stop_indication(const stop_indication& msg) override
  {
    report_error("Dummy FAPI config message notifier cannot notify on STOP.indication");
  }
};

class error_indication_notifier_dummy : public error_indication_notifier
{
public:
  void on_error_indication(const error_indication& msg) override
  {
    report_error("Dummy FAPI ERROR.indication notifier cannot notify errors");
  }
};

class p5_operational_change_request_notifier_dummy : public p5_operational_change_request_notifier
{
public:
  bool on_start_request(const cell_configuration& config) override { return false; }
  void on_stop_request() override {}
};

} // namespace

static p5_notifier_dummy                            p5_dummy_notifier;
static error_indication_notifier_dummy              p5_error_notifier;
static p5_operational_change_request_notifier_dummy p5_operational_change_notifier;

configuration_procedure::configuration_procedure(ocudulog::basic_logger& logger_) :
  logger(logger_),
  p5_notifier(&p5_dummy_notifier),
  error_notifier(&p5_error_notifier),
  cell_operation_notifier(&p5_operational_change_notifier)
{
}

void configuration_procedure::send_param_request(const fapi::param_request& msg)
{
  // Do nothing for the PARAM.response.
  param_response response;

  // If current status is running, report back error code, as per SCF-222 v4.0 section 3.3.1.3.
  response.error_code = (status == cell_status::RUNNING) ? error_code_id::msg_invalid_state : error_code_id::msg_ok;

  p5_notifier->on_param_response(response);
}

void configuration_procedure::send_config_request(const fapi::config_request& msg)
{
  cell_status current_status = status;

  bool config_updated = false;

  // [Implementation defined] Do not allow configuring the cell in the running state.
  if (current_status != cell_status::RUNNING) {
    config_updated = update_cell_config(msg);
  }

  config_response response;
  response.error_code = config_updated ? error_code_id::msg_ok : error_code_id::msg_invalid_config;

  // Move cell to configured status if current status is idle and the configuration update was successful.
  if ((current_status == cell_status::IDLE) && config_updated) {
    status = cell_status::CONFIGURED;
  }

  p5_notifier->on_config_response(response);
}

void configuration_procedure::send_start_request(const fapi::start_request& msg)
{
  cell_status current_status = status;

  // Unexpected status, notify error.
  if (current_status != cell_status::CONFIGURED) {
    error_indication indication;
    indication.message_id = message_type_id::start_request;
    indication.error_code = error_code_id::msg_invalid_state;
    error_notifier->on_error_indication(indication);

    return;
  }

  // Status is configured. Create DU low and set the status to start.
  if (!cell_operation_notifier->on_start_request(cell_cfg)) {
    logger.error("Failed to start cell id '{}'", cell_cfg.pci);
    error_indication indication;
    indication.message_id = message_type_id::start_request;
    indication.error_code = error_code_id::msg_invalid_config;
    error_notifier->on_error_indication(indication);

    return;
  }

  // Set the cell status to running.
  status = cell_status::RUNNING;
}

void configuration_procedure::send_stop_request(const fapi::stop_request& msg)
{
  cell_status current_status = status;

  // Unexpected status, notify error.
  if (current_status != cell_status::RUNNING) {
    error_indication indication;
    indication.message_id = message_type_id::stop_request;
    indication.error_code = error_code_id::msg_invalid_state;
    error_notifier->on_error_indication(indication);

    return;
  }

  // Status is configured. Create DU low and set the status to start.
  cell_operation_notifier->on_stop_request();

  // Set the cell status to configured.
  status = cell_status::CONFIGURED;

  stop_indication indication;
  p5_notifier->on_stop_indication(indication);
}

bool configuration_procedure::update_cell_config(const fapi::config_request& msg)
{
  // Update the configuration.
  cell_cfg = msg.cell_cfg;

  return true;
}
