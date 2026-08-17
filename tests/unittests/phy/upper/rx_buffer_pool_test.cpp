// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "support/compare_sequences.h"
#include "ocudu/adt/detail/concurrent_queue_params.h"
#include "ocudu/phy/upper/log_likelihood_ratio.h"
#include "ocudu/phy/upper/rx_buffer_decoder_callback.h"
#include "ocudu/phy/upper/rx_buffer_pool.h"
#include "ocudu/phy/upper/unique_rx_buffer.h"
#include "ocudu/support/executors/task_worker_pool.h"
#include "ocudu/support/synchronization/sync_event.h"
#include <gtest/gtest.h>
#include <limits>

using namespace ocudu;

namespace ocudu {

/// \brief Tries to reserve buffers from a pool for a number of trials with a waiting time after every failed
/// reservation.
///
/// This function is intended for being used when it is expect that a buffer shall be available but due memory
/// synchronization it could potentially fail.
///
/// \param     pool           Receive buffer pool.
/// \param[in] slot           Slot context in which the reservation occurs.
/// \param[in] id             Buffer identifier.
/// \param[in] nof_codeblocks Indicates the number of codeblocks to reserve.
/// \param[in] new_data       Set to true if the transmission is for new data.
/// \return A valid receive buffer if the reservation is successful before exhausting the number trials. Otherwise, an
/// invalid buffer.
inline unique_rx_buffer reserve_buffer_trial(rx_buffer_pool&       pool,
                                             slot_point            slot,
                                             trx_buffer_identifier id,
                                             unsigned              nof_codeblocks,
                                             bool                  new_data)
{
  // Maximum number of trials.
  static constexpr unsigned max_nof_trials = 10;
  // Waiting time between failed reservations.
  static constexpr std::chrono::milliseconds wait_time_between_trials(100);

  for (unsigned i_trial = 0; i_trial != max_nof_trials; ++i_trial) {
    // Try to reserve buffer.
    unique_rx_buffer rx_buffer = pool.reserve(slot, id, nof_codeblocks, new_data);

    // Return a valid buffer if the reservation is successful.
    if (rx_buffer) {
      return rx_buffer;
    }

    // Wait time before trying again.
    std::this_thread::sleep_for(wait_time_between_trials);
  }

  // It did not reserve successfully a buffer. Return an invalid buffer.
  return unique_rx_buffer();
}

} // namespace ocudu

namespace {

/// Test decoder callback class.
class test_decoder_callback : private rx_buffer_decoder_callback
{
public:
  /// Gets the reference of the callback interface.
  rx_buffer_decoder_callback& get_callback()
  {
    sync_token = sync.get_token();
    return *this;
  }

  /// Waits for the callback to be called.
  void wait_callback() { sync.wait(); }

  /// Gets the last codeblock identifier.
  unsigned get_last_codeblock_id() const { return last_codeblock_id; }

private:
  // See the rx_buffer_decoder_callback interface for documentation.
  void codeblock_decode(unsigned codeblock_id) override
  {
    sync_token.reset();
    last_codeblock_id = codeblock_id;
  }

  sync_event        sync;
  scoped_sync_token sync_token;
  unsigned          last_codeblock_id = std::numeric_limits<unsigned>::max();
};

} // namespace

// Tests that the pool returns nullptr when the limit of buffers is reached.
TEST(rx_buffer_pool, buffer_limit)
{
  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 4;
  pool_config.nof_codeblocks       = 4;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Create as many buffers as the limit is set.
  std::vector<unique_rx_buffer> buffers;
  for (unsigned rnti = 0; rnti != pool_config.nof_buffers; ++rnti) {
    trx_buffer_identifier buffer_id(to_rnti(rnti), 0);

    // Reserve buffer, it shall not fail.
    buffers.emplace_back(pool->get_pool().reserve(slot, buffer_id, 1, true));
    ASSERT_TRUE(buffers.back());
  }

  // Create one more buffer. No buffers are available. It must fail to reserve.
  trx_buffer_identifier buffer_id(to_rnti(pool_config.nof_buffers), 0);
  ASSERT_FALSE(pool->get_pool().reserve(slot, buffer_id, 1, true));
}

