// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "ocudu/mac/mac_cell_result.h"
#include "ocudu/scheduler/result/sched_result.h"

namespace ocudu {
class task_executor;

struct phy_cell_test_dummy : public mac_cell_result_notifier {
public:
  explicit phy_cell_test_dummy(task_executor& exec_);
  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override;
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override;
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override;
  void on_cell_results_completion(slot_point slot) override;

  slot_point                         last_slot_res;
  std::optional<mac_dl_sched_result> last_dl_res;
  std::optional<mac_dl_data_result>  last_dl_data;
  std::optional<mac_ul_sched_result> last_ul_res;

private:
  task_executor&  test_exec;
  dl_sched_result last_dl_sched_res;
  ul_sched_result last_ul_sched_res;

  std::optional<mac_dl_sched_result> cached_dl_res;
  std::optional<mac_dl_data_result>  cached_dl_data;
  std::optional<mac_ul_sched_result> cached_ul_res;
};

/// \brief Dummy implementation of mac_cell_result_notifier that stores the last results in the class. The user can
/// specify through which task executor the results are transferred and stored.
struct phy_test_dummy : public mac_result_notifier {
public:
  phy_test_dummy(unsigned nof_cells, task_executor& exec_) : cells(nof_cells, phy_cell_test_dummy(exec_)) {}

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cells[cell_index]; }

  std::vector<phy_cell_test_dummy> cells;
};

} // namespace ocudu
