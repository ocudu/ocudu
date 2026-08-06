// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "radio_difi_data_packet.h"
#include "ocudu/adt/to_array.h"
#include <cstring>
#include <gtest/gtest.h>
#include <vector>

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

/// Reads one IQ component from the payload. Unlike every metadata field, the 16-bit IQ payload is
/// in host byte order per the DIFI convention, so no swap is applied here.
static int16_t read_i16_iq(const uint8_t* p)
{
  int16_t v = 0;
  std::memcpy(&v, p, 2);
  return v;
}

static difi_data_packet_params make_params(unsigned bit_depth = 16, uint8_t pkt_n = 0)
{
  difi_data_packet_params p{};
  p.stream_id = 0xcafebabeU;
  p.full_secs = 1740100000U;
  p.frac_ps   = 250000000000ULL; // 0.25 s
  p.bit_depth = bit_depth;
  p.pkt_n     = pkt_n;
  return p;
}

// ---- Tests ------------------------------------------------------------------

TEST(DifiDataPacket, SizeFor16BitSamples)
{
  using namespace units::literals;

  // 28-byte header + 4 bytes/sample, always 4-byte aligned.
  EXPECT_EQ(difi_data_packet_size(16, 0), 28_bytes);
  EXPECT_EQ(difi_data_packet_size(16, 1), 32_bytes);
  EXPECT_EQ(difi_data_packet_size(16, 3), 40_bytes);
}

TEST(DifiDataPacket, SizeFor8BitSamples)
{
  using namespace units::literals;

  // 28-byte header + 2 bytes/sample, padded to 4-byte boundary.
  EXPECT_EQ(difi_data_packet_size(8, 0), 28_bytes);           // no payload
  EXPECT_EQ(difi_data_packet_size(8, 2), 32_bytes);           // 4 bytes payload, aligned
  EXPECT_EQ(difi_data_packet_size(8, 3), 34_bytes + 2_bytes); // 6 bytes → padded to 8
}

TEST(DifiDataPacket, HeaderWordPktN0)
{
  // DATA_STATIC_BITS = 0x18e00000, pkt_n=0, size_words = (28+0)/4 = 7
  const auto           p = make_params(16, 0);
  std::vector<uint8_t> buf(difi_data_packet_size(16, 0).value());
  build_difi_data_packet(buf, p, {});

  const uint32_t header   = read_u32_be(buf.data());
  const uint32_t expected = 0x18e00000U | (0U << 16) | 7U;
  EXPECT_EQ(header, expected);
}

TEST(DifiDataPacket, HeaderWordPktN5With4Samples)
{
  const auto           p   = make_params(16, 5);
  const unsigned       nof = 4;
  std::vector<ci16_t>  samples(nof, ci16_t(0, 0));
  std::vector<uint8_t> buf(difi_data_packet_size(16, nof).value());
  build_difi_data_packet(buf, p, samples);

  // packet = 28 + 16 = 44 bytes = 11 words.
  const uint32_t header   = read_u32_be(buf.data());
  const uint32_t expected = 0x18e00000U | (5U << 16) | 11U;
  EXPECT_EQ(header, expected);
}

TEST(DifiDataPacket, PktNWrapsAt16)
{
  const auto           p = make_params(16, 18); // 18 & 0xf = 2
  std::vector<uint8_t> buf(difi_data_packet_size(16, 0).value());
  build_difi_data_packet(buf, p, {});

  const uint32_t header = read_u32_be(buf.data());
  EXPECT_EQ((header >> 16) & 0xfU, 2U);
}

TEST(DifiDataPacket, StreamId)
{
  const auto           p = make_params();
  std::vector<uint8_t> buf(difi_data_packet_size(16, 0).value());
  build_difi_data_packet(buf, p, {});

  EXPECT_EQ(read_u32_be(buf.data() + 4), p.stream_id);
}

TEST(DifiDataPacket, ClassIdContainsDifiOui)
{
  const auto           p = make_params();
  std::vector<uint8_t> buf(difi_data_packet_size(16, 0).value());
  build_difi_data_packet(buf, p, {});

  // Upper 32-bit word: OUI = 0x006a621e; lower 32-bit word: device_class = 0.
  EXPECT_EQ(read_u32_be(buf.data() + 8), 0x006a621eU);
  EXPECT_EQ(read_u32_be(buf.data() + 12), 0U);
}

TEST(DifiDataPacket, TimestampFields)
{
  const auto           p = make_params();
  std::vector<uint8_t> buf(difi_data_packet_size(16, 0).value());
  build_difi_data_packet(buf, p, {});

  EXPECT_EQ(read_u32_be(buf.data() + 16), p.full_secs);
  EXPECT_EQ(read_u64_be(buf.data() + 20), p.frac_ps);
}

TEST(DifiDataPacket, Iq16BitSingleSample)
{
  const auto           p = make_params(16);
  const ci16_t         sample(0x1234, static_cast<int16_t>(0xabcd));
  std::vector<uint8_t> buf(difi_data_packet_size(16, 1).value());
  build_difi_data_packet(buf, p, span<const ci16_t>(&sample, 1));

  // Offset 28: host-order I, then host-order Q.
  EXPECT_EQ(read_i16_iq(buf.data() + 28), 0x1234);
  EXPECT_EQ(read_i16_iq(buf.data() + 30), static_cast<int16_t>(0xabcd));
}

