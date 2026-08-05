// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/mac/mac_config_interfaces.h"
#include "lib/mac/mac_ctrl/mac_scheduler_configurator.h"
#include "mac_test_helpers.h"
#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/mac/mac_executor_mapper.h"
#include "ocudu/pcap/dlt_pcap.h"
#include "ocudu/scheduler/scheduler_metrics.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/async/manual_event.h"
#include "ocudu/support/executors/task_executor.h"

namespace ocudu {

class mac_ctrl_dummy_configurer final : public mac_ctrl_configurator
{
public:
  rnti_t add_ue(du_ue_index_t ue_index, du_cell_index_t pcell_index, rnti_t rnti) override { return rnti; }
  void   remove_ue(du_ue_index_t ue_index) override {}
};

/// Dummy class to emulate MAC UL procedures to add/remove/configure UEs
class mac_ul_dummy_configurer final : public mac_ul_configurator
{
public:
  bool                                                                             expected_result   = true;
  bool                                                                             ul_ccch_forwarded = false;
  manual_event_flag                                                                ue_created_ev;
  std::optional<mac_ue_create_request>                                             last_ue_create_request;
  std::optional<mac_ue_delete_request>                                             last_ue_delete_request;
  std::optional<std::pair<du_ue_index_t, std::vector<mac_logical_channel_config>>> last_ue_bearers_added;
  std::optional<std::pair<du_ue_index_t, std::vector<lcid_t>>>                     last_ue_bearers_rem;

  async_task<bool> add_ue(const mac_ue_create_request& msg) override;
  async_task<bool> addmod_bearers(du_ue_index_t                                  ue_index,
                                  const std::vector<mac_logical_channel_config>& ul_logical_channels) override;
  async_task<bool> remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids_to_rem) override;
  async_task<void> remove_ue(const mac_ue_delete_request& msg) override;
  bool             flush_ul_ccch_msg(du_ue_index_t    ue_index,
                                     du_cell_index_t  cell_index,
                                     slot_point       slot_rx,
                                     byte_buffer      ul_ccch_msg,
                                     msg3_mac_ce_list mac_ces) override
  {
    if (not ul_ccch_msg.empty()) {
      ul_ccch_forwarded = true;
    }
    return true;
  }
  void handle_ue_config_applied(du_ue_index_t ue_index) override {}
};

class mac_cell_dummy_controller final : public mac_dl_cell_controller
{
public:
  unsigned                                    nof_starts = 0;
  std::optional<si_version_type>              last_si_version;
  std::optional<si_version_type>              last_pws_si_version;
  std::unique_ptr<pws_broadcast_end_notifier> pws_end_notifier;

  async_task<void> start() override;
  async_task<void> stop() override { return start(); }
  void             start_broadcast(std::shared_ptr<si_message_extension_handler> ext_handler,
                                   const si_update_command&                      cmd,
                                   std::unique_ptr<pws_broadcast_end_notifier>   pws_end_notifier_) override
  {
    pws_end_notifier = std::move(pws_end_notifier_);
    handle_si_update(cmd);
  }
  void handle_si_update(const si_update_command& cmd) override
  {
    if (not cmd.active_pws_si_messages.empty()) {
      last_pws_si_version = cmd.version;
      return;
    }
    last_si_version = cmd.version;
  }
  async_task<void> reconfigure(const mac_dl_cell_reconfig_request& request) override { return start(); }
};

class mac_dl_dummy_configurer final : public mac_dl_configurator
{
public:
  bool                                                                             expected_result = true;
  manual_event_flag                                                                ue_created_ev;
  std::optional<mac_ue_create_request>                                             last_ue_create_request;
  std::optional<mac_ue_delete_request>                                             last_ue_delete_request;
  std::optional<std::pair<du_ue_index_t, std::vector<mac_logical_channel_config>>> last_ue_bearers_added;
  std::optional<std::pair<du_ue_index_t, std::vector<lcid_t>>>                     last_ue_bearers_rem;
  std::vector<std::unique_ptr<mac_cell_dummy_controller>>                          cell_ctrls;
  test_helpers::dummy_mac_sfn_time_mapper                                          sfn_time_mapper;

