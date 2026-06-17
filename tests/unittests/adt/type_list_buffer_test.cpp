// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/adt/type_list_buffer.h"
#include <gtest/gtest.h>
#include <vector>

using namespace ocudu;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/// Trivially copyable small struct.
struct small_pod {
  uint8_t x;
  bool    operator==(const small_pod& o) const { return x == o.x; }
};

/// Larger trivially copyable struct.
struct large_pod {
  uint64_t a;
  uint64_t b;
  bool     operator==(const large_pod& o) const { return a == o.a && b == o.b; }
};

/// Struct with an 8-byte-aligned member.
struct aligned8 {
  alignas(8) uint8_t data[8];
  bool operator==(const aligned8& o) const { return std::memcmp(data, o.data, 8) == 0; }
};

/// Non-trivially destructible type (tracks construction / destruction counts).
struct counted {
  inline static int constructions = 0;
  inline static int destructions  = 0;

  int value;

  explicit counted(int v) : value(v) { ++constructions; }
  counted(const counted& o) : value(o.value) { ++constructions; }
  counted(counted&& o) noexcept : value(o.value) { ++constructions; }
  ~counted() { ++destructions; }

  bool operator==(const counted& o) const { return value == o.value; }

  static void reset() { constructions = destructions = 0; }
};

struct two_arg_pod {
  uint32_t x;
  uint32_t y;
  bool     operator==(const two_arg_pod& o) const { return x == o.x && y == o.y; }
};

// ---------------------------------------------------------------------------
// Compile-time checks
// ---------------------------------------------------------------------------

static_assert(std::is_default_constructible_v<dyn_type_list_buffer<int, float>>);
static_assert(std::is_move_constructible_v<dyn_type_list_buffer<int, float>>);
static_assert(std::is_copy_constructible_v<dyn_type_list_buffer<int, float>>);

static_assert(!std::is_copy_constructible_v<static_type_list_buffer<64, int, float>>);
static_assert(std::is_move_constructible_v<static_type_list_buffer<64, int, float>>);

static_assert(std::is_copy_constructible_v<type_list_buffer_stream<int, float>>);
static_assert(std::is_move_constructible_v<type_list_buffer_stream<int, float>>);
static_assert(std::is_default_constructible_v<type_list_buffer_stream<int, float>>);

// ---------------------------------------------------------------------------
// dyn_type_list_buffer tests
// ---------------------------------------------------------------------------

TEST(dyn_type_list_buffer_test, default_ctor_is_empty)
{
  dyn_type_list_buffer<int, float, double> buf;
  ASSERT_TRUE(buf.empty());
  ASSERT_EQ(buf.size(), 0u);
  ASSERT_EQ(buf.byte_size(), 0u);
}

TEST(dyn_type_list_buffer_test, push_single_type_and_visit)
{
  dyn_type_list_buffer<int> buf;
  buf.push(42);
  ASSERT_EQ(buf.size(), 1u);

  std::vector<int> visited;
  buf.for_each([&](int v) { visited.push_back(v); });
  ASSERT_EQ(visited.size(), 1u);
  ASSERT_EQ(visited[0], 42);
}

TEST(dyn_type_list_buffer_test, push_multiple_types_preserves_order)
{
  dyn_type_list_buffer<small_pod, int, large_pod> buf;

  buf.push(small_pod{7});
  buf.push(1234);
  buf.push(large_pod{0xaa, 0xbb});
  buf.push(small_pod{99});
  buf.push(-1);

  ASSERT_EQ(buf.size(), 5u);

  // Use a reference-capturing lambda so accumulated state is visible after for_each returns.
  std::vector<int> kinds; // 0=small_pod, 1=int, 2=large_pod
  buf.for_each([&kinds](const auto& obj) {
    using T = std::decay_t<decltype(obj)>;
    if constexpr (std::is_same_v<T, small_pod>) {
      kinds.push_back(0);
    } else if constexpr (std::is_same_v<T, int>) {
      kinds.push_back(1);
    } else if constexpr (std::is_same_v<T, large_pod>) {
      kinds.push_back(2);
    }
  });
  ASSERT_EQ(kinds.size(), 5u);
  ASSERT_EQ(kinds[0], 0);
  ASSERT_EQ(kinds[1], 1);
  ASSERT_EQ(kinds[2], 2);
  ASSERT_EQ(kinds[3], 0);
  ASSERT_EQ(kinds[4], 1);
}

