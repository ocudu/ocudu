// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcp_tx_reestablish_test.h"
#include "pdcp_test_vectors.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace ocudu;

/// Test SRB reestablishment
TEST_P(pdcp_tx_reestablish_test_srb, when_srb_reestablish_then_pdus_dropped)
{
  init(GetParam(), pdcp_rb_type::srb);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);

  // Set state of PDCP entiy
  pdcp_tx_state st = {0, 0, 0, 0, 0};
  pdcp_tx->set_state(st);
  pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::off);

  // Write 5 SDU
  int n_pdus = 5;
  for (int i = 0; i < n_pdus; i++) {
    byte_buffer sdu = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(sdu));
  }
  wait_pending_crypto();
  worker.run_pending_tasks();

  FLUSH_AND_ASSERT_EQ(5, pdcp_tx->nof_pdus_in_window());
  pdcp_tx->reestablish({});
  FLUSH_AND_ASSERT_EQ(0, pdcp_tx->nof_pdus_in_window());

  // Check the state is reset
  pdcp_tx_state exp_st{0, 0, 0, 0, 0};
  assert_pdcp_state_with_reordering(pdcp_tx->get_state(), exp_st);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);
}

/// Test DRB UM reestablishment
TEST_P(pdcp_tx_reestablish_test, when_drb_um_reestablish_then_pdus_and_discard_timers_preserved)
{
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::um);
  unsigned exp_nof_compressors = header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), exp_nof_compressors);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);

  // Set state of PDCP entiy
  pdcp_tx_state st = {0, 0, 0, 0, 0};
  pdcp_tx->set_state(st);
  pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::off);

  // Write 5 SDU
  int n_pdus = 5;
  for (int i = 0; i < n_pdus; i++) {
    byte_buffer sdu = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(sdu));
  }
  wait_pending_crypto();
  worker.run_pending_tasks();
  FLUSH_AND_ASSERT_EQ(5, pdcp_tx->nof_pdus_in_window());

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), exp_nof_compressors);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);

  pdcp_tx->reestablish(sec_cfg);
  FLUSH_AND_ASSERT_EQ(0, pdcp_tx->nof_pdus_in_window());

  // Check the state is reset
  pdcp_tx_state exp_st{0, 0, 0, 0, 0};
  assert_pdcp_state_with_reordering(pdcp_tx->get_state(), exp_st);

  exp_nof_compressors = header_compression.has_value() ? 2 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), exp_nof_compressors);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);
}

/// Test DRB AM reestablishment
TEST_P(pdcp_tx_reestablish_test, when_drb_am_reestablish_then_pdus_retx)
{
  init(GetParam(), pdcp_rb_type::drb, pdcp_rlc_mode::am);
  unsigned exp_nof_compressors = header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), exp_nof_compressors);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);

  // Set state of PDCP entiy
  pdcp_tx_state st = {0, 0, 0, 0, 0};
  pdcp_tx->set_state(st);
  pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::off);

  // Write 5 SDU
  int n_pdus = 5;
  for (int i = 0; i < n_pdus; i++) {
    byte_buffer sdu = byte_buffer::create(sdu1).value();
    pdcp_tx->handle_sdu(std::move(sdu));
  }
  wait_pending_crypto();
  worker.run_pending_tasks();
  tick_all(5);
  FLUSH_AND_ASSERT_EQ(5, pdcp_tx->nof_pdus_in_window());

  // Confirm transmission of all PDUs (up to and including SN=4)
  pdcp_tx->handle_transmit_notification(4);

  {
    // Check the expected state: tx_trans awaits SN=5; tx_next_ack still awaits delivery of SN=0
    pdcp_tx_state exp_st = {5, 5, 0, 5, 0};
    assert_pdcp_state_with_reordering(pdcp_tx->get_state(), exp_st);
  }

  // Check that non-ACKed SDUs are retransmitted.
  FLUSH_AND_ASSERT_EQ(5, test_frame.pdu_queue.size());
  pdcp_tx->handle_delivery_notification(1); // ACK SN=0 and 1.

  {
    // Check the expected state: tx_trans awaits SN=5; tx_next_ack is advanced to SN=2
    pdcp_tx_state exp_st = {5, 5, 0, 5, 2};
    assert_pdcp_state_with_reordering(pdcp_tx->get_state(), exp_st);
  }

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), exp_nof_compressors);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);

  pdcp_tx->reestablish(sec_cfg);
  wait_pending_crypto();
  worker.run_pending_tasks();

  FLUSH_AND_ASSERT_EQ(5, test_frame.pdu_queue.size());  // unchanged
  FLUSH_AND_ASSERT_EQ(3, test_frame.retx_queue.size()); // SN=2, 3 and 4 RETXed

  // Check if discard timer was not reset.
  FLUSH_AND_ASSERT_EQ(3, pdcp_tx->nof_pdus_in_window());

  {
    // Check the expected state: not reset, but tx_trans shall be rewinded to tx_next_ack
    pdcp_tx_state exp_st = {5, 5, 0, 2, 2};
    assert_pdcp_state_with_reordering(pdcp_tx->get_state(), exp_st);
  }

  tick_all(5);
  FLUSH_AND_ASSERT_EQ(0, pdcp_tx->nof_pdus_in_window());

  {
    // Check the expected state: everything advanced to 5
    pdcp_tx_state exp_st = {5, 5, 0, 5, 5};
    assert_pdcp_state_with_reordering(pdcp_tx->get_state(), exp_st);
  }

  exp_nof_compressors = header_compression.has_value() ? 2 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), exp_nof_compressors);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);
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
                         pdcp_tx_reestablish_test_srb,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits),
                                            ::testing::Values(1),
                                            ::testing::Values(cfg_rohc_disabled)),
                         test_param_info_to_string);

INSTANTIATE_TEST_SUITE_P(
    pdcp_tx_test_all_sn_sizes,
    pdcp_tx_reestablish_test,
    ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                       ::testing::Values(1),
                       ::testing::Values(cfg_rohc_disabled, cfg_rohc_uncompressed, cfg_rohc_compressed)),
    test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
