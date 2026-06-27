// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/du/du_high/adapters/f1ap_adapters.h"
#include "lib/du/du_high/du_manager/converters/asn1_ref_time_r16_helpers.h"
#include "ocudu/asn1/rrc_nr/sys_info.h"
#include <gtest/gtest.h>

using namespace ocudu;
using namespace odu;

namespace {

class dummy_mac_subframe_time_mapper : public mac_subframe_time_mapper
{
public:
  std::optional<mac_slot_time_info> next_mapping;

  std::optional<mac_slot_time_info> get_last_mapping(subcarrier_spacing scs) const override { return next_mapping; }

  std::optional<time_point> get_time_point(slot_point slot) const override { return std::nullopt; }

  std::optional<slot_point> get_slot_point(time_point time, subcarrier_spacing scs) const override
  {
    return std::nullopt;
  }
};

asn1::rrc_nr::ref_time_r16_s decode(const byte_buffer& encoded)
{
  asn1::rrc_nr::ref_time_r16_s decoded;
  asn1::cbit_ref               bref{encoded};
  EXPECT_EQ(decoded.unpack(bref), asn1::OCUDUASN_SUCCESS);
  return decoded;
}

} // namespace

class f1ap_ref_time_provider_adapter_test : public ::testing::Test
{
protected:
  dummy_mac_subframe_time_mapper    mapper;
  f1ap_du_mac_time_provider_adapter adapter;

  f1ap_ref_time_provider_adapter_test() { adapter.connect(mapper); }
};

TEST_F(f1ap_ref_time_provider_adapter_test, no_mapping_returns_nullopt)
{
  mapper.next_mapping.reset();
  EXPECT_FALSE(adapter.get_last_mapping(subcarrier_spacing::kHz15).has_value());
}

TEST_F(f1ap_ref_time_provider_adapter_test, whole_second_time_encodes_losslessly)
{
  mac_slot_time_info m;
  m.sl_tx             = slot_point{subcarrier_spacing::kHz15, 42, 0};
  m.time_point        = std::chrono::system_clock::time_point{std::chrono::seconds{1735689600LL}};
  mapper.next_mapping = m;

  auto mapping = adapter.get_last_mapping(subcarrier_spacing::kHz15);
  ASSERT_TRUE(mapping.has_value());
  EXPECT_EQ(mapping->ref_slot.sfn(), 42U);
  // is_local_clock is currently hardcoded true (see the TODO in f1ap_adapters.h) since
  // mac_subframe_time_mapper does not expose the clock source.
  EXPECT_TRUE(mapping->is_local_clock);

  // 1735689600 s since the Unix epoch divides evenly into whole days (20089 days, 0 remainder).
  auto decoded = decode(mapping->ref_time_r16);
  EXPECT_EQ(decoded.ref_days_r16, 20089U);
  EXPECT_EQ(decoded.ref_seconds_r16, 0U);
  EXPECT_EQ(decoded.ref_milli_seconds_r16, 0U);
  EXPECT_EQ(decoded.ref_ten_nano_seconds_r16, 0U);
}

TEST_F(f1ap_ref_time_provider_adapter_test, subsecond_component_encodes_losslessly)
{
  mac_slot_time_info m;
  m.sl_tx = slot_point{subcarrier_spacing::kHz15, 42, 0};
  // 123456780 ns = 12345678 * 10 ns, an exact multiple of the 10 ns field granularity.
  m.time_point =
      std::chrono::system_clock::time_point{std::chrono::seconds{1735689600LL} + std::chrono::nanoseconds{123456780LL}};
  mapper.next_mapping = m;

  auto mapping = adapter.get_last_mapping(subcarrier_spacing::kHz15);
  ASSERT_TRUE(mapping.has_value());

  auto decoded = decode(mapping->ref_time_r16);
  EXPECT_EQ(decoded.ref_days_r16, 20089U);
  EXPECT_EQ(decoded.ref_seconds_r16, 0U);
  EXPECT_EQ(decoded.ref_milli_seconds_r16, 123U);
  EXPECT_EQ(decoded.ref_ten_nano_seconds_r16, 45678U);
}

// f1ap_du_mac_time_provider_adapter currently always passes is_local_clock=true (see the TODO above), so the
// GPS-epoch branch of pack_ref_time_r16 below is exercised directly rather than through the adapter.

TEST(f1ap_ref_time_r16_codec_test, gps_epoch_time_encodes_losslessly)
{
  auto time_point = std::chrono::system_clock::time_point{std::chrono::seconds{1735689600LL}};

  // GPS epoch offset (315964800 s = 3657 days) is subtracted before decomposition: 20089 - 3657 = 16432 days.
  auto decoded = decode(pack_ref_time_r16(time_point, /*is_local_clock=*/false));
  EXPECT_EQ(decoded.ref_days_r16, 16432U);
  EXPECT_EQ(decoded.ref_seconds_r16, 0U);
  EXPECT_EQ(decoded.ref_milli_seconds_r16, 0U);
  EXPECT_EQ(decoded.ref_ten_nano_seconds_r16, 0U);
}
