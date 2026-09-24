# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_subdirectory(pdu_translators)

add_executable(fapi_to_mac_indications_fastpath_translator_test fapi_to_mac_indications_fastpath_translator_test.cpp)
target_include_directories(fapi_to_mac_indications_fastpath_translator_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/fapi_adaptor/mac/p7)
target_link_libraries(fapi_to_mac_indications_fastpath_translator_test PRIVATE
        ocudu_support
        ocudu_fapi_to_mac_p7_fastpath_translator
        ocudulog
        ocudu_mac_translator_helpers
        gtest
        gtest_main)
gtest_discover_tests(fapi_to_mac_indications_fastpath_translator_test)

add_executable(mac_to_fapi_fastpath_translator_test mac_to_fapi_fastpath_translator_test.cpp)
target_include_directories(mac_to_fapi_fastpath_translator_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/fapi_adaptor/mac/p7)
target_link_libraries(mac_to_fapi_fastpath_translator_test PRIVATE
        ocudu_support
        ocudu_mac_to_fapi_p7_fastpath_translator
        ocudu_fapi_precoding_codebook_tools
        ocudulog
        ocudu_mac_translator_helpers
        gtest
        gtest_main)
gtest_discover_tests(mac_to_fapi_fastpath_translator_test)
