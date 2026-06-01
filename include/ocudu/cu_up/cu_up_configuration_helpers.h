// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/cu_up/cu_up_types.h"
#include "ocudu/ran/qos/five_qi.h"
#include <map>

namespace ocudu::config_helpers {

inline ocuup::cu_up_qos_config
make_default_cu_up_custom_cfg_item(bool warn_on_drop, timer_duration metrics_period, bool test_mode)
{
  ocuup::cu_up_qos_config cfg{};
  cfg.pdcp_custom_cfg = pdcp_custom_config{}; // defaults are configured as member-initialization within the struct.
  cfg.pdcp_custom_cfg.metrics_period  = metrics_period;
  cfg.pdcp_custom_cfg.rx.warn_on_drop = warn_on_drop;
  cfg.pdcp_custom_cfg.tx.warn_on_drop = warn_on_drop;
  cfg.pdcp_custom_cfg.tx.test_mode    = test_mode;

  cfg.f1u_cfg = ocuup::f1u_config{}; // defaults are configured as member-initialization within the struct.
  cfg.f1u_cfg.metrics_period = metrics_period;

  return cfg;
}

/// Generates default QoS configuration used by gNB CU-UP.
inline std::map<five_qi_t, ocuup::cu_up_qos_config>
make_default_cu_up_qos_config_list(bool warn_on_drop, timer_duration metrics_period, bool test_mode)
{
  std::map<five_qi_t, ocuup::cu_up_qos_config> qos_list = {};
  //
  // Guaranteed Bitrate 5QIs
  //
  {
    // 5QI=1
    qos_list[uint_to_five_qi(1)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=2
    qos_list[uint_to_five_qi(2)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=3
    qos_list[uint_to_five_qi(3)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=4
    qos_list[uint_to_five_qi(4)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=65
    qos_list[uint_to_five_qi(65)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=66
    qos_list[uint_to_five_qi(66)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=67
    qos_list[uint_to_five_qi(67)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  //
  // Non-Guaranteed Bitrate 5QIs
  //
  {
    // 5QI=5
    qos_list[uint_to_five_qi(5)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=6
    qos_list[uint_to_five_qi(6)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=7
    qos_list[uint_to_five_qi(7)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=8
    qos_list[uint_to_five_qi(8)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=9
    qos_list[uint_to_five_qi(9)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=10
    qos_list[uint_to_five_qi(10)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=69
    qos_list[uint_to_five_qi(69)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=70
    qos_list[uint_to_five_qi(70)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  //
  // Delay-critical Guaranteed Bitrate 5QIs
  //
  {
    // 5QI=82
    qos_list[uint_to_five_qi(82)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=83
    qos_list[uint_to_five_qi(83)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=84
    qos_list[uint_to_five_qi(84)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=85
    qos_list[uint_to_five_qi(85)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=86
    qos_list[uint_to_five_qi(86)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=87
    qos_list[uint_to_five_qi(87)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=88
    qos_list[uint_to_five_qi(88)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=89
    qos_list[uint_to_five_qi(89)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }
  {
    // 5QI=90
    qos_list[uint_to_five_qi(90)] = make_default_cu_up_custom_cfg_item(warn_on_drop, metrics_period, test_mode);
  }

  return qos_list;
}

} // namespace ocudu::config_helpers
