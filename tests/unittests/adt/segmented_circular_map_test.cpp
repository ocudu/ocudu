// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
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

/// Simple single-type pool backed by pre-allocated typed storage.
template <typename K, typename V>
class simple_pool : public map_segment_pool_interface<K, V>
{
  using opt_t = std::optional<detail::kv_obj<K, V>>;

public:
  simple_pool(size_t capacity, size_t seg_len) : seg_size_val(seg_len), backing(capacity * seg_len)
  {
    free_list.reserve(capacity);
    for (size_t i = 0; i < capacity; ++i) {
      free_list.push_back(&backing[i * seg_len]);
    }
  }

  ocudu::span<opt_t> get_segment() override
  {
    if (free_list.empty()) {
      return {};
    }
    opt_t* p = free_list.back();
    free_list.pop_back();
    return {p, seg_size_val};
  }

  void return_segment(ocudu::span<opt_t> seg) override
  {
    for (auto& slot : seg) {
      slot.reset();
    }
    free_list.push_back(seg.data());
  }

  size_t segment_size() const override { return seg_size_val; }

  size_t available() const { return free_list.size(); }

private:
  size_t              seg_size_val;
  std::vector<opt_t>  backing;
  std::vector<opt_t*> free_list;
};

// ---- Four-variant parameterization ----------------------------------------

/// Tag type carrying ForcePower2MapSize and ForcePower2SegSize template flags.
template <bool MapPow2, bool SegPow2>
struct MapConfig {
  static constexpr bool pow2_map = MapPow2;
  static constexpr bool pow2_seg = SegPow2;
};

struct MapConfigNames {
  template <typename T>
  static std::string GetName(int)
  {
    if constexpr (T::pow2_map && T::pow2_seg)
      return "pow2map_pow2seg";
    if constexpr (T::pow2_map && !T::pow2_seg)
      return "pow2map_anyseg";
    if constexpr (!T::pow2_map && T::pow2_seg)
      return "anymap_pow2seg";
    return "anymap_anyseg";
  }
};

/// All four flag combinations, for tests where map_size and seg_size are both powers of 2.
using AllMapConfigs =
    ::testing::Types<MapConfig<false, false>, MapConfig<false, true>, MapConfig<true, false>, MapConfig<true, true>>;

/// ForcePower2MapSize=false variants only, for tests that use non-power-of-2 map sizes.
using AnyMapSizeConfigs = ::testing::Types<MapConfig<false, false>, MapConfig<false, true>>;

// ---- Object-lifetime helper for destruction tests -------------------------

struct C {
  C() { ++count; }
  ~C() { --count; }
  C(C&&) { ++count; }
  C(const C&)       = delete;
  C& operator=(C&&) = default;

  static size_t count;
};
size_t C::count = 0;

// ---- Main typed tests (map_size=8, seg_size=4 — both power of 2) ----------

template <typename Config>
class segmented_circular_map_test : public ::testing::Test
{};

TYPED_TEST_SUITE(segmented_circular_map_test, AllMapConfigs, MapConfigNames);

TYPED_TEST(segmented_circular_map_test, basic_operations)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(8, pool);

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

TYPED_TEST(segmented_circular_map_test, segment_lifecycle)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(8, pool);

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

TYPED_TEST(segmented_circular_map_test, clear_returns_all_segments)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(8, pool);

  mymap.insert(0, "a");
  mymap.insert(4, "b");
  ASSERT_EQ(0U, pool.available());

  mymap.clear();
  ASSERT_EQ(2U, pool.available());
}

TYPED_TEST(segmented_circular_map_test, pool_exhaustion)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(1, 4);
  map_t                              mymap(8, pool);

  ASSERT_TRUE(mymap.insert(0, "a"));
  ASSERT_FALSE(mymap.insert(4, "b"));
  ASSERT_EQ(1U, mymap.size());
  ASSERT_FALSE(mymap.contains(4));
}

TYPED_TEST(segmented_circular_map_test, collision)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(8, pool);

  ASSERT_TRUE(mymap.insert(0, "a"));
  // key 8 maps to flat index 8%8=0, same slot as key 0 -> collision
  ASSERT_FALSE(mymap.insert(8, "b"));
  ASSERT_FALSE(mymap.contains(8));
  ASSERT_EQ(1U, mymap.size());
}

TYPED_TEST(segmented_circular_map_test, overwrite)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(8, pool);

  mymap.insert(0, "old");
  mymap.overwrite(8, "new"); // key 8 collides with key 0; old entry erased first
  ASSERT_FALSE(mymap.contains(0));
  ASSERT_TRUE(mymap.contains(8) and mymap[8] == "new");
  ASSERT_EQ(1U, mymap.size());
}

TYPED_TEST(segmented_circular_map_test, find_absent)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(8, pool);

  ASSERT_TRUE(mymap.find(42) == mymap.end());
  mymap.insert(1, "x");
  ASSERT_TRUE(mymap.find(42) == mymap.end());
  ASSERT_TRUE(mymap.find(1) != mymap.end());
}

TYPED_TEST(segmented_circular_map_test, rvalue_insert)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(8, pool);

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

