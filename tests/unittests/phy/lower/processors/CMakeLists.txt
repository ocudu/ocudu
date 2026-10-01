# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(downlink)
add_subdirectory(uplink)

add_executable(resource_request_pool_test resource_request_pool_test.cpp)
target_include_directories(resource_request_pool_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/phy/lower/processors)
target_link_libraries(resource_request_pool_test ocudu_ran ocudulog ocudu_support GTest::gtest GTest::gtest_main)
add_test(resource_request_pool_test resource_request_pool_test)
