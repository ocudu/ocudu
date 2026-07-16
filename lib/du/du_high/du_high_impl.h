// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/du/du_high/du_high.h"
#include "ocudu/du/du_high/du_high_configuration.h"
#include "ocudu/du/du_high/du_manager/du_manager.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/mac/mac.h"
#include "ocudu/scheduler/scheduler_metrics.h"
#include <memory>

namespace ocudu {
namespace odu {

class du_test_mode_controller;

class du_high_impl final : public du_high
{
public:
  explicit du_high_impl(const du_high_configuration& cfg_, const du_high_dependencies& dependencies);
  ~du_high_impl() override;

  void start() override;

  void stop() override;

  f1ap_message_handler& get_f1ap_pdu_handler() override;

  f1ap_ue_id_translator& get_f1ap_ue_id_translator() override;

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) override;

  mac_cell_rach_handler& get_rach_handler(du_cell_index_t cell_index) override;

  mac_pdu_handler& get_pdu_handler() override;

  mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) override;

  du_configurator& get_du_configurator() override;

  mac_subframe_time_mapper& get_subframe_time_mapper() override;

private:
  class layer_connector;

  du_high_configuration cfg;

  ocudulog::basic_logger& logger;

  timer_manager& timers;

  /// \brief Whether the DU is in operational mode.
  /// \remark This doesn't need to be atomic as start/stop should be always called from the same thread.
  bool is_running = false;

  // Connection between DU-high layers.
  std::unique_ptr<layer_connector> adapters;

  std::unique_ptr<scheduler_metrics_notifier> hub_metrics;

  // Test mode controller: intercepts F1-C and MAC to run without a real CU (only when test mode is configured).
  std::unique_ptr<du_test_mode_controller> test_mode_ctrl;

  // DU-high Layers.
  std::unique_ptr<du_manager>    du_mng;
  std::unique_ptr<f1ap_du>       f1ap;
  std::unique_ptr<mac_interface> mac;
};

} // namespace odu
} // namespace ocudu
