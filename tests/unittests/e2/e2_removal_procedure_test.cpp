// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/e2/common/e2ap_asn1_utils.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// E2 Removal REQUEST sent; RIC responds with E2 Removal RESPONSE -> procedure and TNL teardown complete.
TEST_F(e2_test, when_e2_removal_response_received_then_procedure_completes)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");

  async_task<void>         t = e2->handle_e2_node_initiated_removal_request();
  lazy_task_launcher<void> launcher(t);

  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.type().value, asn1::e2ap::e2ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(e2_client->last_tx_e2_pdu.pdu.init_msg().value.type().value,
            asn1::e2ap::e2ap_elem_procs_o::init_msg_c::types_opts::e2_removal_request);

  ASSERT_FALSE(t.ready());

  unsigned   transaction_id = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message response       = generate_e2_removal_response(transaction_id);
  e2->handle_message(response);
  task_worker.run_pending_tasks();

  ASSERT_TRUE(t.ready());
}

/// E2 Removal REQUEST sent; RIC responds with E2 Removal FAILURE -> procedure still completes (non-blocking).
TEST_F(e2_test, when_e2_removal_failure_received_then_procedure_still_completes)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");

  async_task<void>         t = e2->handle_e2_node_initiated_removal_request();
  lazy_task_launcher<void> launcher(t);

  ASSERT_FALSE(t.ready());

  unsigned   transaction_id = get_transaction_id(e2_client->last_tx_e2_pdu.pdu).value();
  e2_message failure        = generate_e2_removal_failure(transaction_id);
  e2->handle_message(failure);
  task_worker.run_pending_tasks();

  ASSERT_TRUE(t.ready());
}

/// E2 Removal REQUEST sent; RIC never responds -> 5 s transaction timeout fires -> procedure still completes.
TEST_F(e2_test, when_e2_removal_timeout_fires_then_procedure_still_completes)
{
  report_fatal_error_if_not(e2->handle_e2_tnl_connection_request(), "Unable to establish dummy SCTP connection");

  async_task<void>         t = e2->handle_e2_node_initiated_removal_request();
  lazy_task_launcher<void> launcher(t);

  ASSERT_FALSE(t.ready());

  for (unsigned i = 0; i <= 5000; ++i) {
    tick();
  }

  ASSERT_TRUE(t.ready());
}

/// No TNL connection present -> removal skips the E2AP handshake and completes immediately.
TEST_F(e2_test, when_no_tnl_connection_removal_skips_pdu_and_completes)
{
  async_task<void>         t = e2->handle_e2_node_initiated_removal_request();
  lazy_task_launcher<void> launcher(t);

  ASSERT_TRUE(t.ready());
}
