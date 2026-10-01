// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

#include "lib/mac/mac_dl/mac_subframe_time_mapper_impl.h"
#include "ocudu/adt/format.h"
#include "ocudu/support/test_utils.h"
#include <atomic>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

using namespace ocudu;

// Test suite for mac_subframe_time_mapper_impl (DU-wide subframe time mapper).
class subframe_time_mapper_test : public ::testing::TestWithParam<unsigned>
{
protected:
  using slot_time_point = std::chrono::time_point<std::chrono::system_clock>;

  const std::chrono::milliseconds frame_dur{10};
  unsigned                        numerology{GetParam()};
  mac_subframe_time_mapper_impl   manager;
  slot_point                      next_point{numerology, 0, 0};
};

TEST_P(subframe_time_mapper_test, slot_time_point_mapping_not_available)
{
  const slot_point      report_slot       = {numerology, 0, 0};
  const slot_time_point report_time_point = std::chrono::system_clock::now();
  const auto            scs               = to_subcarrier_spacing(numerology);

  ASSERT_FALSE(manager.get_last_mapping(scs).has_value());
  auto mapping = manager.get_last_mapping(scs);
  ASSERT_FALSE(mapping.has_value());
  std::optional<slot_time_point> ret_time_point = manager.get_time_point(report_slot);
  ASSERT_FALSE(ret_time_point.has_value());
  std::optional<slot_point> ret_slot = manager.get_slot_point(report_time_point, scs);
  ASSERT_FALSE(ret_slot.has_value());
}

TEST_P(subframe_time_mapper_test, when_called_with_unitialized_value)
{
  const auto                scs               = to_subcarrier_spacing(numerology);
  const slot_point_extended report_slot       = {scs, 0};
  const slot_time_point     report_time_point = std::chrono::system_clock::now();

  manager.handle_slot_indication({report_slot, report_time_point});

  // Mapping is available.
  ASSERT_TRUE(manager.get_last_mapping(scs).has_value());
  slot_point      slot;
  slot_time_point time_point;
  // But cannot map from an uninitialized slot/time point.
  std::optional<slot_time_point> ret_time_point = manager.get_time_point(slot);
  ASSERT_FALSE(ret_time_point.has_value());
  std::optional<slot_point> ret_slot = manager.get_slot_point(time_point, scs);
  ASSERT_FALSE(ret_slot.has_value());
}

TEST_P(subframe_time_mapper_test, slot_time_point_mapping_available)
{
  const auto                scs               = to_subcarrier_spacing(numerology);
  const slot_point_extended report_slot       = {scs, 0};
  const slot_time_point     report_time_point = std::chrono::system_clock::now();

  ASSERT_FALSE(manager.get_last_mapping(scs).has_value());
  manager.handle_slot_indication({report_slot, report_time_point});
  ASSERT_TRUE(manager.get_last_mapping(scs).has_value());

  auto mapping = manager.get_last_mapping(scs);
  ASSERT_TRUE(mapping.has_value());
  slot_point      slot       = mapping.value().sl_tx;
  slot_time_point time_point = mapping.value().time_point;
  ASSERT_EQ(slot, report_slot.without_hyper_sfn());
  ASSERT_EQ(time_point, report_time_point);

  std::optional<slot_time_point> ret_time_point = manager.get_time_point(report_slot.without_hyper_sfn());
  ASSERT_TRUE(ret_time_point.has_value());
  ASSERT_EQ(ret_time_point.value(), report_time_point);

  std::optional<slot_point> ret_slot = manager.get_slot_point(time_point, scs);
  ASSERT_TRUE(ret_slot.has_value());
  ASSERT_EQ(ret_slot.value(), report_slot.without_hyper_sfn());
}

