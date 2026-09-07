// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI
// Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

/// \file Generic harness for flatbuffer roundtrip tests.
///
/// To test a serializable type T:
///  - Specialize test_values<F> for every leaf field type F that needs boundary-value coverage.
///  - Specialize reflection<T> with:
///     - `members`: a std::make_tuple of field(name, pointer-to-member) for every field that is actually part of
///       the wire format (fields that are supplied out-of-band by the caller, e.g. an index or context looked up
///       separately, should be omitted). A field reached through more than one level of nesting, or that is not
///       itself a stored member (e.g. a container element), uses computed_field(name, accessor) instead.
///     - `roundtrip(T)`: serializes a T to its flatbuffer form and deserializes it back into a new T.
///  - In the test file, call test_helper::test_roundtrip<T>() from a TEST body.
///
/// test_values<T> for a composite T (has reflection<T>::members, no explicit test_values<T> override) is derived for
/// free by sweeping each field through its own test_values, one field after another, carrying prior fields' swept
/// values forward -- see field_variations_from below. Nested structs (e.g. a std::variant<...> alternative) just need
/// their own reflection<Inner>::members; test_values<Inner> derives the same way, recursively.
///
/// Coverage is bottom-up: an inner type that is independently roundtrip-tested (its reflection<Inner> defines
/// roundtrip(), i.e. it has its own exposed converters and its own TEST) is swept by enclosing types only at its two
/// extremes -- all fields at their first value and all at their last -- since its per-field coverage already lives in
/// its own test. An embedded-only inner type (members but no roundtrip()) keeps the full recursive sweep, because the
/// enclosing type's test is the only place its serialization is exercised. This is enforced by member_sweep_values
/// below, so promoting an inner type to independently-tested automatically thins the outer sweeps.
///
/// Diagnostics: on mismatch, fields_eq prints only the differing fields, recursing into reflected structs and
/// narrowing containers to the differing elements. Values print via fmt (every ocudu RAN type has a
/// fmt::formatter/format_as; a bare enum falls back to fmt::underlying, other types to to_string() or their
/// reflected members).

#pragma once

#include "flatbuffers/flatbuffer_builder.h"
#include "ocudu/adt/static_vector.h"
#include <algorithm>
#include <fmt/format.h>
#include <fmt/std.h>
#include <gtest/gtest.h>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

