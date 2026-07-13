// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../du_manager/converters/asn1_ref_time_r16_helpers.h"
#include "ocudu/adt/slotted_array.h"
#include "ocudu/du/du_high/du_manager/du_manager.h"
#include "ocudu/f1ap/du/f1ap_du.h"
#include "ocudu/mac/mac_subframe_time_mapper.h"
#include "ocudu/support/timers.h"

namespace ocudu {

class timer_manager;

namespace odu {

class f1ap_ue_task_scheduler_adapter final : public f1ap_ue_task_scheduler
{
public:
  explicit f1ap_ue_task_scheduler_adapter(du_ue_index_t ue_index_, timer_factory timers_) :
    ue_index(ue_index_), timers(timers_)
  {
  }

  unique_timer create_timer() override { return timers.create_timer(); }

  void schedule_async_task(async_task<void>&& task) override { du_mng->schedule_async_task(ue_index, std::move(task)); }

  void connect(du_manager_f1ap_event_handler& du_mng_) { du_mng = &du_mng_; }

private:
  du_ue_index_t                  ue_index;
  timer_factory                  timers;
  du_manager_f1ap_event_handler* du_mng = nullptr;
};

/// Adapts mac_subframe_time_mapper to f1ap_du_time_provider.
class f1ap_du_mac_time_provider_adapter : public f1ap_du_time_provider
{
public:
  void connect(mac_subframe_time_mapper& mapper_) { mapper = &mapper_; }

  std::optional<f1ap_du_slot_time_info> get_last_mapping(subcarrier_spacing scs) override
  {
    if (mapper == nullptr) {
      return std::nullopt;
    }
    auto m = mapper->get_last_mapping(scs);
    if (not m.has_value()) {
      return std::nullopt;
    }

    // mac_subframe_time_mapper always maps to the local system clock, so is_local_clock is hardcoded true.
    constexpr bool is_local_clock = true;

    return f1ap_du_slot_time_info{m->sl_tx, pack_ref_time_r16(m->time_point, is_local_clock), is_local_clock};
  }

private:
  mac_subframe_time_mapper* mapper = nullptr;
};

class f1ap_du_configurator_adapter : public f1ap_du_configurator, public f1ap_du_pws_notifier
{
public:
  explicit f1ap_du_configurator_adapter(timer_factory timers_) : timers(timers_)
  {
    for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
      ues.emplace(i, to_du_ue_index(i), timers);
    }
  }

  void connect(du_manager& du_mng_)
  {
    du_mng     = &du_mng_.get_f1ap_event_handler();
    pws_du_mng = &du_mng_.get_pws_handler();
    for (unsigned i = 0; i != MAX_NOF_DU_UES; ++i) {
      ues[i].connect(*du_mng);
    }
  }

  void connect_time_mapper(mac_subframe_time_mapper& mac_mapper) { time_provider_adapter.connect(mac_mapper); }

  timer_factory& get_timer_factory() override { return timers; }

  void schedule_async_task(async_task<void>&& task) override { du_mng->schedule_async_task(std::move(task)); }

  f1ap_ue_task_scheduler& get_ue_handler(du_ue_index_t ue_index) override { return ues[ue_index]; }

  void on_f1c_disconnection() override { return du_mng->handle_f1c_connection_loss(); }

  async_task<void> request_reset(const std::vector<du_ue_index_t>& ues_to_reset) override
  {
    return du_mng->handle_f1_reset_request(ues_to_reset);
  }

  du_ue_index_t find_free_ue_index() override { return du_mng->find_unused_du_ue_index(); }

  async_task<f1ap_ue_context_creation_response>
  request_ue_creation(const f1ap_ue_context_creation_request& request) override
  {
    return du_mng->handle_ue_context_creation(request);
  }

  async_task<f1ap_ue_context_update_response>
  request_ue_context_update(const f1ap_ue_context_update_request& request) override
  {
    return du_mng->handle_ue_context_update(request);
  }

  async_task<void> request_ue_removal(const f1ap_ue_delete_request& request) override
  {
    return du_mng->handle_ue_delete_request(request);
  }

  async_task<void> request_ue_drb_deactivation(du_ue_index_t ue_index) override
  {
    return du_mng->handle_ue_drb_deactivation_request(ue_index);
  }

  async_task<gnbcu_config_update_response>
  request_cu_context_update(const gnbcu_config_update_request& request) override
  {
    return du_mng->handle_cu_context_update_request(request);
  }

  void notify_reestablishment_of_old_ue(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index) override
  {
    du_mng->handle_ue_reestablishment(new_ue_index, old_ue_index);
  }

  void on_ue_config_applied(du_ue_index_t ue_index) override { du_mng->handle_ue_config_applied(ue_index); }

  f1ap_du_positioning_handler& get_positioning_handler() override { return du_mng->get_positioning_handler(); }

  f1ap_du_time_provider& get_time_provider() override { return time_provider_adapter; }

  async_task<std::vector<du_cell_index_t>>
  on_write_replace_warning_received(const write_replace_warning_information& msg) override
  {
    return pws_du_mng->handle_write_replace_warning(msg);
  }

private:
  timer_factory                                                 timers;
  du_manager_f1ap_event_handler*                                du_mng     = nullptr;
  du_manager_pws_handler*                                       pws_du_mng = nullptr;
  f1ap_du_mac_time_provider_adapter                             time_provider_adapter;
  slotted_array<f1ap_ue_task_scheduler_adapter, MAX_NOF_DU_UES> ues;
};

} // namespace odu
} // namespace ocudu
