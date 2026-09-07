# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

add_executable(schedtrace_roundtrip_test
               roundtrip_test.cpp
               roundtrip_harness_test.cpp)
target_link_libraries(schedtrace_roundtrip_test schedtrace schedtrace_fbs scheduler_test_utils ocudu_support ocudulog gtest gtest_main)
gtest_discover_tests(schedtrace_roundtrip_test)
