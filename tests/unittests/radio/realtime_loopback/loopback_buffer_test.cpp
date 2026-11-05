// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "loopback_buffer.h"
#include "ocudu/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include <gtest/gtest.h>
#include <random>

using namespace ocudu;

namespace {

// Parameters:
// - Number of channels;
// - Block size in samples.
using loopback_buffer_test_parameters = std::tuple<unsigned, unsigned>;

class LoopbackBufferFixture : public ::testing::TestWithParam<loopback_buffer_test_parameters>
{
protected:
  static constexpr unsigned buffer_size = 1000;

  static void SetUpTestSuite()
  {
    ocudulog::init();
    ocudulog::fetch_basic_logger("RF").set_level(ocudulog::basic_levels::error);
  }

  void SetUp() override
  {
    nof_channels = std::get<0>(GetParam());
    block_size   = std::get<1>(GetParam());
  }

  // Fills the buffer with samples that encode the channel and the timestamp of each sample.
  void fill(baseband_gateway_buffer_writer& data, baseband_gateway_timestamp timestamp) const
  {
    for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
      span<ci16_t> ch = data.get_channel_buffer(i_channel);
      for (unsigned i = 0; i != ch.size(); ++i) {
        ch[i] = expected(i_channel, timestamp + i);
      }
    }
  }

  // Fills the buffer with a non-zero pattern that never matches a written sample.
  void fill_garbage(baseband_gateway_buffer_writer& data) const
  {
    for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
      for (ci16_t& sample : data.get_channel_buffer(i_channel)) {
        sample = {-1, -1};
      }
    }
  }

  static ci16_t expected(unsigned i_channel, baseband_gateway_timestamp timestamp)
  {
    return {static_cast<int16_t>(i_channel + 1), static_cast<int16_t>(timestamp % 30000)};
  }

  // Checks that samples [timestamp, timestamp + nof_valid) match the written pattern and the rest are zero.
  void check(const baseband_gateway_buffer_reader& data, baseband_gateway_timestamp timestamp, unsigned nof_valid) const
  {
    for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
      span<const ci16_t> ch = data.get_channel_buffer(i_channel);
      for (unsigned i = 0; i != ch.size(); ++i) {
        ci16_t ref = (i < nof_valid) ? expected(i_channel, timestamp + i) : ci16_t{0, 0};
        ASSERT_EQ(ch[i], ref) << "channel=" << i_channel << " ts=" << timestamp + i;
      }
    }
  }

  unsigned nof_channels;
  unsigned block_size;
};

} // namespace

// Writes and reads at timestamp zero.
TEST_P(LoopbackBufferFixture, FirstWriteAtTimestampZero)
{
  loopback_buffer                 buffer(nof_channels, buffer_size);
  baseband_gateway_buffer_dynamic tx(nof_channels, block_size);
  baseband_gateway_buffer_dynamic rx(nof_channels, block_size);

  fill(tx.get_writer(), 0);
  ASSERT_TRUE(buffer.write(tx.get_reader(), 0));
  ASSERT_EQ(buffer.read(rx.get_writer(), 0), block_size);
  check(rx.get_reader(), 0, block_size);
}

// Streams several buffer laps, so that the blocks wrap around the end of the circular buffer at different offsets.
TEST_P(LoopbackBufferFixture, WrapAround)
{
  loopback_buffer                 buffer(nof_channels, buffer_size);
  baseband_gateway_buffer_dynamic tx(nof_channels, block_size);
  baseband_gateway_buffer_dynamic rx(nof_channels, block_size);

  baseband_gateway_timestamp start = 12345;
  buffer.set_first_expected_rx_timestamp(start);
  for (baseband_gateway_timestamp ts = start; ts < start + 5 * buffer_size; ts += block_size) {
    fill(tx.get_writer(), ts);
    ASSERT_TRUE(buffer.write(tx.get_reader(), ts));
    fill_garbage(rx.get_writer());
    ASSERT_EQ(buffer.read(rx.get_writer(), ts), block_size);
    check(rx.get_reader(), ts, block_size);
  }
}