TEST(dyn_type_list_buffer_test, visited_values_are_correct)
{
  dyn_type_list_buffer<small_pod, int, large_pod> buf;
  buf.push(small_pod{3});
  buf.push(-7);
  buf.push(large_pod{100, 200});

  small_pod sp{};
  int       iv{};
  large_pod lp{};

  buf.for_each([&](const auto& obj) {
    using T = std::decay_t<decltype(obj)>;
    if constexpr (std::is_same_v<T, small_pod>) {
      sp = obj;
    } else if constexpr (std::is_same_v<T, int>) {
      iv = obj;
    } else if constexpr (std::is_same_v<T, large_pod>) {
      lp = obj;
    }
  });

  ASSERT_EQ(sp, (small_pod{3}));
  ASSERT_EQ(iv, -7);
  ASSERT_EQ(lp, (large_pod{100, 200}));
}

TEST(dyn_type_list_buffer_test, smaller_types_occupy_less_space_than_largest)
{
  // A single small_pod (1 byte) + uint8_t type index = 2 bytes.
  // A single large_pod (16 bytes) + uint8_t type index = 17 bytes.
  // Together: at most 2 + 17 = 19 bytes (ignoring any 1-byte alignment padding).
  // A variant<small_pod, large_pod>[2] would need 2 * (16 + 1) = 34 bytes.
  dyn_type_list_buffer<small_pod, large_pod> buf;
  buf.push(small_pod{1});
  buf.push(large_pod{0, 0});

  ASSERT_LT(buf.byte_size(), 2 * (sizeof(large_pod) + sizeof(uint8_t)));
}

TEST(dyn_type_list_buffer_test, alignment_is_respected)
{
  dyn_type_list_buffer<uint8_t, aligned8> buf;
  buf.push(uint8_t{0xab});
  buf.emplace<aligned8>(aligned8{0, 1, 2, 3, 4, 5, 6, 7});

  aligned8 out{};
  buf.for_each([&](const auto& obj) {
    if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, aligned8>) {
      out = obj;
    }
  });

  ASSERT_EQ(out, (aligned8{0, 1, 2, 3, 4, 5, 6, 7}));
}

TEST(dyn_type_list_buffer_test, mutable_for_each_allows_modification)
{
  dyn_type_list_buffer<int, float> buf;
  buf.push(10);
  buf.push(2.5f);
  buf.push(20);

  buf.for_each([](auto& obj) {
    using T = std::decay_t<decltype(obj)>;
    if constexpr (std::is_same_v<T, int>) {
      obj *= 2;
    }
  });

  std::vector<int> ints;
  buf.for_each([&](const auto& obj) {
    if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, int>) {
      ints.push_back(obj);
    }
  });

  ASSERT_EQ(ints.size(), 2u);
  ASSERT_EQ(ints[0], 20);
  ASSERT_EQ(ints[1], 40);
}

TEST(dyn_type_list_buffer_test, clear_resets_buffer)
{
  dyn_type_list_buffer<int, double> buf;
  buf.push(1);
  buf.push(2.0);
  ASSERT_EQ(buf.size(), 2u);

  buf.clear();
  ASSERT_TRUE(buf.empty());
  ASSERT_EQ(buf.size(), 0u);
  ASSERT_EQ(buf.byte_size(), 0u);

  // Re-use after clear.
  buf.push(99);
  ASSERT_EQ(buf.size(), 1u);
}

TEST(dyn_type_list_buffer_test, non_trivial_destructors_are_called_on_clear)
{
  counted::reset();
  {
    dyn_type_list_buffer<counted, int> buf;
    buf.emplace<counted>(1);
    buf.push(0);
    buf.emplace<counted>(2);

    // Relocation during growth may add extra move-constructions, but two counted
    // objects must be alive in the buffer at this point.
    const int live = counted::constructions - counted::destructions;
    ASSERT_EQ(live, 2);
    const int dtors_before_clear = counted::destructions;

    buf.clear();
    // clear() must destroy exactly the two live objects.
    ASSERT_EQ(counted::destructions, dtors_before_clear + 2);
  }
  // No extra destructions from the (now-empty) buffer's destructor.
  ASSERT_EQ(counted::destructions, counted::constructions);
}

