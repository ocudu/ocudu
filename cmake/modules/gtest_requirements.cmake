# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


# Collection of the requirement tags that the tests record.
#
# A test declares what it covers by calling OCUDU_TEST_REQUIREMENTS("<id>", ...) in its body, which
# makes gtest write the identifiers into the XML report of the test process. Nothing here reads the
# test sources: the tags travel with the running test, so renaming, moving or parameterising a test
# cannot desynchronise them.
#
# With OCUDU_TEST_REQUIREMENT_REPORTS=ON every test runs through a launcher that hands it a private
# XML report, which the CI then folds into the ctest JUnit output:
#
#   OCUDU_GTEST_XML_DIR=$PWD/gtest_xml ctest --output-junit xunit.xml ...
#   add_junit_properties.py xunit.xml --gtest-xml-dir gtest_xml --strict

option(OCUDU_TEST_REQUIREMENT_REPORTS "Give each test process its own gtest XML report" OFF)

if (OCUDU_TEST_REQUIREMENT_REPORTS)
    if (CMAKE_VERSION VERSION_LESS 3.29)
        message(FATAL_ERROR "OCUDU_TEST_REQUIREMENT_REPORTS needs CMake 3.29 for CMAKE_TEST_LAUNCHER.")
    endif ()

    # A launcher is the only way to give each test process a distinct report: the tests of a target
    # created by gtest_discover_tests share one set of properties, so a per-test ENVIRONMENT is not
    # available. It applies to every test, including the ones that are not gtest binaries, which is
    # why the launcher stays a no-op unless OCUDU_GTEST_XML_DIR is set when ctest runs.
    set(CMAKE_TEST_LAUNCHER "${PROJECT_SOURCE_DIR}/cmake/scripts/gtest_xml_launcher.sh")
endif ()