// Tests that the pool returns nullptr when the limit of codeblocks is reached.
TEST(rx_buffer_pool, codeblock_limit)
{
  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 2;
  pool_config.nof_codeblocks       = 1;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Reserve buffer with all the codeblocks, it shall not fail.
  trx_buffer_identifier buffer_id0(to_rnti(0x1234), 0x3);
  unique_rx_buffer      buffer = pool->get_pool().reserve(slot, buffer_id0, pool_config.nof_codeblocks, true);
  ASSERT_TRUE(buffer);

  // Create one more buffer. No codeblocks are available. It must fail to reserve.
  trx_buffer_identifier buffer_id1(to_rnti(0x1234), buffer_id0.get_harq() + 1);
  ASSERT_FALSE(pool->get_pool().reserve(slot, buffer_id1, pool_config.nof_codeblocks, true));
}

// Tests that the pool frees reserved buffer.
TEST(rx_buffer_pool, buffer_free)
{
  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 1;
  pool_config.nof_codeblocks       = 1;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Reserve buffer with all the codeblocks, it shall not fail.
  trx_buffer_identifier buffer_id0(to_rnti(0x1234), 0x3);
  unique_rx_buffer      buffer = pool->get_pool().reserve(slot, buffer_id0, pool_config.nof_codeblocks, true);
  ASSERT_TRUE(buffer);

  // Extract CRCs.
  span<bool> crc = buffer->get_codeblocks_crc();

  // Check the CRC are all false.
  ASSERT_TRUE(std::find(crc.begin(), crc.end(), true) == crc.end());

  // Fill codeblocks with true.
  std::fill(crc.begin(), crc.end(), true);

  // Unlock buffer. It is still reserved.
  buffer.unlock();

  // Reserve buffer with the same identifier. It shall not fail.
  buffer = pool->get_pool().reserve(slot, buffer_id0, pool_config.nof_codeblocks, false);
  ASSERT_TRUE(buffer);

  // Reserve buffer with a different identifier. It shall fail.
  trx_buffer_identifier buffer_id1(to_rnti(0x1234), buffer_id0.get_harq() + 1);
  ASSERT_FALSE(pool->get_pool().reserve(slot, buffer_id1, pool_config.nof_codeblocks, true));

  // Extract CRCs.
  crc = buffer->get_codeblocks_crc();

  // Check the CRC are all true.
  ASSERT_TRUE(std::find(crc.begin(), crc.end(), false) == crc.end());

  // Free the first buffer identifier.
  buffer.release();

  // Run slot for clearing the buffer.
  pool->get_pool().run_slot(slot);

  // Reserve buffer with all the codeblocks, it shall not fail.
  buffer = pool->get_pool().reserve(slot, buffer_id0, pool_config.nof_codeblocks, true);
  ASSERT_TRUE(buffer);

  // Check the CRC are all false.
  ASSERT_TRUE(std::find(crc.begin(), crc.end(), true) == crc.end());
}

// Tests that the pool expires buffers after the last reserved slot.
TEST(rx_buffer_pool, buffer_expire)
{
  unsigned delay = 3;

  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 1;
  pool_config.nof_codeblocks       = 1;
  pool_config.expire_timeout_slots = 4;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Reserve buffer with all the codeblocks, it shall not fail.
  trx_buffer_identifier buffer_id0(to_rnti(0x1234), 0x3);
  ASSERT_TRUE(pool->get_pool().reserve(slot, buffer_id0, pool_config.nof_codeblocks, true));

  // Run slot and reserve the same buffer.
  slot += delay;
  pool->get_pool().run_slot(slot);
  ASSERT_TRUE(pool->get_pool().reserve(slot, buffer_id0, pool_config.nof_codeblocks, true));

  // Run for each slot until it expires.
  do {
    // Try to reserve another buffer. As there are no buffers available it shall fail.
    trx_buffer_identifier buffer_id1(to_rnti(0x1234), buffer_id0.get_harq() + 1);
    ASSERT_FALSE(pool->get_pool().reserve(slot, buffer_id1, pool_config.nof_codeblocks, true));
    ++slot;
    pool->get_pool().run_slot(slot);
  } while (slot.system_slot() < pool_config.expire_timeout_slots + delay);

  // After the first buffer expired, buffer reservation shall not fail.
  trx_buffer_identifier buffer_id2(to_rnti(0x1234), buffer_id0.get_harq() + 2);
  ASSERT_TRUE(reserve_buffer_trial(pool->get_pool(), slot, buffer_id2, pool_config.nof_codeblocks, true));
}

