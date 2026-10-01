// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "f1u_cu_up_rx_metrics_test.h"
#include "ocudu/f1u/cu_up/f1u_rx_metrics.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST_F(f1u_cu_up_rx_metrics_container_test, init)
{
  ocuup::f1u_rx_metrics_container m = {};

  ocudulog::fetch_basic_logger("TEST", false).info("Metrics: {}", m);

  // Check values
  ASSERT_EQ(m.num_pdus, 0);
  ASSERT_EQ(m.num_dropped_pdus, 0);
  ASSERT_EQ(m.num_sdus, 0);
  ASSERT_EQ(m.num_sdu_bytes, 0);
  ASSERT_EQ(m.num_dds, 0);
  ASSERT_EQ(m.num_dds_failures, 0);
  ASSERT_EQ(m.counter, 0);

  {
    // Check fmt formatter
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "{}", m);
    std::string out_str = to_c_str(buffer);
    std::string exp_str = "num_pdus=0 num_dropped_pdus=0 num_sdus=0 num_sdu_bytes=0 num_dds=0 num_dds_failures=0";
    ocudulog::fetch_basic_logger("TEST", false).info("out_str={}", out_str);
    ocudulog::fetch_basic_logger("TEST", false).info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }

  {
    // Check custom formatter
    timer_duration dur{2}; // 2ms
    std::string    out_str = format_f1u_rx_metrics(dur, m);
    std::string    exp_str = "num_pdus=0 num_dropped_pdus=0 num_sdus=0 sdu_rate= 0bps num_dds=0 num_dds_failures=0";
    ocudulog::fetch_basic_logger("TEST", false).info("out_str={}", out_str);
    ocudulog::fetch_basic_logger("TEST", false).info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }
}

TEST_F(f1u_cu_up_rx_metrics_container_test, values)
{
  ocuup::f1u_rx_metrics_container m = {.num_pdus         = 1,
                                       .num_dropped_pdus = 2,
                                       .num_sdus         = 3,
                                       .num_sdu_bytes    = 4,
                                       .num_dds          = 5,
                                       .num_dds_failures = 6,
                                       .counter          = 7};

  ocudulog::fetch_basic_logger("TEST", false).info("Metrics: {}", m);

  // Check values
  ASSERT_EQ(m.num_pdus, 1);
  ASSERT_EQ(m.num_dropped_pdus, 2);
  ASSERT_EQ(m.num_sdus, 3);
  ASSERT_EQ(m.num_sdu_bytes, 4);
  ASSERT_EQ(m.num_dds, 5);
  ASSERT_EQ(m.num_dds_failures, 6);
  ASSERT_EQ(m.counter, 7);

  {
    // Check fmt formatter
    fmt::memory_buffer buffer;
    fmt::format_to(std::back_inserter(buffer), "{}", m);
    std::string out_str = to_c_str(buffer);
    std::string exp_str = "num_pdus=1 num_dropped_pdus=2 num_sdus=3 num_sdu_bytes=4 num_dds=5 num_dds_failures=6";
    ocudulog::fetch_basic_logger("TEST", false).info("out_str={}", out_str);
    ocudulog::fetch_basic_logger("TEST", false).info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }

  {
    // Check custom formatter
    timer_duration dur{2}; // 2ms
    std::string    out_str = format_f1u_rx_metrics(dur, m);
    std::string    exp_str = "num_pdus=1 num_dropped_pdus=2 num_sdus=3 sdu_rate=16kbps num_dds=5 num_dds_failures=6";
    ocudulog::fetch_basic_logger("TEST", false).info("out_str={}", out_str);
    ocudulog::fetch_basic_logger("TEST", false).info("exp_str={}", exp_str);
    EXPECT_EQ(out_str, exp_str);
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
