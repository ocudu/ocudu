// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

/// \file
/// \brief Tagging of unit tests with the requirements they cover.

#include <gtest/gtest.h>
#include <initializer_list>
#include <string>
#include <string_view>

namespace ocudu::detail {

inline constexpr const char* requirements_property_key = "requirements";

/// Returns the requirement identifiers already recorded for the running test, or an empty string if there are none.
inline std::string get_recorded_test_requirements()
{
  const ::testing::TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();
  if (info == nullptr) {
    return {};
  }
  const ::testing::TestResult& result = *info->result();
  for (int i = 0, e = result.test_property_count(); i != e; ++i) {
    const ::testing::TestProperty& prop = result.GetTestProperty(i);
    if (std::string_view(prop.key()) == requirements_property_key) {
      return prop.value();
    }
  }
  return {};
}

/// \brief Records the requirement identifiers as a gtest test property, separated the way the report splits them.
///
/// gtest keeps only the last value recorded under a key, so the identifiers are appended to the ones already
/// recorded. This lets a fixture and a test body both tag the same test.
inline void record_test_requirements(std::initializer_list<const char*> ids)
{
  std::string joined = get_recorded_test_requirements();
  for (const char* id : ids) {
    if (not joined.empty()) {
      joined += ';';
    }
    joined += id;
  }
  ::testing::Test::RecordProperty(requirements_property_key, joined);
}

} // namespace ocudu::detail

/// \brief Tags the running gtest case with the requirement identifiers it covers.
///
/// The identifiers must exist in the feature list of the requirement coverage report. Place the tag
/// as the first statement of the test body:
/// \code
/// TEST_F(my_fixture, my_case)
/// {
///   OCUDU_TEST_REQUIREMENTS("MVP-FUNC-SVCS-16-8-d");
///   ...
/// }
/// \endcode
///
/// When every test of a fixture covers a requirement, place the tag in the fixture constructor or SetUp()
/// instead. A tag in a test body adds to the ones of its fixture, it does not replace them.
///
/// The tag reaches the report only if the test actually runs: gtest writes it into the XML report of
/// the test process, and add_junit_properties.py copies it onto the matching ctest JUnit entry.
/// Unlike a ctest label, it cannot claim coverage for a test that never executed.
///
/// Works in TEST, TEST_F, TEST_P and TYPED_TEST alike.
#define OCUDU_TEST_REQUIREMENTS(...) ::ocudu::detail::record_test_requirements({__VA_ARGS__})
