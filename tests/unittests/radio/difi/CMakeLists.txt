# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

add_executable(radio_difi_context_packet_test radio_difi_context_packet_test.cpp)
target_link_libraries(radio_difi_context_packet_test ocudu_radio_difi gtest gtest_main)
target_include_directories(radio_difi_context_packet_test PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/radio/difi)
add_test(radio_difi_context_packet_test radio_difi_context_packet_test)

add_executable(radio_difi_data_packet_test radio_difi_data_packet_test.cpp)
target_link_libraries(radio_difi_data_packet_test ocudu_radio_difi gtest gtest_main)
target_include_directories(radio_difi_data_packet_test PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/radio/difi
        ${CMAKE_SOURCE_DIR}/include)
add_test(radio_difi_data_packet_test radio_difi_data_packet_test)
