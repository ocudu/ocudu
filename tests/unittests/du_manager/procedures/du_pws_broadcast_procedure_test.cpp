// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../du_manager_test_helpers.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/du/du_high/du_manager/du_manager_factory.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/executors/task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

namespace {

class du_pws_broadcast_procedure_test : public ::testing::Test
{
protected:
  du_pws_broadcast_procedure_test() :
    cell_cfgs({config_helpers::make_default_du_cell_config()}),
    dependencies(cell_cfgs),
    du_mng(create_du_manager(dependencies.params))
  {
    dependencies.f1ap.wait_f1_setup.result.value().cells_to_activate.resize(1);
    dependencies.f1ap.wait_f1_setup.result.value().cells_to_activate[0].cgi = cell_cfgs[0].nr_cgi;
    dependencies.f1ap.wait_f1_setup.ready_ev.set();
    dependencies.f1ap.wait_f1_removal.ready_ev.set();
    dependencies.mac.mac_cell.wait_start.ready_ev.set();
    dependencies.mac.mac_cell.wait_stop.ready_ev.set();

    du_mng->get_controller().start();
  }
  ~du_pws_broadcast_procedure_test() override
  {
    std::atomic<bool> done{false};
    worker.push_task_blocking([this, &done]() {
      du_mng->get_controller().stop();
      done = true;
    });
    while (not done) {
      dependencies.worker.run_pending_tasks();
      std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    worker.wait_pending_tasks();
  }

  task_worker                 worker{"worker", 16};
  std::vector<du_cell_config> cell_cfgs;
  du_manager_test_bench       dependencies;
  std::unique_ptr<du_manager> du_mng;
};

} // namespace

TEST_F(du_pws_broadcast_procedure_test, when_cell_not_provisioned_for_sib_type_then_it_is_not_accepted)
{
  write_replace_warning_information req;
  req.sib_type                 = 6;
  req.sib_msg                  = byte_buffer::create({0x1, 0x2, 0x3}).value();
  req.repeat_period            = std::chrono::seconds{60};
  req.nof_broadcasts_requested = 4;
  req.cells                    = {du_cell_index_t::MIN_DU_CELL_INDEX};

  // The default cell config has no static SI window for SIB6/7/8 (no etws_cfg/cmas_cfg provisioned).
  async_task<std::vector<du_cell_index_t>>         t = du_mng->get_pws_handler().handle_write_replace_warning(req);
  lazy_task_launcher<std::vector<du_cell_index_t>> launcher{t};

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().empty());
  ASSERT_FALSE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());
}
