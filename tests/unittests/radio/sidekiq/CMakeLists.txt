# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

add_executable(convert_i12_to_ci16_test convert_i12_to_ci16_test.cpp)
target_link_libraries(convert_i12_to_ci16_test ocudu_radio_sidekiq ocudu_support GTest::gtest GTest::gtest_main)
target_include_directories(convert_i12_to_ci16_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/radio/sidekiq)
add_test(convert_i12_to_ci16_test convert_i12_to_ci16_test)
