// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "pdcp_rx_test_helpers.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

namespace ocudu {

ocudu::log_sink_spy& test_spy = []() -> ocudu::log_sink_spy& {
  if (!ocudulog::install_custom_sink(
          ocudu::log_sink_spy::name(),
          std::unique_ptr<ocudu::log_sink_spy>(new ocudu::log_sink_spy(ocudulog::get_default_log_formatter())))) {
    report_fatal_error("Unable to create logger spy");
  }
  auto* spy = static_cast<ocudu::log_sink_spy*>(ocudulog::find_sink(ocudu::log_sink_spy::name()));
  if (spy == nullptr) {
    report_fatal_error("Unable to create logger spy");
  }

  ocudulog::fetch_basic_logger("PDCP", *spy, true);
  return *spy;
}();

/// Fixture class for PDCP tests with DRBs
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class pdcp_rx_test_drb
  : public pdcp_rx_test_helper_default_crypto,
    public ::testing::Test,
    public ::testing::WithParamInterface<std::tuple<pdcp_sn_size, unsigned, rohc_test_params, bool>>
{
protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // reset log spy
    test_spy.reset_counters();

    // init RLC logger
    ocudulog::fetch_basic_logger("PDCP", false).set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("PDCP", false).set_hex_dump_max_size(100);

    set_out_of_order_delivery(std::get<bool>(GetParam()));
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }
};

/// Fixture class for PDCP tests with SRBs
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class pdcp_rx_test_srb : public pdcp_rx_test_helper_default_crypto,
                         public ::testing::Test,
                         public ::testing::WithParamInterface<std::tuple<unsigned, security::integrity_enabled>>
{
protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // reset log spy
    test_spy.reset_counters();

    // init RLC logger
    ocudulog::fetch_basic_logger("PDCP", false).set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("PDCP", false).set_hex_dump_max_size(100);
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }
};

} // namespace ocudu
