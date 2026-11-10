// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/gtpu/gtpu_teid_pool_impl.h"
#include <gtest/gtest.h>

using namespace ocudu;

constexpr uint16_t       MAX_TEIDS = 16;
constexpr timer_duration teid_release_linger_time{2};

/// GTPU pool test
TEST(gtpu_pool_test, normal_request_succeeds)
{
  timer_manager       timers;
  gtpu_teid_pool_impl pool(MAX_TEIDS, teid_release_linger_time, timers);

  for (unsigned n = GTPU_TEID_MIN.value(); n < 3 + GTPU_TEID_MIN.value(); ++n) {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{n});
  }
}

TEST(gtpu_pool_test, full_pool_request_fails)
{
  timer_manager       timers;
  gtpu_teid_pool_impl pool(MAX_TEIDS, teid_release_linger_time, timers);

  for (unsigned n = GTPU_TEID_MIN.value(); n < MAX_TEIDS + GTPU_TEID_MIN.value(); ++n) {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{n});
  }
  expected<gtpu_teid_t> teid = pool.request_teid();
  ASSERT_EQ(false, teid.has_value());
}

TEST(gtpu_pool_test, request_after_all_release_succeeds)
{
  timer_manager       timers;
  gtpu_teid_pool_impl pool(MAX_TEIDS, teid_release_linger_time, timers);

  for (unsigned n = GTPU_TEID_MIN.value(); n < MAX_TEIDS + GTPU_TEID_MIN.value(); ++n) {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{n});
    ASSERT_EQ(true, pool.release_teid(teid.value()));
  }
  expected<gtpu_teid_t> teid = pool.request_teid();
  ASSERT_EQ(true, teid.has_value());
}

TEST(gtpu_pool_test, request_after_few_free_succeeds)
{
  timer_manager       timers;
  gtpu_teid_pool_impl pool(MAX_TEIDS, teid_release_linger_time, timers);

  for (unsigned n = GTPU_TEID_MIN.value(); n < MAX_TEIDS + GTPU_TEID_MIN.value(); ++n) {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{n});
  }

  // free the TEID 6 and 8
  {
    ASSERT_EQ(true, pool.release_teid(gtpu_teid_t{6}));
    ASSERT_EQ(true, pool.release_teid(gtpu_teid_t{8}));
  }
  {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{6});
  }
  {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(true, teid.has_value());
    ASSERT_EQ(teid.value(), gtpu_teid_t{8});
  }
  // full again
  {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_EQ(false, teid.has_value());
  }
}

TEST(gtpu_pool_test, released_teid_is_lingering)
{
  timer_manager       timers;
  gtpu_teid_pool_impl pool(MAX_TEIDS, teid_release_linger_time, timers);

  expected<gtpu_teid_t> teid1 = pool.request_teid();
  expected<gtpu_teid_t> teid2 = pool.request_teid();
  ASSERT_TRUE(teid1.has_value());
  ASSERT_TRUE(teid2.has_value());

  // Advance clock by lingering time.
  for (unsigned i = 0; i < teid_release_linger_time.count(); i++) {
    timers.tick();
  }

  // Nothing is lingering as nothing was released yet.
  EXPECT_FALSE(pool.is_teid_lingering(teid1.value()));
  EXPECT_FALSE(pool.is_teid_lingering(teid2.value()));

  // Release TEID2.
  ASSERT_TRUE(pool.release_teid(teid2.value()));

  // Advance clock just before expiring the lingering time.
  for (unsigned i = 0; i < teid_release_linger_time.count() - 1; i++) {
    timers.tick();
  }

  // Only released TEID2 is lingering.
  EXPECT_FALSE(pool.is_teid_lingering(teid1.value()));
  EXPECT_TRUE(pool.is_teid_lingering(teid2.value()));

  // Now also release TEID1.
  ASSERT_TRUE(pool.release_teid(teid1.value()));

  // Both are lingering now.
  EXPECT_TRUE(pool.is_teid_lingering(teid1.value()));
  EXPECT_TRUE(pool.is_teid_lingering(teid2.value()));

  // Advance clock one more tick to expire lingering of TEID2.
  timers.tick();

  // Only released TEID1 is lingering, TEID2 is cooled down.
  EXPECT_TRUE(pool.is_teid_lingering(teid1.value()));
  EXPECT_FALSE(pool.is_teid_lingering(teid2.value()));

  // Advance clock to expire the lingering time.
  for (unsigned i = 0; i < teid_release_linger_time.count() - 1; i++) {
    timers.tick();
  }

  // Both TEIDs are not lingering anymore.
  EXPECT_FALSE(pool.is_teid_lingering(teid1.value()));
  EXPECT_FALSE(pool.is_teid_lingering(teid2.value()));
}