TEST_P(subframe_time_mapper_test, get_last_mapping)
{
  const auto          scs                = to_subcarrier_spacing(numerology);
  slot_point_extended report_slot        = {scs, 0};
  slot_time_point     report_time_point  = std::chrono::system_clock::now();
  unsigned            slots_per_subframe = report_slot.nof_slots_per_subframe();

  slot_point      slot;
  slot_time_point time_point;

  ASSERT_FALSE(manager.get_last_mapping(scs).has_value());
  // Note: only SFN is stored, slot_idx = 0.
  for (unsigned i = 0; i < 10; i++) {
    report_slot += slots_per_subframe;
    report_time_point += frame_dur;
    manager.handle_slot_indication({report_slot, report_time_point});

    auto mapping = manager.get_last_mapping(scs);
    ASSERT_TRUE(mapping.has_value());
    slot       = mapping.value().sl_tx;
    time_point = mapping.value().time_point;
    ASSERT_EQ(slot, report_slot.without_hyper_sfn());
    ASSERT_EQ(time_point, report_time_point);
  }
}

TEST_P(subframe_time_mapper_test, get_last_mapping_only_sub_frame_stored)
{
  const auto          scs                      = to_subcarrier_spacing(numerology);
  slot_point_extended full_subframe_slot       = {scs, 0};
  slot_time_point     full_subframe_time_point = std::chrono::system_clock::now();
  const auto          slot_dur                 = std::chrono::microseconds{1000U >> numerology};

  slot_point      slot;
  slot_time_point time_point;

  ASSERT_FALSE(manager.get_last_mapping(scs).has_value());
  manager.handle_slot_indication({full_subframe_slot, full_subframe_time_point});
  ASSERT_TRUE(manager.get_last_mapping(scs).has_value());

  slot_point_extended report_slot       = full_subframe_slot;
  slot_time_point     report_time_point = full_subframe_time_point;
  // Note: only subframe is stored, slot_idx = 0.
  // Note: The following updates will not be stored.
  for (unsigned i = 0; i < full_subframe_slot.nof_slots_per_frame(); i++) {
    report_slot++;
    report_time_point += slot_dur;
    if (report_slot.subframe_slot_index() == 0) {
      full_subframe_slot       = report_slot;
      full_subframe_time_point = report_time_point;
    }

    manager.handle_slot_indication({report_slot, report_time_point});
    ASSERT_TRUE(manager.get_last_mapping(scs).has_value());

    auto mapping = manager.get_last_mapping(scs);
    ASSERT_TRUE(mapping.has_value());
    slot       = mapping.value().sl_tx;
    time_point = mapping.value().time_point;
    ASSERT_EQ(slot, full_subframe_slot.without_hyper_sfn());
    ASSERT_EQ(time_point, full_subframe_time_point);
  }
}

TEST_P(subframe_time_mapper_test, get_slot_time_point_from_slot_point)
{
  ocudulog::basic_logger&   logger            = ocudulog::fetch_basic_logger("TEST");
  const auto                scs               = to_subcarrier_spacing(numerology);
  const slot_point_extended report_slot       = {scs, 0};
  const slot_time_point     report_time_point = std::chrono::system_clock::now();
  const auto                slot_dur          = std::chrono::microseconds{1000U >> numerology};

  ASSERT_FALSE(manager.get_last_mapping(scs).has_value());
  logger.info(
      "N={} Report slot={} time_point={}", numerology, report_slot, report_time_point.time_since_epoch().count());
  manager.handle_slot_indication({report_slot, report_time_point});
  ASSERT_TRUE(manager.get_last_mapping(scs).has_value());

  slot_point_extended            slot;
  std::optional<slot_time_point> time_point;
  slot_time_point                expected_time_point;
  auto time_difference = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::microseconds(0));

  for (signed i = -10; i < 10; i++) {
    slot                = report_slot + i;
    expected_time_point = report_time_point + slot_dur * i;
    time_point          = manager.get_time_point(slot.without_hyper_sfn());
    ASSERT_TRUE(time_point.has_value());
    time_difference = std::chrono::duration_cast<std::chrono::microseconds>(time_point.value() - report_time_point);
    logger.info("N={} Returned time_point={} for slot={} (time_diff={})",
                numerology,
                time_point.value().time_since_epoch().count(),
                slot,
                time_difference.count());
    ASSERT_EQ(time_point, expected_time_point);
  }
}

