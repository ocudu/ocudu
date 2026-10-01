// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/byte_buffer.h"
#include "ocudu/adt/detail/shared_segment_list.h"
#include <gtest/gtest.h>
#include <numeric>

using namespace ocudu;
using namespace ocudu::detail;

namespace {

struct test_segment_view {
  explicit test_segment_view(span<uint8_t> storage_) noexcept : storage(storage_) {}

  span<uint8_t> storage;
};

using test_shared_segment_list = shared_segment_list<test_segment_view>;

class shared_segment_list_test : public ::testing::Test
{
protected:
  byte_buffer_memory_resource& pool = get_default_byte_buffer_segment_pool();

  // Convenience: make a list and assert success.
  test_shared_segment_list make_list(size_t seg_size_hint = 128)
  {
    return test_shared_segment_list::make(seg_size_hint, pool).value();
  }
};

TEST_F(shared_segment_list_test, default_constructed_is_empty)
{
  test_shared_segment_list list;
  ASSERT_TRUE(list.empty());
  ASSERT_EQ(list.length(), 0U);
  ASSERT_EQ(list.tail(), nullptr);

  size_t count = 0;
  for (auto& seg : list) {
    (void)seg;
    ++count;
  }
  ASSERT_EQ(count, 0U);
}

TEST_F(shared_segment_list_test, make_creates_initial_segment)
{
  auto list = make_list();

  ASSERT_FALSE(list.empty());
  ASSERT_NE(list.tail(), nullptr);
  ASSERT_GE(list.tail()->storage.size(), 1U);
  ASSERT_EQ(list.length(), 1U);

  size_t count = 0;
  for (auto& seg : list) {
    ASSERT_FALSE(seg.storage.empty());
    ++count;
  }
  ASSERT_EQ(count, 1U);
}

TEST_F(shared_segment_list_test, initial_segment_write_and_read)
{
  auto list = make_list();

  span<uint8_t> storage = list.tail()->storage;
  ASSERT_GE(storage.size(), 1U);
  std::iota(storage.begin(), storage.end(), uint8_t{0});

  // Verify via const range.
  const test_shared_segment_list& clist = list;
  size_t                          idx   = 0;
  for (const auto& cseg : clist) {
    for (uint8_t byte : cseg.storage) {
      ASSERT_EQ(byte, static_cast<uint8_t>(idx++));
    }
  }
}

TEST_F(shared_segment_list_test, append_segment_adds_segment)
{
  auto list = make_list();
  ASSERT_TRUE(list.append_segment(128));

  ASSERT_FALSE(list.empty());

  size_t count = 0;
  for (auto& seg : list) {
    ASSERT_FALSE(seg.storage.empty());
    ++count;
  }
  // Initial segment from make + 1 appended.
  ASSERT_EQ(count, 2U);
  ASSERT_EQ(list.length(), 2U);
}

TEST_F(shared_segment_list_test, multi_segment_append)
{
  auto             list         = make_list();
  constexpr size_t num_appended = 3;
  for (size_t i = 0; i < num_appended; ++i) {
    ASSERT_TRUE(list.append_segment(128));
  }

  ASSERT_FALSE(list.empty());

  size_t count = 0;
  for (auto& seg : list) {
    ASSERT_FALSE(seg.storage.empty());
    ++count;
  }
  // Initial segment from make + num_appended.
  ASSERT_EQ(count, num_appended + 1);
  ASSERT_EQ(list.length(), num_appended + 1);
  ASSERT_NE(list.tail(), nullptr);
}

TEST_F(shared_segment_list_test, clear_resets_to_empty)
{
  auto list = make_list();
  ASSERT_FALSE(list.empty());

  list.clear();
  ASSERT_TRUE(list.empty());
  ASSERT_EQ(list.length(), 0U);
  ASSERT_EQ(list.tail(), nullptr);
}

TEST_F(shared_segment_list_test, shallow_copy_shares_control_block)
{
  auto list = make_list();

  span<uint8_t> storage = list.tail()->storage;
  std::fill(storage.begin(), storage.end(), uint8_t{0xab});

  test_shared_segment_list copy = list;
  ASSERT_FALSE(copy.empty());
  ASSERT_EQ(copy.length(), list.length());

  // Both should see the same bytes (shared ownership).
  size_t total = 0;
  for (const auto& cseg : copy) {
    for (uint8_t byte : cseg.storage) {
      ASSERT_EQ(byte, uint8_t{0xab});
      ++total;
    }
  }
  ASSERT_EQ(total, list.tail()->storage.size());
}

TEST_F(shared_segment_list_test, original_still_valid_after_copy_destroyed)
{
  auto list = make_list();
  std::fill(list.tail()->storage.begin(), list.tail()->storage.end(), uint8_t{0xcd});

  {
    const test_shared_segment_list copy = list;
    ASSERT_FALSE(copy.empty());
  } // copy destroyed here; ref count drops from 2 to 1.

  ASSERT_FALSE(list.empty());
  for (const auto& cseg : list) {
    for (uint8_t byte : cseg.storage) {
      ASSERT_EQ(byte, uint8_t{0xcd});
    }
  }
}

TEST_F(shared_segment_list_test, move_constructor_leaves_source_empty)
{
  auto src = make_list();
  ASSERT_TRUE(src.append_segment(128));

  test_shared_segment_list dst{std::move(src)};
  ASSERT_TRUE(src.empty());
  ASSERT_FALSE(dst.empty());
}

TEST_F(shared_segment_list_test, move_assignment_leaves_source_empty)
{
  auto src = make_list();
  ASSERT_TRUE(src.append_segment(128));

  auto dst = make_list();
  dst      = std::move(src);

  ASSERT_TRUE(src.empty());
  ASSERT_FALSE(dst.empty());
}

TEST_F(shared_segment_list_test, move_assignment_drops_previous_content)
{
  auto   a     = make_list();
  size_t a_len = a.length();

  auto b = make_list();
  ASSERT_TRUE(b.append_segment(128));
  ASSERT_TRUE(b.append_segment(128));

  b = std::move(a);

  ASSERT_EQ(b.length(), a_len);
  size_t count = 0;
  for (auto& seg : b) {
    ASSERT_FALSE(seg.storage.empty());
    ++count;
  }
  ASSERT_EQ(count, 1U);
}

TEST_F(shared_segment_list_test, default_ctor_append_segment_uses_malloc_pool)
{
  test_shared_segment_list list;
  ASSERT_TRUE(list.empty());

  ASSERT_TRUE(list.append_segment(128));
  ASSERT_FALSE(list.empty());
  ASSERT_NE(list.tail(), nullptr);
  ASSERT_GE(list.tail()->storage.size(), 1U);

  ASSERT_TRUE(list.append_segment(64));
  size_t count = 0;
  for (auto& seg : list) {
    ASSERT_FALSE(seg.storage.empty());
    ++count;
  }
  ASSERT_EQ(count, 2U);
}

TEST_F(shared_segment_list_test, appended_segment_storage_starts_after_view_object)
{
  // For non-initial segments (create_segment path) the storage starts right after the node_t,
  // which for T obj means right after the T object within the node.
  auto list = make_list();
  ASSERT_TRUE(list.append_segment(128));
  auto* seg = list.tail();
  ASSERT_NE(seg, nullptr);
  ASSERT_EQ(seg->storage.data(), reinterpret_cast<uint8_t*>(seg + 1));
  ASSERT_GE(seg->storage.size(), 1U);
}

} // namespace