// Tests that the pool renews buffer expiration if they are locked.
TEST(rx_buffer_pool, buffer_renew_expire)
{
  unsigned expire_timeout_slots = 4;

  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 2;
  pool_config.nof_codeblocks       = 1;
  pool_config.expire_timeout_slots = expire_timeout_slots;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Reserve buffer with all the codeblocks, it shall not fail.
  trx_buffer_identifier buffer_id0(to_rnti(0x1234), 0x3);
  unique_rx_buffer      buffer = pool->get_pool().reserve(slot, buffer_id0, pool_config.nof_codeblocks, true);
  ASSERT_TRUE(buffer);

  // Advance slots. As the buffer is locked, the expiration shall be renewed.
  slot += expire_timeout_slots;
  pool->get_pool().run_slot(slot);

  // Try to get the same buffer. It must fail as the buffer is locked.
  unique_rx_buffer locked_buffer = pool->get_pool().reserve(slot, buffer_id0, pool_config.nof_codeblocks, true);
  ASSERT_FALSE(locked_buffer);

  // Unlock buffer.
  buffer.unlock();

  // Run for each slot until it expires.
  do {
    // Try to reserve another buffer. As there are no buffers available it shall fail.
    trx_buffer_identifier buffer_id1(to_rnti(0x1234), buffer_id0.get_harq() + 1);
    unique_rx_buffer      invalid_buffer = pool->get_pool().reserve(slot, buffer_id1, pool_config.nof_codeblocks, true);
    ASSERT_FALSE(invalid_buffer);
    ++slot;
    pool->get_pool().run_slot(slot);
  } while (slot.system_slot() < pool_config.expire_timeout_slots + expire_timeout_slots);

  // After the first buffer expired, buffer reservation shall not fail.
  trx_buffer_identifier buffer_id2(to_rnti(0x1234), buffer_id0.get_harq() + 2);
  ASSERT_TRUE(pool->get_pool().reserve(slot, buffer_id2, pool_config.nof_codeblocks, true));
}

// Tests that the pool renews buffer expiration if they are locked.
TEST(rx_buffer_pool, buffer_resize)
{
  static constexpr unsigned max_nof_codeblocks = 4;

  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 1;
  pool_config.nof_codeblocks       = max_nof_codeblocks;
  pool_config.expire_timeout_slots = 4;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Reserve buffer with max_nof_codeblocks - 1 codeblocks, it shall not fail.
  trx_buffer_identifier buffer_id0(to_rnti(0x1234), 0x3);
  unique_rx_buffer      buffer = pool->get_pool().reserve(slot, buffer_id0, max_nof_codeblocks, true);
  ASSERT_TRUE(buffer);

  // Check the number of codeblock matches.
  ASSERT_EQ(buffer.get().get_nof_codeblocks(), max_nof_codeblocks);

  // Set all CRC to true.
  span<bool> crc = buffer.get().get_codeblocks_crc();
  std::fill(crc.begin(), crc.end(), true);

  // Unlock the buffer.
  buffer.unlock();

  // Reserve the same buffer with more codeblocks.
  buffer = pool->get_pool().reserve(slot, buffer_id0, max_nof_codeblocks - 1, true);

  // Check the number of codeblock matches.
  ASSERT_EQ(buffer.get().get_nof_codeblocks(), max_nof_codeblocks - 1);

  // Check the CRC have been reset.
  std::array<bool, max_nof_codeblocks - 1> expected_crc = {false};
  {
    error_type<std::string> compare_result =
        compare_sequences(span<const bool>(buffer->get_codeblocks_crc()), span<const bool>(expected_crc));
    ASSERT_TRUE(compare_result.has_value()) << compare_result.error();
  }
}

