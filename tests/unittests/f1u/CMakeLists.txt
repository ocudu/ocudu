# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(cu_up)
add_subdirectory(du)

add_executable(f1u_local_connector_test common/f1u_connector_test.cpp)
target_link_libraries(f1u_local_connector_test
ocudu_f1u_connector
ocudu_f1u_du
ocudu_f1u_cu_up
ocudu_support
ocudulog
GTest::gtest
GTest::gtest_main)

target_include_directories(f1u_local_connector_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1u_local_connector_test)

add_executable(f1u_cu_up_split_connector_test common/f1u_cu_split_connector_test.cpp)
target_link_libraries(f1u_cu_up_split_connector_test
ocudu_f1u_cu_up_split_connector
ocudu_f1u_cu_up
ocudu_gtpu
ocudu_support
ocudulog
GTest::gtest
GTest::gtest_main)

target_include_directories(f1u_cu_up_split_connector_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1u_cu_up_split_connector_test)

add_executable(f1u_du_split_connector_test common/f1u_du_split_connector_test.cpp)
target_link_libraries(f1u_du_split_connector_test
ocudu_f1u_du_split_connector
ocudu_f1u_du
ocudu_gtpu
ocudu_support
ocudulog
GTest::gtest
GTest::gtest_main)

target_include_directories(f1u_du_split_connector_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1u_du_split_connector_test)

add_executable(f1u_session_manager_test common/f1u_session_manager_test.cpp)
target_link_libraries(f1u_session_manager_test
ocudu_f1u_split_connector
ocudu_support
ocudulog
GTest::gtest
GTest::gtest_main)

target_include_directories(f1u_session_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1u_session_manager_test)
