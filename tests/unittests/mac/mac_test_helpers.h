// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/mac/mac_dl/mac_scheduler_cell_info_handler.h"
#include "tests/test_doubles/utils/test_rng.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/mac/cell_configuration.h"
#include "ocudu/mac/config/mac_cell_group_config_factory.h"
#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/mac/mac_clock_controller.h"
#include "ocudu/mac/mac_subframe_time_mapper.h"
#include "ocudu/mac/mac_ue_configurator.h"
#include "ocudu/ran/rrm.h"
#include "ocudu/scheduler/config/cell_config_builder_params.h"
#include "ocudu/scheduler/config/ran_cell_config_helper.h"
#include "ocudu/scheduler/config/serving_cell_config_factory.h"
#include "ocudu/scheduler/mac_scheduler.h"
#include "ocudu/scheduler/result/sched_result.h"
#include "ocudu/support/timers.h"

namespace ocudu::test_helpers {

/// Generates default MAC Cell Configuration to be used in unit tests.
inline mac_cell_creation_request make_default_mac_cell_config(const cell_config_builder_params& params = {})
{
  mac_cell_creation_request req{};

  config_helpers::cell_config_builder_params_extended params_ext{params};

  req.pci        = params.pci;
  req.cell_index = to_du_cell_index(0);
  req.scs_common = params.scs_common;
  req.dl_carrier = config_helpers::make_default_dl_carrier_configuration(params);
  req.ul_carrier = config_helpers::make_default_ul_carrier_configuration(params);
  req.ssb_cfg    = config_helpers::make_default_ssb_config(params);
  req.cs0_index  = params_ext.cs0_index.value();
  req.ss0_index  = params_ext.ss0_index;
  req.sched_req  = {}; // Note: MAC should not touch this field.

  byte_buffer dummy_sib1;
  for (unsigned i = 0; i != 100; ++i) {
    report_fatal_error_if_not(dummy_sib1.append(i), "Failed to append to create dummy SIB1");
  }
  req.sys_info.sib1 = std::move(dummy_sib1);
  return req;
}

class dummy_ue_rlf_notifier : public mac_ue_radio_link_notifier
{
public:
  bool                         rlf_detected      = false;
  bool                         crnti_ce_detected = false;
  std::optional<mac_rlf_cause> last_cause;

  void on_rlf_detected(mac_rlf_cause cause) override
  {
    rlf_detected = true;
    last_cause   = cause;
  }

  void on_crnti_ce_received() override { crnti_ce_detected = true; }
};

inline mac_ue_create_request make_default_ue_creation_request(const cell_config_builder_params& params = {})
{
  mac_ue_create_request msg{};

  msg.ue_index   = to_du_ue_index(0);
  msg.crnti      = to_rnti(0x4601);
  msg.cell_index = to_du_cell_index(0);

  msg.rlf_notifier = nullptr;

  msg.mac_cell_group_cfg = config_helpers::make_initial_mac_cell_group_config();

  physical_cell_group_config& pcg_cfg = msg.phy_cell_group_cfg;
  pcg_cfg.pdsch_harq_codebook         = pdsch_harq_ack_codebook::dynamic;

  msg.sched_cfg.cells.emplace();
  msg.sched_cfg.cells->push_back(
      config_helpers::make_default_ue_cell_config(config_helpers::make_default_ran_cell_config(params)));

  return msg;
}

class dummy_mac_scheduler : public mac_scheduler
{
public:
  sched_result                           next_sched_result = {};
  std::optional<rach_indication_message> last_rach_ind;

  bool handle_cell_configuration_request(const sched_cell_configuration_request_message& msg) override { return true; }
  void handle_cell_removal_request(du_cell_index_t cell_index) override {}
  void handle_cell_activation_request(du_cell_index_t cell_index) override {}
  void handle_cell_deactivation_request(du_cell_index_t cell_index) override {}
  void handle_rach_indication(const rach_indication_message& msg) override { last_rach_ind = msg; }
  void handle_ue_creation_request(const sched_ue_creation_request_message& ue_request) override {}
  void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) override {}
  void handle_ue_removal_request(du_ue_index_t ue_index) override {}
  void handle_ue_config_applied(du_ue_index_t ue_index) override {}
  void handle_ue_deactivation_request(du_ue_index_t ue_index) override {}
  void handle_si_update_request(const si_scheduling_update_request& req) override {}
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override {}
  void handle_crc_indication(const ul_crc_indication& crc) override {}
  void handle_uci_indication(const uci_indication& uci) override {}
  void handle_srs_indication(const srs_indication& srs) override {}
  void handle_ul_phr_indication(const ul_phr_indication_message& phr_ind) override {}
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce) override {}
  void handle_crnti_ce_received(du_ue_index_t ue_index) override {}
  void handle_paging_information(const sched_paging_information& pi) override {}
  const sched_result& slot_indication(slot_point_extended sl_tx, du_cell_index_t cell_index) override
  {
    return next_sched_result;
  }
  void handle_error_indication(slot_point sl_tx, du_cell_index_t cell_index, error_outcome event) override {}
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs) override {}
  void handle_positioning_measurement_request(const positioning_measurement_request& req) override {}
  void handle_positioning_measurement_stop(const positioning_measurement_stop_request& req) override {}
  void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& req) override {}
};