// Tests that the pool returns an invalid buffer upon a retransmission with an incorrect number of CBs.
TEST(rx_buffer_pool, buffer_resize_false_retransmission)
{
  static constexpr unsigned nof_codeblocks = 4;

  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 1;
  pool_config.nof_codeblocks       = nof_codeblocks;
  pool_config.expire_timeout_slots = 4;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Reserve buffer with nof_codeblocks - 1 codeblocks, it shall not fail.
  trx_buffer_identifier buffer_id0(to_rnti(0x1234), 0x3);
  unique_rx_buffer      buffer = pool->get_pool().reserve(slot, buffer_id0, nof_codeblocks, true);
  ASSERT_TRUE(buffer);

  // Check the number of codeblock matches.
  ASSERT_EQ(buffer.get().get_nof_codeblocks(), nof_codeblocks);

  // Unlock the buffer.
  buffer.unlock();

  // Reserve the same buffer with less codeblocks, the buffer shall be invalid.
  ASSERT_FALSE(pool->get_pool().reserve(slot, buffer_id0, nof_codeblocks - 1, false));
}

// Tests that stresses concurrent reservation and release.
TEST(rx_buffer_pool, buffer_reserve_release_contention)
{
  static constexpr trx_buffer_identifier buffer_id(to_rnti(0x1234), 0);
  static constexpr unsigned              nof_repetitions = 32;
  static constexpr unsigned              nof_codeblocks  = 2;

  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 1;
  pool_config.nof_codeblocks       = nof_codeblocks + 1;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  slot_point slot(0, 0);

  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Reserve a buffer for each repetition and execute the decoding asynchronously.
  std::vector<std::thread> threads;
  threads.reserve(nof_repetitions);
  for (unsigned i_rep = 0; i_rep != nof_repetitions; ++i_rep) {
    // Reserve buffer from the main thread. Only the first reservation shall reset CRCs.
    unique_rx_buffer buffer = pool->get_pool().reserve(slot, buffer_id, nof_codeblocks, i_rep == 0);

    // Buffer reservation failure is acceptable.
    if (!buffer) {
      continue;
    }

    // Release buffer asynchronously.
    unique_task task = [local_buffer = std::move(buffer)]() mutable {
      // Release buffer.
      local_buffer.release();
    };

    // Create asynchronous thread.
    threads.emplace_back([local_task = std::move(task)] { local_task(); });
  }

  // Join threads.
  std::for_each(threads.rbegin(), threads.rend(), [](std::thread& thread) { thread.join(); });
}

// Tests that the pool returns an invalid buffer upon a retransmission without a previous reservation.
TEST(rx_buffer_pool, fresh_false_retransmission)
{
  static constexpr unsigned nof_codeblocks = 4;

  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 1;
  pool_config.nof_codeblocks       = nof_codeblocks;
  pool_config.expire_timeout_slots = 4;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Reserve buffer as retransmission without a previous reservation, the buffer shall be invalid.
  trx_buffer_identifier buffer_id0(to_rnti(0x1234), 0x3);
  unique_rx_buffer      buffer = pool->get_pool().reserve(slot, buffer_id0, nof_codeblocks, false);
  ASSERT_FALSE(buffer);
}

