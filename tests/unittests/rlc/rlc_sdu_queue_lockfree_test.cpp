// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/rlc/rlc_bearer_logger.h"
#include "lib/rlc/rlc_sdu_queue_lockfree.h"
#include "ocudu/adt/byte_buffer.h"
#include "ocudu/ran/du_types.h"
#include <gtest/gtest.h>

using namespace ocudu;

/// Fixture class for RLC SDU queue tests
class rlc_sdu_queue_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    ocudulog::init();
    test_logger.set_level(ocudulog::basic_levels::debug);

    // init RLC logger
    ocudulog::fetch_basic_logger("RLC", false).set_level(ocudulog::basic_levels::debug);
    ocudulog::fetch_basic_logger("RLC", false).set_hex_dump_max_size(-1);

    test_logger.info("Creating RLC SDU queue");
  }

  void TearDown() override
  {
    // flush logger after each test
    ocudulog::flush();
  }

  ///  Default value for RLC SDU queue limit in bytes are chosen such that it allows for 4096 PDCP pdus of 1500 of
  ///  payload and 7 bytes of PDCP overhead. The SDU limit should be much larger then this, so that the limit is the
  ///  number of bytes in the queue, not the number of SDUs, even in the case of small PDUs
  static constexpr uint32_t default_rlc_queue_size_sdus  = 16384;
  static constexpr uint32_t default_rlc_queue_size_bytes = 4096 * (1500 + 7);

  ocudulog::basic_logger& test_logger = ocudulog::fetch_basic_logger("TEST", false);
  rlc_bearer_logger       rlc_logger  = {"RLC",
                                         {gnb_du_id_t::min, du_ue_index_t::MIN_DU_UE_INDEX, rb_id_t(drb_id_t::drb1), "DL"}};
};

TEST_F(rlc_sdu_queue_test, queue_unqueue)
{
  rlc_sdu_queue_lockfree tx_queue(default_rlc_queue_size_sdus, default_rlc_queue_size_bytes, rlc_logger);

  // Write 1 SDU
  byte_buffer buf       = byte_buffer::create({0x00, 0x01}).value();
  rlc_sdu     write_sdu = {std::move(buf), 10};
  ASSERT_TRUE(tx_queue.write(std::move(write_sdu)));

  // Check basic stats
  rlc_sdu_queue_lockfree::state_t state = tx_queue.get_state();
  ASSERT_EQ(1, state.n_sdus);
  ASSERT_EQ(2, state.n_bytes);

  // Read one SDU
  rlc_sdu read_sdu;
  ASSERT_TRUE(tx_queue.read(read_sdu));

  // Check basic stats
  state = tx_queue.get_state();
  ASSERT_EQ(0, state.n_sdus);
  ASSERT_EQ(0, state.n_bytes);

  // Check SDU
  byte_buffer expected_msg = byte_buffer::create({0x00, 0x01}).value();
  ASSERT_TRUE(read_sdu.pdcp_sn.has_value());
  ASSERT_EQ(10, read_sdu.pdcp_sn.value());
  ASSERT_EQ(expected_msg, read_sdu.buf);
}

TEST_F(rlc_sdu_queue_test, full_capacity)
{
  unsigned capacity = 5;

  rlc_sdu_queue_lockfree tx_queue(capacity, default_rlc_queue_size_bytes, rlc_logger);

  // Write Capacity + 1 SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < capacity + 1; pdcp_sn++) {
    byte_buffer buf = {};
    ASSERT_TRUE(buf.append(pdcp_sn));
    ASSERT_TRUE(buf.append(pdcp_sn));
    rlc_sdu write_sdu = {std::move(buf), pdcp_sn};
    if (pdcp_sn != capacity) {
      ASSERT_TRUE(tx_queue.write(std::move(write_sdu)) == true);
    } else {
      ASSERT_TRUE(tx_queue.write(std::move(write_sdu)) == false);
    }
  }
  rlc_sdu_queue_lockfree::state_t state = tx_queue.get_state();
  ASSERT_EQ(capacity, state.n_sdus);
  ASSERT_EQ(2 * capacity, state.n_bytes);

  // Read all SDUs and try to read on SDU over capacity
  for (uint32_t pdcp_sn = 0; pdcp_sn < capacity + 1; pdcp_sn++) {
    byte_buffer expected_msg = {};
    ASSERT_TRUE(expected_msg.append(pdcp_sn));
    ASSERT_TRUE(expected_msg.append(pdcp_sn));
    rlc_sdu read_sdu = {};
    if (pdcp_sn != capacity) {
      ASSERT_TRUE(tx_queue.read(read_sdu));
      ASSERT_TRUE(expected_msg == read_sdu.buf);
    } else {
      ASSERT_TRUE(false == tx_queue.read(read_sdu));
    }
  }

  state = tx_queue.get_state();
  ASSERT_EQ(0, state.n_sdus);
  ASSERT_EQ(0, state.n_bytes);
}

