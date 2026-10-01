# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_library(cell_meas_manager_helpers cell_meas_manager_test_helpers.cpp)
target_include_directories(cell_meas_manager_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cell_meas_manager_helpers ocudu_cu_cp_cell_meas_manager ocudu_support ocudulog GTest::gtest gtest_main)

set(SOURCES
        cell_meas_manager_test.cpp)

add_executable(cell_meas_manager_test ${SOURCES})
target_include_directories(cell_meas_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cell_meas_manager_test cell_meas_manager_helpers cu_cp_test_helpers)
gtest_discover_tests(cell_meas_manager_test PROPERTIES "LABELS;cell_meas_manager")
