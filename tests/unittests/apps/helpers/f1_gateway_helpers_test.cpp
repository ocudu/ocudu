// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

/// \file
/// \brief Unit tests for the internal configuration helpers of the F1 gateway factories.
///
/// The helpers \c make_sctp_network_gateway_config and \c make_udp_gtpu_gateway_config are file-local (static)
/// functions inside f1_gateway_helpers.cpp by design (see review feedback in MR !1085), so they are not part of
/// the public API. To still cover them with unit tests, this file compiles the implementation directly by
/// including the .cpp. This gives the test translation unit access to the static functions without exposing them
/// through a header.

#include "apps/helpers/f1/f1_gateway_helpers.cpp"
#include <gtest/gtest.h>

using namespace ocudu;

/// Verifies that the SCTP gateway configuration helper propagates the interface name, bind addresses, port and
/// PPID from the input arguments, and applies the SCTP socket parameters coming from the application config.
TEST(f1_gateway_helpers, make_sctp_network_gateway_config_sets_expected_values)
{
  sctp_appconfig app_cfg;
  app_cfg.rto_initial_ms = 1000;
  app_cfg.nodelay        = true;

  const auto cfg = make_sctp_network_gateway_config("F1-C", {"127.0.10.1"}, 38472, 1, app_cfg, {});

  EXPECT_EQ(cfg.if_name, "F1-C");
  EXPECT_EQ(cfg.bind_addresses, std::vector<std::string>{"127.0.10.1"});
  EXPECT_EQ(cfg.bind_port, 38472);
  EXPECT_EQ(cfg.ppid, 1);
  ASSERT_TRUE(cfg.rto_initial.has_value());
  EXPECT_EQ(cfg.rto_initial, std::chrono::milliseconds(1000));
  ASSERT_TRUE(cfg.nodelay.has_value());
  EXPECT_TRUE(*cfg.nodelay);
}

/// Verifies that the SCTP gateway configuration helper fills the optional socket parameters from a default
/// (unmodified) application config and uses the provided F1AP port/PPID constants.
TEST(f1_gateway_helpers, make_sctp_network_gateway_config_uses_defaults_when_unset)
{
  const sctp_appconfig app_cfg;

  const auto cfg = make_sctp_network_gateway_config("F1-C", {"127.0.10.1"}, F1AP_PORT, F1AP_PPID, app_cfg, {});

  EXPECT_EQ(cfg.bind_port, F1AP_PORT);
  EXPECT_EQ(cfg.ppid, F1AP_PPID);
  ASSERT_TRUE(cfg.rto_initial.has_value());
  EXPECT_EQ(cfg.rto_initial, std::chrono::milliseconds(app_cfg.rto_initial_ms));
  ASSERT_TRUE(cfg.nodelay.has_value());
  EXPECT_FALSE(*cfg.nodelay);
}

/// Verifies that the UDP GTP-U gateway configuration helper propagates the interface name, bind address, external
/// bind address, bind port and all the per-socket UDP parameters from the F1-U application socket configuration.
TEST(f1_gateway_helpers, make_udp_gtpu_gateway_config_uses_f1u_appconfig_values)
{
  f1u_socket_appconfig sock_cfg;
  sock_cfg.bind_addr                 = "127.0.10.2";
  sock_cfg.udp_config.ext_addr       = "127.0.10.3";
  sock_cfg.udp_config.reuse_addr     = true;
  sock_cfg.udp_config.pool_threshold = 42;
  sock_cfg.udp_config.rx_max_msgs    = 3;
  sock_cfg.udp_config.dscp           = 46;

  f1u_sockets_appconfig sockets_cfg;
  sockets_cfg.bind_port = 2152;

  const auto cfg = make_udp_gtpu_gateway_config(sock_cfg, sockets_cfg, "CU-F1-U", true);

  EXPECT_EQ(cfg.if_name, "CU-F1-U");
  EXPECT_EQ(cfg.bind_address, "127.0.10.2");
  EXPECT_EQ(cfg.ext_bind_addr, "127.0.10.3");
  EXPECT_EQ(cfg.bind_port, 2152);
  EXPECT_TRUE(cfg.reuse_addr);
  EXPECT_EQ(cfg.pool_occupancy_threshold, 42);
  EXPECT_EQ(cfg.rx_max_mmsg, 3);
  ASSERT_TRUE(cfg.dscp.has_value());
  EXPECT_EQ(cfg.dscp, 46);
  EXPECT_TRUE(cfg.warn_on_drop);
}

/// Verifies that the UDP GTP-U gateway configuration helper fills the optional fields with their defaults when
/// the application socket config is left untouched, and forwards the warn_on_drop flag as provided.
TEST(f1_gateway_helpers, make_udp_gtpu_gateway_config_uses_defaults_when_unset)
{
  const f1u_socket_appconfig  sock_cfg;
  const f1u_sockets_appconfig sockets_cfg;

  const auto cfg = make_udp_gtpu_gateway_config(sock_cfg, sockets_cfg, "DU-F1-U", false);

  EXPECT_EQ(cfg.if_name, "DU-F1-U");
  EXPECT_EQ(cfg.bind_port, sockets_cfg.bind_port);
  EXPECT_FALSE(cfg.reuse_addr);
  EXPECT_FALSE(cfg.dscp.has_value());
  EXPECT_FALSE(cfg.warn_on_drop);
}
