// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "rlc_tx_metrics_test.h"
#include "ocudu/rlc/rlc_tx_metrics.h"
#include "ocudu/support/format/fmt_to_c_str.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST_F(rlc_tx_metrics_test, poll_latency_empty)
{
  {
    // Check custom formatter
    rlc_am_tx_metrics_poll_latency m = {};
    fmt::memory_buffer             buffer;
    format_rlc_am_tx_t_poll_latencty_metrics(buffer, m);
    std::string out_str = to_c_str(buffer);
    std::string exp_str = " t_poll_nof_expiration=0";
    logger.info("out_str={}", out_str);
    logger.info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }
}

TEST_F(rlc_tx_metrics_test, poll_latency_full)
{
  {
    // Check custom formatter
    rlc_am_tx_metrics_poll_latency m = {};

    m.num_latency_meas = 2;
    m.sum_latency_us   = 600;
    m.min_latency_us   = 200;
    m.max_latency_us   = 400;

    fmt::memory_buffer buffer;
    format_rlc_am_tx_t_poll_latencty_metrics(buffer, m);
    std::string out_str = to_c_str(buffer);
    std::string exp_str =
        " t_poll_nof_expiration=2 t_poll_latency_avg=300.0us t_poll_latency_min=200us t_poll_latency_max=400us";
    logger.info("out_str={}", out_str);
    logger.info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
