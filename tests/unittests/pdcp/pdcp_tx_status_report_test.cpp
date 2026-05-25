// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcp_tx_status_report_test.h"
#include "pdcp_test_vectors.h"
#include "ocudu/support/bit_encoding.h"
#include "ocudu/support/test_utils.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <queue>

using namespace ocudu;

/// Test correct handling of PDCP status report.
TEST_P(pdcp_tx_status_report_test, handle_status_report)
{
  init(GetParam());
  auto test_with_count = [this](uint32_t tx_next) {
    // clear queue from previous runs
    test_frame.sdu_discard_queue   = {};
    unsigned                n_sdus = 5;
    std::queue<byte_buffer> exp_pdu_list;
    pdcp_tx_state           st = {tx_next, tx_next, 0, tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::off, security::ciphering_enabled::off);
    ocudu::test_delimit_logger delimiter("Testing data recovery. SN_SIZE={} COUNT={}", sn_size, tx_next);
    for (uint32_t count = tx_next; count < tx_next + n_sdus; ++count) {
      // Write SDU
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Wait for crypto and reordering
      wait_pending_crypto();
      worker.run_pending_tasks();

      pdcp_tx->handle_transmit_notification(pdcp_compute_sn(count, sn_size));

      // Get generated PDU
      FLUSH_AND_ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      exp_pdu_list.push(std::move(test_frame.pdu_queue.front()));
      test_frame.pdu_queue.pop();
      ASSERT_EQ(test_frame.pdu_queue.size(), 0);
    }

    FLUSH_AND_ASSERT_TRUE(test_frame.sdu_discard_queue.empty());

    // Build status report that confirms RX of SDUs:
    // Confirm: [tx_next,            tx_next+2,            tx_next+4]
    // Missing: [         tx_next+1,            tx_next+3,          ]
    //
    // Report:            [  FMC  ][    1          0          1     ][  0   0   0   0   0  ]
    //                    |-first-||--------- bitmap ---------------;;-- bitmap padding ---|
    byte_buffer buf = {};
    bit_encoder enc(buf);

    // Pack PDU header
    enc.pack(to_number(pdcp_dc_field::control), 1);
    enc.pack(to_number(pdcp_control_pdu_type::status_report), 3);
    enc.pack(0b0000, 4);

    // Pack tx_next+1 into FMC field
    enc.pack(tx_next + 1, 32);

    // Pack bitmap
    enc.pack(0b10100000, 8);

    // Handle this status report
    pdcp_tx->on_status_report(byte_buffer_chain::create(std::move(buf)).value());

    // Verify discard
    {
      std::array<uint32_t, 3> exp_pdcp_sns = {pdcp_compute_sn(tx_next, sn_size),
                                              pdcp_compute_sn(tx_next + 2, sn_size),
                                              pdcp_compute_sn(tx_next + 4, sn_size)};
      FLUSH_AND_ASSERT_FALSE(test_frame.sdu_discard_queue.empty());
      while (not test_frame.sdu_discard_queue.empty()) {
        ASSERT_NE(std::find(exp_pdcp_sns.begin(), exp_pdcp_sns.end(), test_frame.sdu_discard_queue.front()),
                  exp_pdcp_sns.end());
        test_frame.sdu_discard_queue.pop();
      }
    }

    // Let timers expire
    for (int i = 0; i < 10; i++) {
      timers.tick();
      worker.run_pending_tasks();
    }

    // Verify discard timers for already discarded SDU were disarmed (no duplicated discard)
    {
      std::array<uint32_t, 2> exp_pdcp_sns = {pdcp_compute_sn(tx_next + 1, sn_size),
                                              pdcp_compute_sn(tx_next + 3, sn_size)};
      FLUSH_AND_ASSERT_FALSE(test_frame.sdu_discard_queue.empty());
      while (not test_frame.sdu_discard_queue.empty()) {
        ASSERT_NE(std::find(exp_pdcp_sns.begin(), exp_pdcp_sns.end(), test_frame.sdu_discard_queue.front()),
                  exp_pdcp_sns.end());
        test_frame.sdu_discard_queue.pop();
      }
    }
  };

  if (sn_size == pdcp_sn_size::size12bits) {
    test_with_count(0);
    test_with_count(2048);
    test_with_count(4096);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_with_count(0);
    test_with_count(131072);
    test_with_count(262144);
  } else {
    FAIL();
  }
}

