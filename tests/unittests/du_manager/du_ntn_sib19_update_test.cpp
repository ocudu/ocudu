// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "du_manager_test_helpers.h"
#include "lib/du/du_high/du_manager/du_high_ntn_sib19_update_handler_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/bcch_dl_sch_msg.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/du/du_high/du_manager/du_manager_factory.h"
#include "ocudu/ntn/ntn_sib19_update_handler.h"
#include "ocudu/ran/nr_cgi.h"
#include "ocudu/ran/ntn.h"
#include "ocudu/ran/sib/system_info_config.h"
#include "ocudu/support/async/manual_event.h"
#include "ocudu/support/executors/task_worker.h"
#include <atomic>
#include <chrono>
#include <gtest/gtest.h>
#include <thread>

using namespace ocudu;
using namespace ocudu_ntn;
using namespace odu;

namespace {

// Build a minimal valid ntn_sib19_update_request that the handler can pack, targeting the given cell. The handler
// dereferences ntn_cfg->ephemeris_info.value(), so both ntn_cfg and ephemeris_info must be set.
ntn_sib19_update_request make_ntn_request(const nr_cell_global_id_t& nr_cgi)
{
  ntn_sib19_update_request req;
  req.nr_cgi         = nr_cgi;
  req.si_msg_idx     = 0;
  req.sib_idx        = 19;
  req.slot           = slot_point{subcarrier_spacing::kHz15, 0, 0};
  req.si_slot_period = 320;
  // Keep si_valuetag_change=false so the update takes the plain SI-PDU path and does not depend on a cell SIB1
  // reconfiguration succeeding; the SI message buffers (the subject of the use-after-free) are built either way.
  req.si_valuetag_change = false;

  sib19_info& sib19 = req.sib19;
  sib19.ntn_cfg.emplace();
  sib19.ntn_cfg->cell_specific_koffset.emplace(std::chrono::milliseconds(260));
  sib19.ntn_cfg->k_mac     = std::chrono::milliseconds{128};
  sib19.ntn_cfg->ta_report = true;
  sib19.ntn_cfg->epoch_time.emplace();
  sib19.ntn_cfg->epoch_time->sfn             = 0;
  sib19.ntn_cfg->epoch_time->subframe_number = 0;

  ecef_coordinates_t ecef{};
  ecef.position_x  = 1300.0;
  ecef.position_y  = 2600.0;
  ecef.position_z  = 3900.0;
  ecef.velocity_vx = 0.26;
  ecef.velocity_vy = 0.52;
  ecef.velocity_vz = 0.78;
  sib19.ntn_cfg->ephemeris_info.emplace(ecef);

  return req;
}

// Decode the captured PDU as a BCCH-DL-SCH SI message and assert it carries a SIB19. This proves the MAC received the
// intact packed SIB19 rather than merely a non-empty buffer (a truncated or wrong-SIB PDU would still be non-empty).
// It decodes independently rather than re-running the packer, so producer and oracle do not share the same bug.
void expect_decodes_as_sib19(const byte_buffer& packed)
{
  ASSERT_FALSE(packed.empty());
  asn1::cbit_ref                  bref{packed};
  asn1::rrc_nr::bcch_dl_sch_msg_s msg;
  ASSERT_EQ(msg.unpack(bref), asn1::OCUDUASN_SUCCESS);
  ASSERT_EQ(msg.msg.type().value, asn1::rrc_nr::bcch_dl_sch_msg_type_c::types::c1);
  ASSERT_EQ(msg.msg.c1().type().value, asn1::rrc_nr::bcch_dl_sch_msg_type_c::c1_c_::types::sys_info);
  const asn1::rrc_nr::sys_info_ies_s& si = msg.msg.c1().sys_info().crit_exts.sys_info();
  ASSERT_NE(si.sib_type_and_info.size(), 0U);
  EXPECT_EQ(si.sib_type_and_info[0].type().value, asn1::rrc_nr::sys_info_ies_s::item_c_::types::sib19_v1700);
}

// Fixture: a real, started DU manager driven through the real SIB19 update handler. Mirrors the start/stop harness used
// by du_param_config_procedure_test: the DU manager runs on a manual task worker pumped from the test, while the
// blocking controller start()/stop() are driven so the setup/teardown handshakes complete.
class du_ntn_sib19_update_test : public ::testing::Test
{
protected:
  du_ntn_sib19_update_test()
  {
    // Auto-complete the F1 setup and MAC cell start/stop handshakes so start()/stop() return.
    dependencies.f1ap.wait_f1_setup.result.value().cells_to_activate.resize(1);
    dependencies.f1ap.wait_f1_setup.result.value().cells_to_activate[0].cgi = cell_cfgs[0].nr_cgi;
    dependencies.f1ap.wait_f1_setup.ready_ev.set();
    dependencies.f1ap.wait_f1_removal.ready_ev.set();
    dependencies.mac.mac_cell.wait_start.ready_ev.set();
    dependencies.mac.mac_cell.wait_stop.ready_ev.set();

    du_mng->get_controller().start();
  }

