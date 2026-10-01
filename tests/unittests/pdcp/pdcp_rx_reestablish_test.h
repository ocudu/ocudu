// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "pdcp_rx_test_helpers.h"
#include <gtest/gtest.h>

namespace ocudu {

/// Fixture class for PDCP tests
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size
class pdcp_rx_reestablish_test
  : public pdcp_rx_test_helper_default_crypto,
    public ::testing::Test,
    public ::testing::WithParamInterface<std::tuple<pdcp_sn_size, unsigned, rohc_test_params>>
{
protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

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

class pdcp_rx_reestablish_test_srb : public pdcp_rx_reestablish_test
{};

} // namespace ocudu
