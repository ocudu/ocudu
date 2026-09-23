# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set_directory_properties(PROPERTIES LABELS "ofh;MVP-ARCH-INTF-4")

add_executable(ofh_integration_test ofh_integration_test.cpp ofh_integration_test_config.cpp)
target_include_directories(ofh_integration_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/ofh)
target_link_libraries(ofh_integration_test
        ocudulog
        ocudu_ru_ofh
        ocudu_phy_support
        ocudu_channel_precoder
        ocudu_support
        ocudu_yaml_util)
add_test(ofh_integration_test ofh_integration_test --dl_port_id 0 1)
set_tests_properties(ofh_integration_test PROPERTIES LABELS "tsan;NO_MEMCHECK")