TEST_P(subframe_time_mapper_test, get_slot_point_from_time_point)
{
  ocudulog::basic_logger&   logger            = ocudulog::fetch_basic_logger("TEST");
  const auto                scs               = to_subcarrier_spacing(numerology);
  const slot_point_extended report_slot       = {scs, 0};
  const slot_time_point     report_time_point = std::chrono::system_clock::now();
  const auto                slot_dur          = std::chrono::microseconds{1000U >> numerology};

  ASSERT_FALSE(manager.get_last_mapping(scs).has_value());
  logger.info(
      "N={} Report slot={} time_point={}", numerology, report_slot, report_time_point.time_since_epoch().count());
  manager.handle_slot_indication({report_slot, report_time_point});
  ASSERT_TRUE(manager.get_last_mapping(scs).has_value());

  std::optional<slot_point> slot;
  slot_time_point           time_point;
  slot_point_extended       expected_slot_point;
  auto                      time_difference = std::chrono::microseconds(0);
  for (signed i = -10; i < 10; i++) {
    time_point          = report_time_point + slot_dur * i;
    expected_slot_point = report_slot + i;

    slot = manager.get_slot_point(time_point, scs);
    ASSERT_TRUE(slot.has_value());
    time_difference = std::chrono::duration_cast<std::chrono::microseconds>(time_point - report_time_point);
    logger.info("N={} Returned slot={} for time_point={} (time_diff={})",
                numerology,
                slot.value(),
                time_point.time_since_epoch().count(),
                time_difference.count());
    ASSERT_EQ(slot.value(), expected_slot_point.without_hyper_sfn());
  }
}

// Test suite for atomic_subframe_time_mapper (multi-cell, lock-free mapper).
class atomic_subframe_time_mapper_test : public ::testing::TestWithParam<unsigned>
{
protected:
  using time_point = std::chrono::time_point<std::chrono::system_clock>;

  const std::chrono::milliseconds frame_dur{10};
  unsigned                        numerology{GetParam()};
  atomic_subframe_time_mapper     atomic_mapper;
  slot_point                      next_point{numerology, 0, 0};
};

TEST_P(atomic_subframe_time_mapper_test, store_and_load)
{
  const slot_point report_slot       = {numerology, 11, 0};
  const time_point report_time_point = std::chrono::system_clock::now();

  const auto no_mapping = atomic_mapper.load(numerology);
  ASSERT_EQ(no_mapping.sl_tx.valid(), false);

  ASSERT_TRUE(atomic_mapper.store(report_slot, report_time_point));
  const auto mapping = atomic_mapper.load(numerology);
  ASSERT_EQ(mapping.sl_tx, report_slot);
  ASSERT_EQ(mapping.time_point, report_time_point);
}

TEST_P(atomic_subframe_time_mapper_test, store_rejects_non_subframe_boundary)
{
  const slot_point report_slot       = {numerology, 11, 1};
  const time_point report_time_point = std::chrono::system_clock::now();

  // For numerology 0, there is only 1 slot per subframe, so every slot is at a subframe boundary.
  // For other numerologies, slot index 1 is not at a subframe boundary.
  if (numerology == 0) {
    // For numerology 0, slot index 1 is at subframe boundary (subframe 1, slot 0 within subframe).
    // First store should succeed.
    ASSERT_TRUE(atomic_mapper.store(report_slot, report_time_point));
  } else {
    // For other numerologies, slot index 1 is not at subframe boundary.
    ASSERT_FALSE(atomic_mapper.store(report_slot, report_time_point));
  }
}

