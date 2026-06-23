// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "pdcp_rx_test.h"
#include "pdcp_test_vectors.h"
#include "ocudu/pdcp/pdcp_config.h"
#include "ocudu/support/test_utils.h"
#include <gtest/gtest.h>
#include <queue>

using namespace ocudu;

/// Test creation of PDCP RX entities
TEST_P(pdcp_rx_test_drb, create_new_entity)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  init();
  ocudu::test_delimit_logger delimiter("Entity creation test. SN_SIZE={} ", config.sn_size);
  unsigned                   exp_nof_deompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_deompressors);
  EXPECT_EQ(pdcp_rohc_factory->get_last_decompressor_config(), config.header_compression);

  ASSERT_NE(pdcp_rx, nullptr);
  ASSERT_NE(test_frame, nullptr);

  // No warnings or errors during construction
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);
}

/// Test extraction of PDCP sequence numbers
TEST_P(pdcp_rx_test_drb, sn_unpack)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  init();
  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  auto test_hdr_reader = [this](uint32_t count) {
    ocudu::test_delimit_logger delimiter("Header reader test. SN_SIZE={} COUNT={}", config.sn_size, count);
    // Get PDU to test
    byte_buffer test_pdu;
    get_test_pdu(count, test_pdu);
    pdcp_data_pdu_header hdr;
    ASSERT_TRUE(pdcp_rx->read_data_pdu_header(hdr, test_pdu));
    ASSERT_EQ(hdr.sn, SN(count));
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_hdr_reader(0);
    test_hdr_reader(2048);
    test_hdr_reader(4096);
    test_hdr_reader(4294967295);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_hdr_reader(0);
    test_hdr_reader(131072);
    test_hdr_reader(262144);
    test_hdr_reader(4294967295);
  } else {
    FAIL();
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test in-order reception of PDCP PDUs
TEST_P(pdcp_rx_test_drb, rx_in_order)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  init();
  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  auto test_rx_in_order = [this](uint32_t count) {
    ocudu::test_delimit_logger delimiter("RX in order test. SN_SIZE={} COUNT={}", config.sn_size, count);

    pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    ASSERT_EQ(2, test_frame->sdu_queue.size());
    while (not test_frame->sdu_queue.empty()) {
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    }
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_rx_in_order(0);
    test_rx_in_order(2047);
    test_rx_in_order(4095);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_rx_in_order(0);
    test_rx_in_order(131071);
    test_rx_in_order(262143);
  } else {
    FAIL();
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test out of order reception of PDUs.
/// All PDUs are received before the t-Reordering expires.
TEST_P(pdcp_rx_test_drb, rx_out_of_order)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  init();
  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  auto test_rx_out_of_order = [this](uint32_t count) {
    ocudu::test_delimit_logger delimiter(
        "RX out-of-order test, no t-Reordering. SN_SIZE={} COUNT=[{}, {}]", config.sn_size, count + 1, count);

    pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    byte_buffer test_pdu3;
    get_test_pdu(count + 2, test_pdu3);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);

    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    if (config.out_of_order_delivery) {
      ASSERT_EQ(1, test_frame->sdu_queue.size());
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    } else {
      ASSERT_EQ(0, test_frame->sdu_queue.size());
    }
    // check rx_reord matches rx_next matches count + 2
    EXPECT_EQ(pdcp_rx->get_state().rx_reord, count + 2);
    EXPECT_EQ(pdcp_rx->get_state().rx_reord, pdcp_rx->get_state().rx_next);

    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu3)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    if (config.out_of_order_delivery) {
      ASSERT_EQ(1, test_frame->sdu_queue.size());
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    } else {
      ASSERT_EQ(0, test_frame->sdu_queue.size());
    }
    // check rx_reord still maches count + 2, i.e did not change because t_reord is already running; rx_next moved on
    EXPECT_EQ(pdcp_rx->get_state().rx_reord, count + 2);
    EXPECT_EQ(pdcp_rx->get_state().rx_next, count + 3);

    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    if (config.out_of_order_delivery) {
      ASSERT_EQ(1, test_frame->sdu_queue.size());
    } else {
      ASSERT_EQ(3, test_frame->sdu_queue.size());
    }
    while (not test_frame->sdu_queue.empty()) {
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    }
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_rx_out_of_order(0);
    test_rx_out_of_order(2047);
    test_rx_out_of_order(4095);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_rx_out_of_order(0);
    test_rx_out_of_order(131071);
    test_rx_out_of_order(262143);
  } else {
    FAIL();
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test reception of duplicate PDCP PDUs
TEST_P(pdcp_rx_test_drb, rx_duplicate)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  init();
  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  auto test_rx_out_of_order = [this](uint32_t count) {
    ocudu::test_delimit_logger delimiter(
        "RX duplicate test. SN_SIZE={} COUNT=[{}, {}]", config.sn_size, count + 1, count);

    pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    byte_buffer test_pdu2_dup;
    get_test_pdu(count + 1, test_pdu2_dup);
    byte_buffer test_pdu3;
    get_test_pdu(count + 2, test_pdu3);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);

    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    if (config.out_of_order_delivery) {
      ASSERT_EQ(1, test_frame->sdu_queue.size());
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    } else {
      ASSERT_EQ(0, test_frame->sdu_queue.size());
    }
    // check rx_reord matches rx_next matches count + 2
    EXPECT_EQ(pdcp_rx->get_state().rx_reord, count + 2);
    EXPECT_EQ(pdcp_rx->get_state().rx_reord, pdcp_rx->get_state().rx_next);

    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu3)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    if (config.out_of_order_delivery) {
      ASSERT_EQ(1, test_frame->sdu_queue.size());
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    } else {
      ASSERT_EQ(0, test_frame->sdu_queue.size());
    }
    // check rx_reord still maches count + 2, i.e did not change because t_reord is already running; rx_next moved on
    EXPECT_EQ(pdcp_rx->get_state().rx_reord, count + 2);
    EXPECT_EQ(pdcp_rx->get_state().rx_next, count + 3);

    // Send the duplicate
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2_dup)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    ASSERT_EQ(0, test_frame->sdu_queue.size());
    // check rx_reord still maches count + 2
    EXPECT_EQ(pdcp_rx->get_state().rx_reord, count + 2);
    EXPECT_EQ(pdcp_rx->get_state().rx_next, count + 3);

    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    if (config.out_of_order_delivery) {
      ASSERT_EQ(1, test_frame->sdu_queue.size());
    } else {
      ASSERT_EQ(3, test_frame->sdu_queue.size());
    }
    while (not test_frame->sdu_queue.empty()) {
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    }
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_rx_out_of_order(0);
    test_rx_out_of_order(2047);
    test_rx_out_of_order(4095);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_rx_out_of_order(0);
    test_rx_out_of_order(131071);
    test_rx_out_of_order(262143);
  } else {
    FAIL();
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test out of order reception of PDUs.
/// The out-of-order PDU is received after the t-Reordering expires (except if OOO delivery is configured).
TEST_P(pdcp_rx_test_drb, rx_reordering_timer)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  init();
  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  auto test_rx_t_reorder = [this](uint32_t count) {
    ocudu::test_delimit_logger delimiter(
        "RX out-of-order test, t-Reordering expires. SN_SIZE={} COUNT=[{}, {}]", config.sn_size, count + 1, count);

    pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    if (config.out_of_order_delivery) {
      ASSERT_EQ(1, test_frame->sdu_queue.size());
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
    } else {
      ASSERT_EQ(0, test_frame->sdu_queue.size());
    }
    tick_all(10);
    ASSERT_EQ(1, test_frame->sdu_queue.size());
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    ASSERT_EQ(1, test_frame->sdu_queue.size());
    while (not test_frame->sdu_queue.empty()) {
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    }
  };
  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(2047);
    test_rx_t_reorder(4095);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(131071);
    test_rx_t_reorder(262143);
  } else {
    FAIL();
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test out of order reception of PDUs.
/// t-Reordering is set to 0, so PDUs are immediately delivered.
TEST_P(pdcp_rx_test_drb, rx_reordering_timer_0ms)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  set_t_reordering(pdcp_t_reordering::ms0);
  init();
  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  auto test_rx_t_reorder = [this](uint32_t count) {
    ocudu::test_delimit_logger delimiter(
        "RX out-of-order test, t-Reordering is set to 0. SN_SIZE={} COUNT=[{}, {}]", config.sn_size, count + 1, count);

    pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    ASSERT_EQ(1, test_frame->sdu_queue.size());
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    ASSERT_EQ(1, test_frame->sdu_queue.size());
    while (not test_frame->sdu_queue.empty()) {
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    }
  };
  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(2047);
    test_rx_t_reorder(4095);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(131071);
    test_rx_t_reorder(262143);
  } else {
    FAIL();
  }

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test out of order reception of PDUs.
/// t-Reordering is set to infinite, so no PDUs are delivered
/// until they are received in order (except if OOO delivery is configured).
TEST_P(pdcp_rx_test_drb, rx_reordering_timer_infinite)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  set_t_reordering(pdcp_t_reordering::infinity);
  init();
  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  auto test_rx_t_reorder = [this](uint32_t count) {
    ocudu::test_delimit_logger delimiter(
        "RX out-of-order test, t-Reordering is set to infinity. SN_SIZE={} COUNT=[{}, {}]",
        config.sn_size,
        count + 1,
        count);

    pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    byte_buffer test_pdu2;
    get_test_pdu(count + 1, test_pdu2);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu2)).value());
    tick_all(6000); // max t-Reordering is 3000ms

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    if (config.out_of_order_delivery) {
      ASSERT_EQ(1, test_frame->sdu_queue.size());
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
    } else {
      ASSERT_EQ(0, test_frame->sdu_queue.size());
    }

    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    ASSERT_EQ(2, test_frame->sdu_queue.size());
    while (not test_frame->sdu_queue.empty()) {
      ASSERT_EQ(test_frame->sdu_queue.front(), sdu1);
      test_frame->sdu_queue.pop();
    }
  };
  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(2047);
    test_rx_t_reorder(4095);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_rx_t_reorder(0);
    test_rx_t_reorder(131071);
    test_rx_t_reorder(262143);
  } else {
    FAIL();
  }

  // One warning but no errors
  EXPECT_EQ(test_spy.get_warning_counter(), 1);
  EXPECT_EQ(test_spy.get_error_counter(), 0);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test reception of PDUs with bad integrity checks.
