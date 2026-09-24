# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_subdirectory(pdu_translators)

add_library(ocudu_fapi_message_builder_test_helpers STATIC message_builder_helpers.cpp)

add_executable(fapi_to_phy_fastpath_translator_test fapi_to_phy_fastpath_translator_test.cpp)
target_include_directories(fapi_to_phy_fastpath_translator_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/fapi_adaptor/phy/p7)
target_link_libraries(fapi_to_phy_fastpath_translator_test PRIVATE ocudu_fapi_to_phy_p7_fastpath_translator
    ocudu_fapi_precoding_codebook_tools
    ocudu_fapi_uci_part2_tools
    ocudulog
    ocudu_fapi_message_builder_test_helpers
    ocudu_upper_phy_support gtest gtest_main)
gtest_discover_tests(fapi_to_phy_fastpath_translator_test)

add_executable(phy_to_fapi_results_event_fastpath_translator_test
    phy_to_fapi_results_event_fastpath_translator_test.cpp)
target_include_directories(phy_to_fapi_results_event_fastpath_translator_test
    PRIVATE ${CMAKE_SOURCE_DIR}/lib/fapi_adaptor/phy/p7)
target_link_libraries(phy_to_fapi_results_event_fastpath_translator_test
    PRIVATE ocudu_phy_to_fapi_p7_fastpath_translator
    ocudulog
    ocudu_upper_phy_support gtest gtest_main)
gtest_discover_tests(phy_to_fapi_results_event_fastpath_translator_test)