TEST_P(atomic_subframe_time_mapper_test, monotonic_advancement)
{
  const time_point base_time = std::chrono::system_clock::now();

  // Store initial mapping for SFN 10.
  slot_point slot1 = {numerology, 10, 0};
  ASSERT_TRUE(atomic_mapper.store(slot1, base_time));

  // Try to store an older SFN 5. It should be rejected.
  slot_point slot2 = {numerology, 5, 0};
  ASSERT_FALSE(atomic_mapper.store(slot2, base_time + std::chrono::milliseconds(1)));

  // Try to store the same SFN 10. It should be rejected.
  ASSERT_FALSE(atomic_mapper.store(slot1, base_time + std::chrono::milliseconds(2)));

  // Store a newer SFN 15. It should succeed.
  slot_point slot3 = {numerology, 15, 0};
  ASSERT_TRUE(atomic_mapper.store(slot3, base_time + std::chrono::milliseconds(5)));

  // Verify the mapping is for SFN 15.
  const auto mapping = atomic_mapper.load(numerology);
  ASSERT_EQ(mapping.sl_tx, slot3);
}

TEST_P(atomic_subframe_time_mapper_test, sfn_wraparound)
{
  const time_point base_time = std::chrono::system_clock::now();

  // Store mapping for SFN 1023 (max SFN).
  slot_point slot_max = {numerology, 1023, 0};
  ASSERT_TRUE(atomic_mapper.store(slot_max, base_time));

  // Store mapping for SFN 0 (wraparound). It should succeed (0 > 1023 with wraparound).
  slot_point slot_zero = {numerology, 0, 0};
  ASSERT_TRUE(atomic_mapper.store(slot_zero, base_time + std::chrono::milliseconds(10)));

  // Verify the mapping is for SFN 0.
  const auto mapping = atomic_mapper.load(numerology);
  ASSERT_EQ(mapping.sl_tx, slot_zero);

  // Try to store SFN 1020 (older than 0 with wraparound). It should be rejected.
  slot_point slot_old = {numerology, 1020, 0};
  ASSERT_FALSE(atomic_mapper.store(slot_old, base_time + std::chrono::milliseconds(15)));
}

TEST_P(atomic_subframe_time_mapper_test, multi_threaded_race_condition)
{
  const unsigned   num_threads   = 4;
  const slot_point target_slot   = {numerology, 100, 0};
  const time_point base_time     = std::chrono::system_clock::now();
  std::atomic<int> success_count = {0};
  std::atomic<int> failure_count = {0};

  std::vector<std::thread> threads;
  std::atomic<bool>        start_flag = {false};

  // Launch multiple threads that all try to store the same subframe.
  for (unsigned i = 0; i < num_threads; i++) {
    threads.emplace_back([&, i]() {
      // Wait for all threads to be ready
      while (!start_flag.load(std::memory_order_acquire)) {
        std::this_thread::yield();
      }

      // Each thread tries to store the same slot with a different timestamp.
      time_point thread_time = base_time + std::chrono::nanoseconds(i * 100);
      bool       result      = atomic_mapper.store(target_slot, thread_time);

      if (result) {
        success_count.fetch_add(1, std::memory_order_relaxed);
      } else {
        failure_count.fetch_add(1, std::memory_order_relaxed);
      }
    });
  }

  // Signal all threads to start simultaneously.
  start_flag.store(true, std::memory_order_release);

  // Wait for all threads to complete.
  for (auto& t : threads) {
    t.join();
  }

  // Verify that exactly one thread succeeded.
  ASSERT_EQ(success_count.load(), 1);
  ASSERT_EQ(failure_count.load(), num_threads - 1);

  // Verify the mapping was stored.
  const auto mapping = atomic_mapper.load(numerology);
  ASSERT_EQ(mapping.sl_tx, target_slot);
}

