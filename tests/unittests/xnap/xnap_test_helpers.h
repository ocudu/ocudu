// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "lib/xnap/xnap_impl.h"
#include "ocudu/ocudulog/ocudulog.h"
#include "ocudu/ran/cause/xnap_cause.h"
#include "ocudu/support/async/async_no_op_task.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/xnap/gateways/xnc_connection_gateway.h"
#include "ocudu/xnap/xnap_message.h"
#include "ocudu/xnap/xnap_message_notifier.h"
#include <chrono>
#include <gtest/gtest.h>

namespace ocudu::ocucp {

/// Dummy class to check for XN TX messages.
class dummy_xnap_message_notifier : public xnap_message_notifier
{
public:
  dummy_xnap_message_notifier(xnap_message& last_msg_) : last_msg(last_msg_) {}
  ~dummy_xnap_message_notifier() override = default;

  bool on_new_message(const xnap_message& msg) override
  {
    last_msg = msg;
    return true;
  }

  xnap_message& last_msg;
};

/// Reusable class that stores the messages sent over XNAP for test inspection.
class dummy_xnc_gateway : public xnc_connection_gateway
{
public:
  dummy_xnc_gateway() : logger(ocudulog::fetch_basic_logger("TEST")) {}

  async_task<bool> connect_to_peer(std::vector<transport_layer_address> peer_addrs) override
  {
    return launch_no_op_task(true);
  }

  void attach_cu_cp(cu_cp_xnc_handler& xnc_handler_) override { logger.info("CU-CP attached to XN-C gateway"); }

  void stop() override {}

  std::optional<uint16_t> get_listen_port() const override { return std::nullopt; }

private:
  ocudulog::basic_logger& logger;
};

class dummy_xnap_cu_cp_notifier : public xnap_cu_cp_notifier
{
public:
  dummy_xnap_cu_cp_notifier(ue_manager& ue_mng_) : ue_mng(ue_mng_), logger(ocudulog::fetch_basic_logger("TEST")) {}

  void set_xnap_handover_request_outcome(bool success) { ho_request_outcome = success; }

  async_task<bool> on_new_rrc_handover_command(cu_cp_ue_index_t ue_index, byte_buffer command) override
  {
    logger.info("Received a new RRC Handover Command for UE index {}", ue_index);
    last_handover_command = std::move(command);
    return launch_async([](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(true);
    });
  }

  cu_cp_ue_index_t request_new_ue_index_allocation(const nr_cell_global_id_t& cgi, const plmn_identity& plmn) override
  {
    if (ho_request_outcome) {
      cu_cp_ue_index_t ue_index = ue_mng.add_ue(cu_cp_du_index_t::min);
      if (ue_index == cu_cp_ue_index_t::invalid) {
        logger.error("Failed to create UE");
        return cu_cp_ue_index_t::invalid;
      }
      if (ue_mng.ue_admission_limit_reached()) {
        ue_mng.remove_ue(ue_index);
        logger.error("Failed to create UE. UE not servable");
        return cu_cp_ue_index_t::invalid;
      }

      return ue_index;
    }
    return cu_cp_ue_index_t::invalid;
  }

  bool on_handover_request_received(cu_cp_ue_index_t                  ue_index,
                                    const plmn_identity&              selected_plmn,
                                    const security::security_context& sec_ctxt) override
  {
    ocudu_assert(ue_mng.find_ue(ue_index) != nullptr, "UE must be present\n");
    logger.info("Received a handover request");

    if (!ue_mng.find_ue(ue_index)->get_security_manager().init_security_context(sec_ctxt)) {
      logger.info("Failed to initialize security context");
      return false;
    }

    return true;
  }

  bool schedule_async_task(cu_cp_ue_index_t ue_index, async_task<void> task) override
  {
    ocudu_assert(ue_mng.find_ue_task_scheduler(ue_index) != nullptr, "UE task scheduler must be present");
    return ue_mng.find_ue_task_scheduler(ue_index)->schedule_async_task(std::move(task));
  }

