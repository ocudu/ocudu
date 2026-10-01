# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(e1_gateway_test e1_gateway_test.cpp)
target_link_libraries(e1_gateway_test
        ocudu_e1_gateway
        ocudu_gateway
        e1ap_test_helpers
        ocudu_e1ap_common
        ocudu_support
        ocudulog
        e1ap_asn1
        gtest
        gtest_main)
add_test(e1_gateway_test e1_gateway_test)
set_tests_properties(e1_gateway_test PROPERTIES LABELS "sctp")
