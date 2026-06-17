# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

add_subdirectory(common)
add_subdirectory(p7)

set_directory_properties(PROPERTIES LABELS "fapi")

add_executable(fapi_power_unit_test fapi_power_unit_test.cpp)
target_link_libraries(fapi_power_unit_test GTest::gtest GTest::gtest_main)
gtest_discover_tests(fapi_power_unit_test)
