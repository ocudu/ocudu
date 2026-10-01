// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#pragma once

#include "lib/sdap/sdap_entity_impl.h"
#include "ocudu/sdap/sdap.h"
#include <gtest/gtest.h>
#include <queue>

namespace ocudu {

namespace ocuup {

/// Mocking class of the upper layers invoked by the SDAP entity.
class sdap_upper_dummy : public sdap_rx_sdu_notifier
{
public:
  std::queue<byte_buffer> sdu_queue;

  // sdap_rx_sdu_notifier interface
  void on_new_sdu(byte_buffer pdu, qos_flow_id_t qfi) override { sdu_queue.push(std::move(pdu)); }
};

/// Mocking class of the lower layers invoked by the SDAP entity.
class sdap_lower_dummy : public sdap_tx_pdu_notifier
{
public:
  std::queue<byte_buffer> pdu_queue;

  // sdap_tx_pdu_notifier interface
  void on_new_pdu(byte_buffer pdu) override { pdu_queue.push(std::move(pdu)); }
};

/// Fixture class for SDAP tests
class sdap_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    logger.set_level(ocudulog::basic_levels::debug);

    // init SDAP logger
    ocudulog::fetch_basic_logger("SDAP", false).set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("SDAP", false).set_hex_dump_max_size(-1);

    logger.info("Creating SDAP entity.");

    // Create test frames
    ul_sink  = std::make_unique<sdap_upper_dummy>();
    dl_sink1 = std::make_unique<sdap_lower_dummy>();
    dl_sink2 = std::make_unique<sdap_lower_dummy>();

    // Create SDAP TX entity
    sdap = std::make_unique<sdap_entity_impl>(7, pdu_session_id_t::min, *ul_sink);
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  ocudulog::basic_logger&           logger = ocudulog::fetch_basic_logger("TEST", false);
  std::unique_ptr<sdap_upper_dummy> ul_sink;
  std::unique_ptr<sdap_lower_dummy> dl_sink1;
  std::unique_ptr<sdap_lower_dummy> dl_sink2;
  std::unique_ptr<sdap_entity_impl> sdap;
};

} // namespace ocuup

} // namespace ocudu
