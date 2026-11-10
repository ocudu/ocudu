// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "../../../../lib/ofh/receiver/ofh_uplane_prach_data_flow_notifier.h"
#include "../ofh_uplane_rx_symbol_notifier_test_doubles.h"
#include "helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ofh;
using namespace ofh::testing;

TEST(ofh_uplane_prach_data_flow_notifier, empty_context_does_not_notify)
{
  auto                            notifier = std::make_shared<uplane_rx_symbol_notifier_spy>();
  auto                            repo     = std::make_shared<prach_context_repository>(1);
  uplane_prach_data_flow_notifier sender(ocudulog::fetch_basic_logger("TEST"), repo, notifier);
  slot_point                      slot(0, 0, 1);

  sender.notify_prach(slot);

  ASSERT_TRUE(repo->get(slot).empty());
  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_FALSE(notifier->has_new_prach_function_been_called());
}

TEST(ofh_uplane_prach_data_flow_notifier, unwritten_buffer_does_not_notify)
{
  auto prach_buffer_pool = create_prach_buffer_pool(1);

  auto                            notifier = std::make_shared<uplane_rx_symbol_notifier_spy>();
  auto                            repo     = std::make_shared<prach_context_repository>(1);
  uplane_prach_data_flow_notifier sender(ocudulog::fetch_basic_logger("TEST"), repo, notifier);
  slot_point                      slot(0, 0, 1);

  prach_buffer_context context = {.ports                 = {0},
                                  .slot                  = slot,
                                  .nof_prb_ul_grid       = 12,
                                  .root_sequence_index   = 0,
                                  .rb_offset             = 0,
                                  .sector                = 0,
                                  .start_symbol          = 0,
                                  .format                = prach_format_type::zero,
                                  .nof_td_occasions      = 1,
                                  .nof_fd_occasions      = 1,
                                  .pusch_scs             = ocudu::subcarrier_spacing::kHz30,
                                  .restricted_set        = restricted_set_config::UNRESTRICTED,
                                  .zero_correlation_zone = 0,
                                  .start_preamble_index  = 0,
                                  .nof_preamble_indices  = 1};

  repo->add(context, prach_buffer_pool->get(), ocudulog::fetch_basic_logger("TEST"), std::nullopt);
  repo->process_pending_contexts();
  sender.notify_prach(slot);

  ASSERT_FALSE(repo->get(slot).empty());
  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_FALSE(notifier->has_new_prach_function_been_called());
}

TEST(ofh_uplane_prach_data_flow_notifier, completed_long_prach_buffer_triggers_notification)
{
  auto                            notifier = std::make_shared<uplane_rx_symbol_notifier_spy>();
  auto                            repo     = std::make_shared<prach_context_repository>(1);
  uplane_prach_data_flow_notifier sender(ocudulog::fetch_basic_logger("TEST"), repo, notifier);
  slot_point                      slot(0, 0, 1);
  unsigned                        symbol = 0;
  unsigned                        port   = 0;

  auto prach_buffer_pool = create_prach_buffer_pool(1);

  prach_buffer_context context = {.ports                 = {0},
                                  .slot                  = slot,
                                  .nof_prb_ul_grid       = 12,
                                  .root_sequence_index   = 0,
                                  .rb_offset             = 0,
                                  .sector                = 0,
                                  .start_symbol          = 0,
                                  .format                = prach_format_type::zero,
                                  .nof_td_occasions      = 1,
                                  .nof_fd_occasions      = 1,
                                  .pusch_scs             = ocudu::subcarrier_spacing::kHz30,
                                  .restricted_set        = restricted_set_config::UNRESTRICTED,
                                  .zero_correlation_zone = 0,
                                  .start_preamble_index  = 0,
                                  .nof_preamble_indices  = 1};

  static_vector<cbf16_t, 839> samples(839);
  repo->add(context, prach_buffer_pool->get(), ocudulog::fetch_basic_logger("TEST"), std::nullopt);
  repo->process_pending_contexts();
  ASSERT_FALSE(repo->get(slot).empty());

  // Fill the grid.
  ASSERT_TRUE(repo->write_iq(slot, port, symbol, 0, samples));

  sender.notify_prach(slot);

  // Assert that the context is cleared when it is notified.
  ASSERT_TRUE(repo->get(slot).empty());

  // Assert the notification.
  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_TRUE(notifier->has_new_prach_function_been_called());
}

TEST(ofh_uplane_prach_data_flow_notifier, completed_short_prach_buffer_triggers_notification)
{
  auto                            notifier = std::make_shared<uplane_rx_symbol_notifier_spy>();
  auto                            repo     = std::make_shared<prach_context_repository>(1);
  uplane_prach_data_flow_notifier sender(ocudulog::fetch_basic_logger("TEST"), repo, notifier);
  slot_point                      slot(0, 0, 1);
  unsigned                        port = 0;

  auto prach_buffer_pool = create_prach_buffer_pool(1);

  prach_buffer_context context = {.ports                 = {0},
                                  .slot                  = slot,
                                  .nof_prb_ul_grid       = 12,
                                  .root_sequence_index   = 0,
                                  .rb_offset             = 0,
                                  .sector                = 0,
                                  .start_symbol          = 0,
                                  .format                = prach_format_type::B4,
                                  .nof_td_occasions      = 1,
                                  .nof_fd_occasions      = 1,
                                  .pusch_scs             = ocudu::subcarrier_spacing::kHz30,
                                  .restricted_set        = restricted_set_config::UNRESTRICTED,
                                  .zero_correlation_zone = 0,
                                  .start_preamble_index  = 0,
                                  .nof_preamble_indices  = 1};

  static_vector<cbf16_t, 139> samples(139);
  repo->add(context, prach_buffer_pool->get(), ocudulog::fetch_basic_logger("TEST"), std::nullopt);
  repo->process_pending_contexts();
  ASSERT_FALSE(repo->get(slot).empty());

  // Fill the grid.
  for (unsigned i = 0; i != 12; ++i) {
    ASSERT_TRUE(repo->write_iq(slot, port, i, 0, samples));
  }

  sender.notify_prach(slot);

  // Assert that the context is cleared when it is notified.
  ASSERT_TRUE(repo->get(slot).empty());

  // Assert the notification.
  ASSERT_FALSE(notifier->has_new_uplink_symbol_function_been_called());
  ASSERT_TRUE(notifier->has_new_prach_function_been_called());
}
