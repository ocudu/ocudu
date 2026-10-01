// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../../../lib/phy/upper/uplink_request_processor_impl.h"
#include "../support/prach_buffer_test_doubles.h"
#include "upper_phy_rx_symbol_request_notifier_test_doubles.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/phy/support/prach_buffer_context.h"
#include "ocudu/phy/support/support_factories.h"
#include <gtest/gtest.h>

using namespace ocudu;

namespace {

TEST(UplinkRequestProcessor, process_prach_request_produces_event_that_request_capture_prach)
{
  upper_phy_rx_symbol_request_notifier_spy   symbol_notifier;
  std::vector<std::unique_ptr<prach_buffer>> config_prach_pool;
  config_prach_pool.push_back(std::make_unique<prach_buffer_spy>());
  uplink_request_processor_impl ul_request_processor(
      symbol_notifier, config_prach_pool, ocudulog::fetch_basic_logger("PHY", false));
  prach_buffer_context context;
  context.slot   = slot_point(0, 0, 0);
  context.sector = 0;

  ASSERT_FALSE(symbol_notifier.has_prach_result_been_notified());

  ul_request_processor.process_prach_request(context);

  ASSERT_TRUE(symbol_notifier.has_prach_result_been_notified());
}

} // namespace
