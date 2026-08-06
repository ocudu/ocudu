// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_difi_context_packet.h"
#include <cstring>
#include <gtest/gtest.h>

using namespace ocudu;

// ---- Helpers ----------------------------------------------------------------

static uint32_t read_u32_be(const uint8_t* p)
{
  return (static_cast<uint32_t>(p[0]) << 24) | (static_cast<uint32_t>(p[1]) << 16) |
         (static_cast<uint32_t>(p[2]) << 8) | static_cast<uint32_t>(p[3]);
}

static uint64_t read_u64_be(const uint8_t* p)
{
  return (static_cast<uint64_t>(read_u32_be(p)) << 32) | read_u32_be(p + 4);
}

static int64_t read_i64_be(const uint8_t* p)
{
  return static_cast<int64_t>(read_u64_be(p));
}

/// Minimal valid params used as a base for most tests.
static difi_context_packet_params make_params()
{
  difi_context_packet_params p{};
  p.stream_id      = 0xdeadbeefU;
  p.full_secs      = 1740000000U;     // arbitrary Unix timestamp
  p.frac_ps        = 500000000000ULL; // 0.5 s in picoseconds
  p.sample_rate_Hz = 1920000.0;
  p.center_freq_Hz = 3.5e9;
  p.bit_depth      = 16;
  p.ctx_pkt_n      = 0;
  return p;
}

// ---- Tests ------------------------------------------------------------------

TEST(DifiContextPacket, PacketSizeIs108Bytes)
{
  using namespace units::literals;

  EXPECT_EQ(DIFI_CONTEXT_PACKET_SIZE, 108_bytes);
}

TEST(DifiContextPacket, HeaderWordPktN0)
{
  // type=0x4, static bits = 0x49e00000, pkt_n=0, size=27=0x1b
  // Expected: 0x49e0001b
  auto p      = make_params();
  p.ctx_pkt_n = 0;

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  const uint32_t header   = read_u32_be(buf + 0);
  const uint32_t expected = 0x49e00000U | (0U << 16) | 27U;
  EXPECT_EQ(header, expected);
}

TEST(DifiContextPacket, HeaderWordPktN7)
{
  auto p      = make_params();
  p.ctx_pkt_n = 7;

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  const uint32_t header   = read_u32_be(buf + 0);
  const uint32_t expected = 0x49e00000U | (7U << 16) | 27U;
  EXPECT_EQ(header, expected);
}

TEST(DifiContextPacket, PktNWrapsAt16)
{
  // ctx_pkt_n is masked to 4 bits, so 17 wraps to 1.
  auto p      = make_params();
  p.ctx_pkt_n = 17; // 17 & 0xf = 1

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  const uint32_t header = read_u32_be(buf + 0);
  EXPECT_EQ((header >> 16) & 0xfU, 1U);
}

TEST(DifiContextPacket, StreamId)
{
  const auto p = make_params();

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  EXPECT_EQ(read_u32_be(buf + 4), p.stream_id);
}

TEST(DifiContextPacket, ClassId)
{
  const auto p = make_params();

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  // Upper 32-bit word: OUI = 0x006a621e; lower 32-bit word: device_class = 0.
  EXPECT_EQ(read_u32_be(buf + 8), 0x006a621eU);
  EXPECT_EQ(read_u32_be(buf + 12), 0U);
}

TEST(DifiContextPacket, TimestampFields)
{
  const auto p = make_params();

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  EXPECT_EQ(read_u32_be(buf + 16), p.full_secs);
  EXPECT_EQ(read_u64_be(buf + 20), p.frac_ps);
}

TEST(DifiContextPacket, Cif0IsZero)
{
  const auto p = make_params();

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  EXPECT_EQ(read_u32_be(buf + 28), 0U);
}

TEST(DifiContextPacket, RefPoint)
{
  const auto p = make_params();

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  EXPECT_EQ(read_u32_be(buf + 32), 0xfbb98000U);
}

TEST(DifiContextPacket, BandwidthEqualsFixedPointSampleRate)
{
  const auto p = make_params();

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  const uint64_t expected = static_cast<uint64_t>(p.sample_rate_Hz * (1 << 20));
  EXPECT_EQ(read_u64_be(buf + 36), expected);
}

