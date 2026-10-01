// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "ocudu/ran/gnb_id.h"
#include "ocudu/ran/i_rnti.h"
#include <gtest/gtest.h>

using namespace ocudu;

constexpr gnb_id_t default_gnb_id{411, 22};

/// Widths the Local NG-RAN Node Identifier IE of TS 38.423 section 9.2.2.101 gives for each I-RNTI profile.
const std::vector<std::pair<full_i_rnti_profile, unsigned>>  full_profiles  = {{full_i_rnti_profile::profile_0, 21},
                                                                               {full_i_rnti_profile::profile_1, 18},
                                                                               {full_i_rnti_profile::profile_2, 15},
                                                                               {full_i_rnti_profile::profile_3, 12}};
const std::vector<std::pair<short_i_rnti_profile, unsigned>> short_profiles = {{short_i_rnti_profile::profile_0, 8},
                                                                               {short_i_rnti_profile::profile_1, 6}};

class rnti_test : public ::testing::Test
{};

TEST_F(rnti_test, full_i_rnti_creation_from_invalid_number_fails)
{
  // A Full-I-RNTI is 40 bits (TS 38.423 section 9.2.3.46).
  ASSERT_FALSE(full_i_rnti_t::from_uint(0x10000000000).has_value());
  ASSERT_TRUE(full_i_rnti_t::from_uint(0xffffffffff).has_value());
}

TEST_F(rnti_test, full_i_rnti_creation_from_valid_number_succeeds)
{
  auto ret = full_i_rnti_t::from_uint(0xdeadbeef);
  ASSERT_TRUE(ret.has_value());
  ASSERT_EQ(ret.value().value(), 0xdeadbeef);
}

TEST_F(rnti_test, short_i_rnti_creation_from_invalid_number_fails)
{
  // A Short-I-RNTI is 24 bits (TS 38.423 section 9.2.3.46).
  ASSERT_FALSE(short_i_rnti_t::from_uint(0x1000000).has_value());
  ASSERT_TRUE(short_i_rnti_t::from_uint(0xffffff).has_value())
      << "The full value range must be accepted, including I-RNTIs allocated by a peer";
}

TEST_F(rnti_test, short_i_rnti_creation_from_valid_number_succeeds)
{
  auto ret = short_i_rnti_t::from_uint(0x4d8000);
  ASSERT_TRUE(ret.has_value());
  ASSERT_EQ(ret.value().value(), 0x4d8000);
}

TEST_F(rnti_test, i_rnti_field_widths_follow_the_profile)
{
  for (const auto& [profile, nof_node_id_bits] : full_profiles) {
    ASSERT_EQ(full_i_rnti_t::nof_node_id_bits(profile), nof_node_id_bits);
    ASSERT_EQ(full_i_rnti_t::nof_ue_ref_bits(profile), 40 - 2 - nof_node_id_bits);
  }

  for (const auto& [profile, nof_node_id_bits] : short_profiles) {
    ASSERT_EQ(short_i_rnti_t::nof_node_id_bits(profile), nof_node_id_bits);
    ASSERT_EQ(short_i_rnti_t::nof_ue_ref_bits(profile), 24 - 1 - nof_node_id_bits);
  }
}

TEST_F(rnti_test, i_rnti_is_composed_of_profile_node_id_and_ue_reference)
{
  // TS 38.300 Annex F composes the value from the MSB down: the profile, the Local NG-RAN Node Identifier and the UE
  // reference.
  for (const auto& [profile, nof_node_id_bits] : full_profiles) {
    const uint32_t      node_id = full_i_rnti_t::to_local_node_id(profile, default_gnb_id.id);
    const full_i_rnti_t i_rnti{profile, node_id, 5};

    const unsigned nof_ue_ref_bits = full_i_rnti_t::nof_ue_ref_bits(profile);
    ASSERT_EQ(i_rnti.value(),
              (static_cast<uint64_t>(profile) << 38) | (static_cast<uint64_t>(node_id) << nof_ue_ref_bits) | 5);
    ASSERT_EQ(i_rnti.profile(), profile);
    ASSERT_EQ(i_rnti.node_id(), node_id);
    ASSERT_EQ(i_rnti.ue_ref(), 5);
  }

  for (const auto& [profile, nof_node_id_bits] : short_profiles) {
    const uint32_t       node_id = short_i_rnti_t::to_local_node_id(profile, default_gnb_id.id);
    const short_i_rnti_t i_rnti{profile, node_id, 5};

    const unsigned nof_ue_ref_bits = short_i_rnti_t::nof_ue_ref_bits(profile);
    ASSERT_EQ(i_rnti.value(), (static_cast<uint32_t>(profile) << 23) | (node_id << nof_ue_ref_bits) | 5);
    ASSERT_EQ(i_rnti.profile(), profile);
    ASSERT_EQ(i_rnti.node_id(), node_id);
    ASSERT_EQ(i_rnti.ue_ref(), 5);
  }
}

