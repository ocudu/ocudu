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

  ocudulog::fetch_basic_logger("PDCP", *spy, true);
  return *spy;
}();

/// Fixture class for PDCP tests.
/// It requires TEST_P() and INSTANTIATE_TEST_SUITE_P() to create/spawn tests for each supported SN size.
class pdcp_tx_forward_test : public pdcp_tx_test_helper_default_crypto,
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
TEST_P(pdcp_tx_forward_test, when_forward_sdu_has_valid_sn_pdu_is_forwarded)
{
  init(GetParam(), pdcp_rb_type::drb);

  // Set state of PDCP entity.
  pdcp_tx_state st = {0, 0, 0, 0, 0};
  pdcp_tx->set_state(st);
  pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  // When forwarding is enabled, the PDCP will buffer SDUs until forwarding is finished.
  pdcp_tx->begin_buffering();

  // State the PDCP will be set to.
  pdcp_count_info fwd_count_info{.sn = 5, .hfn = 0};

  // Write 3 forwarded SDUs.
  uint32_t n_fwd_pdus = 3;
  for (uint32_t i = 0; i < n_fwd_pdus; i++) {
    byte_buffer sdu = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(sdu), i);
  }

  // Write 3 forwarded SDUs with invalid SNs.
  uint32_t n_invalid_fwd_pdus = 3;
  for (uint32_t i = fwd_count_info.sn; i < fwd_count_info.sn + n_invalid_fwd_pdus; i++) {
    byte_buffer sdu = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(sdu), i);
  }

  // Write 5 SDUs.
  uint32_t n_pdus = 5;
  for (uint32_t i = 0; i < n_pdus; i++) {
    byte_buffer sdu = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(sdu));
  }

  wait_pending_crypto();
  worker.run_pending_tasks();

  FLUSH_AND_ASSERT_EQ(0, pdcp_tx->nof_pdus_in_window());
  FLUSH_AND_ASSERT_EQ(test_frame.pdu_queue.size(), 0);

  // End the buffering. Buffered forwarded and regular SDUs should be flushed.
  pdcp_tx->set_count(fwd_count_info);
  pdcp_tx->end_buffering();

  wait_pending_crypto();
  worker.run_pending_tasks();

  // We comput the size of the TX window as TX_NEXT - TX_NEXT_ACK.
  // This is should be the COUNT set plus nof_sdus.
  FLUSH_AND_ASSERT_EQ(fwd_count_info.sn + n_pdus, pdcp_tx->nof_pdus_in_window());
  FLUSH_AND_ASSERT_EQ(test_frame.pdu_queue.size(), n_fwd_pdus + n_pdus);

  EXPECT_EQ(test_spy.get_warning_counter(), n_invalid_fwd_pdus);
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

INSTANTIATE_TEST_SUITE_P(pdcp_tx_test_all_sn_sizes,
                         pdcp_tx_forward_test,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1),
                                            ::testing::Values(cfg_rohc_disabled)),
                         test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