TEST(dyn_type_list_buffer_test, non_trivial_destructors_called_on_scope_exit)
{
  counted::reset();
  {
    dyn_type_list_buffer<counted, int> buf;
    buf.emplace<counted>(10);
    buf.emplace<counted>(20);
    // Relocation during growth may add extra move-constructions; two objects must be alive.
    ASSERT_EQ(counted::constructions - counted::destructions, 2);
  }
  // Every construction must be matched by a destruction.
  ASSERT_EQ(counted::destructions, counted::constructions);
}

TEST(dyn_type_list_buffer_test, copy_ctor_produces_independent_copy)
{
  dyn_type_list_buffer<int, float> original;
  original.push(1);
  original.push(3.14f);
  original.push(2);

  dyn_type_list_buffer<int, float> copy = original;

  ASSERT_EQ(copy.size(), original.size());

  // Modify original; copy must be unaffected.
  original.for_each([](auto& obj) {
    if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, int>) {
      obj = -1;
    }
  });

  std::vector<int> copy_ints;
  copy.for_each([&](const auto& obj) {
    if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, int>) {
      copy_ints.push_back(obj);
    }
  });

  ASSERT_EQ(copy_ints.size(), 2u);
  ASSERT_EQ(copy_ints[0], 1);
  ASSERT_EQ(copy_ints[1], 2);
}

TEST(dyn_type_list_buffer_test, copy_ctor_calls_copy_constructor_for_non_trivial_types)
{
  counted::reset();
  {
    dyn_type_list_buffer<counted, int> src;
    src.emplace<counted>(5);
    src.push(0);

    int constructions_before_copy = counted::constructions;

    dyn_type_list_buffer<counted, int> dst = src;
    ASSERT_EQ(counted::constructions, constructions_before_copy + 1);

    // Both src and dst hold one counted; both destructors must fire.
  }
  ASSERT_EQ(counted::destructions, counted::constructions);
}

TEST(dyn_type_list_buffer_test, move_ctor_transfers_ownership)
{
  dyn_type_list_buffer<int, double> buf;
  buf.push(7);
  buf.push(3.0);
  const size_t orig_size    = buf.size();
  const size_t orig_byte_sz = buf.byte_size();

  dyn_type_list_buffer<int, double> moved = std::move(buf);

  ASSERT_EQ(moved.size(), orig_size);
  ASSERT_EQ(moved.byte_size(), orig_byte_sz);
}

TEST(dyn_type_list_buffer_test, emplace_constructs_in_place)
{
  // Verifies that emplace forwards arguments to the object constructor.
  dyn_type_list_buffer<two_arg_pod, int> buf;
  buf.emplace<two_arg_pod>(two_arg_pod{42u, 99u});
  buf.push(0);

  two_arg_pod result{};
  buf.for_each([&](const auto& obj) {
    if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, two_arg_pod>) {
      result = obj;
    }
  });
  ASSERT_EQ(result, (two_arg_pod{42u, 99u}));
}

// ---------------------------------------------------------------------------
// Non-trivially-relocatable type: holds a pointer to its own member.
// ---------------------------------------------------------------------------

struct self_ref {
  int  value;
  int* self;
  explicit self_ref(int v) noexcept : value(v), self(&value) {}
  self_ref(const self_ref& o) noexcept : value(o.value), self(&value) {}
  self_ref(self_ref&& o) noexcept : value(o.value), self(&value) {}
  bool valid() const noexcept { return self == &value; }
};

TEST(dyn_type_list_buffer_test, non_trivially_relocatable_survives_growth)
{
  dyn_type_list_buffer<self_ref> buf;
  // Push enough elements to guarantee several reallocations.
  for (int i = 0; i < 64; ++i) {
    buf.emplace<self_ref>(i);
  }
  int idx = 0;
  buf.for_each([&](const self_ref& r) {
    ASSERT_TRUE(r.valid()) << "self-pointer corrupted at index " << idx;
    ASSERT_EQ(r.value, idx);
    ++idx;
  });
  ASSERT_EQ(idx, 64);
}

