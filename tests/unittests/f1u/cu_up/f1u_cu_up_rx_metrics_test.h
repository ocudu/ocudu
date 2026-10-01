// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/ocudulog/logger.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <gtest/gtest.h>

namespace ocudu {

/// Fixture class for CU-UP F1-U RX metrics container tests
class f1u_cu_up_rx_metrics_container_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("TEST", false);
};

} // namespace ocudu
