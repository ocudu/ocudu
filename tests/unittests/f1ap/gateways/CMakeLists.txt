# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


add_executable(f1c_gateway_test f1c_gateway_test.cpp)
target_link_libraries(f1c_gateway_test
        ocudu_f1c_gateway
        ocudu_gateway
        ocudu_f1ap_common
        ocudu_support
        ocudulog
        f1ap_asn1
        gtest
        gtest_main)
add_test(f1c_gateway_test f1c_gateway_test)
set_tests_properties(f1c_gateway_test PROPERTIES LABELS "sctp")
