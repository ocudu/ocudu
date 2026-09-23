# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI

set_directory_properties(PROPERTIES LABELS "ofh;MVP-ARCH-INTF-4")

add_executable(ofh_integration_test ofh_integration_test.cpp ofh_integration_test_config.cpp ofh_integration_test_non_rt_ru_factory.cpp)
target_include_directories(ofh_integration_test PRIVATE ${CMAKE_SOURCE_DIR}/lib/ofh ${CMAKE_SOURCE_DIR}/lib/ru/ofh)
target_link_libraries(ofh_integration_test
        ocudulog
        ocudu_ru_ofh
        ocudu_phy_support
        ocudu_channel_precoder
        ocudu_support
        ocudu_yaml_util)
add_test(ofh_integration_test_non_rt ofh_integration_test --non_realtime true --nof_test_slots 200
        --dl_port_id 0 1)
add_test(ofh_integration_test_non_rt_4x2 ofh_integration_test --non_realtime true --nof_test_slots 200)
add_test(ofh_integration_test_non_rt_8x8 ofh_integration_test --non_realtime true --nof_test_slots 200
        --dl_port_id 0 1 2 3 4 5 6 7 --ul_port_id 0 1 2 3 4 5 6 7 --prach_port_id 8 9)
add_test(ofh_integration_test_non_rt_8x8_100MHz ofh_integration_test --non_realtime true --nof_test_slots 200
        --channel_bandwidth_MHz 100 --dl_port_id 0 1 2 3 4 5 6 7 --ul_port_id 0 1 2 3 4 5 6 7 --prach_port_id 8 9)
add_test(ofh_integration_test_non_rt_custom_eaxc ofh_integration_test --non_realtime true --nof_test_slots 200
        --dl_port_id 5 7 --ul_port_id 9 3 --prach_port_id 1 2)
add_test(ofh_integration_test_non_rt_fragmented_no_compr ofh_integration_test --non_realtime true --nof_test_slots 200
        --mtu 1500 --scs 15 --channel_bandwidth_MHz 50 --tdd_pattern 6d3u
        --compr_method_data none --compr_bitwidth_data 16 --enable_dl_static_compr_hdr true)
set_tests_properties(ofh_integration_test_non_rt
                     ofh_integration_test_non_rt_4x2
                     ofh_integration_test_non_rt_8x8
                     ofh_integration_test_non_rt_8x8_100MHz
                     ofh_integration_test_non_rt_custom_eaxc
                     ofh_integration_test_non_rt_fragmented_no_compr
                     PROPERTIES LABELS "tsan;NO_MEMCHECK")

set_property(TEST ofh_integration_test_non_rt_8x8 ofh_integration_test_non_rt_8x8_100MHz
             APPEND PROPERTY LABELS "MVP-FUNC-PHY-16-9")