  ~du_ntn_sib19_update_test() override
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
  std::vector<du_cell_config> cell_cfgs{config_helpers::make_default_du_cell_config()};
  du_manager_test_bench       dependencies{cell_cfgs};
  std::unique_ptr<du_manager> du_mng{create_du_manager(dependencies.params)};
};

} // namespace

// Regression test for the use-after-free in the NTN SIB19 update path (issue #643).
//
// #643 had two independent lifetime failures on the same path, each a use-after-free on its own once
// handle_sib19_msg_update() returns and its stack unwinds:
//   (1) du_manager_impl scheduled a coroutine that captured the caller's request by reference, and
//   (2) the request carried a non-owning span<byte_buffer> into the handler's stack-local message vector.
//
// The DU manager's fifo task scheduler resumes its loop inline while idle, so simply calling the handler would run the
// whole update synchronously inside handle_sib19_msg_update() with the request still alive, hiding both bugs. To force
// the update to run only after the handler returns, the test first occupies main_ctrl_loop with a task blocked on an
// event; the NTN update then queues behind it. Releasing the blocker after the handler returns runs the deferred
// update against the now-destroyed caller stack, so under ASan this aborts on either pre-fix defect and passes once the
// request owns the SI messages end to end. (Verified with mutation testing: reverting only the manager capture to
// [&req], or only the owning vector back to a span, each makes this test abort.)
TEST_F(du_ntn_sib19_update_test, si_messages_survive_deferred_update_and_reach_mac_intact)
{
  du_high_ntn_sib19_update_handler_impl handler{du_mng->get_operation_configurator()};
  const ntn_sib19_update_request        req = make_ntn_request(cell_cfgs[0].nr_cgi);

  // Occupy the DU manager task loop (main_ctrl_loop, the same loop handle_ntn_param_update schedules onto) with a task
  // that blocks on an event. The NTN update scheduled by the handler cannot start until the blocker is released, i.e.
  // not until after the handler has returned and its stack has unwound.
  manual_event_flag scheduler_blocker;
  du_mng->get_f1ap_event_handler().schedule_async_task(
      launch_async([&scheduler_blocker](coro_context<async_task<void>>& ctx) {
        CORO_BEGIN(ctx);
        CORO_AWAIT(scheduler_blocker);
        CORO_RETURN();
      }));

  handler.handle_sib19_msg_update(req);

  // The handler has returned: the SI message vector and the DU request on its stack are gone. Only now release the
  // blocker so the deferred NTN update runs. On the pre-fix code the manager coroutine dereferences the freed request
  // and/or the dummy MAC reads the freed SI buffers (heap-use-after-free under ASan).
  scheduler_blocker.set();
  for (unsigned i = 0; i != 32 and dependencies.mac.mac_cell.last_si_msg_bytes.empty(); ++i) {
    dependencies.worker.run_pending_tasks();
  }

  // The MAC received the SI PDU update and the captured bytes decode as the SIB19 that was sent.
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req->new_si_pdu_info.has_value());
  EXPECT_EQ(dependencies.mac.mac_cell.last_cell_recfg_req->new_si_pdu_info->si_msg_idx, req.si_msg_idx);
  ASSERT_EQ(dependencies.mac.mac_cell.last_si_msg_bytes.size(), 1U);
  expect_decodes_as_sib19(dependencies.mac.mac_cell.last_si_msg_bytes[0]);
}

int main(int argc, char** argv)
{
  ocudulog::init();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
