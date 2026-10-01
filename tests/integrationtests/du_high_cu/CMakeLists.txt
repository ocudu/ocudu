# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "cu;du;integrationtest")

add_executable(cu_du_test cu_du_test.cpp du_high_cu_test_simulator.cpp cu_multi_du_test.cpp)
target_link_libraries(cu_du_test ocudu_cu_cp
        ocudu_du_high
        du_high_worker_manager
        ocudu_f1ap_cu
        mac_test_doubles
        f1ap_test_doubles
        du_test_doubles
        cu_cp_test_helpers
        ocudu_rrc
        ocudu_support
        ocudulog
        ngap_test_helpers
        f1ap_asn1
        ngap_asn1
        gtest
        ocudu_gtest_main)
target_include_directories(cu_du_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(cu_du_test)
