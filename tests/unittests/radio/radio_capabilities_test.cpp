// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#include "ocudu/radio/radio_capabilities_registry.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

using namespace ocudu;

namespace ocudu {

std::ostream& operator<<(std::ostream& os, radio_capability_key key)
{
  fmt::print(os, "{}", key.value());
  return os;
}

} // namespace ocudu

namespace test_caps {

class capability_a
{
public:
  virtual ~capability_a()   = default;
  virtual int get_a() const = 0;
};

class capability_b
{
public:
  virtual ~capability_b()   = default;
  virtual int get_b() const = 0;
};

class capability_c
{
public:
  virtual ~capability_c() = default;
};

} // namespace test_caps

namespace {

/// Implements two capabilities, one through multiple inheritance and another through a member object.
class radio_with_capabilities : public radio_capabilities_registry, public test_caps::capability_a
{
public:
  class impl_b : public test_caps::capability_b
  {
  public:
    int get_b() const override { return 2; }
  };

  int get_a() const override { return 1; }

  void* query(radio_capability_key key) override
  {
    if (key == get_radio_capability_key<test_caps::capability_a>()) {
      return static_cast<test_caps::capability_a*>(this);
    }
    if (key == get_radio_capability_key<test_caps::capability_b>()) {
      return static_cast<test_caps::capability_b*>(&b);
    }
    return nullptr;
  }

  impl_b b;
};

/// Radio without capabilities.
class radio_without_capabilities : public radio_capabilities_registry
{
public:
  void* query(radio_capability_key key) override { return nullptr; }
};

} // namespace

TEST(radio_capability_key_test, key_is_derived_from_type_name)
{
  static_assert(detail::derive_type_name<test_caps::capability_a>() == "test_caps::capability_a");
  static_assert(detail::derive_type_name<test_caps::capability_b>() == "test_caps::capability_b");

  constexpr auto key_a = get_radio_capability_key<test_caps::capability_a>();
  constexpr auto key_b = get_radio_capability_key<test_caps::capability_b>();

  ASSERT_EQ(key_a, get_radio_capability_key<test_caps::capability_a>());
  ASSERT_NE(key_a, key_b);
}

TEST(radio_capabilities_registry_test, registered_capabilities_are_retrieved)
{
  radio_with_capabilities      radio;
  radio_capabilities_registry& registry = radio;

  auto* cap_a = registry.get<test_caps::capability_a>();
  auto* cap_b = registry.get<test_caps::capability_b>();

  // The returned pointers must address the right subobjects.
  ASSERT_EQ(cap_a, static_cast<test_caps::capability_a*>(&radio));
  ASSERT_EQ(cap_b, static_cast<test_caps::capability_b*>(&radio.b));
  ASSERT_EQ(cap_a->get_a(), 1);
  ASSERT_EQ(cap_b->get_b(), 2);
}

TEST(radio_capabilities_registry_test, unsupported_capability_returns_nullptr)
{
  radio_with_capabilities    radio;
  radio_without_capabilities empty_radio;

  ASSERT_EQ(radio.get<test_caps::capability_c>(), nullptr);
  ASSERT_EQ(empty_radio.get<test_caps::capability_a>(), nullptr);
}