/// The PDCP should notify the RRC of the integrity error.
TEST_P(pdcp_rx_test_drb, rx_integrity_fail)
{
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  init();
  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  auto test_rx_integrity_fail = [this](uint32_t count) {
    ocudu::test_delimit_logger delimiter("RX PDU with bad integrity. SN_SIZE={} COUNT={}", config.sn_size, count);

    pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    ASSERT_TRUE(test_pdu1.append(0)); // mess up MAC-I
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    uint32_t prev_integrity_fail_counter = test_frame->integrity_fail_counter;
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    ASSERT_EQ(0, test_frame->sdu_queue.size());
    ASSERT_EQ(prev_integrity_fail_counter + 1, test_frame->integrity_fail_counter);
  };

  if (config.sn_size == pdcp_sn_size::size12bits) {
    test_rx_integrity_fail(0);
    test_rx_integrity_fail(2047);
    test_rx_integrity_fail(4095);
  } else if (config.sn_size == pdcp_sn_size::size18bits) {
    test_rx_integrity_fail(0);
    test_rx_integrity_fail(131071);
    test_rx_integrity_fail(262143);
  } else {
    FAIL();
  }

  // Three warnings but no errors
  EXPECT_EQ(test_spy.get_warning_counter(), 3);
  EXPECT_EQ(test_spy.get_error_counter(), 0);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test reception of SRB PDUs with zero-padded MAC-I across all integrity modes (off, on, SMC transition mode).
/// The PDCP should reject the PDUs only in when integrity is fully enabled.
TEST_P(pdcp_rx_test_srb, rx_zero_padded_mac)
{
  set_rb_type(pdcp_rb_type::srb);
  init();
  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  auto test_rx_integrity_mode = [this](uint32_t count) {
    ocudu::test_delimit_logger delimiter(
        "RX PDU with unverified integrity. SN_SIZE={} COUNT={}", config.sn_size, count);

    if (std::get<security::integrity_enabled>(GetParam()) != security::integrity_enabled::off) {
      pdcp_rx->configure_security(
          sec_cfg, std::get<security::integrity_enabled>(GetParam()), security::ciphering_enabled::off);
    }

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    // overwrite MAC-I with zero padding
    byte_buffer_view mac{test_pdu1, test_pdu1.length() - security::sec_mac_len, security::sec_mac_len};
    std::fill(mac.begin(), mac.end(), 0x0);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    uint32_t prev_integrity_fail_counter = test_frame->integrity_fail_counter;
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();
    if (std::get<security::integrity_enabled>(GetParam()) == security::integrity_enabled::on) {
      // Integrity on: Expect PDU to be dropped.
      ASSERT_EQ(0, test_frame->sdu_queue.size());
      ASSERT_EQ(prev_integrity_fail_counter + 1, test_frame->integrity_fail_counter);
    } else {
      // Integrity off or in SMC transition: Expect PDU to pass.
      ASSERT_EQ(1, test_frame->sdu_queue.size());
      ASSERT_EQ(prev_integrity_fail_counter, test_frame->integrity_fail_counter);
      while (not test_frame->sdu_queue.empty()) {
        test_frame->sdu_queue.pop();
      }
    }
  };

  test_rx_integrity_mode(0);
  test_rx_integrity_mode(2047);
  test_rx_integrity_mode(4095);

  if (std::get<security::integrity_enabled>(GetParam()) == security::integrity_enabled::on) {
    // Three warnings but no errors
    EXPECT_EQ(test_spy.get_warning_counter(), 3);
    EXPECT_EQ(test_spy.get_error_counter(), 0);
  } else {
    // No warnings and no errors
    EXPECT_EQ(test_spy.get_warning_counter(), 0);
    EXPECT_EQ(test_spy.get_error_counter(), 0);
  }

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test reception of SRB PDUs with non zero-padded MAC-I across all integrity modes (off, on, SMC transition mode).
/// The PDCP should reject all PDUs.
TEST_P(pdcp_rx_test_srb, rx_non_zero_padded_mac)
{
  set_rb_type(pdcp_rb_type::srb);
  init();
  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  auto test_rx_integrity_mode = [this](uint32_t count) {
    ocudu::test_delimit_logger delimiter(
        "RX PDU with unverified integrity. SN_SIZE={} COUNT={}", config.sn_size, count);

    if (std::get<security::integrity_enabled>(GetParam()) != security::integrity_enabled::off) {
      pdcp_rx->configure_security(
          sec_cfg, std::get<security::integrity_enabled>(GetParam()), security::ciphering_enabled::off);
    }

    byte_buffer test_pdu1;
    get_test_pdu(count, test_pdu1);
    // overwrite MAC-I with non-zero padding
    byte_buffer_view mac{test_pdu1, test_pdu1.length() - security::sec_mac_len, security::sec_mac_len};
    std::fill(mac.begin(), mac.end(), 0x1);
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);
    uint32_t prev_integrity_fail_counter = test_frame->integrity_fail_counter;
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu1)).value());

    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();

    // Expect PDU to be dropped.
    ASSERT_EQ(0, test_frame->sdu_queue.size());
    ASSERT_EQ(prev_integrity_fail_counter + 1, test_frame->integrity_fail_counter);
  };

  test_rx_integrity_mode(0);
  test_rx_integrity_mode(2047);
  test_rx_integrity_mode(4095);

  // Three warnings but no errors
  EXPECT_EQ(test_spy.get_warning_counter(), 3);
  EXPECT_EQ(test_spy.get_error_counter(), 0);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test count wrap-around protection for PDCP RX