namespace ocudu::schedtrace::roundtrip_test {

/// A reflected member of T: a direct pointer-to-member (\c ptr) or, for a field nested below a stored member (or
/// behind a container index), a computed accessor (\c get). Exactly one is set. \c sweep tells whether the harness
/// may pick values for the field, see \c compare_only_field.
template <typename T, typename M>
struct member {
  const char* name;
  M T::* ptr    = nullptr;
  M& (*get)(T&) = nullptr;
  bool sweep    = true;
};

template <typename T, typename M>
constexpr member<T, M> field(const char* name, M T::* ptr)
{
  return member<T, M>{name, ptr, nullptr, true};
}

/// \brief A field that is compared on every roundtrip check but never given its own sweep axis.
///
/// For a field whose valid values depend on another field's, which a one-field-at-a-time sweep cannot express. Pair
/// with a custom \c test_values<T> that appends the valid combinations explicitly.
///
/// Note that a field whose type simply has no \c test_values (e.g. pucch_info::res, a looked-up pointer) is already
/// compare-only without this -- use \c compare_only_field for a field whose type does have test_values the harness
/// must not apply to it.
template <typename T, typename M>
constexpr member<T, M> compare_only_field(const char* name, M T::* ptr)
{
  return member<T, M>{name, ptr, nullptr, false};
}

/// \p get must be safe on any instance the harness builds -- pair with a hand-built base in a custom
/// \c test_values<T> (not the auto-derived composite one, which default-constructs T first).
template <typename T, typename M>
constexpr member<T, M> computed_field(const char* name, M& (*get)(T&))
{
  return member<T, M>{name, nullptr, get, true};
}

template <typename T, typename M>
M& member_ref(const member<T, M>& m, T& obj)
{
  return m.get != nullptr ? m.get(obj) : obj.*m.ptr;
}

template <typename T, typename M>
const M& member_ref(const member<T, M>& m, const T& obj)
{
  if (m.get != nullptr) {
    return m.get(const_cast<T&>(obj));
  }
  return obj.*m.ptr;
}

/// \brief Wire-format description of T, and (for top-level types) how to roundtrip it through its flatbuffer form.
///
/// Specialize per serializable type T with:
///   // List of fields that are part of the wire format and should be preserved across a roundtrip.
///   static constexpr auto members = std::make_tuple(field("name", &T::name), ...);
///   // Roundtrip (serialize and deserialize) T. Returns the deserialized T.
///   // Only needed for top-level (independently serialized) types.
///   static T roundtrip(const T& value) { ... }
template <typename T>
struct roundtrip_traits;

/// \brief Corner-case values to probe for a given field type.
///
/// Specializations should list the boundary values (min, max, invalid) that are likely to expose serialization bugs.
template <typename T, typename = void>
struct test_values {};

/// Whether test_values<T> has a get() (either an explicit specialization, or an auto-derived composite/variant one).
/// A member with no test_values is still reflected (compared on every roundtrip check) but is never independently
/// varied -- used for fields whose correctness can only be checked, not given independent boundary values, such as
/// a pointer to context looked up elsewhere.
template <typename T, typename = void>
struct has_test_values : std::false_type {};

template <typename T>
struct has_test_values<T, std::void_t<decltype(test_values<T>::get())>> : std::true_type {};

/// Whether T is independently roundtrip-tested, i.e. roundtrip_traits<T> defines roundtrip(). Only top-level
/// (independently serialized) types do; an embedded-only type has members but no roundtrip().
template <typename T, typename = void>
struct has_own_roundtrip : std::false_type {};

template <typename T>
struct has_own_roundtrip<T, std::void_t<decltype(roundtrip_traits<T>::roundtrip(std::declval<const T&>()))>>
  : std::true_type {};

// --- Derived test_values for composite (reflection<T>::members) and variant fields -------------

/// Values to sweep T through when it appears as a member (or variant alternative) of an enclosing type. An
/// independently roundtrip-tested T already gets exhaustive per-field coverage from its own test, so enclosing
/// types only re-check its two extremes: test_values<T> front (all fields at their first value) and back (all at
/// their last -- see field_variations_from). An embedded-only T is swept in full, as the enclosing type's test is
/// its only coverage.
template <typename T>
std::vector<T> member_sweep_values()
{
  std::vector<T> values = test_values<T>::get();
  if constexpr (has_own_roundtrip<T>::value) {
    if (values.size() > 2) {
      values = {values.front(), values.back()};
    }
  }
  return values;
}

/// Sweeps each of \p base's reflected fields through its other test_values, one field at a time, carrying prior
/// fields' swept (not reset) values forward -- so the last instance returned has every field at its extreme
/// simultaneously, at no extra cost over resetting to \p base each time. Also used directly by explicit
/// test_values<T> overrides that need a custom-built \p base (e.g. pucch_info, see pucch_info_roundtrip_test.cpp).
template <typename T>
std::vector<T> field_variations_from(T base)
{
  std::vector<T> result{base};
  T              current = base;
  std::apply(
      [&](auto... members) {
        (
            [&](auto m) {
              using member_t = std::decay_t<decltype(member_ref(m, current))>;
              if constexpr (has_test_values<member_t>::value) {
                if (!m.sweep) {
                  return;
                }
                const std::vector<member_t> values = member_sweep_values<member_t>();
                for (size_t i = 1; i != values.size(); ++i) {
                  member_ref(m, current) = values[i];
                  result.push_back(current);
                }
              }
            }(members),
            ...);
      },
      roundtrip_traits<T>::members);
  return result;
}

template <typename T>
struct test_values<T, std::void_t<decltype(roundtrip_traits<T>::members)>> {
  static std::vector<T> get()
  {
    T base{};
    std::apply(
        [&](auto... members) {
          (
              [&](auto m) {
                using member_t = std::decay_t<decltype(member_ref(m, base))>;
                if constexpr (has_test_values<member_t>::value) {
                  if (m.sweep) {
                    member_ref(m, base) = test_values<member_t>::get().front();
                  }
                }
              }(members),
              ...);
        },
        roundtrip_traits<T>::members);
    return field_variations_from(base);
  }
};

/// \brief Specialization of \c test_values for variant.
///
/// Sweeps each alternative through its own test_values, one alternative at a time.
template <typename... Alts>
struct test_values<std::variant<Alts...>> {
  static std::vector<std::variant<Alts...>> get()
  {
    std::vector<std::variant<Alts...>> result;
    (
        [&] {
          for (const auto& alt_value : member_sweep_values<Alts>()) {
            result.emplace_back(alt_value);
          }
        }(),
        ...);
    return result;
  }
};

// --- Comparison, including pointers and types without operator== -------------------------------
//
// A reflected type is compared through its members, never through its operator==: members is the wire format, and a
// field left out of it must not decide the verdict. Only a leaf type with no reflection falls back to operator==,
// hence the static_vector, variant and optional overloads below. A pointer (e.g. pucch_info::res) compares by
// pointee, not by address.

template <typename T, typename = void>
struct has_reflected_members : std::false_type {};

template <typename T>
struct has_reflected_members<T, std::void_t<decltype(roundtrip_traits<T>::members)>> : std::true_type {};

template <typename T, typename = void>
struct has_equality : std::false_type {};

template <typename T>
struct has_equality<T, std::void_t<decltype(std::declval<const T&>() == std::declval<const T&>())>> : std::true_type {};

// Declared before the generic values_equal, which must see them to recurse into a std:: or ocudu:: container.
template <typename T, size_t N>
bool values_equal(const static_vector<T, N>& a, const static_vector<T, N>& b);

template <typename... Alts>
bool values_equal(const std::variant<Alts...>& a, const std::variant<Alts...>& b);

template <typename T>
bool values_equal(const std::optional<T>& a, const std::optional<T>& b);

template <typename T>
bool values_equal(const T& a, const T& b)
{
  if constexpr (has_reflected_members<T>::value) {
    bool equal = true;
    std::apply(
        [&](auto... members) {
          ([&](auto m) { equal = equal and values_equal(member_ref(m, a), member_ref(m, b)); }(members), ...);
        },
        roundtrip_traits<T>::members);
    return equal;
  } else {
    static_assert(has_equality<T>::value, "type has neither reflected members nor operator==");
    return a == b;
  }
}

template <typename T>
bool values_equal(T* a, T* b)
{
  if (a == nullptr or b == nullptr) {
    return a == b;
  }
  return values_equal(*a, *b);
}

template <typename T, size_t N>
bool values_equal(const static_vector<T, N>& a, const static_vector<T, N>& b)
{
  return a.size() == b.size() and
         std::equal(a.begin(), a.end(), b.begin(), [](const T& x, const T& y) { return values_equal(x, y); });
}

template <typename... Alts>
bool values_equal(const std::variant<Alts...>& a, const std::variant<Alts...>& b)
{
  if (a.index() != b.index()) {
    return false;
  }
  return std::visit(
      [&b](const auto& lhs) {
        using alt_t = std::decay_t<decltype(lhs)>;
        return values_equal(lhs, std::get<alt_t>(b));
      },
      a);
}

template <typename T>
bool values_equal(const std::optional<T>& a, const std::optional<T>& b)
{
  if (a.has_value() != b.has_value()) {
    return false;
  }
  return !a.has_value() or values_equal(*a, *b);
}

// --- Printing for diagnostics -------------------------------------------------------------------
//
// Defers to fmt, which most ocudu RAN types already support (a fmt::formatter or format_as). An enum with neither
// just prints its underlying value via fmt::underlying, and a class without one falls back to its to_string() or its
// own reflected members, so no per-type boilerplate is needed here. A variant, pointer, optional or static_vector
// prints the contained value's own fields, reusing its reflection<T>::members -- no per-element formatter needed.

template <typename T, typename = void>
struct has_to_string : std::false_type {};

template <typename T>
struct has_to_string<T, std::void_t<decltype(std::declval<const T&>().to_string())>> : std::true_type {};

/// Extension point for a leaf type none of describe's generic fallbacks can print: specialize with a
/// `static std::string print(const T&)` next to the type's test_values (e.g. phy_time_unit, see rach_indication.h).
template <typename T, typename = void>
struct printer {};

template <typename T, typename = void>
struct has_printer : std::false_type {};

template <typename T>
struct has_printer<T, std::void_t<decltype(printer<T>::print(std::declval<const T&>()))>> : std::true_type {};

template <typename T>
std::string describe_fields(const T& value);

template <typename T>
std::string describe(const T& value)
{
  if constexpr (has_printer<T>::value) {
    return printer<T>::print(value);
  } else if constexpr (fmt::is_formattable<T>::value) {
    return fmt::format("{}", value);
  } else if constexpr (std::is_enum_v<T>) {
    return fmt::format("{}", fmt::underlying(value));
  } else if constexpr (has_to_string<T>::value) {
    return value.to_string();
  } else if constexpr (has_reflected_members<T>::value) {
    return describe_fields(value);
  } else {
    return "<unprintable>";
  }
}

template <typename T>
std::string describe(T* value)
{
  // describe_fields is explicitly instantiated on the decayed (non-const) type: reflection<T> specializations are
  // only ever written for the plain type, e.g. reflection<pucch_resource>, not reflection<const pucch_resource>.
  return value == nullptr ? "null" : describe_fields<std::remove_cv_t<T>>(*value);
}

template <typename... Alts>
std::string describe(const std::variant<Alts...>& value)
{
  return std::visit(
      [&value](const auto& alt) { return fmt::format("variant#{}[{}]", value.index(), describe_fields(alt)); }, value);
}

template <typename T>
std::string describe(const std::optional<T>& value)
{
  return value.has_value() ? describe_fields(*value) : "nullopt";
}

template <typename T, size_t N>
std::string describe(const static_vector<T, N>& value)
{
  std::ostringstream oss;
  oss << "[";
  for (const T& element : value) {
    oss << "{" << describe_fields(element) << "} ";
  }
  oss << "]";
  return oss.str();
}

/// Prints "name=value" for every field reflection<T>::members knows about. Falls back to describe(value) itself if
/// T has no reflected members (e.g. it is directly fmt-formattable).
template <typename T>
std::string describe_fields(const T& value)
{
  if constexpr (has_reflected_members<T>::value) {
    std::ostringstream oss;
    std::apply(
        [&](auto... members) {
          ([&](auto m) { oss << m.name << "=" << describe(member_ref(m, value)) << " "; }(members), ...);
        },
        roundtrip_traits<T>::members);
    return oss.str();
  } else {
    return describe(value);
  }
}

// --- Mismatch reporting ---------------------------------------------------------------------------
//
// On failure, fields_eq reports only what actually differs: describe_mismatch renders one differing field. A
// reflected struct recurses into just its differing fields, and a static_vector narrows to the differing elements
// (capped), so a mismatch in a 200-element list prints one line, not the whole list twice.

template <typename T, size_t N>
std::string describe_mismatch(const static_vector<T, N>& expected, const static_vector<T, N>& actual);

template <typename T>
std::string describe_mismatch(const T& expected, const T& actual)
{
  if constexpr (has_reflected_members<T>::value) {
    std::ostringstream oss;
    std::apply(
        [&](auto... members) {
          (
              [&](auto m) {
                if (!values_equal(member_ref(m, expected), member_ref(m, actual))) {
                  oss << m.name << "={" << describe_mismatch(member_ref(m, expected), member_ref(m, actual)) << "} ";
                }
              }(members),
              ...);
        },
        roundtrip_traits<T>::members);
    return oss.str();
  } else {
    return "expected=" + describe(expected) + " actual=" + describe(actual);
  }
}

template <typename T, size_t N>
std::string describe_mismatch(const static_vector<T, N>& expected, const static_vector<T, N>& actual)
{
  if (expected.size() != actual.size()) {
    return fmt::format("size expected={} actual={}", expected.size(), actual.size());
  }
  constexpr unsigned max_reported   = 3;
  unsigned           nof_mismatches = 0;
  std::ostringstream oss;
  for (unsigned i = 0; i != expected.size(); ++i) {
    if (!values_equal(expected[i], actual[i])) {
      if (nof_mismatches != max_reported) {
        oss << "[" << i << "]={" << describe_mismatch(expected[i], actual[i]) << "} ";
      }
      ++nof_mismatches;
    }
  }
  if (nof_mismatches > max_reported) {
    oss << "... (" << nof_mismatches - max_reported << " more differing elements)";
  }
  return oss.str();
}

// --- Generic flatbuffer roundtrip boilerplate ---------------------------------------------------
//
// Every reflection<T>::roundtrip() builds a FlatBufferBuilder, serializes \p value into it, gets the decoded
// flatbuffer root, and decodes that back into a fresh T -- the same four lines regardless of T, FbsRoot, or which
// converter functions are involved. roundtrip_via factors that out: callers only supply \p encode (fbb, value) ->
// flatbuffers::Offset<FbsRoot> and \p decode (T&, const FbsRoot&) -> void, i.e. exactly the type-specific converter
// calls, with FbsRoot named explicitly since it can't be deduced from either callback alone.
template <typename FbsRoot, typename T, typename Encode, typename Decode>
T roundtrip_via(const T& value, Encode encode, Decode decode)
{
  flatbuffers::FlatBufferBuilder fbb;
  fbb.Finish(encode(fbb, value));
  const auto& fb_value = *flatbuffers::GetRoot<FbsRoot>(fbb.GetBufferPointer());

  T result{};
  decode(result, fb_value);
  return result;
}

// --- Generic reflection-driven comparison and test bodies --------------------------------------

/// Compares \p expected and \p actual field-by-field, using the fields reflected in reflection<T>::members, i.e.
/// every field that is actually part of the wire format. On mismatch, reports only the fields that differ.
template <typename T>
::testing::AssertionResult fields_eq(const T& expected, const T& actual)
{
  bool               equal = true;
  std::ostringstream oss;

  std::apply(
      [&](auto... members) {
        (
            [&](auto m) {
              const auto& expected_value = member_ref(m, expected);
              const auto& actual_value   = member_ref(m, actual);
              if (!values_equal(expected_value, actual_value)) {
                equal = false;
                oss << "\n  " << m.name << ": " << describe_mismatch(expected_value, actual_value);
              }
            }(members),
            ...);
      },
      roundtrip_traits<T>::members);

  if (!equal) {
    return ::testing::AssertionFailure() << "fields differ after roundtrip:" << oss.str();
  }
  return ::testing::AssertionSuccess();
}

/// Roundtrips every instance test_values<T>::get() produces -- the base instance plus one variation per reflected
/// field per one of that field's other registered corner values, see test_values<T>'s composite specialization above
/// -- and checks that each one comes back unchanged.
///
/// A reflected member with no test_values (see has_test_values) is never varied here, but every instance still
/// checks it via fields_eq -- so a compare-only field (e.g. a looked-up pointer) still gets its roundtrip verified,
/// just riding along on whichever other field is being varied, rather than as an independent axis.
template <typename T>
void test_roundtrip()
{
  for (const T& input : test_values<T>::get()) {
    EXPECT_TRUE(fields_eq(input, roundtrip_traits<T>::roundtrip(input)));
  }
}

} // namespace ocudu::schedtrace::roundtrip_test
