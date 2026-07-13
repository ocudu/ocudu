# SPDX-FileCopyrightText: Copyright (C) 2021-2026 Software Radio Systems Limited
# SPDX-License-Identifier: BSD-3-Clause-Open-MPI
# Portions of this file may implement 3GPP specifications, which may be subject to additional licensing requirements.


set_directory_properties(PROPERTIES LABELS "ran")

include_directories(../../..)

add_executable(beam_codebook_generator_test beamforming/beam_codebook_generator_test.cpp)
target_link_libraries(beam_codebook_generator_test ocudu_ran GTest::gtest ocudu_gtest_main)
gtest_discover_tests(beam_codebook_generator_test beam_codebook_generator_test)

add_executable(csi_report_on_pucch_helpers_test csi_report/csi_report_on_pucch_helpers_test.cpp)
target_link_libraries(csi_report_on_pucch_helpers_test ocudu_ran GTest::gtest ocudu_gtest_main)
gtest_discover_tests(csi_report_on_pucch_helpers_test)

add_executable(csi_report_on_pucch_validator_test csi_report/csi_report_on_pucch_validator_test.cpp)
target_link_libraries(csi_report_on_pucch_validator_test ocudu_ran GTest::gtest ocudu_gtest_main)
gtest_discover_tests(csi_report_on_pucch_validator_test)

add_executable(csi_report_on_pusch_helpers_test csi_report/csi_report_on_pusch_helpers_test.cpp)
target_link_libraries(csi_report_on_pusch_helpers_test ocudu_ran GTest::gtest ocudu_gtest_main)
gtest_discover_tests(csi_report_on_pusch_helpers_test)

add_executable(csi_report_on_pusch_validator_test csi_report/csi_report_on_pusch_validator_test.cpp)
target_link_libraries(csi_report_on_pusch_validator_test ocudu_ran GTest::gtest ocudu_gtest_main)
gtest_discover_tests(csi_report_on_pusch_validator_test)

add_library(du_test_suite ssb_coreset0_freq_pos_checker.cpp)
target_link_libraries(du_test_suite ocudu_ran gtest)

add_executable(test_ssb_coreset0_freq_pos_generator test_ssb_coreset0_freq_pos_generator.cpp)
target_link_libraries(test_ssb_coreset0_freq_pos_generator ocudu_support du_test_suite GTest::gtest ocudu_gtest_main)
add_test(test_ssb_coreset0_freq_pos_generator test_ssb_coreset0_freq_pos_generator)

add_executable(dci_packing_test pdcch/dci_packing_test.cpp)
target_link_libraries(dci_packing_test ocudu_support ocudu_ran GTest::gtest ocudu_gtest_main)
add_test(dci_packing_test dci_packing_test)

add_executable(dci_packing_validator_test pdcch/dci_packing_validator_test.cpp)
target_link_libraries(dci_packing_validator_test ocudu_support ocudu_ran GTest::gtest ocudu_gtest_main)
add_test(dci_packing_validator_test dci_packing_validator_test)

add_executable(pdcch_type0_css_coreset_config_test pdcch/pdcch_type0_css_coreset_config_test.cpp)
target_link_libraries(pdcch_type0_css_coreset_config_test ocudu_ran ocudulog)
add_test(pdcch_type0_css_coreset_config_test pdcch_type0_css_coreset_config_test)

add_executable(pdsch_mcs_table_test pdsch/pdsch_mcs_table_test.cpp)
target_link_libraries(pdsch_mcs_table_test ocudu_ran ocudulog GTest::gtest ocudu_gtest_main)
add_test(pdsch_mcs_table_test pdsch_mcs_table_test)

add_executable(prach_configuration_test prach/prach_configuration_test.cpp)
target_link_libraries(prach_configuration_test ocudu_ran ocudulog GTest::gtest ocudu_gtest_main)
add_test(prach_configuration_test prach_configuration_test)

add_executable(prach_cyclic_shifts_test prach/prach_cyclic_shifts_test.cpp)
target_link_libraries(prach_cyclic_shifts_test ocudu_ran ocudulog GTest::gtest ocudu_gtest_main)
add_test(prach_cyclic_shifts_test prach_cyclic_shifts_test)

add_executable(prach_frequency_mapping_test prach/prach_frequency_mapping_test.cpp)
target_link_libraries(prach_frequency_mapping_test ocudu_ran ocudulog GTest::gtest  ocudu_gtest_main)
add_test(prach_frequency_mapping_test prach_frequency_mapping_test)

add_executable(pucch_info_test pucch/pucch_info_test.cpp)
target_link_libraries(pucch_info_test ocudu_ran GTest::gtest ocudu_gtest_main)
add_test(pucch_info_test pucch_info_test)