TEST_F(rnti_test, i_rnti_parsed_from_a_number_reports_the_profile_it_was_composed_with)
{
  // The profile occupies the leading bits, so a node reading an I-RNTI it did not allocate recovers the widths of the
  // remaining fields from the value alone.
  for (const auto& [profile, nof_node_id_bits] : full_profiles) {
    const full_i_rnti_t composed{profile, 0x1234, 7};
    const full_i_rnti_t parsed = full_i_rnti_t::from_uint(composed.value()).value();

    ASSERT_EQ(parsed.profile(), profile);
    ASSERT_EQ(parsed.node_id(), composed.node_id());
    ASSERT_EQ(parsed.ue_ref(), 7);
  }

  for (const auto& [profile, nof_node_id_bits] : short_profiles) {
    const short_i_rnti_t composed{profile, 0x2a, 7};
    const short_i_rnti_t parsed = short_i_rnti_t::from_uint(composed.value()).value();

    ASSERT_EQ(parsed.profile(), profile);
    ASSERT_EQ(parsed.node_id(), composed.node_id());
    ASSERT_EQ(parsed.ue_ref(), 7);
  }
}

TEST_F(rnti_test, i_rnti_node_id_and_ue_reference_stay_within_the_profile_widths)
{
  for (const auto& [profile, nof_node_id_bits] : full_profiles) {
    const full_i_rnti_t i_rnti{profile, 0xffffffff, 0xffffffff};

    ASSERT_EQ(i_rnti.value() >> 38, static_cast<uint64_t>(profile))
        << "The node ID and UE reference reached into the I-RNTI profile";
    ASSERT_EQ(i_rnti.node_id(), (1U << nof_node_id_bits) - 1);
    ASSERT_EQ(i_rnti.ue_ref(), full_i_rnti_t::max_ue_ref(profile));
  }

  for (const auto& [profile, nof_node_id_bits] : short_profiles) {
    const short_i_rnti_t i_rnti{profile, 0xffffffff, 0xffffffff};

    ASSERT_EQ(i_rnti.value() >> 23, static_cast<uint32_t>(profile))
        << "The node ID and UE reference reached into the I-RNTI profile";
    ASSERT_EQ(i_rnti.node_id(), (1U << nof_node_id_bits) - 1);
    ASSERT_EQ(i_rnti.ue_ref(), short_i_rnti_t::max_ue_ref(profile));
  }
}

TEST_F(rnti_test, i_rnti_node_id_carries_the_least_significant_gnb_id_bits)
{
  // A gNB ID is wider than the node identifier of either I-RNTI, so a node whose gNB ID differs only above that width
  // shares the identifier.
  ASSERT_EQ(full_i_rnti_t::to_local_node_id(full_i_rnti_profile::profile_0, 0x200000 + 411), 411);
  ASSERT_EQ(full_i_rnti_t::to_local_node_id(full_i_rnti_profile::profile_3, 0x1000 + 411), 411);
  ASSERT_EQ(short_i_rnti_t::to_local_node_id(short_i_rnti_profile::profile_0, 0x100 + 42), 42);
  ASSERT_EQ(short_i_rnti_t::to_local_node_id(short_i_rnti_profile::profile_1, 0x40 + 42 - 40), 2);
}

TEST_F(rnti_test, i_rnti_equality_operators_work_as_expected)
{
  auto ret1 = full_i_rnti_t{full_i_rnti_profile::profile_0, default_gnb_id.id, 0xff};
  auto ret2 = full_i_rnti_t{full_i_rnti_profile::profile_0, default_gnb_id.id, 0xff};
  auto ret3 = full_i_rnti_t{full_i_rnti_profile::profile_0, default_gnb_id.id, 0xfe};

  ASSERT_TRUE(ret1 == ret2);
  ASSERT_FALSE(ret1 != ret2);
  ASSERT_FALSE(ret1 < ret2);
  ASSERT_TRUE(ret1 <= ret2);
  ASSERT_FALSE(ret1 > ret2);
  ASSERT_TRUE(ret1 >= ret2);

  ASSERT_FALSE(ret1 == ret3);
  ASSERT_TRUE(ret1 != ret3);
  ASSERT_FALSE(ret1 < ret3);
  ASSERT_FALSE(ret1 <= ret3);
  ASSERT_TRUE(ret1 > ret3);
  ASSERT_TRUE(ret1 >= ret3);
}
