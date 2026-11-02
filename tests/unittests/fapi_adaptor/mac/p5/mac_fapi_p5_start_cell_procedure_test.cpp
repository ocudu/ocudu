// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "mac_fapi_p5_start_cell_procedure.h"

using namespace ocudu;
using namespace fapi_adaptor;

namespace {

class mac_fapi_p5_start_cell_procedure_fixture : public unittest::mac_fapi_p5_procedure_fixture
{
protected:
  /// Launches the start procedure. Each reply is the answer of the PHY to the matching request, or none.
  void start_procedure(std::optional<bool> reply_to_param_request,
                       std::optional<bool> reply_to_config_request,
                       std::optional<bool> reply_to_start_request)
  {
    create_gateway(unittest::p5_requests_gateway_spy::replies{.param  = reply_to_param_request,
                                                              .config = reply_to_config_request,
                                                              .stop   = std::nullopt,
                                                              .start  = reply_to_start_request});

    launch_procedure([this]() {
      return launch_async<mac_fapi_start_cell_procedure>(
          config, make_dependencies<mac_fapi_start_cell_procedure_dependencies>());
    });
  }

  /// Checks which of the requests of the start procedure have been sent.
  void expect_requests_sent(bool param, bool config_, bool start) const
  {
    EXPECT_EQ(param, gateway_spy->has_param_request_been_sent());
    EXPECT_EQ(config_, gateway_spy->has_config_request_been_sent());
    EXPECT_EQ(start, gateway_spy->has_start_request_been_sent());
  }

  fapi::cell_configuration             cell_cfg;
  mac_fapi_start_cell_procedure_config config{.is_rt_mode_enabled = true, .cell_cfg = cell_cfg, .timeout = timeout};
};

} // namespace

TEST_F(mac_fapi_p5_start_cell_procedure_fixture, param_request_is_sent_and_no_response_triggers_timeout)
{
  start_procedure(std::nullopt, std::nullopt, std::nullopt);
  ASSERT_TRUE(wait_procedure_to_time_out_after([this]() { return gateway_spy->has_param_request_been_sent(); }));

  EXPECT_TRUE(procedure_finished_with(false));
  expect_requests_sent(true, false, false);
}

TEST_F(mac_fapi_p5_start_cell_procedure_fixture, config_request_is_sent_and_no_response_triggers_timeout)
{
  start_procedure(true, std::nullopt, std::nullopt);
  ASSERT_TRUE(wait_procedure_to_time_out_after([this]() { return gateway_spy->has_config_request_been_sent(); }));

  EXPECT_TRUE(procedure_finished_with(false));
  expect_requests_sent(true, true, false);
}

TEST_F(mac_fapi_p5_start_cell_procedure_fixture, start_request_is_sent_and_no_response_triggers_timeout)
{
  start_procedure(true, true, std::nullopt);
  ASSERT_TRUE(wait_procedure_to_time_out_after([this]() { return gateway_spy->has_start_request_been_sent(); }));

  EXPECT_TRUE(procedure_finished_with(false));
  expect_requests_sent(true, true, true);
}

TEST_F(mac_fapi_p5_start_cell_procedure_fixture, start_procedure_completes_correctly)
{
  start_procedure(true, true, true);
  wait_procedure_to_finish();

  EXPECT_TRUE(procedure_finished_with(true));
  expect_requests_sent(true, true, true);
}

TEST_F(mac_fapi_p5_start_cell_procedure_fixture, param_response_contain_error_then_procedure_returns_error)
{
  start_procedure(false, true, true);
  wait_procedure_to_finish();

  EXPECT_TRUE(procedure_finished_with(false));
  expect_requests_sent(true, false, false);
}

TEST_F(mac_fapi_p5_start_cell_procedure_fixture, config_response_contain_error_then_procedure_returns_error)
{
  start_procedure(true, false, std::nullopt);
  wait_procedure_to_finish();

  EXPECT_TRUE(procedure_finished_with(false));
  expect_requests_sent(true, true, false);
}

TEST_F(mac_fapi_p5_start_cell_procedure_fixture, start_request_produces_error_then_procedure_returns_error)
{
  start_procedure(true, true, false);
  wait_procedure_to_finish();

  EXPECT_TRUE(procedure_finished_with(false));
  expect_requests_sent(true, true, true);
}