/// Receive one PDU before notify limit, four after notify
/// limit and one after the hard limit.
TEST_P(pdcp_rx_test_drb, count_wraparound)
{
  uint32_t       rx_next_notify = 262144;
  uint32_t       rx_next_max    = 262148;
  uint32_t       rx_next_start  = 262143;
  uint32_t       n_sdus         = 6;
  pdcp_max_count max_count{rx_next_notify, rx_next_max};

  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  set_max_count(max_count);
  init();

  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  auto test_max_count = [this, n_sdus](uint32_t count) {
    // Set state of PDCP entiy
    // Do not enable integrity or ciphering, to make it easier to generate test vectors.
    pdcp_rx_state init_state = {.rx_next = count, .rx_deliv = count, .rx_reord = 0};
    pdcp_rx->set_state(init_state);

    pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::off, security::ciphering_enabled::off);

    // Write SDUs
    for (uint32_t i = 0; i < n_sdus; i++) {
      byte_buffer pdu;
      get_test_pdu(count + i, pdu);
      pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(pdu)).value());
    }
    // Wait for crypto and reordering
    wait_pending_crypto();
    worker.run_pending_tasks();

    // check nof max_count reached and max protocol failures.
    ASSERT_EQ(5, test_frame->sdu_queue.size());
    ASSERT_EQ(1, test_frame->nof_max_count_reached);
    ASSERT_EQ(1, test_frame->nof_protocol_failure);
  };

  test_max_count(rx_next_start);

  // One warning and one error
  EXPECT_EQ(test_spy.get_warning_counter(), 1);
  EXPECT_EQ(test_spy.get_error_counter(), 1);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

