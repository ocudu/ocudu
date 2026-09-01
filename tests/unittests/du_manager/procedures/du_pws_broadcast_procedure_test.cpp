// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "../du_manager_test_helpers.h"
#include "ocudu/adt/format.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include "ocudu/du/du_cell_config_helpers.h"
#include "ocudu/du/du_high/du_manager/du_manager_factory.h"
#include "ocudu/ran/sib/system_info_config.h"
#include "ocudu/support/async/async_test_utils.h"
#include "ocudu/support/error_handling.h"
#include "ocudu/support/executors/task_worker.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

namespace {

/// A cell provisioned for an ETWS primary notification, with no content configured for it -- i.e. it stays dormant
/// until a Write-Replace Warning provides one.
du_cell_config make_cell_config_with_dormant_pws_si_message()
{
  du_cell_config cfg = config_helpers::make_default_du_cell_config();

  cfg.si.si_config.emplace();
  cfg.si.si_config->si_window_len_slots = 10;
  cfg.si.si_config->pws_si_messages.push_back(pws_si_message_config{sib_type::sib6, 32, false});

  return cfg;
}

/// Builds a minimal, validly-packed SIB6 PDU, as if it had come from the CU over F1AP.
byte_buffer pack_valid_sib6_pdu()
{
  asn1::rrc_nr::sib6_s sib6;
  sib6.msg_id.from_number(0x1112);
  sib6.serial_num.from_number(0x3210);
  sib6.warning_type.from_number(0x8000);

  byte_buffer   buf;
  asn1::bit_ref bref{buf};
  report_fatal_error_if_not(sib6.pack(bref) == asn1::OCUDUASN_SUCCESS, "Failed to pack test SIB6");
  return buf;
}

class du_pws_broadcast_procedure_test : public ::testing::Test
{
protected:
  explicit du_pws_broadcast_procedure_test(du_cell_config cell_cfg = config_helpers::make_default_du_cell_config()) :
    cell_cfgs({std::move(cell_cfg)}), dependencies(cell_cfgs), du_mng(create_du_manager(dependencies.params))
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

/// Fixture whose cell is provisioned for an ETWS primary notification, with no content configured for it.
class du_pws_broadcast_procedure_provisioned_test : public du_pws_broadcast_procedure_test
{
protected:
  du_pws_broadcast_procedure_provisioned_test() :
    du_pws_broadcast_procedure_test(make_cell_config_with_dormant_pws_si_message())
  {
  }
};

} // namespace

TEST_F(du_pws_broadcast_procedure_test, when_cell_not_provisioned_for_sib_type_then_it_is_not_accepted)
{
  write_replace_warning_information req;
  req.sib_type = 6;
  req.sib_msgs.push_back(byte_buffer::create({0x1, 0x2, 0x3}).value());
  req.repeat_period            = std::chrono::seconds{60};
  req.nof_broadcasts_requested = 4;
  req.cells                    = {du_cell_index_t::MIN_DU_CELL_INDEX};

  // The default cell config is not provisioned for any warning (no etws_cfg/cmas_cfg configured).
  async_task<std::vector<du_cell_index_t>> t =
      du_mng->get_f1ap_event_handler().get_pws_handler().handle_write_replace_warning(req);
  lazy_task_launcher<std::vector<du_cell_index_t>> launcher{t};

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().empty());
  ASSERT_FALSE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());
}

/// Regression test: a CU-CP peer that (incorrectly) forwards a Write-Replace Warning with no actual SIB content must
/// not be accepted, and must not reach the MAC.
TEST_F(du_pws_broadcast_procedure_provisioned_test, when_sib_content_is_empty_then_broadcast_is_not_accepted)
{
  write_replace_warning_information req;
  req.sib_type = 6;
  req.sib_msgs.push_back(byte_buffer{});
  req.repeat_period            = std::chrono::seconds{60};
  req.nof_broadcasts_requested = 4;
  req.cells                    = {du_cell_index_t::MIN_DU_CELL_INDEX};

  async_task<std::vector<du_cell_index_t>> t =
      du_mng->get_f1ap_event_handler().get_pws_handler().handle_write_replace_warning(req);
  lazy_task_launcher<std::vector<du_cell_index_t>> launcher{t};

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().empty()) << "An empty SIB6 payload must fail to unpack and must not be accepted";
  ASSERT_FALSE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value())
      << "MAC must not be reconfigured when the SIB content fails to unpack";
}

/// Same as above, but with non-empty, garbage bytes instead of an empty buffer (e.g. a truncated/corrupted segment).
TEST_F(du_pws_broadcast_procedure_provisioned_test, when_sib_content_is_malformed_then_broadcast_is_not_accepted)
{
  write_replace_warning_information req;
  req.sib_type = 6;
  req.sib_msgs.push_back(byte_buffer::create({0x1, 0x2, 0x3}).value());
  req.repeat_period            = std::chrono::seconds{60};
  req.nof_broadcasts_requested = 4;
  req.cells                    = {du_cell_index_t::MIN_DU_CELL_INDEX};

  async_task<std::vector<du_cell_index_t>> t =
      du_mng->get_f1ap_event_handler().get_pws_handler().handle_write_replace_warning(req);
  lazy_task_launcher<std::vector<du_cell_index_t>> launcher{t};

  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().empty()) << "A malformed SIB6 payload must fail to unpack and must not be accepted";
  ASSERT_FALSE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value())
      << "MAC must not be reconfigured when the SIB content fails to unpack";
}

/// Positive control for the two tests above: the very same request is accepted once its SIB6 content is valid, which
/// is what makes them exercise the content validation rather than the cell provisioning check.
TEST_F(du_pws_broadcast_procedure_provisioned_test, when_sib_content_is_valid_then_broadcast_is_accepted)
{
  write_replace_warning_information req;
  req.sib_type = 6;
  req.sib_msgs.push_back(pack_valid_sib6_pdu());
  req.repeat_period            = std::chrono::seconds{60};
  req.nof_broadcasts_requested = 4;
  req.cells                    = {du_cell_index_t::MIN_DU_CELL_INDEX};

  async_task<std::vector<du_cell_index_t>> t =
      du_mng->get_f1ap_event_handler().get_pws_handler().handle_write_replace_warning(req);
  lazy_task_launcher<std::vector<du_cell_index_t>> launcher{t};

  ASSERT_TRUE(t.ready());
  ASSERT_EQ(t.get(), std::vector<du_cell_index_t>{du_cell_index_t::MIN_DU_CELL_INDEX});
  ASSERT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req.has_value());
  EXPECT_TRUE(dependencies.mac.mac_cell.last_cell_recfg_req->new_si_pdu_info.has_value());
}