// Tests buffer soft bits contents persists between retransmissions.
TEST(rx_buffer_pool, buffer_contents)
{
  unsigned nof_cb_x_buffer = 2;
  unsigned cb_size         = 16;
  // Data size cannot be larger than cb_size / 3 (recall that 1/3 is the maximum coding rate).
  unsigned data_size = 5;

  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = cb_size;
  pool_config.nof_buffers          = 4;
  pool_config.nof_codeblocks       = pool_config.nof_buffers * nof_cb_x_buffer;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Create as many buffers as the limit is set.
  trx_buffer_identifier buffer_id(to_rnti(0x1234), 0x3);

  // Temporal storage of buffer codeblock information.
  std::vector<span<log_likelihood_ratio>> cb_soft_bits;
  std::vector<bit_buffer>                 cb_data_bits;

  // Note: two buffers with the same identifier cannot be simultaneously in scope.
  {
    // Reserve buffer, it shall not fail.
    unique_rx_buffer rm_buffer = pool->get_pool().reserve(slot, buffer_id, nof_cb_x_buffer, true);
    ASSERT_TRUE(rm_buffer);

    // For each codeblock...
    for (unsigned cb_id = 0; cb_id != nof_cb_x_buffer; ++cb_id) {
      // Get codeblock soft and data bits.
      span<log_likelihood_ratio> buffer      = rm_buffer.get().get_codeblock_soft_bits(cb_id, cb_size);
      bit_buffer                 data_buffer = rm_buffer.get().get_codeblock_data_bits(cb_id, data_size);

      cb_soft_bits.emplace_back(buffer);
      cb_data_bits.emplace_back(data_buffer);

      // Make sure size matches.
      ASSERT_TRUE(buffer.size() == cb_size);
      ASSERT_TRUE(data_buffer.size() == data_size);

      // Write data in codeblock.
      for (unsigned bit_idx = 0; bit_idx != cb_size; ++bit_idx) {
        int8_t data     = (cb_id << 4) | bit_idx;
        buffer[bit_idx] = data;
        if (bit_idx < data_size) {
          data_buffer.insert(bit_idx & 1U, bit_idx, 1);
        }
      }
    }
  }

  // Reserve buffer, it shall not fail.
  unique_rx_buffer buffer = pool->get_pool().reserve(slot, buffer_id, nof_cb_x_buffer, true);
  ASSERT_TRUE(buffer);

  // For each codeblock...
  for (unsigned cb_id = 0; cb_id != nof_cb_x_buffer; ++cb_id) {
    // Get codeblock soft bits.
    span<log_likelihood_ratio> buffer0      = cb_soft_bits[cb_id];
    span<log_likelihood_ratio> buffer1      = buffer.get().get_codeblock_soft_bits(cb_id, cb_size);
    bit_buffer                 data_buffer0 = cb_data_bits[cb_id];
    bit_buffer                 data_buffer1 = buffer.get().get_codeblock_data_bits(cb_id, data_size);

    // Make sure absolute codeblock indexes match.
    ASSERT_EQ(buffer.get().get_absolute_codeblock_id(cb_id), cb_id);

    // Make sure the data pointers match.
    ASSERT_TRUE(buffer0.data() == buffer1.data());
    ASSERT_TRUE(data_buffer0.get_buffer().data() == data_buffer1.get_buffer().data());

    // Validate data persists in the codeblock.
    for (unsigned bit_idx = 0; bit_idx != cb_size; ++bit_idx) {
      log_likelihood_ratio data = static_cast<int>((cb_id << 4) | bit_idx);
      ASSERT_EQ(buffer0[bit_idx], data);
      if (bit_idx < data_size) {
        ASSERT_EQ(data_buffer0.extract(bit_idx, 1), bit_idx & 1U);
      }
    }
  }
}

// Tests buffer pool fails to reserve after stopping.
TEST(rx_buffer_pool, reserve_after_stop)
{
  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 1;
  pool_config.nof_buffers          = 4;
  pool_config.nof_codeblocks       = pool_config.nof_buffers;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Stop the pool operation.
  pool->stop();

  // Try to reserve a buffer.
  unique_rx_buffer buffer =
      pool->get_pool().reserve(slot, trx_buffer_identifier(to_rnti(0), 0), pool_config.nof_codeblocks, true);

  // The buffer must be invalid.
  ASSERT_FALSE(buffer);
}

// Tests buffer pool waits to stop.
TEST(rx_buffer_pool, wait_to_stop)
{
  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 1;
  pool_config.nof_buffers          = 4;
  pool_config.nof_codeblocks       = pool_config.nof_buffers;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Try to reserve a buffer.
  unique_rx_buffer buffer =
      pool->get_pool().reserve(slot, trx_buffer_identifier(to_rnti(0), 0), pool_config.nof_codeblocks, true);

  // Create asynchronous task for unlocking the buffer.
  std::thread async_unlock([&buffer] {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    buffer.unlock();
  });

  // Stop the pool operation.
  pool->stop();

  // Make sure the asynchronous thread joined.
  async_unlock.join();

  // The buffer must be invalid.
  ASSERT_FALSE(buffer);
}

