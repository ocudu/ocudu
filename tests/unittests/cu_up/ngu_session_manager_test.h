// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "cu_up_test_helpers.h"
#include "lib/cu_up/ngu_session_manager_impl.h"
#include <gtest/gtest.h>

namespace ocudu::ocuup {

/// Fixture base class for PDU session manager tests
class ngu_session_manager_test : public ::testing::Test
{
  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    unsigned nof_gws = 2;
    for (unsigned i = 0; i < nof_gws; i++) {
      auto        ngu_gw = std::make_unique<dummy_gtpu_gateway>();
      std::string addr   = fmt::format("127.0.0.{}", 1 + i);
      ngu_gw->set_bind_address(addr);
      ngu_gws.push_back(std::move(ngu_gw));
    }

    // todo init ngu session manager
    ngu_session_mngr = std::make_unique<ngu_session_manager_impl>(ngu_gws);
  }

  void TearDown() override
  {
    ngu_gws.clear();
    ngu_session_mngr.reset();

    // flush logger after each test
    ocudulog::flush();
  }

protected:
  std::unique_ptr<ngu_session_manager>               ngu_session_mngr;
  std::vector<std::unique_ptr<gtpu_tnl_pdu_session>> ngu_gws;
};

} // namespace ocudu::ocuup
