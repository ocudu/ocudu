// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcp_rx_test_helpers.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>

using namespace ocudu;

static ocudu::log_sink_spy& test_spy = []() -> ocudu::log_sink_spy& {
  if (!ocudulog::install_custom_sink(ocudu::log_sink_spy::name(),
                                     std::make_unique<ocudu::log_sink_spy>(ocudulog::get_default_log_formatter()))) {
    report_fatal_error("Unable to create logger spy");
  }
  auto* spy = static_cast<ocudu::log_sink_spy*>(ocudulog::find_sink(ocudu::log_sink_spy::name()));
  if (spy == nullptr) {
    report_fatal_error("Unable to create logger spy");
  }

  ocudulog::fetch_basic_logger("ROHC", *spy, true);
  return *spy;
}();

/// Fixture class for PDCP tests.
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size.
class pdcp_rx_suspend_test : public pdcp_rx_test_helper_default_crypto,
                             public ::testing::Test,
                             public ::testing::WithParamInterface<std::tuple<pdcp_sn_size, unsigned, rohc_test_params>>
{
protected:
  void SetUp() override
  {
    // Init test's logger.
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // Init RLC logger.
    ocudulog::fetch_basic_logger("PDCP", false).set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("PDCP", false).set_hex_dump_max_size(100);

    // Reset log spy.
    test_spy.reset_counters();
  }

  void TearDown() override
  {
    // Flush logger after each test.
    ocudulog::flush();
  }
};

/// Test DRB suspend.
TEST_P(pdcp_rx_suspend_test, when_suspend_called_state_is_reset)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  set_rb_type(pdcp_rb_type::drb);
  init();

  // Set state of PDCP entity.
  pdcp_rx_state st = {0, 0, 0};
  pdcp_rx->set_state(st);
  pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  // Write one PDUs out-of-order, so that there is one PDU on the window.
  {
    uint32_t    count = 0;
    byte_buffer test_pdu1;
    get_test_pdu(count + 1, test_pdu1);
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());
  }

  wait_pending_crypto();
  worker.run_pending_tasks();

  FLUSH_AND_ASSERT_EQ(1, pdcp_rx->nof_pdus_in_window());
  pdcp_rx->begin_buffering();
  pdcp_rx->suspend();

  // The window should be clear and out-of-order PDUs delivered.
  FLUSH_AND_ASSERT_EQ(0, pdcp_rx->nof_pdus_in_window());
  FLUSH_AND_ASSERT_EQ(1, test_frame->sdu_counter);

  // Check the state is reset.
  pdcp_rx_state exp_st{0, 0, 0};
  assert_pdcp_state(pdcp_rx->get_state(), exp_st);

  // Write 2 PDUs. These should be buffered.
  // Check correct number of PDUs in the window and only one resume request.
  {
    uint32_t    count = 0;
    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());
  }

  FLUSH_AND_ASSERT_EQ(0, pdcp_rx->nof_pdus_in_window());
  FLUSH_AND_ASSERT_EQ(1, test_frame->nof_resume_required);

  // Resume PDCP entity. Buffered SDUs should be flushed.
  pdcp_rx->resume();
  pdcp_rx->end_buffering();

  wait_pending_crypto();
  worker.run_pending_tasks();

  // The window should be clear and in-of-order PDUs delivered.
  // Resume required should only be requested once.
  FLUSH_AND_ASSERT_EQ(0, pdcp_rx->nof_pdus_in_window());
  FLUSH_AND_ASSERT_EQ(3, test_frame->sdu_counter);
  FLUSH_AND_ASSERT_EQ(1, test_frame->nof_resume_required);

  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
static std::string
test_param_info_to_string(const ::testing::TestParamInfo<std::tuple<pdcp_sn_size, unsigned, rohc_test_params>>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "{}bit_{}",
                 pdcp_sn_size_to_uint(std::get<pdcp_sn_size>(info.param)),
                 std::get<rohc_test_params>(info.param).name);
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(
    pdcp_rx_test_all_variants,
    pdcp_rx_suspend_test,
    ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                       ::testing::Values(1),
                       ::testing::Values(cfg_rohc_disabled, cfg_rohc_uncompressed, cfg_rohc_compressed)),
    test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