TEST(DifiDataPacket, Iq16BitMultipleSamples)
{
  const auto                p       = make_params(16);
  const std::vector<ci16_t> samples = {ci16_t(100, -200), ci16_t(300, -400)};
  std::vector<uint8_t>      buf(difi_data_packet_size(16, samples.size()).value());
  build_difi_data_packet(buf, p, samples);

  EXPECT_EQ(read_i16_iq(buf.data() + 28), 100);
  EXPECT_EQ(read_i16_iq(buf.data() + 30), -200);
  EXPECT_EQ(read_i16_iq(buf.data() + 32), 300);
  EXPECT_EQ(read_i16_iq(buf.data() + 34), -400);
}

TEST(DifiDataPacket, Iq8BitSingleSample)
{
  // 8-bit: upper byte of int16 is the int8 sample.
  // Sample (0x1200, 0xab00) → packed bytes 0x12, 0xab.
  const auto           p = make_params(8);
  const ci16_t         sample(0x1200, static_cast<int16_t>(0xab00));
  std::vector<uint8_t> buf(difi_data_packet_size(8, 1).value());
  build_difi_data_packet(buf, p, span<const ci16_t>(&sample, 1));

  EXPECT_EQ(buf[28], 0x12U);
  EXPECT_EQ(buf[29], 0xabU);
}

TEST(DifiDataPacket, Iq8BitOddSampleCountPaddedToWordBoundary)
{
  // 3 samples × 2 bytes = 6 bytes; padded to 8 bytes → total packet = 36 bytes.
  const auto           p = make_params(8);
  std::vector<ci16_t>  samples(3, ci16_t(0, 0));
  const units::bytes   expected_total = difi_data_packet_size(8, 3);
  std::vector<uint8_t> buf(expected_total.value());
  const units::bytes   actual = build_difi_data_packet(buf, p, samples);

  EXPECT_EQ(actual, expected_total);
  EXPECT_EQ(actual.value() % 4, 0U);
  // Padding bytes are zero.
  EXPECT_EQ(buf[34], 0U);
  EXPECT_EQ(buf[35], 0U);
}

#ifdef ASSERTS_ENABLED

TEST(DifiDataPacket, BufferSmallerThanPacketAsserts)
{
  const auto          p = make_params(16);
  std::vector<ci16_t> samples(4, ci16_t(0, 0));
  // One byte short of the 44 bytes the packet needs.
  std::vector<uint8_t> buf(difi_data_packet_size(16, 4).value() - 1);

  ASSERT_DEATH(build_difi_data_packet(buf, p, samples), "too small");
}

TEST(DifiDataPacket, PacketLargerThanSizeFieldAsserts)
{
  // The 16-bit size field tops out at 65535 words; 65529 samples of 16-bit IQ needs 65536.
  const auto           p   = make_params(16);
  const unsigned       nof = 65529;
  std::vector<ci16_t>  samples(nof, ci16_t(0, 0));
  std::vector<uint8_t> buf(difi_data_packet_size(16, nof).value());

  ASSERT_DEATH(build_difi_data_packet(buf, p, samples), "exceeds");
}

#endif // ASSERTS_ENABLED

// ---- Timestamp conversion ---------------------------------------------------

TEST(DifiTimestamp, TicksRoundTripAtEveryStandardRate)
{
  // Every 3GPP sample rate in the split-8 range. The conversion must be exact at all of them,
  // because a single tick of error reads as a lost sample at the receiver.
  static constexpr auto rates = to_array<double>({1920000.0,
                                                  3840000.0,
                                                  5760000.0,
                                                  7680000.0,
                                                  11520000.0,
                                                  15360000.0,
                                                  23040000.0,
                                                  30720000.0,
                                                  46080000.0,
                                                  61440000.0,
                                                  92160000.0});

  for (const double srate : rates) {
    const auto srate_ticks = static_cast<uint64_t>(srate);
    // Sub-second offsets, whole seconds, and a slot boundary part-way through a second.
    const uint64_t ticks[] = {0,
                              1,
                              1920,
                              srate_ticks / 2,
                              srate_ticks - 1,
                              srate_ticks,
                              srate_ticks + 1920,
                              7 * srate_ticks + 11520,
                              3600 * srate_ticks + 1};

    for (uint64_t t : ticks) {
      uint32_t full_secs = 0;
      uint64_t frac_ps   = 0;
      difi_ticks_to_time(t, srate, full_secs, frac_ps);

      EXPECT_LT(frac_ps, 1000000000000ULL) << "frac_ps out of range at " << srate << " Hz, tick " << t;
      EXPECT_EQ(full_secs, t / srate_ticks) << "Wrong seconds at " << srate << " Hz, tick " << t;
      EXPECT_EQ(difi_time_to_ticks(full_secs, frac_ps, srate), t)
          << "Round-trip lost precision at " << srate << " Hz, tick " << t;
    }
  }
}

TEST(DifiTimestamp, ZeroSampleRateYieldsZero)
{
  uint32_t full_secs = 0xffffffffU;
  uint64_t frac_ps   = 0xffffffffffffffffULL;
  difi_ticks_to_time(1234, 0.0, full_secs, frac_ps);

  EXPECT_EQ(full_secs, 0U);
  EXPECT_EQ(frac_ps, 0ULL);
  EXPECT_EQ(difi_time_to_ticks(5, 5000, 0.0), 0ULL);
}