add_executable(pusch_mcs_table_test pusch/pusch_mcs_table_test.cpp)
target_link_libraries(pusch_mcs_table_test ocudu_ran ocudulog GTest::gtest ocudu_gtest_main)
add_test(pusch_mcs_table_test pusch_mcs_table_test)

add_executable(pusch_precoding_info_test pusch/pusch_precoding_info_test.cpp)
target_link_libraries(pusch_precoding_info_test ocudu_ran ocudulog GTest::gtest ocudu_gtest_main)
add_test(pusch_precoding_info_test pusch_precoding_info_test)

add_executable(resource_allocation_frequency_test resource_allocation/resource_allocation_frequency_test.cpp)
target_link_libraries(resource_allocation_frequency_test ocudu_support ocudu_ran)
add_test(resource_allocation_frequency_test resource_allocation_frequency_test)

add_executable(vrb_to_prb_test resource_allocation/vrb_to_prb_test.cpp)
target_link_libraries(vrb_to_prb_test ocudu_ran ocudulog GTest::gtest ocudu_gtest_main)
add_test(vrb_to_prb_test vrb_to_prb_test)

add_executable(uci_part2_size_calculator_test uci/uci_part2_size_calculator_test.cpp)
target_link_libraries(uci_part2_size_calculator_test ocudu_support ocudu_ran ocuduvec GTest::gtest ocudu_gtest_main)
add_test(uci_part2_size_calculator_test uci_part2_size_calculator_test)

add_executable(slot_point_test slot_point_test.cpp)
target_link_libraries(slot_point_test ocudu_support)
add_test(slot_point_test slot_point_test)

add_executable(band_helper_test band_helper_test.cpp)
target_link_libraries(band_helper_test ocudu_ran ocudulog GTest::gtest ocudu_gtest_main)
target_include_directories(band_helper_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(band_helper_test band_helper_test)

add_executable(ssb_gscn_test ssb_gscn_test.cpp)
target_link_libraries(ssb_gscn_test ocudu_ran GTest::gtest ocudu_gtest_main)
add_test(ssb_gscn_test ssb_gscn_test)

add_executable(ssb_mapping_test ssb_mapping_test.cpp)
target_link_libraries(ssb_mapping_test ocudu_ran ocudulog)
add_test(ssb_mapping_test ssb_mapping_test)

add_executable(srs_bandwidth_configuration_test srs/srs_bandwidth_configuration_test.cpp)
target_link_libraries(srs_bandwidth_configuration_test ocudu_ran ocudulog GTest::gtest ocudu_gtest_main)
add_test(srs_bandwidth_configuration_test srs_bandwidth_configuration_test)

add_executable(cell_identity_test cell_identity_test.cpp)
target_link_libraries(cell_identity_test ocudu_support GTest::gtest ocudu_gtest_main)
add_test(cell_identity_test cell_identity_test)

add_executable(phy_time_unit_test phy_time_unit_test.cpp)
target_link_libraries(phy_time_unit_test ocudu_support GTest::gtest ocudu_gtest_main)
gtest_discover_tests(phy_time_unit_test)

add_executable(i_rnti_test i_rnti_test.cpp)
target_link_libraries(i_rnti_test ocudu_support GTest::gtest ocudu_gtest_main)
add_test(i_rnti_test i_rnti_test)

add_executable(meas_gap_config_test meas_gap_config_test.cpp)
target_link_libraries(meas_gap_config_test ocudu_ran GTest::gtest ocudu_gtest_main)
gtest_discover_tests(meas_gap_config_test)

add_executable(precoding_codebooks_test precoding/precoding_codebooks_test.cpp)
target_link_libraries(precoding_codebooks_test ocudu_ran GTest::gtest ocudu_gtest_main)
add_test(precoding_codebooks_test precoding_codebooks_test)

add_executable(precoding_matrix_factorization_test precoding/precoding_matrix_factorization_test.cpp)
target_link_libraries(precoding_matrix_factorization_test
        ocudu_ran
        ocuduvec
        ocudu_phy_support
        ocudu_channel_precoder
        ocudulog
        GTest::gtest
        ocudu_gtest_main)
add_test(precoding_matrix_factorization_test precoding_matrix_factorization_test)

add_executable(precoding_matrix_resource_grid_test precoding/precoding_matrix_resource_grid_test.cpp)
target_link_libraries(precoding_matrix_resource_grid_test
        ocudu_ran
        ocuduvec
        ocudu_phy_support
        ocudu_channel_precoder
        ocudulog
        GTest::gtest
        ocudu_gtest_main)
add_test(precoding_matrix_resource_grid_test precoding_matrix_resource_grid_test)
