// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "p5_requests_handler.h"
#include "ocudu/fapi/common/error_indication_notifier.h"
#include "ocudu/fapi/p5/p5_messages.h"
#include "ocudu/fapi/p5/p5_responses_notifier.h"
#include "ocudu/fapi_adaptor/phy/p5/phy_fapi_p5_sector_fastpath_adaptor_config.h"
#include "ocudu/phy/upper/upper_phy_operation_controller.h"
#include "ocudu/support/executors/task_executor.h"

using namespace ocudu;
using namespace fapi_adaptor;

namespace {

class p5_notifier_dummy : public fapi::p5_responses_notifier
{
public:
  void on_config_response(const fapi::config_response& msg) override {}
  void on_param_response(const fapi::param_response& msg) override {}
  void on_stop_indication(const fapi::stop_indication& msg) override {}
};

class error_indication_notifier_dummy : public fapi::error_indication_notifier
{
public:
  void on_error_indication(const fapi::error_indication& msg) override {}
};

} // namespace

static p5_notifier_dummy               p5_dummy_notifier;
static error_indication_notifier_dummy dummy_error_notifier;

p5_requests_handler::p5_requests_handler(const phy_fapi_p5_sector_fastpath_adaptor_config&       config,
                                         const phy_fapi_p5_sector_fastpath_adaptor_dependencies& dependencies) :
  sector(config.sector_id),
  logger(dependencies.logger),
  executor(dependencies.executor),
  upper_phy_controller(dependencies.upper_phy_controller),
  p5_notifier(&p5_dummy_notifier),
  error_notifier(&dummy_error_notifier)
{
}

p5_requests_handler::~p5_requests_handler()
{
  stop_manager.stop();
}

void p5_requests_handler::send_param_request(const fapi::param_request& msg)
{
  fapi::param_response response;
  response.error_code = fapi::error_code_id::msg_ok;

  p5_notifier->on_param_response(response);
}

void p5_requests_handler::send_config_request(const fapi::config_request& msg)
{
  fapi::config_response response;
  response.error_code = fapi::error_code_id::msg_ok;

  p5_notifier->on_config_response(response);
}

void p5_requests_handler::send_start_request(const fapi::start_request& msg)
{
  if (!executor.defer([this, token = stop_manager.get_token()]() { upper_phy_controller.start(); })) {
    logger.warning("Sector #{}: PHY-FAPI P5 sector adaptor failed to enqueue start task ", sector);
  }
}

void p5_requests_handler::send_stop_request(const fapi::stop_request& msg)
{
  if (!executor.defer([this, token = stop_manager.get_token()]() {
        upper_phy_controller.stop();

        fapi::stop_indication indication;
        p5_notifier->on_stop_indication(indication);
      })) {
    logger.warning("Sector #{}: PHY-FAPI P5 sector adaptor failed to enqueue stop task ", sector);
  }
}
