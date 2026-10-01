# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "nru")

add_executable(nru_packing_test nru_packing_test.cpp)
target_include_directories(nru_packing_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(nru_packing_test ocudu_nru ocudu_support ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(nru_packing_test)
