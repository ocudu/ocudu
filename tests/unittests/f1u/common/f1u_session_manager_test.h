// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/f1u/split_connector/f1u_session_manager_impl.h"
#include "tests/unittests/cu_up/cu_up_test_helpers.h"
#include "ocudu/ocudulog/ocudulog.h"
#include <gtest/gtest.h>

namespace ocudu::ocuup {

/// Fixture base class for PDU session manager tests
class f1u_session_manager_test : public ::testing::Test
{
  void SetUp() override
  {
    ocudulog::fetch_basic_logger("TEST").set_level(ocudulog::basic_levels::debug);
    ocudulog::init();

    // Setup default GWs.
    {
      unsigned nof_gws = 2;
      for (unsigned i = 0; i < nof_gws; i++) {
        auto        f1u_gw = std::make_unique<dummy_gtpu_gateway>();
        std::string addr   = fmt::format("127.0.0.{}", 1 + i);
        f1u_gw->set_bind_address(addr);
        f1u_sessions.default_gw_sessions.push_back(std::move(f1u_gw));
      }
    }

    // Setup 5QI GWs for S-NSSAIs {1,10} and {1,11}.
    {
      for (unsigned sd_int = 10; sd_int < 12; sd_int++) {
        auto      sd = slice_differentiator::create(sd_int);
        s_nssai_t s_nssai{slice_service_type{1}, *sd};
        five_qi_t five_qi{7};
        unsigned  nof_gws = 2;
        for (unsigned i = 0; i < nof_gws; i++) {
          auto        f1u_gw = std::make_unique<dummy_gtpu_gateway>();
          std::string addr   = fmt::format("127.0.{}.{}", sd->value(), 1 + i);
          f1u_gw->set_bind_address(addr);
          f1u_sessions.session_maps[s_nssai][five_qi].push_back(std::move(f1u_gw));
        }
      }
    }

    // todo init ngu session manager
    f1u_session_mngr = std::make_unique<f1u_session_manager_impl>(f1u_sessions);
  }

  void TearDown() override
  {
    f1u_sessions.default_gw_sessions.clear();
    f1u_sessions.session_maps.clear();
    f1u_session_mngr.reset();

    // flush logger after each test
    ocudulog::flush();
  }

protected:
  std::unique_ptr<f1u_session_manager> f1u_session_mngr;
  f1u_session_maps                     f1u_sessions;
  dummy_inner_f1u_bearer               f1u_bearer;
};

} // namespace ocudu::ocuup