TEST(dyn_type_list_buffer_test, push_and_emplace_return_reference_to_stored_object)
{
  dyn_type_list_buffer<int, std::string> buf;

  int& i = buf.emplace<int>(7);
  ASSERT_EQ(i, 7);
  i = 99; // mutate through the returned reference

  std::string& s = buf.push(std::string{"hello"});
  ASSERT_EQ(s, "hello");
  s += " world";

  // Verify the mutations are visible through for_each.
  std::vector<int>         ints;
  std::vector<std::string> strs;
  buf.for_each([&](const auto& obj) {
    using T = std::decay_t<decltype(obj)>;
    if constexpr (std::is_same_v<T, int>) {
      ints.push_back(obj);
    } else if constexpr (std::is_same_v<T, std::string>) {
      strs.push_back(obj);
    }
  });
  ASSERT_EQ(ints.size(), 1u);
  ASSERT_EQ(ints[0], 99);
  ASSERT_EQ(strs.size(), 1u);
  ASSERT_EQ(strs[0], "hello world");
}

TEST(dyn_type_list_buffer_test, large_number_of_elements)
{
  dyn_type_list_buffer<uint8_t, uint32_t> buf;
  constexpr int                           num_elements = 1000;
  for (int i = 0; i < num_elements; ++i) {
    if (i % 2 == 0) {
      buf.push(static_cast<uint8_t>(i & 0xff));
    } else {
      buf.push(static_cast<uint32_t>(i));
    }
  }
  ASSERT_EQ(buf.size(), static_cast<size_t>(num_elements));

  int count = 0;
  buf.for_each([&](const auto&) { ++count; });
  ASSERT_EQ(count, num_elements);
}

// ---------------------------------------------------------------------------
// static_type_list_buffer tests
// ---------------------------------------------------------------------------

TEST(static_type_list_buffer_test, basic_push_and_emplace)
{
  // Capacity: enough for one int and one float.
  // int record:   type_idx(1 byte) + pad(3) + int(4)  = 8 bytes  (start=0, obj_pos=4)
  // float record: type_idx(1 byte) + pad(3) + float(4) = 8 bytes (start=8, obj_pos=12)
  static_type_list_buffer<64, int, float> buf;

  int*   ip = buf.emplace<int>(42);
  float* fp = buf.push(1.5f);

  ASSERT_NE(ip, nullptr);
  ASSERT_NE(fp, nullptr);
  ASSERT_EQ(*ip, 42);
  ASSERT_FLOAT_EQ(*fp, 1.5f);
  ASSERT_EQ(buf.size(), 2u);
}

TEST(static_type_list_buffer_test, overflow_returns_nullptr)
{
  // Capacity = 1 byte: not enough even for the smallest record (needs at least 5 bytes for int).
  static_type_list_buffer<1, int, float> buf;

  int* p = buf.emplace<int>(7);
  ASSERT_EQ(p, nullptr);
  ASSERT_TRUE(buf.empty());
  ASSERT_EQ(buf.size(), 0u);
}

TEST(static_type_list_buffer_test, overflow_after_partial_fill)
{
  // Capacity for exactly one int record (8 bytes); second push must fail.
  static_type_list_buffer<8, int, float> buf;

  int* first = buf.emplace<int>(10);
  ASSERT_NE(first, nullptr);
  ASSERT_EQ(*first, 10);

  int* second = buf.emplace<int>(20);
  ASSERT_EQ(second, nullptr);

  // First element must still be intact.
  ASSERT_EQ(buf.size(), 1u);
  ASSERT_EQ(*first, 10);
}

TEST(static_type_list_buffer_test, for_each_visits_in_order)
{
  static_type_list_buffer<64, int, float> buf;

  ASSERT_NE(buf.emplace<int>(1), nullptr);
  ASSERT_NE(buf.push(2.0f), nullptr);
  ASSERT_NE(buf.emplace<int>(3), nullptr);
  ASSERT_NE(buf.push(4.0f), nullptr);

  std::vector<int> order; // 0 = int, 1 = float
  buf.for_each([&](const auto& obj) {
    using T = std::decay_t<decltype(obj)>;
    if constexpr (std::is_same_v<T, int>) {
      order.push_back(0);
    } else {
      order.push_back(1);
    }
  });

  ASSERT_EQ(order.size(), 4u);
  ASSERT_EQ(order[0], 0);
  ASSERT_EQ(order[1], 1);
  ASSERT_EQ(order[2], 0);
  ASSERT_EQ(order[3], 1);
}

TEST(static_type_list_buffer_test, references_remain_stable)
{
  static_type_list_buffer<256, int, float> buf;

  int*   p0 = buf.emplace<int>(10);
  float* p1 = buf.push(1.0f);
  int*   p2 = buf.emplace<int>(20);
  float* p3 = buf.push(2.0f);

  ASSERT_NE(p0, nullptr);
  ASSERT_NE(p1, nullptr);
  ASSERT_NE(p2, nullptr);
  ASSERT_NE(p3, nullptr);

  // All pointers must still point to the original values after further insertions.
  ASSERT_EQ(*p0, 10);
  ASSERT_FLOAT_EQ(*p1, 1.0f);
  ASSERT_EQ(*p2, 20);
  ASSERT_FLOAT_EQ(*p3, 2.0f);
}