TEST_P(atomic_subframe_time_mapper_test, multi_threaded_monotonic_updates)
{
  const unsigned           num_threads = 4;
  const time_point         base_time   = std::chrono::system_clock::now();
  std::vector<std::thread> threads;
  std::atomic<bool>        start_flag = {false};

  // Each thread tries to store progressively newer SFNs.
  for (unsigned i = 0; i < num_threads; i++) {
    threads.emplace_back([&, i]() {
      // Wait for all threads to be ready.
      while (!start_flag.load(std::memory_order_acquire)) {
        std::this_thread::yield();
      }

      // Each thread stores multiple slots in sequence.
      for (unsigned j = 0; j < 10; j++) {
        slot_point slot = {numerology, i * 10 + j, 0};
        time_point time = base_time + std::chrono::milliseconds(i * 10 + j);
        atomic_mapper.store(slot, time);
      }
    });
  }

  // Signal all threads to start.
  start_flag.store(true, std::memory_order_release);

  // Wait for all threads to complete.
  for (auto& t : threads) {
    t.join();
  }

  // Verify the final mapping is one of the latest slots (monotonic guarantee).
  const auto mapping          = atomic_mapper.load(numerology);
  unsigned   expected_min_sfn = (num_threads - 1) * 10; // Last thread's starting SFN
  ASSERT_GE(mapping.sl_tx.sfn(), expected_min_sfn);
}

TEST_P(atomic_subframe_time_mapper_test, multi_threaded_different_numerologies)
{
  // Test concurrent updates from cells with different numerologies.
  // The mapper stores SFN+subframe but not numerology, so different numerologies
  // should be able to coexist as long as they represent the same SFN/subframe.
  const unsigned           num_threads = 5;
  const time_point         base_time   = std::chrono::system_clock::now();
  std::vector<std::thread> threads;
  std::atomic<bool>        start_flag = {false};
  std::atomic<int>         store_successes{0};
  const unsigned           sfn_start = 100;
  const unsigned           sfn_stop  = 120;
  // Each thread uses a different numerology but stores the same SFN.
  for (unsigned thread_numerology = 0; thread_numerology < num_threads; thread_numerology++) {
    threads.emplace_back([&, thread_numerology]() {
      // Wait for all threads to be ready.
      while (!start_flag.load(std::memory_order_acquire)) {
        std::this_thread::yield();
      }

      // Each thread stores slots for its numerology.
      // All threads target the same SFN range to create contention.
      for (unsigned sfn = sfn_start; sfn <= sfn_stop; sfn++) {
        slot_point slot = {thread_numerology, sfn, 0};
        time_point time = base_time + std::chrono::milliseconds(sfn);
        if (atomic_mapper.store(slot, time)) {
          store_successes.fetch_add(1, std::memory_order_relaxed);
        }
      }
    });
  }

  // Signal all threads to start.
  start_flag.store(true, std::memory_order_release);

  // Wait for all threads to complete.
  for (auto& t : threads) {
    t.join();
  }

  // Verify that some stores succeeded.
  ASSERT_GT(store_successes.load(), 0);

  // Verify we can load the mapping with any numerology and get consistent SFN.
  // The stored SFN should be the same regardless of which numerology we use to load.
  unsigned stored_sfn = atomic_mapper.load(0).sl_tx.sfn();

  for (unsigned load_numerology = 1; load_numerology < 5; load_numerology++) {
    auto mapping = atomic_mapper.load(load_numerology);
    ASSERT_TRUE(mapping.sl_tx.valid());
    ASSERT_EQ(mapping.sl_tx.sfn(), stored_sfn);
  }

  // Verify the final SFN equals sfn_stop.
  ASSERT_EQ(stored_sfn, sfn_stop);
}

// Instantiate the test suite with different numerology values
INSTANTIATE_TEST_SUITE_P(NumerologyTests, subframe_time_mapper_test, ::testing::Values(0, 1, 2, 3, 4));
INSTANTIATE_TEST_SUITE_P(NumerologyTests, atomic_subframe_time_mapper_test, ::testing::Values(0, 1, 2, 3, 4));
