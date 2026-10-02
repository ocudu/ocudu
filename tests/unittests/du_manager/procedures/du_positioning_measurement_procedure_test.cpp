// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_manager_procedure_test_helpers.h"
#include "lib/du/du_high/du_manager/du_ue/du_ue_manager.h"
#include "lib/du/du_high/du_manager/procedures/du_positioning_measurement_procedure.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

namespace {

std::vector<du_cell_config> make_test_cells()
{
  return {config_helpers::make_default_du_cell_config()};
}

/// Builds an SRS configuration with one periodic resource.
srs_config make_test_srs_config()
{
  srs_config cfg{};
  cfg.srs_res_list.resize(1);
  cfg.srs_res_list[0].id.ue_res_id   = srs_config::MIN_SRS_RES_ID;
  cfg.srs_res_list[0].id.cell_res_id = 0;
  cfg.srs_res_list[0].sequence_id    = 1;
  cfg.srs_res_list[0].periodicity_and_offset =
      srs_config::srs_periodicity_and_offset{.period = srs_periodicity::sl80, .offset = 0};
  cfg.srs_res_set_list.resize(1);
  cfg.srs_res_set_list[0].res_type.emplace<srs_config::srs_resource_set::periodic_resource_type>();
  return cfg;
}

class du_positioning_measurement_procedure_test : public du_manager_proc_tester, public ::testing::Test
{
protected:
  du_positioning_measurement_procedure_test() : du_manager_proc_tester(make_test_cells())
  {
    ocudulog::fetch_basic_logger("DU-MNG").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    // The TRP IDs start from 1, as in du_positioning_manager_impl.
    du_trp_info trp;
    trp.trp_id = trp_id_t::min;
    trp.cgi    = cell_mng.get_cell_cfg(to_du_cell_index(0)).nr_cgi;
    trps.emplace(trp.trp_id, trp);
  }

  ~du_positioning_measurement_procedure_test() override { ocudulog::flush(); }

  du_positioning_meas_request make_request()
  {
    du_positioning_meas_request req;
    req.trp_meas_req_list.push_back(du_trp_meas_request{.trp_id = trp_id_t::min});
    req.pos_meas_quants.push_back(positioning_meas_quantity{.meas_type = pos_meas_type::ul_rtoa});
    srs_carrier carrier;
    carrier.srs_cfg        = make_test_srs_config();
    carrier.ul_bwp_cfg.scs = subcarrier_spacing::kHz15;
    req.srs_carriers.push_back(carrier);
    return req;
  }

  du_positioning_meas_response run_procedure(const du_positioning_meas_request& req)
  {
    async_task<du_positioning_meas_response> t =
        launch_async<positioning_measurement_procedure>(req, cell_mng, real_ue_mng, params, trps);
    lazy_task_launcher<du_positioning_meas_response> launcher{t};
    ocudu_assert(launcher.ready(), "The positioning measurement procedure should have completed by now");
    return t.get();
  }

  du_ue_manager                   real_ue_mng{params, mem_resources, cell_res_alloc, cell_mng, proc_metrics};
  std::map<trp_id_t, du_trp_info> trps;
};

} // namespace

/// The MAC answers with an empty response when the measurement times out, or when it already runs another
/// measurement. The procedure must report a failure instead of reading a result that the MAC did not produce.
TEST_F(du_positioning_measurement_procedure_test, when_mac_reports_no_measurement_then_the_response_is_empty)
{
  // The MAC test double returns an empty response by default.
  du_positioning_meas_response resp = run_procedure(make_request());

  ASSERT_TRUE(resp.pos_meas_list.empty());
}

TEST_F(du_positioning_measurement_procedure_test, when_request_is_sent_then_the_mac_timeout_follows_the_srs_period)
{
  run_procedure(make_request());

  ASSERT_TRUE(mac.last_positioning_meas_request.has_value());
  // Two periods of 80 slots at 15 kHz.
  ASSERT_EQ(mac.last_positioning_meas_request->timeout, std::chrono::milliseconds{160});
}

/// The gNB-CU expects the result within the Response Time, as per TS 38.473 section 8.13.2.2, so a shorter Response
/// Time caps the time that the MAC waits.
TEST_F(du_positioning_measurement_procedure_test, when_response_time_is_shorter_then_it_caps_the_mac_timeout)
{
  du_positioning_meas_request req = make_request();
  req.response_time               = std::chrono::milliseconds{50};

  run_procedure(req);

  ASSERT_TRUE(mac.last_positioning_meas_request.has_value());
  // The measurement stops before the Response Time, so that the answer still arrives inside it.
  ASSERT_EQ(mac.last_positioning_meas_request->timeout, std::chrono::milliseconds{40});
}

/// A very short Response Time still starts a measurement, which then ends in a failure.
TEST_F(du_positioning_measurement_procedure_test,
       when_response_time_is_shorter_than_the_margin_then_the_wait_stays_positive)
{
  du_positioning_meas_request req = make_request();
  req.response_time               = std::chrono::milliseconds{10};

  run_procedure(req);

  ASSERT_TRUE(mac.last_positioning_meas_request.has_value());
  ASSERT_GT(mac.last_positioning_meas_request->timeout.count(), 0);
  ASSERT_LT(mac.last_positioning_meas_request->timeout, std::chrono::milliseconds{10});
}

/// A Response Time longer than the SRS period does not extend the wait.
TEST_F(du_positioning_measurement_procedure_test, when_response_time_is_longer_then_the_srs_period_decides)
{
  du_positioning_meas_request req = make_request();
  req.response_time               = std::chrono::milliseconds{5000};

  run_procedure(req);

  ASSERT_TRUE(mac.last_positioning_meas_request.has_value());
  // Two periods of 80 slots at 15 kHz.
  ASSERT_EQ(mac.last_positioning_meas_request->timeout, std::chrono::milliseconds{160});
}