/// Test correct handling of PDCP status report with too large FMC.
TEST_P(pdcp_tx_status_report_test, handle_status_report_with_fmc_too_large)
{
  init(GetParam());
  auto test_with_count = [this](uint32_t tx_next) {
    // clear queue from previous runs
    test_frame.sdu_discard_queue   = {};
    unsigned                n_sdus = 5;
    std::queue<byte_buffer> exp_pdu_list;
    pdcp_tx_state           st = {tx_next, tx_next, 0, tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::off, security::ciphering_enabled::off);
    ocudu::test_delimit_logger delimiter("Testing data recovery. SN_SIZE={} COUNT={}", sn_size, tx_next);
    for (uint32_t count = tx_next; count < tx_next + n_sdus; ++count) {
      // Write SDU
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Wait for crypto and reordering
      wait_pending_crypto();
      worker.run_pending_tasks();

      pdcp_tx->handle_transmit_notification(pdcp_compute_sn(count, sn_size));

      // Get generated PDU
      FLUSH_AND_ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      exp_pdu_list.push(std::move(test_frame.pdu_queue.front()));
      test_frame.pdu_queue.pop();
      ASSERT_EQ(test_frame.pdu_queue.size(), 0);
    }

    FLUSH_AND_ASSERT_TRUE(test_frame.sdu_discard_queue.empty());

    // Build status report that confirms RX of SDUs:
    // Confirm: [tx_next, tx_next+1, tx_next+2, tx_next+3, tx_next+4]
    // Missing: [                                                   ]
    //
    // Report:                                                       [  FMC  ]
    //                                                               |-first-|
    byte_buffer buf = {};
    bit_encoder enc(buf);

    // Pack PDU header
    enc.pack(to_number(pdcp_dc_field::control), 1);
    enc.pack(to_number(pdcp_control_pdu_type::status_report), 3);
    enc.pack(0b0000, 4);

    // Pack st.tx_next+1 into FMC field
    // This means that we are getting a first missing COUNT report for a COUNT that
    // has not been transmitted yet, thus this status report will be ignored.
    enc.pack(tx_next + n_sdus + 1, 32);

    // Handle this status report
    pdcp_tx->on_status_report(byte_buffer_chain::create(std::move(buf)).value());

    // Make sure no PDU were discarded as the status report is invalid.
    FLUSH_AND_ASSERT_TRUE(test_frame.sdu_discard_queue.empty());
  };

  if (sn_size == pdcp_sn_size::size12bits) {
    test_with_count(0);
    test_with_count(2048);
    test_with_count(4096);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_with_count(0);
    test_with_count(131072);
    test_with_count(262144);
  } else {
    FAIL();
  }
}