TEST(static_type_list_buffer_test, clear_destroys_and_allows_reuse)
{
  counted::reset();
  {
    static_type_list_buffer<256, counted, int> buf;

    ASSERT_NE(buf.emplace<counted>(1), nullptr);
    ASSERT_NE(buf.push(0), nullptr);
    ASSERT_NE(buf.emplace<counted>(2), nullptr);

    const int live = counted::constructions - counted::destructions;
    ASSERT_EQ(live, 2);
    const int dtors_before_clear = counted::destructions;

    buf.clear();
    ASSERT_EQ(counted::destructions, dtors_before_clear + 2);
    ASSERT_TRUE(buf.empty());

    // Buffer can be reused after clear.
    auto* p = buf.emplace<counted>(99);
    ASSERT_NE(p, nullptr);
    ASSERT_EQ(p->value, 99);
    ASSERT_EQ(buf.size(), 1u);
  }
  ASSERT_EQ(counted::destructions, counted::constructions);
}

// ---------------------------------------------------------------------------
// type_list_buffer_view tests
// ---------------------------------------------------------------------------

static_assert(!std::is_default_constructible_v<type_list_buffer_view<int, float>>);
static_assert(!std::is_copy_constructible_v<type_list_buffer_view<int, float>>);
static_assert(std::is_move_constructible_v<type_list_buffer_view<int, float>>);

TEST(type_list_buffer_view_test, basic_push_and_emplace)
{
  std::array<uint8_t, 64>           storage{};
  type_list_buffer_view<int, float> buf{span<uint8_t>{storage}};

  int*   ip = buf.emplace<int>(42);
  float* fp = buf.push(1.5f);

  ASSERT_NE(ip, nullptr);
  ASSERT_NE(fp, nullptr);
  ASSERT_EQ(*ip, 42);
  ASSERT_FLOAT_EQ(*fp, 1.5f);
  ASSERT_EQ(buf.size(), 2u);
}

TEST(type_list_buffer_view_test, capacity_bytes_reflects_span_size)
{
  std::array<uint8_t, 128>   storage{};
  type_list_buffer_view<int> buf{span<uint8_t>{storage}};
  ASSERT_EQ(buf.capacity_bytes(), 128u);
}

TEST(type_list_buffer_view_test, overflow_returns_nullptr)
{
  std::array<uint8_t, 1>     storage{};
  type_list_buffer_view<int> buf{span<uint8_t>{storage}};

  int* p = buf.emplace<int>(7);
  ASSERT_EQ(p, nullptr);
  ASSERT_TRUE(buf.empty());
}

TEST(type_list_buffer_view_test, overflow_after_partial_fill)
{
  // One int record needs 8 bytes (type_idx=1 + pad=3 + int=4).
  std::array<uint8_t, 8>            storage{};
  type_list_buffer_view<int, float> buf{span<uint8_t>{storage}};

  int* first = buf.emplace<int>(10);
  ASSERT_NE(first, nullptr);
  ASSERT_EQ(*first, 10);

  int* second = buf.emplace<int>(20);
  ASSERT_EQ(second, nullptr);

  ASSERT_EQ(buf.size(), 1u);
  ASSERT_EQ(*first, 10);
}

TEST(type_list_buffer_view_test, for_each_visits_in_order)
{
  std::array<uint8_t, 64>           storage{};
  type_list_buffer_view<int, float> buf{span<uint8_t>{storage}};

  ASSERT_NE(buf.emplace<int>(1), nullptr);
  ASSERT_NE(buf.push(2.0f), nullptr);
  ASSERT_NE(buf.emplace<int>(3), nullptr);
  ASSERT_NE(buf.push(4.0f), nullptr);

  std::vector<int> order; // 0 = int, 1 = float
  buf.for_each([&](const auto& obj) {
    using T = std::decay_t<decltype(obj)>;
    order.push_back(std::is_same_v<T, int> ? 0 : 1);
  });

  ASSERT_EQ(order, (std::vector<int>{0, 1, 0, 1}));
}