  async_task<bool> add_ue(const mac_ue_create_request& msg) override;
  async_task<void> remove_ue(const mac_ue_delete_request& msg) override;
  async_task<bool> addmod_bearers(du_ue_index_t                          ue_index,
                                  du_cell_index_t                        pcell_index,
                                  span<const mac_logical_channel_config> logical_channels) override;
  async_task<bool>
  remove_bearers(du_ue_index_t ue_index, du_cell_index_t pcell_index, span<const lcid_t> lcids_to_rem) override;

  mac_dl_cell_controller& add_cell(const mac_cell_creation_request& cell_cfg,
                                   mac_cell_config_dependencies     deps) override
  {
    cell_ctrls.push_back(std::make_unique<mac_cell_dummy_controller>());
    return *cell_ctrls.back();
  }
  void                      remove_cell(du_cell_index_t cell_index) override {}
  mac_subframe_time_mapper& get_subframe_time_mapper() override { return sfn_time_mapper; }
};

class dummy_ue_executor_mapper : public mac_ue_executor_mapper
{
public:
  dummy_ue_executor_mapper(task_executor& exec_) : exec(exec_) {}

  void           rebind_executors(du_ue_index_t ue_index, du_cell_index_t pcell_index) override {}
  task_executor& ctrl_executor(du_ue_index_t ue_index) override { return exec; }
  task_executor& mac_ul_pdu_executor(du_ue_index_t ue_index) override { return exec; }

  task_executor& exec;
};

class dummy_dl_executor_mapper : public mac_cell_executor_mapper
{
public:
  dummy_dl_executor_mapper(const std::initializer_list<task_executor*>& execs_) : execs(execs_.begin(), execs_.end()) {}

  task_executor& mac_cell_executor(du_cell_index_t cell_index) override { return *execs[cell_index % execs.size()]; }
  task_executor& slot_ind_executor(du_cell_index_t cell_index) override { return *execs[cell_index % execs.size()]; }

  std::vector<task_executor*> execs;
};

class dummy_mac_event_indicator : public mac_ul_ccch_notifier
{
public:
  std::optional<ul_ccch_indication_message>     last_ccch_ind;
  std::optional<ul_crnti_ce_indication_message> last_crnti_ce_ind;

  void on_ul_ccch_msg_received(const ul_ccch_indication_message& msg) override { last_ccch_ind = msg; }
  void on_crnti_ce_received(const ul_crnti_ce_indication_message& msg) override { last_crnti_ce_ind = msg; }

  bool verify_ul_ccch_msg(const ul_ccch_indication_message& test_msg)
  {
    if (not last_ccch_ind.has_value()) {
      return false;
    }
    bool test = last_ccch_ind.value().tc_rnti == test_msg.tc_rnti &&
                last_ccch_ind.value().cell_index == test_msg.cell_index &&
                last_ccch_ind.value().slot_rx == test_msg.slot_rx && last_ccch_ind.value().subpdu == test_msg.subpdu;
    return test;
  }

  bool verify_no_ul_ccch_msg() const { return not last_ccch_ind.has_value(); }
};

class dummy_mac_cell_result_notifier : public mac_cell_result_notifier
{
public:
  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override {}
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override {}
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override {}
  void on_cell_results_completion(slot_point slot) override {}
};

class dummy_mac_result_notifier : public mac_result_notifier
{
  dummy_mac_cell_result_notifier cell;

public:
  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
};

class mac_scheduler_dummy_adapter : public mac_scheduler_configurator
{
public:
  manual_event<bool>                   ue_created_ev;
  std::optional<mac_ue_create_request> last_ue_create_request;

  test_helpers::dummy_mac_scheduler_adapter cell_cfg;

  void add_cell(const mac_scheduler_cell_creation_request& msg) override {}

  void remove_cell(du_cell_index_t cell_index) override {}

  async_task<bool> handle_ue_creation_request(const mac_ue_create_request& msg) override;
  async_task<bool> handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& msg) override;
  async_task<void> handle_ue_removal_request(const mac_ue_delete_request& msg) override;
  void             handle_ue_config_applied(du_ue_index_t ue_idx) override {}

  class dummy_notifier : public sched_configuration_notifier
  {
    void on_ue_config_complete(du_ue_index_t ue_index, bool success) override {}
    void on_ue_deletion_completed(du_ue_index_t ue_index) override {}
  } notifier;
};

} // namespace ocudu
