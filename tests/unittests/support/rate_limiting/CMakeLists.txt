# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


add_executable(token_bucket_test token_bucket_test.cpp)
target_link_libraries(token_bucket_test ocudu_support ocudulog gtest GTest::gtest_main)
target_include_directories(token_bucket_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(token_bucket_test token_bucket_test)

add_executable(lockfree_token_bucket_test lockfree_token_bucket_test.cpp)
target_link_libraries(lockfree_token_bucket_test ocudu_support atomic ocudulog gtest GTest::gtest_main)
target_include_directories(lockfree_token_bucket_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(lockfree_token_bucket_test lockfree_token_bucket_test)
