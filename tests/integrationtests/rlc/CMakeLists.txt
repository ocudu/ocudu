# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-FileCopyrightText: Copyright (C) 2026 OCUDU contributors
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "rlc")

add_library(rlc_stress_test_mac rlc_stress_test_mac.cpp)
target_include_directories(rlc_stress_test_mac PRIVATE ${CMAKE_SOURCE_DIR})
add_library(rlc_stress_test_traffic rlc_stress_test_traffic.cpp)
target_include_directories(rlc_stress_test_traffic PRIVATE ${CMAKE_SOURCE_DIR})
add_executable(rlc_stress_test rlc_stress_test.cpp)
target_include_directories(rlc_stress_test PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(rlc_stress_test rlc_stress_test_traffic rlc_stress_test_mac ocudu_cu_cp ocudu_rlc ocudu_support ocudulog )
target_include_directories(rlc_stress_test PRIVATE ${CMAKE_SOURCE_DIR})

#
# Test different RLC modes (PDCP using EIA2/EEA2 for validation)
#

# UM6 (PDCP: EIA2/EEA2)
add_test(rlc_um6_eia2_eea2_stress_test rlc_stress_test --mode=UM6 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_um6_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

# UM12 (PDCP: EIA2/EEA2)
add_test(rlc_um12_eia2_eea2_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_um12_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

# AM12 (PDCP: EIA2/EEA2)
add_test(rlc_am12_eia2_eea2_stress_test rlc_stress_test --mode=AM12 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_am12_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

# AM18 (PDCP: EIA2/EEA2)
add_test(rlc_am18_eia2_eea2_stress_test rlc_stress_test --mode=AM18 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_am18_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

#
# Test one RLC mode with different cipher/integrity algorithms in the PDCP
#

# UM12 (PDCP: EIA1/EEA1)
# Note: Until EIA1/EEA1 is optimized, only test 50 TTIs instead of 500 (default)
add_test(rlc_um12_eia1_eea1_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=1 --pdcp_ciphering_algo=1 --nof_ttis=50)
set_tests_properties(rlc_um12_eia1_eea1_stress_test PROPERTIES LABELS "tsan")

# UM12 (PDCP: EIA3/EEA3)
add_test(rlc_um12_eia3_eea3_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=3 --pdcp_ciphering_algo=3)
set_tests_properties(rlc_um12_eia3_eea3_stress_test PROPERTIES LABELS "tsan")

# UM12 (PDCP: EIA2/EEA0)
add_test(rlc_um12_eia2_eea0_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=0)
set_tests_properties(rlc_um12_eia2_eea0_stress_test PROPERTIES LABELS "tsan")
