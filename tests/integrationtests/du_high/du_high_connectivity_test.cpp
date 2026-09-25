// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "tests/integrationtests/du_high/test_utils/du_high_env_simulator.h"
#include "tests/ocudu_test_requirements.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/mac/mac_test_messages.h"
#include "ocudu/asn1/f1ap/common.h"
#include "ocudu/f1ap/f1ap_message.h"
#include <gtest/gtest.h>
#include <thread>

using namespace ocudu;
using namespace odu;
using namespace asn1::f1ap;

class du_high_connectivity_test_base : public du_high_env_simulator
{
protected:
  explicit du_high_connectivity_test_base(bool retry_f1c_connection = false) :
    du_high_env_simulator(
        du_high_env_sim_params{.nof_cells = 1, .auto_start = false, .retry_f1c_connection = retry_f1c_connection})
  {
    OCUDU_TEST_REQUIREMENTS("MVP-ARCH-INTF-6");
  }

  void run_f1_setup()
  {
    // Start DU and try to connect to CU.
    this->du_hi->start();

    // Ensure the result is saved in the notifier.
    run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });

    // Clearing the F1AP messages sent during setup for further tests.
    cu_notifier.f1ap_ul_msgs.clear();
  }

  /// \brief Processes pending test thread tasks until \c cond is met or the timeout elapses.
  ///
  /// Unlike \c run_until, it does not dispatch slot indications, so it can be used while the DU has no cell
  /// configured, i.e. while it is still trying to set the F1 interface up.
  bool poll_until(const std::function<bool()>& cond, std::chrono::milliseconds timeout = std::chrono::seconds{5})
  {
    static constexpr std::chrono::milliseconds poll_period{10};
    for (std::chrono::milliseconds elapsed{0}; elapsed < timeout; elapsed += poll_period) {
      workers.test_worker.run_pending_tasks();
      if (cond()) {
        return true;
      }
      std::this_thread::sleep_for(poll_period);
    }
    workers.test_worker.run_pending_tasks();
    return cond();
  }
};

class du_high_connectivity_test : public du_high_connectivity_test_base, public testing::Test
{};

/// Test suite of a DU that retries the F1-C TNL connection instead of closing the application when it fails.
class du_high_f1c_retry_test : public du_high_connectivity_test_base, public testing::Test
{
protected:
  du_high_f1c_retry_test() : du_high_connectivity_test_base(true) {}
};

TEST_F(du_high_connectivity_test, when_du_does_not_start_then_no_f1_setup_is_sent)
{
  // DU-high is not started, so no F1 setup should be sent.
  ASSERT_TRUE(this->cu_notifier.f1ap_ul_msgs.empty());
}

TEST_F(du_high_connectivity_test, when_du_starts_it_then_initiates_f1_setup)
{
  // Start DU and try to connect to CU.
  this->du_hi->start();

  // Ensure the F1 Setup Request is saved in the CU notifier.
  ASSERT_TRUE(run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));

  // Starting the DU-high initiates the F1 Setup procedure.
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.size(), 1);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.type().value, f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().proc_code, ASN1_F1AP_ID_F1_SETUP);
}

TEST_F(du_high_f1c_retry_test, when_cu_cp_is_not_reachable_on_start_then_du_retries_the_tnl_association)
{
  // CU-CP is not reachable yet.
  cu_notifier.set_f1_channel_state(false);

  // The start does not wait for the F1 Setup to complete, as the CU-CP may never show up.
  du_hi->start();

  // No F1 Setup Request can be sent while the F1-C TNL association is down.
  EXPECT_FALSE(poll_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }, std::chrono::milliseconds{100}));

  // The CU-CP becomes reachable and the DU completes its setup.
  cu_notifier.set_f1_channel_state(true);

  ASSERT_TRUE(poll_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }));
  ASSERT_EQ(cu_notifier.f1ap_ul_msgs.size(), 1);
  ASSERT_EQ(cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.type().value, f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().proc_code, ASN1_F1AP_ID_F1_SETUP);
}

TEST_F(du_high_f1c_retry_test, when_cu_cp_is_not_reachable_then_du_can_still_be_stopped)
{
  // CU-CP is not reachable and never will be.
  cu_notifier.set_f1_channel_state(false);
  du_hi->start();

  // The stop cancels the pending setup instead of waiting for a CU-CP that is not coming.
  du_hi->stop();

  ASSERT_TRUE(cu_notifier.f1ap_ul_msgs.empty());
}

TEST_F(du_high_connectivity_test, when_f1_connection_is_lost_then_du_detects_connection_loss)
{
  run_f1_setup();
  ASSERT_FALSE(cu_notifier.du_released_connection());

  // Trigger a TNL disconnection (and the channel will stay down).
  cu_notifier.set_f1_channel_state(false);

  // DU releases its F1 connection, it will attempt to establish a new one and fail.
  cu_notifier.f1ap_ul_msgs.clear();
  ASSERT_FALSE(cu_notifier.du_released_connection());
  workers.flush_pending_control_tasks();
  ASSERT_TRUE(cu_notifier.du_released_connection());

  // Ensure no F1 setup request is sent (because TNL association failed).
  ASSERT_FALSE(run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); }, 200));
}

TEST_F(du_high_connectivity_test, when_f1_connection_is_lost_and_reestablished_then_du_completes_f1_setup)
{
  run_f1_setup();

  // Signal a temporary F1 connection loss. The DU should retry the connection.
  cu_notifier.set_f1_channel_state(false);
  cu_notifier.set_f1_channel_state(true);

  ASSERT_TRUE(cu_notifier.f1ap_ul_msgs.empty());

  // DU stops and restarts the F1 setup procedure after detecting that the connection is back up.
  run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.size(), 1);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.type().value, f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->cu_notifier.f1ap_ul_msgs.rbegin()->second.pdu.init_msg().proc_code, ASN1_F1AP_ID_F1_SETUP);
  test_logger.info("STATUS: DU successfully retried F1 Setup after connection loss.");
}

TEST_F(du_high_connectivity_test, when_f1_connection_is_lost_then_ues_are_removed)
{
  // Run F1 Setup
  run_f1_setup();

  // Add UE
  du_hi->get_pdu_handler().handle_rx_data_indication(
      test_helpers::create_ccch_message(next_slot.without_hyper_sfn(), to_rnti(0x4601)));
  this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
  ASSERT_EQ(cu_notifier.f1ap_ul_msgs.size(), 1);
  ASSERT_TRUE(
      test_helpers::is_init_ul_rrc_msg_transfer_valid(cu_notifier.f1ap_ul_msgs.rbegin()->second, to_rnti(0x4601)));
  cu_notifier.f1ap_ul_msgs.clear();

  // Signal a temporary F1 connection loss. The DU should retry the connection.
  cu_notifier.set_f1_channel_state(false);
  cu_notifier.set_f1_channel_state(true);
  run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
  cu_notifier.f1ap_ul_msgs.clear();
  test_logger.info("STATUS: DU successfully retried F1 Setup after connection loss.");

  // Add new UE
  du_hi->get_pdu_handler().handle_rx_data_indication(
      test_helpers::create_ccch_message(next_slot.without_hyper_sfn(), to_rnti(0x4602)));
  this->run_until([this]() { return not cu_notifier.f1ap_ul_msgs.empty(); });
  ASSERT_EQ(cu_notifier.f1ap_ul_msgs.size(), 1);
  ASSERT_TRUE(
      test_helpers::is_init_ul_rrc_msg_transfer_valid(cu_notifier.f1ap_ul_msgs.rbegin()->second, to_rnti(0x4602)));
}