/// Test TX SDU buffering.
TEST_P(pdcp_rx_test_drb, rx_buffer)
{
  uint32_t n_no_buffer_pdus = 1;
  uint32_t n_buffer_pdus    = 2;
  set_sn_size(std::get<pdcp_sn_size>(GetParam()));
  set_algo(std::get<unsigned>(GetParam()));
  set_header_compression(std::get<rohc_test_params>(GetParam()).config);
  init();
  unsigned exp_nof_decompressors = config.header_compression.has_value() ? 1 : 0;
  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);

  pdcp_rx->configure_security(sec_cfg, security::integrity_enabled::on, security::ciphering_enabled::on);

  for (uint32_t count = 0; count < n_no_buffer_pdus; count++) {
    byte_buffer test_pdu;
    get_test_pdu(count, test_pdu);
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu)).value());
  }

  // Wait for crypto and reordering.
  wait_pending_crypto();
  worker.run_pending_tasks();

  // Check SDUs were received correctly.
  FLUSH_AND_ASSERT_EQ(1, test_frame->sdu_queue.size());
  test_frame->sdu_queue = {}; // clear queue.

  pdcp_rx->begin_buffering();
  for (uint32_t n = 0; n < n_buffer_pdus; n++) {
    uint32_t    count = n_no_buffer_pdus + n;
    byte_buffer test_pdu;
    get_test_pdu(count, test_pdu);
    pdcp_rx->handle_pdu(byte_buffer_chain::create(std::move(test_pdu)).value());
  }
  FLUSH_AND_ASSERT_EQ(0, test_frame->sdu_queue.size());
  pdcp_rx->end_buffering();
  wait_pending_crypto();
  worker.run_pending_tasks();
  FLUSH_AND_ASSERT_EQ(2, test_frame->sdu_queue.size());

  // No warnings or errors
  EXPECT_EQ(test_spy.get_warning_counter(), 0);
  EXPECT_EQ(test_spy.get_error_counter(), 0);

  EXPECT_EQ(pdcp_rohc_factory->get_nof_compressors(), 0);
  EXPECT_EQ(pdcp_rohc_factory->get_nof_decompressors(), exp_nof_decompressors);
}