/// Test correct handling of PDCP status report with too large bitmap.
TEST_P(pdcp_tx_status_report_test, handle_status_report_with_too_large_bitmap)
{
  init(GetParam());
  auto test_with_count = [this](uint32_t tx_next) {
    // clear queue from previous runs
    test_frame.sdu_discard_queue   = {};
    unsigned                n_sdus = 5;
    std::queue<byte_buffer> exp_pdu_list;
    pdcp_tx_state           st = {tx_next, tx_next, 0, tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::off, security::ciphering_enabled::off);
    ocudu::test_delimit_logger delimiter(
        "Testing status report with too large FMC. SN_SIZE={} COUNT={}", sn_size, tx_next);
    for (uint32_t count = tx_next; count < tx_next + n_sdus; ++count) {
      // Write SDU
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Wait for crypto and reordering
      wait_pending_crypto();
      worker.run_pending_tasks();

      pdcp_tx->handle_transmit_notification(pdcp_compute_sn(count, sn_size));

      // Get generated PDU
      FLUSH_AND_ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      exp_pdu_list.push(std::move(test_frame.pdu_queue.front()));
      test_frame.pdu_queue.pop();
      ASSERT_EQ(test_frame.pdu_queue.size(), 0);
    }

    FLUSH_AND_ASSERT_TRUE(test_frame.sdu_discard_queue.empty());

    // Build status report that confirms RX of SDUs:
    // Confirm: [tx_next,            tx_next+2,            tx_next+4]
    // Missing: [         tx_next+1,            tx_next+3,          ]
    //
    // Report:            [  FMC  ][    1          0          1     ][  0   0   0   0   0  ][ 0  0  0  0  0  0  0  0 ]
    //                    |-first-||--------- bitmap ---------------;;-- bitmap padding ---|| --   extra bitmap   -- |
    byte_buffer buf = {};
    bit_encoder enc(buf);

    // Pack PDU header
    enc.pack(to_number(pdcp_dc_field::control), 1);
    enc.pack(to_number(pdcp_control_pdu_type::status_report), 3);
    enc.pack(0b0000, 4);

    // Pack tx_next+1 into FMC field
    enc.pack(tx_next + 1, 32);

    // Pack bitmap
    enc.pack(0b10100000, 8);
    enc.pack(0b00000000, 8);

    // Handle this status report
    pdcp_tx->on_status_report(byte_buffer_chain::create(std::move(buf)).value());

    // Make sure no PDU were discarded as the status report is invalid.
    FLUSH_AND_ASSERT_TRUE(test_frame.sdu_discard_queue.empty());
  };

  if (sn_size == pdcp_sn_size::size12bits) {
    test_with_count(0);
    test_with_count(2048);
    test_with_count(4096);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_with_count(0);
    test_with_count(131072);
    test_with_count(262144);
  } else {
    FAIL();
  }
}

/// Test correct handling of PDCP status report with bitmap with bad tail.
TEST_P(pdcp_tx_status_report_test, handle_status_report_with_bad_tail_bitmap)
{
  init(GetParam());
  auto test_with_count = [this](uint32_t tx_next) {
    // clear queue from previous runs
    test_frame.sdu_discard_queue   = {};
    unsigned                n_sdus = 5;
    std::queue<byte_buffer> exp_pdu_list;
    pdcp_tx_state           st = {tx_next, tx_next, 0, tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::off, security::ciphering_enabled::off);
    ocudu::test_delimit_logger delimiter(
        "Testing status report with bad padding in bitmap. SN_SIZE={} COUNT={}", sn_size, tx_next);
    for (uint32_t count = tx_next; count < tx_next + n_sdus; ++count) {
      // Write SDU
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Wait for crypto and reordering
      wait_pending_crypto();
      worker.run_pending_tasks();

      pdcp_tx->handle_transmit_notification(pdcp_compute_sn(count, sn_size));

      // Get generated PDU
      FLUSH_AND_ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      exp_pdu_list.push(std::move(test_frame.pdu_queue.front()));
      test_frame.pdu_queue.pop();
      ASSERT_EQ(test_frame.pdu_queue.size(), 0);
    }

    FLUSH_AND_ASSERT_TRUE(test_frame.sdu_discard_queue.empty());

    // Build status report that confirms RX of SDUs:
    // Confirm: [tx_next,            tx_next+2,            tx_next+4]
    // Missing: [         tx_next+1,            tx_next+3,          ]
    //
    // Report:            [  FMC  ][    1          0          1     ][  1   0   0   0   0  ]
    //                    |-first-||--------- bitmap ---------------;;-- bitmap padding ---|
    byte_buffer buf = {};
    bit_encoder enc(buf);

    // Pack PDU header
    enc.pack(to_number(pdcp_dc_field::control), 1);
    enc.pack(to_number(pdcp_control_pdu_type::status_report), 3);
    enc.pack(0b0000, 4);

    // Pack tx_next+1 into FMC field
    enc.pack(tx_next + 1, 32);

    // Pack bitmap
    enc.pack(0b10110000, 8);

    // Handle this status report
    pdcp_tx->on_status_report(byte_buffer_chain::create(std::move(buf)).value());

    // Make sure no PDU were discarded as the status report is invalid.
    FLUSH_AND_ASSERT_TRUE(test_frame.sdu_discard_queue.empty());
  };

  if (sn_size == pdcp_sn_size::size12bits) {
    test_with_count(0);
    test_with_count(2048);
    test_with_count(4096);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_with_count(0);
    test_with_count(131072);
    test_with_count(262144);
  } else {
    FAIL();
  }
}