class dummy_mac_sfn_time_mapper : public mac_subframe_time_mapper
{
public:
  std::optional<mac_slot_time_info> get_last_mapping(subcarrier_spacing scs) const override
  {
    return mac_slot_time_info{slot_point(to_numerology_value(scs), 1), std::chrono::system_clock::now()};
  }
  std::optional<time_point> get_time_point(slot_point slot) const override { return std::nullopt; }
  std::optional<slot_point> get_slot_point(time_point time, subcarrier_spacing scs) const override
  {
    return std::nullopt;
  }
};

class dummy_mac_sfn_time_adapter : public mac_slot_time_handler
{
public:
  void handle_slot_indication(const mac_cell_timing_context& context) override {}
};

class dummy_mac_scheduler_adapter : public mac_scheduler_cell_info_handler
{
public:
  bool         active            = false;
  sched_result next_sched_result = {};

  void handle_dl_buffer_state_update(const mac_dl_buffer_state_indication_message& dl_bs) override {}

  void handle_cell_activation(du_cell_index_t cell_idx) override { active = true; }

  void handle_cell_deactivation(du_cell_index_t cell_idx) override { active = false; }

  const sched_result& slot_indication(slot_point_extended slot_tx, du_cell_index_t cell_idx) override
  {
    return next_sched_result;
  }
  void handle_error_indication(slot_point                         slot_tx,
                               du_cell_index_t                    cell_idx,
                               mac_cell_slot_handler::error_event event) override
  {
  }

  void handle_si_change_indication(const si_scheduling_update_request& request) override {}

  void handle_slice_reconfiguration_request(const du_cell_slice_reconfig_request& req) override {}
};

class dummy_mac_cell_result_notifier : public mac_cell_result_notifier
{
public:
  std::optional<mac_dl_sched_result> last_sched_res;
  std::optional<mac_dl_data_result>  last_dl_data_res;
  std::optional<mac_ul_sched_result> last_ul_res;
  bool                               is_complete = false;

  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override { last_sched_res = dl_res; }
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override { last_dl_data_res = dl_data; }
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override { last_ul_res = ul_res; }
  void on_cell_results_completion(slot_point slot) override { is_complete = true; }
};

class mac_sdu_rx_test_notifier : public mac_sdu_rx_notifier
{
public:
  byte_buffer_slice last_sdu;

  void on_new_sdu(byte_buffer_slice sdu) override { last_sdu = std::move(sdu); }
};

class mac_sdu_tx_builder_test_notifier : public mac_sdu_tx_builder
{
public:
  unsigned    next_bs = 0;
  byte_buffer previous_tx_sdu;

  size_t on_new_tx_sdu(span<uint8_t> mac_sdu_buf) override
  {
    previous_tx_sdu = byte_buffer::create(test_rng::vector_of_uniform_ints<uint8_t>(mac_sdu_buf.size())).value();
    auto* out_it    = mac_sdu_buf.begin();
    for (span<const uint8_t> seg : previous_tx_sdu.segments()) {
      out_it = std::copy(seg.begin(), seg.end(), out_it);
    }
    return mac_sdu_buf.size();
  }

  rlc_buffer_state on_buffer_state_update() override
  {
    rlc_buffer_state bs = {};
    bs.pending_bytes    = next_bs;
    // TODO: set bs.hol_toa
    return bs;
  }
};

class dummy_mac_clock_controller : public mac_clock_controller
{
public:
  class dummy_mac_cell_clock_controller : public mac_cell_clock_controller
  {
  public:
    dummy_mac_cell_clock_controller(dummy_mac_clock_controller& parent_, du_cell_index_t cell_index_) :
      parent(parent_), cell_index(cell_index_)
    {
    }

    slot_point_extended do_on_slot_indication(slot_point_extended sl_tx) override
    {
      if (not parent.active_cells[cell_index]) {
        parent.active_cells[cell_index] = true;
        if (parent.nof_active_cells == 0) {
          parent.master_slot = sl_tx - 1;
        }
        parent.nof_active_cells++;
      }

      if (sl_tx.subframe_slot_index() != 0) {
        return parent.master_slot;
      }

      if (sl_tx > parent.master_slot) {
        unsigned nof_ticks = sl_tx - parent.master_slot;
        parent.master_slot = sl_tx;
        for (unsigned i = 0; i != nof_ticks; ++i) {
          parent.timers.tick();
        }
      }
      return parent.master_slot;
    }

    void on_cell_deactivation() override
    {
      if (parent.active_cells[cell_index]) {
        parent.active_cells[cell_index] = false;
        parent.nof_active_cells--;
      }
    }

  private:
    dummy_mac_clock_controller& parent;
    du_cell_index_t             cell_index;
  };

  dummy_mac_clock_controller(timer_manager& timers_) : timers(timers_) {}

  std::unique_ptr<mac_cell_clock_controller> add_cell(du_cell_index_t cell_index) override
  {
    return std::make_unique<dummy_mac_cell_clock_controller>(*this, cell_index);
  }

  timer_manager& get_timer_manager() override { return timers; }

private:
  timer_manager& timers;

  unsigned                           nof_active_cells = 0;
  slot_point_extended                master_slot;
  std::array<bool, MAX_NOF_DU_CELLS> active_cells{false};
};

struct mac_test_ue_bearer {
  mac_sdu_rx_test_notifier         ul_notifier;
  mac_sdu_tx_builder_test_notifier dl_notifier;
  mac_logical_channel_config       bearer;
};

struct mac_test_ue {
  du_ue_index_t                                       ue_index;
  rnti_t                                              rnti;
  slotted_array<mac_test_ue_bearer, MAX_NOF_RB_LCIDS> bearers;

  void                  add_bearer(lcid_t lcid);
  mac_ue_create_request make_ue_create_request();
};

} // namespace ocudu::test_helpers
