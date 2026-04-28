// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/gtpu/gtpu_teid_pool_impl.h"
#include "tests/test_doubles/du/test_du_high_worker_manager.h"
#include "tests/test_doubles/f1ap/f1c_test_local_gateway.h"
#include "tests/test_doubles/mac/dummy_mac_result_notifier.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "ocudu/cu_cp/cu_cp.h"
#include "ocudu/du/du_high/du_high.h"
#include "ocudu/du/du_high/du_high_configuration.h"
#include "ocudu/ran/slot_point_extended.h"
#include "ocudu/support/executors/manual_task_worker.h"
#include "ocudu/support/executors/task_worker.h"

namespace ocudu {

class io_broker;

class du_high_cu_cp_worker_manager
{
public:
  explicit du_high_cu_cp_worker_manager(unsigned nof_dus, timer_manager& timers);
  ~du_high_cu_cp_worker_manager();

  void stop();

  manual_task_worker                                                 test_worker;
  std::vector<std::unique_ptr<test_helpers::du_high_worker_manager>> dus;
  task_executor*                                                     cu_cp_exec = nullptr;
};

struct du_high_cu_cp_test_simulator_config {
  std::vector<std::vector<odu::du_cell_config>> dus;
};

class du_high_cu_test_simulator
{
public:
  struct du_sim {
    odu::du_high_configuration    du_high_cfg;
    phy_test_dummy                phy;
    null_mac_pcap                 mac_pcap;
    null_rlc_pcap                 rlc_pcap;
    std::unique_ptr<odu::du_high> du_high_inst;

    slot_point_extended next_slot;

    du_sim(task_executor& phy_exec) : phy(1, phy_exec) {}
  };

  explicit du_high_cu_test_simulator(const du_high_cu_cp_test_simulator_config& cfg);
  ~du_high_cu_test_simulator();

  void start_dus();

  bool add_ue(unsigned du_index, rnti_t rnti);

  void run_slot();

  bool run_until(unique_function<bool()> condition);

  const du_high_cu_cp_test_simulator_config cfg;

  ocudulog::basic_logger&               logger;
  timer_manager                         timers;
  du_high_cu_cp_worker_manager          workers;
  std::unique_ptr<io_broker>            broker;
  std::unique_ptr<mac_clock_controller> timer_ctrl;
  ocucp::dummy_n2_gateway               n2_gw;
  f1c_test_local_gateway                f1c_gw;
  gtpu_teid_pool_impl f1u_teid_allocator{MAX_NOF_DU_UES * MAX_NOF_DRBS, GTPU_DEFAULT_TEID_RELEASE_LINGER_TIME, timers};

  std::unique_ptr<ocucp::cu_cp>        cu_cp_inst;
  std::vector<std::unique_ptr<du_sim>> dus;
};

} // namespace ocudu