///////////////////////////////////////////////////////////////////
// Finally, instantiate all testcases for each supported SN size //
///////////////////////////////////////////////////////////////////
static std::string pdcp_rx_test_drb_param_info_to_string(
    const ::testing::TestParamInfo<std::tuple<pdcp_sn_size, unsigned, rohc_test_params, bool>>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "{}bit_nia{}_nea{}_{}_ooo_{}",
                 pdcp_sn_size_to_uint(std::get<pdcp_sn_size>(info.param)),
                 std::get<unsigned>(info.param),
                 std::get<unsigned>(info.param),
                 std::get<rohc_test_params>(info.param).name,
                 std::get<bool>(info.param));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(
    pdcp_rx_test_sn_all_algo_all_rohc_all_ooo_off,
    pdcp_rx_test_drb,
    ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                       ::testing::Values(1, 2, 3),
                       ::testing::Values(cfg_rohc_disabled, cfg_rohc_uncompressed, cfg_rohc_compressed),
                       ::testing::Values(false)),
    pdcp_rx_test_drb_param_info_to_string);

INSTANTIATE_TEST_SUITE_P(pdcp_rx_test_sn_all_algo_all_rohc_off_ooo_all,
                         pdcp_rx_test_drb,
                         ::testing::Combine(::testing::Values(pdcp_sn_size::size12bits, pdcp_sn_size::size18bits),
                                            ::testing::Values(1, 2, 3),
                                            ::testing::Values(cfg_rohc_disabled),
                                            ::testing::Values(true)),
                         pdcp_rx_test_drb_param_info_to_string);

static std::string pdcp_rx_test_srb_param_info_to_string(
    const ::testing::TestParamInfo<std::tuple<unsigned, security::integrity_enabled>>& info)
{
  fmt::memory_buffer buffer;
  fmt::format_to(std::back_inserter(buffer),
                 "nia{}_{}_nea{}",
                 std::get<unsigned>(info.param),
                 std::get<security::integrity_enabled>(info.param),
                 std::get<unsigned>(info.param));
  return fmt::to_string(buffer);
}

INSTANTIATE_TEST_SUITE_P(pdcp_rx_test_all_variants,
                         pdcp_rx_test_srb,
                         ::testing::Combine(::testing::Values(1, 2, 3),
                                            ::testing::Values(security::integrity_enabled::off,
                                                              security::integrity_enabled::on,
                                                              security::integrity_enabled::smc_transition)),
                         pdcp_rx_test_srb_param_info_to_string);

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
