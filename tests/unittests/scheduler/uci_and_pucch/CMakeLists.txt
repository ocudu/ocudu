# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.



add_executable(uci_pucch_sched_test
        uci_indication_selector_test.cpp
        pucch_alloc_common_harq_test.cpp
        pucch_alloc_ded_resources_test.cpp
        pucch_alloc_format_0_test.cpp
        pucch_alloc_harq_sr_csi_test.cpp
        pucch_collision_manager_test.cpp
        scheduler_uci_indication_test.cpp
        uci_allocator_test.cpp
        uci_scheduling_test.cpp
        uci_test_utils.cpp)
target_link_libraries(uci_pucch_sched_test
        ocudu_sched
        scheduler_test_suite
        ocudulog
        sched_config
        mac_configuration_helpers
        sched_test_doubles
        gtest
        ocudu_gtest_main)
gtest_discover_tests(uci_pucch_sched_test)
