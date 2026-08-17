# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "ofh")

add_subdirectory(compression)
add_subdirectory(ecpri)
add_subdirectory(ethernet)
add_subdirectory(receiver)
add_subdirectory(serdes)
add_subdirectory(support)
add_subdirectory(transmitter)

add_executable(slot_symbol_point_test slot_symbol_point_test.cpp)
target_link_libraries(slot_symbol_point_test ocudulog GTest::gtest GTest::gtest_main)
gtest_discover_tests(slot_symbol_point_test)