/// \brief Test basic data recovery functionality
TEST_P(pdcp_tx_status_report_test, data_recovery)
{
  init(GetParam());
  auto test_with_count = [this](uint32_t tx_next) {
    unsigned                n_sdus = 5;
    std::queue<byte_buffer> exp_pdu_list;
    pdcp_tx_state           st = {tx_next, tx_next, 0, tx_next, tx_next};
    pdcp_tx->set_state(st);
    pdcp_tx->configure_security(sec_cfg, security::integrity_enabled::off, security::ciphering_enabled::off);
    ocudu::test_delimit_logger delimiter("Testing data recovery. SN_SIZE={} COUNT={}", sn_size, tx_next);
    for (uint32_t count = tx_next; count < tx_next + n_sdus; ++count) {
      // Write SDU
      byte_buffer sdu = byte_buffer::create(sdu1).value();
      pdcp_tx->handle_sdu(std::move(sdu));

      // Wait for crypto and reordering
      wait_pending_crypto();
      worker.run_pending_tasks();

      pdcp_tx->handle_transmit_notification(pdcp_compute_sn(count, sn_size));

      // Get generated PDU
      FLUSH_AND_ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      exp_pdu_list.push(std::move(test_frame.pdu_queue.front()));
      test_frame.pdu_queue.pop();
      FLUSH_AND_ASSERT_EQ(test_frame.pdu_queue.size(), 0);
    }

    pdcp_tx->data_recovery();

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();

    // read the status report
    {
      FLUSH_AND_ASSERT_EQ(test_frame.pdu_queue.size(), 1);
      byte_buffer pdu = std::move(test_frame.pdu_queue.front());
      test_frame.pdu_queue.pop();
      byte_buffer exp_pdu = test_frame.compile_status_report();
      FLUSH_AND_ASSERT_EQ(pdu.length(), exp_pdu.length());
      FLUSH_AND_ASSERT_EQ(pdu, exp_pdu);
    }

    // read data PDUs
    FLUSH_AND_ASSERT_EQ(test_frame.retx_queue.size(), n_sdus);
    for (uint32_t count = tx_next; count < tx_next + n_sdus; ++count) {
      byte_buffer pdu = std::move(test_frame.retx_queue.front());
      test_frame.retx_queue.pop();
      byte_buffer exp_pdu = std::move(exp_pdu_list.front());
      exp_pdu_list.pop();
      FLUSH_AND_ASSERT_EQ(pdu.length(), exp_pdu.length());
      FLUSH_AND_ASSERT_EQ(pdu, exp_pdu);
    }

    while (not test_frame.pdu_queue.empty()) {
      test_frame.pdu_queue.pop();
    }
    while (not test_frame.retx_queue.empty()) {
      test_frame.retx_queue.pop();
    }
  };

  if (sn_size == pdcp_sn_size::size12bits) {
    test_with_count(0);
    test_with_count(2048);
    test_with_count(4096);
  } else if (sn_size == pdcp_sn_size::size18bits) {
    test_with_count(0);
    test_with_count(131072);
    test_with_count(262144);
  } else {
    FAIL();
  }
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
    pdcp_tx_status_report_test,
    ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                       ::testing::Values(1),
                       ::testing::Values(cfg_rohc_disabled, cfg_rohc_uncompressed, cfg_rohc_compressed)),
    test_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