  async_task<cu_cp_handover_resource_allocation_response>
  on_xnap_handover_request(const xnap_handover_request& request) override
  {
    cu_cp_handover_resource_allocation_response response;
    // Generate dummy response based on pre-configured outcome.
    if (ho_request_outcome) {
      cu_cp_handover_request_ack ho_ack;
      ho_ack.ue_index = request.ue_index;
      for (const auto& session_to_setup : request.ue_context_info_ho_request.pdu_session_res_to_be_setup_list) {
        cu_cp_xn_pdu_session_res_admitted_item item;
        item.pdu_session_id = session_to_setup.pdu_session_id;
        item.data_forwarding_info_from_target.emplace();
        for (const auto& qos_flow_to_setup : session_to_setup.qos_flow_setup_request_items) {
          cu_cp_qos_flow_with_data_forwarding_item qos_flow_item;
          qos_flow_item.qos_flow_id = qos_flow_to_setup.qos_flow_id;
          item.qos_flows_setup_list.push_back(qos_flow_item);
          item.data_forwarding_info_from_target->qos_flows_accepted_for_data_forwarding_list.push_back(
              qos_flow_to_setup.qos_flow_id);
        }
        item.data_forwarding_info_from_target->data_forwarding_resp_drb_item_list.push_back({.drb_id = drb_id_t::drb1});

        ho_ack.pdu_session_res_admitted_list.push_back(item);
      }
      ho_ack.rrc_handover_command = make_byte_buffer("deadbeef").value();

      response = ho_ack;
    } else {
      cu_cp_handover_request_failure ho_fail;
      ho_fail.ue_index = request.ue_index;
      ho_fail.cause    = xnap_cause_radio_network_t::unspecified;

      response = ho_fail;
    }

    return launch_async([res = std::move(response)](
                            coro_context<async_task<cu_cp_handover_resource_allocation_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      CORO_RETURN(res);
    });
  }

  void on_xn_handover_execution(cu_cp_ue_index_t                              ue_index,
                                const xnap_handover_target_execution_context& xnap_ho_target_execution_ctxt) override
  {
    logger.info("Requested XN handover execution for UE index {}", ue_index);
  }

  void on_handover_success_received(cu_cp_ue_index_t source_ue_index, peer_xnap_ue_id_t winner_peer_xnap_ue_id) override
  {
    logger.info("HandoverSuccess received for source UE index {}", source_ue_index);
  }

  void on_handover_cancel_received(cu_cp_ue_index_t ue_index) override
  {
    logger.info("Received a handover cancel for UE index {}", ue_index);
  }

  void on_ue_context_release_received(cu_cp_ue_index_t ue_index) override
  {
    logger.info("Received a UE context release for UE index {}", ue_index);
  }

  byte_buffer last_handover_command;

private:
  bool ho_request_outcome = false;

  ue_manager&             ue_mng;
  ocudulog::basic_logger& logger;
};

/// Fixture class for XNAP Setup tests.
class xnap_test : public ::testing::Test
{
protected:
  xnap_test();

  void TearDown() override;

  /// \brief Helper method to successfully run XN setup in XNAP.
  bool run_xn_setup(const xnap_configuration& peer_cfg);

  /// \brief Helper method to successfully create UE instance in ue manager.
  cu_cp_ue_index_t create_ue(rnti_t rnti = rnti_t::MIN_CRNTI);

  /// \brief Manually tick timers.
  template <typename T>
  bool tick(async_task<T>& task, std::chrono::milliseconds duration)
  {
    for (unsigned msec_elapsed = 0; msec_elapsed < duration.count(); ++msec_elapsed) {
      if (task.ready()) {
        return false;
      };
      timers.tick();
      ctrl_worker.run_pending_tasks();
    }
    return true;
  }

  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("TEST", false);
  timer_manager           timers;
  manual_task_worker      ctrl_worker{128};
  cu_cp_configuration     cu_cp_cfg;

  ue_manager                 ue_mng{cu_cp_cfg};
  dummy_xnc_gateway          xnc_gw;
  dummy_xnap_cu_cp_notifier  cu_cp_notifier{ue_mng};
  std::unique_ptr<xnap_impl> xnap = nullptr;

  /// Local configuration.
  gnb_id_t               local_gnb_id             = {0, 22};
  plmn_identity          local_plmn               = plmn_identity::test_value();
  tac_t                  local_tac                = {8};
  s_nssai_t              local_slice              = {};
  std::vector<s_nssai_t> local_slice_support_list = {local_slice};
  xnap_configuration     xnap_local_cfg           = {
      std::chrono::milliseconds{5000},
      std::chrono::milliseconds{10000},
      local_gnb_id,
      std::vector<supported_tracking_area>{{local_tac, std::vector<plmn_item>{{local_plmn, local_slice_support_list}}}},
      std::vector<guami_t>{{.plmn = local_plmn, .amf_set_id = 0, .amf_pointer = 0, .amf_region_id = 1}}};

  /// Peer configuration.
  gnb_id_t               peer_gnb_id             = {1, 22};
  plmn_identity          peer_plmn               = plmn_identity::test_value();
  tac_t                  peer_tac                = {7};
  s_nssai_t              peer_slice              = {};
  std::vector<s_nssai_t> peer_slice_support_list = {peer_slice};

  xnap_configuration xnap_peer_cfg = {
      std::chrono::milliseconds{5000},
      std::chrono::milliseconds{10000},
      peer_gnb_id,
      std::vector<supported_tracking_area>{{peer_tac, std::vector<plmn_item>{{peer_plmn, peer_slice_support_list}}}},
      std::vector<guami_t>{{peer_plmn, 1}},
  };

  xnap_message get_last_message() { return last_tx_msg; }

private:
  xnap_message last_tx_msg;
};

} // namespace ocudu::ocucp