// Reading samples that have not been written returns zeros instead of leaving the output untouched.
TEST_P(LoopbackBufferFixture, ReadAheadOfTransmitterReturnsZeros)
{
  loopback_buffer                 buffer(nof_channels, buffer_size);
  baseband_gateway_buffer_dynamic rx(nof_channels, block_size);

  fill_garbage(rx.get_writer());
  ASSERT_EQ(buffer.read(rx.get_writer(), 0), 0);
  check(rx.get_reader(), 0, 0);

  // The reading position advances, so a later read of the same samples is in the past.
  fill_garbage(rx.get_writer());
  ASSERT_EQ(buffer.read(rx.get_writer(), block_size), 0);
  check(rx.get_reader(), block_size, 0);
}

// Reading a block that is only partially written copies the written part and zeroes the rest.
TEST_P(LoopbackBufferFixture, PartialRead)
{
  loopback_buffer                 buffer(nof_channels, buffer_size);
  unsigned                        nof_written = std::max(1U, block_size / 2);
  baseband_gateway_buffer_dynamic tx(nof_channels, nof_written);
  baseband_gateway_buffer_dynamic rx(nof_channels, block_size);

  baseband_gateway_timestamp start = 777;
  buffer.set_first_expected_rx_timestamp(start);
  fill(tx.get_writer(), start);
  ASSERT_TRUE(buffer.write(tx.get_reader(), start));
  fill_garbage(rx.get_writer());
  ASSERT_EQ(buffer.read(rx.get_writer(), start), nof_written);
  check(rx.get_reader(), start, nof_written);
}

// Samples skipped by the transmitter read as zeros, even after the buffer has been filled in previous laps.
TEST_P(LoopbackBufferFixture, TransmitGapReadsZeros)
{
  loopback_buffer                 buffer(nof_channels, buffer_size);
  baseband_gateway_buffer_dynamic tx(nof_channels, block_size);
  baseband_gateway_buffer_dynamic rx(nof_channels, block_size);

  // Fill the whole buffer several times.
  baseband_gateway_timestamp ts = 0;
  for (; ts < 3 * buffer_size; ts += block_size) {
    fill(tx.get_writer(), ts);
    ASSERT_TRUE(buffer.write(tx.get_reader(), ts));
    ASSERT_EQ(buffer.read(rx.get_writer(), ts), block_size);
  }

  // Skip a gap of samples on transmission and write the next block after it.
  unsigned                   gap       = std::min(buffer_size - block_size, 3 * block_size);
  baseband_gateway_timestamp tx_resume = ts + gap;
  fill(tx.get_writer(), tx_resume);
  ASSERT_TRUE(buffer.write(tx.get_reader(), tx_resume));

  // The gap reads as zeros.
  for (; ts + block_size <= tx_resume; ts += block_size) {
    fill_garbage(rx.get_writer());
    ASSERT_EQ(buffer.read(rx.get_writer(), ts), block_size);
    check(rx.get_reader(), 0, 0);
  }

  // The block after the gap is intact.
  baseband_gateway_buffer_dynamic rx_tail(nof_channels, tx_resume + block_size - ts);
  ASSERT_EQ(buffer.read(rx_tail.get_writer(), ts), rx_tail.get_nof_samples());
  for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
    span<const ci16_t> ch = rx_tail.get_reader().get_channel_buffer(i_channel);
    for (unsigned i = 0; i != ch.size(); ++i) {
      ci16_t ref = (ts + i < tx_resume) ? ci16_t{0, 0} : expected(i_channel, ts + i);
      ASSERT_EQ(ch[i], ref) << "channel=" << i_channel << " ts=" << ts + i;
    }
  }
}

