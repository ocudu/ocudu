// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "helpers.h"
#include "mac_fapi_p5_stop_cell_procedure.h"

using namespace ocudu;
using namespace fapi_adaptor;

namespace {

class mac_fapi_p5_stop_cell_procedure_fixture : public unittest::mac_fapi_p5_procedure_fixture
{
protected:
  /// Launches the stop procedure. The reply is the answer of the PHY to the STOP.request, or none.
  void start_procedure(std::optional<bool> reply_to_stop_request)
  {
    create_gateway(unittest::p5_requests_gateway_spy::replies{
        .param = std::nullopt, .config = std::nullopt, .stop = reply_to_stop_request, .start = std::nullopt});

    launch_procedure([this]() {
      return launch_async<mac_fapi_stop_cell_procedure>(timeout,
                                                        make_dependencies<mac_fapi_stop_cell_procedure_dependencies>());
    });
  }
};

} // namespace

TEST_F(mac_fapi_p5_stop_cell_procedure_fixture, stop_request_is_sent_and_no_response_triggers_timeout)
{
  start_procedure(std::nullopt);
  ASSERT_TRUE(wait_procedure_to_time_out_after([this]() { return gateway_spy->has_stop_request_been_sent(); }));

  EXPECT_TRUE(procedure_finished_with(false));
}

TEST_F(mac_fapi_p5_stop_cell_procedure_fixture, procedure_ends_correctly)
{
  start_procedure(true);
  wait_procedure_to_finish();

  EXPECT_TRUE(gateway_spy->has_stop_request_been_sent());
  EXPECT_TRUE(procedure_finished_with(true));
}

TEST_F(mac_fapi_p5_stop_cell_procedure_fixture, stop_request_produces_error_indication_then_procedure_returns_error)
{
  start_procedure(false);
  wait_procedure_to_finish();

  EXPECT_TRUE(gateway_spy->has_stop_request_been_sent());
  EXPECT_TRUE(procedure_finished_with(false));
}
