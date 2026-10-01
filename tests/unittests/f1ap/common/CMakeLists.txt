# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(f1ap_asn1_packer_test f1ap_asn1_packer_test.cpp f1ap_asn1_helpers_test.cpp)
target_link_libraries(f1ap_asn1_packer_test
        ocudu_f1ap_common
        f1ap_test_doubles
        ocudu_support
        ocudulog
        f1ap_asn1
        ocudu_ran
        gtest
        ocudu_gtest_main)
target_include_directories(f1ap_asn1_packer_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1ap_asn1_packer_test)