// A write that would overwrite unread samples is dropped without corrupting them, and the stream recovers.
TEST_P(LoopbackBufferFixture, OverwriteUnreadSamplesIsDropped)
{
  loopback_buffer                 buffer(nof_channels, buffer_size);
  baseband_gateway_buffer_dynamic tx(nof_channels, block_size);
  baseband_gateway_buffer_dynamic rx(nof_channels, block_size);

  // Write a first block that stays unread.
  fill(tx.get_writer(), 0);
  ASSERT_TRUE(buffer.write(tx.get_reader(), 0));

  // A block written at this timestamp ends past the capacity left by the first unread sample.
  baseband_gateway_timestamp overflow_ts = std::max(block_size, buffer_size - block_size + 1);
  fill(tx.get_writer(), overflow_ts);
  ASSERT_FALSE(buffer.write(tx.get_reader(), overflow_ts));

  // A block that ends exactly at the buffer capacity is accepted.
  baseband_gateway_timestamp limit_ts = buffer_size - block_size;
  if (limit_ts >= block_size) {
    fill(tx.get_writer(), limit_ts);
    ASSERT_TRUE(buffer.write(tx.get_reader(), limit_ts));
  }

  // The unread block is intact.
  ASSERT_EQ(buffer.read(rx.get_writer(), 0), block_size);
  check(rx.get_reader(), 0, block_size);

  // After the receiver advances, a write in the dropped region succeeds.
  baseband_gateway_timestamp next_ts = std::max<baseband_gateway_timestamp>(buffer_size, overflow_ts);
  fill(tx.get_writer(), next_ts);
  ASSERT_TRUE(buffer.write(tx.get_reader(), next_ts));
  ASSERT_EQ(buffer.read(rx.get_writer(), next_ts), block_size);
  check(rx.get_reader(), next_ts, block_size);
}

// Misusing the buffer is a fatal error.
TEST(LoopbackBufferDeathTest, Misuse)
{
  // Forking is unsafe while the logger backend thread runs.
  GTEST_FLAG_SET(death_test_style, "threadsafe");

  static constexpr unsigned nof_channels = 2;
  static constexpr unsigned buffer_size  = 100;

  baseband_gateway_buffer_dynamic block(nof_channels, 10);
  baseband_gateway_buffer_dynamic too_long(nof_channels, buffer_size + 1);
  baseband_gateway_buffer_dynamic wrong_channels(nof_channels + 1, 10);

#ifdef ASSERTS_ENABLED
  // Asserts are compiled out unless ASSERTS_ENABLED is defined.
  ASSERT_DEATH(
      {
        loopback_buffer buffer(nof_channels, buffer_size);
        buffer.write(too_long.get_reader(), 0);
      },
      "Buffer size is too small");
  ASSERT_DEATH(
      {
        loopback_buffer buffer(nof_channels, buffer_size);
        buffer.write(wrong_channels.get_reader(), 0);
      },
      "Number of channels");
  ASSERT_DEATH(
      {
        loopback_buffer buffer(nof_channels, buffer_size);
        buffer.read(too_long.get_writer(), 0);
      },
      "Buffer size is too small");
  ASSERT_DEATH(
      {
        loopback_buffer buffer(nof_channels, buffer_size);
        buffer.read(wrong_channels.get_writer(), 0);
      },
      "Number of channels");
#endif // ASSERTS_ENABLED
  ASSERT_DEATH(
      {
        loopback_buffer buffer(nof_channels, buffer_size);
        buffer.write(block.get_reader(), 20);
        buffer.write(block.get_reader(), 25);
      },
      "in the past");
  ASSERT_DEATH(
      {
        loopback_buffer buffer(nof_channels, buffer_size);
        buffer.read(block.get_writer(), 20);
        buffer.read(block.get_writer(), 25);
      },
      "in the past");
  ASSERT_DEATH(
      {
        loopback_buffer buffer(nof_channels, buffer_size);
        buffer.write(block.get_reader(), 0);
        buffer.set_first_expected_rx_timestamp(5);
      },
      "before any sample is written");
  ASSERT_DEATH(
      {
        loopback_buffer buffer(nof_channels, buffer_size);
        buffer.set_first_expected_rx_timestamp(5);
        buffer.set_first_expected_rx_timestamp(5);
      },
      "only once");
}

INSTANTIATE_TEST_SUITE_P(LoopbackBufferTest,
                         LoopbackBufferFixture,
                         ::testing::Combine(::testing::Values(1, 4), ::testing::Values(1, 37, 250, 1000)));