TEST(gtpu_pool_test, unused_teid_is_not_lingering)
{
  timer_manager       timers;
  gtpu_teid_pool_impl pool(MAX_TEIDS, teid_release_linger_time, timers);

  gtpu_teid_t unused_teid = int_to_gtpu_teid(0x3);

  EXPECT_FALSE(pool.is_teid_lingering(unused_teid));

  // Advance clock by lingering time.
  for (unsigned i = 0; i < teid_release_linger_time.count(); i++) {
    timers.tick();
    EXPECT_FALSE(pool.is_teid_lingering(unused_teid));
  }
}

TEST(gtpu_pool_test, teid_outside_of_pool_is_not_lingering)
{
  timer_manager       timers;
  gtpu_teid_pool_impl pool(MAX_TEIDS, teid_release_linger_time, timers);

  // The demux checks the TEID of every received packet that has no tunnel, so any value must be handled.
  EXPECT_FALSE(pool.is_teid_lingering(int_to_gtpu_teid(0)));
  EXPECT_FALSE(pool.is_teid_lingering(int_to_gtpu_teid(MAX_TEIDS + GTPU_TEID_MIN.value())));
  EXPECT_FALSE(pool.is_teid_lingering(int_to_gtpu_teid(0xffffffff)));
}

TEST(gtpu_pool_test, release_of_teid_outside_of_pool_fails)
{
  timer_manager       timers;
  gtpu_teid_pool_impl pool(MAX_TEIDS, teid_release_linger_time, timers);

  for (unsigned n = 0; n != MAX_TEIDS; ++n) {
    ASSERT_TRUE(pool.request_teid().has_value());
  }

  EXPECT_FALSE(pool.release_teid(int_to_gtpu_teid(0)));
  EXPECT_FALSE(pool.release_teid(int_to_gtpu_teid(MAX_TEIDS + GTPU_TEID_MIN.value())));
  EXPECT_FALSE(pool.release_teid(int_to_gtpu_teid(0xffffffff)));

  // The pool is unchanged, so it is still full.
  EXPECT_TRUE(pool.full());
}

TEST(gtpu_pool_test, pool_larger_than_16_bit_allocates_all_teids)
{
  // The CU-UP sizes its pools for up to MAX_NOF_PDU_SESSIONS TEIDs per UE, well beyond 16-bit indexes.
  constexpr uint32_t  large_pool_size = 70000;
  timer_manager       timers;
  gtpu_teid_pool_impl pool(large_pool_size, teid_release_linger_time, timers);

  for (uint32_t n = 0; n != large_pool_size; ++n) {
    expected<gtpu_teid_t> teid = pool.request_teid();
    ASSERT_TRUE(teid.has_value());
    ASSERT_EQ(teid.value(), int_to_gtpu_teid(n + GTPU_TEID_MIN.value()));
  }
  EXPECT_FALSE(pool.request_teid().has_value());

  // A freed TEID past the 16-bit range is allocated again.
  const gtpu_teid_t high_teid = int_to_gtpu_teid(large_pool_size - 1 + GTPU_TEID_MIN.value());
  ASSERT_TRUE(pool.release_teid(high_teid));
  expected<gtpu_teid_t> teid = pool.request_teid();
  ASSERT_TRUE(teid.has_value());
  EXPECT_EQ(teid.value(), high_teid);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
