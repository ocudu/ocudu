// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/segmented_circular_map.h"
#include <gtest/gtest.h>

using namespace ocudu;

#ifdef __clang__
#pragma GCC diagnostic ignored "-Wall"
#else
#pragma GCC diagnostic ignored "-Wsuggest-override"
#endif

namespace {

/// Simple pool backed by a fixed array of segments.
template <typename K, typename V, size_t L>
class simple_pool : public map_segment_pool_interface<K, V, L>
{
public:
  explicit simple_pool(size_t capacity)
  {
    storage.resize(capacity);
    for (auto& seg : storage) {
      free_list.push_back(&seg);
    }
  }

  map_segment<K, V, L>* get_segment() override
  {
    if (free_list.empty()) {
      return nullptr;
    }
    auto* seg = free_list.back();
    free_list.pop_back();
    return seg;
  }

  void return_segment(map_segment<K, V, L>* seg) override { free_list.push_back(seg); }

  size_t available() const { return free_list.size(); }

private:
  std::vector<map_segment<K, V, L>>  storage;
  std::vector<map_segment<K, V, L>*> free_list;
};

// L=4 segments of 4 slots each.
using str_pool = simple_pool<unsigned, std::string, 4>;
using str_map  = segmented_circular_map<unsigned, std::string, 4>;

TEST(segmented_circular_map_test, test_basic_operations)
{
  str_pool pool(2);
  str_map  mymap(8, pool);

  ASSERT_EQ(0U, mymap.size());
  ASSERT_TRUE(mymap.empty() and not mymap.full());
  ASSERT_EQ(mymap.capacity(), 8U);
  ASSERT_TRUE(mymap.begin() == mymap.end());

  ASSERT_FALSE(mymap.contains(0));
  ASSERT_TRUE(mymap.insert(0, "obj0"));
  ASSERT_TRUE(mymap.contains(0) and mymap[0] == "obj0");
  ASSERT_EQ(1U, mymap.size());
  ASSERT_FALSE(mymap.empty());
  ASSERT_TRUE(mymap.begin() != mymap.end());

  ASSERT_FALSE(mymap.insert(0, "obj0"));
  ASSERT_TRUE(mymap.insert(1, "obj1"));
  ASSERT_TRUE(mymap.contains(0) and mymap.contains(1) and mymap[1] == "obj1");
  ASSERT_EQ(2U, mymap.size());

  ASSERT_TRUE(mymap.find(1) != mymap.end());
  ASSERT_EQ(1U, mymap.find(1)->first);
  ASSERT_EQ("obj1", mymap.find(1)->second);

  uint32_t count = 0;
  for (const auto& obj : mymap) {
    ASSERT_EQ("obj" + std::to_string(count++), obj.second);
  }
  ASSERT_EQ(2U, count);

  ASSERT_TRUE(mymap.erase(0));
  ASSERT_TRUE(mymap.erase(1));
  ASSERT_EQ(0U, mymap.size());
  ASSERT_TRUE(mymap.empty());

  ASSERT_TRUE(mymap.insert(0, "obj0"));
  ASSERT_TRUE(mymap.insert(1, "obj1"));
  mymap.clear();
  ASSERT_EQ(0U, mymap.size());
  ASSERT_TRUE(mymap.empty());
}

TEST(segmented_circular_map_test, test_segment_lifecycle)
{
  str_pool pool(2);
  str_map  mymap(8, pool);

  ASSERT_EQ(2U, pool.available());

  ASSERT_TRUE(mymap.insert(0, "a"));
  ASSERT_EQ(1U, pool.available());

  ASSERT_TRUE(mymap.insert(4, "b"));
  ASSERT_EQ(0U, pool.available());

  ASSERT_TRUE(mymap.erase(0));
  ASSERT_EQ(1U, pool.available());

  ASSERT_TRUE(mymap.erase(4));
  ASSERT_EQ(2U, pool.available());
}

TEST(segmented_circular_map_test, test_clear_returns_all_segments)
{
  str_pool pool(2);
  str_map  mymap(8, pool);

  mymap.insert(0, "a");
  mymap.insert(4, "b");
  ASSERT_EQ(0U, pool.available());

  mymap.clear();
  ASSERT_EQ(2U, pool.available());
}

TEST(segmented_circular_map_test, test_pool_exhaustion)
{
  // Pool with capacity for only 1 segment.
  str_pool pool(1);
  str_map  mymap(8, pool);

  ASSERT_TRUE(mymap.insert(0, "a"));
  ASSERT_FALSE(mymap.insert(4, "b"));
  ASSERT_EQ(1U, mymap.size());
  ASSERT_FALSE(mymap.contains(4));
}

TEST(segmented_circular_map_test, test_collision)
{
  str_pool pool(2);
  str_map  mymap(8, pool);

  ASSERT_TRUE(mymap.insert(0, "a"));
  // key 8 maps to flat index 8%8=0, same slot as key 0 -> collision
  ASSERT_FALSE(mymap.insert(8, "b"));
  ASSERT_FALSE(mymap.contains(8));
  ASSERT_EQ(1U, mymap.size());
}

TEST(segmented_circular_map_test, test_overwrite)
{
  str_pool pool(2);
  str_map  mymap(8, pool);

  mymap.insert(0, "old");
  mymap.overwrite(8, "new"); // key 8 collides with key 0; old entry erased first
  ASSERT_FALSE(mymap.contains(0));
  ASSERT_TRUE(mymap.contains(8) and mymap[8] == "new");
  ASSERT_EQ(1U, mymap.size());
}

TEST(segmented_circular_map_test, test_find_absent)
{
  str_pool pool(2);
  str_map  mymap(8, pool);

  ASSERT_TRUE(mymap.find(42) == mymap.end());
  mymap.insert(1, "x");
  ASSERT_TRUE(mymap.find(42) == mymap.end());
  ASSERT_TRUE(mymap.find(1) != mymap.end());
}

TEST(segmented_circular_map_test, test_rvalue_insert)
{
  str_pool pool(2);
  str_map  mymap(8, pool);

  std::string val = "hello";
  auto        res = mymap.insert(0, std::move(val));
  ASSERT_TRUE(res);
  ASSERT_EQ(0U, (*res)->first);
  ASSERT_EQ("hello", (*res)->second);

  std::string val2 = "world";
  auto        res2 = mymap.insert(0, std::move(val2));
  ASSERT_FALSE(res2);
  ASSERT_EQ("world", res2.error());
}

TEST(segmented_circular_map_test, test_erase_by_iterator)
{
  str_pool pool(2);
  str_map  mymap(8, pool);

  mymap.insert(0, "a");
  mymap.insert(1, "b");
  mymap.insert(2, "c");

  auto it   = mymap.begin();
  auto next = mymap.erase(it);
  ASSERT_FALSE(mymap.contains(0));
  ASSERT_EQ(2U, mymap.size());
  ASSERT_EQ(1U, next->first);
}

TEST(segmented_circular_map_test, test_iterator_skips_null_segments)
{
  str_pool pool(3);
  str_map  mymap(12, pool); // capacity = 12

  // Insert only in segment 2 (slots 8..11)
  mymap.insert(8, "x");
  mymap.insert(9, "y");

  size_t count = 0;
  for (const auto& kv : mymap) {
    ASSERT_GE(kv.first, 8U);
    ++count;
  }
  ASSERT_EQ(2U, count);
  // Keys 8 and 9 both map to segment 2 (flat 8,9 → primary 2), so only 1 segment allocated.
  ASSERT_EQ(2U, pool.available());
}

TEST(segmented_circular_map_test, test_emplace)
{
  str_pool pool(2);
  str_map  mymap(8, pool);

  ASSERT_TRUE(mymap.emplace(3, "emplace_val"));
  ASSERT_TRUE(mymap.contains(3));
  ASSERT_EQ("emplace_val", mymap[3]);
}

TEST(segmented_circular_map_test, test_destructor_returns_segments)
{
  str_pool pool(2);
  {
    str_map mymap(8, pool);
    mymap.insert(0, "a");
    mymap.insert(4, "b");
    ASSERT_EQ(0U, pool.available());
  }
  ASSERT_EQ(2U, pool.available());
}

struct C {
  C() { ++count; }
  ~C() { --count; }
  C(C&&) { ++count; }
  C(const C&)       = delete;
  C& operator=(C&&) = default;

