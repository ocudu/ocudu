# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(e2ap_integration_test e2ap_integration_test.cpp)
target_link_libraries(e2ap_integration_test ocudulog ocudu_e2 e2ap_asn1 e2_test_helpers ocudu_gateway ocudu_support gtest gtest_main)
target_include_directories(e2ap_integration_test PRIVATE ${CMAKE_SOURCE_DIR})
#gtest_discover_tests(e2ap_integration_test)
