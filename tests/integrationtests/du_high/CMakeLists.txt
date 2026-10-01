# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.

set_directory_properties(PROPERTIES LABELS "du_high;integrationtest")

add_library(du_high_worker_manager
        test_utils/du_high_worker_manager.cpp
        test_utils/du_high_env_simulator.cpp
        test_utils/du_high_ue_simulator.cpp
        test_utils/du_high_sim_dependencies.cpp
        test_utils/typeii_csi_report_packer.cpp
        test_utils/typeii_ue_capabilities.cpp)
target_include_directories(du_high_worker_manager PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_high_worker_manager
        ocudu_du_high
        f1ap_du_test_helpers
        pdcp_test_doubles
        sched_test_doubles
        mac_test_doubles
        f1ap_test_doubles)

add_executable(du_high_test du_high_test.cpp
        du_high_many_ues_test.cpp
        du_high_many_cells_test.cpp
        du_high_connectivity_test.cpp
        du_high_positioning_test.cpp
        paging_test.cpp
        typeii_csi_test.cpp)
target_link_libraries(du_high_test
        ocudu_du_high
        du_high_worker_manager
        ocudu_du_high_unit_helpers
        du_high_test_doubles
        ocudu_gateway
        gtest
        ocudu_gtest_main)
target_include_directories(du_high_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(du_high_test PROPERTIES LABELS "tsan")

add_executable(mac_test_mode_adapter_test mac_test_mode_adapter_test.cpp)
target_link_libraries(mac_test_mode_adapter_test ocudu_du_high gtest ocudu_gtest_main)
target_include_directories(mac_test_mode_adapter_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(mac_test_mode_adapter_test mac_test_mode_adapter_test)

add_subdirectory(ntn)
