// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "metrics/rrc_du_metrics_aggregator.h"
#include "ue/rrc_ue_impl.h"
#include "ocudu/rrc/rrc_config.h"
#include "ocudu/rrc/rrc_du.h"
#include <unordered_map>

namespace ocudu::ocucp {

/// Adapter between RRC UE and RRC DU.
class rrc_ue_rrc_du_adapter : public rrc_ue_event_notifier
{
public:
  explicit rrc_ue_rrc_du_adapter(rrc_du_connection_event_handler& metrics_handler_) : metrics_handler(metrics_handler_)
  {
  }

  void on_new_rrc_connection() override { metrics_handler.handle_successful_rrc_setup(); }

  void on_attempted_rrc_connection_establishment(establishment_cause_t cause) override
  {
    metrics_handler.handle_attempted_rrc_setup(cause);
  }

  void on_successful_rrc_connection_establishment(establishment_cause_t cause) override
  {
    metrics_handler.handle_successful_rrc_setup(cause);
  }

  void on_failed_rrc_connection_establishment(establishment_fail_cause_t cause) override
  {
    metrics_handler.handle_failed_rrc_connection_establishment(cause);
  }

  void on_attempted_rrc_connection_reestablishment() override
  {
    metrics_handler.handle_attempted_rrc_reestablishment();
  }

  void on_successful_rrc_connection_reestablishment() override
  {
    metrics_handler.handle_successful_rrc_reestablishment();
  }

  void on_successful_rrc_connection_reestablishment_fallback() override
  {
    metrics_handler.handle_successful_rrc_reestablishment_fallback();
  }

  void on_successful_rrc_connection_resume(resume_cause_t cause) override
  {
    metrics_handler.handle_successful_rrc_resume(cause);
  }

  void on_successful_rrc_connection_resume_with_fallback(resume_cause_t cause) override
  {
    metrics_handler.handle_successful_rrc_resume_with_fallback(cause);
  }

  void on_rrc_connection_resume_followed_by_network_release(resume_cause_t cause) override
  {
    metrics_handler.handle_rrc_resume_followed_by_network_release(cause);
  }

  void on_attempted_rrc_connection_resume_followed_by_rrc_setup(resume_cause_t cause) override
  {
    metrics_handler.handle_attempted_rrc_resume_followed_by_rrc_setup(cause);
  }

private:
  rrc_du_connection_event_handler& metrics_handler;
};

/// Main RRC representation with the DU.
class rrc_du_impl : public rrc_du
{
public:
  rrc_du_impl(const rrc_cfg_t& cfg_);
  ~rrc_du_impl() = default;

  // rrc_du_cell_manager.
  std::map<nr_cell_global_id_t, rrc_cell_info>
  get_cell_info(const std::vector<cu_cp_du_served_cells_item>& served_cell_list) const override;

  std::vector<rrc_plmn_ran_area_cell_t> get_ran_area_cells() override;

  void store_cell_info_db(const std::map<nr_cell_global_id_t, rrc_cell_info>& cell_infos) override;

  // rrc_du_ue_repository.
  byte_buffer                         get_rrc_reject() override;
  byte_buffer                         pack_meas_config(const rrc_meas_cfg& meas_cfg) override;
  std::optional<rrc_resume_context_t> get_rrc_resume_context(byte_buffer rrc_container,
                                                             uint8_t     nof_i_rnti_ue_bits) override;
  rrc_ue_interface*                   add_ue(const rrc_ue_creation_message& msg) override;
  void                                release_ues() override;
  size_t                              get_nof_ues() const override { return ue_db.size(); }

  // rrc_ue_handler.
  rrc_ue_interface* find_ue(cu_cp_ue_index_t ue_index) override
  {
    if (ue_db.find(ue_index) == ue_db.end()) {
      return nullptr;
    }
    return ue_db.at(ue_index).get();
  }
  void remove_ue(cu_cp_ue_index_t ue_index) override;

  // rrc_du_connection_event_handler.
  void handle_successful_rrc_setup(std::optional<establishment_cause_t> cause) override;
  void handle_rrc_inactive() override;
  void handle_attempted_rrc_setup(establishment_cause_t cause) override;
  void handle_failed_rrc_connection_establishment(establishment_fail_cause_t cause) override;
  void handle_attempted_rrc_reestablishment() override;
  void handle_successful_rrc_reestablishment() override;
  void handle_successful_rrc_reestablishment_fallback() override;
  void handle_attempted_rrc_resume(resume_cause_t cause) override;
  void handle_successful_rrc_resume(resume_cause_t cause) override;
  void handle_successful_rrc_resume_with_fallback(resume_cause_t cause) override;
  void handle_rrc_resume_followed_by_network_release(resume_cause_t cause) override;
  void handle_attempted_rrc_resume_followed_by_rrc_setup(resume_cause_t cause) override;

  rrc_du_cell_manager&             get_rrc_du_cell_manager() override { return *this; }
  rrc_du_ue_repository&            get_rrc_du_ue_repository() override { return *this; }
  rrc_ue_handler&                  get_rrc_ue_handler() override { return *this; }
  rrc_du_connection_event_handler& get_rrc_du_connection_event_handler() override { return *this; }
  rrc_du_metrics_collector&        get_rrc_du_metrics_collector() override { return metrics_aggregator; }

private:
  // Helpers.
  const rrc_cfg_t cfg;

  // Notifier to the CU-CP.
  ocudulog::basic_logger& logger;

  // RRC UE to RRC DU adapters.
  std::unordered_map<cu_cp_ue_index_t, rrc_ue_rrc_du_adapter> rrc_ue_rrc_du_adapters;

  // RRC-internal user database indexed by ue_index.
  std::unordered_map<cu_cp_ue_index_t, std::unique_ptr<rrc_ue_impl>> ue_db;
  // Cell database to store cell information from the DU.
  std::map<nr_cell_identity, rrc_cell_info> cell_info_db;

  // Metrics aggregator.
  rrc_du_metrics_aggregator metrics_aggregator;
};

} // namespace ocudu::ocucp
