// SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
// SPDX-License-Identifier: BSD-3-Clause-Open-MPI

#pragma once

/// \file
/// \brief Tagging of unit tests with the requirements they cover.

#include <gtest/gtest.h>
#include <initializer_list>
#include <string>

namespace ocudu::detail {

/// Records the requirement identifiers as a gtest test property, separated the way the report splits them.
inline void record_test_requirements(std::initializer_list<const char*> ids)
{
  std::string joined;
  for (const char* id : ids) {
    if (not joined.empty()) {
      joined += ';';
    }
    joined += id;
  }
  ::testing::Test::RecordProperty("requirements", joined);
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
/// The tag reaches the report only if the test actually runs: gtest writes it into the XML report of
/// the test process, and merge_gtest_requirements.py copies it onto the matching ctest JUnit entry.
/// Unlike a ctest label, it cannot claim coverage for a test that never executed.
///
/// Works in TEST, TEST_F, TEST_P and TYPED_TEST alike.
#define OCUDU_TEST_REQUIREMENTS(...) ::ocudu::detail::record_test_requirements({__VA_ARGS__})
