// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "../test_helpers.h"
#include "lib/cu_cp/cell_meas_manager/cell_meas_manager_impl.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <optional>

namespace ocudu::ocucp {

class dummy_mobility_manager : public cell_meas_mobility_manager_notifier
{
public:
  void on_neighbor_better_than_spcell(cu_cp_ue_index_t     ue_index,
                                      gnb_id_t             neighbor_gnb_id,
                                      nr_cell_identity     neighbor_nci,
                                      pci_t                neighbor_pci,
                                      plmn_identity        neighbor_plmn,
                                      std::optional<tac_t> neighbor_tac = std::nullopt) override
  {
    fmt::print("on_neighbor_better_than_spcell() called.\n");
  }
};

/// Fixture class to create cell meas manager object.
class cell_meas_manager_test : public ::testing::Test
{
protected:
  cell_meas_manager_test();
  ~cell_meas_manager_test() override;

  void create_empty_manager();
  void create_default_manager(std::optional<unsigned> t312 = std::nullopt);
  void create_default_manager_with_cell_params();
  void create_manager_with_incomplete_cells_and_periodic_report_at_target_cell();
  void create_manager_without_ncells_and_periodic_report();
  void create_manager_inter_freq_without_periodic_report();
  void create_cho_manager_single_frequency();
  void create_cho_manager_multi_frequency();
  void create_cho_manager_multi_trigger();
  void create_cho_manager_a5_inter_frequency();
  void check_default_meas_cfg(const std::optional<rrc_meas_cfg>& meas_cfg, meas_obj_id_t meas_obj_id);
  void verify_meas_cfg(const std::optional<rrc_meas_cfg>& meas_cfg);
  void verify_empty_meas_cfg(const std::optional<rrc_meas_cfg>& meas_cfg);

  /// Attach the shared dummy RRC UE to the given UE so CHO capability checks pass.
  void attach_rrc_ue(cu_cp_ue_index_t ue_index)
  {
    cu_cp_ue* ue = ue_mng.find_ue(ue_index);
    ASSERT_NE(ue, nullptr);
    ue->set_rrc_ue(rrc_ue_stub);
  }

  ocudulog::basic_logger& test_logger  = ocudulog::fetch_basic_logger("TEST");
  ocudulog::basic_logger& cu_cp_logger = ocudulog::fetch_basic_logger("CU-CP", false);

  std::unique_ptr<cell_meas_manager> manager;
  dummy_mobility_manager             mobility_manager;
  dummy_rrc_ue                       rrc_ue_stub;
  manual_task_worker                 ctrl_worker{128};
  timer_manager                      timers;
  cu_cp_configuration                cu_cp_cfg;

  ue_manager ue_mng{cu_cp_cfg};
};

} // namespace ocudu::ocucp