TEST(rx_buffer_pool, concurrent)
{
  unsigned nof_repetitions     = 100;
  unsigned nof_release_threads = 4;
  unsigned max_nof_buffers     = 16;
  unsigned nof_cb_x_buffer     = 8;
  unsigned cb_size             = 16;

  task_worker_pool<concurrent_queue_policy::lockfree_mpmc> release_worker_pool(
      "release", nof_release_threads, nof_repetitions * max_nof_buffers);

  // Create pool configuration for the test.
  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = cb_size;
  pool_config.nof_buffers          = max_nof_buffers;
  pool_config.nof_codeblocks       = max_nof_buffers * nof_cb_x_buffer;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  // Current slot.
  slot_point slot(0, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  for (unsigned i_repetition = 0; i_repetition != nof_repetitions; ++i_repetition) {
    for (unsigned i_buffer = 0; i_buffer != max_nof_buffers; ++i_buffer) {
      // Reserve buffer.
      unique_rx_buffer buffer =
          pool->get_pool().reserve(slot, trx_buffer_identifier(to_rnti(0x1234), i_buffer), nof_cb_x_buffer, true);

      // The reservation should be successful for the first time.
      ASSERT_TRUE((i_repetition > 0) || buffer);

      // Release or unlock buffer asynchronously in the worker pool.
      if (buffer) {
        if (i_repetition & 1) {
          ASSERT_TRUE(release_worker_pool.push_task([buffer2 = std::move(buffer)]() mutable { buffer2.unlock(); }));
        } else {
          ASSERT_TRUE(release_worker_pool.push_task([buffer2 = std::move(buffer)]() mutable { buffer2.release(); }));
        }
      }
    }

    // Process housekeeping asynchronously.
    pool->get_pool().run_slot(slot++);
  }

  // Wait for tasks to finish.
  release_worker_pool.wait_pending_tasks();

  // Stop workers before destroying them.
  release_worker_pool.stop();
}

// Tests decode_cb_in_sequence() in order.
TEST(rx_buffer_pool, decode_cb_in_sequence_ordered)
{
  static constexpr trx_buffer_identifier buffer_id(to_rnti(0x1234), 0);
  static constexpr unsigned              nof_repetitions = 4;
  static constexpr unsigned              nof_codeblocks  = 2;
  static constexpr unsigned              codeblock_id    = nof_codeblocks - 1;

  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 1;
  pool_config.nof_codeblocks       = nof_codeblocks + 1;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  slot_point slot(0, 0);

  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  // Reserve a buffer for each repetition.
  for (unsigned i_rep = 0; i_rep != nof_repetitions; ++i_rep) {
    // Reserve buffer from the main thread. Only the first reservation shall reset CRCs.
    unique_rx_buffer buffer = pool->get_pool().reserve(slot, buffer_id, nof_codeblocks, i_rep == 0);

    // Decoder callback.
    test_decoder_callback callback;

    // Request decode in sequence.
    buffer.decode_cb_in_sequence(codeblock_id, callback.get_callback());

    // Wait for the callback.
    callback.wait_callback();

    // Assert the codeblock identifier matches with the expected.
    ASSERT_EQ(codeblock_id, callback.get_last_codeblock_id());

    // Unlock buffer, it shall execute the next repetition.
    buffer.unlock();
  }
}

// Tests decode_cb_in_sequence() with unordered concurrent access.
TEST(rx_buffer_pool, decode_cb_in_sequence_concurrent)
{
  static constexpr trx_buffer_identifier buffer_id(to_rnti(0x1234), 0);
  static constexpr unsigned              nof_repetitions = 8;
  static constexpr unsigned              nof_codeblocks  = 2;
  static constexpr unsigned              codeblock_id    = nof_codeblocks - 1;

  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 1;
  pool_config.nof_codeblocks       = nof_codeblocks + 1;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  slot_point slot(0, 0);

  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  std::vector<unsigned> decode_sequence;
  decode_sequence.reserve(nof_repetitions);

  // Thread signaling for ensuring a deterministic unordered decode.
  std::vector<sync_event>        thread_order_sync(nof_repetitions);
  std::vector<scoped_sync_token> thread_order_token;
  thread_order_token.reserve(nof_repetitions);
  std::transform(thread_order_sync.begin(),
                 thread_order_sync.end(),
                 std::back_inserter(thread_order_token),
                 [](sync_event& sync) { return sync.get_token(); });

  // Reserve a buffer for each repetition and execute the decoding asynchronously.
  std::vector<std::thread> threads;
  threads.reserve(nof_repetitions);
  for (unsigned i_rep = 0; i_rep != nof_repetitions; ++i_rep) {
    // Reserve buffer from the main thread. Only the first reservation shall reset CRCs.
    unique_rx_buffer buffer = pool->get_pool().reserve(slot, buffer_id, nof_codeblocks, i_rep == 0);

    unique_task task =
        [&decode_sequence, &thread_order_sync, &thread_order_token, i_rep, local_buffer = std::move(buffer)]() mutable {
          // Wait for this thread turn to request execution.
          thread_order_sync[i_rep].wait();

          // Decoder callback.
          test_decoder_callback callback;

          // Request decode in sequence.
          local_buffer.decode_cb_in_sequence(codeblock_id, callback.get_callback());

          // Trigger another thread to decode in sequence.
          thread_order_token[((i_rep + 3) * 5) % nof_repetitions].reset();

          // Wait for the callback.
          callback.wait_callback();

          // Write the current repetition identifier in the list.
          decode_sequence.push_back(i_rep);

          // Unlock buffer, it shall execute the next repetition.
          local_buffer.unlock();
        };

    // Create asynchronous thread.
    threads.emplace_back([local_task = std::move(task)] { local_task(); });
  }

  // Trigger first thread to start.
  thread_order_token.front().reset();

  // Join threads.
  std::for_each(threads.rbegin(), threads.rend(), [](std::thread& thread) { thread.join(); });

  // Generate expected sequence.
  std::vector<unsigned> expected_sequence(nof_repetitions);
  std::iota(expected_sequence.begin(), expected_sequence.end(), 0);

  // Make sure the decode sequence matches the expected.
  ASSERT_EQ(decode_sequence, expected_sequence);
}

// Test that covers the external_soft_bits=true constructor path.
TEST(rx_buffer_pool, external_soft_bits)
{
  static constexpr unsigned nof_codeblocks = 2;

  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 1;
  pool_config.nof_codeblocks       = nof_codeblocks + 1;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = true;

  slot_point slot(0, 0);

  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  trx_buffer_identifier buffer_id(to_rnti(0x1234), 0);
  unique_rx_buffer      buffer = pool->get_pool().reserve(slot, buffer_id, nof_codeblocks, true);
  ASSERT_TRUE(buffer);

  ASSERT_EQ(buffer->get_nof_codeblocks(), nof_codeblocks);
}

// Test that exercises the on_release().
TEST(rx_buffer_pool, multi_scope_release)
{
  static constexpr unsigned nof_codeblocks = 2;

  rx_buffer_pool_config pool_config;
  pool_config.max_codeblock_size   = 16;
  pool_config.nof_buffers          = 1;
  pool_config.nof_codeblocks       = nof_codeblocks + 1;
  pool_config.expire_timeout_slots = 10;
  pool_config.external_soft_bits   = false;

  slot_point slot(0, 0);

  std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
  ASSERT_TRUE(pool);

  trx_buffer_identifier buffer_id(to_rnti(0x1234), 0);

  // First reservation.
  unique_rx_buffer buffer1 = pool->get_pool().reserve(slot, buffer_id, nof_codeblocks, true);
  ASSERT_TRUE(buffer1);

  // Second reservation. The codeblock count must match for retransmissions.
  unique_rx_buffer buffer2 = pool->get_pool().reserve(slot, buffer_id, nof_codeblocks, false);
  ASSERT_TRUE(buffer2);

  // Release buffer1.
  buffer1.release();

  // Release buffer2.
  buffer2.release();

  // Run slot to process the release and free codeblocks.
  pool->get_pool().run_slot(slot);
}

int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);

  // Make sure logger is enabled and use /dev/null as sink.
  ocudulog::set_default_sink(*ocudulog::create_file_sink("/dev/null"));
  ocudulog::init();
  ocudulog::basic_logger& logger = ocudulog::fetch_basic_logger("PHY", true);
  logger.set_level(ocudulog::basic_levels::debug);

  int ret = RUN_ALL_TESTS();

  ocudulog::flush();

  return ret;
}
