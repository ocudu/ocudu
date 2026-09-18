# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "security")

add_executable(security_test security_test.cpp)
target_include_directories(security_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(security_test security_test_doubles ocudu_security ocudu_support ocudulog gtest gtest_main)
gtest_discover_tests(security_test)

add_executable(integrity_engine_test integrity_engine_test.cpp)
target_include_directories(integrity_engine_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(integrity_engine_test security_test_doubles ocudu_security ocudu_support ocudulog gtest gtest_main)
gtest_discover_tests(integrity_engine_test)

add_executable(ciphering_engine_test ciphering_engine_test.cpp)
target_include_directories(ciphering_engine_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ciphering_engine_test security_test_doubles ocudu_security ocudu_support ocudulog gtest gtest_main)
gtest_discover_tests(ciphering_engine_test)

add_executable(k_gnb_star_gen_helper k_gnb_star_gen_helper.cpp)
target_include_directories(k_gnb_star_gen_helper PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(k_gnb_star_gen_helper ocudu_security ocudu_support ocudulog)
