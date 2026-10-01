// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ngu_session_manager_test.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace ocuup;

TEST_F(ngu_session_manager_test, mngr_creation)
{
  ASSERT_NE(ngu_session_mngr, nullptr);
}

TEST_F(ngu_session_manager_test, rr_session_selection)
{
  {
    const auto& ngu_gw = ngu_session_mngr->get_next_ngu_gateway();
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.0.1");
  }
  {
    const auto& ngu_gw = ngu_session_mngr->get_next_ngu_gateway();
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.0.2");
  }
  {
    const auto& ngu_gw = ngu_session_mngr->get_next_ngu_gateway();
    std::string ip_addr;
    ASSERT_TRUE(ngu_gw.get_bind_address(ip_addr));
    ASSERT_EQ(ip_addr, "127.0.0.1");
  }
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
