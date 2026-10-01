// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/support/units.h"
#include <gtest/gtest.h>

using namespace ocudu;

TEST(bit_units, basic)
{
  using namespace units::literals;

  units::bits a = 2_bits;
  ASSERT_EQ(a.value(), 2);
}

TEST(bit_units, conversion)
{
  using namespace units::literals;

  units::bits a = 10_bits;
  ASSERT_EQ(a.value(), 10);
  ASSERT_EQ(a.truncate_to_bytes().value(), 1);
  ASSERT_EQ(a.round_up_to_bytes().value(), 2);

  units::bits b = 8_bits;
  ASSERT_TRUE(b.is_byte_exact());
}

TEST(byte_units, basic)
{
  using namespace units::literals;

  units::bytes a = 2_bytes;

  // To bit translation methods.
  ASSERT_EQ(a.to_bits(), 16_bits);
  units::bits c = static_cast<units::bits>(a);
  ASSERT_EQ(c, 16_bits);
}

TEST(bitrate_units, basic)
{
  using namespace units::literals;

  units::bitrate a = 2_Mbps;
  ASSERT_EQ(a.value(), 2e6);

  ASSERT_EQ((1.5_kbps).value(), 1500.0);
  ASSERT_EQ((10_bps).value(), 10.0);
  ASSERT_EQ((1.2_Gbps).value(), 1.2e9);
}

TEST(bitrate_units, conversion)
{
  using namespace units::literals;

  units::bitrate a = 10_Mbps;
  ASSERT_EQ(a.to_kbps(), 10e3);
  ASSERT_EQ(a.to_Mbps(), 10.0);
  ASSERT_EQ(a.to_Gbps(), 0.01);

  // Conversion to integer rounds to the nearest value.
  ASSERT_EQ(a.to_uint(), 10000000U);
  ASSERT_EQ(units::bitrate(1000.4).to_uint(), 1000U);
  ASSERT_EQ(units::bitrate(1000.5).to_uint(), 1001U);
}

TEST(bitrate_units, arithmetic)
{
  using namespace units::literals;

  // Comparison across units.
  ASSERT_EQ(1_Gbps, 1000_Mbps);
  ASSERT_GT(1_Mbps, 999_kbps);

  // Addition and scaling.
  ASSERT_EQ(1_Mbps + 500_kbps, 1.5_Mbps);
  ASSERT_EQ(2.0 * 3_Mbps, 6_Mbps);
}

TEST(bitrate_units, fmt)
{
  using namespace units::literals;

  ASSERT_EQ(fmt::format("{}", 1500_bps), "1500bps");
}

TEST(bitrate_units, bits_divided_by_duration)
{
  using namespace units::literals;

  ASSERT_EQ(1000_bits / std::chrono::duration<double>(2.0), 500_bps);
  ASSERT_EQ(1_bits / std::chrono::duration<double>(0.001), 1_kbps);

  // Implicit conversion from integral std::chrono durations.
  ASSERT_EQ(3000_bits / std::chrono::seconds(2), 1.5_kbps);
  ASSERT_EQ(100_bits / std::chrono::milliseconds(50), 2_kbps);
}

TEST(bitrate_units, bitrate_multiplied_by_duration)
{
  using namespace units::literals;

  ASSERT_EQ(1_kbps * std::chrono::duration<double>(0.5), 500_bits);
  ASSERT_EQ(100_bps * std::chrono::milliseconds(1500), 150_bits);

  // The result is rounded up.
  ASSERT_EQ(999.9_bps * std::chrono::seconds(1), 1000_bits);
  ASSERT_EQ(1001_bps * std::chrono::duration<double>(0.0005), 1_bits);
  ASSERT_EQ(0_bps * std::chrono::seconds(1), 0_bits);
}

TEST(byterate_units, basic)
{
  using namespace units::literals;

  units::byterate a = 2_MBps;
  ASSERT_EQ(a.value(), 2e6);

  ASSERT_EQ((1.5_kBps).value(), 1500.0);
  ASSERT_EQ((10_Bps).value(), 10.0);
  ASSERT_EQ((1.2_GBps).value(), 1.2e9);
}

TEST(byterate_units, conversion)
{
  using namespace units::literals;

  units::byterate a = 10_MBps;
  ASSERT_EQ(a.to_kBps(), 10e3);
  ASSERT_EQ(a.to_MBps(), 10.0);
  ASSERT_EQ(a.to_GBps(), 0.01);

  // To bitrate translation methods.
  ASSERT_EQ(a.to_bitrate(), 80_Mbps);
  units::bitrate b = static_cast<units::bitrate>(a);
  ASSERT_EQ(b, 80_Mbps);

  // Conversion to integer rounds to the nearest value.
  ASSERT_EQ(a.to_uint(), 10000000U);
  ASSERT_EQ(units::byterate(1000.4).to_uint(), 1000U);
  ASSERT_EQ(units::byterate(1000.5).to_uint(), 1001U);
}

TEST(byterate_units, arithmetic)
{
  using namespace units::literals;

  // Comparison across units.
  ASSERT_EQ(1_GBps, 1000_MBps);
  ASSERT_GT(1_MBps, 999_kBps);

  // Addition and scaling.
  ASSERT_EQ(1_MBps + 500_kBps, 1.5_MBps);
  ASSERT_EQ(2.0 * 3_MBps, 6_MBps);
}

TEST(byterate_units, fmt)
{
  using namespace units::literals;

  ASSERT_EQ(fmt::format("{}", 1500_Bps), "1500Bps");
}

TEST(byterate_units, bytes_divided_by_duration)
{
  using namespace units::literals;

  ASSERT_EQ(1000_bytes / std::chrono::duration<double>(2.0), 500_Bps);
  ASSERT_EQ(1_bytes / std::chrono::duration<double>(0.001), 1_kBps);

  // Implicit conversion from integral std::chrono durations.
  ASSERT_EQ(3000_bytes / std::chrono::seconds(2), 1.5_kBps);
  ASSERT_EQ(100_bytes / std::chrono::milliseconds(50), 2_kBps);
}

TEST(byterate_units, byterate_multiplied_by_duration)
{
  using namespace units::literals;

  ASSERT_EQ(1_kBps * std::chrono::duration<double>(0.5), 500_bytes);
  ASSERT_EQ(100_Bps * std::chrono::milliseconds(1500), 150_bytes);

  // The result is rounded up.
  ASSERT_EQ(999.9_Bps * std::chrono::seconds(1), 1000_bytes);
  ASSERT_EQ(1001_Bps * std::chrono::duration<double>(0.0005), 1_bytes);
  ASSERT_EQ(0_Bps * std::chrono::seconds(1), 0_bytes);
}