TEST(type_list_buffer_view_test, references_remain_stable)
{
  std::array<uint8_t, 256>          storage{};
  type_list_buffer_view<int, float> buf{span<uint8_t>{storage}};

  int*   p0 = buf.emplace<int>(10);
  float* p1 = buf.push(1.0f);
  int*   p2 = buf.emplace<int>(20);
  float* p3 = buf.push(2.0f);

  ASSERT_NE(p0, nullptr);
  ASSERT_NE(p1, nullptr);
  ASSERT_NE(p2, nullptr);
  ASSERT_NE(p3, nullptr);

  ASSERT_EQ(*p0, 10);
  ASSERT_FLOAT_EQ(*p1, 1.0f);
  ASSERT_EQ(*p2, 20);
  ASSERT_FLOAT_EQ(*p3, 2.0f);
}

TEST(type_list_buffer_view_test, clear_destroys_and_allows_reuse)
{
  counted::reset();
  {
    std::array<uint8_t, 256>            storage{};
    type_list_buffer_view<counted, int> buf{span<uint8_t>{storage}};

    ASSERT_NE(buf.emplace<counted>(1), nullptr);
    ASSERT_NE(buf.push(0), nullptr);
    ASSERT_NE(buf.emplace<counted>(2), nullptr);

    const int live = counted::constructions - counted::destructions;
    ASSERT_EQ(live, 2);
    const int dtors_before_clear = counted::destructions;

    buf.clear();
    ASSERT_EQ(counted::destructions, dtors_before_clear + 2);
    ASSERT_TRUE(buf.empty());

    auto* p = buf.emplace<counted>(99);
    ASSERT_NE(p, nullptr);
    ASSERT_EQ(p->value, 99);
    ASSERT_EQ(buf.size(), 1u);
  }
  ASSERT_EQ(counted::destructions, counted::constructions);
}

TEST(type_list_buffer_view_test, move_transfers_elements)
{
  std::array<uint8_t, 256>          storage{};
  type_list_buffer_view<int, float> src{span<uint8_t>{storage}};
  ASSERT_NE(src.emplace<int>(10), nullptr);
  ASSERT_NE(src.push(2.5f), nullptr);
  ASSERT_NE(src.emplace<int>(20), nullptr);

  type_list_buffer_view<int, float> dst(std::move(src));

  ASSERT_TRUE(src.empty());
  ASSERT_EQ(dst.size(), 3u);

  std::vector<int> vals;
  dst.for_each([&](const auto& v) {
    if constexpr (std::is_same_v<std::decay_t<decltype(v)>, int>) {
      vals.push_back(v);
    }
  });
  ASSERT_EQ(vals, (std::vector<int>{10, 20}));
}

// ---------------------------------------------------------------------------
// type_list_buffer_stream
// ---------------------------------------------------------------------------

TEST(type_list_buffer_stream_test, basic_push_and_emplace)
{
  type_list_buffer_stream<int, float> buf;

  int*   ip = buf.emplace<int>(42);
  float* fp = buf.push(1.5f);

  ASSERT_NE(ip, nullptr);
  ASSERT_NE(fp, nullptr);
  ASSERT_EQ(*ip, 42);
  ASSERT_FLOAT_EQ(*fp, 1.5f);
  ASSERT_EQ(buf.size(), 2u);
}

TEST(type_list_buffer_stream_test, for_each_visits_in_order)
{
  type_list_buffer_stream<int, float> buf;

  ASSERT_NE(buf.emplace<int>(1), nullptr);
  ASSERT_NE(buf.push(2.0f), nullptr);
  ASSERT_NE(buf.emplace<int>(3), nullptr);
  ASSERT_NE(buf.push(4.0f), nullptr);

  std::vector<int> order; // 0 = int, 1 = float
  buf.for_each([&](const auto& obj) {
    using T = std::decay_t<decltype(obj)>;
    order.push_back(std::is_same_v<T, int> ? 0 : 1);
  });

  ASSERT_EQ(order, (std::vector<int>{0, 1, 0, 1}));
}

TEST(type_list_buffer_stream_test, segment_overflow_creates_new_segment)
{
  type_list_buffer_stream<int> buf;
  constexpr int                num_elements = 500; // guaranteed to span multiple segments
  for (int i = 0; i < num_elements; ++i) {
    ASSERT_NE(buf.emplace<int>(i), nullptr);
  }
  ASSERT_EQ(buf.size(), static_cast<size_t>(num_elements));
  int last = -1;
  buf.for_each([&](const int& v) { last = v; });
  ASSERT_EQ(last, num_elements - 1);
}

