# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI


set_directory_properties(PROPERTIES LABELS "transmitter")

add_executable(ofh_data_flow_cplane_scheduling_commands_test ofh_data_flow_cplane_scheduling_commands_test.cpp)
target_link_libraries(ofh_data_flow_cplane_scheduling_commands_test ocudu_ofh_transmitter ocudu_support GTest::gtest GTest::gtest_main)
gtest_discover_tests(ofh_data_flow_cplane_scheduling_commands_test)

add_executable(ofh_data_flow_uplane_downlink_data_impl_test ofh_data_flow_uplane_downlink_data_impl_test.cpp)
target_link_libraries(ofh_data_flow_uplane_downlink_data_impl_test ocudu_ofh_transmitter ocudu_support GTest::gtest GTest::gtest_main)
gtest_discover_tests(ofh_data_flow_uplane_downlink_data_impl_test)

add_executable(ofh_downlink_handler_impl_test ofh_downlink_handler_impl_test.cpp)
target_link_libraries(ofh_downlink_handler_impl_test ocudu_ofh_transmitter ocudu_support GTest::gtest GTest::gtest_main)
gtest_discover_tests(ofh_downlink_handler_impl_test)

# The OFH Downlink handler tests for Category A/B processing are registered separately below, so that they can carry the
# RU-OFH-CATB-WDBF label.
add_test(NAME ofh_downlink_handler_cat_a_cat_b_test
         COMMAND ofh_downlink_handler_impl_test
                 "--gtest_filter=ofh_downlink_handler_impl.category_*"
                 --gtest_fail_if_no_test_selected)
set_tests_properties(ofh_downlink_handler_cat_a_cat_b_test PROPERTIES LABELS "RU-OFH-CATB-WDBF")

add_executable(ofh_uplane_packet_segment_calculator_test ofh_uplane_packet_segment_calculator_test.cpp)
target_link_libraries(ofh_uplane_packet_segment_calculator_test ocudu_ofh_transmitter ocudu_support GTest::gtest GTest::gtest_main)
gtest_discover_tests(ofh_uplane_packet_segment_calculator_test)

add_executable(ofh_uplink_request_handler_impl_test ofh_uplink_request_handler_impl_test.cpp)
target_link_libraries(ofh_uplink_request_handler_impl_test ocudu_ofh_transmitter ocudu_support GTest::gtest GTest::gtest_main)
gtest_discover_tests(ofh_uplink_request_handler_impl_test)

add_executable(sequence_identifier_generator_test sequence_identifier_generator_test.cpp)
target_link_libraries(sequence_identifier_generator_test ocudu_support GTest::gtest GTest::gtest_main)
gtest_discover_tests(sequence_identifier_generator_test)