TYPED_TEST(segmented_circular_map_test, erase_by_iterator)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(8, pool);

  mymap.insert(0, "a");
  mymap.insert(1, "b");
  mymap.insert(2, "c");

  auto it   = mymap.begin();
  auto next = mymap.erase(it);
  ASSERT_FALSE(mymap.contains(0));
  ASSERT_EQ(2U, mymap.size());
  ASSERT_EQ(1U, next->first);
}

TYPED_TEST(segmented_circular_map_test, iterator_skips_null_segments)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  // map_size=16 (pow2), seg_size=4 (pow2): 4 segments of 4 slots each.
  simple_pool<unsigned, std::string> pool(4, 4);
  map_t                              mymap(16, pool);

  // Insert only into segment 2 (flat slots 8..11).
  mymap.insert(8, "x");
  mymap.insert(9, "y");

  size_t count = 0;
  for (const auto& kv : mymap) {
    ASSERT_GE(kv.first, 8U);
    ++count;
  }
  ASSERT_EQ(2U, count);
  ASSERT_EQ(3U, pool.available()); // only 1 of 4 segments acquired
}

TYPED_TEST(segmented_circular_map_test, emplace)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(8, pool);

  ASSERT_TRUE(mymap.emplace(3, "emplace_val"));
  ASSERT_TRUE(mymap.contains(3));
  ASSERT_EQ("emplace_val", mymap[3]);
}

TYPED_TEST(segmented_circular_map_test, destructor_returns_segments)
{
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  {
    map_t mymap(8, pool);
    mymap.insert(0, "a");
    mymap.insert(4, "b");
    ASSERT_EQ(0U, pool.available());
  }
  ASSERT_EQ(2U, pool.available());
}

TYPED_TEST(segmented_circular_map_test, correct_destruction)
{
  using c_pool = simple_pool<uint32_t, C>;
  using c_map  = segmented_circular_map<uint32_t, C, TypeParam::pow2_map, TypeParam::pow2_seg>;

  C::count = 0;
  c_pool pool(2, 4);

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

// ---- Non-power-of-2 map size tests (ForcePower2MapSize=false only) ---------

template <typename Config>
class segmented_circular_map_nonpow2_map_test : public ::testing::Test
{};

TYPED_TEST_SUITE(segmented_circular_map_nonpow2_map_test, AnyMapSizeConfigs, MapConfigNames);

TYPED_TEST(segmented_circular_map_nonpow2_map_test, non_multiple_size_capacity)
{
  // size=6 is not a multiple of seg_size=4; capacity() must report 6, not 8.
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(6, pool);

  ASSERT_EQ(6U, mymap.capacity());
}

TYPED_TEST(segmented_circular_map_nonpow2_map_test, non_multiple_size_collision)
{
  // With size=6, key 6 wraps to flat 0 and must collide with key 0.
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(6, pool);

  ASSERT_TRUE(mymap.insert(0, "a"));
  ASSERT_FALSE(mymap.insert(6, "b")); // 6 % 6 == 0 → collision
  ASSERT_TRUE(mymap.contains(0));
  ASSERT_FALSE(mymap.contains(6));
  ASSERT_EQ(1U, mymap.size());
}

TYPED_TEST(segmented_circular_map_nonpow2_map_test, non_multiple_size_no_spurious_collision)
{
  // Keys 5 and 6 must NOT collide: 5 % 6 == 5, 6 % 6 == 0 (different slots).
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(2, 4);
  map_t                              mymap(6, pool);

  ASSERT_TRUE(mymap.insert(5, "a"));
  ASSERT_TRUE(mymap.insert(6, "b")); // 6 % 6 == 0 → different slot from 5
  ASSERT_EQ(2U, mymap.size());
}

TYPED_TEST(segmented_circular_map_nonpow2_map_test, size_smaller_than_segment)
{
  // size=3 < seg_size=4: one segment allocated, only slots 0-2 reachable.
  using map_t = segmented_circular_map<unsigned, std::string, TypeParam::pow2_map, TypeParam::pow2_seg>;
  simple_pool<unsigned, std::string> pool(1, 4);
  map_t                              mymap(3, pool);

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

// 4 slots per segment, ForcePower2SegSize=true (seg_size=4 is a power of 2).
using shared_pool_2t = shared_map_segment_pool<unsigned, true, std::string, int>;
using str_smap       = segmented_circular_map<unsigned, std::string>;
using int_smap       = segmented_circular_map<unsigned, int>;

TEST(shared_map_segment_pool_test, basic_single_type)
{
  shared_map_segment_pool<unsigned, true, std::string> pool(2, 4);
  str_smap                                             mymap(8, pool.get_pool_of_type<std::string>());

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
  shared_pool_2t pool(2, 4);
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
  shared_pool_2t pool(1, 4);
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
  shared_pool_2t pool(2, 4);
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
  shared_pool_2t pool(4, 4);
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
  using pool_t = shared_map_segment_pool<unsigned, true, Ca, Cb>;
  using a_map  = segmented_circular_map<unsigned, Ca>;
  using b_map  = segmented_circular_map<unsigned, Cb>;

  Ca::live = 0;
  Cb::live = 0;

  pool_t pool(2, 4);

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