TEST(type_list_buffer_stream_test, clear_destroys_and_allows_reuse)
{
  counted::reset();
  {
    type_list_buffer_stream<counted, int> buf;

    ASSERT_NE(buf.emplace<counted>(1), nullptr);
    ASSERT_NE(buf.push(0), nullptr);
    ASSERT_NE(buf.emplace<counted>(2), nullptr);

    ASSERT_EQ(counted::constructions - counted::destructions, 2);
    const int dtors_before = counted::destructions;

    buf.clear();
    ASSERT_EQ(counted::destructions, dtors_before + 2);
    ASSERT_TRUE(buf.empty());

    auto* p = buf.emplace<counted>(99);
    ASSERT_NE(p, nullptr);
    ASSERT_EQ(p->value, 99);
    ASSERT_EQ(buf.size(), 1u);
  }
  ASSERT_EQ(counted::destructions, counted::constructions);
}

TEST(type_list_buffer_stream_test, move_transfers_elements)
{
  type_list_buffer_stream<int, float> src;
  ASSERT_NE(src.emplace<int>(10), nullptr);
  ASSERT_NE(src.push(2.5f), nullptr);
  ASSERT_NE(src.emplace<int>(20), nullptr);

  type_list_buffer_stream<int, float> dst(std::move(src));

  ASSERT_TRUE(src.empty());
  ASSERT_EQ(dst.size(), 3u);

  std::vector<int> vals;
  dst.for_each([&](const auto& v) {
    if constexpr (std::is_same_v<std::decay_t<decltype(v)>, int>) {
      vals.push_back(v);
    }
  });
  ASSERT_EQ(vals, (std::vector<int>{10, 20}));
}

TEST(type_list_buffer_stream_test, destructor_calls_dtors)
{
  counted::reset();
  {
    type_list_buffer_stream<counted, int> buf;
    ASSERT_NE(buf.emplace<counted>(1), nullptr);
    ASSERT_NE(buf.emplace<counted>(2), nullptr);
    ASSERT_NE(buf.emplace<counted>(3), nullptr);
    ASSERT_EQ(counted::constructions, 3);
  }
  ASSERT_EQ(counted::destructions, 3);
}

TEST(type_list_buffer_stream_test, copy_assignment_destroys_existing_elements)
{
  counted::reset();

  type_list_buffer_stream<counted, int> dst;
  ASSERT_NE(dst.emplace<counted>(1), nullptr);
  ASSERT_NE(dst.emplace<counted>(2), nullptr);
  ASSERT_EQ(counted::constructions - counted::destructions, 2);

  type_list_buffer_stream<counted, int> src;
  ASSERT_NE(src.emplace<counted>(99), nullptr);

  const int dtors_before = counted::destructions;
  dst                    = src;
  // The two elements originally in dst must have been destroyed.
  ASSERT_EQ(counted::destructions, dtors_before + 2);
  ASSERT_EQ(dst.size(), 1u);
}

TEST(type_list_buffer_stream_test, shallow_copy_shares_elements)
{
  type_list_buffer_stream<int, float> src;
  ASSERT_NE(src.emplace<int>(7), nullptr);
  ASSERT_NE(src.push(3.14f), nullptr);

  // Shallow copy: both see the same elements.
  type_list_buffer_stream<int, float> c = src;
  ASSERT_EQ(c.size(), 2u);

  // Elements pushed after the copy are visible through both handles.
  ASSERT_NE(src.emplace<int>(99), nullptr);
  ASSERT_EQ(src.size(), 3u);
  ASSERT_EQ(c.size(), 3u);
}

TEST(type_list_buffer_stream_test, shallow_copy_defers_destruction_until_last_owner)
{
  counted::reset();

  {
    type_list_buffer_stream<counted, int> src;
    ASSERT_NE(src.emplace<counted>(11), nullptr);
    ASSERT_EQ(counted::constructions, 1);
    ASSERT_EQ(counted::destructions, 0);

    {
      type_list_buffer_stream<counted, int> copy = src;
      ASSERT_EQ(copy.size(), 1u);
    }

    ASSERT_EQ(counted::destructions, 0);
  }

  ASSERT_EQ(counted::destructions, counted::constructions);
}
