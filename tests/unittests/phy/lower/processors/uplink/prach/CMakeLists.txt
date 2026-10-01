# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(prach_processor_test prach_processor_test.cpp)
target_include_directories(prach_processor_test PRIVATE ${CMAKE_SOURCE_DIR}/tests/unittests)
target_link_libraries(prach_processor_test ocudu_lower_phy_prach_processor ocudulog GTest::gtest GTest::gtest_main)
add_test(prach_processor_test prach_processor_test)
