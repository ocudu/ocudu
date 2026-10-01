# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "gtpu")

add_executable(pdcp_pdu_number_packing_test
  pdcp_pdu_number_packing_test.cpp
)
target_include_directories(pdcp_pdu_number_packing_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(pdcp_pdu_number_packing_test
  ocudu_gtpu_extension_header
  ocudu_support
  ocudulog
  GTest::gtest
  GTest::gtest_main
)
gtest_discover_tests(pdcp_pdu_number_packing_test)

add_executable(long_pdcp_pdu_number_packing_test
  long_pdcp_pdu_number_packing_test.cpp
)
target_include_directories(long_pdcp_pdu_number_packing_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(long_pdcp_pdu_number_packing_test
  ocudu_gtpu_extension_header
  ocudu_support
  ocudulog
  GTest::gtest
  GTest::gtest_main
)
gtest_discover_tests(long_pdcp_pdu_number_packing_test)
