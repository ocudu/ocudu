// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngap_test_helpers.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "ocudu/adt/format.h"
#include "ocudu/ngap/ngap_handover.h"
#include "ocudu/ran/cu_types.h"
#include "ocudu/ran/rb_id.h"
#include "ocudu/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocucp;

/// Test successful handover preparation procedure
TEST_F(ngap_test, when_ue_missing_then_handover_preparation_procedure_fails)
{
  OCUDU_TEST_REQUIREMENTS("MVP-FUNC-MOB-4");

  ngap_handover_preparation_request request = {};
  request.ue_index                          = uint_to_ue_index(0);

  // Action 1: Launch HO preparation procedure
  test_logger.info("Launch source NGAP handover preparation procedure");
  async_task<ngap_handover_preparation_response>         t = ngap->handle_handover_preparation_request(request);
  lazy_task_launcher<ngap_handover_preparation_response> t_launcher(t);

  // Status: should have failed already, as there is no UE.
  ASSERT_TRUE(t.ready());

  // Procedure should have failed.
  ASSERT_FALSE(t.get().success);

  // Make sure no NGAP pdu was sent
  ASSERT_TRUE(n2_gw.last_ngap_msgs.empty());
}
