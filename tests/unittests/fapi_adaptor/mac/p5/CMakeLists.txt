# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "fapi_adaptor")

add_executable(mac_fapi_p5_start_cell_procedure_test mac_fapi_p5_start_cell_procedure_test.cpp)
target_include_directories(mac_fapi_p5_start_cell_procedure_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/fapi_adaptor/mac/p5)
target_link_libraries(mac_fapi_p5_start_cell_procedure_test PRIVATE
        ocudu_mac_to_fapi_p5_sector_fastpath_adaptor
        ocudu_support
        ocudulog
        gtest
        gtest_main)
gtest_discover_tests(mac_fapi_p5_start_cell_procedure_test)

add_executable(mac_fapi_p5_stop_cell_procedure_test mac_fapi_p5_stop_cell_procedure_test.cpp)
target_include_directories(mac_fapi_p5_stop_cell_procedure_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/fapi_adaptor/mac/p5)
target_link_libraries(mac_fapi_p5_stop_cell_procedure_test PRIVATE
        ocudu_mac_to_fapi_p5_sector_fastpath_adaptor
        ocudu_support
        ocudulog
        gtest
        gtest_main)
gtest_discover_tests(mac_fapi_p5_stop_cell_procedure_test)