  static size_t count;
};
size_t C::count = 0;

TEST(segmented_circular_map_test, test_correct_destruction)
{
  using c_pool = simple_pool<uint32_t, C, 4>;
  using c_map  = segmented_circular_map<uint32_t, C, 4>;

  c_pool pool(2);
  ASSERT_EQ(0U, C::count);

  {
    c_map mymap(8, pool);
    ASSERT_TRUE(mymap.insert(0, C{}));
    ASSERT_TRUE(mymap.insert(1, C{}));
    ASSERT_TRUE(mymap.insert(2, C{}));
    ASSERT_EQ(3U, C::count);

    ASSERT_TRUE(mymap.erase(1));
    ASSERT_EQ(2U, C::count);

    mymap.clear();
    ASSERT_EQ(0U, C::count);
  }
  ASSERT_EQ(0U, C::count);
  ASSERT_EQ(2U, pool.available());
}

// ---- non-multiple-of-L size tests -----------------------------------------

TEST(segmented_circular_map_test, non_multiple_size_capacity)
{
  // size=6 is not a multiple of L=4; total_capacity() must report 6, not 8.
  str_pool pool(2);
  str_map  mymap(6, pool);

  ASSERT_EQ(6U, mymap.capacity());
}

TEST(segmented_circular_map_test, non_multiple_size_collision)
{
  // With size=6, key 6 wraps to flat 0 and must collide with key 0.
  str_pool pool(2);
  str_map  mymap(6, pool);

  ASSERT_TRUE(mymap.insert(0, "a"));
  ASSERT_FALSE(mymap.insert(6, "b")); // 6 % 6 == 0 → collision
  ASSERT_TRUE(mymap.contains(0));
  ASSERT_FALSE(mymap.contains(6));
  ASSERT_EQ(1U, mymap.size());
}

TEST(segmented_circular_map_test, non_multiple_size_no_spurious_collision)
{
  // Keys 5 and 6 must NOT collide: 5 % 6 == 5, 6 % 6 == 0 (different slots).
  str_pool pool(2);
  str_map  mymap(6, pool);

  ASSERT_TRUE(mymap.insert(5, "a"));
  ASSERT_TRUE(mymap.insert(6, "b")); // 6 % 6 == 0 → different slot from 5
  ASSERT_EQ(2U, mymap.size());
}

TEST(segmented_circular_map_test, size_smaller_than_segment)
{
  // size=3 < L=4: one segment allocated, only slots 0-2 reachable.
  str_pool pool(1);
  str_map  mymap(3, pool);

  ASSERT_EQ(3U, mymap.capacity());
  ASSERT_TRUE(mymap.insert(0, "a"));
  ASSERT_TRUE(mymap.insert(1, "b"));
  ASSERT_TRUE(mymap.insert(2, "c"));
  ASSERT_FALSE(mymap.insert(3, "d")); // 3 % 3 == 0 → collision with key 0
  ASSERT_EQ(3U, mymap.size());

  size_t count = 0;
  for (const auto& kv : mymap) {
    (void)kv;
    ++count;
  }
  ASSERT_EQ(3U, count);
}

// ---- shared_map_segment_pool tests ----------------------------------------

// Two counter types used to verify object lifetime across type-switched slots.
struct Ca {
  Ca() noexcept { ++live; }
  Ca(Ca&&) noexcept { ++live; }
  Ca(const Ca&)       = delete;
  Ca& operator=(Ca&&) = default;
  ~Ca() { --live; }
  static int live;
};
int Ca::live = 0;

struct Cb {
  Cb() noexcept { ++live; }
  Cb(Cb&&) noexcept { ++live; }
  Cb(const Cb&)       = delete;
  Cb& operator=(Cb&&) = default;
  ~Cb() { --live; }
  static int live;
};
int Cb::live = 0;

// L=4, 4 slots per segment.
using shared_pool_2t = shared_map_segment_pool<unsigned, 4, std::string, int>;
using str_smap       = segmented_circular_map<unsigned, std::string, 4>;
using int_smap       = segmented_circular_map<unsigned, int, 4>;

TEST(shared_map_segment_pool_test, basic_single_type)
{
  shared_map_segment_pool<unsigned, 4, std::string> pool(2);
  str_smap                                          mymap(8, pool.get_pool_of_type<std::string>());

  ASSERT_TRUE(mymap.insert(0, "a"));
  ASSERT_TRUE(mymap.insert(4, "b"));
  ASSERT_TRUE(mymap.contains(0) and mymap[0] == "a");
  ASSERT_TRUE(mymap.contains(4) and mymap[4] == "b");
  ASSERT_EQ(2U, mymap.size());

  mymap.clear();
  ASSERT_TRUE(mymap.empty());
}

TEST(shared_map_segment_pool_test, maps_share_capacity)
{
  // 2-slot pool shared between a str_smap and an int_smap.
  shared_pool_2t pool(2);
  str_smap       smap(8, pool.get_pool_of_type<std::string>());
  int_smap       imap(8, pool.get_pool_of_type<int>());

  // Consume both slots.
  ASSERT_TRUE(smap.insert(0, "x")); // acquires slot 0
  ASSERT_TRUE(imap.insert(4, 99));  // acquires slot 1 (key 4 → segment index 1)
  ASSERT_EQ(2U, smap.size() + imap.size());

  // Pool exhausted — a third segment cannot be acquired by either type.
  ASSERT_FALSE(smap.insert(4, "y")); // would need a new segment
  ASSERT_FALSE(imap.insert(0, 1));
}

TEST(shared_map_segment_pool_test, cross_type_slot_reuse)
{
  // 1-slot pool: a segment freed by a str_smap must be reusable by an int_smap.
  shared_pool_2t pool(1);
  str_smap       smap(8, pool.get_pool_of_type<std::string>());
  int_smap       imap(8, pool.get_pool_of_type<int>());

  ASSERT_TRUE(smap.insert(0, "held"));
  ASSERT_FALSE(imap.insert(0, 1)); // pool exhausted

  smap.erase(0); // returns the slot to the pool

  ASSERT_TRUE(imap.insert(0, 42)); // same slot reused for a different type
  ASSERT_EQ(42, imap[0]);
}

TEST(shared_map_segment_pool_test, clear_restores_shared_capacity)
{
  shared_pool_2t pool(2);
  str_smap       smap(8, pool.get_pool_of_type<std::string>());
  int_smap       imap(8, pool.get_pool_of_type<int>());

  smap.insert(0, "a");
  smap.insert(4, "b"); // both pool slots taken
  ASSERT_FALSE(imap.insert(0, 1));

  smap.clear(); // returns both slots

  ASSERT_TRUE(imap.insert(0, 10));
  ASSERT_TRUE(imap.insert(4, 20));
  ASSERT_EQ(10, imap[0]);
  ASSERT_EQ(20, imap[4]);
}

TEST(shared_map_segment_pool_test, values_stored_independently)
{
  shared_pool_2t pool(4);
  str_smap       smap(8, pool.get_pool_of_type<std::string>());
  int_smap       imap(8, pool.get_pool_of_type<int>());

  smap.insert(0, "hello");
  smap.insert(1, "world");
  imap.insert(0, 100);
  imap.insert(1, 200);

  ASSERT_EQ("hello", smap[0]);
  ASSERT_EQ("world", smap[1]);
  ASSERT_EQ(100, imap[0]);
  ASSERT_EQ(200, imap[1]);

  smap.erase(0);
  ASSERT_FALSE(smap.contains(0));
  ASSERT_EQ(100, imap[0]); // imap unaffected
}

TEST(shared_map_segment_pool_test, correct_object_destruction)
{
  using pool_t = shared_map_segment_pool<unsigned, 4, Ca, Cb>;
  using a_map  = segmented_circular_map<unsigned, Ca, 4>;
  using b_map  = segmented_circular_map<unsigned, Cb, 4>;

  Ca::live = 0;
  Cb::live = 0;

  pool_t pool(2);

  {
    a_map mymap(8, pool.get_pool_of_type<Ca>());
    mymap.emplace(0); // default-constructs Ca in-place
    mymap.emplace(1);
    ASSERT_EQ(2, Ca::live);

    mymap.erase(1);
    ASSERT_EQ(1, Ca::live);

    mymap.clear(); // returns segment, destroys remaining Ca
    ASSERT_EQ(0, Ca::live);
  }

  // Pool slots freed; now reuse them for Cb.
  {
    b_map mymap(8, pool.get_pool_of_type<Cb>());
    mymap.emplace(0);
    mymap.emplace(1);
    ASSERT_EQ(2, Cb::live);
  } // destructor clears map → destroys Cb objects
  ASSERT_EQ(0, Cb::live);
}

} // namespace
