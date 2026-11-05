# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

add_executable(radio_realtime_loopback_test
        coverage_on_error.cpp
        loopback_buffer_test.cpp
        radio_realtime_loopback_test.cpp
        radio_realtime_loopback_validator_test.cpp)
target_link_libraries(radio_realtime_loopback_test ocudu_radio ocudu_support GTest::gtest GTest::gtest_main)
target_include_directories(radio_realtime_loopback_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/radio/realtime_loopback)
if (ENABLE_GCOV)
    target_compile_definitions(radio_realtime_loopback_test PRIVATE ENABLE_GCOV)
endif ()
add_test(radio_realtime_loopback_test radio_realtime_loopback_test)
