// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcp_rx_reestablish_test.h"
#include "ocudu/pdcp/pdcp_config.h"
#include <gtest/gtest.h>
#include <queue>

using namespace ocudu;

/// Test SRB reestablishment
TEST_P(pdcp_rx_reestablish_test_srb, when_srb_reestablish_then_sdus_dropped)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  set_rb_type(pdcp_rb_type::srb);
  init();

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);
  uint32_t count = 0;

  security::sec_128_as_config reest_sec_cfg = sec_cfg;
  reest_sec_cfg.integ_algo                  = security::integrity_algorithm::nia3;
  reest_sec_cfg.cipher_algo                 = security::ciphering_algorithm::nea3;
  pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  // Prepare 3 PDUs.
  byte_buffer test_pdu1;
  get_test_pdu(count, test_pdu1);
  byte_buffer test_pdu2;
  get_test_pdu(count + 1, test_pdu2);
  byte_buffer test_pdu3;
  get_test_pdu(count + 2, test_pdu3);

  // RX PDU 2 and 3, none should arrive.
  pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
  pdcp_rx->set_state(init_state);
  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());

  // Wait for crypto and reordering
  wait_pending_crypto();
  worker.run_pending_tasks();
  ASSERT_EQ(0, test_frame->sdu_queue.size());

  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu3)).value());

  // Wait for crypto and reordering
  wait_pending_crypto();
  worker.run_pending_tasks();
  ASSERT_EQ(0, test_frame->sdu_queue.size());

  // Check PDCP state.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 3);
  }

  // Re-establish entity.
  pdcp_rx->reestablish(reest_sec_cfg);

  // Check no PDU was delivered
  // and that state was reset.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 0);
  }
  ASSERT_EQ(0, test_frame->sdu_queue.size());

  // Check security config changed by passing test PDUs from NxA3 testset
  byte_buffer test_pdu_nxa3;
  get_test_pdu(count, test_pdu_nxa3, 3);
  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu_nxa3)).value());

  // Wait for crypto and reordering
  wait_pending_crypto();
  worker.run_pending_tasks();
  ASSERT_EQ(1, test_frame->sdu_queue.size());

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), 0);
}

/// Test DRB UM reestablishment
TEST_P(pdcp_rx_reestablish_test, when_drb_um_reestablish_then_pdus_forwared)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  set_rb_type(pdcp_rb_type::drb);
  set_rlc_mode(pdcp_rlc_mode::um);
  init();

  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
  uint32_t count = 0;

  pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  // Prepare 3 PDUs.
  byte_buffer test_pdu1;
  get_test_pdu(count, test_pdu1);
  byte_buffer test_pdu2;
  get_test_pdu(count + 1, test_pdu2);
  byte_buffer test_pdu3;
  get_test_pdu(count + 2, test_pdu3);

  // RX PDU 2 and 3, none should arrive.
  pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
  pdcp_rx->set_state(init_state);
  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());

  // Wait for crypto and reordering
  wait_pending_crypto();
  worker.run_pending_tasks();
  ASSERT_EQ(0, test_frame->sdu_queue.size());
  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu3)).value());

  // Wait for crypto and reordering
  wait_pending_crypto();
  worker.run_pending_tasks();
  ASSERT_EQ(0, test_frame->sdu_queue.size());

  // Check PDCP state.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 3);
    ASSERT_EQ(true, pdcp_rx->is_reordering_timer_running());
  }

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  // Re-establish entity.
  pdcp_rx->reestablish(sec_cfg);

  // Check that 2 PDUs were delivered
  // and that state was reset.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 0);
  }
  ASSERT_EQ(2, test_frame->sdu_queue.size());

  // Check re-ordering timer was stopped
  ASSERT_EQ(false, pdcp_rx->is_reordering_timer_running());

  exp_nof_decompressors = config.header_compression.has_value() ? 2 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test DRB AM reestablishment
TEST_P(pdcp_rx_reestablish_test, when_drb_am_reestablish_then_state_preserved)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  set_rb_type(pdcp_rb_type::drb);
  set_rlc_mode(pdcp_rlc_mode::am);
  init();

  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  uint32_t                    count         = 0;
  security::sec_128_as_config reest_sec_cfg = sec_cfg;

  pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  // Prepare 3 PDUs.
  byte_buffer test_pdu1;
  get_test_pdu(count, test_pdu1);
  byte_buffer test_pdu2;
  get_test_pdu(count + 1, test_pdu2);
  byte_buffer test_pdu3;
  get_test_pdu(count + 2, test_pdu3);

  // RX PDU 2 and 3, none should arrive.
  pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
  pdcp_rx->set_state(init_state);
  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());

  // Wait for crypto and reordering
  wait_pending_crypto();
  worker.run_pending_tasks();
  ASSERT_EQ(0, test_frame->sdu_queue.size());
  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu3)).value());

  // Wait for crypto and reordering
  wait_pending_crypto();
  worker.run_pending_tasks();
  ASSERT_EQ(0, test_frame->sdu_queue.size());

  // Check PDCP state.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 3);
    ASSERT_EQ(true, pdcp_rx->is_reordering_timer_running());
  }

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  // Re-establish entity.
  pdcp_rx->reestablish(reest_sec_cfg);

  // Check that PDUs were *not* delivered
  // and that state was *not* reset.
  {
    pdcp_rx_state st = pdcp_rx->get_state();
    ASSERT_EQ(st.rx_next, 3);
  }
  ASSERT_EQ(0, test_frame->sdu_queue.size());

  // Check re-ordering timer is still running
  ASSERT_EQ(true, pdcp_rx->is_reordering_timer_running());

  // Deliver first PDU
  pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());

  // Wait for crypto and reordering
  wait_pending_crypto();
  worker.run_pending_tasks();
  ASSERT_EQ(3, test_frame->sdu_queue.size());

  exp_nof_decompressors = config.header_compression.has_value() ? 2 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
static std::string
test_param_info_to_string(const ::testing::TestParamInfo<std::tuple<pdcp_sn_size, unsigned, rohc_test_params>>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "{}bit_nia{}_nea{}_{}",
                 pdcp_sn_size_to_uint(std::get<pdcp_sn_size>(info.param)),
                 std::get<unsigned>(info.param),
                 std::get<unsigned>(info.param),
                 std::get<rohc_test_params>(info.param).name);
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(pdcp_rx_test_all_variants,
                         pdcp_rx_reestablish_test_srb,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1),
                                            ::testing::Values(cfg_rohc_disabled)),
                         test_param_info_to_string);

INSTANTIATE_TEST_SUITE_P(
    pdcp_rx_test_all_variants,
    pdcp_rx_reestablish_test,
    ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                       ::testing::Values(1),
                       ::testing::Values(cfg_rohc_disabled, cfg_rohc_uncompressed, cfg_rohc_compressed)),
    test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