TEST_F(rlc_sdu_queue_test, full_capacity_bytes)
{
  unsigned sdu_len        = 1;
  unsigned capacity_bytes = 5;

  rlc_sdu_queue_lockfree tx_queue(default_rlc_queue_size_sdus, capacity_bytes, rlc_logger);

  // Write Capacity + 1 SDUs
  unsigned written_bytes = 0;
  unsigned written_sdus  = 0;
  for (unsigned i = 0; i < capacity_bytes / sdu_len + 1; i++) {
    byte_buffer buf = {};
    ASSERT_TRUE(buf.append(static_cast<uint8_t>(i)));
    rlc_sdu write_sdu = {std::move(buf), written_sdus};
    bool    write_ok  = tx_queue.write(std::move(write_sdu));
    written_bytes += sdu_len;
    written_sdus += 1;

    if (written_bytes <= capacity_bytes) {
      ASSERT_TRUE(write_ok) << "Write to queue failed before reaching full capacity";
    } else {
      ASSERT_FALSE(write_ok) << "Write to queue succeeded beyond reaching full capacity";
    }
  }
  rlc_sdu_queue_lockfree::state_t state = tx_queue.get_state();
  ASSERT_EQ(written_sdus, capacity_bytes / sdu_len + 1); // Tried to write one extra SDU beyond capacity.
  ASSERT_EQ(state.n_sdus, capacity_bytes / sdu_len);     // Actually wrote only those who fit the capacity.
  ASSERT_EQ(written_bytes, capacity_bytes + sdu_len);    // Tried to write one SDU length beyond capacity.
  ASSERT_EQ(state.n_bytes, capacity_bytes);              // Actually wrote up to the full capacity.

  // Read all SDUs and try to read on SDU over capacity
  for (unsigned i = 0; i < capacity_bytes / sdu_len + 1; i++) {
    byte_buffer expected_msg = {};
    ASSERT_TRUE(expected_msg.append(static_cast<uint8_t>(i)));
    rlc_sdu read_sdu = {};
    if (i < capacity_bytes / sdu_len) {
      ASSERT_TRUE(tx_queue.read(read_sdu));
      ASSERT_EQ(expected_msg, read_sdu.buf);
    } else {
      ASSERT_FALSE(tx_queue.read(read_sdu));
    }
  }

  state = tx_queue.get_state();
  ASSERT_EQ(0, state.n_sdus);
  ASSERT_EQ(0, state.n_bytes);
}

TEST_F(rlc_sdu_queue_test, discard)
{
  unsigned       capacity          = 10;
  unsigned       n_sdus            = capacity;
  const uint32_t queue_bytes_limit = 6172672;

  rlc_sdu_queue_lockfree tx_queue(capacity, queue_bytes_limit, rlc_logger);

  // Fill SDU queue with SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_sdus; pdcp_sn++) {
    byte_buffer buf = {};
    ASSERT_TRUE(buf.append(pdcp_sn));
    ASSERT_TRUE(buf.append(pdcp_sn));
    rlc_sdu write_sdu = {std::move(buf), pdcp_sn};
    ASSERT_TRUE(tx_queue.write(std::move(write_sdu)));
  }
  rlc_sdu_queue_lockfree::state_t state = tx_queue.get_state();
  ASSERT_EQ(n_sdus, state.n_sdus);
  ASSERT_EQ(2 * n_sdus, state.n_bytes);

  // Discard pdcp_sn 2 and 4
  ASSERT_TRUE(tx_queue.try_discard(2));
  ASSERT_TRUE(tx_queue.try_discard(4));

  // Try to discard non-existing pdcp_sn
  ASSERT_TRUE(false == tx_queue.try_discard(16));

  // Double check correct number of SDUs and SDU bytes
  unsigned leftover_sdus = n_sdus - 2;
  state                  = tx_queue.get_state();
  ASSERT_EQ(leftover_sdus, state.n_sdus);
  ASSERT_EQ(leftover_sdus * 2, state.n_bytes);

  // Read SDUs
  for (uint32_t n = 0; n < leftover_sdus; n++) {
    rlc_sdu read_sdu = {};
    ASSERT_TRUE(tx_queue.read(read_sdu));
  }
  state = tx_queue.get_state();
  ASSERT_EQ(0, state.n_sdus);
  ASSERT_EQ(0, state.n_bytes);
}

TEST_F(rlc_sdu_queue_test, discard_all)
{
  unsigned capacity = 10;
  unsigned n_sdus   = capacity / 2;

  rlc_sdu_queue_lockfree tx_queue(capacity, default_rlc_queue_size_bytes, rlc_logger);

  // Fill SDU queue with SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_sdus; pdcp_sn++) {
    byte_buffer buf = {};
    ASSERT_TRUE(buf.append(pdcp_sn));
    ASSERT_TRUE(buf.append(pdcp_sn));
    rlc_sdu write_sdu = {std::move(buf), pdcp_sn};
    ASSERT_TRUE(tx_queue.write(std::move(write_sdu)) == true);
  }
  rlc_sdu_queue_lockfree::state_t state = tx_queue.get_state();
  ASSERT_EQ(n_sdus, state.n_sdus);
  ASSERT_EQ(2 * n_sdus, state.n_bytes);

  // Discard all SDUs
  for (uint32_t pdcp_sn = 0; pdcp_sn < n_sdus; pdcp_sn++) {
    ASSERT_TRUE(tx_queue.try_discard(pdcp_sn));
  }

  state = tx_queue.get_state();
  ASSERT_EQ(0, state.n_sdus);
  ASSERT_EQ(0, state.n_bytes);

  // Read SDU
  {
    rlc_sdu read_sdu = {};
    ASSERT_TRUE(tx_queue.read(read_sdu) == false);
  }
  state = tx_queue.get_state();
  ASSERT_EQ(0, state.n_sdus);
  ASSERT_EQ(0, state.n_bytes);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
