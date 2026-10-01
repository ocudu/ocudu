// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcp_test_vectors.h"
#include "pdcp_tx_test_helpers.h"
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
class pdcp_tx_suspend_test : public pdcp_tx_test_helper_default_crypto,
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
TEST_P(pdcp_tx_suspend_test, when_suspend_called_state_is_reset)
{
  init(GetParam(), pdcp_rb_type::drb);
  unsigned exp_nof_compressors = header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), exp_nof_compressors);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);

  // Set state of PDCP entity.
  pdcp_tx_state st = {0, 0, 0, 0, 0};
  pdcp_tx->set_state(st);
  pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::off);

  // Write 5 SDUs.
  int n_pdus = 5;
  for (int i = 0; i < n_pdus; i++) {
    byte_buffer sdu = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(sdu));
  }
  wait_pending_crypto();
  worker.run_pending_tasks();

  FLUSH_AND_ASSERT_EQ(5, pdcp_tx->nof_pdus_in_window());
  pdcp_tx->begin_buffering();
  pdcp_tx->suspend();
  FLUSH_AND_ASSERT_EQ(0, pdcp_tx->nof_pdus_in_window());

  // Check the state is reset.
  pdcp_tx_state exp_st{0, 0, 0, 0, 0};
  assert_pdcp_state_with_reordering(pdcp_tx->get_state(), exp_st);

  // Write 5 SDUs again. These should be buffered.
  // Check correct number of PDUs in the window and only one resume request.
  for (int i = 0; i < n_pdus; i++) {
    byte_buffer sdu = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(sdu));
  }
  FLUSH_AND_ASSERT_EQ(0, pdcp_tx->nof_pdus_in_window());
  FLUSH_AND_ASSERT_EQ(1, test_frame.nof_resume_required);

  // Resume PDCP entity. Buffered SDUs should be flushed.
  pdcp_tx->resume();
  pdcp_tx->end_buffering();

  FLUSH_AND_ASSERT_EQ(5, pdcp_tx->nof_pdus_in_window());
  FLUSH_AND_ASSERT_EQ(1, test_frame.nof_resume_required);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), exp_nof_compressors);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);

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
    pdcp_tx_test_all_sn_sizes,
    pdcp_tx_suspend_test,
    ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                       ::testing::Values(1),
                       ::testing::Values(cfg_rohc_disabled, cfg_rohc_uncompressed, cfg_rohc_compressed)),
    test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
