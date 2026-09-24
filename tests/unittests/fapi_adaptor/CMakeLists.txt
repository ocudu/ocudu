# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_subdirectory(mac/p5)
add_subdirectory(mac/p7)
add_subdirectory(phy/p7)

add_executable(precoding_codebook_generator_test precoding_codebook_generator_test.cpp)
target_link_libraries(precoding_codebook_generator_test ocudu_fapi_precoding_codebook_tools ocudulog gtest gtest_main)
gtest_discover_tests(precoding_codebook_generator_test)

add_executable(uci_part2_correspondence_test uci_part2_correspondence_test.cpp)
target_link_libraries(uci_part2_correspondence_test ocudu_fapi_uci_part2_tools ocudulog gtest gtest_main)
gtest_discover_tests(uci_part2_correspondence_test)