TEST(DifiContextPacket, IfRefFreqIsZero)
{
  const auto p = make_params();

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  EXPECT_EQ(read_i64_be(buf + 44), 0LL);
}

TEST(DifiContextPacket, RfRefFreqEncoding)
{
  auto p           = make_params();
  p.center_freq_Hz = 3.5e9;

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  const int64_t expected = static_cast<int64_t>(p.center_freq_Hz * (1 << 20));
  EXPECT_EQ(read_i64_be(buf + 52), expected);
}

TEST(DifiContextPacket, RfRefFreqNegativeIsSignedEncoded)
{
  // Negative RF freq should round-trip through int64_t fixed-point encoding.
  auto p           = make_params();
  p.center_freq_Hz = -1.0e6;

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  const int64_t expected = static_cast<int64_t>(p.center_freq_Hz * (1 << 20));
  EXPECT_LT(expected, 0LL);
  EXPECT_EQ(read_i64_be(buf + 52), expected);
}

TEST(DifiContextPacket, SampleRateEncoding)
{
  auto p           = make_params();
  p.sample_rate_Hz = 7680000.0;

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  const uint64_t expected = static_cast<uint64_t>(p.sample_rate_Hz * (1 << 20));
  EXPECT_EQ(read_u64_be(buf + 76), expected);
}

TEST(DifiContextPacket, TimestampAdj10Us)
{
  const auto p = make_params();

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  // 10 µs = 10,000,000,000 femtoseconds.
  EXPECT_EQ(read_u64_be(buf + 84), 10000000000ULL);
}

TEST(DifiContextPacket, TimestampCalMirrorsFullSecs)
{
  const auto p = make_params();

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  EXPECT_EQ(read_u32_be(buf + 92), p.full_secs);
}

TEST(DifiContextPacket, StateAndEvent)
{
  const auto p = make_params();

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  EXPECT_EQ(read_u32_be(buf + 96), 0x9ff00000U);
}

TEST(DifiContextPacket, PayloadFormat16Bit)
{
  auto p      = make_params();
  p.bit_depth = 16;

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  EXPECT_EQ(read_u64_be(buf + 100), 0xa00007cf00000000ULL);
}

TEST(DifiContextPacket, PayloadFormat8Bit)
{
  auto p      = make_params();
  p.bit_depth = 8;

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  EXPECT_EQ(read_u64_be(buf + 100), 0xa00003c700000000ULL);
}

TEST(DifiContextPacket, GainsDefaultZero)
{
  const auto p = make_params(); // rf_gain_dB and if_gain_dB are zero-initialised

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  EXPECT_EQ(read_u32_be(buf + 72), 0U);
}

TEST(DifiContextPacket, RfGainEncoding)
{
  auto p       = make_params();
  p.rf_gain_dB = 6.0; // 6 * 128 = 768 = 0x0300
  p.if_gain_dB = 0.0;

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  // Lower 16 bits = rf_gain * 128; upper 16 bits = if_gain * 128 = 0.
  const uint32_t gains     = read_u32_be(buf + 72);
  const int16_t  rf_actual = static_cast<int16_t>(gains & 0xffffU);
  EXPECT_EQ(rf_actual, static_cast<int16_t>(6.0 * 128));
  EXPECT_EQ(gains >> 16, 0U);
}

TEST(DifiContextPacket, IfGainEncoding)
{
  auto p       = make_params();
  p.rf_gain_dB = 0.0;
  p.if_gain_dB = 3.0; // 3 * 128 = 384 = 0x0180

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value()];
  build_difi_context_packet(buf, p);

  // Upper 16 bits = if_gain * 128; lower 16 bits = rf_gain * 128 = 0.
  const uint32_t gains     = read_u32_be(buf + 72);
  const int16_t  if_actual = static_cast<int16_t>(gains >> 16);
  EXPECT_EQ(if_actual, static_cast<int16_t>(3.0 * 128));
  EXPECT_EQ(gains & 0xffffU, 0U);
}

#ifdef ASSERTS_ENABLED

TEST(DifiContextPacket, BufferSmallerThanPacketAsserts)
{
  const auto p = make_params();

  uint8_t buf[DIFI_CONTEXT_PACKET_SIZE.value() - 1];
  ASSERT_DEATH(build_difi_context_packet(buf, p), "too small");
}

#endif // ASSERTS_ENABLED
