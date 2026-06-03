// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/e2/procedures/ric_connection_loss_routine.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

class dummy_sub_mngr_for_loss : public e2_subscription_manager
{
public:
  int stop_count = 0;

  e2_subscribe_reponse_message handle_subscription_setup(const asn1::e2ap::ric_sub_request_s&) override { return {}; }
  e2_subscribe_delete_response_message handle_subscription_delete(const asn1::e2ap::ric_sub_delete_request_s&) override
  {
    return {};
  }
  void
  start_subscription(const asn1::e2ap::ric_request_id_s&, uint16_t, e2_event_manager&, e2_message_notifier&) override
  {
  }
  void stop_subscription(const asn1::e2ap::ric_request_id_s&,
                         e2_event_manager&,
                         const asn1::e2ap::ric_sub_delete_request_s&) override
  {
  }
  void            add_e2sm_service(std::string, std::unique_ptr<e2sm_interface>) override {}
  e2sm_interface* get_e2sm_interface(const std::string) override { return nullptr; }
  void            add_ran_function_oid(uint16_t, std::string) override {}
  void            stop() override { ++stop_count; }
};

TEST(ric_connection_loss_routine_test, stop_called_exactly_once)
{
  ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
  ocudulog::init();

  dummy_sub_mngr_for_loss sub_mngr;
  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("TEST");

  async_task<void>         t = launch_async<ric_connection_loss_routine>(sub_mngr, logger);
  lazy_task_launcher<void> launcher(t);

  ASSERT_TRUE(t.ready());
  ASSERT_EQ(sub_mngr.stop_count, 1);

  ocudulog::flush();
}
